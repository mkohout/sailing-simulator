#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Game_Logic
{

class OceanSimulator
{
	struct particle
	{
		glm::vec2 position; // between 0.0 and 1.0
		glm::vec2 velocity;
		float radius;
		float amplitude;
		float max_amplitude;
		float max_radius;
		float dampening;
	};

	template<typename T> struct heightmap
	{
		std::vector<T> m_data;
		int w, h;

		heightmap(int w, int h, T defaultValue)
			: w(w), h(h)
		{
			m_data.resize(w * h, defaultValue);
		}

		T& operator()(int x, int y)
		{
			return m_data[y * w + x];
		}

		const T& operator()(int x, int y) const
		{
			return m_data[y * w + x];
		}

		void Reset(T defaultValue)
		{
			std::fill(m_data.begin(), m_data.end(), defaultValue);
		}
	};

	float m_defaultValue;
	int m_width, m_height;
	std::vector<particle> m_particles;
	heightmap<float> m_simpleHeightField1, m_simpleHeightField2;
	heightmap<float>* pSimpleHeightMapBack, *pSimpleHeightMapFront;

	heightmap<float> m_complexHeightField1, m_complexHeightField2;
	heightmap<float>* pComplexHeightMapBack, *pComplexHeightMapFront;
	heightmap<float> m_buffer;

	heightmap<glm::vec2> m_deviationField1, m_deviationField2;
	heightmap<glm::vec2>* pDeviationMapBack, *pDeviationMapFront;

	heightmap<glm::vec2> m_velocityAccuField1, m_velocityAccuField2;
	heightmap<glm::vec2>* pVeclocityAccuMapBack, *pVeclocityAccuMapFront;

	heightmap<glm::vec3> m_velocityField1, m_velocityField2;
	heightmap<glm::vec3>* pVelocityMapBack, *pVelocityMapFront;

	float RectangleFunction(float x) const
	{
		return glm::abs(x) < 0.5 ? 1 : (glm::abs(x) == 0.5) ? 0.5 : 0;
	}

public:

	OceanSimulator(int width, int height, float defaultValue, int particleCount);
	~OceanSimulator();

	const float* GetCoarseHeightData() const
	{
		return &pSimpleHeightMapFront->operator()(0, 0);
	}

	const glm::vec3* GetVelocityData() const
	{
		return &pVelocityMapFront->operator()(0, 0);
	}

	const glm::vec2* GetDeviationData() const
	{
		return &pDeviationMapFront->operator()(0, 0);
	}

	const glm::vec2* GetVelocityAccuData() const
	{
		return &pVeclocityAccuMapFront->operator()(0, 0);
	}

	int GetParticleCount() const
	{
		return m_particles.size();
	}
	void ComputeHeightfield();

	void CreateParticle(glm::vec2 position, glm::vec2 velocity, float max_radius, float max_amplitude, float dampening);
	void UpdateParticle(const unsigned int deltaMilliseconds);
	float GetHeightAtPosition(int x, int y) const;
	glm::vec3 GetVelocityAtPosition(int x, int y) const;

	float ComputeHeight(const glm::vec2& uv) const;

	//computes the gradient of the heightfield in [0,1] lcocal space
	glm::vec2 computeGradient(const glm::vec2& uv, float* h = 0) const;

	glm::vec2 compute_dp_du_dp_dv(const glm::vec2& uv, glm::vec2& dp_du, glm::vec2& dp_dv) const;

	void computeFrame(const glm::vec2& uv, glm::vec3& tangent, glm::vec3& bitangent, glm::vec3& normal, float* h = 0, glm::vec2* deviation = 0) const
	{
		auto gradient = computeGradient(uv, h);
		glm::vec2 dp_du, dp_dv;
		auto dev = compute_dp_du_dp_dv(uv, dp_du, dp_dv);
		if (deviation) *deviation = dev;
		tangent = glm::normalize(glm::vec3(dp_du.x, gradient.x, dp_du.y));
		bitangent = glm::normalize(glm::vec3(dp_dv.x, gradient.y, dp_dv.y));
		normal = glm::normalize(glm::cross(bitangent, tangent));
	}

	glm::vec2 computeDeviation(const glm::vec2& uv);
};

 }