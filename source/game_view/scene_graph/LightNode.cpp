#include "LightNode.h"
#include <glm/gtx/matrix_decompose.hpp>

using namespace Game_View;

void LightNode::VInitByXMLElement(StrongXMLElementPtr pXmlElement)
{
	if (!pXmlElement->HasAttributeNamed("type"))
	{
		return;
	}

	const string type = pXmlElement->GetValueOfAttribute("type");

	SetTypeByString(type);

	if (GetType() != DIRECTIONAL_LIGHT)
	{
		if (pXmlElement->HasChildrenNamed("position"))
		{
			StrongXMLElementPtr pPositionElement =
				pXmlElement->FindFirstChildNamed("position");

            vec3 position(
                          std::stof(pPositionElement->GetValueOfAttribute("x")),
                          std::stof(pPositionElement->GetValueOfAttribute("y")),
                          std::stof(pPositionElement->GetValueOfAttribute("z")));
            
            VTransform(glm::translate(mat4(1.0f), position));
		}
	}

	if (pXmlElement->HasChildrenNamed("energy"))
	{
		StrongXMLElementPtr pEnergyElement =
			pXmlElement->FindFirstChildNamed("energy");

		SetEnergy(
			std::stof(pEnergyElement->GetValueOfAttribute("value"))
			);
	}

	if (pXmlElement->HasChildrenNamed("color"))
	{
		StrongXMLElementPtr pColorElement =
			pXmlElement->FindFirstChildNamed("color");

		SetColor(
			std::stof(pColorElement->GetValueOfAttribute("r")),
			std::stof(pColorElement->GetValueOfAttribute("g")),
			std::stof(pColorElement->GetValueOfAttribute("b"))
			);
	}

	if (GetType() != POINT_LIGHT)
	{
		if (pXmlElement->HasChildrenNamed("direction"))
		{
			StrongXMLElementPtr pDirectionElement =
				pXmlElement->FindFirstChildNamed("direction");

			SetDirection(
				std::stof(pDirectionElement->GetValueOfAttribute("x")),
				std::stof(pDirectionElement->GetValueOfAttribute("y")),
				std::stof(pDirectionElement->GetValueOfAttribute("z"))
				);
		}
	}
}

void LightNode::VSetTransformationMatrix(const mat4& transform)
{
	m_transform = transform;
}

void LightNode::VTransform(const mat4& transform)
{
	m_transform = m_transform * transform;
}

const mat4 LightNode::VGetTransformationMatrix() const
{
	return m_transform;
}

SceneNodes& LightNode::VGetAllChildren()
{
	return m_childNodes;
}

StrongActorPtr LightNode::VGetActor() const
{
	return nullptr;
}

bool LightNode::VHasActor() const
{
	return false;
}

void LightNode::SetTypeByString(string type)
{
	if (type.compare("pointLight") == 0)
	{
		m_type = POINT_LIGHT;
	}
	else if (type.compare("directionalLight") == 0)
	{
		m_type = DIRECTIONAL_LIGHT;
	}
	else if (type.compare("spotLight") == 0)
	{
		m_type = SPOT_LIGHT;
	}
}

vec3 LightNode::VGetPosition() const
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

void LightNode::VSetActor(StrongActorPtr pActor)
{
}

void LightNode::VSetParent(StrongSceneNodePtr pSceneNode)
{
    m_pParent = pSceneNode;
}