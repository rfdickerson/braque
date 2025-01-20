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