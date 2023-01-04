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
        Scene mainScene;
        Quad quadVAO;

        ComputeShader compTest{"test", "test.comp"};

        Shader mainSceneShader{"mainScene", "mainScene.vert", "mainScene.frag"};
        Shader postProcess{"postProcess", "postProcess.vert", "postProcess.frag"};

        std::vector <Texture> textureObjects;
        std::vector <ComputeShader> csObjects;

        Texture colorLookup{"FUJI_ETERNA_250D_FUJI_3510.DAT", 2, 21, 21, 21, GL_RGB32F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR};
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 lightAngles = glm::vec3(0.0f);
        glm::vec3 lightVector = glm::vec3(0.0f);

        GLuint mainFBO;
        GLuint sceneAlbedo, sceneDepth, compOut;

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
            //compTest.load();

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

            std::string jsonSource {};
            std::string filePath = "pipeline.json";
            filePath = RESOURCE_PATH + filePath;
            std::ifstream jsonFile(filePath);
            
            if(jsonFile.is_open()) {
                std::ostringstream oss;
                oss << jsonFile.rdbuf();
                jsonSource = oss.str();
            }

            rapidjson::Document pipelineDoc;
            pipelineDoc.Parse(jsonSource.c_str());
            
            const rapidjson::Value &textures = pipelineDoc["textures"];
            

            for(std::size_t i = 0; i < textures.Size(); i++) {
                std::string name            = textures[i]["name"].GetString();
                std::string source          = textures[i]["source"].GetString();
                std::string type            = textures[i]["type"].GetString();
                std::string internalFormat  = textures[i]["internalFormat"].GetString();
                std::string pixelFormat     = textures[i]["pixelFormat"].GetString();
                std::string pixelType       = textures[i]["pixelType"].GetString();
                std::string textureWrap     = textures[i]["textureWrap"].GetString();
                std::string filter          = textures[i]["filter"].GetString();
                unsigned int textureUnit    = i + 5;
                unsigned int width          = textures[i]["width"].GetUint();
                unsigned int height         = textures[i]["height"].GetUint();

                TextureTypes textureType = getTypeFromString(type);

                if(textureType == TEXTURE_2D) {
                    Texture currentTexture(
                        source, 
                        textureUnit, 
                        getWrapParamFromString(textureWrap),
                        getFilterParamFromString(filter)
                    );
                    textureObjects.push_back(currentTexture);
                }
            }

            /*const rapidjson::Value &passes = pipelineDoc["passes"];

            for(std::size_t i = 0; i < passes.Size(); i++) {
                ComputeShader currentCS {};
                currentCS.name = passes[i]["name"].GetString();
                currentCS.computeSource = passes[i]["computeSource"].GetString();
                currentCS.dispatchSizeX = passes[i]["dispatchSizeX"].GetUint();
                currentCS.dispatchSizeY = passes[i]["dispatchSizeY"].GetUint();

                const rapidjson::Value &boundImages = passes[i]["imagesBound"];

                std::vector <std::string> boundImagesNames{};
                
                for(std::size_t i = 0; i < boundImages.Size(); i++) {
                    boundImagesNames.push_back(boundImages[i].GetString());
                }

                for(std::size_t i = 0; i < boundImagesNames.size(); i++) {
                    std::size_t k;
                    for(k = 0; k < textureObjects.size(); k++) {
                        if(boundImagesNames[i] == textureObjects[k].name) {
                            currentCS.imagesBound.push_back(textureObjects[k].textureID);
                            std::cout << k;
                        }
                    }
                }
            }*/

            /*glGenTextures(1, &compOut);
            glActiveTexture(GL_TEXTURE5);
            glBindTexture(GL_TEXTURE_2D, compOut);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 500, 500, 0, GL_RGBA, GL_FLOAT, nullptr);

            glBindImageTexture(5, compOut, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);*/

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

                ImGui::SliderFloat3("Light Vector Direction", &lightAngles[0], 0.0f, 360.0f, "%.2f°");

                ImGui::Checkbox("Use Color Grade", &useColorGrade);

                if(ImGui::Button("Reload Shaders")) {
                    mainSceneShader.load();
                    postProcess.load();
                    compTest.load();
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
                ImGui::Text("Vertices:\t%d", (int) mainScene.numVerts);
                ImGui::Text("Triangles:\t%d", (int) mainScene.numTriangles);
                ImGui::Text("Size:\t%.2f MB", (mainScene.numVerts * 3.0 * sizeof(float)) * 1e-6);

                ImGui::End();

                // ImGui::ShowDemoWindow();

                ImGui::Render();

                // Update misc. data
                lightVector = (glm::mat3) (glm::rotate(glm::mat4(1.0f), glm::radians(lightAngles.x), glm::vec3(1.0f, 0.0f, 0.0f))) * glm::vec3(0.0f, 1.0f, 0.0f);
                lightVector = (glm::mat3) (glm::rotate(glm::mat4(1.0f), glm::radians(lightAngles.y), glm::vec3(0.0f, 1.0f, 0.0f))) * lightVector;
                lightVector = (glm::mat3) (glm::rotate(glm::mat4(1.0f), glm::radians(lightAngles.z), glm::vec3(0.0f, 0.0f, 1.0f))) * lightVector;

                // Main Pass
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, 11, "Main Scene");
                glEnable(GL_DEPTH_TEST);
                glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                mainSceneShader.useProgram();
                mainSceneShader.pushVec3Uniform("lightVector", lightVector);
                mainSceneShader.pushMat4Uniform("cameraViewMatrix", camera.viewMatrix);
                mainSceneShader.pushMat4Uniform("cameraViewMatrixInverse", camera.viewMatrixInverse);
                mainSceneShader.pushMat4Uniform("cameraProjectionMatrix", camera.projectionMatrix);
                mainSceneShader.pushMat4Uniform("cameraProjectionMatrixInverse", camera.projectionMatrixInverse);
                mainSceneShader.pushMat4Uniform("modelMatrix", model);
                mainScene.draw();

                glPopDebugGroup();

                /*glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, sceneAlbedo);

                glActiveTexture(GL_TEXTURE2);
                colorLookup.bind();

                textureObjects[0].bind();

                compTest.use();

                compTest.pushIntUniform("sceneAlbedo", 0);
                compTest.pushIntUniform("colorLookup", 3);
                compTest.pushIntUniform("heightMap", textureObjects[0].texUnit);

                glDispatchCompute(500, 500, 1);
                glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
                */
                // Post-processing pass
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, 13, "Post Process");
                glDisable(GL_DEPTH_TEST);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, compOut);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, sceneDepth);

                glActiveTexture(GL_TEXTURE3);
                colorLookup.bind();

                textureObjects[0].bind();

                postProcess.useProgram();

                postProcess.pushMat4Uniform("cameraViewMatrix", camera.viewMatrix);
                postProcess.pushMat4Uniform("cameraViewMatrixInverse", camera.viewMatrixInverse);
                postProcess.pushMat4Uniform("cameraProjectionMatrix", camera.projectionMatrix);
                postProcess.pushMat4Uniform("cameraProjectionMatrixInverse", camera.projectionMatrixInverse);
                postProcess.pushMat4Uniform("modelMatrix", model);
                postProcess.pushIntUniform("useColorGrade", useColorGrade); 
                postProcess.pushIntUniform("sceneAlbedo", 0);
                postProcess.pushIntUniform("sceneDepth", 1);
                postProcess.pushIntUniform("colorLookup", 3);
                postProcess.pushIntUniform("heightMap", textureObjects[0].texUnit);
            
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