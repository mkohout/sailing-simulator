#include "GameCore.h"

#include "WorldSimulationThread.h"
#include "PhysicSimulationThread.h"
#include "PlayerInputThread.h"
#include "SoundThread.h"

using namespace Application_Layer;
using namespace Game_Logic;

GameCore* GameCore::s_pSingleton = nullptr;

GameCore::~GameCore()
{
    SAFE_DELETE(m_pGameApp);
	SAFE_DELETE(m_pFileManager);
    SAFE_DELETE(m_pLogger);
}

void GameCore::Init()
{
	InitLogger();
	InitFileManager();

	m_pGameApp = new GameApp();
	m_pGameApp->SetGameCore(this);
}

void GameCore::InitFileManager()
{
	m_pFileManager = new FileManager();
    
    m_pFileManager->Init();
}

void GameCore::InitLogger()
{
	m_pLogger = new Logger();
}

void GameCore::LogString(
	Logger::LOG_FLAG logFlag, string logMessage) const
{
	m_pLogger->Log(logFlag, logMessage);
}

void GameCore::StartThreads()
{
	// Audio Thread
    StartPhysicSimulation();
	StartWorldSimulation();
	StartPlayerInputListener();
	StartSoundThread();
}

void GameCore::StopThreads()
{
    for(auto& mapEntry : m_threadList)
    {
        mapEntry.second->Kill();
        mapEntry.second->WaitForThread();
    }
}

void GameCore::StartPhysicSimulation()
{
    shared_ptr<Thread> physicThread(
        new PhysicSimulationThread());
    
    physicThread->VStartThread();
    
    AddToThreadList(physicThread);
}


void GameCore::StartSoundThread()
{
    shared_ptr<Thread> soundThread(
                    new SoundThread());
    
    soundThread->VStartThread();
    
    AddToThreadList(soundThread);
}

void GameCore::StartWorldSimulation()
{
	shared_ptr<Thread> worldSimulationThread(
		new WorldSimulationThread());

	worldSimulationThread->VStartThread();

	AddToThreadList(worldSimulationThread);
}

void GameCore::StartPlayerInputListener()
{
	shared_ptr<Thread> playerInputThread(
		new PlayerInputThread());

	playerInputThread->VStartThread();

	AddToThreadList(playerInputThread);
}

void GameCore::AddToThreadList(const ThreadPtr pThread)
{
	m_threadList.insert(
		pair<string, ThreadPtr>(pThread->VGetIdentifier(), pThread));
}

bool GameCore::RemoveFromThreadList(string threadName)
{
	auto findIt = m_threadList.find(threadName);
	if (findIt == m_threadList.end())
	{
		return false;
	}

	m_threadList.erase(findIt);
	return true;
}

ThreadPtr GameCore::GetThread(string threadName)
{
	auto findIt = m_threadList.find(threadName);

	if (findIt == m_threadList.end())
	{
		return nullptr;
	}

	return findIt->second;
}