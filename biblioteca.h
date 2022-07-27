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
typedef struct ClientList_t
{
	int id; //Guarda o id do cliente
	int nFiles; //Guarda a quantidade de arquivos contidos no cliente
	unsigned int porta; //Guarda a porta que o cliente esta conectado
	unsigned int ip;
	ClientFile *data; //Cabeca dos arquivos do cliente
	struct ClientList_t *next; //Aponta para o proximo
	
}ClientList;

//Lista que guarda o primeiro e o ultimo cliente
typedef struct
{
	ClientList *first; //Guarda o primeiro cliente da lista
	ClientList *last; //Guarda o ultimo cliente da lista
	
}Edges;

//Libera o cliente da memoria
void freeClient(ClientList* client); //Deleta o cliente da memoria

Edges* createList(); //Inicia os ponteiros do primeiro e ultimo clientes para NULL
void addClient(Edges *edge, ClientList *client, int id); //Adiciona um cliente na lista
ClientList* selectClient(int id, Edges *edges); //Procura um cliente pelo id
int deleteClient(int id, Edges *edges); //Deleta um cliente da lista

ClientList* searchClientByFile(Edges* edges, char* fileName); //Procura um cliente pelo nome do arquivo

//Envia os arquivos do cliente
void sendClientFiles(char* diretorio, int socket);
//Recebe os arquivos do clientes
ClientFile* recvClientFiles(int socket);
