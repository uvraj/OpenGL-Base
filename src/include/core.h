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
        Shader drawScene{"drawScene", "drawScene.vert", "drawScene.frag"};

        PipelineManager pipeline{"pipeline.json"};

        STLModel testModel{"test.stl"};

        GLuint mainFBO;

        void appInit() {
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
            drawScene.create();

            testModel.setup();

            glCreateFramebuffers(1, &mainFBO);

            glViewport(0, 0, 1920, 1080);

            glNamedFramebufferTexture(mainFBO, GL_COLOR_ATTACHMENT0, pipeline.findTexture2DByName("gBuffer").getID(), 0);
            glNamedFramebufferTexture(mainFBO, GL_DEPTH_ATTACHMENT, pipeline.findTexture2DByName("depthTex").getID(), 0);

            if (glCheckNamedFramebufferStatus(mainFBO, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                throw std::runtime_error("Framebuffer incomplete.");
            }    
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
                    // pipeline.reload();
                    drawScene.load();
                    final.load();
                }

                if (ImGui::Button("Write Image")) {
                    pipeline.findTexture2DByName("ptAccumulated").writeImageToDisk();
                }

                ImGui::End();

                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

                ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
                ImGui::SetNextWindowBgAlpha(0.35f);

                ImGui::Begin("Performance Metrics", nullptr, window_flags);
                ImGui::Text("FPS: %.2f", io->Framerate);
                ImGui::Text("Accumulated: %i", window.accumulationIndex);
                ImGui::Text("Camera Position:");
                ImGui::TextColored(ImVec4(1.0, 0.2, 0.2, 1.0), "X: %.2f", camera.Position.x);
                ImGui::TextColored(ImVec4(0.2, 1.0, 0.2, 1.0), "Y: %.2f", camera.Position.y);
                ImGui::TextColored(ImVec4(0.2, 0.2, 1.0, 1.0), "Z: %.2f", camera.Position.z); 
                ImGui::Text("Camera FoV:\t%.2f", camera.FoV);
                ImGui::Text("Camera Pitch:\t%.2f", camera.Pitch);
                ImGui::Text("Camera Yaw:\t%.2f", camera.Yaw);

                ImGui::End();

                // ImGui::ShowDemoWindow();

                ImGui::Render();

                pipeline.preRenderSetup();

                glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
                glEnable(GL_DEPTH_TEST);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                drawScene.useProgram();

                drawScene.pushMat4Uniform("cameraViewMatrix", camera.viewMatrix);
                drawScene.pushMat4Uniform("cameraViewMatrixInverse", camera.viewMatrixInverse);
                drawScene.pushMat4Uniform("cameraProjectionMatrix", camera.projectionMatrix);
                drawScene.pushMat4Uniform("cameraProjectionMatrixInverse", camera.projectionMatrixInverse);
                drawScene.pushMat4Uniform("previousCameraViewMatrix", camera.viewMatrix);
                drawScene.pushMat4Uniform("previousCameraViewMatrixInverse", camera.viewMatrixInverse);
                drawScene.pushMat4Uniform("previousCameraProjectionMatrix", camera.projectionMatrix);
                drawScene.pushMat4Uniform("previousCameraProjectionMatrixInverse", camera.projectionMatrixInverse);
                drawScene.pushVec3Uniform("cameraPosition", camera.Position);
                drawScene.pushVec3Uniform("previousCameraPosition", camera.previousPosition);
                drawScene.pushFloatUniform("currentFrame", (float) window.currentFrame);
                drawScene.pushBoolUniform("shouldAccumulate", window.shouldAccumulate);
                drawScene.pushUnsignedIntUniform("frameIndex", window.frameIndex);
                drawScene.pushUnsignedIntUniform("accumulationIndex", window.accumulationIndex);

                testModel.draw();

                pipeline.mainLoop(camera, window);

                // Post-processing pass
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glDisable(GL_DEPTH_TEST);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                pipeline.getFinalImageTexture().bind(0);

                final.useProgram();
                final.pushIntUniform("finalImage", 0);
                quadVAO.draw();

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