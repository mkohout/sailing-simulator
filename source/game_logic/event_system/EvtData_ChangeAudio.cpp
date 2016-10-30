#include "EvtData_ChangeAudio.h"

using namespace Game_Logic;

const EventType EvtData_ChangeAudio::s_eventType = 0x3213B9E5;

EvtData_ChangeAudio::EvtData_ChangeAudio(const std::string& audioIdentifier, const float& newVolumeLevel)
    : m_audioIdentifier(audioIdentifier), m_newVolumeLevel(newVolumeLevel)
{
    
}