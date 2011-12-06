#include "level.h"
#include "blast_it.h"
#include "gui.h"
#include <stdlib.h>

LevelInfo generate_level_info(int level_nr)
{
	LevelInfo level_info;
	level_info.width = 20;
	level_info.height = 12;
	level_info.level_nr = level_nr;
	level_info.realm = CLASSIC;
	level_info.fill_ratio = 0.25;
	level_info.nr_of_enemies = 1;
	level_info.spawn_boss = 0;
	level_info.bonus_spawn_ratio = 1;
	return level_info;
}

void init_level(Level * level, LevelInfo level_info) {
	int i, j;
	level->level_info = level_info;
	level->entities = (Entity**) malloc(level_info.width * sizeof(Entity*));
    randomize();
	for(i = 0; i < level_info.width; i++) {
        level->entities[i] = (Entity*) calloc(level_info.height, sizeof(Entity));
        for(j = 0; j < level_info.height - 1; j++) {
            // Het veld opvullen met entiteiten, te beginnen met vaste blokken.
            if(i == 0 || j == 0 || i+1 == level_info.height || (i%2==0 && j%2==0)) {
                level->entities[i][j].type = OBSTACLE;
                level->entities[i][j].obstacle.x = i;
                level->entities[i][j].obstacle.y = j;
                level->entities[i][j].obstacle.is_destructable = 1
            } else if(0 == random(4)) {
                level->entities[i][j].type = OBSTACLE;
                level->entities[i][j].obstacle.x = i;
                level->entities[i][j].obstacle.y = j;
                level->entities[i][j].obstacle.is_destructable = 0;
            } else {
                level->entities[i][j].type = EMPTY_SPACE;
                level->entities[i][j].empty_space.x = i;
                level->entities[i][j].empty_space.y = j;
            }
        }
        level->entities[i][level_info.height - 1].type = OBSTACLE;
        level->entities[i][level_info.height - 1].obstacle.x = i;
        level->entities[i][level_info.height - 1].obstacle.y = level_info.height - 1;
        level->entities[i][level_info.height - 1].obstacle.is_destructable = 1;
    }
}

void render_level(Level * level) {
    int i, j;
    for(i = 0; i < level->level_info.height; i++) for(j = 0; j < level->level_info.width; j++) {
        switch(level->entities[i][j].type) {
            case BOMB: gui_add_bomb(level->entities[i][j].bomb);
            case EXPLOSION:;
            case POWERUP: gui_add_powerup(level->entities[i][j].powerup);
            case OBSTACLE: gui_add_obstacle(level->entities[i][j].obstacle);
        }
    }
}

void destroy_level(Level * level) {
	int i;
	for(i = 0; i < level->level_info.width; i++) free(level->entities[i]);
	free(level->entities);
}
