#ifndef DRONELIGHT_H
#define DRONELIGHT_H

#include "GameObject.h"
#include "MLight.h"

/*This class will be used in the future to implement the lights for the assignment. */

class DroneLight: public GameObject
{
public:
    DroneLight();
    ~DroneLight();
private:
    MLight *m_Light;
};

#endif // DRONELIGHT_H
