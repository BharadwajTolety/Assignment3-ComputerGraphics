#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "GameObject.h"

/*This class is a child of class "Gameobject". It is used to update the camera when user updates its positions using the arrow keys.
It updates the prjection matrices for the camera using SetProjMat, GetProjMat and GetViewMat, sets camera using the settomaincamera and zooms using GetFOV function.*/

class Camera: public GameObject
{
public:
    static Camera* s_MainCamera;
public:
    static glm::mat4 s_DefaultProjMat;
    static glm::mat4 s_PerspectiveProjMat;
    static glm::mat4 s_OrthoProjMat;
public:
    Camera();
    virtual void Update();

public:
    inline glm::mat4 GetViewMat() const { return m_View; }
    inline glm::mat4 GetProjMat() const { return m_Proj; }
    inline void SetProjMat(const glm::mat4& _proj) { m_Proj = _proj; }
    inline float GetFOV() const { return m_FOV; }
    inline void SetToMainCamera() { s_MainCamera = this; }
protected:
    void m_ViewMatUpdate();
    void m_ProjMatUpdate();

protected:
    glm::mat4 m_View;
    glm::mat4 m_Proj;
    float m_Pitch;
    float m_Yaw;
    float m_Roll;
    float m_FOV;
};

#endif // CAMERA_H
