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

void CompareHitData(inout HitData hitData, HitData newHitData) {
    if (abs(newHitData.t) < abs(hitData.t) && newHitData.rayHit) hitData = newHitData;
}

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

    hitData.rayHit = false;
    hitData.hitPos = rayOrigin;

    if (abs(denom) < 1e-6) return;

    vec3 originsDelta = planeOrigin - rayOrigin;
    hitData.t = dot(originsDelta, hitData.hitNormal) / denom;

    if (hitData.t >= 0) {
        hitData.hitPos = hitData.t * rayDirection + rayOrigin;
        hitData.rayHit = true;
        hitData.material = material;
    }
}

void IntersectVoxel(inout HitData hitData, const Material material, const vec3 rayOrigin, const vec3 rayDirection) {
    hitData.rayHit = false;

    const int steps = 256;

    vec3 stepSizes = 1.0 / abs(rayDirection);
    vec3 stepDir = sign(rayDirection);
    vec3 nextDist = (stepDir * 0.5 + 0.5 - fract(rayOrigin)) / rayDirection;

    vec3 voxelPos = floor(rayOrigin);
    hitData.hitPos = rayOrigin;
    hitData.hitNormal = vec3(0.0);

    for (int i = 0; i < steps; i++) {
        bool isOccupied = texelFetch(voxelTex, ivec3(voxelPos.yxz * vec3(1.0, 1.0, 1.0)), 0).r * 100 > 0.1;

        if (isOccupied) {
            hitData.rayHit = true;
            break;
        }

        stepSizes = 1.0 / abs(rayDirection);
        stepDir = sign(rayDirection);

        float closestDist = min(nextDist.x, min(nextDist.y, nextDist.z));
        hitData.hitPos += rayDirection * closestDist;
        vec3 stepAxis = vec3(lessThanEqual(nextDist, vec3(closestDist)));
        voxelPos += stepAxis * stepDir;
        nextDist -= closestDist;
        nextDist += stepSizes * stepAxis;
        hitData.hitNormal = -stepAxis * stepDir;
    }

    hitData.t = length(rayOrigin - hitData.hitPos);

    hitData.material = material;
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

void RayTraceScene(inout HitData hitData, const vec3 rayOrigin, const vec3 rayDirection, const vec3 lightPositions[3], const vec3 lightColors[3], const bool ignoreLights) {
    Material diffuseMaterial;
    diffuseMaterial.albedo = vec3(1.0);
    diffuseMaterial.isEmissive = false;
    diffuseMaterial.isMetal = false;
    diffuseMaterial.isRefractive = false;
    diffuseMaterial.emissivity = 0.0;
    diffuseMaterial.roughness = 1.0;

    Material emitterMaterial = emptyMaterial;
    emitterMaterial.albedo = vec3(1.0f, 1.0f, 1.0f);
    emitterMaterial.emissivity = 5.0f;
    emitterMaterial.isEmissive = true;

    hitData = emptyHitData;

    HitData tempHitData = emptyHitData;
    IntersectVoxel(tempHitData, diffuseMaterial, rayOrigin, rayDirection);
    CompareHitData(hitData, tempHitData);

    tempHitData = emptyHitData;
    IntersectPlane(tempHitData, diffuseMaterial, vec3(0.0), vec3(0.0, 1.0, 0.0), rayOrigin, rayDirection);
    CompareHitData(hitData, tempHitData);

    if (!ignoreLights){
        tempHitData = emptyHitData;
        emitterMaterial.albedo = lightColors[0];
        IntersectSphere(tempHitData, emitterMaterial, lightPositions[0], 5.0, rayOrigin, rayDirection);
        CompareHitData(hitData, tempHitData);

        tempHitData = emptyHitData;
        emitterMaterial.albedo = lightColors[1];
        IntersectSphere(tempHitData, emitterMaterial, lightPositions[1], 5.0, rayOrigin, rayDirection);
        CompareHitData(hitData, tempHitData);

        tempHitData = emptyHitData;
        emitterMaterial.albedo = lightColors[2];
        IntersectSphere(tempHitData, emitterMaterial, lightPositions[2], 5.0, rayOrigin, rayDirection);
        CompareHitData(hitData, tempHitData);
    }
 
}

#endif // INTERSECTIONS_GLSL