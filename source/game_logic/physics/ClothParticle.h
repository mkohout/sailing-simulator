#pragma once

#include <glm/vec3.hpp>
#include <memory>
#include "../../game_view/rendering/VertexData.h"

using namespace glm;
using namespace Game_View;
using std::shared_ptr;

namespace Game_Logic
{
    /**
     * Class that represents a particle used
     * in the cloth simulation. A cloth particle
     * needs - in addition to the geometry - further
     * informations so that the cloth simulation can
     * proceed
     */
    class ClothParticle
    {
    private:
        /**
         * Indicates whether the current particle is a fixpoint
         * or not
         */
        bool m_moveable;

        /**
         * The mass of the current particle.
         * Important for applying forces to the particle.
         */
        float m_mass;

        /**
         * Vertex index in the cloth's geometry of the current particle
         */
        unsigned int m_vertexIndex;
        
        /**
         * Position before the accelerations
         * were applied
         */
        vec3 m_oldPosition;
        
        /**
         * Vector that stores all accelerations of the current
         * particle
         */
        vec3 m_acceleration;
        
        /**
         * The velocity of the current particle
         */
        vec3 m_velocity;
        
        /**
         * Constant damping that will be applied to the velocity
         * every timestep
         */
        float m_damping;
        
    public:
        ClothParticle(
            unsigned int,
            vec3,
            float,
            float
            );
        
        ~ClothParticle()
        {
        }
        
        /**
         * Removes the current particle as a fixpoint
         */
        void MakeUnmovable()
        {
            m_moveable = false;
        }
        
        /**
         * Sets the current particle as a fix point
         */
        void MakeMoveable()
        {
            m_moveable = true;
        }
        
        /**
         * Integrates the accelerations saved in the acceleration property
         * and adds the change in the position to the current position
         */
        void TimeStep(const unsigned int&);
        
        /**
         * Adds the given force to the acceleration property
         * of the current particle
         */
        void AddForce(const vec3&);
        
        /**
         * Adds the force to the current particle whereas
         * the force is mass independent
         */
        void AddMassIndependentForce(const vec3&);
        
        /**
         * Returns the current acceleration of the particle.
         * The acceleration will be resetted after every
         * integration step.
         */
        const vec3 GetAcceleration() const
        {
            return m_acceleration/m_mass;
        }
        
        unsigned int GetIndex()
        {
            return m_vertexIndex;
        }
        
        bool IsMoveable()
        {
            return m_moveable;
        }
        
        vec3& GetOldPosition()
        {
            return m_oldPosition;
        }
        
        void SetOldPosition(vec3 oldPosition)
        {
            m_oldPosition = oldPosition;
        }
        
        vec3& GetVelocity()
        {
            return m_velocity;
        }
        
        /**
         * Sets the acceleration of the current acceleration
         * back to (0, 0, 0), because the acceleration were
         * translated into a velocity
         */
        void ResetAcceleration();
    };
    
    typedef shared_ptr<ClothParticle> StrongClothParticlePtr;
}