/*
 * grid.c - grid module
 *
 * see grid.h for more documentation
 *
 * Grace Wang, Neha Ramsurrun, Ryan Yong, May 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h> 
#include <math.h>
#include "mem.h"
#include "file.h"

/**************** global constant ****************/
static const char solidRock = ' ';  // character for the solid rock
static const char goldPile = '*';   // character for the gold pile
static const char roomSpot = '.';   // character for the room spot
static const char playerChar = '@'; // character for the player character

/**************** local types ****************/
/* none */

/**************** global types ****************/
typedef struct grid{
    int numRows;
    int numCols;
    char** map;   
} grid_t;

/**************** local functions ****************/
static void grid_calcVisibility(grid_t* staticGrid, grid_t* liveGrid, grid_t* playerGrid, int rowPlayer, int colPlayer);
static bool grid_isVisible(grid_t* staticGrid, int row, int col, int rowPlayer, int colPlayer);
static bool grid_rowVisibility(grid_t* staticGrid, int row, int col, int rowPlayer, int colPlayer);
static bool grid_colVisibility(grid_t* staticGrid, int row, int col, int rowPlayer, int colPlayer);

/**************** grid_new() ****************/
/* see grid.h for description */
grid_t*
grid_new(int numRows, int numCols)
{
  if (numCols > 0 && numRows > 0)
  {
    grid_t* grid = mem_malloc(sizeof(grid_t));
    grid->numRows = numRows;
    grid->numCols = numCols;
    
    if (grid == NULL) {
      return NULL;              // error allocating grid
    } else {
      // initialize contents of grid structure
      grid->map = (char**)mem_calloc(numRows + 1, sizeof(char*));

      // initialize each array within the 2D array
      for (int i = 0; i <= numRows; i++){
        grid->map[i] = (char*)mem_calloc(numCols + 1, sizeof(char));
      }
      
      // initialize with spaces
      for (int row = 0; row <= numRows; row++) {
        for (int col = 0; col <= numCols; col++) {
          grid->map[row][col] = solidRock;
        }
      }
      return grid;
    }
  } else {
    return NULL;
  }
}

/**************** grid_load() ****************/
/* see grid.h for description */
grid_t*
grid_load(const char* mapFile)
{
  FILE *f = fopen(mapFile, "r");

  if (f != NULL) {
    // get number of rows for the grid
    int numRows = file_numLines(f);

    // get number of columns for the grid
    int numCols = 0;
    char* line = NULL;
    while ((line = file_readLine(f)) != NULL) {
      if (strlen(line) > numCols) {
        numCols = strlen(line) + 1;
      }
      free(line);
    }
    fclose(f);

    grid_t* grid = grid_new(numRows, numCols);

    // check if dimensions of grid is valid
    if (numCols <= grid->numCols && numRows <= grid->numRows) {
      // populate the grid with characters
      int row = 0;
      line = NULL;
      f = fopen(mapFile, "r");
      while ((line = file_readLine(f)) != NULL) {
        // for each character in the line, add to 2D array
        for (int col = 0; col <= strlen(line); col++) {
          grid->map[row][col] = line[col];
          if ( col == strlen(line) && col < numCols) {
            while(col <= numCols) {
              grid->map[row][col] = solidRock;
              col++;
            }
          }
        }
        free(line);
        row++;
      }
      fclose(f);
      return grid;
    }
  }
  return NULL;

}

/**************** grid_update() ****************/
/* see grid.h for description */
void
grid_update(grid_t* staticGrid, grid_t* liveGrid, grid_t* playerGrid, char id, int row, int col ) 
{ 
  if (row >= 0 && col >= 0) {
    // move player to new position
    liveGrid->map[row][col] = id;  
    grid_calcVisibility(staticGrid, liveGrid, playerGrid, row, col);
    playerGrid->map[row][col] = playerChar;
  }
}

/**************** grid_remove() ****************/
/* see grid.h for description */
void
grid_remove(grid_t* staticGrid, grid_t* liveGrid, grid_t* playerGrid, int row, int col) 
{
  // remove player from the position
  liveGrid->map[row][col] = staticGrid->map[row][col];
  if (playerGrid->map[row][col] != solidRock) {
    playerGrid->map[row][col] = staticGrid->map[row][col];
  } 
}

/**************** grid_toString() ****************/
/* see grid.h for description */
char*
grid_toString(grid_t* grid){
  if (grid == NULL) {
    return NULL;
  } else {
    char* string = (char*)(malloc((grid->numCols + 1) * (grid->numRows + 1) + grid->numRows + 2));
    int idx = 0;
    for (int row = 0; row <= grid->numRows; row++) {
      for (int col = 0; col <= grid->numCols; col++) {
        if (&grid->map[row][col] != NULL){
          string[idx] = grid->map[row][col];
          idx++;
        }
      }
      string[idx] = '\n';
      idx ++;
    }
    string[idx] = '\0';
    return string;
  }
}

/**************** grid_setGold() ****************/
/* see grid.h for description */
int
grid_setGold(grid_t* liveGrid, int minGoldPiles, int maxGoldPiles) {
  if (liveGrid == NULL || minGoldPiles > maxGoldPiles) {
    return false;
  } else {
    int count = 0;
    int numPiles = (rand() % (maxGoldPiles - minGoldPiles + 1)) + minGoldPiles;
    for (int i = 0; i <= numPiles; i++) {
      int row = (rand() % (liveGrid->numRows + 1));
      int col = (rand() % (liveGrid->numCols +1));
      if (liveGrid->map[row][col] == roomSpot) {
        liveGrid->map[row][col] = goldPile;
        count++;
      } else {
        i--;
      }
    }
    return count;
  }
}


/**************** grid_delete() ****************/
/* see grid.h for description */
void 
grid_delete(grid_t* grid) 
{
  if (grid != NULL) {
    // loop through each array in map
    for(int i = 0; i <= grid->numRows; i++) {
      mem_free(grid->map[i]);
    }
    mem_free(grid->map);
    mem_free(grid);
  }
}

/**************** grid_getChar() ****************/
/* see grid.h for description */
char
grid_getChar(grid_t* grid, int row, int col)
{
  if (grid != NULL && row <= grid->numRows && col <= grid->numCols) {
    return grid->map[row][col];
  } else {
    return '\0';
  }
}

/**************** grid_getMap() ****************/
/* see grid.h for description */
char**
grid_getMap(grid_t* grid) 
{
  if (grid != NULL){
    return grid->map;
  } else {
    return NULL;
  }
}

/**************** grid_getRows() ****************/
/* see grid.h for description */
int
grid_getRows(grid_t* grid) 
{
  if (grid != NULL){
    return grid->numRows;
  } else {
    return 0;
  }
}

/**************** grid_getCols() ****************/
/* see grid.h for description */
int
grid_getCols(grid_t* grid) 
{
  if (grid != NULL){
    return grid->numCols;
  } else {
    return 0;
  }
}

/**************** grid_calcVisibility() **************** /
 * loops through each point in the grid map. If the point is visible, it is
 * added to our player's grid map.
 */
static void
grid_calcVisibility(grid_t* staticGrid, grid_t* liveGrid, grid_t* playerGrid, int rPlayer, int cPlayer) 
{
  if (staticGrid == NULL || liveGrid == NULL || playerGrid == NULL){
    exit(1);
  }
  // loop through each point and determine if it is visible
  for (int r = 0; r <= liveGrid->numRows; r++) {
    for (int c = 0; c <= liveGrid->numCols; c++) {
      // if it is visible add it to the player grid
        if (grid_isVisible(staticGrid, r, c, rPlayer, cPlayer )) {
          playerGrid->map[r][c] = liveGrid->map[r][c];
        } 
        // if it is no longer visible, change to what it has remembered
        else if (playerGrid->map[r][c] != solidRock && playerGrid->map[r][c] != playerChar) {
          playerGrid->map[r][c] = staticGrid->map[r][c];
        } 
    }
  }
}

/* *************** grid_isVisible() **************** /
 * takes in a staticGrid, row and column position of a point, and the row 
 * and column position of the player. Determines if the point is visible from
 * the player's position. 
 * We return:
 *    true if visible, false if not
 */
static bool
grid_isVisible(grid_t* staticGrid, int row, int col, int rowPlayer, int colPlayer) {
  // if we are in the same column
  if (col == colPlayer){
    return grid_colVisibility(staticGrid, row, col, rowPlayer, colPlayer);
  }

   // if we are in the same row
   if (row == rowPlayer){
    return grid_rowVisibility(staticGrid, row, col, rowPlayer, colPlayer);
  }

  // r = mc + b
  int y1 = -row;
  int y2 = -rowPlayer; 
  double m = (double)(y2-y1)/(double)(colPlayer-col);
  double b = y1 - (m * col);

  // if point is to the left of player
  if (col < colPlayer) {
    // loop from the player column to the point column
    for (int currCol = colPlayer - 1; currCol > col; currCol--) {
      // calculate the row position
      float calcRow = -((float)(m * currCol) + b);
      // if row position is a whole number
      if (ceilf(calcRow) == calcRow) {
        if (staticGrid->map[(int)calcRow][currCol] != roomSpot) {
          return false;
        }
      }
      // if row position is not a whole number 
      else {
        // check if point above or below is a room spot
        if (staticGrid->map[(int)floorf(calcRow)][currCol] != roomSpot && staticGrid->map[(int)ceilf(calcRow)][currCol] != roomSpot ) {
          return false;
        }
      }
    }
  }

  // if point is to the right of the player
  if (col > colPlayer){
    // loop from the player column to the point column
    for (int currCol = colPlayer + 1; currCol < col; currCol++) {
      // calculate the row position
      float calcRow = -((float)(m * currCol) + b);
      // if row position is a whole number
      if (ceilf(calcRow) == calcRow) {
        if (staticGrid->map[(int)calcRow][currCol] != roomSpot) {
          return false;
        }
      }
      // if row position is not a whole number 
      else {
        // check if point above or below is a room spot
        if (staticGrid->map[(int)floorf(calcRow)][currCol] != roomSpot && staticGrid->map[(int)ceilf(calcRow)][currCol] != roomSpot ) {
          return false;
        }
      }
    }
  }

  // if point is above player
  if (row < rowPlayer) {
    // loop from the player row to the point row
    for (int currRow = rowPlayer - 1; currRow > row; currRow--) {
      // calculate the column position
      float calcCol = ((float)(-currRow)-b)/(m);
      // if column position is a whole number
      if (ceilf(calcCol) == calcCol) {
        if (staticGrid->map[currRow][(int)calcCol] != roomSpot) {
          return false;
        }
      }
      // if column position is not a whole number 
      else {
        // check if point above or below is a room spot
        if (staticGrid->map[currRow][(int)floorf(calcCol)] != roomSpot && staticGrid->map[currRow][(int)ceilf(calcCol)] != roomSpot ) {
          return false;
        }
      }
    }
  }

  // if point is below player
  if (row > rowPlayer) {
    // loop from the player row to the point row
    for (int currRow = rowPlayer + 1; currRow < row; currRow++) {
      // calculate the column position
      float calcCol = ((float)(-currRow)-b)/(m);
      // if column position is a whole number
      if (ceilf(calcCol) == calcCol) {
        if (staticGrid->map[currRow][(int)calcCol] != roomSpot) {
          return false;
        }
      }
      // if column position is not a whole number 
      else {
        // check if point above or below is a room spot
        if (staticGrid->map[currRow][(int)floorf(calcCol)] != roomSpot && staticGrid->map[currRow][(int)ceilf(calcCol)] != roomSpot ) {
          return false;
        }
      }
    }
  }
  return true;
}

/* *************** grid_rowVisibility() **************** /
 * determines visibility of a point when the point is in the same row as player
 * We return:
 *    true if visible, false if not
 */
static bool grid_rowVisibility(grid_t* staticGrid, int row, int col, int rowPlayer, int colPlayer)
{
  // if the point is to the left of the player
  if (col < colPlayer) {
    // loop from the player, up to the point
    for (int currCol = colPlayer - 1; currCol > col; currCol-- ) {
      if (staticGrid->map[row][currCol] != roomSpot){
        return false;
      }
    }
  }
  // if the point is to the right of the player
  if (col > colPlayer) {
    // loop from the player, down to the point
    for (int currCol = colPlayer + 1; currCol < col; currCol++) {
      if (staticGrid->map[row][currCol] != roomSpot){
        return false;
      }
    }
  }
  return true;
}

/* *************** grid_colVisibility() **************** /
 * determines visibility of a point when the point is in the same column as player
 * We return:
 *    true if visible, false if not
 */
static bool grid_colVisibility(grid_t* staticGrid, int row, int col, int rowPlayer, int colPlayer)
{
  // if the point is above the player
  if (row < rowPlayer) {
    // loop from the player, up to the point
    for (int currRow = rowPlayer - 1; currRow > row; currRow-- ) {
      if (staticGrid->map[currRow][col] != roomSpot) {
        return false;
      }
    }
  }
  // if the point is below the player
  if (row > rowPlayer) {
    // loop from the player, down to the point
    for (int currRow = rowPlayer + 1; currRow < row; currRow++) {
      if (staticGrid->map[currRow][col] != roomSpot) {
        return false;
      }
    }
  }
  return true;
}