#!/bin/bash
#
# serverargtesting.sh - tests server with various arguments
#
# user must provide a port number as an argument (in run.log in logs directory)
#
# Grace Wang, Neha Ramsurrun, Ryan Yong, May 2021
#

# No arguments
./server

# One argument - invalid map filename
./server asdfasdfafdsasfdfasdasdfafsdasdf

# Two arguments - invalid map filename and valid seed
./server asdfsdafdadfs9898 123

# Two arguments - valid map filename and invalid seed (negative)
./server maps/challenge.txt -123

# Two arguments - valid filename and invalid seed (float)
./server maps/challenge.txt -1.34

# Three arguments - valid filename, valid seed, and random third argument
./server maps/challenge.txt 123 sample
