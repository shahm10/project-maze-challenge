/* 
 * maze.c module
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

// See object.h for object implementation
typedef struct maze {
    object_t ***grid; // 2D array of Objects
    int width;
    int height;
} maze_t;

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
maze_t *maze_new(const int width, const int height)
{
    // Allocate memory for maze struct
    maze_t *mz = malloc(sizeof(maze_t));
    
    /* Initialize 2D array to be 2x + 1 width and height of 
     * the actual maze to make space for walls and corners
     */
    mz->width = (width * 2) + 1;
    mz->height = (height *2) + 1;

    // Allocate memory for 2D Array of Objects
    mz->grid = malloc(mz->height * sizeof(*mz->grid));
    for (int i = 0; i < mz->height; i++) {
        mz->grid[i] = malloc(mz->width*sizeof(object_t**));
    }
    
    // Initialize all objects in array to blank tiles
    for (int i = 0; i < mz->width; i++) {
        for (int j = 0; j < mz->height; j++) {
            mz->grid[j][i] = object_new();
            setTile(mz->grid[j][i]);
        }
    }

    // Set top row of corners and walls
    for (int i = 0; i < mz->width; i++) {
        if (i % 2 == 0) {
            setCorner(mz->grid[0][i]);
        } else {
            setHWall(mz->grid[0][i]);
        }
    }

    // Set bottom row of corners and walls
    for (int i = 0; i < mz->width; i++) {
        if (i % 2 == 0) {
            setCorner(mz->grid[mz->height-1][i]);
        } else {
            setHWall(mz->grid[mz->height-1][i]);
        }
    }

    // Set left column of corners and walls
    for (int i = 1; i < mz->height-1; i++) {
        if (i % 2 == 0) {
            setCorner(mz->grid[i][0]);
        } else {
            setVWall(mz->grid[i][0]);
        }
    }

    // Set right column of corners and walls
    for (int i = 1; i < mz->height-1; i++) {
        if (i % 2 == 0) {
            setCorner(mz->grid[i][mz->width-1]);
        } else {
            setVWall(mz->grid[i][mz->width-1]);
        }
    }

    // Set vertices in center to corners
    for (int i = 1; i < mz->height; i++) {
        for (int j = 1; j < mz->width; j++) {
            if (i % 2 == 0 && j % 2 == 0) {
                setCorner(mz->grid[i][j]);
            }
        }
    }
    return mz;
}

/**************** getTile ****************/
/* Returns the integer type of a desired tile
 *
 * Caller provides:
 *   maze and X Y position of desired tile
 * We return:
 *   integer type of the desired tile
 */
int getTile(maze_t *mz, int x, int y)
{
    return getType(mz->grid[x][y]);
}

/**************** setObj ****************/
/* Sets the integer type of a desired tile
 *
 * Caller provides:
 *   maze, X Y position of desired tile, and desired type
 * We do:
 *   Set tile in the 2D array to desired type
 */
void setObj(maze_t *mz, int x, int y, int type)
{

    if (type == 1) { // Set a blank tile
        setTile(mz->grid[x][y]);
    } else if (type == 2) { // Set a horizontal wall
        setHWall(mz->grid[x][y]);
    } else if (type == 3) { // Set a vertical wall
        setVWall(mz->grid[x][y]);
    } else if (type == 4) { // Set a corner
        setCorner(mz->grid[x][y]);
    } else if (type == 5) { // Set an avatar's presence
        setAvatar(mz->grid[x][y]);
    }

}

// Helper function to get maze's width
int getMazeWidth(maze_t *mz) {
    return (mz->width-1)/2;
}

// Helper function to get maze's height
int getMazeHeight(maze_t *mz) {
    return (mz->height-1)/2;
}

void maze_delete(maze_t *mz)
{
    // Free each object in the 2D Array
    for (int i = 0; i < mz->width; i++) {
        for (int j = 0; j < mz->height; j++) {
            object_delete(mz->grid[j][i]);
        }
    } 

    // Free the 2D array
    for (int i = 0; i < mz->height; i++) {
        free(mz->grid[i]);
    }

    // Free the maze struct itself
    free(mz);
    
}

// Maze printing function
void maze_print(maze_t* mz)
{   
    // Print column numbers above maze
    printf(" ");
    for (int i = 0; i < mz->width; i++) {
        if (i % 2 != 0) {
            printf("%d", (i-1)/2);
        } else {
            printf(" ");
        }
    }
    printf("\n");

    // Iterate through entire 2D array
    for (int i = 0; i < mz->height; i++) {
        
        // Prints row numbers to the left of maze's first column
        if (i % 2 != 0) {
            printf("%d", (i-1)/2);
        } else {
            printf(" ");
        }

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
    printf("maze width: %d, height: %d\n", mz->width, mz->height);
}