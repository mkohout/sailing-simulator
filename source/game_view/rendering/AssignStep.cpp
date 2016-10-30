#include "AssignStep.h"
#include "../video_system/OpenGLRenderer.h"
#include "../scene_graph/SceneNode.h"
#include "../scene_graph/LightNode.h"
#include "../scene_graph/CameraNode.h"

using namespace Game_View;

void AssignStep::VExecute(
                SceneGraph* pSceneGraph,
                const unsigned int deltaMilliseconds)
{
    RenderFunction renderFunction =
                fastdelegate::MakeDelegate(
                               this,
                               &AssignStep::AssignFromSceneNode
                               );
    
    ExecuteOnAllSceneNodes(pSceneGraph, deltaMilliseconds, renderFunction);
}

void AssignStep::AssignFromSceneNode(
                StrongSceneNodePtr pSceneNode,
                const unsigned int deltaMilliseconds)
{
    if (!pSceneNode->VHasActor())
    {
        LightNode* pLightNode = dynamic_cast<LightNode*>(pSceneNode.get());
        
        if (pLightNode != nullptr)
        {
            m_pRenderer->AddLight(pLightNode);
            return;
        }
        
        CameraNode* pCameraNode = dynamic_cast<CameraNode*>(pSceneNode.get());
        
        if (pCameraNode != nullptr)
        {
            m_pRenderer->SetCamera(pCameraNode);
            return;
        }
        
        return;
    }
}