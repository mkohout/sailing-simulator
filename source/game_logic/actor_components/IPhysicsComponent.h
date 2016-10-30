#pragma once

namespace Game_Logic
{
    /**
     * Base component for all actor components
     * that will be updated by the PhysicsThread
     */
    class IPhysicsComponent
    {
    public:
        virtual void VUpdate(const unsigned int) = 0;
    };
    
}