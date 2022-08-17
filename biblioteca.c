#include "biblioteca.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>

//Enviar inteiro (Funciona)
/*void sendInt(int number, int socket)
{
	send(socket, &number, sizeof(number), 0);
}*/
//Enviar inteiro, garantindo que todos o bytes sejam enviados(Parece que funciona)
void sendInt(int number, int socket)
{
	int numberbytesend = send(socket, &number, sizeof(int), 0);

	//Transforma o endereco do inteiro em char, para poder percorre-lo
	//char* numberPointer = (char*) &number;
	
	while(numberbytesend < sizeof(int))
	{
		numberbytesend += send(socket, (&number + numberbytesend), (sizeof(int)- numberbytesend), 0);
	}
	
}

//Receber inteiro (Funciona)
/*int recvInt(int socket)
{
	int number;
	
	recv(socket, &number, sizeof(number), 0);
	
	return number;
}*/
//Receber inteiro, garantindo que todos os bytes sejam recebidos(Parece que funciona)
int recvInt(int socket)
{
	int number;
	int numberbyterecv = recv(socket, &number, sizeof(int), 0);
	
	while(numberbyterecv < sizeof(int))
	{
		numberbyterecv += recv(socket, (&number + numberbyterecv), (sizeof(int)- numberbyterecv), 0);
	}
	
	return number;
}

//Enviar double (Funciona)
void sendDouble(double number, int socket)
{
	send(socket, &number, sizeof(double), 0);
}

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
double recvDouble(int socket)
{
	double number;

	recv(socket, &number, sizeof(double), 0);
	
	return number;
}

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
/*void sendString(char* string, int socket)
{
	int nBytes = sizeof(char) * strlen(string);
	//printf("Enviado nBytes : %d\n", nBytes);
	//Guardar o tamanho pra usar em recv
	sendInt(nBytes, socket);
	//printf("Enviado string : %s\n", string);
	send(socket, string, nBytes, 0);
}*/

//Enviar string(Não sei se funciona)
void sendString(char* string, int socket)
{
	int nBytes = sizeof(char) * strlen(string);
	
	//Guardar o tamanho pra usar em recv
	sendInt(nBytes, socket);

	int nBytesSend = send(socket, string, nBytes, 0);
	
	while(nBytes > nBytesSend)
	{
		nBytesSend += send(socket, (string + nBytesSend), (nBytes - nBytesSend), 0);
	}
}

//Receber string(Funciona)
/*char* recvString(int socket)
{	
	int nBytes = recvInt(socket);
	//printf("Recv nBytes : %d\n", nBytes);
	
	char* string = (char*)calloc(nBytes+1, sizeof(char));
	
	recv(socket, string, nBytes, 0);
	//printf("Recv String : %s\n", string);
	return string;
}*/

//Receber string(Não sei se funciona)
char* recvString(int socket)
{	
	int nBytes = recvInt(socket);
	
	char* string = (char*)calloc(nBytes+1, sizeof(char));

	int nBytesSend = recv(socket, string, nBytes, 0);
	
	while(nBytes > nBytesSend)
	{
		nBytesSend += recv(socket, (string + nBytesSend), (nBytes - nBytesSend), 0);
	}
	
	return string;
}


//Vai enviar os dados do Cliente
void sendClient(Client *client, int socket)
{
	sendInt (client->id, socket); // envia id
	sendInt (client->nFiles, socket); //envia a quantidade de arquivos
	sendInt (client->porta, socket); //envia a porta 
	sendInt (client->ip, socket); //envia o ip
	sendClientFiles(client->data, socket); //envia os dados(nome dos arquivos)
}

//Vai receber os dados do Cliente e retornar o cliente com os dados preenchidos
Client* recvClient(int socket)
{
	Client * client = calloc(1, sizeof(Client));

	client->id = recvInt (socket); // recebe id
	//printf("Fooooooor1111\n");
	client->nFiles = recvInt (socket); //recebe a quantidade de arquivos
	//printf("Fooooooor2222\n");
	client->porta = recvInt (socket); //recebe a porta
	//printf("Fooooooor3333\n");
	client->ip = recvInt (socket); //recebe o ip
	//printf("Fooooooor4444\n");
	client->data = recvClientFiles(socket); //recebe os dados(nome dos arquivos)
	//printf("Fooooooor5555\n");

	return client;
}

//Vai ler um arquivo e enviar os dados dele como string
void sendFile(char* path, int socket)
{
	//Abrir arquivo para leitura
	FILE *fptr;

	if((fptr = fopen(path, "r")) == NULL)
	{
		printf("Erro ao abrir arquivo\n");
		return;
	}
	printf("Arquivo para leitura aberto\n");

	fseek(fptr, 0, SEEK_END);
	int size = ftell(fptr); //Tamanho do arquivo
	fseek(fptr, 0, SEEK_SET); //Colocar no inicio do arquivo para leitura
	printf("Tamanho do arquivo: %d\n", size);

	char *block = (char*)calloc(BLOCK_SIZE, sizeof(char));

	int nblocks = 0;
	for(int i = 0; ;i++) //Conta quantos blocos existem
	{
		if((i*BLOCK_SIZE) >= size)
			break;

		nblocks++;
	}
	sendInt(nblocks, socket);
	printf("Blocos: %d\n",nblocks);
	
	int total_size = 0;
	int bread = 0;

	while(bread = fread(block, sizeof(char), BLOCK_SIZE, fptr))
	{
			//printf("bread: %d\n", bread);

			//printf("String lida: %s      length: %d\n", block, bread);
			sendInt(bread, socket);
			sendString(block, socket);
	}
	//Fechar o arquivo
	fclose(fptr);
}

//Vai ler um arquivo e enviar os dados dele como string com thread
void *sendFilet(void *ptr)
{
	sendFiles_arg *arg = (sendFiles_arg*)ptr;

	//Abrir arquivo para leitura
	FILE *fptr;

	if((fptr = fopen(arg->path, "r")) == NULL)
	{
		printf("Erro ao abrir arquivo\n");
		return (void*)0;
	}
	printf("Arquivo para leitura aberto\n");

	fseek(fptr, 0, SEEK_END);
	int size = ftell(fptr); //Tamanho do arquivo
	fseek(fptr, 0, SEEK_SET); //Colocar no inicio do arquivo para leitura
	printf("Tamanho do arquivo: %d bytes\n", size);

	char *block = (char*)calloc(BLOCK_SIZE, sizeof(char));

	int nblocks = 0;
	for(int i = 0; ;i++) //Conta quantos blocos existem
	{
		if((i*BLOCK_SIZE) >= size)
			break;

		nblocks++;
	}
	sendInt(nblocks, arg->socket);
	//printf("Blocos: %d\n",nblocks);
	
	int total_size = 0;
	int bread = 0;

	while(bread = fread(block, sizeof(char), BLOCK_SIZE, fptr))
	{
		//printf("bread: %d\n", bread);
		//printf("String lida: %s   length: %d\n", block, bread);
		sendInt(bread, arg->socket);
		sendString(block, arg->socket);
	}
	//printf("Saiu do while do SENDFILE\n");
	fclose(fptr);

	return (void*)0;
}

//Recebe o caminho para poder criar o arquivo aqui mesmo
void recvFile(char* path, int socket)
{
	FILE *fptr;

	//printf("PAth: %s\n", path);

	if((fptr = fopen(path, "w")) == NULL)
	{
		printf("Erro ao abrir arquivo\n");
		return;
	}
	int nblocks = recvInt(socket); //Recebe a quantidade de blocos a ser enviada
	printf("Blocos: %d\n",nblocks);
	//printf("Arquivo para escrita aberto\n");
	int length = 0;
	char *block = calloc(1, sizeof(BLOCK_SIZE));

	for(int i = 0; i < nblocks; i++)
	{
		length = recvInt(socket);
		block = recvString(socket);

		fwrite(block, 1, length, fptr);

		//printf("String lida: %s\n", block);		
	}

	fclose(fptr);
}

//Doubles aleatórios
double randomDouble(double min, double max)
{
	return ((double)rand()* (max - min))/(double)RAND_MAX + min;
}

//Retirar o cliente da memoria
void freeClient(Client* client)
{
	ClientFile* files = client->data;
	ClientFile* before = NULL;
	
	while(files != NULL)
	{

		before = files;
		files = files->next;
		
		free(before->name);
		free(before);
	}
	
	free(client);
}

//Criar lista, aloca para o first e o last NULL(lista ainda vazia)
List* createList()
{
	List * list = (List*)calloc(1, sizeof(List));
	
	return list;
}

//Inserir cliente na lista, insere o cliente no final da lista de clientes
void addClient(List *list, Client *client)
{
	Client *lastc = list->last;

	client->next = NULL;

	//Verifica se é o primeiro a ser inserido(vazio)
	if(list->first == NULL)
	{
		list->first = client;
		list->last = client;

		return;
	}

	lastc->next = client;
	list->last = client;
}

//Seleciona um cliente da lista, caso nao haja, retorna NULL
Client* selectClient(int id, List *list)
{
	Client *temp = list->first;

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
int deleteClient(int id, List *list)
{
	Client *temp = list->first;
	Client *before = NULL;
	
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
		if(list->first == list->last)
		{
			list->first = NULL;
			list->last = NULL;
			freeClient(temp);
			return 1;
		}else
		{
			list->first = temp->next;
			freeClient(temp);
			return 1;
		}

	}else if(temp->next == NULL) //Se for o ultimo da lista
	{
		list->last = before;
		before->next = NULL;
		freeClient(temp);
		return 1;
	}else
	{
		//Se nao entrou em nenhum dos anteriores, esta no meio da lista
		before->next = temp->next;
		freeClient(temp);
		return 1;
	}

	return 0;
}

//Procura o cliente pelo nome do arquivo, retorna nulo, caso nao encontre
Client* searchClientByFile(List* list, char* fileName)
{
	Client* client = list->first;
	ClientFile* current;

	//Percorrer os clientes
	while(client != NULL)
	{
		current = client->data;

		//Percorre os arquivos do cliente, caso encontre coloca finish para 1 e termina
		while(current != NULL)
		{
			if(strcmp(current->name, fileName) == 0)
			{
				return client;
			}
			current = current->next;
		}
		client = client->next;
	}

	return NULL;
}

//Procura o client pela id dele caso exista retorna ele, se não retorna NULL
Client* searchClientById(List* list, int id)
{
	Client* client = list->first;

	//Percorrer os clientes
	while(client != NULL)
	{
		if(client->id == id)
			return client;

		client = client->next;
	}

	return NULL;
}


//Envia os arquivos do cliente pelo socket
void sendClientFiles(ClientFile* cf, int socket)
{
	ClientFile* file = cf;
	int count = 0;

	while(file != NULL)
	{
		count++;
		file = file->next;
	}

	sendInt(count, socket);

	file = cf;

	//Enviar o que tem no diretório
	while(file != NULL)
	{
		sendString(file->name, socket);

		file = file->next;
	}	
}

//Recebe os arquivos enviados do cliente pelo socket
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

//Conta quantos arquivos existem no cliente e adiciona eles ao cliente, ligando na lista
int countFiles(char* diretorio, Client *novo)
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

	ClientFile *first = NULL;
	ClientFile *last = NULL;
	
	while((dp = readdir (dir)) != NULL)
	{
		if((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0))
		{
			ClientFile *aux = (ClientFile*)calloc(1, sizeof(ClientFile));			

			aux->name = (char*)calloc(1, sizeof(dp->d_name));
			aux->next = NULL;
			strcpy(aux->name, dp->d_name);
			if(first == NULL)
				first = aux;
			if(last != NULL)
			{
				last->next = aux;
				last = aux;
			}
			else
				last = aux;
				
			count++;
		}
	}
	novo->data = first;

	return count;
}

//Adicionar o nome do arquivo, na lista do servidor
void addFile(List *list, char* file_name, int id)
{
	Client *client = searchClientById(list, id);

	ClientFile *new = (ClientFile*)calloc(1, sizeof(ClientFile));
	new->name = file_name;
	new->next = client->data;

	client->data = new; //Adiciona na primeira posição da lista
	
}

//Recebe o nome de arquivo, id do client que vai ter o arquivo apagado e a lista para procurar o cliente e apagar
void deleteFile(char *file_name, char * dir)
{
	char *path = calloc(1, (strlen(file_name)+ strlen(dir) + 2));

	//Pegar o caminho do arquivo
	strcat(path, dir);
	strcat(path, file_name);

	printf("path = %s\n", path);

	if(remove(path) == 0)
		printf("Arquivo deletado com sucesso!\n");
	else
		printf("Erro ao deletar arquivo!\n");

}

//Remove o arquivo da lista do servidor
int removeFileFromList(char *file_name, List *list)
{

	Client* client = list->first;
	ClientFile* current;

	//Percorrer os clientes
	while(client != NULL)
	{
		current = client->data;
		ClientFile *before = NULL;
		//Percorre os arquivos do cliente, caso encontre coloca finish para 1 e termina
		while(current != NULL)
		{
			if(strcmp(current->name, file_name) == 0)
			{
				//Verifica se foi era o primeiro da lista
				if(before == NULL)
				{
					//Se so existe 1 na lista
					if(current->next == NULL)
					{
						client->data = NULL;

						return 1;
					}else
					{
						client->data = current->next;

						return 1;
					}

				}else if(current->next == NULL) //Se for o ultimo da lista
				{
					before->next = NULL;

					return 1;
				}else
				{
					//Se nao entrou em nenhum dos anteriores, esta no meio da lista
					before->next = current->next;

					return 1;
				}
			}
			before = current;
			current = current->next;
		}
		client = client->next;
	}

	return 0;
}

//Le e escreve no buffer o conteudo da linha do terminal
char *getTerminalCommand()
{
	size_t buffer_size = 512;

	char *buffer = (char*)calloc(buffer_size, sizeof(char));

	int bytes_read = getline(&buffer, &buffer_size, stdin);

	if(bytes_read == -1)
	{
		printf("Erro ao ler o comando do terminal\n");
		return NULL;
	}

	return buffer;
}