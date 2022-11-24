#version 330 core

uniform sampler3D colorLookup;

uniform sampler2D imageLinear;

uniform bool useColorGrade;

out vec4 fragColor;

in vec2 texcoord;

void main() {
    fragColor = texture(imageLinear, texcoord);

    if(useColorGrade)
        fragColor.rgb = texture(colorLookup, fragColor.rgb).rgb;
}