#
# Makefile for player.c
#
# Elizabeth Frey, Jack Gnibus, Isaac Feldman
# CS 50 Project
# Winter 2021
#

PROG = 
OBJS = ../gameboard/gameboard.o player.o 
LIBS = ../support/support.a
CFLAGS =  -Wall -pedantic -std=c11 -ggdb -I../gameboard -I../support
CC = gcc

.PHONY: all clean

all: player.o

unittest: $(OBJS) playerUnitTest.o $(LIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
	./unittest
	rm -f unittest
#dependences

../gameboard/gameboard.o: ../gameboard/gameboard.h player.h
player.o: player.h ../gameboard/gameboard.h
playerUnitTest.o: playerUnitTest.c

clean:
	rm -f *~ *.o core
	rm -f $(PROG)
