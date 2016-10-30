#pragma once

#include "IEventData.h"

namespace Game_Logic
{
	/*
	 * Event that is being triggered every time
     * the user used the scroll functionality of
     * an input device
	 */
	class EvtData_Scrolled : public IEventData
	{
	private:
		/*
		 * Scroll step size that were registered by the OS
		 */
		double m_scrollOffsetX;
		double m_scrollOffsetY;

	public:
		static const EventType s_eventType;

		explicit EvtData_Scrolled(const double&, const double&);

		virtual const EventType& VGetEventType() const
		{
			return s_eventType;
		}

		virtual IEventDataPtr VCopy() const
		{
			return IEventDataPtr(
                        new EvtData_Scrolled(
                                               m_scrollOffsetX,
                                               m_scrollOffsetY
                                               ));
		}

		inline const double& GetScrollOffsetX() const
		{
			return m_scrollOffsetX;
		}

		inline const double& GetScrollOffsetY() const
		{
			return m_scrollOffsetY;
		}
    };
}