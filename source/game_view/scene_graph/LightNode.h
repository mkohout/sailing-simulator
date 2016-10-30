#pragma once

#include "ISceneNode.h"

#include <glm/vec3.hpp>

namespace Game_View
{
    /**
     * Enum for the different kinds of light,
     * that are supported by the engine
     */
    typedef enum {
        UNKNOWN,
        POINT_LIGHT,
        SPOT_LIGHT,
        DIRECTIONAL_LIGHT
    } LIGHT_TYPE;
    
    /**
     * Base class for different kinds of lights
     */
    class LightNode : public ISceneNode
    {
    private:
        /**
         * Type of the current light
         */
        LIGHT_TYPE m_type;
        
        /**
         * A light cannot store any child nodes, list will always be empty
         */
        SceneNodes m_childNodes;
        
        /**
         * Transformation matrix of the current light node
         */
        mat4 m_transform;
        
        /**
         * Direction of the current light - only if its a directional light
         */
        vec3 m_direction;
        
        /**
         * Color of the current light
         */
        vec3 m_color;
        
        /**
         * Energy of the current light
         */
        float m_energy;
        
        /**
         * A pointer to the parent scene node of the current light node
         */
        StrongSceneNodePtr m_pParent;
        
    public:
        LightNode()
        {
            m_type = UNKNOWN;
            m_transform = mat4(1.0f);
        }
        
        virtual ~LightNode()
        {
        }
        
        /**
         * Initializes the current light by the given xml element
         */
        virtual void VInitByXMLElement(StrongXMLElementPtr);
        
        /**
         * LightNodes cannot have child nodes
         */
        virtual void VAddChildNode(StrongSceneNodePtr pSceneNode) { }
        
        /**
         * LightNodes cannot have child nodes
         */
        virtual void VRemoveChildNode(StrongSceneNodePtr pSceneNode) { }
        
        /**
         * Overrides the transformation matrix by the given one
         */
        virtual void VSetTransformationMatrix(const mat4&);
        
        /**
         * Applies a transformation matrix on the position of the light node,
         * so that the light can change its position during runtime
         */
        virtual void VTransform(const mat4&);
        
        /**
         * Returns the current transformation matrix of the light node
         */
        virtual const mat4 VGetTransformationMatrix() const;
        
        /**
         * Lightnodes cannot have any children,
         * will return an empty list
         */
        virtual SceneNodes& VGetAllChildren();
        
        /**
         * LightNodes cannot have child nodes
         */
        virtual bool VHasChildren() const
        {
            return false;
        }
        
        /**
         * Lightnode cannot have an actor
         */
        virtual StrongActorPtr VGetActor() const;
        
        /**
         * Lightnode cannot have an actor
         */
        virtual bool VHasActor() const;
        
        /**
         * No actor is allowed in a light node, so this method
         * is a stub
         */
        virtual void VSetActor(StrongActorPtr);
        
        /**
         * Returns true, if the light node represents a legal type
         */
        bool HasType() const
        {
            return m_type!=UNKNOWN;
        }
        
        /**
         * Matches the strings against the values in the enum
         */
        void SetTypeByString(string);
        
        /**
         * Returns the type of the current light
         */
        const LIGHT_TYPE GetType() const
        {
            return m_type;
        }
        
        void SetDirection(vec3 direction)
        {
            m_direction = direction;
        }
        
        void SetDirection(float x, float y, float z)
        {
            SetDirection(vec3(x, y, z));
        }
        
        void SetEnergy(float energy)
        {
            m_energy = energy;
        }
        
        void SetColor(vec3 color)
        {
            m_color = color;
        }
        
        void SetColor(float r, float g, float b)
        {
            SetColor(vec3(r, g, b));
        }
        
        const vec3 GetColor() const
        {
            return m_color;
        }
        
        virtual vec3 VGetPosition() const;
        
        /**
         * Returns the direction of the current light
         * - only if the current light is a directional light
         */
        const vec3 GetDirection() const
        {
            return m_direction;
        }
        
        const float GetEnergy() const
        {
            return m_energy;
        }
        
        /**
         * Sets the given scene node as the parent of the current light node
         */
        virtual void VSetParent(StrongSceneNodePtr);
        
        /**
         * Returns a pointer to the parent of the current light node
         */
        virtual StrongSceneNodePtr VGetParent() const
        {
            return m_pParent;
        }
    };
}