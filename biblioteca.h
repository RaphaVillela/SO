#define COMMAND_EXIT 0
#define COMMAND_GET 1
#define COMMAND_SEND 2
#define COMMAND_DELETE 3
#define COMMAND_LIST 4
#define COMMAND_STATS 5
#define COMMAND_CLIENT 6
#define DELETE_LIST 7
#define ADD_LIST 8
#define MAX_LENGTH 6
#define BLOCK_SIZE 16 

//Enviar e receber mensagem de inteiros
void sendInt( int number, int socket );
int recvInt( int socket );

//Enviar e receber mensagem de double
void sendDouble( double number, int socket );
double recvDouble( int socket );

//Enviar e receber mensagem de string
void sendString( char* string, int socket );
char* recvString( int socket );

//Doubles aleatórios
double randomDouble(double min, double max);

//Arquivos de cada cliente
typedef struct ClientFile_t
{
	char* name; //Nome do arquivo
	struct ClientFile_t *next; //Aponta para o proximo

}ClientFile;

//Lista de clientes
typedef struct Client_t
{
	int id; //Guarda o id do cliente
	int nFiles; //Guarda a quantidade de arquivos contidos no cliente
	unsigned int porta; //Guarda a porta que o cliente esta conectado
	unsigned int ip; //Guarda o IP do cliente
	ClientFile *data; //Cabeca dos arquivos do cliente
	struct Client_t *next; //Aponta para o proximo
	
}Client;

//Lista que guarda o primeiro e o ultimo cliente
typedef struct
{
	Client *first; //Guarda o primeiro cliente da lista
	Client *last; //Guarda o ultimo cliente da lista	

}List;

//Enviar e receber os clientes
void sendClient(Client *client, int socket);
Client* recvClient(int socket);

//Enviar e receber arquivos
void sendFile(char* path, int socket);
void recvFile(char *path, int socket);

//Libera o cliente da memoria
void freeClient(Client* client); //Deleta o cliente da memoria

List* createList(); //Inicia os ponteiros do primeiro e ultimo clientes para NULL
void addClient(List *list, Client *client); //Adiciona um cliente na lista
Client* selectClient(int id, List *list); //Procura um cliente pelo id
int deleteClient(int id, List *list); //Deleta um cliente da lista

Client* searchClientByFile(List* list, char* fileName); //Procura um cliente pelo nome do arquivo
Client* searchClientById(List* list, int id); //Procura um cliente pelo seu id

//Enviar e receber os arquivos do cliente
void sendClientFiles(ClientFile* cf, int socket);
ClientFile* recvClientFiles(int socket);

//Conta quantos arquivos tem no diretorio e adiciona os arquivos no cliente
int countFiles(char* diretorio, Client *novo);

//Adiciona o nome do arquivo na lista do servidor
void addFile(List *list, char* file_name, int id);

//Deleta o arquivo
void deleteFile(char *file_name, char * dir);

//Remove o arquivo do ponteiro
int removeFileFromList(char *file_name, List *list);

//Pega a linha do terminal
char *getTerminalCommand();