#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"

static void *xfb[2];
static GXRModeObj *rmode;

static void draw(GameState *g) {
    printf("\x1b[2;2H R4NS0M SIMULATOR");
    printf("\x1b[4;2H Coins: %d / %d", g->currentCoins, g->targetCoins);
    printf("\x1b[5;2H Time: %.0f", g->timeRemaining);
    printf("\x1b[7;2H Phase: %s",
           g->phase == GAME_PLAYING ? "PLAYING" :
           g->phase == GAME_SUCCESS ? "SUCCESS" : "GAME OVER");
    printf("\x1b[9;2H Point at items with the Wii Remote.");
}

int main(int argc, char **argv) {
    VIDEO_Init();
    WPAD_Init();
    rmode = VIDEO_GetPreferredMode(NULL);
    xfb[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    xfb[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    console_init(xfb[0], 20, 20, rmode->fbWidth, rmode->xfbHeight,
                 rmode->fbWidth * VI_DISPLAY_PIX_SZ);
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb[0]);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();

    GameState game;
    game_init(&game);

    while (1) {
        WPAD_ScanPads();
        u32 held = WPAD_ButtonsHeld(0);
        u32 down = WPAD_ButtonsDown(0);

        if (down & WPAD_BUTTON_HOME) break;

        struct ir_t ir;
        WPAD_IR(0, &ir);
        if (ir.valid) {
            game_pointer(&game, ir.x, ir.y, held & WPAD_BUTTON_A);
        }

        game_update(&game, 1.0f / 60.0f);
        draw(&game);

        VIDEO_SetNextFramebuffer(xfb[0]);
        VIDEO_Flush();
        VIDEO_WaitVSync();
    }

    WPAD_Shutdown();
    return 0;
}
