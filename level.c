#include "level.h"
#include "blast_it.h"
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
	int i;
	level->level_info = level_info;
	level->entities = (Entity**) malloc(level_info.width * sizeof(Entity*));
	for(i = 0; i < level_info.width; i++) level->entities[i] = (Entity*) calloc(level_info.height, sizeof(Entity));
}

void render_level(Level * level) {
}

void destroy_level(Level * level) {
	int i;
	for(i = 0; i < level->level_info.width; i++) free(level->entities[i]);
	free(level->entities);
}
