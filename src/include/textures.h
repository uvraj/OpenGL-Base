#pragma once

class TextureBase {
public:
    GLuint getID() {
        return id;
    }

    std::string getName() const {
        return name;
    }

protected:
    std::string name;
    std::string fileName;
    GLuint id;
    GLenum internalFormat;
    GLenum format;
    GLenum pixelType;
    GLenum wrapParam;
    GLenum filterParam;

    std::vector<std::uint8_t> getDataFromFile() {
        std::string filePath = RESOURCE_PATH + fileName;
        std::ifstream binaryFile{filePath, std::ios::binary};

        std::vector <std::uint8_t> binaryData;
        binaryData.assign(std::istreambuf_iterator <char> (binaryFile), {});

        // Check for errors and output accordingly
        if(!binaryFile.is_open()) {
            throw std::runtime_error(ERROR_HINT + "Loading binary texture " + filePath + " failed.\n");
        }
        
        return binaryData;
    }
};


class Texture3D : public TextureBase {
public:
    Texture3D(
        const std::string& inName,
        const std::string& inFileName,
        const GLuint inWidth,
        const GLuint inHeight, 
        const GLuint inDepth,  
        const GLenum inInternalFormat, 
        const GLenum inFormat, 
        const GLenum inPixelType,
        const GLenum inWrapParam,
        const GLenum inFilterParam
    ) {
        name = inName;
        fileName = inFileName;
        width = inWidth;
        height = inHeight;
        depth = inDepth;
        internalFormat = inInternalFormat;
        format = inFormat;
        pixelType = inPixelType;
        wrapParam = inWrapParam;
        filterParam = inFilterParam;
    }

    void create() {
        glCreateTextures(GL_TEXTURE_3D, 1, &id);

        glObjectLabel(GL_TEXTURE, id, name.length(), name.c_str());

        glTextureParameteri(id, GL_TEXTURE_WRAP_S, wrapParam);	
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, wrapParam);
        glTextureParameteri(id, GL_TEXTURE_WRAP_R, wrapParam);
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, filterParam);
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, filterParam);

        // Hardcode the border color for the time being.
        if (wrapParam == GL_CLAMP_TO_BORDER) {
            float borderColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
            glTextureParameterfv(id, GL_TEXTURE_BORDER_COLOR, borderColor);
        }

        glTextureStorage3D(id, 1, internalFormat, width, height, depth);

        if (!fileName.empty()) {
            std::vector<uint8_t> data = getDataFromFile();
            glTextureSubImage3D(id, 0, 0, 0, 0, width, height, depth, format, pixelType, data.data());
        }
    }

    void bind(const GLuint& unit) {
        glBindTextureUnit(unit, id);
    }

    void bindImageTexture(const GLuint& unit) {
        glBindImageTexture(unit, id, 0, GL_TRUE, 0, GL_READ_WRITE, internalFormat);
    }

    void destroy() {
        glDeleteTextures(1, &id);
    }

protected:
    GLuint width;
    GLuint height;
    GLuint depth;
};

class Texture2D : public TextureBase {
public:
    Texture2D(
        const std::string& inName,
        const std::string& inFileName,
        const GLuint inWidth,
        const GLuint inHeight,
        const GLuint inInternalFormat, 
        const GLenum inFormat, 
        const GLenum inPixelType,
        const GLenum inWrapParam,
        const GLenum inFilterParam
    ) {
        name = inName;
        fileName = inFileName;
        width = inWidth;
        height = inHeight;
        internalFormat = inInternalFormat;
        format = inFormat;
        pixelType = inPixelType;
        wrapParam = inWrapParam;
        filterParam = inFilterParam;
    }

    void create() {
        glCreateTextures(GL_TEXTURE_2D, 1, &id);

        glObjectLabel(GL_TEXTURE, id, name.length(), name.c_str());

        glTextureParameteri(id, GL_TEXTURE_WRAP_S, wrapParam);	
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, wrapParam);
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, filterParam);
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, filterParam);

        glTextureStorage2D(id, 1, internalFormat, width, height);

        if (!fileName.empty()) {
            std::vector<uint8_t> data = getDataFromFile();

            // TODO: do size checking. If the JSON specifies a larger size than what the file actually is, OpenGL is going to access memory out of bounds.
            glTextureSubImage2D(id, 0, 0, 0, width, height, format, pixelType, data.data());
        }
        
    }

    void bind(const GLuint& unit) {
        glBindTextureUnit(unit, id);
    }

    void bindImageTexture(const GLuint& unit) {
        glBindImageTexture(unit, id, 0, GL_TRUE, 0, GL_READ_WRITE, internalFormat);
    }

    void destroy() {
        glDeleteTextures(1, &id);
    }

    void writeImageToDisk() {
        std::ofstream binaryFile{name + ".dat", std::ios::binary};

        if (!binaryFile.is_open()) {
            throw std::runtime_error(ERROR_HINT + "Writing texture " + name + " failed.");
        }

        std::size_t size = width * height * sizeof(float) * 4;
        std::uint8_t* data = new std::uint8_t[size];

        glGetTextureImage(id, 0, GL_RGBA, GL_FLOAT, size, data);

        binaryFile.write(reinterpret_cast<const char*>(data), size);

        delete[] data;
    }

    void writeImageToDiskPNG() {
        static uint64_t index = 0;
        std::size_t size = width * height * sizeof(std::uint8_t) * 4;
        std::uint8_t* data = new std::uint8_t[size];

        glGetTextureImage(id, 0, GL_RGBA, GL_UNSIGNED_BYTE, size, data);

        std::string fileName = name + "_" + std::to_string(index) + ".png";

        stbi_write_png(fileName.c_str(), width, height, 4, data, width * 4);

        index++;
    }

protected:
    GLuint width;
    GLuint height;
};

class Texture1D : public TextureBase {
public:
    Texture1D(
        const std::string& inName,
        const std::string& inFileName,
        const GLuint inWidth,
        const GLuint inInternalFormat, 
        const GLenum inFormat, 
        const GLenum inPixelType,
        const GLenum inWrapParam,
        const GLenum inFilterParam
    ) {
        name = inName;
        fileName = inFileName;
        width = inWidth;
        internalFormat = inInternalFormat;
        format = inFormat;
        pixelType = inPixelType;
        wrapParam = inWrapParam;
        filterParam = inFilterParam;
    }

    void create() {
        glCreateTextures(GL_TEXTURE_1D, 1, &id);

        glObjectLabel(GL_TEXTURE, id, name.length(), name.c_str());

        glTextureParameteri(id, GL_TEXTURE_WRAP_S, wrapParam);	
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, filterParam);
        glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, filterParam);

        glTextureStorage1D(id, 1, internalFormat, width);

        if (!fileName.empty()) {
            std::vector<uint8_t> data = getDataFromFile();

            // TODO: do size checking. If the JSON specifies a larger size than what the file actually is, OpenGL is going to access memory out of bounds.
            glTextureSubImage1D(id, 0, 0, width, format, pixelType, data.data());
        }
        
    }

    void bind(const GLuint& unit) {
        glBindTextureUnit(unit, id);
    }

    void bindImageTexture(const GLuint& unit) {
        glBindImageTexture(unit, id, 0, GL_TRUE, 0, GL_READ_WRITE, internalFormat);
    }

    void destroy() {
        glDeleteTextures(1, &id);
    }

    void writeImageToDisk() {
        std::ofstream binaryFile{name + ".dat", std::ios::binary};

        if (!binaryFile.is_open()) {
            throw std::runtime_error(ERROR_HINT + "Writing texture " + name + " failed.");
        }

        std::size_t size = width * sizeof(float) * 4;
        std::uint8_t* data = new std::uint8_t[size];

        glGetTextureImage(id, 0, GL_RGBA, GL_FLOAT, size, data);

        binaryFile.write(reinterpret_cast<const char*>(data), size);

        delete[] data;
    }

protected:
    GLuint width;
};
