#ifndef CAMERA_H
#define CAMERA_H

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  1.0f;
const float SENSITIVITY =  0.1f;
const float FOV        =  70.0f;

// A camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL / GLSL
class Camera {
    public:
        glm::mat4 projectionMatrix = glm::mat4(1.0f);
        glm::mat4 projectionMatrixInverse = glm::mat4(1.0f);
        glm::mat4 viewMatrix = glm::mat4(1.0f);
        glm::mat4 viewMatrixInverse = glm::mat4(1.0f);

        // Previous frame stuff
        glm::mat4 previousProjectionMatrix = glm::mat4(1.0f);
        glm::mat4 previousProjectionMatrixInverse = glm::mat4(1.0f);
        glm::mat4 previousViewMatrix = glm::mat4(1.0f);
        glm::mat4 previousViewMatrixInverse = glm::mat4(1.0f);

        // camera Attributes
        glm::vec3 Position;
        glm::vec3 previousPosition = glm::vec3(0.0f);

        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        // euler Angles
        float Yaw;
        float Pitch;
        float Roll;
        // camera options
        float MovementSpeed;
        float MouseSensitivity;
        float FoV;

        // constructor with vectors
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), FoV(FOV) {
            Position = position;
            WorldUp = up;
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }

        // constructor with scalar values
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), FoV(FOV) {
            Position = glm::vec3(posX, posY, posZ);
            WorldUp = glm::vec3(upX, upY, upZ);
            Yaw = yaw;
            Pitch = pitch;
            updateCameraVectors();
        }

        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 GetViewMatrix() {
            return glm::lookAt(Position, Position + Front, Up);
        }

        // processes input received from any keyboard-like input system.
        // Abstaction from GLFW
        void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
            float velocity = MovementSpeed * deltaTime;
            if (direction == FORWARD)
                Position += Front * velocity;
            if (direction == BACKWARD)
                Position -= Front * velocity;
            if (direction == LEFT)
                Position -= Right * velocity;
            if (direction == RIGHT)
                Position += Right * velocity;
        }

        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;

            Yaw   += xoffset;
            Pitch += yoffset;

            // make sure that when pitch is out of bounds, screen doesn't get flipped
            if (constrainPitch) {
                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;
            }

            // update Front, Right and Up Vectors using the updated Euler angles
            updateCameraVectors();
        }

        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float yoffset)
        {
            FoV -= (float)yoffset;
            if (FoV < 1.0f)
                FoV = 1.0f;
            if (FoV > 180.0f)
                FoV = 180.0f; 
        }

        void updateCameraData(float aspectRatio) {
            // Projection Matrix
            projectionMatrix = glm::perspective(glm::radians(FoV), aspectRatio, 0.1f, 5000.0f); 
            // projectionMatrix = glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f, 0.1f, 5000.0f / FOV);
            // projectionMatrix = glm::scale(projectionMatrix, glm::vec3(1.0 / FoV));
            projectionMatrixInverse = glm::inverse(projectionMatrix);

            // View Matrix
            viewMatrix = GetViewMatrix();
            viewMatrixInverse = glm::inverse(viewMatrix);
        }

        void writePreviousData() {
            previousProjectionMatrix = projectionMatrix;
            previousProjectionMatrixInverse = projectionMatrixInverse;
            previousViewMatrix = viewMatrix;
            previousViewMatrixInverse = viewMatrixInverse;
            previousPosition = Position;
        }

    private:
        // calculates the front vector from the Camera's (updated) Euler Angles
        void updateCameraVectors() {
            // calculate the new Front vector
            glm::vec3 front;
            front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            front.y = sin(glm::radians(Pitch));
            front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
            Front = glm::normalize(front);
            // also re-calculate the Right and Up vector
            Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            Up    = glm::normalize(glm::cross(Right, Front));
        }
};
#endif