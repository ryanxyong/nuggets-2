#!/bin/bash
#
# miniclienttesting.sh - tests server with malformatted messages
#
# user must provide a port number as an argument (in run.log in logs directory)
#
# Grace Wang, Neha Ramsurrun, Ryan Yong, May 2021
#

# starts miniclient to which miniclient.input file gives malformatted messages
support/miniclient localhost "$1" < miniclient.input
