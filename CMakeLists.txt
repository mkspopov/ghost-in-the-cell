cmake_minimum_required(VERSION 3.28)
project(ghost_in_the_cell)

set(CMAKE_CXX_STANDARD 17)

add_library(engine engine.cpp)

add_executable(${PROJECT_NAME} main.cpp game.cpp)
find_package(SFML REQUIRED COMPONENTS graphics window system)
target_link_libraries(${PROJECT_NAME} engine sfml-graphics sfml-window sfml-system)
