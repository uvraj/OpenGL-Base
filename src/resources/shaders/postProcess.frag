#version 330 core

uniform sampler2D imageLinear;

out vec4 fragColor;

in vec2 texcoord;

void main() {
    fragColor = texture(imageLinear, texcoord);
}