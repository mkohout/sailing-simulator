#pragma once

#include <string>

using std::string;

namespace Game_View
{
	/**
	 * Interface for different kinds of shader: GLSL, HLSL
	 */
	class IShader
	{
	public:
		/**
		 * All available kinds of shaders in the engine
		 */
		enum ShaderType
		{
            NO_SHADER,
			VERTEX_SHADER,
			GEOMETRY_SHADER,
			FRAGMENT_SHADER,
			COMPUTE_SHADER,
            TESSELLATION_CONTROL_SHADER,
			TESSELLATION_EVAL_SHADER,
		};

		/**
		 * Load shader with source code from string
		 */
		virtual void VLoadFromString(ShaderType, const string&) = 0;

		/**
		 * Load shader source from file
		 */
		virtual void VLoadFromFile(ShaderType, const string&) = 0;

		/**
		 * Returns the identifier of the represented shader (e.g. "default")
		 */
		virtual const string VGetIdentifier() const = 0;

		/**
		 * Uses the current shader by attaching it to the GLProgram/Graphicscard/Whatsoever
		 */
		virtual void VUse() = 0;
	};
}