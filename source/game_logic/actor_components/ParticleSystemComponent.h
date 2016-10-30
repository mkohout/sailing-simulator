#pragma once
#include "ActorComponent.h"
#include "IPhysicsComponent.h"
#include <glew/glew.h>
#include <glm/common.hpp>
#include <functional>

namespace Game_View
{
	class Texture;
}

namespace Game_Logic
{
	class ParticleSystemComponent : public ActorComponent, public IPhysicsComponent
	{
		struct particle
		{
			glm::vec3 pos;
			float rad;
			glm::vec3 dir;
			float time;
			int flags;

			particle(const glm::vec3& p, const glm::vec3& d, float r, bool objSpace)
				: pos(p), dir(d), rad(r), flags(((int)objSpace) << 1 | 1), time(0)
			{
				
			}

			particle()
				: flags(0)
			{
				
			}
		};


		std::vector<particle> m_particles;
		unsigned int m_numParticles;
		bool looped;

		/**
		* The OpenGL handle for the vertex array
		*/
		GLuint m_vertexArrayHandle;

		/**
		* OpenGL handle for buffering the geometry data
		*/
		GLuint m_vertexBufferHandle;

		GLuint m_particleMap;
		std::string m_particleTextureName;

		glm::vec3 m_offset;

		bool m_gpuBufferNeedsUpdate;

		float m_decayTime;
	public:
		ParticleSystemComponent();

		virtual ~ParticleSystemComponent()
		{
			
		}

		virtual void VInit();

		virtual string VGetIdentifier() const
		{
			return "particleSystemComponent";
		}

		GLuint GetParticleMap() const
		{
			return m_particleMap;
		}

		virtual void VFetchData(StrongXMLElementPtr);

		const glm::vec3& GetOffset() const
		{
			return m_offset;
		}

		float GetDecayTime() const
		{
			return m_decayTime;
		}

		void CreateSpray(int N, const glm::vec3& dir, const glm::vec3& position, float angleCutoff, bool inObjSpace);
		
		void LoadOpenGLData(std::function<void(GLuint, Game_View::Texture&)> assignClb);
		void Draw();

		virtual void VUpdate(const unsigned int);
	};
}