#pragma once

#include "../../application_layer/resources/XMLResource.h"
#include "../../application_layer/resources/SoundResource.h"

#include <string>
#include <memory>
#include <fmod/fmod.h>

using namespace Application_Layer;

namespace Game_View
{
    /**
     * Class that encapsulates the functionality of a single
     * sound file that can be played by the audio manager
     */
    class Sound
    {
    private:
        /**
         * Identifier of the current sound
         */
        std::string m_identifier;
        
        /**
         * Indicates whether the current sound is only allowed
         * once in the current scene - or not.
         */
        bool m_duplicatesAllowed;
        
        /**
         * Indicates whether the sound loops or stops after
         * playing one time
         */
        unsigned int m_loopMode;
        
        /**
         * The level of the volume of the current sound.
         * 1.0f = 100%
         */
        float m_volumeLevel;
        
        StrongResourcePtr m_pResource;
        
        StrongResourcePtr m_pSoundResource;
        
        /**
         * The filename of the sound file the current sound
         * is playing
         */
        std::string m_filename;
        
        /**
         * The belonging fmod channel
         */
        FMOD_CHANNEL* m_channel;
        
        /**
         * The underlying resource structure of fmod
         */
        FMOD_SOUND* m_sound;
        
        /**
         * Loads the current sound from the given resource by
         * using the given fmod system
         */
        void LoadFromResource(FMOD_SYSTEM*, StrongResourcePtr);
        
        /**
         * Initializes the properties of the current sound by the given
         * xml element
         */
        void InitPropertiesFromXMLElement(StrongXMLElementPtr);
        
        /**
         * Returns a pointer to the xml file that specifies the properties
         * of the current sound
         */
        StrongResourcePtr GetSoundSpecification(const string&);
        
    public:
        Sound() = default;
        ~Sound() { }
        
        /**
         * Initializes the sound by loading it from the hard disk
         * and storing it in the memory
         */
        void Init(FMOD_SYSTEM*);
        
        /**
         * Loads the current sound from the given xml element
         */
        void LoadFromXMLElement(StrongXMLElementPtr);
        
        /**
         * Load the sound data from the sound file with the given
         * name.
         */
        void LoadSoundData(const std::string);
        
        /**
         * Returns the filename of the current soundfile
         */
        const std::string& GetFilename() const
        {
            return m_filename;
        }
        
        FMOD_SOUND* GetSound() const
        {
            return m_sound;
        }
        
        /**
         * Returns the identifier of the current sound
         */
        const std::string GetIdentifier() const
        {
            return m_identifier;
        }
        
        /**
         * Starts the playback of the current sound
         */
        void Play(FMOD_SYSTEM*);
        
        /**
         * Returns true if duplicates of the same sound are allowed in
         * the scene
         */
        bool DuplicatesAllowed() const
        {
            return m_duplicatesAllowed;
        }
        
        /**
         * Overwrites the volume level of the current sound to the given
         */
        void SetVolumeTo(const float);
    };
    
    typedef std::shared_ptr<Sound> StrongSoundPtr;
}