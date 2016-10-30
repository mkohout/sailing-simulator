#pragma once

#include <string>
#include <vector>

#include "IResource.h"
#include "CompiledMeshResource.h"

#include "../../game_view/rendering/Geometry.h"
#include "../../game_view/rendering/Material.h"

using std::string;

using namespace Game_View;

namespace Application_Layer
{
    typedef vec3 Vertex;
    
    /**
     * Data structure for storing all vertices of a geometry
     */
    typedef vector<Vertex> Vertices;
    
    /**
     * Data structure for storing all normals of a geometry
     */
    typedef vector<vec3> Normals;
    
    /**
     * Data structure for storing all uvs of a geometry
     */
    typedef vector<vec2> UVs;
    
	/**
	 * Class is representing the value extracted out of a Wavefront .obj-File
	 */
	class ObjResource : public IResource
	{
	private:
        /**
         * All vertices of the current geometry
         */
        Vertices m_vertices;
        
        /**
         * All vertex normals of the current geometry
         */
        Normals m_normals;
        
        /**
         * UV coordinates of the current geometry
         */
        UVs m_uvs;
        
		/**
		 * Geometry the obj file is representing
		 */
		StrongGeometryPtr m_pGeometry;

        /**
         * A vector that stores all different materials,
         * the current object uses
         */
        Materials m_materialList;
        
        /**
         * The index for the material, that is used
         * for the next faces
         */
        float m_currentMaterialIndex;
        
		/**
		 * The wavefront specification offers
		 * these kinds of line types that are
		 * relevant to the engine
		 */
		typedef enum {
			UNKNOWN,
			VERTEX,
			TEXTURE_COORDINATE,
			NORMAL,
			FACE,
            MATERIAL
		} LINE_TYPE;

		/**
		 * Returns the type of the given line
		 */
		const LINE_TYPE GetLineType(const string&);

		/**
		 * Takes care of the given line and delegates
		 * it to the according function
		 */
		void HandleLine(const string&);

		void HandleVertexLine(const vector<string>&);

		void HandleNormalLine(const vector<string>&);

		void HandleUVLine(const vector<string>&);

		void HandleFaceLine(const vector<string>&);
        
        void HandleMaterialLine(const vector<string>&);
        
        /**
         * Adds a single vertex to the current geometry
         * most likely from an obj resource - performs a transform into floats first
         */
        void AddVertex(const string&, const string&, const string&);
        
        /**
         * Adds a single vertex to the current geometry
         */
        void AddVertex(const float&, const float&, const float&);
        
        /**
         * Returns the vertex that is belonging to the given index
         */
        const Vertex GetVertexByIndex(const unsigned int) const;
        
        /**
         * Adds a normal to the current geometry
         * most likely from an obj resource - performans a transform into floats first
         */
        void AddNormal(const string&, const string&, const string&);
        
        /**
         * Adds a normal to the current geometry
         */
        void AddNormal(const float&, const float&, const float&);
        
        /**
         * Returns the normal of the given index
         * to combine a vertex with the corresponding normal
         */
        const vec3 GetNormalByIndex(const unsigned int) const;
        
        /**
         * Adds uv pair to the current geometry
         * most likely from an obj resource - performans a transform into floats first
         */
        void AddUV(const string&, const string&);
        
        void AddUV(const float&, const float&);
        
        /**
         * Returns the UV coordinates of the given index
         * to combine a vertex with the corresponding uv
         */
        const vec2 GetUVByIndex(const unsigned int) const;
        
        /**
         * Returns true if the given material is already loaded
         */
        bool MaterialLoaded(const string&);

        /**
         * Loads a material with the given name
         */
        void LoadMaterial(const string&);
        
        /**
         * Sets the given material as the current.
         * The current material will be used for added
         * faces until a new material is set
         */
        void SetCurrentMaterial(const string&);
        
        /**
         * Returns the current material index
         */
        const float GetCurrentMaterialIndex();
        
        /**
         * After the geometry and the materials were processed
         * the list of materials will be attached to the geometry
         */
        void AssignMaterialsToGeometry();


	public:
		static const TYPE s_resourceType = OBJ;

		ObjResource()
		{
			m_pGeometry = StrongGeometryPtr(new Geometry());
		}
		virtual ~ObjResource();

		virtual void VInitFromFile(const string&);

		StrongGeometryPtr GetCreatedGeometry() const
		{
			if (!m_pGeometry->HasVertices())
			{
				return nullptr;
			}

			return m_pGeometry;
		}
        
        /**
         * Compiles the current geometry and material data
         * and saves it to the given location
         */
        void Compile(const string&);
        
        /**
         * Initializes the geometry and material data from the
         * given pointer that points to the compiled resource
         */
        void LoadFromCompiledResource(CompiledMeshResource*);
        
        virtual void VDispose();

		/**
		 * Returns the type of the current resource
		 */
		virtual TYPE VGetType() const
		{
			return s_resourceType;
		}
	};

}