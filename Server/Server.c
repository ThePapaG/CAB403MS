//CAB403 Assignment 2018 SEM2 - Server
//Written by Callum Scott n9695443
//in conjunction with Grant Dare n9476512

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
	if(auth != 1){
		fprintf(stderr, "User is not authenticated! exiting...\n");
		send(sock, &auth, sizeof(int), 0);
		pthread_exit(NULL);
	}
	else{
		send(sock, &auth, sizeof(int), 0);
	}
	
	GameState client_game;
	while(1){
		//user is connected and starting a game
		//wait for user main menu option where 1-3 is play, leaderboard and quit
		int selection = Receive(sock, 1);
		switch(selection) {
			case 1:
				client_game = initialiseGame();
			break;

			case 2:
				//show leaderboard
			break;

			default:
			break;
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
					game.tile[d][]
				}
			}
		}
	}


}

bool tile_contains_mine(Tile tile){
	return tile.is_mine == true;
}

int GetAUTH(int socket_id){
	char auth[100] = "";
	recv(socket_id, &auth, 100, 0);

	FILE *fp;
	char buf[100];
	fp = fopen("Authentication.txt", "r");
	if (fp == NULL) {
		puts("Unable to read Auth file");
		exit(0);
	}
	while (fgets(buf,100, fp)!=NULL){
		char user[100] = {0};
		int d = 0;
		for(int i = 0; i<strlen(buf); i++){
			if(!(isspace(buf[i]))){
				user[d] = buf[i];
				d++;
			}
		}
		if(strcmp(user, auth) == 0){
			printf("yes\n");
			return 1;
		}
	}
	return 0;
}

void Send(int socket_id, int *myArray) {
	int i=0;
	uint16_t statistics;  
	for (i = 0; i < ARRAY_SIZE; i++) {
		statistics = htons(myArray[i]);
		send(socket_id, &statistics, sizeof(uint16_t), 0);
	}
}

int Receive(int socket_identifier, int size) {
    int number_of_bytes, i=0;
    uint16_t statistics;

    int *input;
	input = malloc(sizeof(int)*ARRAY_SIZE);

	for (i=0; i < size; i++) {
		if ((number_of_bytes=recv(socket_identifier, &statistics, sizeof(uint16_t), 0))
		         == -1) {
			perror("recv");
			exit(EXIT_FAILURE);			
		    
		}
		input[i] = ntohs(statistics);
	}
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
