#include "SoundThread.h"
#include "GameCore.h"

using namespace Application_Layer;

void SoundThread::VInit()
{
    Thread::VInit();
}

void SoundThread::VUpdate(
                    const unsigned int deltaMilliseconds)
{
    g_pGameApp->GetAudioManager()->Update(deltaMilliseconds);
}

const short SoundThread::VGetFramesForThread() const
{
    return std::stoi(
            g_pGameApp->GetGameSettings()->GetValue("soundRate"));
}