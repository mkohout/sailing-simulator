#include "RenderComponent.h"

#include "../game_app/GameApp.h"

#include "../../application_layer/core/GameCore.h"
#include "../../application_layer/resources/CompiledMeshResource.h"

using namespace Game_Logic;

RenderComponent::~RenderComponent()
{
}

void RenderComponent::VInit()
{
    // Every render components needs a vertex shader
    // and a fragment shader
    // the other shaders are optional
    if (!HasShader(IShader::VERTEX_SHADER))
	{
        AddShader(
                  IShader::VERTEX_SHADER,
                  g_pGameApp->GetGameSettings()->GetValue("defaultVertexShader")
                  );
	}
    
    if (!HasShader(IShader::FRAGMENT_SHADER))
    {
        AddShader(
                  IShader::FRAGMENT_SHADER,
                  g_pGameApp->GetGameSettings()->GetValue("defaultFragmentShader")
                  );
    }

	if (m_pGeometry!=nullptr && !HasMaterial())
	{
		UseDefaultMaterial();
	}
}

void RenderComponent::AddShader(
                    IShader::ShaderType shaderType, string shaderName)
{
    if(HasShader(shaderType))
    {
        return ;
    }
    
    m_shaderNames.insert(
            std::pair<IShader::ShaderType, string>(shaderType, shaderName));
}

void RenderComponent::VFetchData(StrongXMLElementPtr pXmlElement)
{
	if (pXmlElement->HasChildrenNamed("geometry"))
	{
		InitGeometry(
			pXmlElement->FindFirstChildNamed("geometry"));
	}

	if (pXmlElement->HasChildrenNamed("shader"))
	{
		InitShader(
			pXmlElement->FindFirstChildNamed("shader"));
	}

	if (pXmlElement->HasChildrenNamed("textures"))
	{
		InitTextures(
			pXmlElement->FindFirstChildNamed("textures"));
	}
    
    if(pXmlElement->HasChildrenNamed("materials"))
    {
        InitMaterials(pXmlElement->FindFirstChildNamed("materials"));
    }
}
                      
void RenderComponent::InitMaterials(StrongXMLElementPtr pMaterialsElement)
{
    for(StrongXMLElementPtr pMaterialElement : pMaterialsElement->GetChildren())
    {
        AddMaterial(
                    LoadMaterialWithName(pMaterialElement->GetValueOfAttribute("name")));
    }
}

Material RenderComponent::LoadMaterialWithName(const std::string materialName)
{
    Material material;
    
    material.LoadFromFile(materialName);
    
    return material;
}

void RenderComponent::InitTextures(StrongXMLElementPtr pXmlElement)
{
	StrongXMLElementPtr pTextureElement =
		pXmlElement->FindFirstChildNamed("texture");

	int index = 0;

	while (pTextureElement != nullptr)
	{
        StrongTexturePtr pTexture(new Texture());
		StrongResourcePtr pResource =
			g_pGameApp->GetResourceManager()->GetResource(
				g_pGameApp->GetFileManager()->GetPathToTextureFile(
					pTextureElement->GetValueOfAttribute("source")
					),
				IResource::IMAGE
				);
        
        ImageResource* pImageResource =
                        dynamic_cast<ImageResource*>(pResource.get());

		pTexture->LoadFromResource(pResource);
		pTexture->SetType(pTextureElement->GetValueOfAttribute("type", ""));
        pTexture->Init();//init after setting type!
        pTexture->SetName(pImageResource->GetBasename());

		AddTexture(pTexture);

		pTextureElement =
			pXmlElement->FindChildNamed("texture", ++index);
	}
}

void RenderComponent::AddTexture(StrongTexturePtr pTexture)
{
	m_textures.push_back(pTexture);
}

void RenderComponent::AddMaterial(Material material)
{
    if(m_pGeometry==nullptr)
    {
        return ;
    }
    
    m_pGeometry->GetMaterials().push_back(material);
}

void RenderComponent::InitGeometry(StrongXMLElementPtr pXMLElement)
{
	if (!pXMLElement->HasAttributeNamed("source"))
	{
		g_pGameApp->LogString(
			Logger::WARNING,
			"RenderComponent(Actor={{value}} has Geometry without a source specified"
			);
		return;
	}

	string objFile =
		pXMLElement->GetValueOfAttribute("source");
	string objFilename =
		g_pGameApp->GetFileManager()->GetPathToObjectFile(objFile);

    StrongResourcePtr pResource =
            g_pGameApp->GetResourceManager()->GetResource(objFilename, IResource::OBJ);
    shared_ptr<ObjResource> pObjResource =
                std::static_pointer_cast<ObjResource>(pResource);
        
    m_pGeometry = StrongGeometryPtr(pObjResource->GetCreatedGeometry());

	if (pXMLElement->HasAttributeNamed("disableCulling") && pXMLElement->GetValueOfAttribute("disableCulling", "false") == "true")
		m_pGeometry->SetBackFaceCulling(false);

	if (m_pGeometry!=nullptr && !HasMaterial())
	{
		UseDefaultMaterial();
	}
}

void RenderComponent::InitShader(StrongXMLElementPtr pXMLElement)
{
    std::string possibleShaderElements[] =
                    {"vertexShader", "fragmentShader", "tessellationControlShader",
                        "tessellationEvalShader", "computeShader", "geometryShader"};
    
    
    for(std::string shaderElementName : possibleShaderElements)
    {
        if (pXMLElement->HasChildrenNamed(shaderElementName) &&
            pXMLElement->FindFirstChildNamed(shaderElementName)->HasAttributeNamed("source"))
        {
            StrongXMLElementPtr pShaderElement =
                                pXMLElement->FindFirstChildNamed(shaderElementName);
            
            IShader::ShaderType shaderType =
                            GetShaderTypeByIdentifier(shaderElementName);
            
            AddShader(
                      shaderType,
                      pXMLElement->FindFirstChildNamed(shaderElementName)->GetValueOfAttribute("source")
                      );
            
            if(pShaderElement->HasChildrenNamed("parameters"))
            {
                InitShaderParameters(
                        shaderType, pShaderElement->FindFirstChildNamed("parameters"));
            }
        }
    }
}

void RenderComponent::InitShaderParameters(
            IShader::ShaderType shaderType, StrongXMLElementPtr pXMLElement)
{
    XMLElements parameters = pXMLElement->GetChildren();
    
    for(StrongXMLElementPtr pParameterElement : parameters)
    {
        AddShaderParameter(
                        shaderType,
                        pParameterElement->GetName(),
                        std::stof(pParameterElement->GetContent())
                        );
    }
}

IShader::ShaderType RenderComponent::GetShaderTypeByIdentifier(std::string shaderIdentifier)
{
    if(shaderIdentifier.compare("vertexShader")==0)
    {
        return IShader::VERTEX_SHADER;
    }
    else if(shaderIdentifier.compare("fragmentShader")==0)
    {
        return IShader::FRAGMENT_SHADER;
    }
    else if(shaderIdentifier.compare("geometryShader")==0)
    {
        return IShader::GEOMETRY_SHADER;
    }
	else if (shaderIdentifier.compare("tessellationControlShader") == 0)
	{
		return IShader::TESSELLATION_CONTROL_SHADER;
	}
	else if (shaderIdentifier.compare("tessellationEvalShader") == 0)
	{
		return IShader::TESSELLATION_EVAL_SHADER;
	}
    else if(shaderIdentifier.compare("computeShader")==0)
    {
        return IShader::COMPUTE_SHADER;
    }
    
    return IShader::NO_SHADER;
}

void RenderComponent::UseDefaultMaterial()
{
    Material defaultMaterial;

	defaultMaterial.LoadDefault();
    
    AddMaterial(defaultMaterial);
}

void RenderComponent::AddShaderParameter(
                IShader::ShaderType shaderType, string key, float value)
{
    if(HasShaderParameter(shaderType, key))
    {
        return ;
    }
    
    auto findIt = m_shaderParameters.find(shaderType);
    
    if(findIt==m_shaderParameters.end())
    {
        m_shaderParameters.insert(
                std::pair<IShader::ShaderType, SpecificShaderParameters>(
                                        shaderType, SpecificShaderParameters()));
    }
    
    SpecificShaderParameters& shaderParameter =
                            m_shaderParameters.at(shaderType);
    
    shaderParameter.insert(std::pair<string, float>(key, value));
}

bool RenderComponent::HasShaderParameter(
                    IShader::ShaderType shaderType, const string& key)
{
    auto findIt = m_shaderParameters.find(shaderType);
    
    if (findIt == m_shaderParameters.end())
    {
        return false;
    }
    
    SpecificShaderParameters shaderParameters =
    m_shaderParameters.at(shaderType);
    
    auto findShaderParam = shaderParameters.find(key);
    
    if (findShaderParam == shaderParameters.end())
    {
        return false;
    }
    
    return true;
}

bool RenderComponent::HasShader(IShader::ShaderType shaderType) const
{
    auto findIt = m_shaderNames.find(shaderType);
    
    return findIt!=m_shaderNames.end();
}

float RenderComponent::GetShaderParameter(
                    IShader::ShaderType shaderType, const string& key)
throw (KeyNotFoundException)
{
    if(!HasShaderParameter(shaderType, key))
    {
        throw new KeyNotFoundException("Shader Parameter not found");
    }
    
    SpecificShaderParameters shaderParameters =
                                    m_shaderParameters.at(shaderType);
    
    
    return shaderParameters.at(key);
}

SpecificShaderParameters RenderComponent::GetShaderParametersForShader(
                                                IShader::ShaderType shaderType)
{
    auto findIt = m_shaderParameters.find(shaderType);
    
    if(findIt==m_shaderParameters.end())
    {
        throw new KeyNotFoundException("Shader type wasnt found");
    }
    
    return m_shaderParameters.at(shaderType);
}

ShaderParameters RenderComponent::GetAllShaderParameters()
{
    return m_shaderParameters;
}