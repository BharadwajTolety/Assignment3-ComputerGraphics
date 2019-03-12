#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

Camera* Camera::s_MainCamera = nullptr;

glm::mat4 Camera::s_DefaultProjMat = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.01f, 100.0f);
glm::mat4 Camera::s_OrthoProjMat = glm::ortho(-540.0f, 540.0f, -360.0f, 360.0f, -5.0f, 5.0f);
glm::mat4 Camera::s_PerspectiveProjMat = glm::perspective(glm::radians(120.0f), 1.0f, 0.1f, 10000.0f);

Camera::Camera()
    : GameObject ("Camera"), m_View(), m_Proj(s_DefaultProjMat), m_Pitch(0.0f), m_Yaw(0.0f), m_Roll(0.0f), m_FOV(120.0f)
{
    GetTransform()->SetDirection(glm::vec3(0.0f, 0.0f, -1.0f));
    if (Camera::s_MainCamera == nullptr)
    {
        s_MainCamera = this;
    }
}

void Camera::Update()
{
    m_ViewMatUpdate();
    m_ProjMatUpdate();
}

void Camera::m_ViewMatUpdate()
{
    auto tf = GetTransform();
    glm::vec3 pos = tf->GetPosition();
    glm::vec3 forward = tf->GetForward();
    m_View = glm::lookAt(pos, pos + forward, Transform::Up);
}
void Camera::m_ProjMatUpdate()
{
    m_Proj = glm::perspective(glm::radians(m_FOV), 1.0f, 0.1f, 10000.0f);;
}
