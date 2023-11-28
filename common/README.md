# CS50 21S Nuggets
 
## Common README.md
 
### Team name: grn-rng

This subdirectory consists of the common.a library and contains two modules
that facilitate the nuggets game.

## 'grid' module

This module implements a `grid_struct` which holds two integers (number of rows
and number of columns) and a 2D array of characters that represents the game
map. See `grid.h` for interface details.

## 'player' module

This module implements a `player_struct` which holds information relating to a
player of the nuggets game. See `player.h` for interface details and 
`playertest.c` for usage examples.

## Usage

To build common.a, run `make`

To clean up, run `make clean`

`make` can be called from the top level directory to compile this directory. `make clean` can also be called from the top level directory to clean up this directory.
