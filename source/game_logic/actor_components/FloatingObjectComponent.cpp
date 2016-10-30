#include "FloatingObjectComponent.h"
#include <random>
#include "../../application_layer/core/GameCore.h"
#include "../game_view/scene_graph/ISceneNode.h"
#include <iostream>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/transform.hpp>

namespace Game_Logic
{
	StrongSceneNodePtr FindOceanNode(StrongSceneNodePtr pSceneNode)
	{
		if (pSceneNode->VHasChildren())
		{
			for (StrongSceneNodePtr pChildNode : pSceneNode->VGetAllChildren())
			{
				auto f = FindOceanNode(pChildNode);
				if (f) return f;
			}
		}

		StrongActorPtr pCurrentActor = pSceneNode->VGetActor();

		if (!pCurrentActor)
		{
			return 0;
		}

		for (auto& mapEntry : pCurrentActor->GetComponents())
		{
			OceanComponent* pPhysicsComponent =
				dynamic_cast<OceanComponent*>(mapEntry.second.get());

			if (pPhysicsComponent == nullptr)
			{
				continue;
			}

			return pSceneNode;
		}
		return 0;
	}

	
	FloatingObjectComponent::FloatingObjectComponent(bool randomPos)
		: m_pNode(0)
	{
		static std::random_device rd;
		static std::mt19937 e2(rd());
		static std::uniform_real_distribution<> dist(0, 1);
		if (randomPos)
			uv = vec2(dist(e2), dist(e2));
	}

	void FloatingObjectComponent::VFetchData(StrongXMLElementPtr pXmlElement)
	{
		auto offXml = pXmlElement->FindFirstChildNamed("uv");
		if (offXml)
		{
			auto posXml = offXml->FindFirstChildNamed("position");
			uv = vec2(std::stof(posXml->GetValueOfAttribute("u", "0")),
				std::stof(posXml->GetValueOfAttribute("v", "0")));
		}
	}

	void FloatingObjectComponent::VInit()
	{

	}

	void FloatingObjectComponent::VUpdate(const unsigned int)
	{
		if(m_pOceanComp && m_pNode)
		{
			vec3 frame[3];
			vec3 pos = m_pOceanComp->GetFrameAtUV(uv, frame[0], frame[1], frame[2]);
			mat4 mat = mat4(1);
			for (int i = 0; i < 3; i++)
				mat = glm::column(mat, i, vec4(frame[i][0], frame[i][1], frame[i][2], 0));
			m_pNode->VSetTransformationMatrix(glm::translate(pos) * mat);
		}
		else
		{
			auto actor = GetActor();
			m_pNode = g_pGameApp->GetCurrentScene()->GetSceneGraph()->FindNodeWithActor(actor->GetActorId());
			m_pOceanNode = FindOceanNode(g_pGameApp->GetCurrentScene()->GetSceneGraph()->GetRootNode());
			if (m_pOceanNode)
				m_pOceanComp = (OceanComponent*)m_pOceanNode->VGetActor()->GetComponent("oceanComponent").get();
			else std::cout << "Created FloatingObjectComponent without water object.";
		}
	}

}