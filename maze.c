#include <stdio.h>
#include <stdlib.h>
#include "amazing.h"
#include "object.h"

/*
Types:
1 is a blank tile
2 is a horizontal wall
3 is a vertical wall
4 is a corner
*/

// Object definition
typedef struct maze {
    object_t ***grid;
    int width;
    int height;
} maze_t;

// Function Prototypes
maze_t *maze_new(const int width, const int height);
int getTile(maze_t *mz, int x, int y);
void setObj(maze_t *mz, int x, int y, int type);
void maze_delete(maze_t *mz);
void maze_print(maze_t* mz);

maze_t *maze_new(const int width, const int height)
{

    maze_t *mz = malloc(sizeof(maze_t));
    mz->width = (width * 2) + 1;
    mz->height = (height *2) + 1;

    mz->grid = malloc(mz->height * sizeof(*mz->grid));
    for (int i = 0; i < mz->height; i++) {
        // printf("Coordinate: %d\n", i);
        mz->grid[i] = malloc(mz->width*sizeof(object_t**));
    }
    
    for (int i = 0; i < mz->width; i++) {
        for (int j = 0; j < mz->height; j++) {
            mz->grid[j][i] = object_new();
            setTile(mz->grid[j][i]);
        }
    }

    // Set top row
    for (int i = 0; i < mz->width; i++) {
        if (i % 2 == 0) {
            setCorner(mz->grid[0][i]);
        } else {
            setHWall(mz->grid[0][i]);
        }
    }

    // Set bottom row
    for (int i = 0; i < mz->width; i++) {
        if (i % 2 == 0) {
            setCorner(mz->grid[mz->height-1][i]);
        } else {
            setHWall(mz->grid[mz->height-1][i]);
        }
    }

    // Set left column
    for (int i = 1; i < mz->height-1; i++) {
        if (i % 2 == 0) {
            setCorner(mz->grid[i][0]);
        } else {
            setVWall(mz->grid[i][0]);
        }
    }

    // Set right column
    for (int i = 1; i < mz->height-1; i++) {
        if (i % 2 == 0) {
            setCorner(mz->grid[i][mz->width-1]);
        } else {
            setVWall(mz->grid[i][mz->width-1]);
        }
    }

    // Set the center corners
    for (int i = 1; i < mz->height; i++) {
        for (int j = 1; j < mz->width; j++) {
            if (i % 2 == 0 && j % 2 == 0) {
                setCorner(mz->grid[i][j]);
            }
        }
    }
    
    return mz;
}

int getTile(maze_t *mz, int x, int y)
{
    return getType(mz->grid[x][y]);
}

void setObj(maze_t *mz, int x, int y, int type)
{

    if (type == 1) {
        setTile(mz->grid[y][x]);
    } else if (type == 2) {
        setHWall(mz->grid[y][x]);
    } else if (type == 3) {
        setVWall(mz->grid[y][x]);
    } else if (type == 4) {
        setCorner(mz->grid[y][x]);
    }

}

void maze_delete(maze_t *mz)
{

    for (int i = 0; i < mz->width; i++) {
        for (int j = 0; j < mz->height; j++) {
            object_delete(mz->grid[j][i]);
        }
    }

    for (int i = 0; i < mz->height; i++) {
        free(mz->grid[i]);
    }

    free(mz);
    
}

// Maze printing function
void maze_print(maze_t* mz)
{
    for (int i = 0; i < mz->height; i++) {
        for (int j = 0; j < mz->width; j++) {
        
            // 1 is a blank tile
            if (getTile(mz, i, j) == 1) {
                printf(" ");
            }

            // 2 is a horizontal wall
            if (getTile(mz, i, j) == 2) {
                printf("-");
            }

            // 3 is a vertical wall
            if (getTile(mz, i, j) == 3) {
                printf("|");
            }
            
            // 4 is a corner
            if (getTile(mz, i, j) == 4) {
                printf("+");
            }
        }
        // Move onto the next row
        printf("\n");
    }
}