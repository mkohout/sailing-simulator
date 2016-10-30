#include "Scene.h"
#include "../scene_graph/SceneGroup.h"
#include "../scene_graph/SceneNode.h"
#include "../scene_graph/SkyNode.h"
#include "../../application_layer/core/stdafx.h"
#include "../../application_layer/core/GameCore.h"
#include "../../game_logic/event_system/EvtData_TransformActor.h"
#include <fastdelegate/FastDelegate.h>
#include <glm/gtx/transform.hpp>

using namespace Game_Logic;
using namespace Game_View;

using std::pair;

Scene::~Scene()
{
	SAFE_DELETE(m_pSceneGraph);
}

void Scene::InitFromXMLResource(shared_ptr<XMLResource> pXMLResource)
{
	m_pSceneGraph = new SceneGraph();

	StrongXMLElementPtr children =
		pXMLResource->GetRootElement()->FindFirstChildNamed("scene");

	if (children == nullptr)
	{
		return;
	}

	for (auto pChild : children->GetChildren())
	{
		ProcessXMLElement(pChild);
	}
    
    InitTransformationListeners();
    InitCamera();
    InitForces();
    InitSounds();
}

void Scene::InitCamera()
{
    StrongActorPtr pMainActor = FindMainActor();
    
    if(pMainActor!=nullptr)
    {
        StrongSceneNodePtr pMainActorSceneNodePtr =
        m_pSceneGraph->FindNodeWithActor(pMainActor->GetActorId());
        
        m_pCameraNode->SetLookAt(
                               pMainActorSceneNodePtr->VGetPosition());
    } else
    {
        m_pCameraNode->SetLookAt(vec3(0, 0, 0));
    }
}

void Scene::InitTransformationListeners()
{
    EventListenerDelegate actorTransformFunction =
        fastdelegate::MakeDelegate(
                               this,
                               &Scene::ApplyTransformation
                               );
    
    g_pGameApp->GetPlayerInputEvtMgr()->VAddListener(
                                        actorTransformFunction,
                                        EvtData_TransformActor::s_eventType);
}

void Scene::ApplyTransformation(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_TransformActor> pCastEventData =
                std::static_pointer_cast<EvtData_TransformActor>(pEventData);
    
    StrongSceneNodePtr pSceneNode =
        m_pSceneGraph->FindNodeWithActor(pCastEventData->GetActorId());
    
    if(pSceneNode==nullptr)
    {
        return ;
    }
    
    if(pCastEventData->ApplyTransformationToParent())
    {
        StrongSceneNodePtr pParent =
                                pSceneNode->VGetParent();
        
        pParent->VTransform(pCastEventData->GetTransformationMatrix());
    }
    else
    {
        pSceneNode->VTransform(pCastEventData->GetTransformationMatrix());
    }
}

void Scene::ProcessXMLElement(StrongXMLElementPtr pXmlElement)
{
	ProcessXMLElement(pXmlElement, nullptr);
}

void Scene::ProcessXMLElement(
	StrongXMLElementPtr pXmlElement, StrongSceneNodePtr pParentGroup)
{
    if(pXmlElement->GetName().compare("forces")==0)
    {
        ProcessForces(pXmlElement);
    }
    else if(pXmlElement->GetName().compare("sounds")==0)
    {
        ProcessSounds(pXmlElement);
    }
	else if (pXmlElement->GetName().compare("sceneGroup") == 0)
	{
		StrongSceneNodePtr pSceneGroup =
                    StrongSceneNodePtr(new SceneGroup());
        
        pSceneGroup->VInitByXMLElement(pXmlElement);

		for (auto pChild : pXmlElement->GetChildren())
		{
			ProcessXMLElement(pChild, pSceneGroup);
		}
        
        if(pParentGroup != nullptr)
        {
            pSceneGroup->VSetParent(pParentGroup);
            // Scene group is child of another scene group
            pParentGroup->VAddChildNode(pSceneGroup);
        }
        else
        { // First SceneGroup in the scene
            m_pSceneGraph->AddNode(pSceneGroup);
        }
	}
	else if (pXmlElement->GetName().compare("cameraNode") == 0)
	{
		AddCameraToScene(pXmlElement, pParentGroup);
	}
	else if (pXmlElement->GetName().compare("skyNode") == 0)
	{
		AddSkyNodeToScene(pXmlElement, pParentGroup);
	}
	else if (pXmlElement->GetName().compare("actor") == 0)
	{
		AddActorToSceneGroup(pXmlElement, pParentGroup);
	}
	else if (pXmlElement->GetName().compare("lightNode") == 0)
	{
		AddLightToScene(pXmlElement, pParentGroup);
	}
}

void Scene::ProcessForces(StrongXMLElementPtr pForcesElement)
{
    for(StrongXMLElementPtr pForceElement : pForcesElement->GetChildren())
    {
        AddForceToScene(pForceElement);
    }
}

void Scene::AddForceToScene(StrongXMLElementPtr pForceElement)
{
    StrongForcePtr pForce(new Force());
    
    
    pForce->InitFromXMLElement(pForceElement);
    
    m_forces.push_back(pForce);
}

Forces Scene::GetForces() const
{
    return m_forces;
}

StrongForcePtr Scene::FindSpecificForce(const string& identifier) const
{
    for(StrongForcePtr pForce : m_forces)
    {
        if(pForce->GetIdentifier().compare(identifier)==0)
        {
            return pForce;
        }
    }
    
    return nullptr;
}

void Scene::AddSkyNodeToScene(
	StrongXMLElementPtr pXmlElement, StrongSceneNodePtr pSceneGroup)
{
	StrongXMLElementPtr pActorElement =
		pXmlElement->FindFirstChildNamed("actor");

	if (pActorElement == nullptr)
	{
		return;
	}

	const string actorIdentifier =
		pActorElement->GetValueOfAttribute("identifier");
	StrongActorPtr pActor =
		CreateActor(actorIdentifier, pActorElement);

	if (pActor == nullptr)
	{
		return;
	}

	shared_ptr<SkyNode> pSkyNode =
		shared_ptr<SkyNode>(new SkyNode());

    pSkyNode->VInitByXMLElement(pXmlElement);
	pSkyNode->VSetActor(pActor);
    pSkyNode->VSetParent(pSceneGroup);

	AddActor(actorIdentifier, pActor);

	pSceneGroup->VAddChildNode(pSkyNode);
}

void Scene::AddLightToScene(
        StrongXMLElementPtr pXmlElement, StrongSceneNodePtr pSceneGroup)
{
	shared_ptr<LightNode> pLightNode = shared_ptr<LightNode>(new LightNode());

	pLightNode->VInitByXMLElement(pXmlElement);

	if (!pLightNode->HasType())
	{
		return;
	}
    
    pLightNode->VSetParent(pSceneGroup);

	pSceneGroup->VAddChildNode(pLightNode);
}

shared_ptr<CameraNode> Scene::GetCamera() const
{
    return m_pCameraNode;
}

void Scene::AddCameraToScene(
        StrongXMLElementPtr pXmlElement, StrongSceneNodePtr pSceneGroup)
{
	shared_ptr<CameraNode> pCameraNode =
                shared_ptr<CameraNode>(new CameraNode());

	pCameraNode->VInitByXMLElement(pXmlElement);
    pCameraNode->VSetParent(pSceneGroup);
    
	pSceneGroup->VAddChildNode(pCameraNode);
    
    m_pCameraNode = pCameraNode;
}

void Scene::AddActorToSceneGroup(
            StrongXMLElementPtr pXmlElement, StrongSceneNodePtr pParentGroup)
{
	// An actor which is not in a group is not possible
	if (pParentGroup == nullptr)
	{
		return;
	}

	const string actorIdentifier =
		pXmlElement->GetValueOfAttribute("identifier");
	StrongActorPtr pActor =
		CreateActor(actorIdentifier, pXmlElement);

	if (pActor == nullptr)
	{
		return;
	}

	shared_ptr<SceneNode> pActorNode =
		shared_ptr<SceneNode>(new SceneNode());

	pActorNode->VSetActor(pActor);

    //TODO, refactoring
	glm::mat4 localToWorld = glm::mat4(1);
	auto children = pXmlElement->GetChildren();
	for(auto c : children)
	{
		glm::mat4 transform = glm::mat4(1);
		float x = 0, y = 0, z = 0;
		if (c->GetName() == "scale")
			x = y = z = 1;
		if (c->HasAttributeNamed("x"))
			x = std::stof(c->GetValueOfAttribute("x"));
		if (c->HasAttributeNamed("y"))
			y = std::stof(c->GetValueOfAttribute("y"));
		if (c->HasAttributeNamed("z"))
			z = std::stof(c->GetValueOfAttribute("z"));
		if (c->HasAttributeNamed("v"))
			x = y = z = std::stof(c->GetValueOfAttribute("v"));//easy method to set all components at the same time
		if (c->GetName() == "position")
			transform = glm::translate(glm::vec3(x, y, z));
		else if (c->GetName() == "scale")
			transform = glm::scale(glm::vec3(x, y, z));
		else if (c->GetName() == "rotation")
			transform = glm::rotate(glm::radians(std::stof(c->GetValueOfAttribute("angle"))), glm::vec3(x, y, z));
        
		localToWorld = transform * localToWorld;
	}
    
	pActorNode->VSetTransformationMatrix(localToWorld);
    pActorNode->VSetParent(pParentGroup);

	AddActor(actorIdentifier, pActor);

	pParentGroup->VAddChildNode(pActorNode);
}

StrongActorPtr Scene::CreateActor(
	const string& identifier, StrongXMLElementPtr pXmlElement)
{
	StrongActorPtr pActor;

    pActor =
			(ActorFactory::GetInstance())->CreateActorByXMLElement(pXmlElement);

	return pActor;
}

StrongActorPtr Scene::FindMainActor()
{
    for(auto actorEntry : m_actorList)
    {
        if(!actorEntry.second->IsMainActor())
        {
            continue;
        }
        
        return actorEntry.second;
    }
    
    return nullptr;
}

bool Scene::HasActorWithIdentifier(const string& actorIdentifier)
{
	ActorCache::iterator it = m_actors.find(actorIdentifier);

	return (it != m_actors.end());
}

StrongActorPtr Scene::GetActorWithIdentifier(const string& actorIdentifier) const
{
	return m_actors.find(actorIdentifier)->second;
}

StrongActorPtr Scene::GetActorWithActorId(ActorId actorId) const
{
	return m_actorList.find(actorId)->second;
}

bool Scene::HasActorWithActorId(ActorId actorId)
{
	ActorList::iterator it = m_actorList.find(actorId);

	return (it != m_actorList.end());
}

void Scene::AddActor(const string& identifier, StrongActorPtr pActor)
{
	if (HasActorWithIdentifier(identifier))
	{
		AddActorToScene(pActor);
		return;
	}

	AddActorToScene(pActor);
	m_actors.insert(pair<string, StrongActorPtr>(identifier, pActor));
}

void Scene::AddActorToScene(StrongActorPtr pActor)
{
	if (HasActorWithActorId(pActor->GetActorId()))
	{
		return;
	}

	m_actorList.insert(
            pair<ActorId, StrongActorPtr>(pActor->GetActorId(), pActor));
}

void Scene::InitForces()
{
    for(StrongForcePtr pForce : m_forces)
    {
        pForce->Init();
    }
}

void Scene::ProcessSounds(StrongXMLElementPtr pSoundsElement)
{
    for(StrongXMLElementPtr pSoundElement : pSoundsElement->GetChildren())
    {
        StrongSoundPtr pSound = StrongSoundPtr(new Sound());
        
        pSound->LoadFromXMLElement(pSoundElement);
        
        m_sounds.push_back(pSound);
    }
}

void Scene::InitSounds()
{
    for(StrongSoundPtr pSound : m_sounds)
    {
        g_pGameApp->GetAudioManager()->LoadSoundFile(pSound);
    }
    
    g_pGameApp->GetAudioManager()->PlayAllSounds();
}

void Scene::UpdateForces(const unsigned int& deltaMilliseconds)
{
    for(StrongForcePtr pForce : m_forces)
    {
        pForce->Update(deltaMilliseconds);
    }
}