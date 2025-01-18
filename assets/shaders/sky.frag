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

const vec3 RAYLEIGH_SCATTERING = vec3(5.5e-4, 13.0e-4, 40.4e-4);
const float MIE_SCATTERING = 1.0e-3;
const float MIE_DIRECTIONAL_G = 0.8;
const vec3 SUN_DIRECTION = normalize(vec3(0.0, 1.00, 0.0));
const vec3 SUN_COLOR = vec3(1.0, 0.98, 0.95);
const float EXPOSURE = 5000.0;

vec3 toneMapFilmic(vec3 color) {
    color = max(vec3(0.0), color - vec3(0.004)); // Adjust black level
    return (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);
}

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float rayleighPhase(float cosTheta) {
    return 3.0 / (16.0 * 3.141592) * (1.0 + cosTheta * cosTheta);
}

float miePhase(float cosTheta, float g) {
    float g2 = g * g;
    return (1.0 - g2) / (4.0 * 3.141592 * pow(1.0 + g2 - 2.0 * g * cosTheta, 1.5));
}

void main() {
    vec2 screenPos = vec2(gl_FragCoord.x / 1280.0f, gl_FragCoord.y / 720.0f) * 2.0 - 1.0;
    vec4 clipSpacePos = vec4(screenPos, 1.0, 1.0);
    vec4 viewSpacePos = camera.inverseProjectionMatrix * clipSpacePos;
    viewSpacePos.xyz /= viewSpacePos.w;
    vec3 worldDir = normalize((camera.inverseViewMatrix * vec4(viewSpacePos.xyz, 0.0)).xyz);

    // Compute Rayleigh and Mie scattering
    float cosTheta = dot(worldDir, SUN_DIRECTION);
    vec3 rayleigh = RAYLEIGH_SCATTERING * rayleighPhase(cosTheta);
    vec3 mie = vec3(MIE_SCATTERING * miePhase(cosTheta, MIE_DIRECTIONAL_G));

    // Add a sun glow
    float sunGlow = exp(-200.0 * (1.0 - cosTheta));
    vec3 glowColor = SUN_COLOR * sunGlow * 0.25;

    // Combine scattering
    vec3 scattering = (rayleigh + mie) * SUN_COLOR;

    // Add a gradient from the horizon to the zenith
    float horizonFactor = smoothstep(0.0, 0.35, worldDir.y); // Transition near horizon
    vec3 horizonColor = vec3(0.55, 0.75, 1.0);  // Light blue at horizon
    vec3 zenithColor = vec3(0.1, 0.2, 0.4);   // Dark saturated blue at zenith
    vec3 gradientColor = mix(horizonColor, zenithColor, horizonFactor);


    // Combine scattering and apply exposure
    vec3 skyColor = scattering * gradientColor + glowColor;
    skyColor *= EXPOSURE;

    vec3 toneMappedColor = toneMapFilmic(skyColor);

    // Add dithering to reduce banding
    float ditherStrength = 1.0 / 255.0; // Based on 8-bit quantization
    vec3 dither = vec3(rand(gl_FragCoord.xy) * ditherStrength);

    // Clamp for stability
    outColor = vec4(toneMappedColor + dither, 1.0);

}