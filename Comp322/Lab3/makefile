# Name: Jared Fowler
# Project: Catcher


# This is a Makefile for my lab assignment. It allows me to easily and accurately rebuild
# the lab assignment.




# EXECUTABLES is a Make Variable (similar to an environment variable) that lists all the
# "executables" associated with the lab. 
EXECUTABLES=catcher
OBJ=comp322Lab3.o


# The CC and CFLAGS variables are defined to ensure I'm using the native compiler and
# that the correct command-line options are set
CC=gcc
CFLAGS=-g


# Here is my default target for the Makefile.  When I type make, this first target will
# be created. I.e., all the executables
all: $(EXECUTABLES)


# ---------------------------------------------------------------------

catcher: comp322Lab3.o
	$(CC) -o catcher comp322Lab3.o

comp322Lab3.o: comp322Lab3.c
	$(CC) -c comp322Lab3.c



# By convention, I always have a target called
# "clean" that I used to define all temporary
# or intermediates.
clean:
	rm -f $(EXECUTABLES) $(OBJ) *~
