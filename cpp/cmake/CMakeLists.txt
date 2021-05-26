project(MyProject)

cmake_minimum_required(VERSION 3.18)

include(FindPkgConfig)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_FLAGS "-Wall -std=c++11")

pkg_check_modules(SDL2 REQUIRED sdl2)
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
