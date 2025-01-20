#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragNormal;
layout (location = 2) in vec3 fragPosition;
layout (location = 3) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform sampler2D texSampler;

#include "camera_ubo.glsl"

// Directional light properties
const vec3 lightDir = normalize(vec3(1.0, -0.7, 1)); // Direction towards the light
const vec3 lightColor = vec3(1.0, 1.0, 1.0); // White light
const float ambientStrength = 0.2;

void main () {
    // Normalize the fragment normal
    vec3 norm = normalize(fragNormal);

    // Calculate diffuse lighting
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Calculate ambient lighting
    vec3 ambient = ambientStrength * lightColor;

    // Sample the texture
    vec3 texColor = texture(texSampler, fragUV).rgb;

    // Combine lighting with texture and vertex color
    vec3 result = (ambient + diffuse) * texColor;

    outColor = vec4(result, 1.0);
    
}