#pragma once

#include "Thread.h"

using std::string;

namespace Application_Layer
{
	/**
	 * The thread that is taking care of
	 * the user input by constantly checking
	 * for keyboard or mouse events
	 */
	class PlayerInputThread : public Thread
	{
	public:
		PlayerInputThread() { }

		virtual string VGetIdentifier()
		{
			return "PlayerInputThread";
		}

		/**
		 * Initializes the player input thread
		 */
		virtual void VInit();

		/**
		 * Check for user input
		 */
		virtual void VUpdate(const unsigned int);

		virtual void VStartThread()
		{
			_thread = thread(&PlayerInputThread::VRun, this);

			_thread.detach();
		}

		/**
		 * Returns the number of calls to VUpdate per second
		 * acording to the game settings.
		 */
		virtual const short VGetFramesForThread() const;

		virtual ~PlayerInputThread()
		{
		}
	};

}