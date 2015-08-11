/*Created By: Jared Fowler
October 28, 2014
CSUN Comp 322L ; Prof. Fitzgerald


Notes:

A very simple program which will take a user given memory address location (less than 2^32) and
return the page number and offset of such address. The page size and max memory location values
can be changed below.

*/


#include <stdio.h>      //Used for: printf()
#include <stdlib.h>     //Used for: exit()


//typedef's and define's-------------------------------------

typedef unsigned char     BYTE;
typedef unsigned int      UINT32;
typedef signed int        INT32;
typedef unsigned char     BOOL;

typedef unsigned long long int UINT64;

#define true              1
#define false             0
#define DEBUGGING         0

#define PAGE_SIZE         4096         //4K = 4096 Bytes
#define MAX_ADDRESS       4294967295   //2^32 - 1


//Prototypes-------------------------------------------------
BOOL stringToUINT64(char* c, UINT64* i);

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/*FUNCTIONS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
int main(int argc, char * argv[]){

    //Local Variables:
    UINT64 usrAdd  = 0;
    UINT32 address = 0;
    UINT32 pageNum = 0;
    UINT32 offset  = 0;

    //Valid Argument Count?
    if(argc < 2){
        fprintf(stderr, "Invalid argument count.\n");
        exit(1);
    }

    //Well need to convert the second argument into a UINT32.
    if(!stringToUINT64(argv[1], &usrAdd)){
        fprintf(stderr, "Argument not a valid address.\n");
        exit(1);
    }

    //Is the converted number within range?
    if(usrAdd > MAX_ADDRESS){
        fprintf(stderr, "Address out of range.\n");
        exit(1);
    }

    //Else, assign user address to UINT32 address
    address = (UINT32)usrAdd;

    //At this point, determine the page and offset.
    pageNum = address / PAGE_SIZE;
    offset  = address - (pageNum * PAGE_SIZE);

    //Print out the results
    printf("    Address: %10u \n", address);
    printf("Page Number: %10u \n", pageNum);
    printf("     Offset: %10u \n", offset);          
     

    return 0;

}

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
BOOL stringToUINT64(char* c, UINT64* i){

    /*Converts a string of chars to an int. Assumes the char
    array to have integer values in char form. If non-int value
    is found function will return false. Else, pointer to int
    will be given the int value.*/

    UINT32 index = 0;
    UINT64 value = 0;

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
