#pragma once

#include "IResource.h"
#include <fmod/fmod.h>
#include <boost/filesystem.hpp>

namespace Application_Layer
{
    /**
     * Resource that is holding the binary data
     * of a sound file
     */
    class SoundResource : public IResource
    {
    private:
        string m_filename;
        
        /**
         * The data structure fmod is using for 
         * holding the sound data
         */
        FMOD_SOUND* m_sound;
        
    public:
        static const TYPE s_resourceType = SOUND;
        
        SoundResource() = default;
        
        virtual void VDispose()
        {
            FMOD_Sound_Release(m_sound);
        }
        
        virtual ~SoundResource()
        {
            VDispose();
        }
        
        virtual void VInitFromFile(const string&);
        
        string GetFilename() const;
        
        string GetBasename() const
        {
            boost::filesystem::path pathname(m_filename);
            
            return pathname.filename().string();
        }
        
        /**
         * Loads the sound data from the file
         */
        void LoadSound(FMOD_SYSTEM*, const unsigned int);
        
        FMOD_SOUND* GetSound(FMOD_SYSTEM*, const unsigned int);
        
        virtual TYPE VGetType() const
        {
            return s_resourceType;
        }
    };
}