// sky.frag
#version 450

layout(location = 0) in vec3 inViewDir;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform CameraUbo {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewProjectionMatrix;
    mat4 inverseViewMatrix;
    mat4 inverseProjectionMatrix;
    vec3 cameraPosition;
    vec3 viewDir;
    vec3 upVector;
    vec3 rightVector;
    vec2 nearFarPlanes;
    float aspectRatio;
    float fov;
} camera;

const vec3 RAYLEIGH_SCATTERING = vec3(5.5e-3, 13.5e-3, 22.4e-3);
const float MIE_SCATTERING = 2.0e-3;
const float MIE_DIRECTIONAL_G = 0.85;
const vec3 SUN_DIRECTION = normalize(vec3(0.0, 0.3, -1.0));
const vec3 SUN_COLOR = vec3(1.0, 0.95, 0.85);
const float EXPOSURE = 25.0;

float rayleighPhase(float cosTheta) {
    return 3.0 / (16.0 * 3.141592) * (1.0 + cosTheta * cosTheta);
}

float miePhase(float cosTheta, float g) {
    float g2 = g * g;
    return (1.0 - g2) / (4.0 * 3.141592 * pow(1.0 + g2 - 2.0 * g * cosTheta, 1.5));
}

vec3 computeScattering(vec3 rayDirection, vec3 lightDirection) {
    float cosTheta = dot(rayDirection, lightDirection);

    // Rayleigh Scattering
    float rayleighPhaseFactor = rayleighPhase(cosTheta);
    vec3 rayleighScattering = RAYLEIGH_SCATTERING * rayleighPhaseFactor;

    // Mie Scattering
    float miePhaseFactor = miePhase(cosTheta, MIE_DIRECTIONAL_G);
    vec3 mieScattering = vec3(MIE_SCATTERING * miePhaseFactor);

    // Apply directional factor to ensure single sun
    float directionalFactor = max(cosTheta, 0.0);
    directionalFactor = pow(directionalFactor, 8.0); // Adjust power for sun size

    // Combine Rayleigh and Mie, applying directional factor
    vec3 totalScattering = (rayleighScattering + mieScattering) * directionalFactor;

    return totalScattering;
}

void main() {
    vec3 viewDir = normalize(inViewDir);

    // Transform to world space
    vec3 worldDir = normalize((camera.inverseViewMatrix * vec4(viewDir, 0.0)).xyz);

    // Compute scattering
    vec3 scattering = computeScattering(worldDir, SUN_DIRECTION);

    // Apply sun color and exposure
    vec3 skyColor = scattering * SUN_COLOR * EXPOSURE;

    // Add a bit of ambient light to avoid completely black sky
    skyColor += vec3(0.02, 0.04, 0.08);

    skyColor = pow(skyColor, vec3(1.0 / 2.2));

    // Clamp for stability
    outColor = vec4(clamp(skyColor, 0.0, 1.0), 1.0);

}