#include "PlayerInputThread.h"
#include "GameCore.h"

using namespace Application_Layer;

void PlayerInputThread::VInit()
{
    Thread::VInit();
}

void PlayerInputThread::VUpdate(
	const unsigned int deltaMilliseconds)
{
	g_pGameApp->GetPlayerInputEvtMgr()->VUpdate(
		deltaMilliseconds);
}

const short PlayerInputThread::VGetFramesForThread() const
{
	return std::stoi(
		g_pGameApp->GetGameSettings()->GetValue("inputRate"));
}