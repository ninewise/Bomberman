#ifndef H_HIGHSCORE_TABLE
#define H_HIGHSCORE_TABLE

#define HIGHSCORE_FILE "highscores.dat"
#define MAX_HIGHSCORE_ENTRIES 5

void load_highscores();

void check_highscore_entry(int score);

void display_highscores();

void save_highscores();

typedef struct Highscore
{
	int score;
	char name [60];	
} Highscore;

typedef struct Highscorelist
{
	Highscore* list;
	unsigned int* size;
} Highscorelist;

#endif
