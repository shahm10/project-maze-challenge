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


int avatar_new(int AvatarID, int nAvatars, int Difficulty, char* hostname, int MazePort, int MazeHeight, int MazeWidth, char* logname) ;
// bool avatar_move(int AvatarID, int comm_sock, int MazeWidth, int MazeHeight, int visited[MazeHeight][MazeWidth], int direction, XYPos currPos, XYPos destination);

bool avatar_move(int AvatarID, int nAvatars, int comm_sock, int MazeWidth, int MazeHeight, int** visited, int direction, XYPos destination);

bool avatar_move(int AvatarID, int comm_sock, int MazeWidth, int MazeHeight, int** visited, int direction, XYPos destination);
XYPos convertXYPos(XYPos pos);

bool comparePos(XYPos posA, XYPos posB) ;
XYPos getNextPos(XYPos curr, int direction);
bool sendMsg(int comm_sock, int avatarID, int direction);