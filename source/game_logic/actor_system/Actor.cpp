#include "Actor.h"

using namespace Game_Logic;

Actor::Actor() : m_actorId(0)
{

}

Actor::Actor(ActorId actorId) : m_actorId(0)
{
	m_actorId = actorId;
}

Actor::Actor(ActorId actorId, bool isMainActor) : m_isMainActor(isMainActor), m_actorId(actorId)
{
}

Actor::~Actor()
{
}

void Actor::Init()
{
	for (auto pair : m_components)
	{
		pair.second->VInit();
	}
}

void Actor::SetIsMainActor(bool isMainActor)
{
    m_isMainActor = isMainActor;
}

void Actor::SetActorId(ActorId actorId)
{
	m_actorId = actorId;
}

ActorId Actor::GetActorId() const
{
	return m_actorId;
}

bool Actor::HasComponent(const string& componentName)
{
	auto findIt = m_components.find(componentName);

	if (findIt == m_components.end())
	{
		return false;
	}

	return true;
}

StrongActorComponentPtr Actor::GetComponent(const string& componentName)
{
	auto findIt = m_components.find(componentName);

	if (findIt == m_components.end())
	{
		return nullptr;
	}

	return findIt->second;
}

ComponentList Actor::GetComponents()
{
	return m_components;
}

void Actor::AddComponent(StrongActorComponentPtr pComponent)
{
	if (pComponent == nullptr ||
		HasComponent(pComponent->VGetIdentifier()))
	{
		return;
	}

	m_components.insert(
		pair<string, StrongActorComponentPtr>(
			pComponent->VGetIdentifier(), pComponent));
}