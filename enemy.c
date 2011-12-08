
#include "enemy.h"
#include "blast_it.h"
#include "gui.h"

void init_enemy(Enemy* enemy){
	enemy->x = 2 * TILE_SIZE;
	enemy->y = 1 * TILE_SIZE;
	enemy->move_direction = WEST;
	enemy->is_boss = 0;
	enemy->remaining_lives = 1;
	enemy->is_dead = 0;
	enemy->frozen = 0;
}

void render_enemy(Enemy* enemy){
	gui_add_enemy(enemy);
}

void destroy_enemy(Enemy* enemy){}
