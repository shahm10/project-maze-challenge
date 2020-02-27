# DESIGN.md for Amazing Maze
## LEPC (Evan, Sally, Wylie & Jason)

### AM_Startup Design Spec
**AM_Startup** is the startup script for the Amazing Maze Challenge. It takes in the parameters for the number of avatars, the difficulty of the maze, and the hostname of the server. It initiates the different avatar programs and their corresponding threads, creating the client server communication.

#### Input: Any inputs to the module
- nAvatars: (int) the number of Avatars in the maze
- Difficulty: (int) the difficulty level, on the scale 0 (easy) to 9 (excruciatingly difficult)
- Hostname: (char \*) the hostname of the server. Our server will be running on *flume.cs.dartmouth.edu* and using port 17235.

* `AM_INIT_OK` which means the message was successfully processed. Provides a TCP/IP number that allow Avatars to communicate.
* `AM_INIT_FAILED` which means the initialize maze message was not processed. The difficult or number of avatars likely exceeds the range.
* `AM_UNEXPECTED_MSG_TYPE` which means a server has been sent out of order. The server will abort and die during initialization if this is triggered.
* `AM_SERVER_TIMEOUT` which means the server has waited too long without receiving a message from a client.
* `AM_SERVER_OUT_OF_MEM` which means the server cannot allocatew enough memory to serve a maze. The server will abort and die.

#### Output: Any outputs of the module
*AM_INIT  nAvatars	Difficulty* is the initial message sent to the server. This is the first step in starting a new maze game.

The startup also creates the logfile that will be written into by the avatars and used to produce the GUI.

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

#### Pseudo Code: Pseudo code description of the module.
1. Be initialized by the AM_Startup and connect to the server at the MazePort.
2. Send an `AM_AVATAR_READY` message to the server and wait for an `AM_AVATAR_TURN` message back that contains the position and TurnID.
3. Wait for turn to move. Receive updated `AM_AVATAR_TURN` whilst waiting.
4. Sends `AM_AVATAR_MOVE` to the server specifying what direction it wants to move.
5. Continue in wait and moving pattern, logging progress, until:
    1. One of the avatar's connections is broken
    2. Exceed the maximum number of moves (determined by `AM_MAX_MOVES` and Difficulty)
    3. Server's `AM_WAIT_TIME` timer expires
    4. The server determines that all of the Avatars are located at the same (x, y) position
6. If received, write `AM_MAZE_SOLVED` message is written into log file once. Otherwise, log any success/progress after each action/non-action.
7. Close files, free memory, exit.