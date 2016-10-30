#include "ResourceManager.h"
#include "ImageResource.h"
#include "ObjResource.h"
#include "SoundResource.h"
#include "CompiledMeshResource.h"
#include "XMLResource.h"
#include "../core/GameCore.h"
#include "../core/stdafx.h"

using namespace Application_Layer;

void ResourceManager::Init()
{
}

ResourceManager::~ResourceManager()
{
}

StrongResourcePtr ResourceManager::GetResource(
	const string& filename,
	IResource::TYPE resourceType)
{
    if(!g_pGameApp->GetFileManager()->FileExists(filename))
    {
        return nullptr;
    }
    
    
	switch (resourceType)
	{
        case IResource::IMAGE:
            return LoadImageResource(filename);
		break;

        case IResource::OBJ:
            return LoadObjResource(filename);
		break;

        case IResource::XML:
            return LoadXMLResource(filename);
		break;
            
        case IResource::SOUND:
            return LoadSoundResource(filename);
            
        default:
            return nullptr;
	}
}

StrongResourcePtr ResourceManager::LoadObjResource(const string& filename)
{
    ObjResource* pObjResource =
    new ObjResource();
    
    string compiledFilename =
    g_pGameApp->GetFileManager()->GetPathToCompiledMeshFile(
                                                            g_pGameApp->GetFileManager()->GetBaseName(filename));
    
    // Compiled version available
    if(g_pGameApp->GetFileManager()->FileExists(compiledFilename) && CompiledMeshResource::CanParseFile(compiledFilename))
    {
        CompiledMeshResource* pCompiledMeshResource =
                                        new CompiledMeshResource();
        
        pCompiledMeshResource->VInitFromFile(compiledFilename);
        
        pObjResource->LoadFromCompiledResource(pCompiledMeshResource);
        
        delete pCompiledMeshResource;
        
        return StrongResourcePtr(pObjResource);
    }
    else
    {
        pObjResource->VInitFromFile(filename);
        
        pObjResource->Compile(compiledFilename);
    }
    
    StrongResourcePtr pOutput =
    StrongResourcePtr(pObjResource);
    
    return pOutput;
}

StrongResourcePtr ResourceManager::LoadImageResource(
                                                     const string& filename)
{
    StrongResourcePtr pImageResource =
    StrongResourcePtr(new ImageResource());
    
    pImageResource->VInitFromFile(filename);
    
    return pImageResource;
}

StrongResourcePtr ResourceManager::LoadSoundResource(const string& filename)
{
    SoundResource* pSoundResource =
                new SoundResource();

    string compiledFilename =
            g_pGameApp->GetFileManager()->GetPathToCompiledMeshFile(
                g_pGameApp->GetFileManager()->GetBaseName(filename));
    
    pSoundResource->VInitFromFile(filename);
    
    StrongResourcePtr pOutput =
                StrongResourcePtr(pSoundResource);
    
    return pOutput;
}

StrongResourcePtr ResourceManager::LoadXMLResource(const string& filename)
{

    StrongResourcePtr pXmlResource =
                    StrongResourcePtr(new XMLResource());
    
    pXmlResource->VInitFromFile(filename);
    
    return pXmlResource;
}