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
#include "biblioteca.h"

int main(int argc, char *argv[])
{

    if( argc != 2 ){ 

        printf("USAGE: server port_number\n");

        return EXIT_FAILURE;

    }

    srand( time( NULL ));
    
    //Id dos clientes
    int id = 0;

    //Inicia o primeiro e ultimo cliente da lista como nulos
    Edges *edges = createList();

    struct sockaddr_in dest; /* socket info about the machine connecting to us */
    struct sockaddr_in serv; /* socket info about our server */
    int mysocket;            /* socket used to listen for incoming connections */
    socklen_t socksize = sizeof(struct sockaddr_in);

    memset(&serv, 0, sizeof(serv));           /* zero the struct before filling the fields */
    serv.sin_family = AF_INET;                /* set the type of connection to TCP/IP */
    serv.sin_addr.s_addr = htonl(INADDR_ANY); /* set our address to any interface */
    serv.sin_port = htons( atoi( argv[ 1 ] ) );           /* set the server port number */

    mysocket = socket(AF_INET, SOCK_STREAM, 0);

    /* bind serv information to mysocket */
    bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));

    /* start listening, allowing a queue of up to 1 pending connection */
    listen(mysocket, 1);


    printf("Server is waiting for connections on port:%s\n", argv[ 1 ] );

    int comsocket = accept(mysocket, (struct sockaddr *)&dest, &socksize);

    printf("Incoming connection from %s - sending welcome\n", inet_ntoa(dest.sin_addr));

	//Receber a porta do cliente para guardar
	//recvInt()

	//Receber o IP do cliente para guardar
	//recvInt()

    ClientFile * files = recvClientFiles(comsocket);

	while(files != NULL)
	{
		printf("* %s\n", files->name);
		files = files->next;
	}

	//Adicionar o cliente na lista

    //int randomInt = rand() % 100;
    //double randomD = randomDouble(0,1);
    //char randomS[7] = "Random";
    //strcpy("Random", randomS);
      
    //printf("sendInt[%d]: %d\n", counter, randomInt);
    //printf("sendDouble[%d]: %lf\n", counter, randomD);
    //printf("sendString: %s\n", randomS);

    //sendInt(randomInt,comsocket);
    //sendDouble(randomD, comsocket);
    //sendString(randomS, comsocket);
        
    //close(socket);
    //socket = accept(mysocket, (struct sockaddr *)&dest, &socksize);
        
    //double dnumber = recvDouble(comsocket);
    //printf("ReceivedDouble[%d] %lf\n\n", counter, dnumber);

    close(mysocket);
    return EXIT_SUCCESS;
}
