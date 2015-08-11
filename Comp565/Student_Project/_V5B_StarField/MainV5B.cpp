/*Created By: Jared Fowler
**April 6, 2015
**
**Using the openGL tutorials as a reference, this
**project's purpose is to load in a model .obj file,
**which comes with vertices, normals, texture coords,
**and indices. Place this model in the middle of the
**scene and have it rotate.
**
*/

#include "../Common/Includes/JWF_INCLUDES.hpp"

#define SC      0.0002f//0.00045f

//PROTOTYPES**************************************
//************************************************
void init(void);
void cleanUp(void);
void draw(void);
void update(int a);

//ENUMERATIONS************************************
//************************************************
const UINT16 NUM_MODELS = 5;

enum SatelliteModelsEnum {
    SUN = 0,
    EARTH,
    MOON,
    CUBESAT,
    END
};

enum BackgroundModelsEnum {
    STARFIELD = 0,
    END_BACKGROUNDS
};

//MODEL INFO**************************************
//************************************************
const ModelInfo modelInfo[NUM_MODELS] {
    { "../Common/Content/StarField.obj", { "../Common/Content/StarField.DDS", NULL }, "./Shaders/StarField.vertexshader", "./Shaders/StarField.fragmentshader" },
    { "../Common/Content/Sun.obj", { "../Common/Content/Sun.DDS", NULL }, "./Shaders/SunShader.vertexshader", "./Shaders/SunShader.fragmentshader" },
    { "../Common/Content/Earth_Clouds.obj", { "../Common/Content/Earth_Clouds.DDS", "../Common/Content/Earth_Clouds_Night.DDS", NULL }, "./Shaders/EarthShader.vertexshader", "./Shaders/EarthShader.fragmentshader" },
    { "../Common/Content/Moon.obj", { "../Common/Content/Moon.DDS", NULL }, "./Shaders/MoonShader.vertexshader", "./Shaders/MoonShader.fragmentshader" },
    { "../Common/Content/CubeSat.obj", { "../Common/Content/CubeSat.DDS", NULL }, "./Shaders/CubeSatShader.vertexshader", "./Shaders/CubeSatShader.fragmentshader" },
};

const glm::vec4 staticLights[MAX_LIGHTS] {
    glm::vec4(0.0f, 0.0f, 0.0f, 5000000000.0f),
};

//GLOBAL VARIABLES********************************
//************************************************
std::vector<ModelBuffer> modelBuffers(NUM_MODELS);
Object3D* modelPointer[NUM_MODELS];
CelestialBodySolid3D* celestialSolid[END];
Object3D* backgroundSolid[END_BACKGROUNDS];

glm::mat4 viewMatrix;
glm::mat4 projectionMatrix = glm::perspective(.5f, (float)(1024 / 550), 50.0f, 200000.0f);
glm::mat4 modelViewMatrix;

//FUNCTIONS***************************************
//************************************************
int main(int argc, char* argv[]){

    //Initialize window size and projection
    //GetDesktopResolution(WindowDim.x, WindowDim.y);
    windowResize((INT32)WindowDim.x, (INT32)WindowDim.y);

    //Initialize GLUT Window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize((int)WindowDim.x, (int)WindowDim.y);
    glutInitWindowPosition(15, 15);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("Star Field");

    //Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    //Initialize openGL Configs
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Initialize shaders, models, etc.
    init();

    //Music
    audio.engine = irrklang::createIrrKlangDevice();
    if (!audio.engine){
        fprintf(stderr, "Could not startup engine.\n");
        return 0;
    }
    audio.engine->play2D(audio.musicFile[0], true);

    //Set glut callback functions
    glutDisplayFunc(draw);
    glutTimerFunc(2000, update, 0);
    glutKeyboardFunc(keyBoardEvent);
    glutPassiveMotionFunc(mouseMoveEvent);
    glutSpecialFunc(specialKeyEvent);
    glutReshapeFunc(windowResize);
    glutIdleFunc(NULL);
    //glutFullScreen();
    glutMainLoop();

    //Cleanup Time
    cleanUp();
    audio.engine->drop();

    return 0;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//Initializes shaders, models, textures, etc.
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void init(void){
    //Local Variables
    char uniformName[50] = { 0 };

    //Load Shaders, generate VAO's, bind, and fill buffers 
    for (UINT16 i = 0; i < modelBuffers.size(); i++){
        //For simplicity, create a pointer to the current modelBuffer
        ModelBuffer* mbPtr = &modelBuffers[i];

        //Initialize the modelBuffer structure to all 0's
        initModelBufferStruct(mbPtr);

        //Create a new VAO and bind it
        glGenVertexArrays(1, &(mbPtr->vao));
        glBindVertexArray(mbPtr->vao);

        //Load Shader and set handles
        mbPtr->glsl.shader = LoadShaders(modelInfo[i].vertexShaderFileName, modelInfo[i].fragShaderFileName);
        mbPtr->glsl.matrixID = glGetUniformLocation(mbPtr->glsl.shader, "MVP");
        mbPtr->glsl.modelViewMatrixID = glGetUniformLocation(mbPtr->glsl.shader, "MV");
        mbPtr->glsl.viewMatrixID = glGetUniformLocation(mbPtr->glsl.shader, "V");
        mbPtr->glsl.modelMatrixID = glGetUniformLocation(mbPtr->glsl.shader, "M");

        for (UINT8 j = 0; j < MAX_TEXTURES; j++){
            sprintf(uniformName, "myTextureSampler_%u", j);
            mbPtr->glsl.textureID[j] = glGetUniformLocation(mbPtr->glsl.shader, uniformName);
        }
        for (UINT8 j = 0; j < MAX_TEXTURES; j++){
            sprintf(uniformName, "LightPosition_worldspace_%u", j);
            mbPtr->glsl.lightID[j] = glGetUniformLocation(mbPtr->glsl.shader, uniformName);
        }

        //Load textures
        for (UINT8 j = 0; j < MAX_TEXTURES; j++){
            if (modelInfo[i].textureFileName[j] == NULL) //Done loading?
                break;
            if ((mbPtr->texture[j] = loadDDS(modelInfo[i].textureFileName[j])) == 0){
                fprintf(stderr, "Failed to load texture: %s\r\n", modelInfo[i].textureFileName[j]);
            }
        }

        //Load model from file .obj
        if (loadOBJ(modelInfo[i].modelFileName, mbPtr->vertexBuffer, mbPtr->textureBuffer, mbPtr->normalBuffer) == false){
            fprintf(stderr, "Failed to load object: %s\r\n", modelInfo[i].modelFileName);
        }
        //Transform object vertices, normals into indexed_vertices and normals
        indexVBO(mbPtr->vertexBuffer, mbPtr->textureBuffer, mbPtr->normalBuffer,
            mbPtr->indices, mbPtr->indexed_vertices, mbPtr->indexed_texture, mbPtr->indexed_normals);

        //Load vertex info into a VBO
        glGenBuffers(1, &(mbPtr->vbo));
        glBindBuffer(GL_ARRAY_BUFFER, mbPtr->vbo);
        glBufferData(GL_ARRAY_BUFFER, mbPtr->indexed_vertices.size() * sizeof(glm::vec3), &(mbPtr->indexed_vertices[0]), GL_STATIC_DRAW);
        //Load normal info into a VBO
        glGenBuffers(1, &(mbPtr->vno));
        glBindBuffer(GL_ARRAY_BUFFER, mbPtr->vno);
        glBufferData(GL_ARRAY_BUFFER, mbPtr->indexed_normals.size() * sizeof(glm::vec3), &(mbPtr->indexed_normals[0]), GL_STATIC_DRAW);
        //Load texture vertices into a VBO
        glGenBuffers(1, &(mbPtr->vto));
        glBindBuffer(GL_ARRAY_BUFFER, mbPtr->vto);
        glBufferData(GL_ARRAY_BUFFER, mbPtr->indexed_texture.size() * sizeof(glm::vec2), &(mbPtr->indexed_texture[0]), GL_STATIC_DRAW);
        //Load indices into an element buffer
        glGenBuffers(1, &(mbPtr->veo));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mbPtr->veo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mbPtr->indices.size() * sizeof(UINT16), &(mbPtr->indices[0]), GL_STATIC_DRAW);

        //Unbind VAO
        glBindVertexArray(0);

        //Clean up local storage
        mbPtr->vertexBuffer.clear();
        mbPtr->normalBuffer.clear();
        mbPtr->textureBuffer.clear();

    }

    //Initiate 2D-Text Handler
    initText2D("../Common/Content/Holstein.DDS");

    //Initiage model objects
    /*Constructor:
    CelestialBodySolid3D(bool active = true, CelestialBodySolid3D* cb3D = NULL,
    float orbitSpeed = 0, float rotationSpeed = 0,
    glm::mat4 scaleMatrix = glm::mat4(),
    glm::mat4 translationMatrix = glm::mat4(),
    glm::mat4 rotationalMatrix_0 = glm::mat4(),
    glm::mat4 rotationalMatrix_1 = glm::mat4())
    */

    celestialSolid[SUN] = new CelestialBodySolid3D(true, NULL, 0.0f * SC, .0366f * SC, glm::mat4(), glm::mat4(), glm::mat4(), glm::mat4());
    celestialSolid[EARTH] = new CelestialBodySolid3D(true, celestialSolid[SUN], 0.00274f * SC, 1.0f * SC, glm::mat4(), glm::translate(glm::mat4(), glm::vec3(0, 0, 85000)), glm::rotate(glm::mat4(), glm::radians(23.5f), glm::vec3(0, 0, 1)), glm::mat4());
    celestialSolid[MOON] = new CelestialBodySolid3D(true, celestialSolid[EARTH], .0366f * SC, 0.0366f * SC, glm::mat4(), glm::translate(glm::mat4(), glm::vec3(0, 0, -2500)), glm::mat4(), glm::mat4());
    celestialSolid[CUBESAT] = new CelestialBodySolid3D(true, celestialSolid[EARTH], -16.166f * SC, 0.0f * SC, glm::scale(glm::mat4(), glm::vec3(.3, .3, .3)), glm::translate(glm::mat4(), glm::vec3(0, 0, -175)), glm::rotate(glm::mat4(), glm::radians(30.0f), glm::vec3(1,0,0)), glm::rotate(glm::mat4(), glm::radians(55.0f), glm::vec3(1,0,0)) * glm::rotate(glm::mat4(), glm::radians(60.0f), glm::vec3(0,1,0)));
    
    backgroundSolid[STARFIELD] = new Object3D(true, glm::scale(glm::mat4(), glm::vec3(100,100,100)));    ///*Default Constructor*/);
    backgroundSolid[STARFIELD]->setConfigurationFlags(Object3D::BACKGROUND_MODEL);

    //List of pointers to all existing objects
    modelPointer[0] = backgroundSolid[STARFIELD];
    modelPointer[1] = celestialSolid[SUN];
    modelPointer[2] = celestialSolid[EARTH];
    modelPointer[3] = celestialSolid[MOON];
    modelPointer[4] = celestialSolid[CUBESAT];

    //Initialize viewMatrix
    viewMatrix = glm::lookAt(celestialSolid[EARTH]->getLMM_Translation() + glm::vec3(-200, 500, -2000), celestialSolid[EARTH]->getLMM_Translation(), glm::vec3(0, 1, 0));
    
    //Initialize Mouse
    glutWarpPointer((int)centerMouse.x, (int)centerMouse.y);
    controlIn.mousePosition = centerMouse;
    glutSetCursor(GLUT_CURSOR_NONE);

    return;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//Cleans up buffers on the graphics card, shaders, etc.
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void cleanUp(void){
    //Clean up for every object
    for (UINT16 i = 0; i < modelBuffers.size(); i++){
        //Get a handle to the current model
        ModelBuffer* mbPtr = &modelBuffers[i];

        //Delete VBO if it exists
        if (mbPtr->vbo != 0){
            glDeleteBuffers(1, &mbPtr->vbo);
            mbPtr->vbo = 0;
        }
        //Delete VTO if it exists
        if (mbPtr->vto != 0){
            glDeleteBuffers(1, &mbPtr->vto);
            mbPtr->vto = 0;
        }
        //Delete VEO if it exists
        if (mbPtr->veo != 0){
            glDeleteBuffers(1, &mbPtr->veo);
            mbPtr->veo = 0;
        }
        //Delete Texture if it exists
        for (UINT8 j = 0; j < MAX_TEXTURES; j++){
            if (mbPtr->texture[j] != 0){
                glDeleteTextures(1, &mbPtr->texture[j]);
                mbPtr->texture[j] = 0;
            }
        }
        //Delete VAO if it exists
        if (mbPtr->vao != 0){
            glDeleteVertexArrays(1, &mbPtr->vao);
            mbPtr->vao = 0;
        }
        //Delete Shaders if they exist
        if (mbPtr->glsl.shader != 0){
            glDeleteProgram(mbPtr->glsl.shader);
            mbPtr->glsl.shader = 0;
        }
    }
    //Clean up object models
    for (UINT16 i = 0; i < NUM_MODELS; i++){
        delete modelPointer[i];
        modelPointer[i] = NULL;
    }
        
    return;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//Display Function
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void draw(void){
    //Local Variables
    float* ptrF = NULL;

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Iterate through our list of models and load up draw buffer
    for (UINT16 i = 0; i < modelBuffers.size(); i++){
        //Get a handle to the models modelBuffer
        ModelBuffer* mbPtr = &modelBuffers[i];

        //Bind VAO and Shaders
        glBindVertexArray(mbPtr->vao);
        glUseProgram(mbPtr->glsl.shader);

        //Get the modelView matrix and MVP matrix
        modelViewMatrix = viewMatrix * modelPointer[i]->getLastModelMatrix();
        glm::mat4 mvp = projectionMatrix * modelViewMatrix;
        glm::mat4 viewMatrixT = viewMatrix;
        
        //Configurations
        
        UINT32 flags = modelPointer[i]->getConfigurationFlags();
        //Depending on configs, we may want to change some of the matrices
        if ((flags & Object3D::BACKGROUND_MODEL) == Object3D::BACKGROUND_MODEL){
            //We can never get close to the background
            glDisable(GL_DEPTH_TEST);
            //We only want to rotate the background, not translate it
            viewMatrixT[3][0] = viewMatrixT[3][1] = viewMatrixT[3][2] = 0;
            viewMatrixT[3][3] = 1;
            viewMatrixT = projectionMatrix * viewMatrixT;
        }
        else{
            glEnable(GL_DEPTH_TEST);
        }
        

        //Set matrix shader uniforms. -1 indicates that they don't exist
        //Matrices
        if (mbPtr->glsl.matrixID != -1)          glUniformMatrix4fv(mbPtr->glsl.matrixID, 1, GL_FALSE, &mvp[0][0]);
        if (mbPtr->glsl.modelMatrixID != -1)     glUniformMatrix4fv(mbPtr->glsl.modelMatrixID, 1, GL_FALSE, &(modelPointer[i]->getLastModelMatrix()[0][0]));
        if (mbPtr->glsl.viewMatrixID != -1)      glUniformMatrix4fv(mbPtr->glsl.viewMatrixID, 1, GL_FALSE, &viewMatrixT[0][0]);
        if (mbPtr->glsl.modelViewMatrixID != -1) glUniformMatrix4fv(mbPtr->glsl.modelViewMatrixID, 1, GL_FALSE, &modelViewMatrix[0][0]);
        //Light Positions
        for (UINT8 j = 0; j < MAX_LIGHTS; j++){
            if ((INT32)mbPtr->glsl.lightID[j] > 0){
                glUniform4f(mbPtr->glsl.lightID[j], staticLights[j].x, staticLights[j].y, staticLights[j].z, staticLights[j].w);
            }
            else
                break;
        }

        //Bind Textures
        for (UINT16 j = 0; j < MAX_TEXTURES; j++){
            if ((INT32)mbPtr->texture[j] > 0){
                glActiveTexture(GL_TEXTURE0 + j); //Each gl texture number just increments by 1
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, mbPtr->texture[j]);
                //Link this texture to the shader
                glUniform1i(mbPtr->glsl.textureID[j], j);
            }
        }
        
        //Link vertices to shader
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, mbPtr->vbo);
        glVertexAttribPointer(
            0,                  // attribute
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        //Link texture coords to shader
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, mbPtr->vto);
        glVertexAttribPointer(
            1,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
       );
        
        //Link normals to shader
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, mbPtr->vno);
        glVertexAttribPointer(
            2,                                // attribute
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        //Bind the index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mbPtr->veo);
        
        // Draw the triangle !
        glDrawElements(GL_TRIANGLES, mbPtr->indices.size(), GL_UNSIGNED_SHORT, (void*)0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }

    //Update fps
    handleFPS();

    //Draw 2D-Text
    if (flags.showText){
        printText2D(fps.fpsC, 10, 10, 20);
    }
    
    //Swap buffers
    glutSwapBuffers();
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//Update Function
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void update(int a){

    //Handle keyboard and mouse events
    UINT16 retFlag = keyboardAndMouse();
    if (retFlag != 0){
        if ((retFlag & CONTROL_EXIT) == CONTROL_EXIT){ //Quit the program
            glutLeaveMainLoop(); //Only works with freeGlut
        }
        if ((retFlag & CONTROL_FPS) == CONTROL_FPS){ //Max out FPS
            if (flags.maxFPS) glutIdleFunc(draw);
            else glutIdleFunc(NULL);
        }
    }

    //Update the models
    for (UINT16 i = 0; i < NUM_MODELS; i++){
        modelPointer[i]->update(true);
    }

    //Update the viewMatrix
    if (cameraMode == FREE_CAM)
        viewMatrix = controlOutput.translation * controlOutput.rotation * viewMatrix;
    else if (cameraMode == SAT_CHASE){
        //We stick right behind the CubeSat with slight adjustment
        glm::vec3 a, b;
        a = glm::vec3(celestialSolid[CUBESAT]->getLMM_Translation() - celestialSolid[EARTH]->getLMM_Translation());
        b = get3D_ScaledVector(glm::cross(celestialSolid[CUBESAT]->getLMM_Up(),a), 250);
        viewMatrix = glm::lookAt((1.5f * a) + b + celestialSolid[EARTH]->getLMM_Translation() + glm::vec3(0, 150, 0), 
            (glm::vec3(celestialSolid[CUBESAT]->getLMM_Translation() - celestialSolid[EARTH]->getLMM_Translation()) * .75f) + glm::vec3(celestialSolid[EARTH]->getLMM_Translation()), 
            glm::vec3(0, 1, 0));
    }
    else if (cameraMode == EARTH_CHASE){
        glm::vec3 a, b;
        a = glm::vec3(celestialSolid[EARTH]->getLMM_Translation() - celestialSolid[SUN]->getLMM_Translation());
        b = get3D_ScaledVector(glm::cross(a, celestialSolid[EARTH]->getLMM_Up()), 1000);
        viewMatrix = glm::lookAt((1.01f * a) + b + celestialSolid[SUN]->getLMM_Translation() + glm::vec3(0, 1000, 0), glm::vec3(celestialSolid[EARTH]->getLMM_Translation()), glm::vec3(0, 1, 0));
    }
    else if (cameraMode == MOON_EARTH){
        viewMatrix = glm::lookAt((1.3f * (celestialSolid[MOON]->getLMM_Translation() - celestialSolid[EARTH]->getLMM_Translation())) + (glm::normalize(celestialSolid[MOON]->getLMM_Up()) * 200.0f) + celestialSolid[EARTH]->getLMM_Translation(),
            celestialSolid[EARTH]->getLMM_Translation(), glm::vec3(0, 1, 0));
    }
    else if (cameraMode == SUN_EARTH){
        viewMatrix = glm::lookAt((1.05f * (celestialSolid[SUN]->getLMM_Translation() - celestialSolid[EARTH]->getLMM_Translation())) + (glm::normalize(celestialSolid[SUN]->getLMM_Up()) * 1000.0f) + celestialSolid[EARTH]->getLMM_Translation(),
            celestialSolid[EARTH]->getLMM_Translation(), glm::vec3(0, 1, 0));
    }

    //If this is left blank we get a swoove effect
    resetControlOutMatrices();

    //Signal for the draw function
    glutPostRedisplay();

    //Schedule the next call to this function
    glutTimerFunc((UINT32)fps.interval, update, 0);

    return;
}