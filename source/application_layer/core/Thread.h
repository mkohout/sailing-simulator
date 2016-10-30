#pragma once

#include "stdafx.h"

#include "../utils/Timer.h"

#include <thread>
#include <string>

using std::string;
using std::thread;

namespace Application_Layer
{
	/**
	* Interface for the threads used in the engine
	*/
	class Thread
	{
	protected:
		thread _thread;
        
        bool m_keepAlive;

	public:
		virtual ~Thread()
		{
        }

		virtual string VGetIdentifier() = 0;

		virtual void VInit()
        {
            m_keepAlive = true;
        }

		virtual void VRun()
		{
			VInit();

			double timePerFrame =
				1000.0 / VGetFramesForThread();
			double threadStart = 0;
			double sleepFor = 0;
			double timeEllapsed = 0;
			double lastFrame = 0;

			while (IsAlive())
			{
				threadStart = Timer::GetCurrentTimestamp();
				VUpdate((threadStart - lastFrame) * 1000);
				timeEllapsed = Timer::GetTimeEllapsed(threadStart);
				lastFrame = threadStart;

				sleepFor = (timePerFrame/1000.0 - timeEllapsed);
				if (sleepFor <= 0)
				{
					continue;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds((int)(sleepFor*1000)));
			}
		}

		virtual void VStartThread() = 0;

		virtual void VUpdate(const unsigned int) = 0;

		/**
		* Returns the number of times, the update function should be called
		*/
		virtual const short VGetFramesForThread() const = 0;

		/**
		* Returns true, if the current thread is still alive
		*/
        bool IsAlive() const
        {
            return m_keepAlive;
        }

        /**
         * Sets the member variable keepAlive to false so that
         * the thread will terminate after the next frame
         */
		void Kill()
        {
            m_keepAlive = false;
        }
        
        void WaitForThread()
        {
            if (_thread.joinable())
            {
                _thread.join();
            }

        }
	};

}