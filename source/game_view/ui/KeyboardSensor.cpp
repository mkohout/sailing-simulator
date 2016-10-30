#include "../application_layer/core/GameCore.h"
#include "../game_logic/event_system/EvtData_KeyPressed.h"

#include "KeyboardSensor.h"

using namespace Game_View;

void KeyboardSensor::VOnInit()
{
	EventListenerDelegate listenerFunction =
		fastdelegate::MakeDelegate(
			this,
			&KeyboardSensor::Triggered
			);

	g_pGameApp->GetPlayerInputEvtMgr()->VAddListener(
		listenerFunction,
		EvtData_KeyPressed::s_eventType
		);
}

void KeyboardSensor::Triggered(IEventDataPtr pEventData)
{
	shared_ptr<EvtData_KeyPressed> pKeyPressedEvent =
		std::static_pointer_cast<EvtData_KeyPressed>(pEventData);


	if (pKeyPressedEvent->GetAction() == GLFW_PRESS)
	{
		SetActiveValue(pKeyPressedEvent->GetKey(), true);
	}
	else if (pKeyPressedEvent->GetAction() == GLFW_RELEASE)
	{
		SetActiveValue(pKeyPressedEvent->GetKey(), false);
	}
}