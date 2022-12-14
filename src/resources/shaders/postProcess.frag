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
    float depth = texture(sceneDepth, texcoord).x;
    vec3 viewPos = vec3(texcoord, depth) * 2.0 - 1.0;
    vec3 worldPos = cameraViewMatrixInverse[0].xyz;
    fragColor.xyz = normalize(viewPos);
}