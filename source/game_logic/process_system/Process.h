#pragma once

#include <memory>

using std::shared_ptr;

namespace Game_Logic
{
	class Process;
	class ProcessManager;

	typedef shared_ptr<Process> StrongProcessPtr;

	/**
	 * Class to provide the functionality
     * to calculate/execute more expensive or returning actions
     * over a span of multiple frames
	 */
	class Process
	{
		friend class ProcessManager;

	public:
		/*
		 * Possible states a process can be in
		 */
		enum State
		{
			UNINTIALIZED = 0,
			REMOVED,
			RUNNING,
			PAUSED,
			SUCCEEDED,
			FAILED,
			ABORTED
		};

	private:
		void SetState(State newState)
		{
			m_currentState = newState;
		}

	protected:
		State m_currentState;
		StrongProcessPtr m_pChild;

		/*
		 * If a process has been created and initialized
         * it is by default in the state RUNNING
		 */
		virtual void VInit()
		{
			m_currentState = RUNNING;
		}

		/*
		 * Functionality that will be executed
         * when the current process has been aborted
		 */
		virtual void VOnAbort();

		/*
		 * Function that will be executed every frame
         * of the world simulation thread
		 */
		virtual void VOnUpdate(const unsigned int) = 0;

		/*
		 * Functionality that will be executed
         * when the current process has been executed
         * successfully
		 */
		virtual void VOnSuccess();

		/*
		 * Functionality that will be executed when the
         * current process has failed
		 */
		virtual void VOnFail();

	public:
		Process() { }
		virtual ~Process() { }

		/*
		 * The current process succeeded, method
         * will set the state of the process to SUCCEEDED
		 */
		void Succeed();

		/*
		 * Process failed, set the state accordingly
		 */
		void Fail();

		/*
		 * Pauses the current process
		 */
		void Pause();

		void UnPause();
        
        void Abort();

		/*
		 * Returns the current state of
         * the current process
		 */
		State GetState() const
		{
			return m_currentState;
		}

		bool IsAlive() const
		{
			return (m_currentState == RUNNING ||
				m_currentState == PAUSED);
		}

		bool IsDead() const
		{
			return (m_currentState == SUCCEEDED ||
				m_currentState == FAILED ||
				m_currentState == ABORTED);
		}

		bool IsRemoved() const
		{
			return m_currentState == REMOVED;
		}

		bool IsPaused() const
		{
			return m_currentState == PAUSED;
		}

		/*
		 * Attaches a new process to the current
         * that will be executed right after the
         * current process succeeded
		 */
		void AttachChild(StrongProcessPtr pChild)
		{
			m_pChild = pChild;
		}

		/*
		 * Removes the current child
		 */
		StrongProcessPtr RemoveChild()
		{
			StrongProcessPtr pChild = StrongProcessPtr(m_pChild);

			m_pChild = nullptr;

			return pChild;
		}

		/*
		 * Returns the child of the current process
		 */
		StrongProcessPtr PeekChild()
		{
			return m_pChild;
		}

	};
}