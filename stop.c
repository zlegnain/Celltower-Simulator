#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "simulator.h"
#include <string.h>


void main() {
  int                 clientSocket;  // client socket id
  struct sockaddr_in  serverAddress; // client address
  int                 status;
  unsigned char       command = SHUTDOWN;
  unsigned buffer[30];

  // Contact all the cell towers and ask them to shut down
  // ...

  /*Loop through and send a command for each tower*/
  for(int i=0;i<7;i++){
    buffer[0] = command;

    /*create client socket*/
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket < 0) {
      printf("*** CLIENT ERROR: Could not open socket.\n");
      exit(-1);
    }
    // Setup address
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddress.sin_port = htons((unsigned short) SERVER_PORT+i);

    /*connect to server*/
    status = connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    send(clientSocket, buffer, strlen((char*)buffer),0);
    close(clientSocket);
  }


}
