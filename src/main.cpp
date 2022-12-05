#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdio>
#include <cstring>
#include <vector>
#include <cmath>

#define STBI_MAX_DIMENSIONS (1 << 26)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_INCLUDE_IMPLEMENTATION
#include <stb_include.h>

#include "include/formatting.h"
#include "include/common.h"
#include "include/textures.h"
#include "include/camera.h"
#include "include/OpenSans.h"

#define TITLE "OpenGL Base"

#define APPLICATION_GL_VERSION_MAJOR 4 
#define APPLICATION_GL_VERSION_MINOR 6

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

bool firstMouse = true;
bool sineTaylorEnabled = true;
bool expTaylorEnabled = true;

int sineTaylorIterations = 10;
int expTaylorIterations = 10;

int viewWidth = SCREEN_WIDTH;
int viewHeight = SCREEN_HEIGHT;
float aspectRatio = 1.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float graphScale = 1.0f;
float graphMovementSpeed = 1.0f;

glm::vec2 cursorPosition = glm::vec2(0.0f);
glm::vec2 graphOffset = glm::vec2(0.0f);

void frameBufferSizeCallBack(GLFWwindow *window, int width, int height) {
    // Zhis function handles the user changing the resolution of the window.
    // Personal note: ALL glTexImage2D() calls related to framebuffer attachments MUST be placed inside here,
    // otherwise they won't resize!
    glViewport(0, 0, width, height);
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
}

void GLAPIENTRY messageCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    printGLError("GL Output: \n");
    std::printf("%s", message);
    std::printf("\n");
}

void handleInput(GLFWwindow *window, float time) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        graphOffset.y += deltaTime * graphMovementSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        graphOffset.y -= deltaTime * graphMovementSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        graphOffset.x += deltaTime * graphMovementSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        graphOffset.y -= deltaTime * graphMovementSpeed;
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
    cursorPosition.x = graphScale * aspectRatio * (static_cast <float> (xposIn) / viewWidth - 0.5);
    cursorPosition.y = -graphScale * (static_cast <float> (yposIn) / viewHeight - 0.5);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    graphScale -= static_cast <float> (yoffset);

    if(graphScale < 0.25f) 
        graphScale = 0.25f;
    
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_R)
        graphOffset = glm::vec2(0.0f);
}

void handleDPI(GLFWwindow *window, ImGuiIO &io) {
    static float prevScale = 1.0;
    /*
     * This function handles:
     * - DPI
     *  -> Resizing everything upon detecting a GLFW scale change
     !WONTFIX: Only works on Windows!
    */

    // Keep track of the x scale
    // We don't need the y scale. One scale suffices. Pass NULL.
    float xScale;
    glfwGetWindowContentScale(window, &xScale, NULL);

    // Check whether the scale changed
    if (xScale != prevScale) {
        // If it did, reload the fonts at the desired scale and rebuild the font atlas.
        prevScale = xScale;
        io.Fonts->Clear();

        io.Fonts->AddFontFromMemoryCompressedBase85TTF(OpenSans_compressed_data_base85, 20 * xScale);

        io.Fonts->Build();
        ImGui_ImplOpenGL3_DestroyFontsTexture();
        ImGui_ImplOpenGL3_CreateFontsTexture();
        
        // Handle user Output
        printInfo();
        std::printf("Window scale changed -> rebuilt font atlas.\n");
    }
}

int main(void) {
    // Tell STB that I want to flip the y coord
    stbi_set_flip_vertically_on_load(true);

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, APPLICATION_GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, APPLICATION_GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create a window object with the following resolution and title
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE, NULL, NULL); 

    if (window == NULL) {
        printError();
        std::printf("GLFW window creation failed.\n");
        glfwTerminate();
        return -1;
    }

    // Make "window" the main context on the current thread
    glfwMakeContextCurrent(window);

    // Pass GLAD the function to load the address of OS-specific OPENGL function pointers.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printError();
        std::printf("GLAD initialization failed.\n");
        return -1;
    }

    printSuccess();
    std::printf("Window and OpenGL context created!\n");

    // OpenGL can be used safely, let's enable debug messages.
    // glEnable(GL_DEBUG_OUTPUT);
    // glDebugMessageCallback(messageCallBack, 0);

    // Create a viewport with:
    glViewport(
        0, 0, // The location
        SCREEN_WIDTH, SCREEN_HEIGHT // The resolution
    );

    // Handle callbacks.
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallBack);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // Initialize ImGUI
    ImGui::CreateContext();
    printSuccess();
    std::printf("ImGui context created!\n");

    // Create an instsace of the ImGuiIO struct
    ImGuiIO& io = ImGui::GetIO(); (void) io;

    // Create a style instance
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 7.5f; // Round our windows a bi
    
    // Start the ImGUI GLFW / OpenGL impl.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Initilize our OpenGL objects.
    quad quadVAO;

    // Shaders
    shader graphRender;
    graphRender.load("graphRender.vert", "graphRender.frag");

    shader postProcess;
    postProcess.load("postProcess.vert", "postProcess.frag");

    // Main Framebuffer
    GLuint mainFBO;
    glGenFramebuffers(1, &mainFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);

    GLuint imageLinear;
    glGenTextures(1, &imageLinear);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imageLinear);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);  

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, imageLinear, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printError("Framebuffer incomplete!\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Time...
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();
        handleInput(window, currentFrame);
        glfwGetFramebufferSize(window, &viewWidth, &viewHeight);
        aspectRatio = static_cast <float> (viewWidth) / static_cast <float> (viewHeight);

        // Resize fonts upon DPI change.
        handleDPI(window, io);

        // Start ImGUI frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(1000.0f, 10.0f), ImGuiCond_Once);

        ImGui::Begin("Main Window");

        // ImGui::SliderFloat("Speed", &graphMovementSpeed, 0.0, 5.0);
        ImGui::Checkbox("Sine Taylor Approx.", &sineTaylorEnabled);
        ImGui::SliderInt("Sine Taylor Approx. Steps", &sineTaylorIterations, 1, 100);
        ImGui::Checkbox("Exp. Taylor Approx.", &expTaylorEnabled);
        ImGui::SliderInt("Exp. Taylor Approx. Steps", &expTaylorIterations, 1, 100);

        if(ImGui::Button("Reload Shaders")) {
            graphRender.load("graphRender.vert", "graphRender.frag");
            postProcess.load("postProcess.vert", "postProcess.frag");
        }

        ImGui::End();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.35f);

        ImGui::Begin("Performance Metrics", NULL, window_flags);
        ImGui::Text("FPS: %.2f", io.Framerate);
        ImGui::Text("Cursor Position:");
        ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "X: %.2f", cursorPosition.x);
        ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "Y: %.2f", cursorPosition.y);
        ImGui::Text("Scale: %f", graphScale);

        ImGui::End();

        ImGui::Render();

        // Main Pass
        glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
        glClear(GL_COLOR_BUFFER_BIT);

        graphRender.useProgram();

        graphRender.pushBoolUniform("sineTaylorEnabled", sineTaylorEnabled);
        graphRender.pushBoolUniform("expTaylorEnabled", expTaylorEnabled);
        graphRender.pushIntUniform("sineTaylorIterations", sineTaylorIterations);
        graphRender.pushIntUniform("expTaylorIterations", expTaylorIterations);
        graphRender.pushFloatUniform("graphScale", graphScale);
        graphRender.pushFloatUniform("aspectRatio", aspectRatio);
        graphRender.pushVec2Uniform("graphOffset", graphOffset.x, graphOffset.y);

        quadVAO.draw();

        // Post-processing pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, imageLinear);

        postProcess.useProgram();
        postProcess.pushIntUniform("imageLinear", 0);

        quadVAO.draw();

        // Finally draw the imgui stuff
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Turn the back side of the buffer over (or vice-versa)
        glfwSwapBuffers(window);
    }

    glDeleteFramebuffers(1, &mainFBO);

    glfwTerminate();
    return 0;
}

