cmake_minimum_required(VERSION 3.22)

project(
	TrueHUD
	VERSION 1.1.9
	LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
	message(FATAL_ERROR "in-source builds are not allowed")
endif()

macro(set_from_environment VARIABLE)
	if(NOT DEFINED ${VARIABLE} AND DEFINED ENV{${VARIABLE}})
		set(${VARIABLE} $ENV{${VARIABLE}})
	endif()
endmacro()

set_from_environment(CompiledPluginsPath)
# Provide a default deploy path if not specified via environment
if(NOT DEFINED CompiledPluginsPath)
	set(CompiledPluginsPath "${CMAKE_BINARY_DIR}/_deploy")
endif()

option(COPY_OUTPUT "Copy the output of build operations to the game directory" ON)
option(ENABLE_SKYRIM_SE "Enable support for Skyrim SE in the dynamic runtime feature." ON)
option(ENABLE_SKYRIM_AE "Enable support for Skyrim AE in the dynamic runtime feature." ON)
option(ENABLE_SKYRIM_VR "Enable support for Skyrim VR in the dynamic runtime feature." OFF)
option(BUILD_TESTS "Build unit tests" ON)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

add_subdirectory(src)

if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

include(cmake/packaging.cmake)
