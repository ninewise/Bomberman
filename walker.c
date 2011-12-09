
#include "walker.h"
#include "level.h"
#include "blast_it.h"
#include "player.h"
#include "entity.h"

void move_walker(Walker* walker, int moves[4], Entity** entities, int distance) {
    // We translateren de walker naar het midden van de onderste helft van het
    // hokje, wat het gemakkelijker maakt om geldige plaatsen te berekenen.
    int ox = walker->x + PLAYER_TRANSLATE_X;   // De oude x-coordinaat;
    int oy = walker->y + PLAYER_TRANSLATE_Y; // De oude y-coordinaat;
    int nx = ox;                        // De nieuwe x-coordinaat;
    int ny = oy;                        // De nieuwe y-coordinaat;

    // De afstand die de walker van de randen/obstakels moet blijven.
    int dx = TILE_SIZE / 3;
    int dy = TILE_SIZE / 4;

    // We berekenen de nieuwe kandidaatplaats.
    if(moves[0]) ny -= distance;
    if(moves[1]) ny += distance;
    if(moves[2]) nx += distance;
    if(moves[3]) nx -= distance;

    // We lossen horizontale collisions met entiteiten op. Dit door te kijken
    // naar de vier hoeken als we horizontaal bewegen (en dit dan horizontaal
    // te corrigeren).
    if((nx <= ox && !is_abs_walkable(entities, nx - dx, ny - dy))
    || (nx >= ox && !is_abs_walkable(entities, nx + dx, ny - dy))
    || (nx <= ox && !is_abs_walkable(entities, nx - dx, ny + dy))
    || (nx >= ox && !is_abs_walkable(entities, nx + dx, ny + dy)))
        nx = ox;

    // Eveneens de verticale.
    if((ny <= oy && !is_abs_walkable(entities, nx - dx, ny - dy))
    || (ny >= oy && !is_abs_walkable(entities, nx - dx, ny + dy))
    || (ny <= oy && !is_abs_walkable(entities, nx + dx, ny - dy))
    || (ny >= oy && !is_abs_walkable(entities, nx + dx, ny + dy)))
        ny = oy;

    // Tenslotte maken we dit de nieuwe positie.
    walker->x = nx - TILE_SIZE/2;
    walker->y = ny - 3*TILE_SIZE/4;

    // Nu nog de orientatie veranderen.
    if(ox == nx) walker->orientation = (oy > ny) ? NORTH : SOUTH;
    else walker->orientation = (ox > nx) ? WEST : EAST;

}
