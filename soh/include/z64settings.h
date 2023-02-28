#ifndef Z64SETTINGS_H
#define Z64SETTINGS_H

#include <libultraship/libultra.h>

typedef struct {
    s32 matchKeyColor;
} RandomizerSettings;

typedef struct {
    s32 language; // NTSC 0: Japanese; 1: English | PAL 0: English; 1: German; 2: French
    s32 zTargetSetting;
    s32 audioSetting;
    RandomizerSettings rando;
} GlobalSettingsStruct;

#endif
