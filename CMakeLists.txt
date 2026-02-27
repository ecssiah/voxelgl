cmake_minimum_required(VERSION 3.20)

project(
    voxelgl
    VERSION 0.1
    LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "" FORCE)
endif()

find_package(glfw3 CONFIG REQUIRED)

add_executable(
    voxelgl
    src/main.cpp
)

target_include_directories(
    voxelgl 
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
    voxelgl 
    PRIVATE
    glfw
)

if(APPLE)
    target_link_libraries(
        voxelgl 
        PRIVATE
        "-framework OpenGL"
    )
endif()

target_sources(
    voxelgl 
    PRIVATE
    vendor/glad/src/glad.c
    src/app/App.cpp
    src/app/Camera.cpp
    src/app/World/Cell.cpp
    src/app/World/Sector.cpp
    src/app/World/World.cpp
    src/platform/WindowSystem.cpp
    src/platform/InputSystem.cpp
    src/render/Renderer.cpp
    src/render/STBImageLoader.cpp
)

target_compile_definitions(
    voxelgl
    PRIVATE
    GLFW_INCLUDE_NONE
    GL_SILENCE_DEPRECATION
)