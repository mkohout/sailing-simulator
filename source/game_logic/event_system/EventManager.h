#pragma once

#include <string>
#include <map>
#include <list>

#include "../event_system/IEventManager.h"
#include "../../application_layer/utils/Timer.h"

using std::string;
using std::map;
using std::list;
using std::pair;

namespace Game_Logic
{
	/**
	 * Number of queues available for the event manager
	 */
	const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

	/**
	 * Event manager for processing different kinds of occuring events
	 * offers two kinds of priorities: immediately and in-queue
	 */
	class EventManager : public IEventManager
	{
	private:
		/**
		 * Data structure for storing the event listener delegates
		 */
		typedef list<EventListenerDelegate> EventListenerList;

		/**
		 * Data structure for storing the delegates for specific events
		 */
		typedef map<EventType, EventListenerList> EventListenerMap;

		/**
		 * Data structure for storing a single event queue
		 */
		typedef list<IEventDataPtr> EventQueue;

		/**
		 * List that holds all listeners to the specific event types
		 */
		EventListenerMap m_eventListeners;

		/**
		 * Number of queues - as defined in the given constant
		 */
		EventQueue m_queues[EVENTMANAGER_NUM_QUEUES];

		/**
		 * The selected event queue at the start of calling OnUpdate
		 */
		unsigned int m_activeQueue;


	public:
		explicit EventManager();
		virtual ~EventManager() { }

		/**
		 * Adds a listener to the event manager,
		 * which will be executed if the registered event will
		 * be fired somehow
		 */
		virtual bool VAddListener(const EventListenerDelegate&,
			const EventType&);

		/**
		 * Stops listening to a specific event
		 */
		virtual bool VRemoveListener(const EventListenerDelegate&,
			const EventType&);

		/**
		 * Triggers an event immediately, the queue is completely ignored,
		 * and the event manager will execute each registered delegate during this process
		 */
		virtual bool VTriggerEvent(const IEventDataPtr&) const;

		/**
		 * Aborts the given event type by removing every trigger
         * from the queue
		 */
		virtual bool VAbortEvent(const EventType&, bool);

		/**
		 * Adds an event to the queue of the event manager. The queue
         * will be processed the next time the update function is called
		 */
		virtual bool VQueueEvent(const IEventDataPtr&);

		/**
		 * Will process the queue of the event manager,
		 * while processing, the queue is flipped,
		 * so that new incoming events can be stored without any problems.
		 */
		virtual bool VUpdate(unsigned long);
	};
}