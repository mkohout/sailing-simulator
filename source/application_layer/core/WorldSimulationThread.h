#pragma once

#include "Thread.h"

using std::string;

namespace Application_Layer
{
	/**
	 * WorldSimluationThread will take care of the execution of the lua scripts to
	 * simulate behaviour (AI, etc.) in the world
	 */
	class WorldSimulationThread : public Thread
	{
	public:
		WorldSimulationThread() { }

		virtual string VGetIdentifier()
		{
			return "WorldSimulationThread";
		}

		virtual void VInit();

		/**
		 * Update function that is called every frame
         * in the world simulation thread
		 */
		virtual void VUpdate(const unsigned int);

		virtual void VStartThread()
		{
			_thread = thread(
                    &WorldSimulationThread::VRun, this);

			_thread.detach();
		}

		/**
		 * Returns the number of calls to VUpdate per second
		 * acording to the game settings.
		 */
		virtual const short VGetFramesForThread() const;

		virtual ~WorldSimulationThread()
		{
		}
	};

}