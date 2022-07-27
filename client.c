#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include "biblioteca.h"

#define MAXRCVLEN 5000

int main(int argc, char *argv[])
{
	//Porta
	if( argc < 0)
	{ 
		printf("USAGE: client directory port_number\n");

		return EXIT_FAILURE;
	}
    
    srand( time( NULL ));

   char buffer[MAXRCVLEN + 1]; /* +1 so we can add null terminator */
   bzero( buffer, MAXRCVLEN + 1 );
   int len, comsocket;
   struct sockaddr_in dest;

   comsocket = socket(AF_INET, SOCK_STREAM, 0);

   memset(&dest, 0, sizeof(dest));                /* zero the struct */
   dest.sin_family = AF_INET;
   dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* set destination IP number - localhost, 127.0.0.1*/
   dest.sin_port = htons(  atoi( argv[ 1 ]) );                /* set destination port number */

   int connectResult = connect(comsocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

   if( connectResult == - 1 ){

   		printf("CLIENT ERROR: %s\n", strerror(errno));

   		return EXIT_FAILURE;
   }
	//Enviar a porta do cliente para guardar
	//sendInt()

	//Enviar o IP do cliente para guardar
	//sendInt()

	sendClientFiles("./client1/",comsocket);
   
   	//int number = recvInt(comsocket);
   	//double dnumber = recvDouble(comsocket);
   	//char string[7];
   	//strcpy(string, recvString(comsocket));
		
   	/* We have to null terminate the received data ourselves */
   	//buffer[len] = '\0';
		
   	//printf("ReceivedInt[%d] %d\n", counter, number);
   	//printf("ReceivedDouble[%d] %lf\n\n", counter, dnumber);
   	//printf("ReceivedString %s\n", string);
   		
   	//double randomD = randomDouble(0,1);
   	//printf("sendDouble[%d]: %lf\n\n", counter, randomD);
   	//sendDouble(randomD, comsocket);
   
   //close(mysocket);
   return EXIT_SUCCESS;
}
