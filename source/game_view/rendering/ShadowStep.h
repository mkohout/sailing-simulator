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

	/**
	* TODO
	*/
	class ShadowStep : public RenderingStep
	{
		struct ShadowMapData
		{
			int m_size;

			float m_fov;
			float m_extend;
			glm::vec2 m_nearFar;

			GLuint m_framebufferName;
			GLuint m_renderedTexture;
			GLuint m_depthrenderbuffer;

			ShadowMapData(int size);
			ShadowMapData()
				: m_size(-1)
			{
				
			}
		};
		std::map<LightNode*, ShadowMapData> m_shadowMaps;
		int m_texSize;

		mat4 m_ProjViewMat, m_ProjMat, m_ViewMat;
		SceneGraph* m_pSceneGraph;
		LightNode* m_pcurrentLight;
		StrongSceneNodePtr m_targetNode;

		ShadowMapData m_blurMap;
		GLProgram* m_blurProgram;
	private:

		void IterateLights(StrongSceneNodePtr, const unsigned int);

		void RenderNode(StrongSceneNodePtr, unsigned int);

		void CreateShadowMap(LightNode*, const ShadowMapData&);

	public:
		ShadowStep();

        /**
         * TODO
         */
        virtual void VInit() { }
        
		/**
		* TODO
		*/
		virtual void VExecute(SceneGraph*, unsigned int);

		glm::mat4 ComputeViewMat(LightNode* pLight);

		glm::mat4 ComputeProjMat(LightNode* pLight)
		{
			auto dat = m_shadowMaps[pLight];
			float e = dat.m_extend;
			glm::mat4 proj =/* dirLight ? glm::ortho(-e, e, -e, e) : */glm::perspective(dat.m_fov, 1.0f, dat.m_nearFar.x, dat.m_nearFar.y);
			return proj;
		}

		glm::mat4 ComputeProjViewForLight(LightNode* pLight)
		{
			return ComputeProjMat(pLight) * ComputeViewMat(pLight);
		}

		GLuint GetShadowMapId(LightNode* pLight)
		{
			return m_shadowMaps[pLight].m_renderedTexture;
		}

	};
}