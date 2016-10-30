#pragma once

#include <memory>
#include <vector>
#include <glm/vec3.hpp>

#include "../../application_layer/core/stdafx.h"

#include "ActorFactory.h"
#include "../actor_components/ActorComponent.h"

#include "../data_structures/Settings.h"

using std::vector;
using std::map;
using std::pair;
using std::string;
using std::weak_ptr;

using namespace Application_Layer;
using namespace glm;

namespace Game_Logic
{
	class ActorFactory;

	typedef weak_ptr<ActorComponent> WeakComponentPtr;
	typedef shared_ptr<ActorComponent> StrongActorComponentPtr;
	typedef map<string, StrongActorComponentPtr> ComponentList;
	typedef unsigned long ActorId;

	/**
	 * Everything which has an influence in the game world is an actor.
	 * Actors can have several components which are responsible for the kind of interactions
	 */
	class Actor
	{
	private:
		friend ActorFactory;

        bool m_isMainActor;
        
		/*
		 * Unique id for the current actor - to identify the actor
		 */
		ActorId m_actorId;

		/*
		 * The components the actor holds
		 */
		ComponentList m_components;

		Actor();

		/**
		 * Initialize Actor with specific actor id
		 */
		Actor(ActorId);

        /*
         * Initialize Actor with specific actor id and if its
         * the main actor (second parameter is true)
         */
        Actor(ActorId, bool);

		/**
		 * Executes the init function in every assigned component
		 */
		void Init();

		/*
		 * Adds a component to the list of the currents
		 */
		void AddComponent(StrongActorComponentPtr);

	public:
		~Actor();

		/**
		 * Returns true if the current actor has a component with the given name
		 */
		bool HasComponent(const string&);

		/*
		 * Returns the requested component
		 */
		StrongActorComponentPtr GetComponent(const string&);

		/*
		 * Returns the components that are assigned to the current actor
		 */
		ComponentList GetComponents();

		/*
		 * Sets the current actor id by the given
		 */
		void SetActorId(ActorId);
        
        /**
         * Sets the boolean that implies if the current actor
         * is the main actor of the scene
         */
        void SetIsMainActor(bool);

		/*
		 * Returns the id of the current actor
		 */
		ActorId GetActorId() const;
        
        bool IsMainActor() const
        {
            return m_isMainActor;
        }
	};

	typedef shared_ptr<Actor> StrongActorPtr;
	typedef weak_ptr<Actor> WeakActorPtr;

}