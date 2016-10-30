#include "ClothTriangle.h"

using namespace Game_Logic;

ClothTriangle::ClothTriangle(
        StrongClothParticlePtr pParticle1,
        StrongClothParticlePtr pParticle2,
        StrongClothParticlePtr pParticle3)
{
    m_particles.push_back(pParticle1);
    m_particles.push_back(pParticle2);
    m_particles.push_back(pParticle3);
}