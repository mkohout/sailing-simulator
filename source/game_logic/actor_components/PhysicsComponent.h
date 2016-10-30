#pragma once

#include "ActorComponent.h"
#include "IPhysicsComponent.h"
#include <btBulletDynamicsCommon.h>

#include "../game_view/rendering/Geometry.h"

namespace Game_Logic
{
	/**
	 * Physics component that enables basic physics
     * to the belonging actor such as gravity etc.
	 */
	class PhysicsComponent : public ActorComponent, public IPhysicsComponent
	{
	private:
		btCollisionShape* shape;
		btDefaultMotionState* motionState;
		btRigidBody* rigidBody;

		btScalar mass;
        
        btVector3 m_pointingTo;

		Game_View::StrongGeometryPtr m_pGeometry;
		glm::vec3 m_bouyancyDirection;
		float m_gravityStrength;
        
	public:
		PhysicsComponent()
			: shape(nullptr), motionState(nullptr), rigidBody(nullptr), mass(0), m_pGeometry(nullptr)
		{
			
		}

		~PhysicsComponent()
		{
			if (rigidBody != nullptr)
				delete rigidBody;
			if(motionState != nullptr)
				delete motionState;
			if(shape != nullptr)
				delete shape;
		}

		virtual void VInit();

		virtual string VGetIdentifier() const
		{
			return "physicsComponent";
		}
        
        /**
         * Applies the given force to the center of mass
         * of the current actor
         */
        void ApplyForceToActor(const glm::vec3&);
        
        /**
         * Method that applies the given force to the
         * actor of the current physics component with the given
         * offset
         */
        void ApplyForceToActor(const glm::vec3&, const glm::vec3&);
        
        /**
         * Applies the given torque to the current actor
         */
        void ApplyTorqueToActor(const glm::vec3&);

		/*
		 * Assigns the data to the component from
         * the given XML element
		 */
		virtual void VFetchData(StrongXMLElementPtr);
        
        /**
         * Updates the physics component every frame of the physics thread
         */
        virtual void VUpdate(const unsigned int);

		btRigidBody* const& rigid_body() const
		{
			return rigidBody;
		}
        
        /**
         * Returns a vector to the direction the actor
         * is currently pointing to
         */
        const glm::vec3 GetPointingTo() const;
        
        /**
         * Returns the current velocity of the actor
         */
        const glm::vec3 GetVelocity() const;
	};
}