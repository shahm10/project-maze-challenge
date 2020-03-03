#include <stdio.h>
#include <stdlib.h>

typedef struct object object_t;

object_t *object_new(void);

int getType(object_t *obj);

void setTile(object_t *obj);

void setHWall(object_t *obj);

void setVWal(object_t *obj);

void setCorner(object_t *obj);

void object_delete(object_t *obj);