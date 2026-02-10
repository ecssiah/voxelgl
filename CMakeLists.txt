cmake_minimum_required(VERSION 3.20)

project(
    voxelgl
    VERSION 0.1
    LANGUAGES C CXX
)

# ---- Language standards ----
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- macOS OpenGL requirements ----
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "" FORCE)
endif()

# ---- GLFW ----
find_package(glfw3 CONFIG REQUIRED)

# ---- Executable ----
add_executable(
    voxelgl
    src/main.cpp
    vendor/glad/src/glad.c
)

# ---- Include paths ----
target_include_directories(
    voxelgl PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb/include
)

# ---- Link libraries ----
target_link_libraries(
    voxelgl PRIVATE
    glfw
)

if(APPLE)
    target_link_libraries(
        voxelgl PRIVATE
        "-framework OpenGL"
    )
endif()

target_sources(
    voxelgl PRIVATE
    src/app/App.cpp
    src/platform/GlfwWindow.cpp
    src/texture/StbImage.cpp
)

# ---- Definitions ----
target_compile_definitions(
    voxelgl PRIVATE
    GLFW_INCLUDE_NONE
)