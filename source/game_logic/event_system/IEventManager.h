#pragma once

#include <memory>
#include <string>

#include "IEventData.h"

#include <fastdelegate/FastDelegate.h>

using std::shared_ptr;
using std::string;

namespace Game_Logic
{
	/*
	 * Data structure for storing member functions and their instance
	 */
	typedef fastdelegate::FastDelegate1<IEventDataPtr> EventListenerDelegate;

	/**
	 * Interface for event managers to provide
     * the basic functionalities in every event manager
     * in the engine
	 */
	class IEventManager
	{
	public:
		enum eConstants { kINFINITE = 0xffffffff };

		explicit IEventManager() { }
		virtual ~IEventManager() { }

		/*
		 * Adds a delegate to the queue that is listening to the
         * given event type
		 */
		virtual bool VAddListener(const EventListenerDelegate&,
			const EventType&) = 0;

		/*
		 * Removes the listener from the queue of the given event type
		 */
		virtual bool VRemoveListener(const EventListenerDelegate&,
			const EventType&) = 0;

		/*
		 * Will execute all listener functions that are listening
         * to the event that was triggered
		 */
		virtual bool VTriggerEvent(const IEventDataPtr&) const = 0;

		virtual bool VAbortEvent(const EventType& type, bool) = 0;

		/*
		 * Method that is executed to process the queue
		 */
		virtual bool VUpdate(unsigned long) = 0;
	};
}