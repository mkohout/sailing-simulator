#include "Sound.h"

#include "../../application_layer/resources/SoundResource.h"
#include "../../application_layer/core/GameCore.h"

using namespace Game_View;

void Sound::LoadFromResource(FMOD_SYSTEM* pSystem, StrongResourcePtr pResource)
{
    if(pResource==nullptr)
    {
        return ;
    }
    
    SoundResource* pSoundResource =
                dynamic_cast<SoundResource*>(pResource.get());
    
    m_sound = pSoundResource->GetSound(pSystem, m_loopMode);
}

StrongResourcePtr Sound::GetSoundSpecification(const string& identifier)
{
    const string soundSpecFile = identifier + ".xml";
    const string filename =
                g_pGameApp->GetFileManager()->GetPathToSoundSpecFile(soundSpecFile);
    
    if (!g_pGameApp->GetFileManager()->FileExists(filename))
    {
        throw "Sound specification file does not exist";
        return nullptr;
    }
    
    StrongResourcePtr pResource =
                g_pGameApp->GetResourceManager()->GetResource(filename, IResource::XML);
    
    return pResource;
}

void Sound::LoadSoundData(const std::string identifier)
{
    StrongResourcePtr pSoundResource =
                    GetSoundSpecification(identifier);
    shared_ptr<XMLResource> pSoundSpec(
                        dynamic_cast<XMLResource*>(
                            pSoundResource.get()));
    
    if(pSoundSpec==nullptr)
    {
        return ;
    }
    
    StrongXMLElementPtr pSoundElement =
                    pSoundSpec->GetRootElement()->FindFirstChildNamed("sound");
    
    if(pSoundElement==nullptr)
    {
        return ;
    }
    
    InitPropertiesFromXMLElement(pSoundElement);

    m_identifier = identifier;
    m_filename = pSoundElement->GetValueOfAttribute("file");
    
    m_pResource = pSoundResource;
}

void Sound::LoadFromXMLElement(StrongXMLElementPtr pXmlElement)
{
    string soundIdentifier =
            pXmlElement->GetValueOfAttribute("identifier");
    
    LoadSoundData(soundIdentifier);
}

void Sound::InitPropertiesFromXMLElement(StrongXMLElementPtr pXmlElement)
{
    if(pXmlElement->HasAttributeNamed("loop"))
    {
        m_loopMode = FMOD_LOOP_NORMAL;
    }
    else
    {
        m_loopMode = FMOD_DEFAULT;
    }
    
    if(pXmlElement->HasAttributeNamed("noDuplicates"))
    {
        m_duplicatesAllowed = false;
    }
    else
    {
        m_duplicatesAllowed = true;
    }
    
    if(pXmlElement->HasAttributeNamed("volume"))
    {
        m_volumeLevel =
                std::stof(pXmlElement->GetValueOfAttribute("volume"));
    }
    else
    {
        m_volumeLevel = 1.0f;
    }
}

void Sound::Init(FMOD_SYSTEM* pSystem)
{
    std::string filePath =
                        g_pGameApp->GetFileManager()->GetPathToSoundFile(m_filename);
    StrongResourcePtr pResource =
                        g_pGameApp->GetResourceManager()->GetResource(
                                                  filePath, IResource::SOUND);
    
    m_pSoundResource = pResource;
    
    LoadFromResource(pSystem, pResource);
}

void Sound::SetVolumeTo(const float newVolume)
{
    m_volumeLevel = newVolume;
    FMOD_Channel_SetVolume(m_channel, m_volumeLevel);
}

void Sound::Play(FMOD_SYSTEM* pSystem)
{
    // Start paused
    
    FMOD_System_PlaySound(
                pSystem, m_sound, nullptr, true, &m_channel);
    
    SetVolumeTo(m_volumeLevel);
    
    FMOD_Channel_SetPaused(m_channel, false);
}