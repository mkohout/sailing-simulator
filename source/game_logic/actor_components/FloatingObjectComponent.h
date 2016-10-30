#pragma once
#include "ActorComponent.h"
#include "IPhysicsComponent.h"
#include "OceanComponent.h"

namespace Game_View
{
	class ISceneNode;
}

namespace Game_Logic
{
	class FloatingObjectComponent : public ActorComponent, public IPhysicsComponent
	{
		OceanComponent* m_pOceanComp;
		shared_ptr<Game_View::ISceneNode> m_pOceanNode;
		shared_ptr<Game_View::ISceneNode> m_pNode;
		glm::vec2 uv;
	public:
		FloatingObjectComponent(bool randomPos = true);

		virtual ~FloatingObjectComponent()
		{

		}

		virtual void VInit();

		virtual string VGetIdentifier() const
		{
			return "floatingObjectComponent";
		}

		virtual void VFetchData(StrongXMLElementPtr);

		virtual void VUpdate(const unsigned int);

		void SetNormalizedPos(const glm::vec2& _uv)
		{
			uv = _uv;
		}

		glm::vec2 GetNormalizedPos() const
		{
			return uv;
		}
	};
}