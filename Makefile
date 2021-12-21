# make file for nuggets project
#
# Elizabeth Frey, Jack Gnibus, Isaac Feldman
# Winter 2021
# CS 50 Project

all:
	$(MAKE) --directory=support
	$(MAKE) --directory=gameboard
	$(MAKE) --directory=player
	$(MAKE) --directory=server

.PHONY: clean

clean:
	rm -f *~
	$(MAKE) --directory=support clean
	$(MAKE) --directory=gameboard clean
	$(MAKE) --directory=player clean
	$(MAKE) --directory=server clean
