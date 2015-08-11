/* Created By: Jared Fowler , jared.fowler.379@my.csun.edu
   September 11, 2014
   Comp 465 "Warbird Simulation"
   Professor Barnes


ABOUT:

Helpful camera handler I created which enables 
both static and dynamic camera views.  Has the option
to link the camera to an Object3D and to follow that
object!  Also provides useful functions to enable 
free cam movement modes.

*/

# ifndef __INCLUDES465__
# include "../includes465/include465.hpp"
# define __INCLUDES465__
# endif

#ifndef CAMERAVIEW_HPP
#define CAMERAVIEW_HPP

#include <cmath>
#include "Object3D.hpp"
#include "TypeDefinesAndHelpers.hpp"

class CameraView
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
    char viewID[20];       //What is the name of this view point?
    Object3D* obj = NULL;  //Are we following an object?

    glm::vec3 eyeDynamic;  //Dynamic Eye View
    glm::vec3 atDynamic;   //Dynamic At View
    glm::vec3 upDynamic;   //Dynamic Up View

public:
    // Public Vars ###################################################
    //################################################################

    enum VIEW_TYPE{  //View Type Flags used to modify 'up'
        Back_Follow = 0, //Follow from behind
        Front_Follow,    //Follow from front
        Top_View,        //Looking down on the object   
        Bottom_View,     //Looking up at the object
        Free_View,       //Free Cam
        NOT_VALID
    } VIEW_FLAG;

    // Constructor(s) ################################################
    //################################################################
    CameraView(char* viewId, glm::vec3 eye, glm::vec3 at, glm::vec3 up, BYTE viewFlag, Object3D* objToFollow = NULL)
    {
        setViewId(viewId);
        setEye(eye);
        setAt(at);
        setUp(up);
        setViewFlag(viewFlag);
        setObj(objToFollow);
    }

    CameraView(){  //Default Constructor (Static Camera)

        setViewId("UnNamed");
        setEye(glm::vec3(0, 0, 0));
        setAt(glm::vec3(0, 0, 100));
        setUp(glm::vec3(0, 1, 0));
    }

    // gets and sets #################################################
    //################################################################
    inline char* getViewId(void){ return this->viewID; }
    inline bool setViewId(char* id){ strcpy(this->viewID, id); return true; }

    inline glm::vec3 getEye(void){ return this->eye; }
    inline bool setEye(glm::vec3 e){ this->eye = e; return true; }

    inline glm::vec3 getAt(void){ return this->at; }
    inline bool setAt(glm::vec3 a){ this->at = a; return true; }

    inline glm::vec3 getUp(void){ return this->up; }
    inline bool setUp(glm::vec3 u){ this->up = u; return true; }

    inline Object3D* getObj(void){ return this->obj; }
    inline bool setObj(Object3D* o){ this->obj = o; return true; }

    inline BYTE getViewFlag(void){ return this->VIEW_FLAG; }
    inline bool setViewFlag(BYTE f){
        //Valid input?
        if (f >= NOT_VALID)
            return false;

        VIEW_FLAG = (VIEW_TYPE)f;
        return true;
    }


    /*Using the three vertices, we can use the glm library to get the lookAt matrix.
    The function will operate depending if the view is static or dynamic.  This check
    is made by seeing if "obj" points to any object.  If it does, we will first need
    to get the location of the object, update the vectors, and then return the lookAt.
    Also note that there are several flags which can be set (eg. topView, behindView,
    etc. which can also affect the 'up' and 'eye' vertices.*/
    glm::mat4 getLookAt(void);


    /*This is a helper function that should be used with a CameraView object.  The primary use will
    be with the intention of moving a camera foward or backwards.  Arguments should consist of the
    cameraView object pointer, and a distance to move.  Depending on the flag variable, this function
    will automatically update the eye and at vectors.  In any case, a vector which represents the
    distance-view will be returned.*/
    static glm::vec3 moveForwardOrBackwards(CameraView* obj, float distance, bool update);


    /*This is a helper function that should be used with a CameraView object.  The primary use will
    be with the intention of turing a camera angle right or left.  User passes in the camerview object.
    Based upon the boolean flag, these vectors may or may not be automatically updated.  In either case,
    a vector representing what is added to at will be returned. Note: Positive radian represents a left turn,
    while negative is right turn.  */
    static glm::vec3 turnRightOrLeft(CameraView* obj, float radians, bool update);

    /*This is a helper function that should be used with a CameraView object.  The primary use will
    be with the intention of turing a camera pitch up or down.  User passes in the camerview object.
    Based upon the boolean flag, these vectors may or may not be automatically updated.  In either case,
    a vector representing what is added to at will be returned.*/
    static glm::vec3 pitchUpOrDown(CameraView* obj, float radians, bool update);

};

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
glm::mat4 CameraView::getLookAt(void){

    if (obj != NULL) //Dynamic Vew... Do some stuff first
    {
        //Get a copy of the model matrix from object we are following
        glm::mat4 modelMatrix = obj->getModelMatrix();
        //Get the right vector to the object
        glm::vec3 rightVec(modelMatrix[0][0], modelMatrix[0][1], modelMatrix[0][2]);
        //Get the up vector of the object
        glm::vec3 upVec(modelMatrix[1][0], modelMatrix[1][1], modelMatrix[1][2]);
        //Get the at vector of the object
        glm::vec3 atVec(modelMatrix[2][0], modelMatrix[2][1], modelMatrix[2][2]);
        //Get the pos vector of the object
        glm::vec3 posVec(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

        //We should be looking at the object that we are following (at = modelPosition)
        this->atDynamic = posVec;

        //This cameras 'at' vector should be added as an offset. Note: at should be along objects up vector
        this->atDynamic += MathHelperJWF::get3D_ScaledVector(upVec, glm::distance(glm::vec3(0,0,0), at));


        /*The 'eye' and 'up' vectors are dependent upon what type of dynamic view we want*/
        if (VIEW_FLAG == Top_View || VIEW_FLAG == Bottom_View){

            /*The general idea of top view is to have the cameras eye serve as an offset
            of the model's current position.  The 'up' vector will reflect the direcitonal
            vector of the model.*/

            //Store new 'Dynamic Eye' position
            this->eyeDynamic = posVec;

            //Use 'eye' as an offset to dynamic eye
            this->eyeDynamic += this->eye;

            /*The 'up' vector is what the model is looking at
            Note: Depending on rotation and other factors, 
            the at vector might represent the opposite 
            direction that the object is facing. */
            this->upDynamic = atVec;

        }
        else if (VIEW_FLAG == Front_Follow || VIEW_FLAG == Back_Follow){

            /*The up vector will mimic that of the object's. The 
            eye vector will be offset by a mixture of the objects
            position, the cameras translation, and the model's
            at vector.  The at vector basically acts as our new
            z axis.*/

            //Copy the 'up' vector
            upDynamic = upVec;

            //Step 1: Get the object's z-axis offset (along its 'at' vector)
            glm::vec3 zOffset = MathHelperJWF::get3D_ScaledVector(atVec, this->eye[2]);
            //Step 2: Get the object's y-axis offset (along its 'up' vector)
            glm::vec3 yOffset = MathHelperJWF::get3D_ScaledVector(upVec, this->eye[1]);
            //Step 3: Get the object's x-axis offset (along its 'right' vector)
            glm::vec3 xOffset = MathHelperJWF::get3D_ScaledVector(rightVec, this->eye[0]);

            //Step 4: Add these offsets to the models position, resulting in the eye vector
            if (VIEW_FLAG == Front_Follow)
                eyeDynamic = zOffset + yOffset + xOffset + posVec;
            else
                eyeDynamic = -zOffset + yOffset + xOffset + posVec;

        }

        //Return the lookAt Matrix
        return glm::lookAt(eyeDynamic, atDynamic, upDynamic);

    }

    //Else if static view, return set lookAt matrix
    glm::mat4 temp = glm::lookAt(eye, at, up);
    return temp;

}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
glm::vec3 CameraView::moveForwardOrBackwards(CameraView* obj, float distance, bool update){

    //Find the desired directional vector. Remember 'at' is more of a position, not a vector
    glm::vec3 directionalVector = obj->getAt() - obj->getEye();

    //Use a helper function to obtain a vector that reflects the distance
    glm::vec3 allPositive = MathHelperJWF::get3D_ScaledVector(directionalVector, distance);

    //Depending on the boolean flag, update the passed in vectors
    if (update){
        if (distance < 0){  //Move camera back
            obj->setEye(obj->getEye() - allPositive);
            obj->setAt(obj->getAt() - allPositive);
        }
        else{  //Move camera forward
            obj->setEye(obj->getEye() + allPositive);
            obj->setAt(obj->getAt() + allPositive);
        }
    }

    return allPositive;
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
glm::vec3 CameraView::turnRightOrLeft(CameraView* obj, float radians, bool update)
{
    //If radians is 0, we are done
    if (radians == 0)
        return glm::vec3(0, 0, 0);

    //Get copies for convenience
    glm::vec3 eye = obj->getEye();
    glm::vec3 at = obj->getAt();

    glm::mat4 returnM;

    //Translate out to cameras current location
    returnM *= glm::translate(glm::mat4(), glm::vec3(eye[0], eye[1], eye[2]));

    //Rotate by the user defined number of radians
    returnM *= glm::rotate(glm::mat4(), glm::radians(radians), obj->getUp());

    //Translate out based upon original eye to at vector, thus forming an orbit effect
    returnM *= glm::translate(glm::mat4(), (at - eye));

    //returnM now represents where we want to look at, so update 'at'
    at[0] = returnM[3][0];
    at[1] = returnM[3][1];
    at[2] = returnM[3][2];

    obj->setAt(at);


    //Return a copy of the vertex
    return obj->at;

}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
glm::vec3 CameraView::pitchUpOrDown(CameraView* obj, float radians, bool update){

    //TODO: Impliment

    return glm::vec3();
}


#endif