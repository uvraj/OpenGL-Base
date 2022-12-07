#version 420 core

layout (binding = 0) uniform sampler2D imageLinear;
//uniform sampler2D sceneDepth;

layout (binding = 2) uniform sampler3D colorLookup;

uniform bool useColorGrade;

out vec4 fragColor;

in vec2 texcoord;

void main() {
    fragColor = texture(imageLinear, texcoord);

    if(useColorGrade)
        fragColor.rgb = texture(colorLookup, fragColor.rgb).rgb;
}