cmake_minimum_required(VERSION 3.15)
project(JogoNave C)

set(CMAKE_PIPELINE_DEPS_DIRECTORY "${CMAKE_BINARY_DIR}")
list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/build")

find_package(raylib REQUIRED)

add_executable(JogoNave src/main.c src/listaObstaculos.c src/listaObstaculos.h)
target_link_libraries(JogoNave PRIVATE raylib)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/images"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/images"
)
