# Testing

## Unit Testing

For both the  `Player` and `Gameboard` modules, we initially tested them using unit tests before we integrated them into `Server`. For `Player`, we used the file playerUnitTest.c, and for `Gameboard`, we used gameboardUnitTest.c. In these files, we tested both functionality and memory allocation. 

### Player

In playerUnitTest.c, we first made a `Player`, and checked to see that each parameter passed into `playerNew` was properly stored. Becuase this was unit testing, we did not test the visibility array in this part of the testing.

### Gameboard

In gameboardUnitTest.c, we made a `Gameboard`, tested inserting and removing players, and made sure that `gameboardString` worked properly with `isVisible` for both `Players` and spectators (a type of `Player` that can see everything on the board). Although this was in our unit test, we had to include `Player` to test out important functionality like `gameboardMovePlayer` and `gameboardString`. 

## Integration Testing

Our main integration testing began when we tested out `Player` in `Gameboard` during the gameboardUnitTest.c, but we mainly focused on integrating our modules together when testing our `Server`. This had to be done manually because we could not figure out how to code client server messages.

Initially, we just tested to make sure that the player could move and collect gold properly. During this, we had to make a lot of changes to `isVisible` based on specific edge cases. One example of an error that we fixed through integration testing was that our players could see all the way down hallways, which was not what we intended. It was also through integration testing that we fixed errors in the `updateVisibilityArray` funciton in `Player`. Additionally, we made a decision to make it so that `Players` cannot walk through each other, so we had to make some changes to `gameboardMovePlayer` to support this functionality. 

## Boundary Cases

Once we got our `Server`, `Gameboard`, and `Player` to function as intended, we started to work on more challenging maps like challenging.txt. It was through these maps that we realized that `gameboardMovePlayer` did not account for if a hallway was on the edge of the map, so we added a checker in the movement to prevent segmentation faults. Additionally, we tested this out on our own, new, newMapVeryNarrow.txt to make sure that our player was able to move through a very narrow map. 
