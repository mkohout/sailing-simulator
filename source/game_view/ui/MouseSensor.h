#pragma once

#include "Sensor.h"

namespace Game_View
{

	/**
	 * Is being triggered when the mouse was moved
	 */
	class MouseSensor : public Sensor
	{
	public:
		static const int MOUSE_MOVED;
        static const int SCROLLED;

	private:
		/*
		 * Cursor positions in x and y direction
		 */
		double m_xPos, m_yPos;
        
        /**
         * Step size of the scroll in two dimensions
         */
        double m_scrollOffsetX, m_scrollOffsetY;

		void InitClickListener();

		void InitMoveListener();
        
        void InitScrollListener();

		void SetCursorPosition(
                const double& xPos, const double& yPos)
		{
			m_xPos = xPos;
			m_yPos = yPos;
		}
        
        void SetScrollOffset(
                const double& scrollOffsetX,
                const double& scrollOffsetY)
        {
            m_scrollOffsetX = scrollOffsetX;
            m_scrollOffsetY = scrollOffsetY;
        }

	public:
		MouseSensor() = default;

        virtual ~MouseSensor()
        {
        }
        
		virtual void VOnInit();

        /*
         * Method is executed if the user has scrolled
         */
        void ScrollTriggered(IEventDataPtr);

		/*
		 * Method is executed if the mouse moved
		 */
		void MoveTriggered(IEventDataPtr);

		/*
		 * Method will be executed if a mouse button was clicked
		 */
		void ClickTriggered(IEventDataPtr);
	};

}