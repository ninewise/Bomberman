#include "level.h"
#include "blast_it.h"
#include "gui.h"
#include <stdlib.h>
#include "entity.h"
#include <math.h>
#include <stdio.h> 

LevelInfo generate_level_info(int level_nr)
{
	LevelInfo level_info;
	level_info.width = rand() % 10 + level_nr + 15;
	level_info.height = level_info.width * 3/4;
	level_info.level_nr = level_nr;
	level_info.realm = ((level_nr - 1) % 5) + 1;
	level_info.fill_ratio = 0.05 + 1/2 * log(level_nr);
	level_info.nr_of_enemies = 3 + level_nr * 5/4 * log(level_nr);
	level_info.spawn_boss = level_nr / 5;
    if( (rand() % 5) + 1 < ((level_nr - 1) % 5) + 1 ){
        level_info.spawn_boss += (rand() % level_nr) * 1/5 * log(level_nr);
    }
	level_info.bonus_spawn_ratio = 1;
	return level_info;
}

void init_level(Level * level, LevelInfo level_info) {
	int i, j, total;
    float ratio;

	level->level_info = level_info;
    // Plaats alloceren voor de tabel van entiteiten.
	level->entities = (Entity**) malloc(level_info.width * sizeof(Entity*));
	for(i = 0; i < level_info.width; i++) {
        level->entities[i] = (Entity*) calloc(level_info.height, sizeof(Entity));
    }

    // We vormen de verhouding zodanig om, dat de drie vakjes linksboven die
    // nooit gevuld mogen worden met obstakels niet meegerekend worden. Moesten
    // die hokjes namelijk opgevuld worden, zou de speler of niet kunnen
    // bewegen, of geen bom kunnen plaatsen zonder zichzelf te raken.
    //
    // De nieuwe verhouding is de oude, vermenigvuldigd met het totaal aantal
    // blokken dat gevuld kan worden, gedeelt door dat aantal minus drie.
    total = (level_info.width - 2)*(level_info.height - 2)
            - ((level_info.width - 2)/2) * ((level_info.height - 2)/2);
    ratio = level_info.fill_ratio * total / (total - 3);

	for(i = 0; i < level_info.width; i++) {
        for(j = 0; j < level_info.height; j++) {
            // Het veld opvullen met entiteiten, te beginnen met vaste blokken.
            if(i == 0 || j == 0 || i+1 == level_info.width || j+1 == level_info.height || (i%2==0 && j%2==0)) {
                put_obstacle(level->entities, i, j, 0);
            } else if(rand() % 100 < ratio * 100) {
                put_obstacle(level->entities, i, j, 1);
            } else {
                put_empty_space(level->entities, i, j);
            }
        }
    }
    // Nu verzekeren we ons ervan dat de coordinaten (1,1) (1,2) en (2,1) leeg
    // zijn, dan kan de speler zeker beginnen.
    put_empty_space(level->entities, 1, 1);
    put_empty_space(level->entities, 1, 2);
    put_empty_space(level->entities, 2, 1);
}

void render_level(Level * level) {
    int i, j;
    // Placing entities.
    for(i = 0; i < level->level_info.width; i++) for(j = 0; j < level->level_info.height; j++) {
        Entity entity = level->entities[i][j];
        switch(entity.type) {
            case EMPTY_SPACE: break;
            case BOMB: gui_add_bomb(&entity.bomb); break;
            case OBSTACLE: if(entity.obstacle.is_destructable) gui_add_obstacle(&entity.obstacle); break;
            case EXPLOSION: break; // Explosions worden pas achteraf getekend, over de rest.
            case POWERUP: gui_add_powerup(&entity.powerup); break;
        }
    }
    for(i = 0; i < level->level_info.width; i++) for(j = 0; j < level->level_info.height; j++)
        if(level->entities[i][j].type == EXPLOSION) {
            Explosion exp = level->entities[i][j].explosion;
            if(level->entities[i][j - 1].type != OBSTACLE
                || level->entities[i][j - 1].obstacle.is_destructable) exp.spread[0] = 1;
            printf("%d %d %d %d \n", exp.spread[0], exp.spread[1],exp.spread[2],exp.spread[3]);
            int a = 0;
            int spread[4] = {exp.spread[0],exp.spread[1],exp.spread[2],exp.spread[3]};
            gui_add_explosion_tile(exp.x, exp.y, 42);
            // De explosies in elke richting uitbereiden.
            while(a <= exp.power) {

                // Als de explosie zich nog in de richting verspreid, tekenen we ze ook.
                if(spread[0]) gui_add_explosion_tile(exp.x, exp.y - a * TILE_SIZE, 42);
                if(spread[1]) gui_add_explosion_tile(exp.x, exp.y + a * TILE_SIZE, 42);
                if(spread[2]) gui_add_explosion_tile(exp.x + a * TILE_SIZE, exp.y, 42);
                if(spread[3]) gui_add_explosion_tile(exp.x - a * TILE_SIZE, exp.y, 42);

                // Als we een obstacle tegen komen, stoppen we de explosie.
                if(spread[0] && level->entities[i][j - a].type == OBSTACLE) spread[0] = 0;
                if(spread[1] && level->entities[i][j + a].type == OBSTACLE) spread[1] = 0;
                if(spread[2] && level->entities[i + a][j].type == OBSTACLE) spread[2] = 0;
                if(spread[3] && level->entities[i - a][j].type == OBSTACLE) spread[3] = 0;

                // 
                a++;
            }
    }
}

void destroy_level(Level * level) {
	int i;
	for(i = 0; i < level->level_info.width; i++) free(level->entities[i]);
	free(level->entities);
}
