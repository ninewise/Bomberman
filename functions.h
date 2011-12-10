
#ifndef H_FUNCTIONS
#define H_FUNCTIONS

/**
 * Geeft terug of de twee gegeven rechthoeken overlappen.
 * Een rechthoek is een array van 4 integers, in volgorde de x en y coordinaat
 * van de linkerbovenhoek en de z en y coordinaat van de rechteronderhoek.
 */
int rectangle_overlap(int rect1[4], int rect2[4]);

/**
 * Zelfde, maar voor twee tegels. Enkel de linkerbovenhoek moet dus meegegeven
 * worden.
 */
int tile_overlap(int tile1[2], int tile2[2]);

#endif