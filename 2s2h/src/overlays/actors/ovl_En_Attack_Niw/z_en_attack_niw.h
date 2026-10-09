#ifndef Z_EN_ATTACK_NIW_H
#define Z_EN_ATTACK_NIW_H

#include "global.h"
#include "objects/object_niw/object_niw.h"

struct EnAttackNiw;

typedef void (*EnAttackNiwActionFunc)(struct EnAttackNiw*, PlayState*);

typedef struct EnAttackNiw {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[NIW_LIMB_MAX];
    /* 0x1E8 */ Vec3s morphTable[NIW_LIMB_MAX];
    /* 0x248 */ EnAttackNiwActionFunc actionFunc;
    /* 0x24C */ s16 unkTimer24C; // control behavior in _UpdateRotations
    /* 0x24E */ s16 unusedTimer24E; // set never read
    /* 0x250 */ s16 unkTimer250; // control behavior in _UpdateRotations
    /* 0x252 */ s16 hopTimer;
    /* 0x254 */ s16 randomAngleChangeTimer;
    /* 0x256 */ s16 crySfxTimer;
    /* 0x258 */ s16 flutterSfxTimer;
    /* 0x25A */ s16 unkTimer25A; // set to 50 in run at player, locks fly away without water?
    /* 0x25C */ f32 targetBodyRotY;
    /* 0x260 */ f32 targetRightWingRotZ;
    /* 0x264 */ f32 targetLeftWingRotZ;
    /* 0x268 */ f32 unusedRotations[2]; // in EnNiw this is an array of targetRot
    /* 0x270 */ f32 targetRightWingRotY;
    /* 0x274 */ f32 targetRightWingRotX;
    /* 0x278 */ f32 targetLeftWingRotY;
    /* 0x27C */ f32 targetLeftWingRotX;
    /* 0x280 */ f32 targetHeadRotZ;
    /* 0x284 */ s16 unusedCounter284; // incremented in _Update, never read
    /* 0x286 */ s16 clearRotYToggleTimer;
    /* 0x288 */ UNK_TYPE1 pad288[2];
    /* 0x28A */ s16 unkToggle28A;
    /* 0x28C */ UNK_TYPE1 pad28C[4];
    /* 0x290 */ Vec3f randomTargetCenterOffset; // see EnAttackNiw_EnterViewFromOffscreen
    /* 0x29C */ f32 leftWingRotZ;
    /* 0x2A0 */ f32 leftWingRotY;
    /* 0x2A4 */ f32 leftWingRotX;
    /* 0x2A8 */ f32 rightWingRotZ;
    /* 0x2AC */ f32 rightWingRotY;
    /* 0x2B0 */ f32 rightWingRotX;
    /* 0x2B4 */ f32 upperBodyRotY;
    /* 0x2B8 */ f32 headRotZ;
    /* 0x2BC */ UNK_TYPE1 pad2BC[0xC]; // unused rotations?
    /* 0x2C8 */ f32 targetRotX;
    /* 0x2CC */ f32 targetRotY;
    /* 0x2D0 */ UNK_TYPE1 pad2D0[4];
    /* 0x2D4 */ f32 rotStep;
    /* 0x2D8 */ f32 targetXZSpeed;
    /* 0x2DC */ f32 targetHeight; // never set, but sent to Actor_SetFocus 
} EnAttackNiw; // size = 0x2E0


#define ATTACK_NIW_REGULAR  0    // spawned by EnNiw

#endif // Z_EN_ATTACK_NIW_H
