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
	Client *client;

}clientServer_arg;

//Struct para os comandos do client
typedef struct clientCommand_arg_
{
	int client_socket;
	int server_socket;
	char* diretorio;
	Client *client;

}clientCommand_arg;

//Struct para iniciar a thread e conexao com outro cliente
typedef struct connectClient_
{
	int id;
	int server_socket;
	int ip;
	int porta;
	char* file_name;
	char* diretorio;

}connectClient;

//Struct para o send thread
typedef struct thread_of_send_
{
	int client_socket;
	char *path;

}thread_of_send;

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
            case CLIENT_DELETE:;
				
				char *file_name = recvString(arg->client_socket);

				//Deletar chamando deleteFile
				deleteFile(file_name, arg->diretorio);


				//Deletar da list no servidor
				sendInt(DELETE_LIST, arg->server_socket);
				sendString(file_name, arg->server_socket);

				break;
			
			case CLIENT_GET:;

				char* gfile_name = recvString(arg->client_socket); //Receber o nome do arquivo a ser enviado
				
				char* path = (char *)calloc( 1, (strlen(gfile_name) + strlen(arg->diretorio) + 1));
				strcpy(path, arg->diretorio);
				strcat(path, gfile_name);
				//printf("Path que sai: %s", path);

				/*pthread_t *thread_file = calloc(1, sizeof(pthread_t));

				sendFiles_arg *farg = (sendFiles_arg*)calloc(1, sizeof(sendFiles_arg));
				farg->path = path;
				farg->socket = arg->client_socket;

				printf("Antes do sendFile\n");
				pthread_create(thread_file, NULL, sendFilet, farg);*/
				sendFile(path, arg->client_socket);
				//printf("Depois do sendFile\n");
				break;

			case CLIENT_SEND:;
				
				char* sfile_name = recvString(arg->client_socket); //Receber o nome do arquivo a ser enviado

				char* spath = (char *)calloc( 1, (strlen(sfile_name) + strlen(arg->diretorio) + 1));
				strcpy(spath, arg->diretorio);
				strcat(spath, sfile_name);

				recvFile(spath, arg->client_socket); //Cria o arquivo no diretorio
				//printf("Terminou recvFile do Send\n");
				sendInt(ADD_LIST, arg->server_socket); //Pedir pra adicionar o novo arquivo na lista
				sendString(sfile_name, arg->server_socket);
				sendInt(client->id, arg->server_socket);
				//printf("Terminou o CLIENT_SEND\n");
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
		ptr->client_socket = clientSocket; //socket cliente - cliente
		ptr->server_socket = arg->server_socket; // socket client- servidor
		ptr->diretorio = arg->diretorio;
		ptr->client = arg->client;

		pthread_create(thread_client, NULL, whatClientDo, ptr);
	}

	return (void*)0;
}

//Função da conexao da thread com o get
void* thread_get_Connect(void *ptr)
{
	connectClient *arg = (connectClient*)ptr;

	int client_socket;
	struct sockaddr_in dest;

	client_socket = socket(AF_INET, SOCK_STREAM, 0);

	memset(&dest, 0, sizeof(dest));                
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
	dest.sin_port = htons(  arg->porta );                

	int connectResult = connect(client_socket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

	if( connectResult == - 1 ){

		printf("CLIENT ERROR: %s\n", strerror(errno));

		return (void*)0;
	}
	
	sendInt(CLIENT_GET, client_socket);
	sendString(arg->file_name, client_socket);
	printf("Enviado o nome do arquivo\n");

	srand( time( NULL ));
	double randomD = randomDouble(0,100);

	char new_name[1024];

	sprintf(new_name, "%s_client%d_%lf", arg->file_name, arg->id, randomD);

	char* path = (char *)calloc( 1, (strlen(new_name) + strlen(arg->diretorio) + 1));
	strcpy(path, arg->diretorio);
	strcat(path, new_name);

	recvFile(path, client_socket); //Cria o arquivo no diretorio
	sendInt(ADD_LIST, arg->server_socket); //Pedir pra adicionar o novo arquivo na lista
	sendString(new_name, arg->server_socket);
	sendInt(arg->id, arg->server_socket);

	return (void*)0;
}

//Função da conexao da thread com o send
void* thread_send_Connect(void *ptr)
{
	thread_of_send * arg = (thread_of_send*)ptr;
	//printf("AAAAAAAAA socket: %d  path: %s\n", arg->client_socket, arg->path);

	sendFile(arg->path, arg->client_socket);


	return (void*)0;
}

//Fica no cliente para o usuario digitar o comando desejado
int whichFunction(int server_socket, char* diretorio, int id)
{
	char *command = (char*)calloc(MAX_LENGTH, sizeof(char));

	while(1)
	{
		printf("-");

		scanf("%s", command);
		printf("command : %s\n", command);

		if(strcmp(command, "exit") == 0)
		{
			sendInt(COMMAND_EXIT, server_socket);

			break;
		}
		else if(strcmp(command, "get") == 0)
		{
			char *buffer = getTerminalCommand(); //Ler entrada do terminal
			char *copy = strdup(buffer); //Copiar o buffer

			if(copy == NULL) //Verifica se tem espaço
			{
				printf("Não há espaço suficiente\n");
				continue;
			}

			char *file_name = strtok(copy, SPACE);

			while(file_name != NULL)
			{
				if(file_name[strlen(file_name)-1] == '\n')
					file_name[strlen(file_name)-1] = '\0';

				sendInt(COMMAND_GET, server_socket); //Entra no comando get do servidor
				sendString(file_name, server_socket);

				if(recvInt(server_socket) == -1) //Sai caso não exista o cliente
				{
					printf("Arquivo %s não existe\n", file_name);
					file_name = strtok(NULL, SPACE);
					continue;
				}

				connectClient *arg = (connectClient*)calloc(1, sizeof(connectClient));
				arg->id = id;
				arg->server_socket = server_socket;
				arg->ip = recvInt(server_socket);
				arg->porta = recvInt(server_socket);
				arg->file_name = file_name;
				arg->diretorio = diretorio;

				pthread_t *getClient_thread = (pthread_t*)calloc(1, sizeof(pthread_t));

				pthread_create(getClient_thread, NULL, thread_get_Connect , arg);
				
				/*int ip = recvInt(server_socket);
				int porta = recvInt(server_socket);				 

				int client_socket;
				struct sockaddr_in dest;

				client_socket = socket(AF_INET, SOCK_STREAM, 0);

				memset(&dest, 0, sizeof(dest));                
				dest.sin_family = AF_INET;
				dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
				dest.sin_port = htons(  porta );                

				int connectResult = connect(client_socket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

				if( connectResult == - 1 ){

					printf("CLIENT ERROR: %s\n", strerror(errno));

					return EXIT_FAILURE;
				}
				
				sendInt(CLIENT_GET, client_socket);
				sendString(file_name, client_socket);
				printf("Enviado o nome do arquivo\n");

				srand( time( NULL ));
				double randomD = randomDouble(0,100);

				char new_name[1024];

				sprintf(new_name, "%s_client%d_%lf", file_name, id, randomD);

				char* path = (char *)calloc( 1, (strlen(new_name) + strlen(diretorio) + 1));
				strcpy(path, diretorio);
				strcat(path, new_name);

				recvFile(path, client_socket); //Cria o arquivo no diretorio
				sendInt(ADD_LIST, server_socket); //Pedir pra adicionar o novo arquivo na lista
				sendString(new_name, server_socket);
				sendInt(id, server_socket);*/

				file_name = strtok(NULL, SPACE);
			}
			free(file_name);
		}
		else if(strcmp(command, "send") == 0)
		{
			//printf("Entrou no senddddddddddd\n");
			char *ID = (char*)calloc(ID_MAX, sizeof(char));
			scanf("%s ", ID); //Pega o ID do cliente que vai receber os arquivos

			int cid = atoi(ID);

			printf("%d\n", cid);

			sendInt(WHO_IS_CLIENT, server_socket); //Entrar no case WHO IS CLIENT no servidor
			sendInt(cid, server_socket); //Enviar id do cliente que vai receber os arquivos

			if(recvInt(server_socket) == -1) //Sai caso não exista o cliente
				{
					printf("Cliente de id: %d não existe\n", id);
					continue;
				}

			Client *rclient = recvClient(server_socket); //Recebe o clinte do id digitado
			//printf("DAdos do client: id:%d  nFiles:%d  Port:%d\n", rclient->id, rclient->nFiles, rclient->porta);
			char *buffer = getTerminalCommand();
			char *copy = strdup(buffer);

			if(copy == NULL)
			{
				printf("Não há espaço suficiente\n");
				continue;
			}

			char *sfile_name = strtok(copy, SPACE);

			while(sfile_name != NULL)
			{
				if(sfile_name[strlen(sfile_name)-1] == '\n')
					sfile_name[strlen(sfile_name)-1] = '\0';

				//printf("Passou do if\n");
				//printf("File_name : %s\n", sfile_name);

				/*connectClient *arg = (connectClient*)calloc(1, sizeof(connectClient));
				arg->id = id;
				arg->server_socket = server_socket;
				arg->ip = rclient->ip;
				arg->porta = rclient->porta;
				arg->file_name = sfile_name;
				arg->diretorio = diretorio;*/

						 
				int client_socket;
				struct sockaddr_in dest;

				client_socket = socket(AF_INET, SOCK_STREAM, 0);

				memset(&dest, 0, sizeof(dest));                
				dest.sin_family = AF_INET;
				dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
				dest.sin_port = htons(  rclient->porta );                

				int connectResult = connect(client_socket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

				if( connectResult == - 1 ){

					printf("CLIENT ERROR: %s\n", strerror(errno));

					return EXIT_FAILURE;
				}

				srand( time( NULL ));
				double randomD = randomDouble(0,100);

				char new_name[1024];

				sprintf(new_name, "%s_client%d_%lf", sfile_name, id, randomD);

				char* path = (char *)calloc( 1, (strlen(sfile_name) + strlen(diretorio) + 1));
				strcpy(path, diretorio);
				strcat(path, sfile_name);
				//printf("Path para o envio: %s\n", path);
				sendInt(CLIENT_SEND, client_socket);
				//printf("File_name : %s\n", new_name);
				sendString(new_name, client_socket); //Enviar nome do arquivo a ser enviado		
				//printf("Path : %s\n", path);

				thread_of_send *ptr = (thread_of_send*)calloc(1, sizeof(thread_of_send));
				ptr->path = path;
				ptr->client_socket = client_socket;

				pthread_t *sendClient_thread = (pthread_t*)calloc(1, sizeof(pthread_t));

				pthread_create(sendClient_thread, NULL, thread_send_Connect , ptr);
				//printf("socket: %d  path: %s\n", client_socket, path);

				//sendFile(path, client_socket);
				//printf("File terminado\n");

				sfile_name = strtok(NULL, SPACE);
				//printf("File_name atualizado: %s\n", sfile_name);
			}
			free(sfile_name);
		}
		else if((strcmp(command, "del") == 0) || (strcmp(command, "delete") == 0) || (strcmp(command, "rmv") == 0))
		{
			char *buffer = getTerminalCommand();
			char *copy = strdup(buffer);

			if(copy == NULL)
			{
				printf("Não há espaço suficiente\n");
				continue;
			}

			char *file_name = strtok(copy, SPACE);

			while(file_name != NULL)
			{
				if(file_name[strlen(file_name)-1] == '\n')
					file_name[strlen(file_name)-1] = '\0';

				sendInt(COMMAND_DELETE, server_socket); //Entra no comando delete do servidor
				sendString(file_name, server_socket);

				if(recvInt(server_socket) == -1) //Sai caso não exista o cliente
				{
					printf("Arquivo %s não existe\n", file_name);
					file_name = strtok(NULL, SPACE);
					continue;
				}
				
				int ip = recvInt(server_socket);
				int porta = recvInt(server_socket);				 

				int client_socket;
				struct sockaddr_in dest;

				client_socket = socket(AF_INET, SOCK_STREAM, 0);

				memset(&dest, 0, sizeof(dest));                /* zero the struct */
				dest.sin_family = AF_INET;
				dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* set destination IP number - localhost, 127.0.0.1*/
				dest.sin_port = htons(  porta );                /* set destination port number */

				int connectResult = connect(client_socket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

				if( connectResult == - 1 ){

						printf("CLIENT ERROR: %s\n", strerror(errno));

						return EXIT_FAILURE;
				}

				sendInt(CLIENT_DELETE, client_socket);
				sendString(file_name, client_socket);

				file_name = strtok(NULL, SPACE);
			}
			free(file_name);

		}
		else if(strcmp(command, "list") == 0)
		{
			sendInt(COMMAND_LIST, server_socket);

			int n = recvInt(server_socket);
			//printf("entrei list - %d\n", n);
			for(int i = 0; i < n; i++)
			{
				//printf("Fooooooor\n");
				Client* client = recvClient(server_socket);
				//printf("Fooooooor222222\n");
				ClientFile *file = client->data;
				printf("Cliente %d:\n", client->id);
				while(file != NULL)
				{
					printf(" %s\n", file->name);
					file = file->next;
				}
			}
			printf("3\n");
		}
		else if(strcmp(command, "stats") == 0)
		{
			sendInt(COMMAND_STATS, server_socket);

			int n = recvInt(server_socket);

			for(int i = 0; i < n; i++)
			{
				Client* client = recvClient(server_socket);

				ClientFile *file = client->data;
				printf("Cliente %d:\n", client->id);
				printf("Qtd files = %d\n", client->nFiles);
        		printf("Porta = %d\n", client->porta);
        		printf("\n");
				/*while(file != NULL)
				{
					printf(" %s\n", file->name);
					file = file->next;
				}*/
			}
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
    
	srand( time( NULL ));

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
	novo->id = recvInt(comsocket); //Receber o id pelo servidor
	
	pthread_t *clientServer_thread = (pthread_t*)calloc(1, sizeof(pthread_t));

	clientServer_arg *arg = (clientServer_arg*)calloc(1, sizeof(clientServer_arg));
	arg->port = port;
	arg->server_socket = comsocket;
	arg->diretorio = diretorio;
	arg->client = novo;

	pthread_create(clientServer_thread, NULL, clientServer , arg);

  	whichFunction(comsocket, diretorio, novo->id);

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
