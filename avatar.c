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


// initializes avatar (one of N threads)
int avatar_new(int AvatarID, int nAvatars, int Difficulty, char* hostname, int MazePort, int MazeHeight, int MazeWidth, char *logname) 
{
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
    // printf ("Received message from server\n");

    // Receive AM_AVATAR_TURN (avatarID, XYPos of all avatars)
    if (ntohl(servermsg.type) == AM_AVATAR_TURN) {
         printf ("AM_AVATAR_TURN received from server");
        int turn;

        // Make sure it is the avatar's turn
        int TurnID = ntohl(servermsg.avatar_turn.TurnId);
        XYPos start = servermsg.avatar_turn.Pos[AvatarID];
        XYPos destination;
        destination.x = 15;
        destination.y = 15;

            // static int visited[MazeHeight][MazeWidth];
            // if static doesnt work iterate
            // int visited[MazeHeight][MazeWidth];
            // for (int i = 0; i < MazeWidth; i++){
            //     for (int j = 0; j <MazeHeight; j++){
            //         visited[j][i] = 0;
            //     }
            // }
            
            printf("allocating visited.. \n");
            int **visited;
            visited = malloc(MazeHeight * sizeof(*visited));            //needs to be freed eventually - FIX
            // for (int i = 0; i<MazeHeight; i++){
            //     visited[i] = malloc(MazeWidth*sizeof(*visited[i]));

            // }

            int start_direction = 0;
            avatar_move(AvatarID, comm_sock, MazeWidth, MazeHeight, visited, start_direction, start, destination, fp); 
        }

    close(comm_sock);
    printf("closing socket ... \n");
    return 0;
}

    
/*
* Recursively read turn messages from server and pass move messages
*/
// bool avatar_move(int AvatarID, int comm_sock, int MazeWidth, int MazeHeight, int visited[MazeHeight][MazeWidth], int direction, XYPos currPos, XYPos destination) 
bool avatar_move(int AvatarID, int comm_sock, int MazeWidth, int MazeHeight, int** visited, int direction, XYPos currPos, XYPos destination, FILE *fp) 

{
    printf("in move\n");
    //receive message AM_AVATAR_TURN
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
    printf("type: %d \n", servermsg.type);
    printf("type: %d \n", ntohl (servermsg.type));

    // Receive AM_AVATAR_TURN (avatarID, XYPos of all avatars)
    if (ntohl (servermsg.type) == AM_AVATAR_TURN) {
        printf ("AM_AVATAR_TURN received from server \n");

        // Make sure it is the avatar's turn
        int TurnID = ntohl(servermsg.avatar_turn.TurnId);
        printf("TID %d AID %d", TurnID, AvatarID);
        if (TurnID == AvatarID) {
            
            XYPos prevPos = currPos;

            currPos = servermsg.avatar_turn.Pos[AvatarID];   // get position of self in the maze
            visited[currPos.y][currPos.x] = 1;
            // counters_set(visited, currPos.x, currPos.y);


            //Print positions/turnID to stdoutput if received
            fprintf (fp, "x:%d y:%d\n", currPos.x, currPos.y);
            fprintf (fp, "TurnId: %d\n", TurnID);

            // if avatar has reached destination.
            if (comparePos(currPos, destination)) {
                printf("Avatar has reached destination. \n");
                return true;
            }
            
            // if avatar is in the same position as last move
            if (comparePos(currPos, prevPos)) {
                fprintf(stderr, "Avatar hit a wall.  \n");
                return false;
            }

            // if position has already been visited
            if (visited[currPos.y][currPos.x] == 1) {
                fprintf(stderr, "Avatar has reached a visited point. \n");
                return false;
            }
            printf("looping.. \n");
            // otherwise:
            for (int dir = 0; dir <= 3; dir++){
                sendMsg(comm_sock, AvatarID, direction);
                if (avatar_move(AvatarID, comm_sock, MazeHeight, MazeWidth, visited, dir, currPos, destination, fp) == true){

                    return true;
                }
            }

            // counters_set()  // unmark current square
            return false;  
        }            
    }
    // remove currPos (effectively, by manipulating coord to one that will never be hit)
    // counters_set(visited, currPos.x, 5001);
    // visited[currPos.y][currPos.x] = 0;
    return false;
}

/*
 * Returns false if positions are different or one is NULL
 * returns true if positions are same 
 */
bool comparePos(XYPos posA, XYPos posB) 
{
    if (posA.x == 0 || posA.y == 0 || posB.x == 0 || posB.y == 0) {
        return false;
    }
    if (posA.x == posB.x && posA.y == posB.y) {
        return true;
    }
    return false;
}

XYPos getNextPos(XYPos curr, int direction)
{
    XYPos nextPos;
    nextPos = curr;
    
    if (direction < 0 || direction >3){
        fprintf(stderr, "Invalid direction val. \n");
    } else if (direction == 0) {
        nextPos.x = curr.x -1;
    } else if (direction == 1){
        nextPos.y = curr.y +1;
    } else if (direction == 2){
        nextPos.x = curr.x +1;
    } else if (direction ==3){
        nextPos.y = curr.y -1;
    }
    return nextPos;
}


bool sendMsg(int comm_sock, int avatarID, int direction) 
{
    // 4. write to socket
    AM_Message msg;
    msg.type = htonl(AM_AVATAR_MOVE);
    msg.avatar_move.AvatarId = avatarID;
    msg.avatar_move.Direction = direction;

    //try to send the move message to the server
    printf ("Try to send the AM_AVATAR_MOVE message to the server... \n");
    send(comm_sock, &msg, sizeof(AM_Message), 0);
    if (send(comm_sock, &msg, sizeof(AM_Message), 0) == -1) {
        fprintf (stderr, "Error: can't send message\n");
        exit (5);
    } 
    return true;
}


/*
 * takes XYPos as arg
 */
// void unvisit_coord(void *arg, int key, int count) {
//     XYPos currPos = arg;
//     if (currPos.x == key && currPos.y == count){

        
//     }
// }