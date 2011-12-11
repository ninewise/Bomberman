/**
 * Naam: Frederique De Baerdemaeker, Felix Van der Jeugt
 * Bestand: enemy.c
 * Groepsnummer: 3 
 */
#include "enemy.h"
#include "blast_it.h"
#include "gui.h"
#include <stdlib.h>
#include "entity.h"
#include <stdio.h>
#include "collision.h"

#define ABSTILE(X)      ((X) / TILE_SIZE * TILE_SIZE)
#define TILE(X)         ((X) / TILE_SIZE)

void init_enemy(Enemy* enemy, Level* level){
	do{
		enemy->x = rand() % (level->level_info.width - 1) * TILE_SIZE;
		enemy->y = rand() % (level->level_info.height - 1) * TILE_SIZE;
	} while( (enemy->x < level->level_info.width * TILE_SIZE / 5 && enemy->y < level->level_info.height * TILE_SIZE / 5) 
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
    int dx, dy, increment, count = 0, next;
    if(enemy->frozen){
       enemy->frozen--;
    } else {
    
        if(enemy->is_boss) increment = BOSS_MOVEMENT_INCREMENT;
        else increment = ENEMY_MOVEMENT_INCREMENT;

        // Als de vijand mooi op een tegel staat, geven we hem een nieuwe richting.
        if(ABSTILE(enemy->x) == enemy->x && ABSTILE(enemy->y) == enemy->y) {
         // Een array die bijhoudt in welke richtingen de vijand kan bewegen.
            int walkable[4] = {
              is_walkable(game->level.entities[TILE(enemy->x)][TILE(enemy->y) - 1]),
              is_walkable(game->level.entities[TILE(enemy->x) + 1][TILE(enemy->y)]),
              is_walkable(game->level.entities[TILE(enemy->x)][TILE(enemy->y) + 1]),
              is_walkable(game->level.entities[TILE(enemy->x) - 1][TILE(enemy->y)])
          };
          do {
                // Met een kans van 4/10 kiezen we een nieuwe richting voor de vijand.
                if(rand() % 10 < 4) enemy->move_direction = rand() % 4;
                count++;
            } while(!walkable[enemy->move_direction] && count < 10);
        }   

        dx = 0;
        dy = 0;

        if(count == 10); // De vijand zit hoogstwaarschijnlijk vast, we doen niets.
        else if(enemy->move_direction == NORTH) dy--;
        else if(enemy->move_direction == SOUTH) dy++;
        else if(enemy->move_direction == EAST) dx++;
        else if(enemy->move_direction == WEST) dx--;

        // We kijken of de tegel waar de vijand naar gaat een bom is.
        if(dx > 0 || dy > 0)
            next = is_walkable(game->level.entities[TILE(enemy->x) + dx][TILE(enemy->y) + dy]);
        else
            next = is_walkable(game->level.entities[TILE(enemy->x)][TILE(enemy->y)]);
        if(next == 2) { // Als de vijand op een bom zit, laten we hem
                        // achteruit lopen.
            enemy->x += -dx;
            enemy->y += -dy;
        } else if(is_abs_walkable(game->level.entities, enemy->x + dx * increment, enemy->y + dy * increment)) {
            enemy->x += dx;
            enemy->y += dy;
        }
    }
    
    if(collides_with(game, enemy->x, enemy->y) == - 1) game->game_over = 1;
}

void render_enemy(Enemy* enemy){
	gui_add_enemy(enemy);
}

void destroy_enemy(Game* game, Enemy* enemy){
    if(enemy->remaining_lives > 0){
        enemy->remaining_lives--;
        enemy->remaining_lives *= -1;
    }
    if(!enemy->remaining_lives){
    // Bonus droppen, enemy naar kerkhof (-5,-5) verplaatsen, score bijtellen
        if(rand() % 100 < game->level.level_info.bonus_spawn_ratio * 100){
            put_powerup(game->level.entities, enemy->x / TILE_SIZE, enemy->y / TILE_SIZE, rand() % 3);
        }
        enemy->x = -5;
        enemy->y = -5;
        enemy->is_dead = 1;
        game->enemies_left--;
        if(enemy->is_boss) game->score += SCORE_BOSS;
        else if(BASE_SCORE_ENEMY - gui_get_timer_score() > 0) game->score += (BASE_SCORE_ENEMY - gui_get_timer_score());
    }
}
