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
    by initializtion for object models.  The user should initialize both the models and cameras.
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
# include "./includes465/texture.hpp"

#include "./JaredFowler_465Lib/JWF_465_Lib.hpp"  //General Library of my helper objects

//##############################################################################################
// Function Prototypes #########################################################################
//##############################################################################################

void initObjects(void);
void initCameras(void);
void initAnimationSpeeds(void);
void init(void);
void scaleObjects(void);
void reshape(INT32 width, INT32 height);
void display(void);
void update(void);
void intervalTimer(INT32 i);
void keyboard(BYTE key, INT32 x, INT32 y);
void specialKeyEvent(INT32 key, INT32 x, INT32 y);
void findMissleTarget(void);
void gravityEffects(void);
void colissionCheck(void);
void printInfo(void);
void cleanUp(void);

//##############################################################################################
// Constants and Global Variables ##############################################################
//##############################################################################################
bool PROJECT_REQUIREMENTS = true;  //Project Requirements versus my settings.

const BYTE NUM_MODELS   = 11;    //Number of Models (all models)
const BYTE NUM_CEL_MOD  = 5;     //Number of CelestialBodySolid3D models
const BYTE NUM_SS_MOD   = 1;     //Number of SpaceShip models
const BYTE NUM_MIS_MOD  = 3;     //Number of missle modles
const BYTE NUM_MISS_MOD = 2;     //Number of missle sites
const BYTE NUM_CAMERAS  = 6;     //Total number of available camera views
const BYTE NUM_SPEEDS   = 5;     //Number of different animation speeds 

enum Model_ID{ruber_mod = 0, unum_mod, duo_mod, primus_mod, secundus_mod, missle_mod};

enum Camera_ID{front_cam = 0, top_cam, warbird_cam, unum_cam, duo_cam, free_cam};

enum Missle_ID{warbird_miss = 0, ms_0_miss, ms_1_miss};

//Celestial Model Objects
CelestialBodySolid3D* celestialSolid[NUM_CEL_MOD];

//SpaceShip Model Objects
SpaceShip* spaceShipSolid;

//Missle Model Objects
SmartMissle* smartMissleSolid[NUM_MIS_MOD];

//Missle Station Model Objects
MissleSite* missleSiteSolid[NUM_MISS_MOD];

//Object3D pointers to point at each model object
Object3D* modelPointer[NUM_MODELS];

//Animation Speed Objects
AnimationSpeed * animationSpeeds[NUM_SPEEDS];
BYTE currentSpeed = 0;

//Cameras and Camera Handlers
CameraView* cameras[NUM_CAMERAS]; //Objects that will keep track and update views
BYTE currentCamera    = 0;        //Keep track of which view we are currently using
float FREE_CAM_TRAVEL = 100;      //In free cam mode, how far do we travel per key check?
float FREE_CAM_TURN   = .05f;     //In free cam mode, how many radians should we rotate?

//Warbird warp location, speed factors, etc.
BYTE warpLocation             = 0;    //Warbird can warp between Unum (0) and Secundus (1)
const BYTE NUM_SPEEDS_WARBIRD = 3;    //Number of different speeds the warbird can go
BYTE currentWarbirdSpeed = 0;         //Which element in the speed array are we looking at?
const float warbirdSpeed[NUM_SPEEDS_WARBIRD] = { 10, 50, 200 };
const float warbirdTurnSpeed  = 0.2f; //How fast can the warbird turn?
bool updatedWarbird = true;           //We only want to update the postiion on every TQ update

//Missle and Missle Site Behaviors
const float MISSLE_SITE_DETECT = 5000.0f;   //At what distance can missle sites detect a target?
const float DETECTION_DISTANCE = 5000.0f;   //At what distance can missles detect their target?
const float MAX_DIST_TIL_XPLOD = 10000.0f;  //How far can missle go until it runs out of rocket power?

float MISSLE_SPEED             = 5.0f;      //How fast does the missles travel?
const float MISSLE_SPEEDS[2]   = { 5.0f, 75.0f };  //Options for missle speed

const float DIST_TIL_SMART     = 245.0f;    //How far does a missle go until its smart mode turns on?

//Other Physics
bool gravity = true;  //Does gravity exist?
const double GRAVITY_CONSTANT = 90000000.0;
glm::vec4 toggledLights(1, 1, 1, 0);

//Files used for reading models, etc.
const char* VERTEX_SHADER_FILE     = "jwf_Vertex_Shader_V1.glsl";
const char* FRAGMENT_SHADER_FILE   = "jwf_Fragment_Shader_V1.glsl";
const char* TEX_VERTEX_SHADER_FILE = "jwf_Tex_Vertex_Shader_V1.glsl";
const char* TEX_FRAG_SHADER_FILE   = "jwf_Tex_Fragment_Shader_V1.glsl";

GLuint shaderProgram, textureShader;
GLuint VAO[NUM_MODELS];  //Vertex Array Objects
GLuint VBO[NUM_MODELS];  //Vertex Buffer Objects

/*Shader handles, matrices, etc*/
//Regular Shader
GLuint MVP;  //Model View Projection matrix's handle
GLuint MVM;  //Model View Matrix handle
GLuint PLL;  //Point Light Location
GLuint SLL;  //Spot Light Location
GLuint SLD;  //Spot Light Direction
GLuint SLA;  //Spot Light Angle
GLuint KA;   //Min ambient value
GLuint KD;   //Min diffuse value
GLuint KS;   //Max specular value
GLuint TL;   //Toggle Lights - Vec4(Ambient, Point, Spot, Crazy)
//Texture Shader
GLuint VP;   //View Projection Matrix

//Matrices used throughout the functions below
glm::mat4 projectionMatrix;          //set in reshape()
glm::mat4 viewMatrix;                //set in display();
glm::mat4 ModelViewProjectionMatrix; //set in display();
glm::mat4 scaleMatrix[NUM_MODELS];   //set in init(), used in scaleObjects

//Variables for writing to the windows title bar
char info[200]      = { 0 };
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

bool gameOver = false;
bool gameWon  = false;

//Space Box Texture Items ---------------------------------------------------------------------
const BYTE NUM_TEXTURES = 6;     //Number of Textures (Just the sky box.)
GLuint ibo[NUM_TEXTURES];        //IndexBufferObject
GLuint VAO_T[NUM_TEXTURES];      //Vertex Array Object for Textures
GLuint VBO_T[NUM_TEXTURES];      //Vertex Buffer Ojbect for Textures
GLuint texture[NUM_TEXTURES];

/*Names of the texture files.*/
static const char* TEX_FILE_NAME[] = {
    { "spacebox_front_1920_1200.raw" },
    { "spacebox_top_1280_720.raw" },
    { "spacebox_bottom_1920_1080.raw" },
    { "spacebox_back_1920_1200.raw" },
    { "spacebox_right_1920_1200.raw" },
    { "spacebox_left_2630_1315.raw" }
};

static const UINT32 PixelSizes[NUM_TEXTURES][2] = {
    { 1920, 1200 },
    { 1280, 720 },
    { 1920, 1080 },
    { 1920, 1200 },
    { 1920, 1200 },
    { 2630, 1315 }
};

/*A cube can be represented by 8 points... However, due to the way that the buffers work
I'm goint to get points for every planear surface. Mapped as if standing in middle of box.
These values will then be used to manually input a model (no RBG values, just vectors), and
we will map textures to these models.*/
static const GLfloat point[6][16] = {

    {//Front Square
        40000.0f, 40000.0f, 40000.0f, 1.0f,   // 0 Top Left
        -40000.0f, 40000.0f, 40000.0f, 1.0f,  // 1 Top Right
        -40000.0f, -40000.0f, 40000.0f, 1.0f, // 2 Bottom Right
        40000.0f, -40000.0f, 40000.0f, 1.0f   // 3 Bottom Left
    },
    {//Back Square
        -40000.0f, 40000.0f, -40000.0f, 1.0f,// 0 Top Left
        40000.0f, 40000.0f, -40000.0f, 1.0f,  // 1 Top Right
        40000.0f, -40000.0f, -40000.0f, 1.0f,  // 2 Bottom Right
        -40000.0f, -40000.0f, -40000.0f, 1.0f // 3 Bottom Left
    },
    {//Right Square
        40000.0f, 40000.0f, -40000.0f, 1.0f,  // 0 Top Left
        40000.0f, 40000.0f, 40000.0f, 1.0f,   // 1 Top Right
        40000.0f, -40000.0f, 40000.0f, 1.0f,  // 2 Bottom Right
        40000.0f, -40000.0f, -40000.0f, 1.0f  // 3 Bottom Left
    },
    {//Left Square
        -40000.0f, 40000.0f, 40000.0f, 1.0f,  // 0 Top Left
        -40000.0f, 40000.0f, -40000.0f, 1.0f, // 1 Top Right
        -40000.0f, -40000.0f, -40000.0f, 1.0f,// 2 Bottom Right
        -40000.0f, -40000.0f, 40000.0f, 1.0f  // 3 Bottom Left
    },
    {//Top square
        -40000.0f, 40000.0f, -40000.0f, 1.0f,  // 0 Top Left
        40000.0f, 40000.0f, -40000.0f, 1.0f,   // 1 Top Right
        40000.0f, 40000.0f, 40000.0f, 1.0f,    // 2 Bottom Right
        -40000.0f, 40000.0f, 40000.0f, 1.0f    // 3 Bottom Left
    },
    {//Bottom Square
        -40000.0f, -40000.0f, -40000.0f, 1.0f, // 0 Top Left
        40000.0f, -40000.0f, -40000.0f, 1.0f,  // 1 Top Right
        40000.0f, -40000.0f, 40000.0f, 1.0f,   // 2 Bottom Right
        -40000.0f, -40000.0f, 40000.0f, 1.0f   // 3 Bottom Left
    }

};


/*Using the points of the cube listed above, map out 6 planes which will act as
a box which we will map texture to. These planes will need to be in triangle format
and the point order <vertices> will need to be in a counter-clockwise orientation order.*/
static const unsigned int indices[6][6] = { // 2 faces, 3 vertices per face

    {/*Upper Triangle*/ 0, 3, 1,  /*Lower Triangle*/ 1, 3, 2 }, //Front
    {/*Upper Triangle*/ 0, 3, 1,  /*Lower Triangle*/ 1, 3, 2 }, //Back
    {/*Upper Triangle*/ 0, 3, 1,  /*Lower Triangle*/ 1, 3, 2 }, //Right
    {/*Upper Triangle*/ 0, 3, 1,  /*Lower Triangle*/ 1, 3, 2 }, //Left
    {/*Upper Triangle*/ 0, 3, 1,  /*Lower Triangle*/ 1, 3, 2 }, //Top
    {/*Upper Triangle*/ 0, 3, 1,  /*Lower Triangle*/ 1, 3, 2 } //Bottom
};

/*This array will have a one-to-one relation to the indices array above. This represents 
how a texture will be mapped to the object. */
static const GLfloat texCoords[] = {
    0.0f, 0.0f,     // 0
    1.0f, 0.0f,     // 1
    1.0f, 1.0f,     // 2
    0.0f, 1.0f };   // 3



//##############################################################################################
// Functions ###################################################################################
//##############################################################################################

void initObjects(void){
    /*This funciton's purpose is to initialize the different objects that will exist in
    the openGL window.  This function is variable in that I expect the programmer to modify
    this function with their specific models for each modification of scene.*/

    /*Constructor:
    CelestialBodySolid3D(char* objName, char* fileName, UINT32 vertexCount, float modelSize, bool active = true, 
                         CelestialBodySolid3D* cb3D = NULL, float radiansRot = 0, float radiansSpin = 0, bool orbital = false, bool spinning = false,
                         glm::mat4 spinningMatrix = glm::mat4(),
                         glm::vec3 spinAxis = glm::vec3(0,1,0),
                         glm::vec3 rotationAxis = glm::vec3(0, 1, 0),
                         glm::mat4 scaleMatrix = glm::mat4(),
                         glm::mat4 translationMatrix = glm::mat4(),
                         glm::mat4 rotationalMatrix = glm::mat4(),
                         glm::vec3 ambientMin = glm::vec3(.4, .4, .4),
                         glm::vec3 diffuseMin = glm::vec3(.4, .4, .4),
                         glm::vec3 specularMax = glm::vec3(.1, .1, .1)
    */

    //Red-Dwarf Star RUBER
    celestialSolid[ruber_mod] = new CelestialBodySolid3D("Ruber", "ruber.tri", (6336 * TRI), 2000, true, NULL, 0, .001f, false, true,
                                                         glm::mat4(), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::mat4(), glm::translate(glm::mat4(), glm::vec3(0, 0, 0)), glm::mat4(),
                                                         glm::vec3(1,1,1), glm::vec3(1,1,1));

    //Planet UNUM
    celestialSolid[unum_mod] = new CelestialBodySolid3D("Unum", "unum.tri", (1104 * TRI), 200, true, celestialSolid[ruber_mod], .004f, 0, true, false,
                                                        glm::mat4(), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::mat4(), glm::translate(glm::mat4(), glm::vec3(4000, 0, 0)), glm::mat4());

    //Planet DUO
    celestialSolid[duo_mod] = new CelestialBodySolid3D("Duo", "duo.tri", (1104 * TRI), 400, true, celestialSolid[ruber_mod], .002f, 0, true, false,
                                                       glm::mat4(), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::mat4(), glm::translate(glm::mat4(), glm::vec3(-9000, 0, 0)), glm::mat4());

    //Moon Primus
    celestialSolid[primus_mod] = new CelestialBodySolid3D("Primus", "primus.tri", (1104 * TRI), 100, true, celestialSolid[duo_mod], .004f, 0, true, false,
                                                          glm::mat4(), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::mat4(), glm::translate(glm::mat4(), glm::vec3(900, 0, 0)), glm::mat4());

    //Moon Secundus
    celestialSolid[secundus_mod] = new CelestialBodySolid3D("Secundus", "secundus.tri", (1104 * TRI), 150, true, celestialSolid[duo_mod], .002f, 0, true, false,
                                                            glm::mat4(), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::mat4(), glm::translate(glm::mat4(), glm::vec3(1650, 0, 0)), glm::mat4());


    /*Constructor:
    SpaceShip(char* objName, char* fileName, UINT32 vertexCount, float modelSize,
    bool active = true, UINT32 missleCount = 0,
    glm::mat4 scaleMatrix = glm::mat4(),
    glm::mat4 translationMatrix = glm::mat4(),
    glm::mat4 rotationalMatrix = glm::mat4(),
    glm::vec3 ambientMin = glm::vec3(.6, .6, .6),
    glm::vec3 diffuseMin = glm::vec3(.4, .4, .4),
    glm::vec3 specularMax = glm::vec3(.8, .8, .8)
    */

    //Warbird
    spaceShipSolid = new SpaceShip("Warbird", "warbird_v1.tri", (1816 * TRI), 100, true, 10,
                                   glm::mat4(), glm::translate(glm::mat4(), glm::vec3(5000, 1000, 5000)));



    /*Constructor:
        MissleSite(char* objName, char* fileName, UINT32 vertexCount, float modelSize,
        bool active = true, UINT32 missleCount = 0, CelestialBodySolid3D* obj = NULL,
        glm::mat4 scaleMatrix = glm::mat4(), 
        glm::mat4 translationMatrix = glm::mat4(),
        glm::mat4 rotationalMatrix = glm::mat4(),
        glm::vec3 ambientMin = glm::vec3(.6, .6, .6),
        glm::vec3 diffuseMin = glm::vec3(.4, .4, .4),
        glm::vec3 specularMax = glm::vec3(.8, .8, .8))
    */

    //Missle Sites
    missleSiteSolid[0] = new MissleSite("MissleSite0", "missle_site.tri", 266 * TRI, 30, true, 5, celestialSolid[unum_mod],
                                        glm::mat4(), glm::translate(glm::mat4(), glm::vec3(0, 200, 0)), glm::mat4()); // glm::rotate(glm::mat4(), PI / 2, glm::vec3(0, 0, -1)));

    missleSiteSolid[1] = new MissleSite("MissleSite1", "missle_site.tri", 266 * TRI, 30, true, 5, celestialSolid[secundus_mod],
                                        glm::mat4(), glm::translate(glm::mat4(), glm::vec3(0, 150, 0)), glm::mat4()); // glm::rotate(glm::mat4(), PI / 2, glm::vec3(0, 0, -1)));


    /*Constructor:
       SmartMissle(char* objName, char* fileName, UINT32 vertexCount, float modelSize,
                bool active = true, Object3D* objToFollow = NULL, Object3D* deployer = NULL, 
                float speed = 5, BYTE head_direction = 0, float distanceTilSmart = 245, 
                float maxDistanceTilDead = 15000,
                glm::mat4 scaleMatrix = glm::mat4(),
                glm::mat4 translationMatrix = glm::mat4(), 
                glm::mat4 rotationalMatrix = glm::mat4(),
                glm::vec3 ambientMin = glm::vec3(.6, .6, .6),
                glm::vec3 diffuseMin = glm::vec3(.4, .4, .4),
                glm::vec3 specularMax = glm::vec3(.8, .8, .8))
   */

    //Smart Missle
    smartMissleSolid[warbird_miss] = new SmartMissle("SmartMissle0", "missle.tri", 4120 * TRI, 25, false, NULL, spaceShipSolid, MISSLE_SPEED, SmartMissle::Top, DIST_TIL_SMART, MAX_DIST_TIL_XPLOD, glm::mat4(), glm::mat4(),
                                       glm::rotate(glm::mat4(), PI/2, glm::vec3(-1,0,0)));

    smartMissleSolid[ms_0_miss] = new SmartMissle("SmartMissle1", "missle.tri", 4120 * TRI, 25, false, NULL, missleSiteSolid[0], MISSLE_SPEED, SmartMissle::Top, DIST_TIL_SMART, MAX_DIST_TIL_XPLOD, glm::mat4(), glm::mat4(),
                                       glm::rotate(glm::mat4(), PI / 2, glm::vec3(-1, 0, 0)));

    smartMissleSolid[ms_1_miss] = new SmartMissle("SmartMissle2", "missle.tri", 4120 * TRI, 25, false, NULL, missleSiteSolid[1], MISSLE_SPEED, SmartMissle::Top, DIST_TIL_SMART, MAX_DIST_TIL_XPLOD, glm::mat4(), glm::mat4(),
                                       glm::rotate(glm::mat4(), PI / 2, glm::vec3(-1, 0, 0)));

    /*Have the modelPointer point to each object thats been created. (Take advantage of polymorphism)*/
    modelPointer[0] = celestialSolid[ruber_mod];
    modelPointer[1] = celestialSolid[unum_mod];
    modelPointer[2] = celestialSolid[duo_mod];
    modelPointer[3] = celestialSolid[primus_mod];
    modelPointer[4] = celestialSolid[secundus_mod];
    modelPointer[5] = spaceShipSolid;
    modelPointer[6] = missleSiteSolid[0];
    modelPointer[7] = missleSiteSolid[1];
    modelPointer[8] = smartMissleSolid[warbird_miss];
    modelPointer[9] = smartMissleSolid[ms_0_miss];
    modelPointer[10] = smartMissleSolid[ms_1_miss];

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
    cameras[free_cam] = new CameraView("FreeLook", glm::vec3(0, 0, 6000), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), CameraView::Free_View, NULL);
}

//##############################################################################################
void initAnimationSpeeds(void){
    /*Initialize different animation speeds.  The animation class is very basic... serves more as
    a simple struct but takes advantage of object programming.*/

    animationSpeeds[0] = new AnimationSpeed(40, "ace");
    animationSpeeds[1] = new AnimationSpeed(100, "pilot");
    animationSpeeds[2] = new AnimationSpeed(250, "trainee");
    animationSpeeds[3] = new AnimationSpeed(500, "debug");
    animationSpeeds[4] = new AnimationSpeed(1, "LUDIKRUS!");

}

//##############################################################################################
//Load the shader programs, vertex data from model files, create the solids, set initial view
void init(void) {

    //Local Variables
    float modelBoundingRadius[NUM_MODELS];  //Model's bounding radius

    // load the shader programs
    shaderProgram = loadShaders(VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE);
    textureShader = loadShaders(TEX_VERTEX_SHADER_FILE, TEX_FRAG_SHADER_FILE);

    // generate VAOs and VBOs
    glGenVertexArrays(NUM_MODELS, VAO);
    glGenBuffers(NUM_MODELS, VBO);
    glGenVertexArrays(NUM_TEXTURES, VAO_T);
    glGenBuffers(NUM_TEXTURES, VBO_T);
    glGenBuffers(NUM_TEXTURES, ibo);
    glGenTextures(NUM_TEXTURES, texture);



    /*Note: The for loop below is what binds our models to be used with the GL library.
    The function loadModelBuffer is provided by Prof. Barnes and is located in the iclude465.hpp .
    This call needs to be done after the generation of VAOs and VBOs*/

    //Specify the shader we want to use
    glUseProgram(shaderProgram);

    /*Load Buffer and get the models scale values*/
    for (int i = 0; i < NUM_MODELS; i++) {
        //Load the buffers from the model files
        modelBoundingRadius[i] = loadModelBuffer(modelPointer[i]->getFileName(),
                                                 modelPointer[i]->getVertexCount(),
                                                 VAO[i], VBO[i], shaderProgram,
                                                 "vPosition", "vColor", "vNormal");

        //Set scale for models given bounding radius
        /*Notes: AC3D's sizeTo function sets the diameters, not the radius.  The function
        above, provided by the professor, returns a value equal to the models radius
        according to as it is drawn.*/
        scaleMatrix[i] = glm::scale( glm::mat4(), glm::vec3(modelPointer[i]->getModelSize() * 1.0f / modelBoundingRadius[i]) );

    }

    //Set all the model scaling matrices
    scaleObjects();

    //Get handles for the uniform structures in the shader program
    MVP = glGetUniformLocation(shaderProgram, "ModelViewProjection");
    MVM = glGetUniformLocation(shaderProgram, "ModelViewMatrix");
    PLL = glGetUniformLocation(shaderProgram, "PointLightLocation");
    SLL = glGetUniformLocation(shaderProgram, "SpotLightLocation");
    SLD = glGetUniformLocation(shaderProgram, "SpotLightDirection");
    SLA = glGetUniformLocation(shaderProgram, "SpotLightAngle");
    KA = glGetUniformLocation(shaderProgram, "KA");
    KD = glGetUniformLocation(shaderProgram, "KD");
    KS = glGetUniformLocation(shaderProgram, "KS");
    TL = glGetUniformLocation(shaderProgram, "ToggleLights");

    //We can set some of these uniform values which wont change or have initial values
    glUniform1f(SLA, 0.349f /*20 degrees*/);
    glUniform4fv(TL, 1, glm::value_ptr(toggledLights));


    //Set up Texture stuff ******************************************************
    glUseProgram(textureShader);

    for (int i = 0; i < NUM_TEXTURES; i++){

        //Tel GL which VAO we are using/setting up.
        glBindVertexArray(VAO_T[i]);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[i]), indices[i], GL_STATIC_DRAW);

        //  initialize a buffer object
        glEnableVertexAttribArray(VBO_T[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_T[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(point[i]) + sizeof(texCoords), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point[i]), point[i]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(point[i]), sizeof(texCoords), texCoords);

        GLuint vPosition = glGetAttribLocation(textureShader, "vPosition");
        glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glEnableVertexAttribArray(vPosition);

        GLuint vTexCoord = glGetAttribLocation(textureShader, "vTexCoord");
        glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point[i])));
        glEnableVertexAttribArray(vTexCoord);

        //Get handles for the uniform structures in the texture shader program
        VP = glGetUniformLocation(textureShader, "ViewProjection");

        //Bind the texture that we want to use
        glBindTexture(GL_TEXTURE_2D, texture[i]); //bind the texture 

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //set texture environment parameters
        // set texture parameters
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        //Load texture
        texture[i] = loadRawTexture(texture[i], TEX_FILE_NAME[i], PixelSizes[i][0], PixelSizes[i][1]); // TEX_FILE_NAME[i], 750, 750);
        if (texture[i] != 0) {
            printf("texture file, read, texture generated and bound  \n");
        }
        else  // texture file loaded
            printf("Texture in file %s NOT LOADED !!! \n");
    }


    // set render state values
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    //Get initial time reading for statistical reporting
    lastTime = glutGet(GLUT_ELAPSED_TIME);

}


//##############################################################################################
void scaleObjects(void){

    /*Takes the scaling matrix and sets it to each object. Scaling matrix found in init().  This 
    function exists for the use of program restart.*/

    for (int i = 0; i < NUM_MODELS; i++)
        modelPointer[i]->setScaleMatrix(scaleMatrix[i]);

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

    //We always begin working with a clear slate.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Get the current view matrix
    viewMatrix = cameras[currentCamera]->getLookAt();

    //Switch to the appropriate shader $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    glUseProgram(shaderProgram);

    //Set the current point light location
    glUniform3fv(PLL, 1, glm::value_ptr(glm::vec3(viewMatrix * celestialSolid[ruber_mod]->getLastModelMatrix() * glm::vec4(0, 0, 0, 1))));

    /*Set the current spot light location, direction, and angle (TODO: Might want to start the light some distance in front of the ship)*/
    //Get the final to be location of the space ship
    glm::vec3 warbirdLoc = MathHelperJWF::getVector(viewMatrix * spaceShipSolid->getLastModelMatrix(), 3);
    //We'll want the light to be in front of the object, not coming from its' insides
    glm::vec3 warbirdLightLoc = MathHelperJWF::get3D_ScaledVector(MathHelperJWF::getVector(viewMatrix * spaceShipSolid->getLastModelMatrix(), 2), 100) + warbirdLoc;
    glUniform3fv(SLL, 1, glm::value_ptr(warbirdLightLoc));
    glUniform3fv(SLD, 1, glm::value_ptr(glm::vec3(viewMatrix * glm::vec4(MathHelperJWF::getVector(spaceShipSolid->getLastModelMatrix(), 2), 0))));

    /*Perform operations on all models and draw them ------------------------------------------*/

    //GO!
    for (int m = 0; m < NUM_MODELS; m++) {

        //Only draw object if it is active
        if (!modelPointer[m]->isActive())
            continue;

        //Get the model view projection matrix
        ModelViewProjectionMatrix = projectionMatrix * viewMatrix * modelPointer[m]->getLastModelMatrix(); // getModelMatrix();

        //Set the uniform structures in the shadre programs
        glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(ModelViewProjectionMatrix));
        glUniformMatrix4fv(MVM, 1, GL_FALSE, glm::value_ptr(viewMatrix * modelPointer[m]->getLastModelMatrix()));
        glUniform3fv(KA, 1, glm::value_ptr(modelPointer[m]->getAmbientMin()));
        glUniform3fv(KD, 1, glm::value_ptr(modelPointer[m]->getDiffuseMin()));
        glUniform3fv(KS, 1, glm::value_ptr(modelPointer[m]->getSpecularMax()));

        //Notify the graphics card that we will be using this models VAO information to draw it.
        glBindVertexArray(VAO[m]);

        //Invoke the vertex shader which will in turn continue down the pipeline and fill up the buffer
        glDrawArrays(GL_TRIANGLES, 0, modelPointer[m]->getVertexCount());
    }

    //Switch to the appropriate shader $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    glUseProgram(textureShader);

    //Set the View-Projection Matrix to be used with textures
    glUniformMatrix4fv(VP, 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix));

    /*Perform operations on all textures and draw them ------------------------------------------*/
    for (int i = 0; i < NUM_TEXTURES; i++){

        glBindVertexArray(VAO_T[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[i]);
        glBindTexture(GL_TEXTURE_2D, texture[i]);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

    }

    //Because most of the external operations depend upon the regular shader, switch to it.
    glUseProgram(shaderProgram);

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

    /*Missle Sites to fire missle?*/
    for (int index = 0; index < NUM_MISS_MOD; index++){

        if (missleSiteSolid[index]->isActive() && missleSiteSolid[index]->getMissleCount() > 0 && !smartMissleSolid[index + 1]->isActive()){

            float d = abs(glm::distance(MathHelperJWF::getVector(missleSiteSolid[index]->getLastModelMatrix(), 3), MathHelperJWF::getVector(spaceShipSolid->getLastModelMatrix(), 3)));

            //Are we within firing distance?
            if (abs(glm::distance(MathHelperJWF::getVector(missleSiteSolid[index]->getLastModelMatrix(), 3), MathHelperJWF::getVector(spaceShipSolid->getLastModelMatrix(), 3))) < DETECTION_DISTANCE){

                //Fire!!
                missleSiteSolid[index]->setMissleCount(missleSiteSolid[index]->getMissleCount() - 1);  //Decrement missle count

                smartMissleSolid[index + 1]->setTranslationMatrix(glm::translate(glm::mat4(), MathHelperJWF::get3D_ScaledVector(MathHelperJWF::getVector(missleSiteSolid[index]->getLastModelMatrix(), 1), 60)) 
                                                                  * glm::translate(glm::mat4(), MathHelperJWF::getVector(missleSiteSolid[index]->getLastModelMatrix(), 3)));
                smartMissleSolid[index + 1]->setRotationMatrix(missleSiteSolid[index]->getRotationMatrix());
                smartMissleSolid[index + 1]->setSmartMissleOn(false);
                smartMissleSolid[index + 1]->setCurrentDistance(0);
                smartMissleSolid[index + 1]->setActive(true);
                smartMissleSolid[index + 1]->getModelMatrix();  //update it
                smartMissleSolid[index + 1]->setObjectToFollow(spaceShipSolid);

            }

        }

    }

    //Does a warbird missle know where it's at?
    if (smartMissleSolid[warbird_miss]->isActive() && smartMissleSolid[warbird_miss]->getObjectToFollow() == NULL)
        findMissleTarget();

    /*Update all solid objects.*/
    for (int i = 0; i < NUM_MODELS; i++){
        modelPointer[i]->update(false);
    }

    /*Gravity Effects*/
    if (gravity && spaceShipSolid->isActive())
        gravityEffects();

    /*Collision Check*/
    colissionCheck();

    //Signal the need to update the display
    glutPostRedisplay();

    //Remove warbird movement flag restriction
    updatedWarbird = true;

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

    //If game is over, we stop updating...
    if (gameOver){
        //Update title bar
        printInfo();
        return;
    }


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

    case 'r': case 'R':  //Restart
        if (!gameOver) break;
        //To restart the game, we'll want to first clean up
        cleanUp();
        //Next, re-init the model, cameras, etc. objects
        initAnimationSpeeds();
        initCameras();
        initObjects();
        scaleObjects();
        //Re-init other variables
            //TODO Later
        //Finally, set game to not being over, and schedule update
        gameOver = false;
        glutTimerFunc(animationSpeeds[currentSpeed]->getSpeed(), intervalTimer, 1);
        break;

    case 'v': case 'V':  //Change To Next Camera View
        currentCamera++;
        if (currentCamera >= NUM_CAMERAS)
            currentCamera = 0;
        break;
    
    case 't': case 'T':  //Change animation speed
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
        if (currentWarbirdSpeed >= NUM_SPEEDS_WARBIRD)
            currentWarbirdSpeed = 0;
        break;

    case 'g': case 'G':  //Turn Gravity on/off
        gravity = (gravity == true) ? false : true;
        break;

    case 'f': case 'F':  //Fire Warbird Missle

        //Does the warbird have any missles left, or is a missle already active or does warbird still exist?
        if (spaceShipSolid->getMissleCount() <= 0 || smartMissleSolid[warbird_miss]->isActive() || !spaceShipSolid->isActive())
            return;

        //Decrement our missle count
        spaceShipSolid->setMissleCount(spaceShipSolid->getMissleCount() - 1);


        //Set the missles location, direction, speed, etc., and then set it to active
        //smartMissleSolid->setRotationMatrix(spaceShipSolid->getRotationMatrix());
        smartMissleSolid[warbird_miss]->setTranslationMatrix(spaceShipSolid->getTranslationMatrix());
        smartMissleSolid[warbird_miss]->setRotationMatrix(spaceShipSolid->getRotationMatrix());
        smartMissleSolid[warbird_miss]->setRotationMatrix(glm::rotate(glm::mat4(), PI / 2, MathHelperJWF::getVector(spaceShipSolid->getLastModelMatrix(), 0)) * smartMissleSolid[warbird_miss]->getRotationMatrix());
        smartMissleSolid[warbird_miss]->setSmartMissleOn(false);
        smartMissleSolid[warbird_miss]->setCurrentDistance(0);
        smartMissleSolid[warbird_miss]->setActive(true);
        smartMissleSolid[warbird_miss]->getModelMatrix();  //update it

        //Determine which missle site is in range, closer, and active
        findMissleTarget();

        break;

    case 'a': case 'A':  //Enable or Disable ambient lights
        toggledLights.x = (toggledLights.x == 0) ? 1.0f : 0.0f;
        glUniform4fv(TL, 1, glm::value_ptr(toggledLights));
        break;

    case 'p': case 'P':  //Enable or Disable ambient lights
        toggledLights.y = (toggledLights.y == 0) ? 1.0f : 0.0f;
        glUniform4fv(TL, 1, glm::value_ptr(toggledLights));
        break;

    case 'h': case 'H':  //Enable or Disable ambient lights
        toggledLights.z = (toggledLights.z == 0) ? 1.0f : 0.0f;
        glUniform4fv(TL, 1, glm::value_ptr(toggledLights));
        break;

    case 'z': case 'Z':  //Enable or Disable ambient lights
        toggledLights.w = (toggledLights.w == 0) ? 1.0f : 0.0f;
        glUniform4fv(TL, 1, glm::value_ptr(toggledLights));
        break;

    case '0':  //Change between project requirements and my preferred settings.

        //Change missle speed 
        if (PROJECT_REQUIREMENTS){
            PROJECT_REQUIREMENTS = false;
            MISSLE_SPEED = MISSLE_SPEEDS[1];
            glutIdleFunc(NULL);
        }
        else{
            PROJECT_REQUIREMENTS = true;
            MISSLE_SPEED = MISSLE_SPEEDS[0];
            glutIdleFunc(display);
        }

        //Manually update the missle speeds
        for (int index = 0; index < NUM_MIS_MOD; index++){
            smartMissleSolid[index]->setSpeed(MISSLE_SPEED);
        }

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
            CameraView::moveForwardOrBackwards(cameras[free_cam], FREE_CAM_TRAVEL);
            break;

        case 'k': case 'K':
            /*Same as forward, but final values are subtracted instead of added*/

            //Get directional-distance vector and auto-update values
            CameraView::moveForwardOrBackwards(cameras[free_cam], -FREE_CAM_TRAVEL);
            break;

        case 'j': case 'J':
            /*Turn left.*/
            CameraView::turnRightOrLeft(cameras[free_cam], -FREE_CAM_TURN);
            break;

        case 'l': case 'L':
            /*Turn right.*/
            CameraView::turnRightOrLeft(cameras[free_cam], FREE_CAM_TURN);
            break;

        case 'u': case 'U':
            /*Pitch Down.*/
            CameraView::pitchUpOrDown(cameras[free_cam], -FREE_CAM_TURN);
            break;

        case 'o': case 'O':
            /*Pitch Up.*/
            CameraView::pitchUpOrDown(cameras[free_cam], FREE_CAM_TURN);
            break;

        case 'm': case 'M':
            /*Roll left.*/
            CameraView::rollLeftOrRight(cameras[free_cam], FREE_CAM_TURN);
            break;

        case '.':
            /*Roll right.*/
            CameraView::rollLeftOrRight(cameras[free_cam], -FREE_CAM_TURN);
            break;
        }
    }

    //Request redisplay
    glutPostRedisplay();
}

//##############################################################################################
void specialKeyEvent(INT32 key, INT32 x, INT32 y){

    //Has the warbird already been updated on this round of updates?
    if (!updatedWarbird)
        return;

    //Assume that the warbird will be updated
    updatedWarbird = false;

    /*Space Ship movement forwards/backwards*/
    if ((key == GLUT_KEY_UP) && (glutGetModifiers() != GLUT_ACTIVE_CTRL)){ //Move forward
        spaceShipSolid->moveAdjust(warbirdSpeed[currentWarbirdSpeed]);
    }
    else if ((key == GLUT_KEY_DOWN) && (glutGetModifiers() != GLUT_ACTIVE_CTRL)){ //Move backwards
        spaceShipSolid->moveAdjust(-warbirdSpeed[currentWarbirdSpeed]);
    }
    
    /*Space Ship yaw adjusts.  Note: Remember right hand rule makes these opposite.*/
    else if ((key == GLUT_KEY_RIGHT) && (glutGetModifiers() != GLUT_ACTIVE_CTRL)){ //Yaw right
        spaceShipSolid->yawAdjust(-warbirdTurnSpeed);
    }
    else if ((key == GLUT_KEY_LEFT) && (glutGetModifiers() != GLUT_ACTIVE_CTRL)){ //Yaw left
        spaceShipSolid->yawAdjust(warbirdTurnSpeed);
    }

    /*Space Ship pitch adjusts.*/
    else if ((key == GLUT_KEY_UP) && (glutGetModifiers() == GLUT_ACTIVE_CTRL)){ //Pitch up
        spaceShipSolid->pitchAdjust(-warbirdTurnSpeed);
    }
    else if ((key == GLUT_KEY_DOWN) && (glutGetModifiers() == GLUT_ACTIVE_CTRL)){ //Pitch down
        spaceShipSolid->pitchAdjust(warbirdTurnSpeed);
    }

    /*Space Ship roll adjusts.  Note: Remember right hand rule makes these opposite.*/
    else if ((key == GLUT_KEY_RIGHT) && (glutGetModifiers() == GLUT_ACTIVE_CTRL)){ //Roll right
        spaceShipSolid->rollAdjust(warbirdTurnSpeed);
    }
    else if ((key == GLUT_KEY_LEFT) && (glutGetModifiers() == GLUT_ACTIVE_CTRL)){ //Roll left
        spaceShipSolid->rollAdjust(-warbirdTurnSpeed);
    }
    else{
        //If no update occurrs, set updated to false.
        updatedWarbird = true;
    }

    //Request redisplay
    glutPostRedisplay();

}

//##############################################################################################
void findMissleTarget(void){


    float distance1 = -1.0f;
    float distance2 = -1.0f;


    //Determine which missle site is in range, closer, and active
    if (missleSiteSolid[0]->isActive()){
        distance1 = abs(glm::distance(MathHelperJWF::getVector(missleSiteSolid[0]->getLastModelMatrix(), 3), MathHelperJWF::getVector(smartMissleSolid[warbird_miss]->getLastModelMatrix(), 3)));
        if (distance1 > DETECTION_DISTANCE)
            distance1 = -1.0f;
    }
    if (missleSiteSolid[1]->isActive()){
        distance2 = abs(glm::distance(MathHelperJWF::getVector(missleSiteSolid[1]->getLastModelMatrix(), 3), MathHelperJWF::getVector(smartMissleSolid[warbird_miss]->getLastModelMatrix(), 3)));
        if (distance2 > DETECTION_DISTANCE)
            distance2 = -1.0f;
    }

    if (distance1 == -1.0f && distance2 == -1.0f)
        smartMissleSolid[warbird_miss]->setObjectToFollow(NULL);
    else if (distance1 == -1.0f && distance2 > 0)
        smartMissleSolid[warbird_miss]->setObjectToFollow(missleSiteSolid[1]);
    else if (distance2 == -1.0f && distance1 > 0)
        smartMissleSolid[warbird_miss]->setObjectToFollow(missleSiteSolid[0]);
    else if (distance1 > distance2)
        smartMissleSolid[warbird_miss]->setObjectToFollow(missleSiteSolid[1]);
    else
        smartMissleSolid[warbird_miss]->setObjectToFollow(missleSiteSolid[0]);

}

//##############################################################################################
/*Simulates the effects of gravity between ruber and the space ship*/
void gravityEffects(void){

    /*Effects of Gravity*/  //Currently only exists between warbird and ruber
    /*Note: For simplicity, gravity will be represented as the following:
        GravityConstant / ( x^2 + y^2 + z^2)
    */

    //Get a copy of the spaceShips Model Matrix translation
    glm::vec3 tm(MathHelperJWF::getVector(spaceShipSolid->getLastModelMatrix(), 3));

    //Get the current force of gravity upon the spaceship
    double force = GRAVITY_CONSTANT / ((pow(tm[0], 2)) + (pow(tm[1], 2)) + (pow(tm[2], 2)));

    //Discover the vector between spaceship and ruber, scale it, and add it to ships translation
    glm::vec3 dir(MathHelperJWF::getVector(spaceShipSolid->getLastModelMatrix(), 3) - MathHelperJWF::getVector(celestialSolid[ruber_mod]->getLastModelMatrix(), 3));
    dir = MathHelperJWF::get3D_ScaledVector(dir, (float)force);

    //Note that if spaceship reaches <0,0,0>, it will fail b/c of the multiplication.
    spaceShipSolid->setTranslationMatrix(glm::translate(glm::mat4(), -dir) * spaceShipSolid->getTranslationMatrix());

}

//##############################################################################################
/*Checks for possible coliisions and updates accordingly.*/
void colissionCheck(void){

    //Currently, we only need to check if a colission exists between the warbird, missle, and missle sites.
    //Collision will happen if 10 or less pixels exist between the bounding spheres of two objects

    float spaceBetween;

    //If space ship is blown up, skip it
    if (!spaceShipSolid->isActive())
        goto MissleUpdate;

    /*Has the warbird collided with the sun, a planet, or a moon?*/
    for (int i = 0; i < NUM_CEL_MOD; i++){

        //If the celestial object isn't active, skip
        if (!celestialSolid[i]->isActive())
            continue;

        //Check for collisions
        spaceBetween = abs(glm::distance(MathHelperJWF::getVector(spaceShipSolid->getLastModelMatrix(), 3), MathHelperJWF::getVector(celestialSolid[i]->getLastModelMatrix(), 3)))
            - spaceShipSolid->getModelSize() - celestialSolid[i]->getModelSize();

        //If spaceBetween is less than 10, the spaceship blows up and game ends
        if (spaceBetween < 10){
            spaceShipSolid->setActive(false);
            gameOver = true;
            gameWon = false;
            break;
        }

    }

    /*Has the spaceship collided with a missle site?*/
    for (int i = 0; i < NUM_MISS_MOD; i++){

        //If the missle site isn't active, skip
        if (!missleSiteSolid[i]->isActive())
            continue;

        //Check for collisions
        spaceBetween = abs(glm::distance(MathHelperJWF::getVector(spaceShipSolid->getLastModelMatrix(), 3), MathHelperJWF::getVector(missleSiteSolid[i]->getLastModelMatrix(), 3)))
            - spaceShipSolid->getModelSize() - missleSiteSolid[i]->getModelSize();

        //If spaceBetween is less than 10, the spaceship blows up, missle site blows up, game ends
        if (spaceBetween < 10){
            spaceShipSolid->setActive(false);
            missleSiteSolid[i]->setActive(false);
            gameOver = true;
            gameWon = false;
            break;
        }

    }

MissleUpdate:  //Used by a goto

    for (int index = 0; index < NUM_MIS_MOD; index++){

        //Is the missle active?
        if (!smartMissleSolid[index]->isActive())
            continue;

        /*Has the missle collided with a missle site?*/
        for (int i = 0; i < NUM_MISS_MOD; i++){

            //If the missle site isn't active, skip
            if (!missleSiteSolid[i]->isActive())
                continue;

            //Check for collisions
            spaceBetween = abs(glm::distance(MathHelperJWF::getVector(smartMissleSolid[index]->getLastModelMatrix(), 3), MathHelperJWF::getVector(missleSiteSolid[i]->getLastModelMatrix(), 3)))
                - smartMissleSolid[index]->getModelSize() - missleSiteSolid[i]->getModelSize();

            //If spaceBetween is less than 10, the missle blows up, missle site blows up, unless...
            if (spaceBetween < 10){

                //We don't want the missle blowing up the missle site that launched it.
                if (smartMissleSolid[index]->getDeployer() == missleSiteSolid[i] && smartMissleSolid[index]->getCurrentDistance() < smartMissleSolid[index]->getDistanceTilSmart())
                    continue;

                //Else, blow up.
                smartMissleSolid[index]->setActive(false);
                missleSiteSolid[i]->setActive(false);
                smartMissleSolid[index]->setObjectToFollow(NULL);

                //Is the game over?
                if (!missleSiteSolid[0]->isActive() && !missleSiteSolid[1]->isActive()){
                    gameOver = true;
                    gameWon = true;
                }

                break;
            }

        }


        /*Has the missle collided with a celestial body?*/
        for (int i = 0; i < NUM_CEL_MOD; i++){

            //If the celestial object isn't active, skip
            if (!celestialSolid[i]->isActive())
                continue;

            //Check for collisions
            spaceBetween = abs(glm::distance(MathHelperJWF::getVector(smartMissleSolid[index]->getLastModelMatrix(), 3), MathHelperJWF::getVector(celestialSolid[i]->getLastModelMatrix(), 3)))
                - smartMissleSolid[index]->getModelSize() - celestialSolid[i]->getModelSize();

            //If spaceBetween is less than 10, the spaceship blows up and game ends
            if (spaceBetween < 10){
                smartMissleSolid[index]->setActive(false);
                smartMissleSolid[index]->setObjectToFollow(NULL);
                goto THEEND;
            }

        }

        /*Has the missle collided with the space ship?*/

        //Check for collisions
        spaceBetween = abs(glm::distance(MathHelperJWF::getVector(smartMissleSolid[index]->getLastModelMatrix(), 3), MathHelperJWF::getVector(spaceShipSolid->getLastModelMatrix(), 3)))
            - smartMissleSolid[index]->getModelSize() - spaceShipSolid->getModelSize();

        //If spaceBetween is less than 10, the missle blows up, space ship blows up, game ends
        if (spaceBetween < 20){

            //We don't want the missle blowing up the spaceship upon launch
            if (!(smartMissleSolid[index]->getDeployer() == spaceShipSolid && smartMissleSolid[index]->getCurrentDistance() < smartMissleSolid[index]->getDistanceTilSmart())){

                smartMissleSolid[index]->setActive(false);
                spaceShipSolid->setActive(false);
                smartMissleSolid[index]->setObjectToFollow(NULL);
                gameOver = true;
                gameWon = false;

                goto THEEND;

            }
        }

        /*Has the missle collided with another missle?*/
        for (int i = 0; i < NUM_MIS_MOD; i++){

            //Skip inactives
            if (!smartMissleSolid[i]->isActive())
                continue;

            //A missle cannot collide with itself
            if (smartMissleSolid[i] == smartMissleSolid[index])
                continue;

            //Check for collisions
            spaceBetween = abs(glm::distance(MathHelperJWF::getVector(smartMissleSolid[index]->getLastModelMatrix(), 3), MathHelperJWF::getVector(smartMissleSolid[i]->getLastModelMatrix(), 3)))
                - smartMissleSolid[index]->getModelSize() - smartMissleSolid[i]->getModelSize();

            //If spaceBetween < 10, both missles will blow up!
            if (spaceBetween < 10){

                smartMissleSolid[index]->setActive(false);
                smartMissleSolid[i]->setActive(false);
                smartMissleSolid[index]->setObjectToFollow(NULL);
                smartMissleSolid[i]->setObjectToFollow(NULL);

            }

        }



    }

    THEEND:

    return;

}

//##############################################################################################
/*Prints general information to the windows title bar.*/
void printInfo(void){

    if (gameOver){
        if (gameWon)
            sprintf(info, "Cadet PASSES Flight Training!");
        else
            sprintf(info, "Cadet Resigns from War College! --I hear the Federation is recruiting.");
    }
    else{
        sprintf(info, "Warbird: %d  Unum: %d  Secundus:  %d  U/S %d  F/S %d  Camera View %s  Setting: %s",
                spaceShipSolid->getMissleCount(), missleSiteSolid[0]->getMissleCount(), missleSiteSolid[1]->getMissleCount(),
                updateRate, frameRate, cameras[currentCamera]->getViewId(), animationSpeeds[currentSpeed]->getID());

        if (PROJECT_REQUIREMENTS)
            sprintf(info, "%s     PROGRAM SETTINGS: Project_Requirements", info);
        else
            sprintf(info, "%s     PROGRAM SETTINGS: J-Soft Mods", info);
    }

    //Set the windows title
    glutSetWindowTitle(info);

    //Request re-display
    glutPostRedisplay();

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
    glutIdleFunc(display);
    glutTimerFunc(animationSpeeds[currentSpeed]->getSpeed(), intervalTimer, 1);
    glutMainLoop();


    //Unallocate everything
    cleanUp();


    printf("done\n");
    return 0;
}