#include "game.h"
#include "sound.h"
#include <stdlib.h>

static float frand01(void) {
    return (float)rand() / (float)RAND_MAX;
}

static void spawn_item(GameState *g) {
    for (int i = 0; i < MAX_ITEMS; ++i) {
        if (!g->items[i].active) {
            GameItem *it = &g->items[i];
            float r = frand01();

            it->type = (r < 0.004f) ? ITEM_CRUCIFIX :
                       (r < 0.009f) ? ITEM_HONEYPOT :
                       (r < 0.012f) ? ITEM_CD : ITEM_COIN;

            it->x = 30.0f + frand01() * 550.0f;
            it->y = 80.0f + frand01() * 350.0f;
            it->width = 42.0f;
            it->height = 42.0f;
            it->active = 1;
            it->id = g->nextItemId++;
            sound_play_item_spawned();
            return;
        }
    }
}

void game_init(GameState *g) {
    *g = (GameState){0};
    g->targetCoins = TARGET_COINS;
    g->timeRemaining = START_TIME_SEC;
    g->phase = GAME_PLAYING;
    g->nextItemId = 1;
    g->nextSpawn = 1.0f;
}

void game_update(GameState *g, float dt) {
    if (g->phase != GAME_PLAYING) return;

    g->timeRemaining -= dt;
    if (g->timeRemaining <= 0.0f) {
        g->timeRemaining = 0.0f;
        g->phase = GAME_OVER;
        sound_play_gameover();
        return;
    }

    g->spawnTimer += dt;
    if (g->spawnTimer >= g->nextSpawn) {
        g->spawnTimer = 0.0f;
        spawn_item(g);
        sound_play_spawn();
        g->nextSpawn = 1.0f + frand01() * 59.0f;
    }

    if (g->currentCoins >= g->targetCoins) {
        g->phase = GAME_SUCCESS;
        sound_play_success();
    }
}

void game_pointer(GameState *g, float x, float y, int pressed) {
    if (!pressed || g->phase != GAME_PLAYING) return;

    for (int i = 0; i < MAX_ITEMS; ++i) {
        GameItem *it = &g->items[i];
        if (!it->active) continue;

        if (x >= it->x && x <= it->x + it->width &&
            y >= it->y && y <= it->y + it->height) {
            if (it->type == ITEM_COIN) {
                g->currentCoins++;
                sound_play_coin();
            } else if (it->type == ITEM_HONEYPOT) {
                sound_play_honeypot();
            } else if (it->type == ITEM_CRUCIFIX) {
                sound_play_crucifix();
            }
            it->active = 0;
            break;
        }
    }
}
