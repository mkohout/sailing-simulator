#include "AudioManager.h"
#include "../../application_layer/core/GameCore.h"

using namespace Game_View;

const unsigned int AudioManager::m_maximumNumberOfChannels = 100;

AudioManager::~AudioManager()
{
    FMOD_System_Release(m_system);
}

void AudioManager::Init()
{
    InitListeners();
    
    FMOD_System_Create(&m_system);
    FMOD_System_Init(
                     m_system,
                     m_maximumNumberOfChannels,
                     FMOD_INIT_NORMAL,
                     0
                     );
}

void AudioManager::InitListeners()
{
    EventListenerDelegate changeAudioCallback =
            fastdelegate::MakeDelegate(
                               this,
                               &AudioManager::ChangeAudio
                               );
    
    
    g_pGameApp->GetPlayerInputEvtMgr()->VAddListener(
                                                     changeAudioCallback,
                                                     EvtData_ChangeAudio::s_eventType);
}

void AudioManager::ChangeAudio(IEventDataPtr pEventData)
{
    if(!dynamic_cast<EvtData_ChangeAudio*>(pEventData.get()))
    {
        return ;
    }
    
    EvtData_ChangeAudio* pCastEventData =
                    dynamic_cast<EvtData_ChangeAudio*>(pEventData.get());
    
    ChangeVolumeOfSoundFile(
            pCastEventData->GetAudioIdentifier(), pCastEventData->GetNewVolumeLevel());
}

void AudioManager::ChangeVolumeOfSoundFile(
                const std::string audioIdentifier, const float newVolumeLevel)
{
    for(StrongSoundPtr pSound : m_sounds)
    {
        if(pSound->GetIdentifier().compare(audioIdentifier)!=0)
        {
            continue;
        }
        
        pSound->SetVolumeTo(newVolumeLevel);
    }
}

void AudioManager::LoadSoundFile(StrongSoundPtr pSound)
{
    if(!pSound->DuplicatesAllowed())
    {
        if(SoundFileAlreadyPlaying(pSound->GetFilename()))
        {
            return ;
        }
    }
    
    pSound->Init(m_system);
    
    m_sounds.push_back(pSound);
}

bool AudioManager::SoundFileAlreadyPlaying(const std::string filename) const
{
    for(StrongSoundPtr pSound : m_sounds)
    {
        if(pSound->GetFilename().compare(filename)==0)
        {
            return true;
        }
    }
    
    return false;
}

void AudioManager::Update(
            const unsigned int& deltaMilliseconds)
{
    FMOD_System_Update(m_system);
}

void AudioManager::PlayAllSounds()
{
    for(StrongSoundPtr pSound : m_sounds)
    {
        pSound->Play(m_system);
    }
}

void AudioManager::PlaySoundFile(StrongSoundPtr pSound)
{
    LoadSoundFile(pSound);
    
    pSound->Play(m_system);
}