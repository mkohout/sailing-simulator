#include "../../application_layer/core/stdafx.h"
#include "../../game_logic/game_app/GameApp.h"
#include "../../application_layer/core/GameCore.h"
#include "GLSLShader.h"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>

using namespace Game_View;
using namespace Application_Layer;

using std::ifstream;
using std::stringstream;

void GLSLShader::Compile()
{
	glCompileShader(m_shader);

	GLint maxLength = 0;
	glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &maxLength);
	if(maxLength > 1)
	{
		auto msg = std::string(maxLength + 2, 0);
		glGetShaderInfoLog(m_shader, maxLength, 0, &msg[0]);
		g_pGameApp->LogString(
			Application_Layer::Logger::WARNING,
			msg.c_str()
			);
	}

	GLint isCompiled = 0;
	glGetShaderiv(m_shader, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
        string errorMessage = "Error during compiling a shader: " + m_identifier;

		g_pGameApp->LogString(
			Application_Layer::Logger::ERROR,
            errorMessage
			);

		glDeleteShader(m_shader);
		return;
	}
}

void GLSLShader::SetGLProgram(GLProgram *pGlProgram)
{
	m_program = pGlProgram;
}

void GLSLShader::CreateShader(ShaderType whichShader)
{
	switch (whichShader)
	{
        case VERTEX_SHADER:
            m_shader = glCreateShader(GL_VERTEX_SHADER);
            break;

        case GEOMETRY_SHADER:
            m_shader = glCreateShader(GL_GEOMETRY_SHADER);
            break;
            
        case TESSELLATION_CONTROL_SHADER:
            m_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
            break;

		case TESSELLATION_EVAL_SHADER:
			m_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
            break;

        case FRAGMENT_SHADER:
            m_shader = glCreateShader(GL_FRAGMENT_SHADER);
            break;

        case COMPUTE_SHADER:
            m_shader = glCreateShader(GL_COMPUTE_SHADER);
            break;
            
        case NO_SHADER:
            break;
	}
}

void GLSLShader::VLoadFromString(ShaderType whichShader, const string& source)
{
	const GLchar* shaderSource = source.c_str();

	CreateShader(whichShader);

	glShaderSource(m_shader, 1, &shaderSource, NULL);

	Compile();
}

void GLSLShader::VLoadFromFile(ShaderType whichShader, const string& filename)
{
    boost::filesystem::path path(filename);
	string source;
	ifstream handle(filename);
	stringstream buffer;

	buffer << handle.rdbuf();

	source = buffer.str();

	SetIdentifier(path.filename().string());

	VLoadFromString(whichShader, source);
}

void GLSLShader::VUse()
{
	glAttachShader((*m_program->GetProgramHandle()), m_shader);
}