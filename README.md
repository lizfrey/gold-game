# Gameboard

## Notes:

`Gameboard` has a set of exposed methods defined in [gameboard.h](./gameboard.h), but also has some internal helper methods. The `Gameboard`data structure will hold a `Cell` and total gold, and gold remaining. It provides all functionality for making a `Gameboard`, setting up the gold piles, holding players, moving players, and creating the visible array (the part of the grid that the player can see) for each player. 

### Testing

To test, call `make unittest`.

### Other assumptions:
- There can only be one player with a specific address on the board at a time 
(i.e. player A can be on one square and one square only).
