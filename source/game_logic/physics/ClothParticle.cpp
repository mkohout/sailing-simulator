#include "ClothParticle.h"
#include <math.h>


using namespace Game_Logic;

ClothParticle::ClothParticle(
                             unsigned int index,
                             vec3 oldPosition,
                             float massPerParticle,
                             float damping)
    :   m_moveable(true), m_mass(massPerParticle), m_vertexIndex(index), m_oldPosition(oldPosition), m_damping(damping),
        m_acceleration(vec3(0,0,0))
{
}

void ClothParticle::AddForce(const vec3& force)
{
    m_acceleration += force;
}

void ClothParticle::AddMassIndependentForce(const vec3& force)
{
    m_acceleration += force * m_mass;
}

void ClothParticle::ResetAcceleration()
{
    m_acceleration = vec3(0,0,0);
    m_velocity *= m_damping;
}