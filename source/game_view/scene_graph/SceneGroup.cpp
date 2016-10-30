#include "SceneGroup.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

using namespace Game_View;

void SceneGroup::VInitByXMLElement(StrongXMLElementPtr pXmlElement)
{
    if(pXmlElement->HasChildrenNamed("position"))
    {
        StrongXMLElementPtr pPositionElement =
            pXmlElement->FindFirstChildNamed("position");
        
        vec3 position = vec3(
                    std::stof(pPositionElement->GetValueOfAttribute("x")),
                    std::stof(pPositionElement->GetValueOfAttribute("y")),
                    std::stof(pPositionElement->GetValueOfAttribute("z"))
                    );
        const mat4 positionTransform =
                    glm::translate(mat4(1.0f), position);
        
        VTransform(positionTransform);
    }
}

void SceneGroup::VAddChildNode(StrongSceneNodePtr pSceneNode)
{
    // Apply current transformation
    pSceneNode->VTransform(m_transform);
    
	m_childNodes.push_back(pSceneNode);
}

void SceneGroup::VRemoveChildNode(StrongSceneNodePtr pSceneNode)
{
    int index = 0;
    for(StrongSceneNodePtr pChildNode : m_childNodes)
    {
        if(pChildNode!=pSceneNode)
        {
            index++;
            continue;
        }
        
        vector<StrongSceneNodePtr>::iterator nth =
                        m_childNodes.begin() + index;
        
        m_childNodes.erase(nth);
        
        return ;
    }
}

void SceneGroup::VSetTransformationMatrix(const mat4& transform)
{
    m_transform = transform;
    
    for(StrongSceneNodePtr pChildNode : m_childNodes)
    {
        pChildNode->VSetTransformationMatrix(transform);
    }
}

void SceneGroup::VTransform(const mat4& transform)
{
    m_transform *= transform;
    
    for(StrongSceneNodePtr pChildNode : m_childNodes)
    {
        pChildNode->VTransform(transform);
    }
}

const mat4x4 SceneGroup::VGetTransformationMatrix() const
{
	return m_transform;
}

SceneNodes& SceneGroup::VGetAllChildren()
{
	return m_childNodes;
}

StrongActorPtr SceneGroup::VGetActor() const
{
	return nullptr;
}

bool SceneGroup::VHasActor() const
{
	return false;
}

vec3 SceneGroup::VGetPosition() const
{
    vec3 position;
    vec3 scale;
    quat orientation;
    vec3 skew;
    vec4 perspective;
    
    glm::decompose(
              m_transform,
              scale,
              orientation,
              position,
              skew,
              perspective
              );
    
    return position;
}

void SceneGroup::VSetActor(StrongActorPtr pActor)
{
}

void SceneGroup::VSetParent(StrongSceneNodePtr pSceneNode)
{
    m_pParent = pSceneNode;
}