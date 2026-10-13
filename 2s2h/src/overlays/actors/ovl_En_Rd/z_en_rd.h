#ifndef Z_EN_RD_H
#define Z_EN_RD_H

#include "global.h"
#include "objects/object_rd/object_rd.h"

struct EnRd;

typedef void (*EnRdActionFunc)(struct EnRd*, PlayState*);
typedef void (*EnRdSetupDanceFunc)(struct EnRd*);

#define EN_RD_GET_80(thisx) ((thisx)->params & 0x80)
#define EN_RD_GET_FLAGS(thisx) (((thisx)->params & 0xFF00) >> 8)
#define EN_RD_GET_TYPE(thisx) ((thisx)->params)
#define EN_RD_GET_SWITCH_FLAG(this) ((this)->flags & 0x7F)

#define EN_RD_FLAG_CANNOT_FREEZE_PLAYER (1 << 7)

typedef enum {
    /* -3 */ EN_RD_TYPE_GIBDO_RISING_OUT_OF_COFFIN = -3, // Unused OoT leftover
    /* -2 */ EN_RD_TYPE_GIBDO = -2, // Unused OoT leftover
    /* -1 */ EN_RD_TYPE_DOES_NOT_MOURN = -1, // Unused OoT leftover
    /*  0 */ EN_RD_TYPE_DOES_NOT_MOURN_IF_WALKING = 0, // Unused OoT leftover. Still mourns, but it must come to a complete stop first
    /*  1 */ EN_RD_TYPE_REGULAR, // Unused OoT leftover
    /*  2 */ EN_RD_TYPE_CRYING, // Unused in the final game
    /*  3 */ EN_RD_TYPE_INVISIBLE, // Unused OoT leftover
    /*  4 */ EN_RD_TYPE_FROZEN, // Unused in the final game
    /*  5 */ EN_RD_TYPE_SQUATTING_DANCE,
    /*  6 */ EN_RD_TYPE_CLAPPING_DANCE,
    /*  7 */ EN_RD_TYPE_PIROUETTE
} EnRdType;

typedef enum EnRdBodyPart {
    /*  0 */ EN_RD_BODYPART_0,
    /*  1 */ EN_RD_BODYPART_1,
    /*  2 */ EN_RD_BODYPART_2,
    /*  3 */ EN_RD_BODYPART_3,
    /*  4 */ EN_RD_BODYPART_4,
    /*  5 */ EN_RD_BODYPART_5,
    /*  6 */ EN_RD_BODYPART_6,
    /*  7 */ EN_RD_BODYPART_7,
    /*  8 */ EN_RD_BODYPART_8,
    /*  9 */ EN_RD_BODYPART_9,
    /* 10 */ EN_RD_BODYPART_10,
    /* 11 */ EN_RD_BODYPART_11,
    /* 12 */ EN_RD_BODYPART_12,
    /* 13 */ EN_RD_BODYPART_13,
    /* 14 */ EN_RD_BODYPART_14,
    /* 15 */ EN_RD_BODYPART_MAX
} EnRdBodyPart;

typedef struct EnRd {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnRdActionFunc actionFunc;
    /* 0x18C */ EnRdSetupDanceFunc setupDanceFunc;
    /* 0x190 */ ColliderCylinder collider;
    /* 0x1DC */ Vec3f bodyPartsPos[EN_RD_BODYPART_MAX];
    /* 0x290 */ s32 bodyPartIndex;
    /* 0x294 */ f32 drawDmgEffAlpha;
    /* 0x298 */ f32 drawDmgEffScale;
    /* 0x29C */ Vec3s jointTable[REDEAD_LIMB_MAX];
    /* 0x338 */ Vec3s morphTable[REDEAD_LIMB_MAX];
    /* 0x3D4 */ s16 headShakeTimer;
    /* 0x3D6 */ union {
                    s16 animationJudderTimer;
                    s16 stunTimer;
                    s16 deathTimer;
                    s16 coffinRiseForwardAccelTimer;
                };
    /* 0x3D8 */ s16 headRotY;
    /* 0x3DA */ s16 torsoRotY;
    /* 0x3DC */ s16 flags;
    /* 0x3DE */ s16 alpha;
    /* 0x3E0 */ s16 sunsSongStunTimer;
    /* 0x3E2 */ UNK_TYPE1 unk3E2[0x2];
    /* 0x3E4 */ union {
                    s16 danceEndTimer;
                    s16 pirouetteAngularVelocity;
                };
    /* 0x3E6 */ s16 drawDmgEffTimer;
    /* 0x3E8 */ u8 drawDmgEffType;
    /* 0x3E9 */ u8 stunnedBySunsSong;
    /* 0x3EA */ u8 grabDamageTimer;
    /* 0x3EB */ u8 grabState;
    /* 0x3EC */ u8 isMourning;
    /* 0x3ED */ u8 playerStunWaitTimer; // Cannot stun the player if this is non-zero
    /* 0x3EE */ u8 grabWaitTimer; // Cannot grab the player if this is non-zero
    /* 0x3EF */ u8 action;
    /* 0x3F0 */ u8 damageEffect;
    /* 0x3F1 */ s8 unk_3F1; // related to player->unk_ADD
} EnRd; // size = 0x3F4

#endif // Z_EN_RD_H
