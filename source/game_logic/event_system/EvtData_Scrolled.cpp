#include "EvtData_Scrolled.h"
#include <glfw/glfw3.h>

using namespace Game_Logic;

const EventType EvtData_Scrolled::s_eventType = 0x5871BAF5;

EvtData_Scrolled::EvtData_Scrolled(
                            const double& scrollOffsetX,
                            const double& scrollOffsetY)
	: m_scrollOffsetX(scrollOffsetX), m_scrollOffsetY(scrollOffsetY)
{
}

