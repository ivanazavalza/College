/*Created By: Jared Fowler
September 16, 2014
CSUN Comp 322L ; Prof. Fitzgerald

"Tube"

A command line tool that launches two child processes and sets up 
 a pipe between them for inter-process communication


NOTES:

 int dup2(int oldfd, int newfd);  ->  Re-assigns a file dycripter on the table to point to something else


int pipe(int pipefd[2]);  ->  pipe()  creates  a pipe, a unidirectional data channel that can be used for interprocess communica‐
                              tion.  The array pipefd is used to return two file descriptors referring to the ends of  the  pipe.
                              pipefd[0] refers to the read end of the pipe.  pipefd[1] refers to the write end of the pipe. 


pid_t fork(void)        ->  Creates a new process by duplicating the calling process.  The 
                            return value to the parent will be some number (probably PID),
							while the return value for the child will be zero.

pid_t getpid(void)      ->  Gets process id of the process calling this function

pid_t waitpid(pid_t pid, 
              int* status, 
              int options)  ->  The  wait() system call suspends execution 
                                of the calling process until children terminates.

int execve(const char *filename, 
           char *const argv[],
           char *const envp[]);   ->  Executes a program pointed to by filename. On success, 
                                      execve() does not return, on error -1 is returned, and 
                                      errno is set appropriately.
	
*/

#include <stdio.h>     //Used for printing to screen
#include <sys/types.h> //Used to get PPID and PID
#include <sys/wait.h>  //Used for waiting for a child process to finish
#include <unistd.h>    //Used to invoke the fork function
#include <inttypes.h>  //Used for typeCast "intmax_t"
#include <stdlib.h>    //Used to allocate memory for pointers


//typedef's and define's-------------------------------------
#define INVALID_ARGUMENTS   -1
#define PIPE_FAIL           -2
#define FORK_FAIL           -3
#define DUP_FAIL            -4
#define EXEC_FAIL           -5

//Global Constants-------------------------------------------
    //Void

//Prototypes-------------------------------------------------
int findStringOffsetInArrayOfStrings(char** argv, char* val, int count);
int createNewArgv(char*** dest, char** source, int start, int end);

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/*FUNCTIONS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

int main(int argc, char * argv[]){
    
    int index      = 0;      //General index for loops
    int pipefd[2]  = {0,0};  //File Decryptor in, out
    pid_t cpid     = 0;      //Child 1 process id
    pid_t cpid2    = 0;      //Child 2 process id
    int returnVal  = 0;      //Child 1 return value
    int returnVal2 = 0;      //Child 2 return value
    int offset     = 0;      //2nd program argument offset in argv
    char** argv1   = NULL;   //Command args to pass into child 1
    char** argv2   = NULL;   //Command args to pass into child 2

    //Do we have valid input?
    if (argc < 4){  //We are expecting at least 2 executables with a comma seperator
        fprintf(stderr, "Error: Invalid Argument(s)\n");
        _exit(INVALID_ARGUMENTS);
    }

    //Valid input check 2 & get parser offset
    offset = findStringOffsetInArrayOfStrings(argv, ",", argc);
    if(offset < 0){
        fprintf(stderr, "Error: Invalid Argument(s). Missing ','\n");
        _exit(INVALID_ARGUMENTS);
    }

    //Valid input check 3 :: Is the comma seperator >= 3rd spot?
    if(offset < 2){
        fprintf(stderr, "Error: Invalid Argument(s)/Order\n");
        _exit(INVALID_ARGUMENTS);
    }


    //Allocate a pipe
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "Error: Failed to allocate pipe\n");
        //Clean up
        free(argv1);
        free(argv2);
        //Exit
        _exit(PIPE_FAIL);
    }

    //********Fork into 2 children************//

    cpid = fork();
    if(cpid < 0){  //Forking Successful?
        fprintf(stderr, "Error: Failed to fork\n");
        //Clean up
        free(argv1);
        free(argv2);
        //Exit
        _exit(FORK_FAIL);
    }
    
    if(cpid == 0){  //Is this the child process?
        
        //Link our write side of the pipe to stdout
        if( dup2(pipefd[1], 1 /*1 == stdout*/) < 0){
            fprintf(stderr, "Error: Failed to dup\n");
            _exit(DUP_FAIL);
        }

        //This child only writes, so we close the read side of the pipe
        close(pipefd[0]);


        //Prepare argument structure for child 1
        char** argv1 = (char**)malloc((offset) * sizeof(char*));

        //Have argv1 point to appropriate argv arguments
        for(index = 0; index < offset - 1; index++)
            argv1[index] = argv[index + 1];
        argv1[index] = NULL;


        //Attempt to execute the first program
        if(execve(argv1[0], argv1, NULL)){
            fprintf(stderr, "Error: EXEC 1 failure\n");
            free(argv1);  //Clean up
            _exit(EXEC_FAIL);
        }

            
    }
    else{  //This is the parent process

        //Fork the second child
        cpid2 = fork();
        if(cpid2 < 0){  //Forking Successful?
            fprintf(stderr, "Error: Failed to fork\n");
            //Clean up
            free(argv1);
            free(argv2);
            //Exit
            _exit(FORK_FAIL);
        }

        if(cpid2 == 0){  //Is this the child process?

            //Link our read side of the pipe to stdin
            if( dup2(pipefd[0], 0 /*0 == stdin*/) < 0){
                fprintf(stderr, "Error: Failed to dup\n");
                _exit(DUP_FAIL);
            }

            //This child only reads, so we close the write side of the pipe
            close(pipefd[1]);

            //Prepare argument structure for child 2
            char** argv2 = (char**)malloc((argc - offset) * sizeof(char*));

            //Have argv2 point to appropriate argv arguments
            for(index = 0; index < (argc - offset - 1); index++)
                argv2[index] = argv[(offset + 1 + index)];
            argv2[index] = NULL;


            //Attempt to execute the second program
            if(execve(argv2[0], argv2, NULL)){
                fprintf(stderr, "Error: EXEC 2 failure\n");
                free(argv2);  //Clean up
                _exit(EXEC_FAIL);
            }

        }
        else{  //This is the parent process

            //Print the process id's of both children
            fprintf(stderr, "%s: $$ %d\n", argv[1], cpid);
            fprintf(stderr, "%s: $$ %d\n", argv[offset + 1], cpid2);

            //Close parent's access to the pipe
            close(pipefd[0]);
            close(pipefd[1]);

            //Wait for the children processes to finish
            waitpid(cpid, &returnVal, 0);
            waitpid(cpid2, &returnVal2, 0);

            //Print the children's return value via stderr
            fprintf(stderr, "%s: $? %d\n", argv[1], returnVal);
            fprintf(stderr, "%s: $? %d\n", argv[offset + 1], returnVal);
    
        }

    }

    //We'll want to free up the pointers to pointers we created within the heap
    
    
    

    return 0;
}

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
int findStringOffsetInArrayOfStrings(char** argv, char* val, int count){

    /*Basic function that steps through an array of char* and
    tries to locate a specified string.  If found, its index
    in the pointer array is given, else, -1 is returned.  Will
    search until count is reached or null terminator found.*/

    //Get the size of string pointed to by val
    int lengthVal = 0;
    while(val[lengthVal] != '\0')
        lengthVal++;

    //Go through the array of strings.  Stop when null is reached
    int index = 0;
    int i;
    int valueFound = 1;


    for(index = 0; index < count; index++){
        //NULL terminator reached?
        if(argv[index][0] == '\0')
            return -1;

        //Set value found as true
        valueFound = 1;

        //Compare!
        for(i = 0; i < lengthVal + 1; i++){
            if(val[i] != argv[index][i]){
                valueFound = 0;
                break;
            }
        }

        if(valueFound)
            return index;
        
    }

    //Value not found
    return -1;

}
