#ifndef Z64EFF_FOOTMARK_H
#define Z64EFF_FOOTMARK_H

#include "ultra64.h"
#include "z64math.h"

struct PlayState;
struct Actor;

typedef struct EffFootmark {
    /* 0x00 */ MtxF mf;
    /* 0x40 */ struct Actor* actor;
    /* 0x44 */ Vec3f pos;
    /* 0x50 */ u8 flags; // bit 0 - footmark fades out
    /* 0x51 */ u8 id;
    /* 0x52 */ u8 red;
    /* 0x53 */ u8 blue;
    /* 0x54 */ u8 green;
    /* 0x56 */ u16 alpha;
    /* 0x58 */ u16 alphaChange;
    /* 0x5A */ u16 size;
    /* 0x5C */ u16 fadeOutDelay;
    /* 0x5E */ u16 age;
} EffFootmark; // size = 0x60

#define FOOTMARK_FLAG_1 (1 << 0)

void EffFootmark_Init(struct PlayState* play);
void EffFootmark_Add(struct PlayState* play, MtxF* mf, struct Actor* actor, u8 id, Vec3f* pos, u16 size, u8 red, u8 green, u8 blue, u16 alpha, u16 alphaChange, u16 fadeOutDelay);
void EffFootmark_Update(struct PlayState* play);
void EffFootmark_Draw(struct PlayState* play);

#endif
