cmake_minimum_required(VERSION 3.16)
project(OpenGL-Base)

message("Build type of project is: " ${CMAKE_BUILD_TYPE})

# Set GLFW flags

set(GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL  "GLFW lib only")
set(GLFW_BUILD_TESTS OFF CACHE BOOL  "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only")

# Source, external and binary directories

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(RESOURCES_DIR "${SRC_DIR}/resources")
set(EXT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
set(GLFW_DIR "${EXT_DIR}/glfw")
set(GLAD_DIR "${EXT_DIR}/glad")
set(GLM_DIR "${EXT_DIR}/glm-master")
set(IMGUI_DIR "${EXT_DIR}/imgui")
set(STB_DIR   "${EXT_DIR}/stb")
set(PROJECT_VERSION "a0.0.1")
set(WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)

aux_source_directory(${SRC_DIR} SOURCES)

# Avoid unnecessary VS stuff
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${EXECUTABLE_OUTPUT_PATH})

# Handle warnings
if (MSVC)
    # warning level 4 
    add_compile_options(/W4)
else()
    # lots of warnings 
    add_compile_options(-Wall -Wextra)
endif()

# Add the exectable to CMAKE
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# GLFW 
add_subdirectory(external/glfw)
target_include_directories(${PROJECT_NAME} PUBLIC "${GLFW_DIR}/include")

# GLAD
add_library(glad "${GLAD_DIR}/src/glad.c")
target_include_directories(glad PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")

# GLM
file(GLOB glm_src "${GLM_DIR}/glm/*.hpp")
target_include_directories(${PROJECT_NAME} PRIVATE ${GLM_DIR})
# We might want to make this PUBLIC in the future.
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
# We might want to make this PUBLIC in the future.
target_precompile_headers(${PROJECT_NAME} PRIVATE ${stb_src})

# Link everything
target_link_libraries(${PROJECT_NAME} imgui glfw glad)

# Handle shaders and resources
if(NOT MSVC)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")

        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND  $(CMAKE_COMMAND) -E copy_directory ${RESOURCES_DIR}   ${EXECUTABLE_OUTPUT_PATH}/resources
        )

        add_compile_definitions(RESOURCE_PATH="resources/")
        add_compile_definitions(SHADER_PATH="resources/shaders/")

    else()

        add_compile_definitions(RESOURCE_PATH="../../src/resources/")
        add_compile_definitions(SHADER_PATH="../../src/resources/shaders/")

    endif()

else()

    add_compile_definitions(RESOURCE_PATH="../../src/resources/")
    add_compile_definitions(SHADER_PATH="../../src/resources/shaders/")

endif()