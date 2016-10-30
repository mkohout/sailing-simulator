#pragma once

#include <string>
#include <memory>
#include <map>

#include "Actor.h"

#include "../actor_components/ActorComponent.h"
#include "../actor_components/ClothComponent.h"
#include "../actor_components/PhysicsComponent.h"
#include "../actor_components/OceanComponent.h"
#include "../actor_components/PlayerComponent.h"
#include "../actor_components/RenderComponent.h"
#include "../actor_components/SailComponent.h"
#include "../actor_components/ParticleSystemComponent.h"
#include "../actor_components/FloatingObjectComponent.h"

#include "../../application_layer/resources/XMLResource.h"

using namespace Application_Layer;

using std::string;
using std::shared_ptr;
using std::map;
using std::pair;

namespace Game_Logic
{
	class Actor;
	class ActorComponent;

	typedef unsigned long ActorId;
	typedef shared_ptr<Actor> StrongActorPtr;

	typedef ActorComponent* (*ActorComponentCreator)(void);
	typedef map<string, ActorComponentCreator> ActorComponentCreatorMap;
	typedef shared_ptr<ActorComponent> StrongActorComponentPtr;

	/**
	 * Class is used to create actors and assign their components
	 */
	class ActorFactory
	{
	private:
		/*
		 * Data structure which holds the specific creation function
		 * for the components
		 */
		ActorComponentCreatorMap m_componentCreatorMap;

		/*
		 * The id of the actor that will be inserted
         * to the running engine next
		 */
		ActorId m_nextActorId;

		/*
		 * Singleton instance for the ActorFactory
		 */
		static ActorFactory* s_singleton;

		ActorFactory() = default;
		~ActorFactory() { }

		/*
		 * Initializes the ActorFactory
		 */
		void Init();

		/*
		 * Returns the id, that will be assigned to the next created actor
		 */
		ActorId GetNextActorId();

		/**
		 * Returns the resource of the specification of the actor with the given identifier
		 */
		StrongResourcePtr GetSpecificationOfActor(const string&);

		/*
		 * Adds a component - represented by the given xml element - to the given actor
		 */
		void AddComponentsToActor(StrongActorPtr, StrongXMLElementPtr);

	public:
		StrongActorPtr CreateActorByXMLElement(StrongXMLElementPtr);

		/*
		 * ActorFactory is implementing the singleton
         * pattern. This method returns the singleton
         * instance
		 */
		static ActorFactory* GetInstance()
		{
			if (s_singleton == nullptr)
			{
				s_singleton = new ActorFactory();

				s_singleton->Init();
			}

			return s_singleton;
		}

		/**
		 * Try to find the corresponding creator for the given component xml element and
		 * creates the component with the given settings - filled up with the default settings
		 */
		StrongActorComponentPtr CreateComponent(StrongXMLElementPtr);

		/*
		 * Destroys the singleton instance
		 */
		static void DestroyInstance();
	};


	/*
	 * Function that creates the ActorComponent object
     * for the cloth component
	 */
	ActorComponent* CreateClothComponent();

	/*
     * Function that creates the ActorComponent object
     * for the physics component.
	 */
	ActorComponent* CreatePhysicsComponent();
    
    /**
     * Function that creates the ActorComponent object
     * for the player component. The player component
     * is listening to keyboard events and transforms
     * the main actor
     */
    ActorComponent* CreatePlayerComponent();
    
    /**
     * Function that creates the ActorComponent object
     * for the ocean component. The ocean component calculates
     * the height map for a plane that turns it into an ocean.
     */
    ActorComponent* CreateOceanComponent();

	/*
     * Function that creates the ActorComponent object
     * for the render component
	 */
	ActorComponent* CreateRenderComponent();
    
    /**
     * Function that creates the ActorComponent object
     * for the sail component that will apply the force acting
     * on the cloth to the underlying body
     */
    ActorComponent* CreateSailComponent();

    /**
     * TODO
     */
	ActorComponent* CreateParticleSystemComponent();

	ActorComponent* CreateFloatingObjectComponent();
}