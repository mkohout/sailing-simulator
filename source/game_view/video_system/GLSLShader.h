#pragma once

#include <string>

#include "IShader.h"
#include "GLProgram.h"

#include <glew/glew.h>
#include <glfw/glfw3.h>

using std::string;

namespace Game_View
{
	class GLProgram;

	/**
	 * GLSL Shader class for supplying glsl based shader
	 */
	class GLSLShader : public IShader
	{
	private:
		/**
		 * The identifier of the current shader, e.g. ("default")
		 */
		string m_identifier;

		/**
		 * Internal OpenGL representation of a shader
		 */
		GLuint m_shader;

		/**
		 * Pointer to the program handle of the OpenGL instance
		 */
		GLProgram* m_program;

		/**
		 * Creates the GLSL shader object according to the requested type of shader
		 */
		void CreateShader(ShaderType);

		/**
		 * Sets the identifier of the shader that is usually the basename of
         * the shader file, like vsh_default.glsl
		 */
		void SetIdentifier(string identifier)
		{
			m_identifier = identifier;
		}

		/**
		 * Will compile the held shader object
		 */
		void Compile();

	public:
		GLSLShader() = default;

		virtual ~GLSLShader() { }

		/**
		 * Takes the current gl program as parameter
		 * and set it as the valid program for current shader
		 */
		void SetGLProgram(GLProgram*);

		/**
		 * Loads the source from given string
		 */
		virtual void VLoadFromString(ShaderType, const string&);

		/**
		 * Loads the source from the requested file
		 */
		virtual void VLoadFromFile(ShaderType, const string&);

		/**
		 * Returns the identifier of the current shader
		 */
		virtual const string VGetIdentifier() const
		{
			return m_identifier;
		}

		/**
		 * Applies the current shader to the previous given glProgram
		 */
		virtual void VUse();
	};

}