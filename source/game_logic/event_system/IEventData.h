#pragma once

#include <string>
#include <memory>

using std::string;
using std::shared_ptr;

namespace Game_Logic
{
	class IEventData;

	typedef shared_ptr<IEventData> IEventDataPtr;
	typedef unsigned long EventType;

	class IEventData
	{
	public:
		/*
		 * Copies the current type of event data
		 */
		virtual IEventDataPtr VCopy() const = 0;

		/*
		 * Returns the event type of the current selected event
		 */
		virtual const EventType& VGetEventType() const = 0;
	};
}