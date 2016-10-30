#include "EvtData_KeyPressed.h"

using namespace Game_Logic;

const EventType EvtData_KeyPressed::s_eventType = 0x3625B9E5;

EvtData_KeyPressed::EvtData_KeyPressed(const int& key, const int& action)
	: m_key(key), m_action(action)
{

}