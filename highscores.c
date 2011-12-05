#include <stdio.h>
#include <stdlib.h> 
#include "highscores.h"

Highscorelist *list;

void load_highscores()
{	
	FILE * hs = fopen(HIGHSCORE_FILE,"rb+");
    list = malloc(sizeof(Highscorelist));
	list->size = malloc(sizeof(int));
	if(hs){
		fread(list->size,sizeof(int),1,hs);
		list->list = malloc(*list->size * sizeof(Highscore));
		fread(list->list,sizeof(Highscore),*list->size,hs);
		fclose(hs);
	} else {
		*list->size = 0;
        list->list = calloc(0, 0);
	}
}

void check_highscore_entry(int score)
{
	int i = 0;
	Highscore current, next;

    // We zetten i waar de nieuwe score zou moeten komen.
	while(i < *list->size && list->list[i].score > score) i++;

    // Als i = MAX_HIGHSCORE_ENTRIES, dan is de nieuwe score kleiner dan alle
    // highscores, en moeten we dus niet toevoegen. Anders inserten we de
    // nieuwe score op plaats i.
	if( i < MAX_HIGHSCORE_ENTRIES ) {
		current.score = score;
		printf("Enter your name: ");
		scanf("%s", current.name);

		if( *list->size < MAX_HIGHSCORE_ENTRIES ){
            // We maken de lijst groter, zodat ze de nieuwe score kan bevatten.
			list->list = realloc(list->list, ++*list->size * sizeof(Highscore));
		}

        // We passen een bubblesort toe. Dat kunnen we enkelvoudig doen, omdat
        // er maar 1 element niet juist staat. Dat element zit nu in current.
		while(i < *list->size) {
			if( i + 1 != *list->size ) next = list->list[i];
			list->list[i] = current;
			if( i + 1 != *list->size ) current = next;
			i++;
		}
		
	}	


}

void display_highscores()
{
	int i;
	printf("\n HIGHSCORELIST \n");
	for( i = 0 ; i < *list->size ; i++){
		printf("%d. Score: %d \t Naam: %s \n",i+1,list->list[i].score,list->list[i].name);
	}
}

void save_highscores()
{
	FILE* hs = fopen(HIGHSCORE_FILE,"wb");
    if(hs) {
        fwrite(list->size,sizeof(int),1,hs);
        fwrite(list->list,sizeof(Highscore),*list->size,hs);
        fclose(hs);
    }
	free(list->list);
	free(list->size);
}
