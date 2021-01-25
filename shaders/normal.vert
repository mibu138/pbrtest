#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uvw;
layout(location = 3) in vec3 tangent;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec2 outUv;
layout(location = 2) out mat3 outTBN;

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
    const mat4 xform = model.xform[push.primId];
    const vec4 worldPos = xform * vec4(pos, 1.0);
    gl_Position = camera.proj * camera.view * worldPos;
    outWorldPos = worldPos.xyz; 
    outUv = uvw.st;
    vec3 bitangent = normalize(cross(norm, tangent));
    vec3 T = normalize(vec3(xform * vec4(tangent, 0)));
    vec3 B = normalize(vec3(xform * vec4(bitangent, 0)));
    vec3 N = normalize(vec3(xform * vec4(norm, 0)));
    outTBN = mat3(T, B, N);
}
