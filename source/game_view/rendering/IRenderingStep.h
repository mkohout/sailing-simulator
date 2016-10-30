#pragma once

#include <memory>
#include "../video_system/GLProgram.h"

using std::shared_ptr;

namespace Game_View
{
    class OpenGLRenderer;
    class SceneGraph;
    
    /**
     * Base class for the different steps of the rendering
     * pipeline. A rendering step needs to implement the
     * VExecute function, that will do the stuff
     */
    class IRenderingStep
    {
	protected:
		int m_canvasWidth, m_canvasHeight;
        
    public:
        /**
         * Functionality that needs to be executed before the rendering step
         * is executed the first time
         */
        virtual void VInit() = 0;
        
        /**
         * Assigns the m_renderer method later on to provide
         * access to renderer data such as lights, camera, etc.
         */
        virtual void VSetRenderer(OpenGLRenderer*) = 0;
        
        /**
         * Method that will be executed by the Renderer
         */
        virtual void VExecute(SceneGraph*, unsigned int) = 0;
        
        /**
         * Sets the size of the canvas the renderer
         * will render to
         */
        virtual void VSetCanvasSize(int, int) = 0;

        /**
         * Sets the target to render the current step to the back buffer
         */
        virtual void VActivateBackbuffer() = 0;
    };
    
    typedef shared_ptr<IRenderingStep> StrongRenderStepPtr;
}