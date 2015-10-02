/* A simple server in the internet domain using TCP.
myServer.c
D. Thiebaut
Adapted from http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
The port number used in 51717.
This code is compiled and run on the Raspberry as follows:
   
    g++ -o myServer myServer.c
    ./myServer

The server waits for a connection request from a client.
The server assumes the client will send positive integers, which it sends back multiplied by 2.
If the server receives -1 it closes the socket with the client.
If the server receives -2, it exits.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


//int sockfd, newsockfd, clilen;
 //char buffer[256];
//struct sockaddr_in serv_addr, cli_addr;
//int n;
//int data;

// TCP
int socket_desc , client_sock , c , read_size;
struct sockaddr_in server , client;

char client_message[2000];

void error( char *msg ) {
  perror(msg );
//  exit(1);
}



unsigned char tcpOpen(unsigned int portno){
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        //printf("Could not create socket");
        return 0;
    }
//	    puts("Socket created...");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( portno );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
  //      perror("bind failed. Error");
        return 0;
    }
  //  puts("bind done");
    return 1;
// ----- FIN OUVERTURE PORT TCP
}

unsigned char waitTcpClient() // BLOCANT
{
	// ATTENTE CONNEXION CLIENT
	    //Listen
	    listen(socket_desc , 3);

	    //Accept and incoming connection
	    //puts("Waiting for incoming connections...");
	    c = sizeof(struct sockaddr_in);

	    //accept connection from an incoming client // BLOQUANT
	    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);

	    if (client_sock < 0)
	    {
	    //    perror("accept failed");
	        return 0;
	    }
	    //puts("Connection accepted");
	    return 1;
	    // FIN ATTENTE CONNEXION CLIENT

}

int tcpCheck(){
	int myCommand;

    //Receive a message from client
    if((read_size = recv(client_sock , client_message , 3 , 0))>0)
    {
        myCommand=atoi(client_message);
        return (myCommand);
    }

    if(read_size == 0){
   //     puts("Client disconnected");
        fflush(stdout);
    	return (-1);
    }
    else if(read_size == -1){
     //   perror("recv failed");
        return (-2);
    }
    return -1;
}


void tcpWrite(int x){
    //Send the message back to client
	//write(client_sock , client_message , read_size);

	  int n;

	  char buffer[32];
	  sprintf( buffer, "%d\n", x );
	  if ( (n = write( client_sock, buffer, strlen(buffer) ) ) < 0 )
	    error( (char *)( "TCP ERROR writing to socket") );
	buffer[n] = '\0';
}


void tcpWriteString(char *myStringTcp,int count){
	 write(client_sock, myStringTcp, count);
}

void tcpClose(){
	close( socket_desc );
}
