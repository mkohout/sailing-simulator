SET(SOURCE )
SET(HEADERS )

###########################################

add_subdirectory(audio)
source_group(audio FILES ${AUDIO_SOURCE} ${AUDIO_HEADERS})

###########################################

add_subdirectory(scene)
source_group(scene FILES ${SCENE_SOURCE} ${SCENE_HEADERS})

###########################################

add_subdirectory(scene_graph)
source_group(scene_graph FILES ${SCENE_GRAPH_SOURCE} ${SCENE_GRAPH_HEADERS})

###########################################

add_subdirectory(ui)
source_group(ui FILES ${UI_SOURCE} ${UI_HEADERS})

###########################################

add_subdirectory(rendering)
source_group(rendering FILES ${RENDERING_SOURCE} ${RENDERING_HEADERS})

###########################################

add_subdirectory(video_system)
source_group(video_system FILES ${VIDEO_SYSTEM_SOURCE} ${VIDEO_SYSTEM_HEADERS})

###########################################

SET(SOURCE ${SOURCE}	${AUDIO_SOURCE}
			${SCENE_SOURCE}
			${SCENE_GRAPH_SOURCE}
			${UI_SOURCE}
			${RENDERING_SOURCE}
			${VIDEO_SYSTEM_SOURCE}
			)
SET(HEADERS ${HEADERS}	${AUDIO_HEADERS}
			${SCENE_HEADERS}
			${SCENE_GRAPH_HEADERS}
			${UI_HEADERS}
			${RENDERING_HEADERS}
			${VIDEO_SYSTEM_HEADERS}
			)

###########################################

SET(LIB_INCLUDES ../../lib/include)

set(GPU_HEAVY_RENDERING ON CACHE BOOL "Enables gpu heavy operations as shadow mapping and screen space reflections.")
if(${GPU_HEAVY_RENDERING})
	ADD_DEFINITIONS(-DENABLE_GPU_HEAVY)
endif()

include_directories(${LIB_INCLUDES})

add_library(game_view STATIC ${SOURCE} ${HEADERS})

target_link_libraries(game_view application_layer)
if(UNIX)
    target_link_libraries(game_view game_logic)
endif()

if(APPLE)
   enable_cpp11(game_view)
endif()
