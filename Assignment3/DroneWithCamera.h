#ifndef DRONEWITHCAMERA_H
#define DRONEWITHCAMERA_H

#include "Drone.h"
#include "KeyCtrlCamera.h"

class DroneWithCamera: public Drone
{
public:
    DroneWithCamera(Controller* _ctrl);

public:
    inline void SetCamera(Camera* _cam) { m_Camera = _cam; }
    inline Camera* GetCamera() const { return m_Camera; }

private:
    Camera* m_Camera;
};

#endif // DRONEWITHCAMERA_H
