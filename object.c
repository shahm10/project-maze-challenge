/* 
 * object.c module
 * 
 * An object has a type (see below for details) and is either a blank
 * tile, horizontal wall, vertical wall, or corner. The objects will be
 * placed in a 2D array to represent the maze, which will be shared by 
 * all avatars.
 *
 * CS50 Winter 2020
 */

/*
Types:
1 is a blank tile
2 is a horizontal wall
3 is a vertical wall
4 is a corner
5 is an avatar
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct object {
    int type;
} object_t;

/* Initialize a new object and allocate memory */
object_t *object_new(void) {
    // Allocate memory and initialize type to 0
    object_t *obj = malloc(sizeof(object_t));
    obj->type = 0;
    return obj;
}

/* Returns the type of a desired object */
int getType(object_t *obj) {
    if (obj->type == 0) {
        return 0;
    } else {
        return obj->type;
    }
}

// Sets the object's type to a blank tile (1)
void setTile(object_t *obj) {
    obj->type = 1;
}

// Sets the object's type to a horizontal wall (2)
void setHWall(object_t *obj) {
    obj->type = 2;
}

// Sets the object's type to a vertical wall (3)
void setVWall(object_t *obj) {
    obj->type = 3;
}

// Sets the object's type to a corner (4)
void setCorner(object_t *obj) {
    obj->type = 4;
}

// Sets the object's type to a present avatar (5)
void setAvatar(object_t *obj) {
    obj->type = 5;
}

// Deletes the object and frees memory
void object_delete(object_t *obj) {
    free(obj);
}

