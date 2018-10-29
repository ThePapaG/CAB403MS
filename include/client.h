#include <errno.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h>  
#include <strings.h>

#define BUF_SIZE            256
#define DISCONNECT_SIGNAL   "QUIT"

static bool cont = true;

static void *Send(void *data);
static void *Rec(void *data);
int getSelection(char *msg, char *input_str);

int sockfd;
struct hostent *he;
struct sockaddr_in host_addr;
bool rec;
pthread_t   readth, writeth;