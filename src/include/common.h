#ifndef COMMON_H
#define COMMON_H

class Shader {
    // This class encapsulates everything shader-related
    // It covers shader creation, user outputting and uniform creation
    public:
        GLuint programID = glCreateProgram();
        GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
        GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);
        std::string programName;
        std::string vertexFileName;
        std::string fragmentFileName;

        Shader(const std::string currentProgramName, const std::string currentVSFileName, const std::string currentFSFileName) {
            programName = currentProgramName;
            vertexFileName = currentVSFileName;
            fragmentFileName = currentFSFileName;
        }

        ~Shader() {
            glDeleteShader(vsID);
            glDeleteShader(fsID);
            glDeleteProgram(programID);
        }

        void load() {
            /* 
            * This functions handles:
            * - loading shaders from a file
            * - compiling them
            * - linking them with a program
            */ 
            // Load the shaders from their respective file paths
            // A path prefix is passed from CMAKE, so that the user can choose where to load the shaders from
            // The path prefix is then concatenated with the shader name
            // I.e.: RESOURCE_PATH + FOLDER + SHADER_NAME = COMPLETE_SHADER_LOCATION
            // With these concatenation conventions, it is also possible to load our resources from the "src" path directly
            // without copying over the files

            glObjectLabel(GL_PROGRAM, programID, programName.length(), programName.c_str());
            glObjectLabel(GL_SHADER, vsID, vertexFileName.length(), vertexFileName.c_str());
            glObjectLabel(GL_SHADER, fsID, fragmentFileName.length(), fragmentFileName.c_str());

            std::string vertexFilePath = SHADER_PATH + vertexFileName;
            std::string fragmentFilePath = SHADER_PATH + fragmentFileName;

            // The shader text itself
            std::string vertexShader;
            std::string fragmentShader;

            // Create two instances of the fstream class, each with ios::in for read
            // VS
            std::ifstream vertexShaderFile(vertexFilePath);

            // FS
            std::ifstream fragmentShaderFile(fragmentFilePath); 
            
            // If the file loading succeeded
            if (vertexShaderFile.is_open()) {
                std::ostringstream oss;
                oss << vertexShaderFile.rdbuf();
                vertexShader = oss.str();
            }

            // If it didn't: 
            else {
                printError();
                std::cout << "Loading vertex shader from file ";
                std::cout << vertexFilePath;
                std::cout << " failed!\n";
                assert(vertexShaderFile.is_open());
            }
            
            // If the file loading succeeded
            if (fragmentShaderFile.is_open()) {
                std::ostringstream oss;
                oss << fragmentShaderFile.rdbuf();
                fragmentShader = oss.str();
            }

            // If it didn't: 
            else {
                printError();
                std::cout << "Loading fragment shader from file ";
                std::cout << fragmentFilePath;
                std::cout << " failed!\n";
                assert(fragmentShaderFile.is_open());
            }

            // Push our stuff into a const GLchar* so OpenGL (gcc) doesn't yell at me
            const GLchar *vertexShaderSource = vertexShader.c_str();
            const GLchar *fragmentShaderSource = fragmentShader.c_str();

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
        }

        void useProgram() {
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

        void pushUnsignedIntUniform(const GLchar *name, GLint value) {
            glUniform1ui(glGetUniformLocation(programID, name), value); 
        }

        void pushBoolUniform(const GLchar *name, GLboolean value) {
            glUniform1i(glGetUniformLocation(programID, name), value);
        }

        void pushVec2Uniform(const GLchar *name, GLfloat x, GLfloat y) {
            glUniform2f(glGetUniformLocation(programID, name), x, y);
        }

        void pushVec2Uniform(const GLchar *name, glm::vec2 vec){
            glUniform2fv(glGetUniformLocation(programID, name), 2, &vec[0]);
        }

        void pushIVec2Uniform(const GLchar *name, glm::ivec2 vec) {
            glUniform2iv(glGetUniformLocation(programID, name), 2, &vec[0]);
        }

        void pushVec3Uniform(const GLchar *name, GLfloat x, GLfloat y, GLfloat z) {
            glUniform3f(glGetUniformLocation(programID, name), x, y, z);
        }

        void pushVec4Uniform(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
            glUniform4f(glGetUniformLocation(programID, name), x, y, z, w);
        }

        void pushMat4Uniform(const GLchar *name, const glm::mat4 mat) {
            glUniformMatrix4fv(glGetUniformLocation(programID, name), 1, GL_FALSE, &mat[0][0]);
        }

    private:
        void checkErrors(GLuint fsID, GLuint vsID, GLuint programID) {
            // Print potential errors into the console
            int success = 0;
            GLchar infoLog[2048];

            // Check if compilation was successful
            glGetShaderiv(vsID, GL_COMPILE_STATUS, &success);

            // If it wasn't get the info log and print it!
            if (!success) {
                glGetShaderInfoLog(vsID, 512, NULL, infoLog);
                printError();
                std::cout << "OpenGL Output: \n" << infoLog << '\n';
            }

            // Continue doing this for the rest.

            glGetShaderiv(fsID, GL_COMPILE_STATUS, &success);

            if (!success) {
                glGetShaderInfoLog(fsID, 512, NULL, infoLog);
                printError();
                textYellow();
                std::cout << "OpenGL Output: \n" << infoLog << '\n';
                textReset();
            }


            glGetProgramiv(programID, GL_LINK_STATUS, &success);

            if (!success) {
                glGetProgramInfoLog(programID, 512, NULL, infoLog);
                printError();
                textYellow();
                std::cout << "OpenGL Output: \n" << infoLog << '\n';
                textReset();
            }
        }
};

class Quad {
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

        Quad() {
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

        ~Quad() {
            glDeleteBuffers(1, &vertexBufferObjects);
            glDeleteBuffers(1, &EBO);
            glDeleteVertexArrays(1, &vertexArrayObjects);
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

class Scene {
    public:
        GLuint EBO, VBO, VAO;

        GLfloat vertexData[42] = {
            -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f,
            0.0f, 0.5f, 0.0f,       0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, -1.0f,     0.5f, 0.5f, 0.5f,
            -1.0f, -1.0f, -1.0f,    0.5f, 0.5f, 0.5f,
            1.0f, -1.0f, 1.0f,      0.5f, 0.5f, 0.5f,
            -1.0f, -1.0f, 1.0f,      0.5f, 0.5f, 0.5f
        };

        GLuint indices[9] = {0, 1, 2, 3, 4, 5, 5, 6, 4};

        Scene() {
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

        ~Scene() {
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteVertexArrays(1, &VAO);
        }

        void draw() {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, NULL);
            glBindVertexArray(0);
        }

};

/*
This seemed like a good idea at first.
class FrameData {
    public:
        glm::ivec2 viewDimensions = glm::ivec2(SCREEN_WIDTH, SCREEN_HEIGHT);
        float aspectRatio = static_cast <float> (viewDimensions.x) / static_cast <float> (viewDimensions.y);
        float frameTime = 0.0f;
        float currentFrame = 0.0f;
        int frameIndex = 0;
        
};
*/

#endif