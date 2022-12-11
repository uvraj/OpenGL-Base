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

#define STBI_MAX_DIMENSIONS (1 << 26)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TITLE "OpenGL Base"

#define APPLICATION_GL_VERSION_MAJOR 4 
#define APPLICATION_GL_VERSION_MINOR 6

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#include "include/formatting.h"
#include "include/common.h"
#include "include/textures.h"

#include "include/OpenSans.h"
#include "include/camera.h"
#include "include/window.h"


class Application {
    public:
        void run() {
            appInit();
            appLoop();
            appExit();
        }
    
    private:
        Camera camera;
        Window window{&camera};
        ImGuiIO *io;

        // Initilize our OpenGL objects.
        Scene mainScene;
        Quad quadVAO;

        Shader mainSceneShader{"mainScene", "mainScene.vert", "mainScene.frag"};
        Shader postProcess{"postProcess", "postProcess.vert", "postProcess.frag"};

        Texture3D_bin colorLookup{"FUJI_ETERNA_250D_FUJI_3510.DAT", 2, 21, 21, 21, GL_RGB32F, GL_RGB, GL_FLOAT};

        GLuint mainFBO;
        GLuint sceneAlbedo, sceneDepth;

        bool useColorGrade = false;

        void appInit() {
            glViewport(
                0, 0, // The location
                SCREEN_WIDTH, SCREEN_HEIGHT // The resolution
            );

            // Initialize ImGUI
            if(!ImGui::CreateContext()) {
                printError("ImGui context creation failed!\n");
                assert(0);
            }

            else {
                printSuccess("ImGui context created!\n");
            }

            // Create an ImGui IO instance
            io = &ImGui::GetIO();
            setImGuiStyle(window.glfwWindow);

            
            // Shaders
            
            mainSceneShader.load();

            
            postProcess.load();

            // Textures
            
            // Texture2D testImage("testImage.png", 3);

            // Main Framebuffer
            
            glGenFramebuffers(1, &mainFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);

            
            glGenTextures(1, &sceneAlbedo);
            glGenTextures(1, &sceneDepth);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sceneAlbedo);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, nullptr);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, sceneDepth);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  

            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneAlbedo, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, sceneDepth, 0); 

            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                printError("Framebuffer incomplete!\n");
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);

            glEnable(GL_DEPTH_TEST);
        }

        void appLoop() {
            while (!window.shouldClose()) {
                window.renderLoop(io);
                camera.updateCameraData(window.aspectRatio);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::rotate(model, (float) window.currentFrame, glm::vec3(0.0f, 0.0f, 1.0f));
                model = glm::translate(model, glm::vec3(10.0f, 0.0, 0.0));

                // Start ImGUI frame
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                ImGui::SetNextWindowPos(ImVec2(1000.0f, 10.0f), ImGuiCond_Once);

                ImGui::Begin("Main Window");

                ImGui::Checkbox("Use Color Grade", &useColorGrade);

                if(ImGui::Button("Reload Shaders")) {
                    mainSceneShader.load();
                    postProcess.load();
                }

                ImGui::End();

                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

                ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
                ImGui::SetNextWindowBgAlpha(0.35f);

                ImGui::Begin("Performance Metrics", nullptr, window_flags);
                ImGui::Text("FPS: %.2f", io->Framerate);
                ImGui::Text("Camera Position:");
                ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "X: %.2f", camera.Position.x);
                ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "Y: %.2f", camera.Position.y);
                ImGui::TextColored(ImVec4(0.0, 0.0, 1.0, 1.0), "Z: %.2f", camera.Position.z); 
                ImGui::Text("Camera FoV:\t%.2f", camera.FoV);
                ImGui::Text("Camera Pitch:\t%.2f", camera.Pitch);
                ImGui::Text("Camera Yaw:\t%.2f", camera.Yaw);

                ImGui::End();

                ImGui::ShowDemoWindow();

                ImGui::Render();
                

                // Main Pass
                glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, 11, "Main Scene");
                glEnable(GL_DEPTH_TEST);
                glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
                glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                mainSceneShader.useProgram();
                mainSceneShader.pushMat4Uniform("cameraViewMatrix", camera.viewMatrix);
                mainSceneShader.pushMat4Uniform("cameraViewMatrixInverse", camera.viewMatrixInverse);
                mainSceneShader.pushMat4Uniform("cameraProjectionMatrix", camera.projectionMatrix);
                mainSceneShader.pushMat4Uniform("cameraProjectionMatrixInverse", camera.projectionMatrixInverse);
                mainSceneShader.pushMat4Uniform("modelMatrix", model);
                mainScene.draw();

                glPopDebugGroup();

                // Post-processing pass
                glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, 13, "Post Process");
                glDisable(GL_DEPTH_TEST);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, sceneAlbedo);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, sceneDepth);

                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_3D, colorLookup.textureID);

                postProcess.useProgram();

                postProcess.pushIntUniform("useColorGrade", useColorGrade); 

                quadVAO.draw();

                glPopDebugGroup();

                glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, 13, "ImGui Render");

                // Finally draw the imgui stuff
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                glPopDebugGroup();

                // Swap GLFW framebuffers
                window.swapBuffers();
            }
        }

        void appExit() {

        }
};

int main(void) {
    Application progMain;
    progMain.run();
    return 0;
}

