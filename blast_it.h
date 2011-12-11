/**
 * Naam: Frederique De Baerdemaeker, Felix Van der Jeugt
 * Bestand: blast_it.h
 * Groepsnummer: 3 
 */
#ifndef H_BLAST_IT
#define H_BLAST_IT

#define MAX_LEVEL_WIDTH 90
#define MAX_LEVEL_HEIGHT 60
#define TILE_SIZE 30
#define PLAYER_MOVEMENT_INCREMENT 3
#define BOSS_MOVEMENT_INCREMENT 2
#define ENEMY_MOVEMENT_INCREMENT 1
#define BOMB_TICKS 60
#define EXPLOSION_TICKS 30
#define PLAYER_MAX_POWER 5
#define FREEZE_DURATION 100
#define BASE_SCORE_ENEMY 60
#define SCORE_BOSS 150

typedef enum {BOMB, EXPLOSION, POWERUP, OBSTACLE, EMPTY_SPACE} ENTITY_TYPE;
typedef enum {EXTRA_POWER, EXTRA_BOMB, FREEZE_ENEMIES} POWERUP_TYPE;  
typedef enum {NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3} ORIENTATION;
typedef enum {CLASSIC=1, ICE=2, WAREHOUSE=3, DESERT=4, PORTAL=5} REALM;

typedef struct {
	int width;
	int height;
	int level_nr;
	REALM realm;
	double fill_ratio;
	int nr_of_enemies;
	int spawn_boss;
	double bonus_spawn_ratio;
} LevelInfo;

typedef struct {
	int x;
	int y;
	ORIENTATION orientation;
	int current_bomb_power;
	int remaining_bombs;
} Player;

typedef struct {
	int x;
	int y;
	ORIENTATION move_direction;
	int is_boss;
	int remaining_lives;
	int is_dead;
	int frozen;
} Enemy;

typedef struct {
	ENTITY_TYPE type;
	int x;
	int y;
	int ticks_left;
} Bomb;

typedef struct {
	ENTITY_TYPE type;
	int x;
	int y;
	int spread[4];
	int power;
	int ticks_left;
} Explosion;

typedef struct {
	ENTITY_TYPE type;
	int x;
	int y;
	POWERUP_TYPE powerup_type;
} Powerup;

typedef struct {
	ENTITY_TYPE type;
	int x;
	int y;
	int is_destructable;
} Obstacle;

typedef struct {
	ENTITY_TYPE type;
	int x;
	int y;
} EmptySpace;

typedef union {
	ENTITY_TYPE type;
	Bomb bomb;
	Explosion explosion;
	Powerup powerup;
	Obstacle obstacle;
	EmptySpace empty_space;
} Entity;

#endif
