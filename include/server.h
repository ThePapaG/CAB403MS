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
#define NUM_MINES 			1
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

const char PLACE_PROMPT[] = "\nPlace a flag (eg. A1): ";

const char EMPTY_LEADERBOARD[] = "\n==============================================================================\n"
                              "\nThere is no information currently stored in the Leader Board. Try again later.\n"
                              "\n==============================================================================\n";

const char GAME_HEADER[] =  "\n\t1\t2\t3\t4\t5\t6\t7\t8\t9\n"
                            "\t_\t_\t_\t_\t_\t_\t_\t_\t_\n";

const char TILE_REVEALED[] = "\nThat tile has already been revealed!\n";

const char NOT_MINE[] = "\nOof, that tile isn't a mine!\n";

typedef struct ClientEntry{
 	double time_elapsed;
    char user[AUTH_LENGTH];
 	struct ClientEntry *nextClientEntry;
} ClientEntry;

typedef struct Client{
    int sock;
    bool authenticated;
    char user[AUTH_LENGTH];
    char pass[AUTH_LENGTH];
 	int games_played;
 	int games_won;
    struct ClientEntry *firstEntry;
    struct ClientEntry *lastEntry;
 	int entries;
} Client;

typedef struct Tile{
	int adjacent_mines;
	bool revealed;
	bool is_flag;
	bool is_mine;
} Tile;

typedef struct GameState{
	struct Tile tile[NUM_TILES_X][NUM_TILES_Y];
	int minesRemaining;
    time_t start;
} GameState;

 typedef struct LeaderboardEntry{
    struct ClientEntry *current;
 	struct LeaderboardEntry *next;
 } LeaderboardEntry;

 typedef struct Leaderboard{
 	struct LeaderboardEntry *firstEntry;
    struct LeaderboardEntry *lastEntry;
    int entries;
 } Leaderboard;

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
static int              queue_pos = 0; 
static pthread_mutex_t  queue_mutex;
static int              next_client = 0;
static pthread_mutex_t  next_client_mutex;
static pthread_cond_t  new_client   = PTHREAD_COND_INITIALIZER;
static int rc; 

/* For leaderboard locking*/
static pthread_mutex_t  leaderboard_mut;
Leaderboard *leaderboard; 

/* Function def */
void killServer(int sig);
int CreateSocket(void);
void GenerateEP(void);
void BindListen(int sockfd);

void* ClientGame(void *arg);
void addClient(int sock_id);
int getClient(void);
void killClient(Client *client);
int getAuth(Client *client);
bool checkAuth(Client *client);
int getMenuSelection(Client *client);

bool playMinesweeper(Client *client);
void initGame(GameState *game);
void drawGame(GameState *game, char *board, bool alive);
char getGameSelection(Client *client);
void getTile(Client *client, int *tile);
bool revealTile(Client *client, GameState *game);
bool placeTile(Client *client, GameState *game);
bool isZero(GameState *game, int x, int y);

void showLeaderboard(Client *client);
Leaderboard* initLeaderboard(void);
void drawLeaderboard(char *str);
void sortClientEntries(void);
void addLeaderEntry(ClientEntry *point);
void getClientEntry(char *str, Client *client);
void addEntry(Client *client, double time);
void swapEntries(LeaderboardEntry *entry1, LeaderboardEntry *entry2);
void bubbleboi(void);
void equalCheck(LeaderboardEntry *entry1, LeaderboardEntry *entry2);
Client *getClientInfo(LeaderboardEntry *entry);

void Send(int sock_id, const char *out);
int Rec(int sock_id, char *rec);