#pragma once

#include "Thread.h"
#include "stdafx.h"
#include "GameCore.h"
#include <btBulletDynamicsCommon.h>

using std::string;

namespace Application_Layer
{
    /**
     * Thread that will iterate over the
     * actor list of the current scene and
     * apply the forces that are acting in the
     * scene on these actors
     */
    class PhysicSimulationThread : public Thread
    {
    public:
        PhysicSimulationThread() { }
        
        virtual string VGetIdentifier()
        {
            return "PhysicSimulationThread";
        }
        
        /**
         * Initializes the physics thread
         */
        virtual void VInit();
        
        /**
         * Update function that is called every frame
         * in the physics thread
         */
        virtual void VUpdate(const unsigned int);
        
        virtual void VStartThread()
        {
            _thread = thread(&PhysicSimulationThread::VRun, this);
            
            _thread.detach();
        }
        
        /**
         * Returns the frame rate for the physics thread
         * that was set in the config
         */
        virtual const short VGetFramesForThread() const;
        
        virtual ~PhysicSimulationThread()
        {
			g_pGameApp;
        }
	private:
		virtual void Update(const unsigned int, StrongSceneNodePtr);
    };
    
}