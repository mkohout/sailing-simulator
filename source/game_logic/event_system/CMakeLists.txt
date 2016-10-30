SET(EVENT_SYSTEM_HEADERS	event_system/BaseEventData.h
				event_system/EventManager.h
				event_system/IEventData.h
				event_system/IEventManager.h
				event_system/EvtData_ActorHasMoved.h
				event_system/EvtData_KeyPressed.h
				event_system/EvtData_MouseClicked.h
				event_system/EvtData_MouseMoved.h
				event_system/EvtData_Scrolled.h
				event_system/EvtData_TransformActor.h
				event_system/EvtData_ChangeAudio.h
				PARENT_SCOPE
				)

SET(EVENT_SYSTEM_SOURCE	event_system/BaseEventData.cpp
				event_system/EventManager.cpp
				event_system/EvtData_ActorHasMoved.cpp
				event_system/EvtData_KeyPressed.cpp
				event_system/EvtData_MouseClicked.cpp
				event_system/EvtData_MouseMoved.cpp
				event_system/EvtData_Scrolled.cpp
				event_system/EvtData_TransformActor.cpp
				event_system/EvtData_ChangeAudio.cpp
				PARENT_SCOPE
				)