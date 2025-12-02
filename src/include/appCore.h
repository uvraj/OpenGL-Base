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
        PlanarMesh planarMesh{1024, 1024};
        AuxData auxData;

        Shader final{"final", "final.vert", "final.frag"};
        Shader planarMeshShader{"genMesh", "genMesh.vert", "genMesh.frag"};

        PipelineManager pipeline{"pipeline.json"};

        GLuint mainFBO;

        void appInit() {
            glViewport(
                0, 0, // The location
                SCREEN_WIDTH, SCREEN_HEIGHT // The resolution
            );

            // Initialize ImGUI
            if(!ImGui::CreateContext()) {
                throw std::runtime_error(ERROR_HINT + "ImGui context creation failed.");
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
            planarMeshShader.create();
            planarMesh.init();

            // Handle FBs
            glCreateFramebuffers(1, &mainFBO);
                
            glNamedFramebufferTexture(mainFBO, GL_COLOR_ATTACHMENT0, pipeline.findTexture2DByName("albedoTex").getID(), 0);
            glNamedFramebufferTexture(mainFBO, GL_COLOR_ATTACHMENT1, pipeline.findTexture2DByName("normalTex").getID(), 0);
            glNamedFramebufferTexture(mainFBO, GL_DEPTH_ATTACHMENT, pipeline.findTexture2DByName("depthTex").getID(), 0);

            GLuint attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

            glNamedFramebufferDrawBuffers(mainFBO, 2, attachments);

            if (glCheckNamedFramebufferStatus(mainFBO, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
                printSuccess("Created main FBO.}\n");
            } else {
                throw std::runtime_error(ERROR_HINT + "Framebuffer incomplete.");
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

                if (ImGui::CollapsingHeader("Pipeline")) {
                    if (ImGui::Button("Reload Pipeline")) {
                        pipeline.reload();
                        final.load();
                        planarMeshShader.load();

                        glNamedFramebufferTexture(mainFBO, GL_COLOR_ATTACHMENT0, pipeline.findTexture2DByName("albedoTex").getID(), 0);
                        glNamedFramebufferTexture(mainFBO, GL_COLOR_ATTACHMENT1, pipeline.findTexture2DByName("normalTex").getID(), 0);
                        glNamedFramebufferTexture(mainFBO, GL_DEPTH_ATTACHMENT, pipeline.findTexture2DByName("depthTex").getID(), 0);
                    }
                }

                if (ImGui::CollapsingHeader("Settings")) {
                    ImGui::SliderFloat("Height Multiplier", &auxData.waveHeightMult, 0.0, 2.0);
                    ImGui::SliderFloat("Wind Speed", &auxData.windSpeed, 0.0, 30.0, "%f m/s");
                    ImGui::SliderFloat("Wave Field Size", &auxData.fieldSize, 0.0, 2048.0);
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
                
                glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
                glClearColor(0, 0, 0, 0);
                glClearDepth(1.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glEnable(GL_DEPTH_TEST);
                Texture2D& depthTex = pipeline.findTexture2DByName("depthTex");
                glViewport(0, 0, depthTex.getWidth(), depthTex.getHeight());
                planarMeshShader.useProgram();
                planarMeshShader.pushMat4Uniform("cameraViewMatrix", camera.viewMatrix);
                planarMeshShader.pushMat4Uniform("cameraViewMatrixInverse", camera.viewMatrixInverse);
                planarMeshShader.pushMat4Uniform("cameraProjectionMatrix", camera.projectionMatrix);
                planarMeshShader.pushMat4Uniform("cameraProjectionMatrixInverse", camera.projectionMatrixInverse);
                planarMeshShader.pushMat4Uniform("previousCameraViewMatrix", camera.viewMatrix);
                planarMeshShader.pushMat4Uniform("previousCameraViewMatrixInverse", camera.viewMatrixInverse);
                planarMeshShader.pushMat4Uniform("previousCameraProjectionMatrix", camera.projectionMatrix);
                planarMeshShader.pushMat4Uniform("previousCameraProjectionMatrixInverse", camera.projectionMatrixInverse);
                planarMeshShader.pushVec3Uniform("cameraPosition", camera.Position);
                planarMeshShader.pushVec3Uniform("previousCameraPosition", camera.previousPosition);
                planarMeshShader.pushFloatUniform("currentFrame", (float) window.currentFrame);
                planarMeshShader.pushBoolUniform("shouldAccumulate", window.shouldAccumulate);
                planarMeshShader.pushUnsignedIntUniform("frameIndex", window.frameIndex);
                planarMeshShader.pushUnsignedIntUniform("accumulationIndex", window.accumulationIndex);
                pipeline.findTexture2DByName("waveImgRe").bind(0);
                planarMeshShader.pushIntUniform("waveTexRe", 0);
                planarMesh.draw();

                pipeline.mainLoop(camera, window, auxData);

                // Post-processing pass
                glClearColor(0, 0, 0, 0);
                glClearDepth(1.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, 13, "Post Process");
                glDisable(GL_DEPTH_TEST);
                glViewport(0, 0, window.viewWidth, window.viewHeight);

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