#include "Geometry.h"
#include <iostream>

using namespace Game_View;

StrongGeometryPtr Geometry::VCopy()
{
    StrongGeometryPtr pOutput(new Geometry());
    
    vector<VertexData> vertexData(m_vertexData);
    vector<unsigned int> indices(m_indices);
    Materials materials(m_materials);
    
    pOutput->SetSerializationData(
            vertexData, indices, materials);
    
    return pOutput;
}

vector<VertexData>& Geometry::GetAllVertexData()
{
    if(m_vertexData.size()==0)
    {
        TransferVertexData();
    }
    
    return m_vertexData;
}

void Geometry::TransferVertexData()
{
    for(unsigned int i = 0; i < m_vertexMapping.size(); ++i)
    {
        m_vertexData.push_back(m_vertexMapping.at(i));
    }
    
    m_vertexMapping.clear();
}

void Geometry::GetSerializationData(
                          const vector<VertexData>*& a_vertexData,
                          const vector<unsigned int>*& a_indices,
                          const Materials*& a_materials)
{
    if(m_vertexData.size()==0)
    {
        TransferVertexData();
    }
    
    a_vertexData = &m_vertexData;
    a_indices = &m_indices;
    a_materials = &m_materials;
}

void Geometry::SetSerializationData(
                          const vector<VertexData>& a_vertexData,
                          const vector<unsigned int>& a_indices,
                          const Materials& a_materials)
{
    m_vertexData = a_vertexData;
    m_indices = a_indices;
    m_materials = a_materials;
}

void Geometry::AddVertexData(
        const unsigned int index, VertexData vertexData)
{
    m_vertexMapping.insert(
            pair<unsigned int, VertexData>(index, vertexData));
	m_localBox = m_localBox.Extend(vertexData.Position);
}

void Geometry::AddIndex(const unsigned int index)
{
    m_indices.push_back(index);
}

void Geometry::Enable()
{
	GLuint arrayH, bufferH;
	GetGLBufferInfo(arrayH, bufferH);
	glBindVertexArray(arrayH);
	glBindBuffer(GL_ARRAY_BUFFER, bufferH);

    glEnableVertexAttribArray(LOCATION_VERTEX);
    glEnableVertexAttribArray(LOCATION_NORMAL);
    glEnableVertexAttribArray(LOCATION_UV_COORD);
    glEnableVertexAttribArray(LOCATION_MATERIAL_INDEX);
	if (!m_useBackfaceCulling)
		glDisable(GL_CULL_FACE);
}

void Geometry::Disable() const
{
    glDisableVertexAttribArray(LOCATION_VERTEX);
    glDisableVertexAttribArray(LOCATION_NORMAL);
    glDisableVertexAttribArray(LOCATION_UV_COORD);
    glDisableVertexAttribArray(LOCATION_MATERIAL_INDEX);
	if (!m_useBackfaceCulling)
		glEnable(GL_CULL_FACE);
}

bool Geometry::HasVertexDataForPosition(const unsigned int& index)
{
    auto findIt = m_vertexMapping.find(index);
    
    if(findIt==m_vertexMapping.end())
    {
        return false;
    }
    
    return true;
}

void Geometry::GetGLBufferInfo(GLuint& arrayH, GLuint& bufferH)
{
    bool a = false;
    
    if(m_vertexArrayHandle == std::numeric_limits<GLuint>::max())
    {
        glGenVertexArrays(1, &m_vertexArrayHandle);
        glGenBuffers(1, &m_vertexBufferHandle);
        glGenBuffers(1, &m_elementBuffer);
        
        a = true; //TODO
    }
    
	if (m_isVolatile==true || a==true)
	{
		auto vertices = GetAllVertexData();
        
        glBindVertexArray(m_vertexArrayHandle);
        
		glBindBuffer(
			GL_ARRAY_BUFFER,
			m_vertexBufferHandle
			);
		glBufferData(
			GL_ARRAY_BUFFER,
			vertices.size() * sizeof(VertexData),
			&vertices[0],
			GL_STATIC_DRAW
			);

		// Set up vertex attributes
		// Position
		glVertexAttribPointer(
			LOCATION_VERTEX,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(VertexData),
			nullptr
			);

		// Normal
		glVertexAttribPointer(
			LOCATION_NORMAL,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(VertexData),
			(void*)offsetof(VertexData, Normal)
			);

		// UV
		glVertexAttribPointer(
			LOCATION_UV_COORD,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(VertexData),
			(void*)offsetof(VertexData, UV)
			);
        
        // Material index
        glVertexAttribPointer(
            LOCATION_MATERIAL_INDEX,
            1,
            GL_FLOAT,
            GL_FALSE,
            sizeof(VertexData),
            (void*)offsetof(VertexData, MaterialIndex)
            );
        
        // Indices
        glBindBuffer(
                     GL_ELEMENT_ARRAY_BUFFER,
                     m_elementBuffer
                     );
        glBufferData(
                     GL_ELEMENT_ARRAY_BUFFER,
                     m_indices.size() * sizeof(unsigned int),
                     &m_indices[0],
                     GL_STATIC_DRAW
                     );
	}
	arrayH = m_vertexArrayHandle;
	bufferH = m_vertexBufferHandle;
}

void Geometry::AssignMaterials(Materials materialList)
{
    m_materials = materialList;
}

bool Geometry::HasMaterials() const
{
    return m_materials.size()>0;
}

Materials& Geometry::GetMaterials()
{
    return m_materials;
}
