    ___                     ________       ________      ________  _________   
   |\  \                   |\   ____\     |\   __  \    |\  _____\|\___   ___\ 
   \ \  \    ____________  \ \  \___|_    \ \  \|\  \   \ \  \__/ \|___ \  \_| 
 __ \ \  \  |\____________\ \ \_____  \    \ \  \\\  \   \ \   __\     \ \  \  
|\  \\_\  \ \|____________|  \|____|\  \    \ \  \\\  \   \ \  \_|      \ \  \ 
\ \________\                   ____\_\  \    \ \_______\   \ \__\        \ \__\
 \|________|                  |\_________\    \|_______|    \|__|         \|__|
                              \|_________|                                     
                                                                               
                                                                               

	   _____    ____    __  __   _____    ____    ___    ___  
	  / ____|  / __ \  |  \/  | |  __ \  |___ \  |__ \  |__ \ 
	 | |      | |  | | | \  / | | |__) |   __) |    ) |    ) |
	 | |      | |  | | | |\/| | |  ___/   |__ <    / /    / / 
	 | |____  | |__| | | |  | | | |       ___) |  / /_   / /_ 
	  \_____|  \____/  |_|  |_| |_|      |____/  |____| |____|
		                                                  


			    Created By: Jared Fowler
			       September 9, 2014
			CSUN Comp 322L ; Prof. Fitzgerald


###################################################################################

___                         __                _     __                     
 |  o __  _     _ __  _|   |_  _  __ |  o __ (_|   /  |_  o  |  _| __ _ __ 
 |  | |||(/_   (_|| |(_|   |  (_) |  |< | | |__|   \__| | |  | (_| | (/_| |

###################################################################################

ABOUT:

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
●	The program prints the number of seconds since..	   (see time(2))

###################################################################################

STATUS:

September 10, 2014  :  Fully functional and working!

###################################################################################

USAGE:


$ time-4-baby-and-me

###################################################################################

UPDATE LOG:

September 10, 2014

-Further testing seems to reveal that the program was working properly by filling the
tms struct with zeros.  To verify this, two for loops were added to increase the run time
of the program. As a result, the values in tms were non-zeros.  The typedef for unsigned
long was removed.  The timestamp value is now stored in its native time_t typedef var. Tested
the return value for a non-zero value. Test was successful.

September 9, 2014

-Entire program has been implemented and tested.  Everything works great except that
the call to times(struct tms *buf) fails to put legit values into the struct.  I have
gone as far as to copy and paste an online "working" example in a separate .c file and try
running it on both my virtual machined linux mint and on a stand-alone mac.  Both attempts failed!
