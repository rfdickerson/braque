#version 450
#extension GL_ARB_separate_shader_objects : enable

// camera ubo
layout (binding = 0) uniform CameraUniforms
{
    mat4 view;
    mat4 proj;
} camera;

layout (location = 0) out vec3 fragColor;

vec3 positions[3] = vec3[](vec3 (0.0, -0.5, -1.0), vec3 (0.5, 0.5, -1.0), vec3 (-0.5, 0.5, -1.0));

vec3 colors[3] = vec3[](vec3 (1.0, 0.0, 0.0), vec3 (0.0, 1.0, 0.0), vec3 (0.0, 0.0, 1.0));

void main ()
{
    //gl_Position = vec4 (positions[gl_VertexIndex], 0.0, 1.0);
    gl_Position = camera.proj * camera.view * vec4 (positions[gl_VertexIndex], 1.0);
    fragColor = colors[gl_VertexIndex];
}