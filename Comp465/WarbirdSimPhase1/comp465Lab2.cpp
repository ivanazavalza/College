/* Created By: Jared Fowler , jared.fowler.379@my.csun.edu
   September 10, 2014
   Comp 465 "Warbird Simulation"
   Professor Barnes


    About:

    Warbird Simulator is a Comp 465 project in Prof. Barnes' class @ CSUN.  The basic
    idea is to create a small solar system, Ruber being the central red dwarf star.  There
    will exist planets and moons which will orbit depending on the radians of rotation provided
    by the user.  The warbird and planets will have missles.  The warbird will be controled via
    user.

    The general context of this file is variables needed for different openGL operations, followed
    by initializtion for object models.  The user should initialize both the models and cameras.  \
    There is also the choice to init. animation speeds.  

    After initializing stuff in main, we start the glut loop.  This will serve as the main loop
    updating and drawing our models.  The functionality of these updates depend on the functions
    that we pass into the glut fucntions. (Passed in via function pointer) This should also be 
    done in initializtion, in this case it is done in main.


*/

//##############################################################################################
// Includes, Typedefs, and Defines #############################################################
//##############################################################################################

# define __Windows__
# include "./includes465/include465.hpp"
# define __INCLUDES465__ 

#include "./JaredFowler_465Lib/JWF_465_Lib.hpp"  //General Library of my helper objects


//##############################################################################################
// Function Prototypes #########################################################################
//##############################################################################################

void initObjects(void);
void initCameras(void);
void initAnimationSpeeds(void);
void init(void);
void reshape(INT32 width, INT32 height);
void display(void);
void update(void);
void intervalTimer(INT32 i);
void keyboard(BYTE key, INT32 x, INT32 y);
void specialKeyEvent(INT32 key, INT32 x, INT32 y);
void printInfo(void);
void cleanUp(void);

//##############################################################################################
// Constants and Global Variables ##############################################################
//##############################################################################################
const BYTE NUM_MODELS  = 7;     //Number of Models (all models)
const BYTE NUM_CEL_MOD = 6;     //Number of CelestialBodySolid3D models
const BYTE NUM_SS_MOD  = 1;     //Number of SpaceShip models
const BYTE NUM_MIS_MOD = 1;     //Number of missle modles
const BYTE NUM_CAMERAS = 6;     //Total number of available camera views
const BYTE NUM_SPEEDS  = 4;     //Number of different animation speeds 

enum Model_ID{ruber_mod = 0, unum_mod, duo_mod, primus_mod, secundus_mod, missle_mod};

enum Camera_ID{front_cam = 0, top_cam, warbird_cam, unum_cam, duo_cam, free_cam};

//Celestial Model Objects
CelestialBodySolid3D* celestialSolid[NUM_CEL_MOD];

//SpaceShip Model Objects
SpaceShip* spaceShipSolid;

//Missle Model Objects

//Missle Station Model Objects

//Object3D pointers to point at each model object
Object3D* modelPointer[NUM_MODELS];

//Cameras and Camera Handlers
CameraView* cameras[NUM_CAMERAS]; //Objects that will keep track and update views
BYTE currentCamera    = 0;        //Keep track of which view we are currently using
float FREE_CAM_TRAVEL = 100;      //In free cam mode, how far do we travel per key check?
float FREE_CAM_TURN   = PI;       //In free cam mode, how many radians should we rotate?

//Warbird warp location, speed factors, etc.
BYTE warpLocation             = 0;    //Warbird can warp between Unum (0) and Secundus (1)
const BYTE NUM_SPEEDS_WARBIRD = 3;    //Number of different speeds the warbird can go
BYTE currentWarbirdSpeed = 0;         //Which element in the speed array are we looking at?
const float warbirdSpeed[NUM_SPEEDS_WARBIRD] = { 10, 50, 200 };
const float warbirdTurnSpeed  = 0.2f; //How fast can the warbird turn?

//Animation Speed Objects
AnimationSpeed * animationSpeeds[NUM_SPEEDS];
BYTE currentSpeed = 0;

//Files used for reading models, etc.
const char* VERTEX_SHADER_FILE   = "simpleVertex.glsl";
const char* FRAGMENT_SHADER_FILE = "simpleFragment.glsl";

GLuint shaderProgram;
GLuint VAO[NUM_MODELS];     //Vertex Array Objects
GLuint buffer[NUM_MODELS];  //Vertex Buffer Objects

//Shader handles, matrices, etc
GLuint MVP;  //Model View Projection matrix's handle
GLuint vPosition[NUM_MODELS];
GLuint vColor[NUM_MODELS];
GLuint vNormal[NUM_MODELS];

//Matrices used throughout the functions below
glm::mat4 projectionMatrix;          //set in reshape()
glm::mat4 ModelViewProjectionMatrix; //set in display();

//Variables for writing to the windows title bar
char info[100]      = { 0 };
UINT32 updateRate   = 0;
UINT32 updateCount  = 0;
UINT32 frameRate    = 0;
UINT32 frameCount   = 0;
UINT64 currentTime  = 0;
UINT64 lastTime     = 0;
UINT64 timeInterval = 0;
UINT64 currentTime2 = 0;
UINT64 lastTime2    = 0;
UINT64 timeInterval2= 0;

//##############################################################################################
// Functions ###################################################################################
//##############################################################################################

void initObjects(void){
    /*This funciton's purpose is to initialize the different objects that will exist in
    the openGL window.  This function is variable in that I expect the programmer to modify
    this function with their specific models for each modification of scene.*/

    /*CelestialBodySolid3D(char* objName, char* fileName, UINT32 vertexCount, float modelSize, bool active = true, 
                         CelestialBodySolid3D* cb3D = NULL, float radiansRot = 0, float radiansSpin = 0, bool orbital = false, bool spinning = false,
                         glm::mat4 spinningMatrix = glm::mat4(),
                         glm::vec3 spinAxis = glm::vec3(0,1,0),
                         glm::vec3 rotationAxis = glm::vec3(0, 1, 0),
                         glm::mat4 scaleMatrix = glm::mat4(),
                         glm::mat4 translationMatrix = glm::mat4(),
                         glm::mat4 rotationalMatrix = glm::mat4())*/

    //Red-Dwarf Star RUBER
    celestialSolid[ruber_mod] = new CelestialBodySolid3D("Ruber", "ruber.tri", (6336 * TRI), 2000, true, NULL, 0, .001f, false, true,
                                                         glm::mat4(), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::mat4(), glm::translate(glm::mat4(), glm::vec3(0, 0, 0)), glm::mat4());

    //Planet UNUM
    celestialSolid[unum_mod] = new CelestialBodySolid3D("Unum", "unum.tri", (1104 * TRI), 200, true, celestialSolid[ruber_mod], .004f, 0, true, false,
                                                        glm::mat4(), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::mat4(), glm::translate(glm::mat4(), glm::vec3(4000, 0, 0)), glm::mat4());

    celestialSolid[unum_mod]->setMissleCount(5);

    //Planet DUO
    celestialSolid[duo_mod] = new CelestialBodySolid3D("Duo", "duo.tri", (1104 * TRI), 400, true, celestialSolid[ruber_mod], .002f, 0, true, false,
                                                       glm::mat4(), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::mat4(), glm::translate(glm::mat4(), glm::vec3(-9000, 0, 0)), glm::mat4());

    //Moon Primus
    celestialSolid[primus_mod] = new CelestialBodySolid3D("Primus", "primus.tri", (1104 * TRI), 100, true, celestialSolid[duo_mod], .004f, 0, true, false,
                                                          glm::mat4(), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::mat4(), glm::translate(glm::mat4(), glm::vec3(900, 0, 0)), glm::mat4());

    //Moon Secundus
    celestialSolid[secundus_mod] = new CelestialBodySolid3D("Secundus", "secundus.tri", (1104 * TRI), 150, true, celestialSolid[duo_mod], .002f, 0, true, false,
                                                            glm::mat4(), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::mat4(), glm::translate(glm::mat4(), glm::vec3(1650, 0, 0)), glm::mat4());

    celestialSolid[secundus_mod]->setMissleCount(5);


    //Missle
    celestialSolid[missle_mod] = new CelestialBodySolid3D("Missle", "missle.tri", (4120 * TRI), 25, true, NULL, 0.0f, 0, false, false,
                                                          glm::mat4(), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::mat4(), glm::translate(glm::mat4(), glm::vec3(4900, 1000, 4900)), glm::mat4());




    /*Constructor:
    SpaceShip(char* objName, char* fileName, UINT32 vertexCount, float modelSize,
              bool active = true, UINT32 missleCount = 0,
              glm::mat4 scaleMatrix = glm::mat4(), glm::mat4 translationMatrix = glm::mat4())
    */


    //Warbird
    spaceShipSolid = new SpaceShip("Warbid", "warbird_v1.tri", (1816 * TRI), 100, true, 9,
                                   glm::mat4(), glm::translate(glm::mat4(), glm::vec3(5000, 1000, 5000)));




    /*Have the modelPointer point to each object thats been created. (Take advantage of polymorphism)*/
    modelPointer[0] = celestialSolid[ruber_mod];
    modelPointer[1] = celestialSolid[unum_mod];
    modelPointer[2] = celestialSolid[duo_mod];
    modelPointer[3] = celestialSolid[primus_mod];
    modelPointer[4] = celestialSolid[secundus_mod];
    modelPointer[5] = celestialSolid[missle_mod];
    modelPointer[6] = spaceShipSolid;

}

//##############################################################################################
void initCameras(void){
    /*This funciton's purpose is to initialize the different Cameras that will be available for
    the user.  Cameras can be initialized to static locations or to follow a specific object 
    based upon the parameters passed into the constructor.*/

    /*CameraView(char* viewId, glm::vec3 eye, glm::vec3 at, glm::vec3 up, BYTE viewFlag, Object3D* objToFollow = NULL)*/

    //Static Front View
    cameras[front_cam] = new CameraView("FRONT", glm::vec3(0, 10000, 20000), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 0, NULL);

    //Static Top View
    cameras[top_cam] = new CameraView("TOP", glm::vec3(0, 20000, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), CameraView::Top_View, NULL);

    //Dynamic Ship View
    cameras[warbird_cam] = new CameraView("WARBIRD", glm::vec3(0, 300, -1000), glm::vec3(0, 300, 0), glm::vec3(0, 1, 0), CameraView::Back_Follow, spaceShipSolid);

    //Dynamic Unum View
    cameras[unum_cam] = new CameraView("UNUM", glm::vec3(0, 4000, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), CameraView::Top_View, celestialSolid[unum_mod]);

    //Dynamic Duo View
    cameras[duo_cam] = new CameraView("DUO", glm::vec3(0, 4000, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), CameraView::Top_View, celestialSolid[duo_mod]);

    //Free Look Camera
    cameras[free_cam] = new CameraView("FreeLook", glm::vec3(5000, 1200, 6000), glm::vec3(5000, 1000, 5000), glm::vec3(0, 1, 0), CameraView::Free_View, NULL);
}

//##############################################################################################
void initAnimationSpeeds(void){
    /*Initialize different animation speeds.  The animation class is very basic... serves more as
    a simple struct but takes advantage of object programming.*/

    animationSpeeds[0] = new AnimationSpeed(40, "ace");
    animationSpeeds[1] = new AnimationSpeed(100, "pilot");
    animationSpeeds[2] = new AnimationSpeed(250, "trainee");
    animationSpeeds[3] = new AnimationSpeed(1, "debug");

}

//##############################################################################################
//Load the shader programs, vertex data from model files, create the solids, set initial view
void init(void) {

    //Local Variables
    float modelBoundingRadius[NUM_MODELS];  //Model's bounding radius
    glm::vec3 scale[NUM_MODELS];  //Temporarily holds scales of models

    // load the shader programs
    shaderProgram = loadShaders(VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE);
    glUseProgram(shaderProgram);


    // generate VAOs and VBOs
    glGenVertexArrays(NUM_MODELS, VAO);
    glGenBuffers(NUM_MODELS, buffer);


    /*Note: The for loop below is what binds our models to be used with the GL library.
    The function loadModelBuffer is provided by Prof. Barnes and is located in the iclude465.hpp .
    This call needs to be done after the generation of VAOs and VBOs*/

    /*Load Buffer and get the models scale values*/
    for (int i = 0; i < NUM_MODELS; i++) {
        //Load the buffers from the model files
        modelBoundingRadius[i] = loadModelBuffer(modelPointer[i]->getFileName(),
                                                 modelPointer[i]->getVertexCount(),
                                                 VAO[i], buffer[i], shaderProgram,
                                                 vPosition[i], vColor[i], vNormal[i],
                                                 "vPosition", "vColor", "vNormal");

        //Set scale for models given bounding radius
        /*Notes: AC3D's sizeTo function sets the diameters, not the radius.  The function
        above, provided by the professor, returns a value equal to the models radius
        according to as it is drawn.*/
        scale[i] = glm::vec3(modelPointer[i]->getModelSize() * 1.0f / modelBoundingRadius[i]);

        //Now that we have the scale, update the scale matrix in each of the objects
        modelPointer[i]->setScaleMatrix(glm::scale(glm::mat4(), glm::vec3(scale[i])));
    }


    MVP = glGetUniformLocation(shaderProgram, "ModelViewProjection");


    // set render state values
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f);

    //Get initial time reading for statistical reporting
    lastTime = glutGet(GLUT_ELAPSED_TIME);
}


//##############################################################################################
void reshape(INT32 width, INT32 height){

    float aspectRatio = (float)width / (float)height;
    float FOVY = glm::radians(60.0f);
    glViewport(0, 0, width, height);
    printf("Reshape: FOVY = %5.2f, width = %4d height = %4d aspect = %5.2f \n", FOVY, width, height, aspectRatio);
    projectionMatrix = glm::perspective(FOVY, aspectRatio, 1.0f, 100000.0f);

}

//##############################################################################################
void display(void){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Load Sun, Planets, Moons ------------------------------------------------------------------
    for (int m = 0; m < NUM_MODELS; m++) {

        //Get the model view projection matrix
        ModelViewProjectionMatrix = projectionMatrix * cameras[currentCamera]->getLookAt() * modelPointer[m]->getModelMatrix();

        //Get handle, bind, and draw!
        glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
        glBindVertexArray(VAO[m]);
        glEnableVertexAttribArray(vPosition[m]);
        glEnableVertexAttribArray(vColor[m]);
        glEnableVertexAttribArray(vNormal[m]);
        glDrawArrays(GL_TRIANGLES, 0, modelPointer[m]->getVertexCount());
    }


    //Change buffers to update the display
    glutSwapBuffers();

    //Update the frame count and estimate fps information
    frameCount++;
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    timeInterval = currentTime - lastTime;
    //Determine if 1 second has passed. If so, update frameRate
    if (timeInterval >= 1000) {
        frameRate = (UINT32)(frameCount / (timeInterval / 1000.0f));
        lastTime = currentTime;
        frameCount = 0;
        //Update title bar
        printInfo();
    }

}

//##############################################################################################
void update(void){

    /*Update Sun, Planets, Moons.*/
    for (int i = 0; i < NUM_CEL_MOD; i++){
        celestialSolid[i]->update(false);
    }

    glutPostRedisplay();

    //Update the update count and estimate ups information
    updateCount++;
    currentTime2 = glutGet(GLUT_ELAPSED_TIME);
    timeInterval2 = currentTime2 - lastTime2;
    //Determine if 1 second has passed. If so, update frameRate
    if (timeInterval2 >= 1000) {
        updateRate = (UINT32)(updateCount / (timeInterval2 / 1000.0f));
        lastTime2 = currentTime2;
        updateCount = 0;
        //Update title bar
        printInfo();
    }

}

//##############################################################################################
void intervalTimer(INT32 i) {
    /*This function is in charge of calling the update function. The update function in change
    will then call the update functions of all the individual models.  This function prepares
    to call itself in the future and then updates.*/

    glutTimerFunc(animationSpeeds[currentSpeed]->getSpeed(), intervalTimer, 1);
    update();
}

//##############################################################################################
void keyboard(BYTE key, INT32 x, INT32 y) {

    switch (key) {

    case 033: case 'q': case 'Q':  //Quit
        cleanUp();
        exit(EXIT_SUCCESS);
        break;

    case 'v': case 'V':  //Change To Next Camera View
        currentCamera++;
        if (currentCamera >= NUM_CAMERAS)
            currentCamera = 0;
        break;
    
    case 'b': case 'B':  //Change animation speed
        currentSpeed++;
        if (currentSpeed >= NUM_SPEEDS)
            currentSpeed = 0;
        break;

    case 'w': case 'W': //Warp the ship to a desired location
        if (warpLocation % 2 == 0){ //Warp to Unum

            //Get a copy of Unum's model matrix
            glm::mat4 unumM =  modelPointer[unum_mod]->getModelMatrix();

            //We translate out the unum's position, and then translate the offset.
            unumM = glm::translate(glm::mat4(), MathHelperJWF::getVector(unumM, 3));
            unumM = unumM * glm::translate(glm::mat4(), glm::vec3(0, 4000, 0));
            //Update space ships translation and rotation matrices
            spaceShipSolid->warp(unumM, glm::rotate(glm::mat4(), PI / 2, glm::vec3(1, 0, 0)));

        }
        else{ //Warp to Duo
            
            //Get a copy of Duo's model matrix
            glm::mat4 duo = modelPointer[duo_mod]->getModelMatrix();

            //We translate out the unum's position, and then translate the offset.
            duo = glm::translate(glm::mat4(), MathHelperJWF::getVector(duo, 3));
            duo = duo * glm::translate(glm::mat4(), glm::vec3(0, 4000, 0));
            //Update space ships translation and rotation matrices
            spaceShipSolid->warp(duo, glm::rotate(glm::mat4(), PI / 2, glm::vec3(1, 0, 0)));

        }
        //Update warp location
        warpLocation++;
        break;

    case 's': case 'S':  //Change warbird speed
        currentWarbirdSpeed++;
        if (currentWarbirdSpeed > NUM_SPEEDS_WARBIRD)
            currentWarbirdSpeed = 0;
        break;

    }

    /*In the below statement, various keys will update
    the camera position if the mode is set to free camera*/
    if ((Camera_ID)currentCamera == free_cam){

        glm::vec3 travel;

        switch (key){

        case 'i': case 'I':
            /*"Forward" would be moving towards what we are looking at.  We can determine 
            the vector between current eye and at, and then based on a distance we want to
            travel, find an appropriate vector to add to both eye and at*/

            //Get directional-distance vector and auto-update values
            CameraView::moveForwardOrBackwards(cameras[free_cam], FREE_CAM_TRAVEL, true);
            break;

        case 'k': case 'K':
            /*Same as forward, but final values are subtracted instead of added*/

            //Get directional-distance vector and auto-update values
            CameraView::moveForwardOrBackwards(cameras[free_cam], -FREE_CAM_TRAVEL, true);
            break;

        case 'j': case 'J':
            /*Turn left.*/
            CameraView::turnRightOrLeft(cameras[free_cam], FREE_CAM_TURN, true);
            break;

        case 'l': case 'L':
            /*Turn right.*/
            CameraView::turnRightOrLeft(cameras[free_cam], -FREE_CAM_TURN, true);
            break;
        }
    }
}

//##############################################################################################
void specialKeyEvent(INT32 key, INT32 x, INT32 y){

    /*Space Ship movement forwards/backwards*/
    if ((key == GLUT_KEY_UP) && (glutGetModifiers() != GLUT_ACTIVE_CTRL)){ //Move forward
        spaceShipSolid->moveAdjust(warbirdSpeed[currentWarbirdSpeed]);
    }
    else if ((key == GLUT_KEY_DOWN) && (glutGetModifiers() != GLUT_ACTIVE_CTRL)){ //Move backwards
        spaceShipSolid->moveAdjust(-warbirdSpeed[currentWarbirdSpeed]);
    }
    
    /*Space Ship yaw adjusts.  Note: Remember right hand rule makes these opposite.*/
    if ((key == GLUT_KEY_RIGHT) && (glutGetModifiers() != GLUT_ACTIVE_CTRL)){ //Yaw right
        spaceShipSolid->yawAdjust(-warbirdTurnSpeed);
    }
    else if ((key == GLUT_KEY_LEFT) && (glutGetModifiers() != GLUT_ACTIVE_CTRL)){ //Yaw left
        spaceShipSolid->yawAdjust(warbirdTurnSpeed);
    }

    /*Space Ship pitch adjusts.*/
    if ((key == GLUT_KEY_UP) && (glutGetModifiers() == GLUT_ACTIVE_CTRL)){ //Pitch up
        spaceShipSolid->pitchAdjust(-warbirdTurnSpeed);
    }
    else if ((key == GLUT_KEY_DOWN) && (glutGetModifiers() == GLUT_ACTIVE_CTRL)){ //Pitch down
        spaceShipSolid->pitchAdjust(warbirdTurnSpeed);
    }

    /*Space Ship roll adjusts.  Note: Remember right hand rule makes these opposite.*/
    if ((key == GLUT_KEY_RIGHT) && (glutGetModifiers() == GLUT_ACTIVE_CTRL)){ //Roll right
        spaceShipSolid->rollAdjust(warbirdTurnSpeed);
    }
    else if ((key == GLUT_KEY_LEFT) && (glutGetModifiers() == GLUT_ACTIVE_CTRL)){ //Roll left
        spaceShipSolid->rollAdjust(-warbirdTurnSpeed);
    }

}

//##############################################################################################
/*Prints general information to the windows title bar.*/
void printInfo(void){

    sprintf(info, "Warbird: %d  Unum: %d  Secundus:  %d  U/S %d  F/S %d  View %s  Setting: %s",
        spaceShipSolid->getMissleCount(), celestialSolid[unum_mod]->getMissleCount(), celestialSolid[secundus_mod]->getMissleCount(), 
        updateRate, frameRate, cameras[currentCamera]->getViewId(), animationSpeeds[currentSpeed]->getID());

    glutSetWindowTitle(info);

}

//##############################################################################################
void cleanUp(void){

    //Clean-up solids
    for (int i = 0; i < NUM_MODELS; i++){
        delete modelPointer[i];
        modelPointer[i] = NULL;
    }

    //Clean-up cameras
    for (int i = 0; i < NUM_CAMERAS; i++){
        delete cameras[i];
        cameras[i] = NULL;
    }

    //Clean-up animation speeds
    for (int i = 0; i < NUM_SPEEDS; i++){
        delete animationSpeeds[i];
        animationSpeeds[i] = NULL;
    }

}

//##############################################################################################
int main(int argc, char* argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("Warbird Simulation");

    // initialize and verify glew
    glewExperimental = GL_TRUE;  // needed my home system 
    GLenum err = glewInit();
    if (GLEW_OK != err)
        printf("GLEW Error: %s \n", glewGetErrorString(err));
    else {
        printf("Using GLEW %s \n", glewGetString(GLEW_VERSION));
        printf("OpenGL %s, GLSL %s\n",
            glGetString(GL_VERSION),
            glGetString(GL_SHADING_LANGUAGE_VERSION));
    }


    // initialize objects
    initObjects();

    // initialize cameras
    initCameras();

    // initialize scene
    init();

    // initialize animation speeds
    initAnimationSpeeds();
    
    // set glut callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyEvent);
    glutIdleFunc(display);  // start with intervalTimer
    glutTimerFunc(animationSpeeds[currentSpeed]->getSpeed(), intervalTimer, 1);
    glutMainLoop();


    //Unallocate everything
    cleanUp();


    printf("done\n");
    return 0;
}