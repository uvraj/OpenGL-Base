#version 330 core 

uniform vec3 lightVector;

in vec3 color;
in vec3 normal;

out vec4 fragColor;

void main() {
    fragColor.rgb = vec3(dot(normal, lightVector));
}