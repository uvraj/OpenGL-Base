#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

Camera mainCamera(glm::vec3(0.0f, 0.0f, 0.0f));

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

float frameTime = 0.0f;
float lastFrame = 0.0f;

bool mouseCaught = true;

void frameBufferSizeCallBack(GLFWwindow *window, int width, int height) {
    // Zhis function handles the user changing the resolution of the window.
    // Personal note: ALL glTexImage2D() calls related to framebuffer attachments MUST be placed inside here,
    // otherwise they won't resize!
    glViewport(0, 0, width, height);
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glActiveTexture(GL_TEXTURE1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
}

void GLAPIENTRY messageCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    printGLError("GL Output: \n");
    std::cout << message << std::endl;
}

void handleInput(GLFWwindow *window, float time) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(FORWARD, frameTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(BACKWARD, frameTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(LEFT, frameTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(RIGHT, frameTime);
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = xposIn;
    float ypos = yposIn;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if(mouseCaught)
        mainCamera.ProcessMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    mainCamera.ProcessMouseScroll(yoffset);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        mouseCaught = !mouseCaught;
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
    style.WindowRounding = 7.5f; // Round our windows a bit
    
    // Start the ImGUI GLFW / OpenGL impl.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 420 core");

    // Initilize our OpenGL objects.
    scene mainScene;
    quad quadVAO;

    // Shaders
    shader mainSceneShader;
    mainSceneShader.load("mainScene.vert", "mainScene.frag");

    shader postProcess;
    postProcess.load("postProcess.vert", "postProcess.frag");

    // Textures
    texture3D_binaryDump colorLookup("FUJI_ETERNA_250D_FUJI_3510.DAT", 2, 21, 21, 21, 3, GL_RGB32F, GL_RGB, GL_FLOAT);

    // Main Framebuffer
    GLuint mainFBO;
    glGenFramebuffers(1, &mainFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);

    GLuint imageLinear, sceneDepth;
    glGenTextures(1, &imageLinear);
    glGenTextures(1, &sceneDepth);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imageLinear);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sceneDepth);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, imageLinear, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, sceneDepth, 0); 

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printError("Framebuffer incomplete!\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Loop variables
    glm::mat4 model = glm::mat4(1.0f);
    glm::ivec2 viewDimensions = glm::ivec2(SCREEN_WIDTH, SCREEN_HEIGHT);

    float currentFrame = 0.0f;
    float aspectRatio = 1.0f;
    bool useColorGrade = false;
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Time...
        currentFrame = glfwGetTime();
        frameTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();
        handleInput(window, currentFrame);
        glfwGetFramebufferSize(window, &viewDimensions[0], &viewDimensions[1]);
        aspectRatio = static_cast <float> (viewDimensions.x) / static_cast <float> (viewDimensions.y);

        // Catch / Uncatch mouse based on mouseCaught.
        if(mouseCaught) {
            // Capture our mouse
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        // Resize fonts upon DPI change.
        handleDPI(window, io);

        // Update camera data
        mainCamera.updateCameraData(currentFrame, aspectRatio);

        // Start ImGUI frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(1000.0f, 10.0f), ImGuiCond_Once);

        ImGui::Begin("Main Window");

        ImGui::Checkbox("Use Color Grade", &useColorGrade);

        if(ImGui::Button("Reload Shaders")) {
            mainSceneShader.load("mainScene.vert", "mainScene.frag");
            postProcess.load("postProcess.vert", "postProcess.frag");
        }

        ImGui::End();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.35f);

        ImGui::Begin("Performance Metrics", NULL, window_flags);
        ImGui::Text("FPS: %.2f", io.Framerate);
        ImGui::Text("Camera Position:");
        ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "X: %.2f", mainCamera.Position.x);
        ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "Y: %.2f", mainCamera.Position.y);
        ImGui::TextColored(ImVec4(0.0, 0.0, 1.0, 1.0), "Z: %.2f", mainCamera.Position.z); 
        ImGui::Text("Camera FoV:\t%.2f", mainCamera.FoV);
        ImGui::Text("Camera Pitch:\t%.2f", mainCamera.Pitch);
        ImGui::Text("Camera Yaw:\t%.2f", mainCamera.Yaw);

        ImGui::End();

        ImGui::Render();

        glEnable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Main Pass
        glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mainSceneShader.useProgram();
        mainSceneShader.pushMat4Uniform("cameraViewMatrix", mainCamera.viewMatrix);
        mainSceneShader.pushMat4Uniform("cameraViewMatrixInverse", mainCamera.viewMatrixInverse);
        mainSceneShader.pushMat4Uniform("cameraProjectionMatrix", mainCamera.projectionMatrix);
        mainSceneShader.pushMat4Uniform("cameraProjectionMatrixInverse", mainCamera.projectionMatrixInverse);
        mainSceneShader.pushMat4Uniform("modelMatrix", model);
        mainScene.draw();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_DEPTH_TEST);

        // Post-processing pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, imageLinear);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, sceneDepth);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, colorLookup.textureID);

        postProcess.useProgram();

        postProcess.pushIntUniform("useColorGrade", useColorGrade);

        quadVAO.draw();

        // Finally draw the imgui stuff
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Turn the back side of the buffer over (or vice-versa)
        glfwSwapBuffers(window);
    }

    glDeleteTextures(1, &colorLookup.textureID);

    glDeleteFramebuffers(1, &mainFBO);

    glfwTerminate();
    return 0;
}

