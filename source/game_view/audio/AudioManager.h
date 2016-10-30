#pragma once

#include <vector>
#include <fmod/fmod.h>
#include "Sound.h"
#include "../../game_logic/event_system/EvtData_ChangeAudio.h"

using std::vector;

using namespace Game_Logic;

namespace Game_View
{
    /**
     * Data structure to handle several sounds
     */
    typedef vector<StrongSoundPtr> Sounds;
    
    /**
     * Class that handles the playback of multiple kinds
     * of audio files
     */
    class AudioManager
    {
    private:
        /**
         * Maximum number of sounds that can be played in parallel
         */
        static const unsigned int m_maximumNumberOfChannels;
        
        /**
         * Sounds that are currently played
         */
        Sounds m_sounds;
        
        /**
         * Pointer to the underlying FMOD library
         */
        FMOD_SYSTEM* m_system;
        
        /**
         * Initializes the event listener that reacts to the
         * ChangeAudio event that can manipulate the volume of
         * specific sound effects
         */
        void InitListeners();
        
        /**
         * Callback that is executed whenever the event mentioned
         * above is triggered
         */
        void ChangeAudio(IEventDataPtr);
        
        /**
         * Change the volume of the sound identified by the given string
         * to the new value
         */
        void ChangeVolumeOfSoundFile(const std::string, const float);
        
    public:
        AudioManager() = default;
        
        ~AudioManager();
        
        /**
         * Initializes the audio manager by creating the fmod system
         * instance
         */
        void Init();
        
        /**
         * Loads the given sound file into the data structure,
         * but does not start immediately playing that sound
         */
        void LoadSoundFile(StrongSoundPtr);
        
        /**
         * Loads the given sound file and plays it right away
         */
        void PlaySoundFile(StrongSoundPtr);
        
        /**
         * Keeps the audio manager working,
         * called by the audio thread
         */
        void Update(const unsigned int&);
        
        /**
         * Returns true if a sound with the given identifier
         * is already playing currently
         */
        bool SoundFileAlreadyPlaying(const std::string) const;
        
        /**
         * Play all sounds stored in the data structure.
         * this method is called when the scene was initialized 
         * properly
         */
        void PlayAllSounds();
    };
}