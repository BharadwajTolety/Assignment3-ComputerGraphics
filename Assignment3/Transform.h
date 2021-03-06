#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>

#define AXIS_X glm::vec3(1.0f, 0.0f, 0.0f)
#define AXIS_Y glm::vec3(0.0f, 1.0f, 0.0f)
#define AXIS_Z glm::vec3(0.0f, 0.0f, 1.0f)

/**
    Transform is the storage of gameobjects' transformations.
**/

class Transform
{
public:
    static glm::vec3 Up;
public:
    Transform();
    Transform(const glm::vec3& _pos, const glm::vec4& _rot, const glm::vec3& _scale, const glm::vec3& _dir);
public:
    inline void SetPosition(const glm::vec3& _p) { m_Position = _p; }
    inline void SetRotation(const glm::vec4& _r) { m_Rotation = _r; }
    inline void SetDirection(const glm::vec3& _d) { m_Direction = _d; }
    inline void SetScale(const glm::vec3& _s) { m_Scale = _s; }

    inline glm::vec3 GetPosition() const { return m_Position; }
    inline glm::vec3 GetDirection() const { return m_Direction; }
    inline glm::vec3 GetScale() const { return m_Scale; }
    inline glm::vec4 GetRotation() const { return m_Rotation; }


public:
    inline glm::vec3 GetForward() const { return glm::normalize(m_Direction);}
    inline glm::vec3 GetRight() const { return glm::normalize(glm::cross(GetForward(), m_Up));}

    inline glm::vec3 GetUp() const { return glm::normalize(m_Up);}

public:
    glm::mat4 GetModelTransformMat() const;

public:
    void DirRotate(const float& _ang, const glm::vec3& _axis);
    void UpRotate(const float& _ang);

    void Translate(const glm::vec3& _vec);

private:
    glm::vec3 m_Position;
    glm::vec4 m_Rotation;
    glm::vec3 m_Scale;

    glm::mat4 m_RotMat;
    glm::vec3 m_Direction;
    glm::vec3 m_Up;
};

#endif // TRANSFORM_H
