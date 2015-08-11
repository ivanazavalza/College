/* Created By: Jared Fowler , jared.fowler.379@my.csun.edu
   September 11, 2014
   Professor Barnes


ABOUT:

Child class of Object3D.  Includes functionality such as 
rotation, orbits, etc.  Includes functionality to transition
origin from natural origin to parent location origin.

This class referrs to objects such as planets, moons, or basic
satelites.

*/

#ifndef CELESTIALBODYSOLID3D_HPP
#define CELESTIALBODYSOLID3D_HPP


#include "Object3D.hpp"
#define PI  (3.14159f)
#define PI2 (6.28318f)

class CelestialBodySolid3D : public Object3D
{

private:
    // Constants #####################################################
    //################################################################
        //Void

    // Variables #####################################################
    //################################################################
    float orbitSpeed = 0;
    float rotationSpeed = 0;

    //Keep these as pointers, as axis may change...
    glm::vec3 axisRot;
    glm::vec3 axisOrbit;

    CelestialBodySolid3D* cb3D = NULL;  //Pointer to associated Model (Parent)

    //These values will help prevent large value matrices
    float radiansOrbit = 0;
    float radiansRotation = 0;

public:
    // Constructor(s) ################################################
    //################################################################
    CelestialBodySolid3D(bool active = true, CelestialBodySolid3D* cb3D = NULL, 
                         float orbitSpeed = 0, float rotationSpeed = 0,
                         glm::mat4 scaleMatrix = glm::mat4(),
                         glm::mat4 translationMatrix = glm::mat4(),
                         glm::mat4 rotationalMatrix_0 = glm::mat4(),
                         glm::mat4 rotationalMatrix_1 = glm::mat4(),
                         glm::vec3 axisRotation = glm::vec3(0, 1, 0), glm::vec3 axisOrbit = glm::vec3(0, 1, 0))

        //Call the parent constructor
        : Object3D(active, scaleMatrix, translationMatrix, rotationalMatrix_0, rotationalMatrix_1)
    {
        setOrbitSpeed(orbitSpeed);
        setRotationSpeed(rotationSpeed);
        setParentObj(cb3D);
        setAxisOrbit(axisOrbit);
        setAxisRotation(axisRotation);

        //Invoke the objects first update to get it situated
        this->getModelMatrix();
    }

    // gets and sets #################################################
    //################################################################
    inline float getOrbitSpeed(void){ return this->orbitSpeed; }
    inline bool setOrbitSpeed(float r){ this->orbitSpeed = r; return true; }

    inline float getRotationSpeed(void){ return this->rotationSpeed; }
    inline bool setRotationSpeed(float r){ this->rotationSpeed = r; return true; }

    inline bool setParentObj(CelestialBodySolid3D* p){ this->cb3D = p; return true; }
    inline CelestialBodySolid3D* getParentObj(void){ return this->cb3D; }

    inline bool setAxisRotation(glm::vec3 axis){ this->axisRot = axis; return true; }
    inline glm::vec3 getAxisRotation(void){ return this->axisRot; }

    inline bool setAxisOrbit(glm::vec3 axis){ this->axisOrbit = axis; return true; }
    inline glm::vec3 getAxisOrbit(void){ return this->axisOrbit; }

    /*Using the rotation, translation, and scale Matrices, returns an appropiate model matrix.
    This returned matrix will probably be multiplied with the view and projection matrices in
    order to get the final coordinates for the model.*/
    glm::mat4 getModelMatrix(void);

    /*This object has the ability to update itself based upon variable values
    that we set such as radians, translation, etc. The user can choose to update all
    objects whether active or not (true), or just to update the active ojbects (false)*/
    void update(bool updateIfInactive);


};

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
glm::mat4 CelestialBodySolid3D::getModelMatrix(void){

    /*We essentially only have the matrices provided by the parent class to keep track of the original positions
    of the models. We handle rotations and tranlation as a continuous matter. Angles of rotation rae kept under
    control, hence, we also keep the size of matrices under control.*/

    //Remember, the procedure below is applied in reverse order
    //Final return value initialized to identity matrix
    glm::mat4 returnM(1.0f);

    //Translate out to parent if it has one
    if (this->getParentObj() != NULL)
        returnM *= glm::translate(glm::mat4(), this->getParentObj()->getLMM_Translation());

    //Orbital -- This order matters... 
    returnM *= this->getRotationMatrix_1() * glm::rotate(glm::mat4(), this->radiansOrbit, this->getAxisOrbit());

    //Translation
    returnM *= this->getTranslationMatrix();

    //Rotation -- This order matters... Spin on axis (y-axis), then rotate to axis
    returnM *= this->getRotationMatrix_0() * glm::rotate(glm::mat4(), this->radiansRotation, this->getAxisRotation());
    
    //Scale
    returnM *= this->getScaleMatrix();

    //Back up our work
    this->setLastModelMatrix(returnM);

    //Return the Model Matrix
    return returnM;

}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void CelestialBodySolid3D::update(bool updateIfInactive) {

    //Is the fucntion active or is user by-passing the check?
    if (!(updateIfInactive || this->active))
        return;

    //Update the rotation and spin values
    this->radiansRotation += this->getRotationSpeed();
    this->radiansOrbit += this->getOrbitSpeed();
    if (this->radiansRotation > PI2)
        this->radiansRotation = this->radiansRotation - ((UINT16)(this->radiansRotation / PI2) * PI2);
    if (this->radiansOrbit > PI2)
        this->radiansOrbit = this->radiansOrbit - ((UINT16)(this->radiansOrbit / PI2) * PI2);

    //Update the lastModelMatrix
    this->getModelMatrix();
}

#endif