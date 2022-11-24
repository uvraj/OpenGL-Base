cmake_minimum_required(VERSION 3.10)
project(OpenGL-Base)

message("Build type of project is: " ${CMAKE_BUILD_TYPE})

#set some flags
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL  "GLFW lib only")
set(GLFW_BUILD_TESTS OFF CACHE BOOL  "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only")

#compile GLFW from source?
option(COMP_GLFW_SOURCE "Compile GLFW from Source" OFF)

#source and external package files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(RESOURCES_DIR "${SRC_DIR}/resources")
set(EXT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
aux_source_directory(${SRC_DIR} SOURCES)
set(GLFW_DIR "${EXT_DIR}/glfw_lib")
set(GLAD_DIR "${EXT_DIR}/glad")
set(GLM_DIR "${EXT_DIR}/glm-master")
set(IMGUI_DIR "${EXT_DIR}/imgui")
set(FONTS_DIR "${EXT_DIR}/misc/Open_Sans/static/OpenSans")
set(STB_DIR   "${EXT_DIR}/stb")
set(PROJECT_VERSION "a0.0.1")
set(WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}")
set(CMAKE_C_STANDARD 99)

#avoid unnecessary VS stuff
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${EXECUTABLE_OUTPUT_PATH}")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_PATH}")
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${EXECUTABLE_OUTPUT_PATH}")
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_PATH}")
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   "${EXECUTABLE_OUTPUT_PATH}")
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${EXECUTABLE_OUTPUT_PATH}")

link_directories(${GLFW_DIR}/lib)

if(COMP_GLFW_SOURCE)
add_subdirectory(external/glfw)
endif()

#handle warnings
if (MSVC)
    # warning level 4 
    add_compile_options(/W4)
else()
    # lots of warnings 
    add_compile_options(-Wall -Wextra)
endif()

#add the exectable to CMAKE
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

#library stuff
#GLFW
if(WIN32 AND COMP_GLFW_SOURCE)
target_link_libraries(${PROJECT_NAME} glfw)
elseif(WIN32)
target_link_libraries(${PROJECT_NAME} glfw3)
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")
else()
target_link_libraries(${PROJECT_NAME} glfw)
endif()

#GLAD
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

#GLM
target_include_directories(${PROJECT_NAME} PRIVATE "${GLM_DIR}")

#OPENGL
find_package(OpenGL REQUIRED)

#imgui
add_library("imgui" "${IMGUI_DIR}/imgui.cpp")
add_library("imgui_demo" "${IMGUI_DIR}/imgui_demo.cpp")
add_library("imgui_draw" "${IMGUI_DIR}/imgui_draw.cpp")
add_library("imgui_tables" "${IMGUI_DIR}/imgui_tables.cpp")
add_library("imgui_widgets" "${IMGUI_DIR}/imgui_widgets.cpp")
add_library("imgui_glfw" "${IMGUI_DIR}/imgui_impl_glfw.cpp")
add_library("imgui_opengl" "${IMGUI_DIR}/imgui_impl_opengl3.cpp")

target_include_directories("imgui" PRIVATE "${IMGUI_DIR}")
target_include_directories("imgui" PRIVATE "${IMGUI_DIR}/backends")
target_include_directories("imgui" PRIVATE "${GLFW_DIR}/include")

target_include_directories("imgui_glfw" PRIVATE "${GLFW_DIR}/include")

target_include_directories(${PROJECT_NAME} PRIVATE "${IMGUI_DIR}")

target_link_libraries(${PROJECT_NAME} "imgui" "imgui_demo" "imgui_draw" "imgui_tables" "imgui_widgets" "imgui_glfw" "imgui_opengl" glfw3)

#stb
target_include_directories(${PROJECT_NAME} PRIVATE "${STB_DIR}")

#handle libgcc and libstdc++
if(WIN32)
target_link_libraries(${PROJECT_NAME} -static-libgcc -static-libstdc++)
endif()

#copy dlls
if(NOT MSVC AND WIN32)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND  $(CMAKE_COMMAND) -E copy ${GLFW_DIR}/lib/glfw3.dll     ${EXECUTABLE_OUTPUT_PATH}
)
endif()

#handle shaders
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