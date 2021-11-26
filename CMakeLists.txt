cmake_minimum_required(VERSION 3.5)
project(sdl-metal)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

find_program(XXD xxd)
include(metal)

add_subdirectory(metal-cpp)

set(sdl_metal_SOURCES main.cpp)
add_compiled_metal_sources(sdl_metal_SOURCES triangle.metal)

add_executable(sdl-metal ${sdl_metal_SOURCES})

target_include_directories(
    sdl-metal
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
            "${SDL2_INCLUDE_DIRS}"
            "${CMAKE_SOURCE_DIR}/metal-cpp")

target_link_libraries(
    sdl-metal
    PRIVATE "${SDL2_LIBRARIES}" MetalCPP "-framework Metal" "-framework QuartzCore" "-framework Foundation")