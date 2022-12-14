#version 330 core 

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

out vec3 color;
out vec3 normal;

uniform mat4 cameraProjectionMatrix;
uniform mat4 cameraViewMatrix;
uniform mat4 modelMatrix;

void main() {
    gl_Position = cameraProjectionMatrix * cameraViewMatrix * modelMatrix * vec4(aPosition, 1.0);
    color = vec3(0.5);
    normal = aNormal;
}

