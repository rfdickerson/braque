#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec2 inUV;

// camera ubo
layout (binding = 0) uniform CameraUniforms
{
    mat4 view;
    mat4 proj;
} camera;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec3 fragNormal;
layout (location = 2) out vec3 fragPosition;
layout (location = 3) out vec2 fragUV;

void main ()
{
    vec4 pos = camera.proj * camera.view * vec4(inPosition, 1.0);
    gl_Position = pos;
    // For reversed depth in Vulkan, we don't need to modify Z
    // The projection matrix already handles this
    fragColor = inColor;
    fragNormal = inNormal;
    fragPosition = inPosition;
    fragUV = inUV;
}