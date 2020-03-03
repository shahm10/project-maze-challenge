#include <stdio.h>
#include <stdlib.h>

/*
Types:
1 is a blank tile
2 is a horizontal wall
3 is a vertical wall
4 is a corner
*/

typedef struct object {
    int type;
} object_t;

object_t *object_new(void) {
    object_t *obj = malloc(sizeof(object_t));
    obj->type = 0;
    return obj;
}

int getType(object_t *obj) {
    if (obj->type == 0) {
        return 0;
    } else {
        return obj->type;
    }
}

void setTile(object_t *obj) {
    obj->type = 1;
}

void setHWall(object_t *obj) {
    obj->type = 2;
}

void setVWall(object_t *obj) {
    obj->type = 3;
}

void setCorner(object_t *obj) {
    obj->type = 4;
}

void object_delete(object_t *obj) {
    free(obj);
}

