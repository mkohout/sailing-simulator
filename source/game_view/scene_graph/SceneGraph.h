#pragma once

#include <memory>

#include "ISceneNode.h"
#include "SceneGroup.h"
#include "../../game_logic/actor_system/Actor.h"

using std::shared_ptr;

namespace Game_View
{
	typedef shared_ptr<ISceneNode> StrongSceneNodePtr;

	class SceneNode;

	/**
	 * Class is representing the hierarchy of the scene
	 * represented as a tree
	 */
	class SceneGraph
	{
	private:
		/**
		  * The root of the scene graph
		  */
		StrongSceneNodePtr m_pVirtualUniverse;

		/**
		  * Init the root element of the SceneGraph
		  */
		void InitVirtualUniverse();
        
        /**
         * Searches in the given scene node (and its childs) for an actor with
         * the given ActorId
         */
        StrongSceneNodePtr SearchForActorInNode(ActorId, StrongSceneNodePtr);

	public:
		SceneGraph()
		{

		}
        
        ~SceneGraph()
        {
        }

		/**
		  * Adds a note to the current scene graph on top level => below the virtual universe
		  */
		void AddNode(StrongSceneNodePtr pSceneNode);
        
        /**
         * Returns the actor with the given actor id if there is
         * an actor with this id.
         */
        StrongSceneNodePtr FindNodeWithActor(ActorId);

		/**
		 * Returns the root node of the scene graph
		 */
		inline const StrongSceneNodePtr GetRootNode()
		{
			return m_pVirtualUniverse;
		}
	};

}