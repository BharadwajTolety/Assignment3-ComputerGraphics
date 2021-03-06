#include "KeyCtrlCamera.h"
#include <QKeyEvent>
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

KeyCtrlCamera::KeyCtrlCamera(Controller* _ctrl)
    : Camera(), m_Ctrl(nullptr), m_MoveSpeed(2.0f), m_RotSpeed(glm::radians(1.0f)), m_FovIncre(0.2f)
{
    m_Ctrl = _ctrl;
}
KeyCtrlCamera::~KeyCtrlCamera()
{

}
void KeyCtrlCamera::Update()
{
    KeyUpdate();
    m_View = glm::lookAt(GetTransform()->GetPosition(), GetTransform()->GetPosition() + GetTransform()->GetForward(), GetTransform()->GetUp());
    m_ProjMatUpdate();
}

void KeyCtrlCamera::m_FovUpdate(const unsigned char &_in_or_out)
{
    float incre = m_FovIncre;
    if (_in_or_out == CAMERA_ZOOM_IN)
        incre *= -1;

    m_FOV += incre;
    if (m_FOV < 45) m_FOV = 45.0f;
    if (m_FOV > 120) m_FOV = 120.0f;
}

void KeyCtrlCamera::KeyUpdate()
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

    unsigned char plus = m_Ctrl->GetKeys()[m_Ctrl->QtKeyCodeTranslate(Qt::Key_Plus)];
    if (plus) m_FovUpdate(CAMERA_ZOOM_IN);

    unsigned char minus = m_Ctrl->GetKeys()[m_Ctrl->QtKeyCodeTranslate(Qt::Key_Minus)];
    if (minus)
    {
//        printf("minus pressed.");
        m_FovUpdate(CAMERA_ZOOM_OUT);
    }
}
