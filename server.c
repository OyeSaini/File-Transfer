/************************************************************************
* Server -- program to to act like a server                             *
*                                                                       *
*                                                                       *
*                                                                       *
* Author:  Mandeep Singh Sidhu                                          *
*                                                                       *
* Purpose: Processing commands on user's input.                         *
*                                                                       *
* Usage: ./server.out                                                   *
************************************************************************/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

void serviceClient(int);
void sendFile(int client, char *argv[]);
void recieveFile(int client, char *argv[]);

int main(int argc, char * argv[]) {
  int sd, client, portNumber;
  struct sockaddr_in servAdd; // server socket address
  if (argc != 2) {
    printf("Call model: %s <Port Number>\n", argv[0]);
    exit(0);
  }
  /*  socket creates an endpoint for communication and returns a file descriptor */
  sd = socket(AF_INET, SOCK_STREAM, 0);
  /*
     * sockaddr_in is used for ip manipulation
     * we define the port and IP for the connection.
     */
  servAdd.sin_family = AF_INET;
  servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
  sscanf(argv[1], "%d", & portNumber);
  servAdd.sin_port = htons((uint16_t) portNumber);
  bind(sd, (struct sockaddr * ) & servAdd, sizeof(servAdd));
  listen(sd, 20);
  fprintf(stderr, "Server Started...\nWaiting for Clients\n");
  while (1) {
    client = accept(sd, NULL, NULL);
    printf("Got a client\n\n");
    if (!fork())
      serviceClient(client);

    close(client);
  }
}

void serviceClient(int sd) {
  int i = 0;
  char command[255];
  char * substring[10];
  char * token;
  while (1) {
    read(sd, command, 255);
    token = strtok(command, " ");
    while (token != NULL) {
      substring[i] = token;
      token = strtok(NULL, " ");
      i++;
    }
    if (strcmp(substring[0], "get") == 0) {
      sendFile(sd, substring);
    } else if (strcmp(substring[0], "put") == 0) {
      recieveFile(sd, substring);
    } else if (strcmp(substring[0], "quit") == 0) {
      close(sd);
      fprintf(stderr, "Bye, my client is dead, wait for a new one\n");
      fprintf(stderr, "------------------------------------------------------------------------------------\n\n");
      exit(0);
    }
  }
}

void sendFile(int client, char * argv[]) {
  int fd1, end;
  char ch;
  if ((fd1 = open(argv[1], O_RDONLY)) == -1) {
    perror("ERROR");
    exit(1);
  }
  int size = lseek(fd1, 0L, SEEK_END);
  lseek(fd1, 0L, SEEK_SET);
  while ((end = read(fd1, & ch, 1)) == 1) {
    write(client, & ch, 1);
  }
  ch = EOF;
  write(client, & ch, 1);
  close(fd1);
  printf("%s file sent successfully.\n", argv[1]);
  fprintf(stderr, "------------------------------------------------------------------------------------\n\n");
}

void recieveFile(int client, char * argv[]) {
  int fd1, end;
  char name[50], ch, a[50];
  strcpy(name, "Client_");
  strncat(name, argv[1], 20);
  if ((fd1 = open(name, O_CREAT | O_WRONLY | O_TRUNC, 0700)) == -1) {
    perror("ERROR");
  }
  lseek(fd1, 0L, SEEK_SET);
  while ((end = read(client, & ch, 1)) == 1) {
    if (ch == EOF) {
      close(fd1);
      fprintf(stderr, "%s file recieved sucessfully.\n", name);
      fprintf(stderr, "------------------------------------------------------------------------------------\n\n");
      break;
    }
    write(fd1, & ch, 1);
  }
}
