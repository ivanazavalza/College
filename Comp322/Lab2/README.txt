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
			       September 16, 2014
			CSUN Comp 322L ; Prof. Fitzgerald


###################################################################################

_    ____ _  _ _  _ ____ _  _          ___ _  _ ___  ____ 
|    |__| |  | |\ | |    |__|    __     |  |  | |__] |___ 
|___ |  | |__| | \| |___ |  |           |  |__| |__] |___ 
                                                          

###################################################################################

ABOUT:

Launch: A program that allows one to set in motion another program, while also reporting
some basic information.


Tube: Command-line process that launches two child processes, sets a pipe between them
for inter-process communication.

###################################################################################

STATUS:

September 18, 2014  :  Tested and Works! Meets all required lab requirements.

###################################################################################

USAGE:

Launch:
$ launch <program>    //Note: might have to include dir (example: $ launch /bin/ls )

Tube:
$ tube <program1> , <program2>     //Note: There needs to be a comma separater

###################################################################################

UPDATE LOG:


September 18, 2014

-Changed location of some of the code.  For example, creation on individual argv structures
have been placed within the child only portion of the program after the fork.  Other minor 
fixes and code cleanup.

September 17, 2014

-Minor fixes to the code.  Further testing has been done and everything seems to work 
properly on linux mint. Has also been tested on mac os and seems to work just as well.
makefile has been created.

September 16, 2014

-Implimented all functionality according to lab requirements.  Main challenge came 
from calling the executable function.  The argv array of pointers to char strings
NEEDS to end with a null ptr.  For this reason, tube creates 2 char** which are given
new memory in the heap.  Instead of copying values over from the original argv, I simply
point to the existing values.  I end each of the new argv's with a pointer to NULL.  I did
some initial testing, and everything seems to work.  Still needs additional testing and 
white-box walkthrough.
