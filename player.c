
#include "player.h"
#include "blast_it.h"
#include "level.h"
#include "gui.h"
#include <stdio.h>

void init_player(Player* player) {
    player->x = 1 * TILE_SIZE;
    player->y = 1 * TILE_SIZE;
    player->orientation = NORTH;
    player->current_bomb_power = 1;
    player->remaining_bombs = 10;
}

void move_player(Player* player, int moves[4], Level* level) {
    Entity entity;

    // We bepalen de nieuwe coordinaten van de speler.
    int new_x = player->x;
    int new_y = player->y;
    if(moves[0]) new_y -= PLAYER_MOVEMENT_INCREMENT;
    if(moves[1]) new_y += PLAYER_MOVEMENT_INCREMENT;
    if(moves[2]) new_x += PLAYER_MOVEMENT_INCREMENT;
    if(moves[3]) new_x -= PLAYER_MOVEMENT_INCREMENT;

/*
    // We zorgen dat de speler niet buiten het veld loopt.
    if(0 > new_x) new_x = 0;
    else if(new_x >= level->level_info.width)
        new_x = level->level_info.width - 1;

    if(0 > new_y) new_y = 0;
    else if(new_y >= level->level_info.height)
        new_y = level->level_info.height - 1;

    // We controleren of de speler wel naar links/rechts kon bewegen.
    entity = level->entities[new_x / TILE_SIZE][player->y / TILE_SIZE];
    if(entity.type != EMPTY_SPACE) {
        new_x = player->x - (player->x % TILE_SIZE);
    }
    // We controleren of de speler wel naar boven/beneden kon bewegen.
    entity = level->entities[player->x / TILE_SIZE][new_y / TILE_SIZE];
    if(entity.type != EMPTY_SPACE) {
        new_y = player->y - (player->y % TILE_SIZE);
    }
    */

    // En we stellen de nieuwe coordinaten in.
    player->x = new_x;
    player->y = new_y;
}

void render_player(Player* player) {
    gui_add_player(player);
}

void destroy_player(Player* player) {}
