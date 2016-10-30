#include "EvtData_MouseClicked.h"

using namespace Game_Logic;

const EventType EvtData_MouseClicked::s_eventType = 0x5817A9E5;

EvtData_MouseClicked::EvtData_MouseClicked(const int& button, const int& action)
	: m_button(button), m_action(action)
{
}