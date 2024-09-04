#if !defined INTERSECTIONS_GLSL
#define INTERSECTIONS_GLSL

struct Material {
    vec3 albedo;
    bool isEmissive;
    bool isMetal;
    bool isRefractive;
    float emissivity;
    float roughness;
};

struct HitData {
    vec3 hitPos;
    vec3 hitNormal;
    Material material;
    float t;
    bool rayHit;
};

const Material emptyMaterial = Material(
    vec3(0.0),
    false,
    false, 
    false, 
    0.0, 
    0.0
);

const HitData emptyHitData = HitData(
    vec3(0.0), 
    vec3(0.0),
    emptyMaterial, 
    100000, 
    false
);

void IntersectSphere(inout HitData hitData, const Material material, const vec3 sphereOrigin, const float radius, const vec3 rayOrigin, const vec3 direction) {
    // Analytical Sphere Intersection
    vec3 originsDelta = sphereOrigin - rayOrigin;
    float tca = dot(originsDelta, direction);
    float d2 = dot(originsDelta, originsDelta) - tca * tca;
    if (d2 > radius * radius) return; 

    float thc = sqrt(radius * radius - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;

    // Intersections are behind the ray origin
    if (t0 < 0.0 && t1 < 0.0) return;

    float t = (t0 < t1) ? t0 : t1;
          t = (t0 < 0.0) ? t1 : t;

    hitData.hitPos = rayOrigin + direction * t;
    hitData.hitNormal = normalize(hitData.hitPos - sphereOrigin);
    hitData.t = t;
    hitData.rayHit = true;
    hitData.material = material;
}

void IntersectPlane(inout HitData hitData, const Material material, const vec3 planeOrigin, const vec3 planeNormal, const vec3 rayOrigin, const vec3 rayDirection) {
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-plane-and-ray-disk-intersection.html
    // Equation 5
    hitData.hitNormal = planeNormal;
    float denom = dot(hitData.hitNormal, rayDirection);

    if (abs(denom) < 1e-6) return;

    vec3 originsDelta = planeOrigin - rayOrigin;
    hitData.t = dot(originsDelta, hitData.hitNormal) / denom;

    if (hitData.t >= 0) {
        hitData.hitPos = hitData.t * rayDirection + rayOrigin;
        hitData.rayHit = true;
        hitData.material = material;
    }
}

void IntersectDisk(
    inout HitData hitData, 
    const Material material, 
    const vec3 origin,
    const float radius, 
    const vec3 normal, 
    const vec3 rayOrigin, 
    const vec3 rayDirection
) {
    IntersectPlane(hitData, material, origin, normal, rayOrigin, rayDirection);

    if (hitData.rayHit) {
        float dist = distance(hitData.hitPos, origin);
        hitData.rayHit = dist < radius ? true : false;
    }
}

#endif // INTERSECTIONS_GLSL