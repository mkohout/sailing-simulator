#pragma once
#include "ActorComponent.h"
#include "IPhysicsComponent.h"
#include "../physics/OceanSimulator.h"
#include <glew/glew.h>
#include <functional>
#include <random>

namespace Game_View
{
	class Texture;
}

namespace Game_Logic
{

	struct OceanDetailMap
	{
		float m_detailMapUVScale;
		float m_detailHeightScale;
		std::string m_detailHeightMapName;
		std::string m_detailNormalMapName;

		GLuint m_detailHeightMap;
		GLuint m_detailNormalMap;

		OceanDetailMap()
			: m_detailHeightMap(-1), m_detailNormalMap(-1)
		{
			
		}

		OceanDetailMap(float uvScale, float heightScale, const std::string& heightmap, const std::string& normalMap)
			: m_detailMapUVScale(uvScale), m_detailHeightScale(heightScale), m_detailHeightMapName(heightmap), m_detailNormalMapName(normalMap),
			  m_detailHeightMap(-1), m_detailNormalMap(-1)
		{
			
		}

		void LoadOpenGLTextures(std::function<void(GLuint, Game_View::Texture&)> assignClb);
	};

	class OceanComponent : public ActorComponent, public IPhysicsComponent
	{
		OceanSimulator* m_pSimulation;
		int m_width, m_height;
		float m_speed, m_speedVariance;
		float m_amplitude, m_amplitudeVariance;
		float m_radius, m_radiusVariance;
		float m_wavewidth, m_wavewidthVariance;
		int m_timeElapsed;
		bool m_amplitudePositive;
		int m_maxParticleNumber;
		std::default_random_engine generator;
		std::uniform_real_distribution<float> distribution;

		//rendering settings
		int m_numQuads;
		float m_triSize;
		float m_timePriode;

		OceanDetailMap m_detailMap_1;
		OceanDetailMap m_detailMap_2;

		std::string m_detailBlendMapName;
		float m_detailBlendMapUVScale;

		//rendering opengl objects
		GLuint m_heightMapTex1, m_heightMapTex2;
		GLuint m_velocityMapTex1, m_velocityMapTex2;
		GLuint m_deviationMapTex1, m_deviationMapTex2;
		GLuint m_velocityAccuMapTex1, m_velocityAccuMapTex2;
		GLuint m_detailBlendMapTex;
		unsigned int m_time;

		//rendering state
		bool m_bOpenGlTextureNeedsUpdate;
	public:
		int GetNumberQuads() const
		{
			return m_numQuads;
		}

		float GetTriangleSize() const
		{
			return m_triSize;
		}

		float GetTimePeriod() const
		{
			return m_timePriode;
		}

		float GetDetailBlendMapUVScale() const
		{
			return m_detailBlendMapUVScale;
		}

		GLuint GetDetailBlendMapTex() const
		{
			return m_detailBlendMapTex;
		}

		GLuint GetOpenGLHightTexture() const
		{
			return m_heightMapTex1;
		}

		GLuint GetOpenGLVelocityTexture() const
		{
			return m_velocityMapTex1;
		}

		GLuint GetOpenGLDeviationTexture() const
		{
			return m_deviationMapTex1;
		}

		GLuint GetOpenGLVelocityAccuTexture() const
		{
			return m_velocityAccuMapTex1;
		}

		const OceanDetailMap& GetDetailMap1() const
		{
			return m_detailMap_1;
		}

		const OceanDetailMap& GetDetailMap2() const
		{
			return m_detailMap_2;
		}

		unsigned int& GetTimer()
		{
			return m_time;
		}

		OceanComponent()
			: m_pSimulation(nullptr),
			m_timeElapsed(0),
			m_amplitudePositive(true),
			m_time(0),
			m_bOpenGlTextureNeedsUpdate(true),
			m_heightMapTex1(-1),
			m_maxParticleNumber(1000),
			m_speed(3), m_speedVariance(0),
			m_amplitude(2), m_amplitudeVariance(0),
			m_radius(15), m_radiusVariance(0),
			m_wavewidth(0.25f), m_wavewidthVariance(0),
			distribution(0.0f, 1.0f)
		{
			
		}
		~OceanComponent()
		{
			if (m_pSimulation != nullptr)
				delete m_pSimulation;
		}
		virtual void VInit()
		{
			
		}

		virtual string VGetIdentifier() const
		{
			return "oceanComponent";
		}

		/**
		* Extract data from given XML specification
		*/
		virtual void VFetchData(StrongXMLElementPtr pXmlElement);

		void CreateOpenGLObjects(std::function<void(GLuint, Game_View::Texture&)> assignClb);

		virtual void VUpdate(const unsigned int deltaMilliseconds);

		void FinishFrame();

		template<typename T>
		struct TexInfo
		{
			const T* data;
			int w, h;
		};

		TexInfo<float> getHightTextureInfo() const
		{
			TexInfo<float> r;
			r.data = m_pSimulation->GetCoarseHeightData();
			r.w = m_width; r.h = m_height;
			return r;
		}

		TexInfo<glm::vec3> getVelocityTextureInfo() const
		{
			TexInfo<glm::vec3> r;
			r.data = m_pSimulation->GetVelocityData();
			r.w = m_width; r.h = m_height;
			return r;
		}

		TexInfo<glm::vec2> getDeviationTextureInfo() const
		{
			TexInfo<glm::vec2> r;
			r.data = m_pSimulation->GetDeviationData();
			r.w = m_width; r.h = m_height;
			return r;
		}

		TexInfo<glm::vec2> getDeviationAccuTextureInfo() const
		{
			TexInfo<glm::vec2> r;
			r.data = m_pSimulation->GetVelocityAccuData();
			r.w = m_width; r.h = m_height;
			return r;
		}

		float GetHeightAtPoint(glm::vec3 pos) const;
		glm::vec3 GetVelocityAtPoint(glm::vec3 pos) const;
		void GetFrameAtPoint(const glm::vec3& pos, const glm::mat4 & toWorld, const glm::mat4& toLocal, glm::vec3& tangent, glm::vec3& bitangent, glm::vec3& normal) const;
		void GetFrameAtPoint(const glm::vec3& pos, glm::vec3& tangent, glm::vec3& bitangent, glm::vec3& normal) const;
		glm::vec2 GetGradientAtPoint(const glm::vec2& uv, float* h = 0) const
		{
			return m_pSimulation->computeGradient(uv, h);
		}
		//returns a frame in world space
		glm::vec3 GetFrameAtUV(const glm::vec2& uv, glm::vec3& tangent, glm::vec3& bitangent, glm::vec3& normal) const;
		void CreateWave(glm::vec2 startPosition, glm::vec2 direction, float width, float speed, float amplitude, float radius, float dampening) const;
	};

}
