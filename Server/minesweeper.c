#include <minesweeper.h>

void Create_Tiles(){
	int x=0, y=0;
	/* Create an array of squares of first 30 whole numbers */
	Tile tiles[NUM_TILES_X][NUM_TILES_Y] = {0};
	for (x = 0; x < NUM_TILES_X; x++) {
		for (int y = 0; y < NUM_TILES_Y; ++y)
		{
			tiles[x][y].adjacent_mines=0;
			tiles[x][y].revealed=0;
			tiles[x][y].is_mine=0;
		}
	}

	return tiles;
}

void Place_Mines(Tile tiles[NUM_TILES_X][NUM_TILES_Y]){
	for (int i = 0; i < NUM_MINES; ++i)
	{
		do
		{
			x = rand()%NUM_TILES_X;
			y = rand()%NUM_TILES_Y;
		}while (tiles[x][y].is_mine == 1);
		tiles[x][y].is_mine = 1;
	}
}

void Update_Adjacent(Tile tiles[NUM_TILES_X][NUM_TILES_Y]){
	for (x = 0; x < NUM_TILES_X; x++) {
		for (int y = 0; y < NUM_TILES_Y; ++y)
		{
			if (tiles[x-1][y].is_mine==1)
			{
				tiles[x][y].adjacent_mines+=1;
			}
			if (tiles[x+1][y].is_mine==1)
			{
				tiles[x][y].adjacent_mines+=1;
			}
			if (tiles[x][y-1].is_mine==1)
			{
				tiles[x][y].adjacent_mines+=1;
			}
			if (tiles[x][y+1].is_mine==1)
			{
				tiles[x][y].adjacent_mines+=1;
			}
			if (tiles[x-1][y-1].is_mine==1)
			{
				tiles[x][y].adjacent_mines+=1;
			}
			if (tiles[x+1][y+1].is_mine==1)
			{
				tiles[x][y].adjacent_mines+=1;
			}
		}
	}
}

// void Send_Array_Data(int socket_id) {
// 	int i;
// 	uint16_t gameInfo;  
// 	for (i = 0; i < ARRAY_SIZE; i++) {
// 		gameInfo = htons(gameArray[i]);
// 		send(socket_id, &gameInfo, sizeof(uint16_t), 0);
// 	}
// }
// int *Receive_Array_Int_Data(int socket_identifier, int size) {
//     int number_of_bytes, i=0;
//     uint16_t gameInfo;
	
// 	int *packages = malloc(sizeof(int)*size);
// 	for (i=0; i < size; i++) {
// 		if ((number_of_bytes=recv(socket_identifier, &gameInfo, sizeof(uint16_t), 0))
// 		         == RETURN_ERROR) {
// 			perror("recv");
// 			exit(EXIT_FAILURE);			
		    
// 		}
// 		packages[i] = ntohs(gameInfo);
// 	}
// 	return packages;
// }

int StartGame()
{

	Tile tiles[NUM_TILES_X][NUM_TILES_Y];
	tiles= Create_Tiles();
	Place_Mines(tiles);

	return GAME_STATE = 0;

}

