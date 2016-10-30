SET(RESOURCES_HEADERS	resources/IResource.h
			resources/FileManager.h
			resources/ImageResource.h
			resources/ObjResource.h
			resources/ResourceManager.h
			resources/XMLElement.h
			resources/XMLResource.h
			resources/SoundResource.h
			resources/CompiledMeshResource.h
			PARENT_SCOPE
			)

SET(RESOURCES_SOURCE	resources/ResourceManager.cpp
			resources/FileManager.cpp
			resources/ImageResource.cpp
			resources/ObjResource.cpp
			resources/XMLElement.cpp
			resources/XMLResource.cpp
			resources/SoundResource.cpp
			resources/CompiledMeshResource.cpp
			PARENT_SCOPE
			)

SET(SOURCE ${SOURCE} ${RESOURCES_SOURCE} PARENT_SCOPE)
SET(HEADERS ${HEADERS} ${RESOURCES_HEADERS} PARENT_SCOPE)