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

const vec3 sunDirection = normalize(vec3(0.0, 0.5, -1.0));
const vec3 sunColor = vec3(1.0, 0.8, 0.6);
const float exposure = 20.0;

const vec3 RAYLEIGH_SCATTERING = vec3(5.5e-2, 13.0e-2, 22.4e-2);
const float MIE_SCATTERING = 21.0e-2;
const float MIE_DIRECTIONAL_G = 0.90; // Controls the directional bias of Mie scattering

void main() {
    vec3 viewDir = normalize(inViewDir);
    vec4 adjustedViewDir = vec4(viewDir, 0.0);

    // Adjust view direction based on aspect ratio
    adjustedViewDir.x *= camera.aspectRatio;

    // Transform to world space
    vec3 worldDir = normalize((camera.inverseViewMatrix * adjustedViewDir).xyz);
    vec3 sunDir = normalize(sunDirection);

    // Rayleigh scattering
    float cosTheta = clamp(dot(worldDir, sunDir), -1.0, 1.0);
    float rayleighPhase = 3.0 / (16.0 * 3.141592) * (1.0 + cosTheta * cosTheta);
    vec3 rayleighColor = RAYLEIGH_SCATTERING * rayleighPhase;

    // Mie scattering
    float g2 = MIE_DIRECTIONAL_G * MIE_DIRECTIONAL_G;
    float miePhase = (1.0 - g2) / (4.0 * 3.141592 * pow(1.0 + g2 - 2.0 * MIE_DIRECTIONAL_G * cosTheta, 1.5));

    // Apply an additional exponential falloff to make the sun smaller
    float sunSizeFactor = exp(-pow(acos(cosTheta) / radians(2.0), 2.0)); // Adjust "2.0" for smaller/bigger sun
    vec3 mieColor = vec3(MIE_SCATTERING) * miePhase * sunSizeFactor;

    // Combine scattering
    vec3 totalScattering = rayleighColor + mieColor;
    totalScattering *= sunColor; // Modulate with the sun's color/intensity

    // Adjust brightness and tone
    vec3 skyColor = totalScattering * exposure;

    // Clamp for stability
    outColor = vec4(clamp(skyColor, 0.0, 1.0), 1.0);

}