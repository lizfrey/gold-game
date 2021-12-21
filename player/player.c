/*
 * player.c - support file for nuggets.c
 * stores a player, with an address, a number
 * of gold pieces, and a visibility array
 * 
 * Elizabeth Frey, Jack Gnibus, Isaac Feldman
 * CS 50 Project
 * Winter 2021
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "player.h"
#include "../gameboard/gameboard.h"

/********** global types **********/
typedef struct player{
    char **visibilityArray;
    char character;
    char *name;
    int gold;
    addr_t address;
    int rows;
    int column;
} player_t;

/********** global functions declaration **********/
/* see player.h for description */

/********** global functions **********/
/********** playerNew **********/
/* see function declaration for description*/
player_t 
*playerNew(char *newName, char charac, addr_t addr, int n, int m){
    player_t *player = malloc(sizeof(player_t));
    //if memory allocation fails
    if(player == NULL){
        free(player);
        return NULL;
    }
    else{
        player->gold = 0;
        player->visibilityArray = malloc(m * sizeof(char *));
        for(int i = 0; i<m; i++){
            player->visibilityArray[i] = calloc(n, sizeof(char));
        }
        player->address = addr;
        player->name = malloc((strlen(newName)+1)*sizeof(char));
        strcpy(player->name, newName);
        player->rows = m;
        player->column = n;
        player->character = charac;
    }
    return player;
}

/********** getGold **********/
/* see function declaration for description*/
int 
playerGetGold(player_t *player){
    return player->gold;
}

/********** addGold **********/
/* see function declaration for description*/
void 
playerAddGold(player_t *player, int additionalGold){
    player->gold += additionalGold;
}

/********** getAddress **********/
/* see function declaration for description*/
addr_t 
playerGetAddress(player_t *player){
    return player->address;
}

/********** getVisibilityArray **********/
/* see function declaration for description*/
char 
**getVisibilityArray(player_t *player){
    return player->visibilityArray;
}

/********** updateVisibilityArray **********/
/* see function declaration for description*/
void 
updateVisibilityArray(player_t *player, gameboard_t *gameboard, bool isSpectator){
    for(int x = 0; x<player->rows; x++){
        for(int y = 0; y<player->column; y++){
            char result = isVisible(gameboard,player,x,y, isSpectator);
            if(result == '0') //failure
            {
                return;
            }
            if(result == '\0'){
                //if the player can't see it and its gold
                if(player->visibilityArray[x][y] == '*'){
                    player->visibilityArray[x][y] = '.';
                }
                else {
                    char c = player->visibilityArray[x][y];
                    if(c>=65 && c<=90){
                        player->visibilityArray[x][y] = '.';
                    }
                //otherwise, do nothing
                }
            }
            else{
                player->visibilityArray[x][y] = result;
            }
        }
    }
}

/********** getName **********/
/* see function declaration for description*/
char 
*playerGetName(player_t *player){
    return player->name;
}

/********** getCharacter **********/
/* see function declaration for description*/
char 
playerGetChar(player_t *player){
    return player->character;
}

/********* playerDelete **********/
/* see function declaration for description*/
void
playerDelete(player_t *player){
    free(player->name);
    for(int i = 0; i<player->rows; i++){
        free(player->visibilityArray[i]);
    }
    free(player->visibilityArray);
    free(player);
}

