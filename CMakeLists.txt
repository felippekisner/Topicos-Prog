cmake_minimum_required(VERSION 3.15)
project(JogoNave C)

set(CMAKE_PIPELINE_DEPS_DIRECTORY "${CMAKE_BINARY_DIR}")
list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build")

find_package(raylib REQUIRED)

add_executable(JogoNave main.c)
target_link_libraries(JogoNave PRIVATE raylib)