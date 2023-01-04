#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include <cmath>
#include <ctime>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <rapidjson/document.h>

#define STBI_MAX_DIMENSIONS (1 << 26)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_INCLUDE_IMPLEMENTATION
#include <stb_include.h>

#define TITLE "OpenGL Base"

#define APPLICATION_GL_VERSION_MAJOR 4 
#define APPLICATION_GL_VERSION_MINOR 6

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#include "include/openSans.h"
#include "include/formatting.h"
#include "include/shaders.h"
#include "include/textures.h"
#include "include/camera.h"
#include "include/windowing.h"
#include "include/common.h"
#include "include/pipeline.h"

#include "include/appCore.h"

int main(void) {
    Application application;
    application.run();

    return 0;
}

