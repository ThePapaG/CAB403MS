#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <errno.h> 
#include <string.h> 
#include <strings.h>
#include <signal.h>
#include <time.h>
#include <unistd.h> //Write
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <ctype.h>


/* ---- Defines ---- */
#define BUF_SIZE            256
#define DISCONNECT_SIGNAL   "QUIT"
#define BACKLOG      10
#define PLAY_MINESWEEPER    1
#define SHOW_LEADERBOARD    2
#define QUIT                3
#define AUTH_LENGTH			10
#define MAX_CLIENTS         10
#define NUM_TILES_X 		9
#define NUM_TILES_Y 		9
#define NUM_MINES 			10
#define RANDOM_NUMBER_SEED	42

/* ---- Menu Graphics ---- */
const char WELCOME_MESSAGE[] = "\n"
                              "================================================\n"
                              "\n"
                              " Welcome to the online Minesweeper gaming system\n"
                              "\n"
                              "================================================\n"
                              "\n"
							  "You are required to logon with your username and password:\n";

const char USERNAME_PROMPT[] = "\nUsername: ";
const char PASSWORD_PROMPT[] = "\nPassword: ";

const char AUTH_FAILED[] = "\nYou entered either an incorrect username or password - disconnecting...\n";

const char MAIN_MENU[] = "\n"
                        "Welcome to the Minesweeper game system\n"
                        "\n"
						"Please enter a selection: \n"
                        "<1> Play MineSweeper\n"
                        "<2> Show Leaderboard\n"
                        "<3> Quit\n";

const char MENU_PROMPT[] = "\nSelection option (1 - 3): ";

const char GAME_MENU[] =    "\n"
                            "Choose an option:\n"
                            "<R> Reveal tile\n"
                            "<P> Place flag\n"
                            "<Q> Quit game\n"
                            "\n";

const char GAME_PROMPT[] =  "\nOption (R,P,Q): ";

const char REVEAL_PROMPT[] = "\nReveal tile (eg. A1): ";

const char NO_LEADERBOARD[] = "\n==============================================================================\n"
                              "\nThere is no information currently stored in the Leader Board. Try again later.\n"
                              "\n==============================================================================\n";

const char GAME_HEADER[] =  "\n\t1\t2\t3\t4\t5\t6\t7\t8\t9\n"
                            "\t_\t_\t_\t_\t_\t_\t_\t_\t_\n";

const char TILE_REVEALED[] = "\nThat tile has already been revealed!\n";

const char NOT_MINE[] = "\nOof, that tile isn't a mine!\n";

typedef struct {
    int         sock;
    bool        authenticated;
    char        user[AUTH_LENGTH];
    char        pass[AUTH_LENGTH];
} Client;

typedef struct{
	int adjacent_mines;
	bool revealed;
	bool is_flag;
	bool is_mine;
} Tile;

typedef struct{
	Tile tile[NUM_TILES_X][NUM_TILES_Y];
	int minesRemaining;
} GameState;

int PORT;
bool ok;
const char *yCoord[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I"};

/* Variables for connection */
int sockfd;  /* listen on sock_fd*/
struct sockaddr_in my_addr;    /* host address information */
struct sockaddr_in their_addr; /* connector's address information */
socklen_t sin_size;

/* Variables for threadpool/critical */
static Client clients[MAX_CLIENTS]; 
static pthread_t threads[MAX_CLIENTS];       
static int sock_fds[MAX_CLIENTS];
static sem_t client_handler;
static sem_t sem_clients;
static int              queue_pos = 0; 
static pthread_mutex_t  queue_mutex;
static int              next_client = 0;
static pthread_mutex_t  next_client_mutex;

/* Function def */
int CreateSocket(void);
void GenerateEP(void);
void BindListen(int sockfd);
void* ClientGame(void *arg);
void addClient(int sock_id);
int getClient(void);
int getAuth(Client *client);
bool checkAuth(Client *client);
int getMenuSelection(Client *client);
bool playMinesweeper(Client *client);
void initGame(GameState *game);
void drawGame(GameState *game, char *board);
char getGameSelection(Client *client);
void getTile(Client *client, int *tile);
bool revealTile(Client *client, GameState *game);
bool placeTile(Client *client, GameState *game);
void Send(int sock_id, const char *out);
int Rec(int sock_id, char *rec);