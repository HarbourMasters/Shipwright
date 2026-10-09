#ifndef Z_DM_AH_H
#define Z_DM_AH_H

#include "global.h"
#include "objects/object_ah/object_ah.h"

struct DmAh;

typedef void (*DmAhActionFunc)(struct DmAh*, PlayState*);

typedef struct DmAh {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ DmAhActionFunc actionFunc;
    /* 0x18C */ Vec3f unk_18C;
    /* 0x198 */ Vec3f unk_198;
    /* 0x1A4 */ Vec3s unk_1A4;
    /* 0x1AA */ Vec3s unk_1AA;
    /* 0x1B0 */ Vec3s jointTable[OBJECT_AH_LIMB_MAX];
    /* 0x216 */ Vec3s morphTable[OBJECT_AH_LIMB_MAX];
    /* 0x27C */ u16 unk_27C;
    /* 0x27E */ u8 cueId;
    /* 0x280 */ Actor* unk_280;
    /* 0x284 */ s16 unk_284;
    /* 0x286 */ s16 unk_286;
    /* 0x288 */ s16 unk_288;
    /* 0x28A */ s16 unk_28A;
    /* 0x28C */ s16 unk_28C;
    /* 0x28E */ s16 unk_28E;
    /* 0x290 */ s16 unk_290;
    /* 0x294 */ s32 animIndex;
    /* 0x298 */ s32 prevAnimIndex;
    /* 0x29C */ s32 isCutscenePlaying;
} DmAh; // size = 0x2A0

#endif // Z_DM_AH_H
