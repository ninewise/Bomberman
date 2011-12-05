#define CMD_ALLOC_SIZE 64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jni.h"
#include "blast_it.h"
#include "cmdqueue.h"
#include "gui.h"

queue cmd_queue;
int ready = 0;
JNIEnv* env = NULL;
JavaVM * jvm = NULL;
jclass clsMain = NULL;
jmethodID setLevelInfo = NULL;
jmethodID drawBuffer = NULL;
jmethodID startTimer = NULL;
jmethodID getTimerScore = NULL;
jmethodID setGameOver = NULL;
jmethodID setFinishedLevel = NULL;
jmethodID isMovementKeyDown = NULL;
jmethodID wasSpacePressed = NULL;
jmethodID isTerminated = NULL;

JNIEnv* create_vm(JavaVM ** jvm) {
	int ret;

	JavaVMInitArgs vm_args;
	JavaVMOption options[1];
	options[0].optionString = "-Djava.class.path=blast-it-soi-java.jar";
	vm_args.version = JNI_VERSION_1_6;
	vm_args.nOptions = 1;
	vm_args.options = options;
	vm_args.ignoreUnrecognized = 0;

	ret = JNI_CreateJavaVM(jvm, (void**)&env, &vm_args);
	if(ret < 0)
		printf("gui error: Unable to launch embedded JVM!!.\n");
	return env;
}

void gui_initialize() 
{
	/* create VM */
	env = create_vm(&jvm);
	if(env == NULL) {
		printf("gui error: JVM environment is NULL!!.\n");
		return;
	}

	/* Initialize JNI vars */
	clsMain = (*env)->NewGlobalRef(env, (*env)->FindClass(env, "blastit/Main"));

	if(clsMain != NULL) {
		setLevelInfo = (*env)->GetStaticMethodID(env, clsMain,"setLevelInfo","(IIII)V");
		drawBuffer = (*env)->GetStaticMethodID(env, clsMain,"drawBuffer","([Ljava/lang/String;)V");
		startTimer = (*env)->GetStaticMethodID(env, clsMain,"startTimer","()V");
		getTimerScore = (*env)->GetStaticMethodID(env, clsMain,"getTimerScore","()I");
		setGameOver = (*env)->GetStaticMethodID(env, clsMain,"setGameOver","()V");
		setFinishedLevel = (*env)->GetStaticMethodID(env, clsMain,"setFinishedLevel","(I)V");
		isMovementKeyDown = (*env)->GetStaticMethodID(env, clsMain,"isMovementKeyDown","()I");
		wasSpacePressed = (*env)->GetStaticMethodID(env, clsMain,"wasSpacePressed","()I");
		isTerminated = (*env)->GetStaticMethodID(env, clsMain,"isTerminated","()I");
	} else {
		printf("gui error: Unable to find main GUI class!!.\n");
		return;
	}

	init_queue(&cmd_queue);
	ready = 1;
}

void gui_clean() {
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		ready = 0;
		if(env != NULL) {
			(*env)->DeleteGlobalRef(env, clsMain);
		}
	}
}

void gui_set_level_info(LevelInfo * level_info) 
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(setLevelInfo != NULL) {
			if(level_info->width <= MAX_LEVEL_WIDTH && level_info->height <= MAX_LEVEL_HEIGHT) {
				(*env)->CallStaticVoidMethod(env, clsMain, setLevelInfo, level_info->width, level_info->height, level_info->level_nr, level_info->realm);
			}
		} else {
			printf("gui error: JNI method call failed!!.\n");
			return;
		}
	}
}

void copy_str(char* dest, const char* source)
{
	strncpy(dest, source, CMD_ALLOC_SIZE * sizeof(char)) ;
}

void append_str(char* dest, const char* source)
{
	strncat(dest, source, CMD_ALLOC_SIZE * sizeof(char));
}

/* helper method for command creation */
char* create_cmd_string(const char* op)
{
	char* cmd = (char*)calloc(CMD_ALLOC_SIZE, sizeof(char));
	copy_str(cmd, op);
	return cmd;
}

void destroy_cmd_string(char* cmd)
{
	if(cmd != NULL) {
		free(cmd);
		cmd = NULL;
	}
}

void append_coord(char* cmd, int x, int y, int add_space)
{
	char xy_str[32];
	sprintf(xy_str, "%d %d", x, y);
	append_str(cmd, xy_str);
	if(add_space)
		append_str(cmd, " ");
}


/* helper method for adding ORIENTATION to cmd string */
void append_orientation(char* cmd, ORIENTATION orientation) 
{
	if(orientation == NORTH) {
		append_str(cmd, "north");
	} else if(orientation == SOUTH) {
		append_str(cmd, "south");
	} else if(orientation == EAST) {
		append_str(cmd, "east");
	} else {
		append_str(cmd, "west");
	}
}


void gui_add_powerup(Powerup *  powerup) 
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(powerup->x < MAX_LEVEL_WIDTH*TILE_SIZE && powerup->y < MAX_LEVEL_HEIGHT*TILE_SIZE) {
			char* cmd = create_cmd_string("POWERUP ");
			append_coord(cmd, powerup->x, powerup->y, 1);
			if(powerup->powerup_type == EXTRA_POWER) {
				append_str(cmd, "normal");
			} else if(powerup->powerup_type == EXTRA_BOMB) {
				append_str(cmd, "extra_bomb");
			} else if(powerup->powerup_type == FREEZE_ENEMIES) {
				append_str(cmd, "freeze_enemies");
			}
			enqueue(&cmd_queue, cmd);
		}
	}
}

void gui_add_enemy(Enemy * enemy) 
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(enemy->x < MAX_LEVEL_WIDTH*TILE_SIZE && enemy->y < MAX_LEVEL_HEIGHT*TILE_SIZE) {
			char* cmd = create_cmd_string("NPC ");
			append_coord(cmd, enemy->x, enemy->y, 1);
			if(!enemy->is_boss) {
				append_str(cmd, "critter ");
			} else {
				append_str(cmd, "boss ");
			}
			append_orientation(cmd, enemy->move_direction);
			if(enemy->frozen) {
				append_str(cmd, " frozen");
			}
			enqueue(&cmd_queue, cmd);
		}
	}
}


void gui_add_player(Player * player)
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(player->x < MAX_LEVEL_WIDTH*TILE_SIZE && player->y < MAX_LEVEL_HEIGHT*TILE_SIZE) {
			char* cmd = create_cmd_string("PLAYER ");
			append_coord(cmd, player->x, player->y, 1);
			append_orientation(cmd, player->orientation);
			enqueue(&cmd_queue, cmd);
		}
	}
}

void gui_add_obstacle(Obstacle * obstacle) 
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(obstacle->x < MAX_LEVEL_WIDTH*TILE_SIZE && obstacle->y < MAX_LEVEL_HEIGHT*TILE_SIZE) {
			char* cmd = create_cmd_string("OBSTACLE ");
			append_coord(cmd, obstacle->x, obstacle->y, 0);
			enqueue(&cmd_queue, cmd);
		}
	}
}

void gui_add_bomb(Bomb * bomb)
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(bomb->x < MAX_LEVEL_WIDTH*TILE_SIZE && bomb->y < MAX_LEVEL_HEIGHT*TILE_SIZE) {
			char* cmd = create_cmd_string("BOMB ");
			append_coord(cmd, bomb->x, bomb->y, 0);
			enqueue(&cmd_queue, cmd);
		}
	}
}

/* tekent 'kruis' van bepaalde grootte, gui checkt voor randen */
void gui_add_explosion_tile(int x, int y, int intensity)
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(x < MAX_LEVEL_WIDTH*TILE_SIZE && y < MAX_LEVEL_HEIGHT*TILE_SIZE) {
			char power_str[8];
			char* cmd = create_cmd_string("EXPLOSION ");
			append_coord(cmd, x, y, 1);
			sprintf(power_str, "%d", intensity);
			append_str(cmd, power_str);
			enqueue(&cmd_queue, cmd);
		}
	}
}

void gui_set_enemies_left(int enemies)
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		char enemies_str[8];
		char* cmd = create_cmd_string("ENEMIES_LEFT ");
		sprintf(enemies_str, "%d", enemies);
		append_str(cmd, enemies_str);
		enqueue(&cmd_queue, cmd);
	}
}

void gui_set_bombs_left(int nr_of_bombs)
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		char lives_str[8];
		char* cmd = create_cmd_string("LIVES ");
		sprintf(lives_str, 8 * sizeof(char), "%d", nr_of_bombs);
		append_str(cmd, lives_str);
		enqueue(&cmd_queue, cmd);
	}
}

/*
Commands:

POWERUP [X coord] [Y coord] [normal|surprise]
NPC [X coord] [Y coord] [north|south|east|west] [frozen(opt.)]
PLAYER [X coord] [Y coord] [north|south|east|west] [dizzy(opt.)]
OBSTACLE [X coord] [Y coord]
BOMB [X coord] [Y coord]
EXPLOSION [X coord] [Y coord] [power]
ENEMIES_LEFT [enemies left]
LIVES [lives]

*/
void gui_draw_buffer()
{
	char** delete_buffer;
	int queue_size;
	int index;
	jobjectArray command_array;
	char* cmd;
	jstring tmp;
	jclass stringClass;

	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(drawBuffer != NULL) {
			delete_buffer = (char**) malloc(cmd_queue.size * sizeof(char*));

			stringClass = (*env)->FindClass(env, "java/lang/String");
			command_array = (*env)->NewObjectArray(env, cmd_queue.size, stringClass, (*env)->NewStringUTF(env, ""));
			
			queue_size = cmd_queue.size;

			for(index = 0; index < queue_size; index++) {
				cmd = dequeue(&cmd_queue);
				tmp = (*env)->NewStringUTF(env, cmd);
				(*env)->SetObjectArrayElement(env, command_array, index, tmp);
				(*env)->DeleteLocalRef(env, tmp);
				delete_buffer[index] = cmd;
			}	

 			(*env)->CallStaticVoidMethod(env, clsMain, drawBuffer, command_array);

			(*env)->DeleteLocalRef(env, command_array);
			(*env)->DeleteLocalRef(env, stringClass);

			for(index = 0; index < queue_size; index++) {
				destroy_cmd_string(delete_buffer[index]);
			}
			free(delete_buffer);

			if(cmd_queue.size > 0) {
				printf("Warning: command queue not empty!!! Size: %d \n ", cmd_queue.size);
			}

		} else {
			printf("gui error: JNI method call failed!!.\n");
			return;
		}
	}

}

void gui_start_timer()
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(startTimer != NULL) {
			(*env)->CallStaticVoidMethod(env, clsMain, startTimer);
		} else {
			printf("gui error: JNI method call failed!!.\n");
			return;
		}
	}
}

int gui_get_timer_score()
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(getTimerScore != NULL) {
			return (*env)->CallStaticIntMethod(env, clsMain, getTimerScore);
		} else {
			printf("gui error: JNI method call failed!!.\n");
		}
	}
	return -1;
}

void gui_set_game_over()
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(setGameOver != NULL) {
			(*env)->CallStaticVoidMethod(env, clsMain, setGameOver);
		} else {
			printf("gui error: JNI method call failed!!.\n");
			return;
		}
	}
}

void gui_set_finished_level(int score)
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(setFinishedLevel != NULL) {
			(*env)->CallStaticVoidMethod(env, clsMain, setFinishedLevel, score);
		} else {
			printf("gui error: JNI method call failed!!.\n");
			return;
		}
	}
}

int gui_is_movement_key_down(void)
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(isMovementKeyDown != NULL) {
			return (*env)->CallStaticIntMethod(env, clsMain, isMovementKeyDown);
		} else {
			printf("gui error: JNI method call failed!!.\n");
		}
	}
	return -1;
}

int gui_is_space_pressed(void)
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(wasSpacePressed != NULL) {
			return (*env)->CallStaticIntMethod(env, clsMain, wasSpacePressed);
		} else {
			printf("gui error: JNI method call failed!!.\n");
		}
	}
	return 0;
}

int gui_is_terminated(void)
{
	if(!ready) {
		printf("gui error: Please call the initialize method first!");
	} else {
		if(isTerminated != NULL) {
			return (*env)->CallStaticIntMethod(env, clsMain, isTerminated);
		} else {
			printf("gui error: JNI method call failed!!.\n");
		}
	}

	return 0;
}
