#pragma once

#include "IEventData.h"

namespace Game_Logic
{
	/*
	 * Event that is being triggered every time
     * the user has pressed or released a mouse button
	 */
	class EvtData_MouseClicked : public IEventData
	{
	private:
        /*
         * The button that caused the event
         */
		const int m_button;

        /*
         * The action, similiar to the keypressed callback: pressed, released, etc.
         */
		const int m_action;

	public:
		static const EventType s_eventType;

		explicit EvtData_MouseClicked(const int&, const int&);

		virtual const EventType& VGetEventType() const
		{
			return s_eventType;
		}

		virtual IEventDataPtr VCopy() const
		{
			return IEventDataPtr(new EvtData_MouseClicked(m_button, m_action));
		}

		inline const int& GetButton() const
		{
			return m_button;
		}

		inline const int& GetAction() const
		{
			return m_action;
		}
	};
}