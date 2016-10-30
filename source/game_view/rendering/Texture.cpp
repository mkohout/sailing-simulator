#include <string>
#include "Texture.h"
#include "../../application_layer/core/GameCore.h"
#include "../../application_layer/utils/Logger.h"
#include "../../extern/soil/SOIL.h"

using namespace Game_View;

void Texture::LoadFromResource(StrongResourcePtr pResource)
{
    ImageResource* pImageResource =
                    dynamic_cast<ImageResource*>(pResource.get());
 
    std::string filename = pImageResource->GetFilename();
    
    SetFilename(filename);
}

void Texture::SetFilename(const std::string filename)
{
    auto l = std::min(filename.size(), sizeof(m_filename) - 1);
    memcpy(m_filename, filename.c_str(), l);
    
    m_filename[l] = 0;
}

void Texture::Init()
{
    LoadDefaultParameters();
}

void Texture::LoadDefaultParameters()
{
    //TODO, assign from XML
    switch(m_type)
    {
        case CUBEMAP:
        {
            m_textureParameters.insert(
                    pair<GLenum, GLuint>(
                        GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            m_textureParameters.insert(
                    pair<GLenum, GLuint>(
                        GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            m_textureParameters.insert(
                    pair<GLenum, GLuint>(
                        GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        }
        break;
        
        default:
        {
            m_textureParameters.insert(
                    pair<GLenum, GLuint>(
                        GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
            m_textureParameters.insert(
                    pair<GLenum, GLuint>(
                        GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
        }
        break;
    }
    
    m_textureParameters.insert(
                    pair<GLenum, GLuint>(
                        GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    m_textureParameters.insert(
                    pair<GLenum, GLuint>(
                        GL_TEXTURE_MAG_FILTER, GL_LINEAR));
}

void Texture::SetType(const string& typeString)
{
    if(typeString.compare("height_map")==0)
    {
        m_type = HEIGHT_MAP;
    }
    else if(typeString.compare("normal_map")==0)
    {
        m_type = NORMAL_MAP;
    }
    else if(typeString.compare("cubemap")==0)
    {
        m_type = CUBEMAP;
    }
    else
    {
        m_type = TEXTURE;
    }
    
    LoadImageData();
}

const unsigned char* Texture::GetImageData()
{
    if(m_needsReload==true)
    {
        LoadImageData();
    }
    
    return m_pImageData;
}

void Texture::LoadImageData()
{
    switch(m_type)
    {
        case CUBEMAP:
        case TEXTURE:
            LoadRegularTexture();
            break;
        
        case NORMAL_MAP:
        case HEIGHT_MAP:
            LoadHeightmap();
            break;
    }
    
    if(m_pImageData==nullptr)
    {
        const string errorMessage =
            "Cannot load texture " + std::string(m_name) + ", " + SOIL_last_result();
        
        g_pGameApp->LogString(
                              Application_Layer::Logger::ERROR,
                              errorMessage
                              );
        
        throw "Cannot load texture";
    }
    
    FlipImage();
}

void Texture::FlipImage()
{
    int i, j;
    
    for(j = 0; j * 2 < m_height; ++j)
    {
        int index1 = j * m_width * m_channels;
        int index2 = (m_height - 1 - j) * m_width * m_channels;
        
        for(i = m_width * m_channels; i > 0; --i)
        {
            unsigned char temp = m_pImageData[index1];
            m_pImageData[index1] = m_pImageData[index2];
            m_pImageData[index2] = temp;
            
            ++index1;
            ++index2;
        }
    }
}

void Texture::LoadRegularTexture()
{
    m_pImageData = SOIL_load_image(
                                   m_filename,
                                   &m_width,
                                   &m_height,
                                   &m_channels,
                                   SOIL_LOAD_AUTO
                                   );
}

void Texture::LoadHeightmap()
{
    m_pImageData = SOIL_load_image(
                                   m_filename,
                                   &m_width,
                                   &m_height,
                                   &m_channels,
                                   SOIL_LOAD_L
                                   );
}

void Texture::Dispose()
{
    SOIL_free_image_data(m_pImageData);
    m_needsReload = true;
}
