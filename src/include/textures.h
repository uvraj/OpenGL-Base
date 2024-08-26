#pragma once

class TextureInfo {
public:
    GLuint getID() {
        return id;
    }

    std::string getName() const {
        return name;
    }

protected:
    std::string name;
    GLuint id;
    GLenum internalFormat;
    GLenum format;
    GLenum pixelType;
    GLenum wrapParam;
    GLenum filterParam;
};


class Texture3D : public TextureInfo {
public:
    Texture3D(
        const std::string& inName,
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

        glTextureStorage3D(id, 1, internalFormat, width, height, depth);
        glTextureSubImage3D(id, 0, 0, 0, 0, width, height, depth, internalFormat, pixelType, nullptr);
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

class Texture2D : public TextureInfo {
public:
    Texture2D(
        const std::string& inName,
        const GLuint inWidth,
        const GLuint inHeight,   
        const GLuint inInternalFormat, 
        const GLenum inFormat, 
        const GLenum inPixelType,
        const GLenum inWrapParam,
        const GLenum inFilterParam
    ) {
        name = inName;
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
        glTextureSubImage2D(id, 0, 0, 0, width, height, internalFormat, pixelType, nullptr);
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
};
