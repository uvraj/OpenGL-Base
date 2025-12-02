#version 460

uniform sampler2D finalImage;

in vec3 normal;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outNormal;

void main() {
    outColor.rgb = normal.xyz;
    outNormal = vec4(normal * 0.5 + 0.5, 0.0);
} 