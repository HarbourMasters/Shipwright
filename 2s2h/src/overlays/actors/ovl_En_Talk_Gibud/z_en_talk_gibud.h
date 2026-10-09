#ifndef Z_EN_TALK_GIBUD_H
#define Z_EN_TALK_GIBUD_H

#include "global.h"
#include "objects/object_rd/object_rd.h"

#define EN_TALK_GIBUD_REQUESTED_ITEM_INDEX(thisx) ((thisx)->params & 0xF)
#define EN_TALK_GIBUD_GET_SWITCH_FLAG(thisx) (((thisx)->params & 0xFF0) >> 4)

#define EN_TALK_GIBUD_SWITCH_FLAG_NONE 0xFF

struct EnTalkGibud;

typedef void (*EnTalkGibudActionFunc)(struct EnTalkGibud*, PlayState*);

typedef enum EnTalkGibudBodyPart {
    /*  0 */ EN_TALK_GIBUD_BODYPART_0,
    /*  1 */ EN_TALK_GIBUD_BODYPART_1,
    /*  2 */ EN_TALK_GIBUD_BODYPART_2,
    /*  3 */ EN_TALK_GIBUD_BODYPART_3,
    /*  4 */ EN_TALK_GIBUD_BODYPART_4,
    /*  5 */ EN_TALK_GIBUD_BODYPART_5,
    /*  6 */ EN_TALK_GIBUD_BODYPART_6,
    /*  7 */ EN_TALK_GIBUD_BODYPART_7,
    /*  8 */ EN_TALK_GIBUD_BODYPART_8,
    /*  9 */ EN_TALK_GIBUD_BODYPART_9,
    /* 10 */ EN_TALK_GIBUD_BODYPART_10,
    /* 11 */ EN_TALK_GIBUD_BODYPART_11,
    /* 12 */ EN_TALK_GIBUD_BODYPART_12,
    /* 13 */ EN_TALK_GIBUD_BODYPART_13,
    /* 14 */ EN_TALK_GIBUD_BODYPART_14,
    /* 15 */ EN_TALK_GIBUD_BODYPART_MAX
} EnTalkGibudBodyPart;

typedef struct EnTalkGibud {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ EnTalkGibudActionFunc actionFunc;
    /* 0x1D8 */ Vec3f bodyPartsPos[EN_TALK_GIBUD_BODYPART_MAX];
    /* 0x28C */ s32 bodyPartIndex;
    /* 0x290 */ s32 requestedItemIndex;
    /* 0x294 */ PlayerItemAction itemAction;
    /* 0x298 */ s32 switchFlag;
    /* 0x29C */ f32 drawDmgEffAlpha;
    /* 0x2A0 */ f32 drawDmgEffScale;
    /* 0x2A4 */ Vec3s jointTable[GIBDO_LIMB_MAX];
    /* 0x340 */ Vec3s morphTable[GIBDO_LIMB_MAX];
    /* 0x3DC */ s16 textId;
    /* 0x3DE */ Vec3s headRot;
    /* 0x3E4 */ Vec3s torsoRot;
    /* 0x3EA */ union {
                    s16 playerStunWaitTimer; // Cannot stun the player if this is non-zero
                    s16 grabDamageTimer;
                    s16 headShakeTimer;
                    s16 stunTimer;
                    s16 deathTimer;
                    s16 disappearanceTimer;
                };
    /* 0x3EC */ s16 grabState;
    /* 0x3EE */ s16 grabWaitTimer; // Cannot grab the player if this is non-zero
    /* 0x3F0 */ s16 drawDmgEffTimer;
    /* 0x3F2 */ s16 type;
    /* 0x3F4 */ s16 isTalking;
    /* 0x3F6 */ u8 drawDmgEffType;
    /* 0x3F7 */ s8 unk_3F7; // related to player->unk_ADD
} EnTalkGibud; // size = 0x3F8

#endif // Z_EN_TALK_GIBUD_H
