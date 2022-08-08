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

//Fica no cliente para o usuario digitar o comando desejado
void whichFunction(int socket)
{
	char *command = (char*)calloc(MAX_LENGTH, sizeof(char));

	while(1)
	{
		printf("-");

		scanf("%s", command);

		if(strcmp(command, "exit") == 0)
		{
			sendInt(COMMAND_EXIT, socket);

			break;
		}
		else if(strcmp(command, "get") == 0)
		{
			sendInt(COMMAND_GET, socket);
		}
		else if(strcmp(command, "send") == 0)
		{
			sendInt(COMMAND_SEND, socket);
		}
		else if((strcmp(command, "del") == 0) || (strcmp(command, "delete") == 0))
		{
			sendInt(COMMAND_DELETE, socket);
		}
		else if(strcmp(command, "list") == 0)
		{
			sendInt(COMMAND_LIST, socket);

			int n = recvInt(socket);

			for(int i = 0; i < n; i++)
			{
				Client* client = recvClient(socket);

				ClientFile *file = client->data;
				printf("Cliente %d:\n", client->id);
				while(file != NULL)
				{
					printf(" %s\n", file->name);
					file = file->next;
				}
			}
		}
		else if(strcmp(command, "stats") == 0)
		{
			sendInt(COMMAND_STATS, socket);
		}
		else
		{
			printf("comando inválido\n");
		}
	}
}

int main(int argc, char *argv[])
{
	//Porta
	if( argc < 0)
	{ 
		printf("USAGE: client directory port_number\n");

		return EXIT_FAILURE;
	}

	//Argumentos
	char *diretorio = argv[2]; //diretorio

	printf("%s \n", diretorio);
    
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

	//Pegar os dados do cliente para depois enviar para o servidor
	Client *novo = (Client*)calloc(sizeof(Client) , 1);
	novo->ip = 1;
	novo->porta = 2222;
	novo->data =  (ClientFile*)calloc(sizeof(ClientFile), 1);
	novo->nFiles = countFiles(diretorio, novo);
	//"./client1/"

	sendInt(COMMAND_CLIENT, comsocket);
	sendClient(novo, comsocket);

  	whichFunction(comsocket);

	//sendClientFiles("./client1/",comsocket);
   
   	//int number = recvInt(comsocket);
   	//double dnumber = recvDouble(comsocket);
   	//char string[7];
   	//strcpy(string, recvString(comsocket));
		
   	//printf("ReceivedInt[%d] %d\n", counter, number);
   	//printf("ReceivedDouble[%d] %lf\n\n", counter, dnumber);
   	//printf("ReceivedString %s\n", string);
   		
   	//double randomD = randomDouble(0,1);
   	//printf("sendDouble[%d]: %lf\n\n", counter, randomD);
   	//sendDouble(randomD, comsocket);
   
   //close(mysocket);
   return EXIT_SUCCESS;
}
