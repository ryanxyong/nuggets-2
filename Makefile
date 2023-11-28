# Makefile for server
#
# Grace Wang, Neha Ramsurrun, Ryan Yong, May 2021

PROG = server
OBJS = server.o
PROG1 = playertest
OBJS1 = playertest.o
PROG2 = gridtest
OBJS2 = gridtest.o
LIBDIR = libcs50
SUPDIR = support
COMDIR = common
LIB =  $(SUPDIR)/support.a $(COMDIR)/common.a $(LIBDIR)/libcs50-given.a -lm

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$(LIBDIR) -I$(SUPDIR) -I$(COMDIR)

.PHONY: all tests test_player test_grid arg_test valgrind valgrind_grid valgrind_player clean

all: $(PROG) $(PROG1) $(PROG2)
	(cd $(LIBDIR) && if [ -r set.c ]; then make $(LIBDIR).a; else cp $(LIBDIR)-given.a $(LIBDIR).a; fi)
	make -C $(SUPDIR)
	make -C $(COMDIR)
	
$(PROG): $(OBJS) $(LIB)
	$(CC) $(CFLAGS) $^ -o $@

$(PROG1): $(OBJS1) $(LIB)
	$(CC) $(CFLAGS) $^ -o $@

$(PROG2): $(OBJS2) $(LIB)
	$(CC) $(CFLAGS) $^ -o $@

server.o: $(SUPDIR)/message.h $(SUPDIR)/log.h $(COMDIR)/player.h $(COMDIR)/grid.h $(LIBDIR)/hashtable.h
playertest.o: $(COMDIR)/player.h $(COMDIR)/grid.h $(SUPDIR)/message.h $(LIBDIR)/mem.h
gridtest.o: $(COMDIR)/grid.h $(LIBDIR)/file.h

$(SUPDIR)/support.a:
	make -C $(SUPDIR) support.a

$(COMDIR)/common.a:
	make -C $(COMDIR) common.a

tests:
	./gridtest	
	./playertest

test_player:
	./playertest

test_grid:
	./gridtest

arg_test:
	bash -v serverargtesting.sh

valgrind:
	valgrind ./gridtest
	valgrind ./playertest

valgrind_grid:
	valgrind ./gridtest
	
valgrind_player:
	valgrind ./playertest

clean:
	rm -f $(PROG)
	rm -f $(PROG1)
	rm -f $(PROG2)
	make -C libcs50 clean
	make -C common clean
	make -C support clean
	rm -f core
	rm -f *~ *.o
	rm -rf *.dSYM
	rm -f vgcore.*
	rm -f logs/*.*
	rm -f testingOutputs/*.out
