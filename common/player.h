/* 
 * player.h - header file for CS50 'player.c' module
 *
 * This module defines the player_t type and functions that are 
 * used to create and manipulate "players" for the CS50 Nuggets Final Project. 
 * The player struct contains:
 *      player's ID
 *      player's name
 *      player's address
 *      player's current (col,row) location
 *      player's number of gold nuggets collected  
 *      player's custom grid based on visibility
 *      player's quit status
 *
 * Grace Wang, Neha Ramsurrun, Ryan Yong, May 2021
 */

#ifndef __PLAYER_H
#define __PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "message.h"
#include "grid.h"

/***********************************************************************/
/* player_t: struct to represent a player, and its contents.
 */
typedef struct player player_t;

/* getter methods */
char player_getID(const player_t* player);
char* player_getName(const player_t* player);
addr_t player_getAddress(const player_t* player);
int player_getCol(const player_t* player);
int player_getRow(const player_t* player);
int player_getGold(const player_t* player);
grid_t* player_getVisGrid(const player_t* player);
bool player_getQuit(const player_t* player);

/**************** player_newPlayer ****************/
/* Allocate and initialize a new player_t structure for a player.
 * Caller provides:
 *   ID: a character ID
 *   address: player's address
 *   name: must be a non-null pointer to malloc'd memory.
 *   col,row: non-negative integers pertaining to location of player
 *   playerGrid: non-null pointer to a grid_t
 *
  * We return:
 *   pointer to new player_t, or NULL on any error.
 *
 * Caller is responsible for:
 *   later calling player_delete with the returned pointer.
 */
player_t* player_newPlayer(char ID, addr_t address, char* name, int col, int row,
                           grid_t* playerGrid);


/**************** player_newSpect ****************/
/* Allocate and initialize a new player_t structure for a spectator.
 * Caller provides:
 *   liveGrid: non-null pointer to a grid_t
 *   address: spectator's address
 *
 * We return:
 *   pointer to new player_t, or NULL on any error.
 *
 * Caller is responsible for:
 *   later calling player_deleteSpect with the returned pointer.
 */
player_t* player_newSpect(grid_t* liveGrid, addr_t address);


/**************** player_quit ****************/
/* Set given player's quit status to true, indicating that player has quit 
 * the game.
 *
 * Caller provides
 *   pointer to a player_t provided by player_newPlayer.
 *
 * We do:
 *   set player->quit to true
 */
void player_quit(player_t* player);


/**************** player_delete ****************/
/* Delete a player_t structure created by player_newPlayer or 
 * player_newSpect.
 *
 * Caller provides
 *   pointer to a player_t provided by player_newPlayer or player_newSpect.
 *
 * We do:
 *   we free the malloc'd name of the player, if not NULL
 *   we call grid_delete on the player's grid, if not NULL.
 *   we free pointer to the player_t provided, if not NULL
 */
void player_delete(void* player);


/**************** player_deleteSpect ****************/
/* Delete a player_t structure created by player_newSpect.
 *
 * Caller provides:
 *   pointer to a player_t provided by player_newPlayer or player_newSpect.
 *
 * We do:
 *   we free pointer to the player_t provided, if not NULL.
 *
 * IMPORTANT:
 *   liveMap should be free'd later on.
 */
void player_deleteSpect(player_t* spectator);

/**************** player_move ****************/
/* Alter position of player by col and row amounts provided
 *
 * Caller provides:
 *   player: pointer to a player_t provided by player_newPlayer.
 *   col, row: amounts by which to move player in x and y directions, respectively
 *
 * We do:
 *   Move player by col and row amounts provided
 */
void player_move(player_t* player, int col, int row);

/**************** player_addGold ****************/
/* Increase the player's gold by the number of gold provided
 * 
 * Caller provides:
 *   player: pointer to a player_t provided by player_newPlayer.
 *   numGold: number of gold newly collected
 *
 * We do:
 *   increase player->numGold by numGold provided
 */
void player_addGold(player_t* player, int numGold);


#endif // __PLAYER_H
