#pragma once

#include "../../application_layer/core/stdafx.h"

#include "IVideoSystem.h"
#include "GLSLShader.h"
#include "OpenGLRenderer.h"

#include <glew/glew.h>
#include <glfw/glfw3.h>

namespace Game_View
{

	/**
	 * VideoSystem class using the OpenGL API
	 */
	class OpenGLVideoSystem : public IVideoSystem
	{
	private:
		/**
		 * Pointer to the underlying renderer the video system uses
		 */
		OpenGLRenderer* m_pRenderer;

		/**
		 * Pointer to the window object of
		 * the glfw library
		 */
		GLFWwindow* m_pWindow;

		int m_windowWidth;

		int m_windowHeight;

	public:
		OpenGLVideoSystem() = default;
		virtual ~OpenGLVideoSystem();

		/**
		 * Initializes the OpenGL video system
		 */
		virtual void VInit();

		/**
		 * Initialize gaming window to render in
		 */
		virtual void VInitWindow();
        
        /**
         * Sets the window dimensions of the current window
         */
        virtual void VSetWindowDimensions(const int&, const int&);

		/**
		 * Will close the gaming window
		 */
		virtual void VCloseWindow(IEventDataPtr);

		/**
		 * Initializes the renderer for the opengl video system
		 */
		virtual void VInitRenderer();

		/**
		 * Procedures that are going to be executed before every frame
		 */
		virtual void VPreRender();

		/**
		 * Returns a pointer to the currently used renderer
		 */
		virtual OpenGLRenderer* VGetRenderer() const;

		/**
		 * Procedures executed after the rendering
		 */
		virtual void VPostRender();

		/**
		 * Will check for notifications of the operating system
		 */
		virtual bool VCheckForNotifications();

        /**
         * Function that will be executed by the GLFW framework if the
         * window has been resized
         */
        static void ResizeCallback(GLFWwindow*, int, int);
        
		/**
		 * Function that is executed if a key was pressed,
		 * will fire an event for the input event system
		 */
		static void KeyCallback(GLFWwindow*, int, int, int, int);
        
        /**
         * Function that is executed if the user has used the
         * scroll functionality of touchpad/mouse/keyboard
         */
        static void ScrollCallback(GLFWwindow*, double, double);

		/**
		 * Function that is executed if a the was mouse moved,
		 * will fire an event for the input event system
		 */
		static void MousePositionCallback(GLFWwindow*, double, double);

		/**
		 * Function that is executed if a the a mousebutton was clicked,
		 * will fire an event for the input event system
		 */
		static void MouseButtonCallback(GLFWwindow*, int, int, int);
	};

}