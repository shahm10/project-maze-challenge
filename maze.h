/* 
 * maze.h - header file for 'maze.c' module
 * 
 * A maze contains a 2D array of objects that will be shared and updated
 * by all the avatars. The avatar will make use of this maze to update walls
 * and check for any existing walls. See object.h for details on each
 * object in the array.
 *
 * CS50 Winter 2020
 */

/*
Object types:
1 is a blank tile
2 is a horizontal wall
3 is a vertical wall
4 is a corner
5 is an avatar
*/

#include <stdio.h>
#include <stdlib.h>
#include "amazing.h"
#include "object.h"

typedef struct maze maze_t;

/**************** maze_new ****************/
/* Create a new maze and initialize array
 *
 * Caller provides:
 *   width and height of maze
 * We return:
 *   the maze after initialization
 * We do:
 *   Allocate memory for the maze
 *   Initialize 2D Array 
 *   Initialize outer edges of the array
 */
maze_t *maze_new(const int width, const int height);

/**************** getTile ****************/
/* Returns the integer type of a desired tile
 *
 * Caller provides:
 *   maze and X Y position of desired tile
 * We return:
 *   integer type of the desired tile
 */
int getTile(maze_t *mz, int x, int y);

/**************** setObj ****************/
/* Sets the integer type of a desired tile
 *
 * Caller provides:
 *   maze, X Y position of desired tile, and desired type
 * We do:
 *   Set tile in the 2D array to desired type
 */
void setObj(maze_t *mz, int x, int y, int type);

/* Helper functions to get maze's width and height */
int getMazeWidth(maze_t *mz);

int getMazeHeight(maze_t *mz);

/* Helper functions to delete and print the maze */
void maze_delete(maze_t *mz);

void maze_print(maze_t* mz);