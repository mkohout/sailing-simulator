#include "CompiledMeshResource.h"
#include <stdio.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "../core/GameCore.h"
#include <iostream>

static unsigned int CURRENT_VERSION = 127u;

CompiledMeshResource::~CompiledMeshResource()
{
	
}

bool CompiledMeshResource::CanParseFile(const string& fileName)
{
	FILE* pFile = fopen(fileName.c_str(), "rb");
	unsigned int fileVersion;
	fread(&fileVersion, sizeof(CURRENT_VERSION), 1, pFile);
	fclose(pFile);
	return fileVersion == CURRENT_VERSION;
}

template<typename T> void write(const std::vector<T>* vec, FILE * pFile)
{
	size_t l = vec->size();
	fwrite(&l, sizeof(l), 1, pFile);
	if(l)
		fwrite(&vec->operator[](0), sizeof(T), l, pFile);
}
void CompiledMeshResource::WriteToFile(const string& fileName) const
{
	//ensure that the compiled meshes folder exists
	boost::filesystem::path dir = boost::filesystem::path(fileName).parent_path();
	if (!boost::filesystem::exists(dir))
		boost::filesystem::create_directory(dir);

	const vector<VertexData>* pVertexData;
	const vector<unsigned int>* pIndices;
    const Materials* pMaterials;
    
	m_pGeometry->GetSerializationData(pVertexData, pIndices, pMaterials);
	FILE* pFile = fopen(fileName.c_str(), "wb");
	fwrite(&CURRENT_VERSION, sizeof(CURRENT_VERSION), 1, pFile);
	auto box = m_pGeometry->GetLocalAABB();
	fwrite(&box, sizeof(box), 1, pFile);
	write(pVertexData, pFile);
	write(pIndices, pFile);
    
    //TODO
    size_t length = pMaterials->size();
    fwrite(&length, sizeof(size_t), 1, pFile);
    for(Material a : *pMaterials)
    {
        fwrite(a.GetIdentifier().c_str(), 255, 1, pFile);
    }
    
	fclose(pFile);
}

template<typename T> void read(std::vector<T>& vec, FILE* pFile)
{
	size_t l;
	fread(&l, sizeof(l), 1, pFile);
	vec.resize(l);
	if(l)
		fread(&vec[0], sizeof(T), l, pFile);
}
void CompiledMeshResource::VInitFromFile(const string& fileName)
{
    vector<VertexData> vertexData;
	vector<unsigned int> indices;
    Materials materials;
    
	FILE* pFile = fopen(fileName.c_str(), "rb");
	unsigned int fileVersion;
	fread(&fileVersion, sizeof(CURRENT_VERSION), 1, pFile);
	if(CURRENT_VERSION != fileVersion)
	{
		std::cout << "CURRENT_VERSION = " << CURRENT_VERSION << ", but version in compiled mesh file was = " << fileVersion << "\n";
		std::cout << "Please clear your compiled meshes folder!\n";
		throw std::runtime_error("Incorrect compiled file Version. Clear cache!");
	}
	AABB box;
	fread(&box, sizeof(box), 1, pFile);
	read(vertexData, pFile);
	read(indices, pFile);

    size_t length;
    fread(&length, sizeof(size_t), 1, pFile);

    for(int i = 0; i < length; i++)
    {
        Material material;
        char materialName[255];
        
        fread(&materialName[0], sizeof(materialName), 1, pFile);
        
        material.LoadFromFile(std::string(materialName));
        
        materials.push_back(material);
    }

    
	fclose(pFile);
	m_pGeometry = StrongGeometryPtr(new Geometry());
	m_pGeometry->SetSerializationData(vertexData, indices, materials);
	m_pGeometry->SetLocalBox(box);
}

void CompiledMeshResource::VDispose()
{
	
}
