# CS50 21S Nuggets
## Server TESTING.md
### Team name: grn-rng
 
The testing for the server portion of the Nuggets game will include unit testing and integration/system testing as described below.
 
## Unit Testing
We perform unit testing on the `player` and `grid` modules, found in the `common` directory through C drivers for each module, found in the top level directory.

* The `player` module is tested in the C driver `playertest.c`, where the module functions are called to create a player and to manipulate its data, such as its position, its grid and its number of gold nuggets. We also test the player getter methods. These are all tested mainly through print statements and our output for `playertest.c`, which was run with valgrind, appears in `playertest.out`.
 
* The `grid` module is tested in the C driver `gridtest.c`, where the module functions are called to create grids. In this test, we created a staticGrid, with the loaded map, and it is not changed at all throughout the test. We also created a liveGrid, which gets updated, and playerGrid that represents what a player sees. We also test the grid getter methods. Our functions are mainly tests with print statements, printing the grid maps and values from getter methods, and our output for `gridtest.c`, which was run with valgrind, appears in `gridtest.out`.
 
In order to run these tests, we call `make` in the top level directory to compile the C drivers. In our top level directory, we run `make tests` to run both `gridtest.c` and `playertest.c`. We run `make test_grid` to run `gridtest.c` and `make test_player` to run `playertest.c`. We run `make valgrind` to run valgrind and check for memory leaks on both `gridtest.c` and `playertest.c`. We can run `make valgrind_grid` to test valgrind on just the `gridtest.c` and `make valgrind_player` to test valgrind on just `playertest.c`.
 
## Integration/System Testing
Once the modules have been tested and are working correctly, we start testing on `server.c` using bash scripts. We run a variety of tests, including tests for erroneous/invalid arguments, for memory leaks (using valgrind) and for manually checking the performance of the interactive game itself by providing valid arguments to the `server` program and playing the game.

We start by testing with invalid argument counts (argc < 2 and argc > 3) and invalid arguments, such as an invalid integer for the seed parameter and an unreadable file. We do not check for the validity of the map passed in since the `server` assumes all map files are valid, as per the Requirement spec. We test for these in `serverargtesting.sh` and its output is directed to `serverargtesting.out` in the *testingOutputs* directory.


### Manual Testing
 
As we start testing with various valid maps and seeds, we run those tests with valgrind to ensure that the game does not end with memory leaks. We manually play the game using the provided `player` program in the `~/cs50-dev/shared/nuggets/` directory to ensure that every required aspect of the game, specified in the Requirement spec, works.
All three members of the team will play the game, joining as `player`, to test the server’s ability to handle a single player and subsequently multiple players. We also join as `spectator`, to ensure that the replacement of the `spectator` takes place smoothly. By manually playing the game as such, we monitor the server’s messages: we ensure that the server is sending correct messages to players, that it is correctly monitoring the number of gold left, that each player’s visibility functions correctly and that the final Game Over summary is correctly produced and displayed. We run a test on a map designed by us, `grn-rng.txt`, which is in the `maps` directory.

We also simulate manual testing with our `miniclienttesting.sh` where we feed in a series of valid and invalid *client messages* from our `miniclient.input` file. The results are directed to `miniclienttesting.out` in our *testingOutputs* directory.
 
### Automated Testing
 
We also perform automated testing by using the provided `player` program's special bot mode capability. This tests random movement keystrokes sent to the `server` and this thus further tests our server’s ability to handle single or multiple players. We provide `botbg` as the `playerName` when performing this test. We test this in `bottesting.sh` and its output is directed to `bottesting.out` in the *testingOutputs* directory.
 
 
