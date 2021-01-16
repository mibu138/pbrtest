#include "game.h"
#include "coal/m_math.h"
#include "coal/util.h"
#include "render.h"
#include "common.h"
#include "tanto/t_utils.h"
#include <coal/coal.h>
#include <assert.h>
#include <string.h>
#include <tanto/t_def.h>
#include <tanto/i_input.h>
#include <tanto/u_ui.h>
#include <tanto/s_scene.h>


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
    tanto_s_CreateSimpleScene3(&scene);
    r_BindScene(&scene);
    tanto_s_UpdateLight(&scene, 0, 1);
    tanto_s_UpdateLight(&scene, 1, 1);
    tanto_s_UpdateCamera(&scene, 0.16, 0, 0, 0, 0, 0, true);
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
    tanto_s_UpdateCamera(&scene, 0.016, iMouseX, iMouseY, 
            mouseFlags & MMB_BIT, mouseFlags & LMB_BIT, mouseFlags & RMB_BIT, homeCamera);
    homeCamera = false;
}

