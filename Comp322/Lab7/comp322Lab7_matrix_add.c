/*Created By: Jared Fowler
November 13, 2014
CSUN Comp 322L ; Prof. Fitzgerald


Notes:

    matrix_add:

    A program that performs matrix addition on a given matrix, and emits the amount
    of time to perform the operation on standard error.


VERY HELPFUL EXAMPLE:    http://fwheel.net/aio.html

int aio_read(struct aiocb *aiocbp);
Link with -lrt.

       The  aio_read() function queues the I/O request described by the buffer
       pointed to by aiocbp.  This function  is  the  asynchronous  analog  of
       read(2).  The arguments of the call

           read(fd, buf, count)

       correspond (in order) to the fields aio_fildes, aio_buf, and aio_nbytes
       of the structure pointed to by aiocbp.

       On success, 0 is returned.  On error the request is not enqueued, -1 is
       returned, and errno is set appropriately.


int aio_write(struct aiocb *aiocbp);
Link with -lrt.

       The aio_write() function queues the I/O request described by the buffer
       pointed to by aiocbp.  This function  is  the  asynchronous  analog  of
       write(2).  The arguments of the call

           write(fd, buf, count)

       correspond (in order) to the fields aio_fildes, aio_buf, and aio_nbytes
       of the structure pointed to by aiocbp.

       On success, 0 is returned.  On error the request is not enqueued, -1 is
       returned, and errno is set appropriately. 


int aio_error(const struct aiocb *aiocbp);
Link with -lrt.

       The  aio_error()  function  returns the error status for the asynchronous
       I/O request with control block pointed to by aiocbp.  (See aio(7)  for  a
       description of the aiocb structure.)

       This function returns one of the following:

       *  EINPROGRESS, if the request has not been completed yet.

       *  ECANCELED, if the request was canceled.

       *  0, if the request completed successfully.


ssize_t aio_return(struct aiocb *aiocbp);
Link with -lrt.

       The aio_return() function returns the final return status for the asyn‐
       chronous I/O request with control block pointed  to  by  aiocbp.   (See
       aio(7) for a description of the aiocb structure.)

       This  function  should be called only once for any given request, after
       aio_error(3) returns something other than EINPROGRESS.

       If the asynchronous I/O operation has completed, this function  returns
       the  value  that  would  have  been  returned  in case of a synchronous
       read(2), write(2), fsync(2) or fdatasync(2), call.

       If the asynchronous I/O operation has not  yet  completed,  the  return
       value and effect of aio_return() are undefined.

       READ RETURN:
       On success, the number of bytes read is returned (zero indicates end of
       file), and the file position is advanced by this number.  It is not  an
       error  if  this  number  is smaller than the number of bytes requested;
       this may happen for example because fewer bytes are actually  available
       right  now  (maybe  because we were close to end-of-file, or because we
       are reading from a pipe, or from a terminal),  or  because  read()  was
       interrupted  by  a  signal.  On error, -1 is returned, and errno is set
       appropriately.  In this case it is left unspecified  whether  the  file
       position (if any) changes.



        struct aiocb {
               //The order of these fields is implementation-dependent

               int             aio_fildes;     // File descriptor
               off_t           aio_offset;     // File offset 
               volatile void  *aio_buf;        // Location of buffer 
               size_t          aio_nbytes;     // Length of transfer 
               int             aio_reqprio;    // Request priority 
               struct sigevent aio_sigevent;   // Notification method 
               int             aio_lio_opcode; // Operation to be performed;
                                                  lio_listio() only 

               //Various implementation-internal fields not shown
        };

*/


#include <stdio.h>        //Used for: printf()
#include <time.h>         //Used for: time()
#include <stdlib.h>       //Used for: srand(), rand()
#include <unistd.h>       //Used for: exit()
#include <aio.h>          //Used for: aio_read()
#include <string.h>       //Used for: memset()
#include <errno.h>


//typedef's and define's-------------------------------------
typedef unsigned char     BYTE;
typedef unsigned int      UINT32;
typedef signed int        INT32;
typedef unsigned char     BOOL;

#define true              1
#define false             0

#define DEBUGGING         0

#define MIN_RAND          -100
#define MAX_RAND          100


//Prototypes-------------------------------------------------
void matrixScalarAdd(INT32* block, UINT32 size, INT32 scalar);
void matrixScalarAddLab7(char* block, UINT32 size, INT32 scalar);
BOOL stringToUINT32(char* c, UINT32* i);

//Globals-----------------------------------------------------


/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/*FUNCTIONS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
int main(int argc, char * argv[]){

    //Local variables
    time_t startTime = 0;
    time_t endTime   = 0;
    INT32 scalar     = 0;      //Value to be added to each value in matrix
    UINT32 blockSize = 0;      //Size of block in BYTES
    UINT32 size      = 0;      //User passed in. Defines size x size matrix
    UINT32 blocks    = 0;      //User passed in. Defines number of divisions to make
    char* buffer1    = NULL;
    char* buffer2    = NULL;
    char* buffer3    = NULL;
    struct aiocb aio1;         //Argument used for aio operations
    struct aiocb aio2;         //Argument used for aio operations
    UINT32 i,j;

    //Valid number of arguments?
    if(argc < 3){
        fprintf(stderr, "Error: Invalid argument count.\n");
        exit(1);
    }

    //Attempt to get the size of the matrix
    if(!stringToUINT32(argv[1], &size)){
        fprintf(stderr, "Error: arg1 needs be of type UINT\n");
        exit(1);
    }

    //Attempt to get the number of blocks
    if(!stringToUINT32(argv[2], &blocks)){
        fprintf(stderr, "Error: arg2 needs be of type UINT\n");
        exit(1);
    }

    //Does the size and number of blocks make sense?
    if(blocks <= 0 || size <= 0){
        fprintf(stderr, "Error: Matrix size and/or # of blocks cannot be 0\n");
        exit(1);
    }

    //Determine the block size. If size is 0, report error.
    if( (blockSize = ( ( (size * size) / (blocks * blocks) ) * sizeof(int) ) ) == 0 ){
        fprintf(stderr, "Error: (Size*Size) < Blocks.\n");
        exit(1);
    }

    //We'll also want to make sure that the number of blocks divides the matrix evenly
    if( (size % blocks) != 0 ){
        fprintf(stderr, "Error: Blocks doesn't divide matrix evenly.\n");
        exit(1);
    }

    /*The way in which we allocate this buffer is very determinent upon the matrix_gen
    programs form of storing values into a file. We assume that every interger stored 
    will take up sizeof(int). Because of this precision, we don't need any type of physicaly
    visual seperator.*/
    if((buffer1 = (char*)calloc(blockSize, sizeof(char) )) == NULL){
        fprintf(stderr, "Error: Failed to allocate buffer memory.\n");
        exit(1);
    }
    if((buffer2 = (char*)calloc(blockSize, sizeof(char))) == NULL){
        fprintf(stderr, "Error: Failed to allocate buffer memory.\n");
        exit(1);
    }
    if((buffer3 = (char*)calloc(blockSize, sizeof(char))) == NULL){
        fprintf(stderr, "Error: Failed to allocate buffer memory.\n");
        exit(1);
    }

    /*Prepare the structure argument needed for the aio reads, returns, writes*/

    //Input
    memset(&aio1, 0, sizeof(struct aiocb));
    aio1.aio_nbytes = blockSize;
    aio1.aio_fildes = 0; //stdin
    aio1.aio_offset = 0;
    aio1.aio_buf    = buffer1;
    
    //Output
    memset(&aio2, 0, sizeof(struct aiocb));
    aio2.aio_nbytes = blockSize;
    aio2.aio_fildes = 1; //stdout
    aio2.aio_offset = 0;
    aio2.aio_buf    = buffer2;


    //Seed the random number generator
    srand((UINT32)time(NULL));

    //Get a random number
    scalar = ( ( rand() % (MAX_RAND - MIN_RAND) ) + MIN_RAND );

    #if(DEBUGGING)
    fprintf(stderr, "Size: %u, Blocks: %u, BlockSize: %u, Scalar: %d\n", size, blocks, blockSize, scalar);
    #endif

    //Get the start time
    startTime = time(NULL);

    /*Now that we have all of our needed information, we now read, add, and write.
    Instead of doing this in a linear fashion, however, we will be mixing it up
    in order to do things async rather than sync.*/

    //Flags
    BYTE current, last;
    current = last = 0;
    i = blocks * blocks;

    for( ; i > 0; i--){

        //NOTE:
            //current :: buffer3
            //next    :: buffer1
            //last    :: buffer2

        //Attempt to read from stdin  -buffer1
        if(aio_read(&aio1) == -1){
            fprintf(stderr, "Unable to read from input file.\n");
            goto cleanUp;
        }

        //While we wait for the information to be read, do math -buffer3
        if(current)
            matrixScalarAddLab7(buffer3, blockSize, scalar);

        //Start up the write procedure of the last
        if(last){
            if(aio_write(&aio2) == -1){
                fprintf(stderr, "Unable to write to output file.\n");
                goto cleanUp;
            }

            //Wait until the write request has finished
            while(aio_error(&aio2) == EINPROGRESS)
                ;

            //Get a return from the write process
            if(aio_return(&aio2) == -1){
                fprintf(stderr, "Write fail.\n");
                goto cleanUp;
            }

            //Update the offset
            aio2.aio_offset += blockSize;

        }

        //Copy current buffer over to last
        if(current){
            memcpy(buffer2, buffer3, blockSize);
            last = 1;
        }

        //Wait until the read request has finished
        while(aio_error(&aio1) == EINPROGRESS)
            ;

        //Get the read return
        if(aio_return(&aio1) == -1){
            fprintf(stderr, "Read fail.\n");
            goto cleanUp;
        }

        //Update the offset
        aio1.aio_offset += blockSize;

        //Copy next buffer over to current
        memcpy(buffer3, buffer1, blockSize);
        current = 1;
     
    }

    //We need to drain the pipe

    if(last){
        if(aio_write(&aio2) == -1){
            fprintf(stderr, "Unable to write to output file.\n");
            goto cleanUp;
        }  

        //Wait until the write request has finished
        while(aio_error(&aio2) == EINPROGRESS)
            ;

        //Get a return from the write process
        if(aio_return(&aio2) == -1){
            fprintf(stderr, "Write fail.\n");
            goto cleanUp;
        }      

        //Update the offset
        aio2.aio_offset += blockSize;
    }

    if(current){
        matrixScalarAddLab7(buffer3, blockSize, scalar);
        memcpy(buffer2, buffer3, blockSize);

        if(aio_write(&aio2) == -1){
            fprintf(stderr, "Unable to write to output file.\n");
            goto cleanUp;
        }  

        //Wait until the write request has finished
        while(aio_error(&aio2) == EINPROGRESS)
            ;

        //Get a return from the write process
        if(aio_return(&aio2) == -1){
            fprintf(stderr, "Write fail.\n");
            goto cleanUp;
        } 

        //Update the offset
        aio2.aio_offset += blockSize;
    }


    //Get the end time
    endTime = time(NULL);

    //Print out the throughput time
    fprintf(stderr, "Turnaround Time: %u\n", (UINT32)(endTime - startTime) );

    //Final clean up
    cleanUp: //Label that we can jump to
    free(buffer1);
    free(buffer2);
    free(buffer3);

    return 0;
}

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
void matrixScalarAdd(INT32* block, UINT32 size, INT32 scalar){

    /*Assumes block to be a (size x size) matrix. Adds the passed
    in scalar value to each value within the matrix.*/

    UINT32 i, j;

    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            
            block[(i * size) + j] += scalar;
        }
    }
}

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
void matrixScalarAddLab7(char* block, UINT32 size, INT32 scalar){

    /*Assumes block to be a (size x size) matrix. Adds the passed
    in scalar value to each value within the "matrix". 
    This is a special version of the function which is specific to the way
    information is pulled and output to a file.*/

    UINT32 j, k;
    INT32 value = 0;
    BYTE intSize = sizeof(int);

    for(j = 0; j < size; j += intSize){

        //We first want to interpret the n bytes that make up a integer
        value = 0;
        for(k = 0; k < intSize; k++){
            value = (value << 8) | block[j + k];
        }

        //Now add the scalar value to our interpreted int
        value += scalar;

        //Output the individual bytes (not ascii) that make up the integer value
        for(k = 0; k < intSize; k++){
            block[j + k] = ((value << (8 * k)) >> ((intSize - 1) * 8));
        }

    }
    
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

