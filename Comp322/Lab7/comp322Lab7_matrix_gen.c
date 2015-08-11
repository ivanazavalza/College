/*Created By: Jared Fowler
November 13, 2014
CSUN Comp 322L ; Prof. Fitzgerald


Notes:

    matrix_gen    

    A program that creates a file of integers that represents a NxN matrix.

    We assume the user to link the output of this program to a file via the > char.
    We should note that the piped info will be linked to stdout, not stderr. Therefore,
    all error and notice alerts will be sent via stderr.

    Usage:
    
    $ matrix_gen <size> > <file_name.size>

*/


#include <stdio.h>        //Used for: printf()
#include <stdlib.h>       //Used for: srand(), rand()
#include <time.h>         //Used for: time()


//typedef's and define's-------------------------------------
typedef unsigned char     BYTE;
typedef unsigned int      UINT32;
typedef signed int        INT32;
typedef unsigned char     BOOL;

#define true              1
#define false             0

#define DEBUGGING         0

//Prototypes-------------------------------------------------
BOOL stringToUINT32(char* c, UINT32* i);

//Globals-----------------------------------------------------
const UINT32 MAX_SIZE = 100000; //Maximum number of ints the program will write (MAX_SIZE * MAX_SIZE)
const INT32 LIMIT_LOW = -100;   //Minimum integer value that we will generate
const INT32 LIMIT_HI  =  100;   //Maximum integer value that we will generate

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/*FUNCTIONS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
int main(int argc, char * argv[]){

    //Local Variables
    UINT32 size = 0;

    //Valid number of arguments?
    if(argc < 2){
        fprintf(stderr, "Invalid argument count.\n");
        exit(1);
    }

    //Attempt to convert the second argument into an interger value
    if(!stringToUINT32(argv[1], &size)){
        fprintf(stderr, "Invalid integer argument.\n");
        exit(1);
    }

    //If the value is 0, or if the value is greater than MAX_SIZE, correct.
    if(size <= 0 ){
        fprintf(stderr, "Notice: File Size Argument = %u. Exiting process.\n", size);
        return 0;
    }
    if(size > MAX_SIZE){
        fprintf(stderr, "Notice: File Size Argument = %u, exeeds MAX_SIZE = %u.\nReducing size to match MAX_SIZE.\n", size, MAX_SIZE);
        size = MAX_SIZE;
    }

    //Seed the random number generator
    srand(time(NULL));

    /*Loop until we have written (size * size) random integers. We will write these
    to stdout.  We assume the caller of this function to have linked the output to 
    some sort of file via pipe.*/

    UINT32 index = (size * size);
    UINT32 index2;
    UINT32 intSize = sizeof(int);
    INT32  randN;

    for( ; index > 0; index--){

        //Generate a random number and print it out
        randN = ( (rand() % (LIMIT_HI - LIMIT_LOW)) + LIMIT_LOW);

        //Output the individual bytes (not ascii) that make up the integer value
        for(index2 = 0; index2 < intSize; index2++){
            fprintf(stdout, "%c", ((randN << (8 * index2)) >> ((intSize - 1) * 8)) );
        }

        #if(DEBUGGING)
        fprintf(stderr, "Rand Value: %d\n", randN);
        #endif

    }

    //We're done! Yeah!
    return 0;

}

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
BOOL stringToUINT32(char* c, UINT32* i){

    /*Converts a string of chars to an int. Assumes the char
    array to have integer values in char form. If non-int value
    is found function will return false. Else, pointer to int
    will be given the int value.*/

    UINT32 index = 0;
    UINT32 value = 0;

    while (c[index] != '\0'){
        if (c[index] > '9' || c[index] < '0') { return false; }
        //Multiply running total by 10, and add the integer
        value *= 10;
        value += (c[index] - '0');
        index++;
    }

    //If we are at this point, assign value to *i and return true.
    *i = value;
    return true;
}

