
#include "enemy.h"
#include "blast_it.h"
#include "gui.h"
#include <stdlib.h>
#include "entity.h"


void init_enemy(Enemy* enemy, Level* level){
	do{
		enemy->x = rand() % level->levelinfo.width * TILE_SIZE;
		enemy->y = rand() % level->levelinfo.height * TILE_SIZE;
	} while( enemy->x < level->levelinfo.width * TILE_SIZE / 3 && enemy->y < level->levelinfo.height * TILE_SIZE / 3 && is_walkable(level.entities[enemy->x][enemy->y] );
	enemy->move_direction = rand() % 4;
	if(level->levelinfo.spawn_boss){
		enemy->is_boss = 1;
		enemy->remaining_lives = 3;
	} else {
		enemy->is_boss = 0;
		enemy->remaining_lives = 1;
	}
	enemy->is_dead = 0;
	enemy->frozen = 0;
}

void render_enemy(Enemy* enemy){
	gui_add_enemy(enemy);
}

void destroy_enemy(Enemy* enemy){}
