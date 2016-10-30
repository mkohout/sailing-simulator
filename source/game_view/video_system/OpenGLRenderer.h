#pragma once

#include <string>
#include <map>
#include <cmath>
#include <vector>
#include <memory>

#include "../../application_layer/core/stdafx.h"

#include "../rendering/Geometry.h"
#include "../rendering/Material.h"
#include "IShader.h"
#include "GLSLShader.h"
#include "GLProgram.h"

#include "../rendering/IRenderingStep.h"

#include "../scene_graph/ISceneNode.h"
#include "../scene_graph/LightNode.h"
#include "../scene_graph/CameraNode.h"

#include <glew/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

using std::string;
using std::map;
using std::pair;
using std::shared_ptr;

using namespace glm;

namespace Game_View
{
	typedef string ShaderHash;
	typedef map<ShaderHash, GLProgram*> ProgramList;
	typedef map<string, GLSLShader*> ShaderList;
    typedef vector<shared_ptr<IRenderingStep>> RenderingSteps;
    typedef vector<LightNode*> Lights;

	/**
	 * Rendering using OpenGL, this renderer class will render a given SceneNode
	 */
	class OpenGLRenderer
	{
	private:
		/**
		 * Width and height of the canvas where the renderer will output its result
		 */
		int m_canvasWidth, m_canvasHeight;

		/**
		 * The list of the used programs during the rendering process
		 */
		ProgramList m_programs;
        
        /**
         * Vector that holds the rendering steps in the chronilogical order.
         * These steps will be executed on the scene graph every rendering frame
         */
        RenderingSteps m_renderingSteps;

		/**
		 * The lights attached to the scene, which is currently rendered
		 */
		Lights m_lights;

		/**
		 * The active camera the renderer is using for rendering
		 */
		CameraNode* m_pCurrentCamera;

		/**
		 * The camera view frustum, the projection matrix
		 */
		mat4x4 m_projectionMatrix;

		/**
		 * The cache for the previous used shaders - so compiling them again is unnecessary
		 */
		ShaderList m_usedShaders;
        
		/**
		 * Returns the filename of the shader by the given combination
		 */
		const string GetFullShaderName(IShader::ShaderType, string);

		/**
		 * Stores the program in a map for the next frame
		 */
		void SaveProgram(
            GLProgram*, const std::map<IShader::ShaderType, string> &);

		/* Rendering Properties */

		/**
		 * Enables/disables backface culling
		 */
		void BackFaceCulling(GLboolean);

		/**
		 * Enables/disables z-buffer
		 */
		void DepthTest(GLboolean);
        
        /**
         * Initializes the vector with the rendering steps when the
         * OpenGLRenderer is initialized
         */
        void InitializeRenderingSteps();
        
        /**
         * Creates a hash (see type definition of ShaderHash
         * to find easily the program that already uses the
         * given combination of shaders to avoid creating too
         * many GL programs during runtime
         */
        ShaderHash GetShaderListHash(const std::map<IShader::ShaderType, string> &);

	public:
		OpenGLRenderer() : m_programs(), m_usedShaders()
		{
			m_pCurrentCamera = nullptr;
		}

        ~OpenGLRenderer();

        void Init(const int, const int);

        void SetCanvasSize(const int, const int);
        
        /**
         * Returns the aspect ratio of the canvas (usually a window)
         * the Renderer is rendering on
         */
        const double GetCanvasAspectRatio() const
        {
            return (double)m_canvasWidth/m_canvasHeight;
        }

        /**
         * Starts the rendering by executing the rendering steps on the
         * given scene graph
         */
        void StartRendering(unsigned int, SceneGraph*);
        
        /**
         * Returns the gl program that has the given shader combination
         */
        GLProgram* GetProgram(map<IShader::ShaderType, string>);
        
        /**
         * Returns the shader -
         * will store them in a map to retrieve them more quickly in the next frame
         */
        GLSLShader* GetShader(IShader::ShaderType, const string&);
        
        /**
         * Adds a shader to the list of used shaders to avoid reloading the same
         * shaders multiple times
         */
        GLSLShader* AddShader(IShader::ShaderType, const string&);
        
        /**
         * Returns true if a camera has been assigned to the renderer
         */
        bool HasCamera() const
        {
            return m_pCurrentCamera!=nullptr;
        }
        
        /**
         * Returns the current camera in the renderer
         */
        const CameraNode* GetCamera() const
        {
            return m_pCurrentCamera;
        }

		/**
		 * Returns the prefix for the given shader type, like vsh for a
         * vertex shader to distinguish the file names
		 */
        const string GetShaderPrefix(IShader::ShaderType);

		/**
		 * Returns the file type of the glsl shader (in this case)
		 */
        const string GetShaderType(IShader::ShaderType);

		/**
		 * Set the given camera as the current camera for rendering
		 */
        void SetCamera(CameraNode*);

		/**
		 * Adds light to the image that will be rendered
		 */
        void AddLight(LightNode* pLightNode);
        
        /**
         * Returns the light sources that were added to the OpenGLRenderer
         */
        const Lights GetLights() const
        {
            return m_lights;
        }
        
        /**
         * Resets the shaders that were saved in the Renderer
         */
        void ResetShaders();
	};
}