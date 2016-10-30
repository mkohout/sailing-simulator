#pragma once

#include "ISceneNode.h"
#include "SceneGraph.h"

namespace Game_View
{
	class SceneGraph;

	/**
	 * Class for representing the scene group
	 * Scene group is not able to store an actor directly
	 * scene group will hold scene nodes, that will hold the actors
	 */
	class SceneGroup : public ISceneNode
	{
	private:
		/**
		 * Child nodes of the current scene group
		 */
        SceneNodes m_childNodes;
        
        /**
         * The transformation matrix of the current scene group
         */
        mat4 m_transform;
        
        /**
         * A pointer to the parent of the current scene group
         */
        StrongSceneNodePtr m_pParent;

	public:
		SceneGroup() : m_childNodes()
		{
            m_transform = mat4(1.0f);
		}

		/**
		 * Initializes the scene group by given xml element
		 */
		virtual void VInitByXMLElement(StrongXMLElementPtr);

		/**
		 * Will add a child node to the current scene node
		 */
		virtual void VAddChildNode(StrongSceneNodePtr pSceneNode);

		/**
		 * Will remove the given child from the list of current child nodes
		 */
		virtual void VRemoveChildNode(StrongSceneNodePtr pSceneNode);

		/**
		 * Will override the current transformation matrix by simply
		 * replacing the transformation matrix of every child with the given one
		 */
		virtual void VSetTransformationMatrix(const mat4&);

		/**
		 * Will transform the current node by multiplying the
		 * transformation matrix of all children with the given
		 */
		virtual void VTransform(const mat4&);

		/**
		 * Returns the transformation matrix of the curent scene node
		 */
		virtual const mat4 VGetTransformationMatrix() const;
        
        /**
         * Returns the position of the current scene group
         */
        virtual vec3 VGetPosition() const;

		/**
		 * Returns all children of a scene node
		 */
		virtual SceneNodes& VGetAllChildren();
        
        /**
         * Returns true if there are any children
         */
        virtual bool VHasChildren() const
        {
            return m_childNodes.size()>0;
        }

		/**
		 * SceneGroup cannot have an actor.
		 */
		virtual StrongActorPtr VGetActor() const;

		/**
		 * SceneGroup cannot have an actor.
		 */
		virtual bool VHasActor() const;

		/**
		 * SceneGroup cannot have an actor.
		 */
		virtual void VSetActor(StrongActorPtr);
        
        /**
         * Sets the given scene node as the parent of the current
         * scene group
         */
        virtual void VSetParent(StrongSceneNodePtr);
        
        /**
         * Returns the pointer to the parent of the current
         * scene group
         */
        virtual StrongSceneNodePtr VGetParent() const
        {
            return m_pParent;
        }
	};
}