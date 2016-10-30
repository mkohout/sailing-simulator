#pragma once

#include <memory>

#include "IRenderingStep.h"
#include "../rendering/Material.h"
#include "../rendering/Geometry.h"
#include "../video_system/GLProgram.h"
#include "../../game_logic/actor_components/RenderComponent.h"
#include <fastdelegate/FastDelegate.h>

using std::shared_ptr;
using std::weak_ptr;

namespace Game_View
{
    class Scene;
    class SceneGraph;
    class ISceneNode;
    class OpenGLRenderer;
    
    typedef fastdelegate::FastDelegate2<shared_ptr<ISceneNode>, unsigned int> RenderFunction;
    typedef map<string, GLuint> ActorTextures;
    typedef map<ActorId, ActorTextures> LoadedTextures;
    
    static GLuint g_CubeTexId = std::numeric_limits<GLuint>::max(); //TODO, sollte anders
    
    /**
     * Abstract class that provides the functionality the steps
     * in the rendering need to fulfil their task
     */
    class RenderingStep : public IRenderingStep
    {
    private:
        /**
         * Map of the textures that are loaded in the current step
         */
        LoadedTextures m_loadedTextures;
        
        /**
         * Interacts with the renderer and loads the shader that
         * were specified in the given render component.
         * According to the shaders used in the render component,
         * the OpenGLRenderer will provide the proper GLProgram
         */
        void AssignShaders(RenderComponent*);
        
        /* Render specific data */
        
        /**
         * Pass the material properties to the shader
         */
        void AssignMaterials(GLProgram*, Materials);
        
        /**
         * Assigns the uniforms to the shader, especially the transformation matrices.
         */
        void AssignMatrices(GLProgram*, shared_ptr<ISceneNode>);
        
        /**
         * Provides the camera position to the shaders
         */
        void AssignCamera(GLProgram*);
        
        /**
         * Provides the light sources to the shaders
         */
        void AssignLights(GLProgram*);
        
        /**
         * Provides the shader parameters that were set in the XML
         * of the currently rendered actor
         */
        void AssignShaderParameters(GLProgram*, RenderComponent*);
        
        /**
         * Provides a specific shader parameter from the list of the 
         * AssignShaderParameters method
         */
        void AssignShaderParameter(GLProgram*, const string&, float);
        
        /**
         * Provides the textures to the graphics card the actor has
         * specified in the render component
         */
        void AssignTextures(StrongActorPtr, RenderComponent*);
        
        /**
         * Treats the given texture as a cubemap (e.g. skybox needs a cubemap)
         */
        void AssignCubemapTexture(
                                  GLuint, StrongTexturePtr, int);
        
        /**
         * Returns true if the given texture has already been loaded
         * for the given actor
         */
        bool TextureAlreadyLoaded(
                                  StrongActorPtr, StrongTexturePtr, GLuint&);
        
        /**
         * Adds an entry to the m_loadedTextures to avoid reassigning the same
         * texture to OpenGL over and over again
         */
        void TextureLoadedForActor(
                                   StrongActorPtr, StrongTexturePtr, GLuint);
		        
    protected:
        OpenGLRenderer* m_pRenderer;
        
        /**
         * Treats the given texture as a regular texture
         */
        void AssignTexture(GLuint, StrongTexturePtr);
        
        /**
         * Assigns the given geometry
         */
        void AssignGeometry(StrongGeometryPtr);
        
        /**
         * Assignments that were all needed prior to the actual rendering
         * such as providing the camera details, light sources, matrices, etc.
         */
        void PreRenderAssignments(
                        shared_ptr<ISceneNode>);
        
        /**
         * Function that will execute the given RenderFunction on all
         * SceneNodes in the given Scene Graph
         */
        void ExecuteOnAllSceneNodes(
                    SceneGraph*, const unsigned int, RenderFunction);
        
        /**
         * Executes the given RenderFunction on the given SceneNode
         */
        void ExecuteOnSceneNode(
                    shared_ptr<ISceneNode>, const unsigned int, RenderFunction);


		void AssignWaterTessellation(shared_ptr<ISceneNode> pSceneNode, GLProgram* pCurrentProgram, 
									 const glm::mat4& viewMat, const glm::mat4& projMat, const vec3& viewPos, const vec3& lookingDir, float triScale = 1);

		void AssignGLTex(GLProgram* pCurrentProgram, const std::string& name, GLuint tex, int slot);
        
    public:
        RenderingStep() = default;
        virtual ~RenderingStep() {
        }
        
        /**
         * Abstract method that will be executed before the rendering step
         * will be executed the first time. Initialize resources etc.
         */
        virtual void VInit() = 0;
        
        /**
         * Sets the renderer to easily access general features and properties of
         * the renderer
         */
        virtual void VSetRenderer(OpenGLRenderer* pRenderer)
        {
            m_pRenderer = pRenderer;
        }

        virtual void VExecute(SceneGraph*, const unsigned int) = 0;
        
        /**
         * TODO
         */
        virtual void VSetCanvasSize(int canvasWidth, int canvasHeight);
        
        /**
         * TODO
         */
        virtual void VActivateBackbuffer();

    };
}