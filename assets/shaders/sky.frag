// sky.frag
#version 450

layout(location = 0) in vec3 inViewDir;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform CameraUbo {
    mat4 view;
    mat4 proj;
} camera;

void main() {
    vec3 viewDir = normalize(inViewDir);
    vec3 worldDir = (inverse(camera.view) * vec4(viewDir, 0.0)).xyz;

    float t = clamp(worldDir.y, 0.0, 1.0);
    vec3 skyColorTop = vec3(0.0, 0.0, 0.3);  // Dark blue
    vec3 skyColorHorizon = vec3(0.3, 0.6, 1.0);  // Light blue

    outColor = vec4(mix(skyColorHorizon, skyColorTop, t), 1.0);
}