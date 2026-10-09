#ifndef Z_EN_HORSE_H
#define Z_EN_HORSE_H

#include "global.h"
#include "z64skin.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_ha/object_ha.h"

struct EnHorse;
struct EnIn;

typedef void (*EnHorseActionFunc)(struct EnHorse*, PlayState*);
typedef void (*EnHorsePostdrawFunc)(struct EnHorse*, PlayState*);
typedef void (*EnHorseCsFunc)(struct EnHorse*, PlayState*, CsCmdActorCue*);

#define ENHORSE_BOOST (1 << 0)                 /*         0x1 */
#define ENHORSE_BOOST_DECEL (1 << 1)           /*         0x2 */
#define ENHORSE_JUMPING (1 << 2)               /*         0x4 */
#define ENHORSE_CALC_RIDER_POS (1 << 3)        /*         0x8 */
#define ENHORSE_FORCE_REVERSING (1 << 4)       /*        0x10 */
#define ENHORSE_FORCE_WALKING (1 << 5)         /*        0x20 */
#define ENHORSE_FLAG_6 (1 << 6)                /*        0x40 */
#define ENHORSE_FLAG_7 (1 << 7)                /*        0x80 */
#define ENHORSE_FLAG_8 (1 << 8)                /*       0x100 */
#define ENHORSE_FLAG_9 (1 << 9)                /*       0x200 */
#define ENHORSE_STOPPING_NEIGH_SOUND (1 << 10) /*       0x400 */
#define ENHORSE_LAND2_SOUND (1 << 11)          /*       0x800 */
#define ENHORSE_SANDDUST_SOUND (1 << 12)       /*      0x1000 */
#define ENHORSE_INACTIVE (1 << 13)             /*      0x2000 */
#define ENHORSE_OBSTACLE (1 << 14)             /*      0x4000 */
#define ENHORSE_TURNING_TO_PLAYER (1 << 15)    /*      0x8000 */
#define ENHORSE_UNRIDEABLE (1 << 16)           /*     0x10000 */
#define ENHORSE_CANT_JUMP (1 << 17)            /*     0x20000 */
#define ENHORSE_FLAG_18 (1 << 18)              /*     0x40000 */
#define ENHORSE_FLAG_19 (1 << 19)              /*     0x80000 */
#define ENHORSE_FLAG_20 (1 << 20)              /*    0x100000 */
#define ENHORSE_FLAG_21 (1 << 21)              /*    0x200000 */
#define ENHORSE_FIRST_BOOST_REGEN (1 << 22)    /*    0x400000 */
#define ENHORSE_INGO_WON (1 << 23)             /*    0x800000 */
#define ENHORSE_FLAG_24 (1 << 24)              /*   0x1000000 */
#define ENHORSE_FLAG_25 (1 << 25)              /*   0x2000000 */
#define ENHORSE_FLAG_26 (1 << 26)              /*   0x4000000 */
#define ENHORSE_DRAW (1 << 27)                 /*   0x8000000 */
#define ENHORSE_FLAG_28 (1 << 28)              /*  0x10000000 */
#define ENHORSE_FLAG_29 (1 << 29)              /*  0x20000000 */
#define ENHORSE_FLAG_30 (1 << 30)              /*  0x40000000 */
#define ENHORSE_FLAG_31 (1 << 31)              /*  0x80000000 */

typedef enum EnHorseAction {
    /*  0 */ ENHORSE_ACTION_FROZEN,
    /*  1 */ ENHORSE_ACTION_INACTIVE,
    /*  2 */ ENHORSE_ACTION_IDLE,
    /*  3 */ ENHORSE_ACTION_FOLLOW_PLAYER,
    /*  4 */ ENHORSE_ACTION_INGO_RACE,
    /*  5 */ ENHORSE_ACTION_5,
    /*  6 */ ENHORSE_ACTION_6,
    /*  7 */ ENHORSE_ACTION_MOUNTED_IDLE,
    /*  8 */ ENHORSE_ACTION_MOUNTED_IDLE_WHINNYING,
    /*  9 */ ENHORSE_ACTION_MOUNTED_TURN,
    /* 10 */ ENHORSE_ACTION_MOUNTED_WALK,
    /* 11 */ ENHORSE_ACTION_MOUNTED_TROT,
    /* 12 */ ENHORSE_ACTION_MOUNTED_GALLOP,
    /* 13 */ ENHORSE_ACTION_MOUNTED_REARING,
    /* 14 */ ENHORSE_ACTION_STOPPING,
    /* 15 */ ENHORSE_ACTION_REVERSE,
    /* 16 */ ENHORSE_ACTION_LOW_JUMP,
    /* 17 */ ENHORSE_ACTION_HIGH_JUMP,
    /* 18 */ ENHORSE_ACTION_CS_UPDATE,
    /* 19 */ ENHORSE_ACTION_HBA,
    /* 20 */ ENHORSE_ACTION_FLEE_PLAYER,
    /* 21 */ ENHORSE_ACTION_21,
    /* 22 */ ENHORSE_ACTION_22,
    /* 23 */ ENHORSE_ACTION_23,
    /* 24 */ ENHORSE_ACTION_24,
    /* 25 */ ENHORSE_ACTION_25
} EnHorseAction;

typedef enum EnHorsePlayerDir {
    /* 0 */ PLAYER_DIR_FRONT_R,
    /* 1 */ PLAYER_DIR_FRONT_L,
    /* 2 */ PLAYER_DIR_BACK_R,
    /* 3 */ PLAYER_DIR_BACK_L,
    /* 4 */ PLAYER_DIR_SIDE_R,
    /* 5 */ PLAYER_DIR_SIDE_L
} EnHorsePlayerDir;

typedef enum EnHorseAnimation {
    /* 0 */ ENHORSE_ANIM_IDLE,
    /* 1 */ ENHORSE_ANIM_WHINNY,
    /* 2 */ ENHORSE_ANIM_STOPPING,
    /* 3 */ ENHORSE_ANIM_REARING,
    /* 4 */ ENHORSE_ANIM_WALK,
    /* 5 */ ENHORSE_ANIM_TROT,
    /* 6 */ ENHORSE_ANIM_GALLOP,
    /* 7 */ ENHORSE_ANIM_LOW_JUMP,
    /* 8 */ ENHORSE_ANIM_HIGH_JUMP,
    /* 9 */ ENHORSE_ANIM_MAX
} EnHorseAnimation;

typedef enum HorseType {
    /* 0 */ HORSE_TYPE_EPONA,
    /* 1 */ HORSE_TYPE_HNI,
    /* 2 */ HORSE_TYPE_2,
    /* 3 */ HORSE_TYPE_BANDIT,
    /* 4 */ HORSE_TYPE_DONKEY, // Cremia's donkey
    /* 5 */ HORSE_TYPE_MAX
} HorseType;

#define ENHORSE_PARAM_BANDIT 0x2000
#define ENHORSE_PARAM_4000 0x4000
#define ENHORSE_PARAM_DONKEY 0x8000

#define ENHORSE_IS_BANDIT_TYPE(thisx) ((thisx)->params & ENHORSE_PARAM_BANDIT)
#define ENHORSE_IS_4000_TYPE(thisx) ((thisx)->params & ENHORSE_PARAM_4000)
#define ENHORSE_IS_DONKEY_TYPE(thisx) ((thisx)->params & ENHORSE_PARAM_DONKEY)

typedef enum EnHorseParam {
    /*  0 */ ENHORSE_0,
    /*  1 */ ENHORSE_1,
    /*  2 */ ENHORSE_2,
    /*  3 */ ENHORSE_3,
    /*  4 */ ENHORSE_4,
    /*  5 */ ENHORSE_5,
    /*  6 */ ENHORSE_6,
    /*  7 */ ENHORSE_7,
    /*  8 */ ENHORSE_8,
    /*  9 */ ENHORSE_9,
    /* 10 */ ENHORSE_10,
    /* 11 */ ENHORSE_11,
    /* 12 */ ENHORSE_12,
    /* 13 */ ENHORSE_13,
    /* 14 */ ENHORSE_14,
    /* 15 */ ENHORSE_15,
    /* 16 */ ENHORSE_16,
    /* 17 */ ENHORSE_17,
    /* 18 */ ENHORSE_18,
    /* 19 */ ENHORSE_19,
    /* 20 */ ENHORSE_20
} EnHorseParam;

/**
 * `paramtype` should be `ENHORSE_PARAM_BANDIT`, `ENHORSE_PARAM_4000` or `ENHORSE_PARAM_DONKEY`
 * `lower` should be a value of the enum `EnHorseParam`
 */
#define ENHORSE_PARAMS(type, lower) ((type) | (lower))

typedef struct EnHorse {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnHorseAction action;
    /* 0x148 */ s32 noInputTimer;
    /* 0x14C */ s32 noInputTimerMax;
    /* 0x150 */ s32 type;
    /* 0x154 */ s8 objectSlot;
    /* 0x158 */ Skin skin;
    /* 0x1E8 */ s32 stateFlags;
    /* 0x1EC */ s32 unk_1EC;
    /* 0x1F0 */ Vec3f lastPos;
    /* 0x1FC */ s16 lastYaw;
    /* 0x200 */ s32 curRaceWaypoint;
    /* 0x204 */ s32 boostSpeed;
    /* 0x208 */ s32 playerControlled;
    /* 0x20C */ s32 animIndex;
    /* 0x210 */ f32 curFrame;
    /* 0x214 */ s32 soundTimer;
    /* 0x218 */ Vec3f unk_218;
    /* 0x224 */ UNK_TYPE1 unk224[0xC];
    /* 0x230 */ s32 unk_230;
    /* 0x234 */ u8 numBoosts;
    /* 0x238 */ s32 boostRegenTime;
    /* 0x23C */ s32 boostTimer;
    /* 0x240 */ EnHorsePostdrawFunc postDrawFunc;
    /* 0x244 */ f32 yFront;
    /* 0x248 */ f32 yBack;
    /* 0x24C */ s32 unk_24C;
    /* 0x250 */ s16 followTimer;
    /* 0x254 */ s32 prevAction;
    /* 0x258 */ Vec3f riderPos;
    /* 0x264 */ Vec2f curStick;
    /* 0x26C */ Vec2f lastStick;
    /* 0x274 */ f32 jumpStartY;
    /* 0x278 */ ColliderCylinder colliderCylinder1;
    /* 0x2C4 */ ColliderCylinder colliderCylinder2;
    /* 0x310 */ ColliderJntSph colliderJntSph;
    /* 0x330 */ ColliderJntSphElement colliderJntSphElements[1];
    /* 0x370 */ s32 playerDir;
    /* 0x374 */ UNK_TYPE1 unk374[0x2];
    /* 0x376 */ s16 angleToPlayer;
    /* 0x378 */ s16 followPlayerTurnSpeed;
    /* 0x37A */ u8 blinkTimer;
    /* 0x37C */ s16 waitTimer;
    /* 0x380 */ s32 cutsceneAction;
    /* 0x384 */ u16 cutsceneFlags;
    /* 0x388 */ s32 inRace;
    /* 0x38C */ struct EnIn* rider;
    /* 0x390 */ UNK_TYPE1 unk_390[0x4];
    /* 0x394 */ u16 unk_394;
    /* 0x398 */ f32 unk_398;
    /* 0x39C */ s32 unk_39C;
    /* 0x3A0 */ s32 hbaStarted;
    /* 0x3A4 */ s32 hbaFlags;
    /* 0x3A8 */ s32 hbaTimer;
    /* 0x3AC */ u16 dustFlags;
    /* 0x3B0 */ Vec3f frontRightHoof;
    /* 0x3BC */ Vec3f frontLeftHoof;
    /* 0x3C8 */ Vec3f backRightHoof;
    /* 0x3D4 */ Vec3f backLeftHoof;
    /* 0x3E0 */ s32 cueId;
    /* 0x3E4 */ UNK_TYPE1 unk_3E4[0x4];
    /* 0x3E8 */ f32 unk_3E8;
    /* 0x3EC */ s16 unk_3EC;
    /* 0x3EE */ Vec3s jointTable[HORSE_BANDIT_LIMB_MAX];
    /* 0x48A */ Vec3s morphTable[HORSE_BANDIT_LIMB_MAX];
    /* 0x528 */ f32 unk_528;
    /* 0x52C */ s32 unk_52C;
    /* 0x530 */ s32 cueChannel;
    /* 0x534 */ s32 unk_534;
    /* 0x538 */ s32 unk_538;
    /* 0x53C */ s32 unk_53C;
    /* 0x540 */ Vec3f unk_540;
    /* 0x54C */ UNK_TYPE unk_54C;
    /* 0x550 */ s32 unk_550;
    /* 0x554 */ UNK_TYPE unk_554;
    /* 0x558 */ UNK_TYPE unk_558;
    /* 0x55C */ s32 unk_55C; // maybe currentDistanceToCart... it isn't really a distance tho
    /* 0x560 */ s32 unk_560; // maybe initialDistanceToCart
    /* 0x564 */ s32 unk_564; // set but not used
    /* 0x568 */ f32 unk_568; // set but not used
    /* 0x56C */ f32 unk_56C;
    /* 0x570 */ Vec3f banditPosition; // Milk run minigame bandit
    /* 0x57C */ Vec3f unk_57C;
    /* 0x588 */ s16 unk_588;
    /* 0x58A */ UNK_TYPE1 unk_58A[0x2]; // struct padding?
    /* 0x58C */ s32 unk_58C;
    /* 0x590 */ s32 unk_590;
} EnHorse; // size = 0x594

#define EN_HORSE_CHECK_1(horseActor) (((horseActor)->stateFlags & ENHORSE_FLAG_6) ? true : false)

#define EN_HORSE_CHECK_2(horseActor) (((horseActor)->stateFlags & ENHORSE_FLAG_8) ? true : false)

#define EN_HORSE_CHECK_3(horseActor) (((horseActor)->stateFlags & ENHORSE_FLAG_9) ? true : false)

#define EN_HORSE_CHECK_4(horseActor)                                                                               \
    (((((horseActor)->action == ENHORSE_ACTION_MOUNTED_IDLE) || ((horseActor)->action == ENHORSE_ACTION_FROZEN) || \
       ((horseActor)->action == ENHORSE_ACTION_MOUNTED_IDLE_WHINNYING)) &&                                         \
      !((horseActor)->stateFlags & ENHORSE_FLAG_19) && !((horseActor)->stateFlags & ENHORSE_FLAG_25))              \
         ? true                                                                                                    \
         : false)

#define EN_HORSE_CHECK_JUMPING(horseActor) (((horseActor)->stateFlags & ENHORSE_JUMPING) ? true : false)

#endif // Z_EN_HORSE_H
