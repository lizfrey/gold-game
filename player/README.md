# Player

## Notes:

`Player` has a set of exposed methods defined in [player.h](./player.h), but also has some internal helper methods. The `Player`data structure will hold a all the information about a player, including the part of the grid it can see, its character, its name, its address, and the total number of gold pieces it picked up. 

### Testing

To test, call `make unittest`.

### Other assumptions:
- There can only be one player with a specific address on the board at a time 
(i.e. player A can be on one square and one square only).
