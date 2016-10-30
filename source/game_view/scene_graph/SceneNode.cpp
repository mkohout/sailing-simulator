#include "SceneNode.h"
#include <glfw/glfw3.h>
#include <glm/gtx/matrix_decompose.hpp>

using namespace Game_View;

void SceneNode::VSetActor(StrongActorPtr pActor)
{
	m_pActor = pActor;
}

void SceneNode::VInitByXMLElement(StrongXMLElementPtr pXmlElement)
{
}

void SceneNode::VAddChildNode(StrongSceneNodePtr pSceneNode)
{
	m_childNodes.push_back(pSceneNode);
}

void SceneNode::VRemoveChildNode(StrongSceneNodePtr pSceneNode)
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

void SceneNode::VSetTransformationMatrix(const mat4& transform)
{
	m_transform = transform;
}

void SceneNode::VTransform(const mat4& transform)
{
	m_transform *= transform;
}

void SceneNode::SetPosition(vec3 position)
{
    VTransform(translate(mat4(1.0f), position));
}

void SceneNode::SetPosition(double x, double y, double z)
{
	SceneNode::SetPosition(vec3(x, y, z));
}

vec3 SceneNode::VGetPosition() const
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

const mat4 SceneNode::VGetTransformationMatrix() const
{
	return m_transform;
}

SceneNodes& SceneNode::VGetAllChildren()
{
	return m_childNodes;
}

StrongActorPtr SceneNode::VGetActor() const
{
	return m_pActor;
}

bool SceneNode::VHasActor() const
{
	if (m_pActor == nullptr)
	{
		return false;
	}

	return true;
}

void SceneNode::VSetParent(StrongSceneNodePtr pSceneNode)
{
    m_pParent = pSceneNode;
}