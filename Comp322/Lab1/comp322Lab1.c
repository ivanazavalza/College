/*Created By: Jared Fowler
September 9, 2014
CSUN Comp 322L ; Prof. Fitzgerald

"Time and Forking Children"

A command line tool that will report basic process identification information,
and the associated times for running the command line tool.

The general processing of this tool is as follows:

●	The program prints the number of seconds since.. 		(see time(2))
●	The program creates a child process 			        (see fork(2))
●	The program will wait for the child to finish			(see waitpid(2))
●	The program and its child reports on the information
	○	The process ID of its parent  			        (see getppid(2))
	○	Its own process ID					(see getpid(2))
	○	The process ID of its child (if applicable)		(see fork(2))
	○	The return status of its child (if applicable)		(see exit(2), waitpid(2))
●	The program will report the following time information	        (see times(2))
	○	 user time
	○	 system time
	○	 user time of child
	○	 system time of child
●	The program prints the number of seconds since..	        (see time(2))



NOTES:

time_t time(time_t *t)  ->  Returns, and writes to *t, the time in seconds since the Epoch,
                            1970-01-01 00:00:00
							
pid_t fork(void)        ->  Creates a new process by duplicating the calling process.  The 
                            return value to the parent will be some number (probably PID),
							while the return value for the child will be zero.

clock_t times(struct tms *buf)  ->  tms is a struct of this nature:

                                    struct tms {
                                       clock_t tms_utime;  //user time 
                                       clock_t tms_stime;  //system time
                                       clock_t tms_cutime; //user time of children
                                       clock_t tms_cstime; //system time of children
                                    };
									
									The return value is the number of clock ticks that have
									elapsed since an arbitrary point in the past.

pid_t getpid(void)  ->  gets process id of the process calling this function
pid_t getppid(void) ->  gets process id of the parent of the process calling this function

pid_t waitpid(pid_t pid, int* status, int options)  ->  The  wait() system call suspends execution of the calling process until
                                                        one of its children terminates.
	
*/

#include <stdio.h>     //Used for printing to the screen
#include <time.h>      //Used to get time since special epoch
#include <sys/times.h> //Used for tms struct and associated values
#include <sys/types.h> //Used to get PPID and PID
#include <sys/wait.h>  //Used for waiting for a child process to finish
#include <unistd.h>    //Used to invoke the fork function
#include <inttypes.h>  //Used for typeCast "intmax_t"


//typedef's and define's-------------------------------------
typedef signed int          INT32;

//Global Constants-------------------------------------------

    //Void

//Prototypes-------------------------------------------------
void reportProcessInfo(INT32* childPID, INT32* status);
void reportTimeInfo(void);

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/*FUNCTIONS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

int main(int argc, char * argv[]){
	
	time_t timeValue = 0;  //Long Unsigned to hold time value
	INT32 status     = 0;  //Logs value returned from a child process end
	
	/*--------------------Print out Start--------------------*/
	//Get the start time
	time(&timeValue);
	
	//Print the start time
	printf("START: %jd \n", timeValue);
	
	/*For some reason the child process puts it PC (Process Counter)
	to one instruction before the fork.  To avoid this, we write a 
	line of code that doesn't do anything.*/
	status = 0;

	/*----------Create a child process using fork()----------*/
	//Create a child process. Child is returned the value 0.
	pid_t returnValue = fork();
	
	if(returnValue < 0){ //Did an error occur?
	
		printf("Error: Couldn't create child process!\n");
		return 1;
	}
	else if(!returnValue){ //Is this the child process?
		//Report the process information
		reportProcessInfo(&returnValue, &status);
	}
	else{ //This is the parent process

		/*The first thing we want to do is wait for the child to finish*/
		wait(&status);
			 
		//Now that the child is done, print out parent info
		reportProcessInfo(&returnValue, &status);
		
		//Print out time info
		reportTimeInfo();
		
		//Finally, print out a stop time, aka, time since epoch
		time(&timeValue);
		printf("STOP: %jd \n", timeValue);
	}
	
	
	return 0;
}

//------------------------------------------------------------
void reportProcessInfo(INT32* childPID, INT32* status){
	
	//Print out this process's process id
	printf("PID: %d", getpid());
	
	//Print out this process's parent process id
	printf(", PPID: %d", getppid());
	
	//If available, print out child process id and status
	if((*childPID)){
		printf(", CPID: %d", *childPID);
		printf(", RETVAL: %d", *status);
	}
	
	//We'll want to skip a space
	printf("\n");
	 
}

//------------------------------------------------------------
void reportTimeInfo(void){
	
	//Create a new times struct
	struct tms timeStruct;
	
	//Fill timeStruct with times values
	times(&timeStruct);
	
	//Print out time stamp information
	printf("USER: %jd, SYS: %jd, CUSER: %jd, CSYS: %jd\n", (intmax_t)timeStruct.tms_utime,
	                                                       (intmax_t)timeStruct.tms_stime,
                                                               (intmax_t)timeStruct.tms_cutime,
                                                               (intmax_t)timeStruct.tms_cstime);
	
}
