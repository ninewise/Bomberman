
#ifndef H_PLAYER
#define H_PLAYER

#include "blast_it.h"
#include "level.h"

/**
 * Initialiseerd de speler, met dummy coordinaten en dergelijke.
 */
void init_player(Player* player);

/**
 * Tekent de speler op het scherm.
 */
void render_player(Player* player);

/**
 * Laat de speler een bom plaatsen op zijn huidige locatie.
 */
void player_drop_bomb(Player * player, Entity** entities);

/**
 * Verwijderd de speler uit het geheugen.
 */
void destroy_player(Player* player);

#endif
