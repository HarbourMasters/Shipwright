#ifndef Z_EN_HORSE_H
#define Z_EN_HORSE_H

#include "ultra64.h"
#include "global.h"

typedef enum {
    /* 0  */ ENHORSE_ACT_FROZEN,
    /* 1  */ ENHORSE_ACT_INACTIVE,
    /* 2  */ ENHORSE_ACT_IDLE,
    /* 3  */ ENHORSE_ACT_FOLLOW_PLAYER,
    /* 4  */ ENHORSE_ACT_INGO_RACE,
    /* 5  */ ENHORSE_ACT_MOUNTED_IDLE,
    /* 6  */ ENHORSE_ACT_MOUNTED_IDLE_WHINNEYING,
    /* 7  */ ENHORSE_ACT_MOUNTED_TURN,
    /* 8  */ ENHORSE_ACT_MOUNTED_WALK,
    /* 9  */ ENHORSE_ACT_MOUNTED_TROT,
    /* 10 */ ENHORSE_ACT_MOUNTED_GALLOP,
    /* 11 */ ENHORSE_ACT_MOUNTED_REARING,
    /* 12 */ ENHORSE_ACT_STOPPING,
    /* 13 */ ENHORSE_ACT_REVERSE,
    /* 14 */ ENHORSE_ACT_LOW_JUMP,
    /* 15 */ ENHORSE_ACT_HIGH_JUMP,
    /* 16 */ ENHORSE_ACT_BRIDGE_JUMP,
    /* 17 */ ENHORSE_ACT_CS_UPDATE,
    /* 18 */ ENHORSE_ACT_HBA,
    /* 19 */ ENHORSE_ACT_FLEE_PLAYER
} EnHorseAction;


#define ENHORSE_BOOST                      (1 << 0)   /*         0x1 */
#define ENHORSE_BOOST_DECEL                (1 << 1)   /*         0x2 */
#define ENHORSE_JUMPING                    (1 << 2)   /*         0x4 */
#define ENHORSE_CALC_RIDER_POS             (1 << 3)   /*         0x8 */
#define ENHORSE_FORCE_REVERSING            (1 << 4)   /*        0x10 */
#define ENHORSE_FORCE_WALKING              (1 << 5)   /*        0x20 */
#define ENHORSE_FLAG_6                     (1 << 6)   /*        0x40 */
#define ENHORSE_FLAG_7                     (1 << 7)   /*        0x80 */
#define ENHORSE_FLAG_8                     (1 << 8)   /*       0x100 */
#define ENHORSE_FLAG_9                     (1 << 9)   /*       0x200 */
#define ENHORSE_STOPPING_NEIGH_SOUND       (1 << 10)  /*       0x400 */
#define ENHORSE_LAND2_SOUND                (1 << 11)  /*       0x800 */
#define ENHORSE_SANDDUST_SOUND             (1 << 12)  /*      0x1000 */
#define ENHORSE_INACTIVE                   (1 << 13)  /*      0x2000 */
#define ENHORSE_OBSTACLE                   (1 << 14)  /*      0x4000 */
#define ENHORSE_TURNING_TO_PLAYER          (1 << 15)  /*      0x8000 */
#define ENHORSE_UNRIDEABLE                 (1 << 16)  /*    0x1 0000 */
#define ENHORSE_CANT_JUMP                  (1 << 17)  /*    0x2 0000 */
#define ENHORSE_FLAG_18                    (1 << 18)  /*    0x4 0000 */
#define ENHORSE_FLAG_19                    (1 << 19)  /*    0x8 0000 */
#define ENHORSE_FLAG_20                    (1 << 20)  /*   0x10 0000 */
#define ENHORSE_FLAG_21                    (1 << 21)  /*   0x20 0000 */
#define ENHORSE_FIRST_BOOST_REGEN          (1 << 22)  /*   0x40 0000 */
#define ENHORSE_INGO_WON                   (1 << 23)  /*   0x80 0000 */
#define ENHORSE_FLAG_24                    (1 << 24)  /*  0x100 0000 */
#define ENHORSE_FLAG_25                    (1 << 25)  /*  0x200 0000 */
#define ENHORSE_FLAG_26                    (1 << 26)  /*  0x400 0000 */
#define ENHORSE_DRAW                       (1 << 27)  /*  0x800 0000 */
#define ENHORSE_FLAG_28                    (1 << 28)  /* 0x1000 0000 */
#define ENHORSE_FLAG_29                    (1 << 29)  /* 0x2000 0000 */
#define ENHORSE_FLAG_30                    (1 << 30)  /* 0x4000 0000 */
#define ENHORSE_FLAG_31                    (1 << 31)  /* 0x8000 0000 */

struct EnHorse;

typedef enum {
    /* 0 */ PLAYER_DIR_FRONT_R,
    /* 1 */ PLAYER_DIR_FRONT_L,
    /* 2 */ PLAYER_DIR_BACK_R,
    /* 3 */ PLAYER_DIR_BACK_L,
    /* 4 */ PLAYER_DIR_SIDE_R,
    /* 5 */ PLAYER_DIR_SIDE_L
} EnHorsePlayerDir;

typedef enum {
    /* 0 */ ENHORSE_ANIM_IDLE,
    /* 1 */ ENHORSE_ANIM_WHINNEY,
    /* 2 */ ENHORSE_ANIM_STOPPING,
    /* 3 */ ENHORSE_ANIM_REARING,
    /* 4 */ ENHORSE_ANIM_WALK,
    /* 5 */ ENHORSE_ANIM_TROT,
    /* 6 */ ENHORSE_ANIM_GALLOP,
    /* 7 */ ENHORSE_ANIM_LOW_JUMP,
    /* 8 */ ENHORSE_ANIM_HIGH_JUMP
} EnHorseAnimationIndex;

typedef enum {
    /* 0 */ HORSE_EPONA,
    /* 1 */ HORSE_HNI
} HorseType;

typedef void (*EnHorsePostdrawFunc)(struct EnHorse*, PlayState*);

typedef struct EnHorse {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnHorseAction action;
    /* 0x0150 */ s32 noInputTimer;
    /* 0x0154 */ s32 noInputTimerMax;
    /* 0x0158 */ s32 type;
    /* 0x015C */ s8 bankIndex;
    /* 0x0160 */ Skin skin;
    /* 0x01F0 */ u32 stateFlags;
    /* 0x01F4 */ Vec3f lastPos;
    /* 0x0200 */ s16 lastYaw;
    /* 0x0204 */ s32 curRaceWaypoint;
    /* 0x0208 */ s32 boostSpeed;
    /* 0x020C */ s32 playerControlled;
    /* 0x0210 */ s32 animationIdx;
    /* 0x0214 */ f32 curFrame;
    /* 0x0218 */ s32 soundTimer;
    /* 0x021C */ Vec3f unk_21C;
    /* 0x0228 */ Vec3f unk_228;
    /* 0x0234 */ s32 unk_234;
    /* 0x0238 */ u8 numBoosts;
    /* 0x023C */ s32 boostRegenTime;
    /* 0x0240 */ s32 boostTimer;
    /* 0x0244 */ EnHorsePostdrawFunc postDrawFunc;
    /* 0x0248 */ f32 yFront; // The y coordinate of the floor under the front feet
    /* 0x024C */ f32 yBack;  // The y coordinate of the floor under the back feet
    /* 0x0250 */ s16 followTimer;
    /* 0x0252 */ s16 unk_252;
    /* 0x0254 */ EnHorseAction prevAction;
    /* 0x0258 */ Vec3f riderPos;
    /* 0x0264 */ Vec2f curStick;
    /* 0x026C */ Vec2f lastStick;
    /* 0x0274 */ f32 jumpStartY;
    /* 0x0278 */ ColliderCylinder cyl1;
    /* 0x02C4 */ ColliderCylinder cyl2;
    /* 0x0310 */ ColliderJntSph jntSph;
    /* 0x0330 */ ColliderJntSphElement jntSphList;
    /* 0x0370 */ u32 playerDir;
    /* 0x0374 */ s16 unk_374;
    /* 0x0376 */ s16 angleToPlayer;
    /* 0x0378 */ s16 followPlayerTurnSpeed;
    /* 0x037A */ u8 blinkTimer;
    /* 0x037C */ s16 waitTimer;
    /* 0x037E */ s16 unk_37E;
    /* 0x0380 */ s32 cutsceneAction;
    /* 0x0384 */ u16 cutsceneFlags;
    // struct {
    /* 0x0388 */ s32 inRace;
    /* 0x038C */ Actor* rider;
    /* 0x0390 */ u32 unk_390;
    /* 0x0394 */ u16 ingoRaceFlags;
    /* 0x0398 */ f32 ingoHorseMaxSpeed;
    // } race; //?
    /* 0x039C */ s32 unk_39C; // probably hbaAction
    /* 0x03A0 */ s32 hbaStarted;
    /* 0x03A4 */ s32 hbaFlags;
    /* 0x03A8 */ s32 hbaTimer;
    /* 0x03AC */ u8 bridgeJumpIdx;
    /* 0x03B0 */ Vec3f bridgeJumpStart;
    /* 0x03BC */ s32 bridgeJumpTimer;
    /* 0x03C0 */ f32 bridgeJumpYVel;
    /* 0x03C4 */ s16 bridgeJumpRelAngle;
    /* 0x03C6 */ s16 unk_3C6; // pad
    // sub struct?
    /* 0x03C8 */ u16 dustFlags;
    /* 0x03CC */ Vec3f frontRightHoof;
    /* 0x03D8 */ Vec3f frontLeftHoof;
    /* 0x03E4 */ Vec3f backRightHoof;
    /* 0x03F0 */ Vec3f backLeftHoof;
} EnHorse; // size = 0x03FC

#define EN_HORSE_CHECK_1(horseActor) \
    (((horseActor)->stateFlags & ENHORSE_FLAG_6)  \
        ? true                       \
        : false)

#define EN_HORSE_CHECK_2(horseActor) \
    (((horseActor)->stateFlags & ENHORSE_FLAG_8) \
        ? true                       \
        : false)

#define EN_HORSE_CHECK_3(horseActor) \
    (((horseActor)->stateFlags & ENHORSE_FLAG_9) \
        ? true                       \
        : false)

#define EN_HORSE_CHECK_4(horseActor)                                                                 \
    (((((horseActor)->action == ENHORSE_ACT_MOUNTED_IDLE) || ((horseActor)->action == ENHORSE_ACT_FROZEN) || ((horseActor)->action == ENHORSE_ACT_MOUNTED_IDLE_WHINNEYING)) && \
      !((horseActor)->stateFlags & ENHORSE_FLAG_19) && !((horseActor)->stateFlags & ENHORSE_FLAG_25))          \
         ? true                                                                                      \
         : false)

#define EN_HORSE_CHECK_JUMPING(horseActor) \
    (((horseActor)->stateFlags & ENHORSE_JUMPING) \
        ? true                       \
        : false)

#endif
