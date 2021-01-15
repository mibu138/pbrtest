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
#define ARCBALL_CAMERA_IMPLEMENTATION
#include "arcball_camera.h"


static Vec2  mousePos;
static int   iMouseX;
static int   iMouseY;
Parms parms; 
struct ShaderParms* pShaderParms;

Tanto_U_Widget* slider0;

static Tanto_S_Scene   scene;

static float t;

#define ZOOM_RATE 0.01
#define PAN_RATE 1
#define TUMBLE_RATE 2

typedef Tanto_Mask MouseFlags;

enum {
    LMB_BIT = (MouseFlags)1 << 0,
    RMB_BIT = (MouseFlags)1 << 1,
    MMB_BIT = (MouseFlags)1 << 2,
} MouseBits;

MouseFlags mouseFlags;

#define HOME_POS    {1.0, 1.4, 2.0}
#define HOME_TARGET {0.0, 0.5, 0.0}
#define HOME_UP     {0.0, 1.0, 0.0}

static bool homeCamera = false;

static void updateCamera(float dt, int16_t mx, int16_t my, bool panning, bool tumbling, bool zooming, bool home)
{
    static Vec3 pos    = HOME_POS;
    static Vec3 target = HOME_TARGET;
    static Vec3 up     = HOME_UP;
    static int xPrev = 0, yPrev = 0;
    static int zoom_ticks = 0;
    if (zooming)
        zoom_ticks = mx - xPrev;
    else
        zoom_ticks = 0;
    if (home)
    {
        pos =    (Vec3)HOME_POS;
        target = (Vec3)HOME_TARGET;
        up =     (Vec3)HOME_UP;
    }
    //pos = m_RotateY_Vec3(dt, &pos);
    arcball_camera_update(pos.x, target.x, up.x, NULL, dt, 
            ZOOM_RATE, PAN_RATE, TUMBLE_RATE, TANTO_WINDOW_WIDTH, TANTO_WINDOW_HEIGHT, xPrev, mx, yPrev, my, 
            panning, tumbling, zoom_ticks, 0);
    Mat4 m = m_LookAt(&pos, &target, &up);
    scene.camera.xform = m;
    xPrev = mx;
    yPrev = my;
    scene.dirt |= TANTO_S_CAMERA_BIT;
}

static void updateLights(void)
{
    static float lastVal;
    const  float curVal = slider0->data.slider.sliderPos;
    if (curVal != lastVal)
    {
        scene.lights[0].intensity = curVal;
        scene.dirt |= TANTO_S_LIGHTS_BIT;
    }
    lastVal = curVal;
}

void g_Init(void)
{
    parms.shouldRun = true;
    t = 0.0;
    slider0 = tanto_u_CreateSlider(0, 40, NULL);
    tanto_s_CreateSimpleScene2(&scene);
    r_BindScene(&scene);
    updateCamera(0.16, 0, 0, 0, 0, 0, true);
}

bool g_Responder(const Tanto_I_Event *event)
{
    switch (event->type) 
    {
        case TANTO_I_KEYDOWN: switch (event->data.keyCode)
        {
            case TANTO_KEY_ESC: parms.shouldRun = false; break;
            case TANTO_KEY_H: homeCamera = true; break;
            default: return true;
        } break;
        case TANTO_I_KEYUP:   switch (event->data.keyCode)
        {
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
    updateLights();
    updateCamera(0.016, iMouseX, iMouseY, mouseFlags & MMB_BIT, mouseFlags & LMB_BIT, mouseFlags & RMB_BIT, homeCamera);
    homeCamera = false;
}

