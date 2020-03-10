# Makefile for socket examples
#
# David Kotz, May 2016
# Xia Zhou, August 2017
# Temi Prioleau, 2020

PROGS = AMSTARTUP
OBJS = AMSTARTUP.o avatar.o object.o maze.o

FLAGS = # 
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS) -lpthread
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

all: $(PROGS) $(OBJS)

########### inclient ##################
AMSTARTUP: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

AMSTARTUP.o: 

avatar.o: avatar.h maze.h object.h

object.o: object.h

maze.o: maze.h

test:
	### Uncomment this line below if you are running the make test for the first time ###
	# mkdir Results
	chmod +x testing.sh
	./testing.sh

clean:
	rm -rf *~ *.o *.dSYM
	rm -f $(PROGS) 
	rm -f vgcore.* core
	rm -f ./Results/*.log
