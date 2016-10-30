SET(SOURCE )
SET(HEADERS )

###########################################

add_subdirectory(actor_system)
source_group(actor_system FILES ${ACTOR_SYSTEM_SOURCE} ${ACTOR_SYSTEM_HEADERS})

###########################################

add_subdirectory(actor_components)
source_group(actor_components FILES ${ACTOR_COMPONENTS_SOURCE} ${ACTOR_COMPONENTS_HEADERS})

###########################################

add_subdirectory(data_structures)
source_group(data_structures FILES ${DATA_STRUCTURES_SOURCE} ${DATA_STRUCTURES_HEADERS})

###########################################

add_subdirectory(event_system)
source_group(event_system FILES ${EVENT_SYSTEM_SOURCE} ${EVENT_SYSTEM_HEADERS})

###########################################

add_subdirectory(game_app)
source_group(game_app FILES ${GAME_APP_SOURCE} ${GAME_APP_HEADERS})

###########################################

add_subdirectory(physics)
source_group(physics FILES ${PHYSICS_SOURCE} ${PHYSICS_HEADERS})

###########################################

add_subdirectory(process_system)
source_group(process_system FILES ${PROCESS_SYSTEM_SOURCE} ${PROCESS_SYSTEM_HEADERS})

###########################################

SET(SOURCE ${SOURCE}	${ACTOR_COMPONENTS_SOURCE}
			${ACTOR_SYSTEM_SOURCE}
			${DATA_STRUCTURES_SOURCE}
			${EVENT_SYSTEM_SOURCE}
			${GAME_APP_SOURCE}
			${PHYSICS_SOURCE}
			${PROCESS_SYSTEM_SOURCE}
			)
SET(HEADERS ${HEADERS}	${ACTOR_COMPONENTS_HEADERS}
			${ACTOR_SYSTEM_HEADERS}
			${DATA_STRUCTURES_HEADERS}
			${EVENT_SYSTEM_HEADERS}
			${GAME_APP_HEADERS}
			${PHYSICS_HEADERS}
			${PROCESS_SYSTEM_HEADERS}
			)

SET(LIB_INCLUDES ../../lib/include)

include_directories(${LIB_INCLUDES})

add_library(game_logic STATIC ${SOURCE} ${HEADERS})
set_source_files_properties(physics/OceanSimulator.cpp PROPERTIES COMPILE_FLAGS -ffast-math)

if(APPLE)
	enable_cpp11(game_logic)
endif()