/* Created By: Jared Fowler , jared.fowler.379@my.csun.edu
   April 10, 2015

ABOUT:
I'm making this class special to work with a solar system
type enviroment. Chase vectors are an equation of both the 
chased object and its parent. There will be several camera
modes available.
I will also add functionality to teleport above a model.

*/

#ifndef SATTELLITECAMERA_HPP
#define SATTELLITECAMERA_HPP

#include <cmath>
#include "CelestialBodySolid3D.hpp"

class SattelliteCamera
{
private:
    // Constants #####################################################
    //################################################################
    //Void

    // Variables #####################################################
    //################################################################
    //Note, the following variables are what glm::lookAt uses in order
    //to get our viewMatrix
    glm::vec3 eye;         //Where are we standing?
    glm::vec3 at;          //Towards what point are we looking?
    glm::vec3 up;          //What is considered up? (Normally (0,1,0))
    CelestialBodySolid3D* obj = NULL;  //Are we following an object?

    UINT16 flags = 0;

public:
    // Public Vars ###################################################
    //################################################################

    enum VIEW_TYPE{  //View Type Flags used to modify 'up'
        VIEW_CHASE = 0x01,           //Have the look at vector towards the object
        SNGL_VEC_POS_CHASE = 0x02,   //Chase object based on single vector
        DBL_VEC_CHASE_BEHIND = 0x04, //Based on obj position, parent position, and vector
        DBL_VEC_CHASE_AHEAD = 0x08,  //Looking up at the object
        FREE_VIEW           = 0x10,  //Free camera mode
    } VIEW_FLAG;

    // Constructor(s) ################################################
    //################################################################
    SattelliteCamera(glm::vec3 eye, glm::vec3 at, glm::vec3 up, UINT16 viewFlag, CelestialBodySolid3D* objToFollow = NULL)
    {
        setEye(eye);
        setAt(at);
        setUp(up);
        setViewFlag(viewFlag);
        setObj(objToFollow);

    }

    // gets and sets #################################################
    //################################################################
    inline glm::vec3 getEye(void){ return this->eye; }
    inline bool setEye(glm::vec3 e){ this->eye = e; return true; }

    inline glm::vec3 getAt(void){ return this->at; }
    inline bool setAt(glm::vec3 a){ this->at = a; return true; }

    inline glm::vec3 getUp(void){ return this->up; }
    inline bool setUp(glm::vec3 u){ this->up = u; return true; }

    inline Object3D* getObj(void){ return this->obj; }
    inline bool setObj(CelestialBodySolid3D* o){ this->obj = o; return true; }

    inline UINT16 getViewFlag(void){ return this->flags; }
    inline void setViewFlag(UINT16 f){ this->flags = f; }

    /*Using the three vertices, we can use the glm library to get the lookAt matrix.
    The function will operate depending if the view is static or dynamic.  This check
    is made by seeing if "obj" points to any object.  If it does, we will first need
    to get the location of the object, update the vectors, and then return the lookAt.
    Also note that there are several flags which can be set (eg. topView, behindView,
    etc. which can also affect the 'up' and 'eye' vertices.*/
    glm::mat4 getLookAt(void);

};

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
glm::mat4 SattelliteCamera::getLookAt(void){

    //Are we in free view mode?
    if ((flags & FREE_VIEW) == FREE_VIEW){
        return glm::mat4();
    }
    //Is the camera facing an object?
    if ((flags & VIEW_CHASE) == VIEW_CHASE){
        this->setAt(this->obj->getLMM_Translation());
    }
    //Are we following off a single vector?
    if ((flags & SNGL_VEC_POS_CHASE) == SNGL_VEC_POS_CHASE){
        //Base the single vector off the position vector
        return glm::lookAt(this->eye + this->obj->getLMM_Translation(), this->at, this->up);
    }



    return glm::mat4();


}


#endif