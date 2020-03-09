/* 
 * avatar.c module
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
#include "avatar.h"
#include "object.h"
#include "maze.h"

#include <pthread.h>
#include <threads.h>

/*** Global variables ****/

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

thread_local XYPos prev; // tracks avatar's last position

thread_local int last_dir = -1;  // tracks avatar's last direction

/*  direction and righthand
 *  direction tracks avatar's forward direction
 *  righthand tracks avatar's righthand direction
 *  righthand is always 90 degrees clockwise of direction
 */
thread_local int direction = -1;
thread_local int righthand = -1; 

/* 
 * Once avatar reaches destination, stop moving
 */
int destination_x; 
int destination_y;


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

    // Set destination to top-right corner of maze
    destination_x = getMazeWidth(maze)-1;
    destination_y = 0;
    last_dir = -1;

    AM_Message turn_msg = getMessage(comm_sock);

    // While server keeps sending us turn message
    while ( ntohl(turn_msg.type) == AM_AVATAR_TURN){
        // Only tell this avatar to move when it is its turn
        int TurnID = ntohl(turn_msg.avatar_turn.TurnId);   
        if (TurnID == AvatarID) {
            // Lock functions to this avatar and call move function
            pthread_mutex_lock(&mutex1);
            avatar_move(maze, turn_msg, AvatarID, nAvatars, comm_sock, fp);
            pthread_mutex_unlock(&mutex1);
        }
        turn_msg = getMessage(comm_sock);
    }

    // Handle all error messages from server
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

    // Prints when server returns maze solved message
    if (ntohl(turn_msg.type) == AM_MAZE_SOLVED) {

        printf("Avatars found each other!\n");
        fprintf(fp, "Maze is solved!\n");

    } else {
        printf("maze not solved \n");
    }
    
    // Close socket
    close(comm_sock);
    free(fp);
    printf("closing socket ... \n");
    return 0;
}

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
void avatar_move(maze_t *maze, AM_Message msg, int AvatarID, int nAvatars, int comm_sock, FILE *fp) 
{
    fprintf(fp, "AvatarID: %i\n", AvatarID);
    for (int i = 0; i < nAvatars; i++){
        XYPos location = msg.avatar_turn.Pos[i];
        updateAvatar(maze, location);
    }

    // If this iteration is the avatar's first turn
    if (last_dir == -1) {

        //Track X and Y for avatar's previous position
        prev = msg.avatar_turn.Pos[AvatarID];
        int initial_x = ntohl(prev.x);
        int initial_y = ntohl(prev.y);
        
        // Check whether avatar was placed at destination
        if (destination_x == initial_x && destination_y == initial_y) {
            // Send "east" repeatedly so that avatar does not move
            if (sendMsg(comm_sock, AvatarID, 3)) {
                printf("%d Initial move sent successfully\n", AvatarID);
            } else {
                fprintf(stderr, "Initial move did not send successfully\n");
            }

        // If avatar was not placed at destination
        } else {
            // Avatar's initial move will be south
            last_dir = 2;
            righthand = 2;
            direction = 3;
            if (sendMsg(comm_sock, AvatarID, 2)) {
                printf("%d Initial move sent successfully\n", AvatarID);
            } else {
                fprintf(stderr, "Initial move did not send successfully\n");
            }
        }

    // If this iteration is not the avatar's first turn
    } else {

        // Track X and Y for avatar's current position
        XYPos curr = msg.avatar_turn.Pos[AvatarID];
        int x = ntohl(msg.avatar_turn.Pos[AvatarID].x);
        int y = ntohl(msg.avatar_turn.Pos[AvatarID].y);

        // Check whether avatar has reached destination
        if (destination_x == x && destination_y == y) {
            // Send "east" repeatedly so that avatar does not move
            if (sendMsg(comm_sock, AvatarID, 3)) {
                // printf("Move sent successfully with direction 3 by avatar %d\n", AvatarID);
            } else {
                fprintf(stderr, "Move with direction 3 not sent successfully\n");
            }
        } else {

            // Previous position's X and Y
            int px = ntohl(prev.x);
            int py = ntohl(prev.y);

            fprintf(fp, "Current position of Avatar %i: %d, %d\n\n", AvatarID, x, y);

            /* Compare previous and current positions
             * if the avatar has not moved, add a wall and change direction
            */
            if (px == x && py == y) {
                // Add a wall to global map based on last direction
                updateWall(maze, curr);

                // If avatar last tried moving right, try moving forward
                if (last_dir == righthand) {
                    last_dir = direction;
                    if (sendMsg(comm_sock, AvatarID, direction)) {
                        printf("Move sent successfully with direction %d by avatar %d\n", direction, AvatarID);
                    } else {
                        fprintf(stderr, "Move with direction %d not sent successfully\n", direction);
                    } 

                /* 
                 * If avatar has tried moving forward and right, rotate directions
                 * 90 degrees counter-clockwise and try moving right again   
                */ 
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

            // If avatar successfully moved on last move
            } else {
                /* If avatar's successfully moved right, make this
                 * the avatar's new righthand direction
                 */ 
                if (last_dir == righthand) {
                    rotateRight();
                }
                removeAvatar(maze, prev);

                // Update previous position and set equal to current
                prev = curr;

                // If avatar can see that a wall is to its right
                if (checkWall(maze, curr, righthand)) {
                    // If both forward and right have walls, rotate 90 degrees
                    while (checkWall(maze, curr, direction)) {
                        rotateLeft();
                    }
                    // If there is a wall on the right, try moving forward
                    last_dir = direction;
                    if (sendMsg(comm_sock, AvatarID, direction)) {
                        // printf("Move sent successfully with direction %d by avatar %d\n", direction, AvatarID);
                    } else {
                        fprintf(stderr, "Move with direction %d not sent successfully\n", direction);
                    } 

                // If avatar cannot see a wall to the right
                } else {
                    // Attempt to move right
                    last_dir = righthand;
                    if (sendMsg(comm_sock, AvatarID, righthand)) {
                        // printf("Move sent successfully with direction %d by avatar %d\n", righthand, AvatarID);
                    } else {
                        fprintf(stderr, "Move with direction %d not sent successfully\n", righthand);
                    }
                }
            } 
        }
    }
    system("clear");
    maze_print(maze);
}

/**************** rotateRight ****************/
/* Rotates avatar's forward and righthand directions 90
 * degrees clockwise
*/
void rotateRight(void) {

    // Rotate forward direction
    if (direction == 3){
        direction = 2; // Rotate from east to south
    } else if (direction == 0){
        direction = 1; // Rotate from west to north
    } else if (direction == 1) {
        direction = 3; // Rotate from north to east
    } else if (direction == 2) {
        direction = 0; // Rotate from south to west
    } 
    
    // Rotate righthand directon
    if (righthand == 3){
        righthand = 2; // Rotate from east to south
    } else if (righthand == 0){
        righthand = 1; // Rotate from west to north
    } else if (righthand == 1) {
        righthand = 3; // Rotate from north to east
    } else if (righthand == 2) {
        righthand = 0; // Rotate from south to west
    } 

}

/**************** rotateLeft ****************/
/* Rotates avatar's forward and righthand directions 90
 * degrees counterclockwise
*/
void rotateLeft(void)
{

    if (direction == 3){
        direction = 1; // Rotate from east to north
    } else if (direction == 0){
        direction = 2; // Rotate from west to south
    } else if (direction == 1) {
        direction = 0; // Rotate from north to west
    } else if (direction == 2) {
        direction = 3; // Rotate from south to east
    } 

    if (righthand == 3){
        righthand = 1; // Rotate from east to north
    } else if (righthand == 0){
        righthand = 2; // Rotate from west to south
    } else if (righthand == 1) {
        righthand = 0; // Rotate from north to west
    } else if (righthand == 2) {
        righthand = 3; // Rotate from south to east
    }
}

/**************** updateWall ****************/
/* Adds a wall to map based on current position and
 * last-attempted direction
 *
 * Caller provides:
 *   valid pointer to maze, current position
 */
void updateWall(maze_t *maze, XYPos curr) {

    int col = ntohl(curr.x) * 2 + 1;
    int row = ntohl(curr.y) * 2 + 1;

    if (last_dir == 3) { // Add a wall to the east
        setObj(maze, row, col+1, 3);
    } else if (last_dir == 1) { // Add a wall to the north
        setObj(maze, row-1, col, 2);
    } else if (last_dir == 2) { // Add a wall to the south
        setObj(maze, row+1, col, 2);
    } else if (last_dir == 0) { // Add a wall to the west
        setObj(maze, row, col-1, 3);
    }

}

void updateAvatar(maze_t *maze, XYPos curr) {
    int col = ntohl(curr.x) * 2 + 1;
    int row = ntohl(curr.y) * 2 + 1;
    setObj(maze, row, col, 5);
}

void removeAvatar(maze_t *maze, XYPos prev) {
    int col = ntohl(prev.x) * 2 + 1;
    int row = ntohl(prev.y) * 2 + 1;
    setObj(maze, row, col, 1);
}

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
bool checkWall(maze_t* maze, XYPos curr, int direction)
{

    int col = ntohl(curr.x) * 2 + 1;
    int row = ntohl(curr.y) * 2 + 1;

    if (direction == 3) { // If a wall exists to the east
        if (getTile(maze, row, col+1) == 3) {
            return true;
        }
    } else if (direction == 1) { // If a wall exists to the north
        if (getTile(maze, row-1, col) == 2) {
            return true;
        }
    } else if (direction == 2) { // If a wall exists to the south
        if (getTile(maze, row+1, col) == 2) {
            return true;
        }
    } else if (direction == 0) { // If a wall exists to the west
        if (getTile(maze, row, col-1) == 3) {
            return true;
        }
    }

    return false;

}

/**************** sendMsg ****************/
/* Sends AM_AVATAR_MOVE message to server
 *
 * Caller provides:
 *   communication socket, this avatar's ID, and desired direction
 *
 * We return:
 *   true if successful
 */
bool sendMsg(int comm_sock, int avatarID, int direction) 
{
    // 4. write to socket
    AM_Message msg;
    msg.type = htonl(AM_AVATAR_MOVE);
    msg.avatar_move.AvatarId = htonl(avatarID);
    msg.avatar_move.Direction = htonl(direction); 
    // printf("avatarId:%d, direction : %d\n ", avatarID, direction);

    //try to send the move message to the server
    // printf ("Try to send the AM_AVATAR_MOVE message to the server... \n");
    if (send(comm_sock, &msg, sizeof(msg), 0) == -1) {
        fprintf (stderr, "Error: can't send message\n");
        exit (5);
    } 
    return true;
}

/**************** getMessage ****************/
/* Receives message to server
 *
 * Caller provides:
 *   communication socket
 *
 * We return:
 *   message that server provides
 */
AM_Message getMessage(int comm_sock) {

    AM_Message servermsg;
    int receive = 0;
    receive = recv(comm_sock, &servermsg, sizeof(servermsg), 0);
    if (receive < 0) {
        fprintf (stderr, "Error: cannot receive message\n");
        exit (6);               // Error receiving message
    }

    if (receive == 0) {
        fprintf (stderr, "Error: connection closed\n");
        exit (7);               // Error receiving message
    }
    // printf ("Received message from server \n");

    return servermsg;

}