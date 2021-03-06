#include "Drone.h"
#include "DronePropeller.h"

Drone::Drone(Controller* _ctrl)
    : m_Ctrl(_ctrl), m_RotSpeed(glm::radians(1.0f)), m_MoveSpeed(2.0f)
{
}

void Drone::Update()
{
    KeyUpdate();
}

void Drone::KeyUpdate()
{
    unsigned char w = m_Ctrl->GetKeys()[m_Ctrl->QtKeyCodeTranslate(Qt::Key_W)];
    if (w) GetTransform()->DirRotate(m_RotSpeed, GetTransform()->GetRight());
    unsigned char s = m_Ctrl->GetKeys()[m_Ctrl->QtKeyCodeTranslate(Qt::Key_S)];
    if (s) GetTransform()->DirRotate(-m_RotSpeed, GetTransform()->GetRight());
    unsigned char a = m_Ctrl->GetKeys()[m_Ctrl->QtKeyCodeTranslate(Qt::Key_A)];
    if (a)
    {
        GetTransform()->DirRotate(m_RotSpeed, GetTransform()->GetUp());
    }
    unsigned char d = m_Ctrl->GetKeys()[m_Ctrl->QtKeyCodeTranslate(Qt::Key_D)];
    if (d) GetTransform()->DirRotate(-m_RotSpeed, GetTransform()->GetUp());


    unsigned char f = m_Ctrl->GetKeys()[m_Ctrl->QtKeyCodeTranslate(Qt::Key_F)];
    if (f) GetTransform()->UpRotate(m_RotSpeed);
    unsigned char c = m_Ctrl->GetKeys()[m_Ctrl->QtKeyCodeTranslate(Qt::Key_C)];
    if (c) GetTransform()->UpRotate(-m_RotSpeed);


    unsigned char up = m_Ctrl->GetKeys()[m_Ctrl->QtKeyCodeTranslate(Qt::Key_Up)];
    if (up)
    {
        GetTransform()->Translate(glm::vec3(0.0f, m_MoveSpeed, 0.0f));
    }
    unsigned char down = m_Ctrl->GetKeys()[m_Ctrl->QtKeyCodeTranslate(Qt::Key_Down)];
    if (down) GetTransform()->Translate(glm::vec3(0.0f, -m_MoveSpeed, 0.0f));
    unsigned char left = m_Ctrl->GetKeys()[m_Ctrl->QtKeyCodeTranslate(Qt::Key_Left)];
    if (left)
    {
        GetTransform()->Translate(glm::vec3(-m_MoveSpeed, 0.0f, 0.0f));
    }
    unsigned char right = m_Ctrl->GetKeys()[m_Ctrl->QtKeyCodeTranslate(Qt::Key_Right)];
    if (right) GetTransform()->Translate(glm::vec3(m_MoveSpeed, 0.0f, 0.0f));
}
