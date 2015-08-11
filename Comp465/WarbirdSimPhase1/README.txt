    ___                     ________       ________      ________  _________   
   |\  \                   |\   ____\     |\   __  \    |\  _____\|\___   ___\ 
   \ \  \    ____________  \ \  \___|_    \ \  \|\  \   \ \  \__/ \|___ \  \_| 
 __ \ \  \  |\____________\ \ \_____  \    \ \  \\\  \   \ \   __\     \ \  \  
|\  \\_\  \ \|____________|  \|____|\  \    \ \  \\\  \   \ \  \_|      \ \  \ 
\ \________\                   ____\_\  \    \ \_______\   \ \__\        \ \__\
 \|________|                  |\_________\    \|_______|    \|__|         \|__|
                              \|_________|                                     
                                                                               
                                                                               

       ██████╗ ██████╗ ███╗   ███╗██████╗     ██╗  ██╗ ██████╗ ███████╗
      ██╔════╝██╔═══██╗████╗ ████║██╔══██╗    ██║  ██║██╔════╝ ██╔════╝
      ██║     ██║   ██║██╔████╔██║██████╔╝    ███████║███████╗ ███████╗
      ██║     ██║   ██║██║╚██╔╝██║██╔═══╝     ╚════██║██╔═══██╗╚════██║
       ██████╗╚██████╔╝██║ ╚═╝ ██║██║              ██║╚██████╔╝███████║
       ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝              ╚═╝ ╚═════╝ ╚══════╝


			Created By: Jared Fowler
			   September 10, 2014
		    CSUN Comp 465L ; Prof. Barnes


###################################################################################

_ _ _ ____ ____ ___  _ ____ ___     ____ _ _  _ _  _ ____ ___ _ ____ _  _ 
| | | |__| |__/ |__] | |__/ |  \    [__  | |\/| |  | |__|  |  | |  | |\ | 
|_|_| |  | |  \ |__] | |  \ |__/    ___] | |  | |__| |  |  |  | |__| | \| 

###################################################################################

ABOUT:

The context is composed of a simple solar system with a single spaceship.  In the project's 
current phase, we are able to look at the various models that exist and utilize a free cam 
to explore.


Keys to try:

q : quit
v : change camera
b : change animation delay
s : change ship speed
w : warp ship to one of the planets

If we are in free cam mode:
i : move forward
k : move backwards
j : turn left
l : turn right

'up'   : move ship forward
'down' : move ship backwards
'left' : turn ship left
'right': turn ship right
'ctrl up'   : pitch up
'ctrl down' : pitch down
'ctrl right': roll right
'ctrl left' : roll left

###################################################################################

STATUS:

Meets requirements for Phase 1.
Meets some requirements for Phase 2.


###################################################################################

USAGE:

comp465Lab1.cpp will require the JaredFowler_465Lib and includes465 libraries to run.
It will also require the appropriate .tri model files.  (They can easily be replaced
by any other .tri file.)


###################################################################################

UPDATE LOG:

October3, 2014

-Included warp capabilities to the space ship.  User can also change ship speed by pressing the s key. This should
conclude the warbird funcitonality (besides fireing missles) needed for phase 2. (~3 hours)

-Major bug was fixed in the MathHelperJWF functionality.  Turns out I was missing a step
in my mathematical procedure. I added commentary about how the equation is done at the 
top of the file. Along with this fix resulted the camera following the space ship perfectly! EUREKA!
(~3 hours)   

October 2, 2014

-I once again changed my strategy for how the yaw, pitch, and roll operate.  They seem to work great now
and the process makes good logical sense.  (See SpaceShip.hpp for details.) I still have issues with the 
following camera and plan to fix that next. (~6 hours)

October 1, 2014

-Attempts on making the space ship's yaw, pitch, and roll funciton correctly.  I changed my strategy 
in these functions implimentation, however, there still seems to be some errors.  At any given position the
ship seems to fail in at least one of these rotations.  The following camera also needs to be adjusted. One
good thing is an error fix within MathHelperJWF.  (~4 hours)

September 30, 2014

-This report includeds a combination of changes that have taken place over the last week.  Preparation 
has been taken to incorperate phase 2 into the program.  There is a new class called SpaceShip which is
a child of Object3D. I managed to impliment its ability to move forward and backwards.  An error was found
and corrected from phase 1.  This error deals with starting positions of models. The error was fixed by 
having the initial model translation relative to its parent object, not the true origin.  (~8 hours)

September 19, 2014

-Changed the model class to save its last modelMatrix.  This eliminates the need to recurse through
x number of parents.  I also made a fix to the update funciton in which once 2PI is passed in rotation,
I reset it. This prevents the rotation matrix from becomming too big. (Rotate 0 vs rotate 8PI for example)
Models have also been updated and scaled properly. (~6 hours)

September 13, 2014

-Incorporated Frames Per Second and Updates Per Second counts.  Divided the classes into different
files, but kept implimentation within the respective .hpp files.  The project is currently built
to the specifications of Lab1.  There are some added features such as different update delay times, 
free cam, etc.  There were some personal conflict about making a new class for different types of 
3D models... Ultimately I ended up changing the name of my main model object from Solid3D to CelestialSolid3D. 
This same object now holds variables to keep track of missle count and gravity. One possible point of interest
that might need to be cleaned in the future is the update function of CelestialSolid3D.  It modifies the rotation matrix
and I'm afraid it might be continually increasing the angle of rotation. This means, for example, that instead of rotating
a plaent by 0 degrees, it might rotate it by 720 degrees.

September 13, 2014

-Horray!  After many hours of trial and learning, cameras are set and model objects are made.
The classes I've made have turned out to be very effective, making the need for less code in
the .cpp file.  One of the best accomplishments was the creation of the function getModelMatrix
which uses a recursive call to get position of parent models (all the way to the root) in order
to get a new origin base for the child model. Currently the planets orbit the sun, and the moons 
orbit the planets.  The warbird is stationary in space.  Camera views are great, along with the 
dynamic cameras that follow specified objects.  Greatest part about the work done was the 
learning experience.  I feel much more confident about what I'm doing, and more knowledgeable
about how openGL works.  (~10 hours)

September 10, 2014

-Did some testing with different objects provided by Prof. Barnes.  I ultimately was
able to make the star and 2 planets.  I was able to make a simple noclip mode and could
transition the camera viewpoint. (~4 hours)

###################################################################################

POINTS OF INTEREST:

- Linear Algebra:  The order of matrix multiplication is one way, while the order in which 
  these multiplications are applied to the object is the opposite way.