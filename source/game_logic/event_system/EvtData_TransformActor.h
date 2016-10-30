#pragma once

#include "IEventData.h"
#include "../actor_system/Actor.h"

#include <glm/mat4x4.hpp>

namespace Game_Logic
{
	/*
	 * Event that is being triggered every time an
     * actor will be transformed. This event
     * holds the changes in the transformation in form
     * of a matrix.
	 */
	class EvtData_TransformActor : public IEventData
	{
	private:
        /**
         * The actor that will receive a transformation
         */
        ActorId m_actorId;
        
        /**
         * The transformation matrix that will be applied
         * to the actor
         */
        mat4 m_matrix;
        
        /**
         * Indicates whether the transformation stored
         * in the current event will be applied to the 
         * parent of the target (true) 
         * or just the target itself (false)
         */
        bool m_applyTransformationToParent;
        
        
	public:
		static const EventType s_eventType;

		explicit EvtData_TransformActor(ActorId, mat4);
        
        /**
         * Sets the m_applyTransformationToGroup value
         */
        void SetApplyTransformationToParent(bool);

		virtual const EventType& VGetEventType() const
		{
			return s_eventType;
		}

		virtual IEventDataPtr VCopy() const
		{
			return IEventDataPtr(
                        new EvtData_TransformActor(
                                               m_actorId,
                                               m_matrix
                                               ));
		}
        
        /**
         * Returns the true if the stored transformation
         * will be applied to the parent of the target node
         */
        const bool ApplyTransformationToParent() const
        {
            return m_applyTransformationToParent;
        }

        const ActorId GetActorId() const
        {
            return m_actorId;
        }
        
        const mat4 GetTransformationMatrix() const
        {
            return m_matrix;
        }
    };
}