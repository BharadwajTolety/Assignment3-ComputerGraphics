#include "DroneLight.h"

DroneLight::DroneLight()
    :m_Light(nullptr)
{
    m_Light = new MLight();
}

DroneLight::~DroneLight()
{
    if (m_Light) delete m_Light;
}
