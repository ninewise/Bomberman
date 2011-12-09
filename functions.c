
#include "functions.h"
#include "blast_it.h"

int rectangle_overlap(int rect1[4], int rect2[4]) {
    // Een array van de 4 hoeken van rect2, telkens x en y coordinaat.
    int corners[8] = {
        rect2[0], rect2[1], // linksboven
        rect2[0], rect2[3], // linksonder
        rect2[2], rect2[1], // rechtsboven
        rect2[2], rect2[3]  // rechtonder
    };
    int overlap = 0;
    int i;
    for(i = 0; i < 8; i+=2) overlap = overlap || (
        rect1[0] <= corners[ i ] && corners[ i ] <= rect1[2] &&
        rect1[1] <= corners[i+1] && corners[i+1] <= rect1[2] );
    return overlap;
}

int tile_overlap(int tile1[2], int tile2[2]) {
    int rect1[4] = {tile1[0], tile1[1], tile1[0] + TILE_SIZE, tile1[1] + TILE_SIZE};
    int rect2[4] = {tile2[0], tile2[1], tile2[0] + TILE_SIZE, tile2[1] + TILE_SIZE};
    return rectangle_overlap(rect1, rect2);
}
