
#include "player.h"
#include "blast_it.h"
#include "level.h"
#include "gui.h"
#include <stdio.h>
#include "walker.h"

void init_player(Player* player) {
    player->x = 1 * TILE_SIZE;
    player->y = 1 * TILE_SIZE;
    player->orientation = NORTH;
    player->current_bomb_power = 1;
    player->remaining_bombs = 10;
}

void move_player(Player* player, int moves[4], Entity** entities) {
    Walker walker;
    walker.player = *player;
    move_walker(&walker, moves, entities, PLAYER_MOVEMENT_INCREMENT);
    *player = walker.player;
}

void render_player(Player* player) {
    gui_add_player(player);
}

void destroy_player(Player* player) {}
