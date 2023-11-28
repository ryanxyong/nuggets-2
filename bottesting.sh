#!/bin/bash
#
# bottesting.sh - tests server with bots
#
# user must provide a port number as an argument (in run.log in logs directory)
#
# Grace Wang, Neha Ramsurrun, Ryan Yong, May 2021
#

./player 2>logs/player1.log localhost "$1" botbg &
./player 2>logs/player2.log localhost "$1" botbg &
./player 2>logs/player3.log localhost "$1" botbg &
./player 2>logs/player4.log localhost "$1" botbg &
./player 2>logs/player5.log localhost "$1" botbg &

./player 2>logs/spectator.log localhost "$1"

wait
echo game ended
