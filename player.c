
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

void render_player(Player* player) {
    gui_add_player(player);
}

void player_drop_bomb(Player * player, Entity** entities) {
    Entity bomb_entity;
    Bomb bomb = {
        BOMB,
        player->x/TILE_SIZE * TILE_SIZE,
        player->y/TILE_SIZE * TILE_SIZE,
        BOMB_TICKS
    };
    bomb_entity.bomb = bomb;
    
    entities[player->x / TILE_SIZE][player->y / TILE_SIZE] = bomb_entity;
}

void destroy_player(Player* player) {}
