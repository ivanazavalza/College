# Name: Jared Fowler
# Project: Asynchronous Elephant


# This is a Makefile for my lab assignment. It allows me to easily and accurately rebuild
# the lab assignment.




# EXECUTABLES is a Make Variable (similar to an environment variable) that lists all the
# "executables" associated with the lab. 
EXECUTABLES=matrix_add matrix_gen jwfLab7_decoder
OBJ=comp322Lab7_matrix_add.o comp322Lab7_gen.o comp322Lab7_outputDecoder.o


# The CC and CFLAGS variables are defined to ensure I'm using the native compiler and
# that the correct command-line options are set
CC=gcc
CFLAGS=-g


# Here is my default target for the Makefile.  When I type make, this first target will
# be created. I.e., all the executables
all: $(EXECUTABLES)


# ---------------------------------------------------------------------

matrix_add: comp322Lab7_matrix_add.o
	$(CC) $(CFLAGS) comp322Lab7_matrix_add.o -lrt -o matrix_add 

comp322Lab7_matrix_add.o: comp322Lab7_matrix_add.c
	$(CC) $(CFLAGS) comp322Lab7_matrix_add.c -lrt -c

matrix_gen: comp322Lab7_matrix_gen.o
	$(CC) $(CFLAGS) -o matrix_gen comp322Lab7_matrix_gen.o

comp322Lab7_matrix_gen.o: comp322Lab7_matrix_gen.c
	$(CC) $(CFLAGS) -c comp322Lab7_matrix_gen.c

jwfLab7_decoder: comp322Lab7_outputDecoder.o
	$(CC) $(CFLAGS) -o jwfLab7_decoder comp322Lab7_outputDecoder.o

comp322Lab7_outputDecoder.o: comp322Lab7_outputDecoder.c
	$(CC) $(CFLAGS) -c comp322Lab7_outputDecoder.c



# By convention, I always have a target called
# "clean" that I used to define all temporary
# or intermediates.
clean:
	rm -f $(EXECUTABLES) $(OBJ) *~
