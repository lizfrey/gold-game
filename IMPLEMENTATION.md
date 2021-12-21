# Nuggets Implementation Specification
BBQ - Team 8 - Elizabeth Frey, Isaac Feldman, John Gnibus

## Detailed pseudocode:

### Nuggets Class Methods
---
We anticipate the following methods: 

1. `main`
2. `gameInitializer`
3. `gameManager` (translated user messages into game state changes; maintains a valid game state)
4. `playerRequest`
5. `playerJoin`
6. `playerExit`
7. `announce`
8. `gameboardString`
9. `checkVisibility`
10. `gameOver`
And helper methods to assist with announce and gameOver.

#### `main`

`main` will first validate the arguments, checking the number of arguments and that `map.txt` is a pathname for a map file. If an optional `[seed]` is provided, it must be a positive integer. On any error, it will print an error message and exit non-zero. As said in the Requirements spec, if the `seed` is provided, it will call `srand(seed)`; otherwise, it will call `srand(getpid())`. It will load the `map.txt` into a file, and pass that file and the random number to `gameInitializer` to generate the `Gameboard`. Next, it will call `message_init` to initialize the message module and get the address, which it will announce to all users using `message_send`. Finally, it will call `gameManager`.

#### `gameInitializer`

`gameInitializer` will create a `Gameboard` by calling `gameboardNew`.

#### `gameManager`

`gameManager` sets up the `gamestatus` struct, which holds information about the game. It then calls `message_loop` to start listening for messages. When `message_loop` is complete, it will call `gameOver`.

#### `gameOver`

`gameOver` will do two things:

1. Send messages to all connected players (if any) with finals scores and text thanking them for playing
2. Free all dynamically allocated memory by calling each modules delete method.

#### `playerRequest`

`playerRequest` will take in a message from a player, validates it, and then deal with the players request accordingly. It will parse the request into an array, and based on the first word in the array, proceed accordingly.
- If the player makes a valid move (a valid KEY message), it will call `gameboardMovePlayer` and pass in the arguments to move the player accordingly. 
- If the player wants to exit (KEY Q), it will call `playerExit`
- If the player is just joining the game (PLAY realname), it will call `gameboardPlayerJoin`. If the player does not pass in a valid message, it will only send it an error message.
- If a spectator wants to join, add them into the spectator spot in `gamestatus`
At the end of every request, it will call `announce`, which will send each player a GOLD message and an updated version of the gameboard

#### `playerJoin`

`playerJoin` will create a new `Player` using `playerNew`. It will pass the new `Player` to `gameboardAddPlayer`. Then, it will send the player using `message_send` the OK message, the dimensions of the GRID in the format GRID nrows ncols, and GOLD n p r, where n is the number of gold nuggets they just collected, p is their purse of total nuggets, and r is the number of nuggets remaining.

#### `playerExit`

`playerExit` will call `gameboardRemovePlayer`, and send the player a thank you note for playing, and call `playerDelete` to reallocate the memory used for the `Player`.

#### `announce`

`announce` will loop through all the players in the `Gameboard`, and send a message to each one using `message_send`. It uses the two announce helper methods to send the GOLD and gameboardString messages.


### `Gameboard` Class Methods:
---
We anticipate the following methods: 

1. `gameboardNew`
2. `gameboardDelete`
3. `gameboardString`
4. `gameboardAddPlayer`
5. `gameboardMovePlayer`
6. `gameboardRemovePlayer`
7. `isVisible`
8. `gameboardRemainingGold`
9. `gameboardWidth`
10. `gameboardHeight`

And helper methods to assist with creating and editing the `Cell`s at each spot in the `Gameboard` struct.

#### `gameboardNew`

`gameboardNew` will create a new instance of a `Gameboard` struct, using the `map.txt` file passed in to set it up. It will create a `Cell` in each part of the `Gameboard`, with each parameter pointing to something or `NULL` accordingly. It will properly allocate data, which will need to be deleted later.

#### `gameboardDelete`

`gameboardDelete` will go through every cell in the `Gameboard`, free them, and then free the entire `Gameboard` struct. 

#### `gameboardString`

In order to generate a player-specific string to send to the client, we will take in a `Player` pointer as a parameter. `checkVisibility` is called to update the `visibilityArray` of the `Player`. The `visibilityArray` is then translated into a string to be sent back to the client to indicate what to display.

#### `gameboardAddPlayer`

`gameboardAddPlayer` will take in a `Player`, and place it at a random spot in the `Gameboard` by setting the `Player` pointer at that `Cell` in the `Gameboard` to point to the new `Player`.

#### `gameboardMovePlayer`

`gameboardMovePlayer` will take in a `Player` , and then will set the new `Cell`'s pointer to that `Player`, and set the old `Cell`'s `Player` pointer to `NULL`, effectively moving the player. If the `Player` lands on a piece of gold, it will remove the gold from the `Gameboard` and update the `Player`'s amount of gold accordingly.

#### `gameboardDeletePlayer`

`gameboardDeletePlayer` will set the `Player`'s current `Cell`'s `Player` pointer to `NULL` and return a pointer to the player so that it can be deleted. 

#### `isVisible`

Each players `visibilityArray` member will populate using 2D raycasting. `checkVisibility` will take in a `Player` and a (x,y) coordinate corresponding to a `Cekk` in the `Gameboard`. From the players position, a line will be drawn to the (x,y) coordinate. If the `Player` can currently see the `Cell` at (x,y), this will return a representation of the `Cell`. Else it will return `NULL`.

#### `gameboardRemainingGold`, `gameboardWidth`, `gameboardHeight`

These getter methods will return each variable in gameboard respectively.

### `Player` Class Methods:
---
We anticipate the following methods: 

1. `playerNew`
2. `playerDelete`
3. `playerGetAddress`
4. `playerGetGold`
5. `playerGetGold`
6. `getVisibleArray`
7. `playerGetName`
8. `playerGetAddress`
9. `playerDelete`
10. Helper methods to check the celltype

#### `playerNew`

`playerNew` will allocate memory for a new `struct Player`. It will take set the initial `int gold` to zero and save the `int address` that is passed in. 

#### `playerDelete`

`playerDelete` is responsible for returning any memory allocated in `playerNew`

#### `getAddress`

`playerGetAddress` will return the saved `int address` of the player.

#### `getGold`

`playerGetGold` will return the player's current amount of gold.

#### `addGold`

`playerAddGold` will increase the player's current amount of gold by the `int gold` passed in. 

#### `getVisibleArray`

`getVisibleArray` will return a pointer to the player's current `visibleArray`

#### `updateVisibleArray`

`updateVisibleArray` will loop through every spot in the `Player`'s current `visibleArray` and call `checkVisibility`, updating the `visibleArray`. If `checkVisibility` returns `NULL` (not in the `Player`'s current field of view'), but that spot in the `Player`'s `visibleArray` is not `NULL` (the `Player` has seen it before), it will not update the spot in `visibleArray`. Otherwise, it will update `visibleArray` to whatever `checkVisibility` returns. 

#### `playerGetName`

`playerGetName` will return a `char *name` with the player's current `name`

#### `playerGetAddress`

`playerGetAddress` will return a `addr_t *add` with the player's address

#### `playerDelete`

`playerDelete` will delete the player and free all allocated memory

## Definitions:

**Visibility**: the **visibility** of a player refers to the the part of the map that the player can see. If the player has seen an area of the map before, it is able to see this part of the map for the entire game. The player, however, can only see gold pieces if the pieces are currently in its field of view. 

## Data Structures
`Gameboard` struct will be defined as a global variable to avoid having to pass it to each function and check if its null. `Gameboard` data structure will hold a `Cell` and total gold, and gold remaining. We will use the following methods:

- `gameboardNew`
- `gameboardDelete`
- `gameboardString`
- `addPlayer`
- `movePlayer`
- `removePlayer`
- `checkVisibility`

`Cell` struct will hold the amount of gold at that spot, players, and walls (or NULL or 0 or false if they're not present). The `Cell` struct will be contained within  `Gameboard.c`. 

`Player` struct will hold the players connection address, name, number of gold pieces, a boolean to show whether its a spectator or not, and a `visibilityArray`, storing the parts of the grid that are currently visible to the player. For spectators, the `visibilityArray` will show the entire map. We will use the following functions:

- `playerNew`
- `playerDelete`
- `getAddress`
- `getGold`
- `addGold`
- `getVisibleArray`
- `updateVisibleArray`
- `playerGetName`
- `playerGetAddress`
- `playerDelete`

## Security and Privacy Properties

- Practice good coding principles.
- All dynamic memory will be freed before program termination.
- Network traffic will be trusted (for conveniences sake)
- Map files will be trusted (per REQUIREMENTS)
- Command line input will not be trusted and must be validated before being used 

## Error Handling and Recovery

- Graceful handling of errors equates to carefully crashing the server.
- If mallocs fail, we have bigger problems, in the case we will fail and return to mail thread which will exit

## Resource Management and Persistent Storage

- The server will write log files through piping stderr into a file. The server will not create any persistent files. 

## Style and Practices

Functions definitions will be written in camel case in the following format:
```c
int exampleFunc(char *test)
{
return 0;
}
```