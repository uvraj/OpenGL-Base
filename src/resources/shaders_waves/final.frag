#version 460

uniform sampler2D finalImage;

out vec4 fragColor;

in vec2 texcoord;

void main() {
    fragColor = texture(finalImage, texcoord);
}