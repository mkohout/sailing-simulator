#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "ActorComponent.h"

#include "../actor_system/Actor.h"

#include "../../application_layer/resources/ObjResource.h"
#include "../../application_layer/resources/XMLElement.h"

#include "../../application_layer/error_handling/KeyNotFoundException.h"

#include "../../game_view/rendering/Material.h"
#include "../../game_view/rendering/Texture.h"
#include "../../game_view/video_system/IShader.h"
#include "../../game_view/rendering/Geometry.h"

using namespace Game_View;
using namespace Application_Layer;

using std::map;
using std::pair;
using std::vector;
using std::string;
using std::shared_ptr;

namespace Game_Logic
{

	typedef map<string, float> SpecificShaderParameters;
    typedef map<IShader::ShaderType, SpecificShaderParameters> ShaderParameters;

	/**
	 * Component is responsible for all the stuff that is going to the screen later
	 */
	class RenderComponent : public ActorComponent
	{
	private:
        map<IShader::ShaderType, string> m_shaderNames;

		/*
		 * A list of parameters that will be passed to the
         * shaders when the actor with the current component will be rendered
		 */
		ShaderParameters m_shaderParameters;

		/*
		 * Textures of the render component,
		 * can be anything: normals, textures, mip maps
		 */
		Textures m_textures;

		/*
		 * Geometry that will be rendered of the current actor
		 */
		StrongGeometryPtr m_pGeometry;

		/*
		 * If no material were set by the configuration of the render component
         * use the in the game settings specified default material
		 */
		void UseDefaultMaterial();
        
        IShader::ShaderType GetShaderTypeByIdentifier(std::string);
        
        /**
         * Loads the material with the given identifier from the hard disk
         * and assigns it to the render component.
         */
        Material LoadMaterialWithName(const std::string);

	public:
		RenderComponent()
		{
		}
        
		~RenderComponent();

		virtual void VInit();

		virtual string VGetIdentifier() const
		{
			return "renderComponent";
		}

		/**
		 * Extract data from given XML specification
		 */
		virtual void VFetchData(StrongXMLElementPtr);

		/*
		 * Returns true if the component holds any geometry
		 */
		bool HasGeometry() const
		{
			return m_pGeometry != nullptr;
		}

		/*
		 * Adds a new shader parameter to the list
		 */
        void AddShaderParameter(IShader::ShaderType, string, float);

        /*
         * Returns true if the render component holds a parameter
         * with the given shader and key combination
         */
        bool HasShaderParameter(IShader::ShaderType, const string&);

		/*
		 * Returns a specific shader parameter
		 */
        float GetShaderParameter(IShader::ShaderType, const string&)
            throw (KeyNotFoundException);

        SpecificShaderParameters GetShaderParametersForShader(IShader::ShaderType);
        
		/*
		 * Returns a list with all shader parameters that
         * were set in the xml file
		 */
		ShaderParameters GetAllShaderParameters();

		/**
		 * Initializes the geometry by the given xml element
		 */
		void InitGeometry(StrongXMLElementPtr);
        
        /**
         * Initializes the materials, if the render component has any
         * materials specified in the XML
         */
        void InitMaterials(StrongXMLElementPtr);

		/*
		 * Returns the geometry of the current actor
		 */
		StrongGeometryPtr GetGeometry() const
		{
			return m_pGeometry;
		}

		/**
		 * Extracts the informations from the given XML element
         * and initializes the member variables by the xml data
		 */
		void InitShader(StrongXMLElementPtr);

        /**
         * Fill the parameter list based on the given xml data
         */
        void InitShaderParameters(IShader::ShaderType, StrongXMLElementPtr);
        
		/*
		 * Returns the shader that is used for the given type of shaders
		 */
		const string GetShaderName(IShader::ShaderType whichShader)
		{
            auto findIt = m_shaderNames.find(whichShader);
            
            if(findIt==m_shaderNames.end())
            {
                return "";
            }
            
            return m_shaderNames.at(whichShader);
        }

		/*
		 * Initialize the textures that were given to the render component
		 */
		void InitTextures(StrongXMLElementPtr);

		/*
		 * Returns true, if at least one texture is assigned
		 */
		bool HasTextures() const
		{
			return m_textures.size() > 0;
		}

		const Textures& GetTextures() const
		{
			return m_textures;
		}

        /**
         * Returns a std::map that maps every shader type to
         * its corresponding shader filename
         */
        const map<IShader::ShaderType, string> GetShaderNames() const
        {
            return m_shaderNames;
        }
        
        /*
         * Adds a shader that was read in the xml to the render component
         */
        void AddShader(IShader::ShaderType, string);
        
        /**
         * Returns true if the current render component
         * holds a shader for the given type
         */
        bool HasShader(IShader::ShaderType) const;

		/*
		 * Adds a texture to the current rendering component
		 */
		void AddTexture(StrongTexturePtr);

		/*
		 * Returns true if the underlying geometry has any
         * materials assigned
		 */
		bool HasMaterial() const
		{
            return m_pGeometry!=nullptr && m_pGeometry->HasMaterials();
		}

        /**
         * Adds a material to the underlying geometry
         */
		void AddMaterial(Material);

		Materials GetMaterials() const
		{
            if(m_pGeometry==nullptr)
            {
                return Materials();
            }
            
			return m_pGeometry->GetMaterials();
		}
        
        /**
         * Overwrites the geometry of the render component with
         * the given geometry
         */
        void SetGeometry(StrongGeometryPtr pGeometry)
        {
            m_pGeometry = pGeometry;
        }
	};

}