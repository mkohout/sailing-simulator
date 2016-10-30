#pragma once

#include <memory>
#include <map>

#include "stdafx.h"
#include "Thread.h"
#include "../../game_logic/game_app/GameApp.h"

#include "../resources/FileManager.h"
#include "../utils/Logger.h"
#include "../utils/Timer.h"

using std::string;
using std::shared_ptr;
using std::pair;
using std::map;

namespace Application_Layer
{
	typedef const shared_ptr<Thread> ThreadPtr;
	typedef map<string, ThreadPtr> ThreadList;

	/**
	 * Technical core for the game,
	 * class is handline the subsystems and is taking care of
	 * the operating system
	 */
	class GameCore
	{
	private:
		Game_Logic::GameApp* m_pGameApp;
		Logger* m_pLogger;
		FileManager* m_pFileManager;
		static GameCore* s_pSingleton;

		/**
		 * List of active threads currently running in the game
		 */
		ThreadList m_threadList;

		GameCore() = default;
		~GameCore();

		/* avoid copying */
		GameCore(GameCore* pGameCore);
		GameCore(const GameCore* pGameCore);

		/**
		 * Adds a thread to the thread list
		 */
		void AddToThreadList(ThreadPtr);

		/**
		 * Removes a thread which means,
         * that the thread will going to stop
		 */
		bool RemoveFromThreadList(string);

		/**
		 * Returns the thread with the given identifier
		 */
		ThreadPtr GetThread(string);

		/**
		 * Starts the thread where the process manager and the
		 * games event system will be updated frequently
		 */
		void StartWorldSimulation();
        
        /**
         * Starts the thread that calls the update method
         * of the audio manager in every frame
         */
        void StartSoundThread();
        
        /**
         * Starts the thread where the actors that
         * have a physics component will be updated
         * every frame according to the physical laws
         */
        void StartPhysicSimulation();

		/**
		 * Starts the thread, that will listen to the input event manager
		 */
		void StartPlayerInputListener();

	public:
		static
			GameCore* GetInstance()
		{
			if (!s_pSingleton)
			{
				s_pSingleton = new GameCore();

				s_pSingleton->Init();
			}

			return s_pSingleton;
		}

		/**
		 * Handles the destruction of the singleton instance
		 */
		static
			void DestroyInstance()
		{
			SAFE_DELETE(s_pSingleton);
		}

		::Game_Logic::GameApp* GetGameApp()
		{
			return m_pGameApp;
		}

		/**
		 * Initialize GameCore and create GameApp Instance
		 * GameCore has every time a companion GameApp
		 */
		void Init();

		void InitLogger();

		void InitFileManager();

		void LogString(Logger::LOG_FLAG, string) const;

		inline FileManager* GetFileManager() const
		{
			return m_pFileManager;
		}

		/**
		 * Start tasks in several threads
		 */
		void StartThreads();
        
        /**
         * Send the stop signal to the threads
         */
        void StopThreads();
	};

}