// sky.vert
#version 450

layout(location = 0) out vec3 outViewDir;

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

vec2 positions[4] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0,  1.0)
);

void main() {
    vec2 pos = positions[gl_VertexIndex];
    gl_Position = vec4(pos, 0.0, 1.0);

    // Compute the view direction for each vertex
    vec4 clipSpacePos = vec4(pos, 1.0, 1.0);
    vec4 worldSpaceDir = camera.inverseProjectionMatrix * clipSpacePos;
    outViewDir = normalize((camera.inverseViewMatrix * vec4(worldSpaceDir.xyz, 0.0)).xyz);
}