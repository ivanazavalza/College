# Name: Jared Fowler
# Project: Dining Philosopers dining-p


# This is a Makefile for my lab assignment. It allows me to easily and accurately rebuild
# the lab assignment.




# EXECUTABLES is a Make Variable (similar to an environment variable) that lists all the
# "executables" associated with the lab. 
EXECUTABLES=dining-p
OBJ=comp322Lab4.o


# The CC and CFLAGS variables are defined to ensure I'm using the native compiler and
# that the correct command-line options are set
CC=gcc
CFLAGS=-g -pthread


# Here is my default target for the Makefile.  When I type make, this first target will
# be created. I.e., all the executables
all: $(EXECUTABLES)


# ---------------------------------------------------------------------

dining-p: comp322Lab4.o
	$(CC) $(CFLAGS) -o dining-p comp322Lab4.o

comp322Lab4.o: comp322Lab4.c
	$(CC) $(CFLAGS) -c comp322Lab4.c



# By convention, I always have a target called
# "clean" that I used to define all temporary
# or intermediates.
clean:
	rm -f $(EXECUTABLES) $(OBJ) *~
