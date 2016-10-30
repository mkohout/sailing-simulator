#include "ActorFactory.h"
#include "../game_app/GameApp.h"

#include "../../application_layer/core/GameCore.h"

using namespace Game_Logic;

ActorFactory* ActorFactory::s_singleton = nullptr;

void ActorFactory::Init()
{
    m_componentCreatorMap["clothComponent"] =
        CreateClothComponent;
    m_componentCreatorMap["playerComponent"] =
        CreatePlayerComponent;
	m_componentCreatorMap["physicsComponent"] =
		CreatePhysicsComponent;
	m_componentCreatorMap["renderComponent"] =
		CreateRenderComponent;
    m_componentCreatorMap["oceanComponent"] =
        CreateOceanComponent;
	m_componentCreatorMap["sailComponent"] =
		CreateSailComponent;
	m_componentCreatorMap["particleSystemComponent"] =
		CreateParticleSystemComponent;
	m_componentCreatorMap["floatingObjectComponent"] =
		CreateFloatingObjectComponent;
}

ActorId ActorFactory::GetNextActorId()
{
	return (++m_nextActorId);
}

StrongResourcePtr ActorFactory::GetSpecificationOfActor(
	const string& identifier)
{
	const string actorFilename = identifier + ".xml";
	const string filename =
		g_pGameApp->GetFileManager()->GetPathToActorFile(actorFilename);

	if (!g_pGameApp->GetFileManager()->FileExists(filename))
	{
        throw "Actor specification file does not exist";
		return nullptr;
	}

    StrongResourcePtr pResource =
            g_pGameApp->GetResourceManager()->GetResource(filename, IResource::XML);
    

	return pResource;
}

StrongActorPtr ActorFactory::CreateActorByXMLElement(
	StrongXMLElementPtr pXmlElement)
{
	ActorId nextActorId = GetNextActorId();

	if (!pXmlElement->HasAttributeNamed("identifier"))
	{
		return nullptr;
	}

	string actorIdentifier =
		pXmlElement->GetValueOfAttribute("identifier");
    StrongResourcePtr pXmlResource =
                    GetSpecificationOfActor(actorIdentifier);
	XMLResource* pActorSpec =
		dynamic_cast<XMLResource*>(
                pXmlResource.get());
    
    bool isMainActor =
            pXmlElement->HasAttributeNamed("mainActor");

	if (pActorSpec == nullptr)
	{
		return nullptr; //File not found
	}

	StrongActorPtr pActor = StrongActorPtr(new Actor(nextActorId, isMainActor));
	StrongXMLElementPtr pActorElement =
		pActorSpec->GetRootElement()->FindFirstChildNamed("actor");

	if (pActorElement->HasChildrenNamed("components"))
	{
		AddComponentsToActor(
			pActor, pActorElement->FindFirstChildNamed("components"));
	}

	pActor->Init();

	return pActor;
}

void ActorFactory::AddComponentsToActor(
	StrongActorPtr pActor, StrongXMLElementPtr pXmlElement)
{
	if (pXmlElement == nullptr ||
		!pXmlElement->HasChildren())
	{
		return;
	}

	for (StrongXMLElementPtr pComponentElement : pXmlElement->GetChildren())
	{
        StrongActorComponentPtr pComponent =
            CreateComponent(pComponentElement);
        
        pComponent->SetActor(pActor);
        
		pActor->AddComponent(pComponent);
	}
}

StrongActorComponentPtr ActorFactory::CreateComponent(
	StrongXMLElementPtr pComponentElement)
{
	StrongActorComponentPtr pComponent;

	auto findIt =
        m_componentCreatorMap.find(pComponentElement->GetName());

	if (findIt == m_componentCreatorMap.end())
	{
		return nullptr;
	}

	ActorComponentCreator creator = findIt->second;
	pComponent.reset(creator());

	pComponent->VFetchData(pComponentElement);

	return pComponent;
}

void ActorFactory::DestroyInstance()
{
	SAFE_DELETE(s_singleton);
}

ActorComponent* Game_Logic::CreateClothComponent()
{
    return new ClothComponent();
}

ActorComponent* Game_Logic::CreateOceanComponent()
{
    return new OceanComponent();
}

ActorComponent* Game_Logic::CreatePlayerComponent()
{
    return new PlayerComponent();
}

ActorComponent* Game_Logic::CreatePhysicsComponent()
{
	return new PhysicsComponent();
}

ActorComponent* Game_Logic::CreateRenderComponent()
{
	return new RenderComponent();
}

ActorComponent* Game_Logic::CreateSailComponent()
{
    return new SailComponent();
}

ActorComponent* Game_Logic::CreateParticleSystemComponent()
{
	return new ParticleSystemComponent();
}

ActorComponent* Game_Logic::CreateFloatingObjectComponent()
{
	return new FloatingObjectComponent();
}