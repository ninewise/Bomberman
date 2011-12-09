
#include "enemy.h"
#include "blast_it.h"
#include "gui.h"
#include <stdlib.h>
#include "entity.h"
#include <stdio.h>


void init_enemy(Enemy* enemy, Level* level){
	do{
		enemy->x = rand() % (level->level_info.width - 1) * TILE_SIZE;
		enemy->y = rand() % (level->level_info.height - 1) * TILE_SIZE;
	} while( (enemy->x < level->level_info.width * TILE_SIZE / 3 && enemy->y < level->level_info.height * TILE_SIZE / 3) 
				|| !is_abs_walkable(level->entities, enemy->x, enemy->y) );
	
	enemy->move_direction = rand() % 4;

	if(level->level_info.spawn_boss){
		enemy->is_boss = 1;
		level->level_info.spawn_boss--;
		enemy->remaining_lives = 3;
	} else {
		enemy->is_boss = 0;
		enemy->remaining_lives = 1;
	}

	enemy->is_dead = 0;
	enemy->frozen = 0;
}

void update_enemy(Enemy* enemy, Game* game){      
    int tilex, tiley, increment;    
    
    if(enemy->is_boss){
        increment = BOSS_MOVEMENT_INCREMENT;
    } else {
        increment = ENEMY_MOVEMENT_INCREMENT;
    }

    do {
        tilex = enemy->x;
        tiley = enemy->y;
        if ( enemy->x / TILE_SIZE * TILE_SIZE == enemy->x && enemy->y / TILE_SIZE * TILE_SIZE == enemy->y ){
            if ( rand() % 10 < 4 ) enemy->move_direction = rand() % 4;
            if( enemy->move_direction == NORTH ) tiley -= TILE_SIZE;
            if( enemy->move_direction == SOUTH ) tiley += TILE_SIZE;		
            if( enemy->move_direction == EAST ) tilex += TILE_SIZE;		
            if( enemy->move_direction == WEST ) tilex -= TILE_SIZE;	
        }
    } while ( !is_abs_walkable(game->level.entities, tilex, tiley) );
   
    if( enemy->move_direction == NORTH ) enemy->y -= increment;	
    if( enemy->move_direction == SOUTH ) enemy->y += increment;		
    if( enemy->move_direction == EAST ) enemy->x += increment;		
    if( enemy->move_direction == WEST ) enemy->x -= increment;	

}

void render_enemy(Enemy* enemy){
	gui_add_enemy(enemy);
}

void destroy_enemy(Enemy* enemy){}
