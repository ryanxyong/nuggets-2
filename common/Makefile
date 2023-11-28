# Makefile for 'grid' module
#
# Grace Wang, April 2021

L = ../libcs50
S = ../support/
LIB = common.a
LLIBS = $L/libcs50-given.a
SLIBS = $S/support.a 
OBJS = grid.o player.o
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$L -I$S
CC = gcc
MAKE = make


$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

# object files depend on include files

# gridtest.o: grid.h $L/file.h
# playertest.o: player.h $S/message.h
grid.o: grid.h
player.o: player.h $S/message.h

.PHONY: clean

clean:
	rm -f core
	rm -f *~ *.o
	rm -rf *.dSYM
	rm -f $(LIB) *~ *.o 
