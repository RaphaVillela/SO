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
	unsigned int ip;
	ClientFile *data; //Cabeca dos arquivos do cliente
	struct Client_t *next; //Aponta para o proximo
	
}Client;

//Lista que guarda o primeiro e o ultimo cliente
typedef struct
{
	Client *first; //Guarda o primeiro cliente da lista
	Client *last; //Guarda o ultimo cliente da lista	

}List;

//Libera o cliente da memoria
void freeClient(Client* client); //Deleta o cliente da memoria

List* createList(); //Inicia os ponteiros do primeiro e ultimo clientes para NULL
void addClient(List *list, Client *client, int id); //Adiciona um cliente na lista
Client* selectClient(int id, List *list); //Procura um cliente pelo id
int deleteClient(int id, List *list); //Deleta um cliente da lista

Client* searchClientByFile(List* list, char* fileName); //Procura um cliente pelo nome do arquivo

//Enviar e receber os arquivos do cliente
void sendClientFiles(char* diretorio, int socket);
ClientFile* recvClientFiles(int socket);

//Enviar e receber a porta do cliente
void sendClientPort(unsigned int port, int socket);
unsigned int recvClientPort(int socket);

//Enviar e receber o ip do cliente
void sendClientIP(unsigned int ip, int socket);
unsigned int recvClientIP(int socket);