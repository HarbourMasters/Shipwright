#ifndef Z_EN_OKUTA_H
#define Z_EN_OKUTA_H

#include "global.h"
#include "objects/object_okuta/object_okuta.h"

struct EnOkuta;

typedef void (*EnOkutaActionFunc)(struct EnOkuta*, PlayState*);

#define EN_OKUTA_GET_NUM_CONSECUTIVE_PROJECTILES(thisx) (((thisx)->params >> 8) & 0xFF)
#define EN_OKUTA_GET_TYPE(thisx) ((thisx)->params)

typedef enum EnOkutaType {
    /*  0 */ EN_OKUTA_TYPE_RED_OCTOROK,
    /*  1 */ EN_OKUTA_TYPE_BLUE_OCTOROK, // Unused in the final game. This variant is invincible to all attacks and can chase the player underwater if they dive.
    /* 16 */ EN_OKUTA_TYPE_PROJECTILE_BASE = 16,
    /* 16 */ EN_OKUTA_TYPE_RED_PROJECTILE = EN_OKUTA_TYPE_RED_OCTOROK + EN_OKUTA_TYPE_PROJECTILE_BASE,
    /* 17 */ EN_OKUTA_TYPE_BLUE_PROJECTILE = EN_OKUTA_TYPE_BLUE_OCTOROK + EN_OKUTA_TYPE_PROJECTILE_BASE
} EnOkutaType;

typedef enum EnOkutaBodyPart {
    /*  0 */ EN_OKUTA_BODYPART_BODY,
    /*  1 */ EN_OKUTA_BODYPART_FRONT_LEFT_ARM_END,
    /*  2 */ EN_OKUTA_BODYPART_FRONT_RIGHT_ARM_END,
    /*  3 */ EN_OKUTA_BODYPART_BACK_LEFT_ARM_END,
    /*  4 */ EN_OKUTA_BODYPART_BACK_RIGHT_ARM_END,
    /*  5 */ EN_OKUTA_BODYPART_SNOUT,
    /*  6 */ EN_OKUTA_BODYPART_HEAD,
    /*  7 */ EN_OKUTA_BODYPART_EFFECTS_1,
    /*  8 */ EN_OKUTA_BODYPART_EFFECTS_2,
    /*  9 */ EN_OKUTA_BODYPART_EFFECTS_3,
    /* 10 */ EN_OKUTA_BODYPART_MAX
} EnOkutaBodyPart;

typedef struct EnOkuta {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnOkutaActionFunc actionFunc;
    /* 0x18C */ u8 drawDmgEffType;
    /* 0x18E */ s16 timer;
    /* 0x190 */ s16 numConsecutiveProjectiles; // when the Octorok starts shooting projectiles, it will shoot this many projectiles in a row before stopping
    /* 0x192 */ Vec3s jointTable[OCTOROK_LIMB_MAX];
    /* 0x1F2 */ Vec3s morphTable[OCTOROK_LIMB_MAX];
    /* 0x254 */ f32 drawDmgEffAlpha;
    /* 0x258 */ f32 drawDmgEffScale;
    /* 0x25C */ f32 drawDmgEffFrozenSteamScale;
    /* 0x260 */ f32 jumpHeight;
    /* 0x264 */ Vec3f headScale;
    /* 0x270 */ Vec3f bodyPartsPos[EN_OKUTA_BODYPART_MAX];
    /* 0x2E8 */ ColliderCylinder collider;
} EnOkuta; // size = 0x334

#endif // Z_EN_OKUTA_H
