#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 cameraProjectionMatrix;
uniform mat4 cameraViewMatrix;

out vec3 normal;

void main() {
    gl_Position = cameraProjectionMatrix * cameraViewMatrix * vec4(aPos.xzy * vec3(0.1, 0.1, 0.1), 1.0);
    normal = aNormal.xzy;
}