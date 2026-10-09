#ifndef Z_EN_IK_H
#define Z_EN_IK_H

#include "global.h"
#include "objects/object_ik/object_ik.h"

struct EnIk;

typedef void (*EnIkActionFunc)(struct EnIk*, PlayState*);

#define IK_GET_ARMOR_TYPE(thisx) ((thisx)->params & 0xFF)

typedef enum {
    /* 1 */ IK_TYPE_SILVER = 1,
    /* 2 */ IK_TYPE_BLACK,
    /* 3 */ IK_TYPE_WHITE
} EnIkType;

typedef struct {
    /* 0x00 */ Gfx* dList;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3s rot;
    /* 0x24 */ s32 enabled;
} IronKnuckleEffect; // size = 0x28

typedef enum IronKnuckleBodyPart {
    /*  0 */ IRON_KNUCKLE_BODYPART_TASSET_RIGHT,
    /*  1 */ IRON_KNUCKLE_BODYPART_RIGHT_LEG,
    /*  2 */ IRON_KNUCKLE_BODYPART_RIGHT_FOOT,
    /*  3 */ IRON_KNUCKLE_BODYPART_TASSET_LEFT,
    /*  4 */ IRON_KNUCKLE_BODYPART_LEFT_LEG,
    /*  5 */ IRON_KNUCKLE_BODYPART_LEFT_FOOT,
    /*  6 */ IRON_KNUCKLE_BODYPART_HELMET_ARMOR,
    /*  7 */ IRON_KNUCKLE_BODYPART_RIGHT_FOREARM,
    /*  8 */ IRON_KNUCKLE_BODYPART_RIGHT_HAND,
    /*  9 */ IRON_KNUCKLE_BODYPART_LEFT_FOREARM,
    /* 10 */ IRON_KNUCKLE_BODYPART_LEFT_HAND,
    /* 11 */ IRON_KNUCKLE_BODYPART_LOWER_LEFT_PAULDRON,
    /* 12 */ IRON_KNUCKLE_BODYPART_LOWER_RIGHT_PAULDRON,
    /* 13 */ IRON_KNUCKLE_BODYPART_MAX
} IronKnuckleBodyPart;

typedef enum IronKnuckleArmorBodyPart {
    /* 0 */ IRON_KNUCKLE_ARMOR_BODYPART_HELMET,
    /* 1 */ IRON_KNUCKLE_ARMOR_BODYPART_CHEST_FRONT,
    /* 2 */ IRON_KNUCKLE_ARMOR_BODYPART_CHEST_BACK,
    /* 3 */ IRON_KNUCKLE_ARMOR_BODYPART_UPPER_LEFT_PAULDRON,
    /* 4 */ IRON_KNUCKLE_ARMOR_BODYPART_UPPER_RIGHT_PAULDRON,
    /* 5 */ IRON_KNUCKLE_ARMOR_BODYPART_LOWER_LEFT_PAULDRON,
    /* 6 */ IRON_KNUCKLE_ARMOR_BODYPART_LOWER_RIGHT_PAULDRON,
    /* 7 */ IRON_KNUCKLE_ARMOR_BODYPART_MAX
} IronKnuckleArmorBodyPart;

typedef struct EnIk {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ SkelAnime skelAnime;
    /* 0x0188 */ Vec3s jointTable[IRON_KNUCKLE_LIMB_MAX];
    /* 0x023C */ Vec3s morphTable[IRON_KNUCKLE_LIMB_MAX];
    /* 0x02F0 */ EnIkActionFunc actionFunc;
    /* 0x02F4 */ u8 drawArmorFlags; // Value is 0 when Iron knuckle has armor, then changes to 5 for when armor is lost
    /* 0x02F5 */ u8 drawDmgEffType;
    /* 0x02F6 */ s16 timer;
    /* 0x02F8 */ s16 blurEffectSpawnLock;
    /* 0x02FA */ s16 subCamId;
    /* 0x02FC */ s16 invincibilityFrames;
    /* 0x0300 */ s32 effectIndex;
    /* 0x0304 */ f32 drawDmgEffAlpha;
    /* 0x0308 */ f32 drawDmgEffScale;
    /* 0x030C */ f32 drawDmgEffFrozenSteamScale;
    /* 0x0310 */ Vec3f bodyPartsPos[IRON_KNUCKLE_BODYPART_MAX];
    /* 0x03AC */ ColliderCylinder colliderCylinder;
    /* 0x03F8 */ ColliderQuad colliderQuad;
    /* 0x0478 */ ColliderTris colliderTris;
    /* 0x0498 */ ColliderTrisElement shieldColliderElements[2];
    /* 0x0550 */ IronKnuckleEffect effects[IRON_KNUCKLE_ARMOR_BODYPART_MAX];
} EnIk; // size = 0x668

#endif // Z_EN_IK_H
