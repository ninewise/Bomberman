/**
 * Naam: Frederique De Baerdemaeker, Felix Van der Jeugt
 * Bestand: enemy.h
 * Groepsnummer: 3 
 */
#ifndef H_ENEMY
#define H_ENEMY

#include "blast_it.h"
#include "level.h"
#include "game.h"

/**
 * Initialiseert de enemy, wijst random locatie toe etc
 */
void init_enemy(Enemy* enemy, Level* level);

/**
 * Laat de enemy denken en handelen
 */
 
void update_enemy(Enemy* enemy, Game* game);

/**
 * Tekent de enemy op het scherm
 */
void render_enemy(Enemy* enemy);

/**
 * Beschadigt de vijand en vernietigt deze eventueel
 */
void destroy_enemy(Game* game, Enemy* enemy);

#endif
