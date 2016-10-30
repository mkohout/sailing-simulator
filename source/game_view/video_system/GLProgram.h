#pragma once

#include <string>
#include <map>

#include "GLSLShader.h"

#include <glew/glew.h>
#include <glfw/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

using namespace glm;
using std::string;

namespace Game_View
{

	/**
	 * Class is handling the program handle of OpenGL
	 */
	class GLProgram
	{
	private:
		GLuint* m_programHandle;

		/**
		 * Returns the value for the uniform identified by
         * the given name
		 */
		const GLuint GetUniform(const string&);
        
        /**
         * Returns the attribute location of the given attribute name
         */
        const GLuint GetAttribute(const string&);

        /**
         * Map to access the assigned uniform locations without
         * having to call glUniformLocation every time
         */
		std::map<std::string, GLuint> m_uniformLocations;
        
        /**
         * Map to access the assigned attribute locations without
         * having to call glAttribLocation every time
         */
        std::map<std::string, GLuint> m_attributeLocations;

	public:
		GLProgram() = default;

		~GLProgram();

		void Init();

		void Link();

		/**
		 * Returns the current program handle that
		 * is the result of the given input
		 */
		const GLuint* GetProgramHandle() const
		{
			return m_programHandle;
		}
        
        /**
         * Uses the current OpenGL program for the next rendering,
         * until another programm will be used
         */
        void Use();

		/**
		 * Binds the given shader into the current program
		 */
		void AddShader(IShader*);

		/**
		 * Adds an integer uniform to the current program
		 */
		void AddUniform(const string&, int);

		/**
		 * Adds a boolean uniform to the current program
		 * Because booleans arent supported by GLSL, the function
		 * will assign 1.0f, if true was given, 0.0f otherwise
		 */
		void AddUniform(const string&, bool);

		/**
		 * Adds an unsigned int uniform to the current program
		 */
		void AddUniform(const string&, unsigned int);

		/**
		 * Adds a single float int uniform to the current program
		 */
		void AddUniform(const string&, float);

		/**
		 * Adds a vec4 uniform to the current program by providing
         * the choice of either adding the vec4 by 4 floats or by a
         * vec4 directly
		 */
		void AddUniform(const string&, float, float, float, float);

        void AddUniform(const string&, vec4);

		/**
		* Adds a vec2 uniform to the current program
		*/
		void AddUniform(const string&, vec2);
        
		/**
		 * Adds a vec3 uniform to the current program
		 */
		void AddUniform(const string&, vec3);

		/**
		 * Adds a 3x3 uniform matrix to the current program
		 */
		void AddUniform(const string&, mat3);

		/**
		 * Adds a 4x4 uniform matrix to the current program
		 */
		void AddUniform(const string&, mat4);
        
        /**
         * Adds the given integer as an attribute to the current
         * gl program
         */
        void AddAttribute(const string&, int);
	};

}