#ifndef KEYCTRLCAMERA_H
#define KEYCTRLCAMERA_H

#include "Camera.h"
#include "Controller.h"

class KeyCtrlCamera: public Camera
{
public:
    KeyCtrlCamera(Controller* _ctrl);
    ~KeyCtrlCamera();
    void Update();
    void KeyUpdate();

public:
    inline void SetMoveSpeed(const float& _ms) { m_MoveSpeed = _ms; }
    inline void SetRotSpeed(const float& _rs) { m_RotSpeed = glm::radians(_rs); }
    inline float GetMoveSpeed() const { return m_MoveSpeed; }
    inline float GetRotSpeed() const { return m_RotSpeed; }

private:
    Controller* m_Ctrl;
    float m_MoveSpeed;
    float m_RotSpeed;
};

#endif // KEYCTRLCAMERA_H
