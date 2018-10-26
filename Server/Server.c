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

void* ClientGame(void *arg){
	int sock = *(int*)arg;

	//Check auth and kick user if not authenticated
	int auth = GetAUTH(sock);
	//Send(sock, &auth);
	if(auth != 1){
		fprintf(stderr, "User is not authenticated! exiting...\n");
		pthread_exit(NULL);
	}
	
	GameState client_game;
	while(1){
		//user is connected and starting a game
		//wait for user main menu option where 1-3 is play, leaderboard and quit
		bool playing = false;
		char selection[100];
		recv(sock, &selection, 100, 0);
		if(*selection == 'p'){
			client_game = initialiseGame();
			playing = true;
		}

		else if(*selection == 'l'){

		}

		while(playing){
			int x = 0;
			int y = 0;
			int result;
			int game_choice = 0;
			recv(sock, &game_choice, sizeof(int), 0);
			printf("%d\n", game_choice);
			switch(game_choice){
				case 1:	//reveal
					//get the tile from user [x][y]
					result = reveal_tile(client_game.tile[x][y]);
					switch(result){
						case 1:
							printf("woah\n");
							while(1);
							//revealed already, send value to tell user to try again. send a -1
							break;

						case 0:
							//not revealed
							client_game.tile[x][y].revealed = true;
							//send the client the adjacent tiles value
							//TODO calculate all 0's and send an array of tiles with 0. perhaps send the whole gamestate
							break;

						case -1:
							//tile is a mine, game over
							break;
					}
				break;
				case 2:
					result = reveal_tile(client_game.tile[x][y]);
					switch(result){
						case 1:
							//revealed already, user can't place a flag on revealed tile
						break;

						case 0:
							//not revealed but is not a mine
						break;

						case -1:
							//tile is a mine, set the flag and send to the user the amount of mines left.
						break;
					}
				break;
				default:
					//kill the game
				break;
			}
		}	
	}
}

GameState initialiseGame(void){
	GameState game;
	// init everything to 0
	for(int i = 0; i < NUM_TILES_Y; i++){
		for(int d = 0; d < NUM_TILES_X; d++){
			//starting state is everything is no flag and not revealed
			game.tile[d][i].is_flag = false;
			game.tile[d][i].revealed = false;
			game.tile[d][i].adjacent_mines = 0;
			game.tile[d][i].is_mine = false;
		}
	}
	//place mines
	for(int i = 0; i<NUM_MINES; i++){
		int x,y;
		do{
			x = rand() % NUM_TILES_X;
			y = rand() % NUM_TILES_Y;
		} while (tile_contains_mine(game.tile[x][y]));
		game.tile[x][y].is_mine = true;
		for(int d = x-1; d <= x+1; d++){
			for(int j = y-1; j <= y+1; j++){
				if(d!=x && j!=y){
					game.tile[d][i].adjacent_mines++;
				}
			}
		}
	}

	return game;
}

bool tile_contains_mine(Tile tile){

	return tile.is_mine == true;
}

int GetAUTH(int socket_id){
	char *auth = Receive(socket_id, 20);
	printf("%s\n", auth);

	//open auth file
	FILE *fp;
	char buf[100];
	fp = fopen("Authentication.txt", "r");
	if (fp == NULL) {
		puts("Unable to read Auth file");
		exit(0);
	}

	//loop and look for matching auth. NOTE: users are USERNAMEPASSWORD
	int ret_val = 0;
	while (fgets(buf,100, fp)!=NULL){
		char user[100] = {0};
		int d = 0;

		//remove spaces
		for(int i = 0; i<strlen(buf); i++){
			if(!(isspace(buf[i]))){
				user[d] = buf[i];
				d++;
			}
		}

		//check auth
		if(strcmp(user, auth) == 0){
			printf("\nUser %s successfully authenticated\n", auth);
			ret_val = 1;
			break;
		}
	}

	//return
	return ret_val;
}

int reveal_tile(Tile tile){
	if(tile_contains_mine(tile)){
		return -1;
	}

	return tile.revealed ? 1 : 0;
}

void Send(int socket_id, char *output) {
	int i=0;  
	for (i = 0; i < strlen(output); i++) {
		send(socket_id, &output[i], sizeof(uint16_t), 0);	//char is already byte representation
	}
}

char * Receive(int socket_identifier, int size) {
    int number_of_bytes, i=0;
    char rec;

    char *buff = malloc(size);

	for (i=0; i < size; i++) {
		if ((number_of_bytes=recv(socket_identifier, &rec, 1, 0))
		         == -1) {
			perror("recv");
			exit(EXIT_FAILURE);		
		    
		}
		if(rec == '\0'){
			break;
		}
		buff[i] = rec;
	}
	return buff;
}

int main(int argc, char const *argv[]){
	if(argc!=2){
		fprintf(stderr,"usage: port_number. Default 12345 will be used\n");
		PORT = 12345;
	}
	else{
		PORT = atoi(argv[1]);
	}

	srand(RANDOM_NUMBER_SEED);

	signal(SIGINT, handle_sigint); 
	sockfd=CreateSocket();
	bzero(&my_addr, sizeof(my_addr));
	GenrateEP();
	BindListen(sockfd);

	client_list = 0;

	while(1) {  /* main accept() loop */

		int client_sock;
		int *new_sock;
		sin_size = sizeof(struct sockaddr_in);
	    while((con_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size))){
	    	pthread_t client_thread;
	        new_sock = malloc(sizeof *new_sock);
	        *new_sock = con_fd;
			pthread_create( &client_thread , NULL , ClientGame, (void*) new_sock);

			CLIENTS[client_list].thread = client_thread;
			CLIENTS[client_list].sock_id = new_sock;
			CLIENTS[client_list].counter = client_list;
			client_list++;
    	}

    	if(con_fd < 0){
    		perror("Accept Failed");
    		return 1;
    	}
    }
	return 0;
}
