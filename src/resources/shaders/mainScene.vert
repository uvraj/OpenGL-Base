#version 330 core 

layout (location = 0) in vec3 aPosition;

out vec3 color;

uniform mat4 cameraProjectionMatrix;
uniform mat4 cameraViewMatrix;
uniform mat4 modelMatrix;

void main() {
    gl_Position = cameraProjectionMatrix * cameraViewMatrix * modelMatrix * vec4(aPosition, 1.0);
    color = vec3(0.5);
}

