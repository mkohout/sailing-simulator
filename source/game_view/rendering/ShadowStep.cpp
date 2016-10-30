#include "ShadowStep.h"
#include "../video_system/OpenGLRenderer.h"
#include "../scene_graph/SkyNode.h"

namespace Game_View
{

ShadowStep::ShadowMapData::ShadowMapData(int size)
	: m_size(size)
{
	glGenFramebuffers(1, &m_framebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferName);

	glGenTextures(1, &m_renderedTexture);
	glBindTexture(GL_TEXTURE_2D, m_renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, size, size, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	glGenRenderbuffers(1, &m_depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size, size);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthrenderbuffer);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderedTexture, 0);
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("Invlaid shadow mapping fbo state!");

	m_extend = 1200;
	m_fov = glm::radians(30.0f);
	m_nearFar = glm::vec2(0.1f, 1e4f);
}

ShadowStep::ShadowStep()
	: m_texSize(1024), m_targetNode(0), m_blurMap(m_texSize), m_blurProgram(0)
{

}

void ShadowStep::RenderNode(StrongSceneNodePtr pSceneNode, unsigned int)
{
	StrongActorPtr pCurrentActor = pSceneNode->VGetActor();

	if (!pCurrentActor || dynamic_cast<SkyNode*>(pSceneNode.get()) != 0)
	{
		return;
	}

	if (pCurrentActor->HasComponent("renderComponent"))
	{
		RenderComponent* pRenderComponent =
			dynamic_cast<RenderComponent*>(pCurrentActor->GetComponent("renderComponent").get());
		auto names = pRenderComponent->GetShaderNames();
		names[IShader::FRAGMENT_SHADER] = "shadow";
		GLProgram* pCurrentProgram = m_pRenderer->GetProgram(names);
		pCurrentProgram->Use();

		pCurrentProgram->AddUniform("LightPos", m_pcurrentLight->VGetPosition());

		if (pCurrentActor->HasComponent("oceanComponent"))
		{
			OceanComponent* pOceanComponent =
				dynamic_cast<OceanComponent*>(pCurrentActor->GetComponent("oceanComponent").get());
			AssignWaterTessellation(pSceneNode, pCurrentProgram, m_ViewMat, m_ProjMat, m_pcurrentLight->VGetPosition(), m_pcurrentLight->GetDirection(), 5);
			glPatchParameteri(GL_PATCH_VERTICES, 4);
			glDrawArrays(GL_PATCHES, 0, pOceanComponent->GetNumberQuads() * pOceanComponent->GetNumberQuads() * 4);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			mat4 modelMatrix = pSceneNode->VGetTransformationMatrix();
			pCurrentProgram->AddUniform("uModelMatrix", modelMatrix);
			pCurrentProgram->AddUniform("uMVP", m_ProjViewMat * modelMatrix);

			StrongGeometryPtr pGeometry =
				pRenderComponent->GetGeometry();

			if (pGeometry != nullptr)
			{
				pGeometry->Enable();
				glDrawElements(
					GL_TRIANGLES,
					pGeometry->GetIndices().size(),
					GL_UNSIGNED_INT,
					(void*)0
					);
				pGeometry->Disable();
			}
		}
	}
}

void ShadowStep::CreateShadowMap(LightNode* pLight, const ShadowMapData& map)
{
	//gnerate the actual shadow map
	glBindFramebuffer(GL_FRAMEBUFFER, map.m_framebufferName);
	glViewport(0, 0, map.m_size, map.m_size);
	int o = 5;
	float d = 1e5f;
	glClearColor(d, d*d, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifdef ENABLE_GPU_HEAVY
	glEnable(GL_SCISSOR_TEST);
	glScissor(o, o, map.m_size - 2 * o, map.m_size - 2 * o);

	RenderFunction renderFunction =
		fastdelegate::MakeDelegate(
			this,
			&ShadowStep::RenderNode
			);


	m_pcurrentLight = pLight;
	m_ViewMat = ComputeViewMat(pLight);
	m_ProjMat = ComputeProjMat(pLight);
	m_ProjViewMat = m_ProjMat * m_ViewMat;

	ExecuteOnAllSceneNodes(m_pSceneGraph, 0, renderFunction);
	glDisable(GL_SCISSOR_TEST);

	//blur the shadow map, two passes: horizontal, vertical
	glDisable(GL_DEPTH_TEST);

	m_blurProgram->Use();
	glBindFramebuffer(GL_FRAMEBUFFER, m_blurMap.m_framebufferName);
	m_blurProgram->AddUniform("uBlurDirection", vec2(1, 0));
	m_blurProgram->AddUniform("uInvTextureSize", 1.0f / map.m_size);
	m_blurProgram->AddUniform("s_texture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, map.m_renderedTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, map.m_framebufferName);
	m_blurProgram->AddUniform("uBlurDirection", vec2(0, 1));
	m_blurProgram->AddUniform("uInvTextureSize", 1.0f / map.m_size);
	m_blurProgram->AddUniform("s_texture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_blurMap.m_renderedTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST);
#endif
}

void ShadowStep::IterateLights(StrongSceneNodePtr pSceneNode, const unsigned int)
{
	if (pSceneNode->VHasActor())
		if (pSceneNode->VGetActor()->IsMainActor())
			m_targetNode = pSceneNode;

	if (!pSceneNode->VHasActor())
	{
		LightNode* pLightNode = dynamic_cast<LightNode*>(pSceneNode.get());

		if (pLightNode != nullptr)//&& pLightNode->CastShadows() TODO implement!
		{
			auto it = m_shadowMaps.find(pLightNode);
			if(it == m_shadowMaps.end())
			{
				m_shadowMaps[pLightNode] = ShadowMapData(m_texSize);//TODO use dynamic size based on light
				it = m_shadowMaps.find(pLightNode);
			}
			CreateShadowMap(pLightNode, it->second);
		}
	}
}

void ShadowStep::VExecute(SceneGraph* pSceneGraph, unsigned int deltaMilliseconds)
{
	if(m_blurProgram == 0)
	{
		map<IShader::ShaderType, string> shader;
		shader[IShader::ShaderType::VERTEX_SHADER] = "fullscreen";
		shader[IShader::ShaderType::FRAGMENT_SHADER] = "blur";
		m_blurProgram = m_pRenderer->GetProgram(shader);
	}

	RenderFunction renderFunction =
		fastdelegate::MakeDelegate(
			this,
			&ShadowStep::IterateLights
			);
	m_pSceneGraph = pSceneGraph;
	ExecuteOnAllSceneNodes(pSceneGraph, deltaMilliseconds, renderFunction);

}

glm::mat4 ShadowStep::ComputeViewMat(LightNode* pLight)
{
	auto& dat = m_shadowMaps[pLight];
	glm::vec3 dir = pLight->GetDirection(), pos = pLight->VGetPosition();
	bool dirLight = pLight->GetType() == LIGHT_TYPE::DIRECTIONAL_LIGHT;
	if (dirLight)
	{
		//compute projection of target onto orthogonal plane
		const float gamma = 1e3f;//radius of scene
		AABB local_box;
		if(m_targetNode->VHasActor() && m_targetNode->VGetActor()->HasComponent("renderComponent"))
		{
			RenderComponent* pRenderComponent =
				dynamic_cast<RenderComponent*>(m_targetNode->VGetActor()->GetComponent("renderComponent").get());
			local_box = pRenderComponent->GetGeometry()->GetLocalAABB();
		}
		else
		{
			auto p = m_targetNode->VGetPosition();
			local_box = AABB(p, p);
		}

		vec3 local_corners[8];
		local_box.ComputeCorners(local_corners);
		//float min_lambda = FLT_MAX;
		vec3 center_projected = vec3(0);
		auto toWorld = m_targetNode->VGetTransformationMatrix();
		for (int i = 0; i < 8; i++)
		{
			auto q = toWorld * vec4(local_corners[i], 1.0f);
			auto corner_world = vec3(q.x, q.y, q.z);
			float lambda = -(gamma - dot(corner_world, dir)) / glm::dot(dir, dir);
			//min_lambda = std::min(min_lambda, lambda);
			center_projected += corner_world + lambda * dir;
		}
		center_projected /= 8.0f;

		float max_fov = 0;//using min of dot would be faster due to less trigs
		for (int i = 0; i < 8; i++)
		{
			auto q = toWorld * vec4(local_corners[i], 1.0f);
			auto corner_world = vec3(q.x, q.y, q.z);
			float fov = acosf(dot(normalize(corner_world - center_projected), normalize(dir)));
			max_fov = std::max(max_fov, fov);
		}

		dat.m_fov = max_fov * 2;
		pos = center_projected;
		pLight->VSetTransformationMatrix(glm::translate(glm::mat4(1), pos));
	}
	glm::mat4 view = glm::lookAt(pos, pos + dir, glm::vec3(0, 1, 0));
	return view;
}

}