#pragma once

#include "../../application_layer/resources/ImageResource.h"

#include <vector>
#include <memory>
#include <map>
#include <glew/glew.h>
#include <glfw/glfw3.h>

using namespace Application_Layer;

using std::vector;
using std::shared_ptr;
using std::map;
using std::pair;

namespace Game_View
{
    typedef map<GLenum, GLuint> TextureParameters;
    
	/**
	 * Class that provides functionalities
	 * to load and assign textures to the
	 * rendering engine
	 */
	class Texture
	{
	public:
		typedef enum {
			TEXTURE,
            CUBEMAP,
            HEIGHT_MAP,
            NORMAL_MAP
		} TYPE;

	private:
        /**
         * After the texture has been allocated using OpenGL
         * the image data held in this class will be deleted
         * if the image is needed somehow in the future, this
         * member variable indicates if it needs a fresh reload
         * from the hd
         */
        bool m_needsReload;

        /**
         * The name of the current texture
         */
        char m_name[255];
        
        /**
         * Size of the current texture
         */
        int m_width;
        int m_height;
        
        /**
         * 
         */
        TextureParameters m_textureParameters;
        
        /**
         * Color channels of the current texture
         * RGBA = 4, RGB = 3, etc.
         */
        int m_channels;
        
		/**
		 * Type of the current texture
		 */
		TYPE m_type;
        
        char m_filename[255];
        
        /**
         * The plain image data of the current texture
         */
        unsigned char* m_pImageData;
        
        void LoadImageData();
        
        void LoadRegularTexture();
        
        void LoadHeightmap();
        
        void LoadDefaultParameters();
        
        /**
         * Sets the filename to the given
         */
        void SetFilename(const std::string filename);
        
        /**
         * Since images that were loaded using SOIL
         * are inverted vertically, we need to flip then
         */
        void FlipImage();

	public:
        Texture() = default;
        
        ~Texture()
        {
            //TODO
            //Dispose();
        }
        
        void Init();
        
		void LoadFromResource(StrongResourcePtr);

        void SetType(const string&);
        
        void SetName(const string& name)
        {
            auto l = std::min(name.size(), sizeof(m_name) - 1);
            memcpy(m_name, name.c_str(), l);
            m_name[l] = 0;
        }
        
        inline const int GetWidth() const
        {
            return m_width;
        }
        
        inline const int GetHeight() const
        {
            return m_height;
        }
        
        inline const int GetChannels() const
        {
            return m_channels;
        }
        
        const unsigned char* GetImageData();
        
		const TYPE GetType() const
		{
			return m_type;
		}
        
        const string GetName() const
        {
            return m_name;
        }
        
        TextureParameters& GetParameters()
        {
            return m_textureParameters;
        }
        
        /**
         * Delete the image data
         */
        void Dispose();
	};

	typedef shared_ptr<Texture> StrongTexturePtr;
    
    typedef vector<StrongTexturePtr> Textures;
}