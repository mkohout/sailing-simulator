#pragma once

#include "Process.h"
#include <list>
#include <memory>

using std::list;
using std::shared_ptr;

namespace Game_Logic
{
	typedef list<StrongProcessPtr> ProcessList;

	/**
	 * Class that is constantly processing the
     * processes that were attached to the process list.
	 */
	class ProcessManager
	{
	private:
		ProcessList m_processes;

	public:
		ProcessManager() = default;
		~ProcessManager()
		{
			ClearAllProcesses();
		}

		/*
		 * Processes the queue resp. the process list
         * and executes the processes
		 */
		unsigned int UpdateProcesses(unsigned int);

		/*
		 * Adds the given process to the process list
		 */
		void AttachProcess(StrongProcessPtr);

		void AbortAllProcesses(bool);

		unsigned int GetProcessCount() const
		{
			return (int)m_processes.size();
		}

	private:
		void ClearAllProcesses();
	};
}