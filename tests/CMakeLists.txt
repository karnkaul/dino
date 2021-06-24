project(dino-tests)

set(BUILD_SHARED_LIBS ON)

set(LIB_SOURCES lib/lib.hpp lib/lib.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/lib" FILES ${LIB_SOURCES})
add_library(dinolib ${LIB_SOURCES})
add_library(dino::lib ALIAS dinolib)
target_include_directories(dinolib PRIVATE lib)
target_link_libraries(dinolib PUBLIC dino::dino dino::interface)

set(EXE_SOURCES exe/dino_test.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/exe" FILES ${EXE_SOURCES})
add_executable(dino-test ${EXE_SOURCES})
target_link_libraries(dino-test PRIVATE dino::dino dino::interface)

add_test(NAME dino_test COMMAND
  "$<TARGET_FILE:dino-test>" "$<TARGET_FILE_DIR:dinolib>"
)
