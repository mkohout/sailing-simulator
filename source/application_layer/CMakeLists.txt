SET(SOURCE )
SET(HEADERS )

###########################################

add_subdirectory(core)
source_group(core FILES ${CORE_SOURCE} ${CORE_HEADERS})

###########################################

add_subdirectory(error_handling)
source_group(error_handling FILES ${ERROR_SOURCE} ${ERROR_HEADERS})

###########################################

add_subdirectory(resources)
source_group(resources FILES ${RESOURCES_SOURCE} ${RESOURCES_HEADERS})

###########################################

add_subdirectory(utils)
source_group(utils FILES ${UTILS_SOURCE} ${UTILS_HEADERS})

###########################################

SET(SOURCE	${SOURCE}
			${CORE_SOURCE}
			${RESOURCES_SOURCE}
			${ERROR_SOURCE}
			${UTILS_SOURCE}
			)
SET(HEADERS ${HEADERS}
			${CORE_HEADERS}
			${RESOURCES_HEADERS}
			${ERROR_HEADERS}
			${UTILS_HEADERS}
			)

SET(LIB_INCLUDES ../../lib/include)

include_directories(${LIB_INCLUDES})

add_library(application_layer STATIC ${SOURCE} ${HEADERS})
target_link_libraries(application_layer extern_tinyxml2)
if(UNIX)
    target_link_libraries(application_layer game_view)
endif()

add_executable(sailing-simulator sailing-simulator/main.cpp)

target_link_libraries(sailing-simulator application_layer)
target_link_libraries(sailing-simulator game_logic)
target_link_libraries(sailing-simulator game_view)
target_link_libraries(sailing-simulator ${ENGINE_LIBS})
target_link_libraries(sailing-simulator extern_tinyxml2)
target_link_libraries(sailing-simulator extern_glm)
target_link_libraries(sailing-simulator extern_soil)
target_link_libraries(sailing-simulator ${OPENGL_LIBRARIES})
target_link_libraries(sailing-simulator ${CMAKE_THREAD_LIBS_INIT})

ADD_CUSTOM_COMMAND(TARGET sailing-simulator
          POST_BUILD
		  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
          COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/lib/win64/glew/glew32.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
		  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
		  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/lib/win64/glew/glew32.dll" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
		  )

if(APPLE)
  install(
	TARGETS sailing-simulator
	DESTINATION "."
  )

  enable_cpp11(application_layer)
endif()
