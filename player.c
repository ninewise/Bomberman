
#include "player.h"
#include "blast_it.h"
#include "level.h"
#include "gui.h"
#include <stdio.h>
#include "entity.h"

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

void move_player(Player* player, int moves[4], Entity** entities) {
    // We translateren de player naar het midden van de onderste helft van het
    // hokje, wat het gemakkelijker maakt om geldige plaatsen te berekenen.
    int ox = player->x + TILE_SIZE/2;   // De oude x-coordinaat;
    int oy = player->y + TILE_SIZE/2;   // De oude y-coordinaat;
    int nx = ox;                        // De nieuwe x-coordinaat;
    int ny = oy;                        // De nieuwe y-coordinaat;

    // De afstand die de player van de randen/obstakels moet blijven.
    int dx = TILE_SIZE / 2 - 3;
    int dy = TILE_SIZE / 2 - 3;

    // We berekenen de nieuwe kandidaatplaats.
    if(moves[0]) ny -= PLAYER_MOVEMENT_INCREMENT;
    if(moves[1]) ny += PLAYER_MOVEMENT_INCREMENT;
    if(moves[2]) nx += PLAYER_MOVEMENT_INCREMENT;
    if(moves[3]) nx -= PLAYER_MOVEMENT_INCREMENT;

    // We lossen horizontale collisions met entiteiten op. Dit door te kijken
    // naar de vier hoeken als we horizontaal bewegen (en dit dan horizontaal
    // te corrigeren).
    if((nx <= ox && !is_abs_walkable(entities, nx - dx, oy - dy))
    || (nx >= ox && !is_abs_walkable(entities, nx + dx, oy - dy))
    || (nx <= ox && !is_abs_walkable(entities, nx - dx, oy + dy))
    || (nx >= ox && !is_abs_walkable(entities, nx + dx, oy + dy)))
        nx = ox;

    // Eveneens de verticale.
    if((ny <= oy && !is_abs_walkable(entities, ox - dx, ny - dy))
    || (ny >= oy && !is_abs_walkable(entities, ox - dx, ny + dy))
    || (ny <= oy && !is_abs_walkable(entities, ox + dx, ny - dy))
    || (ny >= oy && !is_abs_walkable(entities, ox + dx, ny + dy)))
        ny = oy;

    // Nu kunnen we nog diagonale clashes, die niet horizontaal of verticaal
    // clashen, hebben. Die kunnen we oplossen door de speler gewoon enkel
    // horizontaal te bewegen.
    if(!is_abs_walkable(entities, nx + dx, ny + dy)
    || !is_abs_walkable(entities, nx + dx, ny - dy)
    || !is_abs_walkable(entities, nx - dx, ny + dy)
    || !is_abs_walkable(entities, nx - dx, ny - dy)) {
        ny = oy;
        nx = ox;
    }

    // Tenslotte maken we dit de nieuwe positie.
    player->x = nx - TILE_SIZE / 2;
    player->y = ny - TILE_SIZE / 2;

    // Nu nog de orientatie veranderen.
    if(ox == nx) player->orientation = (oy > ny) ? NORTH : SOUTH;
    else player->orientation = (ox > nx) ? WEST : EAST;
}

void player_drop_bomb(Player * player, Entity** entities) {
    Entity bomb_entity;
    Bomb bomb = {
        BOMB,
        (player->x + TILE_SIZE/2)/TILE_SIZE * TILE_SIZE,
        (player->y + TILE_SIZE/2)/TILE_SIZE * TILE_SIZE,
        // Een kleine kunstgreep: We houden de ticks_left negatief zolang de
        // speler bovenop de bom staat. Dan kunnen we de speler er over laten
        // lopen.
        -BOMB_TICKS
    };
    bomb_entity.bomb = bomb;

    entities[player->x / TILE_SIZE][player->y / TILE_SIZE] = bomb_entity;
}

void destroy_player(Player* player) {}
