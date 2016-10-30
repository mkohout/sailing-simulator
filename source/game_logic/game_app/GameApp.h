#pragma once

#include "../../application_layer/core/stdafx.h"

#include "../event_system/EventManager.h"
#include "../process_system/ProcessManager.h"
#include "../../application_layer/resources/FileManager.h"
#include "../../application_layer/resources/ResourceManager.h"
#include "../../game_view/scene/Scene.h"
#include "../../game_view/audio/AudioManager.h"
#include "../../game_view/video_system/IVideoSystem.h"
#include "../../application_layer/utils/Logger.h"

#ifdef OPEN_GL
#include "../../game_view/video_system/OpenGLVideoSystem.h"
#endif

using namespace Application_Layer;
using namespace Game_View;

using std::string;

namespace Application_Layer
{
	class GameCore;
}

namespace Game_View
{
	class IVideoSystem;
	class Scene;
}

namespace Game_Logic
{
	/**
	 * Main class for game logic.
	 * Class lays inbetween the Game_View and the Application_Layer
	 * Triggers the renderer and is the holder of the current scene.
	 */
	class GameApp
	{
		friend GameCore;

	private:
		/**
		 * Pointer to the underlying application layer
		 */
		GameCore* m_pGameCore;

		/**
		 * The current scene, which will be rendered,
		 * and that is used to determine the resources to load
		 */
		Scene* m_pCurrentScene;

		/**
		 * Stores the settings for the whole game
		 */
		Settings* m_pGameSettings;

		/**
		 * Pointer to the games main event manager
		 */
		EventManager* m_pEventManager;
        
        /**
         * Pointer to the audio manager
         * that is responsible for playing back sound files
         */
        AudioManager* m_pAudioManager;

		/**
		 * Pointer to the event manager which handles the
		 * player input events
		 */
		EventManager* m_pPlayerInputEvtMgr;

		/**
		 * Pointer to the global process manager
		 */
		ProcessManager* m_pProcessManager;

		/**
		 * The resource manager of the game, that is responsible
         * for a proper loading of the requested assets
		 */
		ResourceManager* m_pResourceManager;

		/**
		 * Pointer to the used video system for the game
		 */
		IVideoSystem* m_pVideoSystem;

		/**
		 * Singleton instance of the game logic layer
		 */
		static GameApp* s_pSingleton;

		GameApp() = default;
		~GameApp();

		/* avoid copying GameApp */
		GameApp(GameApp* pGameApp);
		GameApp(const GameApp* pGameApp);

	public:

		/**
		 * Will set the game core to access the application layer
		 */
		void SetGameCore(GameCore* pGameCore)
		{
			m_pGameCore = pGameCore;
		}

		/**
		 * Read settings and initialize the game
		 */
		void StartGame();

		/**
		 * Triggers the rendering of the current scene
		 */
		void RenderCurrentScene(const unsigned int) const;

		/**
		 * Triggers various function to free memory and release subsystems
		 */
		void Shutdown();

		/**
		 * Initializes the video system for the game
		 */
		void InitVideoSystem();

		/**
		 * Initializes the game settings by the given xml file
		 */
		void InitGameSettings();

		/**
		 * Initializes the resource manager, that is
         * responsible for loading the resources, the
         * engine needs
		 */
		void InitResourceManager();

		/**
		 * Initializes the scene by the given xml file
		 */
		void InitScene();

		/**
		 * Initializes the global event manager
		 */
		void InitEventManager();

		/**
		 * Initializes the global process manager
		 */
		void InitProcessManager();
        
        /**
         * Initializes the audio manager
         */
        void InitAudioManager();

		/**
		 * Returns a pointer to the global file manager
		 */
		FileManager* GetFileManager() const;

		/**
		 * Returns a pointer to the settings data structure,
		 * that holds the game settings
		 */
		Settings* GetGameSettings() const;

		/**
		 * Returns a pointer to the event manager of the game
		 */
		EventManager* GetEventManager() const;
        
        /**
         * Returns a pointer to the audio manager of the game
         */
        AudioManager* GetAudioManager() const;

		/**
		 * Returns a pointer to the player input event manager
		 */
		EventManager* GetPlayerInputEvtMgr() const;

		/**
		 * Returns a pointer to the used video system for rendering
		 */
		IVideoSystem* GetVideoSystem() const;

		/**
		 * Returns a pointer to the global process manager
		 */
		ProcessManager* GetProcessManager() const;
        
        /**
         * Returns the scene that is currently loaded in the 
         * game
         */
        Scene* GetCurrentScene() const;

		/**
		 * Returns a pointer to the global resource manager
         * that is responsible for providing files from the HD wisely
		 */
		ResourceManager* GetResourceManager() const;

		/**
		 * Logs a string according to the given priority
		 */
		void LogString(Logger::LOG_FLAG, string) const;

		/**
		 * Checks for notifications send by the operating system
		 */
		bool CheckForNotifications();
	};

}