/* 
 * avatar.h - header file for 'avatar.c' module
 * 
 * An 'avatar' contains all the code for initializing an individual
 * avatar and reading server messsages. It has an algorithm for navigating the maze
 * and any helper functions to help it achieve this goal.
 *
 * CS50 Winter 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>	      // read, write, close
#include <string.h>	      // memcpy, memset
#include <netdb.h>	      // socket-related structures
#include <sys/socket.h>

#include "amazing.h"
#include "maze.h"
#include "object.h"

/**************** avatar_new ****************/
/* Create a new avatar and initialize its thread
 *
 * Caller provides:
 *   valid pointer to maze, the avatar's ID, number of total avatars,
 *   difficulty of the maze, server hostname, the port to connect to,
 *   height and width of the maze, and a target log file
 * We return:
 *   0 if initialized correctly
 * We do:
 *   Connect a socket to the server
 *   Initialize all variables for the avatar
 *   Call avatar_move
 */
int avatar_new(maze_t *maze, int AvatarID, int nAvatars, int Difficulty, char* hostname, int MazePort, int MazeHeight, int MazeWidth, char* logname);

/**************** avatar_move ****************/
/* Contains algorithm for navigating the maze
 *
 * Caller provides:
 *   valid pointer to maze, server message, this avatar's ID,
 *   number of total avatars, communication socket, and log file
 *
 * We do:
 *   Parse information received from the server
 *   Choose the avatar's next move and send to server
 */
void avatar_move(maze_t *maze, AM_Message msg, int AvatarID, int nAvatars, int comm_sock, FILE *fp);

/**************** rotateRight ****************/
/* Rotates avatar's forward and righthand directions 90
 * degrees clockwise
*/
void rotateRight(void);

/**************** rotateLeft ****************/
/* Rotates avatar's forward and righthand directions 90
 * degrees counterclockwise
*/
void rotateLeft(void);

/**************** updateWall ****************/
/* Adds a wall to map based on current position and
 * last-attempted direction
 *
 * Caller provides:
 *   valid pointer to maze, current position
 */
void updateWall(maze_t *maze, XYPos curr);

/**************** updateAvatar **************/
/*
 * Adds an avatar to maze map based on current position
 * 
 * Caller provides:
 *   valid pointer to maze, current position
 */
void updateAvatar(maze_t *maze, XYPos curr);

/*************** removeAvatar **************/
/*
 * Removes avatar from maze map based on previous position
 * 
 * Caller provides:
 *  valid pointer to maze, previous position 
 */
void removeAvatar(maze_t *maze, XYPos prev);

/**************** checkWall ****************/
/* Checks if a wall exists given current position and
 * desired direction
 * last-attempted direction
 *
 * Caller provides:
 *   valid pointer to maze, current position, desired direction
 *
 * We return:
 *   true if a wall exists and false otherwise
 */
bool checkWall(maze_t* maze, XYPos curr, int direction);

/**************** sendMsg ****************/
/* Sends AM_AVATAR_MOVE message to server
 *
 * Caller provides:
 *   communication socket, this avatar's ID, and desired direction
 *
 * We return:
 *   true if successful
 */
bool sendMsg(int comm_sock, int avatarID, int direction);

/**************** getMessage ****************/
/* Receives message to server
 *
 * Caller provides:
 *   communication socket
 *
 * We return:
 *   message that server provides
 */
AM_Message getMessage(int comm_sock);