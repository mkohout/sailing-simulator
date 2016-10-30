#include "SoundResource.h"
#include <fmod/fmod.h>

using namespace Application_Layer;

string SoundResource::GetFilename() const
{
    return m_filename;
}

void SoundResource::VInitFromFile(const string& filename)
{
    m_filename = filename;
}

void SoundResource::LoadSound(
                FMOD_SYSTEM* pSystem, const unsigned int loopMode)
{
    FMOD_System_CreateSound(
                pSystem, m_filename.c_str(), loopMode, 0, &m_sound);
}

FMOD_SOUND* SoundResource::GetSound(
                FMOD_SYSTEM* pSystem, const unsigned int loopMode)
{
    if(m_sound==nullptr)
    {
        LoadSound(pSystem, loopMode);
    }
    
    return m_sound;
}