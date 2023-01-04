#pragma once
class ShaderText {
    public:
        static std::string getSource(const std::string filePath) {
            const std::string incIdentifier {"#include"};
            std::string source {};
            std::ifstream file {filePath};

            if(!file.is_open()) {
                printError("Error loading shader from ");
                std::cout << filePath << '\n';
                return source;
            }

            std::string buffer {};

            while(std::getline(file, buffer)) {
                if(buffer.find(incIdentifier) != buffer.npos) {
                    buffer.erase(0, incIdentifier.size() + 1);
                    processFileName(buffer);
                    std::ifstream includeFile(SHADER_PATH + buffer);

                    if(!includeFile.is_open()) {
                        printError("Failed resolving #include directive ");
                        std::cout << buffer << '\n';
                    }

                    buffer.erase(0, buffer.size());

                    if(includeFile.is_open()) {
                        std::ostringstream oss;
                        oss << includeFile.rdbuf();
                        buffer += oss.str();
                    }
                }

                source += buffer + '\n';
            }

            source += '\0';
            return source;
        }

    private:
        static void processFileName(std::string &currentFileName) {
            size_t loc1 = currentFileName.find_first_of('"');
            size_t loc2 = currentFileName.find_last_of('"');
            currentFileName.erase(loc1, 1);
            currentFileName.erase(loc2 - 1, 1);
        }
};

class Shader {
    // This class encapsulates everything shader-related
    // It covers shader creation, user outputting and uniform creation
    public:
        // Constructor. Populates the above strings
        Shader(const std::string currentProgramName, const std::string currentVSFileName, const std::string currentFSFileName) {
            programName = currentProgramName;
            vertexFileName = currentVSFileName;
            fragmentFileName = currentFSFileName;
        }
        
        // Destructor. Deletes shaders.
        ~Shader() {
            glDeleteShader(vsID);
            glDeleteShader(fsID);
            glDeleteProgram(programID);
            printInfo("Destroyed program ");
            std::cout << programName << '\n';
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

            // Label all OpenGL objects
            glObjectLabel(GL_PROGRAM, programID, programName.length(), programName.c_str());
            glObjectLabel(GL_SHADER, vsID, vertexFileName.length(), vertexFileName.c_str());
            glObjectLabel(GL_SHADER, fsID, fragmentFileName.length(), fragmentFileName.c_str());

            // Concatenate the SHADER_PATH macro with the file names to create the final file path 
            // the shaders will be loaded from
            std::string vertexFilePath = SHADER_PATH + vertexFileName;
            std::string fragmentFilePath = SHADER_PATH + fragmentFileName;

            std::string vertexShader = ShaderText::getSource(vertexFilePath);
            std::string fragmentShader = ShaderText::getSource(fragmentFilePath);

            // Push our stuff into a const GLchar* so OpenGL (gcc) doesn't yell at me
            const GLchar *vertexShaderSource = vertexShader.c_str();
            const GLchar *fragmentShaderSource = fragmentShader.c_str();

            // Provide the shader source
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
            checkErrors(programID);
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

        void pushVec3Uniform(const GLchar *name, const glm::vec3 vec) {
            glUniform3fv(glGetUniformLocation(programID, name), 1, &vec[0]);
        }

        void pushVec4Uniform(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
            glUniform4f(glGetUniformLocation(programID, name), x, y, z, w);
        }

        void pushMat4Uniform(const GLchar *name, const glm::mat4 mat) {
            glUniformMatrix4fv(glGetUniformLocation(programID, name), 1, GL_FALSE, &mat[0][0]);
        }

    protected:
        Shader() {
            
        }

        void checkErrors(GLuint programID) {
            // Print potential errors into the console
            int success = 0;
            GLchar infoLog[2048] = {0};

            glGetProgramiv(programID, GL_LINK_STATUS, &success);

            if (!success) {
                glGetProgramInfoLog(programID, 2048, NULL, infoLog);
                printError();
                textYellow();
                std::cout << "OpenGL Output: \n" << infoLog << '\n';
                textReset();
            }
        }

    private:
        // OpenGL objects
        GLuint programID = glCreateProgram();
        GLuint vsID = glCreateShader(GL_VERTEX_SHADER);
        GLuint fsID = glCreateShader(GL_FRAGMENT_SHADER);

        // Various strings used throughout the class
        std::string programName = "";
        std::string vertexFileName = "";
        std::string fragmentFileName = "";
};

class ComputeShader : public Shader {
    // This class encapsulates compute shaders.
    public:
        ComputeShader(const std::string currentProgramName, const std::string currentFileName) {
            programName = currentProgramName;
            fileName = currentFileName;
        } 

        ~ComputeShader() {
            glDeleteShader(csID);
            glDeleteProgram(programID);
        }

        void load() {
            glObjectLabel(GL_SHADER, csID, -1, fileName.c_str());
            glObjectLabel(GL_PROGRAM, programID, -1, programName.c_str());
            
            const std::string filePath = SHADER_PATH + fileName;
            std::string computeShader = ShaderText::getSource(filePath);

            const char* computeShaderSource = computeShader.c_str();
            glShaderSource(csID, 1, &computeShaderSource, NULL);
            glCompileShader(csID);

            glAttachShader(programID, csID);
            glLinkProgram(programID);
            checkErrors(programID);
        }

        void use() {
            glUseProgram(programID);
        }
    
    private:
        // OpenGL Objects
        GLuint programID = glCreateProgram();
        GLuint csID = glCreateShader(GL_COMPUTE_SHADER);

        // Various strings used throughout the class
        std::string fileName = "";
        std::string programName = "";
};