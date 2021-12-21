/*
 * gameboard.h - api for dealing with the gameboard data structure
 *
 * made for nuggets, a CS50 final project 21W
 *
 * see IMPLEMENTATION for more details.
 *
 */

#ifndef __GAMEBOARD_H
#define __GAMEBOARD_H

#include <stdlib.h>
#include <stdio.h>
#include "../player/player.h"

/*	global types	*/
typedef struct gameboard gameboard_t;
typedef struct player player_t;

/* 
 * gameboardNew
 * 
 * creates a new gameboard_t in memory.
 *
 * Parameters:
 *	mapFilename	a string with the path to the mapfile
 *	minGold	the minimum amount of gold to spawn
 *	maxGold	the maximum amount of gold to spawn
 *	totalGold	the total number of gold nuggets on the board
 *	randomSeed	a random seed by which to spawn the gold
 *
 * Returns:
 *	a pointer to a new gameboard_t
 *
 * Memory Contract:
 *	The new gameboard_t is stored in dynamic memory and the caller
 *	is responsible for later calling gameboardDelete to free that memory.
 */

gameboard_t *gameboardNew(char *mapFilename, const int minGold, const int maxGold, const int totalGold, int randomSeed);

/*
 * gameboardDelete
 *
 * removes a gameboard_t from memory
 *
 * Parameters:
 *	gameboard	a pointer to a gameboard_t
 *
 * Returns:
 *	zero		on success
 *	nonzero	on failure
 *
 * Memory Contract: 
 *	This method removes the gameboard_t from memory and the player pointers,
 *	but not the player structs themselves. The caller is responsible for 
 *	removing the player structs.
 */

int gameboardDelete(gameboard_t *gameboard);

/*
 * gameboardString
 *
 * creates a string representation of a gameboard_t
 *
 * Parameters:
 *	gameboard	a pointer to a gameboard_t
 *	player		a pointer to a player_t
 *
 * Returns: 
 *	a string of the gameboard from the player's perspective.
 *	See REQUIREMENTS/DESIGN for the specification of this string.
 *
 */

char *gameboardString(gameboard_t *gameboard, player_t *player, bool isSpectator);


/*
 * gameboardAddPlayer
 *
 * adds a player to a random location on the gameboard
 *
 * Parameters:
 *	gameboard	a pointer to a gameboard_t
 *	player		a pointer to a player_t
 *
 * Returns:
 *	zero		on successful placement
 *	nonzero	on error
 *
 */

int gameboardAddPlayer(gameboard_t *gameboard, player_t *player);

/*
 * gameboardMovePlayer
 *
 * moves a player to a location on the gameboard 
 *
 * Parameters:
 *	gameboard	a pointer to a gameboard_t
 *	player		a pointer to a player_t
 *	deltaRow			the change in vertical gridsquares to move the player
 *	deltaCol			the change in horizontal girdsquares to move the player
 *
 * Notes:
 *	if the absolute value of the delta in any axis is 1, then move the player by one
 *	otherwise, move the player as far as possible without hitting a wall.
 *
 * Returns:
 *	zero		on successful move
 *	nonzero	on error
 *
 */

int gameboardMovePlayer(gameboard_t *gameboard, player_t *player, const int deltaRow, const int deltaCol);

/*
 * gameboardRemovePlayer
 *
 * removes a player from the board
 *
 * Parameters
 *	gameboard	a pointer to a gameboard_t
 *	player		a pointer to a player_t on the board
 *
 * Returns:
 *  nothing
 *	
 * Memory Contract:
 *   takes care of freeing player memory
 */

void gameboardRemovePlayer(gameboard_t *gameboard, player_t *player);

/*
 * isVisible
 *
 * checks if a given player can see a gridsquare
 *
 * Parameters:
 *	gameboard	the gameboard
 *	player	a pointer to a player_t
 *	row	the row of the gridsquare
 *	col	the column of the gridsquare
 *
 * Returns:
 * an representation of the gridsquare if visible, which is either:
 * 
 * NULL - if not visible
 * "+", "-", "|", ".", "#" - corner, horizontal wall, vertical wall, floor, hallway
 * "@" - placeholder for a player
 * "*" - gold
 *
 * NULL if not visible to the player
 *
 */
char isVisible(gameboard_t *gameboard, player_t *player, const int row, const int col, bool isSpectator);

/*
 * gamboardGoldRemanining
 *
 * returns the amount of gold that needs to be collected from the board
 *
 * Parameters:
 *	gameboard	the gameboard
 *
 * Returns:
 *	the amount of gold left or NULL on error
 */
int gameboardRemainingGold(gameboard_t *gameboard);


/*	gameboardWidth	*/
int gameboardWidth(gameboard_t *gameboard);

/*	gameboardHeigth	*/
int gameboardHeight(gameboard_t *gameboard);

#endif
