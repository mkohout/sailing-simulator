#include "OpenGLVideoSystem.h"
#include "../../game_logic/event_system/EvtData_KeyPressed.h"
#include "../../game_logic/event_system/EvtData_MouseClicked.h"
#include "../../game_logic/event_system/EvtData_MouseMoved.h"
#include "../../game_logic/event_system/EvtData_Scrolled.h"

#include "../../application_layer/core/GameCore.h"
#include "../../game_logic/data_structures/Settings.h"

using namespace Game_View;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#include <windows.h>
// enable optimus!
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

OpenGLVideoSystem::~OpenGLVideoSystem()
{
	glfwDestroyWindow(m_pWindow);
	m_pWindow = nullptr;

	glfwTerminate();

	SAFE_DELETE(m_pRenderer);
}

void OpenGLVideoSystem::VInit()
{
	glfwInit();

	VInitWindow();
	VInitRenderer();
    
    printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));
    printf("Vendor: %s\n",glGetString (GL_VENDOR));
    printf("Renderer: %s\n",glGetString (GL_RENDERER));
    printf("Version: %s\n",glGetString (GL_VERSION));
    printf("GLSL: %s\n",glGetString (GL_SHADING_LANGUAGE_VERSION));
}

void OpenGLVideoSystem::MouseButtonCallback(
	GLFWwindow* window, int button, int action, int mods)
{
	shared_ptr<EvtData_MouseClicked> pClickEvent(
		new EvtData_MouseClicked(button, action));

	g_pGameApp->GetPlayerInputEvtMgr()->VQueueEvent(pClickEvent);
}

void OpenGLVideoSystem::MousePositionCallback(
	GLFWwindow* window, double xpos, double ypos)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
	shared_ptr<EvtData_MouseMoved> pMoveEvent(
		new EvtData_MouseMoved(
                               xpos,
                               ypos,
                               width,
                               height
                               ));

	g_pGameApp->GetPlayerInputEvtMgr()->VQueueEvent(pMoveEvent);
}

void OpenGLVideoSystem::ScrollCallback(
                                    GLFWwindow* window,
                                    double scrollOffsetX,
                                    double scrollOffsetY)
{
    shared_ptr<EvtData_Scrolled> pKeyEvent(
                new EvtData_Scrolled(scrollOffsetX, scrollOffsetY));
    
    g_pGameApp->GetPlayerInputEvtMgr()->VQueueEvent(pKeyEvent);
}

void OpenGLVideoSystem::KeyCallback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mods)
{
	shared_ptr<EvtData_KeyPressed> pKeyEvent(
		new EvtData_KeyPressed(key, action));

	g_pGameApp->GetPlayerInputEvtMgr()->VQueueEvent(pKeyEvent);
}

void OpenGLVideoSystem::VSetWindowDimensions(
            const int& windowWidth, const int& windowHeight)
{
    m_windowWidth = windowWidth;
    m_windowHeight = windowHeight;
	m_pRenderer->SetCanvasSize(m_windowWidth, m_windowHeight);
}

void OpenGLVideoSystem::ResizeCallback(
                GLFWwindow* pWindow, int width, int height)
{
    g_pGameApp->GetVideoSystem()->VSetWindowDimensions(width, height);
}

void OpenGLVideoSystem::VInitWindow()
{
    GLFWmonitor* pMonitor;
	EventListenerDelegate closeWindowFunc =
		fastdelegate::MakeDelegate(
			this,
			&OpenGLVideoSystem::VCloseWindow
			);

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_windowWidth = 1024;
	m_windowHeight = 768;
    
    pMonitor =
            g_pGameApp->GetGameSettings()->GetValue("windowMode").compare("fullscreen")==0 ?
                glfwGetPrimaryMonitor() : NULL;

	string gameTitle =
		g_pGameApp->GetGameSettings()->GetValue("gameTitle");
	m_pWindow = glfwCreateWindow(
		m_windowWidth,
		m_windowHeight,
		gameTitle.c_str(),
		pMonitor,
		NULL);

	glfwMakeContextCurrent(m_pWindow);
    glfwSetWindowSizeCallback(
        m_pWindow, &OpenGLVideoSystem::ResizeCallback);
	glfwSetKeyCallback(
		m_pWindow, &OpenGLVideoSystem::KeyCallback);
	glfwSetCursorPosCallback(
		m_pWindow, &OpenGLVideoSystem::MousePositionCallback);
	glfwSetMouseButtonCallback(
		m_pWindow, &OpenGLVideoSystem::MouseButtonCallback);
    glfwSetScrollCallback(
        m_pWindow, &OpenGLVideoSystem::ScrollCallback);

	g_pGameApp->GetPlayerInputEvtMgr()->VAddListener(
		closeWindowFunc,
		EvtData_KeyPressed::s_eventType
		);
}

void OpenGLVideoSystem::VCloseWindow(IEventDataPtr pEventData)
{
	shared_ptr<EvtData_KeyPressed> pCastEventData =
		std::static_pointer_cast<EvtData_KeyPressed>(pEventData);

	if (pCastEventData->GetKey() == GLFW_KEY_ESCAPE &&
		pCastEventData->GetAction() == GLFW_PRESS)
	{
		if (m_pWindow == nullptr)
		{
			return;
		}
        
        g_pGameApp->Shutdown();

		glfwSetWindowShouldClose(m_pWindow, GL_TRUE);
	}
}

void OpenGLVideoSystem::VInitRenderer()
{
	glewExperimental = GL_TRUE;
	glewInit();

	m_pRenderer = new OpenGLRenderer();
	m_pRenderer->Init(m_windowWidth, m_windowHeight);
}

void OpenGLVideoSystem::VPreRender()
{
	glfwGetFramebufferSize(
		m_pWindow, &m_windowWidth, &m_windowHeight);

	glViewport(0, 0, m_windowWidth, m_windowHeight);
    

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLVideoSystem::VPostRender()
{
	glfwSwapBuffers(m_pWindow);
	glfwPollEvents();
}

OpenGLRenderer* OpenGLVideoSystem::VGetRenderer() const
{
	return m_pRenderer;
}

bool OpenGLVideoSystem::VCheckForNotifications()
{
	return !glfwWindowShouldClose(m_pWindow);
}