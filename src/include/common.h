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
            printInfo("Deleted quad objects \n");
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

class Scene {
    /*
     * This class handles:
     * creating all the OpenGL data necessary for drawing a quad
     * actually drawing the data via glDrawElements
    */ 
    public:
        GLfloat quadData[24] = {
        // vertex data     // normalized texture sampling coords
        -1.00f, -1.00f, 0.0f,   0.0f, 0.0f,
         1.00f, -1.00f, 0.0f,   1.0f, 0.0f,
         1.00f,  1.00f, 0.0f,   1.0f, 1.0f, 
        -1.00f,  1.00f, 0.0f,   0.0f, 1.0f
        };

        // The order in which we want the vertex data to be drawn in.
        GLuint quadOrder[6] = {
            0, 1, 2, 2, 3, 0
        };
        
        // This stuff shall hold the ID for the magic sauce
        GLuint vertexBufferObjects = 0;
        GLuint vertexArrayObjects  = 0;
        GLuint EBO = 0;

        Scene() {
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
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);

            // Unbind everything
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        ~Scene() {
            glDeleteBuffers(1, &vertexBufferObjects);
            glDeleteBuffers(1, &EBO);
            glDeleteVertexArrays(1, &vertexArrayObjects);
            printInfo("Deleted quad objects \n");
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