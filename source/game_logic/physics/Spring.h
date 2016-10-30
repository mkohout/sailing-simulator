#pragma once

#include <glm/vec3.hpp>
#include <memory>
#include "ClothParticle.h"

using std::shared_ptr;
using namespace glm;

namespace Game_Logic
{
    /**
     * Class that models a spring with
     * its spring constants and the two particles that
     * form the spring
     */
    class Spring
    {
    private:
        /**
         * Spring constant according to Hook's law
         */
        float m_ks;
        
        /**
         * Damping constant
         */
        float m_kd;
        
        /**
         * The two indices of the particles that
         * form the spring
         */
        unsigned int m_index1, m_index2;
        
        /**
         * The distance the two particles have at t = 0
         */
        float m_restDistance;
        
    public:
        Spring(unsigned int, unsigned int, const float, const float, const float);
        Spring()
        {
        }
        
        const unsigned int GetParticleIndex1() const
        {
            return m_index1;
        }
        
        const unsigned int GetParticleIndex2() const
        {
            return m_index2;
        }
        
        const float GetKs() const
        {
            return m_ks;
        }
        
        const float GetKd() const
        {
            return m_kd;
        }
        
        const float GetRestDistance() const
        {
            return m_restDistance;
        }
    };
    
    typedef shared_ptr<Spring> StrongSpringPtr;
}