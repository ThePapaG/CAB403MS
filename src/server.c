//CAB403 Assignment 2018 SEM2 - Server
//Written by Grant Dare n9476512

/* Include */

#include "server.h"

void handle_sigint(int sig) { 
    printf("Caught signal %d\n", sig);
    printf("Exiting has begun.");
    close(sockfd);
    exit(1);
} 

/*
Server setup
*/
int CreateSocket(){
	/* Create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Check for error */
	if (sockfd==-1)
	{
		perror("error socket");
		exit(1);
	}
	return sockfd;
}

void GenrateEP(){
	/* Generate end point */	
	my_addr.sin_family = AF_INET;         /* host byte order */
	my_addr.sin_port = htons(PORT);     /* short, network byte order */
	my_addr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */
}

void BindListen(int sockfd){
	/* Bind socket to end point */
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) \
	== -1) {
		perror("error bind");
		exit(1);
	}

	/* Start listnening */
	if (listen(sockfd, BACKLOG) == -1) {
		perror("error listen");
		exit(1);
	}

	printf("server starts listnening ...\n");
}

/*
Client functions
*/
void* ClientGame(void *arg){
	//client vars
	int selection;
	bool playing;
	Client *client;

	client = (Client *) clients;

	client->authenticated = false;
	client->sock = 0;

	// if server dies this dies
	while(ok){
		sem_wait(&sem_clients);

		if(!ok){	//server died, uhoh
			break;
		}

		client->sock = getClient();

		//check user authentication
		printf("Prompting user for authentication\n");
		Send(client->sock, WELCOME_MESSAGE);
		int authorisation;
		if(authorisation = getAuth(client) < 1){
			//if auth returns 0 or -1
			if(authorisation == -1){
				break;
			}
			printf("User is not authenticated. Exiting...\n");
			break;
		}
		else{
			printf("User, %s, authenticated!\n", client->user);
			client->authenticated = true;
		}

		while(client->authenticated){
			//Sending main menu to user
			printf("Sending the user the menu prompt\n");
			Send(client->sock, MAIN_MENU);

			Send(client->sock, MENU_PROMPT);
			selection = getMenuSelection(client);
			if(selection == -1){
				break;
			}

			//check the server status before we do stuff
			if(!ok){
				break;
			}

			switch(selection){
				case PLAY_MINESWEEPER:
					printf("User selected to play!\n");
					playMinesweeper(client);
					break;

				case SHOW_LEADERBOARD:
					printf("User selected to show leaderboard\n");
					break;

				case QUIT:
					printf("User selected to quit\n");
					break;
			}
		}
		send(client->sock, DISCONNECT_SIGNAL, BUF_SIZE,0);
    	close(client->sock);
    	memset(client, 0, sizeof(Client));
		sem_post(&client_handler);
	}
	return NULL;
}

void addClient(int sock_id){
	pthread_mutex_lock(&queue_mutex);
	printf("next_queue_pos: %d\n", queue_pos);
	sock_fds[queue_pos] = sock_id;
	queue_pos++;
	queue_pos = queue_pos % MAX_CLIENTS;
	pthread_mutex_unlock(&queue_mutex);
}

int getClient(void){
	pthread_mutex_lock(&next_client_mutex);
    printf("cnext_client: %d\n", next_client);
    int client = sock_fds[next_client];
    next_client++;
    next_client = next_client % MAX_CLIENTS;
    pthread_mutex_unlock(&next_client_mutex);

    return client;
}

int getAuth(Client *client){
	printf("Sending username prompt...\n");
	Send(client->sock, USERNAME_PROMPT);
	printf("Waiting for username\n");
	if(Rec(client->sock, client->user) == -1){
		return -1;
	}

	printf("Sending password prompt...\n");
	Send(client->sock, PASSWORD_PROMPT);
	if(Rec(client->sock, client->pass) == -1){
		return -1;
	}

	if(checkAuth(client)){
		return 1;
	}

	return 0;

}

bool checkAuth(Client *client){
	FILE *file;
    char username[AUTH_LENGTH];
    char password[AUTH_LENGTH];
    bool match = false;

   if ((file = fopen("bin/Authentication.txt", "r")) == NULL) {
        perror("fopen");
        return false;
    }

    while (fgetc(file) != '\n');

    /* Each line in the file contains the username and password separated by a space. */
    while (fscanf(file, "%s %s\n", username, password) > 0) {
        /* Check if the username matches one in the file, and if the password matches for that username. */
        if (strcmp(username, client->user) == 0 && strcmp(password, client->pass) == 0) {
            match = true;
            break;
        }
    }

    fclose(file);
    return match;
}

int getMenuSelection(Client *client){
	char selection_str[BUF_SIZE];

    printf("Waiting for user option\n");
    if (Rec(client->sock, selection_str) == -1) {
        return -1;
    }
    printf("User selection received\n");

	//all menu's take a 1-3 input, this is generic and different prompt will be sent pre call.
    return atoi(selection_str); 
}

/*
Game functions
*/

bool playMinesweeper(Client *client){
	GameState game;
	char selection;
	char playboard[BUF_SIZE];
	char game_message[BUF_SIZE];
	bool alive = true;
	
	//init the game
	game.minesRemaining = NUM_MINES;
	initGame(&game);
	
	//game still active
	while(alive){
		memset(playboard, 0, sizeof(playboard));
		if(game.minesRemaining == 0){
			sprintf(game_message, "\n\nCongratulations, %s!\nYou're a winner! You have placed flags on all of the mines in the game.");
			Send(client->sock, game_message);
			return true;
		}

		printf("Drawing game for the user\n");
		drawGame(&game, playboard);
		printf("Sending the playboard to the user\n");
		Send(client->sock, playboard);

		printf("Sending user game menu\n");
		Send(client->sock, GAME_MENU);

		printf("Sending user game prompt\n");
		Send(client->sock, GAME_PROMPT);
		selection = getGameSelection(client);
		if(selection == -1){
			break;
		}

		switch(selection){
			case 'R':
			case 'r':
				alive = revealTile(client, &game);
				break;

			case 'P':
			case 'p':
				break;

			case 'Q':
			case 'q':
				break;
		}

		if(!alive){
			sprintf(game_message, "\n\nUhoh!\nYou have revealed a mine. That's game over for you buddy!\n");
		}
	}
}

void initGame(GameState *game){
	for(int y = 0; y<NUM_TILES_Y; y++){
		for(int x = 0; x<NUM_TILES_X; x++){
			game->tile[x][y].adjacent_mines = 0;
			game->tile[x][y].revealed = false;
			game->tile[x][y].is_flag = false;
			game->tile[x][y].is_mine = false;
		}
	}

	//place mines
	for(int i = 0; i< NUM_MINES; i++){
		int x, y;
		do{
			x = rand() % NUM_TILES_X;
			y = rand() % NUM_TILES_Y;
		} while (game->tile[x][y].is_mine);

		game->tile[x][y].is_mine = true;
		game->tile[x][y].adjacent_mines = -1;
		//increment values around mine
		for(int w = x-1; w<x+1; w++){
			for(int h = y-1; h<y+1; h++){
				if(!(game->tile[w][h].is_mine)){
					game->tile[w][h].adjacent_mines++;
				}
			}
		}
	}
}

void drawGame(GameState *game, char *board){
	char val;
	strcat(board, GAME_HEADER);
	for(int y = 0; y<NUM_TILES_Y; y++){
		strcat(board, yCoord[y]);
		strcat(board, "  |\t");
		for(int x = 0; x<NUM_TILES_X; x++){
			if(game->tile[x][y].revealed){
				val = game->tile[x][y].adjacent_mines + '0';
				strcat(board, &val);
			}else if(game->tile[x][y].is_flag){
				strcat(board, "+");
			}
			strcat(board, "\t");
		}
		strcat(board, "\n\n");
	}
}

char getGameSelection(Client *client){
	char selection_str[BUF_SIZE];

    printf("Waiting for user option\n");
    if (Rec(client->sock, selection_str) == -1) {
        return -1;
    }
    printf("User selection received\n");

    return selection_str[0]; 
}

void getTile(Client *client, int *tile){
	char selection_str[BUF_SIZE];

	Send(client->sock, REVEAL_PROMPT);
    printf("Waiting for user option\n");
    Rec(client->sock, selection_str);
    printf("User selection received\n");

	tile[1] = atoi(&selection_str[1]) - 1;
	for(int i = 0; i<NUM_TILES_Y; i++){
		if(strcmp(yCoord[i], &selection_str[0])){
			tile[0] = i;
			break;
		}
	}
	printf("%d\n", tile[0]);
}

bool revealTile(Client *client, GameState *game){
	int userTile[2];
	int x, y;
	getTile(client, userTile);

	x = userTile[0];
	y = userTile[1];
	if(game->tile[x][y].revealed){
		printf("Tile, %d%d, is already revealed.", x, y);
		Send(client->sock, TILE_REVEALED);
		return true;
	}else if(game->tile[x][y].is_mine){
		return false;
	}else{
		printf("Tile, %d%d, is now revealed.",x,y);
		game->tile[x][y].revealed = true;
	}

	return true;
}

bool placeTile(Client *client, GameState *game){
	int userTile[2];
	int x, y;
	getTile(client, userTile);

	x = userTile[0];
	y = userTile[1];
	if((game->tile[x][y].revealed)){
		printf("Tile, %d%d, is already revealed.", x, y);
		Send(client->sock, TILE_REVEALED);
		return true;
	}else if(game->tile[x][y].is_mine){
		game->minesRemaining--;
		game->tile[x][y].is_flag = true;
		game->tile[x][y].revealed = true;
	}

	return true;
}

/*
Main
*/
int main(int argc, char const *argv[]){
	if(argc!=2){
		fprintf(stderr,"usage: port_number. Default 12345 will be used\n");
		PORT = 12345;
	}
	else{
		PORT = atoi(argv[1]);
	}

	//Seed random number gen
	srand(RANDOM_NUMBER_SEED);

	//setup server socket
	signal(SIGINT, handle_sigint); 
	sockfd=CreateSocket();
	bzero(&my_addr, sizeof(my_addr));
	GenrateEP();
	BindListen(sockfd);

	//setup critical section solution
	if (sem_init(&client_handler, 0, MAX_CLIENTS) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

	if (sem_init(&sem_clients, 0, 0) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

	//setup mutex var for threadpool client handling
	pthread_mutex_init(&queue_mutex, NULL); 
    pthread_mutex_init(&next_client_mutex, NULL); 

	//setup threadpool for client handling
	for (int i = 0; i < MAX_CLIENTS; i++) {
        if (pthread_create(&threads[i], NULL, ClientGame, (void *) &clients[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

	//Assume evewrything went ok up to now, so ok...
	ok = true;

	while(ok){
		sem_wait(&client_handler);
		int new_sock_fd;

		//get a client connection
		printf("Waiting for a client\n");
		new_sock_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_sock_fd == -1) {
            perror("accept");
            continue;
        }
		printf("Client connected. Pending authorisation\n");
		
		addClient(new_sock_fd);

		sem_post(&sem_clients);
	}

	return 0;
}

void Send(int sock_id, const char *out){
    if (send(sock_id, out, BUF_SIZE, 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }
}

int Rec(int sock_id, char *rec){
    if (recv(sock_id, rec, BUF_SIZE, 0) == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    if (strcmp(rec, DISCONNECT_SIGNAL) == 0) {
        printf("\nReceived disconnect signal from client on socket %d.\n", sock_id);
        return -1;
    }

    return 0;
}
