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


bool avatar_move(int AvatarID, int comm_sock, counters_t* visited,int direction, XYPos currPos, XYPos destination);
bool comparePos(XYPos posA, XYPos posB) ;
XYPos getNextPos(XYPos curr, int direction);
bool sendMsg(int comm_sock, int avatarID, int direction);

// initializes avatar (one of N threads)
int avatar_new(int AvatarID, int nAvatars, int Difficulty, char* hostname, int MazePort, char* filename,XYPos start, XYPos destination) 
{
    // open socket
    int comm_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (comm_sock < 0) {
        perror("opening socket");
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
    printf("Connected!\n");

    // initialize set to hold visited coords
    counters_t* visited_coords = counters_new();
    if (visited_coords == NULL){
        fprintf (stderr, "\n Set initialization failed. \n" );
    }

    int start_direction = 0;
    avatar_move(AvatarID, comm_sock, visited_coords, start_direction, start, destination); 

    close(comm_sock);
    return 0;
}

    
/*
* Recursively read turn messages from server and pass move messages
*/
bool avatar_move(int AvatarID, int comm_sock, counters_t* visited, int direction, XYPos currPos, XYPos destination) 
{
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
    printf ("Received message from server\n");

    // Receive AM_AVATAR_TURN (avatarID, XYPos of all avatars)
    if (ntohl (servermsg.type) == AM_AVATAR_TURN) {
        printf ("AM_AVATAR_TURN received from server");

        // Make sure it is the avatar's turn
        int TurnID = ntohl(servermsg.avatar_turn.TurnId);
        if (TurnID == AvatarID) {

            XYPos prevPos = currPos;

            currPos = servermsg.avatar_turn.Pos[AvatarID];   // get position of self in the maze
            counters_set(visited, currPos.x, currPos.y);

            //Print positions/turnID to stdoutput if received
            printf ("x:%d y:%d\n", currPos.x, currPos.y);
            printf ("TurnId: %d\n", TurnID);

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
            if (counters_get(visited, currPos.x) == currPos.y) {
                fprintf(stderr, "Avatar has reached a visited point. \n");
                return false;
            }

            // otherwise:
            for (int dir = 0; dir <= 3; dir++){
                sendMsg(comm_sock, AvatarID, direction);
                if (avatar_move(AvatarID, comm_sock, visited, dir, currPos, destination) == true){
                    return true;
                }
            }
            // counters_set()  // unmark current square
            return false;  
        }            
    }
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