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
t : change animation delay
s : change ship speed
w : warp ship to one of the planets
f : fire warbird smart missle
r : if game is over, this will restart the game.
0 : change between project requirement settings and my settings

Light Effects:

a : Enable/Disable Ambient Lights
p : Enable/Disable Point Light
h : Enable/Disable Warbird Head Light
z : Enable/Disable Crazy Lights

FREE CAMERA MODE:

i : move forward
k : move backwards
j : turn left
l : turn right
u : pitch down
o : pitch up
m : roll left
. : roll right


SHIP MOVEMENT:

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

Meets requirements for Phase 3.


###################################################################################

USAGE:

comp465Lab1.cpp will require the JaredFowler_465Lib and includes465 libraries to run.
It will also require the appropriate .tri model files.  (They can easily be replaced
by any other .tri file.)


###################################################################################

UPDATE LOG:  TIME TOTAL: ~100 HOURS

November 19-21, 2014

-Began and ended the large journey of understanding textures and the associated headaches that come
along with it. Images turned into raw format. Six planes were created which surround the models in a box 
formation. Separate shaders were created which handle the textures. The display function switches between 
this shader group and the original shader group. Hooray for me! (~15 hours)

-Upon getting home I loaded the project on my desktop and it would not work. This was a result from
going from an NVIDIA card to an ATI.  ATI has stricter rules when it comes to procedures. A simple re-arrangement
of some code was in order. (Putting the VAO binding before any code or buffer that is associated with it.) Furthermore,
you have to manually bind the texture buffer before entering shader land. New images were put as backgrounds. I learned 
that there is a certain minimum width that the image needs to be. (1024 pixels wide.) (~1 hour)

November 18, 2014

-Major refactoring of the existing vertex and fragment shaders. Vertex shader is complete. Fragment
shader has a working point light, and 90% working spot light. I should be able to finish it later 
today. Some of the major refactoring includes better efficiency, better organization, and more readable
variable names. Mods were also made to light intensity, in particular to specular light. (~2.5 hours)

-Spot light implimentation has been completed. It adheres to the project requirement in that when the warbird
is over a planet after warping, the light effects are visible. I added the capability of enabling and disabling these light
sources. An extra option has been created called "crazy lights". This is somewhat useful to see more fully the effects of 
the ambient, point, and spot lights. (~1 hour)

November 12, 2014

-Each object is now aware of its own reaction to light. (i.e., they all have a sense of ambient, diffuse, and specular) These
values are then loaded into a uniform variable, from whence the fragment shader takes charge. Specular lighting 
has been added to the fragment shader. (~2 hours)

November 11, 2014

-Made head way into the shader language. A point light was implimented which takes into account
diffuse and ambient lights. The point light's position originates from the position of Ruber. The *.glsl
files which I created are undoubtainly low quality, and are not completely modular, but they seem to do
the job well. Ideas for how to form them came from the phong model. Overall, not too much implimentation
was done, but it was a worth-while learning experience. (~10 hours)

November 4, 2014

-Added the option via the '0' char to change the settings from project requirement settings to my
preferred settings. This includes setting the glutidlefunction to NULL and increasing the speed of
missles. (30 minututes)

October 31, 2014 - November 1, 2014

-Verified FPS and UPS with Professor Barnes. Indeed the graphics card is doing extra "unneccesary" 
work by having the glut idle function call my display funciton. The purpose is to show the power
intensity of the graphics GPU's and computers CPU. (1 hour)

-Successfuly implimented a fully functional free camera mode, including the ability of movement, yaw,
pitch, and roll. The major confusement, which has now been overcome, was that the camera technically does
not move!  It is only the ojbects that move which gives the viewer the illusion that the camera is moving.
Errors at first occurred as I tried to rotate on an axis based on the current camera matrix rotation. This
was of course fixed by changing the axis of rotation to the x,y,z axises. Very simple to impliment after all!
(5 hours)

-Updated some of the window messages. Updated missle speed according to project requirements. (Note: Slowest
 planet speed is 200 u/s while the missle speed is only 125 u/s) (30 min.)

October 26, 2014

-Did some experimenting with glutPostRedisplay and the use of glutIdleFunction. I discovered that
by ignoring the use of the idle function to take care of re-drawing the scene, I was able to reduce
CPU usage from 17% all the way down to 1% !  (There was also a noticeable sound difference from the
graphics card fan.) This brings up the issue of the manner in which we are calculating FPS.  I intend
on asking about this come next lab. It seems that technically FPS is really being represented by 
UPS. (~30 min)

October 25, 2014

-Removed some uneccessary code in the display function. Apparently glutEnableArrayAttribute only
needs to be enabled once. Note that by default user defined shader variables are disabled until 
this function call is made.

-Modified all classes to call getModelMatrix upon initialization. This eliminated a check in the updates function.

-Modified the display function to calculate the viewMatrix before entering the model loop. This elminates NUM_OBJECTs - 1 
calls to getViewMatrix.

(~3 hours)

October 17, 2014

-A max of 3 missles can now exist. 1 from each missle site and 1 from the warbird. A minor 
bug was fixed which occurred on the first update. Because all objects would start at the origin, 
initial missles were fired from the sites. This has been fixed by setting a flag in the update
funciton. Added collision testing between missles. Tested and works great! Updated class
diagrams. (~2 hours)

October 14, 2014

-Missle will blow up space ship if distance is < 20. (Was changed from 10)  "Ludikrus" mode
was added which sets the time delay down to 1. (~20 min)

October 14, 2014

-Gravity has been fixed and works great.  I also re-ordered the collision check so that a missle
checks for collision with a missle site before that of a planet.  This makes it much easier to 
actually hit a missle site. (~1 hour)

-Fixed minor bugs such as space ship firing missles after it was destroyed.  Changed the time quantum
key to 't'.  End dialogs are now integrated.  Once an end dialog displays the animation updates will 
stop. (~1 hour)

October 13, 2014

-Collision detection seems to be working great now. I included code that allows the missle site 
to fire at the space ship. Testing took place in the which a few bugs were worked out.  I did have
a self debate about how missles should react to becomming in range to an object and turning its
smart capabilities on.  I decided to impliment this in the .c file, not the class file.  I figure that
this behavior might be unique and I desire to keep my classes as modular as possible. (~3 hours)

October 12, 2014

-Started modifying the code for both gravatational and colission detection. Currently, both of 
these checks are being done in the update section of the .c file.  Considering that there 
are only a few things that can collide, and gravity will only take effect on the ship towards ruber,
I'll probably keep it the way it is. (~2 hours)

October 11, 2014

-Modifications were made to the SmartMissle update function, in particular an error fix which was
making the missle a little loopy.  The problem originated in the rotation part of the update function.
The missle would attempt to rotate at a very large angle.  The problem was fixed via help from a math
source online. A link can bee seen in the update function itself.  I took away celestialBodySolid3D's 
missle capacity, now that it is fully integrated into SpaceShip and MissleSite.  I also maganged to fix
the missle site orientation.  The fix was accomplished via a minor fix in the update function, along with 
a correct initial rotaion in the object construction. (~4 hours)

October 10, 2014

-Finished writing up the update function for the SmartMissle.  Attempted to make the modifications in
the .c file, also enabling the keyboard even to fire a missle.  There are clearly some issues that need
to be worked out.  Missle will currently spawn next to the ship, but will not move, nor pursue a target.
(~1 hour)

October 8, 2014

-Managed to incorporate the TQ for the space ship by placing a simple flag over the glut special key 
event function. (Currently only used for spaceship updates)  The flag is reset upon an update, therefore
limiting the movement of the spaceship to the TQ. (~20 minutes)

-Set up a peliminary class for SmartMissle.  This class is a child class of Object3D.  Basic get and set
functions have been written.  The update and getModelMatrix still need to be implimented. (~30 minutes)

October 7, 2014

-Created a MissleSite class along with a missle site 3D model.  MissleSite is a child of Object3D.  Missle
sites were created successfuly and placed on the two moons.  I'm not sure if this will be their permanent 
locaiton but changing their location should be only a matter of changing up to 4 values.  I added a virtual 
function to Object3D called update.  SpaceShip and MissleSite were updated to include this function.  The 
functionality of CelestialBodySolid3D was also updated.  The display method in the .c file will now call the 
function getLastModelMatrix, which returns a copy of the last computer model matrix.  This saves the effort of 
having to re-calculate it every time we display a frame.  FPS has jumped from ~1580 to anywhere from ~1800 - ~2500 !!!
Next step in the project will be to set the appropriate Time Quantum functionality with ship movement. 
(~4 hours)

October 3, 2014

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
a plaent by 0 degrees, it might rotate it by 720 degrees. (~4 hours)

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
  these multiplications are applied to the object is the opposite way. This point comes in
  handy esepecially when taking care of space ship yaw, pitch, and roll.  