/*
 * server.c - provides the main functionality for nuggets
 * 
 * Elizabeth Frey, Jack Gnibus, Isaac Feldman
 * CS 50 Final Project
 * Winter 2021
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "../player/player.h"
#include "../gameboard/gameboard.h"
#include "../support/message.h"
#include "../support/log.h"
#include "../support/set.h"

/********** CONSTANTS **********/
static const int MaxNameLength = 50;   // max number of chars in playerName
static const int MaxPlayers = 26;      // maximum number of players
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles

/********** global variables **********/
static gameboard_t *gameboard;

/********** global types **********/
typedef struct gamestatus
{
    set_t *players; //key: ID, item: player
    set_t *deleted;
    int numPlayers;
    player_t *spectator;
    int alphabet;
    char **log;
    char *logMessage;
} gamestatus_t;

/********** FUNCTION DECLARATION **********/
/* LOCAL FUNCTIONS */
/********** GAMEINITIALIZER **********/
/*
 * creates the Gameboard and initializes
 * it, setting up the gold pieces
 * 
 * caller provides: seed
 * 
 * we return: nothing
 */
static void gameInitializer(int seed, char *filename);

/********** gameManager **********/
/*
 * the bulk of the server functionality, 
 * gameManager handles all requests by passing to
 * playerRequest to handle the requests 
 * as necessary. it does not 
 * validate any of the messages sent from
 * clients
 */
static void gameManager();

/********** gameOver **********/
/*
 * ends the game, deleting all allocated data
 * in the gamestatus, all remaining players, and the
 * final gameboard
 * 
 */
static void gameOver(gamestatus_t *gamestatus);

/********** overAnnounce ***********/
/*
 * helps in announcing the final table when the game is over
 *
 */
static void overAnnounce(void *arg, const char *key, void *item);

/********** logRemainingPlayers ***********/
/*
 * helps in adding in the gold values of the remaining players 
 * at the end of the game
 *
 */
static void logRemainingPlayers(void *arg, const char *key, void *item);


/********** playerRequests **********/
/* 
 * is passed into message_loop, handles
 * every message sent in from a client
 * 
 */
static bool playerRequest(void *arg, const addr_t from, const char *messageO);

/********** playerJoin **********/
/*
 * a helper method for player Request
 * takes in a player their name, address
 * and the current gamestatus. adds them to the gamestatus
 * and sends them the dimensions of the grid. finally, adds 
 * them to the current gameboard
 * 
 */
static void playerJoin(addr_t add, char *realName, gamestatus_t *gamestatus);

/********** playerExit **********/
/*
 * a helper method for player request, handles
 * deleting a player and removing them from
 * the gameboard when they exit
 * 
 */
static void playerExit(addr_t add, gamestatus_t *gamestatus);

/********** playerExitLog **********/
/*
 * a helper method for player exit, adds the player's
 * information into the game over log 
 * upon exit;
 * 
 */
static void playerExitLog(player_t *player, gamestatus_t *gamestatus);

/********** announce **********/
/*
 * announce loops through all players in the gamestatus'
 * player set and the spectator (if it isn't null), sending
 * them their current gold status and visibility
 * array from gameboardString
 * 
 * caller provides: valid gamestatus
 * 
 * we return: nothing
 */
static void announce(gamestatus_t *gamestatus);

/********** announceHelper1 **********/
/*
 * loops through each player in the set_t players
 * and prints out their gold status
 * 
 */
static void announceHelper1(void *arg, const char *key, void *item);

/********** announceHelper2 **********/
/*
 * loops through each player in the set_t players
 * and prints out their current, personalized gamemap
 * 
 */
static void announceHelper2(void *arg, const char *key, void *item);


/********** playerFromAddress **********/
/* 
 * returns a player given an address
 * loops through all existing ids to find
 * 
 */
static player_t *playerFromAddress(addr_t add, gamestatus_t *gamestatus);

/********** FUNCTIONS **********/
/********** main **********/
/*
 * main will validate arguments, initialize
 * the gameboard, and call gamemanager
 * 
 */

int
main(const int argc, char *argv[])
{
	log_init(stderr);
		
    if (!(argc == 3 || argc == 2))
    {
        log_v("usage: ./server map.txt [seed]\n");
        return 1;
    }
    //if caller provides a seed
    int seed = -1;
    if (argc == 3)
    {
        scanf(argv[2], "%d", &seed);
    }

    int portNum = message_init(NULL);
    printf("Ready for clients. Portnumber: %d\n", portNum);

    //check to see if mapfile is valid
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        log_s("%s is an invalid file.\n", argv[1]);
        return 1;
    }
    fclose(fp);

    gameInitializer(seed, argv[1]);
    //if game initializer doesn't work
    if (gameboard == NULL)
    {
        log_v("Error creating Gameboard.\n");
        return 1;
    }
    gameManager();
}

/********** GAMEINITIALIZER **********/
/* see function declaration for description */
static void
gameInitializer(int seed, char *filename)
{
    gameboard = gameboardNew(filename, GoldMinNumPiles,GoldMaxNumPiles,GoldTotal,seed);
}

/********** gameManager **********/
/* see function declaration for description */
static void
gameManager()
{
    //and pass into message_loop, so everything can access
    //set up gamestatus
    gamestatus_t *gamestatus = malloc(sizeof(gamestatus_t));
    gamestatus->players = set_new();
    gamestatus->deleted = set_new();
    gamestatus->numPlayers = 0;
    gamestatus->alphabet = 65;
    gamestatus->spectator = NULL;
    gamestatus->log = calloc(MaxPlayers,sizeof(char *));

    //start loop
    message_loop(gamestatus, 0, NULL, NULL, playerRequest);

    //once loop is over, end game and free all memory
    gameOver(gamestatus);
    gameboardDelete(gameboard);
    if(gamestatus->spectator != NULL){
        playerDelete(gamestatus->spectator);
    }
    free(gamestatus->log);
    free(gamestatus);
    message_done();
    log_done();
}

/********** gameOver **********/
/* see function declaration for description */
static void
gameOver(gamestatus_t *gamestatus)
{
	log_v("\nGAME OVER\n");
    //add remaining players in
    set_iterate(gamestatus->players, gamestatus, logRemainingPlayers);
    //final log message should be the length of each log message (13+maxnamelength) + 1 for \n
    //and should have numPlayer rows, with one header saying QUIT GAME OVER
    char *header = "QUIT GAME OVER:\n";
    char endLogMessage[(14+MaxNameLength)*gamestatus->numPlayers+strlen(header) + 1];
    strcpy(endLogMessage,header);
    for(int i = 0; i<gamestatus->numPlayers; i++){
        strcat(endLogMessage,gamestatus->log[i]);
        free(gamestatus->log[i]);
        strcat(endLogMessage,"\n");
    }
    gamestatus->logMessage = endLogMessage;
    set_iterate(gamestatus->players, gamestatus, overAnnounce);
    set_delete(gamestatus->players,NULL);
    set_delete(gamestatus->deleted,NULL);
}

/********** logRemainingPlayers **********/
/* see function declaration for description */
static void
logRemainingPlayers(void *arg, const char *key, void *item)
{
    //if player is not deleted by the end of the game,
    //add them into the exit log
    if (set_find(((gamestatus_t *)arg)->deleted,key)==NULL)
    {
        playerExitLog(item,arg);
    }
}


/********** overAnnounce **********/
/* see function declaration for description */
static void
overAnnounce(void *arg, const char *key, void *item)
{
    addr_t add;
    //if player is not deleted
    if (set_find(((gamestatus_t *)arg)->deleted,key)==NULL)
    {
        add = playerGetAddress(item);
        //send them the ending message
        char *string = ((gamestatus_t *)arg)->logMessage;
        //insert final stats stuff
		message_send(add, string);
    }
}

/********** playerRequest **********/
/* see function declaration for description */
static bool
playerRequest(void *arg, const addr_t from, const char *messageO)
{
	//initalize the log
    //find number of words in the message]
    gamestatus_t *gamestatus = (gamestatus_t *)arg;
    int numWords = 0;
    bool validMessage = true;
    char *message = malloc((strlen(messageO)+1)*sizeof(char));//the max number of bytes from message class
    strcpy(message,messageO);
    for(char *c = message; *c!='\0'; c++){
      //check to see if has invalid characters
      if(!isalpha(*c) && !isspace(*c)){
        validMessage=false;
      }
      else if(isalpha(*c)){
        numWords++;
        //read until you get to the end of the word
        while(isalpha(*(c+1))){
          c++;
        }
      }
    }

    if(validMessage){
        int currentIndex = 0;
        char* request[numWords];
        for(char *c = message; *c!='\0'; c++){
        //start at the start of a word
            if(isalpha(*c)){
                request[currentIndex] = c;
                currentIndex++;
                while(isalpha(*(c+1))){
                c++;
                }
            
                //at end of word, put null
                if(*(c+1)!='\0'){
                    *(c+1) = '\0';
                    c++;
                }
            }
        }

        //process the request
        if (request[0] != NULL)
        {
            if (strcmp(request[0], "PLAY") == 0)
            {
                //if the player doesn't provide a realname
                if (numWords == 1)
                {
                    message_send(from, "QUIT Sorry - you must provide player's name.");
                }
                else if(gamestatus->numPlayers==MaxPlayers){
                    message_send(from, "QUIT Game is full: no more players can join.");
                }
                else
                {
                    //make realName for player and truncate at MaxNameLength
                    char *realName = malloc((MaxNameLength+1)*sizeof(char)); 
                    if(realName == NULL){
                        log_s("error allocating memory for %s", request[1]);
                        return true;
                    }
                    size_t index = 0;
                    while(request[1][index] != '\0' && index < MaxNameLength){
                        index++;
                    }
                    memcpy(realName, request[1], index);
                    realName[index] = '\0';
                    playerJoin(from, realName, gamestatus);
                }
            }
            else if (strcmp(request[0], "KEY") == 0)
            {
                //if is spectator
                if((gamestatus->spectator!=NULL)&&(message_eqAddr(from, playerGetAddress(gamestatus->spectator)))){
                    if (strcmp(request[1], "Q") == 0)
                    {
                        playerDelete(gamestatus->spectator);
                        gamestatus->spectator = NULL;
                        message_send(from, "QUIT Thanks for watching!");
                    }
                    else
                    {
                        message_send(from, "ERROR Key is not part of valid set [Q]");
                    }
                }
                else//if is player
                {
                    //get the player
                    player_t *player = playerFromAddress(from, gamestatus);
                    int gold = 0;

                    //if player is not null (is in the game), process the key movement
                    if (player!=NULL){
                        if (strcmp(request[1], "h") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, -1, 0);
                        }
                        else if (strcmp(request[1], "H") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, -2, 0);
                        }
                        else if (strcmp(request[1], "l") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, 1, 0);
                        }
                        else if (strcmp(request[1], "L") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, 2, 0);
                        }
                        else if (strcmp(request[1], "j") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, 0, 1);
                        }
                        else if (strcmp(request[1], "J") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, 0, 2);
                        }
                        else if (strcmp(request[1], "k") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, 0, -1);
                        }
                        else if (strcmp(request[1], "K") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, 0, -2);
                        }
                        else if (strcmp(request[1], "y") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, -1, -1);
                        }
                        else if (strcmp(request[1], "Y") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, -2, -2);
                        }
                        else if (strcmp(request[1], "u") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, 1, -1);
                        }
                        else if (strcmp(request[1], "U") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, 2, -2);
                        }
                        else if (strcmp(request[1], "b") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, -1, 1);
                        }
                        else if (strcmp(request[1], "B") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, -2, 2);
                        }
                        else if (strcmp(request[1], "n") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, 1, 1);
                        }
                        else if (strcmp(request[1], "N") == 0)
                        {
                            gold = gameboardMovePlayer(gameboard, player, 2, 2);
                        }
                        else if (strcmp(request[1], "Q") == 0)
                        {
                            message_send(from, "QUIT Thanks for playing!");
                            playerExit(from, gamestatus);
                        }
                        else
                        {
                            message_send(from, "ERROR Key is not part of valid set [h, l, j, k, y, u, b, n, Q]");
                        }
                        if(gold>0){
                            char msg[100];
                            sprintf(msg, "GOLD %d %d %d", 0 ,playerGetGold(player),gameboardRemainingGold(gameboard));
                            message_send(from, message);
                        }
                    }
                }
            }
            else if (strcmp(request[0], "SPECTATE") == 0)
            {
                //if spectator already exists
                if (gamestatus->spectator != NULL)
                {
                    message_send(playerGetAddress(gamestatus->spectator), "QUIT You have been replaced by a new spectator.");
                    playerDelete(gamestatus->spectator);
                }

                //update spectator
                gamestatus->spectator = playerNew("spectator", '\0', from, gameboardWidth(gameboard), gameboardHeight(gameboard));
                char grid[20];
                sprintf(grid, "GRID %d %d", gameboardHeight(gameboard), gameboardWidth(gameboard));
                message_send(from, grid);
            }
            //if player makes valid request, announce gameboardstring and everything
            announce(gamestatus);
        }
        else
        {
            //invalid message
            message_send(from, "ERROR invalid request.");
            log_s("invalid message: %s", message);
        }
    }
    else
    {
        //if invalid characters are detected
        message_send(from, "ERROR invalid characters detected.");
		log_s("player sent message with invalid characters; %s", message);
    }
    //add in stopping listening to message if no players
    if(gameboardRemainingGold(gameboard) == 0){
        free(message);
        log_v("all gold collected; stopped listening to players...");
        return true;
    }
    free(message);
    return false;
}

/********** playerJoin **********/
/* see function declaration for description */
static void
playerJoin(addr_t add, char *realName, gamestatus_t *gamestatus)
{
    //update gamestatus
    char nextChar = gamestatus->alphabet;
    gamestatus->alphabet++;
    gamestatus->numPlayers++;

    //create player and add to gameboard
    player_t *player = playerNew(realName, nextChar, add, gameboardWidth(gameboard), gameboardHeight(gameboard));
    gameboardAddPlayer(gameboard, player);

    //send new player message and grid
    char *OK = "OK ";
    char *message = malloc((strlen(OK)+2)*sizeof(char));
    strcpy(message,OK);
    message[3] = nextChar;
    message[4] = '\0';
    message_send(add,message);
    free(message);
    char grid[20];
    sprintf(grid, "GRID %d %d", gameboardHeight(gameboard), gameboardWidth(gameboard));
    message_send(add, grid);

    //add player into player set and log
    char *setChar = malloc(sizeof(char)*2);
    setChar[0] = nextChar;
    setChar[1] = '\0';
    set_insert(gamestatus->players, setChar, player);
    free(setChar);
	log_s("player with name: %s joined the game\n", realName);
    free(realName);
}

/********** playerExit **********/
/* see function declaration for description */
static void
playerExit(addr_t add, gamestatus_t *gamestatus)
{
    //get player to delete, add it into the log
    player_t *toDelete = playerFromAddress(add, gamestatus);
    char *setChar = malloc(sizeof(char)*2);
    setChar[0] = playerGetChar(toDelete);
    setChar[1] = '\0';
    set_insert(gamestatus->deleted,setChar, " ");
    playerExitLog(toDelete, gamestatus);
    free(setChar);
    if(toDelete!=NULL){
        gameboardRemovePlayer(gameboard, toDelete);
    }
    toDelete = NULL;
}

/********** playerExitLog **********/
/* see function declaration for description */
static void 
playerExitLog(player_t *player, gamestatus_t *gamestatus){

    //get char of character
    char *setChar = malloc(sizeof(char)*2);
    setChar[0] = playerGetChar(player);
    setChar[1] = '\0';
    int index = setChar[0];
    //string for that line in log for that player (1- char 1-space 10-number 1-space and name)
    char *goldResult = malloc((13+MaxNameLength+1)*sizeof(char)); 
    //char + ten spaces (for number) + space + name + 1 for null
    sprintf(goldResult,"%s %10d %s",setChar,playerGetGold(player),playerGetName(player));
    goldResult[13+MaxNameLength] = '\0';
    gamestatus->log[index-65] = goldResult;
    free(setChar);
}

/********** gameManager **********/
/* see function declaration for description */
static void
announce(gamestatus_t *gamestatus)
{
    //send gold message
    set_iterate(gamestatus->players, gamestatus->deleted, announceHelper1);
    //send gameboard string
    set_iterate(gamestatus->players, gamestatus->deleted, announceHelper2);
    //if there is a spectator, send it a gold and gameboard string
    if(gamestatus->spectator != NULL){
        char message[100];
        sprintf(message, "GOLD 0 0 %d", gameboardRemainingGold(gameboard));
        message_send(playerGetAddress(gamestatus->spectator), message);
        char *string = gameboardString(gameboard,gamestatus->spectator,true);
        message_send(playerGetAddress(gamestatus->spectator), string);
        free(string);
    }
}

/********** announcehelper1 **********/
/* see function declaration for description */
static void
announceHelper1(void *arg, const char *key, void *item)
{
    addr_t add;
    //if that player hasn't been deleted
    if (set_find(arg,key)==NULL)
    {
        add = playerGetAddress(item);
		char message[100];
		sprintf(message, "GOLD %d %d %d", 0 ,playerGetGold(item),gameboardRemainingGold(gameboard));
		message_send(add, message);
    }
}

/********** announcehelper2 **********/
/* see function declaration for description */
static void
announceHelper2(void *arg, const char *key, void *item)
{
    addr_t add;
    //if player is not deleted
    if (set_find(arg,key)==NULL){
        add = playerGetAddress(item);
        char *string = gameboardString(gameboard,item,false);
		message_send(add, string);
        free(string);
    }
}

/********** playerFromAddress **********/
/* see function declaration for description */
static player_t
*playerFromAddress(addr_t add, gamestatus_t *gamestatus)
{
    //starting at A (ascii 65), loop through set
    int i = 65;
    for (int z = 0; z < gamestatus->numPlayers; z++)
    {
        char c = i;
        char *setChar = malloc(sizeof(char)*2);
        setChar[0] = c;
        setChar[1] = '\0';
        //if player with that char exists in the set, return player
        player_t *player = set_find(gamestatus->players, setChar);
		if (player != NULL) {
			if(message_eqAddr(add, playerGetAddress(player)))
			{
                free(setChar);
			    return player;
			}
        }
        i++;
        free(setChar);
    }
    return NULL;
}

