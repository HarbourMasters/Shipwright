#ifndef Z_DM_GM_H
#define Z_DM_GM_H

#include "global.h"
#include "objects/object_an1/object_an1.h"

struct DmGm;

typedef void (*DmGmActionFunc)(struct DmGm*, PlayState*);

#define DMGM_STATE_LOST_ATTENTION (1 << 0)
#define DMGM_STATE_FACE_TARGET    (1 << 1)

typedef struct DmGm {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ DmGmActionFunc actionFunc;
    /* 0x18C */ Vec3f headComputedPos;
    /* 0x18C */ Vec3f torsoComputedPos;
    /* 0x1A4 */ Vec3s headComputedRot;
    /* 0x1A4 */ Vec3s torsoComputedRot;
    /* 0x1B0 */ Vec3s jointTable[ANJU1_LIMB_MAX];
    /* 0x22E */ Vec3s morphTable[ANJU1_LIMB_MAX];
    /* 0x2AC */ s8 an4ObjectSlot;
    /* 0x2AD */ s8 msmoObjectSlot;
    /* 0x2AE */ u16 stateFlags;
    /* 0x2B0 */ u8 cueId;
    /* 0x2B4 */ Actor* lookAtActor;
    /* 0x2B8 */ s16 eyeTexIndex;
    /* 0x2BA */ s16 eyeTimer;
    /* 0x2BC */ s16 loseAttentionTimer;
    /* 0x2BE */ s16 headRotZ;
    /* 0x2C0 */ s16 headRotY;
    /* 0x2C2 */ s16 torsoRotZ;
    /* 0x2C4 */ s16 torsoRotY;
    /* 0x2C8 */ s32 animIndex;
    /* 0x2CC */ s32 prevAnimIndex;
    /* 0x2D0 */ s32 isCutscenePlaying;
    /* 0x2D4 */ s32 didAnimChangeInCs;
} DmGm; // size = 0x2D8

#endif // Z_DM_GM_H
