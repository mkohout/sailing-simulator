#include "EvtData_ActorHasMoved.h"

using namespace Game_Logic;

const EventType EvtData_ActorHasMoved::s_eventType = 0x1471CAF5;

EvtData_ActorHasMoved::EvtData_ActorHasMoved(
                            ActorId actorId)
    : m_actorId(actorId)
{
}
