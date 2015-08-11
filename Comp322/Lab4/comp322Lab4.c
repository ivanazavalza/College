/*Created By: Jared Fowler
October 14, 2014
CSUN Comp 322L ; Prof. Fitzgerald


Notes:

The idea of the dining philosophers is to have a group of phils. around a table.  There
is a bowl of rice in the middle and each phil. needs two chop-sticks (resources) in order
to eat from this bowl of rice.  The problem is that P1 shares one chop-stick with the phil. to
his left, and the other chop-stick with the phil. to his right. The objective of this program is
to deal with resource sharing and avoiding dead-lock.

Good article about semaphores:
http://www.exforsys.com/tutorials/programming-concepts/semaphore-in-unix-an-overview.html


Signal:

typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
        ->signal() sets the disposition of the signal signum to handler, which is
          either SIG_IGN, SIG_DFL, or the address of a  programmer-defined  function 
          (a "signal handler").
          signal()  returns  the previous value of the signal handler, or SIG_ERR
          on error.  In the event of an error,  errno  is  set  to  indicate  the
          cause.

            SIG_ERROR : -1 : (void (*)(int)) -1
            SIG_DEF   :  0 : (void (*)()) 0
            SIG_IGN   :  1 : (void (*)()) 1

Named semaphores
          ->A named semaphore is identified by a name of the form /somename;
            that  is,  a  null-terminated  string of up to NAME_MAX-4 (i.e.,
            251) characters consisting of an initial slash, followed by  one
            or  more  characters,  none of which are slashes.  Two processes
            can operate on the same named semaphore by passing the same name
            to sem_open(3).
            The  sem_open(3) function creates a new named semaphore or opens
            an existing named  semaphore.   After  the  semaphore  has  been
            opened, it can be operated on using sem_post(3) and sem_wait(3).
            When a process has finished using  the  semaphore,  it  can  use
            sem_close(3)  to  close  the semaphore.  When all processes have
            finished using the semaphore, it can be removed from the  system
            using sem_unlink(3).

sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value);
sem_t *sem_open(const char *name, int oflag);
        ->sem_open()  creates  a  new  POSIX semaphore or opens an existing sema‐
          phore.  The semaphore is identified by name. If  O_CREAT  is specified in
          oflag, then the semaphore is created if it does not already exist.
          If  O_CREAT  is specified in oflag, then the semaphore is
          created if it does not already exist.

int sem_close(sem_t *sem);
        ->sem_close() closes the named semaphore referred to by sem, allowing any
          resources that the system has allocated to the calling process for this
          semaphore to be freed. On success sem_close() returns 0; on error, -1
          is returned, with errno set to indicate the error.

int sem_post(sem_t *sem);
        ->sem_post()  increments  (unlocks)  the semaphore pointed to by sem.  If
          the semaphore's value consequently  becomes  greater  than  zero,  then
          another  process  or thread blocked in a sem_wait(3) call will be woken
          up and proceed to lock the semaphore.  sem_post() returns 0 on success;
          on error, the value of  the  semaphore is  left  unchanged,  -1  is
          returned, and errno is set to indicate the error.

int sem_wait(sem_t *sem);
        ->sem_wait() decrements (locks) the semaphore pointed to by sem.  If the
          semaphore's value is greater than zero, then the decrement proceeds, and
          the function returns, immediately.  If the semaphore currently has the value
          zero, then the call blocks until either it becomes possible to perform the
          decrement  (i.e., the semaphore value rises above zero), or a signal handler
          interrupts the call.

int sem_trywait(sem_t *sem);
          ->sem_trywait()  is  the  same  as  sem_wait(), except that if the decrement cannot
          be immediately performed, then call returns an error (errno set to EAGAIN) instead
          of blocking.

int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
        ->sem_timedwait() is the same as sem_wait(), except that abs_timeout specifies a limit
          on the amount of time that the call should block if the decrement  cannot be immediately
          performed.  The abs_timeout argument points to a structure that specifies an absolute timeout
          in seconds and nanoseconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).  This structure
          is defined as follows:

        struct timespec {
            time_t tv_sec;      //Seconds
            long   tv_nsec;     //Nanoseconds [0 .. 999999999]
        };

          If the timeout has already expired by the time of the call, and the semaphore could not be
          locked immediately, then sem_timedwait()  fails  with  a  timeout error (errno set to ETIMEDOUT).
          If  the operation can be performed immediately, then sem_timedwait() never fails with a timeout
          error, regardless of the value of abs_timeout.  Furthermore, the validity of abs_timeout is not
          checked in this case.

          All of these functions return 0 on success; on error, the value of the semaphore is left unchanged, -1 is returned,
          and errno is set to indicate the error.

unsigned int sleep(unsigned int seconds);
        ->sleep()  makes  the  calling  thread  sleep  until seconds seconds have
          elapsed or a signal arrives which is not ignored.

int usleep(useconds_t usec);
        ->The usleep() function suspends execution of the calling thread for  (at
         least)  usec microseconds.  The sleep may be lengthened slightly by any
         system activity or by the time spent processing  the  call  or  by  the
         granularity of system timers.
         The  usleep() function returns 0 on success.  On error, -1 is returned,
         with errno set to indicate the cause of the error.

*/


#include <stdio.h>      //Used for: printf()
#include <fcntl.h>      //Used for: sem_open
#include <sys/stat.h>   //Used for: sem_open
#include <semaphore.h>  //Used for: sem_* (open, close, post, wait*)
#include <unistd.h>     //Used for: sleep(), getpid()
#include <time.h>       //USed for: time()
#include <sys/types.h>  //Used for: getpid()
#include <stdlib.h>     //Used for: used for malloc
#include <signal.h>     //Used for: signal()
#include <errno.h>      //Used for error code numbers
//#include <sys/ipc.h>
//#include <sys/sem.h>


//typedef's and define's-------------------------------------

typedef unsigned char     BYTE;
typedef unsigned int      UINT32;
typedef signed int        INT32;
typedef unsigned char     BOOL;

#define true              1
#define false             0
#define DEBUGGING         0
#define FILE_NAME         "/jaredFChopstick"
#define PROCESS_SHARE     0600
#define MAX_SLEEP_TIME    50000    //millionths of a second
#define HoldAndWait       1
#define CircularWait      0
#define MaxHoldTime       100      //billionths of a second
#define ALLOW_DEADLOCK    0          

//Global Constants and Vars ---------------------------------
BOOL exitFlag = false;  //Break out of dining loop?

//Prototypes-------------------------------------------------
void eat(const UINT32* philNumber, UINT32* eatCount);
void think(const UINT32* philNumber, UINT32* thinkCount);
void funcHandler(int sigNumber);
BOOL stringToUINT32(char*, UINT32*);
BOOL appendNumToString(char** dest, const char* base, UINT32 num);

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/*FUNCTIONS $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
int main(int argc, char * argv[]){

    //Local Variables:
    UINT32 numberPhils = 0;     //How many philosiphers are there?
    UINT32 seatNumber  = 0;     //Which seat number is this process?
    sem_t* leftStick   = NULL;  // (Seat + 1) % N
    sem_t* rightStick  = NULL;  // (Seat - 1) : < 0 , N
    char*  lsName      = NULL;  //Name of left semaphore
    char*  rsName      = NULL;  //Name of right semaphore
    UINT32 lsNumber    = 0;     //Number of the left stick
    UINT32 rsNumber    = 0;     //Number of the right stick

    UINT32 eatCount    = 0;     //Number of times this phil. has eaten
    UINT32 thinkCount  = 0;     //Number of times this phil. has thunk

    UINT32 timeC       = 0;     //How much time phil. sit at table
    UINT32 index       = 0;     //General use index


    /*We expect to receive two arguments from the caller.  The
    first argument should be the number of philosophers at the
    table, and the second should be which philosopher this process
    is.  If these arguments are not met, exit the program.*/

    //Correct argument count?
    if (argc < 3){ fprintf(stderr, "Illegal Argument Count\n\n"); _exit(1); }

    //Correct argument types?
    if (!stringToUINT32(argv[1], &numberPhils)){
        fprintf(stderr, "Illegal Argument: %s\n\n", argv[1]);
        _exit(1);
    }
    if (!stringToUINT32(argv[2], &seatNumber)){
        fprintf(stderr, "Illegal Argument: %s\n\n", argv[2]);
        _exit(1);
    }

    #if(DEBUGGING)
    printf("Number Phils: %d  Seat: %d\n", numberPhils, seatNumber);
    #endif

    /*The program will end when the signal SIGTERM is sent. We want
    to catch and handle this ourselves.*/
    if(signal(SIGTERM, funcHandler) == SIG_ERR){
        fprintf(stderr, "Failed to set SIGTERM handler. \n");
        _exit(1);
    }

    /*We need to link this phil to his right and left stick. We will
    use the open() function, utlizing the O_CREAT option to either
    create a semaphore if it doesn't exist already, and if it does,
    just link to it.  We will set the file name as such:
    /chopstickXXX  where XXX...X represents a semaphore # 0 - N.*/

    /*Check for chopstick circle completion. We assume CC rotation, couting
    the right chopstick first.*/
    if(seatNumber + 1 > numberPhils)
        lsNumber = 1;
    else
        lsNumber = seatNumber + 1;

    rsNumber = seatNumber;

    //Get the names of the semaphore we want to create or link to
    appendNumToString(&rsName, FILE_NAME, rsNumber);
    appendNumToString(&lsName, FILE_NAME, lsNumber);

    #if(DEBUGGING)
    printf("LeftStickName: %s\n", lsName);
    printf("RightStickName: %s\n", rsName);
    #endif

    //Attempt to open the semaphores, or just link to them
    leftStick  = sem_open(lsName, O_CREAT | O_EXCL, PROCESS_SHARE, 1);
    if(leftStick == SEM_FAILED) { leftStick = sem_open(lsName, O_CREAT); }

    rightStick = sem_open(rsName, O_CREAT | O_EXCL, PROCESS_SHARE, 1);
    if(rightStick == SEM_FAILED) { rightStick = sem_open(rsName, O_CREAT); }

    //Did an error occurr?
    if(leftStick == SEM_FAILED){
        fprintf(stderr, "Failed to create/link to semaphore: %s\n", lsName);
        _exit(1);
    }
    if(rightStick == SEM_FAILED){
        fprintf(stderr, "Failed to create/link to semaphore: %s\n", rsName);
        _exit(1);
    }

    /*At this point we begin our main loop.  Wait, Eat, Signal, Think, Repeat.
    We will end the process when the waiter (an outer process) calls SIGTERM
    on this processes pid. */
    
    //Print the process id so we know how to signal it
    printf("PID: %d\n", getpid());

    //Seed the random number generator
    srand( (unsigned int) time(NULL) );
    
    //Get the start time at the table
    timeC = time(NULL);

    //Set up max HoldAndWait time is necessary
    #if(HoldAndWait)
    struct timespec hAw = {0, MaxHoldTime};
    #endif

    #if(HoldAndWait && DEBUGGING)
    printf("MaxWait Seconds: %jd  NanoSeconds: %jd \n", hAw.tv_sec, hAw.tv_nsec);
    #endif


    do{
        /*The user is given 2 choices to avoid dead-lock. Both cannot be activated b/c this would cause deadlock
        with itself*/

        if(ALLOW_DEADLOCK){
            sem_wait(rightStick);
            sem_wait(leftStick);
            goto WeHaveIt;
        }

        #if(HoldAndWait) //Phil. will not take one stick until both sticks can be taken at once
        //Attempt to obtain both chopsticks.  
        if(!sem_timedwait(leftStick, &hAw)){
            //Try to obtain the other
            if(sem_timedwait(rightStick, &hAw)){
                //Failed to get second stick, so give back leftStick
                sem_post(leftStick);
                if(DEBUGGING){ printf("F"); }
                //Restart the loop
                continue;
            } 
        } //We got both sticks!!!
        else{ //Failed!
            if(DEBUGGING){ printf("F"); }
            continue;
        }
        #endif

        #if(CircularWait) //Phil. will try to take right or left stick first depending on seat number
        //If seat is odd, take right stick first, even is oppposite
        if(seatNumber % 2 != 0){ //odd
            sem_wait(rightStick);
            sem_wait(leftStick);
        }
        else{ //even
            sem_wait(leftStick);
            sem_wait(rightStick);
        }
        #endif

WeHaveIt: //Label
        
        //Eat Chinese food. Mmmm Yummy.
        eat(&seatNumber, &eatCount);

        //Release the chopsticks
        #if(HoldAndWait || CircularWait)
        sem_post(rightStick);
        sem_post(leftStick);
        #endif

        //What is the meaning of life?
        think(&seatNumber, &thinkCount);

        //Is the restaurant closed?
        if(exitFlag)
            break;

    }while(true);


    //Print out number of cycles completed by this phil.
    fprintf(stderr, "Philosopher %d completed %d cycles in %jd seconds.\n\n", seatNumber, eatCount, time(NULL) - timeC );

    //Close the semaphores
    sem_close(leftStick);
    sem_close(rightStick);

    //Destroy the semaphore
    sem_unlink(lsName);
    sem_unlink(rsName);
    
    //Free of allocated space
    free(lsName);
    free(rsName);


    return 0;

}

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
void eat(const UINT32* philNumber, UINT32* eatCount){

    /* Message is output indicating that the phil is eating. 
    The eating count is incremented. Phil. then sleeps. */

    //Print out message
    fprintf(stdout, "Philosopher %d is eating    @ %jd.\n", *philNumber, time(NULL));

    //Update the eat count
    *eatCount = *eatCount + 1;

    //Sleep for a random ammount of time
    UINT32 sleepTime = rand();

    #if(DEBUGGING)
    printf("SleepTime Original: %d \n", sleepTime);
    #endif

    //Mod the sleep time by our maximum allowed sleep time
    sleepTime %= MAX_SLEEP_TIME;

    #if(DEBUGGING)
    printf("SleepTime New: %d \n", sleepTime);
    #endif

    //Sleep
    usleep( (useconds_t)sleepTime );

}

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
void think(const UINT32* philNumber, UINT32* thinkCount){

    /* Message is output indicating that the phil is thinking. 
    The thinking count is incremented. Phil. then sleeps. */

    //Print out message
    fprintf(stdout, "Philosopher %d is thinking  @ %jd.\n", *philNumber, time(NULL));

    //Update the eat count
    *thinkCount = *thinkCount + 1;

    //Sleep for a random ammount of time
    UINT32 sleepTime = rand();

    #if(DEBUGGING)
    printf("SleepTime Original: %d \n", sleepTime);
    #endif

    //Mod the sleep time by our maximum allowed sleep time
    sleepTime %= MAX_SLEEP_TIME;

    #if(DEBUGGING)
    printf("SleepTime New: %d \n", sleepTime);
    #endif

    //Sleep
    usleep( (useconds_t)sleepTime );

}

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
void funcHandler(INT32 i){

    //Set the exitFlag to true
    exitFlag = true;

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

/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
BOOL appendNumToString(char** dest, const char* base, UINT32 num){

    /*Appends a positive number to the end of a char array.*/

    //Local Variables:
    BYTE base_length = 0;
    BYTE num_length  = 0;
    BYTE index       = 0;
    UINT32 tempVal   = 0;
    UINT32 tenPow    = 0;

    //Get the number of chars in string user passed in
    index = 0;
    while(base[index] != '\0'){
        base_length++;
        index++;
    }

    //Get the number of digits in num
    tempVal = num;
    do{
        num_length++;
        tempVal /= 10;
    }while(tempVal != 0);

    #if(DEBUGGING)
    printf("StrLen: %d, NumLen: %d \n", base_length, num_length);
    #endif

    //Allocate a new char array
    *dest = (char*)malloc( (base_length + num_length + 1) * sizeof(char) );

    //Copy original string over to new string.
    for(index = 0; index < base_length; index++)
        (*dest)[index] = base[index];

    //Add to the string the char values of the number
    tempVal = num;
    tenPow = 1;
    for(index = 0; index < num_length - 1; index++)
        tenPow *= 10;

    for(index = 0; index < num_length; index++){
        //We know how many digits are in the number... divide, write, subtract, repeat.
        BYTE divVal = tempVal / tenPow ;
        (*dest)[index + base_length] = '0' + divVal;
        tempVal -= divVal * tenPow;
        tenPow /= 10;
    }

    //Add null terminator to end of array
    (*dest)[base_length + num_length] = '\0';


    return true;
    
}

