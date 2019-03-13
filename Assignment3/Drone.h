#ifndef DRONE_H
#define DRONE_H

#include "GameObject.h"
#include "Controller.h"

/*Inherits from GameObject class and sets speed for rotation and movement for the drone.*/

class Drone: public GameObject
{
public:
    Drone(Controller* _ctrl);
    void Update() override;

public:
    inline void SetRotSpeed(const float& _rs) { m_RotSpeed = _rs; }
    inline void SetMoveSpeed(const float& _ms) { m_MoveSpeed = _ms; }
    inline float GetRotSpeed() const { return m_RotSpeed;}
    inline float GetMoveSpeed() const { return m_MoveSpeed; }
private:
    void KeyUpdate();
private:
    Controller* m_Ctrl;
    float m_RotSpeed;
    float m_MoveSpeed;
};

#endif // DRONE_H
