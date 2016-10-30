#include "OpenGLRenderer.h"

#include "../../game_logic/game_app/GameApp.h"
#include "../../application_layer/core/stdafx.h"
#include "../../application_layer/core/GameCore.h"
#include <sstream>
#include "../game_logic/actor_components/OceanComponent.h"

#include "../rendering/AssignStep.h"
#include "../rendering/RenderToScreenStep.h"
#include "../rendering/PostRenderingStep.h"
#include "../rendering/ShadowStep.h"
#include "../rendering/BulletDebugDrawStep.h"

#include <limits>
#include <memory>

using namespace Game_View;

OpenGLRenderer::~OpenGLRenderer()
{
}

void OpenGLRenderer::StartRendering(
            unsigned int deltaMilliseconds,
            SceneGraph* pSceneGraph)
{
    for(auto pRenderingStep : m_renderingSteps)
    {
        pRenderingStep->VExecute(
                    pSceneGraph, deltaMilliseconds);
    }
}

void OpenGLRenderer::Init(const int canvasWidth, const int canvasHeight)
{
    InitializeRenderingSteps();
	SetCanvasSize(canvasWidth, canvasHeight);

	BackFaceCulling(true);
	DepthTest(true);
}

void OpenGLRenderer::InitializeRenderingSteps()
{
	auto* shadow = new ShadowStep();
	m_renderingSteps.push_back(
		StrongRenderStepPtr(shadow));
    m_renderingSteps.push_back(
            StrongRenderStepPtr(new AssignStep()));
    m_renderingSteps.push_back(
            StrongRenderStepPtr(new RenderToScreenStep(shadow)));
    /*
	m_renderingSteps.push_back(
		StrongRenderStepPtr(new BulletDebugDrawStep()));
     */
	m_renderingSteps.push_back(
		StrongRenderStepPtr(new PostRenderingStep()));
    
    for(shared_ptr<IRenderingStep> pRenderingStep : m_renderingSteps)
    {
        pRenderingStep->VSetRenderer(this);
        pRenderingStep->VInit();
    }
}

void OpenGLRenderer::SetCanvasSize(const int canvasWidth, const int canvasHeight)
{
	m_canvasWidth = canvasWidth;
	m_canvasHeight = canvasHeight;
	for (auto pRenderingStep : m_renderingSteps)
		pRenderingStep->VSetCanvasSize(canvasWidth, canvasHeight);
}

GLSLShader* OpenGLRenderer::AddShader(
                IShader::ShaderType whichShader, const string& shaderName)
{
    const string fullName =
                    GetFullShaderName(whichShader, shaderName);
    const auto findIt = m_usedShaders.find(fullName);
    
    if (findIt == m_usedShaders.end())
    {
        GLSLShader* pShader = new GLSLShader();
        
        pShader->VLoadFromFile(
                              whichShader,
                              g_pGameApp->GetFileManager()->GetPathToShaderFile(fullName)
                              );
        
        m_usedShaders.insert(
                             pair<string, GLSLShader*>(fullName, pShader));
        return pShader;
    }
    
    return findIt->second;
}

GLSLShader* OpenGLRenderer::GetShader(
                IShader::ShaderType whichShader, const string& shaderName)
{
	const string fullName =
		GetFullShaderName(whichShader, shaderName);
	const auto findIt = m_usedShaders.find(fullName);

	if (findIt == m_usedShaders.end())
	{
        return AddShader(whichShader, shaderName);
	}

	return findIt->second;
}

const string OpenGLRenderer::GetFullShaderName(
	IShader::ShaderType whichShader, string shaderName)
{
	const string output = GetShaderPrefix(whichShader) +
		shaderName +
		"." + GetShaderType(whichShader);

	return output;
}

const string OpenGLRenderer::GetShaderPrefix(
	IShader::ShaderType whichShader)
{
    switch(whichShader)
    {
        case IShader::VERTEX_SHADER:
            return "vsh_";
            
        case IShader::FRAGMENT_SHADER:
            return "fsh_";
            
        case IShader::COMPUTE_SHADER:
            return "csh_";
            
        case IShader::TESSELLATION_CONTROL_SHADER:
            return "tcs_";

		case IShader::TESSELLATION_EVAL_SHADER:
			return "tes_";
            
        case IShader::GEOMETRY_SHADER:
            return "gsh_";
            
        case IShader::NO_SHADER:
        default:
            return "";
    }

	return "";
}

const string OpenGLRenderer::GetShaderType(IShader::ShaderType whichShader)
{
	return "glsl";
}

GLProgram* OpenGLRenderer::GetProgram(
                    map<IShader::ShaderType, string> shaderList)
{
	const auto findIt =
            m_programs.find(GetShaderListHash(shaderList));

	if (findIt != m_programs.end())
	{
		return findIt->second;
	}

	GLProgram* pProgram = new GLProgram();

	pProgram->Init();
    
    for(auto& mapEntry : shaderList)
    {
        pProgram->AddShader(GetShader(
                            mapEntry.first,
                            mapEntry.second));
    }

	pProgram->Link();

	SaveProgram(
        pProgram,
        shaderList
        );

	return pProgram;
}

void OpenGLRenderer::ResetShaders()
{
    m_usedShaders.clear();
}

ShaderHash OpenGLRenderer::GetShaderListHash(
                        const std::map<IShader::ShaderType, string> &shaderNames)
{
    std::stringstream hash;
    
    for(auto& mapEntry : shaderNames)
    {
        hash << GetShaderPrefix(mapEntry.first) << mapEntry.second << "|";
    }
    
    return hash.str();
}

void OpenGLRenderer::SaveProgram(
	GLProgram* pProgram,
    const std::map<IShader::ShaderType, string> &shaderNames)
{
	m_programs.insert(pair<ShaderHash, GLProgram*>(
		GetShaderListHash(shaderNames),
		pProgram
		));
}

void OpenGLRenderer::AddLight(LightNode* pLightNode)
{
	if (std::find(
		m_lights.begin(), m_lights.end(), pLightNode) != m_lights.end())
	{
		return;
	}

	m_lights.push_back(pLightNode);
}

void OpenGLRenderer::SetCamera(CameraNode* pCameraNode)
{
	m_pCurrentCamera = pCameraNode;
}

void OpenGLRenderer::BackFaceCulling(GLboolean enabled)
{
	if (!enabled)
	{
		glDisable(GL_CULL_FACE);
		return;
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void OpenGLRenderer::DepthTest(GLboolean enabled)
{
	if (!enabled)
	{
		glDisable(GL_DEPTH_TEST);
		return;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}
