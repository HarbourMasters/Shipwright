#ifndef Z_EN_GIANT_H
#define Z_EN_GIANT_H

#include "global.h"
#include "objects/object_giant/object_giant.h"

#define GIANT_TYPE(thisx) ((thisx)->params & 0xF)

/**
 * The giants are divided into types based on where in the game they appear.
 * - TERMINA_FIELD: These appear in all non-ending cutscenes that take place in Termina Field.
 * - CLOCK_TOWER_SUCCESS: These appear in the Clock Tower when the Oath to Order is played and all giants are freed.
 * - GIANTS_CHAMBER_AND_ENDING: These giants appear in the Giant's Chamber or in all cutscenes that play after defeating Majora.
 * - CLOCK_TOWER_FAILURE: These appear in the Clock Tower when the Oath to Order is played and all giants are NOT freed.
 * 
 * The SWAMP, MOUNTAIN, OCEAN, and CANYON signifiers are used to identify which of the four giants it is,
 * but the devs were not always consistent about this. For example, the giants in the Giant's Chamber are
 * always GIANT_TYPE_MOUNTAIN_GIANTS_CHAMBER_AND_ENDING, regardless of which boss was just defeated. These
 * are named such that EnGiant_IsImprisoned has the expected behavior; the devs were only inconsistent in
 * the cases where they weren't calling this function.
 */
typedef enum {
    /*  0 */ GIANT_TYPE_MOUNTAIN_TERMINA_FIELD,
    /*  1 */ GIANT_TYPE_CANYON_TERMINA_FIELD,
    /*  2 */ GIANT_TYPE_SWAMP_TERMINA_FIELD,
    /*  3 */ GIANT_TYPE_OCEAN_TERMINA_FIELD,
    /*  4 */ GIANT_TYPE_MOUNTAIN_CLOCK_TOWER_SUCCESS,
    /*  5 */ GIANT_TYPE_CANYON_CLOCK_TOWER_SUCCESS,
    /*  6 */ GIANT_TYPE_SWAMP_CLOCK_TOWER_SUCCESS,
    /*  7 */ GIANT_TYPE_OCEAN_CLOCK_TOWER_SUCCESS,
    /*  8 */ GIANT_TYPE_MOUNTAIN_GIANTS_CHAMBER_AND_ENDING,
    /*  9 */ GIANT_TYPE_CANYON_GIANTS_CHAMBER_AND_ENDING,
    /* 10 */ GIANT_TYPE_SWAMP_GIANTS_CHAMBER_AND_ENDING,
    /* 11 */ GIANT_TYPE_OCEAN_GIANTS_CHAMBER_AND_ENDING,
    /* 12 */ GIANT_TYPE_MOUNTAIN_CLOCK_TOWER_FAILURE,
    /* 13 */ GIANT_TYPE_CANYON_CLOCK_TOWER_FAILURE,
    /* 14 */ GIANT_TYPE_SWAMP_CLOCK_TOWER_FAILURE,
    /* 15 */ GIANT_TYPE_OCEAN_CLOCK_TOWER_FAILURE
} GiantType;

struct EnGiant;

typedef void (*EnGiantActionFunc)(struct EnGiant*, PlayState*);

typedef struct EnGiant {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[GIANT_LIMB_MAX];
    /* 0x1E8 */ Vec3s morphTable[GIANT_LIMB_MAX];
    /* 0x248 */ s16 animIndex;
    /* 0x24A */ u16 cueType;
    /* 0x24C */ u16 cueId;
    /* 0x24E */ s16 alpha;
    /* 0x250 */ u16 sfxId;
    /* 0x254 */ MtxF headDrawMtxF;
    /* 0x294 */ s16 faceIndex;
    /* 0x296 */ s16 blinkTimer;
    /* 0x298 */ EnGiantActionFunc actionFunc;
} EnGiant; // size = 0x29C

#endif // Z_EN_GIANT_H
