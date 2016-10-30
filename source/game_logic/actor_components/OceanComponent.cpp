#include "OceanComponent.h"
#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <algorithm>
#include "../../application_layer/core/GameCore.h"
#include  <glm/gtx/matrix_decompose.hpp>
//#include "../../application_layer/core/GameCore.h"
//#include "../../game_view/rendering/Texture.h"
#include <mutex>
#include <random>

static char* gData = (char*)malloc(sizeof(vec4) * 4096 * 4096);

void Game_Logic::OceanComponent::VUpdate(const unsigned deltaMilliseconds)
{
	if (m_pSimulation->GetParticleCount() <= m_maxParticleNumber) {
		auto windForce = g_pGameApp->GetCurrentScene()->FindSpecificForce("wind");
		auto windDirection = windForce->GetDirection();
		glm::vec2 direction = glm::vec2(windDirection.x, windDirection.z);
		glm::vec2 normal = glm::vec2(-direction.y, direction.x);
		float widthOfWave = glm::ceil(glm::sqrt(m_width*m_width + m_height*m_height))*m_wavewidth*(1.0f - distribution(generator)*m_wavewidthVariance);


		glm::vec2 startPosition = glm::vec2(distribution(generator)*(m_width + 2 * m_radius) - m_radius, distribution(generator) * (m_height + 2 * m_radius) - m_radius);
		glm::vec2 leftPosition = startPosition + normal * (-widthOfWave / 2);
		glm::vec2 rightPosition = startPosition + normal * (widthOfWave / 2);
		if (m_pSimulation->GetHeightAtPosition(startPosition.x, startPosition.y) == 0.0f) {
			float dampening = 0.0f;
			float speed = m_speed*(1.0f - distribution(generator)*m_speedVariance);
			float radius = m_radius*(1.0f - distribution(generator)*m_radiusVariance);
			float amplitude = m_amplitude*(1.0f - distribution(generator)*m_amplitudeVariance);
			CreateWave(startPosition, direction, widthOfWave, speed , amplitude, radius , dampening);
		}
	}
	m_pSimulation->UpdateParticle(deltaMilliseconds);
	m_pSimulation->ComputeHeightfield();
	m_timeElapsed += deltaMilliseconds;
	//m_bOpenGlTextureNeedsUpdate = true;
}

float Game_Logic::OceanComponent::GetHeightAtPoint(glm::vec3 pos) const
{
	StrongActorPtr pActor(m_pActor);
	glm::vec4 localPos = inverse(g_pGameApp->GetCurrentScene()->GetSceneGraph()->FindNodeWithActor(pActor->GetActorId())->VGetTransformationMatrix())*glm::vec4(pos,1);
	auto first = glm::mix(m_pSimulation->GetHeightAtPosition(glm::floor(localPos.x*m_width), glm::floor(localPos.z*m_height)), m_pSimulation->GetHeightAtPosition(glm::floor(localPos.x*m_width), glm::ceil(localPos.z*m_height)), glm::fract(localPos.z*m_height));
	auto second = glm::mix(m_pSimulation->GetHeightAtPosition(glm::ceil(localPos.x*m_width), glm::floor(localPos.z*m_height)), m_pSimulation->GetHeightAtPosition(glm::ceil(localPos.x*m_width), glm::ceil(localPos.z*m_height)), glm::fract(localPos.z*m_height));
	return glm::mix(first, second, glm::fract(localPos.x*m_width));
}

glm::vec3 OceanComponent::GetVelocityAtPoint(glm::vec3 pos) const
{
	StrongActorPtr pActor(m_pActor);
	glm::vec4 localPos = inverse(g_pGameApp->GetCurrentScene()->GetSceneGraph()->FindNodeWithActor(pActor->GetActorId())->VGetTransformationMatrix())*glm::vec4(pos, 1);
	auto first = glm::mix(m_pSimulation->GetVelocityAtPosition(glm::floor(localPos.x*m_width), glm::floor(localPos.z*m_height)), m_pSimulation->GetVelocityAtPosition(glm::floor(localPos.x*m_width), glm::ceil(localPos.z*m_height)), glm::fract(localPos.z*m_height));
	auto second = glm::mix(m_pSimulation->GetVelocityAtPosition(glm::ceil(localPos.x*m_width), glm::floor(localPos.z*m_height)), m_pSimulation->GetVelocityAtPosition(glm::ceil(localPos.x*m_width), glm::ceil(localPos.z*m_height)), glm::fract(localPos.z*m_height));
	return glm::mix(first, second, glm::fract(localPos.x*m_width));
}

void Game_Logic::OceanComponent::GetFrameAtPoint(const glm::vec3& pos, const glm::mat4 & toWorld, const glm::mat4 & toLocal, glm::vec3 & tangent, glm::vec3 & bitangent, glm::vec3 & normal) const
{
	auto p = toLocal * vec4(pos, 1);
	m_pSimulation->computeFrame(vec2(p.x, p.z), tangent, bitangent, normal);
	//transform local frame to world
	auto q = toWorld * vec4(tangent, 0);
	tangent = normalize(vec3(q.x, q.y, q.z));
	q = toWorld * vec4(bitangent, 0);
	bitangent = normalize(vec3(q.x, q.y, q.z));
	normal = normalize(cross(bitangent, tangent));
}

void Game_Logic::OceanComponent::GetFrameAtPoint(const glm::vec3& pos, glm::vec3& tangent, glm::vec3& bitangent, glm::vec3& normal) const
{
	StrongActorPtr pActor(m_pActor);
	auto toWorld = g_pGameApp->GetCurrentScene()->GetSceneGraph()->FindNodeWithActor(pActor->GetActorId())->VGetTransformationMatrix();
	auto toLocal = glm::inverse(toWorld);
	GetFrameAtPoint(pos, toWorld, toLocal, tangent, bitangent, normal);
}

glm::vec3 Game_Logic::OceanComponent::GetFrameAtUV(const glm::vec2& uv, glm::vec3& tangent, glm::vec3& bitangent, glm::vec3& normal) const
{
	StrongActorPtr pActor(m_pActor);
	auto toWorld = g_pGameApp->GetCurrentScene()->GetSceneGraph()->FindNodeWithActor(pActor->GetActorId())->VGetTransformationMatrix();
	float h; vec2 dev;
	m_pSimulation->computeFrame(uv, tangent, bitangent, normal, &h, &dev);
	//transform local frame to world
	auto q = toWorld * vec4(tangent, 0);
	tangent = normalize(vec3(q.x, q.y, q.z));
	q = toWorld * vec4(bitangent, 0);
	bitangent = normalize(vec3(q.x, q.y, q.z));
	normal = normalize(cross(bitangent, tangent));
	return vec3(toWorld * vec4(uv.x + dev.x, h, uv.y + dev.y, 1));
}

void Game_Logic::OceanComponent::CreateWave(glm::vec2 startPosition, glm::vec2 direction, float width, float speed, float amplitude, float radius, float dampening) const
{
	glm::vec2 normal = glm::vec2(-direction.y, direction.x);
	for (int i = -width/2 ; i < width/2; i += radius / 2)
	{
		m_pSimulation->CreateParticle(
			(startPosition + normal * static_cast<float>(i)) /*glm::vec2((0.70 + (rand() % 31) / 100.), i) + m_timeElapsed/1000.0f *m_velocity*/,
			direction*speed, 
			radius,
			amplitude,
			dampening);// *windForce->GetStrength() / windForce->GetMaxStrength());
	}
	startPosition -= radius * direction;
	for (int i = -width/2 ; i < width / 2; i += radius / 2)
	{
		m_pSimulation->CreateParticle(
			(startPosition + normal * static_cast<float>(i)) /*glm::vec2((0.70 + (rand() % 31) / 100.), i) + m_timeElapsed/1000.0f *m_velocity*/,
			direction*speed, 
			radius,
			-amplitude,
			dampening);// *windForce->GetStrength() / windForce->GetMaxStrength());
	}
}

void Game_Logic::OceanDetailMap::LoadOpenGLTextures(std::function<void(GLuint, Game_View::Texture&)> assignClb)
{
	ImageResource res;
	res.VInitFromFile(Application_Layer::GameCore::GetInstance()->GetFileManager()->GetPathToTextureFile(m_detailHeightMapName));
	Game_View::Texture tex;
	tex.LoadFromResource(std::make_shared<ImageResource>(res));
	tex.SetName(res.GetBasename());
	tex.SetType("");
	tex.Init(); tex.GetParameters()[GL_TEXTURE_WRAP_S] = GL_REPEAT; tex.GetParameters()[GL_TEXTURE_WRAP_T] = GL_REPEAT;
	glGenTextures(1, &m_detailHeightMap);
	assignClb(m_detailHeightMap, tex);

	ImageResource res2;
	res2.VInitFromFile(Application_Layer::GameCore::GetInstance()->GetFileManager()->GetPathToTextureFile(m_detailNormalMapName));
	Game_View::Texture tex2;
	tex2.LoadFromResource(std::make_shared<ImageResource>(res2));
	tex2.SetName(res2.GetBasename());
	tex2.SetType("");
	tex2.Init(); tex2.GetParameters()[GL_TEXTURE_WRAP_S] = GL_REPEAT; tex2.GetParameters()[GL_TEXTURE_WRAP_T] = GL_REPEAT;
	glGenTextures(1, &m_detailNormalMap);
	assignClb(m_detailNormalMap, tex2);
}

void Game_Logic::OceanComponent::VFetchData(StrongXMLElementPtr pXmlElement)
{
	auto pResolution = pXmlElement->FindFirstChildNamed("resolution");
	m_width = 128;
	m_height = 128;
	if (pResolution != nullptr) {
		m_width = std::stoi(pResolution->GetValueOfAttribute("width", "128"));
		m_height = std::stoi(pResolution->GetValueOfAttribute("height", "128"));
	}

	float defVal = std::stof(pXmlElement->GetValueOfAttribute("defaultValue", "0"));

	auto pParticles = pXmlElement->FindFirstChildNamed("particles");
	if (pParticles != nullptr) {
		m_maxParticleNumber = std::stoi(pParticles->GetValueOfAttribute("count", "1000"));
	}

	m_pSimulation = new OceanSimulator(m_width, m_height, defVal, m_maxParticleNumber);

	auto pVelocity = pXmlElement->FindFirstChildNamed("speed");
	if (pVelocity != nullptr) {
		m_speed = std::stof(pVelocity->GetValueOfAttribute("value", "3"));
		m_speedVariance = std::stof(pVelocity->GetValueOfAttribute("variance", "0.0"));
	}
	auto pAmplitude = pXmlElement->FindFirstChildNamed("amplitude");
	if (pAmplitude != nullptr) {
		m_amplitude = std::stof(pAmplitude->GetValueOfAttribute("value", "1"));
		m_amplitudeVariance = std::stof(pAmplitude->GetValueOfAttribute("variance", "0.0"));
	}
	auto pRadius = pXmlElement->FindFirstChildNamed("radius");
	if (pRadius != nullptr) {
		m_radius = std::stof(pRadius->GetValueOfAttribute("value", "10"));
		m_radiusVariance = std::stof(pRadius->GetValueOfAttribute("variance", "0.0"));
	}
	auto pWaveWidth = pXmlElement->FindFirstChildNamed("wavewidth");
	if (pWaveWidth != nullptr) {
		m_wavewidth = std::stof(pWaveWidth->GetValueOfAttribute("value", "0.25"));
		m_wavewidthVariance = std::stof(pWaveWidth->GetValueOfAttribute("variance", "0.0"));
	}

	m_numQuads = std::stoi(pXmlElement->GetValOfFirstChildsAttrib("numQuads", "256"));
	m_triSize = std::stof(pXmlElement->GetValOfFirstChildsAttrib("triangleSize", "30"));
	m_timePriode = std::stof(pXmlElement->GetValOfFirstChildsAttrib("timePeriod", "500"));
	m_detailBlendMapUVScale = std::stof(pXmlElement->GetValOfFirstChildsAttrib("detailBlendMapUVScale", "1"));
	m_detailBlendMapName = pXmlElement->GetValOfFirstChildsAttrib("detailBlendMapName", "");

	OceanDetailMap* maps[] = {&m_detailMap_1 , &m_detailMap_2};
	for (int i = 0; i < 2; i++)
	{
		auto is = std::to_string(i + 1);
		auto uv = std::stoi(pXmlElement->GetValOfFirstChildsAttrib("detailMapUVScale" + is, "1"));
		auto h = std::stof(pXmlElement->GetValOfFirstChildsAttrib("detailHeightScale" + is, "1"));
		auto hName = pXmlElement->GetValOfFirstChildsAttrib("detailHeightMapName" + is, "");
		auto nName = pXmlElement->GetValOfFirstChildsAttrib("detailNormalMapName" + is, "");
		*maps[i] = OceanDetailMap(uv, h, hName, nName);
	}
}

GLuint createTex(int w, int h, int modus, GLint filter = GL_NEAREST)
{
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	if(modus == 1)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0, GL_RED, GL_FLOAT, 0);
	else if (modus == 2)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, w, h, 0, GL_RG, GL_FLOAT, 0);
	else if (modus == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, 0);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	return tex;
}

void Game_Logic::OceanComponent::CreateOpenGLObjects(std::function<void(GLuint, Game_View::Texture&)> assignClb)
{
	//create opengl objects
	if (m_heightMapTex1 == -1)
	{
		//heightmap textures
		m_heightMapTex1 = createTex(m_width, m_height, 1);
		m_heightMapTex2 = createTex(m_width, m_height, 1);

		//deviation textures
		m_deviationMapTex1 = createTex(m_width, m_height, 2);
		m_deviationMapTex2 = createTex(m_width, m_height, 2);
		//deviation accumulation
		m_velocityAccuMapTex1 = createTex(m_width, m_height, 2, GL_LINEAR);
		m_velocityAccuMapTex2 = createTex(m_width, m_height, 2, GL_LINEAR);

		//velocity textures
		m_velocityMapTex1 = createTex(m_width, m_height, 3);
		m_velocityMapTex2 = createTex(m_width, m_height, 3);

		//detail textures
		m_detailMap_1.LoadOpenGLTextures(assignClb);
		m_detailMap_2.LoadOpenGLTextures(assignClb);

		ImageResource res;
		res.VInitFromFile(Application_Layer::GameCore::GetInstance()->GetFileManager()->GetPathToTextureFile(m_detailBlendMapName));
		Game_View::Texture tex;
		tex.LoadFromResource(std::make_shared<ImageResource>(res));
		tex.SetName(res.GetBasename());
		tex.SetType("");
		tex.Init(); tex.GetParameters()[GL_TEXTURE_WRAP_S] = GL_REPEAT; tex.GetParameters()[GL_TEXTURE_WRAP_T] = GL_REPEAT;
		glGenTextures(1, &m_detailBlendMapTex);
		assignClb(m_detailBlendMapTex, tex);
	}
	if(m_bOpenGlTextureNeedsUpdate)
	{
		glActiveTexture(GL_TEXTURE0);
		m_bOpenGlTextureNeedsUpdate = false;
		auto tInfo = getHightTextureInfo();
		//copy data to temp buffer
		size_t length = m_width * m_height * sizeof(float);
		memcpy(gData, tInfo.data, length);
		glBindTexture(GL_TEXTURE_2D, m_heightMapTex2);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RED, GL_FLOAT, gData);
		//glBindTexture(GL_TEXTURE_2D, m_heightMapTex1);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, tInfo.w, tInfo.h, 0, GL_RED, GL_FLOAT, tInfo.data);

		auto tInfo2 = getVelocityTextureInfo();
		size_t length2 = m_width * m_height * sizeof(vec3);
		glBindTexture(GL_TEXTURE_2D, m_velocityMapTex2);
		memcpy(gData + length, tInfo2.data, length2);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, gData + length);

		auto tInfo3 = getDeviationTextureInfo();
		size_t length3 = m_width * m_height * sizeof(vec2);
		glBindTexture(GL_TEXTURE_2D, m_deviationMapTex2);
		memcpy(gData + length + length2, tInfo3.data, length3);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RG, GL_FLOAT, gData + length + length2);

		auto tInfo4 = getDeviationAccuTextureInfo();
		size_t length4 = m_width * m_height * sizeof(vec2);
		glBindTexture(GL_TEXTURE_2D, m_velocityAccuMapTex1);
		memcpy(gData + length + length2 + length3, tInfo4.data, length4);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RG, GL_FLOAT, gData + length + length2 + length3);
	}
}

void Game_Logic::OceanComponent::FinishFrame()
{
	m_bOpenGlTextureNeedsUpdate = true;
	std::swap(m_heightMapTex1, m_heightMapTex2);
	std::swap(m_velocityMapTex1, m_velocityMapTex2);
	std::swap(m_deviationMapTex1, m_deviationMapTex2);
	std::swap(m_velocityAccuMapTex1, m_velocityAccuMapTex2);
}
