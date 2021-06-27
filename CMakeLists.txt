cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)	# Don't let OPTION erase above non-cache variables
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(project_name dino)
set(project_version "0.2.0.0")

project(${project_name} VERSION "${project_version}")
set(IS_ROOT OFF)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(IS_ROOT TRUE)
endif()

# includes
include(CheckPIESupported)
check_pie_supported()
include(cmake/platform.cmake)

# interface
add_subdirectory(cmake/interface)

# header text
set(BUILD_CONFIG ${CMAKE_BUILD_TYPE})
if(CMAKE_CONFIGURATION_TYPES)
  set(BUILD_CONFIG "Multi-config")
endif()
message(STATUS "[${CMAKE_SYSTEM_PROCESSOR}] [${PLATFORM}] [${CMAKE_GENERATOR}] [${CMAKE_CXX_COMPILER_ID}] [${BUILD_CONFIG}]")

# compile commands
if(IS_ROOT AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  message(STATUS "Exporting compile commands")
endif()

# dino
set(SOURCES
  include/dino/handle.hpp
  include/dino/dino.hpp
  include/dino/dll_api.hpp
  include/dino/dinex_args.hpp
  src/dino.cpp
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
add_library(${PROJECT_NAME} ${SOURCES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} INTERFACE SYSTEM include)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME}
  PRIVATE ${project_name}::interface
  $<$<STREQUAL:${PLATFORM},Linux>:dl>
  $<$<STREQUAL:${PLATFORM},Win64>:kernel32>
)

# extra options
option(DINO_BUILD_TESTS "Build Dino Tests" ${IS_ROOT})
option(DINO_BUILD_DINEX "Build dinex launcher" ${IS_ROOT})

# tests
if(DINO_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# dinex
if(DINO_BUILD_DINEX)
  set(DINEX_EXE_NAME "dinex" CACHE STRING "Name of launcher executable")
  set(DINEX_ENTRYPOINT_NAME "run" CACHE STRING "Default entrypoint name that dinex will search for and run")
  set(DINEX_SILENT false CACHE BOOL "Set to `true` to suppress logging")
  set(DINEX_ERROR_CODE 1 CACHE STRING "Error return code (int)")
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/dinex_constants.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/dinex_constants.hpp")
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES src/dinex.cpp)
  add_executable(dinex src/dinex.cpp)
  target_include_directories(dinex PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
  target_link_libraries(dinex PRIVATE ${project_name}::${project_name} ${project_name}::interface)
  set_target_properties(dinex PROPERTIES OUTPUT_NAME ${DINEX_EXE_NAME})
endif()

if(IS_ROOT)
  # appveyor build script
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/.appveyor.yml.in" "${CMAKE_SOURCE_DIR}/.appveyor.yml")
  # compile_commands.json
  if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
    message(STATUS "Copying compile_commands.json")
    file(COPY "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" DESTINATION "${CMAKE_SOURCE_DIR}")
  endif()
endif()
