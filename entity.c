
#include "blast_it.h"
#include "entity.h"

#define DIV(X)  ((X) / TILE_SIZE)
#define MOD(X)  ((X) % TILE_SIZE)

void put_bomb(Entity** entities, int x, int y, int ticks_left) {
    entities[x][y].type = BOMB;
    entities[x][y].bomb.x = x * TILE_SIZE;
    entities[x][y].bomb.y = y * TILE_SIZE;
    entities[x][y].bomb.ticks_left = ticks_left;
}

void put_explosion(Entity** entities, int x, int y, int spread[4], int power, int ticks_left) {
    entities[x][y].type = EXPLOSION;
    entities[x][y].explosion.x = x * TILE_SIZE;
    entities[x][y].explosion.y = y * TILE_SIZE;
    entities[x][y].explosion.spread[0] = spread[0];
    entities[x][y].explosion.spread[1] = spread[1];
    entities[x][y].explosion.spread[2] = spread[2];
    entities[x][y].explosion.spread[3] = spread[3];
    entities[x][y].explosion.power = power;
    entities[x][y].explosion.ticks_left = ticks_left;
}

void put_powerup(Entity** entities, int x, int y, POWERUP_TYPE type) {
    entities[x][y].type = POWERUP;
    entities[x][y].powerup.x = x * TILE_SIZE;
    entities[x][y].powerup.y = y * TILE_SIZE;
    entities[x][y].powerup.powerup_type = type;
}

void put_obstacle(Entity** entities, int x, int y, int is_destructable) {
    entities[x][y].type = OBSTACLE;
    entities[x][y].obstacle.x = x * TILE_SIZE;
    entities[x][y].obstacle.y = y * TILE_SIZE;
    entities[x][y].obstacle.is_destructable = is_destructable;
}

void put_empty_space(Entity** entities, int x, int y) {
    entities[x][y].type = EMPTY_SPACE;
    entities[x][y].empty_space.x = x * TILE_SIZE;
    entities[x][y].empty_space.y = y * TILE_SIZE;
}

int is_abs_walkable(Entity** entities, int x, int y) {
    int walkable = is_walkable(entities[DIV(x)][DIV(y)]);
    if(MOD(x) != 0 && MOD(y) != 0)
        walkable = walkable && is_walkable(entities[DIV(x) + 1][DIV(y) + 1]);
    if(MOD(x) != 0)
        walkable = walkable && is_walkable(entities[DIV(x) + 1][DIV(y)]);
    if(MOD(y) != 0)
        walkable = walkable && is_walkable(entities[DIV(x)][DIV(y) + 1]);
    return walkable;
}

int is_walkable(Entity entity) {
    return (
        (entity.type == EMPTY_SPACE) ||
        (entity.type == BOMB && entity.bomb.ticks_left < 0) ||
        (entity.type == EXPLOSION) ||
        (entity.type == POWERUP)
    );
}
