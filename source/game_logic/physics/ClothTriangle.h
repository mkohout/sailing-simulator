#pragma once

#include "ClothParticle.h"

#include <glm/vec3.hpp>
#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;
using namespace glm;

namespace Game_Logic
{
    /**
     * A triangle that consists of three cloth particles
     */
    class ClothTriangle
    {
    public:
        vector<StrongClothParticlePtr> m_particles;
        
    public:
        ClothTriangle(StrongClothParticlePtr, StrongClothParticlePtr, StrongClothParticlePtr);
        
        
        ~ClothTriangle() {
            
        }
        
        /**
         * Returns a reference to the particles stored in
         * the triangle
         */
        vector<StrongClothParticlePtr>& GetParticles()
        {
            return m_particles;
        }
        
        
    };
    
    typedef shared_ptr<ClothTriangle> StrongTrianglePtr;
}