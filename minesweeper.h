#include <arpa/inet.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <unistd.h>
#include <errno.h>

 
 /* Setting up GameArray */

#define DATASIZE 100
#define ARRAY_SIZE 81
#define RETURN_ERROR -1
#define NUM_TILES_X 9
#define NUM_TILES_Y 9
#define NUM_MINES 10 

typedef struct 
{
	int adjacent_mines;
	bool revealed;
	bool is_mine;
}Tile;

typedef struct 
{
	Tile tiles[NUM_TILES_X][NUM_TILES_Y];
}GameState;