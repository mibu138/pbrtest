#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec3 uvw;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUv;

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    mat4 xform;
} camera;

layout(set = 0, binding = 1) uniform Model {
    mat4 xform[2];
} model;

layout(push_constant) uniform PushConstant {
    uint primId;
} push;

void main()
{
    vec4 worldPos = model.xform[push.primId] * vec4(pos, 1.0);
    gl_Position = camera.proj * camera.view * worldPos;
    outWorldPos = worldPos.xyz; 
    outNormal = (model.xform[push.primId] * vec4(norm, 1.0)).xyz; // this is fine as long as we only allow uniform scales
    outUv = uvw.st;
}
