/*Created By: Jared Fowler
**April 8, 2015
**
**The openGL tutorials did come
**with a control function, however,
**it was configured for glfw3. This
**is configured for glut
**
*/

#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/freeglut.h>
#include <vector>
#include <wtypes.h>

//Audio Library
#include <irrKlang.h>
#include <conio.h>
#pragma comment(lib, "../Common/AUDIO/irrKlang.lib")

//DEFINES*****************************************
//************************************************
#define MouseSpeed      .05f
#define CONTROL_FORWARD_SPEED       20
#define CONTROL_STRAFF_SPEED        10

//Keys
#define ESCAPE_KEY      033

//Flags
#define CONTROL_EXIT    (0x0080)
#define CONTROL_FPS     (0x0040)
#define CONTROL_TEXT2D  (0x0020)

//Audio
#define CONTROL_MAX_MUSIC   50
#define CONTROL_MAX_SOUND   100

//PROTOTYPES**************************************
//************************************************
UINT16 keyboardAndMouse(void);
void resetControlOutMatrices(void);
void keyBoardEvent(UINT8, INT32, INT32);
void mouseMoveEvent(INT32, INT32);
void specialKeyEvent(INT32, INT32, INT32);
void windowResize(INT32, INT32);
void GetDesktopResolution(float&, float&);
bool handleFPS(void);

//ENUMS*******************************************
//************************************************
enum Cameras {
    FREE_CAM = 0,
    SAT_CHASE,
    EARTH_CHASE,
    MOON_EARTH,
    SUN_EARTH,
    END_CAMERAS
};

//STRUCTS*****************************************
//************************************************
struct Control_Output {
    glm::mat4 translation = glm::mat4();
    glm::mat4 rotation = glm::mat4();
    glm::mat4 projection = glm::mat4();
};

struct Control_Input {
    glm::vec2 mousePosition;
    std::vector<UINT8> keysPressed;
    std::vector<INT32> specialKeysPressed;
};

struct FPS {
    UINT64 lastTime = 0;
    UINT32 frameCount = 0;
    UINT64 interval = 40;
    UINT32 fpsI = 0;
    char fpsC[20];
};

struct AUDIO {
    const char* musicFile[CONTROL_MAX_MUSIC];
    const char* soundFile[CONTROL_MAX_SOUND];
    irrklang::ISoundEngine* engine;
    UINT8 currentTrack;
};

struct FLAGS {
    bool maxFPS = false;
    bool showText = true;
    bool mouseLook = true;
    bool musicOn = true;
    bool soundOn = true;
};

//GLOBAL VAR**************************************
//************************************************
static glm::vec2 WindowDim(1024, 768);
static UINT64 lastTime = glutGet(GLUT_ELAPSED_TIME);
static glm::vec2 centerMouse(WindowDim.x / 2, WindowDim.y / 2);

UINT8 cameraMode = 0;
//OUTPUT******************************************
//************************************************
static Control_Output controlOutput;
static Control_Input controlIn;
static FPS fps;
static FLAGS flags;

static AUDIO audio {
    { "../Common/Content/Ascent.mp3", "../Common/Content/CosmicSunrise.mp3", "../Common/Content/OpenCluster.mp3", "../Common/Content/BrainDead.mp3", },
    { NULL }, 
    NULL,
    0
};

//FUNCTIONS***************************************
//************************************************
UINT16 keyboardAndMouse() {

    //Local Variables
    UINT16 returnFlag = 0;
    UINT64 currentTime = glutGet(GLUT_ELAPSED_TIME);
    UINT32 deltaTime = (UINT32)(currentTime - lastTime);
    float hAngle = 0;
    float vAngle = 0;

    //Handle mouse movement event
    if (flags.mouseLook && (controlIn.mousePosition.x != centerMouse.x || controlIn.mousePosition.y != centerMouse.y)){
        //Get the cange in mouse movement and then reset the mouse
        glm::vec2 deltaMouse(controlIn.mousePosition.x - centerMouse.x, controlIn.mousePosition.y - centerMouse.y);
        glutWarpPointer((int)centerMouse.x, (int)centerMouse.y);
        controlIn.mousePosition = centerMouse;

        //Calculate the angles of rotation
        hAngle = deltaMouse.x * MouseSpeed;
        vAngle = deltaMouse.y * MouseSpeed;

        //Update the rotational output matrix
        controlOutput.rotation = glm::rotate(glm::mat4(), glm::radians(hAngle), glm::vec3(0, 1, 0)) *
                                 glm::rotate(glm::mat4(), glm::radians(vAngle), glm::vec3(1, 0, 0)) *
                                 controlOutput.rotation;

    }
    //Handle keyboard input event
    for (UINT16 i = 0; i < controlIn.keysPressed.size(); i++){
        //Get the next keyboard input from the vector
        UINT8 key = controlIn.keysPressed.at(i);

        switch (key) {
        case ESCAPE_KEY:  //Quit program
            returnFlag |= CONTROL_EXIT;
            goto keyboardAndMouse_END;
            break;
        case 'F': case 'f':
            cameraMode = FREE_CAM;
            break;
        case 'V': case 'v':
            cameraMode = (cameraMode + 1 == END_CAMERAS) ? 1 : cameraMode + 1;
        default:
            break;
        }
    }
    //Handle Special key events
    for (UINT16 i = 0; i < controlIn.specialKeysPressed.size(); i++){
        //Get the next keyboard input from the vector
        INT32 key = controlIn.specialKeysPressed.at(i);
        
        if(key == GLUT_KEY_UP)  //Move camera Forward
            controlOutput.translation = glm::translate(glm::mat4(), glm::vec3(0, 0, CONTROL_FORWARD_SPEED)) *
                controlOutput.translation;
        else if(key == GLUT_KEY_DOWN)  //Move camera Backwards
            controlOutput.translation = glm::translate(glm::mat4(), glm::vec3(0, 0, -CONTROL_FORWARD_SPEED)) *
                controlOutput.translation;
        else if(key == GLUT_KEY_RIGHT)  //Move camera Right
            controlOutput.translation = glm::translate(glm::mat4(), glm::vec3(-CONTROL_STRAFF_SPEED, 0, 0)) *
                controlOutput.translation;
        else if (key == GLUT_KEY_LEFT)  //Move camera Left
            controlOutput.translation = glm::translate(glm::mat4(), glm::vec3(CONTROL_STRAFF_SPEED, 0, 0)) *
            controlOutput.translation;
        else if (key == GLUT_KEY_F1)
            flags.mouseLook = (flags.mouseLook == true) ? false : true;
        else if (key == GLUT_KEY_F2){
            returnFlag |= CONTROL_FPS;
            flags.maxFPS = (flags.maxFPS == true) ? false : true;
        }
        else if (key == GLUT_KEY_F3){
            returnFlag |= CONTROL_TEXT2D;
            flags.showText = (flags.showText == true) ? false : true;
        }
        else if (key == GLUT_KEY_F4){
            if (flags.musicOn){
                flags.musicOn = false;
                audio.engine->stopAllSounds();
            }
            else{
                flags.musicOn = true;
                audio.engine->play2D(audio.musicFile[audio.currentTrack], true);
            }
        }
        else if (key == GLUT_KEY_F5){
            audio.engine->removeSoundSource(audio.musicFile[audio.currentTrack]);
            audio.currentTrack = (audio.musicFile[audio.currentTrack + 1] == NULL) ? 0 : audio.currentTrack + 1;
            audio.engine->play2D(audio.musicFile[audio.currentTrack], true);
        }

    }

    //Label that we can jump to
    keyboardAndMouse_END:

    //Empty the keypressed vectors
    controlIn.keysPressed.clear();
    controlIn.specialKeysPressed.clear();

    return returnFlag;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//Resets the matrices in controlOut
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void resetControlOutMatrices(void){
    controlOutput.translation = glm::mat4();
    controlOutput.rotation = glm::mat4();
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//Handles keyboard events
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void keyBoardEvent(UINT8 key, INT32 x, INT32 y){
    //If key doesn't exist in stack, push it back
    for (UINT16 i = 0; i < controlIn.keysPressed.size(); i++){
        if (controlIn.keysPressed.at(i) == key)
            return;
    }
    controlIn.keysPressed.push_back(key);
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//Handles mouse movement events
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void mouseMoveEvent(INT32 x, INT32 y){
    //Update the current mouse position
    controlIn.mousePosition = glm::vec2(x, y);
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//Handles special events like up, down, alt, ctrl, etc.
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void specialKeyEvent(INT32 key, INT32 x, INT32 y){
    //If key doesn't exist in stack, push it back
    for (UINT16 i = 0; i < controlIn.specialKeysPressed.size(); i++){
        if (controlIn.specialKeysPressed.at(i) == key)
            return;
    }
    controlIn.specialKeysPressed.push_back(key);
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//Handles the resizing of the glut window
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void windowResize(INT32 w, INT32 h){
    //Set the viewport
    glViewport(0, 0, w, h);
    //Set the projection matrix
    controlOutput.projection = glm::perspective(glm::radians(60.0f), (float)w / (float)h, 1.0f, 1000000.0f);
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//Gets the size of the host computer screen
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void GetDesktopResolution(float& horizontal, float& vertical)
{
    RECT desktop;
    // Get a handle to the desktop window
    const HWND hDesktop = GetDesktopWindow();
    // Get the size of screen to the variable desktop
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    horizontal = (float)desktop.right;
    vertical = (float)desktop.bottom;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//This function handles fps info and updates interval timming
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
bool handleFPS(void){
    //Update the number of frames
    fps.frameCount += 1;
    //Has a second gone by?
    UINT64 time = glutGet(GLUT_ELAPSED_TIME);
    if (time - fps.lastTime >= 1000){
        //Get the fps
        fps.fpsI = fps.frameCount;
        sprintf(fps.fpsC, "%u", fps.frameCount);
        //Do we need to slow down or speed up?
        if (flags.maxFPS == false){
            if (fps.fpsI > 60)
                fps.interval += 1;
            else if (fps.fpsI < 60)
                fps.interval -= 1;
        }
        //Reset the frame count
        fps.frameCount = 0;
        //Back-up the current time
        fps.lastTime = time;
        return true;
    }
    //No update, return false
    return false;
}

#endif