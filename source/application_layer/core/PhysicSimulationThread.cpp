#include "PhysicSimulationThread.h"
#include "GameCore.h"
#include "../game_logic/physics/BulletPhysics.h"
#include <iostream>
#include "../../game_logic/event_system/EvtData_TransformActor.h"

using namespace Application_Layer;

void PhysicSimulationThread::VInit()
{
    Thread::VInit();
    
	auto pBullet = BulletPhysics::GetInstance();
	auto force = g_pGameApp->GetCurrentScene()->FindSpecificForce("gravity");
    
    if(force==nullptr)
    {
        return ;
    }
	auto strength = force->GetStrength();
	auto gravity = force->GetDirection()*strength;
	auto btGravity = btVector3(gravity.x, gravity.y, gravity.z);
	pBullet->GetDynamicsWorld()->setGravity(btGravity);
}

void PhysicSimulationThread::VUpdate(
                                const unsigned int deltaMilliseconds)
{
	if (deltaMilliseconds) {
		
		auto pRootNode = g_pGameApp->GetCurrentScene()->GetSceneGraph()->GetRootNode();
		Update(deltaMilliseconds, pRootNode);

		BulletPhysics::GetInstance()->GetDynamicsWorld()->stepSimulation(deltaMilliseconds / 1000.0,5);

		auto pActor = g_pGameApp->GetCurrentScene()->GetActorWithIdentifier("sailingBoat");
		PhysicsComponent* pPhysicsComponent = dynamic_cast<PhysicsComponent*> (pActor->GetComponent("physicsComponent").get());
		btTransform trans;
		pPhysicsComponent->rigid_body()->getMotionState()->getWorldTransform(trans);
		glm::mat4 m;
		trans.getOpenGLMatrix(reinterpret_cast<float*>(&m));
		auto s = pPhysicsComponent->rigid_body()->getCollisionShape()->getLocalScaling();
		m = glm::scale(glm::mat4(1), glm::vec3(s.x(), s.y(), s.z())) * m;
		g_pGameApp->GetCurrentScene()->GetSceneGraph()->FindNodeWithActor(pActor->GetActorId())->VGetParent()->VSetTransformationMatrix(m);
		/*shared_ptr<EvtData_TransformActor> pTransformEvent(
			new EvtData_TransformActor(
				pActor->GetActorId(),
				glm::inverse(
					g_pGameApp->GetCurrentScene()->
					GetSceneGraph()->
					FindNodeWithActor(pActor->GetActorId())->
					VGetTransformationMatrix())*m
				));
		pTransformEvent->SetApplyTransformationToParent(true);
		g_pGameApp->GetPlayerInputEvtMgr()->VQueueEvent(pTransformEvent);*/

		g_pGameApp->GetCurrentScene()->GetCamera()->FollowActor(pActor);
	}
}

const short PhysicSimulationThread::VGetFramesForThread() const
{
    return std::stoi(
                     g_pGameApp->GetGameSettings()->GetValue("physicsRate"));
}

void PhysicSimulationThread::Update(unsigned int deltaMilliseconds, StrongSceneNodePtr pSceneNode)
{
	if (pSceneNode->VHasChildren())
	{
		for (StrongSceneNodePtr pChildNode : pSceneNode->VGetAllChildren())
		{
			Update(deltaMilliseconds, pChildNode);
		}
	}

	StrongActorPtr pCurrentActor = pSceneNode->VGetActor();
    
	if (!pCurrentActor)
    {
		return;
    }
    
    for(auto& mapEntry : pCurrentActor->GetComponents())
    {
        IPhysicsComponent* pPhysicsComponent =
            dynamic_cast<IPhysicsComponent*>(mapEntry.second.get());

        if(pPhysicsComponent==nullptr)
        {
            continue;
        }
        
        pPhysicsComponent->VUpdate(deltaMilliseconds);
    }
}
