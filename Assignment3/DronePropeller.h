#ifndef DRONEPROPELLER_H
#define DRONEPROPELLER_H

#include "GameObject.h"
#include "glm/glm.hpp"

/*Sets rotation speed for the propeller and also inherits from class GameObjects.*/
class DronePropeller: public GameObject
{
public:
    DronePropeller();
    ~DronePropeller();
    void Update() override;
private:
    float m_RotSpeed;
};

#endif // DRONEPROPELLER_H
