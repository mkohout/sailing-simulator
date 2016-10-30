#pragma once

#include "IEventData.h"

namespace Game_Logic
{
	/*
	 * Event that is fired when the
	 * key callback of the video system was triggered
	 */
	class EvtData_KeyPressed : public IEventData
	{
	private:
		/*
		 * The pressed key, which caused the event
		 */
		const int m_key;

		/*
		 * The action - if the event was pressed, released or pressed repeatedly
		 */
		const int m_action;

	public:
		static const EventType s_eventType;

		explicit EvtData_KeyPressed(const int&, const int&);

		virtual const EventType& VGetEventType() const
		{
			return s_eventType;
		}

		virtual IEventDataPtr VCopy() const
		{
			return IEventDataPtr(new EvtData_KeyPressed(m_key, m_action));
		}

		/*
		 * Returns the key which was pressed
		 */
		inline const int& GetKey() const
		{
			return m_key;
		}

		inline const int& GetAction() const
		{
			return m_action;
		}
	};
}