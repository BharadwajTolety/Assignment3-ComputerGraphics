#include "Transform.h"
#include "glm/gtc/matrix_transform.hpp"
glm::vec3 Transform::Up = glm::vec3(0.0f, 1.0f, 0.0f);

Transform::Transform()
    : m_Position(0.0f, 0.0f, 0.0f), m_Rotation(0.0f, 0.0f, 0.0f, 0.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Direction(0.0f, 0.0f, 1.0f)
{}

Transform::Transform(const glm::vec3& _pos, const glm::vec4& _rot, const glm::vec3& _scale, const glm::vec3& _dir)
    : m_Position(_pos), m_Rotation(_rot), m_Scale(_scale), m_Direction(_dir)
{}

glm::mat4 Transform::GetModelTransformMat() const
{
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);
    glm::mat4 rot = (m_Rotation.w == 0.0f)? glm::mat4(1.0f): glm::rotate(glm::mat4(1.0f), m_Rotation.w, glm::vec3(m_Rotation));
//    if (m_Rotation.w != 0.0f)
//    {
//        glm::vec3 newdir = glm::vec3(glm::rotate(glm::mat4(1.0f), m_Rotation.w, glm::vec3(m_Rotation)) * glm::vec4(m_Direction, 1.0f));
//        m_Direction = newdir;
//    }
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), m_Position);

    return trans * rot * scale;
}
