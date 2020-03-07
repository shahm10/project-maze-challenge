#include <stdio.h>
#include <stdlib.h>
#include "amazing.h"
#include "object.h"

typedef struct maze maze_t;

maze_t *maze_new(const int width, const int height);

int getTile(maze_t *mz, int x, int y);

void setObj(maze_t *mz, int x, int y, int type);

int getMazeWidth(maze_t *mz);

int getMazeHeight(maze_t *mz);

void maze_delete(maze_t *mz);

void maze_print(maze_t* mz);