#include "blast_it.h"
#include "gui.h"
#include "game.h"
#include "level.h"
#include "player.h"
#include "enemy.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "collision.h"
#include "entity.h"

#define DECENTER(X)     ((X) - TILE_SIZE/2)
#define CENTER(X)       ((X) + TILE_SIZE/2)
#define TO_TILE(X)      ((X) / TILE_SIZE)

void init_game(Game* game, int level_nr) {
    int i;

    init_level(&game->level, generate_level_info(level_nr));
    init_player(&game->player);
    game->enemies_left = game->level.level_info.nr_of_enemies;
    game->enemies = calloc(game->enemies_left, sizeof(Enemy));
    for( i = 0; i < game->enemies_left; i++ ){
		init_enemy(&game->enemies[i],&game->level);
    }
    game->game_over = 0;
    game->score = 0;
}

void do_game_loop(Game * game) {
	int stop = 0;

    gui_set_level_info(&game->level.level_info);
    gui_start_timer();
	while(!stop) {
		check_game_input(game);
		update_game(game);
		render_game(game);
        stop = (game->enemies_left == 0 || game->game_over == 1 || gui_is_terminated());
   	}
    if(game->enemies_left == 0) gui_set_finished_level(game->score);
    if(game->game_over) gui_set_game_over();
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

	/* Process POWERUP items */
	process_bonus_items(game);

	/* Process bombs */
	process_bombs(game);

}

void render_game(Game * game) {
	int i;
	
    render_level(&game->level);
    render_player(&game->player);
	for( i = 0; i < game->enemies_left; i++ ){
		if( !game->enemies[i].is_dead ) {
			render_enemy(&game->enemies[i]);
		}
    }
    gui_set_enemies_left(game->enemies_left);
    gui_set_bombs_left(game->player.remaining_bombs);
    gui_draw_buffer();
}

void destroy_game(Game * game) {
    destroy_player(&game->player);
    free(game->enemies);
}

void do_player_movement(Game * game) {
    if(game->input.hasMoved) {
        move_player(&game->player, game->input.moves, game->level.entities);
    }
}

void do_enemy_ai(Game * game) {
	int i;
	for ( i = 0; i < game->level.level_info.nr_of_enemies; i++ ){
		update_enemy(&game->enemies[i], game);
	}
}

void process_bonus_items(Game * game) {
}

void process_bombs(Game * game) {
    int i, j;
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
            int occupied = collides_with(game, bomb.x, bomb.y);
            if(!occupied && bomb.ticks_left < 0) bomb.ticks_left *= -1;

            // De ticks van de bom in entities moet ook aangepast worden.
            game->level.entities[i][j].bomb.ticks_left = bomb.ticks_left; 

            // En we ontploffen als we 0 zijn.
            if(bomb.ticks_left == 0) {
                int power = game->player.current_bomb_power;
                int spread[4];
                int done[4] = {0,0,0,0};
                int a = 1;
                // Kijk op welke plekken de explosies zich spreidt
                                                
                while( a <= power ){
                    if(!done[0]){
                        if(game->level.entities[i][j - a].type == OBSTACLE){
                            if(game->level.entities[i][j - a].obstacle.is_destructable) {
                                spread[0] = a;
                            }
                            else {spread[0] = a - 1;}                       
                            done[0] = 1;
                        } else {
                            if(collides_with(game, i * TILE_SIZE, (j - a) * TILE_SIZE) == 1) game->game_over = 1;
                            if(collides_with(game, i * TILE_SIZE, (j - a) * TILE_SIZE) == 2) done[0] = 1;
                            spread[0] = a;
                        }
                    } 
                    if(!done[1]){
                        if(game->level.entities[i][j + a].type == OBSTACLE){
                            if(game->level.entities[i][j + a].obstacle.is_destructable) {
                                spread[1] = a;
                             }
                            else {spread[1] = a - 1;}                    
                            done[1] = 1;
                        } else {
                            if(collides_with(game, i * TILE_SIZE, (j + a) * TILE_SIZE) == 1) game->game_over = 1;
                            if(collides_with(game, i * TILE_SIZE, (j + a) * TILE_SIZE) == 2) done[1] = 1;
                            spread[1] = a;
                        }
                    }
                    if(!done[2]){
                        if(game->level.entities[i + a][j].type == OBSTACLE){
                            if(game->level.entities[i + a][j].obstacle.is_destructable) {
                                spread[2] = a;
                             }
                            else {spread[2] = a - 1;}                        
                            done[2] = 1;
                        } else {
                            if(collides_with(game, (i + a) * TILE_SIZE, j * TILE_SIZE) == 1) game->game_over = 1;
                            if(collides_with(game, (i + a) * TILE_SIZE, j * TILE_SIZE) == 2) done[2] = 1;
                            spread[2] = a;
                        }
                    } 
                    if(!done[3]){
                        if(game->level.entities[i - a][j].type == OBSTACLE){
                            if(game->level.entities[i - a][j].obstacle.is_destructable) {
                                spread[3] = a;
                             }
                            else {spread[3] = a - 1;}                         
                            done[3] = 1;
                        } else {
                            if(collides_with(game, (i - a) * TILE_SIZE, j * TILE_SIZE) == 1) game->game_over = 1;
                            if(collides_with(game, (i - a) * TILE_SIZE, j * TILE_SIZE) == 2) done[3] = 1;
                            spread[3] = a;
                        }
                    }                            
                    a++;
                }
                
                put_explosion(game->level.entities, i, j, spread, game->player.current_bomb_power, EXPLOSION_TICKS);
            }

        } 

        if(game->level.entities[i][j].type == EXPLOSION) {
            Explosion exp = game->level.entities[i][j].explosion;
            exp.ticks_left--;
            
            game->level.entities[i][j].explosion.ticks_left = exp.ticks_left;
            
            if(exp.ticks_left == 0) {
                put_empty_space(game->level.entities, i, j);
                if(game->level.entities[i][j - exp.spread[0]].type == OBSTACLE 
                   && game->level.entities[i][j - exp.spread[0]].obstacle.is_destructable){
                        if(rand() % 100 < game->level.level_info.bonus_spawn_ratio * 100){
                            put_powerup(game->level.entities, i, j - exp.spread[0], rand() % 3);
                        } else {
                            put_empty_space(game->level.entities, i, j - exp.spread[0]);                        
                        }
                }  
                if(game->level.entities[i][j + exp.spread[1]].type == OBSTACLE 
                   && game->level.entities[i][j + exp.spread[1]].obstacle.is_destructable){
                        if(rand() % 100 < game->level.level_info.bonus_spawn_ratio * 100){
                            put_powerup(game->level.entities, i, j + exp.spread[1], rand() % 3);
                        } else {
                            put_empty_space(game->level.entities, i, j + exp.spread[1]);                        
                        }
                } 
                if(game->level.entities[i + exp.spread[2]][j].type == OBSTACLE 
                   && game->level.entities[i + exp.spread[2]][j].obstacle.is_destructable){
                        if(rand() % 100 < game->level.level_info.bonus_spawn_ratio * 100){
                            put_powerup(game->level.entities, i + exp.spread[2], j, rand() % 3);
                        } else {
                            put_empty_space(game->level.entities, i + exp.spread[2], j);                        
                        }
                }  
                if(game->level.entities[i - exp.spread[3]][j].type == OBSTACLE 
                   && game->level.entities[i - exp.spread[3]][j].obstacle.is_destructable){
                        if(rand() % 100 < game->level.level_info.bonus_spawn_ratio * 100){
                            put_powerup(game->level.entities, i - exp.spread[3], j, rand() % 3);
                        } else {
                            put_empty_space(game->level.entities, i - exp.spread[3], j);                        
                        }
                }   
  
            }
            

        }
    }
}

