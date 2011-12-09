#ifndef H_ENEMY
#define H_ENEMY

#include "blast_it.h"
#include "level.h"

/**
 * Initialiseert de enemy, wijst random locatie toe etc
 */
void init_enemy(Enemy* enemy, Level* level);

/**
 * Tekent de enemy op het scherm.
 */
void render_enemy(Enemy* enemy);

/**
 * Verwijdert de enemy.
 */
void destroy_enemy(Enemy* enemy);

#endif
