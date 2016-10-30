#pragma once

#include "IEventData.h"

namespace Game_Logic
{
	/**
	 * Stuff an event needs to provide, event type, etc.
	 */
	class BaseEventData : public IEventData
	{
	protected:
		/**
		 * type of the current event, in this case it will be BaseEvent
		 */
		static const EventType s_eventType;

	public:
		explicit BaseEventData();

		/**
		 * Returns the current type of the event, in this case "BaseEvent"
		 */
		virtual const EventType& VGetEventType() const
		{
			return s_eventType;
		}

		/**
		 * Returns an IEventDataPtr with a freshly created new BaseEvent
		 */
		virtual IEventDataPtr VCopy() const
		{
			return IEventDataPtr(new BaseEventData());
		}
	};
}