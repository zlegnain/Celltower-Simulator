#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>



/* Handle client requests coming in through the server socket.  This code should run
 indefinitiely.  It should wait for a client to send a request, process it, and then
 close the client connection and wait for another client.  The requests that may be
 handles are SHUTDOWN, CONNECT and UPDATE.  A SHUTDOWN request causes the tower to
 go offline.   A CONNECT request contains 4 additional bytes which are the high and
 low bytes of the vehicle's X coordinate, followed by the high and low bytes of its
 Y coordinate.  If within range of this tower, the connection is accepted and a YES
 is returned, along with a char id for the vehicle and the tower id.   If UPDATE is
 received, the additional 4 byes for the (X,Y) coordinate are also received as well
 as the id of the vehicle.   Then YES is returned if the vehicle is still within
 the tower range, otherwise NO is returned. */

void *handleIncomingRequests(void *ct) {
  CellTower       *tower = ct;
  int serverSocket, clientSocket;
  struct sockaddr_in serverAddress, clientAddr;
  int status, addrSize, bytesRcv;
  unsigned char buffer[30];
  unsigned char response[30];

  //create the server socket
  serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serverSocket < 0) {
    printf("*** SERVER ERROR: Could not open socket.\n");
    exit(-1);
  }
  //set up server address
  memset(&serverAddress, 0, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddress.sin_port = htons((unsigned short) SERVER_PORT + tower->id);

  //Bind the server
  status = bind(serverSocket, (struct sockaddr *)&serverAddress,sizeof(serverAddress));
  if (status < 0) {
    printf("*** SERVER ERROR: Could not bind socket.\n");
    exit(-1);
  }

  //Listen to incoming clients
  status = listen(serverSocket, MAX_CONNECTIONS*3);
  if (status < 0) {
    printf("*** SERVER ERROR: Could not listen on socket.\n");
    exit(-1);
  }

  //wait for clients
  while(tower->online){
    addrSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket,(struct sockaddr *)&clientAddr,&addrSize);
    if (clientSocket < 0) {
      printf("*** SERVER ERROR: Could accept incoming client connection.\n");
      exit(-1);
    }
    //receive message from client
    bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);

    //add null terminator to get array of chars
    buffer[bytesRcv] = 0;

    //check if shutdown command
    if(buffer[0]==SHUTDOWN){
      tower->online = 0;
      close(clientSocket);
      break;
    }
    //check if CONNECT command
    else if(buffer[0]==CONNECT){
      /*loop through connectedVehicles array*/
      int boo = 0;
      for(int i=0;i<MAX_CONNECTIONS;i++){
        if(tower->connectedVehicles[i].connected == 0){
          short  X = (buffer[1]*256 + buffer[2]);
          short Y = (buffer[3]*256 + buffer[4]);


          //check if within range of tower
          if(((X-tower->x)*(X-tower->x)) + ((Y-tower->y)*(Y-tower->y)) < ((tower->radius)*(tower->radius))){
            boo = 1;
            response[0] = YES;
            response[1] = tower->id;
            response[2] = (char)i;
            tower->connectedVehicles[i].connected = 1;
            tower->connectedVehicles[i].x = X;
            tower->connectedVehicles[i].y = Y;
            tower->numConnectedVehicles++;
            send(clientSocket, response, 3, 0);
            close(clientSocket);
            break;
          }
        }
      }
      //check if conditions were not met
      if(!boo){
        response[0] = NO;
        send(clientSocket, response, 1,0);
        close(clientSocket);
      }
    //check if update command
    }else if(buffer[0]==UPDATE){

      //set our received variables
      short  X = (buffer[1]*256 + buffer[2]);
      short Y = (buffer[3]*256 + buffer[4]);
      int pos = (int)buffer[5];

      //check if within range of tower
      if(((X-tower->x)*(X-tower->x)) + ((Y-tower->y)*(Y-tower->y)) < ((tower->radius)*(tower->radius))){
        response[0] = YES;
        response[1] = tower->id;
        response[2] = (char)pos;
        tower->connectedVehicles[pos].x = X;
        tower->connectedVehicles[pos].y = Y;
        send(clientSocket, response, 3, 0);
        close(clientSocket);
      }else{
        response[0] = NO;
        tower->connectedVehicles[pos].connected = 0;
        tower->numConnectedVehicles--;
        send(clientSocket, response, 1,0);
        close(clientSocket);
      }

    }

}
  close(serverSocket);

}
