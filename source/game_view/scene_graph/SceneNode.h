#pragma once

#include <memory>

#include "../../game_logic/actor_system/Actor.h"
#include "SceneGraph.h"
#include "ISceneNode.h"

#include "../../application_layer/core/stdafx.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace Game_Logic;
using namespace glm;
using std::shared_ptr;
using std::vector;

namespace Game_View
{

	class SceneGraph;

	/**
	 * Representating a node in the scene graph
	 */
	class SceneNode : public ISceneNode
	{
	private:
		/**
		 * The actor of the current node.
         * A scene node can hold only one actor
		 */
		StrongActorPtr m_pActor;

		/**
		 * The child nodes of the current node. A scene node
         * can have unlimited childs
		 */
        SceneNodes m_childNodes;

		/**
		 * The transformation matrix of the current node.
         * If the actor in the scene node moves, the actors
         * geometry won't be modified. Only the transformation
         * matrix of the scene node will be manipulated
		 */
		mat4 m_transform;
        
        /**
         * Pointer to the parent of the current scene node
         */
        StrongSceneNodePtr m_pParent;

	public:

		SceneNode() : m_childNodes()
		{
			m_pActor = nullptr;
			m_transform = mat4(1.0f); // Identity
		}
        
        virtual ~SceneNode()
        {
        }

		virtual void VSetActor(StrongActorPtr);

		void SetPosition(vec3);

		void SetPosition(double, double, double);

		/**
		 * Initializes the scene node by the given xml element
		 */
		virtual void VInitByXMLElement(StrongXMLElementPtr);

		/**
		 * Adds a child node to the current scene node
		 */
		virtual void VAddChildNode(StrongSceneNodePtr pSceneNode);

		/**
		 * Removes the given child from the scene node
		 */
		virtual void VRemoveChildNode(StrongSceneNodePtr pSceneNode);

		/**
		 * Overrides the current transformation matrix by the given one
		 */
		virtual void VSetTransformationMatrix(const mat4&);

		/**
		 * Will transform the current scene node by multiplying the
		 * current transformation matrix with the given matrix
		 */
		virtual void VTransform(const mat4&);

		/**
		 * Returns the current transformation matrix of the scene node
		 */
		virtual const mat4 VGetTransformationMatrix() const;

		/**
		 * Returns all children of the current scene node
		 */
        virtual SceneNodes& VGetAllChildren();
        
        /**
         * Returns true if the scene node has any children
         */
        virtual bool VHasChildren() const
        {
            return m_childNodes.size()>0;
        }

		/**
		 * Returns the actor of the current scene node,
		 * if the scene node holds an actor
		 */
		virtual StrongActorPtr VGetActor() const;
        
        /**
         * Returns the position of the current scene node
         */
        virtual vec3 VGetPosition() const;

		/**
		 * Returns true if the current scene node holds an actor
		 */
		virtual bool VHasActor() const;
        
        /**
         * Sets the parent of the current scene node to the 
         * given
         */
        virtual void VSetParent(StrongSceneNodePtr);
        
        /**
         * Returns the pointer to the parent of the current scene node
         */
        virtual StrongSceneNodePtr VGetParent() const
        {
            return m_pParent;
        }
	};

}