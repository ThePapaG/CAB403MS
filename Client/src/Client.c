//CAB403 Assignment 2018 Sem2 - Client main
//Written by Grant Dare n9476512
//in conjunction with Callum Scott

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

	initUser();

	while(1){
		//show the main menu
		Menu();

		//loop the play logic while playing
		while(playing){
			
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
	}	//else it went smoothly
}

void Menu(void){
	int selection = 0;
	char userselect[10];
	int response = 0;
	//play menu
	printf("%d\n", selection);
	printf("Welcome to the Minesweeper gaming system.\n\n");
	printf("Please enter a selection:\n<1> Play Minesweeper\n<2> Show Leaderboard\n<3> Quit\n\n");
	printf("Selection option (1-3): ");
	scanf("%d", &selection);
	printf("%d\n", selection);
	switch(selection){
		case 1:
			strcpy(userselect, "p");
			initialiseGame();
			playing = true;
			printf("%s\n", userselect);
			send(sockfd, userselect, 10, 0);
			
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
	if(*selection == 'R' || *selection == 'r'){
		printf("\nSelect a tile to reveal (eg:A1): ");
		scanf(" %c", tile);
		Reveal(tile);
	}
	else if(*selection == 'P' || *selection == 'r'){
		printf("\nSelect a tile to place a flag on (eg:A1): ");
		scanf(" %c", tile);
		Place(tile);
	}
	printf("\n");
	while((*selection = getchar()) != '\n' && *selection != EOF);
}

void Reveal(char* tile){
	int *coordinate = malloc(sizeof(int)*2);
	int success[1] = {0};
	for(int i = 0; i<NUM_TILES_Y; i++){
		if(coord[i] == tile[0]){
			coordinate[0] = i;
			success[0] = 1;
			break;
		}
	}
	if(*success!=1){
		coordinate[1] = atoi(&tile[1]);
	}
}

void Place(char* tile){
	int *coordinate = malloc(sizeof(int)*2);
	int success[1] = {0};
	for(int i = 0; i<NUM_TILES_Y; i++){
		if(coord[i] == tile[0]){
			coordinate[0] = i;
			success[0] = 1;
			break;
		}
	}
	if(*success!=1){
		coordinate[1] = atoi(&tile[1]);
	}
}

int Authenticate(int socket_id, char *username, char *password){
	char Buffer[1000];
	strcpy(Buffer, username);
	strcat(Buffer,password);
	send(sockfd, Buffer, 1000, 0);

	int response = 0;
	recv(sockfd, &response, sizeof(int), 0);
	return response;
}