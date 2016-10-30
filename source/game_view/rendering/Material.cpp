#include "../../game_logic/game_app/GameApp.h"
#include "../../application_layer/core/GameCore.h"
#include "Material.h"

using namespace Game_View;

void Material::InitByResource(XMLResource* pXmlResource)
{
	if (pXmlResource->GetRootElement() == nullptr)
	{
		return;
	}

	shared_ptr<XMLElement> pRootElement =
		pXmlResource->GetRootElement();

	InitByXMLElement(
		pRootElement->FindFirstChildNamed("material"));
}

void Material::InitTexturesByXML(StrongXMLElementPtr pXmlElement)
{
    for(StrongXMLElementPtr pTextureElement : pXmlElement->GetChildren())
    {
        StrongTexturePtr pTexture = StrongTexturePtr(new Texture());
        
        //TODO, initializen
        
        m_textures.push_back(pTexture);
    }
}

void Material::InitByXMLElement(StrongXMLElementPtr pXmlElement)
{
	if (pXmlElement == nullptr)
	{
		return;
	}
    
    if(pXmlElement->HasChildrenNamed("textures"))
    {
        InitTexturesByXML(pXmlElement->FindFirstChildNamed("textures"));
    }

	if (pXmlElement->HasChildrenNamed("ambientColor"))
	{
		StrongXMLElementPtr pAmbientColorElement =
			pXmlElement->FindFirstChildNamed("ambientColor");

		SetAmbientColor(vec3(
			std::stof(pAmbientColorElement->GetValueOfAttribute("r")),
			std::stof(pAmbientColorElement->GetValueOfAttribute("g")),
			std::stof(pAmbientColorElement->GetValueOfAttribute("b"))
			));
	}

	if (pXmlElement->HasChildrenNamed("diffuseColor"))
	{
		StrongXMLElementPtr pDiffuseColorElement =
			pXmlElement->FindFirstChildNamed("diffuseColor");

		SetDiffuseColor(vec3(
			std::stof(pDiffuseColorElement->GetValueOfAttribute("r")),
			std::stof(pDiffuseColorElement->GetValueOfAttribute("g")),
			std::stof(pDiffuseColorElement->GetValueOfAttribute("b"))
			));
	}

	if (pXmlElement->HasChildrenNamed("specularColor"))
	{
		StrongXMLElementPtr pSpecularColorElement =
			pXmlElement->FindFirstChildNamed("specularColor");

		SetSpecularColor(vec3(
			std::stof(pSpecularColorElement->GetValueOfAttribute("r")),
			std::stof(pSpecularColorElement->GetValueOfAttribute("g")),
			std::stof(pSpecularColorElement->GetValueOfAttribute("b"))
			));
	}

	if (pXmlElement->HasChildrenNamed("specularExponent"))
	{
		StrongXMLElementPtr pSpecularExponentElement =
			pXmlElement->FindFirstChildNamed("specularExponent");

		SetSpecularExponent(
			std::stof(
				pSpecularExponentElement->GetValueOfAttribute("value")));
	}

	if (pXmlElement->HasChildrenNamed("transparency"))
	{
		StrongXMLElementPtr pSpecularExponentElement =
			pXmlElement->FindFirstChildNamed("transparency");

		SetTransparency(
			std::stof(
				pSpecularExponentElement->GetValueOfAttribute("value")));
	}
}

void Material::LoadDefault()
{
    const string materialFile =
                    g_pGameApp->GetGameSettings()->GetValue("defaultMaterial") + ".xml";
    const string specFile =
                    g_pGameApp->GetFileManager()->GetPathToMaterialFile(materialFile);
    
    StrongResourcePtr pMaterialResource =
                    g_pGameApp->GetResourceManager()->GetResource(
                            specFile,
                            IResource::XML
                            );
    
    if(pMaterialResource==nullptr)
    {
        const string errorMessage = "Cannot load default material";
        
        g_pGameApp->LogString(Logger::ERROR, errorMessage);
        
        throw errorMessage;
        return ;
    }
    
    XMLResource* pXmlResource =
                dynamic_cast<XMLResource*>(pMaterialResource.get());
    
    InitByResource(pXmlResource);
    SetIdentifier("default");
}

void Material::LoadFromFile(std::string materialName)
{
    const string materialFilename = materialName + ".xml";
    std::string bla = g_pGameApp->GetFileManager()->GetPathToMaterialFile(materialFilename);
    
    StrongResourcePtr pMaterialResource =
                g_pGameApp->GetResourceManager()->GetResource(
                                                  g_pGameApp->GetFileManager()->GetPathToMaterialFile(materialFilename),
                                                  IResource::XML
                                                  );
    
    if(pMaterialResource==nullptr)
    {
        LoadDefault();
    }
    else
    {
        XMLResource* pXmlResource =
                    dynamic_cast<XMLResource*>(pMaterialResource.get());
        
        InitByResource(pXmlResource);
        SetIdentifier(materialName);
    }
}