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
			       September 23, 2014
			CSUN Comp 322L ; Prof. Fitzgerald


###################################################################################

    __   ____  ______    __  __ __    ___  ____  
   /  ] /    T|      T  /  ]|  T  T  /  _]|    \ 
  /  / Y  o  ||      | /  / |  l  | /  [_ |  D  )
 /  /  |     |l_j  l_j/  /  |  _  |Y    _]|    / 
/   \_ |  _  |  |  | /   \_ |  |  ||   [_ |    \ 
\     ||  |  |  |  | \     ||  |  ||     T|  .  Y
 \____jl__j__j  l__j  \____jl__j__jl_____jl__j\_j
                                                                 

###################################################################################

ABOUT:

Catcher: a program that catches a number of predefined signals, and 
         prints status information on stdout.

###################################################################################

STATUS:

September 24, 2014 - Has been tested and works!

###################################################################################

USAGE:

Build using the 'make' command.  The output will be 'catcher'.  You will be required
to add some arguments, system signals in this case, to the program call.  'TERM' is
a requirement for this program to run.  Open up a separate terminal window and use
the kill command along with a signal and the PID.  Once term has been killed 3 times
the program will terminate.

EX:

Terminal 1:  $ ./catcher term USR1 sigusr2  <enter>
             $ ./catcher: $$ = <PID>


Terminal 2:  $ kill -usr1 <PID>
             $ kill -term <PID>
             $ kill -term <PID>
             $ kill -term <PID>

###################################################################################

UPDATE LOG:

September 24, 2014
-Found the bug issue discussed in the previous update entry.  I was able to eliminate
the use of a global variable by using a mailBox function.  I still debate whether or
not this is a better approach than using a single global varaible.  One the one hand
we have information hiding vs. on the other hand it's much simpler to have a global. 
Found one other small bug that would fail the program if user passed in "sig".  This 
was fixed by moving the allToUpper function within the configureSigString function.
Has been tested, and seems to work according to program specification.

September 23, 2014
-Completed most of the lab.  Challenges at this point still include being able
to signal completion to the main process without the use of a global variable,
and debug some issues with unexpected program termination when killing -USR1
first followed by killing -TERM

