#ifndef TEXTURES_H
#define TEXTURES_H

class texture2D {
    public: 
        GLuint textureID = 0;

        texture2D(const GLchar *filePath, GLuint texUnit){
            GLchar completeFilePath[128] = RESOURCE_PATH;
            strcat(completeFilePath, filePath);

            // Image parameters.
            int width, height, nrChannels;
            unsigned char *data = stbi_load(completeFilePath, &width, &height, &nrChannels, 4);

            if(!data) {
                printError();
                std::printf("Loading texture %s failed!\n", filePath);
                return;
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
};


class texture2D_binaryDump {
    public:
        GLuint textureID = 0;
        GLuint elemSizeInBytes = 0;
        FILE *binaryFile = NULL;
        unsigned char *binaryData = NULL;
        
        texture2D_binaryDump(const GLchar *filePath, const GLuint texUnit, const GLuint width, const GLuint height, const GLuint channels, const GLuint internalFormat, const GLuint format, const GLuint pixelType) {

            switch(pixelType) {
                case GL_HALF_FLOAT:
                     elemSizeInBytes = 2;
                     break;
                case GL_FLOAT:
                     elemSizeInBytes = 4;
                     break;
            }

            GLchar completeFilePath[128] = RESOURCE_PATH;
            strcat(completeFilePath, filePath);

            binaryData = (unsigned char*) malloc(width * height * channels * elemSizeInBytes);

            if(!binaryData) {
                printError();
                std::printf("Allocating binary 2D texture %s failed!", filePath);
                return;
            }

            binaryFile = std::fopen(completeFilePath, "rb");
            std::fread(binaryData, 1, width * height * channels * elemSizeInBytes, binaryFile);

            if(!binaryData) {
                printError();
                std::printf("Loading binary 2D texture %s failed!", filePath);
                return;
            }

            glGenTextures(1, &textureID);

            glActiveTexture(GL_TEXTURE0 + texUnit);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, pixelType, binaryData);
            glGenerateMipmap(GL_TEXTURE_2D);

            free(binaryData);

            glBindTexture(GL_TEXTURE_2D, 0);
        }

};


class texture3D_binaryDump {
    public:
        GLuint textureID = 0;
        GLuint elemSizeInBytes = 0;
        FILE *binaryFile = NULL;
        unsigned char *binaryData = NULL;
        
        texture3D_binaryDump(
            const GLchar *filePath, 
            const GLuint texUnit, 
            const GLuint width, 
            const GLuint height, 
            const GLuint depth, 
            const GLuint channels, 
            const GLuint internalFormat, 
            const GLuint format, 
            const GLuint pixelType
        ) {

            // TODO: All pixel types
            switch(pixelType) {
                case GL_HALF_FLOAT:
                     elemSizeInBytes = 2;
                     break;
                case GL_FLOAT:
                     elemSizeInBytes = 4;
                     break;
            }

            GLchar completeFilePath[128] = RESOURCE_PATH;
            strcat(completeFilePath, filePath);

            binaryData = (unsigned char*) malloc(width * height * depth * channels * elemSizeInBytes);

            if(!binaryData) {
                printError();
                std::printf("Allocating binary 3D texture %s failed!", filePath);
                return;
            }

            binaryFile = std::fopen(completeFilePath, "rb");
            std::fread(binaryData, 1, width * height * depth * channels * elemSizeInBytes, binaryFile);

            if(!binaryData) {
                printError();
                std::printf("Loading binary 3D texture %s failed!", filePath);
                return;
            }

            glGenTextures(1, &textureID);

            glActiveTexture(GL_TEXTURE0 + texUnit);
            glBindTexture(GL_TEXTURE_3D, textureID);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, height, depth, 0, format, pixelType, binaryData);

            free(binaryData);

            glBindTexture(GL_TEXTURE_3D, 0);
        }

};

#endif