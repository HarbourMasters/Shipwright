#ifndef Z_BOSS_FD_H
#define Z_BOSS_FD_H

#include "ultra64.h"
#include "global.h"

struct BossFd;

typedef void (*BossFdActionFunc)(struct BossFd*, PlayState*);

typedef enum {
    /*  -1 */ BOSSFD_WAIT_INTRO = -1,
    /*   0 */ BOSSFD_FLY_MAIN,
    /*   1 */ BOSSFD_FLY_HOLE,
    /*   2 */ BOSSFD_BURROW,
    /*   3 */ BOSSFD_EMERGE,
    /*  50 */ BOSSFD_FLY_CEILING = 50,
    /*  51 */ BOSSFD_DROP_ROCKS,
    /* 100 */ BOSSFD_FLY_CHASE = 100,
    /* 101 */ BOSSFD_FLY_UNUSED,
    /* 200 */ BOSSFD_DEATH_START = 200,
    /* 201 */ BOSSFD_SKIN_BURN,
    /* 202 */ BOSSFD_BONES_FALL,
    /* 203 */ BOSSFD_SKULL_PAUSE,
    /* 204 */ BOSSFD_SKULL_FALL,
    /* 205 */ BOSSFD_SKULL_BURN
} BossFdActionState;

typedef enum {
    /* 0 */ BFD_CS_NONE,
    /* 1 */ BFD_CS_WAIT,
    /* 2 */ BFD_CS_START,
    /* 3 */ BFD_CS_LOOK_LINK,
    /* 4 */ BFD_CS_LOOK_GROUND,
    /* 5 */ BFD_CS_COLLAPSE,
    /* 6 */ BFD_CS_EMERGE
} BossFdCutsceneState;

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ u8 type;
    /* 0x25 */ u8 timer1;
    /* 0x26 */ Color_RGB8 color;
    /* 0x2A */ s16 alpha;
    /* 0x2C */ s16 timer2;
    /* 0x2E */ s16 kbAngle;
    /* 0x30 */ f32 scale;
    /* 0x34 */ f32 bFdFxFloat1;
    /* 0x38 */ f32 bFdFxFloat2;
    u32 epoch;
} BossFdEffect; // size = 0x3C

#define BOSSFD_EFFECT_COUNT 180

#define vFdFxRotX bFdFxFloat1
#define vFdFxScaleMod bFdFxFloat1
#define vFdFxRotY bFdFxFloat2
#define vFdFxYStop bFdFxFloat2

typedef enum {
    /* 0 */ BFD_FX_NONE,
    /* 1 */ BFD_FX_EMBER,
    /* 2 */ BFD_FX_DEBRIS,
    /* 3 */ BFD_FX_DUST,
    /* 4 */ BFD_FX_FIRE_BREATH,
    /* 5 */ BFD_FX_SKULL_PIECE
} BossFdEffectType;

typedef struct {
    /* 0x000 */ Vec3f pos[30];
    /* 0x168 */ f32 scale[30];
    /* 0x1E0 */ Vec3f head;
} BossFdMane; // size = 0x1EC

typedef struct {
    /* 0x00 */ Vec3f eye;
    /* 0x0C */ Vec3f at;
    /* 0x18 */ Vec3f pad[2];
    /* 0x30 */ Vec3f eyeVel;
    /* 0x3C */ Vec3f atVel;
    /* 0x48 */ Vec3f nextEye;
    /* 0x54 */ Vec3f eyeMaxVel;
    /* 0x60 */ Vec3f nextAt;
    /* 0x6C */ Vec3f atMaxVel;
    /* 0x78 */ f32 speedMod;
    /* 0x7C */ f32 accel;
    /* 0x80 */ f32 yMod;
    /* 0x84 */ f32 shake;
} BossFdCam; // size = 0x88

typedef enum {
    /*  0 */ BFD_ACTION_STATE,
    /*  1 */ BFD_MOVE_TIMER,
    /*  2 */ BFD_VAR_TIMER,
    /*  3 */ BFD_LEAD_BODY_SEG,
    /*  4 */ BFD_LEAD_MANE_SEG,
    /*  5 */ BFD_BLINK_TIMER,
    /*  6 */ BFD_ROAR_TIMER,
    /*  7 */ BFD_DAMAGE_FLASH_TIMER,
    /*  8 */ BFD_START_ATTACK,
    /*  9 */ BFD_UNK_234,
    /* 10 */ BFD_UNK_236,
    /* 11 */ BFD_MANE_EMBERS_TIMER,
    /* 12 */ BFD_ROCK_TIMER,
    /* 13 */ BFD_CEILING_TARGET,
    /* 14 */ BFD_INVINC_TIMER,
    /* 15 */ BFD_SPLASH_TIMER,
    /* 16 */ BFD_CAM_SHAKE_TIMER,
    /* 17 */ BFD_STOP_FLAG,
    /* 18 */ BFD_FLY_COUNT,
    /* 19 */ BFD_SHORT_COUNT
} BossFdS16Var;

typedef enum {
    /*  0 */ BFD_TEX1_SCROLL_X,
    /*  1 */ BFD_TEX1_SCROLL_Y,
    /*  2 */ BFD_TEX2_SCROLL_X,
    /*  3 */ BFD_TEX2_SCROLL_Y,
    /*  4 */ BFD_UNUSED_F4,
    /*  5 */ BFD_UNUSED_F5,
    /*  6 */ BFD_UNUSED_F6,
    /*  7 */ BFD_BODY_TEX2_ALPHA,
    /*  8 */ BFD_HEAD_TEX2_ALPHA,
    /*  9 */ BFD_TARGET_Y_OFFSET,
    /* 10 */ BFD_CEILING_BOUNCE,
    /* 11 */ BFD_BODY_PULSE,
    /* 12 */ BFD_MANE_COLOR_CENTER,
    /* 13 */ BFD_MANE_COLOR_RIGHT,
    /* 14 */ BFD_MANE_COLOR_LEFT,
    /* 15 */ BFD_MANE_EMBER_SPEED,
    /* 16 */ BFD_MANE_EMBER_RATE,
    /* 17 */ BFD_UNUSED_F17,
    /* 18 */ BFD_UNUSED_F18,
    /* 19 */ BFD_UNUSED_F19,
    /* 20 */ BFD_FLY_SPEED,
    /* 21 */ BFD_TURN_RATE,
    /* 22 */ BFD_TURN_RATE_MAX,
    /* 23 */ BFD_FLY_WOBBLE_AMP,
    /* 24 */ BFD_FLY_WOBBLE_RATE,
    /* 25 */ BFD_UNUSED_F25,
    /* 26 */ BFD_FLOAT_COUNT
} BossFdF32Var;

typedef struct BossFd {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnimeHead;
    /* 0x0190 */ SkelAnime skelAnimeRightArm;
    /* 0x01D4 */ SkelAnime skelAnimeLeftArm;
    /* 0x0218 */ BossFdActionFunc actionFunc;
    /* 0x021C */ s16 fireBreathTimer;
    /* 0x021E */ s16 skinSegments;
    /* 0x0220 */ u8 fogMode;
    /* 0x0222 */ s16 work[BFD_SHORT_COUNT];
    /* 0x0248 */ s16 timers[6];
    /* 0x0254 */ f32 fwork[BFD_FLOAT_COUNT];
    /* 0x02BC */ Vec3f targetPosition;
    /* 0x02C8 */ Vec3f holePosition;
    /* 0x02D4 */ u8 holeIndex;
    /* 0x02D5 */ u8 eyeState;
    /* 0x02D6 */ u8 platformSignal;
    /* 0x02D7 */ u8 faceExposed;
    /* 0x02D8 */ u8 handoffSignal;
    /* 0x02DC */ Vec3f bodySegsRot[100];
    /* 0x078C */ Vec3f bodySegsPos[100];
    /* 0x0C3C */ Vec3f rightArmRot[4];
    /* 0x0C6C */ Vec3f leftArmRot[4];
    /* 0x0C9C */ Vec3f fireManeRot[30];
    /* 0x0E04 */ BossFdMane centerMane;
    /* 0x0FF0 */ BossFdMane rightMane;
    /* 0x11DC */ BossFdMane leftMane;
    /* 0x13C8 */ f32 flattenMane;
    /* 0x13CC */ f32 jawOpening;
    /* 0x13D0 */ s16 bodyFallApart[18];
    /* 0x13F4 */ Vec3f headPos;
    /* 0x1400 */ s16 introFlyState;
    /* 0x1402 */ s16 introState;
    /* 0x1404 */ s16 introCamera;
    /* 0x1408 */ BossFdCam camData;
    /* 0x1490 */ ColliderJntSph collider;
    /* 0x14B0 */ ColliderJntSphElement elements[19];
    /* 0x1970 */ BossFdEffect effects[180];
} BossFd; // size = 0x43A0

#endif
