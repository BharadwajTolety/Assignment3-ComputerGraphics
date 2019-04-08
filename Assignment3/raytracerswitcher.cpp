#include "raytracerswitcher.h"
#include "Controller.h"
#include <QKeyEvent>

RayTracerSwitcher::RayTracerSwitcher()
    : m_RayTracerEnabled(false)
{

}

void RayTracerSwitcher::Update()
{
    Controller* ctrl = Controller::s_GlobalController;
    if (ctrl->GetAndResetToggle(ctrl->QtKeyCodeTranslate(Qt::Key_T)) == TOGGLE_RELEASED)
    {
        m_RayTracerEnabled = !m_RayTracerEnabled;
    }
}
