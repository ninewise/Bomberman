#include "blast_it.h"
#include "gui.h"
#include "game.h"
#include "level.h"
#include "player.h"
#include <stdlib.h>

void init_game(Game* game, int level_nr) {
    init_level(&game->level, generate_level_info(level_nr));
    init_player(&game->player);
    game->enemies_left = game->level.level_info.nr_of_enemies;
    game->enemies = calloc(game->enemies_left, sizeof(Enemy));
    game->score = 0;
}

void do_game_loop(Game * game) {
	int stop = 0;

	while(!stop) {
		check_game_input(game);
		update_game(game);
		render_game(game);
        stop = (game->enemies_left == 0 || game->game_over == 0);
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

    // TODO
    game->enemies_left--;
}

void render_game(Game * game) {
}

void destroy_game(Game * game) {
    destroy_player(&game->player);
    free(game->enemies);
}

void do_player_movement(Game * game) {
    if(game->input.hasMoved) {
        move_player(&game->player, game->input.moves, &game->level);
    }
}

void do_enemy_ai(Game * game) {
}

void process_bonus_items(Game * game) {
}

void process_bombs(Game * game) {
}

