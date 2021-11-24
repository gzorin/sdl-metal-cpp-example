add_library(
    MetalCPP STATIC
    MetalCPP.cpp)

target_include_directories(
    MetalCPP
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")