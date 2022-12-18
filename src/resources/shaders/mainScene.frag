#version 330 core 

in vec3 color;

out vec4 fragColor;

void main() {
    if(!gl_FrontFacing)
        discard;
    fragColor = vec4(color, 0.0);
}