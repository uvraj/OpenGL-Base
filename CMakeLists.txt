# Minimum required version for CMake
cmake_minimum_required(VERSION 3.16)

# Project settings
project(OpenGL-Base)

# Message for build type
message("Build type of project is: ${CMAKE_BUILD_TYPE}")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Verbose makefile for debugging
set(CMAKE_VERBOSE_MAKEFILE ON)

# Ensure -Ofast is applied to both Debug and Release builds
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# Compiler flags for both Debug and Release
if (MSVC)
    # MSVC specific flags
    add_compile_options(/W4 /Ox)  # /W4 for warning level 4, /Ox for full optimization
else()
    # GCC/Clang specific flags
    add_compile_options(-Wall -Wextra -Ofast)  # -Ofast for both Debug and Release
endif()

# Define directories for sources, external libraries, and resources
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(RESOURCES_DIR "${SRC_DIR}/resources")
set(EXT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

set(GLFW_DIR "${EXT_DIR}/glfw")
set(JSON_DIR "${EXT_DIR}/json")
set(GLAD_DIR "${EXT_DIR}/glad")
set(GLM_DIR "${EXT_DIR}/glm-master")
set(IMGUI_DIR "${EXT_DIR}/imgui")
set(STB_DIR "${EXT_DIR}/stb")

# Output directories
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# Sources
aux_source_directory(${SRC_DIR} SOURCES)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Set include directories for the project
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})

# Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# Link libraries
target_link_libraries(${PROJECT_NAME} imgui glfw glad nlohmann_json::nlohmann_json)

# External Libraries

# GLFW
add_subdirectory(${GLFW_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC "${GLFW_DIR}/include")

# RAPIDJSON
add_subdirectory(${JSON_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC "${JSON_DIR}/include")

# GLAD
add_library(glad "${GLAD_DIR}/src/glad.c")
target_include_directories(glad PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")

# GLM
file(GLOB glm_src "${GLM_DIR}/glm/*.hpp")
target_include_directories(${PROJECT_NAME} PRIVATE ${GLM_DIR})
target_precompile_headers(${PROJECT_NAME} PRIVATE ${glm_src})

# OPENGL
find_package(OpenGL REQUIRED)

# ImGui
file(GLOB imgui_src "${IMGUI_DIR}/*.cpp")
add_library(imgui ${imgui_src})
target_include_directories(imgui PRIVATE ${IMGUI_DIR})
target_include_directories(imgui PRIVATE "${GLFW_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE ${IMGUI_DIR})

# STB
file(GLOB stb_src "${STB_DIR}/.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${STB_DIR})
target_precompile_headers(${PROJECT_NAME} PRIVATE ${stb_src})

# Resource paths and custom commands

# Handle resource and shader paths based on build type
if (NOT MSVC)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        add_compile_definitions(RESOURCE_PATH="resources/")
        add_compile_definitions(SHADER_PATH="resources/shaders/")

        # Copy resources on Release build
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND  ${CMAKE_COMMAND} -E copy_directory ${RESOURCES_DIR} ${EXECUTABLE_OUTPUT_PATH}/resources
        )
    else()
        add_compile_definitions(RESOURCE_PATH="../../src/resources/")
        add_compile_definitions(SHADER_PATH="../../src/resources/shaders/")
    endif()
else()
    add_compile_definitions(RESOURCE_PATH="../../src/resources/")
    add_compile_definitions(SHADER_PATH="../../src/resources/shaders/")
endif()

