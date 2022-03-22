#ifndef Z_BG_JYA_BIGMIRROR_H
#define Z_BG_JYA_BIGMIRROR_H

#include "ultra64.h"
#include "global.h"
#include "overlays/actors/ovl_Bg_Jya_Cobra/z_bg_jya_cobra.h"

#define BIGMIR_PUZZLE_COBRA1_SOLVED (1 << 0)
#define BIGMIR_PUZZLE_COBRA2_SOLVED (1 << 1)
#define BIGMIR_PUZZLE_BOMBIWA_DESTROYED (1 << 2)
#define BIGMIR_PUZZLE_IN_STATUE_ROOM (1 << 3)
#define BIGMIR_PUZZLE_IN_1ST_TOP_ROOM (1 << 4)
#define BIGMIR_PUZZLE_IN_2ND_TOP_ROOM (1 << 5)

struct BgJyaBigmirror;

typedef struct {
    /* 0x00 */ BgJyaCobra* cobra;
    /* 0x04 */ s16 rotY;
} BigmirrorCobra; // size = 0x08

typedef struct BgJyaBigmirror {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ BigmirrorCobra cobraInfo[2];
    /* 0x015C */ u8 puzzleFlags;
    /* 0x015D */ u8 spawned;
    /* 0x0160 */ Actor* lightBeams[3];
    /* 0x016C */ s32 mirRayObjIndex;
    /* 0x0170 */ f32 liftHeight;
} BgJyaBigmirror; // size = 0x0174

#endif
