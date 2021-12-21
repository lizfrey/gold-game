# Nuggets Design Specification
BBQ - Team 8 - [Elizabeth Frey](mailto:elizabeth.w.frey.24@dartmouth.edu), [Isaac Feldman](mailto:isaac.c.feldman.23@dartmouth.edu), [John Gnibus](mailto:john.m.gnibus.24@dartmouth.edu)

## User interface
The server’s only interface with the user is on the command-line; it must always have two arguments.

```bash
./server map.txt [seed]
```

## Inputs and Outputs 

### Input:
All input comes from the command line arguments. `map.txt` will provide the map file and the optional seed  will provide the input for a random number generator. Other inputs will come from messages from the clients. 
 
### Output: 
The server will update all clients whenever any player moves or gold is collected. When the number of gold nuggets left reaches zero, it will create and output a tabular summary. Throughout the game, it will log useful information that can be saved in a log file. For example ```./server 2>server.log map.txt```
 
## Functional decomposition into modules
We anticipate the following modules or functions:
- `main`
- `gameInitializer`
- `gameManager` (translated user messages into game state changes; maintains a valid game state)
	- `playerRequest`
	- `playerJoin`
	- `playerExit`
	- `announce`
		- `announceHelper`
	- `playerFromAddress`
- `gameBoardString`
- `gameOver`
	- `gameOverHelper`
 
## Pseudo code for logic/algorithmic flow
1. Validate command-line arguments
2. If the optional seed is provided, pass it to `srand(seed)`. If no seed is provided, use `srand(getpid())` to produce random behavior.
3. Load in the map.txt file to the `Gameboard` struc. Assume that the file is going to be valid.
4. Call `gameInitializer` to generate a number of gold piles between `GoldMinNumPiles` and `GoldMaxNumPiles` on random room spots
5. Call `message_init` to initialize the network. Announce the port number
6. Call `message_loop` to handle input and incoming messages
	1. If a player connects, and the number of current player is less than `MaxPlayers`, accept the player
		- save the address from which the message (PLAY real name)
		- If the “real name” part of the message is empty, send “QUIT Sorry - you must provide the player's name.”
		- Store the name of the player in a new `Player`
		- Add the `player` into the `Gameboard`.
		- Send “OK [char]” to the player if they successfully connected. where [char] is this player's letter in the set {A, B, ... Z}
	2. If there are already `MaxPlayers`, send “QUIT Game is full: no more players can join.” back to the client
	3. If a spectator connects, accept the spectator
		- If there is already a spectator, make it quit and replace as a new `Player`, saving the address
	4. For every new client, after they have connected, immediately send them
		- `GRID nrows ncols` where `nrows` and `ncols` are the dimensions of the `Gameboard`
		- `GOLD n p r`, where `n` is the number of gold nuggets they just collected, `p` is their purse of total nuggets, and `r` is the number of nuggets remaining
		- `DISPLAY\nstring`, where string is a multi-line textual representation of the grid. 
7. For each “KEY k” message from a player, check to see if the key is valid. If it is invalid, send the user a message. Otherwise, 
	1. Update the player’s position in the `Gameboard`
	2. Send all players a message
		- `GOLD n p r`, where `n` is the number of gold nuggets they just collected, `p` is their purse of total nuggets, and `r` is the number of nuggets remaining
		- "DISPLAY\nstring’, which will only show the `player` the part of the board that is visible to them
	3. If a client quits the game, that client’s symbol is removed from the `gameBoard`, all users are updated with the new string representation of the `Gameboard`, and they are sent a thank you message
8. When the number of gold nuggets reaches zero, call `gameOver` to display final scores. 

## Dataflow through modules

1. `main` will parse parameters and passes them to the `gameInitializer`
2. `gameInitializer` will generate the gold piles and set up the map
3. `gameManager` will access the map, and receive messages from clients, updating the `Gameboard` in the process using `playerRequest`. It will regularly send the current `Gameboard` to all clients thorugh `announce`.
4. `gameOver` will take the final gamestate and output the final scores to the user through stdout

## Major data structures
- `Gameboard` data structure will hold `Cells`s, total gold, and gold remaining
- `Cell` data structure will hold gold, players, and walls (or `NULL` if they're not present)
- `Player` struct (holds connection address, name, number of gold pieces, and their custom visibility array)
 
## Testing plan
- Detailed unit testing for each module
	- Unit tests will be written by the composer of a module
	- Unit tests must pass before merge with main
	- Unit tests must be documented
- Integration testing after each module is complete, and with every merge with the main branch
	- For example, testing gameBoardString and gameManager together frequently
- Stress testing and fuzz testing will occur simultaneously
	- We will test it with the maximum number of players and a spectator 
	- First, each player will make normal moves, and we will test to make sure the server can handle of them
	- Next, we will test with a single player but send the server erratic movement requests (random keys, valid and invalid)
	- Finally, we will combine the two, testing the server with the max number of players, all sending erratic keys

