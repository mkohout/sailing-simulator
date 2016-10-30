#pragma once

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>

#include "../../application_layer/core/stdafx.h"

#include "../../game_logic/actor_system/Actor.h"

using namespace glm;
using namespace Game_Logic;

using std::vector;
using std::shared_ptr;

namespace Game_View
{
    class ISceneNode;
    
    typedef shared_ptr<ISceneNode> StrongSceneNodePtr;
    
    /**
     * Data structure to save the child nodes
     */
    typedef vector<StrongSceneNodePtr> SceneNodes;
    
    /**
     * Basic class for the scene nodes
     */
    class ISceneNode
    {
    public:
        /**
         * Initializes a scene node by given xml element
         */
        virtual void VInitByXMLElement(StrongXMLElementPtr) = 0;
        
        /**
         * Adds a child node to the current scene node
         */
        virtual void VAddChildNode(StrongSceneNodePtr pSceneNode) = 0;
        
        /**
         * Removes a child node from the current scene node
         */
        virtual void VRemoveChildNode(StrongSceneNodePtr pSceneNode) = 0;
        
        /**
         * Sets the transformation matrix for the current scene node
         */
        virtual void VSetTransformationMatrix(const mat4&) = 0;
        
        /**
         * Will transform the scene node by multiplying the current transformation matrix
         * with the given new one
         */
        virtual void VTransform(const mat4&) = 0;
        
        /**
         * Sets the parent of the current node to the given
         */
        virtual void VSetParent(StrongSceneNodePtr) = 0;
        
        /**
         * Returns a pointer to the parent of the current node
         */
        virtual StrongSceneNodePtr VGetParent() const = 0;
        
        /**
         * Returns the position of the current scene node
         */
        virtual vec3 VGetPosition() const = 0;
        
        /**
         * Returns the current transformation matrix
         */
        virtual const mat4 VGetTransformationMatrix() const = 0;
        
        /**
         * Returns all children of the current scene node
         */
        virtual SceneNodes& VGetAllChildren() = 0;
        
        /**
         * Returns true if the current node has any children
         */
        virtual bool VHasChildren() const = 0;
        
        /**
         * Returns the actor of the current scene node
         */
        virtual StrongActorPtr VGetActor() const = 0;
        
        /**
         * Adds an actor to the current node. An actor can have at most one actor
         */
        virtual void VSetActor(StrongActorPtr) = 0;
        
        /**
         * Returns true if the current scene node is holding an actor
         */
        virtual bool VHasActor() const = 0;
    };
}