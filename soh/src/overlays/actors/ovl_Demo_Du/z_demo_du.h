#ifndef Z_DEMO_DU_H
#define Z_DEMO_DU_H

#include "ultra64.h"
#include "global.h"

struct DemoDu;

typedef struct DemoDu {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ s16 eyeTexIndex;
    /* 0x0192 */ s16 blinkTimer;
    /* 0x0194 */ s16 mouthTexIndex;
    /* 0x0198 */ s32 updateIndex;
    /* 0x019C */ s32 drawIndex;
    /* 0x01A0 */ s32 unused;
    /* 0x01A4 */ f32 unk_1A4;
    /* 0x01A8 */ s32 shadowAlpha;
    /* 0x01AC */ s32 demo6KSpawned;
    /* 0x01B0 */ s32 lastAction;
} DemoDu; // size = 0x01B4

// This is the parameter of this actor,
typedef enum DemoDu_Cutscene {
    /* 0x00 */ DEMO_DU_CS_FIREMEDALLION, // default
    /* 0x01 */ DEMO_DU_CS_GORONS_RUBY,
    /* 0x02 */ DEMO_DU_CS_CHAMBER_AFTER_GANON,
    /* 0x03 */ DEMO_DU_CS_CREDITS
} DemoDu_Cutscene;

/**
 * To see each one of the cutscenes on the debug rom:
 * DEMO_DU_CS_FIREMEDALLION: I couldn't find how to trigger this one from the map selector, but you can go to Map 78 and beat Volvagia (or use the gameshark 8015E98B 0002 so the game thinks you already has beaten it).
 * DEMO_DU_CS_GORONS_RUBY: Map selector -> Map 16 (SPOT16) -> Stage 01
 * DEMO_DU_CS_CHAMBER_AFTER_GANON: Map selector -> Map 21 -> Stage 02
 * DEMO_DU_CS_CREDITS: Map selector -> Map 16 (SPOT16) -> Stage 04
 * 
 */

#endif
