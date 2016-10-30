#include "SkyNode.h"
#include "../../application_layer/core/GameCore.h"
#include <fastdelegate/FastDelegate.h>
#include <glm/gtx/matrix_decompose.hpp>

using namespace Game_View;

void SkyNode::VInitByXMLElement(StrongXMLElementPtr pXmlElement)
{
}

void SkyNode::VSetTransformationMatrix(const mat4& transform)
{
	m_transform = mat4(1.0f);
}

void SkyNode::VTransform(const mat4& transform)
{
	m_transform *= transform;
}

const mat4x4 SkyNode::VGetTransformationMatrix() const
{
	return m_transform;
}

SceneNodes& SkyNode::VGetAllChildren()
{
	return m_childNodes;
}

StrongActorPtr SkyNode::VGetActor() const
{
	return m_pActor;
}

bool SkyNode::VHasActor() const
{
	return m_pActor != nullptr;
}

void SkyNode::VSetActor(StrongActorPtr pActor)
{
	m_pActor = pActor;
}

void SkyNode::VSetParent(StrongSceneNodePtr pSceneNode)
{
    m_pParent = pSceneNode;
}

vec3 SkyNode::VGetPosition() const
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