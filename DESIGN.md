# CS50 21S Nuggets

## Design Spec

In this document we reference the requirements spec and focus on the implementation-independent design decisions.

We focus on the core subset:

- User interface
- Inputs and outputs
- Function decomposition into modules
- Pseudocode for logic/algorithmic flow
- Major data structures
- Testing plan

## User interface

As described in the [Requirements Spec](REQUIREMENTS.md), the server module’s
only interface with the user is on the command-line; it must always have either one or two arguments.
`./server map.txt [seed]`

For example 
`./server map3.txt 2`

## Inputs and outputs

### Input:

Server: The server reads the pathname for a map file to be used for generating the map and, optionally, a seed to be used for random-number generation (must be a positive integer).
During the running of the game, the player will send a message to the server `PLAY real name`

### Key stroke inputs

These are taken in as a request from the client that the server must handle.
We will only move the player if the request to the new location is valid.

- `Q` quit the game
- `h` move left
- `H` move all the way left
- `l` move right
- `L` move all the way right
- `j` move down
- `J` move all the way down
- `k` move up
- `K` move all the way up
- `y` move diagonally up and left
- `Y` move all the way diagonally up and left
- `u` move diagonally up and right
- `U` move all the way diagonally up and right
- `b` move diagonally down and left
- `B` move all the way diagonally down and left
- `n` move diagonally down and right
- `N` move all the way diagonally down and right
	
### Output:

Server: The server will not print anything but will log information into a logfile with the command: `./server 2>server.log map.txt`.
At the end of the game, the server will output a gameover summary.

## Functional decomposition into modules

### Server modules/functions

1. main, which parses arguments, initializes global variables, and starts the game for clients to join.
2. initGameState, which initializes the global variables for the game.
3. parseArgs, which makes user-inputted arguments usable for the program.
4. handleMessage, which delegates the action requested by the client to a helper function.
5. handleSpectate, which is a helper function for `handleMessage` that creates a spectator.
6. handlePlay, which is a helper function for `handleMessage` that creates a player.
7. handleKey, which is a helper function for `handleMessage` that processes player movement and quitting.
8. moveHelper, which is a helper function for `handleKey` that moves the player and processes gold and player interactions too.
9. reposPlayers, which updates all players' grids.
10. resetLiveGrid, which cleans the live grid of all player ID characters.
11. formatName, which formats a user-inputted name by truncating and replacing non-graph and non-blank characters with underscores.
12. sendSummaryMsg, which sends a summary message with player stats.
13. sendGoldMsg, which sends a gold message with updated gold counts.
14. sendGridMsg, which sends a grid message with the grid size.
15. sendDisplayMsg, which sends a display message with the latest version of a player's grid.
16. sendOkMsg, which sends an ok message to confirm that the player joined the game.
17. calcDigits, which calculates the number of digits in a number.
18. findPlayer, which finds a player given their player ID.
19. str2int, which converts a string form of an integer to an actual integer type value.

### Other modules

- grid
- player

### Struct for server
- gameState

## Pseudocode for logic/algorithmic flow

### main
```
parse the user-inputted arguments
save the values
initialize all game global variables
if successfully loaded file for outputting messages
    if message loop run unsuccessful
        print error message
        exit with non-zero value
else
    exit with non-zero value
return zero upon completion
```

### initGameState
```
initialize static grid by loading map
initialize live grid by loading map
update live grid with gold piles
initialize array of players
initialize spectator address
initialize player count
initialize next available player ID
initialize number of nuggets left in game
```

### parseArgs
```
if two arguments present
    save the map value
    if map is not readable
        print error message
        exit with non-zero value
else if three arguments present
    save map value
    if map is not readable
        print error message
        exit with non-zero value
    if seed converts from string to number
        use it for randomizing
    else
        use the process hash
else
    print error message
    exit with non-zero value
return zero upon completion
```

### handleMessage
```
if the request is for spectate
    call spectate handler
else if play
    call play handler
else if key
    call key handler
else
    send error message to client
upon all clients' grids
if no nuggets left
    send gameover summary to all clients
    delete all players
    delete all global variables
    return true upon game end
return false to continue looping
```

### handleSpectate
```
create a spectator
if spectator exists
    replace old spectator
add new spectator
save the spectator's address
send grid size and gold counts messages to them
```

### handlePlay
```
if game not full
    calculate the maximum name length
    if the formatted name is valid
        loop until a valid random spawn position is found
        create the new player and drop them there
        send ok, grid size, and gold counts messages to them
        update all clients' grids
    else
        send a invalid name message
else
    send a full game message
```

### handleKey
```
find the player given the address of the request
if the key was `Q`
    quit the player or spectator with respective messages
if the key was `h`
    call move helper function to move the player one space to the left
else if the key was `H`
    call move helper function to move the player as far left as possible
else if the key was `l`
    call move helper function to move the player one space to the right
else if the key was `L`
    call move helper function to move the player as far right as possible
else if the key was `j`
    call move helper function to move the player one space down
else if the key was `J`
    call move helper function to move the player as far down as possible
else if the key was `k`
    call move helper function to move the player one space up
else if the key was `K`
    call move helper function to move the player as far up as possible
else if the key was `y`
    call move helper function to move the player one space diagonally up and left
else if the key was `Y`
    call move helper function to move the player as far diagonally up and left as possible
else if the key was `u`
    call move helper function to move the player one space diagonally up and right
else if the key was `U`
    call move helper function to move the player as far diagonally up and right as possible
else if the key was `b`
    call move helper function to move the player one space diagonally down and left
else if the key was `B`
    call move helper function to move the player as far diagonally down and left as possible
else if the key was `n`
    call move helper function to move the player one space diagonally down and right
else if the key was `n`
    call move helper function to move the player as far diagonally down and right as possible
else
    send an unknown key message
```

### moveHelper
```
if nuggets still uncollected
    return false
if new location is in bounds of map
    if new location not a wall
        clear the live grid of all player IDs
        if new location is gold pile
            decrement number of gold piles in game
            loop until valid number of nuggets in pile calculated
            update player with nuggets collected
            update nuggets left in game
            move the player to new location
            send updated gold counts to all clients
        else if new location is another player
            switch the locations of the two players
        else if new location is room or passage spot
            move the player to new location
        update all clients' grids
        return true
    else
        return false
return false
```

### reposPlayers
```
update all players' grids and send updated grids to them
if spectator exists
    update spectator's grid
```

### resetLiveGrid
```
remove all players' player IDs from live grid
```

### formatName
```
loop through all characters in name
    if name is not graph or blank
        replace character with '_'
    else
        keep character the same
return true if name valid (false if not)
```

### sendSummaryMsg
```
calculate maximum length for the summary message
append each player's stats for the game onto the summary message
send all players the finalized version of the summary message
if the client exists
    send the summary message to them
```

### sendGoldMsg
```
calculate maximum length for the gold message
add the nuggets collected, total nuggets, and nuggets left numbers into the gold message
send the gold messgae to the client
```

### sendGridMsg
```
calculate number of rows and columns
add the number of rows and columns into the grid message
send the grid message to the client
```

### sendDisplayMsg
```
calculate the maximum length of the display message
add the grid in string form into the display message
send the display message to the client
```

### sendOkMsg
```
calculate the maximum length of the ok message
add the player ID into the ok message
send the ok message to the client
```

### calcDigits
```
start a digit counter to one
loop while the number provided is greater than or equal to ten
    divide the number by ten
    increment the digit counter
return the digit counter
```

### findPlayer
```
loop through all players until finding one that has matching player ID
return NUll value if not found
```

### str2int

`str2int` is from the CS50 Lectures site for the Guess 6 Unit.

## Major data structures

- gameState (in server program):
    - static grid
    - live grid
    - number of gold piles left
    - array of players
    - address for spectator
    - number of players joined
    - next available player ID
    - number of nuggets left in the game

- grid (in grid module):
    - number of rows in grid
    - number of columns in grid
	- Two-dimensional array of characters 

- player (in player module):
	- player ID
	- player name
	- player address
	- column position
	- row position
	- number of gold nuggets collected by the player
	- player-specific grid based on visibility
	- quit boolean

## Testing plan

### Unit testing

Unit testing will be performed for the `player` and `grid` modules.

- Testing the player module
    The grid module is mainly tested with print statements, invoking its different functions to create and manipulate a grid. The following main tests are performed:
    - player creation
    - player movements
    - player getter methods to ensure that they all work properly.
    - update player data, such as number of gold nuggets
    - player deletion

- Testing the grid module
    The grid module is mainly tested with print statements, invoking its different functions to create and manipulate a grid. The following main tests are performed:
	- grid creation
    - grid printing
    - grid updating
	- grid getter methods to ensure that all the methods work properly.
    - grid deletion

### Integration/system testing

All integration/system tests will be run with valgrind to ensure that the varied tests do not produce memory leaks.

Our main tests for this section will be manual testing and automated testing. Both types of testing will make use of the `player` program provided in the shared directory `~/cs50-dev/shared/nuggets/`.

We will perform our tests on a variety of map files provided in the `maps` directory, including a map file of our own design. 

By manually playing the game, the team will test: 
    - edge cases, such as:
        - maxing out player count
		- replacing spectator with new spectator (if one is present)
		- attempting to move past a barrier and bounds of the grid
    - normal (valid) movements
    - random behavior by seed generation

We will perform automated testing to further ensure that movements are handled properly and do not break the game.
       
## Division of Work Plan

Each member will be coding one of the following programs/modules:

- Server program: Ryan
- Player module: Neha
- Grid module: Grace

In case one member finishes their part early, they will help out in implementing the other two program/modules (if necessary).

In regards to testing, each member writing a program/module will be responsible for unit testing their program/module to make later integration/system testing as smooth as possible. (Refer to testing plan section for more information.)

As for integration/system testing the server program, members will collectively work together to produce a driver file to test out various cases for the server.