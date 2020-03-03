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
#include "avatar.h"
#include "object.h"


// initializes avatar (one of N threads)
int avatar_new(int AvatarID, int nAvatars, int Difficulty, char* hostname, int MazePort, int MazeHeight, int MazeWidth, char *logname) 
{
    printf("Number of avatars: %d \n", nAvatars);
    // open socket
    int comm_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (comm_sock < 0) {
        perror("opening socket");
        //need to clean everytime it hits an error
        exit(2);
    }

    // 2. Initialize the fields of the server address
    struct sockaddr_in server;  // address of the server
    server.sin_family = AF_INET;
    server.sin_port = htons(MazePort);
    // Look up the hostname specified on command line
    struct hostent *hostp = gethostbyname(hostname); // server hostname
    if (hostp == NULL) {
        fprintf(stderr, "unknown host '%s'\n", hostname);
        exit(3);
    }  
    memcpy(&server.sin_addr, hostp->h_addr_list[0], hostp->h_length);

    // 3. Connect the socket to that server   
    if (connect(comm_sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("connecting stream socket");
        exit(4);
    }
    printf("Avatar socket connected!\n");

    // send AM_AVATAR_READY
    AM_Message ready_msg;
    ready_msg.type = htonl(AM_AVATAR_READY);
    ready_msg.avatar_ready.AvatarId = htonl(AvatarID);

    //try to send the AM_AVATAR_READY message to the server
    printf ("Try to send the AM_AVATAR_READY message to the server... \n");
    send(comm_sock, &ready_msg, sizeof(AM_Message), 0);
    if (send(comm_sock, &ready_msg, sizeof(AM_Message), 0) == -1) {
        close (comm_sock);
        fprintf (stderr, "Error: can't send message\n");
        exit (5);
    }
    printf ("Ready message is sent\n");

    //Append to log file the avatar turn number 
    FILE *fp = fopen (logname, "a");
    if (fp == NULL) {
        fprintf(stderr, "logname file cannot be opened\n");
        exit (2);
    }
    fprintf (fp, "*****\n");

    printf("work\n");

    // Receive AM_AVATAR_TURN (avatarID, XYPos of all avatars)

        
    printf("allocating maze components... \n");
    int **maze;
    visited = malloc(MazeHeight * sizeof(*visited));            //2x mazeheight +1
    for (int i = 0; i<MazeHeight; i++){
        visited[i] = malloc(MazeWidth*sizeof(*visited[i]));
    }


    // not freeing correctly
    for (int i = 0; i<MazeHeight; i++){
        free(visited[i]);
    }
    free(visited);
    
    close(comm_sock);
    printf("closing socket ... \n");
    return 0;
}


int choose_direction(object** maze, int last_direction)
{
    // FIRST MOVE
    if (last_direction == NULL || last_direction == 1){
        return 3; // M_EAST
    } else if (last_direction == 0){
        return 1; // M_NORTH
    } else if (last_direction == 2) {
        return 0; // M_WEST
    } else if (last_direction == 3) {
        return 2; // M_SOUTH
    } else {
        fprintf(stderr, "last direction is an invalid int\n");
        return NULL;
    }
}
// assume right is east

/*
 * Returns false if positions are different
 * returns true if positions are same 
 */
bool comparePos(XYPos posA, XYPos posB) 
{
    if (posA.x == posB.x && posA.y == posB.y) {
        return true;
    }
    return false;
}


bool sendMsg(int comm_sock, int avatarID, int direction) 
{
    // 4. write to socket
    AM_Message msg;
    msg.type = htonl(AM_AVATAR_MOVE);
    msg.avatar_move.AvatarId = htonl(avatarID);
    
    // msg.avatar_move.AvatarId = (avatarID);
    printf("%d, direction : %d\n ", avatarID, direction);
    // msg.avatar_move.Direction = htonl(direction);
    
    msg.avatar_move.Direction = (direction);

    //try to send the move message to the server
    printf ("Try to send the AM_AVATAR_MOVE message to the server... \n");
    send(comm_sock, &msg, sizeof(AM_Message), 0);
    if (send(comm_sock, &msg, sizeof(AM_Message), 0) == -1) {
        fprintf (stderr, "Error: can't send message\n");
        exit (5);
    } 
    return true;
}