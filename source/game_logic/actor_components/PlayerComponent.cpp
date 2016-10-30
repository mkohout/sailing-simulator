#include "PlayerComponent.h"
#include "../../application_layer/core/GameCore.h"
#include "../process_system/ProcessManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

using namespace Game_Logic;
using std::shared_ptr;
using namespace glm;

void PlayerComponent::VInit()
{
    Interactive::VInit();
    
    VInitActions();
}

void PlayerComponent::VFetchData(
            StrongXMLElementPtr pXmlElement)
{
    if (pXmlElement->HasChildrenNamed("keyLayout"))
    {
        InitKeyLayout(
            pXmlElement->FindFirstChildNamed("keyLayout"));
    }
    
    if (pXmlElement->HasChildrenNamed("speed"))
    {
        m_speed =
            std::stof(pXmlElement->FindFirstChildNamed("speed")->GetContent());
    }
    
    if (pXmlElement->HasChildrenNamed("rotationalSpeed"))
    {
        m_rotationalSpeed =
            std::stof(pXmlElement->FindFirstChildNamed("rotationalSpeed")->GetContent());
    }
}

void PlayerComponent::VInitActions()
{
    m_actions.insert(
            ActionPair(
                    "haul_head_sail",
                    fastdelegate::MakeDelegate(
                                            this,
                                            &PlayerComponent::HaulHeadSail
                                            )));
    
    m_actions.insert(
            ActionPair(
                    "haul_main_sail",
                    fastdelegate::MakeDelegate(
                                            this,
                                            &PlayerComponent::HaulMainSail
                                            )));
    
    m_actions.insert(
            ActionPair(
                    "veer_head_sail",
                    fastdelegate::MakeDelegate(
                                            this,
                                            &PlayerComponent::VeerHeadSail
                                            )));
    
    m_actions.insert(
            ActionPair(
                    "veer_main_sail",
                    fastdelegate::MakeDelegate(
                                            this,
                                            &PlayerComponent::VeerMainSail
                                            )));
    
    m_actions.insert(
        ActionPair(
            "start_music",
            fastdelegate::MakeDelegate(
                this,
                &PlayerComponent::PlayMusic
                )));
    
    m_actions.insert(
        ActionPair(
            "steer_left",
            fastdelegate::MakeDelegate(
                this,
                &PlayerComponent::SteerLeft
                )));
    
	m_actions.insert(
		ActionPair(
			"steer_right",
			fastdelegate::MakeDelegate(
				this,
				&PlayerComponent::SteerRight
				)));
    
    m_actions.insert(
        ActionPair(
            "increase_wind",
            fastdelegate::MakeDelegate(
                this,
                &PlayerComponent::IncreaseWind
                )));
    
    m_actions.insert(
            ActionPair(
            "decrease_wind",
            fastdelegate::MakeDelegate(
                this,
                &PlayerComponent::DecreaseWind
                )));
    m_actions.insert(
            ActionPair(
            "remove_sheet_head_sail",
            fastdelegate::MakeDelegate(
                this,
                &PlayerComponent::RemoveSheetOfHeadSail
                )));
}

void PlayerComponent::IncreaseWind()
{
    UpdateWindStrength(1);
}

void PlayerComponent::DecreaseWind()
{
    UpdateWindStrength(-1);
}

void PlayerComponent::UpdateWindStrength(const float delta)
{
    StrongForcePtr pWindForce =
                    g_pGameApp->GetCurrentScene()->FindSpecificForce("wind");
    const float newStrength = pWindForce->GetStrengthWithoutNoise()+delta;

    if(pWindForce->GetMaxStrength()<newStrength || newStrength<0.0f)
    {
        return ;
    }
    
    std::string logString = "Set new wind strength to " + std::to_string(newStrength);
    
    g_pGameApp->LogString(Logger::INFO, logString);
    
    pWindForce->SetStrength(newStrength);
}

void PlayerComponent::RemoveSheetOfHeadSail()
{
    StrongActorPtr pActor =
                g_pGameApp->GetCurrentScene()->GetActorWithIdentifier("headSail");
    
    if(pActor==nullptr)
    {
        return ;
    }
    
    ClothComponent* pClothComponent =
                    dynamic_cast<ClothComponent*>(
                                pActor->GetComponent("clothComponent").get());
    
    pClothComponent->RemoveFixPoint(glm::vec2(9, 0));
}

void PlayerComponent::SteerLeft()
{
    StrongActorPtr pActor = StrongActorPtr(m_pActor);
    
	auto pPhysicsComponent =
                dynamic_cast<PhysicsComponent*> (
                        pActor->GetComponent("physicsComponent").get());
    float velocity = glm::length(pPhysicsComponent->GetVelocity());
    
    velocity *= m_rotationalSpeed;
    
    pPhysicsComponent->ApplyTorqueToActor(vec3(0, velocity, 0));
}

void PlayerComponent::SteerRight()
{
	StrongActorPtr pActor(m_pActor);

    auto pPhysicsComponent =
                dynamic_cast<PhysicsComponent*> (
                                     pActor->GetComponent("physicsComponent").get());
    float velocity = glm::length(pPhysicsComponent->GetVelocity());

    velocity *= m_rotationalSpeed;
    
    pPhysicsComponent->ApplyTorqueToActor(vec3(0, velocity * -1, 0));
}

void PlayerComponent::VeerHeadSail()
{
    StrongActorPtr pActor =
                g_pGameApp->GetCurrentScene()->GetActorWithIdentifier("headSail");
    
    if(!pActor->HasComponent("sailComponent"))
    {
        return ;
    }
    
    SailComponent* pSailComponent =
                dynamic_cast<SailComponent*>(
                        pActor->GetComponent("sailComponent").get());
    
    pSailComponent->VeerSail();
}

void PlayerComponent::VeerMainSail()
{
    StrongActorPtr pActor =
                g_pGameApp->GetCurrentScene()->GetActorWithIdentifier("mainSail");
    
    if(!pActor->HasComponent("sailComponent"))
    {
        return ;
    }
    
    SailComponent* pSailComponent =
                dynamic_cast<SailComponent*>(
                                 pActor->GetComponent("sailComponent").get());
    
    pSailComponent->VeerSail();
}

void PlayerComponent::HaulHeadSail()
{
    StrongActorPtr pActor =
                    g_pGameApp->GetCurrentScene()->GetActorWithIdentifier("headSail");
    
    if(!pActor->HasComponent("sailComponent"))
    {
        return ;
    }
    
    SailComponent* pSailComponent =
                    dynamic_cast<SailComponent*>(
                                 pActor->GetComponent("sailComponent").get());
    
    pSailComponent->HaulSail();
}

void PlayerComponent::HaulMainSail()
{
    StrongActorPtr pActor =
                    g_pGameApp->GetCurrentScene()->GetActorWithIdentifier("mainSail");
    
    if(!pActor->HasComponent("sailComponent"))
    {
        return ;
    }
    
    SailComponent* pSailComponent =
                    dynamic_cast<SailComponent*>(
                                 pActor->GetComponent("sailComponent").get());
    
    pSailComponent->HaulSail();
}

void PlayerComponent::PlayMusic()
{
    StrongSoundPtr pSound = StrongSoundPtr(new Sound());
    
    //TODO
    pSound->LoadSoundData("santiano");
    
    g_pGameApp->GetAudioManager()->PlaySoundFile(pSound);
    
    //TODO, dirty
    m_actions.erase("start_music");
}