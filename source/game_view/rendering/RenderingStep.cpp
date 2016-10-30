#include "RenderingStep.h"
#include "../video_system/OpenGLRenderer.h"
#include "../scene_graph/SceneGraph.h"
#include <glm/gtc/matrix_access.hpp>
#include "../../application_layer/core/GameCore.h"
#include "../game_logic/physics/Force.h"
#include <iostream>

using namespace Game_View;

void RenderingStep::ExecuteOnSceneNode(
                        shared_ptr<ISceneNode> pSceneNode,
                        const unsigned int deltaMilliseconds,
                        RenderFunction renderFunction)
{
    if(pSceneNode->VHasChildren())
    {
        for (StrongSceneNodePtr pChildNode : pSceneNode->VGetAllChildren())
        {
            ExecuteOnSceneNode(
                pChildNode, deltaMilliseconds, renderFunction);
        }
    }
    
    renderFunction(pSceneNode, deltaMilliseconds);
}

void RenderingStep::ExecuteOnAllSceneNodes(
                SceneGraph* pSceneGraph,
                const unsigned int deltaMilliseconds,
                RenderFunction renderFunction)
{
    StrongSceneNodePtr pSceneNode = pSceneGraph->GetRootNode();
    
    ExecuteOnSceneNode(
            pSceneNode, deltaMilliseconds, renderFunction);
}

void RenderingStep::PreRenderAssignments(StrongSceneNodePtr pSceneNode)
{
    StrongActorPtr pCurrentActor = pSceneNode->VGetActor();
    
    if (!pCurrentActor)
        return;
    
    if(!pCurrentActor->HasComponent("renderComponent"))
    {
        return ;
    }
    
    RenderComponent* pRenderComponent =
        dynamic_cast<RenderComponent*>(
                pCurrentActor->GetComponent("renderComponent").get());
    
    Materials materials =
                    pRenderComponent->GetMaterials();
    StrongGeometryPtr pGeometry =
                    pRenderComponent->GetGeometry();
    
    AssignShaders(pRenderComponent);
    
    GLProgram* pCurrentProgram =
                        m_pRenderer->GetProgram(pRenderComponent->GetShaderNames());
    
    AssignShaderParameters(
            pCurrentProgram, pRenderComponent);
    AssignMatrices(
            pCurrentProgram, pSceneNode);
    AssignCamera(pCurrentProgram);
    AssignLights(pCurrentProgram);
    AssignGeometry(pGeometry);
    AssignMaterials(
            pCurrentProgram, materials);
    AssignTextures(pCurrentActor, pRenderComponent);
}

void RenderingStep::AssignTexture(
                    GLuint textureID, StrongTexturePtr pTexture)
{
    GLint format = (pTexture->GetChannels()==4 ? GL_RGBA : GL_RGB);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    for(auto& parameterEntry : pTexture->GetParameters())
    {
        glTexParameteri(
                        GL_TEXTURE_2D,
                        parameterEntry.first,
                        parameterEntry.second
                        );
    }
    
    glTexImage2D(
                 GL_TEXTURE_2D,
                 0,
                 format,
                 pTexture->GetWidth(),
                 pTexture->GetHeight(),
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 pTexture->GetImageData()
                 );
    
    pTexture->Dispose();
}

void RenderingStep::AssignMaterials(
                    GLProgram* pCurrentProgram, Materials materials)
{
    string uniformBase;
    short materialIndex = 0;
    
    for(Material& material : materials)
    {
        uniformBase = "uMaterial[" + std::to_string(materialIndex) + "].";
        const string ambientName = uniformBase + "Ka";
        const string diffuseName = uniformBase + "Kd";
        const string specularName = uniformBase + "Ks";
        const string shininessName = uniformBase + "Shininess";
        const string usesTexturesName = uniformBase + "UsesTextures";
        
        //TODO, textur hierrüber einfügen
        
        pCurrentProgram->AddUniform(
                        ambientName, material.GetAmbientColor());
        pCurrentProgram->AddUniform(
                        diffuseName, material.GetDiffuseColor());
        pCurrentProgram->AddUniform(
                        specularName, material.GetSpecularColor());
        pCurrentProgram->AddUniform(
                        usesTexturesName, material.UsesTextures()
                        );
        pCurrentProgram->AddUniform(
                        shininessName, material.GetSpecularExponent());
        
        materialIndex++;
    }
    
    pCurrentProgram->AddUniform(
                        "uNumberOfActiveMaterials",
                        (int)materials.size()
                        );
}

void RenderingStep::AssignCubemapTexture(
                                GLuint textureID, StrongTexturePtr pTexture, int index)
{
    g_CubeTexId = textureID;
    GLint format = (pTexture->GetChannels()==4 ? GL_RGBA : GL_RGB);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    
    /*for(auto& parameterEntry : pTexture->GetParameters())
    {
        glTexParameteri(
                        GL_TEXTURE_CUBE_MAP,
                        parameterEntry.first,
                        parameterEntry.second
                        );
    }*/

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    glTexImage2D(
                 GL_TEXTURE_CUBE_MAP_POSITIVE_X + index,
                 0,
                 format,
                 pTexture->GetWidth(),
                 pTexture->GetHeight(),
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 pTexture->GetImageData()
                 );
}

void RenderingStep::AssignShaderParameter(
                        GLProgram* pCurrentProgram, const string& key, float value)
{
    pCurrentProgram->AddUniform(key, value);
}

void RenderingStep::AssignTextures(
                    StrongActorPtr pActor, RenderComponent* pRenderComponent)
{
    Textures textureList = pRenderComponent->GetTextures();
    size_t texSlot = 0;
    while (texSlot < textureList.size())
    {
        auto pTexture = textureList[texSlot];
        
        glActiveTexture(GL_TEXTURE0 + texSlot);
        GLuint textureID;
        
        if (TextureAlreadyLoaded(pActor, pTexture, textureID))
        {
            glBindTexture(
                          pTexture->GetType() == Texture::CUBEMAP ?
                                GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D,
                          textureID
                          );
            texSlot += pTexture->GetType() == Texture::CUBEMAP ? 6 : 1;
        }
        else
        {
            glGenTextures(1, &textureID);
            
            if (pTexture->GetType() == Texture::CUBEMAP)
            {
                for (int i = 0; i < 6; i++, texSlot++)
                {
                    AssignCubemapTexture(
                        textureID, textureList[texSlot], i);
                }
            }
            else
            {
                AssignTexture(textureID, pTexture);
                texSlot++;
            }

            auto type = pTexture->GetType() == Texture::CUBEMAP ?
                        GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
            glBindTexture(type, textureID);
            glGenerateMipmap(type);
            
            TextureLoadedForActor(pActor, pTexture, textureID);
        }
    }
}

bool RenderingStep::TextureAlreadyLoaded(
                                          StrongActorPtr pActor, StrongTexturePtr pTexture, GLuint& texId)
{
    auto findActor = m_loadedTextures.find(pActor->GetActorId());
    
    if(findActor==m_loadedTextures.end())
    {
        return false;
    }
    
    ActorTextures actorTextures =
    m_loadedTextures.at(pActor->GetActorId());
    
    auto findTexture = actorTextures.find(pTexture->GetName());
    
    if(findTexture==actorTextures.end())
    {
        return false;
    }
    
    texId = findTexture->second;
    
    return true;
}

void RenderingStep::TextureLoadedForActor(
                                           StrongActorPtr pActor, StrongTexturePtr pTexture, GLuint texId)
{
    auto findActor = m_loadedTextures.find(pActor->GetActorId());
    
    if(findActor==m_loadedTextures.end())
    {
        m_loadedTextures.insert(
                                std::pair<ActorId, ActorTextures>(
                                                                  pActor->GetActorId(), ActorTextures()));
    }
    
    ActorTextures& actorTextures = m_loadedTextures.at(pActor->GetActorId());
    
    actorTextures.insert(std::pair<string, GLuint>(pTexture->GetName(), texId));
}

void RenderingStep::AssignGeometry(StrongGeometryPtr pGeometry)
{
    if(pGeometry==nullptr)
    {
        return ;
    }
    
    GLuint arrayH, bufferH;
    pGeometry->GetGLBufferInfo(arrayH, bufferH);
    glBindVertexArray(arrayH);
    glBindBuffer(GL_ARRAY_BUFFER, bufferH);
    pGeometry->Enable();
}

void RenderingStep::AssignMatrices(
                    GLProgram* pCurrentProgram,
                    StrongSceneNodePtr pSceneNode)
{
    mat4 viewMatrix = m_pRenderer->GetCamera() != 0 ?
                m_pRenderer->GetCamera()->GetViewMatrix() : mat4();
    mat4 projectionMatrix = m_pRenderer->GetCamera() != 0 ?
                m_pRenderer->GetCamera()->GetProjectionMatrix(
                                          m_pRenderer->GetCanvasAspectRatio()) : mat4();
    mat4 modelMatrix = pSceneNode->VGetTransformationMatrix();
    
    mat4 modelViewMatrix = viewMatrix * modelMatrix;
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    mat4 mvpMatrix = projectionMatrix * modelViewMatrix;
    
    pCurrentProgram->AddUniform("uModelViewMatrix", modelViewMatrix);
    pCurrentProgram->AddUniform("uProjectionMatrix", projectionMatrix);
    pCurrentProgram->AddUniform("uNormalMatrix", normalMatrix);
    pCurrentProgram->AddUniform("uMVP", mvpMatrix);
    pCurrentProgram->AddUniform("uModelMatrix", modelMatrix);
}

void RenderingStep::AssignLights(GLProgram* pCurrentProgramm)
{
    int lightIndex = 0;
    
    string uniformBase;
    
    for (LightNode* pLightNode : m_pRenderer->GetLights())
    {
        uniformBase = "uLights[" + std::to_string(lightIndex) + "].";
        const string positionName = uniformBase + "Position";
        const string directionName = uniformBase + "Direction";
        const string isDirectionalName = uniformBase + "IsDirectionalLight";
        const string energyName = uniformBase + "Energy";
        const string colorName = uniformBase + "Color";
        
        auto p = pLightNode->VGetPosition();
        pCurrentProgramm->AddUniform(
                            positionName, p);
        pCurrentProgramm->AddUniform(
                            directionName, pLightNode->GetDirection());
        pCurrentProgramm->AddUniform(
                            isDirectionalName, pLightNode->GetType() == DIRECTIONAL_LIGHT);
        pCurrentProgramm->AddUniform(
                            energyName, pLightNode->GetEnergy());
        pCurrentProgramm->AddUniform(
                            colorName, pLightNode->GetColor());
        
        lightIndex++;
    }
    
    pCurrentProgramm->AddUniform(
                        "uNumberOfActiveLights", (float)m_pRenderer->GetLights().size());
    
}

void RenderingStep::AssignShaders(RenderComponent* pRenderComponent)
{    
    m_pRenderer->AddShader(
              IShader::VERTEX_SHADER,
              pRenderComponent->GetShaderName(IShader::VERTEX_SHADER));
    
    m_pRenderer->AddShader(
              IShader::FRAGMENT_SHADER,
              pRenderComponent->GetShaderName(IShader::FRAGMENT_SHADER));
    
    if(pRenderComponent->HasShader(IShader::COMPUTE_SHADER))
    {
        m_pRenderer->AddShader(
                  IShader::COMPUTE_SHADER,
                  pRenderComponent->GetShaderName(IShader::COMPUTE_SHADER));
    }
    
    if(pRenderComponent->HasShader(IShader::TESSELLATION_CONTROL_SHADER))
    {
        m_pRenderer->AddShader(
                  IShader::TESSELLATION_CONTROL_SHADER,
                  pRenderComponent->GetShaderName(IShader::TESSELLATION_CONTROL_SHADER));
    }
    
    if (pRenderComponent->HasShader(IShader::TESSELLATION_EVAL_SHADER))
    {
        m_pRenderer->AddShader(
                  IShader::TESSELLATION_EVAL_SHADER,
                  pRenderComponent->GetShaderName(IShader::TESSELLATION_EVAL_SHADER));
    }
    
    if(pRenderComponent->HasShader(IShader::GEOMETRY_SHADER))
    {
        m_pRenderer->AddShader(
                  IShader::GEOMETRY_SHADER,
                  pRenderComponent->GetShaderName(IShader::GEOMETRY_SHADER));
    }
    
}

void RenderingStep::AssignShaderParameters(
                    GLProgram* pCurrentProgram, RenderComponent* pRenderComponent)
{
    for(auto shaderEntry : pRenderComponent->GetAllShaderParameters())
    {
        SpecificShaderParameters specificParameters = shaderEntry.second;
        
        for(auto parameterEntry : specificParameters)
        {
            AssignShaderParameter(
                                  pCurrentProgram,
                                  parameterEntry.first,
                                  parameterEntry.second
                                  );
        }
    }
}

void RenderingStep::AssignCamera(
                    GLProgram* pCurrentProgram)
{
    const string uniformBase = "uCamera.";
	const string positionName = uniformBase + "Position";
    
	if(m_pRenderer->GetCamera())
	{
		pCurrentProgram->AddUniform(
			positionName,
			m_pRenderer->GetCamera()->VGetPosition()
			);
	}
}

void RenderingStep::AssignGLTex(GLProgram* pCurrentProgram, const std::string& name, GLuint tex, int slot)
{
	pCurrentProgram->AddUniform(name, slot);
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, tex);
}

void computeFrustumPlanes(const mat4& matrix, vec4* plane)
{
	plane[0] = glm::row(matrix, 3) + glm::row(matrix, 0);   // left
	plane[1] = glm::row(matrix, 3) - glm::row(matrix, 0);   // right
	plane[2] = glm::row(matrix, 3) + glm::row(matrix, 1);   // bottom
	plane[3] = glm::row(matrix, 3) - glm::row(matrix, 1);   // top
	plane[4] = glm::row(matrix, 3) + glm::row(matrix, 2);   // far
	plane[5] = glm::row(matrix, 3) - glm::row(matrix, 2);   // near
															// normalize planes
	for (int i = 0; i<6; i++) {
		float l = length(vec3(plane[i]));
		plane[i] = plane[i] / l;
	}
}

void RenderingStep::AssignWaterTessellation(shared_ptr<ISceneNode> pSceneNode, GLProgram* pCurrentProgram, const glm::mat4& viewMat, const glm::mat4& projMat, const vec3& viewPos, const vec3& lookingDir, float triScale)
{
	StrongActorPtr pCurrentActor = pSceneNode->VGetActor();

	RenderComponent* pRenderComponent =
		dynamic_cast<RenderComponent*>(
			pCurrentActor->GetComponent("renderComponent").get());
	OceanComponent* pOceanComponent =
		dynamic_cast<OceanComponent*>(pCurrentActor->GetComponent("oceanComponent").get());
	pOceanComponent->CreateOpenGLObjects([&](GLuint a, Texture& b) {AssignTexture(a, std::make_shared<Game_View::Texture>(b)); });
	auto tInfo = pOceanComponent->getHightTextureInfo();
        mat4 modelMatrix = pSceneNode->VGetTransformationMatrix();

	unsigned int f = pOceanComponent->GetTimer(), p = pOceanComponent->GetTimePeriod(), p1 = (p * 8) / 3, p2 = (p * 5) / 9;
        auto pWind = g_pGameApp->GetCurrentScene()->FindSpecificForce("airstream");
        glm::vec3 globalWindDir = pWind ? pWind->GetDirection() : glm::vec3(0,0,1);
        glm::vec3 localWindDir = glm::normalize(glm::vec3(glm::inverse(modelMatrix) * vec4(globalWindDir, 0)));
        localWindDir *= (pWind ? pWind->GetStrength() : 1.0f) * 0.1f;
        glm::vec2 uvWind = vec2(localWindDir.z, localWindDir.x);

	pCurrentProgram->AddUniform("uPatchesPerDim", (float)pOceanComponent->GetNumberQuads());
	pCurrentProgram->AddUniform("uScreenSize", glm::vec2(m_canvasWidth, m_canvasHeight));
	pCurrentProgram->AddUniform("uTessellatedTriWidth", (float)pOceanComponent->GetTriangleSize() * triScale);
	pCurrentProgram->AddUniform("uHeightMapTexSize", vec2(tInfo.w, tInfo.h));

	const auto& detailMap1 = pOceanComponent->GetDetailMap1();
	pCurrentProgram->AddUniform("uDetailMapUVScale_1", detailMap1.m_detailMapUVScale);
	pCurrentProgram->AddUniform("uDetailMapHeightScale_1", detailMap1.m_detailHeightScale);
        pCurrentProgram->AddUniform("uDetailMapUVOffset_1", (float)(f % p2) / p2 * uvWind);

	const auto& detailMap2 = pOceanComponent->GetDetailMap2();
	pCurrentProgram->AddUniform("uDetailMapUVScale_2", detailMap2.m_detailMapUVScale);
	pCurrentProgram->AddUniform("uDetailMapHeightScale_2", detailMap2.m_detailHeightScale);
        pCurrentProgram->AddUniform("uDetailMapUVOffset_2", (float)(f % p) / p * uvWind);

	pCurrentProgram->AddUniform("uDetailBlendMapUVScale", pOceanComponent->GetDetailBlendMapUVScale());
	pCurrentProgram->AddUniform("uDetailBlendMapUVOffset", vec2(0));

	pCurrentProgram->AddUniform("uModelMatrix", modelMatrix);
	pCurrentProgram->AddUniform("uInvModelMatrix", glm::inverse(modelMatrix));
	pCurrentProgram->AddUniform("uViewMatrix", viewMat);
	pCurrentProgram->AddUniform("uProjMatrix", projMat);
	pCurrentProgram->AddUniform("uProjViewMatrix", projMat * viewMat);
	pCurrentProgram->AddUniform("uMVP", projMat * viewMat * modelMatrix);

	pCurrentProgram->AddUniform("uViewPos", viewPos);
	pCurrentProgram->AddUniform("uViewDir", lookingDir);
	vec4 planes[6];
	computeFrustumPlanes(projMat * viewMat, planes);
	for (int i = 0; i < 6; i++)
	{
		pCurrentProgram->AddUniform("uFrustumPlanes[" + std::to_string(i) + "]", planes[i]);
	}

	AssignGLTex(pCurrentProgram, "heightMap", pOceanComponent->GetOpenGLHightTexture(), 0);

	AssignGLTex(pCurrentProgram, "detailHeightMap_1", detailMap1.m_detailHeightMap, 1);
	AssignGLTex(pCurrentProgram, "detailNormalMap_1", detailMap1.m_detailNormalMap, 2);
	AssignGLTex(pCurrentProgram, "detailHeightMap_2", detailMap2.m_detailHeightMap, 3);
	AssignGLTex(pCurrentProgram, "detailNormalMap_2", detailMap2.m_detailNormalMap, 4);
	AssignGLTex(pCurrentProgram, "detailBlendMap", pOceanComponent->GetDetailBlendMapTex(), 5);

	AssignGLTex(pCurrentProgram, "uDeviationMap", pOceanComponent->GetOpenGLDeviationTexture(), 10);
	AssignGLTex(pCurrentProgram, "uVelocityAccuMap", pOceanComponent->GetOpenGLDeviationTexture(), 11);
	vec3 sizeX = vec3(modelMatrix * vec4(1, 0, 0, 0)), sizeZ = vec3(modelMatrix * vec4(0, 0, 1, 0));
	pCurrentProgram->AddUniform("uOffsetScale", vec2(1.0f) / vec2(length(sizeX), length(sizeZ)));

	static GLuint bufIdx = UINT_MAX, vao;
	if (bufIdx == UINT_MAX)//set an empty vao, it is not used, vertices are generated without buffer
	{
		glGenBuffers(1, &bufIdx);
		glGenVertexArrays(1, &vao);
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, bufIdx);
}

void RenderingStep::VSetCanvasSize(
                    int canvasWidth, int canvasHeight)
{
    m_canvasWidth = canvasWidth;
    m_canvasHeight = canvasHeight;
}

void RenderingStep::VActivateBackbuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_canvasWidth, m_canvasHeight);
}
