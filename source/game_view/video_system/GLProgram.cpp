#include "GLProgram.h"

#include "../../application_layer/core/stdafx.h"
#include "../../application_layer/core/GameCore.h"
#include <exception>

using namespace Game_View;

GLProgram::~GLProgram()
{
	SAFE_DELETE(m_programHandle);
}

void GLProgram::Init()
{
	m_programHandle = new GLuint();

	*m_programHandle = glCreateProgram();

	if (*m_programHandle == 0)
	{
		g_pGameApp->LogString(
			Application_Layer::Logger::ERROR,
			"GL Program did not compile"
			);
        
        throw "GL Program did not compile";
	}
}

void GLProgram::AddShader(IShader* pShader)
{
	GLSLShader* pGLShader = (GLSLShader*)pShader;

	pGLShader->SetGLProgram(this);
	pGLShader->VUse();
}

void GLProgram::Link()
{
	GLint linkSuccess = GL_FALSE;

	glLinkProgram(*GetProgramHandle());

	glGetProgramiv(
                   *GetProgramHandle(),
                   GL_LINK_STATUS,
                   &linkSuccess);

	if (linkSuccess == GL_FALSE)
	{
		GLchar errorLog[1024] = { 0 };
		glGetProgramInfoLog(
                            *GetProgramHandle(),
                            1024,
                            NULL,
                            errorLog);
        
        g_pGameApp->LogString(Logger::ERROR, errorLog);

		g_pGameApp->LogString(
			Application_Layer::Logger::ERROR,
			"GL Program wasn't linked correctly"
			);
        
        throw "GL Program wasn't linked correctly";
	}
}

void GLProgram::Use()
{
    glUseProgram(*GetProgramHandle());
}

void GLProgram::AddUniform(const string& uniformName, vec2 vec)
{
	auto i = GetUniform(uniformName);
	if (i != UINT_MAX)
		glUniform2f(i, vec.x, vec.y);
}

void GLProgram::AddUniform(const string& uniformName, vec3 vec)
{
	auto i = GetUniform(uniformName);
	if (i != UINT_MAX)
		glUniform3f(i, vec.x, vec.y, vec.z);
}

void GLProgram::AddUniform(
	const string& uniformName, vec4 vec)
{
	AddUniform(uniformName, vec.x, vec.y, vec.z, vec.w);
}

void GLProgram::AddUniform(
	const string& uniformName, float x, float y, float z, float w)
{
	auto i = GetUniform(uniformName);
	if (i != UINT_MAX)
		glUniform4f(i, x, y, z, w);
}

void GLProgram::AddUniform(
	const string& uniformName, int value)
{
	auto i = GetUniform(uniformName);
	if (i != UINT_MAX)
		glUniform1i(i, value);
}

void GLProgram::AddUniform(
	const string& uniformName, bool value)
{
	auto i = GetUniform(uniformName);
	if (i != UINT_MAX)
		glUniform1i(i, (value == true ? 1 : 0));
}

void GLProgram::AddUniform(
	const string& uniformName, unsigned int value)
{
	auto i = GetUniform(uniformName);
	if (i != UINT_MAX)
		glUniform1ui(i, value);
}

void GLProgram::AddUniform(
	const string& uniformName, float value)
{
	auto i = GetUniform(uniformName);
	if (i != UINT_MAX)
		glUniform1f(i, value);
}

void GLProgram::AddUniform(
	const string& uniformName, mat4 matrix)
{
	auto i = GetUniform(uniformName);
	if(i != UINT_MAX)
		glUniformMatrix4fv(i, 1, GL_FALSE, &matrix[0][0]);
}

void GLProgram::AddUniform(
	const string& uniformName, mat3 matrix)
{
	auto i = GetUniform(uniformName);
	if (i != UINT_MAX)
		glUniformMatrix3fv(i, 1, GL_FALSE, &matrix[0][0]);
}

void GLProgram::AddAttribute(
            const string& attributeName, int value)
{
    auto i = GetAttribute(attributeName);
    
    if(i != UINT_MAX)
    {
        glVertexAttribI1i(i, value);
    }
}

const GLuint GLProgram::GetUniform(
	const string& uniformName)
{
	auto it = m_uniformLocations.find(uniformName);
	if(it == m_uniformLocations.end())
	{
		auto val = glGetUniformLocation(*GetProgramHandle(), uniformName.c_str());
		m_uniformLocations[uniformName] = val;
		return val;
	}
	else return it->second;
}

const GLuint GLProgram::GetAttribute(
                    const string& attributeName)
{
    auto it = m_attributeLocations.find(attributeName);
    
    if(it == m_attributeLocations.end())
    {
        auto val =
                glGetAttribLocation(
                                    *GetProgramHandle(),
                                    attributeName.c_str());
        m_attributeLocations[attributeName] = val;
        return val;
    }
    else return it->second;
}