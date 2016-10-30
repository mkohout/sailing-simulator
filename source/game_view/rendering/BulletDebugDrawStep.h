#pragma once

#include <memory>

#include "RenderingStep.h"
#include "../../game_logic/actor_components/RenderComponent.h"
#include <fastdelegate/FastDelegate.h>
#include <map>
#include "../scene_graph/LightNode.h"
#include "../video_system/GLProgram.h"
#include <glm/gtc/matrix_transform.hpp>

using std::shared_ptr;

namespace Game_View
{
	class Scene;

	class BulletDebugDrawStep : public RenderingStep
	{
		class btDebugDrawHelper;
		friend btDebugDrawHelper;
		struct VertexData
		{
			vec3 pos;
			vec3 col;
			VertexData() = default;
			VertexData(const vec3& v, const vec3& c)
				: pos(v), col(c)
			{
				
			}
		};
		int m_numVertices;
		int m_vOffset;
		VertexData* m_pVertexData;
		GLuint m_vertexArrayHandle;
		GLuint m_vertexBufferHandle;

		btDebugDrawHelper* m_pDrawHelper;
		void RenderNode(StrongSceneNodePtr, unsigned int);
	public:
		BulletDebugDrawStep();
		virtual void VExecute(SceneGraph*, unsigned int);
		virtual void VInit() { }
	};
}