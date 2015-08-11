/*Created By: Jared Fowler
November 07, 2014
CSUN Comp 322L ; Prof. Fitzgerald

Note:

A very simple program that is to accompany Lab6, which
sets up a daemon to handle a Wack a Mole sort of set up.

This program will output information to a hard coded file
name. The information printed will partially depend on 
the values passed in via argv.


*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>

#define DEBUGGING   0
#define OUTPUT      "lab6.log"
#define SLEEP_TIME  800000

void term_handler(int sigNumber);

//Global Vars ------------------------------------
char exitFlag = 0;

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
int main(int argc, char* argv[]){

    #if(DEBUGGING)
    printf("Starting up MOLE.\n");
    char wd[100] = {0};
    getcwd(wd, 100);
    printf("Working Dir: %s\n", wd);
    #endif

    /*We may have inherited a workign dir from the parent that forked us..
    Change dir to home directory if it has been passed in, else, work locally*/
    if( ( argc >= 2 ) && ( chdir(argv[1]) == -1 ) ){

        //Print error message
        fprintf(stderr, "Failed to change working directory.\n");

        //End program with error
        exit(1);
    }

    #if(DEBUGGING)
    getcwd(wd, 100);
    printf("New working Dir: %s\n", wd);
    #endif

    //Attempt to create/open the output file
    int fd = open(OUTPUT, O_WRONLY | O_APPEND | O_CREAT, 0666);

    if(fd == -1){
        #if(DEBUGGING)
        printf("Failed to open file: %d\n", fd);
        #endif
    }

    //Also, we'll want to set up a signal handler for SIGTERM
    signal(SIGTERM, term_handler);

    //Set up what we want to write to the file
    char buffer[20] = {0};
    sprintf(buffer, "Pop %s\n", argv[0]);

    while(1){

        //Sleep for a bit
        usleep(SLEEP_TIME);

        //Write to a file
        if(write(fd, buffer, 20) <= 0){      
            #if(DEBUGGING)
            printf("Failed to write to file.\n");
            perror(NULL);
            #endif
        }
        else{
            #if(DEBUGGING)
            printf("Wrote to file.\n");
            #endif
        }
        
        //Has the exit flag been triggered?
        if(exitFlag)
            break;

    }

    //Close the file we opened
    close(fd);

    return 0;
}

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
void term_handler(int sigNumber){

    #if(DEBUGGING)
    printf("Inside Handler. Closing.\n");
    #endif
    
    exitFlag = 1;
}
