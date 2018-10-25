#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <signal.h>
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h>

#define MAXDATASIZE 		300
#define ARRAY_SIZE 			10
#define NUM_TILES_X 		9
#define NUM_TILES_Y 		9
#define NUM_MINES 			10
#define BUFFER				100

typedef struct{
	int adjacent_mines;
	bool revealed;
	bool is_flag;
} Tile;

//Client authentication variables
char username[30];
char password[30];
void Menu(void);

int minesRemaining = sizeof(int);
void initialiseGame(void);
void drawGame(void);
void GameSelection(void);
void Reveal(char* tile);
void Place(char* tile);
char coord[NUM_TILES_Y] = {'A','B','C','D','E','F','G','H','I'};
Tile game[NUM_TILES_X][NUM_TILES_Y];
bool playing = false;

int sockfd;  
char buf[MAXDATASIZE];
struct hostent *he;
struct sockaddr_in host_addr;
void Send(int socket_id, int *myArray);
void Receive(int socket_identifier, int size);
void Authentication(int socket_id, char *var);
char *input;