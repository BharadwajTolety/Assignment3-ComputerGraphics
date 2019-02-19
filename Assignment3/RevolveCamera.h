#ifndef REVOLVECAMERA_H
#define REVOLVECAMERA_H

#include "Camera.h"

class RevolveCamera: public Camera
{
public:
    RevolveCamera();
    RevolveCamera(const glm::vec3& _pivot, const float& _speed, const float& _raduis);

public:
    void Update() override;

public:
    inline glm::vec3 GetPivot() const { return m_Pivot; }
    inline void SetPivot(const glm::vec3& _p) { m_Pivot = _p;}
    inline float GetRevolveSpeed() const {return m_RevolveSpeed;}
    inline void SetRevolveSpeed(const float& _s) { m_RevolveSpeed = _s; }

private:
    void Revolve();
private:
    glm::vec3 m_Pivot;
    float m_RevolveSpeed;
    float m_RevolveRadius;
};

#endif // REVOLVECAMERA_H
