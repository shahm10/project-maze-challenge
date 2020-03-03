/*
 * Client program
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>	      // read, write, close
#include <string.h>	      // memcpy, memset
#include <netdb.h>	      // socket-related structures
#include <sys/socket.h>

#include "amazing.h"
#include "counters.h"
#include "maze.h"
#include "object.h"


int avatar_new(int AvatarID, int nAvatars, int Difficulty, char* hostname, int MazePort, int MazeHeight, int MazeWidth, char* logname) ;
// bool avatar_move(int AvatarID, int comm_sock, int MazeWidth, int MazeHeight, int visited[MazeHeight][MazeWidth], int direction, XYPos currPos, XYPos destination);

void avatar_move(maze_t *maze, AM_Message msg, int AvatarID, int nAvatars, int comm_sock, FILE *fp);


void rotateDirection(void);

bool comparePos(XYPos posA, XYPos posB);

void updateWall(maze_t *maze, XYPos curr);
// XYPos getNextPos(XYPos curr, int direction);
bool sendMsg(int comm_sock, int avatarID, int direction);

AM_Message getMessage(int comm_sock);