SET(VIDEO_SYSTEM_HEADERS	video_system/GLProgram.h
				video_system/GLSLShader.h
				video_system/IShader.h
				video_system/IVideoSystem.h
				video_system/OpenGLRenderer.h
				video_system/OpenGLVideoSystem.h
				PARENT_SCOPE
				)

SET(VIDEO_SYSTEM_SOURCE	video_system/GLProgram.cpp
				video_system/GLSLShader.cpp
				video_system/OpenGLRenderer.cpp
				video_system/OpenGLVideoSystem.cpp
				PARENT_SCOPE
				)