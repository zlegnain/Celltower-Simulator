#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "simulator.h"


// GPS Data for this client as well as the connected tower ID
short  x;
short  y;
short  direction;
char  connectionID;
char  connectedTowerID;

int main(int argc, char * argv[]) {
  int status;
  int bytesRcv;
  // Set up the random seed
  srand(time(NULL));

  // Get the starting coordinate and direction from the command line arguments
  x = atoi(argv[1]);
  y = atoi(argv[2]);
  direction = atoi(argv[3]);

  // To start, this vehicle is not connected to any cell towers
  connectionID = -1;
  connectedTowerID = -1;

  while((x<CITY_WIDTH && y<CITY_HEIGHT) || connectedTowerID) {

    //generate a random number
    int rannum = rand()%3;

    //vehicle turn less
    if(rannum==0){
      direction = direction - VEHICLE_TURN_ANGLE;
    }
    //vehicle turn more
    else if(rannum==1){
      direction = direction + VEHICLE_TURN_ANGLE;
    }

    if (direction > 180) {
      direction = direction - 360;
    }
    else if (direction < 180) {
      direction = direction + 360;
    }
    x = x + VEHICLE_SPEED*cos(direction*M_PI/180.0);
    y = y + VEHICLE_SPEED*sin(direction*M_PI/180.0);

    usleep(50000);


    //create client server
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddr;
    int status, addrSize, bytesRcv;
    unsigned char buffer[30];



    //check if connected to tower
    if(connectedTowerID<0){
      for(int i=0;i<NUM_TOWERS;i++){

        //convert short to byte array
        buffer[0] = CONNECT;
        buffer[1] = (unsigned char)(x/256);
        buffer[2] = (unsigned char)x%256;
        buffer[3] = (unsigned char)(y/256);
        buffer[4] = (unsigned char)y%256;


        //set up server address
        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddress.sin_port = htons((unsigned short) SERVER_PORT + i);

        //connect to the server
        status = connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

        if(status<0){
          close(clientSocket);
          exit(0);
        }


        //send array of data to server
        send(clientSocket, buffer, 5,0);

        //get response from server
        bytesRcv = recv(clientSocket, buffer, sizeof(buffer),0);
        buffer[bytesRcv] = 0;
        if(buffer[0]==YES){
          connectedTowerID = buffer[1];
          connectionID = buffer[2];
          close(clientSocket);
          break;
        }else{
          close(clientSocket);
        }
      }
    }else{

      //convert short to byte array
      buffer[0] = UPDATE;
      buffer[1] = (unsigned char)(x/256);
      buffer[2] = (unsigned char)x%256;
      buffer[3] = (unsigned char)(y/256);
      buffer[4] = (unsigned char)y%256;
      buffer[5] = connectionID;

      //set up server address
      clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      memset(&serverAddress, 0, sizeof(serverAddress));
      serverAddress.sin_family = AF_INET;
      serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
      serverAddress.sin_port = htons((unsigned short) SERVER_PORT + connectedTowerID);

      //connect to the server
      status = connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
      //check if status is error
      if(status < 0){
        close(clientSocket);
        exit(0);
      }
      //send array of data to server
      send(clientSocket, buffer, 6,0);
      //get response from server
      bytesRcv = recv(clientSocket, buffer, sizeof(buffer),0);
      buffer[bytesRcv] = 0;
      if(buffer[0]==YES){
        connectedTowerID = buffer[1];
        connectionID = (int)buffer[2];
        close(clientSocket);
      }else{
        connectedTowerID = -1;
        close(clientSocket);
      }


    }




  }
}
