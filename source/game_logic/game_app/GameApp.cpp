#include "GameApp.h"

#include "../../application_layer/core/GameCore.h"
#include "../actor_system/ActorFactory.h"
#include "../../application_layer/resources/XMLResource.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace Game_Logic;

GameApp::~GameApp()
{
    SAFE_DELETE(m_pAudioManager);
	SAFE_DELETE(m_pVideoSystem);
        SAFE_DELETE(m_pCurrentScene);
	SAFE_DELETE(m_pEventManager);
	SAFE_DELETE(m_pPlayerInputEvtMgr);
	SAFE_DELETE(m_pProcessManager);
	SAFE_DELETE(m_pGameSettings);
	SAFE_DELETE(m_pResourceManager);
}

void GameApp::StartGame()
{
	InitResourceManager();
	InitGameSettings();
	InitProcessManager();
	InitEventManager();
    InitAudioManager();
	InitScene();
    
    m_pGameCore->StartThreads();
    
	InitVideoSystem();
}

void GameApp::InitGameSettings()
{
	Settings* pGameSettings = new Settings();
	const string xmlFile = "game.xml";
	const string filename =
		m_pGameCore->GetFileManager()->GetPathToSettingsFile(xmlFile);
    StrongResourcePtr pResource =
                    g_pGameApp->GetResourceManager()->GetResource(filename, IResource::XML);
    shared_ptr<XMLResource> pGameSettingResource =
                    std::static_pointer_cast<XMLResource>(pResource);

	pGameSettingResource->VInitFromFile(filename);
	pGameSettings->InitFromXMLResource(pGameSettingResource);

	m_pGameSettings = pGameSettings;
}

void GameApp::InitResourceManager()
{
	m_pResourceManager = new ResourceManager();
	m_pResourceManager->Init();
}

void GameApp::InitScene()
{
	Scene* pScene = new Scene();

	const string xmlName =
		m_pGameSettings->GetValue("firstScene") + ".xml";
	const string logMessage =
		"Load scene from xml file named " + xmlName;
	const string filename =
		m_pGameCore->GetFileManager()->GetPathToSceneFile(xmlName);
    StrongResourcePtr pResource =
                GetResourceManager()->GetResource(filename, IResource::XML);
    shared_ptr<XMLResource> pSceneResource =
                        std::static_pointer_cast<XMLResource>(pResource);

	LogString(Logger::LOG_FLAG::INFO, logMessage);

	pSceneResource->VInitFromFile(filename);
	pScene->InitFromXMLResource(pSceneResource);

	m_pCurrentScene = pScene;
}

Settings* GameApp::GetGameSettings() const
{
	return m_pGameSettings;
}

void GameApp::LogString(Logger::LOG_FLAG logFlag, string logMessage) const
{
	m_pGameCore->LogString(logFlag, logMessage);
}

void GameApp::Shutdown()
{
    m_pGameCore->StopThreads();
	ActorFactory::DestroyInstance();
}

void GameApp::RenderCurrentScene(const unsigned int deltaMilliseconds) const
{
    m_pCurrentScene->UpdateForces(deltaMilliseconds);
	m_pVideoSystem->VPreRender();
	if(m_pCurrentScene->GetSceneGraph()->GetRootNode() != nullptr)
    {
		m_pVideoSystem->VGetRenderer()->StartRendering(deltaMilliseconds, m_pCurrentScene->GetSceneGraph());
    }
	m_pVideoSystem->VPostRender();
}

bool GameApp::CheckForNotifications()
{
	return m_pVideoSystem->VCheckForNotifications();
}

void GameApp::InitEventManager()
{
	m_pEventManager = new EventManager();
	m_pPlayerInputEvtMgr = new EventManager();
}

void GameApp::InitVideoSystem()
{
	m_pVideoSystem = new OpenGLVideoSystem();
	m_pVideoSystem->VInit();
}

void GameApp::InitProcessManager()
{
	m_pProcessManager = new ProcessManager();
}

void GameApp::InitAudioManager()
{
    m_pAudioManager = new AudioManager();
    m_pAudioManager->Init();
}

AudioManager* GameApp::GetAudioManager() const
{
    return m_pAudioManager;
}

FileManager* GameApp::GetFileManager() const
{
	return m_pGameCore->GetFileManager();
}

IVideoSystem* GameApp::GetVideoSystem() const
{
	return m_pVideoSystem;
}

EventManager* GameApp::GetEventManager() const
{
	return m_pEventManager;
}

EventManager* GameApp::GetPlayerInputEvtMgr() const
{
	return m_pPlayerInputEvtMgr;
}

ProcessManager* GameApp::GetProcessManager() const
{
	return m_pProcessManager;
}

ResourceManager* GameApp::GetResourceManager() const
{
	return m_pResourceManager;
}

Scene* GameApp::GetCurrentScene() const
{
    return m_pCurrentScene;
}
