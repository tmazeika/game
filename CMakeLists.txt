cmake_minimum_required(VERSION 3.21)
project(game)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} glad/src/gl.c main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE glad/include)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw-3.3.6)
target_link_libraries(${PROJECT_NAME} glfw)
