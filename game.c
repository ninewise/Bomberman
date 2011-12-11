/**
 * Naam: Frederique De Baerdemaeker, Felix Van der Jeugt
 * Bestand: game.c
 * Groepsnummer: 3 
 */
#include "blast_it.h"
#include "gui.h"
#include "game.h"
#include "level.h"
#include "player.h"
#include "enemy.h"
#include <stdlib.h>
#include "collision.h"
#include "entity.h"

#define NORTH(X,Y,A)        game->level.entities[X][Y - A]
#define SOUTH(X,Y,A)        game->level.entities[X][Y + A]
#define EAST(X,Y,A)        game->level.entities[X + A][Y]
#define WEST(X,Y,A)        game->level.entities[X - A][Y]

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
    if(game->game_over) gui_set_game_over();
    else if(game->enemies_left == 0) gui_set_finished_level(game->score);
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
	for( i = 0; i < game->level.level_info.nr_of_enemies; i++ ){
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
    int i;
    Entity tile = game->level.entities[(game->player.x + TILE_SIZE / 2) / TILE_SIZE][(game->player.y  + TILE_SIZE / 2) / TILE_SIZE];
    if(tile.type == POWERUP){
        put_empty_space(game->level.entities, (game->player.x + TILE_SIZE / 2) / TILE_SIZE, (game->player.y + TILE_SIZE / 2) / TILE_SIZE);  
        switch(tile.powerup.powerup_type){
            case EXTRA_POWER: if(game->player.current_bomb_power < PLAYER_MAX_POWER) game->player.current_bomb_power++; break;
            case EXTRA_BOMB: game->player.remaining_bombs++; break;
            case FREEZE_ENEMIES: 
                for( i = 0; i < game->level.level_info.nr_of_enemies; i++ ){
                    game->enemies[i].frozen = FREEZE_DURATION;
                }              
                break;
        }
    }
}

void process_explosion(Game * game, int i, int j){
    Explosion exp = game->level.entities[i][j].explosion;
    exp.ticks_left--;
    // Einde ontploffing        
    if(exp.ticks_left == 0) {
        put_empty_space(game->level.entities, i, j);
        // Kijk of we op een vernietigd obstakel een bonus plaatsen
        if(NORTH(i,j,exp.spread[0]).type == OBSTACLE && NORTH(i,j,exp.spread[0]).obstacle.is_destructable){
            if(rand() % 100 < game->level.level_info.bonus_spawn_ratio * 100){
                put_powerup(game->level.entities, i, j - exp.spread[0], rand() % 3);
            } else {
                put_empty_space(game->level.entities, i, j - exp.spread[0]);                        
            }
        }  
        if(SOUTH(i,j,exp.spread[1]).type == OBSTACLE && SOUTH(i,j,exp.spread[1]).obstacle.is_destructable){
            if(rand() % 100 < game->level.level_info.bonus_spawn_ratio * 100){
                put_powerup(game->level.entities, i, j + exp.spread[1], rand() % 3);
            } else {
                put_empty_space(game->level.entities, i, j + exp.spread[1]);                        
            }
        } 
        if(EAST(i,j,exp.spread[2]).type == OBSTACLE && EAST(i,j,exp.spread[2]).obstacle.is_destructable){
            if(rand() % 100 < game->level.level_info.bonus_spawn_ratio * 100){
                put_powerup(game->level.entities, i + exp.spread[2], j, rand() % 3); 
            } else {
                put_empty_space(game->level.entities, i + exp.spread[2], j);
            }
        }  
        if(WEST(i,j,exp.spread[3]).type == OBSTACLE && WEST(i,j,exp.spread[3]).obstacle.is_destructable){
            if(rand() % 100 < game->level.level_info.bonus_spawn_ratio * 100){
                put_powerup(game->level.entities, i - exp.spread[3], j, rand() % 3);
            } else {
                put_empty_space(game->level.entities, i - exp.spread[3], j);    
            }
        }
        // Maakt beschadigde vijanden klaar om opnieuw schade te kunnen krijgen
        int b;
        for( b = 0; b < game->level.level_info.nr_of_enemies; b++ ){
            if(game->enemies[b].remaining_lives < 0) game->enemies[b].remaining_lives *= -1;
        }
        // Geeft speler een nieuwe bom
        game->player.remaining_bombs++;
    }
    
    // Anders controleren we of er nog vijanden of niet snuggere gebruikers in een explosie zijn gewandeld
    int collision;
    while(exp.spread[0]) {
        collision = loose_collides_with(game, i * TILE_SIZE, (j - exp.spread[0]) * TILE_SIZE, 3);
        if(collision == -1) game->game_over = 1;
        while(collision > 0 && game->enemies[collision -  1].remaining_lives > 0){
            destroy_enemy(game, &game->enemies[collision - 1]);
            collision = loose_collides_with(game, i * TILE_SIZE, (j - exp.spread[0]) * TILE_SIZE, 3);
        }
        exp.spread[0]--; 
    }
    while(exp.spread[1]) {
        collision = loose_collides_with(game, i * TILE_SIZE, (j + exp.spread[1]) * TILE_SIZE, 3);
        if(collision == -1) game->game_over = 1;
        while(collision > 0 && game->enemies[collision -  1].remaining_lives > 0){
            destroy_enemy(game, &game->enemies[collision - 1]);
            collision = loose_collides_with(game, i * TILE_SIZE, (j + exp.spread[1]) * TILE_SIZE, 3);
        }
        exp.spread[1]--; 
    }   
    while(exp.spread[2]) {
        collision = loose_collides_with(game, (i + exp.spread[2]) * TILE_SIZE, j * TILE_SIZE, 3);
        if(collision == -1) game->game_over = 1;
        while(collision > 0 && game->enemies[collision -  1].remaining_lives > 0){
            destroy_enemy(game, &game->enemies[collision - 1]);
            collision = loose_collides_with(game, (i + exp.spread[2]) * TILE_SIZE, j * TILE_SIZE, 3);
        }
        exp.spread[2]--; 
    }
    while(exp.spread[3]) {
        collision = loose_collides_with(game, (i - exp.spread[3]) * TILE_SIZE, j * TILE_SIZE, 3);
        if(collision == -1) game->game_over = 1;
        while(collision > 0 && game->enemies[collision -  1].remaining_lives > 0){
            destroy_enemy(game, &game->enemies[collision - 1]);
            collision = loose_collides_with(game, (i - exp.spread[3]) * TILE_SIZE, j * TILE_SIZE, 3);
        }
        exp.spread[3]--; 
    }  
       
    game->level.entities[i][j].explosion.ticks_left = exp.ticks_left;
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
                // Wanneer ze vroegtijdig stopt met spreiden, wordt done gebruikt
                // om niet verder te gaan kijken
                                                
                while( a <= power ){
                    int collision;
                    if(!done[0]){
                        if(NORTH(i,j,a).type == OBSTACLE){ // Is het een obstakel, dan stopt de explosie
                            if(NORTH(i,j,a).obstacle.is_destructable) {
                                spread[0] = a;
                            } else {spread[0] = a - 1;}                       
                            done[0] = 1;
                        } else { // Anders kijken we of een gebruiker of vijand de explosie vangt
                            collision = loose_collides_with(game, i * TILE_SIZE, (j - a) * TILE_SIZE, 3);
                            if(collision == -1) game->game_over = 1;
                            if(collision > 0) done[0] = 1;
                            while(collision > 0 && game->enemies[collision -  1].remaining_lives > 0){
                                destroy_enemy(game, &game->enemies[collision - 1]);
                                collision = loose_collides_with(game, i * TILE_SIZE, (j - a) * TILE_SIZE, 3);
                            }
                            spread[0] = a;
                        }
                    } 
                    if(!done[1]){
                        if(SOUTH(i,j,a).type == OBSTACLE){
                            if(SOUTH(i,j,a).obstacle.is_destructable) {
                                spread[1] = a;
                             } else {spread[1] = a - 1;}                    
                            done[1] = 1;
                        } else {
                            collision = loose_collides_with(game, i * TILE_SIZE, (j + a) * TILE_SIZE, 3);
                            if(collision == -1) game->game_over = 1;
                            if(collision > 0) done[1] = 1;
                            while(collision > 0 && game->enemies[collision -  1].remaining_lives > 0){
                                destroy_enemy(game, &game->enemies[collision - 1]);
                                collision = loose_collides_with(game, i * TILE_SIZE, (j + a) * TILE_SIZE, 3);
                            }
                            spread[1] = a;
                        }
                    }
                    if(!done[2]){
                        if(EAST(i,j,a).type == OBSTACLE){
                            if(EAST(i,j,a).obstacle.is_destructable) {
                                spread[2] = a;
                             } else {spread[2] = a - 1;}                        
                            done[2] = 1;
                        } else {
                            collision = loose_collides_with(game, (i + a) * TILE_SIZE, j * TILE_SIZE, 3);
                            if(collision == -1) game->game_over = 1;
                            if(collision > 0) done[2] = 1;
                            while(collision > 0 && game->enemies[collision -  1].remaining_lives > 0){
                                destroy_enemy(game, &game->enemies[collision - 1]);
                                collision = loose_collides_with(game, (i + a) * TILE_SIZE, j * TILE_SIZE, 3);
                            }
                            spread[2] = a;
                        }
                    } 
                    if(!done[3]){
                        if(WEST(i,j,a).type == OBSTACLE){
                            if(game->level.entities[i - a][j].obstacle.is_destructable) {
                                spread[3] = a;
                             } else {spread[3] = a - 1;}                         
                            done[3] = 1;
                        } else {
                            collision = loose_collides_with(game, (i - a) * TILE_SIZE, j * TILE_SIZE, 3);
                            if(collision == -1) game->game_over = 1;
                            if(collision > 0) done[3] = 1;
                            while(collision > 0 && game->enemies[collision -  1].remaining_lives > 0){
                                destroy_enemy(game, &game->enemies[collision - 1]);
                                collision = loose_collides_with(game, (i - a) * TILE_SIZE, j * TILE_SIZE, 3);
                            }
                            spread[3] = a;
                        }
                    }                            
                    a++;
                }
                // En we plaatsen de bom op plek van de explosie
                put_explosion(game->level.entities, i, j, spread, game->player.current_bomb_power, EXPLOSION_TICKS);
            }

        } 
        // Als er een explosie aan de gang is, verwerk deze
        if(game->level.entities[i][j].type == EXPLOSION) process_explosion(game, i, j);
    }
}
