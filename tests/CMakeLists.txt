project(dino-tests)

set(BUILD_SHARED_LIBS ON)

set(LIB_SOURCES lib/lib.hpp lib/lib.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${LIB_SOURCES})
add_library(dinolib ${LIB_SOURCES})
add_library(dino::lib ALIAS dinolib)
target_include_directories(dinolib PRIVATE lib)
target_link_libraries(dinolib PUBLIC dino::dino dino::interface)

# dino test
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES dino_test.cpp)
add_executable(dino-test dino_test.cpp)
target_link_libraries(dino-test PRIVATE dino::dino dino::interface)

add_test(NAME dino_test
  COMMAND "$<TARGET_FILE:dino-test>" "$<TARGET_FILE_DIR:dinolib>"
)

# dinex test
if(DINO_BUILD_DINEX)
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES dinex_test.cpp)
  add_library(dinex_test SHARED dinex_test.cpp)
  target_link_libraries(dinex_test PRIVATE dino::dino)

  add_test(NAME dinex_test
    COMMAND "$<TARGET_FILE:dinex>" "-dir=$<TARGET_FILE_DIR:dinex_test>" "-entry=run_test" "dinex_test"
  )
endif()
