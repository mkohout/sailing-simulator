#include "OceanSimulator.h"
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext.hpp>
#include <algorithm>
#include <boost/filesystem/convenience.hpp>
#include "../../application_layer/core/stdafx.h"
#include "../actor_components/ActorComponent.h"
#include "../../application_layer/core/GameCore.h"
#include <iostream>

#define PI (3.141592653589793f)
using namespace glm;

namespace Game_Logic
{

    inline float squareRoot(float x)
    {
        unsigned int i = *(unsigned int*)&x;

        // adjust bias
        i += 127 << 23;
        // approximation of square root
        i >>= 1;

        return *(float*)&i;
    }

	inline float Q_rsqrt(float number)
    {
        return 1.0f / squareRoot(number);
		long i;
		float x2, y;
		const float threehalfs = 1.5F;

		x2 = number * 0.5F;
		y = number;
		i = *(long *)&y;                       // evil floating point bit level hacking
		i = 0x5f3759df - (i >> 1);               // what the fuck? 
		y = *(float *)&i;
		y = y * (threehalfs - (x2 * y * y));   // 1st iteration
											   //	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

		return y;
    }

	template<typename T>
	inline T MyCos(T x) noexcept
    {
		constexpr T tp = 1. / (2.*3.141592653589793);
		x *= tp;
		x -= T(.25) + std::floor(x + T(.25));
		x *= T(16.) * (std::abs(x) - T(.5));
        x += T(.225) * x * (std::abs(x) - T(1.));
		return x;
	}

	template<typename T>
	inline T MySin(T x) noexcept
	{
		return -MyCos(x + T(3.141592653589793 / 2));
	}

	inline float MyInverseLength(glm::vec2& vec)
	{
		return Q_rsqrt(vec.x*vec.x + vec.y*vec.y);
	}
	inline float MyLength(glm::vec2 vec)
	{
		return squareRoot(vec.x*vec.x + vec.y*vec.y);
	}


	OceanSimulator::OceanSimulator(int width, int height, float defaultValue, int particleCount) :
		m_defaultValue(defaultValue),
		m_width(width),
		m_height(height),
		m_simpleHeightField1(width, height, defaultValue),
		m_simpleHeightField2(width, height, defaultValue),
		pSimpleHeightMapBack(&m_simpleHeightField2),
		pSimpleHeightMapFront(&m_simpleHeightField1),
		m_complexHeightField1(width, height, defaultValue),
		m_complexHeightField2(width, height, defaultValue),
		pComplexHeightMapBack(&m_complexHeightField2),
		pComplexHeightMapFront(&m_complexHeightField1),
		m_buffer(width, height, defaultValue),
		m_deviationField1(width, height, glm::vec2(0)),
		m_deviationField2(width, height, glm::vec2(0)),
		pDeviationMapBack(&m_deviationField2),
		pDeviationMapFront(&m_deviationField1),
		m_velocityAccuField1(width, height, glm::vec2(0)),
		m_velocityAccuField2(width, height, glm::vec2(0)),
		pVeclocityAccuMapBack(&m_velocityAccuField2),
		pVeclocityAccuMapFront(&m_velocityAccuField1),
		m_velocityField1(width, height, glm::vec3(0)),
		m_velocityField2(width, height, glm::vec3(0)),
		pVelocityMapBack(&m_velocityField2),
		pVelocityMapFront(&m_velocityField1)
	{
		m_particles.reserve(particleCount);
	}


	OceanSimulator::~OceanSimulator()
	{
	}


	void OceanSimulator::ComputeHeightfield()
	{
		pSimpleHeightMapBack->Reset(m_defaultValue);
		pDeviationMapBack->Reset(glm::vec2(0));
		pVelocityMapBack->Reset(glm::vec3(0));
		pComplexHeightMapBack->Reset(m_defaultValue);
		m_buffer.Reset(m_defaultValue);

		//#pragma omp parallel for
		//for (auto& p : m_particles)
		for (int i = 0; i < m_particles.size(); i++)
		{
			particle p = m_particles[i];
			glm::vec2 vel_dir = glm::normalize(p.velocity);
			//if (p.enabled) {
				int y_max = std::min(static_cast<int>(p.position.y + p.radius), m_height-1);
				for (int _y = std::max(static_cast<int>(p.position.y - p.radius), 0); _y <= y_max; _y++)
				{
					int x_max = std::min(static_cast<int>(p.position.x + p.radius), m_width-1);
					for (int _x = std::max(static_cast<int>(p.position.x - p.radius), 0); _x <= x_max; _x++)
					{
						glm::vec2 dir = glm::vec2(_x, _y) - p.position;
						float d = MyLength(dir);
						if(p.radius >= d)
						{	
							float sin_d = MySin(PI * d / p.radius);
							float cos_d = MyCos(PI * d / p.radius);
							float u = glm::dot(vel_dir, dir);
							float sin_u = MySin(PI * u / p.radius);
							//float cos_u = MyCos(PI * u / p.radius);

							float dz = 0.5 * (cos_d + 1);
							float deviation_y = p.amplitude * dz;
							(*pSimpleHeightMapBack)(_x, _y) += deviation_y;

							//eigentlich richtig
							glm::vec2 L = -sin_u*vel_dir/*sin_d*glm::normalize(dir)*/;
							glm::vec2 deviation_xz = L*dz*glm::sign(p.amplitude);
							(*pDeviationMapBack)(_x, _y) += deviation_xz;

							//2D Filter Approximation
							/*float sin_d = MySin(PI*d / p.radius);
							float rect = RectangleFunction(d / (2 * p.radius));
							glm::vec2 deviation_xz = -fa*0.5f*sin_d*(cos_d + 1)*rect*n_dir;
							(*pDeviationMapBack)(_x, _y) += deviation_xz;*/

							//1D Filter Approximation
							/*float sin_x = MySin(PI*dir.x / p.radius);
							float cos_x = MyCos(PI*dir.x / p.radius);
							float sin_y = MySin(PI*dir.y / p.radius);
							float cos_y = MyCos(PI*dir.y / p.radius);
							float rect_x = RectangleFunction(dir.x / (2 * p.radius));
							float rect_y = RectangleFunction(dir.y / (2 * p.radius));
							glm::vec2 deviation_xz = glm::vec2(-0.5f*sin_x*(cos_x+1)*rect_x*0.25f*glm::pow2(cos_y+1)*rect_y, -0.5f*sin_y*(cos_y + 1)*rect_y*0.25f*glm::pow2(cos_x + 1)*rect_x);
							(*pDeviationMapBack)(_x, _y) += deviation_xz;*/

							

							/*float co_x = MyCos(PI*(dir.x) / p.radius);
							float v_x = -(-0.5f*(MyCos(2 * PI*(dir.x) / p.radius) + co_x)*(PI*glm::length(p.velocity) / p.radius)) * faktor1;
							float v_z =  faktor2 * (0.25f*glm::pow2(co_x + 1));*/
							//glm::vec2 v_xz = 0.5f * glm::sin(PI * d / p.radius) * (-glm::dot(glm::normalize(glm::vec2(_x, _y) - p.position), glm::normalize(p.velocity))) * glm::sign(p.amplitude) * p.velocity;
							//0*0.5f * cos_d*	   (glm::dot(n_dir, p.velocity*MyInverseLength(p.velocity))) * glm::sign(p.amplitude) * glm::length(p.velocity);
							//(*pVelocityMapBack)(_x, _y) += glm::vec3(v_xz.x, v_y, v_xz.y);

							
							glm::vec2 v_xz = p.velocity*p.amplitude*0.5f*PI / p.radius * (-/*cos_u*/cos_d*(cos_d + 1.0f) - /*sin_u*/sin_d*sin_d);
							float v_y = MyLength(p.velocity)*p.amplitude*0.5f*/*sin_u*/sin_d*PI/p.radius;
							(*pVelocityMapBack)(_x, _y) += glm::vec3(v_xz.x, v_y, v_xz.y);
						}
						
					}
				}
			//}
        }
		
		for (int y = 0; y < m_height; y++)
		{
			for (int x = 0; x < m_width; x++)
			{
				glm::vec3 vel = (*pVelocityMapBack)(x, y);
				(*pVeclocityAccuMapBack)(x, y) += vec2(vel.x, vel.z);

				glm::vec2 dev = (*pDeviationMapBack)(x, y);
				float height = (*pSimpleHeightMapBack)(x, y);
				
				int xo = glm::floor(x + dev.x);
				int yo = glm::floor(y + dev.y);

				float fract_x = glm::abs(glm::fract(x+dev.x));
				float fract_y = glm::abs(glm::fract(y+dev.y));

				if(xo >= 0 && yo >= 0 && xo < m_width && yo < m_height)
					(m_buffer)(xo  , yo  ) += ((1.0f - fract_x) * (1.0f - fract_y)) * height;

				if(xo + 1 >= 0 && yo >= 0 && xo + 1 < m_width && yo < m_height)
					(m_buffer)(xo+1, yo  ) += ((fract_x)     * (1.0f - fract_y)) * height;

				if (xo >= 0 && yo + 1 >= 0 && xo < m_width && yo + 1 < m_height)
					(m_buffer)(xo  , yo+1) += ((1.0f - fract_x) * (fract_y))     * height;

				if (xo + 1 >= 0 && yo + 1 >= 0 && xo + 1 < m_width && yo + 1 < m_height)
					(m_buffer)(xo+1, yo+1) += ((fract_x)     * (fract_y))     * height;

			}
		}
		for (int y = 0; y < m_height; y++)
		{
			for (int x = 0; x < m_width; x++)
			{
				for (int iy = -1; iy < 2; iy++)
					for (int ix = -1; ix < 2; ix++)
						if((x+ix) >=0 && (x+ix) < m_width && (y + iy) >= 0 && (y + iy) < m_height)
						{
							float fac1 = (iy != 0) ? 0.25f : 0.5f;
							float fac2 = (ix != 0) ? 0.25f : 0.5f;
							float value = m_buffer(x + ix, y + iy);
							(*pComplexHeightMapBack)(x, y) += value*fac1*fac2* (value < 0.0f? 1.3f:0.6f);
						}
							
			}
		}

		std::swap(pSimpleHeightMapFront, pSimpleHeightMapBack);
		std::swap(pComplexHeightMapFront, pComplexHeightMapBack);
		std::swap(pDeviationMapFront, pDeviationMapBack);
		std::swap(pVelocityMapFront, pVelocityMapBack);
	}

	void OceanSimulator::CreateParticle(glm::vec2 position, glm::vec2 velocity, float max_radius, float max_amplitude, float dampening)
	{
		particle p = {
				/*.position		=*/ position,
				/*.velocity		=*/ velocity,
				/*.radius		=*/ 0,
				/*.amplitude	=*/ 0,
				/*.max_amplitude=*/ max_amplitude, 
				/*.max_radius	=*/ max_radius,
				/*.dampening	=*/ dampening};
		m_particles.push_back(p);
	}

	void OceanSimulator::UpdateParticle(const unsigned int deltaMilliseconds)
	{
		auto windForce = g_pGameApp->GetCurrentScene()->FindSpecificForce("wind");
		//glm::vec3 windDirection = windForce->GetDirection();
		float factor = windForce->GetStrength() / windForce->GetMaxStrength();
		for (auto &p : m_particles)
		{
			p.position += static_cast<float>(deltaMilliseconds) * p.velocity / 1000.0f;
			//float factor2 = glm::dot(glm::vec2(windDirection.x, windDirection.z), glm::normalize(p.velocity));
			p.max_amplitude *= (1.0f - p.dampening);
			p.amplitude += (p.max_amplitude*factor/**factor2*/ - p.amplitude)*0.01;
			p.radius = p.max_radius;
		}
		auto w = m_width;
		auto h = m_height;
		m_particles.erase(
			std::remove_if(m_particles.begin(), m_particles.end(), 
			[w,h](const particle& p)
		{
			return ((glm::dot(p.velocity, glm::vec2(w / 2.0f, h / 2.0f) - p.position) < 0) && (p.position.x - p.radius > w || p.position.x + p.radius < 0 || p.position.y - p.radius > h || p.position.y + p.radius < 0)) || glm::abs(p.max_amplitude) <= 0.001f;
		}),
			m_particles.end());
		
	}

	glm::vec3 OceanSimulator::GetVelocityAtPosition(int x, int y) const
	{
		if (x >= 0 && x < m_width && y >= 0 && y < m_height)
			return (*pVelocityMapFront)(x, y);
		else
			return glm::vec3(0.0f);
	}

	float OceanSimulator::GetHeightAtPosition(int x, int y) const
	{
		return (*pComplexHeightMapFront)(clamp(x, 0, m_width - 1), clamp(y, 0, m_height - 1));
	}

	float OceanSimulator::ComputeHeight(const glm::vec2& uv) const
	{
		float h;
		computeGradient(uv, &h);
		return h;
	}

	glm::vec2 OceanSimulator::compute_dp_du_dp_dv(const glm::vec2& uv, glm::vec2& dp_du, glm::vec2& dp_dv) const
	{
		vec2 texcoord = uv * vec2(m_width, m_height);
		float fx = fract(texcoord.x);
		float fy = fract(texcoord.y);
		texcoord.x -= fx;
		texcoord.y -= fy;

#define L(ox, oy) (*pDeviationMapFront)((int)clamp(texcoord.x + ox, 0.0f, (float)m_width - 1), (int)clamp(texcoord.y + oy, 0.0f, (float)m_height - 1))
		vec2 a = L(0, 0), b = L(1, 0);
		vec2 c = L(0, 1), d = L(1, 1);
		dp_du = (-a + b + (a - b - c + d) * fy) + vec2(1, 0);
		dp_dv = (-a + c - (-a + b) * fx + (-c + d) * fx) + vec2(0, 1);
#undef L
		return mix(mix(a, b, fx), mix(c, d, fx), fy) / glm::vec2(m_width, m_height);
	}

	glm::vec2 OceanSimulator::computeDeviation(const glm::vec2& uv)
	{
		vec2 texcoord = uv * vec2(m_width, m_height);
		float fx = fract(texcoord.x);
		float fy = fract(texcoord.y);
		texcoord.x -= fx;
		texcoord.y -= fy;

#define L(ox, oy) (*pDeviationMapBack)((int)clamp(texcoord.x + ox, 0.0f, (float)m_width - 1), (int)clamp(texcoord.y + oy, 0.0f, (float)m_height - 1))
		vec2 a = L(0, 0), b = L(1, 0);
		vec2 c = L(0, 1), d = L(1, 1);
		return mix(mix(a, b, fx), mix(c, d, fx), fy) / glm::vec2(m_width, m_height);
#undef L
	}


	glm::vec2 OceanSimulator::computeGradient(const glm::vec2& uv, float* hr) const
	{
		vec2 texcoord = uv * vec2(m_width, m_height);
		float fx = fract(texcoord.x);
		float fy = fract(texcoord.y);
		texcoord.x -= fx;
		texcoord.y -= fy;
		
		//a -- b
		//|	 |
		//c -- d
		
		//float a = GetHeightAtPosition(texcoord.x, texcoord.y);
		//float b = GetHeightAtPosition(texcoord.x + 1, texcoord.y);
		//float c = GetHeightAtPosition(texcoord.x, texcoord.y + 1);
		//float d = GetHeightAtPosition(texcoord.x + 1, texcoord.y + 1);
		//if (hr) *hr = mix(mix(a, b, fx), mix(c, d, fx), fy);
		//return vec2(-a + b + (a - b - c + d) * fy, -a + c - (-a + b) * fx + (-c + d) * fx) * vec2(m_width, m_height);

#define L(ox, oy) (*pSimpleHeightMapFront)((int)clamp(texcoord.x + ox, 0.0f, (float)m_width - 1), (int)clamp(texcoord.y + oy, 0.0f, (float)m_height - 1))
		float a = L(-1, -1), b = L(0, -1), c = L(1, -1), d = L(2, -1);
		float e = L(-1, 0), f = L(0, 0), g = L(1, 0), h = L(2, 0);
		float i = L(-1, 1), j = L(0, 1), k = L(1, 1), l = L(2, 1);
		float m = L(-1, 2), n = L(0, 2), o = L(1, 2), p = L(2, 2);
#undef L

		float x = fx;
		float y = fy;

		float hval = 0.5*(0.5*(k*(x + 4 * x*x - 3 * x*x*x) + i*(-x + 2 * x*x - x*x*x) +
			l*(-(x*x) + x*x*x) + j*(2 - 5 * x*x + 3 * x*x*x))*(y + 4 * y*y - 3 * y*y*y) +
			0.5*(c*(x + 4 * x*x - 3 * x*x*x) + a*(-x + 2 * x*x - x*x*x) + d*(-(x*x) +
				x*x*x) + b*(2 - 5 * x*x + 3 * x*x*x))*(-y + 2 * y*y - y*y*y) + 0.5*(o*(x +
					4 * x*x - 3 * x*x*x) + m*(-x + 2 * x*x - x*x*x) + p*(-(x*x) + x*x*x) + n*(2
						- 5 * x*x + 3 * x*x*x))*(-(y*y) + y*y*y) + 0.5*(g*(x + 4 * x*x - 3 * x*x*x) +
							e*(-x + 2 * x*x - x*x*x) + h*(-(x*x) + x*x*x) + f*(2 - 5 * x*x +
								3 * x*x*x))*(2 - 5 * y*y + 3 * y*y*y));

		float dh_dx = 0.5*(0.5*(k*(1 + 8 * x - 9 * x*x) + i*(-1 + 4 * x - 3 * x*x) + l*(-2 * x +
			3 * x*x) + j*(-10 * x + 9 * x*x))*(y + 4 * y*y - 3 * y*y*y) + 0.5*(c*(1 + 8 * x -
				9 * x*x) + a*(-1 + 4 * x - 3 * x*x) + d*(-2 * x + 3 * x*x) + b*(-10 * x +
					9 * x*x))*(-y + 2 * y*y - y*y*y) + 0.5*(o*(1 + 8 * x - 9 * x*x) + m*(-1 + 4 * x
						- 3 * x*x) + p*(-2 * x + 3 * x*x) + n*(-10 * x + 9 * x*x))*(-(y*y) + y*y*y) +
			0.5*(g*(1 + 8 * x - 9 * x*x) + e*(-1 + 4 * x - 3 * x*x) + h*(-2 * x + 3 * x*x) +
				f*(-10 * x + 9 * x*x))*(2 - 5 * y*y + 3 * y*y*y));

		float dh_dy = 0.5*(0.5*(k*(x + 4 * x*x - 3 * x*x*x) + i*(-x + 2 * x*x - x*x*x) +
			l*(-(x*x) + x*x*x) + j*(2 - 5 * x*x + 3 * x*x*x))*(1 + 8 * y - 9 * y*y) +
			0.5*(c*(x + 4 * x*x - 3 * x*x*x) + a*(-x + 2 * x*x - x*x*x) + d*(-(x*x) +
				x*x*x) + b*(2 - 5 * x*x + 3 * x*x*x))*(-1 + 4 * y - 3 * y*y) + 0.5*(o*(x +
					4 * x*x - 3 * x*x*x) + m*(-x + 2 * x*x - x*x*x) + p*(-(x*x) + x*x*x) + n*(2
						- 5 * x*x + 3 * x*x*x))*(-2 * y + 3 * y*y) + 0.5*(g*(x + 4 * x*x - 3 * x*x*x) +
							e*(-x + 2 * x*x - x*x*x) + h*(-(x*x) + x*x*x) + f*(2 - 5 * x*x +
								3 * x*x*x))*(-10 * y + 9 * y*y));

		if (hr) *hr = hval;
		return vec2(dh_dx, dh_dy) * vec2(m_width, m_height);
	}
}
