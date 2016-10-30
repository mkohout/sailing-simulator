#pragma once

#include "Sensor.h"

namespace Game_View
{

	/**
	 * Sensor which is used for specific keyboard events
	 */
	class KeyboardSensor : public Sensor
	{
	public:
		KeyboardSensor() = default;

		virtual void VOnInit();

		/*
		 * Method that will be executed if a key was pressed or released
		 */
		void Triggered(IEventDataPtr);
	};

}