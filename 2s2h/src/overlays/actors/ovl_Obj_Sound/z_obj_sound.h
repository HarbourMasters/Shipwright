#ifndef Z_OBJ_SOUND_H
#define Z_OBJ_SOUND_H

#include "global.h"

struct ObjSound;

#define OBJ_SOUND_GET_TYPE(thisx) (((thisx)->params >> 8) & 0xFF)
#define OBJ_SOUND_GET_SFX_TYPE(thisx) (((thisx)->params >> 7) & 1)
#define OBJ_SOUND_ID_MASK 0x7F

typedef enum {
    /* 0 */ OBJ_SOUND_TYPE_SFX,
    /* 1 */ OBJ_SOUND_TYPE_BGM,
    /* 2 */ OBJ_SOUND_TYPE_FIXED_SFX,
    /* 3 */ OBJ_SOUND_TYPE_FANFARE
} ObjSoundType;

typedef struct ObjSound {
    /* 0x000 */ Actor actor;
    /* 0x144 */ u8 unk_144;
    /* 0x145 */ u8 sfxType;
    /* 0x146 */ s16 soundType;
} ObjSound; // size = 0x148

#endif // Z_OBJ_SOUND_H
