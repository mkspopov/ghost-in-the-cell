cmake_minimum_required(VERSION 3.28)
project(ghost_in_the_cell)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML REQUIRED COMPONENTS graphics window system)

set(COMMON engine.cpp utils.cpp)

add_executable(${PROJECT_NAME} main.cpp game.cpp ${COMMON})
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

add_executable(test_strategy test.cpp game.cpp ${COMMON}
    strategy.h)
target_link_libraries(test_strategy sfml-graphics sfml-window sfml-system)
