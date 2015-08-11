/*Created By: Jared Fowler
November 6, 2014
CSUN Comp 322L ; Prof. Fitzgerald


Notes:

A daemon and child process are created. Daemons are processes that live for a long time.
They are often started when the system is bootstrapped and terminate only when the system
is shutdown. Because they don't have a controlling terminal, we say that they run in the 
background.

The daemon will spawn and terminate a child process upon receiving a signal. For this project,
these signals will be: TERM, USER1, USER2.





mode_t umask(mode_t mask);
    #include <sys/types.h>
    #include <sys/stat.h>

       umask()  sets  the calling process's file mode creation mask (umask) to
       mask & 0777 (i.e., only the file permission bits of mask are used), and
       returns the previous value of the mask.

       A  child  process  created  via fork(2) inherits its parent's umask.  The
       umask is left unchanged by execve(2).

       This system call always succeeds and the previous value of the mask  is
       returned.


pid_t setsid(void);

       setsid()  creates  a  new session if the calling process is not a process
       group leader.  The calling process is the leader of the new session,  the
       process  group  leader  of  the new process group, and has no controlling
       terminal.  The process group ID and session ID of the calling process are
       set  to  the PID of the calling process.  The calling process will be the
       only process in this new process group and in this new session.

       On success, the (new) session ID of the calling process is returned.   On
       error, (pid_t) -1 is returned, and errno is set to indicate the error.


int chdir(const char *path);
int fchdir(int fd);

       chdir() changes the current working directory of the calling  process  to
       the directory specified in path.

       fchdir()  is identical to chdir(); the only difference is that the direc‐
       tory is given as an open file descriptor.

       On success, zero is returned.  On error, -1 is returned, and errno is set
       appropriately.


int getrlimit(int resource, struct rlimit *rlim);
int setrlimit(int resource, const struct rlimit *rlim);
   #include <sys/time.h>
   #include <sys/resource.h>

       The  getrlimit() and setrlimit() system calls get and set resource limits
       respectively.  Each resource has an associated soft and  hard  limit,  as
       defined by the rlimit structure:

           struct rlimit {
               rlim_t rlim_cur;  // Soft limit
               rlim_t rlim_max;  // Hard limit (ceiling for rlim_cur)
           };

       The  soft limit is the value that the kernel enforces for the correspond‐
       ing resource.  The hard limit acts as a ceiling for the  soft  limit:  an
       unprivileged  process may set only its soft limit to a value in the range
       from 0 up to the hard limit, and (irreversibly) lower its hard limit.   A
       privileged  process  (under Linux: one with the CAP_SYS_RESOURCE capabil‐
       ity) may make arbitrary changes to either limit value.

       RLIMIT_NOFILE
              Specifies a value one greater than  the  maximum  file  descriptor
              number  that  can  be  opened by this process.

       On success, these system calls return 0.  On error, -1 is  returned,  and
       errno is set appropriately.

*/


#include <stdio.h>        //Used for: printf()
#include <stdlib.h>       //Used for: exit(), malloc()
#include <sys/types.h>    //Used for: umask(), open()
#include <sys/stat.h>     //Used for: umask(), open()
#include <unistd.h>       //Used for: fork(), setsid(), chdir()
#include <sys/time.h>     //Used for: getrlimit()
#include <sys/resource.h> //Used for: getrlimit()
#include <fcntl.h>        //USed for: open()
#include <errno.h>        //Used for: errno
#include <signal.h>       //Used for: signal(), kill()
#include <sys/wait.h>     //Used for: wait()

//typedef's and define's-------------------------------------
typedef unsigned char     BYTE;
typedef unsigned int      UINT32;
typedef signed int        INT32;
typedef unsigned char     BOOL;

typedef unsigned long long int UINT64;

#define true              1
#define false             0
#define DEBUGGING         0

#define UMASK_MODE        0022   //rw - r - r
#define BUFF_SIZE         100
#define PROGRAM           "/mole"


//Prototypes-------------------------------------------------
void sigUSR_handler(int sigNumber);
void sigterm_handler(int sigNumber);

//Globals-----------------------------------------------------
BYTE exitFlag = false;
BYTE moleFlag = 0;

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/*FUNCTIONS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
int main(int argc, char * argv[]){

    //Local Variables
    pid_t child1  = 0;
    pid_t child2  = 0;
    BYTE  number  = 0;
    BYTE  exeFail = 0;

    //These arrays of char arrays will be used for calling mole (argv's)
    char *argvMole1[3] = { "mole1", getenv("HOME"), NULL };
    char *argvMole2[3] = { "mole2", getenv("HOME"), NULL };

    //We'll want to preserve a copy of our original working directory
    char wd_orig[BUFF_SIZE];
    
    //Get the working enviroment
    if(getcwd(wd_orig, BUFF_SIZE) == NULL){
        //Print out error message and exit
        fprintf(stderr, "Error while getting working enviroment.\n");
        exit(1);
    }

    //Attempt to add the to be called program name to the working enviroment path
    if(sprintf(wd_orig, "%s%s", wd_orig, PROGRAM) < 0){
        //Print out error message and exit
        fprintf(stderr, "Error while adding program name to enviroment.\n");
        exit(1);
    }

    #if(DEBUGGING)
    fprintf(stdout, "Working Directory: %s\n", wd_orig);
    #endif

    //Seed the random number generator
    srand( (unsigned int) time(NULL) );

    /************************** PART 1 ***********************************/
    /*********************************************************************/
    /*We want to take steps in order to turn this process into a Daemon*/

    //Step 1: Use umask() in order to set possible privaliges **************
    umask(UMASK_MODE);

    //Step 2: Fork and have the parent process exit ************************
    pid_t pid = fork();

    if(pid != 0){  //Is this the parent process?

        #if(DEBUGGING)
        fprintf(stdout, "Dying Parents PID: %u\n", getpid());
        fprintf(stdout, "Child's PID: %u\n", pid);
        #endif

        exit(0);
    }

    //Step 3: Create a new session and a new process group *****************
    if(setsid() == -1){ //Note: setsid sets this process to both gl and sl

        //Print error message
        fprintf(stderr, "Failed to create new session.\n");

        //End program with error
        exit(1);
    }

    //Step 4: Change the current working directory to be "/" (aka root) ****
    if(chdir("/") == -1){

        //Print error message
        fprintf(stderr, "Failed to change working directory.\n");

        //End program with error
        exit(1);
    }

    //Step 5: Close all unneeded file descriptors **************************

    //We'll need to get the max number of fd's
    struct rlimit lim;
    if(getrlimit(RLIMIT_NOFILE, &lim) == -1){

        //Print error message
        fprintf(stderr, "Failed to get max fd count.\n");

        //End program with error
        exit(1);
    }

    #if(DEBUGGING)
    fprintf(stdout, "MAX FD: %u\n", (UINT32)lim.rlim_cur);
    goto SKIP_FD_CLOSURE;
    #endif

    //Close all files
    UINT32 index;
    for(index = 0; index < lim.rlim_cur; index++)
        close(index);

    #if(DEBUGGING)
    SKIP_FD_CLOSURE:
    #endif

    //Step 6: Reopen the standard file descriptors to map to /dev/null *****
    if( ( open("/dev/null", O_RDONLY) == -1 ) || ( open("/dev/null", O_RDONLY) == -1 )
     || ( open("/dev/null", O_RDONLY) == -1 ) ){

        #if(DEBUGGING)
        fprintf(stdout, "Failed to open to /dev/nll\n");
        #endif

        //End program with error
        exit(1);
    }

    /*DAEMON PREPARATION COMPLETE!!!*/

    /************************** PART 2 ***********************************/
    /*********************************************************************/

    /*We now set the daemon to handle various signals. Afterwhich, we will enter
    into an infinite loop which will not terminate until SIG_TERM is received. Inside 
    this loop we will fork and execute a program called "mole" (in reference to Wack a Mole)
    It will then be up to the user to attempt to wack the mole (via SIG_USER{1,2}) */

    //Set up our signal handlers
    if(signal(SIGTERM, sigterm_handler) == SIG_ERR){
        #if(DEBUGGING)
        fprintf(stderr, "Failed to set SIGTERM handler. \n");
        #endif
        _exit(1);
    }
    if(signal(SIGUSR1, sigUSR_handler) == SIG_ERR){
        #if(DEBUGGING)
        fprintf(stderr, "Failed to set SIGTUSR1 handler. \n");
        #endif
        _exit(1);
    }
    if(signal(SIGUSR2, sigUSR_handler) == SIG_ERR){
        #if(DEBUGGING)
        fprintf(stderr, "Failed to set SIGTUSR2 handler. \n");
        #endif
        _exit(1);
    }


    /*Continue in this main loop until sigterm is sent to this process*/
    while(true){

        //Sleep a bit
        usleep(50000);

        //Has the mole flag been set?
        if(moleFlag != 0){

            #if(DEBUGGING)
            fprintf(stdout, "Child 1 PID: %d Child 2 PID: %d\n", child1, child2);
            #endif

            //Is the requested mole alive? If so, kill it.
            if( moleFlag == 1 && child1 != 0 ){

                if(kill(child1, SIGTERM) == -1){
                    #if(DEBUGGING)
                    fprintf(stderr, "Failed to kill child1: %d\n", child1);
                    #endif
                }
                else{
                    //Clear child from process list (Now a zombie process)
                    wait(NULL);
                    child1 = 0;
                }

            }

            else if( moleFlag == 2 && child2 != 0 ){

                if(kill(child2, SIGTERM) == -1){
                    #if(DEBUGGING)
                    fprintf(stderr, "Failed to kill child2: %d\n", child2);
                    #endif
                }
                else{
                    //Clear child from process list (Now a zombie process)
                    wait(NULL);
                    child2 = 0;
                }

            }

            //Reset the flag
            moleFlag = 0;

            //Randomly determine what number the next mole will be
            number = ( rand() % 2 ) + 1;

            #if(DEBUGGING)
            fprintf(stdout, "Random Number: %d\n", number);
            #endif

            //If the mole doesn't already exist, fork a child process.
            if( (number == 1) && (child1 == 0) ){

                child1 = vfork(); //Parents copy of child1 is set

                if(child1 == 0){

                    if(execve(wd_orig, argvMole1, NULL) == -1){

                        #if(DEBUGGING)
                        fprintf(stderr, "Failed to execute (1).  %d \n",  errno);
                        #endif

                        //Notify parent that execve failed
                        exeFail = 1;

                        exit(1);
                    }

                }
                else{ //Parent Process

                    //If execution failed, set child pid to zero again
                    if(exeFail){
                        child1 = 0;
                        exeFail = 0;
                    }

                    #if(DEBUGGING)
                    fprintf(stdout, "New Child1 Process PID: %d\n", child1);
                    #endif
                }

            }

            else if( (number == 2) && (child2 == 0) ){

                child2 = vfork(); //Parents copy of child2 is set

                if(child2 == 0){

                    if(execve(wd_orig, argvMole2, NULL) == -1){

                        #if(DEBUGGING)
                        fprintf(stderr, "Failed to execute (2).  %d \n",  errno);
                        #endif

                        //Notify parent that execve failed
                        exeFail = 1;

                        exit(1);
                    }
                }
                else{ //Parent Process

                    //If execution failed, set child pid to zero again
                    if(exeFail){
                        child2 = 0;
                        exeFail = 0;
                    }

                    #if(DEBUGGING)
                    fprintf(stdout, "New Child2 Process PID: %d\n", child2);
                    #endif
                }

            }

        }


        //Has the exit flag been set?
        if(exitFlag)
            break;
    
    }

    #if(DEBUGGING)
    fprintf(stderr, "Exiting Program...\n");
    #endif

    //Kill any living children
    kill(child1, SIGTERM);
    kill(child2, SIGTERM);

    return 0;

}

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
void sigUSR_handler(int sigNumber){

    if(sigNumber == SIGUSR1){

        //First, we need to re-arm this signal handler
        if(signal(SIGUSR1, sigUSR_handler) == SIG_ERR){
            #if(DEBUGGING)
            fprintf(stderr, "Failed to set SIGTUSR1 handler. \n");
            #endif
            exit(1);
        }
    
        //Set the moleFlag to reflect which handler
        moleFlag = 1;

    }
    else if(sigNumber == SIGUSR2){

        //First, we need to re-arm this signal handler
        if(signal(SIGUSR2, sigUSR_handler) == SIG_ERR){
            #if(DEBUGGING)
            fprintf(stderr, "Failed to set SIGTUSR2 handler. \n");
            #endif
            exit(1);
        }
    
        //Set the moleFlag to reflect which handler
        moleFlag = 2;

    }
    else{

        //Undefined Signal....
        #if(DEBUGGING)
        fprintf(stderr, "Undefined Signal...\n");
        #endif
        
        exit(1);
    }

}


/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
void sigterm_handler(int sigNumber){
    
    //Set the flag to exit the process successfuly
    exitFlag = true;

}

