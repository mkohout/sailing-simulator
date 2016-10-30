#pragma once

#include <memory>

#include "RenderingStep.h"

#include "../../game_logic/actor_components/ActorComponent.h"
#include "../scene_graph/SceneNode.h"
#include "ShadowStep.h"
#include "Geometry.h"

using std::shared_ptr;

namespace Game_View
{
    class Scene;
    
    /**
     * Rendering step that renders the geometry and the results of the
     * previous steps to the screen
     */
    class RenderToScreenStep : public RenderingStep
    {
    private:
        /**
         * Draws the given geometry
         */
        void DrawGeometry(StrongGeometryPtr);
        
        /**
         * Renders the given scene node to the screen
         */     
        void RenderSceneNode(StrongSceneNodePtr, const unsigned int);
        
        /**
         * Renders the given scene node as a regular scene node,
         * by simply drawing the geometry to the screen
         */
        void RenderRegularSceneNode(
                        StrongSceneNodePtr,
                        const unsigned int,
                        StrongActorComponentPtr);
        
        /**
         * Renders the given scene node as an ocean
         */
        void RenderOceanSceneNode(
                        StrongSceneNodePtr,
                        const unsigned int,
                        StrongActorComponentPtr);

		void FindObjectAABB(StrongSceneNodePtr, const unsigned int);
        
		ShadowStep* m_pShadowMapping;
		GLuint m_framebufferName;
		GLuint m_renderedTexture;
		GLuint m_renderedDepthTexture;
		GLuint m_depthBuffer;
		int m_drawMode;
		int m_SSRWidth, m_SSRHeight;
		AABB m_worldBoxSSR;
		mat4 m_SSRProj, m_SSRView, m_SSRProjView;
		vec3 m_SSRCamPos;
    public:
        RenderToScreenStep(ShadowStep* a_Shadow)
			: m_pShadowMapping(a_Shadow), m_framebufferName(-1)
        {
	        
        }
        
        /**
         * Initializes the render to screen step
         */
		virtual void VInit();
        
        virtual void VExecute(SceneGraph*, const unsigned int);
    };
}