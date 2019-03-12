#ifndef DRONELIGHT_H
#define DRONELIGHT_H

#include "GameObject.h"
#include "MLight.h"

class DroneLight: public GameObject
{
public:
    DroneLight();
    ~DroneLight();
private:
    MLight *m_Light;
};

#endif // DRONELIGHT_H
