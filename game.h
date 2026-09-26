#pragma once
#include <stdint.h>

#define MAX_ITEMS 64
#define TARGET_COINS 500
#define START_TIME_SEC 90.0f

typedef enum {
    ITEM_COIN,
    ITEM_HONEYPOT,
    ITEM_CRUCIFIX,
    ITEM_CD
} ItemType;

typedef struct {
    ItemType type;
    float x, y, width, height;
    int active;
    uint32_t id;
} GameItem;

typedef enum {
    GAME_PLAYING,
    GAME_SUCCESS,
    GAME_OVER
} GamePhase;

typedef struct {
    int currentCoins;
    int targetCoins;
    float timeRemaining;
    float spawnTimer;
    float nextSpawn;
    GamePhase phase;
    uint32_t nextItemId;
    GameItem items[MAX_ITEMS];
} GameState;

void game_init(GameState *g);
void game_update(GameState *g, float dt);
void game_pointer(GameState *g, float x, float y, int pressed);
