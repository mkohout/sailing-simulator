#pragma once

#include "../scene/Scene.h"

namespace Game_View
{
    class OpenGLRenderer;

	/**
	 * Interface class for the video system
	 */
	class IVideoSystem
	{
	public:
		virtual ~IVideoSystem() { }

		/**
		 * Initializes the video system
		 */
		virtual void VInit() = 0;

		/**
		 * Initializes the game window
		 */
		virtual void VInitWindow() = 0;
        
        /**
         * Method that sets the window dimensions if the window has been resized
         */
        virtual void VSetWindowDimensions(const int&, const int&) = 0;

		/**
		 * Will close the game window
		 */
		virtual void VCloseWindow(IEventDataPtr) = 0;

		/**
		 * Initialized the renderer by setting the rendering parameters etc.
		 */
		virtual void VInitRenderer() = 0;

		/**
		 * Procedures that are executed before the actual rendering
		 */
		virtual void VPreRender() = 0;

		/**
		 * Returns a pointer to the used renderer
		 */
		virtual OpenGLRenderer* VGetRenderer() const = 0;

		/**
		 * Procedures that are executed after the rendering of a scene
		 */
		virtual void VPostRender() = 0;

		/**
		 * Function is checking frequently for notifications of the operating system
		 */
		virtual bool VCheckForNotifications() = 0;
	};

}