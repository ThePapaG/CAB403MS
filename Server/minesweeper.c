#include <minesweeper.h>


void Send_Array_Data(int socket_id) {

	int i=0;
	/* Create an array of squares of first 30 whole numbers */
	int gameArray[ARRAY_SIZE] = {0};
	for (i = 0; i < ARRAY_SIZE; i++) {
		gameArray[i] = GameState;
	}

	uint16_t gameInfo;  
	for (i = 0; i < ARRAY_SIZE; i++) {
		gameInfo = htons(gameArray[i]);
		send(socket_id, &gameInfo, sizeof(uint16_t), 0);
	}
}
int *Receive_Array_Int_Data(int socket_identifier, int size) {
    int number_of_bytes, i=0;
    uint16_t gameInfo;
	
	int *packages = malloc(sizeof(int)*size);
	for (i=0; i < size; i++) {
		if ((number_of_bytes=recv(socket_identifier, &gameInfo, sizeof(uint16_t), 0))
		         == RETURN_ERROR) {
			perror("recv");
			exit(EXIT_FAILURE);			
		    
		}
		packages[i] = ntohs(gameInfo);
	}
	return packages;
}

