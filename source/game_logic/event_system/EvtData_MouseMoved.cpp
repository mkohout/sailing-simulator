#include "EvtData_MouseMoved.h"
#include <glfw/glfw3.h>

using namespace Game_Logic;

const EventType EvtData_MouseMoved::s_eventType = 0x5877B9F5;

EvtData_MouseMoved::EvtData_MouseMoved(
                            const double& xpos,
                            const double& ypos,
                            const int& windowWidth,
                            const int& windowHeight)
	: m_x(xpos), m_y(ypos), m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{
}

