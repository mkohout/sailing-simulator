#pragma once

#include <memory>

#include "RenderingStep.h"
#include "../../game_logic/actor_components/RenderComponent.h"
#include <fastdelegate/FastDelegate.h>

using std::shared_ptr;

namespace Game_View
{
    class Scene;
    
    /**
     * Rendering step that assigns multiple objects
     * to the renderer such as the camera, light sources, etc.
     */
    class AssignStep : public RenderingStep
    {
    private:
        /**
         * Assign stuff based on the given scene node.
         * Scene node might be a LightNode or a CameraNode.
         * If it doesn't have any particular pecularities, no action
         * will be executed
         */
        void AssignFromSceneNode(shared_ptr<ISceneNode>, unsigned int);
    
    public:
        AssignStep() = default;

        virtual void VInit() { }
        
        /**
         * Execute the callback function AssignFromSceneNode for every
         * SceneNode in the given SceneGraph
         */
        virtual void VExecute(SceneGraph*, unsigned int);
        
    };
}