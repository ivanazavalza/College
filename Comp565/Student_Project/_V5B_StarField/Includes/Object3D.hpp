/*Created By: Jared Fowler , jared.fowler.379@my.csun.edu
**September 11, 2014
**
**ABOUT:
** Simple container for an objects matrices and whether or not it is active
** Matrices will be applied in this order: scale rotM_0 translate rotM_1
*/


#ifndef OBJECT3D_HPP
#define OBJECT3D_HPP

#include <glm/glm.hpp>

class Object3D
{
public:
    enum Config_Flags{
        BACKGROUND_MODEL = 0x01,
        NON_SHADOW_MAP   = 0x02,
    };

protected:
    // Constants #####################################################
    //################################################################
    //Void

    // Variables #####################################################
    //################################################################
    bool active        = true;    //Do we want to utilize/update/etc this model?
    glm::mat4 rotationMatrix_0;
    glm::mat4 rotationMatrix_1;
    glm::mat4 scaleMatrix;
    glm::mat4 translationMatrix;

    glm::mat4 lastModelMatrix;   //Backs up the last model matrix we worked hard to find

    UINT32 configFlags = 0;

public:
    // Constructor(s) ################################################
    //################################################################
    Object3D(bool active = true,
             glm::mat4 scaleMatrix       = glm::mat4(),
             glm::mat4 translationMatrix = glm::mat4(),
             glm::mat4 rotationalMatrix_0 = glm::mat4(),
             glm::mat4 rotationalMatrix_1 = glm::mat4()){

        setActive(active);
        setRotationMatrix_0(rotationalMatrix_0);
        setRotationMatrix_1(rotationalMatrix_1);
        setScaleMatrix(scaleMatrix);
        setTranslationMatrix(translationMatrix);

        setLastModelMatrix(getModelMatrix());
    }

    // gets and sets #################################################
    //################################################################
    inline bool isActive(void){ return this->active; }
    inline void setActive(bool b){ this->active = b; }

    inline glm::mat4 getRotationMatrix_0(void){ return this->rotationMatrix_0; }
    inline bool setRotationMatrix_0(glm::mat4 rm){ this->rotationMatrix_0 = rm; return true; }

    inline glm::mat4 getRotationMatrix_1(void){ return this->rotationMatrix_1; }
    inline bool setRotationMatrix_1(glm::mat4 rm){ this->rotationMatrix_1 = rm; return true; }

    inline glm::mat4 getScaleMatrix(void){ return this->scaleMatrix; }
    inline bool setScaleMatrix(glm::mat4 rm){ this->scaleMatrix = rm; return true; }

    inline glm::mat4 getTranslationMatrix(void){ return this->translationMatrix; }
    inline bool setTranslationMatrix(glm::mat4 rm){ this->translationMatrix = rm; return true; }

    inline glm::mat4 getLastModelMatrix(void){ return this->lastModelMatrix; }
    inline bool setLastModelMatrix(glm::mat4 m){ this->lastModelMatrix = m; return true; }

    inline glm::vec3 getLMM_Translation(void){ return glm::vec3(this->lastModelMatrix[3][0], this->lastModelMatrix[3][1], this->lastModelMatrix[3][2]); }
    inline glm::vec3 getLMM_Right(void){ return glm::vec3(this->lastModelMatrix[0][0], this->lastModelMatrix[0][1], this->lastModelMatrix[0][2]); }
    inline glm::vec3 getLMM_Up(void){ return glm::vec3(this->lastModelMatrix[1][0], this->lastModelMatrix[1][1], this->lastModelMatrix[1][2]); }
    inline glm::vec3 getLMM_At(void){ return glm::vec3(this->lastModelMatrix[2][0], this->lastModelMatrix[2][1], this->lastModelMatrix[2][2]); }

    inline glm::vec3* getLMM_RightPtr(void){ return (glm::vec3*)(&(this->lastModelMatrix[0])); }
    inline glm::vec3* getLMM_UpPtr(void){ return (glm::vec3*)(&(this->lastModelMatrix[1])); }
    inline glm::vec3* getLMM_AtPtr(void){ return (glm::vec3*)(&(this->lastModelMatrix[2])); }
    inline glm::vec3* getLMM_TranslationPtr(void){ return (glm::vec3*)(&(this->lastModelMatrix[3])); }

    inline UINT32 getConfigurationFlags(void){ return this->configFlags; }
    inline bool setConfigurationFlags(UINT32 f){ this->configFlags = f; return true; }

    virtual glm::mat4 getModelMatrix(void){/*Let future children impliment this*/
        return (this->getRotationMatrix_1() * this->getTranslationMatrix() * this->getRotationMatrix_0() * this->getScaleMatrix());
    }

    virtual void update(bool updateIfInactive){/*Let future children impliment this*/
        return;
    }
};

#endif