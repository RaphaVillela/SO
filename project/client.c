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
#include <pthread.h>
#include "biblioteca.h"

#define MAXRCVLEN 5000
#define CLIENT_DELETE 1
#define CLIENT_GET 2
#define CLIENT_SEND 3
#define SPACE " "

//Struct para o mini servidor do cliente
typedef struct clientServer_arg_
{
	int port;
	int server_socket;
	char *diretorio;

}clientServer_arg;

//Struct para os comandos do client
typedef struct clientCommand_arg_
{
	int client_socket;
	int server_socket;
	char* diretorio;
	Client *client;

}clientCommand_arg;

//Lista de comandos que o cliente faz
void *whatClientDo(void* ptr) //socket
{
	clientCommand_arg *arg = (clientCommand_arg*)ptr;
	Client *client = arg->client;

	while(1)
	{
		int command = recvInt(arg->client_socket);

		switch(command)
        {
            case CLIENT_DELETE:
				
				char *file_name = recvString(arg->client_socket);

				//Deletar chamando deleteFile
				deleteFile(file_name, arg->diretorio , client->data);

				//Deletar da list no servidor
				sendInt(DELETE_LIST, arg->server_socket);
				sendString(file_name, arg->server_socket);
				sendClient(client, arg->server_socket);

				break;
			
			case CLIENT_GET:

				//char* file_name = recvString(arg->socket); Receber o nome do arquivo a ser enviado
				//Enviar o arquivo

				break;

			case CLIENT_SEND:

				break;
        }
    }

	return (void*)0;
}

//Mini servidor do cliente
void *clientServer(void* ptr)
{
	clientServer_arg *arg = (clientServer_arg*)ptr;
	struct sockaddr_in dest; /* socket info about the machine connecting to us */
    struct sockaddr_in serv; /* socket info about our server */
    int mysocket;            /* socket used to listen for incoming connections */
    socklen_t socksize = sizeof(struct sockaddr_in);

    memset(&serv, 0, sizeof(serv));           /* zero the struct before filling the fields */
    serv.sin_family = AF_INET;                /* set the type of connection to TCP/IP */
    serv.sin_addr.s_addr = htonl(INADDR_ANY); /* set our address to any interface */
    serv.sin_port = htons( arg->port );           /* set the server port number */

    mysocket = socket(AF_INET, SOCK_STREAM, 0);

    /* bind serv information to mysocket */
    bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));

    /* start listening, allowing a queue of up to 1 pending connection */
    listen(mysocket, 1);

    while(1)
	{
		printf("Client is waiting for conections\n");

		int clientSocket = accept(mysocket, (struct sockaddr *)&dest, &socksize);

		pthread_t *thread_client = calloc(1, sizeof(pthread_t));

		clientCommand_arg *ptr = (clientCommand_arg*)calloc(1, sizeof(clientCommand_arg));
		ptr->client_socket = mysocket; //socket cliente - cliente
		ptr->server_socket = arg->server_socket; // socket client- servidor
		ptr->diretorio = arg->diretorio;

		pthread_create(thread_client, NULL, whatClientDo, ptr);
	}

	return (void*)0;
}

//Fica no cliente para o usuario digitar o comando desejado
int whichFunction(int server_socket)
{
	char *command = (char*)calloc(MAX_LENGTH, sizeof(char));

	while(1)
	{
		printf("-");

		scanf("%s", command);

		if(strcmp(command, "exit") == 0)
		{
			sendInt(COMMAND_EXIT, server_socket);

			break;
		}
		else if(strcmp(command, "get") == 0)
		{
			sendInt(COMMAND_GET, server_socket); //Entrar no comando get do servidor

			//Ler entrada
			//sendString(file_name, socket); // Envia nome do arquivo pra saber qual é o client que tem
			//int porta = recvInt(socket);
			//int ip = recvInt(socket);

			//Conectar ao client que possui o arquivo
			//Envia comando ao cliente sendInt(CLIENT_GET, socket);
			//sendString(file_name, socket); //Envia nome do arquivo a ser pego
			//Client recebe o arquivo
			//Cliente adiciona arquivo em list

		}
		else if(strcmp(command, "send") == 0)
		{
			sendInt(COMMAND_SEND, server_socket);
		}
		else if((strcmp(command, "del") == 0) || (strcmp(command, "delete") == 0) || (strcmp(command, "rmv") == 0))
		{
			printf("Entrou no delete\n");
			char *buffer = getTerminalCommand();
			printf("buffer criado\n");
			char *copy = strdup(buffer);
			printf("file_name criado\n");
			if(copy == NULL)
			{
				printf("Não há espaço suficiente");
				break;
			}

			char *file_name = calloc(1, sizeof(copy));
			printf("Vai entrar no while\n");
			while((file_name = strtok(copy, SPACE)) != NULL)
			{
				printf("Dentro do while, fileName: %s\n", file_name);
				sendInt(COMMAND_DELETE, server_socket); //Entra no comando delete do servidor
				sendString(file_name, server_socket);
				printf("Enviou nome do arquivo\n");

				if(recvInt(server_socket) != 0) //Sai caso não exista o cliente
					break;

				printf("Vai se conectar ao cliente\n");
				int ip = recvInt(server_socket);
				int porta = recvInt(server_socket);
				printf("Pegou ip e porta\n");
				//Se conectar ao cliente
				char buffer[MAXRCVLEN + 1]; /* +1 so we can add null terminator */
				bzero( buffer, MAXRCVLEN + 1 );
				int client_socket;
				struct sockaddr_in dest;

				client_socket = socket(AF_INET, SOCK_STREAM, 0);
				printf("Socket criado\n");
				memset(&dest, 0, sizeof(dest));                /* zero the struct */
				dest.sin_family = AF_INET;
				dest.sin_addr.s_addr = ip; /* set destination IP number - localhost, 127.0.0.1*/
				dest.sin_port =  porta ; /* set destination port number */
				printf("Iniciar conexao. ip: %d  porta: %d\n", ip, porta);
				int connectResult = connect(client_socket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));
				printf("resultado: %d\n", connectResult);
				if( connectResult == - 1 ){

						printf("CLIENT ERROR: %s\n", strerror(errno));

						return EXIT_FAILURE;
				}
				printf("Faz o pedido pra deletar\n");
				sendInt(CLIENT_DELETE, client_socket);
				sendString(file_name, client_socket);
			}
			free(file_name);

		}
		else if(strcmp(command, "list") == 0)
		{
			sendInt(COMMAND_LIST, server_socket);

			int n = recvInt(server_socket);

			for(int i = 0; i < n; i++)
			{
				Client* client = recvClient(server_socket);

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
			sendInt(COMMAND_STATS, server_socket);
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
	unsigned int ip = atoi(argv[3]); //ip
	unsigned int port = atoi(argv[4]); // port

	printf("%s \n", diretorio);
    
	//srand( time( NULL ));

   char buffer[MAXRCVLEN + 1]; /* +1 so we can add null terminator */
   bzero( buffer, MAXRCVLEN + 1 );
   int comsocket;
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
	Client *novo = (Client*)calloc(1, sizeof(Client));
	//novo->ip = ip;
	novo->ip = dest.sin_addr.s_addr;
	novo->porta = port;
	novo->data =  (ClientFile*)calloc(1, sizeof(ClientFile));
	novo->nFiles = countFiles(diretorio, novo);

	sendInt(COMMAND_CLIENT, comsocket);
	sendClient(novo, comsocket);
	
	pthread_t *clientServer_thread = (pthread_t*)calloc(1, sizeof(pthread_t));

	clientServer_arg *arg = (clientServer_arg*)calloc(1, sizeof(clientServer_arg));
	arg->port = atoi( argv[ 1 ] );
	arg->server_socket = comsocket;
	arg->diretorio = diretorio;

	pthread_create(clientServer_thread, NULL, clientServer , arg);

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
