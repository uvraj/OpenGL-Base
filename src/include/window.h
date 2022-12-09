#ifndef WINDOW_H
#define WINDOW_H

class Window {
    public:
        GLFWwindow *glfwWindow = nullptr;
        Camera *camera = nullptr;

        long double currentFrame = 0.0f;
        long double lastFrame = 0.0f;

        float lastX = SCREEN_WIDTH / 2.0f;
        float lastY = SCREEN_HEIGHT / 2.0f;
        float frameTime = 0.0f;
        float aspectRatio = 1.0f;

        int viewWidth = SCREEN_WIDTH;
        int viewHeight = SCREEN_HEIGHT;
        int frameIndex = 0.0f;

        bool mouseCaught = true;
        bool firstMouse = true;

        Window(Camera *currentCamera) {
            // Initialize GLFW
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, APPLICATION_GL_VERSION_MAJOR);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, APPLICATION_GL_VERSION_MINOR);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

            // Create a window object with the following resolution and title
            glfwWindow = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE, nullptr, nullptr); 

            if (!glfwWindow) {
                printError();
                std::printf("GLFW window creation failed.\n");
                glfwTerminate();
                assert(0);
            }

            // Make "window" the main context on the current thread
            glfwMakeContextCurrent(glfwWindow);

            // Pass GLAD the function to load the address of OS-specific OPENGL function pointers.
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                printError();
                std::printf("GLAD initialization failed.\n");
                assert(0);
            }

            printSuccess();
            std::cout << "Window and OpenGL context created!\n";

            glfwSetWindowUserPointer(glfwWindow, this);

            // Handle callbacks.
            glfwSetFramebufferSizeCallback(glfwWindow, frameBufferSizeCallBack);
            glfwSetKeyCallback(glfwWindow, keyCallback);
            glfwSetCursorPosCallback(glfwWindow, mouseCallback);
            glfwSetScrollCallback(glfwWindow, scrollCallback);

            camera = currentCamera;
        }

        ~Window() {
            glfwTerminate();
        }

        static void frameBufferSizeCallBack(GLFWwindow *window, int width, int height) {
            // Zhis function handles the user changing the resolution of the window.
            // Personal note: ALL glTexImage2D() calls related to framebuffer attachments MUST be placed inside here,
            // otherwise they won't resize!
            glViewport(0, 0, width, height);
            glActiveTexture(GL_TEXTURE0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
            glActiveTexture(GL_TEXTURE1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
        }

        static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
            float xpos = (float) xposIn;
            float ypos = (float) yposIn;

            Window *windowptr = (Window*) glfwGetWindowUserPointer(window);

            if (windowptr->firstMouse) {
                windowptr->lastX = xpos;
                windowptr->lastY = ypos;
                windowptr->firstMouse = false;
            }

            float xoffset = xpos - windowptr->lastX;
            float yoffset = windowptr->lastY - ypos; // reversed since y-coordinates go from bottom to top

            windowptr->lastX = xpos;
            windowptr->lastY = ypos;

            if(windowptr->mouseCaught)
                windowptr->camera->ProcessMouseMovement(xoffset, yoffset);
        }

        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
            Window *windowptr = (Window*) glfwGetWindowUserPointer(window);
            windowptr->camera->ProcessMouseScroll((float) yoffset);
        }

        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
            Window *windowptr = (Window*) glfwGetWindowUserPointer(window);
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                windowptr->mouseCaught = !windowptr->mouseCaught;
        }

        void renderLoop(ImGuiIO &io){
            handleDPI(io);
            currentFrame = glfwGetTime();
            frameTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            glfwPollEvents();
            handleInput();
            glfwGetFramebufferSize(glfwWindow, &viewWidth, &viewHeight);
            aspectRatio = (float) viewWidth / (float) viewHeight;

            if(mouseCaught) {
            // Capture our mouse
                glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }

            else {
                glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            camera->updateCameraData(aspectRatio);
        }

        void swapBuffers() {
            glfwSwapBuffers(glfwWindow);
        }

    private:
        void handleInput() {
            if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
                camera->ProcessKeyboard(FORWARD, frameTime);
            if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
                camera->ProcessKeyboard(BACKWARD, frameTime);
            if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
                camera->ProcessKeyboard(LEFT, frameTime);
            if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
                camera->ProcessKeyboard(RIGHT, frameTime);
        }

        void handleDPI(ImGuiIO &io) {
            static float prevScale = 1.0;
            /*
            * This function handles:
            * - DPI
            *  -> Resizing everything upon detecting a GLFW scale change
            TODO: Only works on Windows!
            This is because GLFW doesn't implement glfwGetContentScale on Linux or Mac windowing systems.
            */

            // Keep track of the x scale
            // We don't need the y scale. One scale suffices. Pass nullptr.
            float xScale;
            glfwGetWindowContentScale(glfwWindow, &xScale, nullptr);

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
                std::cout << "Window scale changed -> rebuilt font atlas.\n";
            }
        }
};


void setImGuiStyle(GLFWwindow *glfwWindow) {
    // Create a style instance
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 7.5f; // Round our windows a bit
    
    // Start the ImGUI GLFW / OpenGL impl.
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 420 core");
}

#endif