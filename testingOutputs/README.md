# testingOutputs

This directory contains files with output from our tests

* `bottesting.out`: result of `bash -v bottesting.sh [port number] &> testingOutputs/bottesting.out`
* `miniclienttesting.out`: result of `bash -v miniclienttesting.sh [port number] &> testingOutputs/miniclienttesting.out`
* `serverargtesting.out`: result of `bash -v serverargtesting.sh &> testingOutputs/serverargtesting.out`
* `gridtest.out`: result of `make valgrind_grid &> testingOutputs/gridtest.out`
* `playertest.out`: resut of `make valgrind_player &> testingOutputs/playertest.out` 