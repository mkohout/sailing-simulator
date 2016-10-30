#include <boost/algorithm/string.hpp>

#include "ObjResource.h"
#include "../../game_view/rendering/VertexData.h"
#include "../core/GameCore.h"
#include <string>

using namespace Application_Layer;

using std::ifstream;

ObjResource::~ObjResource()
{
    // don't delete geometry or material.
    // Geometry will be used by actor
}

void ObjResource::VDispose()
{
}

void ObjResource::Compile(
                    const string& compiledFilename)
{
    CompiledMeshResource cmpMesh(m_pGeometry);

    cmpMesh.WriteToFile(compiledFilename);
}

void ObjResource::LoadFromCompiledResource(
                    CompiledMeshResource* pCompiledResource)
{
     m_pGeometry =
            StrongGeometryPtr(pCompiledResource->GetCreatedGeometry());
}

void ObjResource::VInitFromFile(
	const string& filename)
{
	string line;
	ifstream selectedFile(filename);

	if (selectedFile.is_open())
	{
		while (getline(selectedFile, line))
		{
			HandleLine(line);
		}
	}
    
    AssignMaterialsToGeometry();
}

void ObjResource::AssignMaterialsToGeometry()
{
    m_pGeometry->AssignMaterials(m_materialList);
}

void ObjResource::HandleLine(const string& _line)
{
    vector<string> elements;
    
	if (elements.size() == 0)
    {
		elements.reserve(128);
    }
    
    std::string line = _line;
	boost::trim(line);
	boost::split(elements, line, boost::is_any_of(" "), boost::token_compress_on);

	if (elements.size() == 0)
	{
		return;
	}

	const LINE_TYPE lineType = GetLineType(elements[0]);

	switch (lineType)
	{
	case VERTEX:
            HandleVertexLine(elements);
		break;

    case MATERIAL:
        HandleMaterialLine(elements);
        break;
            
	case NORMAL:
            HandleNormalLine(elements);
		break;

	case TEXTURE_COORDINATE:
            HandleUVLine(elements);
		break;

	case FACE:
            HandleFaceLine(elements);
		break;
	case UNKNOWN:
		break;
	}
}

const ObjResource::LINE_TYPE ObjResource::GetLineType(
	const string& linePrefix)
{
	if (linePrefix == "v")
	{
		return VERTEX;
	}
	else if (linePrefix == "f")
	{
		return FACE;
	}
	else if (linePrefix == "vt")
	{
		return TEXTURE_COORDINATE;
	}
	else if (linePrefix == "vn")
	{
		return NORMAL;
	}
    else if(linePrefix == "usemtl")
    {
        return MATERIAL;
    }

	return UNKNOWN;
}

void ObjResource::HandleVertexLine(
	const vector<string>& lineElements)
{
    AddVertex(
		lineElements[1], lineElements[2], lineElements[3]);
}

void ObjResource::HandleNormalLine(
	const vector<string>& lineElements)
{
    AddNormal(
		lineElements[1], lineElements[2], lineElements[3]);
}

void ObjResource::HandleUVLine(
	const vector<string>& lineElements)
{
    AddUV(
		lineElements[1], lineElements[2]);
}

void ObjResource::HandleMaterialLine(const vector<string>& lineElements)
{
    if(!MaterialLoaded(lineElements[1]))
    {
        LoadMaterial(lineElements[1]);
    }
    
    SetCurrentMaterial(lineElements[1]);
}

const float ObjResource::GetCurrentMaterialIndex()
{
    return m_currentMaterialIndex;
}

bool ObjResource::MaterialLoaded(const string& materialName)
{
    for(Material& material : m_materialList)
    {
        if(material.GetIdentifier().compare(materialName)==0)
        {
            return true;
        }
    }
    
    return false;
}

void ObjResource::LoadMaterial(const string& materialName)
{
    Material material;
    
    material.LoadFromFile(materialName);
    
    m_materialList.push_back(material);
}

void ObjResource::SetCurrentMaterial(const string& materialName)
{
    short i = 0;
    
    for(Material& material : m_materialList)
    {
        if(material.GetIdentifier().compare(materialName)==0)
        {
            m_currentMaterialIndex = i;
            return ;
        }
        
        i++;
    }
}

void ObjResource::HandleFaceLine(
	const vector<string>& lineElements)
{
	for (int i = 1; i <= 3; ++i)//we can only hande triangles!
	{
        vector<string> faceElements;
        boost::split(
                     faceElements, lineElements[i], boost::is_any_of("/"));

        const unsigned int vertexIndex = std::stoi(faceElements[0])-1;
        
        
        m_pGeometry->AddIndex(vertexIndex);
        
        if(m_pGeometry->HasVertexDataForPosition(vertexIndex))
        {
            continue;
        }
        
        VertexData vertex;
        
        vertex.Position = GetVertexByIndex(vertexIndex);

		if (faceElements[1].compare("") != 0)
		{
			vertex.UV = GetUVByIndex(std::stoi(faceElements[1])-1);
		} else
        {
            vertex.UV = glm::vec2(-1.0f, -1.0f);
        }

        vertex.Normal = GetNormalByIndex(std::stoi(faceElements[2])-1);
        vertex.MaterialIndex = GetCurrentMaterialIndex();

        m_pGeometry->AddVertexData(vertexIndex, vertex);
	}
}

void ObjResource::AddVertex(const string& sX, const string& sY, const string& sZ)
{
    const float x = std::stof(sX);
    const float y = std::stof(sY);
    const float z = std::stof(sZ);
    
    AddVertex(x, y, z);
}

void ObjResource::AddVertex(const float& x, const float& y, const float& z)
{
    m_vertices.push_back(vec3(x, y, z));
}

const Vertex ObjResource::GetVertexByIndex(const unsigned int index) const
{
    return m_vertices.at(index);
}

void ObjResource::AddNormal(const string& sX, const string& sY, const string& sZ)
{
    const float x = std::stof(sX);
    const float y = std::stof(sY);
    const float z = std::stof(sZ);
    
    AddNormal(x, y, z);
}

void ObjResource::AddNormal(const float& x, const float& y, const float& z)
{
    m_normals.push_back(vec3(x, y, z));
}

const vec3 ObjResource::GetNormalByIndex(const unsigned int index) const
{
    return m_normals.at(index);
}

void ObjResource::AddUV(const string& sU, const string& sV)
{
    const float u = std::stof(sU);
    const float v = std::stof(sV);
    
    AddUV(u, v);
}

void ObjResource::AddUV(const float& u, const float& v)
{
    m_uvs.push_back(vec2(u, v));
}

const vec2 ObjResource::GetUVByIndex(const unsigned int index) const
{
    return m_uvs.at(index);
}