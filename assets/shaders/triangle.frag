#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragNormal;
layout (location = 2) in vec3 fragPosition;
layout (location = 3) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

layout (binding = 1) uniform sampler2D texSampler;

void main () { 

    vec3 n = 0.5 * (fragNormal + vec3 (1.0)) ;
    outColor = vec4 (fragColor, 1.0); 
    outColor = vec4(fragUV, 0.0, 1.0);
    outColor = texture(texSampler, fragUV);
}