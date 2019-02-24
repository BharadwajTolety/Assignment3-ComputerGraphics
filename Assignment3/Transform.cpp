#include "Transform.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

glm::vec3 Transform::Up = glm::vec3(0.0f, 1.0f, 0.0f);

Transform::Transform()
    : m_Position(0.0f, 0.0f, 0.0f), m_Rotation(0.0f, 0.0f, 0.0f, 0.0f), m_Scale(1.0f, 1.0f, 1.0f),
      m_Direction(0.0f, 0.0f, 1.0f), m_Up(0.0f, 1.0f, 0.0f)
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

void Transform::DirRotate(const float &_ang, const glm::vec3 &_axis)
{
    glm::mat4 rot = (_ang == 0.0f || _axis == m_Direction)? glm::mat4(1.0f): glm::rotate(glm::mat4(1.0f), _ang, _axis);
    m_Direction = glm::vec3(rot * glm::vec4(m_Direction, 1.0f));
}

void Transform::UpRotate(const float &_ang)
{
    glm::mat4 rot = (_ang == 0.0f)? glm::mat4(1.0f): glm::rotate(glm::mat4(1.0f), _ang, m_Direction);
    m_Up = glm::vec3(rot * glm::vec4(m_Up, 1.0f));
}

void Transform::Translate(const glm::vec3 &_vec)
{
    glm::vec3 forward = GetForward() * _vec.z;
    glm::vec3 up = GetUp() * _vec.y;
    glm::vec3 right = GetRight() * _vec.x;

//    std::cout<< "//////////////" <<std::endl;
//    std::cout<< "forward: "<< forward.x << ", " << forward.y << ", " << forward.z <<std::endl;
//    std::cout<< "up: "<< up.x << ", " << up.y << ", " << up.z <<std::endl;
//    std::cout<< "right: "<< right.x << ", " << right.y << ", " << right.z <<std::endl;

    glm::mat4 trans = glm::translate(glm::mat4(1.0f), forward + up + right);

    m_Position = glm::vec3(trans * glm::vec4(m_Position, 1.0f));
}
