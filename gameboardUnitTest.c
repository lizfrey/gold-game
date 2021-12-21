/*
 * playerUnitTest.c - a unit testing
 * program for player.c
 * 
 * Elizabeth Frey, Jack Gnibus, Isaac Feldman
 * CS 50 Project
 * Winter 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "gameboard.h"
#include "message.h"
#include "../player/player.h"

//file-local global variables
static int unit_tested = 0;     // number of test cases run
static int unit_failed = 0;     // number of test cases failed

// a macro for shorthand calls to expect()
#define EXPECT(cond) { unit_expect((cond), __LINE__); }

// Checks 'condition', increments unit_tested, prints FAIL or PASS
void unit_expect(bool condition, int linenum)
{
   unit_tested++;
     if (condition) {
        printf("PASS test %03d at line %d\n", unit_tested, linenum);
    } else {
        printf("FAIL test %03d at line %d\n", unit_tested, linenum);
            unit_failed++;
    }
}

int
main(){
    char *filename = "../maps/main.txt";
    gameboard_t *gameboard = gameboardNew(filename, 10, 30, 250, 1);
    char *stringHolder;
    char *playerName = "player one";
    player_t *player = playerNew(playerName, 'A', message_noAddr(), gameboardWidth(gameboard), gameboardHeight(gameboard));
    EXPECT(gameboardAddPlayer(gameboard, player) == 0);
    gameboardMovePlayer(gameboard, player, -2, -2);
    stringHolder = gameboardString(gameboard, player, true);
    printf("%s", stringHolder);
    free(stringHolder);
    gameboardMovePlayer(gameboard,player,-2,-2);
    stringHolder = gameboardString(gameboard, player, true);
    printf("%s", stringHolder);
    free(stringHolder);
    playerName = "player two";
    player_t *player2 = playerNew(playerName, 'B', message_noAddr(), gameboardWidth(gameboard), gameboardHeight(gameboard));
    EXPECT(gameboardAddPlayer(gameboard, player2) == 0);
    stringHolder = gameboardString(gameboard, player2, false);
    printf("%s", stringHolder);
    free(stringHolder);
    int blueCollected = gameboardMovePlayer(gameboard, player2, -1, -1);
    stringHolder = gameboardString(gameboard, player2, false);
    printf("%s", stringHolder);
    free(stringHolder);
    blueCollected+=gameboardMovePlayer(gameboard, player2, 0, -2);
    stringHolder = gameboardString(gameboard, player2, false); 
    printf("%s", stringHolder);
    free(stringHolder);
    blueCollected+=gameboardMovePlayer(gameboard, player2, 2, 2);
    stringHolder = gameboardString(gameboard, player2, false); 
    printf("%s", stringHolder);
    free(stringHolder);
    EXPECT(blueCollected == playerGetGold(player2));
    printf("%d,%d", blueCollected, playerGetGold(player2));
    gameboardRemovePlayer(gameboard,player2);
    stringHolder = gameboardString(gameboard, player, false); 
    printf("%s", stringHolder);
    free(stringHolder);
    gameboardDelete(gameboard); 
    if (unit_failed > 0) {
        printf("FAILED %d of %d tests\n", unit_failed, unit_tested);
        return unit_failed;
    } 
    else {
        printf("PASSED all of %d tests\n", unit_tested);
        return 0;
    }
    return 0;
}
