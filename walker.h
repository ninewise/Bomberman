
#ifndef H_WALKER
#define H_WALKER

#include "blast_it.h"

typedef union Walker {
    int x;
    int y;
    ORIENTATION orientation;
    Player player;
    Enemy enemy;
} Walker;

/**
 * Verplaats de walker (een player of enemy) over distance, in de richtingen
 * gegeven door moves. Maakt enkel bewegingen die geen collisions met entities
 * geeft.
 */
void move_walker(Walker* walker, int moves[4], Entity** entities, int distance);

#endif
