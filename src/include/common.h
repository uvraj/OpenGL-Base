#ifndef COMMON_H
#define COMMON_H

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

size_t twoDtoOneD(size_t x, size_t y, size_t width) {
    return x + y * width;
}

class Scene {
    public:
        GLuint EBO, VBO, VAO;

        size_t numStrips = 0;
        size_t numVertsPerStrip = 0;
        size_t numVerts = 0;
        size_t numIndices = 0;
        size_t numTriangles = 0;

        int width = 0;
        int height = 0;
        int channels = 0;

        std::vector <glm::vec3> data_VAO;
        std::vector <glm::vec3> vertexData;
        std::vector <glm::vec3> normalData;
        std::vector <GLuint> indices;

        Scene() {
            // Load heightmap
            int width, height, channels;

            uint8_t *heightData = stbi_load("../../src/resources/height.png", &width, &height, &channels, 1);

            if (!heightData) {
                printError("Loading Heightmap failed!");
                assert(heightData);
            }

            numStrips = height - 1;
            numTriangles = (width - 1) * (height - 1) * 2;
            numVertsPerStrip = 2 * width;

            // Generate vertices
            for (size_t z = 0; z < height; z++) {
                for (size_t x = 0; x < width; x++) {
                    size_t y = twoDtoOneD(x, z, width);
                    vertexData.push_back(
                        glm::vec3(
                            x,
                            heightData[y],
                            z
                        )
                    );
                    numVerts++;
                }
            }

            // Generate indices
            for (size_t z = 0; z < height - 1; z++) {
                for (size_t x = 0; x < width - 1; x++) {
                    size_t y = twoDtoOneD(x, z, width);
                    size_t index0 = y;
                    size_t index1 = y + 1;
                    size_t index2 = y + width;
                    size_t index3 = y + width + 1;
                    indices.push_back(index0);
                    indices.push_back(index2);
                    indices.push_back(index1);
                    indices.push_back(index1);
                    indices.push_back(index2);
                    indices.push_back(index3);
                }
            }

            normalData.resize(numVerts);

            // Generate normal data
            for (size_t i = 0; i < indices.size(); i += 3) {
                // Get the indices of the three vertices of the triangular face
                size_t index0 = indices[i];
                size_t index1 = indices[i + 1];
                size_t index2 = indices[i + 2];

                // Get the position of each vertex
                glm::vec3 v0 = vertexData[index0];
                glm::vec3 v1 = vertexData[index1];
                glm::vec3 v2 = vertexData[index2];

                // Compute the normal vector for the triangular face
                glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

                // Add the normal vector to the normal data for each vertex
                normalData[index0] += normal;
                normalData[index1] += normal;
                normalData[index2] += normal;
            }


            // Normalize
            for (size_t i = 0; i < normalData.size(); i++) {
                normalData[i] = glm::normalize(normalData[i]);
            }

            for (size_t i = 0; i < numVerts; i++) {
                data_VAO.push_back(vertexData[i]);
                data_VAO.push_back(normalData[i]);
            }

            // Generate the required objects.
            glGenBuffers(1, &EBO);
            glGenBuffers(1, &VBO);
            glGenVertexArrays(1, &VAO);

            // Bind the VAO
            glBindVertexArray(VAO);

            // Bind the VBO, fill some data in it
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, data_VAO.size() * sizeof(glm::vec3), data_VAO.data(), GL_STATIC_DRAW);

            // Same for the EBO
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

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
            printInfo("Deleted scene objects \n");
        }

        void draw() {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void *) 0);
            
            
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