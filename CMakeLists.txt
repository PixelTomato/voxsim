cmake_minimum_required(VERSION 4.3)

project(VoxSim LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glm REQUIRED CONFIG)

add_subdirectory(external/glad)
add_subdirectory(engine)
add_subdirectory(game)

add_library(stb 
    STATIC
        external/stb/stb_image.cpp
        external/stb/stb_image.h
)

target_include_directories(stb PUBLIC external/stb)