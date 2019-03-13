#ifndef CAMERASWITCHCER_H
#define CAMERASWITCHCER_H

#include "Camera.h"

/*Used to switch cameras between drone and global.*/

class CameraSwitchcer
{
public:
    CameraSwitchcer(Camera* _c1, Camera* _c2);
    void Update();
public:
    Camera* GetCam1() const { return m_Camera1; }
    Camera* GetCam2() const { return m_Camera2; }
private:
    Camera* m_Camera1;
    Camera* m_Camera2;
};

#endif // CAMERASWITCHCER_H
