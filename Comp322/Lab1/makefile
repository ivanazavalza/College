# Name: Jared Fowler
# Project: Time and Forking Children (Lab1)


#makefile template provided by: Prof. Fitzgerald
#slightly modified by: Jared Fowler


# EXECUTABLES is a Make Variable (similar to an environment variable) that lists all the
# "executables" associated with the lab. 
EXECUTABLES=time-4-baby-and-me
SRC=comp322Lab1.c
OBJ=comp322Lab1.o

ODIR=obj

# The CC and CFLAGS variables are defined to ensure I'm using the native compiler and
# that the correct command-line options are set
CC=gcc
#CFLAGS=-g

# Here is my default target for the Makefile.  When I type make, this first target will
# be created. I.e., all the executables
all: $(EXECUTABLES)


# On the following lines (which _must_ start with a tab), are the commands I need to type -- 
# each time my source file is modified.
$(EXECUTABLES):
	$(CC) -o $@ $(SRC)

# By convention, I always have a target called
# "clean" that I used to define all temporary
# or intermediates.
clean:
	rm -f $(EXECUTABLES) $(OBJ) *~



