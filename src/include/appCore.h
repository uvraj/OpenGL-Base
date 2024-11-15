#pragma once

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
        Quad quadVAO;

        Shader final{"final", "final.vert", "final.frag"};

        glm::mat4 model{1.0f};

        PipelineManager pipeline{"pipeline.json"};

        GLuint mainFBO;

        void appInit() {
            stbi_flip_vertically_on_write(1);
            
            glViewport(
                0, 0, // The location
                SCREEN_WIDTH, SCREEN_HEIGHT // The resolution
            );      

            // Initialize ImGUI
            if(!ImGui::CreateContext()) {
                printError("ImGui context creation failed.\n");
                throw std::runtime_error("ImGui context creation failed.");
            }

            else {
                printSuccess("ImGui context created.\n");
            }

            // Create an ImGui IO instance
            io = &ImGui::GetIO();
            setImGuiStyle(window.glfwWindow, 1.0);

            // Start the ImGUI GLFW / OpenGL impl.
            ImGui_ImplGlfw_InitForOpenGL(window.glfwWindow, true);
            ImGui_ImplOpenGL3_Init("#version 330");

            pipeline.load();
            
            // Shaders
            final.create();
        }

        void appLoop() {
            while (!window.shouldClose()) {
                window.renderLoop(io);
                camera.updateCameraData(window.aspectRatio);

                // Start ImGUI frame
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                ImGui::SetNextWindowPos(ImVec2(1000.0f, 10.0f), ImGuiCond_Once);

                ImGui::Begin("Main Window");

                if (ImGui::Button("Reload Pipeline")) {
                    pipeline.reload();
                    final.load();
                }

                if (ImGui::Button("Write Image")) {
                    pipeline.findTexture2DByName("imageOut").writeImageToDiskPNG();
                }

                ImGui::End();

                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

                ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
                ImGui::SetNextWindowBgAlpha(0.35f);

                ImGui::Begin("Performance Metrics", nullptr, window_flags);
                ImGui::Text("FPS: %.2f", io->Framerate);

                ImGui::End();

                // ImGui::ShowDemoWindow();

                ImGui::Render();

                pipeline.mainLoop(camera, window);

                // Post-processing pass
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, 13, "Post Process");
                glDisable(GL_DEPTH_TEST);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                pipeline.getFinalImageTexture().bind(0);

                final.useProgram();
                final.pushIntUniform("finalImage", 0);
                quadVAO.draw();

                glPopDebugGroup();

                glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, 13, "ImGui Render");

                // Finally draw the imgui stuff
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                glPopDebugGroup();

                // Swap GLFW framebuffers
                window.swapBuffers();
                camera.writePreviousData();
            }
        }

        void appExit() {
            ImGui::DestroyContext();
        }
};