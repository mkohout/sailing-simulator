#pragma once

#include <memory>

#include "RenderingStep.h"

using std::shared_ptr;

namespace Game_View
{
    /**
     * Functionality that will be executed right after the
     * renderer has rendered the scene to the screen
     */
    class PostRenderingStep : public RenderingStep
    {
    public:
        PostRenderingStep() = default;
        
        virtual void VInit() { }
        
        virtual void VExecute(SceneGraph*, const unsigned int);
    };
}