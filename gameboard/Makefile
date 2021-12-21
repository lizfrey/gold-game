#
# Makefile for player.c
#
# Elizabeth Frey, Jack Gnibus, Isaac Feldman
# CS 50 Project
# Winter 2021
#

PROG = 
OBJS = gameboard.o ../player/player.o 
LIBS = ../support/support.a -lm
CFLAGS =  -Wall -pedantic -std=c11 -ggdb -I../player -I../support -lm
CC = gcc

.PHONY: all clean

all: gameboard.o

unittest: $(OBJS) gameboardUnitTest.o 
	$(CC) $(CFLAGS) $(OBJS) gameboardUnitTest.o $(LIBS) -o $@
	./unittest
	rm -f unittest
#dependences

gameboard.o: gameboard.h ../player/player.h
../player/player.o: ../player/player.h gameboard.h
gameboardUnitTest.o: gameboardUnitTest.c

clean:
	rm -f *~ *.o
	rm -f $(PROG)
	rm -f core
