/*
 * server.c - server portion of the nuggets game that allows for clients to
 * join in and handles game logic.
 *
 * usage - User must provide map filename and an optional seed.
 *
 * Grace Wang, Neha Ramsurrun, Ryan Yong, May 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "message.h"
#include "player.h"
#include "grid.h"

/************ global constants *************/
static const int MaxNameLength = 50;    // maximum character count for a name
static const int MaxPlayers = 26;       // maximum player count
static const int GoldTotal = 250;       // starting gold count
static const int GoldMinNumPiles = 10;  // minimum gold piles count
static const int GoldMaxNumPiles = 30;  // maximum gold piles count
static const char solidRock = ' ';      // character for the solid rock
static const char horiBound = '-';      // character for the horizontal boundary
static const char vertBound = '|';      // character for the vertical boundary
static const char cornerBound = '+';    // character for the corner boundary
static const char goldPile = '*';       // character for the gold pile
static const char roomSpot = '.';       // character for the room spot
static const char passageSpot = '#';    // character for the passage spot

/************ global types ************/
static struct {           // only visible to server.c
  grid_t* staticGrid;     // starting grid based on provided map file
  grid_t* liveGrid;       // ongoing version of grid with players and gold
  int numPiles;           // number of gold piles left to find
  
  player_t* players[27];  // all players and one spectator in a game
  addr_t spectatorAddr;   // address for the spectator
  int playerCount;        // number of players in game so far
  char playerID;          // next available player ID
  int nuggetsLeft;        // number of nuggets left to find
} gameState;

/************ function prototypes **************/
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

/************ main *************/
int
main(const int argc, char* argv[])
{
  char* mapFilename = NULL; // path to map provided by user
  int seed = 0; // seed value for randomization
  parseArgs(argc, argv, &mapFilename, &seed); // parses user-inputted arguments

  initGameState(mapFilename); // initializes global variables for the game

  const float timeout = 0;
  int port = 0;
  const char* logPath = "logs/run.log"; // output path for actions in server
  FILE* fp = fopen(logPath, "w");

  if ((port=message_init(fp)) > 0) { // check if port is valid
    if (! message_loop(NULL, timeout, NULL,
                        NULL, handleMessage)) { // check if fatal error in loop
      fprintf(stderr, "Fatal error: unable to continue looping\n");
      fclose(fp);
      exit(2);
    }
    message_done();
  } else { // runs if message loop ended cleanly
    fclose(fp);
    exit(1);
  }

  fclose(fp);
  return 0;
}

/*********** initGameState **************/
/* Initializes all variables to be used by the game throughout the program run.
 *
 * Caller provides:
 *  mapFilename: valid pathname to a map file
 *
 * We do:
 *  Load the necessary maps and starting values for count and ID.
 */
static void
initGameState(char* mapFilename)
{
  gameState.staticGrid = grid_load(mapFilename); // load initial map
  gameState.liveGrid = grid_load(mapFilename); // load initial map
   // drop gold piles in live grid
  gameState.numPiles = grid_setGold(gameState.liveGrid,
                                    GoldMinNumPiles, GoldMaxNumPiles);
  
  // initialize each player in the array of players
  for (int i = 0; i < MaxPlayers + 1; i++) { // loops through all players
    gameState.players[i] = NULL;
  }
  
  gameState.spectatorAddr = message_noAddr();
  gameState.playerCount = 0;
  gameState.playerID = 'A'; // starting player's ID
  gameState.nuggetsLeft = GoldTotal; // nuggets left is total at start
}

/************ parseArgs *************/
/* Takes the user provided arguments and makes them into usable values for the
 * program.
 *
 * Caller provides:
 *  Number of arguments
 *  Arguments array
 *  Pointer to mapFilename: pointer to valid pathname to a map file
 *  Pointer to seed: pointer to a number for randomization
 *
 * We do:
 *  Assign values to mapFilename and seed.
 *
 * We return:
 *  0 if successful
 *  Non-zero if not successful
 */
static int
parseArgs(const int argc, char* argv[], char** mapFilename, int* seed)
{
  FILE* fp = NULL;
  if (argc == 2) { // check if two arguments provided
    *mapFilename = argv[1];
    if ((fp=fopen(*mapFilename, "r")) == NULL) { // check if map file readable
      fprintf(stderr, "error: invalid map filename.\n");
      exit(1);
    }
    srand(getpid());
    fclose(fp);
  } else if (argc == 3) { // check if three arguments provided
    *mapFilename = argv[1];
    if ((fp=fopen(*mapFilename, "r")) == NULL) { // check if map file readable
      fprintf(stderr, "error: invalid map filename.\n");
      exit(1);
    }
    fclose(fp);

    // check if convert argument into seed integer successful
    if (str2int(argv[2], seed)) {
      if (*seed>0){
        srand(*seed);
      } else { // provided seed is an integer but not positive
        fprintf(stderr, "error: invalid seed. must be positive integer.\n");
        exit(1);
      }
      
    } else { // provided seed is not an integer
      fprintf(stderr, "error: invalid seed. must be positive integer.\n");
      exit(1);
    }
    
  } else { // runs if invalid number of arguments provided
    fprintf(stderr, "usage: %s map.txt [seed]\n", argv[0]);
    exit(1);
  }
  return 0;
}

/************ handleMessage **************/
/* Takes the message from the server and reads it to delegate actions to helper
 * functions, whether it be to create a spectator or player or move a player.
 *
 * Caller provides:
 *  optional arg: if needed
 *  from: the address of the client who made the request
 *  message: the request from the client
 *
 * We do:
 *  Depending on the request from the client pick the right function calls.
 *
 * We return:
 *  false if game not over
 *  true if game over
 */
static bool
handleMessage(void* arg, const addr_t from, const char* message)
{
  if (strncmp(message, "SPECTATE", strlen("SPECTATE")) == 0) {
    // run if client requests spectate
    handleSpectate(from);
  } else if (strncmp(message, "PLAY ", strlen("PLAY ")) == 0) {
    // run if client requests play
    const char* content = message + strlen("PLAY ");
    handlePlay(from, content);
  } else if (strncmp(message, "KEY ", strlen("KEY ")) == 0) {
    // run if client requests key
    const char* content = message + strlen("KEY ");
    handleKey(from, content);
  } else { // runs if client request invalid
    message_send(from, "ERROR invalid action provided");
  }

  reposPlayers(); // updates each player's grid

  if (gameState.nuggetsLeft == 0) {
    sendSummaryMsg(); // sends game summary to all players

    // deletes all players
    for (int i = 0; i < gameState.playerCount; i++) { // loops through players
      player_delete(gameState.players[i]);
    }

    player_deleteSpect(gameState.players[MaxPlayers]);
    grid_delete(gameState.staticGrid); // delete game static grid
    grid_delete(gameState.liveGrid); // delete game live grid
    return true;
  }
  return false;
}

/************* handleSpectate ***************/
/* Handles the request from a client for a new spectator.
 *
 * Caller provides:
 *  from: the address of the client who made the request
 *
 * We do:
 *  Create a new spectator (also replace the old one if applicable).
 */
static void
handleSpectate(addr_t from)
{
  player_t* spectator = player_newSpect(gameState.liveGrid, from);
  // check if spectator exists
  if (! message_eqAddr(gameState.spectatorAddr, message_noAddr())) {
    player_deleteSpect(gameState.players[MaxPlayers]); // delete old spectator
    message_send(gameState.spectatorAddr,
                  "QUIT You have been replaced by a new spectator.");
  }
  gameState.players[MaxPlayers] = spectator; // add new spectator
  gameState.spectatorAddr = from; // save spectator's address
  sendGridMsg(from, grid_getRows(gameState.staticGrid),
              grid_getCols(gameState.staticGrid));
  sendGoldMsg(from, 0, 0, gameState.nuggetsLeft);
}

/************* handlePlay **************/
/* Handles the request from a client for a new player.
 *
 * Caller provides:
 *  from: the address of the client who made the request
 *
 * We do:
 *  Create a new player and place them at a random place on the grid.
 */
static void
handlePlay(addr_t from, const char* content)
{
  // check if max player count has not been exceeded
  if (gameState.playerCount < MaxPlayers) {
    int length = MaxNameLength;

    // check if provided name is shorter than maximum
    if (strlen(content) < MaxNameLength) {
      length = strlen(content);
    }

    // allocates memory for new player's name
    char* name = calloc((length + 1), sizeof(char));

    if (formatName(content, length, name)) { // check if name is valid
      char id = gameState.playerID;

      // get nr and nc from live grid to generate random player location
      int numRows = grid_getRows(gameState.liveGrid);
      int numCols = grid_getCols(gameState.liveGrid);

      bool validPos = false; // false until calculated player location is valid
      int col = 0;
      int row = 0;

      while (!validPos) { // runs until a valid position is found
        col = rand() % numCols;
        row = rand() % numRows;
        char pos = grid_getChar(gameState.liveGrid, row, col);

        if (pos == roomSpot) { // check if player location is room spot
          validPos = true;
        }
      }

      // create new player
      grid_t* playerGrid = grid_new(numRows, numCols);
      grid_update(gameState.staticGrid, gameState.liveGrid, playerGrid,
                  id, row, col); // update player's grid with visibility
      player_t* player = player_newPlayer(id, from, name, col, row, playerGrid);
      
      // insert new player into the array of players
      gameState.players[gameState.playerCount] = player;
      gameState.playerCount++; // increment player count
      gameState.playerID++; // move onto the next available player ID

      sendOkMsg(from, id);
      sendGridMsg(from, grid_getRows(gameState.staticGrid),
                  grid_getCols(gameState.staticGrid));
      sendGoldMsg(from, 0, 0, gameState.nuggetsLeft);
      reposPlayers(); // updates the positions of all players and visibility
    } else { // runs if name is invalid
      message_send(from, "QUIT Sorry - you must provide player's name.");
    }
  } else { // runs if game is at maximum player capacity
    message_send(from, "QUIT Game is full: no more players can join.");
  }
}

/************* handleKey *************/
/* Handles the request from a client for a keystroke.
 *
 * Caller provides:
 *  from: the address of the client who made the request
 *
 * We do:
 *  Either quit or move the player depending on the keystroke.
 */
static void
handleKey(addr_t from, const char* content)
{
  player_t* player = NULL;

  // finds the player based on address
  for (int i = 0; i < gameState.playerCount; i++) { // loops through players
    player_t* temp = gameState.players[i];
    // checks if addresses are equal
    if (message_eqAddr(from, player_getAddress(temp))) {
      player = temp;
      break;
    }
  }

  if (strcmp("Q", content) == 0) { // quits
    // checks address is spectator's address
    if (message_eqAddr(from, gameState.spectatorAddr)) {
      message_send(from, "QUIT Thanks for watching!");
      gameState.spectatorAddr = message_noAddr();
    } else { // runs if not a spectator
      message_send(from, "QUIT Thanks for playing!");
    }
    player_quit(player);
  } else if (strcmp("h", content) == 0) { // moves left
    moveHelper(player, -1, 0);
  } else if (strcmp("H", content) == 0) { // moves far left
    while (moveHelper(player, -1, 0)) {}
  } else if (strcmp("l", content) == 0) { // moves right
    moveHelper(player, 1, 0);
  } else if (strcmp("L", content) == 0) { // moves far right
    while (moveHelper(player, 1, 0)) {}
  } else if (strcmp("j", content) == 0) { // moves down
    moveHelper(player, 0, 1);
  } else if (strcmp("J", content) == 0) { // moves far down
    while (moveHelper(player, 0, 1)) {}
  } else if (strcmp("k", content) == 0) { // moves up
    moveHelper(player, 0, -1);
  } else if (strcmp("K", content) == 0) { // moves far up
    while (moveHelper(player, 0, -1)) {}
  } else if (strcmp("y", content) == 0) { // moves diagonally up, left
    moveHelper(player, -1, -1);
  } else if (strcmp("Y", content) == 0) { // moves far diagonally up, left
    while (moveHelper(player, -1, -1)) {}
  } else if (strcmp("u", content) == 0) { // moves diagonally up, right
    moveHelper(player, 1, -1);
  } else if (strcmp("U", content) == 0) { // moves far diagonally up, right
    while (moveHelper(player, 1, -1)) {}
  } else if (strcmp("b", content) == 0) { // moves diagonally down, left
    moveHelper(player, -1, 1);
  } else if (strcmp("B", content) == 0) { // moves far diagonally down, left
    while (moveHelper(player, -1, 1)) {}
  } else if (strcmp("n", content) == 0) { // moves diagonally down, right
    moveHelper(player, 1, 1);
  } else if (strcmp("N", content) == 0) { // moves far diagonally down, right
    while (moveHelper(player, 1, 1)) {}
  } else {
    message_send(from, "ERROR unknown keystroke");
  }
}

/************* moveHelper *************/
/* 
 *
 * Caller provides:
 *  player: player being moved
 *  col: change in x direction along columns
 *  row: change in y direction along rows
 *
 * We do:
 *  Check the destination spot.
 *  If it is a gold pile collect gold, another player switch spots, and
 *  room/passage spot move.
 *
 * We return:
 *  false if the movement is invalid
 *  true if the movement is valid
 */
static bool
moveHelper(player_t* player, int col, int row)
{
  int tempCol = player_getCol(player) + col; // destination column location
  int tempRow = player_getRow(player) + row; // destination row location
  
  if (gameState.nuggetsLeft == 0) { // check if game is over
    return false;
  }

  // check if destination location is in bounds of grid
  if (tempCol >= 0 && tempRow >= 0 && tempCol < grid_getCols(gameState.staticGrid)
      && tempRow < grid_getRows(gameState.staticGrid)) {
    // gets character at destination location
    char spot = grid_getChar(gameState.liveGrid, tempRow, tempCol);
    
    // check if destination location is not wall spot
    if (spot != horiBound && spot != vertBound && spot != cornerBound && spot != solidRock) {
      resetLiveGrid(); // removes all players temporarily from live grid
      if (spot == goldPile) { // check if destination location is gold pile

        // calculate random number of gold nuggets for pile
        gameState.numPiles -= 1;
        int nuggetsInPile = 0;
        bool validNum = false;
        while (!validNum) { // runs until valid number of gold calculated
          nuggetsInPile = rand() % gameState.nuggetsLeft + 1;
          if (gameState.numPiles != 0) { // check if piles left is not zero
            // check if at least one gold in pile
            if (((gameState.nuggetsLeft - nuggetsInPile)/gameState.numPiles) > 0) {
              validNum = true;
            }
          } else { // runs if no gold piles left
            validNum = true;
          }
        }
        player_addGold(player, nuggetsInPile);
        gameState.nuggetsLeft -= nuggetsInPile;
        
        player_move(player, col, row);

        sendGoldMsg(player_getAddress(player), nuggetsInPile,
                    player_getGold(player), gameState.nuggetsLeft);

        // loop through players + send gold message to other players in server
        for (int i = 0; i < gameState.playerCount; i++) {
          // check if player is not same as one at current index
          if (gameState.players[i] != player) {
            sendGoldMsg(player_getAddress(gameState.players[i]), 0,
                        player_getGold(gameState.players[i]),
                        gameState.nuggetsLeft);
          }
        }

        // check if spectator exists
        if (! message_eqAddr(gameState.spectatorAddr, message_noAddr())) {
          sendGoldMsg(gameState.spectatorAddr, 0, 0, gameState.nuggetsLeft);
        }
      } else if (isalpha(spot) != 0) {
        // check if destination spot is player
        // flip the positions of the two players
        player_t* temp = findPlayer(spot);
        player_move(player, col, row);
        player_move(temp, col*-1, row*-1);
      } else if (spot == roomSpot || spot == passageSpot) {
        // check if destination is room/passage spot
        player_move(player, col, row);
      }
      
      // loop through players + update the grids of all players
      for (int i = 0; i < gameState.playerCount; i++) {
        player_t* playerTemp = gameState.players[i];
        grid_update(gameState.staticGrid, gameState.liveGrid,
                    player_getVisGrid(playerTemp), player_getID(playerTemp),
                    player_getRow(playerTemp), player_getCol(playerTemp));
      }

      return true;
    } else { // runs if destination location is wall spot
      return false;
    }
  }
  return false;
}

/************ reposPlayers ***************/
/* Updates each player's grids and sends the message to the clients.
 *
 * We do:
 *  Loop through all players and update their grids (visibility included) and
 *  send it off to the clients.
 */
static void
reposPlayers(void)
{
  // updates the grids of all players and sends grids to players
  for (int i = 0; i < gameState.playerCount; i++) { // loops through players
    player_t* playerTemp = gameState.players[i];
    grid_update(gameState.staticGrid, gameState.liveGrid,
                player_getVisGrid(playerTemp), player_getID(playerTemp),
                player_getRow(playerTemp), player_getCol(playerTemp));
    char* gridStr = grid_toString(player_getVisGrid(playerTemp));
    sendDisplayMsg(player_getAddress(playerTemp), gridStr);
    free(gridStr);
  }

  // check if spectator exists
  if (! message_eqAddr(gameState.spectatorAddr, message_noAddr())) {
    char* gridStr = grid_toString(gameState.liveGrid);
    // send the live grid to spectator
    sendDisplayMsg(gameState.spectatorAddr, gridStr);
    free(gridStr);
  }
}

/************ resetLiveGrid ***************/
/* Cleans off all player IDs off the live grid.
 *
 * We do:
 *  Loop through all players and take their ID symbol off of the live grid.
 */
static void
resetLiveGrid(void)
{
  // removes all players from the live grid
  for (int i = 0; i < gameState.playerCount; i++) { // loops through players
    player_t* playerTemp = gameState.players[i];
    grid_remove(gameState.staticGrid, gameState.liveGrid,
                player_getVisGrid(playerTemp),
                player_getRow(playerTemp), player_getCol(playerTemp));
  }
}

/************ formatName **************/
/* 
 *
 * Caller provides:
 *  name: the name as provided in the argument
 *  length: the length of the name 
 *  result: the destination for the formatted name
 *
 * We do:
 *  Format the name by making all characters which are not graphs or blanks
 *  with underscores and transferring the rest of the normal characters.
 *
 * We return:
 *  false if the name is invalid
 *  true if the name is valid
 */
static bool
formatName(const char* name, int length, char* result)
{
  bool validName = false;

  // adds characters of name with correct format to result string
  for (int i = 0; i < length; i++) { // loops through name
    // check if name is not a graph and blank
    if ((isgraph(name[i]) == 0) && (isblank(name[i]) == 0)) {
      validName = true;
      result[i] = '_';
    } else { // runs if name is graph or blank
      validName = true;
      result[i] = name[i];
    }
  }
  return validName;
}

/************ sendSummaryMsg ****************/
/* Creates and sends the summary message for the game
 *
 * We do:
 *  Append the stats of each player onto a running total summary string for
 *  sending to clients later.
 */
static void
sendSummaryMsg(void)
{
  char* msgType = "QUIT GAME OVER:\n";
  int goldDigits = calcDigits(GoldTotal) + 1;
   // +2 for new line and character
  int maxLength = strlen(msgType) + (gameState.playerCount * (2 + goldDigits + MaxNameLength));
  char* summary = calloc(maxLength + 1, sizeof(char));
  sprintf(summary, "%s", msgType);

  // appends player stats to running summary string
  for (int i = 0; i < gameState.playerCount; i++) { // loops through players
    player_t* player = gameState.players[i];
    // +4 for the character and white spaces
    int statLength = goldDigits + strlen(player_getName(player)) + 4;
    char* tempStats = calloc(statLength + 1, sizeof(char));
    sprintf(tempStats, "%c %*d %s\n", player_getID(player), goldDigits,
            player_getGold(player), player_getName(player));
    strcat(summary, tempStats);
    free(tempStats);
  }
  
  // send summary to all players
  for (int i = 0; i < gameState.playerCount; i++) { // loops through players
    player_t* player = gameState.players[i];
    message_send(player_getAddress(player), summary);
  }

  // check if spectator exists
  if (! message_eqAddr(gameState.spectatorAddr, message_noAddr())) {
    // send summary to spectator
    message_send(gameState.spectatorAddr, summary);
  }
  free(summary);
}

/************ sendGoldMsg **************/
/* Creates and sends the gold status message to the client.
 *
 * Caller provides:
 *  from: the address of the client who made the request
 *  n1: the nuggets just collected
 *  n2: the total number of nuggets
 *  n3: the nuggets to be found still
 *
 * We do:
 *  Take the numbers and append them together to get the gold message to send
 *  to clients.
 */
static void
sendGoldMsg(addr_t from, int n1, int n2, int n3)
{
  // takes number of digits of all numbers to allocate memory
  int length = strlen("GOLD");
  int d1 = calcDigits(n1) + 1; // +1 accounts for space
  int d2 = calcDigits(n2) + 1;
  int d3 = calcDigits(n3) + 1;
  length += d1 + d2 + d3 + 1; // +1 accounts for NULL terminator
  char* result = calloc(length, sizeof(char*));
  sprintf(result, "%s %d %d %d", "GOLD", n1, n2, n3);
  message_send(from, result); // send message to client
  free(result);
}

/************ sendGridMsg **************/
/* Creates and sends the grid message to the client.
 *
 * Caller provides:
 *  from: the address of the client who made the request
 *  n1: number of rows
 *  n2: number of columns
 *
 * We do:
 *  Take the numbers and append them together to get the grid message to send
 *  to clients.
 */
static void
sendGridMsg(addr_t from, int n1, int n2)
{
  // takes number of digits of all numbers to allocate memory
  int length = strlen("GRID");
  int d1 = calcDigits(n1) + 1; // +1 accounts for space
  int d2 = calcDigits(n2) + 1;
  length += d1 + d2 + 1;
  char* result = calloc(length, sizeof(char*));
  sprintf(result, "%s %d %d", "GRID", n1, n2);
  message_send(from, result); // send message to client
  free(result);
}

/************ sendDisplayMsg **************/
/* Creates and sends the display message to the client.
 *
 * Caller provides:
 *  from: the address of the client who made the request
 *  gridStr: the string version of the grid
 *
 * We do:
 *  Take the grid string and append it onto the message to send to clients.
 */
static void
sendDisplayMsg(addr_t from, char* gridStr)
{
  // takes a string version of entire grid to allocate memory
  int length = strlen("DISPLAY\n");
  int gridLength = strlen(gridStr);
  length += gridLength + 1;
  char* result = calloc(length, sizeof(char*));
  sprintf(result, "%s%s", "DISPLAY\n", gridStr);
  message_send(from, result); // send message to client
  free(result);
}

/************ sendOkMsg **************/
/* Creates and sends the ok message to the client.
 *
 * Caller provides:
 *  from: the address of the client who made the request
 *  c: the player ID character for a new player
 *
 * We do:
 *  Take the character and append it onto the message to send to clients.
 */
static void
sendOkMsg(addr_t from, char c)
{
  // takes player ID to allocate memory
  int length = strlen("OK ") + 2; // +2 for char and NULL terminator
  char* result = calloc(length, sizeof(char*)); 
  sprintf(result, "%s %c", "OK", c);
  message_send(from, result); // send message to client
  free(result);
}

/************ calcDigits ************/
/* Calculates the number of digits in a number.
 *
 * Caller provides:
 *  n: the number to calculate the number of digits for
 *
 * We do:
 *  Divide the number by ten repeatedly while incrementing the digit count per
 *  loop.
 *
 * We return:
 *  The number of digits in the number provided.
 */
static int
calcDigits(int n)
{
  int digits = 1;   // count of digits
  while (n >= 10) { // calculates total number of digits
    n /= 10;
    digits++;
  }
  return digits;
}

/************ findPlayer ************/
/* Finds the player based on their player ID.
 *
 * Caller provides:
 *  c: the player ID character
 *
 * We do:
 *  Loops through all players and then returns the player if found.
 *
 * We return:
 *  the player with the given ID
 *  NULL if player not found
 */
static player_t*
findPlayer(char c)
{
  for (int i = 0; i < gameState.playerCount; i++) { // loops through players
    if (player_getID(gameState.players[i]) == c) { // check if ID found
      return gameState.players[i]; // return found player
    }
  }
  return NULL;
}

/* ***************** str2int ********************** */
/*
 * This function is from the CS50 Lectures site for Guess 6 Unit
 *
 * Convert a string to an integer, returning that integer.
 * Returns true if successful, or false if any error. 
 * It is an error if there is any additional character beyond the integer.
 * Assumes number is a valid pointer.
 */
static bool
str2int(const char string[], int* number)
{
  // The following is one of my favorite tricks.
  // We use sscanf() to parse a number, expecting there to be no following
  // character ... but if there is, the input is invalid.
  // For example, 1234x will be invalid, as would 12.34 or just x.
  char nextchar;
  return (sscanf(string, "%d%c", number, &nextchar) == 1);
}