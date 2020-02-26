# Makefile for socket examples
#
# David Kotz, May 2016
# Xia Zhou, August 2017
# Temi Prioleau, 2020

PROGS = AMSTARTUP
LLIBS = 

FLAGS = # 
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

all: $(PROGS)

########### inclient ##################
AMSTARTUP: AMSTARTUP.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

AMSTARTUP.o: 

clean:
	rm -rf *~ *.o *.dSYM
	rm -f $(PROGS) 

