#include "CameraSwitchcer.h"
#include "Controller.h"
#include <QKeyEvent>
CameraSwitchcer::CameraSwitchcer(Camera* _c1, Camera* _c2)
    :m_Camera1(_c1), m_Camera2(_c2)
{

}
void CameraSwitchcer::Update()
{
    Controller* ctrl = Controller::s_GlobalController;
    if (ctrl->GetAndResetToggle(ctrl->QtKeyCodeTranslate(Qt::Key_Space)) == TOGGLE_RELEASED)
    {
        Camera* tmp = m_Camera1;
        m_Camera1 = m_Camera2;
        m_Camera2 = tmp;
    }
}
