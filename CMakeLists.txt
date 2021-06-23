cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)	# Don't let OPTION erase above non-cache variables
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")
set(CMAKE_CXX_EXTENSIONS OFF CACHE INTERNAL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(APP_VERSION "1.0.0.0" CACHE STRING "Application Version" FORCE)
project(cpp-template VERSION ${APP_VERSION})

# Includes
include(CheckPIESupported)
check_pie_supported()
include(cmake/platform.cmake)
include(cmake/utils.cmake)

# Git operations
set(GIT_COMMIT_HASH "[unknown]")
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
	get_git_commit_hash(GIT_COMMIT_HASH)
	if ("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
		option(GIT_UPDATE_SUBMODULES "Init/Update submodules during generation" ON)
	else()
		option(GIT_UPDATE_SUBMODULES "Init/Update submodules during generation" OFF)
	endif()
	if(GIT_UPDATE_SUBMODULES)
		update_git_submodules(1)
	endif()
endif()

# Constants
set(GIT_COMMIT_HASH ${GIT_COMMIT_HASH} CACHE STRING "Commit hash" FORCE)

# Interface
include(cmake/interface.cmake)

# Header text
set(BUILD_CONFIG ${CMAKE_BUILD_TYPE})
if(CMAKE_CONFIGURATION_TYPES)
	set(BUILD_CONFIG "Multi-config")
endif()
message(STATUS "[${CMAKE_SYSTEM_PROCESSOR}] [${PLATFORM}] [${CMAKE_GENERATOR}] [${CMAKE_CXX_COMPILER_ID}] [${BUILD_CONFIG}]")
if(CI_BUILD)
	message(STATUS "== CI Build ==")
endif()

# Executable
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.*pp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE interface-lib)
if(W_MSBUILD)
	set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()
