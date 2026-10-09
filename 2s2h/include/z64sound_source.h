#ifndef Z64SOUND_SOURCE_H
#define Z64SOUND_SOURCE_H

#include "ultra64.h"
#include "z64math.h"

struct PlayState;

typedef struct {
    /* 0x00 */ u8 countdown;
    /* 0x01 */ u8 playSfxEachFrame;
    /* 0x02 */ u16 sfxId;
    /* 0x04 */ Vec3f worldPos;
    /* 0x10 */ Vec3f projectedPos;
} SoundSource; // size = 0x1C

#define SOUND_SOURCE_COUNT 16

void SoundSource_InitAll(struct PlayState* play);
void SoundSource_UpdateAll(struct PlayState* play);
void SoundSource_PlaySfxAtFixedWorldPos(struct PlayState* play, Vec3f* worldPos, u32 duration, u16 sfxId);
void SoundSource_PlaySfxEachFrameAtFixedWorldPos(struct PlayState* play, Vec3f* worldPos, u32 duration, u16 sfxId);

#endif
