#pragma once

#include "Thread.h"

using std::string;

namespace Application_Layer
{
    /**
     * Thread that is taking care of the playback
     * of the sounds in the scene
     */
    class SoundThread : public Thread
    {
    public:
        SoundThread() { }
        
        virtual string VGetIdentifier()
        {
            return "SoundThread";
        }
        
        /**
         * Initializes the sound thread
         */
        virtual void VInit();
        
        /**
         * Simply call the fmod update every frame.
         * Fmod is taking care of the rest
         */
        virtual void VUpdate(const unsigned int);
        
        virtual void VStartThread()
        {
            _thread = thread(&SoundThread::VRun, this);
            
            _thread.detach();
        }
        
        /**
         * Returns the frame rate of the sound thread
         */
        virtual const short VGetFramesForThread() const;
        
        virtual ~SoundThread()
        {
        }
    };
    
}