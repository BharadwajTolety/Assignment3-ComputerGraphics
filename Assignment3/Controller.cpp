#include "Controller.h"

Controller* Controller::s_GlobalController = nullptr;

Controller::Controller()
    : m_LastPosX(0.0f), m_LastPosY(0.0f),
      m_CurrPosX(0.0f), m_CurrPosY(0.0f),
      m_IsPressed(false), m_MovingSensitivity(0.05f),
      m_Keys(nullptr), m_Toggles(nullptr)
{
    m_Keys = new unsigned char[65536]{};
    m_Toggles = new unsigned char[65536]{TOGGLE_UNCHANGED};
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
    if (_action == KEY_PRESSED) m_Toggles[_key] = TOGGLE_PRESSED;
    else if (_action == KEY_RELEASED) m_Toggles[_key] = TOGGLE_RELEASED;
}

unsigned char Controller::GetAndResetToggle(int _key)
{
    unsigned char tmp = m_Toggles[_key];
    m_Toggles[_key] = TOGGLE_UNCHANGED;
    return tmp;
}
