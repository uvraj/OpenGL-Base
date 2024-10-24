#version 460

out vec4 fragColor;

in vec3 normal;

void main() {
    fragColor.xyz = abs(normal);
    // fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}