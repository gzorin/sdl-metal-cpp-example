cmake_minimum_required(VERSION 3.5)
project(sdl-metal)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

add_executable(sdl-metal main.cpp)

target_include_directories(
    sdl-metal
    PRIVATE "${SDL2_INCLUDE_DIRS}")

target_link_libraries(
    sdl-metal
    PRIVATE "${SDL2_LIBRARIES}")