#ifndef Z_EN_SSH_H
#define Z_EN_SSH_H

#include "global.h"
#include "objects/object_ssh/object_ssh.h"
#include "overlays/actors/ovl_En_Si/z_en_si.h"
#include "overlays/actors/ovl_En_Sth/z_en_sth.h"

struct EnSsh;

typedef void (*EnSshActionFunc)(struct EnSsh*, PlayState*);

#define SSH_STATE_STUNNED (1 << 0)
#define SSH_STATE_GROUND_START (1 << 2)
#define SSH_STATE_ATTACKED (1 << 3)
#define SSH_STATE_SPIN (1 << 4)
#define SSH_STATE_FATHER (1 << 5)

#define ENSSH_IS_CHILD(thisx) ((thisx)->params & 0xF)

typedef struct EnSsh {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[OBJECT_SSH_LIMB_MAX];
    /* 0x23C */ Vec3s morphTable[OBJECT_SSH_LIMB_MAX];
    /* 0x2F0 */ EnSshActionFunc actionFunc;
    /* 0x2F4 */ ColliderCylinder collider1[6];
    /* 0x4BC */ ColliderJntSph collider2;
    /* 0x4DC */ ColliderJntSphElement collider2Elements[1];
    /* 0x51C */ s16 initialYaw;
    /* 0x51E */ s16 maxTurnRate;
    /* 0x520 */ s16 unkTimer;
    /* 0x522 */ s16 spinTimer;
    /* 0x524 */ s16 hitTimer;
    /* 0x526 */ s16 invincibilityTimer;
    /* 0x528 */ s16 sfxTimer;
    /* 0x52A */ s16 stunTimer;
    /* 0x52C */ s16 animTimer;
    /* 0x52E */ s16 swayTimer;
    /* 0x530 */ s32 blureIdx;
    /* 0x534 */ f32 colliderScale;
    /* 0x538 */ f32 floorHeightOffset;
    /* 0x53C */ Vec3f ceilingPos;
    /* 0x548 */ UNK_TYPE1 unk_548[0x78];
    /* 0x5C0 */ s16 swayAngle;
    /* 0x5C2 */ u16 stateFlags;
    /* 0x5C4 */ u8 hitCount;
    /* 0x5C6 */ s16 blinkState;
    /* 0x5C8 */ s16 blinkTimer;
} EnSsh; // size = 0x5CC

#endif // Z_EN_SSH_H
