#pragma once

#include <glew/glew.h>
#include <glfw/glfw3.h>

namespace Application_Layer
{
	/**
	  * Class is handling the timing functions for several purposes
	  * most likely, that they are using the offered functions of the
	  * graphics libraries.
	  */
	class Timer
	{
	public:
		/**
		 * Returns the current timestamp as a double
		 * 1.0 is one second
		 */
		static const double GetCurrentTimestamp()
		{
			return glfwGetTime();
		}

		/**
		 * Returns the current timestamp as integer, means in milliseconds
		 * 1 second = 1000
		 */
		static const unsigned long GetCurrentTimestampInMilliseconds()
		{
			return glfwGetTime() * 1000;
		}

		/**
		 * Returns the time ellapsed since the given other timestamp
		 */
		static const double GetTimeEllapsed(const double since)
		{
			return GetCurrentTimestamp() - since;
		}
	};
}