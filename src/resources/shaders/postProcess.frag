#version 420 core

layout (binding = 0) uniform sampler2D sceneAlbedo;
layout (binding = 1) uniform sampler2D sceneDepth;
layout (binding = 3) uniform sampler3D colorLookup;

uniform mat4 cameraProjectionMatrixInverse;
uniform mat4 cameraViewMatrixInverse;
uniform bool useColorGrade;

out vec4 fragColor;

in vec2 texcoord;

void main() {
    fragColor = texture(sceneAlbedo, texcoord);
}