all: client server

client: client.o
	gcc -o client client.o biblioteca.o -lpthread

client.o: client.c biblioteca.c
	gcc -c -g client.c biblioteca.c -lpthread

server: server.o
	gcc -o server server.o biblioteca.o -lpthread

server.o: server.c biblioteca.c
	gcc -c -g server.c biblioteca.c -lpthread

clean: 
	rm *.o