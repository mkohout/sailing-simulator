//#define BT_USE_SSE_IN_API
#include "BulletDebugDrawStep.h"
#include "../video_system/OpenGLRenderer.h"
#include "../scene_graph/SkyNode.h"
#include <iostream>
#include "../game_logic/physics/BulletPhysics.h"
#include "../../application_layer/core/GameCore.h"
#include "../../game_logic/actor_components/ParticleSystemComponent.h"

#define LOC_POS 0
#define LOC_COL 1

namespace Game_View
{

class BulletDebugDrawStep::btDebugDrawHelper : public btIDebugDraw
{
	BulletDebugDrawStep* m_step;
	int mode;
public:
	btDebugDrawHelper(BulletDebugDrawStep* s)
		: m_step(s), mode(DebugDrawModes::DBG_DrawAabb | DebugDrawModes::DBG_DrawConstraints | DebugDrawModes::DBG_DrawConstraintLimits | DebugDrawModes::DBG_DrawContactPoints | DebugDrawModes::DBG_DrawFrames | DebugDrawModes::DBG_DrawWireframe)
	{
		
	}

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		if(m_step->m_vOffset + 2 >= m_step->m_numVertices)
		{
			std::cout << "too many bullet debug draw vertices!";
			return;
		}
#define CNV(a) vec3(a.x(), a.y(), a.z())
		m_step->m_pVertexData[m_step->m_vOffset++] = VertexData(CNV(from), CNV(color));
		m_step->m_pVertexData[m_step->m_vOffset++] = VertexData(CNV(to), CNV(color));
#undef CNV
	}

	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
		
	}

	virtual void draw3dText(const btVector3& location, const char* textString)
	{
		std::cout << std::string(textString) << "\n";
	}

	virtual void reportErrorWarning(const char* warningString)
	{
		std::cout << std::string(warningString) << "\n";
	}

	virtual void setDebugMode(int debugMode)
	{
		mode = debugMode;
	}

	virtual int getDebugMode() const
	{
		return mode;
	}
};

BulletDebugDrawStep::BulletDebugDrawStep()
	: m_numVertices(1024 * 16)
{
	m_pVertexData = new VertexData[m_numVertices];

	glGenVertexArrays(1, &m_vertexArrayHandle);
	glGenBuffers(1, &m_vertexBufferHandle);

	glBindVertexArray(m_vertexArrayHandle);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, m_numVertices * sizeof(VertexData), m_pVertexData, GL_DYNAMIC_DRAW);

	// Set up vertex attributes
	// Position
	glVertexAttribPointer(
		LOC_POS,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(VertexData),
		nullptr
		);

	// Normal
	glVertexAttribPointer(
		LOC_COL,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(VertexData),
		(void*)offsetof(VertexData, col)
		);
}

#define CNV(a) btVector3((a).x, (a).y, (a).z)

void BulletDebugDrawStep::RenderNode(StrongSceneNodePtr pSceneNode, unsigned int)
{
	auto toWorld = pSceneNode->VGetTransformationMatrix();
	auto actor = pSceneNode->VGetActor();
	if(actor)
	{
		auto pSystem = (ParticleSystemComponent*)actor->GetComponent("particleSystemComponent").get();
		if(pSystem)
		{
			m_pDrawHelper->drawSphere(CNV(toWorld * vec4(pSystem->GetOffset(), 1)), btScalar(1), btVector3(1, 0, 0));
		}
	}
}

void BulletDebugDrawStep::VExecute(SceneGraph* pSceneGraph, unsigned deltaMilliseconds)
{
	m_vOffset = 0;

	auto* world = BulletPhysics::GetInstance()->GetDynamicsWorld();
	auto dbgDraw = btDebugDrawHelper(this);
	m_pDrawHelper = &dbgDraw;
	world->setDebugDrawer(&dbgDraw);
	world->debugDrawWorld();
	world->setDebugDrawer(0);

	RenderFunction renderFunction =
		fastdelegate::MakeDelegate(
			this,
			&BulletDebugDrawStep::RenderNode
			);
	ExecuteOnAllSceneNodes(pSceneGraph, deltaMilliseconds, renderFunction);

	//auto pPhysicsComponent = std::dynamic_pointer_cast<PhysicsComponent> (g_pGameApp->GetCurrentScene()->GetActorWithIdentifier("sailingBoat")->GetComponent("physicsComponent"));
	//dbgDraw.drawSphere(pPhysicsComponent->rigid_body()->getCenterOfMassPosition(),btScalar(1),btVector3(1,0,0));
	/*auto a = g_pGameApp->GetCurrentScene()->GetActorWithIdentifier("water");
	auto oc = std::dynamic_pointer_cast<OceanComponent>(a->GetComponent("oceanComponent"));
	glm::vec3 tangent, bitangent, normal;
	float h;
	auto uv = glm::vec2(0.05f);
	oc->GetGradientAtPoint(uv, &h);
	auto toWorld = g_pGameApp->GetCurrentScene()->GetSceneGraph()->FindNodeWithActor(a->GetActorId())->VGetTransformationMatrix();
	auto posH = toWorld * vec4(uv.x, h, uv.y, 1);
	oc->GetFrameAtPoint(vec3(posH), tangent, bitangent, normal);
	auto at = btMatrix3x3(CNV(tangent), CNV(bitangent), CNV(normal));
	at = at.transpose();
	btTransform transform = btTransform(at, CNV(vec3(posH) + normal * 0.1f));
	dbgDraw.drawTransform(transform, 10);*/

	if (!m_vOffset)
		return;
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, m_numVertices * sizeof(VertexData), m_pVertexData, GL_DYNAMIC_DRAW);
	glBindVertexArray(m_vertexArrayHandle);
	glEnableVertexAttribArray(LOC_POS);
	glEnableVertexAttribArray(LOC_COL);

	std::map<IShader::ShaderType, string> names;
	names[IShader::ShaderType::VERTEX_SHADER] = "line";
	names[IShader::ShaderType::FRAGMENT_SHADER] = "line";
	GLProgram* pCurrentProgram = m_pRenderer->GetProgram(names);
	pCurrentProgram->Use();
	mat4 viewMatrix = m_pRenderer->GetCamera() != 0 ?
		m_pRenderer->GetCamera()->GetViewMatrix() : mat4();
	mat4 projectionMatrix = m_pRenderer->GetCamera() != 0 ?
		m_pRenderer->GetCamera()->GetProjectionMatrix(
			m_pRenderer->GetCanvasAspectRatio()) : mat4();
	pCurrentProgram->AddUniform("uMVP", projectionMatrix * viewMatrix);
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_LINES, 0, m_vOffset);
	glEnable(GL_DEPTH_TEST);

	glDisableVertexAttribArray(LOC_POS);
	glDisableVertexAttribArray(LOC_COL);
}

}
