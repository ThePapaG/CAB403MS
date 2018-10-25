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

int PORT;

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
