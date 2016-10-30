#include "RenderToScreenStep.h"
#include "../video_system/OpenGLRenderer.h"
#include <iostream>
#include "../scene_graph/SkyNode.h"
#include "../../application_layer/core/GameCore.h"
#include <glm/gtx/transform.hpp>
#include <cmath>

#define FOV_SCALING 2

using namespace Game_View;

void RenderToScreenStep::DrawGeometry(
                    StrongGeometryPtr pGeometry)
{
    if(pGeometry==nullptr)
    {
        return ;
    }
    
    glDrawElements(
                 GL_TRIANGLES,
                 pGeometry->GetIndices().size(),
                 GL_UNSIGNED_INT,
                 (void*)0
                 );
    
    pGeometry->Disable();
}

void RenderToScreenStep::RenderRegularSceneNode(
                    StrongSceneNodePtr pSceneNode,
                    const unsigned int deltaMilliseconds,
                    StrongActorComponentPtr pActorComponent)
{
    RenderComponent* pRenderComponent =
                dynamic_cast<RenderComponent*>(pActorComponent.get());
    
    StrongGeometryPtr pGeometry =
                        pRenderComponent->GetGeometry();
	GLProgram* pCurrentProgram =
		m_pRenderer->GetProgram(pRenderComponent->GetShaderNames());

    if(m_drawMode == 1)
    {
		auto modelMat = pSceneNode->VGetTransformationMatrix();
		pCurrentProgram->AddUniform("uMVP", m_SSRProjView * modelMat);
		pCurrentProgram->AddUniform("uSSRCamPos", m_SSRCamPos);
    }

    DrawGeometry(pGeometry);
}

void RenderToScreenStep::RenderOceanSceneNode(
                StrongSceneNodePtr pSceneNode,
                const unsigned int deltaMilliseconds,
                StrongActorComponentPtr pActorComponent)
{
	StrongActorPtr pCurrentActor = pSceneNode->VGetActor();
	RenderComponent* pRenderComponent =
		dynamic_cast<RenderComponent*>(
			pCurrentActor->GetComponent("renderComponent").get());
	OceanComponent* pOceanComponent =
		dynamic_cast<OceanComponent*>(pCurrentActor->GetComponent("oceanComponent").get());

	GLProgram* pCurrentProgram =
		m_pRenderer->GetProgram(pRenderComponent->GetShaderNames());

	auto viewMat = m_pRenderer->GetCamera()->GetViewMatrix(), 
		 projMat = m_pRenderer->GetCamera()->GetProjectionMatrix(m_pRenderer->GetCanvasAspectRatio());
	AssignWaterTessellation(pSceneNode, pCurrentProgram, viewMat, projMat, m_pRenderer->GetCamera()->VGetPosition(), m_pRenderer->GetCamera()->GetDirection());
							
    if(g_CubeTexId != UINT_MAX)
    { //TODO; sollte anders gelöst werden
		pCurrentProgram->AddUniform("cubeMap", 6);
		glActiveTexture(GL_TEXTURE0 + 6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, g_CubeTexId);
		//AssignGLTex(pCurrentProgram, "cubeMap", g_CubeTexId, 6);
    }
  
	AssignGLTex(pCurrentProgram, "reflColorBuffer", m_renderedTexture, 7);
	AssignGLTex(pCurrentProgram, "reflDepthBuffer", m_renderedDepthTexture, 8);

	bool enableRaytrace = false;
#ifdef ENABLE_GPU_HEAVY
	enableRaytrace = true;
#endif
	pCurrentProgram->AddUniform("uEnableRaytrace", (float)enableRaytrace);
	pCurrentProgram->AddUniform("uSSRBoxMin", m_worldBoxSSR.min);
	pCurrentProgram->AddUniform("uSSRBoxMax", m_worldBoxSSR.max);
	pCurrentProgram->AddUniform("uProjViewMatrixExt", m_SSRProjView);
	pCurrentProgram->AddUniform("uInvProjViewMatrixExt", glm::inverse(m_SSRProjView));
	pCurrentProgram->AddUniform("uSSRCamPos", m_SSRCamPos);

	AssignGLTex(pCurrentProgram, "heightMap222", pOceanComponent->GetOpenGLHightTexture(), 15);
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArrays(GL_PATCHES, 0, pOceanComponent->GetNumberQuads() * pOceanComponent->GetNumberQuads() * 4);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	pOceanComponent->FinishFrame();

	//advace timer
	pOceanComponent->GetTimer()++;
}

void RenderToScreenStep::FindObjectAABB(StrongSceneNodePtr pSceneNode, const unsigned int)
{
	StrongActorPtr pCurrentActor = pSceneNode->VGetActor();
	if (pCurrentActor && pCurrentActor->HasComponent("renderComponent") &&
		dynamic_cast<SkyNode*>(pSceneNode.get()) == 0 && !pCurrentActor->HasComponent("oceanComponent") &&
		!pCurrentActor->HasComponent("floatingObjectComponent"))
	{
		RenderComponent* pRenderComponent =
			dynamic_cast<RenderComponent*>(pCurrentActor->GetComponent("renderComponent").get());
		StrongGeometryPtr pGeometry =
			pRenderComponent->GetGeometry();
		auto modelMat = pSceneNode->VGetTransformationMatrix();
		auto local_box = pGeometry->GetLocalAABB();
		m_worldBoxSSR = m_worldBoxSSR.Extend(local_box.Transform(modelMat));
		if (!std::isfinite(m_worldBoxSSR.min.x) || !std::isfinite(m_worldBoxSSR.min.y) || !std::isfinite(m_worldBoxSSR.min.z))
            std::cout << "Error during screen space reflections calculation : Infinite aabbs. Expect long rendering time!" << std::endl;
	}
}

void RenderToScreenStep::VInit()
{

}

glm::mat4 computeVieProjwMat(const AABB& box, vec3 cPos, const vec3& center, float aspect)
{
	vec3 corners[8];
	box.ComputeCorners(corners);
label0:
	float maxFov = 0;
	for (int i = 0; i < 8; i++)
	{
		float fov = glm::acos(dot(normalize(center - cPos), normalize(corners[i] - cPos)));
		maxFov = max(maxFov, fov);
	}
	//float fov_deg = glm::degrees(maxFov);
	//if(fov_deg >  90)
	//{
	//	cPos -= 1.0f * normalize(center - cPos);
	//	goto label0;
	//}
	auto viewMat = glm::lookAt(cPos, center, vec3(0, 1, 0));
	auto projMat = perspective(min(maxFov * 1.25f, glm::pi<float>() * 0.9f), aspect, 0.01f, 1e5f);
	return projMat * viewMat;
	//auto fwd = center - cPos;
	//fwd = abs(fwd.x) > abs(fwd.y) && abs(fwd.x) > abs(fwd.z) ? vec3(sign(fwd.x), 0, 0) : (abs(fwd.y) > abs(fwd.z) ? vec3(0, sign(fwd.y), 0) : vec3(0, 0, sign(fwd.z)));
	//auto viewMat = glm::lookAt(cPos, center, vec3(0, 1, 0));
	//auto box2 = box.Transform(viewMat);
	//auto projMat = glm::translate(-glm::vec3(1, 0, 1)) * glm::scale(vec3(2, 1, 2) / box2.Size()) * glm::translate(-box2.min);
	//return projMat * viewMat;
}

void RenderToScreenStep::VExecute(
            SceneGraph* pSceneGraph,
            const unsigned int deltaMilliseconds)
{
    if (!m_pRenderer->HasCamera())
    {
        return;
    }

	m_worldBoxSSR = AABB();
	RenderFunction aabbFunction =
		fastdelegate::MakeDelegate(
			this,
			&RenderToScreenStep::FindObjectAABB
			);
	ExecuteOnAllSceneNodes(pSceneGraph, deltaMilliseconds, aabbFunction);

	RenderFunction renderFunction =
		fastdelegate::MakeDelegate(
			this,
			&RenderToScreenStep::RenderSceneNode
			);

#ifdef ENABLE_GPU_HEAVY
	//find high quality perspective
	m_SSRCamPos = m_pRenderer->GetCamera()->VGetPosition();
	m_SSRProjView = computeVieProjwMat(m_worldBoxSSR, m_SSRCamPos, m_worldBoxSSR.Center(), (float)m_pRenderer->GetCanvasAspectRatio());

	if (m_framebufferName == -1)
	{
		m_SSRWidth = 512;
		float aspect = m_canvasHeight / float(m_canvasWidth);
		m_SSRHeight = (int)(m_SSRWidth * aspect);

		glGenFramebuffers(1, &m_framebufferName);
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferName);

		//color texture
		glGenTextures(1, &m_renderedTexture);
		glBindTexture(GL_TEXTURE_2D, m_renderedTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_SSRWidth, m_SSRHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//depth texture
		glGenTextures(1, &m_renderedDepthTexture);
		glBindTexture(GL_TEXTURE_2D, m_renderedDepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_SSRWidth, m_SSRHeight, 0, GL_RED, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glGenRenderbuffers(1, &m_depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_SSRWidth, m_SSRHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderedTexture, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_renderedDepthTexture, 0);
		GLenum DrawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, DrawBuffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			throw std::runtime_error("Invlaid shadow mapping fbo state!");
	}
	m_drawMode = 1;
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferName);
	glViewport(0, 0, m_SSRWidth, m_SSRHeight);
	glClearColor(0, 0, 0, 1);//clear first attachement to 0 and depth to 1
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float a = 1e5f;
	float f[] = {a,a,a,1};
	glClearBufferfv(GL_COLOR, 1, f);//clear linearized depth to inf
	glEnable(GL_SCISSOR_TEST);
	int o = 5;
	glScissor(o, o, m_SSRWidth - 2 * o, m_SSRHeight - 2 * o);
	ExecuteOnAllSceneNodes(pSceneGraph, deltaMilliseconds, renderFunction);
	glDisable(GL_SCISSOR_TEST);
#endif
	//actually set the target to the backbuffer
	VActivateBackbuffer();
	glClearColor(0, 0, 0, 1);//clear first attachemen to 0 and depth to 1
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	m_drawMode = 2;
    ExecuteOnAllSceneNodes(pSceneGraph, deltaMilliseconds, renderFunction);

	m_drawMode = 3;
	ExecuteOnAllSceneNodes(pSceneGraph, deltaMilliseconds, renderFunction);
}

void RenderToScreenStep::RenderSceneNode(
                StrongSceneNodePtr pSceneNode,
                const unsigned int deltaMilliseconds)
{
    StrongActorPtr pCurrentActor = pSceneNode->VGetActor();
    
    if (!pCurrentActor)
    {
        return;
    }
    
    if(pCurrentActor->HasComponent("renderComponent") && m_drawMode != 3)
    {
		RenderComponent* pRenderComponent =
			dynamic_cast<RenderComponent*>(pCurrentActor->GetComponent("renderComponent").get());
		GLProgram* pCurrentProgram =
			m_pRenderer->GetProgram(pRenderComponent->GetShaderNames());

		pCurrentProgram->Use();
		
		int lightIndex = 0;
		string uniformBase;
		for (LightNode* pLightNode : m_pRenderer->GetLights())
		{
			uniformBase = "uLights[" + std::to_string(lightIndex) + "].";
			const string projViewName = uniformBase + "ProjView";
			pCurrentProgram->AddUniform(projViewName, m_pShadowMapping->ComputeProjViewForLight(pLightNode));

			int texSlot = 9 + lightIndex;
			AssignGLTex(pCurrentProgram, "uShadowMap" + std::to_string(lightIndex), m_pShadowMapping->GetShadowMapId(pLightNode), texSlot);

			lightIndex++;
		}
		pCurrentProgram->AddUniform("uMinVariance", 0.1f);

        if (pCurrentActor->HasComponent("oceanComponent"))
        {
			if(m_drawMode != 1)
			{
				PreRenderAssignments(pSceneNode);
				RenderOceanSceneNode(
					pSceneNode,
					deltaMilliseconds,
					pCurrentActor->GetComponent("oceanComponent"));
			}
        }
        else if(m_drawMode != 1 || dynamic_cast<SkyNode*>(pSceneNode.get()) == 0)
        {
			PreRenderAssignments(pSceneNode);
            RenderRegularSceneNode(
                    pSceneNode,
                    deltaMilliseconds,
                    pCurrentActor->GetComponent("renderComponent"));
        }
    }

	if(pCurrentActor->HasComponent("particleSystemComponent") && m_drawMode  == 3)
	{
		mat4 viewMatrix = m_pRenderer->GetCamera() != 0 ?
			m_pRenderer->GetCamera()->GetViewMatrix() : mat4();
		mat4 projectionMatrix = m_pRenderer->GetCamera() != 0 ?
			m_pRenderer->GetCamera()->GetProjectionMatrix(
				m_pRenderer->GetCanvasAspectRatio()) : mat4();
		auto modelMatrix = pSceneNode->VGetTransformationMatrix();
		std::map<IShader::ShaderType, string> names;
		names[IShader::ShaderType::VERTEX_SHADER] = "particle";
		names[IShader::ShaderType::FRAGMENT_SHADER] = "particle";
		GLProgram* pCurrentProgram = m_pRenderer->GetProgram(names);
		pCurrentProgram->Use();
		pCurrentProgram->AddUniform("uModelMatrix", modelMatrix);
		pCurrentProgram->AddUniform("uViewMatrix", viewMatrix);
		pCurrentProgram->AddUniform("uProjMatrix", projectionMatrix);
		pCurrentProgram->AddUniform("uProjViewMatrix", projectionMatrix * viewMatrix);

		auto pSystem = (ParticleSystemComponent*)pCurrentActor->GetComponent("particleSystemComponent").get();
		pSystem->LoadOpenGLData([&](GLuint a, Texture& b) {AssignTexture(a, std::make_shared<Game_View::Texture>(b)); });
		AssignGLTex(pCurrentProgram, "uParticleMap", pSystem->GetParticleMap(), 3);
		pCurrentProgram->AddUniform("uDecayTime", pSystem->GetDecayTime());
		pSystem->Draw();
	}
}