/* 
 * gridtest.c - test program for grid module
 *
 * Grace Wang, Neha Ramsurrun, Ryan Yong, May 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grid.h"
#include "file.h"
#include "mem.h"

int
main()
{
  grid_t* grid = NULL;

  // test grid_new
  FILE* f = fopen("maps/big.txt", "r");
  // get number of rows for the grid
  int numRows = file_numLines(f);
  // get number of columns for the grid
  int numCols = 0;
  char* line = NULL;
  while ((line=file_readLine(f)) != NULL) {
    if (strlen(line) > numCols) {
      numCols = strlen(line) + 1;
    }
    free(line);
  }
  fclose(f);

  grid = grid_new(numRows, numCols);
  char* empty = grid_toString(grid);
  fprintf(stdout, "empty map:\n%s", empty);
  fprintf(stdout, "\n end of empty grid\n");
  free(empty);
  grid_delete(grid);
  
  // test grid_load
  grid = grid_load("maps/big.txt" );

  // test grid_toString
  char* string = grid_toString(grid);
  printf("static grid:\n%s", string);
  mem_free(string);

  grid_t* liveGrid = grid_load("maps/big.txt" );
  grid_t* playerAGrid = grid_new(numRows, numCols);

  // test grid_setGold
  grid_setGold(liveGrid, 1, 5);
  char* gold = grid_toString(liveGrid);
  printf("gold grid:\n%s", gold);
  mem_free(gold);

  // creating another player 
  grid_t* playerBGrid = grid_new(numRows, numCols);
  grid_update(grid, liveGrid, playerBGrid, 'B', 3, 6 );


  char* playerA;
  playerA = grid_toString(playerAGrid);
  printf("initial player grid:\n%s", playerA);
  mem_free(playerA);

  //test grid_update
  grid_update(grid, liveGrid, playerAGrid, 'A', 2, 5 );
  char* updated = grid_toString(liveGrid);
  printf("updated grid:\n%s", updated);
  mem_free(updated);
  playerA = grid_toString(playerAGrid);
  printf("playerA grid:\n%s", playerA);
  mem_free(playerA);

  // test grid_remove on playerA
  grid_remove(grid, liveGrid, playerAGrid, 2, 5);
  char* removed = grid_toString(liveGrid);
  printf("removed from grid:\n%s", removed);
  mem_free(removed);


  // test grid_getChar
  char c = grid_getChar(grid, 3, 146);
  printf("%c\n",c);

  // test grid_getMap
  char p = grid_getMap(grid)[2][5];
  printf("character at row 2, column 5: %c (should be '.')\n",p);

  // test grid_getRows
  printf("rows: %d (should be %d)\n", grid_getRows(grid), numRows);

  // test grid_getCols
  printf("cols: %d (should be %d)\n", grid_getCols(grid), numCols);

  // test grid_delete
  grid_delete(grid);
  grid_delete(playerAGrid);
  grid_delete(playerBGrid);
  grid_delete(liveGrid);

  grid_t* emptyGrid = grid_new(numRows, numCols);
  grid_delete(emptyGrid);
}
