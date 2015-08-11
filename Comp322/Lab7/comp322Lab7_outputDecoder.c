/*Created By: Jared Fowler
November 13, 2014
CSUN Comp 322L ; Prof. Fitzgerald


Notes:

    This is not a project requirement, but for the sake of decoding and 
    verifying the validity of my lab7 programs, I'm creating this simple
    program which will decode an output file from either matrix_gen 
    or matrix_add into a more read-able format via stdout.
    
    Usage:
       
    $ ./lab7_decode <fileName>


    Note: Assumes the file is full of int values, with each int being 4 bytes
    

*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

typedef unsigned int UINT32;


int main(int argc, char* argv[]){

    //Local Variables
    int fd;
    char buffer[4];
    unsigned int value;
    unsigned char temp = 0xFF;

    //Correct argument count?
    if(argc < 2)
        exit(1);

    //Attempt to open the file
    if((fd = open(argv[1], O_RDONLY)) == -1){
        fprintf(stderr, "Failed to open file.\n");
        exit(1);
    }

    //Attempt to read the file 4 bytes at a time until we reach EOF
    while(read(fd, buffer, 4) == 4){

        //Reset value
        value = 0;
        
        //Decode the read value
        value = ((value | (temp & buffer[0])) << 8);
        value = ((value | (temp & buffer[1])) << 8);
        value = ((value | (temp & buffer[2])) << 8);
        value = (value  | (temp & buffer[3]));

        //Send out via stdout
        fprintf(stdout, "%d ", value);
        
    }
 
    //Close file
    close(fd);

    return 0;
}
