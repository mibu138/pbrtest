#include "game.h"
#include "coal/m_math.h"
#include "coal/util.h"
#include "render.h"
#include "common.h"
#include "tanto/t_utils.h"
#include <coal/coal.h>
#include <assert.h>
#include <string.h>
#include <tanto/f_file.h>
#include <tanto/r_geo.h>
#include <tanto/t_def.h>
#include <tanto/i_input.h>
#include <tanto/u_ui.h>
#include <tanto/s_scene.h>
#include <stdlib.h>

static Vec2  mousePos;
static int   iMouseX;
static int   iMouseY;
Parms parms; 

Tanto_U_Widget* slider0;

static Tanto_S_Scene scene;

static float t;

typedef Tanto_Mask MouseFlags;

enum {
    LMB_BIT = (MouseFlags)1 << 0,
    RMB_BIT = (MouseFlags)1 << 1,
    MMB_BIT = (MouseFlags)1 << 2,
} MouseBits;

MouseFlags mouseFlags;

static bool homeCamera = false;

void g_Init(void)
{
    parms.shouldRun = true;
    t = 0.0;
    slider0 = tanto_u_CreateSlider(0, 40, NULL);
    tanto_s_CreateEmptyScene(&scene);
    Tanto_R_Primitive cube = tanto_r_CreateCubePrim(true);
    //Tanto_F_Primitive fcube = tanto_f_CreateFPrimFromRPrim(&cube);
    //tanto_f_WritePrimitive("data/cube.tnt", &fcube);
    //tanto_f_FreePrimitive(&fcube);
    //Tanto_F_Primitive fcube;
    //tanto_f_ReadPrimitive("data/cube.tnt", &fcube);
    //Tanto_R_Primitive cube = tanto_f_CreateRPrimFromFPrim(&fcube);
    //tanto_f_FreePrimitive(&fcube);
    //exit(0);
    tanto_s_AddRPrim(&scene, cube, NULL);
    //{
    //    Mat4 m = m_Ident_Mat4();
    //    m_ScaleUniform_Mat4(0.5, &m);
    //    Tanto_S_PrimId prim         = tanto_s_LoadPrim(&scene, "data/hydrant.tnt", &m);
    //    Tanto_S_TextureId albedo    = tanto_s_LoadTexture(&scene, "data/hydrant-albedo.jpg", 4);
    //    Tanto_S_TextureId roughness = tanto_s_LoadTexture(&scene, "data/hydrant-roughness.jpg", 1);
    //    Tanto_S_MaterialId material = tanto_s_CreateMaterial(&scene, (Vec3){1, 1, 1}, 1, albedo, roughness);
    //    tanto_s_BindPrimToMaterial(&scene, prim, material);
    //}
    //{
    //    Mat4 m = m_Ident_Mat4();
    //    m = m_RotateY_Mat4(M_PI/2, &m);
    //    m = m_RotateZ_Mat4(M_PI/2, &m);
    //    m = m_Translate_Mat4((Vec3){1, .1, 1}, &m);
    //    Tanto_S_PrimId prim         = tanto_s_LoadPrim(&scene, "data/log.tnt", &m);
    //    Tanto_S_TextureId albedo    = tanto_s_LoadTexture(&scene, "data/log-albedo.jpg", 4);
    //    Tanto_S_TextureId roughness = tanto_s_LoadTexture(&scene, "data/log-roughness.jpg", 1);
    //    Tanto_S_MaterialId material = tanto_s_CreateMaterial(&scene, (Vec3){1, 1, 1}, 1, albedo, roughness);
    //    tanto_s_BindPrimToMaterial(&scene, prim, material);
    //}
    //{
    //    Mat4 m = m_Ident_Mat4();
    //    Tanto_S_PrimId prim         = tanto_s_LoadPrim(&scene, "data/sidewalk.tnt", &m);
    //    Tanto_S_TextureId albedo    = tanto_s_LoadTexture(&scene, "data/sidewalk-albedo.jpg", 4);
    //    Tanto_S_TextureId roughness = tanto_s_LoadTexture(&scene, "data/sidewalk-roughness.jpg", 1);
    //    Tanto_S_MaterialId material = tanto_s_CreateMaterial(&scene, (Vec3){1, 1, 1}, 1, albedo, roughness);
    //    tanto_s_BindPrimToMaterial(&scene, prim, material);
    //}
    tanto_s_CreateDirectionLight(&scene, (Vec3){0.9, 0.7, 0.4}, (Vec3){-1, -2, -2});
    tanto_s_CreateDirectionLight(&scene, (Vec3){0.5, 0.6, 0.8}, (Vec3){1, -1, 2});
    tanto_s_CreatePointLight(&scene, (Vec3){0.1, 0.1, 1}, (Vec3){-3, 0, 0});
    r_BindScene(&scene);
    tanto_s_UpdateLight(&scene, 0, 1);
    //tanto_s_UpdateCamera_(&scene, 0.16, 0, 0, 0, 0, 0, true);
}

bool g_Responder(const Tanto_I_Event *event)
{
    switch (event->type) 
    {
        case TANTO_I_KEYDOWN: switch (event->data.keyCode)
        {
            case TANTO_KEY_ESC: parms.shouldRun = false; break;
            case TANTO_KEY_H: homeCamera = true; break;
            case TANTO_KEY_A: mouseFlags |= LMB_BIT; break;
            case TANTO_KEY_S: mouseFlags |= RMB_BIT; break;
            case TANTO_KEY_D: mouseFlags |= MMB_BIT; break;
            default: return true;
        } break;
        case TANTO_I_KEYUP:   switch (event->data.keyCode)
        {
            case TANTO_KEY_A: mouseFlags &= ~LMB_BIT; break;
            case TANTO_KEY_S: mouseFlags &= ~RMB_BIT; break;
            case TANTO_KEY_D: mouseFlags &= ~MMB_BIT; break;
            default: return true;
        } break;
        case TANTO_I_MOTION: 
        {
            mousePos.x = (float)event->data.mouseData.x / TANTO_WINDOW_WIDTH;
            mousePos.y = (float)event->data.mouseData.y / TANTO_WINDOW_HEIGHT;
            iMouseX = event->data.mouseData.x;
            iMouseY = event->data.mouseData.y;
        } break;
        case TANTO_I_MOUSEDOWN: 
        {
            switch (event->data.mouseData.buttonCode)
            {
                case TANTO_MOUSE_LEFT:  mouseFlags |= LMB_BIT; break;
                case TANTO_MOUSE_RIGHT: mouseFlags |= RMB_BIT; break;
                case TANTO_MOUSE_MID:   mouseFlags |= MMB_BIT; break;
            }
        } break;
        case TANTO_I_MOUSEUP:
        {
            switch (event->data.mouseData.buttonCode)
            {
                case TANTO_MOUSE_LEFT:  mouseFlags &= ~LMB_BIT; break;
                case TANTO_MOUSE_RIGHT: mouseFlags &= ~RMB_BIT; break;
                case TANTO_MOUSE_MID:   mouseFlags &= ~MMB_BIT; break;
            }
        } break;
        default: break;
    }
    return false;
}

// currently game has ownership of the scene.
// therefore its this modules job to dirty and clean the scene on each frame
void g_Update(void)
{
    scene.dirt = 0; // clean
    t += 0.016;
    static float lastVal;
    const  float curVal = slider0->data.slider.sliderPos;
    if (curVal != lastVal)
    {
        tanto_s_UpdateLight(&scene, 0, curVal);
    }
    lastVal = curVal;
    static Vec3 campos;
    campos = (Vec3){2 * cos(t), 0.7, 2 * sin(t)};
    tanto_s_UpdateCamera_LookAt(&scene, campos, (Vec3){0, 0, 0}, (Vec3){0, 1, 0});
    //tanto_s_UpdateCamera(&scene, 0.016, iMouseX, iMouseY, 
    //        mouseFlags & MMB_BIT, mouseFlags & LMB_BIT, mouseFlags & RMB_BIT, homeCamera);
    homeCamera = false;
}

