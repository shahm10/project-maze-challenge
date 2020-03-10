/* 
 * object.h - header file for 'object.c' module
 * 
 * An object has a type (see below for details) and is either a blank
 * tile, horizontal wall, vertical wall, or corner. The objects will be
 * placed in a 2D array to represent the maze, which will be shared by 
 * all avatars.
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

typedef struct object object_t;

/* Initialize a new object and allocate memory */
object_t *object_new(void);

/* Returns the type of a desired object */
int getType(object_t *obj);

/* Helper functions to set the type of 
 * the object to a tile, wal, or corner.
 */
void setTile(object_t *obj);

void setHWall(object_t *obj);

void setVWall(object_t *obj);

void setCorner(object_t *obj);

void setAvatar(object_t *obj);

/* Function to delete object and free its memory */
void object_delete(object_t *obj);