#include "sound.h"
#include <gccore.h>
#include <asndlib.h>
#include <string.h>

#define SOUND_RATE 22050
#define SOUND_VOL  255

/*
 * The raw files are embedded by the Makefile when present. They are weak
 * symbols so the program still builds while the asset pack is being added.
 */
#define DECLARE_SOUND(name) \
    extern const unsigned char _binary_assets_sounds_##name##_raw_start[] __attribute__((weak)); \
    extern const unsigned char _binary_assets_sounds_##name##_raw_end[] __attribute__((weak))

DECLARE_SOUND(coin);
DECLARE_SOUND(item_spawned);
DECLARE_SOUND(spawn);
DECLARE_SOUND(honeypot_break);
DECLARE_SOUND(crucifix);
DECLARE_SOUND(ransom_success);

static int sound_ready = 0;

static void play_sample(const unsigned char *start, const unsigned char *end, int voice)
{
    if (!sound_ready || !start || !end || end <= start) return;

    s32 size = (s32)(end - start);
    ASND_SetVoice(voice, VOICE_MONO_8BIT_U, SOUND_RATE, 0,
                  (void *)start, size, SOUND_VOL, SOUND_VOL, NULL);
}

void sound_init(void)
{
    ASND_Init();
    sound_ready = 1;
}

void sound_shutdown(void)
{
    if (!sound_ready) return;
    ASND_End();
    sound_ready = 0;
}

void sound_play_coin(void)
{
    play_sample(_binary_assets_sounds_coin_raw_start,
                _binary_assets_sounds_coin_raw_end, 1);
}

void sound_play_item_spawned(void)
{
    play_sample(_binary_assets_sounds_item_spawned_raw_start,
                _binary_assets_sounds_item_spawned_raw_end, 2);
}

void sound_play_spawn(void)
{
    play_sample(_binary_assets_sounds_spawn_raw_start,
                _binary_assets_sounds_spawn_raw_end, 3);
}

void sound_play_honeypot(void)
{
    play_sample(_binary_assets_sounds_honeypot_break_raw_start,
                _binary_assets_sounds_honeypot_break_raw_end, 4);
}

void sound_play_crucifix(void)
{
    play_sample(_binary_assets_sounds_crucifix_raw_start,
                _binary_assets_sounds_crucifix_raw_end, 5);
}

void sound_play_success(void)
{
    play_sample(_binary_assets_sounds_ransom_success_raw_start,
                _binary_assets_sounds_ransom_success_raw_end, 6);
}

void sound_play_gameover(void)
{
    play_sample(_binary_assets_sounds_crucifix_raw_start,
                _binary_assets_sounds_crucifix_raw_end, 7);
}
