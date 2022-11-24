#ifndef COMMON_H
#define COMMON_H

class shader {
    // This class encapsulates everything shader-related
    // It covers shader creation, user outputting and uniform creation
    public:
        GLuint programID = glCreateProgram();
        GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
        GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);
        
        void load(const GLchar* vertexFileName, const GLchar *fragmentFileName) {
            /* 
            * This functions handles:
            * - loading shaders from a file
            * - compiling them
            * - linking them with a program
            */ 
            // Load the shaders from their respective file paths
            // A path prefix is passed from CMAKE, so that the user can choose where to load the shaders from
            // This is also required for MSVC support, since MS doesn't put the exectuable directly into the build folder!
            // The path prefix is then concatenated with the shader name
            // I.e.: RESOURCE_PATH + FOLDER + SHADER_NAME = COMPLETE_SHADER_LOCATION
            // With these concatenation conventions, it is also possible to load our resources from the "src" path directly
            // without copying over the files

            // glObjectLabel(GL_PROGRAM, programID, strlen(programName), programName);
            // glObjectLabel(GL_VERTEX_SHADER, vsID, strlen(vertexFileName), vertexFileName);
            // glObjectLabel(GL_FRAGMENT_SHADER, fsID, strlen(fragmentFileName), fragmentFileName);

            GLchar vertexFilePath[128] = SHADER_PATH;
            GLchar fragmentFilePath[128] = SHADER_PATH;

            strcat(vertexFilePath, vertexFileName);
            strcat(fragmentFilePath, fragmentFileName);

            // This will hold out vertex shader data
            GLchar *vertexShaderBuffer = NULL;
            // This will hold the size / length of the vertex shader data
            unsigned long vertexFileLength = 0;

            // Do the same for the fragment shader.
            GLchar *fragmentShaderBuffer = NULL;
            unsigned long fragmentFileLength = 0;

            // Create two instances of the FILE struct, each with "r" for "read"
            FILE *vertexShaderFile = std::fopen(vertexFilePath, "r");
            FILE *fragmentShaderFile = std::fopen(fragmentFilePath, "r");
            
            // If the FILE loading succeeded:
            if (vertexShaderFile) {
                // Check the return value of fseek().
                // Sets the FILE position to the end
                if(fseek(vertexShaderFile, 0, SEEK_END)) {
                    printError(); 
                    std::printf("fseek() failed.");
                }

                // Get the length of the vertex shader file.    
                vertexFileLength = ftell(vertexShaderFile);

                // Check the return value of fseek() again.
                // Sets the FILE position to the beginning
                if(fseek(vertexShaderFile, 0, SEEK_SET)) {
                    printError(); 
                    std::printf("fseek() failed.");
                }

                // Allocate the required amount of memory. Plus 1 for the null terminator
                vertexShaderBuffer = (GLchar*) malloc(vertexFileLength + 1);

                // Variable for remembering the end index for the null terminator
                size_t endPos = 0;

                // If the memory allocation succeded:
                // read the file and set endPos.
                if (vertexShaderBuffer) {
                    endPos = std::fread(vertexShaderBuffer, 1, vertexFileLength, vertexShaderFile);
                }

                // If it didn't: 
                else {
                    printError();
                    std::printf("vertex memory allocation failed.");
                }

                // Add the null terminator
                vertexShaderBuffer[endPos] = '\0';
                
                // Close the file. We're done.
                fclose(vertexShaderFile);
            }

            //If it didn't: 
            else {
                printError();
                std::printf("Loading vertex shader from file ");
                std::printf(vertexFilePath);
                std::printf(" failed!\n");
                return;
            }
            
            // If the FILE loading succeeded:
            if (fragmentShaderFile) {
                // Check the return value of fseek().
                // Sets the FILE position to the end
                if(fseek(fragmentShaderFile, 0, SEEK_END)) {
                    printError(); 
                    std::printf("fseek() failed.");
                }

                // Get the length of the fragment shader file.    
                fragmentFileLength = ftell(fragmentShaderFile);

                // Check the return value of fseek() again.
                // Sets the FILE position to the beginning
                if(fseek(fragmentShaderFile, 0, SEEK_SET)) {
                    printError(); 
                    std::printf("fseek() failed.");
                }

                // Allocate the required amount of memory. Plus 1 for the null terminator
                fragmentShaderBuffer = (GLchar*) malloc(fragmentFileLength + 1);

                // Variable for remembering the end index for the null terminator
                size_t endPos = 0;

                // If the memory allocation succeded:
                // read the file and set endPos.
                if (fragmentShaderBuffer) {
                    endPos = std::fread(fragmentShaderBuffer, 1, fragmentFileLength, fragmentShaderFile);
                }

                // If it didn't: 
                else {
                    printError();
                    std::printf("fragment memory allocation failed.");
                }

                // Add the null terminator
                fragmentShaderBuffer[endPos] = '\0';
                
                // Close the file. We're done.
                fclose(fragmentShaderFile);
            }

            // If it didn't: 
            else {
                printError();
                std::printf("Loading fragment shader from file ");
                std::printf(fragmentFilePath);
                std::printf(" failed!\n");
                return;
            }

            // Push our stuff into a const GLchar* so OpenGL (gcc) doesn't yell at me
            const GLchar *vertexShaderSource = vertexShaderBuffer;
            const GLchar *fragmentShaderSource = fragmentShaderBuffer;

            // Attach the shader source to the created ID
            glShaderSource(vsID, 1, &vertexShaderSource, NULL);
            glCompileShader(vsID);

            // Do the same for the fragment shader
            glShaderSource(fsID, 1, &fragmentShaderSource, NULL);
            glCompileShader(fsID);

            // Attach the previously compiled shaders to the shader program ID and link
            glAttachShader(programID, vsID);
            glAttachShader(programID, fsID);
            glLinkProgram(programID);

            // Check for errors
            checkErrors(fsID, vsID, programID);

            // And finally: Free our stuff.
            free(vertexShaderBuffer);
            free(fragmentShaderBuffer);
        }


        void useProgram(){
            glUseProgram(programID);
        }
    
        // Various helpers for uniforms
        // TODO: Implement ALL data types
        void pushFloatUniform(const GLchar* name, GLfloat value) {
            glUniform1f(glGetUniformLocation(programID, name), value); 
        }

        void pushIntUniform(const GLchar* name, GLint value) {
            glUniform1i(glGetUniformLocation(programID, name), value);
        }

        void pushBoolUniform(const GLchar *name, GLboolean value) {
            glUniform1i(glGetUniformLocation(programID, name), value);
        }

        void pushUnsignedIntUniform(const GLchar *name, GLint value) {
            glUniform1ui(glGetUniformLocation(programID, name), value); 
        }

        void pushVec2Uniform(const GLchar *name, GLfloat x, GLfloat y) {
            glUniform2f(glGetUniformLocation(programID, name), x, y);
        }

        void pushVec3Uniform(const GLchar *name, GLfloat x, GLfloat y, GLfloat z) {
            glUniform3f(glGetUniformLocation(programID, name), x, y, z);
        }

        void pushVec4Uniform(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
            glUniform4f(glGetUniformLocation(programID, name), x, y, z, w);
        }

        void pushMat4Uniform(const GLchar *name, const glm::mat4 &mat) {
            glUniformMatrix4fv(glGetUniformLocation(programID, name), 1, GL_FALSE, &mat[0][0]);
        }

    private:
        void checkErrors(GLuint fsID, GLuint vsID, GLuint programID) {
            // Print potential errors into the console
            int success = 0;
            GLchar infoLog[1024];

            // Check if compilation was successful
            glGetShaderiv(vsID, GL_COMPILE_STATUS, &success);

            // If it wasn't get the info log and print it!
            if (!success) {
                glGetShaderInfoLog(vsID, 512, NULL, infoLog);
                printError();
                std::printf("OpenGL Output: \n");
                std::printf(infoLog);
                std::printf("\n");
            }

            // Continue doing this for the rest.

            glGetShaderiv(fsID, GL_COMPILE_STATUS, &success);

            if (!success) {
                glGetShaderInfoLog(fsID, 512, NULL, infoLog);
                printError();
                std::printf("OpenGL Output: \n");
                std::printf(infoLog);
                std::printf("\n");
            }


            glGetProgramiv(programID, GL_LINK_STATUS, &success);

            if (!success) {
                glGetProgramInfoLog(programID, 512, NULL, infoLog);
                printError();
                std::printf("OpenGL Output: \n");
                std::printf(infoLog);
                std::printf("\n");
            }
        }
};

class quad {
    /*
     * This class handles:
     * creating all the OpenGL data necessary for drawing a quad
     * actually drawing the data via glDrawElements
    */ 
    public:
        GLfloat quadData[20] = {
        // vertex data     // normalized texture sampling coords
        -1.00f, -1.00f,    0.0f, 0.0f,
         1.00f, -1.00f,    1.0f, 0.0f,
         1.00f,  1.00f,    1.0f, 1.0f, 
        -1.00f,  1.00f,    0.0f, 1.0f
        };

        // The order in which we want the vertex data to be drawn in.
        // !I'll have one quad, with extra mayonnaise, please!
        GLuint quadOrder[6] = {
            0, 1, 2, 2, 3, 0
        };
        
        // This stuff shall hold the ID for the magic sauce
        GLuint vertexBufferObjects = 0;
        GLuint vertexArrayObjects  = 0;
        GLuint EBO = 0;

        quad() {
            // Generate the required stuff
            glGenBuffers(1, &vertexBufferObjects);
            glGenBuffers(1, &EBO);
            glGenVertexArrays(1, &vertexArrayObjects);

            // Bind the VAO
            glBindVertexArray(vertexArrayObjects);

            // Bind the VBO and fill it.
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadData), quadData, GL_STATIC_DRAW);

            // Bind the EBO and fill it.
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadOrder), quadOrder, GL_DYNAMIC_DRAW);

            // Add and push the vertex attributes
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*) (2 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);

            // Unbind everything
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void draw() {
            // Actually draw everything.
            // Bind the VAO and the EBO. Draw our Elements
            glBindVertexArray(vertexArrayObjects);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
            glBindVertexArray(0);
        }

};

class scene {
    public:
        GLuint EBO, VBO, VAO;

        GLfloat vertexData[18] = {
            -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f,
            0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 1.0f
        };

        GLuint indices[3] = {0, 1, 2};

        scene() {
            // Generate the required objects.
            glGenBuffers(1, &EBO);
            glGenBuffers(1, &VBO);
            glGenVertexArrays(1, &VAO);

            // Bind the VAO
            glBindVertexArray(VAO);

            // Bind the VBO, fill some data in it
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

            // Same for the EBO
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            // Setup our vertex attributes
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) 0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);

            // Unbind everything.
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void draw() {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
            glBindVertexArray(0);
        }

};

#endif