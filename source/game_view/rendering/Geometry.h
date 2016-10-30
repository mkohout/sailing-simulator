#pragma once

#include <map>
#include <vector>
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Material.h"
#include "../../application_layer/core/stdafx.h"
#include "../../game_logic/data_structures/ICopyable.h"
#include "VertexData.h"
#include <glew/glew.h>
#include <limits>
#include <glm/matrix.hpp>
#include <cmath>

using namespace glm;
using namespace Game_Logic;
using std::map;
using std::vector;
using std::string;
using std::pair;

namespace Game_View
{
#define LOCATION_VERTEX 0
#define LOCATION_NORMAL 1
#define LOCATION_UV_COORD 2
#define LOCATION_MATERIAL_INDEX 3
#define LOCATION_INDEX 4

    class Geometry;
    
    typedef shared_ptr<Geometry> StrongGeometryPtr;

	struct AABB
	{
		glm::vec3 min;
		glm::vec3 max;

		AABB()
		{
			min = glm::vec3(std::numeric_limits<float>::max());
			max = glm::vec3(-std::numeric_limits<float>::max());
		}

		AABB(const glm::vec3& min, const glm::vec3& max)
			: min(min), max(max)
		{
			
		}

		glm::vec3 Center() const
		{
			return (min + max) / 2.0f;
		}

		glm::vec3 Size() const
		{
			return max - min;
		}

		AABB Transform(const glm::mat4& mat) const
		{
			//http://dev.theomader.com/transform-bounding-boxes/
			auto r = mat * vec4(1, 0, 0, 0), u = mat * vec4(0, 1, 0, 0), f = mat * vec4(0,0,1,0), t = mat * vec4(0,0,0,1);
			auto xa = vec3(r.x, r.y, r.z) * min.x,
				 xb = vec3(r.x, r.y, r.z) * max.x;
			auto ya = vec3(u.x, u.y, u.z) * min.y,
				 yb = vec3(u.x, u.y, u.z) * max.y;
			auto za = vec3(f.x, f.y, f.z) * min.z,
				 zb = vec3(f.x, f.y, f.z) * max.z;
			return AABB(
				glm::min(xa, xb) + glm::min(ya, yb) + glm::min(za, zb) + vec3(t.x, t.y, t.z),
				glm::max(xa, xb) + glm::max(ya, yb) + glm::max(za, zb) + vec3(t.x, t.y, t.z)
				);
		}

		bool Contains(const vec3& p) const
		{
			return min.x <= p.x && p.x <= max.x &&
				   min.y <= p.y && p.y <= max.y &&
				   min.z <= p.z && p.z <= max.z;
		}

		AABB Extend(const vec3& p) const
		{
			return AABB(glm::min(min, p), glm::max(max, p));
		}

		AABB Extend(const AABB& box) const
		{
			return AABB(glm::min(min, box.min), glm::max(max, box.max));
		}

		void ComputeCorners(vec3* a_corners) const
		{
			a_corners[0] = min;
			a_corners[1] = vec3(max.x, min.y, min.z);
			a_corners[2] = vec3(max.x, min.y, max.z);
			a_corners[3] = vec3(min.x, min.y, max.z);
			a_corners[4] = vec3(min.x, max.y, min.z);
			a_corners[5] = vec3(max.x, max.y, min.z);
			a_corners[6] = max;
			a_corners[7] = vec3(min.x, max.y, max.z);
		}

		bool Intersect(const vec3& m_Dir, const vec3& m_Ori, float* t_min = 0, float* t_max = 0) const
		{
			float t1 = (min.x - m_Ori.x) / m_Dir.x;
			float t2 = (max.x - m_Ori.x) / m_Dir.x;
			float t3 = (min.y - m_Ori.y) / m_Dir.y;
			float t4 = (max.y - m_Ori.y) / m_Dir.y;
			float t5 = (min.z - m_Ori.z) / m_Dir.z;
			float t6 = (max.z - m_Ori.z) / m_Dir.z;
			float mi = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
			float ma = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));
			bool b = ma > mi && ma > 0;
			if (b && t_min)
				*t_min = mi;
			if (b && t_max)
				*t_max = ma;
			return b;
		}
	};

	/**
	 * Contains all geometry informations an object can have
	 */
    class Geometry : public ICopyable<Geometry>
	{
	private:
        /**
         * Indicates whether the geometry needs to get transferred
         * every frame or if it stays constant and only the matrices
         * will change.
         */
        bool m_isVolatile;
        
        /**
         * The list of the materials the current geometry uses
         */
        Materials m_materials;
        
        /**
         * All vertices with their belonging data, such as UV's and normals
         * of the current geometry
         */
        vector<VertexData> m_vertexData;

		AABB m_localBox;
        
        /**
         * Before the indices are added to the geometry,
         * every vertex is mapped to its index. After all
         * indices have been added to the geometry,
         * the map will be transferred into the m_vertexData vector
         */
        map<unsigned int, VertexData> m_vertexMapping;
        
        /**
         * The vector that holds the indices to draw the elements
         */
        vector<unsigned int> m_indices;

		/**
		 * The OpenGL handle for the vertex array
		 */
		GLuint m_vertexArrayHandle;

		/**
		 * OpenGL handle for buffering the geometry data
		 */
		GLuint m_vertexBufferHandle;
        
        /**
         * OpenGL handle for buffering the indices
         */
        GLuint m_elementBuffer;

		bool m_useBackfaceCulling;
        
        /**
         * Iterates through m_vertexMapping and stores all
         * the vertices in a vector
         */
        void TransferVertexData();

	public:
		/**
		 * Geometry object is instantiated without any params,
		 * data will be assigned by member functions
		 */
        Geometry() : m_indices(), m_useBackfaceCulling(true)
		{
			m_vertexArrayHandle = std::numeric_limits<GLuint>::max();
			m_vertexBufferHandle = std::numeric_limits<GLuint>::max();
		}

		/**
		 * Nothing needs to be destroyed here
		 */
		~Geometry() { }

        /**
         * Returns the data needed for storing a binary
         */
		void GetSerializationData(
                const vector<VertexData>*&,
                const vector<unsigned int>*&,
                const Materials*&);

        /**
         * Assigns the data read from the binary file to
         * save loading time
         */
        void SetSerializationData(
                const vector<VertexData>&,
                const vector<unsigned int>&,
                const Materials&);
        
        /**
         * Provide the geometry to OpenGL by loading
         * the geometry into the buffer of the graphics card
         */
        void Enable();
        
        /**
         * Disabling the geometry by clearing the
         * array postions for the vertex arrays
         */
        void Disable() const;
        
        /**
         * Adds a vertex with the given index to the geometry
         */
        void AddVertexData(const unsigned int, VertexData);
        
        vector<VertexData>& GetAllVertexData();
        
        /**
         * Returns true if the geometry has any vertices
         * because the m_vertexMapping will be cleared after
         * the first draw, it will check both member variables
         */
        inline bool HasVertices()
        {
            return m_vertexMapping.size()>0 || m_vertexData.size()>0;
        }
        
        /**
         * Adds an index to the current geometry
         */
        void AddIndex(const unsigned int);
        
        /**
         * Returns a reference to the vector that holds all indices
         */
        vector<unsigned int>& GetIndices()
        {
            return m_indices;
        }

        /**
         * Returns true if the current geometry already has a vertex
         * with the given index, false otherwise
         */
        bool HasVertexDataForPosition(const unsigned int&);

		/*
         * returns a handle to the vertex array object and buffer with the geometrys contents
		 * (in the first call the data is copied to the GPU)
		*/
		void GetGLBufferInfo(GLuint&, GLuint&);
        
        /**
         * Returns a new geometry instance with exact the
         * same geometry
         */
        virtual StrongGeometryPtr VCopy();
        
        /**
         * Assigns the given materials to the current geometry
         */
        void AssignMaterials(Materials);

        /**
         * Returns true if the current geometry has any materials
         */
        bool HasMaterials() const;
        
        /**
         * Returns a reference to the material list of the current geometry
         */
        Materials& GetMaterials();

        /**
         * TODO
         */
		const AABB& GetLocalAABB() const
		{
			return m_localBox;
		}

        /**
         * Access Aligned Bounding Box
         */
		void SetLocalBox(const AABB& box)
		{
			m_localBox = box;
		}
        
        /**
         * Returns the vertex that can be found at the given index
         */
        VertexData& GetVertexAtIndex(unsigned int index)
        {
            return m_vertexData[index];
        }
        
        /**
         * Overwrites the position that can be found at the given index
         * with the new position
         */
        void SetPositionAtIndex(unsigned int index, vec3 newPosition)
        {
            if(index>m_vertexData.size())
            {
                return ;
            }
            
            m_vertexData[index].Position = newPosition;
			m_localBox = m_localBox.Extend(newPosition);
        }
        
        /**
         * Sets the vertex data of the current geometry to
         * the given vertex data
         */
        void SetAllVertexData(vector<VertexData> vertexData)
        {
            m_vertexData = vertexData;
        }
        
        /**
         * Sets the current index data to the given new one
         */
        void SetIndices(vector<unsigned int> indices)
        {
            m_indices = indices;
            m_vertexMapping = map<unsigned int, VertexData>();
        }
        
        /**
         * Changes the flag m_isVolatile to the given value
         */
        void SetIsVolatile(bool isVolatile)
        {
            m_isVolatile = isVolatile;
        }

		bool GetBackFaceCulling() const
		{
			return m_useBackfaceCulling;
		}

		void SetBackFaceCulling(bool val)
		{
			m_useBackfaceCulling = val;
		}
	};
}
