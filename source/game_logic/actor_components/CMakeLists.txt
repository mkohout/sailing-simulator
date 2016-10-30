SET(ACTOR_COMPONENTS_HEADERS	actor_components/ActorComponent.h
				actor_components/PhysicsComponent.h
				actor_components/RenderComponent.h
				actor_components/ClothComponent.h
				actor_components/OceanComponent.h
				actor_components/SailComponent.h
				actor_components/PlayerComponent.h
				actor_components/IPhysicsComponent.h
				actor_components/ParticleSystemComponent.h
				actor_components/FloatingObjectComponent.h
				PARENT_SCOPE
				)

SET(ACTOR_COMPONENTS_SOURCE	actor_components/PhysicsComponent.cpp
				actor_components/RenderComponent.cpp
				actor_components/ClothComponent.cpp
				actor_components/OceanComponent.cpp
				actor_components/PlayerComponent.cpp
				actor_components/ParticleSystemComponent.cpp
				actor_components/SailComponent.cpp
				actor_components/FloatingObjectComponent.cpp
				PARENT_SCOPE
				)
				