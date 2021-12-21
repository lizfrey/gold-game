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
#include "player.h"
#include "message.h"

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
    char *newName = "PlayerName";
    player_t *player = playerNew(newName, 'A', message_noAddr(), 0, 0);
    EXPECT(player != NULL);
    EXPECT(message_eqAddr(playerGetAddress(player), message_noAddr()));
    EXPECT(playerGetChar(player) == 'A');
    EXPECT(playerGetGold(player) == 0);
    playerAddGold(player,5);
    EXPECT(playerGetGold(player) == 5);
    EXPECT(strcmp(playerGetName(player), newName) == 0);
    playerDelete(player);
    if (unit_failed > 0) {
        printf("FAILED %d of %d tests\n", unit_failed, unit_tested);
        return unit_failed;
    } 
    else {
        printf("PASSED all of %d tests\n", unit_tested);
        return 0;
    }
}
