#include "Controller.h"


Controller::Controller()
    : m_LastPosX(0.0f), m_LastPosY(0.0f),
      m_CurrPosX(0.0f), m_CurrPosY(0.0f),
      m_IsPressed(false), m_MovingSensitivity(0.05f),
      m_Keys(nullptr)
{
    m_Keys = new unsigned char[65536]{};
}

Controller::~Controller()
{
    if (m_Keys) delete[] m_Keys;
}

void Controller::Update()
{
    m_LastPosX = m_CurrPosX;
    m_LastPosY = m_CurrPosY;
}

void Controller::KeysCallback(int _key, int _action)
{
    m_Keys[_key] = (_action == KEY_RELEASED)?0x00:0x01;
}


