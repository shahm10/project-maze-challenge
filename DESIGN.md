# DESIGN.md for Amazing Maze
## LEPC (Evan, Sally, Wylie & Jason)

### AM_Startup Design Spec
**AM_Startup** is the startup script for the Amazing Maze Challenge. It takes in the parameters for the number of avatars, the difficulty of the maze, and the hostname of the server. It initiates the different avatar programs and their corresponding threads, creating the client server communication.

#### Input: Any inputs to the module

##### command input:
	./AMSTARTUP flume.cs.dartmouth.edu [nAvatars] [Difficulty]
##### example command input:
	./AMSTARTUP flume.cs.dartmouth.edu 2 2
##### Assumption:
The input has to be in such order: `./AMSTARTUP flume.cs.dartmouth.edu [nAvatars] [Difficulty]` else, it will not run properly.
	
- nAvatars: (int) the number of Avatars in the maze. Required to be an integer.
- Difficulty: (int) the difficulty level, on the scale 0 (easy) to 9 (excruciatingly difficult). Required to be an integer.
- Hostname: (char \*) the hostname of the server. Our server will be running on *flume.cs.dartmouth.edu* and using port 17235.

* `AM_INIT_OK` which means the message was successfully processed. Provides a TCP/IP number that allow Avatars to communicate.
* `AM_INIT_FAILED` which means the initialize maze message was not processed. The difficult or number of avatars likely exceeds the range.
* `AM_UNEXPECTED_MSG_TYPE` which means a server has been sent out of order. The server will abort and die during initialization if this is triggered.
* `AM_SERVER_TIMEOUT` which means the server has waited too long without receiving a message from a client.
* `AM_SERVER_OUT_OF_MEM` which means the server cannot allocatew enough memory to serve a maze. The server will abort and die.

#### Output: Any outputs of the module
*AM_INIT  nAvatars	Difficulty* is the initial message sent to the server. This is the first step in starting a new maze game.

*AM_STARTUP* The AMSTARTUP creates the logfile that will be written into by the avatars and used to produce the GUI. It will produce a log file with the name Amazing_$USER_N_D.log. $USER is the current user id, N is the number of Avatars, and D is the difficulty level. 

#### Data Flow: Any data flow through the module
Input for the parameters from the user are passed onto the server. Upon successful initiation, it creates a log file. The server returns a message to the startup client - including the maze port - and startup then initiates Avatar clients. 

#### Data Structures: Major data structures used by the module
In order to connect with the server, the startup client uses structs from the netdb.h library to make the sockets that connect with the server.

``` c
struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
};

struct in_addr {
    unsigned long s_addr;          // load with inet_aton()
};

struct hostent
{
  char *h_name;                     /* Official name of host.  */
  char **h_aliases;                 /* Alias list.  */
  int h_addrtype;                   /* Host address type.  */
  int h_length;                     /* Length of address.  */
  char **h_addr_list;               /* List of addresses from name server.  */
}
```

References for these structs:
[https://www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html]
[https://sites.uclouvain.be/SystInfo/usr/include/netdb.h.html]

#### Pseudo Code: Pseudo code description of the module.
1. Validate the arguments for *nAvatars*, *Difficulty*, and *Hostname*. 
2. Connect to server and send `AM_INIT` message to server.
3. Receive the *MazePort*, *MazeWidth*, and *MazeHeight* from `AM_INIT_OK` message from the server. Extract the MazePort.
4. Create a new logfile with the name `Amazing_$USER_N_D.log` when *$USER* is current userid, *N* is the number of avatars, and *D* is the difficulty.
5. Start up Avatar clients (including the server threads/processes) for the specified number *nAvatars*, each connecting to the specified MazePort.
6. Wait for maze to end.
7. Free the necessary variables

### Avatar Program Design Spec
The **Avatar Program** is the client that communicates with the server. It uses a custom struct to store its own coordinates and determines its next moves based off the information it receives from the server.

#### Input: Any inputs to the module
Receives *MazePort* from *AM_Startup* to connect to on the server.

Receives messages from the server such as:
* `AM_AVATAR_TURN` which gives the turn order and locations of other avatars.
* `AM_AVATAR_OUT_OF_TURN` which means the Avatar has tried to move out of turn.
* `AM_MAZE_SOLVED` which means the maze has been solved.
* `AM_NO_SUCH_AVATAR` which means the AvatarId passed by any message was invalid.

* `AM_UNKNOWN_MSG_TYPE` which means the server cannot understand the message sent.
* `AM_UNEXPECTED_MSG_TYPE` which means a server has been sent out of order.
* `AM_TOO_MANY_MOVES` which means the number of moves for a given maze has been exceeded.
* `AM_SERVER_TIMEOUT` which means the server has waited too long without receiving a message from a client.
* `AM_SERVER_DISK_QUOTA` which means the server cannot create or write a file due to a disk quota error. Avatars may continue to solve the maze, but no log files will be produced.
* `AM_SERVER_OUT_OF_MEM` which means the server cannot allocatew enough memory to serve a maze. The server will abort and die.

#### Output: Any outputs of the module
Sends messages to and from the server such as:
* `AM_AVATAR_READY` which lets the server know that Avatar is ready and passes *AvatarID*.
* `AM_AVATAR_MOVE` which informs the server what direction, indicated by integer, the Avatar wishes to move. It may use a NULL move to stay in place.

#### Data Flow: Any data flow through the module
The avatar client is initiated by the startup client and then communicates to the server via the messages laid out in the previous two sections. See *amazing.h* to see every definition and corresponding error codes.

#### Data Structures: Major data structures used by the module
The Avatar Program Spec uses the data structure `Avatar` as definied by amazing.h. It uses another data structure called `XYPos` that stores the coordinates as 32-bit unsigned integers.

```c
typedef struct Avatar
{
  int fd;
  XYPos pos;
} Avatar;

typedef struct XYPos
{
    uint32_t x;
    uint32_t y;
} XYPos;
```
It also uses the data strcture `maze` and `object` as defined by `maze.h` and `object.h`. 

`object.h` defines a blank tile, horizontal wall, vertical wall, or corner. The objects will be placed in a 2D array to represent the maze, which will be shared by all avatars. 

Object types:

1 is a blank tile
2 is a horizontal wall
3 is a vertical wall
4 is a corner
5 is an avatar

```
typedef struct object object_t;

/* Initialize a new object and allocate memory */
object_t *object_new(void);

/* Returns the type of a desired object */
int getType(object_t *obj);

/* Helper functions to set the type of 
 * the object to a tile, wal, or corner.
 */
void setTile(object_t *obj);

void setHWall(object_t *obj);

void setVWall(object_t *obj);

void setCorner(object_t *obj);

void setAvatar(object_t *obj);

/* Function to delete object and free its memory */
void object_delete(object_t *obj);
``` 

`maze.h` contains a 2D array of objects that will be shared and updated by all the avatars. The avatar will make use of this maze to update walls and check for any existing walls. 

```
typedef struct maze maze_t;

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
maze_t *maze_new(const int width, const int height);

/**************** getTile ****************/
/* Returns the integer type of a desired tile
 *
 * Caller provides:
 *   maze and X Y position of desired tile
 * We return:
 *   integer type of the desired tile
 */
int getTile(maze_t *mz, int x, int y);

/**************** setObj ****************/
/* Sets the integer type of a desired tile
 *
 * Caller provides:
 *   maze, X Y position of desired tile, and desired type
 * We do:
 *   Set tile in the 2D array to desired type
 */
void setObj(maze_t *mz, int x, int y, int type);

/* Helper functions to get maze's width and height */
int getMazeWidth(maze_t *mz);

int getMazeHeight(maze_t *mz);

/* Helper functions to delete and print the maze */
void maze_delete(maze_t *mz);

void maze_print(maze_t* mz);
```

#### Pseudo Code: Pseudo code description of the module.
1. Be initialized by the AM_Startup and connect to the server at the MazePort.
2. Send an `AM_AVATAR_READY` message to the server and wait for an `AM_AVATAR_TURN` message back that contains the position and TurnID.
3. While server is still sending `AM_AVATAR_TURN`
	4. On initial move, orient avatar east and try moving "right" (south)
	5. If avatar runs into a wall
		6. If last move was an attempt to move "right", try moving forward
		7. If last move was an attempt to move forward, rotate avatar's directions 90 degrees counterclockwise and attempt to move to avatar's new "right"
	6. If avatar successfully moves right
		7. Rotate avatar's directions 90 degrees clockwise so it is now facing its previous "right"
		8. Attempt to move "right"
	7. Sends `AM_AVATAR_MOVE` to the server specifying what direction it wants to move.
	8. Continue in wait and moving pattern, logging progress, until:
		9. One of the avatar's connections is broken
		10. Exceed the maximum number of moves (determined by `AM_MAX_MOVES` and Difficulty)
		11. Server's `AM_WAIT_TIME` timer expires
4. The server determines that all of the Avatars are located at the same (x, y) position
5. If received, write `AM_MAZE_SOLVED` message is written into log file once. Otherwise, log any success/progress after each action/non-action.
6. Close files, free memory, exit.

#### Exit Status for *AMSTARTUP*:
Exit 1: "Avatars should be greater than 1 and less than the AM_MAX_AVATAR"

Exit 2: "Difficulty should be greater than 0 and less than 9"

Exit 3: Error in opening a socket

Exit 4: If the hostanme is unknown

Exit 5: Error connecting to stream socket

Exit 6: Error sending the AM_INIT message to the server

Exit 7: Error receiving the message

Exit 8: Error closign the connection

Exit 9: Error creating a log file

Exit 10: Error creating a thread

Exit 11: error initializing the message


#### Exit Status for *Avatar*

Exit 2: Error opening a socket

Exit 3: Error identifying the host

Exit 4: Error sending a message

Exit 5: Error opening a log file

Exit 6: Error receiving the message

Exit 7: Error closing the connection 


