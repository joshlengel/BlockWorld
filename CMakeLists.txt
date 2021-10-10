cmake_minimum_required(VERSION 3.0)

project(BlockWorld LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wpedantic")

set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC ${ROOT}/src)
set(INCLUDE ${ROOT}/include)
set(DEPS ${ROOT}/deps)
set(BIN ${ROOT}/bin)

set(EXECUTABLE_OUTPUT_PATH ${BIN})

file(GLOB_RECURSE sources "${INCLUDE}/*.h" "${SRC}/*.cpp")

add_subdirectory(${DEPS}/glfw)
add_subdirectory(${DEPS}/glad)

add_executable(BlockWorld ${sources})
target_include_directories(BlockWorld PRIVATE ${INCLUDE} ${DEPS}/glfw/include ${DEPS}/glad/include ${DEPS}/stb)
target_link_libraries(BlockWorld PRIVATE glfw glad)