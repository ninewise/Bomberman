/**
 * Naam: Frederique De Baerdemaeker, Felix Van der Jeugt
 * Bestand: main.c
 * Groepsnummer: 3 
 */
#pragma comment(lib, "jvm.lib")

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "level.h"
#include "gui.h"
#include "cmdqueue.h"
#include "highscores.h"

int mainHS(int argc, char *argv[]) {
	load_highscores();
	display_highscores();
	check_highscore_entry(7);
	display_highscores();
	save_highscores();

	load_highscores();
	display_highscores();
	check_highscore_entry(4);
	display_highscores();
	save_highscores();

	load_highscores();
	display_highscores();
	check_highscore_entry(1);
	check_highscore_entry(8);
	check_highscore_entry(2);
	check_highscore_entry(6);
	check_highscore_entry(9);
	display_highscores();
	save_highscores();

	load_highscores();
	display_highscores();
	save_highscores();

    return 0;
}

/*
(optional) parameters:
	- Number of the level to start in.
	- Random seed for level generation.
*/
int main(int argc, char *argv[]) 
{
	int total_score = 0;	
	{
		Game game;
		char answer = 'y';

		/* Initialize level number */
		int level = argc >= 2 ? atoi(argv[1]) : 1;


		/* Initialize random seed */
		int seed = argc == 3 ? atoi(argv[2]) : (int)time(NULL);
		srand(seed);
		printf("Seed is %d...\n\n", seed);

		/* Initialize the gui */
		gui_initialize();

		while(answer == 'y') {
			/* Initialize the next game instance */
			init_game(&game, level);

			/* Start the actual game loop */
			do_game_loop(&game);

			/* Check if player cleared the level */
			if(!game.game_over) {
				total_score += game.score;
				level++;
			}

			/* Clean up */
			destroy_game(&game);

			/* Ask for user input */
			printf("Continue playing? (y/n) \n");
			answer=getchar();getchar();
		}

		gui_clean();
	}
	printf("\nYour total score is %d\n", total_score);
	

	/* highscores */
	load_highscores();
	check_highscore_entry(total_score);
	display_highscores();
	save_highscores();

	getchar();getchar();

	return 0;
}
