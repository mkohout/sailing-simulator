#include "../../application_layer/core/GameCore.h"
#include "../../game_logic/event_system/EvtData_MouseClicked.h"
#include "../../game_logic/event_system/EvtData_MouseMoved.h"
#include "../../game_logic/event_system/EvtData_Scrolled.h"

#include "MouseSensor.h"

using namespace Game_View;

const int MouseSensor::MOUSE_MOVED = 0x318;
const int MouseSensor::SCROLLED = 0x349;

void MouseSensor::VOnInit()
{
	InitMoveListener();
	InitClickListener();
    InitScrollListener();
}

void MouseSensor::InitScrollListener()
{
    EventListenerDelegate scrollListener =
                fastdelegate::MakeDelegate(
                               this,
                               &MouseSensor::ScrollTriggered
                               );
    
    g_pGameApp->GetPlayerInputEvtMgr()->VAddListener(
                                                     scrollListener,
                                                     EvtData_Scrolled::s_eventType
                                                     );
}

void MouseSensor::InitMoveListener()
{
	EventListenerDelegate mouseMovedListener =
		fastdelegate::MakeDelegate(
			this,
			&MouseSensor::MoveTriggered
			);

	g_pGameApp->GetPlayerInputEvtMgr()->VAddListener(
		mouseMovedListener,
		EvtData_MouseMoved::s_eventType
		);
}

void MouseSensor::InitClickListener()
{
	EventListenerDelegate mouseClickedListener =
		fastdelegate::MakeDelegate(
			this,
			&MouseSensor::ClickTriggered
			);

	g_pGameApp->GetPlayerInputEvtMgr()->VAddListener(
		mouseClickedListener,
		EvtData_MouseClicked::s_eventType
		);
}

void MouseSensor::ScrollTriggered(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_Scrolled> pScrollEvent =
                std::static_pointer_cast<EvtData_Scrolled>(pEventData);
    
    SetScrollOffset(
                    pScrollEvent->GetScrollOffsetX(),
                    pScrollEvent->GetScrollOffsetY());
    
    SetActiveValue(SCROLLED, true);
}

void MouseSensor::MoveTriggered(IEventDataPtr pEventData)
{
	shared_ptr<EvtData_MouseMoved> pMouseMovedEvent =
		std::static_pointer_cast<EvtData_MouseMoved>(pEventData);

	SetCursorPosition(
        pMouseMovedEvent->GetX(), pMouseMovedEvent->GetY());
	SetActiveValue(MOUSE_MOVED, true);
}

void MouseSensor::ClickTriggered(IEventDataPtr pEventData)
{
	shared_ptr<EvtData_MouseClicked> pMouseClickedEvent =
		std::static_pointer_cast<EvtData_MouseClicked>(pEventData);

	if (pMouseClickedEvent->GetAction() == GLFW_PRESS)
	{
		SetActiveValue(pMouseClickedEvent->GetButton(), true);
	}
	else if (pMouseClickedEvent->GetAction() == GLFW_RELEASE)
	{
		SetActiveValue(pMouseClickedEvent->GetButton(), false);
	}
}