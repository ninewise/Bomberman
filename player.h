
#ifndef H_PLAYER
#define H_PLAYER

#include "blast_it.h"
#include "level.h"

/**
 * Initialiseert de speler, met dummy coordinaten en dergelijke.
 */
void init_player(Player* player);

/**
 * Tekent de speler op het scherm.
 */
void render_player(Player* player);

/**
 * Probeert de speler te verplaatsen in de richtingen gegeven door moves, en
 * houdt daarbij rekening met botsingen in de entiteiten.
 */
void move_player(Player* player, int moves[4], Entity** entities);

/**
 * Laat de speler een bom plaatsen op zijn huidige locatie.
 */
void player_drop_bomb(Player * player, Entity** entities);

/**
 * Verwijdert de speler uit het geheugen.
 */
void destroy_player(Player* player);

#endif
