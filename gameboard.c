/*
 * gameboard.c - an implementation of gameboard.h
 * 
 * see gameboard.h and the README for more detailed documentation
 *
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include "gameboard.h"
#include "../player/player.h"

/*	global types	*/
typedef struct gameboard {
	int goldRemaining;
	int width;
	int height;
	int floorCount;
	cell_t ***grid;
} gameboard_t;

/*	local types	*/
typedef struct cell {
	player_t *player; // if a player is in the cell this points to that player
	int gold;	// the amount of gold on this cell (default 0)
	char mapType; // what map square this is (empty, wall, floor, hallway)
} cell_t;

//function prototypes
char getCell(gameboard_t *gameboard, const int row, const int col);
bool isWall(gameboard_t *gameboard, const int row, const int col);
bool isEmpty(gameboard_t *gameboard, const int row, const int col);
bool isPlayer(gameboard_t *gameboard, const int row, const int col);
bool emptyAdjacent(gameboard_t *gameboard, const int row, const int col);
bool inBounds(gameboard_t *gameboard, const int row, const int col);

/*	gameboardNew	*/
gameboard_t
*gameboardNew(char *mapFilename, const int minGold, const int maxGold, const int totalGold,  int randSeed) 
{
	gameboard_t *gameboard = malloc(sizeof(gameboard_t));
	gameboard->goldRemaining = totalGold;

	if(randSeed == -1) {  // initalize the random number generator
		srand(getpid());
	} else {
		srand(randSeed);
	}

	// open the map file and parse it
	FILE *fp = fopen(mapFilename, "r");
	int count_width = 0; 
	int count_lines = 0;

	if (fp != NULL) { // file opened successfully
		while(fgetc(fp) != '\n') { // get the width of the file
			count_width++;
		}
		rewind(fp); // reset the stream

		char c = '\0';
		while( (c = fgetc(fp)) != EOF) { // get the number of lines in the file
			if (c == '\n') {
				count_lines++;
			}
		}
		rewind(fp);
		
		// load in the map

        // allcoate data
		cell_t ***arr = calloc(count_lines, sizeof(cell_t**));
		if (arr != NULL){ // successful allocation
			gameboard->floorCount = 0;
			for (int i = 0; i < count_lines; i++) {
			    cell_t **row = calloc(count_width, sizeof(cell_t));
                if(row != NULL) {
			        for (int j = 0; j < count_width+1; j++) { // read one line
                        row[j] = malloc(sizeof(cell_t));
                        if(row[j] != NULL){
                            row[j]->player=NULL;
                            row[j]->gold=0;
				            char c = fgetc(fp);
							if(c!='\n'){
				            	row[j]->mapType = c;
							}
						    if(c == '.') { //count number of floor tiles for even distribution
							    gameboard->floorCount++; 
						    } 
                        }
                        else{
                            //unsuccessful allocation
                            return NULL;
                        }
			        }
                } else {
                    fclose(fp);
                    return NULL;
                }
                arr[i] = row;
			}



			// fill in the map
			gameboard->grid = arr;
			gameboard->width = count_width;
			gameboard->height = count_lines;

			int numGold = (rand() % (maxGold - minGold + 1)) + minGold;
			int goldToPlace = totalGold;
			int goldPlaced = 0;
            
			for(int x = 0; x < gameboard->height; x++) { //reiterate through grid to distribute gold evenly over floor tiles
				for(int y = 0; y < gameboard->width; y++) {
					if(gameboard->grid[x][y]->mapType == '.'){

                        if(1 + rand()%((gameboard->floorCount)/numGold) == 1 && goldToPlace > 0) {
                            if(goldToPlace <= totalGold/numGold) {
                                gameboard->grid[x][y]->gold = goldToPlace;
								goldPlaced+=goldToPlace;
                                goldToPlace = 0;
                            } else {
                                gameboard->grid[x][y]->gold = totalGold/numGold;
								goldPlaced+=totalGold/numGold;
                                goldToPlace -= totalGold/numGold;
                            }
                        }
                    }
				}
			}

			if(goldPlaced != totalGold){
                bool fixed = false;
				for(int x = 0; x < gameboard->height; x++) { //reiterate through grid to distribute gold evenly over floor tiles
					for(int y = 0; y < gameboard->width; y++) {
						if(gameboard->grid[x][y]->mapType == '.'&& !fixed){
							gameboard->grid[x][y]->gold += (totalGold - goldPlaced);
                            fixed = true;
						}
					}
				}
			}

			fclose(fp);
			return gameboard;
		} else {
            fclose(fp);
			return NULL;
		}
	} else {
        fclose(fp);
		return NULL;
	}
}

/*	gameboardDelete	*/
int
gameboardDelete(gameboard_t *gameboard)
{
	for (int x = 0; x < gameboard->height; x++) {
	 for (int y = 0; y < gameboard->width+1; y++) {
         if(gameboard->grid[x][y]->player != NULL){
             playerDelete(gameboard->grid[x][y]->player);
         }
        free(gameboard->grid[x][y]);
	 }
     free(gameboard->grid[x]);
	}
    free(gameboard->grid);
    free(gameboard);
    return 0;
}

/*	gameboardString	*/
char
*gameboardString(gameboard_t *gameboard, player_t *player, bool isSpectator)
{
    char *display = "DISPLAY\n";
    int cells = (gameboard->width+1)*gameboard->height+1;
    char *output_string = calloc(cells, sizeof(char));
    if(isSpectator){
        int current = 0;

	    for(int x = 0; x < gameboard->height; x++) { //reiterate through grid to distribute gold evenly over floor tiles
		    for(int y = 0; y < gameboard->width; y++) {
                output_string[current] = getCell(gameboard,x,y);
                current ++;
            }
			output_string[current] = '\n';
			current ++;
	    }
	    output_string[cells - 1] = '\0'; // terminate the string
    }
    else{
        updateVisibilityArray(player, gameboard, false);
        char **visibilityArray = getVisibilityArray(player);
        int current = 0;

	    for(int x = 0; x < gameboard->height; x++) { //reiterate through grid to distribute gold evenly over floor tiles
		    for(int y = 0; y < gameboard->width; y++) {
                if(visibilityArray[x][y] == '\0'){
                    output_string[current] = ' ';
                }
                else{
                    output_string[current] = visibilityArray[x][y];
                }
                current ++;
            }
            output_string[current] = '\n';
            current++;
	    }
	    output_string[cells - 1] = '\0'; // terminate the string
    }
    char *returnString = malloc((strlen(output_string) + strlen(display)+1)*sizeof(char));
    strcpy(returnString, display);
    strcat(returnString, output_string);
    free(output_string);
    return returnString;
}

/*	gameboardAddPlayer	*/
int
gameboardAddPlayer(gameboard_t *gameboard, player_t *player)
{
	bool playerInserted = false;
	while(!playerInserted) {
		for(int y = 0; y < gameboard->width; y++) {
			for(int x = 0; x < gameboard->height; x++) {
				if(gameboard->grid[x][y]->mapType == '.' && 1+rand()%(gameboard->floorCount) == 1) {
                    if(!playerInserted && (gameboard->grid[x][y]->player == NULL)){
					    gameboard->grid[x][y]->player = player;
					    playerInserted = true;
                    }
				}
			}
	    }
    }
	return 0;
}

/*	gameboardMovePlayer	*/
int 
gameboardMovePlayer(gameboard_t *gameboard, player_t *player, const int deltaCol, const int deltaRow)
{
    player_t *holder = NULL;
	int goldCollected = 0;
	for (int y = 0; y < gameboard->width; y++) {
		for (int x = 0; x < gameboard->height; x++) {
			if (gameboard->grid[x][y]->player == player) {
				holder = gameboard->grid[x][y]->player;
				gameboard->grid[x][y]->player = NULL;
			
				int finalRow = x;
				int finalCol = y;
			
                if(!(deltaRow == 0 || deltaCol == 0)){
                    //if is a diagonal move
                    if(deltaRow == 1 || deltaRow == -1) { //assume that deltaCol also =+-1
                        if(inBounds(gameboard,finalRow+deltaRow, finalCol+deltaCol)&&!isWall(gameboard, finalRow+deltaRow, finalCol+deltaCol) && !isPlayer(gameboard, finalRow+deltaRow, finalRow+deltaCol)) {
                            finalRow+=deltaRow;
                            finalCol+=deltaCol;
							if(gameboard->grid[finalRow][finalCol]->gold > 0) {
								goldCollected = gameboard->grid[finalRow][finalCol]->gold;
								gameboard->grid[finalRow][finalCol]->gold = 0;
							}
                        }
                    }
                    else {
                        int deltaX = 0, deltaY = 0;
                        if (deltaRow < 0) {
                            deltaX = -1;
                        }
                        else {
                            deltaX = 1;
                        }
						if (deltaCol < 0) {
							deltaY = -1;
						}
						else{
							deltaY = 1;
						}
                        while (inBounds(gameboard, finalRow+deltaX, finalCol+deltaY)&&!isWall(gameboard, finalRow+deltaX, finalCol+deltaY) && !isPlayer(gameboard, finalRow+deltaRow, finalRow+deltaCol)){
                            finalRow+=deltaX;
                            finalCol+=deltaY;
							if(gameboard->grid[finalRow][finalCol]->gold >0){
								goldCollected = gameboard->grid[finalRow][finalCol]->gold ;
								gameboard->grid[finalRow][finalCol]->gold = 0;
							}
                        }
                    }
                }
                else if (deltaRow == -1 || deltaRow == 1) {
					// normal vertical move
                    if(inBounds(gameboard,finalRow + deltaRow, y)&&(!isWall(gameboard,finalRow + deltaRow, y) || isEmpty(gameboard,finalRow + deltaRow, y)) && !isPlayer(gameboard,finalRow + deltaRow, y)){
                        finalRow+=deltaRow;
						if(gameboard->grid[finalRow][finalCol]->gold >0){
							goldCollected = gameboard->grid[finalRow][finalCol]->gold ;
							gameboard->grid[finalRow][finalCol]->gold = 0;
						}
                    }
				} else if (deltaRow == 2){
					//as far down as possible
					while (inBounds(gameboard, finalRow+1, y)&&(!isWall(gameboard, finalRow+1, y) || isEmpty(gameboard, finalRow+1, y)) && !isPlayer(gameboard, finalRow+1, y)) {
						if(gameboard->grid[finalRow][finalCol]->gold >0){
							goldCollected = gameboard->grid[finalRow][finalCol]->gold ;
							gameboard->grid[finalRow][finalCol]->gold = 0;
						}
						finalRow++;
					}
				} else if (deltaRow == -2){
					// as far up as possible
					while (inBounds(gameboard, finalRow-1, y) &&(!isWall(gameboard, finalRow-1, y) || isEmpty(gameboard, finalRow-1, y)) && !isPlayer(gameboard, finalRow-1, y)  ) {
						if(gameboard->grid[finalRow][finalCol]->gold >0){
							goldCollected = gameboard->grid[finalRow][finalCol]->gold ;
							gameboard->grid[finalRow][finalCol]->gold = 0;
						}
						finalRow--;
					}
				}
                else if (deltaCol == -1 || deltaCol == 1){
					// normal horizontal  move
                    if(inBounds(gameboard, x, finalCol + deltaCol)&&(!isWall(gameboard, x, finalCol + deltaCol) || isEmpty(gameboard, x, finalCol + deltaCol)) && !isPlayer(gameboard, x, finalCol + deltaCol) ) {
					    finalCol+=deltaCol;
						if(gameboard->grid[finalRow][finalCol]->gold >0){
							goldCollected = gameboard->grid[finalRow][finalCol]->gold ;
							gameboard->grid[finalRow][finalCol]->gold = 0;
						}
                    }
					
				} else if (deltaCol == 2){
					//as far right as possible
					while ( inBounds(gameboard, x, finalCol+1)&&(!isWall(gameboard, x, finalCol+1) || isEmpty(gameboard, x, finalCol+1)) && !isPlayer(gameboard, x, finalCol+1) ) {
						if(gameboard->grid[finalRow][finalCol]->gold >0){
							goldCollected = gameboard->grid[finalRow][finalCol]->gold ;
							gameboard->grid[finalRow][finalCol]->gold = 0;
						}
						finalCol++;
					}
				} else if (deltaCol == -2){
					// as far left as possible
					while (inBounds(gameboard, x, finalCol-1)&&(!isWall(gameboard, x, finalCol-1) || isEmpty(gameboard, x, finalCol-1)) && !isPlayer(gameboard, x, finalCol-1) ) {
						if(gameboard->grid[finalRow][finalCol]->gold >0){
							goldCollected = gameboard->grid[finalRow][finalCol]->gold ;
							gameboard->grid[finalRow][finalCol]->gold = 0;
						}
						finalCol--;
					}
				}

				gameboard->grid[finalRow][finalCol]->player = holder;
				gameboard->goldRemaining-=goldCollected;
				playerAddGold(holder,goldCollected);
				return goldCollected;
			}
	 	}
	}
	return 0;
}

/*	gameboardRemovePlayer	*/
void
gameboardRemovePlayer(gameboard_t *gameboard, player_t *player)
{
	player_t *holder = NULL;

	for (int y = 0; y < gameboard->width; y++) {
	for (int x = 0; x < gameboard->height; x++) {
		if (gameboard->grid[x][y]->player == player) {
			holder = gameboard->grid[x][y]->player;
			if(holder!=NULL){
            	playerDelete(holder);
			}
			gameboard->grid[x][y]->player = NULL;
		}
	}
	}
}

/*	isVisible	*/

char 
isVisible(gameboard_t *gameboard, player_t *player, const int row, const int col, bool isSpectator)
{
	for (int py = 0; py < gameboard->width; py++) {
		for (int px = 0; px < gameboard->height; px++) {
			if (gameboard->grid[px][py]->player == player) {

				if(px == row && py == col) { //Check if currently on the player square
					return '@';
				}

				double dx = row - px;
				double dy = col - py;
				int step;

				if(abs(dy) >= abs(dx)) {
					step = abs(dy);
				} else {
					step = abs(dx);
				}

				dy = dy / step;
				dx = dx / step;
				
				int x = px;
				int y = py;

				int i = 1;
				double fractionalStep = 0;

				while(i <= step) {

					if(fabs(dy) < 1) {
						x=x+dx;
						fractionalStep = fractionalStep+dy;

						if(fabs(fractionalStep) >= fabs(dx) || fabs(fabs(fractionalStep) - fabs(dx)) <= 0.05) { //tolerance for float arithmetic
							y=y+1*(fabs(dy)/dy);

							if(fractionalStep < 0) {
								fractionalStep += 1;
							} else {
								fractionalStep -= 1;
							}
						}
					} else {
						y = y+dy;
						fractionalStep=fractionalStep+dx;

						if(fabs(fractionalStep) >= fabs(dy) || fabs(fabs(fractionalStep) - fabs(dy)) <= 0.05) {
							x=x+1*(fabs(dx)/dx);
							if(fractionalStep < 0) {
								fractionalStep += 1;
							} else {
								fractionalStep -= 1;
							}
						}
					}
					
					if(isWall(gameboard, x, y)) {
						if(x == row && y == col) { //visible wall
							return gameboard->grid[x][y]->mapType;
						} else { //cell not visible
							return '\0';
						}
					} else if (gameboard->grid[x][y]->mapType == '#') { //hallway
                        if(x==row && y==col){
                            return '#';
                        }

                        if(( (px-row)*(px-row) + (py-col)*(py-col)) <= 2) {
                            return '#';
                        } else return '\0';
						
					} else if(x == row && y == col) { //visible cell
						if(isPlayer(gameboard, x, y)) {
							return playerGetChar(gameboard->grid[x][y]->player);
						} else if(gameboard->grid[x][y]->gold != 0) { //cell contains gold
							return '*';
						} else {
								return gameboard->grid[x][y]->mapType;
						}
					}
					i += 1;
				}
			}
		}
	}
	return '0';
}


/*	gameboardRemainingGold */
int gameboardRemainingGold(gameboard_t *gameboard)
{
	return gameboard->goldRemaining;
}

/*	gameboardWidth	*/
int gameboardWidth(gameboard_t *gameboard) 
{
	return gameboard->width;
}

/*	gameboardHeigth	*/
int gameboardHeight(gameboard_t *gameboard)
{
	return gameboard->height;
}


/********************************************************************************/
/*	private methods	*/


/*
 * getCell - returns an character representation of a cell's contents
 */
char
getCell(gameboard_t *gameboard, const int row, const int col)
{
	cell_t *cell = gameboard->grid[row][col];
	if (cell-> player != NULL){
		return playerGetChar(cell->player); 
	} 
	if (cell->gold > 0){
		return '*';
	} else {
		return cell->mapType;
	}

}

/* isWall */
bool
isWall(gameboard_t *gameboard, const int row, const int col)
{
	cell_t *cell = gameboard->grid[row][col];
	return cell->mapType == '-' || cell->mapType == '+' ||cell->mapType == '|'||cell->mapType == ' ';
}

/* isEmpty */
bool
isEmpty(gameboard_t *gameboard, const int row, const int col)
{
	cell_t *cell = gameboard->grid[row][col];
	return cell->mapType == '.';
}

/* isPlayer */
bool 
isPlayer(gameboard_t *gameboard, const int row, const int col){
    cell_t *cell = gameboard->grid[row][col];
    return cell->player != NULL;
}

bool
emptyAdjacent(gameboard_t *gameboard, const int row, const int col) {

    for(int dx = -1; dx < 2; dx++) {
        for(int dy = -1; dy < 2; dy++) {
            if(((row+dx < 0 || row+dx >= gameboard->height) || (col+dy < 0 || col+dy >= gameboard->width))) {
                //cell does not exist
            } else {
                if( !(dy == 0 && dx == 0) ) { //check to see if cell in question is itself
                    if(isEmpty(gameboard, row+dx, col+dy)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool
inBounds(gameboard_t *gameboard, const int row, const int col) {
	if((row<gameboard->height)&&(col<gameboard->width)&&(row>=0)&&(col>=0)){
		return true;
	}
	else{
		return false;
	}
}
