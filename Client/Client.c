//CAB403 Assignment 2018 Sem2 - Client main
//Written by Grant Dare n9476512

#include "Client.h"

void siganlHandler(int sig){
	printf("caught signal");
	close(sockfd);
	exit(sig);
}

int main(int argc, char* argv[]){
	signal(SIGINT, siganlHandler);	
	//process the input arguments
	if (argc != 3) {
		fprintf(stderr,"usage: client_hostname port_number\n");
		exit(1);
	}

	printf("Connecting to the server...\n");
	//Connect to the server
	if ((he=gethostbyname(argv[1])) == NULL) {
		herror("gethostbyname");
		exit(1);
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(atoi(argv[2]));
	host_addr.sin_addr = *((struct in_addr *)he->h_addr);
	bzero(&(host_addr.sin_zero), 8);
	
	if (connect(sockfd, (struct sockaddr *)&host_addr, \
	sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1);
	}

	printf("Server connected!\n");

	initUser();

	while(1){
		//show the main menu
		Menu();

		//loop the play logic while playing
		while(playing){
			GameSelection();
		}
	}
}

void initUser(void){
	//User menu
	printf("===============================================\n");
	printf("Welcome to the online Minesweeper gaming system\n");
	printf("===============================================\n\n");
	printf("You are required to log on with your registered user name and password.\n\n");
	printf("User name: ");
	scanf("%s", username);
	printf("Password: ");
	scanf("%s", password);
	if(Authenticate(sockfd, username, password) != 1){
		fprintf(stderr, "User failed to authenticate! exiting...\n");
		exit(0);
	}else{
		printf("\n\nUser Authenticted!\n\n");
	}
}

void Menu(void){
	int selection;
	char userselect[1];

	//play menu
	printf("Welcome to the Minesweeper gaming system.\n\n");
	printf("Please enter a selection:\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n\n");
	printf("Selection option (1-3): ");
	scanf("%d", &selection);
	snprintf(userselect, 1, "%d", selection);
	Send(sockfd, userselect);
	switch(selection){
		case 1:
			initialiseGame();
			playing = true; 
			break;
		case 2:
			//show leaderboard
			break;
		default:
			close(sockfd);
			exit(1);
	}
}

void initialiseGame(void){
	//initialise the Minesweeper
	minesRemaining = NUM_MINES;
	for(int i = 0; i < NUM_TILES_Y; i++){
		for(int d = 0; d < NUM_TILES_X; d++){
			//starting state is everything is no flag and not revealed
			game[d][i].is_flag = false;
			game[d][i].revealed = false;
			game[d][i].adjacent_mines = 0;
		}
	}
}

void drawGame(void){
	//setup the Y frame
	printf("Remaining mines: %d\n\n", minesRemaining);
	for(int i = 1; i <= NUM_TILES_X; i++){
		printf("\t%d", i);
	}
	printf("\n");
	for(int i = 1; i <= NUM_TILES_X; i++){
		printf("\t-");
	}
	printf("\n");

	for(int i = 0; i<NUM_TILES_Y; i++){
		//write the X coordinate to the start of the line
		printf("%c |\t", coord[i]);
		for(int d = 0; d<= NUM_TILES_X; d++){
			//if flagged then print that, ignore adjacent tiles parameter
			if(game[d][i].is_flag==true){
				printf("+\t");
			}
			//if revealed is true then print the adjacent tiles parameter
			else if(game[d][i].revealed==true){
				printf("%d\t", game[d][i].adjacent_mines);
			}
			//else ignore and print nothing
			else{
				printf(" \t");
			}
		}
		printf("\n\n");
	}
}

void GameSelection(void){
	char selection[1] = "";
	char tile[2] = "";
	printf("==========================================================================\n");
	drawGame();
	printf("Choose an option:\n<R> Reveal a tile\n<P> Place flag\n<Q> Quit game\n\n");
	printf("Option (R, P, Q): ");
	scanf(" %c", selection);
	send(sockfd, selection, sizeof(int), 0);
	if(*selection == 'R' || *selection == 'r'){
		printf("\nSelect a tile to reveal (eg:A1): ");
		scanf(" %c", tile);
	}
	else if(*selection == 'P' || *selection == 'r'){
		printf("\nSelect a tile to place a flag on (eg:A1): ");
		scanf(" %c", tile);
	}else if(*selection == 'Q' || *selection == 'q'){
		playing = false;
	}
	printf("\n");
	while((*selection = getchar()) != '\n' && *selection != EOF);
}

void Send(int socket_id, char *output) {
	int i=0;  
	for (i = 0; i < strlen(output); i++) {
		printf("%c\n", output[i]);
		send(socket_id, &output[i], 1, 0);	//char is already byte representation
	}
}

char * Receive(int socket_identifier, int size) {
    int number_of_bytes, i=0;
    char rec;

    char *buff[size];

	for (i=0; i < size; i++) {
		if ((number_of_bytes=recv(socket_identifier, &rec, 1, 0))
		         == -1) {
			perror("recv");
			exit(EXIT_FAILURE);		
		    
		}
		buff[i] = &rec;
	}
	return *buff;
}

int Authenticate(int socket_id, char *username, char *password){
	char Buffer[1000];
	strcpy(Buffer, username);
	strcat(Buffer,password);
	strcat(Buffer, '\0');
	printf("%s\n", Buffer);
	Send(sockfd, Buffer);

	char *response = Receive(sockfd, 1);
	printf("%s\n", response);
	int ret_val = atoi(response);
	return ret_val;
}
