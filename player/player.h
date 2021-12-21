/*
 * player.c - header file for player.c
 * stores a player, with an address, a number
 * of gold pieces, and a visibility array
 * 
 * Elizabeth Frey, Jack Gnibus, Isaac Feldman
 * CS 50 Project
 * Winter 2021
 */

#ifndef __PLAYER_H
#define __PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include "message.h"
#include "../gameboard/gameboard.h"

/********** global types **********/
typedef struct player player_t;
typedef struct gameboard gameboard_t;
typedef struct cell cell_t;

/********** playerNew **********/
/*
 * playerNew creates a new player by taking in an address and the 
 * dimensions of the gameboard. it sets the player's amount
 * of gold to 0
 * 
 * caller provides:
 * - valid address
 * - valid int n and m, representing the dimensions of the gameborad
 * 
 * n and m must both be integers that are greater than 0
 * 
 * we return:
 * - a new player_t 
 */
player_t *playerNew(char *newName, char charac, addr_t addr, int n, int m);

/********** getGold **********/
/* 
 * getGold returns the amount of gold that
 * a player currently has
 * 
 * caller provides: a valid player
 * 
 * we return, an int >= 0 representing
 * the player's current number of gold nuggets
 * collected
 */
int playerGetGold(player_t *player);

/********** addGold **********/
/*
 * addGold takes in a player and an amount of gold to add
 * and adds that int of gold to the player's current
 * amount of gold
 * 
 * caller provides: a valid player and an additionalGold >=0
 * 
 * we return: nothing
 */
void playerAddGold(player_t *player, int additionalGold);

/********** getAddress **********/
/*
 * getAddress takes in a player and returns
 * the player's address
 * 
 * caller provides: a valid player
 * 
 * we return: an int representing the player's address
 */
addr_t playerGetAddress(player_t *player);

/********** getVisibilityArray **********/
/*
 * getVisibilityArray takes in a player
 * and returns a pointer to the player's 
 * visibilityArray
 * 
 * caller provides: a valid player
 * 
 * we return: a pointer to the visibility array
 */
char **getVisibilityArray(player_t *player);

/********** updateVisibilityArray **********/
/*
 * updateVisibilityArray loops through 
 * all spaces in the player's current visibilityArray
 * and updates values based on what the player
 * can currently see
 * 
 * caller provides: valid player pointer
 * 
 * we return: nothing
 */
void updateVisibilityArray(player_t *player, gameboard_t *gameboard, bool isSpectator);

/********** getName **********/
/*
 * getName returns the player's name
 * 
 * caller provides: valid player pointers
 * 
 * we return: a char *name
 */
char *playerGetName(player_t *player);

/********** getCharacter **********/
/*
 * getName returns the player's character
 * 
 * caller provides: valid player pointer
 * 
 * we return: a char character
 */
char playerGetChar(player_t *player);

/********** playerDelete **********/
/*
 * reallocates all memory used by the player
 * 
 * caller provides: valid player pointer
 * 
 * we return: nothing
 */
void playerDelete(player_t *player);

#endif //__PLAYER_H
