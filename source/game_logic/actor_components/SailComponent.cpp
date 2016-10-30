#include "SailComponent.h"
#include "ClothComponent.h"
#include "../actor_system/Actor.h"
#include "../../application_layer/core/GameCore.h"
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/projection.hpp>
#include <cmath>
#include <iostream> //TODO

using namespace Game_Logic;

void SailComponent::VFetchData(StrongXMLElementPtr pXmlElement)
{
    if(pXmlElement->HasChildrenNamed("applyForceTo"))
    {
        StrongXMLElementPtr pForceElement =
                pXmlElement->FindFirstChildNamed("applyForceTo");
        
        m_applyForceOnActor = pForceElement->GetValueOfAttribute("name");
    }
    
    if(pXmlElement->HasChildrenNamed("towPoint"))
    {
        StrongXMLElementPtr pHaulPointElement =
                pXmlElement->FindFirstChildNamed("towPoint");
        
        m_towPoint = vec3(
                           std::stof(pHaulPointElement->GetValueOfAttribute("x")),
                           std::stof(pHaulPointElement->GetValueOfAttribute("y")),
                           std::stof(pHaulPointElement->GetValueOfAttribute("z"))
                           );
    }
    else
    {
        m_towPoint = vec3(0, 0, 0);
    }
    
    if(pXmlElement->HasChildrenNamed("offset"))
    {
        StrongXMLElementPtr pOffsetElement =
                    pXmlElement->FindFirstChildNamed("offset");
        
        m_offset = glm::vec3(
                             std::stof(pOffsetElement->GetValueOfAttribute("x")),
                             std::stof(pOffsetElement->GetValueOfAttribute("y")),
                             std::stof(pOffsetElement->GetValueOfAttribute("z"))
                             );
    }
    else
    {
        m_offset = vec3(0, 0, 0);
    }
}

void SailComponent::ApplyForce(const vec3 force)
{
    StrongActorPtr pActor =
                g_pGameApp->GetCurrentScene()->GetActorWithIdentifier(m_applyForceOnActor);
    
    if(pActor==nullptr || !pActor->HasComponent("physicsComponent"))
    {
        return ;
    }
    
    auto pPhysicsComponent =
                dynamic_cast<PhysicsComponent*> (
                                     pActor->GetComponent("physicsComponent").get());
    vec3 forceVector = CalculateForceVector(force);
    
    pPhysicsComponent->ApplyForceToActor(forceVector);
}

const vec3 SailComponent::CalculateForceVector(
                                const glm::vec3 forceOnSail)
{
    StrongActorPtr pActor = g_pGameApp->GetCurrentScene()->FindMainActor();
    StrongSceneNodePtr pActorNode =
            g_pGameApp->GetCurrentScene()
                ->GetSceneGraph()->FindNodeWithActor(
                    pActor->GetActorId());

    vec3 windDirection = GetWindDirection();
    vec3 pointingTo = GetPointingToVector();
    const float angle =
                GetAngleBetweenWindAndPointingTo(windDirection, pointingTo);
    const float speedMultiplier =
            GetSpeedMultiplierAccordingToWindCourse(GetWindCourseFromAngle(angle));

    const float relevantForce =
                        glm::length(vec3(forceOnSail.x, 0, forceOnSail.z));

    const float speedValue = speedMultiplier * relevantForce;
    
    const vec4 output = vec4(speedValue * glm::normalize(pointingTo), 0.0f);
    
    return vec3(glm::inverse(pActorNode->VGetTransformationMatrix()) * output);
}

float SailComponent::GetSpeedMultiplierAccordingToWindCourse(COURSE course)
{
    switch(course)
    {
        case HEAD_TO_WIND:
            return 0.3f;
            
        case CROSSING_WIND:
            return 1.0f;
            
        case BROAD_REACH:
            return 1.1f;
            
        case WIND_RIGHT_AFT:
            return 1.2f;
            
        default:
        case CLOSE_HAULED:
            return 0.8f;
    }
}

const vec3 SailComponent::GetWindDirection() const
{
    StrongForcePtr pWindForce =
                g_pGameApp->GetCurrentScene()->FindSpecificForce("wind");

    return pWindForce->GetDirection();
}

const float SailComponent::GetWindStrength() const
{
    StrongForcePtr pWindForce =
                g_pGameApp->GetCurrentScene()->FindSpecificForce("wind");
    
    return pWindForce->GetStrength();
}

SailComponent::COURSE SailComponent::GetWindCourseFromAngle(
                                            float angleBetweenPointingToAndWind)
{
    angleBetweenPointingToAndWind = abs(angleBetweenPointingToAndWind);
    angleBetweenPointingToAndWind = 180.0f - angleBetweenPointingToAndWind;

    if(angleBetweenPointingToAndWind>15.0f &&
       angleBetweenPointingToAndWind<=60.0f)
    {
        return COURSE::CLOSE_HAULED;
    }
    else if(angleBetweenPointingToAndWind>60.0f &&
            angleBetweenPointingToAndWind<=110.0f)
    {
        return COURSE::CROSSING_WIND;
    }
    else if(angleBetweenPointingToAndWind>110.0f &&
            angleBetweenPointingToAndWind<=160.0f)
    {
        return COURSE::BROAD_REACH;
    }
    else if(angleBetweenPointingToAndWind>160.0f &&
            angleBetweenPointingToAndWind<=180.0f)
    {
        return COURSE::WIND_RIGHT_AFT;
    }
    
    return COURSE::HEAD_TO_WIND;
}

const vec3 SailComponent::GetPointingToVector()
{
    StrongActorPtr pActor =
                g_pGameApp->GetCurrentScene()->GetActorWithIdentifier(m_applyForceOnActor);
    
    if(pActor==nullptr || !pActor->HasComponent("physicsComponent"))
    {
        return vec3(0, 0, 0);
    }
    
    auto pPhysicsComponent =
                        dynamic_cast<PhysicsComponent*> (
                                     pActor->GetComponent("physicsComponent").get());
    
    return pPhysicsComponent->GetPointingTo();
}

const float SailComponent::GetAngleBetweenWindAndPointingTo(
                                    const vec3 windDirection,
                                    const vec3 pointingTo)
{
    //TODO, up
    float angleBetweenPointingToAndWind =
                glm::orientedAngle(
                glm::normalize(windDirection),
                glm::normalize(pointingTo),
                vec3(0, 1, 0)
                );
    
#ifdef GLM_FORCE_RADIANS
    angleBetweenPointingToAndWind =
                    glm::degrees(angleBetweenPointingToAndWind);
#endif
    
    return angleBetweenPointingToAndWind;
}

void SailComponent::VeerSail()
{
    StrongActorPtr pActor(m_pActor);
    
    if(!pActor->HasComponent("clothComponent"))
    {
        return ;
    }
    
    ClothComponent* pClothComponent =
                dynamic_cast<ClothComponent*>(
                        pActor->GetComponent("clothComponent").get());
    
    
    const float t = 1.01f;
    
    pClothComponent->TowPoint(glm::vec2(9, 0), m_towPoint, t);
}

void SailComponent::HaulSail()
{
    StrongActorPtr pActor(m_pActor);
    
    if(!pActor->HasComponent("clothComponent"))
    {
        return ;
    }
    
    ClothComponent* pClothComponent =
                dynamic_cast<ClothComponent*>(
                                  pActor->GetComponent("clothComponent").get());

    const float t = 0.99f;
    
    pClothComponent->TowPoint(glm::vec2(9, 0), m_towPoint, t);
}
