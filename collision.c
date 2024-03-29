/**
 * Naam: Frederique De Baerdemaeker, Felix Van der Jeugt
 * Bestand: collision.c
 * Groepsnummer: 3 
 */
#include "collision.h"
#include "game.h"
#include "enemy.h"
#include "blast_it.h"

int rectangle_overlap(int rect1[4], int rect2[4]) {
    int i;
    // Een array van de 4 hoeken van rect2, telkens x en y coordinaat.
    int corners1[8] = {
        rect1[0], rect1[1], // linksboven
        rect1[0], rect1[3], // linksonder
        rect1[2], rect1[1], // rechtsboven
        rect1[2], rect1[3]  // rechtonder
    };
    int corners2[8] = {
        rect2[0], rect2[1], // linksboven
        rect2[0], rect2[3], // linksonder
        rect2[2], rect2[1], // rechtsboven
        rect2[2], rect2[3]  // rechtonder
    };
    int overlap = 0;
    // Als de rechthoeken elkaar "raken," kunnen ze niet overlappen.
    if(rect1[0] == rect2[2] || rect1[1] == rect2[3]
    || rect1[2] == rect2[0] || rect1[3] == rect2[1]) return 0;

    // Kijken of een hoek van de ene in een andere ligt.
    for(i = 0; i < 8; i+=2) overlap = overlap || (
        rect1[0] <= corners2[ i ] && corners2[ i ] <= rect1[2] &&
        rect1[1] <= corners2[i+1] && corners2[i+1] <= rect1[3] );
    // Vice versa.
    for(i = 0; i < 8; i+=2) overlap = overlap || (
        rect2[0] <= corners1[ i ] && corners1[ i ] <= rect2[2] &&
        rect2[1] <= corners1[i+1] && corners1[i+1] <= rect2[3] );
    return overlap;
}

int tile_overlap(int tile1[2], int tile2[2]) {
    int rect1[4] = {tile1[0], tile1[1],
                    tile1[0] + TILE_SIZE,
                    tile1[1] + TILE_SIZE
    };
    int rect2[4] = {tile2[0], tile2[1],
                    tile2[0] + TILE_SIZE,
                    tile2[1] + TILE_SIZE
    };
    return rectangle_overlap(rect1, rect2);
}

int loose_collides_with(Game* game, int x, int y, int precision) {
    int e;
    int collision = 0;
    // Check voor collisie met player
    int tile[4] = {x + precision, y + precision, x + TILE_SIZE - precision, y + TILE_SIZE - precision};
    int player[4] = {game->player.x + precision, game->player.y + precision,
                     game->player.x + TILE_SIZE - precision,
                     game->player.y + TILE_SIZE - precision};
    collision = rectangle_overlap(tile, player);
    if(collision) return -1;
    // Check voor collisie met vijand, als er 1 is geven we zijn index+1 terug
    // De +1 dient omdat we 0 gebruiken voor return zonder collisies
    for(e = 0; e < game->level.level_info.nr_of_enemies; e++) {
        if(!game->enemies[e].is_dead) {
            int enemy[4] = {game->enemies[e].x + precision, game->enemies[e].y + precision,
                            game->enemies[e].x + TILE_SIZE - precision,
                            game->enemies[e].y + TILE_SIZE - precision};
            if(rectangle_overlap(tile, enemy)) return e+1;
        }
    }
    return 0;
}

int collides_with(Game* game, int x, int y) {
    return loose_collides_with(game, x, y, 0);
}
