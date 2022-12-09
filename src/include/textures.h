#ifndef TEXTURES_H
#define TEXTURES_H

class Texture2D {
    public: 
        GLuint textureID = 0;

        Texture2D(const std::string fileName, GLuint texUnit){
            std::string filePath = RESOURCE_PATH + fileName;

            // Tell STB that I want to flip the y coord
            stbi_set_flip_vertically_on_load(true);

            // Image parameters.
            int width, height, nrChannels;
            std::uint8_t *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 4);

            if(!data) {
                printError();
                std::cout << "Loading texture %s failed!\n";
                assert(data);
            }

            glGenTextures(1, &textureID);
        
            glActiveTexture(GL_TEXTURE0 + texUnit);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        ~Texture2D() {
            glDeleteTextures(1, &textureID);
        }

        void bind() {
            glBindTexture(GL_TEXTURE_2D, textureID);
        }
};

class Texture2D_bin {
    public:
        GLuint textureID = 0;

        Texture2D_bin(
            const std::string fileName, 
            const GLuint texUnit, 
            const GLuint width, 
            const GLuint height, 
            const GLuint internalFormat, 
            const GLuint format, 
            const GLuint pixelType,
            const GLuint wrapParam = GL_CLAMP_TO_EDGE,
            const GLuint filterParam = GL_LINEAR
        ) {
            std::string filePath = RESOURCE_PATH + fileName;
            std::ifstream binaryFile(filePath, std::ios::binary);

            std::vector <std::uint8_t> binaryData;
            binaryData.assign(std::istreambuf_iterator <char> (binaryFile), {});

            if(!binaryFile.is_open()) {
                printError();
                std::cout << "Loading binary 2D texture " << filePath << " failed!\n";
                assert(0);
            }

            glGenTextures(1, &textureID);

            glActiveTexture(GL_TEXTURE0 + texUnit);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, pixelType, &binaryData[0]);

            glBindTexture(GL_TEXTURE_2D, 0);
        }


        ~Texture2D_bin() {
            glDeleteTextures(1, &textureID); 
        }

        void bind() {
            glBindTexture(GL_TEXTURE_2D, textureID);
        }

};

class Texture3D_bin {
    public:
        GLuint textureID = 0;

        Texture3D_bin(
            const std::string fileName, 
            const GLuint texUnit, 
            const GLuint width, 
            const GLuint height, 
            const GLuint depth,  
            const GLuint internalFormat, 
            const GLuint format, 
            const GLuint pixelType,
            const GLuint wrapParam = GL_CLAMP_TO_EDGE,
            const GLuint filterParam = GL_LINEAR
        ) {
            std::string filePath = RESOURCE_PATH + fileName;
            std::ifstream binaryFile(filePath, std::ios::binary);

            std::vector <std::uint8_t> binaryData;
            binaryData.assign(std::istreambuf_iterator <char> (binaryFile), {});

            if(!binaryFile.is_open()) {
                printError();
                std::cout << "Loading binary 3D texture " << filePath << " failed!\n";
                assert(0);
            }

            glGenTextures(1, &textureID);

            glActiveTexture(GL_TEXTURE0 + texUnit);
            glBindTexture(GL_TEXTURE_3D, textureID);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrapParam);	
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrapParam);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrapParam);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filterParam);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filterParam);

            glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, height, depth, 0, format, pixelType, &binaryData[0]);

            glBindTexture(GL_TEXTURE_3D, 0);
        }


        ~Texture3D_bin() {
            glDeleteTextures(1, &textureID);
        }

        void bind() {
            glBindTexture(GL_TEXTURE_3D, textureID);
        }

};

#endif