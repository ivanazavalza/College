/*Created By: Jared Fowler
September 23, 2014
CSUN Comp 322L ; Prof. Fitzgerald

Catcher: a program that catches a number of predefined signals, and 
         prints status information on stdout.


Notes:


Signal:
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
                       ->  signal() sets the disposition of the signal signum to handler, which is
                           either SIG_IGN, SIG_DFL, or the address of a  programmer-defined  func‐
                           tion (a "signal handler").

                           signal()  returns  the previous value of the signal handler, or SIG_ERR
                           on error.  In the event of an error,  errno  is  set  to  indicate  the
                           cause.

                            SIG_ERROR : -1 : (void (*)(int)) -1
                            SIG_DEF   :  0 : (void (*)()) 0
                            SIG_IGN   :  1 : (void (*)()) 1



Kill:
int kill(pid_t pid, int sig);
                       ->  The  kill()  system  call can be used to send any signal to any process
                           group or process.

                           If pid is positive, then signal sig is sent to the process with the  ID
                           specified by pid.

                           If pid equals 0, then sig is sent to every process in the process group
                           of the calling process.

                           If pid equals -1, then sig is sent to every process for which the call‐
                           ing  process  has  permission  to  send  signals,  except for process 1
                           (init), but see below.

                           If pid is less than -1, then sig  is  sent  to  every  process  in  the
                           process group whose ID is -pid.

                           If  sig  is 0, then no signal is sent, but error checking is still per‐
                           formed; this can be used to check for the existence of a process ID  or
                           process group ID.

pause:
int pause(void);
                       ->  pause()  causes the calling process (or thread) to sleep until a signal
                           is delivered that either terminates the process or causes  the  invoca‐
                           tion of a signal-catching function.

                           pause()  returns  only when a signal was caught and the signal-catching
                           function returned.  In this case pause() returns -1, and errno  is  set
                           to EINTR.


*/

#include <signal.h>     //Used for: Signal(), Kill()
#include <sys/types.h>  //Used for: kill()
#include <unistd.h>     //Used for: pause(), getpid()
#include <stdlib.h>     //Used for: malloc()
#include <stdio.h>      //Used for: printf()
#include <string.h>     //Used for: strCpy()
#include <inttypes.h>   //Used for: typeCast "intmax_t"
#include <time.h>       //Used for: time()
#include <ctype.h>      //Used for: toupper()




//typedef's and define's-------------------------------------
typedef unsigned char     BYTE;
typedef unsigned int      UINT32;
typedef signed int        INT32;

#define DEBUGGING         0

//Global Constants-------------------------------------------
    //Void

//Prototypes-------------------------------------------------
void funcHandler(int sigNumber);
BYTE configureSigString(char** dest, const char* source);
void makeAllUpper(char* str); 
int sig_name2number(char *s);
void sig_number2name(UINT32 sigNum, char* sigName);
void cleanUp(char** ary, BYTE size);
BYTE mailBox(BYTE function);

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/*FUNCTIONS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

int main(int argc, char * argv[]){
    
    //Local Variables:
    char** sigArgList = NULL; //To hold modified/corrected user arguments
    BYTE index        = 0;    //General index for loops
    BYTE termUsed     = 0;    //Is SIGTERM one of the processes?
    INT32 tempVal     = 0;    //General use int
    UINT32 sigCount   = 0;    //Number of times an interrupt occurs


    /*We'll want to take some steps to correctly analyze the
    users input. We first create a new 2d array to hold system
    modified values taken from argv.  We discover if these have
    "SIG" as their first three letters. The modified and hopefully
    correct values are stored into sigArgList. */

    //Allocate memory
    sigArgList = (char**)malloc( (argc - 1) * sizeof(char*) );

    //Fill our data structure with valid arguments
    for(index = 0; index < (argc - 1); index++){
        //Add "SIG" if needed and make all letters upper-case
        configureSigString( &(sigArgList[index]), argv[index + 1] );
    }

    #if(DEBUGGING)
    //Print out list of user arguments
    for(index = 0; index < argc - 1; index++)
        fprintf(stdout, "%s\n", sigArgList[index]);
    #endif
    
    /*By requirement, print out the PID.  We'll next want to associate
    the user arguments, now in sigArgList, with a handler. We will do
    this by the use of the function signal().  If any of the user 
    arguments fail, they will simply be ignored.  We will want to make
    sure that "TERM" is included, else, the program will never end.*/
    
    //Print out the process ID
    fprintf( stderr, "%s: $$ = %jd\n", argv[0], (intmax_t)getpid() );

    /*Register handlers for each signal event. We will ignore errors,
    but readily watch to see if SIGTERM is registered.*/
    for(index = 0; index < (argc - 1); index++){

        //Is SIGTERM used?
        if( !(strcmp(sigArgList[index] , "SIGTERM")) )
            termUsed = 1;

        //Get signal numerical value
        tempVal = sig_name2number(sigArgList[index]);

        //Set function handler for signal
        if(tempVal >= 0) //Value found
            signal(tempVal, funcHandler);

    }

    //Is SIGTERM used?  If not, exit the program.
    if(termUsed != 1){
        fprintf(stderr, "Error: SIGTERM must be an argument.\n");
        cleanUp(sigArgList, argc - 1);
        exit(1);
    }

    //Create an infinite loop which will run until SIGTERM is caught thrice
    while(1){
        //Wait for next interrupt
        pause();
        //Update interrupt count
        sigCount++;
        //Are we done? - Check mailbox flag
        if(mailBox(0)) break;
    }

    //Print via stderr the number of interrupts
    fprintf( stderr, "%s: Total Signal Count = %d\n", argv[0], sigCount );

    //Clean-up
    cleanUp(sigArgList, argc - 1);

    return 0;
}

//###########################################################
void funcHandler(int sigNumber){

    //We only want to init. these once but kept within this scope
    static int termCount   = 0;   //Term must be counted 3 times
    static char buffer[10] = {0}; //Holds the name of the signal

    //Re-register signal (OS resets the sig table each time)
    signal(sigNumber, funcHandler);

    //Get the name of the signaling interrupt
    sig_number2name(sigNumber, buffer);

    //Print the signal that was caught and the time it was caught
    fprintf( stdout, "%s caught at %jd\n", buffer, time(NULL));

    //Update the termCount (We run program until termCount == 3)
    if(sigNumber == SIGTERM)
        termCount++;
    else
        termCount = 0;

    //Are we done?
    if(termCount >= 3){
        //Set our mailbox flag
        mailBox(1);
    }

}

//###########################################################
BYTE configureSigString(char** dest, const char* source){

    /*Takes a source char array and determines if it starts 
    with "SIG".  If it does, dest will allocate memory and 
    make a copy of the source string, else, we will allocate
    memory, add "SIG", and finally copy the rest of the char 
    array.  Will make every letter a capital letter. If "SIG" 
    is added we return 1, else we return 0. */

    //Local Variables
    UINT32 count = 0;

    //Get the size of string pointed to by source
    while(source[count] != '\0')
        count++;

    //Are the first 3 chars of source "SIG"? - Warning: put to upper so that we don't get SIGsig
    if( (count > 2) && ((toupper(source[0]) == 'S') && (toupper(source[1]) == 'I') && (toupper(source[2]) == 'G')) ){

        //Copy the source string to the destinatino string
        *dest = (char*)malloc( (count + 1) * sizeof(char) );
        strcpy(*dest, source);

        //Make sure all the letters are upper-case
        makeAllUpper(*dest);

        return 0;

    }
    else{ //The first 3 chars are not "SIG".  We need to add them.
    
        //Allocate memory for new char array
        *dest = (char*)malloc( (count + 4) * sizeof(char) );

        //Place "SIG" in the first 3 locations of the array
        (*dest)[0] = 'S';
        (*dest)[1] = 'I';
        (*dest)[2] = 'G';

        //Copy the rest of source into dest
        strcpy((*dest) + 3, source);

        //Make sure all the letters are upper-case
        makeAllUpper(*dest);

        return 1;

    }

}

//###########################################################
void makeAllUpper(char* str){

    /*Basic function which converts a string into all upper-case.
    Will run until a null terminator is reached.*/

    UINT32 index = 0;

    while(str[index] != '\0'){
        str[index] = toupper(str[index]);
        index++;
    }

}

//###########################################################
int sig_name2number(char *s) {

    /*Basic map of possible signal names to numbers*/

    //Set default return value for signal not found
    int signumber = -1;

    if      (!strcmp(s, "SIGHUP"))     { signumber = SIGHUP; }
    else if (!strcmp(s, "SIGINT"))     { signumber = SIGINT; }
    else if (!strcmp(s, "SIGQUIT"))    { signumber = SIGQUIT; }
    else if (!strcmp(s, "SIGILL"))     { signumber = SIGILL; }
    else if (!strcmp(s, "SIGTRAP"))    { signumber = SIGTRAP; }
    else if (!strcmp(s, "SIGABRT"))    { signumber = SIGABRT; }
    else if (!strcmp(s, "SIGIOT"))     { signumber = SIGIOT; }
    else if (!strcmp(s, "SIGBUS"))     { signumber = SIGBUS; }
    else if (!strcmp(s, "SIGFPE"))     { signumber = SIGFPE; }
    else if (!strcmp(s, "SIGKILL"))    { signumber = SIGKILL; }
    else if (!strcmp(s, "SIGUSR1"))    { signumber = SIGUSR1 ; }
    else if (!strcmp(s, "SIGSEGV"))    { signumber = SIGSEGV ; }
    else if (!strcmp(s, "SIGUSR2"))    { signumber = SIGUSR2 ; }
    else if (!strcmp(s, "SIGPIPE"))    { signumber = SIGPIPE ; }
    else if (!strcmp(s, "SIGALRM"))    { signumber = SIGALRM ; }
    else if (!strcmp(s, "SIGTERM"))    { signumber = SIGTERM ; }
    else if (!strcmp(s, "SIGSTKFLT"))  { signumber = SIGSTKFLT ; }
    else if (!strcmp(s, "SIGCHLD"))    { signumber = SIGCHLD ; }
    else if (!strcmp(s, "SIGCONT"))    { signumber = SIGCONT ; }
    else if (!strcmp(s, "SIGSTOP"))    { signumber = SIGSTOP ; }
    else if (!strcmp(s, "SIGTSTP"))    { signumber = SIGTSTP ; }
    else if (!strcmp(s, "SIGTTIN"))    { signumber = SIGTTIN ; }
    else if (!strcmp(s, "SIGTTOU"))    { signumber = SIGTTOU ; }
    else if (!strcmp(s, "SIGURG"))     { signumber = SIGURG ; }
    else if (!strcmp(s, "SIGXCPU"))    { signumber = SIGXCPU ; }
    else if (!strcmp(s, "SIGXFSZ"))    { signumber = SIGXFSZ ; }
    else if (!strcmp(s, "SIGVTALRM"))  { signumber = SIGVTALRM ; }
    else if (!strcmp(s, "SIGPROF"))    { signumber = SIGPROF ; }
    else if (!strcmp(s, "SIGWINCH"))   { signumber = SIGWINCH ; }
    else                               { /*Do Nothing*/ }

  return signumber;
}

//###########################################################
void sig_number2name(UINT32 sigNum, char* buffer) {

    /*Basic map of possible signal numbers to name.  Will
    write name to passed in argument...Therefore, argument
    should be large enough to handle any of the names below
    (10 chars long). Returns 0 if sucessful, -1 otherwise.*/


    if      (sigNum == SIGHUP)     { strcpy(buffer, "SIGHUP"); }
    else if (sigNum == SIGINT)     { strcpy(buffer, "SIGINT"); }
    else if (sigNum == SIGTRAP)    { strcpy(buffer, "SIGTRAP"); }
    else if (sigNum == SIGABRT)    { strcpy(buffer, "SIGABRT"); }
    else if (sigNum == SIGIOT)     { strcpy(buffer, "SIGIOT"); }
    else if (sigNum == SIGBUS)     { strcpy(buffer, "SIGBUS"); }
    else if (sigNum == SIGFPE)     { strcpy(buffer, "SIGFPE"); }
    else if (sigNum == SIGKILL)    { strcpy(buffer, "SIGKILL"); }
    else if (sigNum == SIGUSR1)    { strcpy(buffer, "SIGUSR1"); }
    else if (sigNum == SIGSEGV)    { strcpy(buffer, "SIGSEGV"); }
    else if (sigNum == SIGUSR2)    { strcpy(buffer, "SIGUSR2"); }
    else if (sigNum == SIGPIPE)    { strcpy(buffer, "SIGPIPE"); }
    else if (sigNum == SIGALRM)    { strcpy(buffer, "SIGALRM"); }
    else if (sigNum == SIGTERM)    { strcpy(buffer, "SIGTERM"); }
    else if (sigNum == SIGSTKFLT)  { strcpy(buffer, "SIGSTKFLT"); }
    else if (sigNum == SIGCHLD)    { strcpy(buffer, "SIGCHLD"); }
    else if (sigNum == SIGCONT)    { strcpy(buffer, "SIGCONT"); }
    else if (sigNum == SIGTSTP)    { strcpy(buffer, "SIGTSTP"); }
    else if (sigNum == SIGTTIN)    { strcpy(buffer, "SIGTTIN"); }
    else if (sigNum == SIGTTOU)    { strcpy(buffer, "SIGTTOU"); }
    else if (sigNum == SIGURG)     { strcpy(buffer, "SIGURG"); }
    else if (sigNum == SIGXCPU)    { strcpy(buffer, "SIGXCPU"); }
    else if (sigNum == SIGXFSZ)    { strcpy(buffer, "SIGXFSZ"); }
    else if (sigNum == SIGVTALRM)  { strcpy(buffer, "SIGVTALRM"); }
    else if (sigNum == SIGPROF)    { strcpy(buffer, "SIGPROF"); }
    else if (sigNum == SIGWINCH)   { strcpy(buffer, "SIGWINCH"); }
    else                           { strcpy(buffer, ""); }

}

//###########################################################
void cleanUp(char** ary, BYTE size){

    /*Basic function that will free memory pointed to by pointers
    in an array. Will go until size is reached or a pointer to
    NULL is reached. Finally, the array of pointers will also
    be freed.*/
       
    int index = 0;
    for(index = 0; index < size; index++)
    {         
        if (ary[index] == NULL)
            break;
        else
            free(ary[index]);
    }

    free(ary);

}

//###########################################################
BYTE mailBox(BYTE function){
    
    /*We need some way to communicate with the main program
    that we are done, but being done is determined in a sub-
    function.  I can't used exit b/c we want to print some
    more info in main before exiting.  I would normally just
    "allow" one global varaible, but in order to avoid doing
    such, this funciton will act as a kind of "mailbox" between
    functions.  The arguments should be 1 for set and 0 for get,
    and 2 for peek which checks but doesn't put the flag down.
    Return value will be 1/0 for peek and get, and will always
    be 0 for set.*/

    static BYTE flag = 0;

    if(function == 1){ //Set the flag
        flag = 1;
        return 0;
    }
    else if(function == 0){ //Get the flag
        if(flag){
            flag = 0;
            return 1;
        }
        else
            return 0;
    }
    else{ //Peek at the flag
        return flag;
    }
}
