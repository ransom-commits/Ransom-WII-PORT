#include "game.h"
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
                       (r < 0.009f) ? ITEM_HONEYPOT : ITEM_COIN;
            it->x = 80.0f + frand01() * 480.0f;
            it->y = 70.0f + frand01() * 330.0f;
            it->width = 48.0f;
            it->height = 48.0f;
            it->active = 1;
            it->id = g->nextItemId++;
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
        return;
    }

    g->spawnTimer += dt;
    if (g->spawnTimer >= g->nextSpawn) {
        g->spawnTimer = 0.0f;
        spawn_item(g);
        g->nextSpawn = 1.0f + frand01() * 59.0f;
    }

    if (g->currentCoins >= g->targetCoins)
        g->phase = GAME_SUCCESS;
}

void game_pointer(GameState *g, float x, float y, int pressed) {
    if (!pressed || g->phase != GAME_PLAYING) return;

    for (int i = 0; i < MAX_ITEMS; ++i) {
        GameItem *it = &g->items[i];
        if (!it->active) continue;
        if (x >= it->x && x <= it->x + it->width &&
            y >= it->y && y <= it->y + it->height) {
            if (it->type == ITEM_COIN) g->currentCoins++;
            it->active = 0;
            break;
        }
    }
}
