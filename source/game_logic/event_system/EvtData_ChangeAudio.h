#pragma once

#include "IEventData.h"

#include <string>

namespace Game_Logic
{
    /**
     * Event that is triggered to manipulate playing audio
     */
    class EvtData_ChangeAudio : public IEventData
    {
    private:
        /**
         * Identifier of the sound that needs
         * to be manipulated
         */
        const std::string m_audioIdentifier;
        
        /**
         * The new volume level the audio will set to
         */
        const float m_newVolumeLevel;
        
    public:
        static const EventType s_eventType;
        
        explicit EvtData_ChangeAudio(const std::string&, const float&);
        
        virtual const EventType& VGetEventType() const
        {
            return s_eventType;
        }
        
        virtual IEventDataPtr VCopy() const
        {
            return IEventDataPtr(new EvtData_ChangeAudio(m_audioIdentifier, m_newVolumeLevel));
        }
        
        inline const float& GetNewVolumeLevel() const
        {
            return m_newVolumeLevel;
        }
        
        inline const std::string& GetAudioIdentifier() const
        {
            return m_audioIdentifier;
        }
    };
}