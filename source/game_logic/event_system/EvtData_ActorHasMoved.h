#pragma once

#include "IEventData.h"
#include "../actor_system/Actor.h"

#include <glm/mat4x4.hpp>

namespace Game_Logic
{
	/**
	 * Event that is triggered when an actor has moved
     * during player input or physical influences
	 */
	class EvtData_ActorHasMoved : public IEventData
	{
	private:
        /**
         * The actor that will receive a transformation
         */
        ActorId m_actorId;
        
        
	public:
		static const EventType s_eventType;

		explicit EvtData_ActorHasMoved(ActorId);
        
		virtual const EventType& VGetEventType() const
		{
			return s_eventType;
		}

		virtual IEventDataPtr VCopy() const
		{
			return IEventDataPtr(
                        new EvtData_ActorHasMoved(m_actorId));
		}
        
        const ActorId GetActorId() const
        {
            return m_actorId;
        }
        
    };
}