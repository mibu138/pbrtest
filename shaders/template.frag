#version 460

layout(location = 0) in  vec3 inColor;
layout(location = 1) in  vec3 inNormal;

layout(location = 0) out vec4 outColor;

#define MAX_LIGHTS 16

struct Light {
    vec3  vector; // position or direction based on type
    float intensity;
    vec3  color;
    int   type;
};

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
} camera;

layout(set = 0, binding = 2) uniform Lights {
    Light light[MAX_LIGHTS]; 
} lights;

layout(push_constant) uniform PushConstant {
    layout(offset = 16) uint lightCount;
} push;

float calcIllume(const vec3 N, const vec3 dir)
{
    return max(dot(-1 * dir, N), 0);
}

void main()
{
    vec3  campos = vec3(-1 * camera.view[3][0], -1 * camera.view[3][1], -1 * camera.view[3][2]);
    float ambient = 0.05;
    float illume = 0;
    for (int i = 0; i < push.lightCount; i++)
    {
        illume += calcIllume(inNormal, lights.light[i].vector) * lights.light[i].intensity;
    }
    outColor = vec4(campos, 1) * max(illume + ambient, 0);
}
