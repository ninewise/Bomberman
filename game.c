#include "blast_it.h"
#include "gui.h"
#include "game.h"
#include "level.h"
#include "player.h"
#include "enemy.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "walker.h"

void init_game(Game* game, int level_nr) {
    int i;

    init_level(&game->level, generate_level_info(level_nr));
    init_player(&game->player);
    game->enemies_left = game->level.level_info.nr_of_enemies;
    game->enemies = calloc(game->enemies_left, sizeof(Enemy));
    for( i = 0; i < game->enemies_left; i++ ){
		init_enemy(&game->enemies[i]);
    }
    game->score = 0;
}

void do_game_loop(Game * game) {
	int stop = 0;

    gui_set_level_info(&game->level.level_info);

	while(!stop) {
		check_game_input(game);
		update_game(game);
		render_game(game);
        stop = (game->enemies_left == 0 || game->game_over == 0 || gui_is_terminated());
	}

	destroy_level(&game->level);
}

void check_game_input(Game * game) {
    game->input.dropBomb = gui_is_space_pressed();
    game->input.hasMoved = gui_is_movement_key_down();
    game->input.moves[0] = (game->input.hasMoved & UP) == UP;
    game->input.moves[1] = (game->input.hasMoved & DOWN) == DOWN;
    game->input.moves[2] = (game->input.hasMoved & RIGHT) == RIGHT;
    game->input.moves[3] = (game->input.hasMoved & LEFT) == LEFT;
}

void update_game(Game * game)
{
	/* Do player movement */
	do_player_movement(game);

	/* Check enemies (move, collision with player) */
	do_enemy_ai(game);

	/* Process bonus items */
	process_bonus_items(game);

	/* Process bombs */
	process_bombs(game);

}

void render_game(Game * game) {
	int i;
	
    render_level(&game->level);
    render_player(&game->player);
	for( i = 0; i < game->enemies_left; i++ ){
		render_enemy(&game->enemies[i]);
    }
    gui_draw_buffer();
}

void destroy_game(Game * game) {
    destroy_player(&game->player);
    free(game->enemies);
}

void do_player_movement(Game * game) {
    if(game->input.hasMoved) {
        Walker walker = {
            game->player.x,
            game->player.y,
            game->player.orientation
        };
        move_walker(&walker, game->input.moves, game->level.entities,
            PLAYER_MOVEMENT_INCREMENT);

        game->player.x = walker.x;
        game->player.y = walker.y;
        game->player.orientation = walker.orientation;
    }
}

void do_enemy_ai(Game * game) {
}

void process_bonus_items(Game * game) {
}

void process_bombs(Game * game) {
    int i, j, e;
    // Als de speler een bom gelegd heeft, en de speler heeft nog bommen ter
    // beschikking, plaatsen we een nieuwe bom.
    if(game->input.dropBomb && game->player.remaining_bombs > 0) {
        player_drop_bomb(&game->player, game->level.entities);
    }
    for(i = 0; i < game->level.level_info.width; i++) for(j = 0; j < game->level.level_info.height; j++) {
        if(game->level.entities[i][j].type == BOMB) {
            Bomb bomb = game->level.entities[i][j].bomb;
            // We brengen de ticks 1 keer dichter bij 0.
            if(bomb.ticks_left < 0) bomb.ticks_left++;
            else bomb.ticks_left--;

            // We zorgen dat bommen niet langer beloopbaar zijn zodra er geen
            // player of enemy meer op staat.
            int occupied = 0;   // Of er al dan niet een vijand of speler op deze bom staat.
            occupied = (occupied) ? occupied :
                        ((game->player.x+PLAYER_TRANSLATE_X) / TILE_SIZE == bomb.x / TILE_SIZE
                      && (game->player.y+PLAYER_TRANSLATE_Y) / TILE_SIZE == bomb.y / TILE_SIZE);
            for(e = 0; e < game->level.level_info.nr_of_enemies; e++)
                occupied = (occupied) ? occupied :
                    (game->enemies[e].x / TILE_SIZE == bomb.x / TILE_SIZE &&
                     game->enemies[e].y / TILE_SIZE == bomb.y / TILE_SIZE);
            if(!occupied && bomb.ticks_left < 0) bomb.ticks_left *= -1;

            // En we ontploffen als we 0 zijn.
            if(bomb.ticks_left == 0) {
                Explosion explosion = {
                    EXPLOSION,
                    bomb.x,
                    bomb.y,
                    {1,1,1,1},
                    3,
                    EXPLOSION_TICKS
                };
                game->level.entities[i][j].explosion = explosion;
            }

            // De ticks van de bom in entities moet ook aangepast worden.
            game->level.entities[i][j].bomb.ticks_left = bomb.ticks_left;
        }
    }
}

