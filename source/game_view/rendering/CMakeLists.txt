SET(RENDERING_HEADERS	rendering/IRenderingStep.h
			rendering/RenderingStep.h
			rendering/RenderToScreenStep.h				
			rendering/AssignStep.h
			rendering/ShadowStep.h
			rendering/Material.h
			rendering/PostRenderingStep.h
			rendering/Texture.h
			rendering/Geometry.h
			rendering/BulletDebugDrawStep.h
			PARENT_SCOPE
			)

SET(RENDERING_SOURCE	rendering/RenderingStep.cpp
			rendering/RenderToScreenStep.cpp
			rendering/AssignStep.cpp
			rendering/PostRenderingStep.cpp
			rendering/Texture.cpp
			rendering/ShadowStep.cpp
			rendering/Geometry.cpp
			rendering/Material.cpp
			rendering/BulletDebugDrawStep.cpp
			PARENT_SCOPE
			)