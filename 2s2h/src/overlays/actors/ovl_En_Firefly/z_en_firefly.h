#ifndef Z_EN_FIREFLY_H
#define Z_EN_FIREFLY_H

#include "global.h"
#include "objects/object_firefly/object_firefly.h"

struct EnFirefly;

typedef void (*EnFireflyActionFunc)(struct EnFirefly*, PlayState*);

typedef enum {
    /* 0 */ KEESE_FIRE_FLY,
    /* 2 */ KEESE_NORMAL_FLY = 2,
    /* 3 */ KEESE_NORMAL_PERCH,
    /* 4 */ KEESE_ICE_FLY
} KeeseMainType;

#define KEESE_INVISIBLE (1 << 0xF)
#define KEESE_GET_MAIN_TYPE(thisx) ((thisx)->params & 0x7FFF)

typedef enum KeeseBodyPart {
    /* 0 */ KEESE_BODYPART_LEFT_WING_END,
    /* 1 */ KEESE_BODYPART_RIGHT_WING_END_ROOT,
    /* 2 */ KEESE_BODYPART_BODY,
    /* 3 */ KEESE_BODYPART_MAX
} KeeseBodyPart;

typedef struct EnFirefly {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnFireflyActionFunc actionFunc;
    /* 0x18C */ u8 auraType;
    /* 0x18D */ u8 currentType;
    /* 0x18E */ u8 isInvisible;
    /* 0x18F */ u8 drawDmgEffType;
    /* 0x190 */ s16 timer;
    /* 0x192 */ s16 pitchTarget;
    /* 0x194 */ Vec3s jointTable[FIRE_KEESE_LIMB_MAX];
    /* 0x23C */ Vec3s morphTable[FIRE_KEESE_LIMB_MAX];
    /* 0x2E4 */ f32 maxAltitude;
    /* 0x2E8 */ f32 drawDmgEffAlpha;
    /* 0x2EC */ f32 drawDmgEffScale;
    /* 0x2F0 */ f32 drawDmgEffFrozenSteamScale;
    /* 0x2F4 */ u32 lastDrawnFrame;
    /* 0x2F8 */ Vec3f bodyPartsPos[KEESE_BODYPART_MAX];
    /* 0x31C */ ColliderSphere collider;
} EnFirefly; // size = 0x374

#endif // Z_EN_FIREFLY_H
