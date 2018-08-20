/************************************************************************
* Server -- program to to act like a client                             *
*                                                                       *
*                                                                       *
*                                                                       *
* Author:  Gagandeep Singh Saini                                        *
*                                                                       *
* Purpose: Processing commands on user's input.                         *
*                                                                       *
* Usage: ./client.out                                                   *
************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>

void sendToServer(int server, char *argv[]);
void receiveFileFromServer(int server, char *argv[]);

int main(int argc, char * argv[]) {
  char command[255];
  char * substring[10];
  char * token;
  int server, portNumber, i = 0;
  struct sockaddr_in servAdd; // server socket address
  if (argc != 3) {
    printf("Call model: %s <IP Address> <Port Number>\n", argv[0]);
    exit(0);
  }
  /*  socket creates an endpoint for communication and returns a file descriptor */
  if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "Cannot create socket\n");
    exit(1);
  }
  /*
     * sockaddr_in is used for ip manipulation
     * we define the port and IP for the connection.
     */
  servAdd.sin_family = AF_INET;
  sscanf(argv[2], "%d", & portNumber);
  servAdd.sin_port = htons((uint16_t) portNumber);

  if (inet_pton(AF_INET, argv[1], & servAdd.sin_addr) < 0) {
    fprintf(stderr, " inet_pton() has failed\n");
    exit(2);
  }

  /*  keep trying to establish connection with server */
  if (connect(server, (struct sockaddr * ) & servAdd, sizeof(servAdd)) < 0) {
    fprintf(stderr, "connect() has failed, exiting\n");
    exit(3);
  }

  while (1) {
    fprintf(stderr, "Make selection: \n");
    fprintf(stderr, "1. get <fileName>\n");
    fprintf(stderr, "2. put <fileName>\n");
    fprintf(stderr, "3. quit\n\n");
    read(STDIN_FILENO, command, 255);
    int j;
    for (j = 0; j < strlen(command); j++) {
      if (command[j] == '\n' || command[j] == EOF)
        command[j] = '\0';
    }
    /*  sending the filename and command to the server */
    write(server, command, strlen(command) + 1);
    token = strtok(command, " ");
    while (token != NULL) {
      substring[i] = token;
      token = strtok(NULL, " ");
      i++;
    }

    if (strcmp(substring[0], "get") == 0) {
      receiveFileFromServer(server, substring);
    } else if (strcmp(substring[0], "put") == 0) {
      sendToServer(server, substring);
    } else if (strcmp(substring[0], "quit") == 0) {
      fprintf(stderr, "Quitting...\nDisconnected\n\n");
      return 0;
    }
  }
}

void sendToServer(int server, char * argv[]) {
  int fd1, end;
  char ch;
  if ((fd1 = open(argv[1], O_RDONLY)) == -1) {
    perror("ERROR");
    exit(1);
  }
  int size = lseek(fd1, 0L, SEEK_END);
  lseek(fd1, 0L, SEEK_SET);

  while ((end = read(fd1, & ch, 1)) == 1) {
    write(server, & ch, 1);
  }
  ch = EOF;
  write(server, & ch, 1);
  close(fd1);
  fprintf(stderr, "%s file sent succesfully.\n", argv[1]);
  fprintf(stderr, "------------------------------------------------------------------------------------\n\n");
}

void receiveFileFromServer(int server, char * argv[]) {
  int fd1, end;
  char name[50], ch;
  strcpy(name, "Server_");
  strncat(name, argv[1], 20);
  if ((fd1 = open(name, O_CREAT | O_WRONLY | O_TRUNC, 0700)) == -1) {
    perror("ERROR");
  }
  lseek(fd1, 0L, SEEK_SET);
  while ((end = read(server, & ch, 1)) == 1) {
    if (ch == EOF) {
      close(fd1);
      fprintf(stderr, "%s file received succesfully.\n", name);
      fprintf(stderr, "------------------------------------------------------------------------------------\n\n");
      break;
    }
    write(fd1, & ch, 1);
  }
}
