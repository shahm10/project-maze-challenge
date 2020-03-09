/* 
 * inclient - an example Internet client.
 *
 * Makes a connection to the given host/port and sends a message 
 * to that socket.
 * 
 * usage: inclient hostname port
 * 
 * David Kotz, 1987, 1992, 2016
 * Adapted from Figure 7a in Introductory 4.3bsd IPC, PS1:7-15.
 *
 * updated by Xia Zhou, August 2016, 2017, 2018
 * updated by Temi Prioleau, 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	      // read, write, close
#include <string.h>	      // memcpy, memset
#include <netdb.h>	      // socket-related structures
#include <pthread.h>
#include <sys/socket.h>
#include "amazing.h"
#include <time.h>
#include <pthread.h>
#include "object.h"
#include "maze.h"

#include "avatar.h"

/**************** file-local constants ****************/
#define BUFSIZE 1024     // read/write buffer size

/**************** global variables ****************/
  char *program;	  // this program's name
  char *hostname;	  // server hostname
  int port;
  int nAvatars;
  int difficulty;
  int mazeport;
  int mazewidth;
  int mazeheight;
  char logname[200];
  maze_t *maze; // map to be shared by all the avatars


void* initiate_avatar(void* id_num_pointer);

/**************** main() ****************/
int
main(const int argc, char *argv[])
{

  // 0. Check arguments
  program = argv[0];
  if (argc != 4) {
    fprintf(stderr, "usage: %s hostname nAvatars difficulty \n", program);
    exit(1);
  } else {
    hostname = argv[1];
    nAvatars = atoi(argv[2]);
    difficulty = atoi(argv[3]);
    port = atoi(AM_SERVER_PORT);// assigned from var in amazing.h
    //Need to include check conditions for nAvatars and difficulty to be an ingteger

  }
  if (nAvatars < 1 || nAvatars > AM_MAX_AVATAR) {
    fprintf (stderr, "Avatars should be greater than 1 and less than %d\n", AM_MAX_AVATAR);
    exit (1);
  }
  
  if (difficulty < 0 || difficulty > AM_MAX_DIFFICULTY) {
    fprintf (stderr, "Difficulty should be greater than 0 and less than 9\n");
    exit (2);
  }

  // 1. Create socket
  int comm_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (comm_sock < 0) {
    perror("opening socket");
    exit(2);
  }

  // 2. Initialize the fields of the server address
  struct sockaddr_in server;  // address of the server
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  // Look up the hostname specified on command line
  struct hostent *hostp = gethostbyname(hostname); // server hostname
  if (hostp == NULL) {
    fprintf(stderr, "%s: unknown host '%s'\n", program, hostname);
    exit(3);
  }  
  memcpy(&server.sin_addr, hostp->h_addr_list[0], hostp->h_length);

  // 3. Connect the socket to that server   
  if (connect(comm_sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
    perror("connecting stream socket");
    exit(4);
  }
  printf("Connected!\n");

  // 4. Read content from stdin (file descriptor = 0) and write to socket
  // char buf[BUFSIZE];    // a buffer for reading data from stdin
  // int bytes_read;       // #bytes read from socket
  // memset(buf, 0, BUFSIZE); // clear up the buffer
  AM_Message msg;
  memset(&msg, 0, sizeof(AM_Message));
  msg.type = htonl(AM_INIT);
  msg.init.Difficulty = htonl(difficulty);
  msg.init.nAvatars = htonl(nAvatars);

  //try to send the AM_INIT message to the server
  send(comm_sock, &msg, sizeof(AM_Message), 0);
  if (send(comm_sock, &msg, sizeof(AM_Message), 0) == -1) {
    fprintf (stderr, "Error: can't send message\n");
    exit (5);
  }

  //receive message AM_INIT OK
  AM_Message servermsg;
  int receive = 0;
  receive = recv(comm_sock, &servermsg, sizeof(AM_Message), 0);
  if (receive < 0) {
    fprintf (stderr, "Error: cannot receive message\n");
    exit (6);
  }

  if (receive == 0) {
    fprintf (stderr, "Error: connection closed\n");
    exit (7);
  }
  printf ("Server connected\n");

  if (ntohl (servermsg.type) == AM_INIT_OK) {
    mazeport =  ntohl(servermsg.init_ok.MazePort);
    mazewidth = ntohl(servermsg.init_ok.MazeWidth);
    mazeheight = ntohl(servermsg.init_ok.MazeHeight);

    // 5. Create the log file 
    FILE *fp;
    char *user = getenv ("USER");
    sprintf (logname, "./Results/Amazing_%s_%d_%d.log", user, nAvatars, difficulty);

    fp = fopen (logname, "w");
    if (fp == NULL) {
      fprintf (stderr, "Error: cannot create log file \n");
      exit (5);
    }
    printf("Log file created!\n");
    
    // Get the time info for log files 
    time_t currentime;
    struct tm *timeptr;

    time(&currentime);
    timeptr = localtime(&currentime);
    fprintf(fp, "%s, %d, %s", user, ntohl(servermsg.init_ok.MazePort), asctime(timeptr));
  
    fclose (fp);
  }

  // Initialize map that will be shared by all avatars
  maze = maze_new(mazewidth, mazeheight);

  //     6. Need to initiate the avatars & start up N threads 
  pthread_t arraythread[nAvatars];
  for (int i = 0; i < nAvatars; i++) {
    int check = pthread_create(&arraythread[i], NULL, initiate_avatar, (void *) (intptr_t) i);
    if (check) {
      fprintf (stderr, "thread not created \n");
      exit (3);
    }

  }
  for (int j = 0; j < nAvatars; j++) {
    pthread_join (arraythread[j], NULL);
    //can change the NULL to exit status later
  }

  if (ntohl (servermsg.type) == AM_INIT_FAILED) {
    fprintf (stderr, "\nInitialization failed.\n");
    //need to clean up and free everything
    close(comm_sock);
    exit (5);
  }  

  // Delete map
  maze_delete(maze);

  close(comm_sock);
  return 0;

}

void* initiate_avatar(void* arg)
{

  // Storing number from the null pointer
  int id_num = (int) (intptr_t) arg;
  // Start new avatar
  avatar_new(maze, id_num, nAvatars, difficulty, hostname, mazeport, mazeheight, mazewidth, logname);
  // printf("initiated avatar \n");

  return NULL;
}
