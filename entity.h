/**
 * Naam: Frederique De Baerdemaeker, Felix Van der Jeugt
 * Bestand: entity.h
 * Groepsnummer: 3 
 */
#ifndef H_ENTITY
#define H_ENTITY

#include "blast_it.h"

void put_bomb(Entity** entities, int x, int y, int ticks_left);
void put_explosion(Entity** entities, int x, int y, int spread[4], int power, int ticks_left);
void put_powerup(Entity** entities, int x, int y, POWERUP_TYPE type);
void put_obstacle(Entity** entities, int x, int y, int is_destructable);
void put_empty_space(Entity** entities, int x, int y);

/**
 * Geeft terug of een speler of vijand over de tegel waarop (x,y) ligt
 * (absolute coordinaten, geen tegelcoordinaten) kan lopen.
 */
int is_abs_walkable(Entity** entities, int x, int y);

/**
 * Geeft terug of op de meegeven entiteit gestapt kan worden.
 */
int is_walkable(Entity entity);

#endif
