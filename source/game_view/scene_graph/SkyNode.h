#pragma once

#include "ISceneNode.h"
#include "../../game_logic/event_system/IEventManager.h"

namespace Game_View
{
    /**
     * Representing Skyboxes
     */
    class SkyNode : public ISceneNode
    {
    private:
        /**
         * A sky node cannot have any children. List will always be empty
         */
        SceneNodes m_childNodes;
        
        /**
         * Transformation matrix of a sky node will always be the identity matrix
         */
        mat4 m_transform;
        
        /**
         * The actor of the skynode.
         * Most likely a cube with a cubemap texture assigned
         */
        StrongActorPtr m_pActor;
        
        /**
         * Pointer to the parent of the current sky node
         */
        StrongSceneNodePtr m_pParent;
        
    public:
        /**
         * Initializes the sky node by setting
         * the transformation matrix to the identity matrix
         */
        SkyNode() : m_childNodes()
        {
            m_transform = mat4(1.0f);
        }
        
        /**
         * Initializes the sky node by a given xml element
         */
        virtual void VInitByXMLElement(StrongXMLElementPtr);
        
        /**
         * SkyNodes cannot have child nodes
         */
        virtual void VAddChildNode(StrongSceneNodePtr pSceneNode) { }
        
        /**
         * SkyNodes cannot have child nodes
         */
        virtual void VRemoveChildNode(StrongSceneNodePtr pSceneNode) { }
        
        /**
         * SkyNodes cannot have child nodes
         */
        virtual bool VHasChildren() const
        {
            return false;
        }
        
        /**
         * Transformation matrix of sky node is always identity matrix
         */
        virtual void VSetTransformationMatrix(const mat4&);
        
        /**
         * Transformation matrix of sky node is always identity matrix
         */
        virtual void VTransform(const mat4&);
        
        /**
         * Returns the transformation matrix of the sky node, which is always the identity matrix
         */
        virtual const mat4 VGetTransformationMatrix() const;
        
        /**
         * A sky node doesnt have any children. Will return an empty container.
         */
        virtual SceneNodes& VGetAllChildren();
        
        virtual StrongActorPtr VGetActor() const;
        
        virtual void VSetActor(StrongActorPtr);
        
        virtual bool VHasActor() const;
        
        /**
         * Returns the position of the current sky node
         */
        virtual vec3 VGetPosition() const;
        
        /**
         * Sets the parent of the current sky node to the given
         */
        virtual void VSetParent(StrongSceneNodePtr);
        
        /**
         * Returns a pointer to the parent of the current SkyNode
         */
        virtual StrongSceneNodePtr VGetParent() const
        {
            return m_pParent;
        }
    };
}