#pragma once

GLenum getInternalFormatFromString(const std::string& intFmt) {
    static const std::unordered_map<std::string, GLenum> internalFormatMap = {
        {"R8", GL_R8},
        {"RG8", GL_RG8},
        {"RGB8", GL_RGB8},
        {"RGBA8", GL_RGBA8},
        {"R16", GL_R16},
        {"RG16", GL_RG16},
        {"RGB16", GL_RGB16},
        {"RGBA16", GL_RGBA16},
        {"R16F", GL_R16F},
        {"RG16F", GL_RG16F},
        {"RGBA16F", GL_RGBA16F},
        {"R32F", GL_R32F},
        {"RG32F", GL_RG32F},
        {"RGB32F", GL_RGB32F},
        {"RGBA32F", GL_RGBA32F},
        {"R11F_G11F_B10F", GL_R11F_G11F_B10F}
    };
    
    auto it = internalFormatMap.find(intFmt);
    if (it != internalFormatMap.end()) {
        return it->second;
    }
    printError("Internal format " + intFmt + " unimplemented.\n");
    throw std::runtime_error("Unimplemented internal format.");
    return 0; 
}

GLenum getPixelFormatFromString(const std::string& pixFmt) {
    static const std::unordered_map<std::string, GLenum> pixelFormatMap = {
        {"RED", GL_RED},
        {"RG", GL_RG},
        {"RGB", GL_RGB},
        {"BGR", GL_BGR},
        {"RGBA", GL_RGBA},
        {"BGRA", GL_BGRA}
    };
    
    auto it = pixelFormatMap.find(pixFmt);
    if (it != pixelFormatMap.end()) {
        return it->second;
    }
    
    printError("Pixel format " + pixFmt + " unimplemented.\n");
    throw std::runtime_error("Unimplemented pixel format.");
    return 0;  
}

GLenum getPixelTypeFromString(const std::string& pixType) {
    static const std::unordered_map<std::string, GLenum> pixelTypeMap = {
        {"BYTE", GL_BYTE},
        {"SHORT", GL_SHORT},
        {"INT", GL_INT},
        {"HALF_FLOAT", GL_HALF_FLOAT},
        {"FLOAT", GL_FLOAT},
        {"UNSIGNED_BYTE", GL_UNSIGNED_BYTE}
    };
    
    auto it = pixelTypeMap.find(pixType);
    if (it != pixelTypeMap.end()) {
        return it->second;
    }
    printError("Pixel type " + pixType + " unimplemented.\n");
    throw std::runtime_error("Unimplemented pixel type.");
    return 0;  // Default case
}

GLenum getWrapParamFromString(const std::string& param) {
    static const std::unordered_map<std::string, GLenum> wrapParamMap = {
        {"CLAMP_TO_EDGE", GL_CLAMP_TO_EDGE},
        {"REPEAT", GL_REPEAT},
        {"CLAMP_TO_BORDER", GL_CLAMP_TO_BORDER}
    };
    
    auto it = wrapParamMap.find(param);
    if (it != wrapParamMap.end()) {
        return it->second;
    }

    printError("Wrap parameter " + param + " unimplemented.\n");
    throw std::runtime_error("Unimplemented wrap parameter.");
    return 0;  // Default case
}

GLenum getFilterParamFromString(const std::string& param) {
    static const std::unordered_map<std::string, GLenum> filterParamMap = {
        {"NEAREST", GL_NEAREST},
        {"LINEAR", GL_LINEAR}
    };
    
    auto it = filterParamMap.find(param);
    if (it != filterParamMap.end()) {
        return it->second;
    }

    printError("Filter parameter " + param + " unimplemented.\n");
    throw std::runtime_error("Unimplemented filter parameter.");
    return 0;  // Default case
}

class PipelineManager {
public:
    std::vector<Texture1D> textures1D;
    std::vector<Texture2D> textures2D;
    std::vector<Texture3D> textures3D;
    std::vector<ComputeShader> computeShaders;
    std::string path;
    json pipeline;

    PipelineManager(const std::string inPath) {
        path = inPath;
    }

    void load() {
        loadPipelineFile();
        parseTextures1D();
        parseTextures2D();
        parseTextures3D();
        parseComputeShaders();

        for (auto& texture : textures1D) {
            texture.create();
        }

        for (auto& texture : textures2D) {
            texture.create();
        }

        for (auto& texture : textures3D) {
            texture.create();
        }

        for (auto& shader : computeShaders) {
            shader.create();
        }
    }

    void reload() {
        textures2D.clear();
        textures3D.clear();
        computeShaders.clear();
        load();
    }

    void mainLoop(const Camera& camera, const Window& window) {
        for (auto& shader : computeShaders) {
            std::string programName = shader.getProgramName();
            glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, programName.size(), programName.data());

            shader.use();

            std::size_t i = 0;

            for (i = 0; i < shader.getBoundImages().size(); i++) {
                try {
                    findTexture2DByName(shader.getBoundImages().at(i)).bindImageTexture(i);
                } catch (const std::exception& e) {
                    try {
                        findTexture3DByName(shader.getBoundImages().at(i)).bindImageTexture(i);
                    } catch (const std::exception& e) {
                        try {
                            findTexture1DByName(shader.getBoundImages().at(i)).bindImageTexture(i); 
                        } catch (const std::exception& e) {
                            printError();
                            std::cout << "Could not find texture " << shader.getBoundImages().at(i) << '\n'; 
                        }
                    }
                }

                

                shader.pushIntUniform(shader.getBoundImages().at(i).c_str(), i);
            }

            // Auxilliary images
            if (shader.getOutputTex() != "") {
                findTexture3DByName(shader.getOutputTex()).bindImageTexture(++i);
                shader.pushIntUniform("outputTex", i);
            }

            i = 0;

            for (i = 0; i < shader.getBoundSamplers().size(); i++) {
                try {
                    findTexture2DByName(shader.getBoundSamplers().at(i)).bind(i);
                } catch (const std::exception& e) {
                    try {
                        findTexture3DByName(shader.getBoundSamplers().at(i)).bind(i);
                    } catch (const std::exception& e) {
                        try {
                            findTexture1DByName(shader.getBoundSamplers().at(i)).bind(i); 
                        } catch (const std::exception& e) {
                            printError();
                            std::cout << "Could not find texture " << shader.getBoundSamplers().at(i) << '\n'; 
                        }
                    }
                }

                shader.pushIntUniform(shader.getBoundSamplers().at(i).c_str(), i);
            }

            // Auxilliary textures
            if (shader.getInputTex() != "") {
                findTexture3DByName(shader.getInputTex()).bind(++i);
                shader.pushIntUniform("inputTex", i);
            }

            if (shader.getKernelTex() != "") {
                findTexture2DByName(shader.getKernelTex()).bind(++i);
                shader.pushIntUniform("kernelTex", i);
            }

            if (shader.getBiasTex() != "") {
                findTexture1DByName(shader.getBiasTex()).bind(++i);
                shader.pushIntUniform("biasTex", i);
            }

            if (shader.getAuxTex() != "") {
                findTexture3DByName(shader.getAuxTex()).bind(++i);
                shader.pushIntUniform("auxTex", i);
            }

            shader.pushMat4Uniform("cameraViewMatrix", camera.viewMatrix);
            shader.pushMat4Uniform("cameraViewMatrixInverse", camera.viewMatrixInverse);
            shader.pushMat4Uniform("cameraProjectionMatrix", camera.projectionMatrix);
            shader.pushMat4Uniform("cameraProjectionMatrixInverse", camera.projectionMatrixInverse);
            shader.pushMat4Uniform("previousCameraViewMatrix", camera.viewMatrix);
            shader.pushMat4Uniform("previousCameraViewMatrixInverse", camera.viewMatrixInverse);
            shader.pushMat4Uniform("previousCameraProjectionMatrix", camera.projectionMatrix);
            shader.pushMat4Uniform("previousCameraProjectionMatrixInverse", camera.projectionMatrixInverse);
            shader.pushVec3Uniform("cameraPosition", camera.Position);
            shader.pushVec3Uniform("previousCameraPosition", camera.previousPosition);
            shader.pushFloatUniform("currentFrame", (float) window.currentFrame);
            shader.pushBoolUniform("shouldAccumulate", window.shouldAccumulate);
            shader.pushUnsignedIntUniform("frameIndex", window.frameIndex);
            shader.pushUnsignedIntUniform("accumulationIndex", window.accumulationIndex);

            glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            shader.dispatch();
            glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

            glPopDebugGroup();
        }
    }

    Texture1D& findTexture1DByName(const std::string& name) {
        auto it = std::find_if(textures1D.begin(), textures1D.end(), [&name](const Texture1D& texture) {
            return texture.getName() == name;
        });

        if (it != textures1D.end()) {
            return *it;
        } else {
            throw std::runtime_error("Texture not found: " + name);
        }
    }

    Texture2D& findTexture2DByName(const std::string& name) {
        auto it = std::find_if(textures2D.begin(), textures2D.end(), [&name](const Texture2D& texture) {
            return texture.getName() == name;
        });

        if (it != textures2D.end()) {
            return *it;
        } else {
            throw std::runtime_error("Texture not found: " + name);
        }
    }

    Texture3D& findTexture3DByName(const std::string& name) {
        auto it = std::find_if(textures3D.begin(), textures3D.end(), [&name](const Texture3D& texture) {
            return texture.getName() == name;
        });

        if (it != textures3D.end()) {
            return *it;
        } else {
            throw std::runtime_error("Texture not found: " + name);
        }
    }

    Texture2D& getFinalImageTexture() {
        std::string name = pipeline["finalImage"];
        return findTexture2DByName(name);
    }

private:
    void loadPipelineFile() {
        std::string totalFilePath = SHADER_PATH + path;
        std::ifstream file(totalFilePath);

        if (!file.is_open()) {
            printError();
            throw std::runtime_error("Could not open the file: " + path);
        }


        try {
            file >> pipeline;
        } catch (json::parse_error& e) {
            printError();
            std::cout << "JSON parse error: " << e.what() << std::endl;
        }

        file.close();
    }

    void parseTextures1D() {
        for (const auto& item : pipeline["textures1D"]) {
            std::string name = item.at("name").get<std::string>();
            GLuint width = item.at("width").get<GLuint>();
            GLuint internalFormat = getInternalFormatFromString(item.at("internalFormat").get<std::string>());
            GLenum format = getPixelFormatFromString(item.at("pixelFormat").get<std::string>());
            GLenum pixelType = getPixelTypeFromString(item.at("pixelType").get<std::string>());
            GLenum wrapParam = getWrapParamFromString(item.at("textureWrap").get<std::string>());
            GLenum filterParam = getFilterParamFromString(item.at("filter").get<std::string>());

            std::string fileName{};

            if (item.find("fileName") != item.end()) {
                fileName = item.at("fileName");
            }

            std::cout << PIPELINE_HINT << "Registered texture \"" + name + "\"\n";

            textures1D.emplace_back(name, fileName, width, internalFormat, format, pixelType, wrapParam, filterParam);
        }
    }

    void parseTextures2D() {
        for (const auto& item : pipeline["textures2D"]) {
            std::string name = item.at("name").get<std::string>();
            GLuint width = item.at("width").get<GLuint>();
            GLuint height = item.at("height").get<GLuint>();
            GLuint internalFormat = getInternalFormatFromString(item.at("internalFormat").get<std::string>());
            GLenum format = getPixelFormatFromString(item.at("pixelFormat").get<std::string>());
            GLenum pixelType = getPixelTypeFromString(item.at("pixelType").get<std::string>());
            GLenum wrapParam = getWrapParamFromString(item.at("textureWrap").get<std::string>());
            GLenum filterParam = getFilterParamFromString(item.at("filter").get<std::string>());

            std::string fileName{};

            if (item.find("fileName") != item.end()) {
                fileName = item.at("fileName");
            }

            std::cout << PIPELINE_HINT << "Registered texture \"" + name + "\"\n";

            textures2D.emplace_back(name, fileName, width, height, internalFormat, format, pixelType, wrapParam, filterParam);
        }
    }

    void parseTextures3D() {
        for (const auto& item : pipeline["textures3D"]) {
            std::string name = item.at("name").get<std::string>();
            GLuint width = item.at("width").get<GLuint>();
            GLuint height = item.at("height").get<GLuint>();
            GLuint depth = item.at("depth").get<GLuint>();
            GLuint internalFormat = getInternalFormatFromString(item.at("internalFormat").get<std::string>());
            GLenum format = getPixelFormatFromString(item.at("pixelFormat").get<std::string>());
            GLenum pixelType = getPixelTypeFromString(item.at("pixelType").get<std::string>());
            GLenum wrapParam = getWrapParamFromString(item.at("textureWrap").get<std::string>());
            GLenum filterParam = getFilterParamFromString(item.at("filter").get<std::string>());

            std::string fileName{};

            if (item.find("fileName") != item.end()) {
                fileName = item.at("fileName");
            }

            std::cout << PIPELINE_HINT << "Registered texture \"" + name + "\"\n";

            textures3D.emplace_back(name, fileName, width, height, depth, internalFormat, format, pixelType, wrapParam, filterParam);
        }
    }

    void parseComputeShaders() {
        for (const auto& item : pipeline["passes"]) {
            std::string name = item.at("name").get<std::string>();
            std::string source = item.at("source").get<std::string>();
            GLuint dispatchSizeX = item.at("dispatchSizeX").get<GLuint>();
            GLuint dispatchSizeY = item.at("dispatchSizeY").get<GLuint>();
            GLuint dispatchSizeZ = item.at("dispatchSizeZ").get<GLuint>();

            std::vector<std::string> boundImages;
            std::vector<std::string> boundSamplers;

            if (item.find("boundImages") != item.end()) {
                for (const auto& image : item.at("boundImages")) {
                    
                    boundImages.emplace_back(image.get<std::string>());
                }
            }

            if (item.find("boundSamplers") != item.end()) {
                for (const auto& sampler : item.at("boundSamplers")) {
                    boundSamplers.emplace_back(sampler.get<std::string>());
                }
            }

            std::string inputTex, outputTex, kernelTex, biasTex, auxTex;

            if (item.find("inputTex") != item.end()) {
                inputTex = item.at("inputTex").get<std::string>();
            }

            if (item.find("outputTex") != item.end()) {
                outputTex = item.at("outputTex").get<std::string>();
            }

            if (item.find("kernelTex") != item.end()) {
                kernelTex = item.at("kernelTex").get<std::string>();
            }

            if (item.find("biasTex") != item.end()) {
                biasTex = item.at("biasTex").get<std::string>();
            }

            if (item.find("auxTex") != item.end()) {
                auxTex = item.at("auxTex").get<std::string>();
            }

            std::cout << PIPELINE_HINT << "Registered compute render pass \"" + name + "\"\n";

            computeShaders.emplace_back(name, source, dispatchSizeX, dispatchSizeY, dispatchSizeZ, inputTex, outputTex, kernelTex, biasTex, auxTex, boundImages, boundSamplers);
        }
    }
};