cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)	# Don't let OPTION erase above non-cache variables
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(DINO_VERSION "0.1.0.0" CACHE STRING "dino version" FORCE)
project(dino VERSION ${DINO_VERSION})

set(DINO_IS_ROOT OFF)
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  set(DINO_IS_ROOT ON)
  message(STATUS "DINO is root B)")
endif()

# Includes
include(CheckPIESupported)
check_pie_supported()
include(cmake/platform.cmake)

# Interface
include(cmake/interface.cmake)

# Header text
set(BUILD_CONFIG ${CMAKE_BUILD_TYPE})
if(CMAKE_CONFIGURATION_TYPES)
  set(BUILD_CONFIG "Multi-config")
endif()
message(STATUS "[${CMAKE_SYSTEM_PROCESSOR}] [${PLATFORM}] [${CMAKE_GENERATOR}] [${CMAKE_CXX_COMPILER_ID}] [${BUILD_CONFIG}]")

# Library
set(SOURCES
  include/dino/handle.hpp
  include/dino/dino.hpp
  include/dino/dll_api.hpp
  src/dino.cpp
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
add_library(${PROJECT_NAME} ${SOURCES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE SYSTEM include)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME}
  PRIVATE dino::interface
  $<$<STREQUAL:${PLATFORM},Linux>:dl>
  $<$<STREQUAL:${PLATFORM},Win64>:kernel32>
)

# Tests
if(DINO_IS_ROOT)
  set(DINO_BUILD_TESTS ON CACHE BOOL "Build Dino Tests")
  if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    message(STATUS "Exporting compile commands")
  endif()
else()
  set(DINO_BUILD_TESTS OFF CACHE BOOL "Build Dino Tests")
endif()
if(DINO_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# Compile commands
if(DINO_IS_ROOT AND EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
  message(STATUS "Copying compile_commands.json")
  file(COPY "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" DESTINATION "${CMAKE_SOURCE_DIR}")
endif()
