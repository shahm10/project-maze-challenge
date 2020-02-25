# DESIGN.md for Amazing Maze
## LEPC (Evan, Sally, Wylie & Jason)

### AM_Startup Design Spec
*AM_Startup* is the startup script for the Amazing Maze Challenge. It takes in the parameters for the number of avatars, the difficulty of the maze, and the hostname of the server. It initiates the different avatar programs and their corresponding threads, creating the client server communication.

#### Input: Any inputs to the module
-n nAvatars: (int) the number of Avatars in the maze
-d Difficulty: (int) the difficulty level, on the scale 0 (easy) to 9 (excruciatingly difficult)
-h Hostname: (char \*) the hostname of the server. Our server will be running on *flume.cs.dartmouth.edu*.

*AM_INITIALIZE_OK	MazePort	MazeWidth	MazeHeight* which means the message was successfully processed. Provides a TCP/IP number that allow Avatars to communicate.
*AAM_INITIALIZE_FAILED	ErrNum*
*AM_NO_SUCH_AVATAR	AvatarId* which means the AvatarId passed was invalid.

#### Output: Any outputs of the module
*AM_INIT  nAvatars	Difficulty* is the initial message sent to the server. This is the first step in starting a new maze game.

The GUI?

#### Data Flow: Any data flow through the module

#### Data Structures: Major data structures used by the module
Avatar Programs?
Sockets/Threads/Processes?

#### Pseudo Code: Pseudo code description of the module.
1. Validate the arguments for *nAvatars*, *Difficulty*, and *Hostname*. 
2. Connect to server and send *AM_INIT* message to server.
3. Receive the *MazePort*, *MazeWidth*, and *MazeHeight*. Extract the MazePort.
4. Start up Avatar clients (including the server threads/processes) for the specified number *nAvatars*, each connecting to the specified MazePort.
5. Wait for maze to end.

### Avatar Program Design Spec
The *Avatar Program* is the client that communicates with the server. It uses a custom struct to store its own coordinates and determines its next moves based off the information it receives from the server.

#### Input: Any inputs to the module
Receives *MazePort* from *AM_Startup* to connect to on the server.

Receives messages from the server such as:
*AM_AVATAR_TURN* which gives the turn order and locations of other avatars.
AM_MAZE_SOLVED
AM_UNKNOWN_MSG_TYPE
AM_UNEXPECTED_MSG_TYPE
AM_AVATAR_OUT_OF_TURN
AM_TOO_MANY_MOVES


AM_SERVER_TIMEOUT
AM_SERVER_DISK_QUOTA
AM_SERVER_OUT_OF_MEM
^ Not sure if these three go here.

#### Output: Any outputs of the module
Sends messages to and from the server such as:
*AM_AVATAR_READY AvatarID* which lets the server know that Avatar is ready.
*AM_AVATAR_MOVE	AvatarId Direction* which informs the server what direction the Avatar wishes to use.

#### Data Flow: Any data flow through the module
Uses the locations of other avatars in its algorithm to determine it's next move. Perhaps it chooses the left/right/up/down direction that is most strong. e.g. another avatar higher up than it is to the right would encourage a move up. This would consider all of the other avatars locations, more than just one; essentially just using distance comparison.

#### Data Structures: Major data structures used by the module
The Avatar Program Spec uses the data structure of an avatar:

```c
typedef struct avatar {
  int x_location;
  int y_location;
  char* next_move; // Probably something different here
} avatar_t
```

#### Pseudo Code: Pseudo code description of the module.
1. Be initialized by the AM_Startup and connect to the server at the MazePort.
2. Send an *AM_AVATAR_READY* message to the server and wait for an *AM_AVATAR_TURN* message back that contains the position and TurnID.
3. Wait for turn to move. Receive updated *AM_AVATAR_TURN* whilst waiting.
4. Sends *AM_AVATAR_MOVE* to the server specifying what direction it wants to move.
5. Continue in wait and moving pattern, logging progress, until:
    1. One of the avatar's connections is broken
    2. Exceed the maximum number of moves (determined by *AM_MAX_MOVES* and Difficulty)
    3. Server's *AM_WAIT_TIME* timer expires
    4. The server determines that all of the Avatars are located at the same (x, y) position
6. If received, write *AM_MAZE_SOLVED* message is written into log file once. Otherwise, log any success/progress.
7. Close files, free memory, exit.
