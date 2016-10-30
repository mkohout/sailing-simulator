#include "SceneGraph.h"

using namespace Game_View;

void SceneGraph::AddNode(StrongSceneNodePtr pSceneNode)
{
	if (m_pVirtualUniverse == nullptr)
	{
		InitVirtualUniverse();
	}

	m_pVirtualUniverse->VAddChildNode(pSceneNode);
}

void SceneGraph::InitVirtualUniverse()
{
	m_pVirtualUniverse = StrongSceneNodePtr(new SceneGroup());
}

StrongSceneNodePtr SceneGraph::FindNodeWithActor(ActorId actorId)
{
    return SearchForActorInNode(actorId, m_pVirtualUniverse);
}

StrongSceneNodePtr SceneGraph::SearchForActorInNode(ActorId actorId, StrongSceneNodePtr pSceneNode)
{
    if(pSceneNode->VHasActor() &&
       pSceneNode->VGetActor()->GetActorId()==actorId)
    {
        return pSceneNode;
    }
    
    if(pSceneNode->VHasChildren())
    {
        for(StrongSceneNodePtr pChildren : pSceneNode->VGetAllChildren())
        {
            StrongSceneNodePtr pResult =
                SearchForActorInNode(actorId, pChildren);
            
            if(pResult!=nullptr)
            {
                return pResult;
            }
        }
    }
    
    return nullptr;
}