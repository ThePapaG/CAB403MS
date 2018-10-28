#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <errno.h> 
#include <string.h> 
#include <signal.h>
#include <time.h>
#include <unistd.h> //Write
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/wait.h>
#include <pthread.h>
#include <ctype.h>


/* ---- Defines ---- */
#define PORT_LENGTH         6
#define NO_FLAGS            0
#define BACKLOG             10
#define BUF_SIZE            256
#define DISCONNECT_SIGNAL   "QUIT"
#define PLAY_MINESWEEPER    1
#define SHOW_LEADERBOARD    2
#define QUIT                3
#define USERNAME_LENGTH     10
#define PASSWORD_LENGTH     10
#define MAX_CLIENTS         10
#define NUM_TILES_X 		9
#define NUM_TILES_Y 		9
#define NUM_MINES 			10
#define RANDOM_NUMBER_SEED	42
#define MAXDATASIZE 		300
#define ARRAY_SIZE 			10

typedef struct{
	pthread_t thread;
	int* sock_id;
	int counter;
}	Client;

typedef struct{
	int adjacent_mines;
	bool revealed;
	bool is_flag;
	bool is_mine;
} Tile;

typedef struct{
	Tile tile[NUM_TILES_X][NUM_TILES_Y];
} GameState;

int PORT;
Client CLIENTS[MAX_CLIENTS];
int client_list;

/* Variables */
int sockfd, con_fd;  /* listen on sock_fd, new connection on con_fd */
char *message;
struct sockaddr_in my_addr;    /* host address information */
struct sockaddr_in their_addr; /* connector's address information */
socklen_t sin_size;

int CreateSocket(void);
void GenerateEP(void);
void BindListen(int sockfd);
void* ClientGame(void *arg);
int GetAUTH(int socket_id);
void Send(int socket_id, char *output);
char * Receive(int socket_identifier, int size);

GameState initialiseGame(void);
bool tile_contains_mine(Tile tile);
int reveal_tile(Tile tile);