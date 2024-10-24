#pragma once

struct DrawData {
    glm::vec3 pos;
    glm::vec3 normal;
};

struct STLData {
    glm::vec3 normal;
    glm::vec3 v1;
    glm::vec3 v2;
    glm::vec3 v3;
    std::uint16_t attributes[2];
};

class STLModel {
public:
    STLModel(const std::string& inFileName) {
        fileName = inFileName;
        readFile();
        parse();
        createDrawData();
    }

    ~STLModel() {
        delete data;
        delete stlData;
        delete drawData;
    }

    void setup() {
        glGenBuffers(1, &VBO);
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, numTris * 3 * 4 * 3 * 2, drawData, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void draw() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, numTris * 3);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

private:
    std::string fileName;

    // OpenGL objects

    GLuint VBO;
    GLuint VAO;

    STLData* stlData;
    DrawData* drawData;
    char header[80];

    std::uint8_t* data;
    std::uint32_t fileSize;
    std::uint32_t numTris;

    void readFile() {
        std::ifstream file{RESOURCE_PATH + fileName, std::ios::binary | std::ios::ate};

        if (!file.is_open()) {
            throw std::runtime_error("Failed opening STL file.");
        }

        fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        data = new std::uint8_t[fileSize];

        if (file.read(reinterpret_cast<char*>(data), fileSize)) {
            std::cout << PIPELINE_HINT << "File read successfully!" << std::endl;
        } else {
            throw std::runtime_error("Failed reading STL file.");
        }
    }

    void parse() {
        for (std::size_t i = 0; i < 80; i++) {
            header[i] = data[i];
        }

        // Read the total amount of triangles.
        numTris = *reinterpret_cast<std::uint32_t*> (data + 80);
        
        // Allocate the vertex array, which will later be used with OpenGL
        stlData = new STLData[numTris];

        for (std::size_t i = 0; i < numTris; i++) {
            stlData[i] = *reinterpret_cast<STLData*> (data + i * 50 + 84);
        }

        printDebugOutput();
    }

    void createDrawData() {
        drawData = new DrawData[3 * numTris];

        for (std::size_t i = 0; i < numTris; i++) {
            drawData[i * 3].normal  = stlData[i].normal;
            drawData[i * 3].pos     = stlData[i].v1;

            drawData[i * 3 + 1].normal  = stlData[i].normal;
            drawData[i * 3 + 1].pos     = stlData[i].v2;

            drawData[i * 3 + 2].normal  = stlData[i].normal;
            drawData[i * 3 + 2].pos     = stlData[i].v3;
        }
    }

    void printDebugOutput() {
        std::cout << "Number of triangles: " << numTris << std::endl;
        std::cout << "Header: " << header << std::endl;
    }
};