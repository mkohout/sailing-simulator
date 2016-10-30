#include "WorldSimulationThread.h"
#include "GameCore.h"

using namespace Application_Layer;

void WorldSimulationThread::VInit()
{
    Thread::VInit();
}

void WorldSimulationThread::VUpdate(
	const unsigned int deltaMilliseconds)
{
	const unsigned int halfTime = deltaMilliseconds / 2;

	g_pGameApp->GetProcessManager()->UpdateProcesses(halfTime);
	g_pGameApp->GetEventManager()->VUpdate(halfTime);
}

const short WorldSimulationThread::VGetFramesForThread() const
{
	return std::stoi(
		g_pGameApp->GetGameSettings()->GetValue("simulationRate"));
}