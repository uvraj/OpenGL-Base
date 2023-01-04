#version 460

uniform sampler2D sceneAlbedo;
uniform sampler2D sceneDepth;
uniform sampler3D colorLookup;
uniform sampler2D heightMap;

uniform mat4 cameraProjectionMatrixInverse;
uniform mat4 cameraViewMatrixInverse;
uniform bool useColorGrade;

out vec4 fragColor;

in vec2 texcoord;

void main() {
    fragColor = texture(heightMap, texcoord);
}