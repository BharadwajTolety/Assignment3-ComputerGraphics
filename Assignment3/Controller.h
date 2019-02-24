#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include <QKeyEvent>
#define KEY_PRESSED 0x01
#define KEY_RELEASED  0x00

class Controller
{
public:
    Controller();
    ~Controller();
    void Update();
public:
    inline std::vector<float> GetMouseLast() const { return {m_LastPosX, m_LastPosY}; }

    inline void SetMouseCurr(const float& _x, const float& _y) { m_CurrPosX = _x; m_CurrPosY = _y; }
    inline void SetMouseLast(const float& _x, const float& _y) {m_LastPosX = _x; m_LastPosY = _y;}

    inline std::vector<float> GetMouseCurr() const { return {m_CurrPosX, m_CurrPosY}; }

    inline bool GetIsPressed() const { return m_IsPressed; }
    inline bool SetIsPressed(const bool& _is) { m_IsPressed = _is; }

    inline void SetMouseMovingSensitivity(const float& _ms) { m_MovingSensitivity = _ms;}
    inline float GetMouseMovingSensitivity() const { return m_MovingSensitivity; }

    inline unsigned char* GetKeys() const {return m_Keys;}
    inline int  QtKeyCodeTranslate(int _qt_code) { return (_qt_code >= Qt::Key_Escape)? (_qt_code - Qt::Key_Escape + (Qt::Key_Z - Qt::Key_Space)):(_qt_code - Qt::Key_Space); }
public:
    void KeysCallback(int _key, int _action);

private:
    float m_LastPosX, m_LastPosY, m_CurrPosX, m_CurrPosY;
    bool m_IsPressed;
    float m_MovingSensitivity;
    unsigned char* m_Keys;
};

#endif // CONTROLLER_H
