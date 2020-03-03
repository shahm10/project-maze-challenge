# Makefile for socket examples
#
# David Kotz, May 2016
# Xia Zhou, August 2017
# Temi Prioleau, 2020

PROGS = AMSTARTUP
OBJS = AMSTARTUP.o avatar.o counters.o
# LLIBS = avatar.h counters.h

FLAGS = # 
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS) -lpthread
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

all: $(PROGS) $(OBJS)

########### inclient ##################
AMSTARTUP: $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) -o $@

AMSTARTUP.o: 

avatar.o: avatar.h counters.h

counters.o: counters.h

clean:
	rm -rf *~ *.o *.dSYM
	rm -f $(PROGS) 
	rm -f vgcore.* core

