#include "Spring.h"
#include <glm/glm.hpp>

using namespace Game_Logic;

Spring::Spring(
        unsigned int index1,
        unsigned int index2,
        const float rest_distance,
        const float ks,
        const float kd
        )
        : m_index1(index1), m_index2(index2), m_restDistance(rest_distance), m_ks(ks), m_kd(kd)
{
    
}