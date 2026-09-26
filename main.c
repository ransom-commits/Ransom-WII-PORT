#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogcsys.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include "game.h"

#define SCREEN_W 640.0f
#define SCREEN_H 480.0f

static void *xfb[2];
static GXRModeObj *rmode;
static u8 *gp_fifo ATTRIBUTE_ALIGN(32);

static void quad(float x, float y, float w, float h, GXColor c) {
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Position3f32(x,     y,     0.0f); GX_Color4u8(c.r,c.g,c.b,c.a);
    GX_Position3f32(x + w, y,     0.0f); GX_Color4u8(c.r,c.g,c.b,c.a);
    GX_Position3f32(x + w, y + h, 0.0f); GX_Color4u8(c.r,c.g,c.b,c.a);
    GX_Position3f32(x,     y + h, 0.0f); GX_Color4u8(c.r,c.g,c.b,c.a);
    GX_End();
}

static void diamond(float cx, float cy, float size, GXColor c) {
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Position3f32(cx, cy-size, 0.0f); GX_Color4u8(c.r,c.g,c.b,c.a);
    GX_Position3f32(cx+size, cy, 0.0f); GX_Color4u8(c.r,c.g,c.b,c.a);
    GX_Position3f32(cx, cy+size, 0.0f); GX_Color4u8(c.r,c.g,c.b,c.a);
    GX_Position3f32(cx-size, cy, 0.0f); GX_Color4u8(c.r,c.g,c.b,c.a);
    GX_End();
}

static void draw_item(const GameItem *it) {
    float cx = it->x + it->width * 0.5f;
    float cy = it->y + it->height * 0.5f;

    if (it->type == ITEM_COIN) {
        GXColor glow = {255, 215, 55, 255};
        GXColor core = {255, 240, 130, 255};
        quad(it->x-3, it->y-3, it->width+6, it->height+6, (GXColor){120,90,20,255});
        diamond(cx, cy, 22.0f, glow);
        quad(cx-5, cy-12, 10, 24, core);
    } else if (it->type == ITEM_HONEYPOT) {
        quad(it->x, it->y+8, it->width, it->height-8, (GXColor){185,110,35,255});
        quad(it->x-3, it->y+5, it->width+6, 10, (GXColor){245,190,60,255});
        quad(cx-12, it->y+18, 24, 5, (GXColor){90,55,20,255});
    } else if (it->type == ITEM_CRUCIFIX) {
        quad(cx-5, cy-22, 10, 44, (GXColor){235,235,235,255});
        quad(cx-17, cy-8, 34, 10, (GXColor){235,235,235,255});
    } else {
        diamond(cx, cy, 23.0f, (GXColor){80,190,255,255});
        quad(cx-15, cy-2, 30, 4, (GXColor){230,245,255,255});
    }
}

static void draw_cursor(float x, float y) {
    quad(x-2, y-14, 4, 28, (GXColor){255,255,255,255});
    quad(x-14, y-2, 28, 4, (GXColor){255,255,255,255});
    quad(x-5, y-5, 10, 10, (GXColor){20,20,20,255});
}

static void draw(GameState *g, float cursor_x, float cursor_y) {
    GXColor bg = {18, 22, 32, 255};
    GXColor panel = {30, 37, 52, 255};
    GXColor white = {240, 245, 250, 255};
    GXColor accent = {255, 190, 50, 255};

    quad(0, 0, SCREEN_W, SCREEN_H, bg);
    quad(0, 0, SCREEN_W, 58, panel);
    quad(18, 14, 260, 30, (GXColor){12,16,24,255});

    float progress = (float)g->currentCoins / (float)g->targetCoins;
    if (progress > 1.0f) progress = 1.0f;
    quad(290, 20, 300, 18, (GXColor){10,12,18,255});
    quad(290, 20, 300.0f * progress, 18, accent);

    float time_width = 120.0f * (g->timeRemaining / START_TIME_SEC);
    if (time_width < 0) time_width = 0;
    quad(505, 20, 120, 18, (GXColor){10,12,18,255});
    quad(505, 20, time_width, 18, (GXColor){80,205,120,255});

    for (int i = 0; i < MAX_ITEMS; ++i)
        if (g->items[i].active) draw_item(&g->items[i]);

    if (g->phase == GAME_SUCCESS) {
        quad(120, 170, 400, 120, (GXColor){20,100,55,245});
        quad(145, 195, 350, 18, white);
        quad(145, 225, 350, 18, (GXColor){180,255,200,255});
    } else if (g->phase == GAME_OVER) {
        quad(120, 170, 400, 120, (GXColor){120,30,35,245});
        quad(145, 195, 350, 18, white);
        quad(145, 225, 350, 18, (GXColor){255,170,170,255});
    }

    (void)white;
    draw_cursor(cursor_x, cursor_y);
}

static void gx_init(void) {
    GX_Init(gp_fifo, 256 * 1024);
    GX_SetCopyClear((GXColor){0,0,0,255}, 0);
    GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
    GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
    GX_SetCullMode(GX_CULL_NONE);
    GX_SetNumChans(1);
    GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_VTX, GX_SRC_REG,
                   GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);
    GX_SetNumTevStages(1);
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetZMode(GX_DISABLE, GX_ALWAYS, GX_FALSE);
    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GX_InvVtxCache();
    GX_InvalidateTexAll();

    Mtx44 projection;
    guOrtho(projection, 0, SCREEN_H, 0, SCREEN_W, -1, 1);
    GX_LoadProjectionMtx(projection, GX_ORTHOGRAPHIC);
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;

    VIDEO_Init();
    WPAD_Init();
    rmode = VIDEO_GetPreferredMode(NULL);

    xfb[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    xfb[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb[0]);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();

    gp_fifo = memalign(32, 256 * 1024);
    gx_init();

    GameState game;
    game_init(&game);

    float cursor_x = SCREEN_W * 0.5f;
    float cursor_y = SCREEN_H * 0.5f;

    while (1) {
        WPAD_ScanPads();
        u32 down = WPAD_ButtonsDown(0);
        u32 held = WPAD_ButtonsHeld(0);

        if (down & WPAD_BUTTON_HOME) break;

        struct ir_t ir;
        WPAD_IR(0, &ir);
        if (ir.valid) {
            cursor_x = ir.x;
            cursor_y = ir.y;
            game_pointer(&game, cursor_x, cursor_y, held & WPAD_BUTTON_A);
        }

        game_update(&game, 1.0f / 60.0f);

        GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
        draw(&game, cursor_x, cursor_y);
        GX_DrawDone();
        GX_CopyDisp(xfb[0], GX_TRUE);
        GX_Flush();

        VIDEO_SetNextFramebuffer(xfb[0]);
        VIDEO_Flush();
        VIDEO_WaitVSync();
    }

    WPAD_Shutdown();
    free(gp_fifo);
    return 0;
}
