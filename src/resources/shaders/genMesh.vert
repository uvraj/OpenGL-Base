#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform sampler2D waveTexRe;

uniform mat4 cameraViewMatrix;
uniform mat4 cameraProjectionMatrix;

out vec3 normal;

float SampleHeightMap(vec2 pos) {
    //return sin(pos.x * 0.01) * 150.0;
    return texture(waveTexRe, pos / 1024).r * 80.0;
}

vec3 GenerateNormalVector(vec3 pos) {
    const float eps = 0.1;
    vec3 a = vec3(0.0); 
    vec3 b = vec3(0.0); 

    a.xz = vec2(eps, 0.0);
    b.xz = vec2(0.0, eps);

    a.y = pos.y - SampleHeightMap(pos.xz + a.xz);
    b.y = pos.y - SampleHeightMap(pos.xz + b.xz);

    return normalize(cross(a, b)) * vec3(1.0, -1.0, 1.0);
}

void main() {
    vec3 newPos = vec3(aPos.x, SampleHeightMap(aPos.xz), aPos.z);
    gl_Position = cameraProjectionMatrix * cameraViewMatrix * vec4(newPos, 1.0);
    normal = GenerateNormalVector(newPos);
}