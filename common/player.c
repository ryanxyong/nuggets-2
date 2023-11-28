/*
 * player.c - CS50 'player' module
 *
 * see player.h for more information
 *
 * Grace Wang, Neha Ramsurrun, Ryan Yong, May 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "grid.h"
#include "mem.h"
#include "message.h"


/* player_t: structure to represent a player, and its contents.*/
typedef struct player{
  char ID;            // player's ID
  char* name;         // name provided by player
  addr_t address;     // player's address
  int col;            // current column location of player
  int row;            // current row location of player
  int numGold;        // gold possessed by player
  grid_t* visGrid;    // player's specific grid
  bool quit;          // player's quit status
}player_t;


/* ******************************************************* */
/* getter methods - see player.h for documentation */
char  player_getID(const player_t* player) { 
  return player ? player->ID : '\0';
}
char* player_getName(const player_t* player)  { 
  return player ? player->name  : NULL;
}
addr_t player_getAddress(const player_t* player){
  return player->address;
}

int player_getCol(const player_t* player)   { 
  return player ? player->col   : 0; 
}

int player_getRow(const player_t* player)   { 
  return player ? player->row   : 0; 
}

int player_getGold(const player_t* player)   { 
  return player ? player->numGold   : 0; 
}

grid_t* player_getVisGrid(const player_t* player) {
  return player ? player->visGrid   : NULL; 
}

bool player_getQuit(const player_t* player) {
  return player->quit;
}


/**************** player_newPlayer ****************/
/* see player.h for documentation */
player_t*
player_newPlayer(char ID, addr_t address, char* name, int col, int row,
                 grid_t* playerGrid)
{
  if (ID == '\0' || name == NULL || col < 0 || row < 0 || playerGrid == NULL){
    return NULL; // player could not be initialized
  }

  player_t* player = mem_assert(malloc(sizeof(player_t)), "player_t");

  player->ID = ID;
  player->name = name;
  player->address = address;
  player->col = col;
  player->row = row;
  player->numGold = 0;
  player->visGrid = playerGrid;
  player->quit = false;

  return player;
}


/**************** player_newSpect ****************/
/* see player.h for documentation */
player_t*
player_newSpect(grid_t* liveGrid, addr_t address)
{
  if (liveGrid == NULL || (message_eqAddr(address, message_noAddr()))) {
    return NULL; // spectator could not be initialized
  }

  player_t* spectator = mem_assert(malloc(sizeof(player_t)), "player_t");
  spectator->address = address;
  spectator->visGrid = liveGrid;
  return spectator;
}


/**************** player_quit ****************/
/* see player.h for documentation */
void
player_quit(player_t* player)
{
  if (player != NULL){
    player->quit = true;
  }
}

/**************** player_delete ****************/
/* see player.h for documentation */
void
player_delete(void* player)
{
  player_t* playerTemp = player;
  if (playerTemp != NULL){

    // then free all malloc'd memory
    if (playerTemp->visGrid != NULL){
      grid_delete(playerTemp->visGrid);
    } 
    if (playerTemp->name != NULL){
      free(playerTemp->name);
    } 
    free(playerTemp);
  }
}

/**************** player_deleteSpect ****************/ 
/* see player.h for documentation */  
void
player_deleteSpect(player_t* spectator)
{
  if (spectator != NULL){
    free(spectator);
  }
}


/**************** player_move ****************/
/* see player.h for documentation */
void
player_move(player_t* player, int col, int row)
{
  if (player != NULL){
    player->row += row;
    player->col += col;
  }
}

/**************** player_addGold ****************/
/* see player.h for documentation */
void
player_addGold(player_t* player, int numGold)
{
  if (player != NULL && numGold > 0){
    player->numGold += numGold;
  }
}
