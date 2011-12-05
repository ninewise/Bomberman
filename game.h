#ifndef H_GAME
#define H_GAME

#include "blast_it.h"
#include "level.h"

#define UP      1
#define DOWN    2
#define RIGHT   4
#define LEFT    8

typedef struct Input {
	int hasMoved;
	int moves[4];
	int dropBomb;
} Input;

typedef struct Game {
	Level level;
	Player player;
	Enemy * enemies;
	int enemies_left;
	int game_over;
	Input input;
	int score;
} Game;

void init_game(Game * game, int level_nr);

void do_game_loop(Game * game);

void check_game_input(Game * game);

void update_game(Game * game);

void render_game(Game * game);

void destroy_game(Game * game);


void do_player_movement(Game * game);

void do_enemy_ai(Game * game);

void process_bonus_items(Game * game);

void process_bombs(Game * game);

#endif
