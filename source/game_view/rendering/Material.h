#pragma once

#include <assert.h>
#include <memory>
#include <string>

#include <glm/vec3.hpp>

#include "Texture.h"

#include "../../application_layer/core/stdafx.h"

#include "../../application_layer/resources/XMLElement.h"
#include "../../application_layer/resources/XMLResource.h"
#include <algorithm>

using namespace glm;
using namespace Application_Layer;
using std::shared_ptr;
using std::string;

namespace Game_View
{
	/**
	 * Class is representing base class for the phong lightning model
	 */
	class Material
	{
	private:
        /**
         * Identifier of the material, such as "wood"
         */
        char m_identifier[255];

        /**
         * The ambient color values of the current material
         */
		vec3 m_ambientColor;

        /**
         * The diffuse color values of the current material
         */
		vec3 m_diffuseColor;

        /**
         * Specular color values of the current material
         */
		vec3 m_specularColor;
        
		float m_specularExponent;

        /**
         * Degree of transparency of the current material
         * 1.0f means fully opaque
         */
		float m_transparency;

        /**
         * The textures of the material
         */
        Textures m_textures;
        
        /**
         * Initializes the textures by the given xml element
         */
        void InitTexturesByXML(StrongXMLElementPtr);
        
	public:
		/**
		 * Default constructor, because there is nothing special to set
		 */
		Material() = default;
        
        void SetIdentifier(const string identifier)
        {
			auto l = std::min(identifier.size(), sizeof(m_identifier) - 1);
			memcpy(m_identifier, identifier.c_str(), l);
			m_identifier[l] = 0;
        }
        
        const string GetIdentifier()
        {
            return m_identifier;
        }

		const vec3 GetAmbientColor() const
		{
			return m_ambientColor;
		}

		void SetAmbientColor(vec3 ambientColor)
		{
			m_ambientColor = ambientColor;
		}

		const vec3 GetDiffuseColor() const
		{
			return m_diffuseColor;
		}

		void SetDiffuseColor(vec3 diffuseColor)
		{
			m_diffuseColor = diffuseColor;
		}

		const vec3 GetSpecularColor() const
		{
			return m_specularColor;
		}

		void SetSpecularColor(vec3 specularColor)
		{
			m_specularColor = specularColor;
		}

		const float GetSpecularExponent() const
		{
			return m_specularExponent;
		}

		void SetSpecularExponent(float specularExponent)
		{
			m_specularExponent = specularExponent;
		}
        
        bool UsesTextures() const
        {
            return m_textures.size()>0;
        }

		/**
		 * Returns the transparency factor of the current material
		 * (value betw. 0.0f and 1.0f)
		 */
		const float GetTransparency() const
		{
			return m_transparency;
		}

		/**
		 * Sets the transparency for the current material
		 * (value betw. 0.0f and 1.0f
		 */
		void SetTransparency(float transparency)
		{
			assert(transparency >= 0.0f && transparency <= 1.0f);

			m_transparency = transparency;
		}

		/**
		 * Initializes the material by given xml resource
		 */
		void InitByResource(XMLResource*);

		/**
		 * Initializes the material by given xml element
		 */
		void InitByXMLElement(StrongXMLElementPtr);
        
        /**
         * Loads the material from the specification with the given
         * identifier
         */
        void LoadFromFile(std::string);

		/**
		 * Loads the default material
		 */
		void LoadDefault();
	};
    
    /**
     * Structure of storing all materials of an object
     */
    typedef vector<Material> Materials;
}