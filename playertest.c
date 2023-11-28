/* 
 * playertest.c - test program for CS50 Nuggets Final Project's player module
 *
 * usage: commandline takes no arguments
 *
 * CS50 Nuggets Final Project Spring 2021
 * Grace Wang, Neha Ramsurrun, Ryan Yong
 */



#include <stdio.h>
#include <stdlib.h>
#include "player.h"
#include "message.h"
#include "grid.h"
#include <string.h>

int
main()
{
    player_t* player;
    addr_t address = message_noAddr();

    // allocate memory for player's name
    char* name = calloc(50, sizeof(char));
    if (name == NULL) {
      fprintf(stderr, "Error: could not allocate memory for name.\n");
      exit(1);
    }
    strcpy(name, "neha rsn");
    
    // create new grid for player
    grid_t* playerGrid;
    playerGrid = grid_new(20,20);
    if (playerGrid == NULL){
        fprintf(stderr, "Error: grid_new failed\n");
        exit(2);
    }

    // TESTING player_newPlayer
    printf("Creating new player A\n");
    player = player_newPlayer('A', address, name, 0, 0, playerGrid);
    if (player == NULL) {
        fprintf(stderr, "Error: player_new failed\n");
        exit(3);
    }

    // TESTING getter methods
    printf("\nTesting player getter methods:\n");
    printf("Player ID: %c\n", player_getID(player));
    printf("Player name: %s\n", player_getName(player));
    printf("Player location: (%d, %d)\n",player_getCol(player), player_getRow(player));
    char* gridstring = grid_toString(player_getVisGrid(player));
    printf("Player's empty grid:\n%s\n", gridstring);
    printf("End of player's empty grid\n");


    // TESTING player_move
    printf("\nTesting player_move:\n");
    printf("Player location at start (should be (0,0)): (%d, %d)\n",player_getCol(player), player_getRow(player));
    player_move(player, 1, 0); // move right
    printf("New player location (should be (1,0)): (%d, %d)\n",player_getCol(player), player_getRow(player));
    player_move(player, 0, 1); // move down
    printf("New player location (should be (1,1)): (%d, %d)\n",player_getCol(player), player_getRow(player));
    player_move(player, -1, 1); // move left, then move down
    printf("New player location (should be (0,2)): (%d, %d)\n",player_getCol(player), player_getRow(player));
    player_move(player, 0, -2); // move up two positions
    printf("New player location (should be (0,0)): (%d, %d)\n",player_getCol(player), player_getRow(player));

    // TESTING player_addGold
    printf("\nTesting player_addGold:\n");
    printf("Player nuggets at beginning (should be 0): %d\n", player_getGold(player));
    player_addGold(player, 46);
    printf("Player nuggets after adding 46 gold (should be 46): %d\n", player_getGold(player));
    player_addGold(player, 34);
    printf("Player nuggets after adding 34 gold (should be 80): %d\n", player_getGold(player));

    // TESTING player_quit
    printf("\nTesting player_quit:\n");
    printf("Print player's quit status at beginning (should be false):\n");
    if (!player_getQuit(player)){
        printf("player_getQuit evaluated to false - player has not quit.\n");
    }
    printf("\nNow calling player_quit. Player will be disconnected.\n");
    player_quit(player);
    printf("Print player's quit status now (should be true):\n");
    if(player_getQuit(player)){
        printf("player_quit evaluated to true - player has quit.\n");
    }

    // TESTING player_delete
    printf("\nTesting player_delete:\n");
    printf("Now deleting player.\n");
    free(gridstring);
    player_delete(player);

    return 0;
}


