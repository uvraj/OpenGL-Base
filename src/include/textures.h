#pragma once

class Texture {
    public:
        std::string name = "";
        std::string fileName = "";
        GLuint textureID = 0;
        GLuint texUnit = 0;
        GLuint width = 0;
        GLuint height = 0;
        GLuint depth = 1;
        GLenum internalFormat = GL_RGBA8;
        GLenum format = GL_RGBA;
        GLenum pixelType = GL_UNSIGNED_BYTE;
        GLenum wrapParam = GL_CLAMP_TO_EDGE;
        GLenum filterParam = GL_LINEAR;

        /**
         * @brief 
         * Constructor for loading a binary dump of a 3D texture.
         * 
         * @param i_fileName The file name itself.
         * @param i_texUnit The texture unit used to bind the texture.
         * @param i_width The width of the texture.
         * @param i_height The height of the texture.
         * @param i_depth The depth of the texture.
         * @param i_internalFormat The internal OpenGL Format of the texture. Example: GL_RGBA8
         * @param i_format The OpenGL Pixel Format. Example: GL_RGBA
         * @param i_pixelType The OpenGL Pixel Type. Example: GL_UNSIGNED_BYTE
         * @param i_wrapParam The wrap parameter. Example: GL_CLAMP_TO_BORDER
         * @param i_filterParam The filtering parameter. Example: GL_LINEAR
         */
        Texture(
            const std::string i_fileName, 
            const GLuint i_texUnit, 
            const GLuint i_width, 
            const GLuint i_height, 
            const GLuint i_depth,  
            const GLuint i_internalFormat, 
            const GLenum i_format, 
            const GLenum i_pixelType,
            const GLenum i_wrapParam,
            const GLenum i_filterParam
        ) : 
            fileName {i_fileName}, 
            texUnit {i_texUnit}, 
            width {i_width}, 
            height {i_height}, 
            depth {i_depth}, 
            internalFormat {i_internalFormat}, 
            format {i_format}, 
            pixelType {i_pixelType}, 
            wrapParam {i_wrapParam},
            filterParam {i_filterParam}
        {   
            // Add the resource path to the file path
            std::string filePath = RESOURCE_PATH + fileName;

            // Load the binary file
            std::ifstream binaryFile(filePath, std::ios::binary);
            
            // Load each byte into an std::vector
            std::vector <std::uint8_t> binaryData;
            binaryData.assign(std::istreambuf_iterator <char> (binaryFile), {});

            // Check whether the file is open, output an according message
            if(!binaryFile.is_open()) {
                printError();
                std::cout << "Loading binary 3D texture " << filePath << " failed!\n";
                assert(binaryFile.is_open());
            }

            // Generate an OpenGL texture object
            glGenTextures(1, &textureID);

            // Active the chosen tex. unit and bind the texture
            glActiveTexture(GL_TEXTURE0 + texUnit);
            glBindTexture(GL_TEXTURE_3D, textureID);

            // Set the tex. parameters
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrapParam);	
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrapParam);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrapParam);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filterParam);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filterParam);

            // Store our data.
            glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, height, depth, 0, format, pixelType, &binaryData[0]);

            // Unbind.
            glBindTexture(GL_TEXTURE_3D, 0);
        }

        /**
         * @brief 
         * Constructor for loading a binary dump of a 2D texture.
         * 
         * @param i_fileName The file name itself.
         * @param i_texUnit The texture unit used to bind the texture.
         * @param i_width The width of the texture.
         * @param i_height The height of the texture.
         * @param i_internalFormat The internal OpenGL Format of the texture. Example: GL_RGBA8
         * @param i_format The OpenGL Pixel Format. Example: GL_RGBA
         * @param i_pixelType The OpenGL Pixel Type. Example: GL_UNSIGNED_BYTE
         * @param i_wrapParam The wrap parameter. Example: GL_CLAMP_TO_BORDER
         * @param i_filterParam The filtering parameter. Example: GL_LINEAR
         */
        Texture(
            const std::string i_fileName, 
            const GLuint i_texUnit, 
            const GLuint i_width, 
            const GLuint i_height,   
            const GLuint i_internalFormat, 
            const GLenum i_format, 
            const GLenum i_pixelType,
            const GLenum i_wrapParam,
            const GLenum i_filterParam
        ) : 
            fileName {i_fileName}, 
            texUnit {i_texUnit}, 
            width {i_width}, 
            height {i_height},  
            internalFormat {i_internalFormat}, 
            format {i_format}, 
            pixelType {i_pixelType}, 
            wrapParam {i_wrapParam},
            filterParam {i_filterParam}
        {   
            // Create the actual file path
            std::string filePath = RESOURCE_PATH + fileName;
            std::ifstream binaryFile(filePath, std::ios::binary);

            // Load each byte into an std::vector
            std::vector <std::uint8_t> binaryData;
            binaryData.assign(std::istreambuf_iterator <char> (binaryFile), {});

            // Check for errors and output accordingly
            if(!binaryFile.is_open()) {
                printError();
                std::cout << "Loading binary 2D texture " << filePath << " failed!\n";
                assert(binaryFile.is_open());
            }

            // Generate the OpenGL Texture Object (R)
            glGenTextures(1, &textureID);

            // Activate the chosen texture unit and 
            glActiveTexture(GL_TEXTURE0 + texUnit);
            glBindTexture(GL_TEXTURE_2D, textureID);

            // Set texture parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);

            // The data itself.
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, pixelType, &binaryData[0]);

            // Unbind.
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        /**
         * @brief 
         * Constructor for loading PNGs, JPEGs and the like.
         * 
         * @param i_fileName The file name itself.
         * @param i_texUnit The texture unit used to bind the texture.
         * @param i_wrapParam The wrap parameter. Example: GL_CLAMP_TO_BORDER
         * @param i_filterParam The filtering parameter. Example: GL_LINEAR
         */

        Texture(
            const std::string i_fileName, 
            const GLuint i_texUnit, 
            const GLenum i_wrapParam,
            const GLenum i_filterParam
        ) : 
            fileName {i_fileName}, 
            texUnit {i_texUnit}, 
            wrapParam {i_wrapParam},
            filterParam {i_filterParam}
        {   
            // Create the file path from the resource path and the file name
            std::string filePath = RESOURCE_PATH + fileName;

            // Tell STB that I want to flip the y coord
            stbi_set_flip_vertically_on_load(true);

            // Image parameters. Load each byte. Force 4 channels
            int width, height, nrChannels;
            std::uint8_t *data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 4);

            // Output an error message if the data is invalid
            if(!data) {
                printError();
                std::cout << "Loading texture %s failed!\n";
                assert(data);
            }

            // Generate textures
            glGenTextures(1, &textureID);
            
            // Activate texture unit and bind texture
            glActiveTexture(GL_TEXTURE0 + texUnit);
            glBindTexture(GL_TEXTURE_2D, textureID);

            // Set parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            // The actual data itself
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            // TODO: parameter for this since we don't always need mipmaps
            glGenerateMipmap(GL_TEXTURE_2D);

            // Free the data.
            stbi_image_free(data);

            // Unbind.
            glBindTexture(GL_TEXTURE_2D, 0);
        }


        ~Texture() {
            // TODO: Manual destructors. 
            /*glDeleteTextures(1, &textureID);
            printInfo("Destroyed Texture ");
            std::cout << textureID << '\n';*/
        }

        // TODO: Make this less rigid.
        void bind() {
            if(depth == 1) {
                glActiveTexture(GL_TEXTURE0 + texUnit);
                glBindTexture(GL_TEXTURE_2D, textureID);
            }

            else {
                glActiveTexture(GL_TEXTURE0 + texUnit);
                glBindTexture(GL_TEXTURE_3D, textureID);
            }
        }

};