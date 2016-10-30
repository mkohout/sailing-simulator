#pragma once

#include "IEventData.h"

namespace Game_Logic
{
	/*
	 * Event that is being triggered every time the 
     * user has moved the mouse
	 */
	class EvtData_MouseMoved : public IEventData
	{
	private:
		/*
		 * Screen space coordinates of the mouse position
         * that has triggered the current event
		 */
		double m_x;
		double m_y;

        /*
         * Width and height of the current window
         */
        int m_windowWidth;
        int m_windowHeight;

	public:
		static const EventType s_eventType;

		/*
		 * Create the Eventdata instance with the new positions of the mouse
         * that caused the event and the window width and height.
		 */
		explicit EvtData_MouseMoved(
                    const double&, const double&, const int&, const int&);

		virtual const EventType& VGetEventType() const
		{
			return s_eventType;
		}

		virtual IEventDataPtr VCopy() const
		{
			return IEventDataPtr(
                        new EvtData_MouseMoved(
                                               m_x,
                                               m_y,
                                               m_windowWidth,
                                               m_windowHeight
                                               ));
		}

		inline const double& GetX() const
		{
			return m_x;
		}

		inline const double& GetY() const
		{
			return m_y;
		}
        
        inline const float GetAspect() const
        {
            return (float)m_windowWidth/m_windowHeight;
        }
	};
}