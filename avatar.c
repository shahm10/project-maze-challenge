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
#include "maze.h"

#include <pthread.h>
#include <threads.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;


XYPos prev;
thread_local int last_dir = -1;   // need to make this thread-specific
bool first_turn = true;


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
        
    printf("Initializing maze\n");
    maze_t *maze = maze_new(MazeWidth, MazeHeight);
    last_dir = -1;
    AM_Message turn_msg = getMessage(comm_sock);
    // int i = 0;
    while (ntohl(turn_msg.type) == AM_AVATAR_TURN) {
        pthread_mutex_lock(&mutex1);
        // printf("iteration %d \n", i);
        // i++;
        avatar_move(maze, turn_msg, AvatarID, nAvatars, comm_sock);
        turn_msg = getMessage(comm_sock);
        pthread_mutex_unlock(&mutex1);
    }
    if (ntohl(turn_msg.type) == AM_AVATAR_OUT_OF_TURN){
        printf("Out of turn \n");
    } else if (ntohl(turn_msg.type) == AM_UNKNOWN_MSG_TYPE){
        printf("unknown msg \n");
    } else if (ntohl(turn_msg.type) == AM_AVATAR_TURN){
        printf("turn \n");
    } else if (ntohl(turn_msg.type) == AM_UNEXPECTED_MSG_TYPE){
        printf("unexpected msg \n");
    } else if (ntohl(turn_msg.type) == AM_TOO_MANY_MOVES) {
        printf("too many moves \n");
    }

    if (ntohl(turn_msg.type) == AM_MAZE_SOLVED) {
        printf("Avatars found each other!\n");
    } else {
        printf("maze not solved \n");
    }

    maze_delete(maze);
    
    close(comm_sock);
    printf("closing socket ... \n");
    return 0;
}

void avatar_move(maze_t *maze, AM_Message msg, int AvatarID, int nAvatars, int comm_sock) 
{
    //   pthread_mutex_lock(&mutex1);
    int TurnID = ntohl(msg.avatar_turn.TurnId);
    if (TurnID % nAvatars == AvatarID) {
        if (last_dir == -1) {
            first_turn = false;
            prev = msg.avatar_turn.Pos[AvatarID];
            if (sendMsg(comm_sock, AvatarID, 3)) {
                printf("%d Initial move sent successfully\n", AvatarID);
                last_dir = 3;
            } else {
                fprintf(stderr, "Initial move did not send successfully\n");
            }
        } else {
            XYPos curr = msg.avatar_turn.Pos[AvatarID];
            if (comparePos(prev, curr)) {
                updateWall(maze, curr);
                rotateDirection();
            } else {
                prev = curr;
                if (sendMsg(comm_sock, AvatarID, last_dir)) {
                    printf("%dMove sent successfully with direction %d\n", last_dir, AvatarID);
                } else {
                    fprintf(stderr, "Move with direction %d not sent successfully\n", last_dir);
                }
            }
            printf("x: %d y: %d\n", ntohl(curr.x), ntohl(curr.y));
        }

    }
//   pthread_mutex_unlock(&mutex1);
}

void rotateDirection(void)
{
    // FIRST MOVE
    if (last_dir == 3){
        last_dir = 1; // Rotate to north
    } else if (last_dir == 0){
        last_dir = 2; // Rotate to south
    } else if (last_dir == 1) {
        last_dir = 0; // Rotate to west
    } else if (last_dir == 2) {
        last_dir = 3; // Rotate to east
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

void updateWall(maze_t *maze, XYPos curr) {
    int x = ntohl(curr.x) * 2 + 1;
    int y = ntohl(curr.y) * 2 + 1;
    if (last_dir == 3) {
        setObj(maze, x+1, y, 3);
    } else if (last_dir == 1) {
        setObj(maze, x, y-1, 2);
    } else if (last_dir == 2) {
        setObj(maze, x, y+1, 2);
    } else if (last_dir == 0) {
        setObj(maze, x-1, y, 3);
    }
}


bool sendMsg(int comm_sock, int avatarID, int direction) 
{
    // 4. write to socket
    AM_Message msg;
    msg.type = htonl(AM_AVATAR_MOVE);
    msg.avatar_move.AvatarId = htonl(avatarID);
    msg.avatar_move.Direction = htonl(direction); 
    printf("avatarId:%d, direction : %d\n ", avatarID, direction);

    //try to send the move message to the server
    printf ("Try to send the AM_AVATAR_MOVE message to the server... \n");
    send(comm_sock, &msg, sizeof(AM_Message), 0);
    if (send(comm_sock, &msg, sizeof(AM_Message), 0) == -1) {
        fprintf (stderr, "Error: can't send message\n");
        exit (5);
    } 
    return true;
}

AM_Message getMessage(int comm_sock) {

    AM_Message servermsg;
    int receive = 0;
    receive = recv(comm_sock, &servermsg, sizeof(AM_Message), 0);
    if (receive < 0) {
        fprintf (stderr, "Error: cannot receive message\n");
        exit (6);               // not sure that this should be exit/return FIX
    }

    if (receive == 0) {
        fprintf (stderr, "Error: connection closed\n");
        exit (7);               // not sure that this should be exit/return FIX
    }
    printf ("Received message from server \n");

    // Receive AM_MAZE_SOLVED

    return servermsg;

}