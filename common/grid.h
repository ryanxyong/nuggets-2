/* 
 * grid.h - header file for CS50 grid module
 *
 * Our grid struct represents the game maps. It holds the number rows and columns
 * of a grid as well as a 2D array of characters that represents the map
 *
 * Grace Wang, Neha Ramsurrun, Ryan Yong, May 2021
 */

#ifndef __GRID_H
#define __GRID_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

/**************** global types ****************/
typedef struct grid grid_t;  // opaque to users of the module

/**************** functions ****************/

/**************** grid_new ****************/
/* Create a new (empty) grid that is populated by spaces.
 *
 * Caller provides:
 *   number of rows and columns (must be greater than 0).
 * We return:
 *   pointer to the new grid; return NULL if error.
 * We guarantee:
 *   grid is initialized with empty spaces.
 * Caller is responsible for:
 *   later calling grid_delete.
 */
grid_t* grid_new(int numRows, int numCols);

/**************** grid_load ****************/
/* Loads a text file that holds the game map into the 2D array of the grid struct.
 *
 * Caller provides:
 *   valid name to a valid map file.
 * We return:
 *  the grid with the loaded map; return NULL if error
 */
grid_t* grid_load(const char* mapFile);

/**************** grid_update ****************/
/* Changes characters in the proper grid to reflect current state and changes
 * updates the live grid and changes player's grid based on visibility
 *
 * Caller provides:
 *   valid pointer to a grid that will be the default (staticGrid), 
 *   valid pointer to a grid that reflects current game (liveGrid),
 *   valid pointer to a grid that reflects what a player sees,
 *   the character ID of a player,
 *   the row and column position that we are manipulating in the grids.
 * We return:
 *   nothing
 */
void grid_update(grid_t* staticGrid, grid_t* liveGrid, grid_t* playerGrid, char id, int row, int col );

/**************** grid_remove ****************/
/* Removes an item from the live grid and sets it back to the default at the row and column positions
 *
 * Caller provides:
 *   valid pointer to the default grid (staticGrid),
 *   valid pointer to the live grid,
 *   valid pointer to the player grid,
 *   valid row and position we are removing a character from
 * We return:
 *   nothing
 */
void grid_remove(grid_t* staticGrid, grid_t* liveGrid, grid_t* playerGrid, int row, int col );

/**************** grid_toString ****************/
/* Return the grid's 2D array as a single string
 * 
 * Caller provides:
 *   valid pointer to the grid we are to turn into a string
 * We return:
 *  a string; return NULL if error
 * Note:
 *   the grid and its contents are not changed by this function
 */
char* grid_toString(grid_t* grid);

/**************** grid_setGold ****************/
/* Set random positions in the grid as gold piles.
 * 
 * Caller provides:
 *   valid pointer to the grid where the piles will be added, 
 *   minimum number of piles,
 *   maximum number of piles.
 * We return:
 *   number of gold piles generated
 */
int grid_setGold(grid_t* liveGrid, int minGoldPiles, int maxGoldPiles);

/**************** grid_delete ****************/
/* Delete grid, deleting each array in the 2D array
 *
 * Caller provides:
 *   valid grid pointer,
 *   valid pointer to function that handles one item (may be NULL).
 * We do:
 *   if grid==NULL, do nothing.
 *   free all the arrays, and the grid itself.
 */
void grid_delete(grid_t* grid);

/**************** grid_getChar ****************/
/* gets the character at a specific position in the 2D array/ map
 *
 * Caller provides:
 *   valid grid pointer,
 *   row position,
 *   column position
 * We return:
 *   the character at the given position; '\0' if error
 */
char grid_getChar(grid_t* grid, int row, int col);

/**************** grid_getMap ****************/
/* returns the 2D array in the grid structure
 *
 * Caller provides:
 *   valid pointer to grid
 * We return:
 *   2D array of characters; NULL if error
 */
char** grid_getMap(grid_t* grid);

/**************** grid_getRows ****************/
/* returns the number of rows of the grid
 *
 * Caller provides:
 *   valid grid pointer,
 * We return:
 *   grid's numRows; 0 is error
 */
int grid_getRows(grid_t* grid);

/**************** grid_getCols ****************/
/* returns the number of columns of the grid
 *
 * Caller provides:
 *   valid grid pointer,
 * We return:
 *   grid's numCols; 0 is error
 */
int grid_getCols(grid_t* grid);

#endif // __GRID_H