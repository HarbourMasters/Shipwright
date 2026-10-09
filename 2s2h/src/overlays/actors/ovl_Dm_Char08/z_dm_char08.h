#ifndef Z_DM_CHAR08_H
#define Z_DM_CHAR08_H

#include "global.h"

struct DmChar08;

typedef void (*DmChar08ActionFunc)(struct DmChar08*, PlayState*);

typedef struct DmChar08 {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ SkelAnime skelAnime;
    /* 0x1A0 */ DmChar08ActionFunc actionFunc;
    /* 0x1A4 */ Actor* palmTree1;
    /* 0x1A8 */ Actor* palmTree2;
    /* 0x1AC */ Vec3f tree1Pos;
    /* 0x1B8 */ Vec3f tree2Pos;
    /* 0x1C4 */ Vec3f bubblePos;
    /* 0x1D0 */ Vec3f focusPos;
    /* 0x1DC */ UNK_TYPE1 unk_1DC[8];
    /* 0x1E4 */ f32 targetYPos;
    /* 0x1E8 */ UNK_TYPE1 unk_1E8[8];
    /* 0x1F0 */ f32 unk_1F0;
    /* 0x1F4 */ s16 unk_1F4;
    /* 0x1F6 */ s16 cueId;
    /* 0x1F6 */ s16 blinkTimer;
    /* 0x1FA */ s16 unk_1FA;
    /* 0x1FC */ u16 unk_1FC;
    /* 0x1FE */ u8 bubbleCount;
    /* 0x1FF */ u8 unk_1FF;
    /* 0x200 */ u8 eyeMode;
    /* 0x201 */ u8 eyeIndex;
    /* 0x202 */ u8 animIndex;
    /* 0x203 */ u8 prevAnimIndex;
    /* 0x204 */ UNK_TYPE1 unk_204;
    /* 0x205 */ u8 alpha;
    /* 0x206 */ u8 unk_206;
    /* 0x207 */ u8 unk_207;
    /* 0x208 */ u8 unk_208;
    /* 0x209 */ u8 dynapolyInitialized;
} DmChar08; // size = 0x20C

#endif // Z_DM_CHAR08_H
