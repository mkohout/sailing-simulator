#include "Force.h"
#include "../event_system/EvtData_ChangeAudio.h"
#include "../event_system/EvtData_ActorHasMoved.h"
#include "../../application_layer/core/GameCore.h"

#include <glfw/glfw3.h>

using namespace Game_Logic;

void Force::Init()
{
    if(m_type!=TYPE::DYNAMIC)
    {
        return ;
    }
    
    m_currentStrengthValue = 0.0f;
    m_currentRotation = 0.0f;
    m_rotation = vec3(0, 0, 0); //TODO
    
        //TODO
    
    InitMovementListenerForActorIdentifier();
}

void Force::InitDynamics(StrongXMLElementPtr pXmlElement)
{
    if(pXmlElement->HasChildrenNamed("rotation"))
    {
        //TODO
        
    }
    
    if(pXmlElement->HasChildrenNamed("dependentOnActor"))
    {
        StrongXMLElementPtr pDependentOnActor =
                pXmlElement->FindFirstChildNamed("dependentOnActor");
        
        m_dependentOnActor = pDependentOnActor->GetValueOfAttribute("identifier");
    }
    
    if(pXmlElement->HasChildrenNamed("strength"))
    {
        StrongXMLElementPtr pStrengthElement =
                            pXmlElement->FindFirstChildNamed("strength");
        
        m_strengthNoise =
                        std::stof(pStrengthElement->GetValueOfAttribute("noise"));
    }
}

void Force::InitMovementListenerForActorIdentifier()
{
    if(m_dependentOnActor.compare("")==0)
    {
        return ;
    }
    
    EventListenerDelegate actorMovedFunc =
                        fastdelegate::MakeDelegate(
                               this,
                               &Force::ActorMoved
                               );
    
    g_pGameApp->GetPlayerInputEvtMgr()->VAddListener(
                                                     actorMovedFunc,
                                                     EvtData_ActorHasMoved::s_eventType
                                                     );
}

void Force::ActorMoved(IEventDataPtr pEventData)
{
    if(!dynamic_cast<EvtData_ActorHasMoved*>(pEventData.get()))
    {
        return ;
    }
    
    EvtData_ActorHasMoved* pCastEventData =
                dynamic_cast<EvtData_ActorHasMoved*>(pEventData.get());
    StrongActorPtr dependentOnActor =
            g_pGameApp->GetCurrentScene()->GetActorWithIdentifier(m_dependentOnActor);
    
    if(dependentOnActor->GetActorId()!=pCastEventData->GetActorId())
    {
        return ;
    }
    
    StrongActorPtr pActor =
                    g_pGameApp->GetCurrentScene()->GetActorWithActorId(
                                            pCastEventData->GetActorId());
    
    if(!pActor->HasComponent("physicsComponent"))
    {
        return ;
    }
    
    PhysicsComponent* pPhysicsComponent =
                        dynamic_cast<PhysicsComponent*>(
                                pActor->GetComponent("physicsComponent").get());
    
    glm::vec3 velocity = pPhysicsComponent->GetVelocity();
    const float strength = glm::length(velocity);
    
    SetDirection(glm::normalize(velocity) * -1.0f);
    SetStrength(strength);
}

void Force::SetDirection(const glm::vec3 direction)
{
    m_direction = direction;
}

void Force::InitFromXMLElement(StrongXMLElementPtr pXmlElement)
{
    if(pXmlElement->HasChildrenNamed("dynamic"))
    {
        InitDynamics(pXmlElement->FindFirstChildNamed("dynamic"));
    }
    
    if(pXmlElement->HasAttributeNamed("massIndependent"))
    {
        m_massIndependent = std::stoi(pXmlElement->GetValueOfAttribute("massIndependent"))==1;
    }
    
    if(pXmlElement->HasAttributeNamed("effect"))
    {
        m_effect =
            (pXmlElement->GetValueOfAttribute("effect").compare("area")==0 ?
                EFFECT::AREA : EFFECT::PARTICLE);
    }
    
    if(pXmlElement->HasAttributeNamed("type"))
    {
        m_type =
            (pXmlElement->GetValueOfAttribute("type").compare("dynamic")==0 ?
                TYPE::DYNAMIC : TYPE::STATIC);
    }
    
    
    if(pXmlElement->HasChildrenNamed("direction"))
    {
        StrongXMLElementPtr directionElement =
                            pXmlElement->FindFirstChildNamed("direction");
        
        SetDirection(
                directionElement->GetValueOfAttribute("x"),
                directionElement->GetValueOfAttribute("y"),
                directionElement->GetValueOfAttribute("z")
                );
    }
    
    if(pXmlElement->HasAttributeNamed("title"))
    {
        m_identifier = pXmlElement->GetValueOfAttribute("title");
    }
    
    if(pXmlElement->HasChildrenNamed("strength"))
    {
        StrongXMLElementPtr pStrengthElement =
                        pXmlElement->FindFirstChildNamed("strength");
        
        if(pStrengthElement->HasAttributeNamed("max"))
        {
            m_maxStrength =
                    std::stof(pStrengthElement->GetValueOfAttribute("max"));
        }
        
        SetStrength(std::stof(pStrengthElement->GetValueOfAttribute("value")));
    }
}

void Force::SetStrength(const float strength)
{
    if(m_maxStrength>0.0f && m_strength<m_maxStrength)
    {
        m_strength = strength;
        TriggerChangeAudioEvent();
    }
    else
    {
        m_strength = strength;
    }
}

void Force::TriggerChangeAudioEvent()
{
    const float newVolumeLevel = m_strength / m_maxStrength;
    shared_ptr<EvtData_ChangeAudio> pAudioEvent(
                    new EvtData_ChangeAudio(m_identifier, newVolumeLevel));
    
    g_pGameApp->GetPlayerInputEvtMgr()->VQueueEvent(pAudioEvent);
}

void Force::SetDirection(
                const std::string& sX, const std::string& sY, const std::string& sZ)
{
    const float x = std::stof(sX);
    const float y = std::stof(sY);
    const float z = std::stof(sZ);
    
    SetDirection(x, y, z);
}

void Force::Update(const unsigned int& deltaMilliseconds)
{
    if(m_type==TYPE::STATIC)
    {
        return ;
    }
    
    m_currentStrengthValue = m_strengthNoise * cos(glfwGetTime());
    m_tempStrength = m_strength * (1.0f - m_currentStrengthValue);
    
    //TODO, rotation
    // (1-cos(glfwGetTime()))*3.14f;
}