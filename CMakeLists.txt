cmake_minimum_required (VERSION 3.0)

# Set policy

# Input directories must have CMakeLists.txt
cmake_policy(SET CMP0014 NEW)

project (SailingSimulator)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE INTERNAL "" FORCE)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE INTERNAL "" FORCE)
set(TESTS_OUTPUT_DIR ${EXECUTABLE_OUTPUT_PATH}/tests CACHE INTERNAL "" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# version number

set (SIMULATOR_VERSION_MAJOR 0)
set (SIMULATOR_VERSION_MINOR 1)
set (SIMULATOR_VERSION_PATCH 0)
set (SIMULATOR_VERSION
            ${SIMULATOR_VERSION}.${SIMULATOR_MINOR_VERSION}.{$SIMULATOR_PATCH_VERSION})

include(specs/build_files/cmake/macros.cmake)

option(DEBUG "Display debugging informations" ON)
option(GLM_TEST_ENABLE "Add GLM to source tree" OFF)

set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

if(APPLE)
    set(LIB_DIR ${LIB_DIR}/macos)
elseif(WIN32)
    set(LIB_DIR ${LIB_DIR}/win64)
elseif(UNIX)
    set(LIB_DIR ${LIB_DIR}/linux)
endif(APPLE)

set(ENGINE_INCLUDES "")
set(ENGINE_LIBS "")
set(ENGINE_LINKS "")

list(APPEND ENGINE_INCLUDES ${CMAKE_SOURCE_DIR}/source/extern/)
list(APPEND ENGINE_INCLUDES ${CMAKE_SOURCE_DIR}/source/extern/bullet3/src/)

SET(BOOST_VERSION 1.59.0)

if(APPLE)
	list(APPEND ENGINE_LIBS "${LIB_DIR}/glew/libGLEW.a")
	list(APPEND ENGINE_LIBS "${LIB_DIR}/glfw/libglfw3.a")
	list(APPEND ENGINE_INCLUDES "/System/Library/Frameworks")
	find_library(COCOA_LIBRARY Cocoa)
	find_library(OpenGL_LIBRARY OpenGL)
	find_library(IOKIT_LIBRARY IOKit)
	find_library(COREVIDEO_LIBRARY CoreVideo)

	mark_as_advanced(COCOA_LIBRARY OpenGL_LIBRARY)

	list(APPEND ENGINE_LIBS ${COCOA_LIBRARY})
	list(APPEND ENGINE_LIBS ${OpenGL_LIBRARY})
	list(APPEND ENGINE_LIBS ${IOKIT_LIBRARY})
	list(APPEND ENGINE_LIBS ${COREVIDEO_LIBRARY})
	list(APPEND ENGINE_LIBS ${LIB_DIR}/bullet/libLinearMath.a)
	list(APPEND ENGINE_LIBS ${LIB_DIR}/bullet/libBulletCollision.a)
	list(APPEND ENGINE_LIBS ${LIB_DIR}/bullet/libBulletDynamics.a)
	list(APPEND ENGINE_LIBS ${LIB_DIR}/bullet/libBulletInverseDynamics.a)
	list(APPEND ENGINE_LIBS ${LIB_DIR}/bullet/libBulletSoftBody.a)

	list(APPEND ENGINE_LIBS "${LIB_DIR}/fmod/libfmod.dylib")

elseif(UNIX)
	list(APPEND ENGINE_LIBS "${LIB_DIR}/glew/libGLEW.a")
	list(APPEND ENGINE_LIBS "${LIB_DIR}/glfw/libglfw3.a")
	list(APPEND ENGINE_LIBS "${LIB_DIR}/fmod/libfmod.so")

	list(APPEND ENGINE_LIBS 
		"${LIB_DIR}/bullet/libBulletDynamics.a"
		"${LIB_DIR}/bullet/libBulletCollision.a"
		"${LIB_DIR}/bullet/libBulletInverseDynamics.a"
		"${LIB_DIR}/bullet/libBulletSoftBody.a" 
		"${LIB_DIR}/bullet/libLinearMath.a"
		)

	find_package(OpenGL REQUIRED)
	find_package(X11 REQUIRED)
	find_package (Threads)
    list(APPEND ENGINE_INCLUDES ${X11_INCLUDE_DIRS})
    list(APPEND ENGINE_LINKS ${X11_LIBRARY_DIR})
    list(APPEND ENGINE_LINKS ${CMAKE_THREAD_LIBS_INIT})
    list(APPEND ENGINE_LIBS ${X11_LIBRARIES} ${X11_LIBRARIES})
    set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -lXrandr")
    set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -lXi")
	include_directories(${OPENGL_INCLUDE_DIR})
    list(APPEND ENGINE_LINKS ${OPENGL_LIBRARY_DIR})
    list(APPEND ENGINE_LIBS ${OPENGL_LIBRARIES} ${OPENGL_LIBRARIES})
    set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
    set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -lXxf86vm")
    set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -lXinerama")
    set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -lXcursor")

    SET(BOOST_VERSION 1.55.0)
elseif(WIN32)
	list(APPEND ENGINE_LIBS "${LIB_DIR}/glfw/glfw3.lib")
	list(APPEND ENGINE_LINKS "${LIB_DIR}/glfw")
	list(APPEND ENGINE_LIBS "${LIB_DIR}/glew/glew32.lib")
	list(APPEND ENGINE_LINKS "${LIB_DIR}/glew")
	list(APPEND ENGINE_LIBS "${LIB_DIR}/fmod/fmod64_vc.lib")
	list(APPEND ENGINE_LIBS 
		debug "${LIB_DIR}/bullet/LinearMath.lib" optimized "${LIB_DIR}/bullet/Release/LinearMath.lib"
		debug "${LIB_DIR}/bullet/BulletCollision.lib" optimized "${LIB_DIR}/bullet/Release/BulletCollision.lib"
		debug "${LIB_DIR}/bullet/BulletDynamics.lib" optimized "${LIB_DIR}/bullet/Release/BulletDynamics.lib"
		debug "${LIB_DIR}/bullet/BulletInverseDynamics.lib" optimized "${LIB_DIR}/bullet/Release/BulletInverseDynamics.lib"
		debug "${LIB_DIR}/bullet/BulletSoftBody.lib" optimized "${LIB_DIR}/bullet/Release/BulletSoftBody.lib"
		)

	find_package(OpenGL REQUIRED)
	include_directories(${OPENGL_INCLUDE_DIR})
endif()

list(APPEND ENGINE_LINKS "${LIB_DIR}/bullet")

find_package(Boost ${BOOST_VERSION}	COMPONENTS
                                   	filesystem
					system
                                      REQUIRED)
  
if(Boost_FOUND)
    list(APPEND ENGINE_INCLUDES ${Boost_INCLUDE_DIRS})
    list(APPEND ENGINE_LINKS ${Boost_LIBRARY_DIR})
    list(APPEND ENGINE_LIBS ${Boost_LIBRARIES} ${Boost_LIBRARIES})

    option(Boost_USE_STATIC_LIBS ON)
    option(Boost_USE_MULTITHREADED OFF)
    option(Boost_USE_STATIC_RUNTIME OFF)
endif(Boost_FOUND)

include_directories(${ENGINE_INCLUDES})
link_directories(${ENGINE_LINKS})

if(APPLE)
  set( CMAKE_C_COMPILER "/usr/bin/gcc")
  set( CMAKE_CXX_COMPILER "/usr/bin/g++")
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++" )
  set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++" )
  set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++" )
  set( CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -stdlib=libc++" )

ADD_DEFINITIONS(-DUSE_FMOD)
elseif(UNIX)
  set( CMAKE_C_COMPILER "/usr/bin/gcc")
  set( CMAKE_CXX_COMPILER "/usr/bin/g++")
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
endif()

ADD_DEFINITIONS(-DCMAKE_PROJECT_PATH=${CMAKE_CURRENT_SOURCE_DIR})
ADD_DEFINITIONS(-DGLM_FORCE_RADIANS)
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

add_subdirectory(lib)
add_subdirectory(source)
