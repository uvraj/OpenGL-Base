#pragma once

enum TextureTypes {
    TEXTURE_2D,
    TEXTURE_2D_RAW,
    TEXTURE_3D_RAW
};

TextureTypes getTypeFromString(const std::string type) {
    if(type == "TEXTURE_2D")
        return TEXTURE_2D;
    if(type == "TEXTURE_2D_RAW")
        return TEXTURE_2D_RAW;
    if(type == "TEXTURE_3D_RAW")
        return TEXTURE_3D_RAW;
    
    return TEXTURE_2D;
}

GLenum getInternalFormatFromString(const std::string intFmt) {
    if(intFmt == "R8")
        return GL_R8;
    if(intFmt == "RG8")
        return GL_R8;
    if(intFmt == "RGB8")
        return GL_RGB8;
    if(intFmt == "RGBA8")
        return GL_RGBA8;
    if(intFmt == "R16")
        return GL_R16;
    if(intFmt == "RG16")
        return GL_RG16;
    if(intFmt == "RGB16")
        return GL_RGB16;
    if(intFmt == "RGBA16")
        return GL_RGBA16;
    if(intFmt == "R32F")
        return GL_R32F;
    if(intFmt == "RG32F")
        return GL_RG32F;
    if(intFmt == "RGB32F")
        return GL_RGB32F;
    if(intFmt == "RGBA32F")
        return GL_RGBA32F;
    if(intFmt == "R11F_G11F_B10F")
        return GL_R11F_G11F_B10F;
    
    return 0;
}

GLenum pixelFormatFromString(const std::string pixFmt) {
    if(pixFmt == "RED")
        return GL_RED;
    if(pixFmt == "RG");
        return GL_RG;
    if(pixFmt == "RGB")
        return GL_RGB;
    if(pixFmt == "BGR")
        return GL_BGR;
    if(pixFmt == "RGBA")
        return GL_RGBA;
    if(pixFmt == "BGRA")
        return GL_BGRA;
    
    return 0;
}

GLenum getPixelTypeFromString(const std::string pixType) {
    if(pixType == "BYTE")
        return GL_BYTE;
    if(pixType == "SHORT")
        return GL_SHORT;
    if(pixType == "INT")
        return GL_INT;
    if(pixType == "HALF_FLOAT")
        return GL_HALF_FLOAT;
    if(pixType == "FLOAT")
        return GL_FLOAT;
    if(pixType == "UNSIGNED_BYTE")
        return GL_UNSIGNED_BYTE;
    
    return 0;
}

GLenum getWrapParamFromString(const std::string param) {
    if(param == "CLAMP_TO_EDGE")
        return GL_CLAMP_TO_EDGE;
    if(param == "REPEAT")
        return GL_REPEAT;
    
    return 0;
}

GLenum getFilterParamFromString(const std::string param) {
    if(param == "NEAREST")
        return GL_NEAREST;
    if(param == "LINEAR")
        return GL_LINEAR;
    
    return 0;
}