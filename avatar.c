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
thread_local XYPos prev;
thread_local bool foundFriend = false;
thread_local int last_dir = -1;   // need to make this thread-specific
thread_local int direction = -1;
thread_local int righthand = -1;
int destination_x;
int destination_y;


// initializes avatar (one of N threads)
int avatar_new(maze_t *maze, int AvatarID, int nAvatars, int Difficulty, char* hostname, int MazePort, int MazeHeight, int MazeWidth, char *logname) 
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
    fprintf (fp, "*******\n");

    // Receive AM_AVATAR_TURN (avatarID, XYPos of all avatars)
        
    printf("Initializing maze\n");
    //maze = maze_new(MazeWidth, MazeHeight);

    destination_x = getMazeWidth(maze)-1;
    destination_y = 0;

    last_dir = -1;
    AM_Message turn_msg = getMessage(comm_sock);
    // int i = 0;
    while ( ntohl(turn_msg.type) == AM_AVATAR_TURN){
        int TurnID = ntohl(turn_msg.avatar_turn.TurnId);   
        if (TurnID == AvatarID) {
            pthread_mutex_lock(&mutex1);
            avatar_move(maze, turn_msg, AvatarID, nAvatars, comm_sock, fp);
            pthread_mutex_unlock(&mutex1);
        // } else {
            // turn_msg = getMessage(comm_sock);
        }
        turn_msg = getMessage(comm_sock);
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

    //maze_delete(maze);
    
    close(comm_sock);
    printf("closing socket ... \n");
    return 0;
}

void avatar_move(maze_t *maze, AM_Message msg, int AvatarID, int nAvatars, int comm_sock, FILE *fp) 
{
    // int TurnID = ntohl(msg.avatar_turn.TurnId);
    fprintf (fp, "AvatarID: %i\n", AvatarID);
    if (last_dir == -1) {
        prev = msg.avatar_turn.Pos[AvatarID];
        int initial_x = ntohl(prev.x);
        int initial_y = ntohl(prev.y);
        if (destination_x == initial_x && destination_y == initial_y) {
            if (sendMsg(comm_sock, AvatarID, 3)) {
                printf("%d Initial move sent successfully\n", AvatarID);
            } else {
                fprintf(stderr, "Initial move did not send successfully\n");
            }
        } else {
            last_dir = 2;
            righthand = 2;
            direction = 3;
            if (sendMsg(comm_sock, AvatarID, 2)) {
                printf("%d Initial move sent successfully\n", AvatarID);
            } else {
                fprintf(stderr, "Initial move did not send successfully\n");
            }
        }
    } else {
        XYPos curr = msg.avatar_turn.Pos[AvatarID];

        int x = ntohl(msg.avatar_turn.Pos[AvatarID].x);
        int y = ntohl(msg.avatar_turn.Pos[AvatarID].y);

        if (destination_x == x && destination_y == y) {
            if (sendMsg(comm_sock, AvatarID, 3)) {
                printf("Move sent successfully with direction 3 by avatar %d\n", AvatarID);
            } else {
                fprintf(stderr, "Move with direction 3 not sent successfully\n");
            }
        } else {
            int px = ntohl(prev.x);
            int py = ntohl(prev.y);

            fprintf(fp, "Current position of Avatar %i: %d, %d\n", AvatarID, x, y);        
            fprintf(fp, "Current position of Avatar %i: %d, %d\n", AvatarID, ntohl(curr.x), ntohl(curr.y));

            // if the avatar has not moved, add a wall and change direction
            if (px == x && py == y) {
                updateWall(maze, curr);
                if (last_dir == righthand) {
                    last_dir = direction;
                    if (sendMsg(comm_sock, AvatarID, direction)) {
                        printf("Move sent successfully with direction %d by avatar %d\n", direction, AvatarID);
                    } else {
                        fprintf(stderr, "Move with direction %d not sent successfully\n", direction);
                    } 
                    printf("x: %d y: %d\n", ntohl(curr.x), ntohl(curr.y));
                    
                } else if (last_dir == direction) {
                    rotateLeft();
                    last_dir = righthand;
                    if (sendMsg(comm_sock, AvatarID, righthand)) {
                        printf("Move sent successfully with direction %d by avatar %d\n", righthand, AvatarID);
                        
                    } else {
                        fprintf(stderr, "Move with direction %d not sent successfully\n", righthand);
                    } 
                }
                printf("x: %d y: %d\n", ntohl(curr.x), ntohl(curr.y));
            } else {
                if (last_dir == righthand) {
                    rotateRight();
                }
                prev = curr;
                if (checkWall(maze, curr, righthand)) {
                    while (checkWall(maze, curr, direction)) {
                        rotateLeft();
                    }
                    last_dir = direction;
                    if (sendMsg(comm_sock, AvatarID, direction)) {
                        printf("Move sent successfully with direction %d by avatar %d\n", direction, AvatarID);
                    } else {
                        fprintf(stderr, "Move with direction %d not sent successfully\n", direction);
                    } 
                } else {
                    last_dir = righthand;
                    if (sendMsg(comm_sock, AvatarID, righthand)) {
                        printf("Move sent successfully with direction %d by avatar %d\n", righthand, AvatarID);
                    } else {
                        fprintf(stderr, "Move with direction %d not sent successfully\n", righthand);
                    }
                }
                printf("x: %d y: %d\n", ntohl(curr.x), ntohl(curr.y));
            } 
        }
    }
    printf("destination x: %d, y: %d\n", destination_x, destination_y);
    //maze_print(maze);
}

void rotateRight(void) {
    if (direction == 3){
        direction = 2; // Rotate to north
    } else if (direction == 0){
        direction = 1; // Rotate to south
    } else if (direction == 1) {
        direction = 3; // Rotate to west
    } else if (direction == 2) {
        direction = 0; // Rotate to east
    } 

    if (righthand == 3){
        righthand = 2; // Rotate to north
    } else if (righthand == 0){
        righthand = 1; // Rotate to south
    } else if (righthand == 1) {
        righthand = 3; // Rotate to west
    } else if (righthand == 2) {
        righthand = 0; // Rotate to east
    } 
}

void rotateLeft(void)
{

    if (direction == 3){
        direction = 1; // Rotate to north
    } else if (direction == 0){
        direction = 2; // Rotate to south
    } else if (direction == 1) {
        direction = 0; // Rotate to west
    } else if (direction == 2) {
        direction = 3; // Rotate to east
    } 

    if (righthand == 3){
        righthand = 1; // Rotate to north
    } else if (righthand == 0){
        righthand = 2; // Rotate to south
    } else if (righthand == 1) {
        righthand = 0; // Rotate to west
    } else if (righthand == 2) {
        righthand = 3; // Rotate to east
    }
}

int rotateDirection(void)
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
    return last_dir;
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
    int col = ntohl(curr.x) * 2 + 1;
    int row = ntohl(curr.y) * 2 + 1;
    if (last_dir == 3) {        
        setObj(maze, row, col+1, 3);
    } else if (last_dir == 1) {
        setObj(maze, row-1, col, 2);
    } else if (last_dir == 2) {
        setObj(maze, row+1, col, 2);
    } else if (last_dir == 0) {
        setObj(maze, row, col-1, 3);
    }
}

bool checkWall(maze_t* maze, XYPos curr, int direction)
{
    int col = ntohl(curr.x) * 2 + 1;
    int row = ntohl(curr.y) * 2 + 1;
    if (direction == 3) {
        if (getTile(maze, row, col+1) == 3) {
            return true;
        }
    } else if (direction == 1) {
        if (getTile(maze, row-1, col) == 2) {
            return true;
        }
    } else if (direction == 2) {
        if (getTile(maze, row+1, col) == 2) {
            return true;
        }
    } else if (direction == 0) {
        if (getTile(maze, row, col-1) == 3) {
            return true;
        }
    }
    return false;
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
    if (send(comm_sock, &msg, sizeof(msg), 0) == -1) {
        fprintf (stderr, "Error: can't send message\n");
        exit (5);
    } 
    return true;
}

AM_Message getMessage(int comm_sock) {

    AM_Message servermsg;
    int receive = 0;
    receive = recv(comm_sock, &servermsg, sizeof(servermsg), 0);
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
    // XYPos curr = (servermsg.avatar_turn.Pos[1]);
    // printf("IN GET_MESSAGE x pos: %d \n", ntohl(curr.x));
    // int x = ntohl(msg.avatar_turn.Pos[AvatarID].x);

    return servermsg;

}