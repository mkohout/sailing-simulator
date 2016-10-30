#include "ParticleSystemComponent.h"
#include "../game_view/video_system/OpenGLRenderer.h"
#include <random>
#include "../../application_layer/core/GameCore.h"
#include <iostream>

#define LOC_POS 0
#define LOC_RAD 1
#define LOC_DIR 2
#define LOC_TME 3
#define LOC_ENA 4

using namespace Game_Logic;

namespace __internal__
{
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_real_distribution<> dist(0, 1);

	vec2 randomVec2()
	{
		return vec2(dist(e2), dist(e2));
	}

	vec3 squareToUniformCone(float cosCutoff, const vec2 &sample)
	{
		float cosTheta = (1 - sample.x) + sample.x * cosCutoff;
		float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

		float sinPhi = std::sin(2.0f * glm::pi<float>() * sample.y), cosPhi = std::cos(2.0f * glm::pi<float>() * sample.y);

		return vec3(cosPhi * sinTheta, sinPhi * sinTheta, cosTheta);
	}
	vec2 squareToUniformDiskConcentric(const vec2 &sample)
	{
		float r1 = 2.0f*sample.x - 1.0f;
		float r2 = 2.0f*sample.y - 1.0f;

		/* Modified concencric map code with less branching (by Dave Cline), see
		http://psgraphics.blogspot.ch/2011/01/improved-code-for-concentric-map.html */
		float phi, r;
		if (r1 == 0 && r2 == 0) {
			r = phi = 0;
		} if (r1*r1 > r2*r2) {
			r = r1;
			phi = (glm::pi<float>() / 4.0f) * (r2 / r1);
		}
		else {
			r = r2;
			phi = (glm::pi<float>() / 2.0f) - (r1 / r2) * (glm::pi<float>() / 4.0f);
		}

		return vec2(r * glm::cos(phi), r * glm::sin(phi));
	}
	vec3 squareToCosineHemisphere(const vec2 &sample)
	{
		vec2 p = squareToUniformDiskConcentric(sample);
		float z = std::sqrt(1.0f - p.x*p.x - p.y*p.y);
		return vec3(p.x, p.y, z);
	}

	void coordinateSystem(const vec3& a, vec3& s, vec3& t)
	{
		if (std::abs(a.x) > std::abs(a.y))
		{
			float invLen = 1.0f / std::sqrt(a.x * a.x + a.z * a.z);
			t = vec3(a.z * invLen, 0.0f, -a.x * invLen);
		}
		else
		{
			float invLen = 1.0f / std::sqrt(a.y * a.y + a.z * a.z);
			t = vec3(0.0f, a.z * invLen, -a.y * invLen);
		}
		s = normalize(cross(t, a));
	}

}

ParticleSystemComponent::ParticleSystemComponent()
	: m_numParticles(0), looped(false), m_vertexArrayHandle(-1), m_gpuBufferNeedsUpdate(false), m_particleMap(-1)
{
	
}

void ParticleSystemComponent::VInit()
{
}

void ParticleSystemComponent::VFetchData(StrongXMLElementPtr pXmlElement)
{
	int N = std::stoi(pXmlElement->GetValOfFirstChildsAttrib("numberParticles", "1000"));
	m_particles.resize(N);
	auto offXml = pXmlElement->FindFirstChildNamed("offset");
	if (offXml)
	{
		auto posXml = offXml->FindFirstChildNamed("position");
		m_offset = vec3(std::stof(posXml->GetValueOfAttribute("x", "0")),
						std::stof(posXml->GetValueOfAttribute("y", "0")),
						std::stof(posXml->GetValueOfAttribute("z", "0")));
	}
	m_particleTextureName = pXmlElement->GetValOfFirstChildsAttrib("texture", "");
	m_decayTime = std::stof(pXmlElement->GetValOfFirstChildsAttrib("decayTime", "3"));
}

void ParticleSystemComponent::CreateSpray(int N, const glm::vec3& _ori,const glm::vec3& position, float angleCutoff, bool inObjSpace)
{
	looped |= m_numParticles + N >= m_particles.size();
	float mag = length(_ori);
	vec3 tangent, bitangent, ori = normalize(_ori);
	auto mat = g_pGameApp->GetCurrentScene()->GetSceneGraph()->FindNodeWithActor(GetActor()->GetActorId())->VGetTransformationMatrix();
	/*if (!inObjSpace)
		ori = normalize(vec3(mat * vec4(ori, 0)));*/
	__internal__::coordinateSystem(ori, tangent, bitangent);
	angleCutoff = glm::cos(angleCutoff);
	for (int i = 0; i < N; i++)
	{
		vec2 xz = __internal__::squareToUniformDiskConcentric(__internal__::randomVec2()) * 2.0f * 0.7f;
		vec3 p = position + tangent * xz.x + bitangent * xz.y;
		//vec3 dir_local = squareToCosineHemisphere(__internal__::randomVec2());
		vec3 dir_local = __internal__::squareToUniformCone(angleCutoff, __internal__::randomVec2());
		vec3 dir = dir_local.x * tangent + dir_local.y * bitangent + dir_local.z * ori;
		m_particles[m_numParticles] = particle(p, normalize(dir) * mag, 1, inObjSpace);
		m_numParticles = (m_numParticles + 1) % m_particles.size();
	}
}

void ParticleSystemComponent::LoadOpenGLData(std::function<void(GLuint, Game_View::Texture&)> assignClb)
{
	if(m_particleMap == -1)
	{
		ImageResource res;
		res.VInitFromFile(GameCore::GetInstance()->GetFileManager()->GetPathToTextureFile(m_particleTextureName));
		Texture tex;
		tex.LoadFromResource(std::make_shared<ImageResource>(res));
		tex.SetName(res.GetBasename());
		tex.SetType("");
		tex.Init();
		glGenTextures(1, &m_particleMap);
		assignClb(m_particleMap, tex);

		glGenVertexArrays(1, &m_vertexArrayHandle);
		glGenBuffers(1, &m_vertexBufferHandle);

		glBindVertexArray(m_vertexArrayHandle);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferHandle);

		// Set up vertex attributes
		glVertexAttribPointer(
			LOC_POS,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(particle),
			nullptr
			);

		glVertexAttribPointer(
			LOC_RAD,
			1,
			GL_FLOAT,
			GL_FALSE,
			sizeof(particle),
			(void*)offsetof(particle, rad)
			);

		glVertexAttribPointer(
			LOC_DIR,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(particle),
			(void*)offsetof(particle, dir)
			);

		glVertexAttribPointer(
			LOC_TME,
			1,
			GL_FLOAT,
			GL_FALSE,
			sizeof(particle),
			(void*)offsetof(particle, time)
			);

		glVertexAttribIPointer(
			LOC_ENA,
			1,
			GL_INT,
			sizeof(particle),
			(void*)offsetof(particle, flags)
			);
	}
}

void ParticleSystemComponent::Draw()
{
	/*if(m_numParticles == 0)
		CreateSpray(100, vec3(-0.1f, 2, -0.9f) * 3.0f, glm::radians(20.0f), false);*/

	int N = looped ? m_particles.size() : m_numParticles;
	if (!N)
		return;

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferHandle);
	if (m_gpuBufferNeedsUpdate)
	{
		glBufferData(GL_ARRAY_BUFFER, N * sizeof(particle), &m_particles[0], GL_DYNAMIC_DRAW);
		m_gpuBufferNeedsUpdate = false;
	}
	glBindVertexArray(m_vertexArrayHandle);
	glEnableVertexAttribArray(LOC_POS);
	glEnableVertexAttribArray(LOC_RAD);
	glEnableVertexAttribArray(LOC_DIR);
	glEnableVertexAttribArray(LOC_TME);
	glEnableVertexAttribArray(LOC_ENA);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_POINTS, 0, N);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glDisable(GL_PROGRAM_POINT_SIZE);
	glDisableVertexAttribArray(LOC_POS);
	glDisableVertexAttribArray(LOC_RAD);
	glDisableVertexAttribArray(LOC_DIR);
	glDisableVertexAttribArray(LOC_TME);
	glDisableVertexAttribArray(LOC_ENA);
}

void ParticleSystemComponent::VUpdate(const unsigned int delta)
{
	float dt = delta / 1000.0f;
	for(auto& p : m_particles)
		if(p.flags & 1)
		{
			p.time += dt;
			p.pos += p.dir * dt;
			for (auto pForce : g_pGameApp->GetCurrentScene()->GetForces())
				p.dir += pForce->GetDirection() * pForce->GetStrength() * dt;
		}
	m_gpuBufferNeedsUpdate = true;
}