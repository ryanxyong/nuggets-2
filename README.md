# CS50 21S Nuggets
 
## README.md
 
### Team name: grn-rng

### Team Members: Grace Wang, Ryan Yong, Neha Ramsurrun

This repository contains the code for the CS50 "Nuggets" game, in which players explore a set of rooms and passageways in search of gold nuggets.
The rooms and passages are defined by a *map* loaded by the server at the start of the game.
The gold nuggets are randomly distributed in *piles* within the rooms.
Up to 26 players, and one spectator, may play a given game.
Each player is randomly dropped into a room when joining the game.
Players move about, collecting nuggets when they move onto a pile.
When all gold nuggets are collected, the game ends and a summary is printed.

## Assumptions

Our server assumes that the map file given contains a valid map.

## Files

- `Makefile`: compilation procedure
- `server.c`: the "Nuggets" game server
- `gridtest.c`: unit test driver for the *grid* module
- `playertest.c`: unit test driver for the *player* module
- `serverargtesting.sh`: invalid argument test script for the *server* program
- `miniclienttesting.sh`: malformatted messages test script for the *server* program
- `bottesting.sh`: automated bot and spectator joining test for the *server* program
- `miniclient.input`: used for feeding input into the *miniclienttesting* script

## Usage

run `make` to compile.

run `make clean` to clean up.

run `./server 2>server.log [map file]` to start the server for the "Nuggets" game.

## Testing

See the [TESTING.md file](TESTING.md) for more detailed information about testing.

run `make tests` to run unit tests on both *grid* and *player* module

run `make test_grid` to run the unit test on the *grid* module

run `make test_player` to run the unit test on the *player* module

run `make arg_test` to run the invalid arguments test on the *server* program

run `make valgrind` to run valgrind with the unit tests on both *grid* and
*player* module to check for memory leaks

run `make valgrind_grid` to run to run valgrind with the unit test on 
*grid* module to check for memory leaks

run `make valgrind_player` to run to run valgrind with the unit test on 
*player* module to check for memory leaks

run a bash script `bottesting.sh` that tests server with bot players, which
takes the server port number for current game:
```
bash -v bottesting.sh [port number]
```

run a bash script `serverargtesting.sh` that tests for invalid/erroneous 
arguments:
```
bash -v serverargtesting.sh
```
run a bash script `miniclienttesting.sh` that tests to see if server properly
handles malformatted messages.
```
bash -v miniclienttesting.sh [port number]
```


## Materials provided

See the [support library](support/README.md) for some useful modules.

See the [maps](maps/README.md) for some draft maps.

See the [common library](common/README.md) for nuggets specific modules.

See the [CS50 library](libcs50/README.md) for implementations of various data structure modules.

See the [logs](logs/README.md) for the output log files from running bot testing and the server.

See the [testing outputs](testingOutputs/README.md) for the output files from the test scripts.