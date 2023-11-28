# CS50 21S Nuggets

## Server Implementation Spec
### Team name: grn-rng

## Data structures

We use three data structures: 
1. `gameState` structure containing the static version of the provided map, live version of the provided map (with gold piles and players), the number of piles left, the array of players, the spectator's address, number of players joined, next available player ID, and the number of nuggets left.

    ```
    static struct {
      grid_t* staticGrid;
      grid_t* liveGrid;
      int numPiles;

      player_t* players[27];
      addr_t spectatorAddr;
      int playerCount;
      char playerID;
      int nuggetsLeft;
    } gameState;
    ```

2. `grid` data structure holding the number of rows, number of columns, and a two-dimensional array of characters:

    ```
    typedef struct grid {
        int numRows;
        int numCols;
        char** map;   
    } grid_t;   
    ```
    
3. `player` data structure storing the player ID, player name, custom grid based on visibility, number of gold nuggets collected, `addr_t` type address, current (column, row)location and player's quit status.

    ```
    typedef struct player{
      char ID;            
      char* name;             
      addr_t address;     
      int col;            
      int row;            
      int numGold;        
      grid_t* visGrid;    
      bool quit;          
    } player_t;
    ```

All structures start empty and are initialized when the game starts.

## Control flow: pseudocode for overall flow and for each of the functions

### main

The main function simply calls `parseArgs`, `log_init` `message_init`, `message_loop`, `messageHandle`, `makeSummary`, `message_done`, and `log_done` then exits zero.

Pseudocode for `main`:
```
initialize a map filename character pointer to NULL
initialize a seed integer to zero
call parseArgs on the arguments
initialize the gameState struct
initialize constant for the timeout value to zero
initialize a port number to zero
create a constant character pointer to the output file pathname
initialize a pointer to the file by opening the file at that path for reading
if the message initialization of the file is greater than zero
    if the message loop runs and experiences a fatal error
        print an error message
        close the file
        exit with a non-zero value
    stop the message module
else
    close the file
    exit with a non-zero value
close the file
return zero
```

### initGameState

`initGameState` takes in a string representing the pathname for a map file and initializes information on the ongoing game contained within the gameState struct. This function does not return anything.

Pseudocode for `initGameState`:
```
initialize the static grid to the provided map as is with no additions
initialize the live grid to the provided map
modify the live grid with the gold piles dropped
initialize the array of players with NULL
initialize the spectator address to nothing
initialize the player count to 0
initialize the next available player ID to 'A'
initialize the number of nuggets left to the starting gold total
```

### parseArgs

`parseArgs` takes in arguments from the command line and extracts them into the function parameters. This function returns zero only if successful. 

Pseudocode for `parseArgs`:
```
if have two arguments
    pass in the map.txt file
    if the map pathname is not readable
        print error message
        exit with non-zero value
else if have three arguments
    pass in the map.txt file
    if the map pathname is not readable
        print error message
        exit with non-zero value
    if seed can be converted to int
        set random generation seed to the seed provided
    else
        set random generation seed to process hash
else
    print an error message
    return with non-zero exit status
return with exit status of zero upon completion
```

### handleMessage

`handleMessage` takes in the address where the message is from and the message itself. The function takes the messages and splits it up into the message category and action (if applicable), it will then do the action corresponding to meet the needs of the message, whether it be handling a new spectator, player, or player movement. At the end it updates all players grids and stops the game and print the summary as necessary. This function returns true if the game is over and false if the game is not over.

Pseudocode for `handleMessage`:
```
if first word is "SPECTATE "
    call the spectate handler
else if "PLAY "
    save the second part of the message to pass into the play handler
    call the play handler
else if "KEY "
    save the second part of the message to pass into the key handler
    call the key handler
else
    send an error message on invalid action to client

update all clients' grids

if there are no nuggets left
    send the summary to all clients
    
    loop through all players
        delete each player
        
    delete the spectator
    delete the static grid
    delete the live grid
    return true to stop game

return false to continue game
```

### handleSpectate

`handleSpectate` takes in the address where the request was from. The function creates a new spectator (replacing an old one if necessary). This function does not return anything.

Pseudocode for `handleSpectate`:
```
create a spectator
if spectator exists
    delete old spectator
    send quit message to old spectator
insert the spectator into the array of players
set the spectator address in the gameState struct
send GRID message to the spectator
send GOLD message to the spectator
```

### handlePlay

`handlePlay` takes in the address where the request was from and the name provided by the user. The function calls formatName and then finds a valid place to spawn the player and updates all other players and spectator with the new player in the game. This function does not return anything.

Pseudocode for `handlePlay`:
```
if capacity not full
    set a length integer to the maximum name length
    if the length of the inputted name is less than the maximum supported name length
        set the length integer to the length of the name
    
    create a temporary name character pointer
    if the formatted name is valid
        get the character for the next available player ID
        find the number rows in the grid
        find the number of columns in the grid
        set a boolean for valid position to false
        create a column position
        create a row position
        randomize a value with time as the seed
        while the position is not valid
            randomize the column position
            randomize the row position
            get the character at that position in the grid
            if the position is a room spot ('.')
                set the valid position boolean to true
        create the new grid for the player
        update the new grid to account for visibility
        create the new player
        insert the player into the array of players
        increment the player count
        increment the current player ID
        send OK message with player ID character to player
        send GRID message to player
        send GOLD message to player
        update the grids of all players to show new playewr
    else
        send QUIT message for invalid name
else
    send QUIT message for full game
```

### handleKey

`handleKey` takes in the address where the request was from and the keystroke provided by the user. The function calls player_quit if the quit is desired or moveHelper if the user provided a movement keystroke. If the keystroke does not fit any of the criteria, the function sends an error message on the unrecognized keystroke. This function does not return anything.

Pseudocode for `handleKey`:
```
initialize a player pointer to NULL
loop through all players
    create a temporary player
    if the temporary player's address is the same as message requester
        player is equal to the temporary player pointer
        break out of the loop
if  'Q'
    if the message requester is from the spectator address
        send QUIT message to the spectator
        set the old spectator's address in the gameState struct to none
    else
        send QUIT message to the player
    call player_quit on the player
else
    if 'h'
        call moveHelper with movement left
    else if 'H'
        while moveHelper with movement left valid, call moveHelper
    else if 'l'
        call moveHelper with movement right
    else if 'L'
        while moveHelper with movement right valid, call moveHelper
    else if 'j'
        call moveHelper with movement down
    else if 'J'
        while moveHelper with movement down valid, call moveHelper
    else if 'k'
        call moveHelper with movement up
    else if 'K'
        while moveHelper with movement up valid, call moveHelper
    else if 'y'
        call moveHelper with movement diagonally up, left
    else if 'Y'
        while moveHelper with movement diagonally up, left valid, call moveHelper
    else if 'u'
        call moveHelper with movement diagonally up, right
    else if 'U'
        while moveHelper with movement diagonally up, right valid, call moveHelper
    else if 'b'
        call moveHelper with movement diagonally down, left
    else if 'B'
        while moveHelper with movement diagonally down, right valid, call moveHelper
    else if 'n'
        call moveHelper with movement diagonally down, right
    else if 'N'
        while moveHelper with movement diagonally down, right valid, call moveHelper
    else
        send an error message to client regarding unknown keystroke
```

### moveHelper

`moveHelper` takes a player, a change in column location, and change in row location. The function finds the new adjusted coordinates of the player after moving and checks if it is valid first (in bounds and not a wall) and then does the necessary of moving the player depending on if a gold pile is found (factoring new gold count), another player is found (move both players), or a normal room spot is found. This function returns true if successful and false if not.

Pseudocode for `moveHelper`:
```
create a temporary column integer
create a temporary row integer
if number of nuggets left is zero
    return false
if the column and row location is in the bounds of the map
    create a spot character based on the coordinates provided
    if the spot is not a wall character
        remove the previous location of the players' characters from all players' grids
        if the spot is a gold pile
            decrement the number of gold piles
            create a new integer with the number of nuggets in the pile
            create a valid number boolean and set to false
            while the number is invalid
                randomly generate a value for the number of gold in the pile
                if the number of gold piles is not zero
                    if there is at least one gold in the pile
                        set valid number boolean to true
                else
                    set valid number boolean to true
            update the player's nuggets collected
            update the game's number of nuggets left
            move the player to the new column and row location
            send the gold message to the player
            loop through all players
                if they are not the player who collected gold
                    send the gold message to them updating the nuggets left
            if the spectator exists
                send the gold message to the spectator updating the nuggets left
        else if the spot is an alpha
            create a temporary player holding the player at that location
            move the current player to the new location
            move the temporary player to the new (displaced) location
        else if the spot is a room spot or a passage spot
            move the current player to the new location
        loop through all players
            update each player's grid
        return true
    else
        return false
return false
```

### reposPlayers

`reposPlayers` takes no parameters. The function calls `grid_update` on the current player's grid to keep it up to date with any changes made from a message request. This function does not return anything.

Pseudocode for `reposPlayers`:
```
loop through all players
    create a temporary player for the current player
    call grid_update to update the current player's grid
    create a character pointer version of the grid
    send the DISPLAY message with the grid string to the current player
    free the grid string
if the spectator exists
    create a character pointer version of the live grid
    send the DISPLAY message with the grid string to the current spectator
    free the grid string
```

### resetLiveGrid

`resetLiveGrid` takes no parameters. The function calls `grid_remove` on the current player to update the current player's grid if a movement is to occur. This prevents the same player from existing on the current player's grid in multiple locations at once. This function does not return anything.

Pseudocode for `resetLiveGrid`:
```
loop through all players
    create a temporary player for the current player
    remove the current player from the live grid
```

### formatName

`formatName` takes in the name inputted by the player, the maximum name length, and the result character pointer. The functions formats the name to meet the requirements of truncating it to the maximum name length and replacing characters that are both non-blanks and non-graphs with '_ '. This function returns true or false depending on the validity of the name.

Pseudocode for `formatName`:
```
create a valid name boolean to hold whether or not the name is valid

loop through the entire length of the name
    if the name is not a graph or a blank
        set that character to an '_'
    else
        set the name validity boolean to true
        set the result character array to the character at the index
return valid name boolean
```

### sendSummaryMsg

`sendSummaryMsg` takes no parameters. The functions creates the formatted message for sending to all clients at the end of the game by going through each player in alphabetical order and adding their contents to a running array. This function does not return anything.

Pseudocode for `sendSummaryMsg`:
```
create a fixed string for the base of the summary message
calculate the number of digits in the gold total
determine the maximum length of the summary
    (look at number of rows and number of columns and gold total digits)
alloc memory for a character array with the maximum length
set the base string into the summary string
loop through all players
    create a temporary player for the current player
    calculate the length for the string of current player's stats
    allocate memory for a temporary string to hold the current player's stats
    set the current player's stats into the temporary string
    concatenate the temporary string onto the summary string
    free the temporary string
loop through all players
    create a temporary player for the current player
    send the complete summary to the player
if the spectator exists
    send the complete summary to the spectator
free the summary string
```

### sendGoldMsg

`sendGoldMsg` takes in the address where the request was from, the number of nuggets just collected, the total number of nuggets, and the number of nuggets left. The function creates and sends a message updating the client on the gold counts. This function does not return anything.

Pseudocode for `sendGoldMsg`:
```
create a length integer for the length of the message (GOLD)
calculate the number of digits for nuggets collected
calculate the number of digits for total nuggets
calculate the number of digits for the nuggets left
add number of digits for all numbers to the length integer
allocate memory for a result string with the length integer
set the message base (GOLD) and nuggets collected, total nuggets, and nuggets left into the result string
send the message to the client
free the result string
```

### sendGridMsg

`sendGridMsg` takes in the address where the request was from, the number of rows, and number of columns. The function creates and sends a message updating the client on the grid size. This function does not return anything.

Pseudocode for `sendGridMsg`:
```
create a length integer for the length of the message (GRID)
calculate the number of digits for number of rows
calculate the number of digits for number of columns
add number of digits for all numbers to the length integer
allocate memory for a result string with the length integer
set the message base (GRID) and number of rows and columns into the result string
send the message to the client
free the result string
```

### sendDisplayMsg

`sendDisplayMsg` takes in the address where the request was from and the grid in string form. The function creates and sends a message updating the client on the current live grid visible for the client. This function does not return anything.

Pseudocode for `sendDisplayMsg`:
```
create a length integer for the length of the message (DISPLAY)
calculate the length of the grid in string form
add grid string length to the length integer
allocate memory for a result string with the length integer
set the message base (DISPLAY) and grid string
send the message to the client
free the result string
```

### sendOkMsg

`sendOkMsg` takes in the address where the request was from and the player ID character. The function creates and sends a message confirming the player ID to the client. This function does not return anything.

Pseudocode for `sendOkMsg`:
```
create a length integer for the length of the message (OK) and player ID character
allocate memory for a result string with the length integer
set the message base (OK) and player ID character
send the message to the client
free the result string
```

### calcDigits

`calcDigits` takes in an integer. The function calculates the number of digits in the integer. This function returns the number of digits.

Pseudocode for `calcDigits`:
```
initialize a digit counter with one
while the number is greater than or equal to ten
    divide the number by ten
    increment the digit counter
return the digit counter
```

### findPlayer

`findPlayer` takes in the player ID character. The function looks for any players with the ID. This function returns the player if found.

Pseudocode for `findPlayer`:
```
loop through all players
    if the current player's ID matches the character of interest
        return the player
return NULL
```

### str2int

`str2int` is from the CS50 Lectures site for the Guess 6 Unit.

## Other modules

### grid


`grid_new` is passed two parameters, the number of columns and the number of rows our 2D arrays will have. It returns a `grid_t` struct.

Pseudocode for `grid_new`:
```
allocate memory for grid structure
assigns the number of columns passed in (c)
assigns the number of rows passed in (r)
allocated memory for a 2D array with r arrays
for each array in the 2D array
    allocate memory for c number of characters
for each position in the 2D array
    assign it to a space (' ')
return grid
```

`grid_load` is passed a file path with a valid map. Returns the `grid_struct` with the map loaded.

Pseudocode for `grid_load`:
``` 
opens the map text file
if file is not null
    find number of rows by finding number of lines
    find number of columns by finding the length of the longest line
close file
create new grid with number of rows and columns in the map
if grid is valid
    open file
    make an integer to keep track of which row
    while read line is not at EOF
        for each character in line
            add to current array in the row we are on
        increment row tracker
    close the file
    return grid
return NULL
```

`grid_update` is passed three `grid_t` structs, one is the static grid (contains initial map), one is the live grid (contains map representing current state of the game) and one is the player's grid (contains map of what player sees and knows), a player's character ID, the player's new row position, and the player's new column position.

Pseudocode for `grid_update`:
```
if any grids are null
    exit 1
if the new row and column positions are valid
    set the character at that position in the live grid map to the player's character ID
    calculate the player's visibility at the new position
    set the character at that position in the player grid map to '@'
```

`grid_remove` is passed three `grid_t` structs, one is the static grid, one is the live grid and one is the player's grid, a row position, and a column position.

Pseudocode for `grid_remove`:
```
if any grid is null
    exit 1
if the row and column positions are valid   
    remove set character in live grid back the character in that position in static grid
```

`grid_toString` takes a `grid_t` struct and turns it into a String to return.

Pseudocode for `grid_toString`:
```
if grid is null
    return null
create a variable to store string
create a variable to keep track of position in string
for each array in the 2D array in grid
    for each character in the array
        if it is not NULL
            add character to string
            increment position
    add '\n' to string
    increment position
return string
```

`grid_setGold` takes a `grid_t` struct, a min number of gold piles, and a max number of gold piles as parameters and assigns positions in the grid as gold spots. Returns number of gold piles made.

Pseudocode for `grid_setGold`:
```
if grid is null or min is greater than max
    return 0
else 
    get a random number of piles between min and max gold piles
    for each pile we need
        generate a random column position within numCols
        generate a random row position within numRows
        if character in position row and column is a room spot
            change to a gold spot
        else 
            retry
    return number of gold piles made
```

`grid_delete` takes a `grid_t` struct and frees memory allocated to it.

Pseudocode for `grid_delete`:
```
for each array in 2D map
    free array
free 2D map
free grid
```

`grid_getChar` takes a `grid_t` struct and the row and column position and returns the character in that position.

Pseudocode for `grid_getChar`:
```
if grid is not NULL
    return the character at position [row][column] in the grid's map
```

`grid_calcVisibility` is passed in the live grid, static grid, player grid, player's row position, and player's column position.

Pseudocode for `grid_calcVisbility`:
```
if any of the grids are null
    exit 1
loop through each row position in grid
    loop through each column in grid
        if the current row and column position is visible from the player's position
            row and column position in player's map is set to the character in the row and column position in the live map
        else if the character at row and position was visible but now is no longer visbile
            row and column position in player's map is set to the character in the row and column position in the static map
```

`grid_isVisible` takes a `grid_t` struct, a row position, a column position, the player's row position, and the player's column position. The function will return true if the row and column position is visible from the player's row and column position. Else it will return false.

Pseudocode for `grid_isVisible`:
```
if the point is in the same column as player
    call helper method to check the rows and return
    
if the point is in the same row as player
    call helper method to check columns and return
    
calculate slope of line from player position to row and column position
calculate according to slope, what would be the value of the row when the column is zero

if the point is to the left of the player
    for each column between player and column position
        calculate row according to slope and intercept
        if calculated row is a whole number
            if character at column and calculated row is not a room spot
                return false
        else
            find floor of calculated row
            find ceiling of calculated row
            if character at column and floor row and ceiling row is not a room spot
                return false
if the point is to the right of the player
    for each column between player and column position
        calculate row according to slope and intercept
        if calculated row is a whole number
            if character at column and calculated row is not a room spot
                return false
        else
            find floor of calculated row
            find ceiling of calculated row
            if character at column and floor row and ceiling row is not a room spot
                return false
if the point is above the player
    for each row between player and column position
        calculate column according to slope and intercept
        if calculated column is a whole number
            if character at calculated column and row is not a room spot
                return false
        else
            find floor of calculated column
            find ceiling of calculated column
            if character at row and floor column and ceiling column is not a room spot
                return false
if the point is below the player
    for each row between player and column position
        calculate column according to slope and intercept
        if calculated column is a whole number
            if character at calculated column and row is not a room spot
                return false
        else
            find floor of calculated column
            find ceiling of calculated column
            if character at row and floor column and ceiling column is not a room spot
                return false
return true
```

`grid_rowVisibility` is called to iterate through columns if a point's position is in the same row as the player and check if the point is visible. It returns true if the point is visible, false if not.

Pseudocode for `grid_rowVisibility:
```
if the point is above the player
        loop from the player up to the point
            if it is not a room spot
                return false
    if the point is below the player
        loop from the player down to the point
            if it is not a room spot
                return false
    return true
```

`grid_colVisibility` is called to iterate through rows if a point's position is in the same column as the player and check if the point is visible. It returns true if the point is visible, false if not.

Pseudocode for `grid_colVisibility`:
```
if the point is to the left of the player
        loop from the player to the point by decrementing column position
            if it is not a room spot
                return false
    if the point is to the right of the player
        loop from the player to the point by incrementing column position
            if it is not a room spot
                return false
    return true
```

`grid_getMap`, `grid_getRows`, and`grid_getCols` are all getter methods that take in a `grid_t` struct. The three are all very similar and return their respective variables.

Pseudocode for `getMap`, `getRows`, and `getCols`:
```
if grid struct is valid
    return the respective variable
else
    return NULL
```

### player

`player_newPlayer` initializes a new player by taking in the ID, address, name, row and column locations, and the grid of the player. 

Pseudocode for `player_newPlayer`:
```
if any of the function parameters are invalid
    return NULL

allocate memory for new player
set player ID to given ID
set player name to given name
set player's address to given address
set player's column to given column
set player's row to given row
initialize player's number of gold nuggets to 0
set player's visible grid to given grid
set player's quit status to false
return player
```

`player_newSpect` simply initializes a new spectator using the `player` struct. It takes as parameter the liveGrid of the game and the address of the spectator. 

Pseudocode for `player_newSpect`:
```
if any function parameters are not valid
    return NULL
    
allocate memory for new spectator
set spectator's address to given address
set spectator's visible grid to liveGrid
return spectator
```

`player_quit` sets the given player's quit status to true, indicating that the player has quit the game and can thus be taken off the liveGrid.

Pseudocode for `player_quit`:
```
if player not NULL
    set player's quit status to true
```

`player_delete` frees all memory allocated for the given player, namely the `player_t` pointer, the player's grid and the player's name.

Pseudocode for `player_delete`:
```
if player is not NULL
    if player's grid is not NULL
        call grid_delete to free player's grid
    if player's name is not NULL 
        free player's name
    free player
```

`player_deleteSpect` takes in a pointer to a spectator and frees memory allocated to it.

Pseudocode for `player_deleteSpect`:
```
if spectator not NULL
    free spectator
```

`player_move` is a function that takes in a pointer to a player, and two integers col and row. These are values by which the player's column and row values increment or decrement.

Pseudocode for `player_move`:
```
if player not NULL
    add col to player's col value
    add row to player's row value
```

`player_addGold` takes in a pointer to a player and an integer by which to increase the player's number of gold nuggets.

Pseudocode for `player_addGold`:
```
if player not NULL and gold nuggets > 0
    increase player's gold
```

`player_getID`, `player_getName`, `player_getAddress`, `player_getCol`, `player_getRow`, `player_getGold`, `player_getVisGrid` and `player_getQuit` are all getter methods that take in a `player_t` struct and return their respective data member, if valid.

## Detailed function prototypes and their parameters

### server
Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `server.c` and is not repeated here.

```c
static void initGameState(char* mapFilename);
static int parseArgs(const int argc, char* argv[],
                      char** mapFilename, int* seed);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static void handleSpectate(addr_t from);
static void handlePlay(addr_t from, const char* content);
static void handleKey(addr_t from, const char* content);
static bool moveHelper(player_t* player, int col, int row);
static void reposPlayers(void);
static void resetLiveGrid(void);
static bool formatName(const char* name, int length, char* result);
static void sendSummaryMsg(void);
static void sendGoldMsg(addr_t from, int n1, int n2, int n3);
static void sendGridMsg(addr_t from, int n1, int n2);
static void sendDisplayMsg(addr_t from, char* gridStr);
static void sendOkMsg(addr_t from, char c);
static int calcDigits(int n);
static player_t* findPlayer(char c);
static bool str2int(const char string[], int* number);
```

### player
Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `player.h` and is not repeated here.

```c
char player_getID(const player_t* player);
char* player_getName(const player_t* player);
addr_t player_getAddress(const player_t* player);
int player_getCol(const player_t* player);
int player_getRow(const player_t* player);
int player_getGold(const player_t* player);
grid_t* player_getVisGrid(const player_t* player);
bool player_getQuit(const player_t* player);
player_t* player_newPlayer(char ID, addr_t address, char* name, int col, int row, grid_t* playerGrid);
player_t* player_newSpect(grid_t* liveGrid, addr_t address);
void player_quit(player_t* player);
void player_delete(player_t* player);
void player_deleteSpect(player_t* spectator);
void player_move(player_t* player, int col, int row);
void player_updateNuggets(player_t* player, int numNuggets);
```

### grid
Detailed descriptions of each function’s interface is provided as a paragraph comment prior to each function’s declaration in `grid.h` and is not repeated here.

```c
grid_t* grid_new(int numRows, int numCols);
grid_t* grid_load(const char* mapFile);
void grid_update(grid_t* staticGrid, grid_t* liveGrid, grid_t* playerGrid, char id, int row, int col );
void grid_remove(grid_t* staticGrid, grid_t* liveGrid, grid_t* playerGrid, int row, int col );
char* grid_toString(grid_t* grid);
int grid_setGold(grid_t* liveGrid, int minGoldPiles, int maxGoldPiles);
void grid_delete(grid_t* grid);
char grid_getChar(grid_t* grid, int row, int col);
char** grid_getMap(grid_t* grid);
int grid_getRows(grid_t* grid);
int grid_getCols(grid_t* grid);
static void grid_calcVisibility(grid_t* staticGrid, grid_t* liveGrid, grid_t* playerGrid, int rowPlayer, int colPlayer);
static bool grid_isVisible(grid_t* staticGrid, int row, int col, int rowPlayer, int colPlayer);
static bool grid_rowVisibility(grid_t* staticGrid, int row, int col, int rowPlayer, int colPlayer);
static bool grid_colVisibility(grid_t* staticGrid, int row, int col, int rowPlayer, int colPlayer);
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by variants of the `mem_assert` functions, which result in a message printed to stderr and a non-zero exit status. 

All code uses defensive-programming tactics to catch and exit (using variants of the mem_assert functions), e.g., if a function receives bad parameters.
 
## Testing plan
### Unit Testing

- The player module will be tested with a small C driver that invokes its different functions with different arguments. The module will be tested mainly with print statements to ensure that data members of the player struct are being updated correctly.

- The grid module will be tested with a small C driver that invokes its different functions with different arguments. The module will be tested mainly with print statements to ensure that the grid struct is being updated correctly.

### Integration/System Testing

- The server program will be tested with scripts to check 
    - if arguments are correctly parsed with various argument counts and 
    invalid arguments 
    - if messages are tokenized properly (action and rest of the message) 
    - if gameState items are initialized/updated correctly 
    - if actions are delegated to the correct functions
    - if the location of the player changes properly with key presses
    - if spectators are deleted and removed completely when they quit or are
being replaced
    - if players are only removed from the live grid (but their information
stays intact)
    - if the names that players provide are processed properly (truncation and
replacement with _ )
    - if the `makeSummary` function produces a proper summary. 

    Valgrind tests will also be conducted on the server program with various 
    pre-determined messages that will be sent to the server.

- We will also use bot mode in the provided `player` (client) executable to test the server by setting the `playerName` as `bot`. This will allow us to test the server's handling of randomly generated key strokes.