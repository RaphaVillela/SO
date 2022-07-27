#include "biblioteca.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>

//Enviar inteiro (Funciona)
void sendInt(int number, int socket)
{
	send(socket, &number, sizeof(number), 0);
}
//Enviar inteiro, garantindo que todos o bytes sejam enviados(Parece que funciona)
/*void sendInt(int number, int socket)
{
	int numberbytesend = send(socket, &number, sizeof(int), 0);

	//Transforma o endereco do inteiro em char, para poder percorre-lo
	char* numberPointer = (char*) &number;
	
	while(numberbytesend < sizeof(int))
	{
		numberbytesend += send(socket, (numberPointer + numberbytesend), (sizeof(int)- numberbytesend), 0);
	}
	
}*/

//Receber inteiro (Funciona)
int recvInt(int socket)
{
	int number;
	
	recv(socket, &number, sizeof(number), 0);
	
	return number;
}
//Receber inteiro, garantindo que todos os bytes sejam recebidos(Parece que funciona)
/*int recvInt(int socket)
{
	int number;
	int numberbyterecv = recv(socket, &number, sizeof(int), 0);
	
	while(numberbyterecv < sizeof(int))
	{
		numberbyterecv += recv(socket, (&number + numberbyterecv), (sizeof(int)- numberbyterecv), 0);
	}
	
	return number;
}*/

//Enviar double (Funciona)
/*void sendDouble(double number, int socket)
{
	send(socket, &number, sizeof(double), 0);
}*/

//Enviar double, garantindo que cada byte seja enviado(Não funciona)
/*void sendDouble(double number, int socket)
{
	double numberbytesend = send(socket, &number, sizeof(double), 0);
	
	while(numberbytesend < sizeof(double))
	{
		numberbytesend += send(socket, (&number + numberbytesend), (sizeof(double)- numberbytesend), 0);
	}
}*/

//Receber double (Funciona)
/*double recvDouble(int socket)
{
	double number;

	recv(socket, &number, sizeof(double), 0);
	
	return number;
}*/

//Receber double, garantindo que cada byte seja recebido(Não funciona)
/*double recvDouble(int socket)
{
	double number;
	int numberbyterecv = recv(socket, &number, sizeof(double), 0);
	
	while(numberbyterecv < sizeof(double))
	{
		numberbyterecv += recv(socket, (&number + numberbyterecv), (sizeof(double)- numberbyterecv), 0);
	}
	
	return number;
}*/


//Enviar string(Funciona)
void sendString(char* string, int socket)
{
	int nBytes = sizeof(char) * strlen(string);
	
	//Guardar o tamanho pra usar em recv
	sendInt(nBytes, socket);

	send(socket, string, nBytes, 0);
}
/*
//Enviar string(Não sei se funciona)
void sendString(char* string, int socket)
{
	int nBytes = sizeof(char) * strlen(string);
	
	//Guardar o tamanho pra usar em recv
	sendInt(nBytes, socket);

	int nBytesSend = send(socket, string, nBytes, 0);
	
	while(nBytes - nBytesSend)
	{
		nBytesSend += send(socket, (string + nBytesSend), (nBytes - nBytesSend), 0);
	}
}
*/
//Receber string(Funciona)
char* recvString(int socket)
{	
	int nBytes = recvInt(socket);
	
	char* string = (char*)calloc(nBytes+1, sizeof(char));

	recv(socket, string, nBytes, 0);
	
	return string;
}
/*
//Receber string(Não sei se funciona)
char* recvString(int socket)
{	
	int nBytes = recvInt(socket);
	
	char* string = (char*)calloc(nBytes+1, sizeof(char));

	int nBytesSend = recv(socket, string, nBytes, 0);
	
	while(nBytes - nBytesSend)
	{
		nBytesSend += recv(socket, (string + nBytesSend), (nBytes - nBytesSend), 0);
	}
	
	return string;
}
*/
//Doubles aleatórios
double randomDouble(double min, double max)
{
	return ((double)rand()* (max - min))/(double)RAND_MAX + min;
}

//Retirar o cliente da memoria
void freeClient(ClientList* client)
{
	ClientFile* files = client->data;
	ClientFile* before;

	while(files != NULL)
	{
		before = files;
		files = files->next;
		
		free(before);	
	}

	free(client);
}

//Criar lista, aloca par o first e o last NULL(lista ainda vazia)
Edges* createList()
{
	Edges * edges = (Edges*)calloc(1, sizeof(Edges));
	
	return edges;
}

//Inserir cliente na lista, insere o cliente no final da lista de clientes e no edge last
void addClient(Edges *edge, ClientList *client, int id)
{
	ClientList *lastc = edge->last;

	client->id = id;
	client->next = NULL;

	//Verifica se e o primeiro a ser inserido(vazio)
	if(edge->first == NULL)
	{
		edge->first = client;
		edge->last = client;

		return;
	}

	lastc->next = client;
	edge->last = client;
}

//Seleciona um cliente da lista, caso nao haja, retorna NULL
ClientList* selectClient(int id, Edges *edges)
{
	ClientList *temp = edges->first;

	//Procura o cliente
	while(temp != NULL)
	{
		if(temp->id == id)
			break;

		temp = temp->next;
	}

	return temp;
}

//Deleta um cliente da lista, caso não exista, retorna 0, caso tenha sucesso, retorna 1
int deleteClient(int id, Edges *edges)
{
	ClientList *temp = edges->first;
	ClientList *before = NULL;

	//Procura o cliente
	while(temp != NULL)
	{
		if(temp->id == id)
			break;

		before = temp;
		temp = temp->next;
	}

	//Verifica se foi era o primeiro da lista
	if(before == NULL)
	{
		//Se so existe 1 na lista
		if(edges->first == edges->last)
		{
			edges->first = NULL;
			edges->last = NULL;

			freeClient(temp);
			return 1;
		}

		edges->first = temp->next;
		freeClient(temp);
		return 1;
	}

	//Se for o ultimo da lista
	if(temp->next == edges->last)
	{
		edges->last = before;
		before->next = NULL;
		freeClient(temp);
		return 1;
	}

	//Se nao entrou em nenhum dos anteriores, esta no meio da lista
	before->next = temp->next;
	freeClient(temp);
	return 1;

	return 0;
}

//Procura o cliente pelo nome do arquivo, retorna nulo, caso nao encontre
ClientList* searchClientByFile(Edges* edges, char* fileName)
{
	ClientList* client = edges->first;
	ClientFile* current;
	int finish = 0;

	//Percorrer os clientes
	while(client != NULL)
	{
		current = client->data;

		//Percorre os arquivos do cliente, caso encontre coloca finish para 1 e termina
		while(current != NULL)
		{
			if(strcmp(current->name, fileName) == 0)
			{
				finish = 1;
				break;
			}
			current = current->next;
		}

		if(finish == 1)
			break;
	}

	return client;
}

void sendClientFiles(char* diretorio, int socket)
{
	DIR *dir;
	struct dirent *dp;
	
	//Abrir o diretório
	if((dir = opendir(diretorio)) == NULL)
	{
		printf("Cannot open it\n");
		exit(1);
	}

	int count = 0;
	
	while((dp = readdir (dir)) != NULL)
	{
		if((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0))
			count++;
	}
	
	sendInt(count, socket);

	dir = opendir(diretorio);

	//Enviar o que tem no diretório
	while((dp = readdir (dir)) != NULL)
	{
		if((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0))
			sendString(dp->d_name, socket);
	}	
}

ClientFile* recvClientFiles(int socket)
{
	ClientFile *lastFile =  NULL;
	int total = recvInt(socket);

	for(int i = 0; i < total; i++)
	{
		ClientFile *file = (ClientFile*)calloc(1, sizeof(ClientFile));

		file->name = recvString(socket);

		file->next = lastFile;
		
		lastFile = file;
	}
	
	return lastFile;
}