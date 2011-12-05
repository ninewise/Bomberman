
#ifndef H_PLAYER
#define H_PLAYER

#include "blast_it.h"
#include "level.h"

/**
 * Initialiseerd de speler, met dummy coordinaten en dergelijke.
 */
void init_player(Player* player);

/**
 * Verplaatst de player volgens de verplaatsing meegeven in moves. Houdt
 * rekening met de botsingen met de objecten in het level.
 */
void move_player(Player* player, int moves[4], Level* level);

/**
 * Verwijderd de speler uit het geheugen.
 */
void destroy_player(Player* player);

#endif
