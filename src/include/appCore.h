#if !defined APP_CORE
#define APP_CORE

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

        glm::mat4 model = glm::mat4(1.0f);

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

            model = glm::translate(model, glm::vec3(mainScene.width / 2.0, 0.0, mainScene.height / 2.0));
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
                ImGui::Text("Vertices:\t%d", mainScene.numVerts);
                ImGui::Text("Memory:\t%2.f Bytes", mainScene.numVerts * 3.0 / 8.0);

                ImGui::End();

                ImGui::ShowDemoWindow();

                ImGui::Render();
                

                // Main Pass
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, 11, "Main Scene");
                glEnable(GL_DEPTH_TEST);
                glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, 13, "Post Process");
                glDisable(GL_DEPTH_TEST);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, sceneAlbedo);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, sceneDepth);

                glActiveTexture(GL_TEXTURE3);
                colorLookup.bind();

                postProcess.useProgram();

                postProcess.pushMat4Uniform("cameraViewMatrix", camera.viewMatrix);
                postProcess.pushMat4Uniform("cameraViewMatrixInverse", camera.viewMatrixInverse);
                postProcess.pushMat4Uniform("cameraProjectionMatrix", camera.projectionMatrix);
                postProcess.pushMat4Uniform("cameraProjectionMatrixInverse", camera.projectionMatrixInverse);
                postProcess.pushMat4Uniform("modelMatrix", model);
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
            ImGui::DestroyContext();
        }
};

#endif 