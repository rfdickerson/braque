#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;

// camera ubo
layout (binding = 0) uniform CameraUniforms
{
    mat4 view;
    mat4 proj;
} camera;

layout (location = 0) out vec3 fragColor;

void main ()
{
    gl_Position = camera.proj * camera.view * vec4 (inPosition, 1.0);
    fragColor = inColor;
}