#include "RevolveCamera.h"
#include "glm/gtc/matrix_transform.hpp"

RevolveCamera::RevolveCamera(const glm::vec3& _pivot, const float& _speed, const float& _radius)
    : Camera(), m_Pivot(_pivot), m_RevolveSpeed(glm::radians(_speed)), m_RevolveRadius(_radius)
{
}

RevolveCamera::RevolveCamera()
    : m_Pivot(glm::vec3(0.0f, 1.0f, 0.0f)), m_RevolveSpeed(1.0f), m_RevolveRadius(80.0f)
{}

void RevolveCamera::Update()
{
    Revolve();
    m_ProjMatUpdate();
}

void RevolveCamera::Revolve()
{
    GetTransform()->SetPosition(glm::vec3(glm::rotate(glm::mat4(1.0f), m_RevolveSpeed, m_Pivot) * glm::vec4(GetTransform()->GetPosition(),1.0f)));
    GetTransform()->SetDirection(glm::vec3(0.0f, GetTransform()->GetPosition().y, 0.0f) - GetTransform()->GetPosition());

    m_View = glm::lookAt(GetTransform()->GetPosition(), GetTransform()->GetPosition() + GetTransform()->GetForward(), Transform::Up);
}
