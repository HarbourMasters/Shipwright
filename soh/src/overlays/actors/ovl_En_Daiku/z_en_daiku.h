#ifndef Z_EN_DAIKU_H
#define Z_EN_DAIKU_H

#include "ultra64.h"
#include "global.h"

struct EnDaiku;

typedef void (*EnDaikuActionFunc)(struct EnDaiku*, PlayState*);

typedef struct EnDaiku {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnDaikuActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ char unk_1E0[4];
    /* 0x01E4 */ s32 talkState;
    /* 0x01E8 */ char unk_1E8[4];
    /* 0x01EC */ s32 waypoint;
    /* 0x01F0 */ f32 runSpeed;
    /* 0x01F4 */ s32 currentAnimIndex;
    /* 0x01F8 */ char unk_1F8[4];
    /* 0x01FC */ s32 subCamActive;
    /* 0x0200 */ s32 escapeSubCamTimer;
    /* 0x0204 */ s32 subCamId;
    /* 0x0208 */ s16 rotYtowardsPath;
    /* 0x020C */ Vec3f subCamEyeInit;
    /* 0x0218 */ Vec3f subCamAtTarget;
    /* 0x0224 */ Vec3f subCamEye;
    /* 0x0230 */ Vec3f subCamAt;
    /* 0x023C */ s32 stateFlags;
    /* 0x0240 */ s32 startFightSwitchFlag;
    /* 0x0244 */ struct_80034A14_arg1 unk_244; // probably related to animating torso and head towards the player
    /* 0x026C */ Vec3s jointTable[17];
    /* 0x02D2 */ Vec3s morphTable[17];
    /* 0x0338 */ Vec3s initRot;
    /* 0x0340 */ Vec3f initPos;
} EnDaiku; // size = 0x034C

#endif
