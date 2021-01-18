#version 460
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "common.glsl"

layout(location = 0) in  vec3 worldPos;
layout(location = 1) in  vec3 normal;
layout(location = 2) in  vec2 uv;

layout(location = 0) out vec4 outColor;

#define MAX_LIGHTS 16

struct Light {
    vec3  vector; // position or direction based on type
    float intensity;
    vec3  color;
    int   type;
};

struct Material {
    vec3  color;
    float roughness;
    uint  textureAlbedo;
    uint  textureRoughness;
};

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    mat4 xform;
} camera;

layout(set = 0, binding = 2) uniform Lights {
    Light light[MAX_LIGHTS]; 
} lights;

layout(set = 0, binding = 3) uniform sampler2D textures[]; 

layout(push_constant) uniform PushConstant {
    layout(offset = 4) uint     lightCount;
    layout(offset = 16) Material material;
} push;

void main()
{
    vec3 campos   = vec3(camera.xform[3][0], camera.xform[3][1], camera.xform[3][2]);
    vec3 ambient  = vec3(0.01);
    vec3 diffuse  = vec3(0);
    vec3 specular = vec3(0);
    for (int i = 0; i < push.lightCount; i++)
    {
        vec3 eyeDir = normalize(campos - worldPos);
        if (lights.light[i].type == 1)
        {
            diffuse += lights.light[i].color * calcDiffuse(normal, lights.light[i].vector) * lights.light[i].intensity;
            specular += lights.light[i].color * calcSpecular(normal, lights.light[i].vector, eyeDir) * lights.light[i].intensity;
        }
        else
        {
            vec3 dir      = normalize(worldPos - lights.light[i].vector);
            float falloff =  1.0f / max(length(worldPos - lights.light[i].vector), 0.001); // to prevent div by 0
            diffuse += lights.light[i].color * calcDiffuse(normal, dir) * lights.light[i].intensity * falloff;
            specular += lights.light[i].color * calcSpecular(normal, dir, eyeDir) * lights.light[i].intensity * falloff;
        }
    }
    vec3 illume = diffuse + specular * 4;
    vec2 st = vec2(uv.x, uv.y * -1 + 1);
    vec4 albedo = texture(textures[push.material.textureAlbedo], st);
    outColor = vec4(albedo.rgb * push.material.color, 1) * vec4(illume + ambient, 1);
}
