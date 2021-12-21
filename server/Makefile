#
# Makefile for server.c
#
# Elizabeth Frey, Jack Gnibus, Isaac Feldman
# CS 50 Project
# Winter 2021
#

PROG = server
OBJS = ../gameboard/gameboard.o ../player/player.o server.o 
LIBS = ../support/support.a
CFLAGS =  -Wall -pedantic -std=c11 -ggdb -I../player -I../gameboard -I../support -lm
CC = gcc

.PHONY: all clean

$(PROG): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $^ -o $@

#dependences

server.o: ../gameboard/gameboard.o ../player/player.o 
../gameboard/gameboard.o: ../gameboard/gameboard.h
../player/player.o: ../player/player.h

clean:
	rm -f *~ *.o
	rm -f $(PROG)
	#make --directory=support clean
	#make --directory=player clean
	#make --directory=server clean
