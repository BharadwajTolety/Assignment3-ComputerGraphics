#include "DronePropeller.h"
#include "glm/gtc/matrix_transform.hpp"

DronePropeller::DronePropeller()
    :m_RotSpeed(glm::radians(5.0f))
{

}

DronePropeller::~DronePropeller()
{

}

void DronePropeller::Update()
{
    Transform* tf = GetTransform();
    tf->UpRotate(m_RotSpeed);
//    tf->DirRotate(m_RotSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
}
