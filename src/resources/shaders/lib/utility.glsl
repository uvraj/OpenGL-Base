#if !defined UTILITY_GLSL
#define UTILITY_GLSL

#define pow2(x) pow(x, 2.0)
#define pow4(x) pow(x, 4.0)

const float PI = radians(180.0);
const float TAU = radians(360.0);

vec2 sincos(float x) {
    return vec2(sin(x), cos(x));
}

vec3 ACESFilm(const vec3 x) {
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), vec3(0.0f), vec3(1.0f));
}

vec3 LinearToSrgb(const vec3 linearValue) {
    return pow(linearValue, vec3(1.0 / 2.2));
}

vec3 UniformSampleHemisphere(vec3 normal, vec2 rand){
    // cos(theta) = r1 = y
    // cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = sqrt(1 - cos^2(theta))
    float sinTheta = sqrt(1.0 - rand.x * rand.x);
    float phi = 2.0 * PI * rand.y;
    float x = sinTheta * cos(phi);
    float z = sinTheta * sin(phi);
    
    // Transform the local hemisphere direction to world space aligned with the normal
    vec3 tangent, bitangent;
    if (abs(normal.x) > 0.1) {
        tangent = normalize(cross(normal, vec3(0.0, 1.0, 0.0)));
    } else {
        tangent = normalize(cross(normal, vec3(1.0, 0.0, 0.0)));
    }
    bitangent = normalize(cross(normal, tangent));

    vec3 hemisphereDir = tangent * x + normal * rand.x + bitangent * z;
    return normalize(hemisphereDir);
}

vec2 CartesianToSpherical(vec3 pos) {
    float lat = acos(pos.y);
    float lon = atan(-pos.x, pos.z);

    return vec2(lon / TAU + 0.5, lat / PI);
}

#endif // UTILITY_GLSL