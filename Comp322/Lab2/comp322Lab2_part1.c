/*Created By: Jared Fowler
September 16, 2014
CSUN Comp 322L ; Prof. Fitzgerald

"Launch"

A program that allows one to set in motion another program, while also report 
some basic information.


NOTES:

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
#include <stdlib.h>    //Used for malloc


//typedef's and define's-------------------------------------
#define NO_ARGUMENT_FOUND   -1
#define EXEC_FAIL           -2

//Global Constants-------------------------------------------
    //Void

//Prototypes-------------------------------------------------
    //Void

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/*FUNCTIONS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

int main(int argc, char * argv[]){
    
    //Local Variables:
    pid_t processID = 0;      //Will hold pid of child
    int returnValue = 0;      //General return value
    int status      = 0;      //Return value of child
    int index       = 0;      //General counter for loops


    //Error check 1: Enough arguments?
    if(argc < 2){
        fprintf(stderr, "Error: Invalid Argument(s)\n");
        _exit(NO_ARGUMENT_FOUND);
    }


    //Fork to a child process
    processID = fork();

    if(processID == 0){ //This is the child process

        /*Child process is to execute supplied program via argv*/

        //We'll need to prepare the arguments to put into child process
        char** argv1 = (char**)malloc(argc * sizeof(char*));
    
        //Have argv1 char* point to appropriate argv arguments
        for(index = 0; index < argc - 1; index++)  //Point to values in argv
            argv1[index] = argv[index + 1];
            argv1[index] = NULL;  //Last value NEEDS to be a null

        //Attempt to execute the program
        if(execve (argv1[0], argv1, NULL)){
            fprintf(stderr, "Error: Couldn't execute child program argument\n");
            free(argv1);  //Clean up -- Note: if exec is successful, heap is cleaned up auto
            _exit(EXEC_FAIL);
        }

    }
    /*Else we are in the parent process*/
    
    //Print out child processes PID via stderr
    fprintf(stderr, "%s: $$ = %jd\n", argv[1], (intmax_t)processID);

    //Wait for child process to finish
    waitpid(processID, &status, 0);

    //Print out child return value via stderr
    fprintf(stderr, "%s: $? = %d\n", argv[1], status);

    return 0;
}
