/*Created By: Jared Fowler
**April 7, 2015
**
**Contains:
**Common #includes we need for openGL
**Common #defines
**Common structs
**Common typedefs
**
**Assumes that we are running MS Windows
**
*/

#ifndef JWF_INCLUDES_HPP
#define JWF_INCLUDES_HPP

//INCLUDES****************************************
//************************************************
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <vector>

#include <Windows.h>
#include <wtypes.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//openGL function helpers
#include "shader.hpp"
#include "texture.hpp"
#include "objloader.hpp"
#include "controls.hpp"
#include "vboindexer.hpp"
#include "text2D.hpp"
#include "Object3D.hpp"
#include "CelestialBodySolid3D.hpp"
//#include "SattelliteCamera.hpp"

//DEFINES*****************************************
//************************************************
#define PI                  (3.14159f)
#define MAX_TEXTURES        3
#define MAX_LIGHTS          5

//STRUCTS*****************************************
//************************************************
struct ShaderHandles {
    GLuint shader    = 0;
    GLuint matrixID  = 0;
    GLuint viewMatrixID = 0;
    GLuint modelMatrixID = 0;
    GLuint modelViewMatrixID = 0;
    GLuint depthBiasMVPID = 0;
    GLuint shadowMapID = 0;
    GLuint lightID[MAX_LIGHTS];
    GLuint textureID[MAX_TEXTURES];
};

struct ModelBuffer {
    std::vector<glm::vec3> vertexBuffer;
    std::vector<glm::vec2> textureBuffer;
    std::vector<glm::vec3> normalBuffer;
    std::vector<UINT16> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_texture;
    std::vector<glm::vec3> indexed_normals;

    GLuint texture[MAX_TEXTURES];
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint vno = 0;
    GLuint vto = 0;
    GLuint veo = 0;

    ShaderHandles glsl;
};

struct ModelInfo {
    const char* modelFileName;
    const char* textureFileName[MAX_TEXTURES];
    const char* vertexShaderFileName;
    const char* fragShaderFileName;
};

struct ShadowMapping {
    GLuint shader = 0;
    GLuint depthMatrixID = 0;
    GLuint FramebufferName = 0;
    GLuint depthTexture = 0;
};

//TYPEDEFS****************************************
//************************************************
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef unsigned long long  UINT64;
typedef short               INT16;
typedef int                 INT32;
typedef long long           INT64;

//FUNCTIONS***************************************
//************************************************
void initModelBufferStruct(ModelBuffer* mbPtr){
    UINT8* ptr = (UINT8*)mbPtr;
    for (UINT16 i = 0; i < sizeof(ModelBuffer); i++){
        ptr[i] = 0;
    }
}

static glm::vec3 get3D_ScaledVector(glm::vec3 directionalVector, float distance){

    /*This function will take a 3D vector, along with a distance and return an equivalant vector which points
    in the same direction but is scaled to reflect the distance it would cover if the vector was treated as a point
    from the origin.

    Theory:

    d^2 = x^2 + y^2 + z^2

    Lets say that x is the smallest:

    d^2 = 1x^2 + (nx)^2 + (mx)^2
    = (x^2)(1 + n^2 + m^2)

    d / ( (1 + n^2 + m^2)^.5) = x

    Multiply x by original vector values to obtain proper magnitude.
    */

    //Make a copy of the above vector, but make all the values positive
    glm::vec3 allPositive;
    allPositive[0] = (directionalVector[0] < 0) ? -(directionalVector[0]) : directionalVector[0];
    allPositive[1] = (directionalVector[1] < 0) ? -(directionalVector[1]) : directionalVector[1];
    allPositive[2] = (directionalVector[2] < 0) ? -(directionalVector[2]) : directionalVector[2];

    /*We know the distance can be fitted into this equation:  d^2 = x^2 + y^2 + z^2
    We'll have to put two of the variables in perpective with one variable, and then solve.*/

    //Begin by finding out which of x,y,z on the directional vector is the smallest.
    //Warning: we want absolute values, not negative ones!
    //Warning..we don't care about zeros!,, in fact, if not done right, they could cause errors.

    //All values are zero?
    if (allPositive[0] == 0 && allPositive[1] == 0 && allPositive[2] == 0)
        return glm::vec3(0, 0, 0);

    //Find the smallest non-zero value
    float smallest = std::numeric_limits<float>::infinity();
    if (allPositive[0] < smallest  && allPositive[0] != 0)
        smallest = allPositive[0];
    if (allPositive[1] < smallest  && allPositive[1] != 0)
        smallest = allPositive[1];
    if (allPositive[2] < smallest && allPositive[2] != 0)
        smallest = allPositive[2];

    //Now divide the values by the smallest to get a reduced ratio
    allPositive[0] /= smallest;
    allPositive[1] /= smallest;
    allPositive[2] /= smallest;

    //We'll need to square each of these values now and store the sum
    float sum = (float)((pow(allPositive[0], 2)) + (pow(allPositive[1], 2)) + (pow(allPositive[2], 2)));

    //Solve for common variable 'a'
    float var = (float)(abs(distance) / (pow(sum, .5)));

    //We can now multiply this var to directional vector to get distance-directional vector
    allPositive[0] *= var;
    allPositive[1] *= var;
    allPositive[2] *= var;

    //Revert back to both positive and negative values
    if (directionalVector[0] < 0)
        allPositive[0] *= -1;
    if (directionalVector[1] < 0)
        allPositive[1] *= -1;
    if (directionalVector[2] < 0)
        allPositive[2] *= -1;

    //We now have our completed vector, so return it to the caller
    return allPositive;

}

#endif