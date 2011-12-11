/**
 * Naam: Frederique De Baerdemaeker, Felix Van der Jeugt
 * Bestand: level.h
 * Groepsnummer: 3 
 */
#include "blast_it.h"

#ifndef H_LEVEL
#define H_LEVEL

typedef struct {
	LevelInfo level_info;
	Entity ** entities;
} Level;

LevelInfo generate_level_info(int level_nr);

void init_level(Level * level, LevelInfo level_info);

void render_level(Level * level);

void destroy_level(Level * level);

#endif
