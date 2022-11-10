/*
 * File: z_fishing.c
 * Overlay: ovl_Fishing
 * Description: Fishing Pond Elements (Owner, Fish, Props, Effects...)
 */

#include "z_fishing.h"

#include "overlays/actors/ovl_En_Kanban/z_en_kanban.h"
#include "objects/object_fish/object_fish.h"
#include "vt.h"

#include "soh/frame_interpolation.h"

#define FLAGS ACTOR_FLAG_4

#define WATER_SURFACE_Y(play) play->colCtx.colHeader->waterBoxes->ySurface

void Fishing_Init(Actor* thisx, PlayState* play);
void Fishing_Destroy(Actor* thisx, PlayState* play);
void Fishing_UpdateFish(Actor* thisx, PlayState* play);
void Fishing_UpdateOwner(Actor* thisx, PlayState* play);
void Fishing_DrawFish(Actor* thisx, PlayState* play);
void Fishing_DrawOwner(Actor* thisx, PlayState* play);

typedef struct {
    /* 0x00 */ u8 unk_00;
    /* 0x02 */ Vec3s pos;
    /* 0x08 */ u8 unk_08;
    /* 0x0C */ f32 unk_0C;
} FishingFishInit; // size = 0x10

#define EFFECT_COUNT 130

typedef enum {
    /* 0x00 */ FS_EFF_NONE,
    /* 0x01 */ FS_EFF_RIPPLE,
    /* 0x02 */ FS_EFF_DUST_SPLASH,
    /* 0x03 */ FS_EFF_WATER_DUST,
    /* 0x04 */ FS_EFF_BUBBLE,
    /* 0x05 */ FS_EFF_RAIN_DROP,
    /* 0x06 */ FS_EFF_OWNER_HAT,
    /* 0x07 */ FS_EFF_RAIN_RIPPLE,
    /* 0x08 */ FS_EFF_RAIN_SPLASH
} FishingEffectType;

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f vel;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ u8 type;
    /* 0x25 */ u8 timer;
    /* 0x26 */ char unk_26[0x04];
    /* 0x2A */ s16 alpha;
    /* 0x2C */ s16 unk_2C;
    /* 0x2E */ s16 unk_2E;
    /* 0x30 */ f32 unk_30;
    /* 0x34 */ f32 unk_34;
    /* 0x38 */ f32 unk_38;
    /* 0x3C */ f32 unk_3C;
    u32 epoch;
} FishingEffect; // size = 0x40

#define POND_PROP_COUNT 140

typedef enum {
    /* 0x00 */ FS_PROP_NONE,
    /* 0x01 */ FS_PROP_REED,
    /* 0x02 */ FS_PROP_LILY_PAD,
    /* 0x03 */ FS_PROP_ROCK,
    /* 0x04 */ FS_PROP_WOOD_POST,
    /* 0x23 */ FS_PROP_INIT_STOP = 0x23
} FishingPropType;

typedef struct {
    /* 0x00 */ u8 type;
    /* 0x02 */ Vec3s pos;
} FishingPropInit; // size = 0x08

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ f32 rotX;
    /* 0x10 */ f32 rotY;
    /* 0x14 */ f32 reedAngle;
    /* 0x18 */ Vec3f projectedPos;
    /* 0x24 */ f32 scale;
    /* 0x28 */ s16 lilyPadAngle;
    /* 0x2C */ f32 lilyPadOffset;
    /* 0x30 */ u8 type;
    /* 0x32 */ s16 timer;
    /* 0x34 */ u8 shouldDraw;
    /* 0x38 */ f32 drawDistance;
    u32 epoch;
} FishingProp; // size = 0x3C

typedef enum {
    /* 0x00 */ FS_GROUP_FISH_NONE,
    /* 0x01 */ FS_GROUP_FISH_NORMAL
} FishingGroupFishType;

#define GROUP_FISH_COUNT 60

typedef struct {
    /* 0x00 */ u8 type;
    /* 0x02 */ s16 timer;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f unk_10;
    /* 0x1C */ Vec3f projectedPos;
    /* 0x28 */ f32 unk_28;
    /* 0x2C */ f32 unk_2C;
    /* 0x30 */ f32 unk_30;
    /* 0x34 */ f32 unk_34;
    /* 0x38 */ f32 unk_38;
    /* 0x3C */ s16 unk_3C;
    /* 0x3E */ s16 unk_3E;
    /* 0x40 */ s16 unk_40;
    /* 0x42 */ s16 unk_42;
    /* 0x44 */ u8 shouldDraw;
    u32 epoch;
} FishingGroupFish; // size = 0x48

#define LINE_SEG_COUNT 200
#define SINKING_LURE_SEG_COUNT 20

const ActorInit Fishing_InitVars = {
    ACTOR_FISHING,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_FISH,
    sizeof(Fishing),
    (ActorFunc)Fishing_Init,
    (ActorFunc)Fishing_Destroy,
    (ActorFunc)Fishing_UpdateFish,
    (ActorFunc)Fishing_DrawFish,
    NULL,
};

static f32 D_80B7A650 = 0.0f;

static u8 D_80B7A654 = 0;

static f32 D_80B7A658 = 0.0f;

static Vec3f D_80B7A65C = { 0.0f, 0.0f, 0.0f };

static f32 D_80B7A668 = 0.0f;

static u8 sSinkingLureLocation = 0;

/// Weight of caught fish. 
static f32 D_80B7A670 = 0.0f;

static u8 D_80B7A674 = true;

static u16 D_80B7A678 = 0;

static u8 D_80B7A67C = 0;

static s32 D_80B7A680 = 0;

static s16 D_80B7A684 = 0;

static u8 D_80B7A688 = 0;
static u8 D_80B7A68C = 0;
static u8 D_80B7A690 = 0;

static s16 D_80B7A694 = 0;

static Vec3f sFishMouthOffset = { 500.0f, 500.0f, 0.0f };

static u8 D_80B7A6A4 = 0;

static f32 D_80B7A6A8 = 0.0f;
static f32 D_80B7A6AC = 0.0f;
static f32 D_80B7A6B0 = 0.0f;
static f32 D_80B7A6B4 = 0.0f;
static f32 D_80B7A6B8 = 0.0f;
static f32 D_80B7A6BC = 0.0f;
static f32 D_80B7A6C0 = 0.0f;

static s16 D_80B7A6C4 = 0;
static s16 D_80B7A6C8 = 0;

static u8 D_80B7A6CC = 0;
static u8 D_80B7A6D0 = 0;
static u8 D_80B7A6D4 = 0;

static ColliderJntSphElementInit sJntSphElementsInit[12] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_TYPE_ENEMY,
        AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    12,
    sJntSphElementsInit,
};

static f32 D_80B7A898 = 0.0f;

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };
static Vec3f D_80B7A8A8 = { 0.0f, 0.0f, 2000.0f };

static Fishing* sFishingMain;
static u8 D_80B7E074;
static u8 sLinkAge;
static u8 D_80B7E076;
static u8 D_80B7E077;
static f32 D_80B7E078;
static u8 D_80B7E07C;
static u8 D_80B7E07D;
static u8 D_80B7E07E;
static s16 D_80B7E080;
static u8 D_80B7E082;
static u16 D_80B7E084;
static u16 D_80B7E086;
static s8 D_80B7E088;
static Vec3f sOwnerHeadPos;
static Vec3s sEffOwnerHatRot;
static u8 D_80B7E0A2;
static s16 D_80B7E0A4;
static s16 D_80B7E0A6;
static Fishing* sFishingHookedFish;
static s16 D_80B7E0AC;
static s16 D_80B7E0AE;
static s16 D_80B7E0B0;
static s16 D_80B7E0B2;
static s16 D_80B7E0B4;
static u8 D_80B7E0B6;
static Vec3f sLurePos;
static Vec3f D_80B7E0C8;
static Vec3f sLureRot;
static Vec3f D_80B7E0E8;
static Vec3f D_80B7E0F8;
static f32 D_80B7E104;
static f32 D_80B7E108;
static f32 D_80B7E10C;
static f32 D_80B7E110;
static s8 D_80B7E114;
static s16 D_80B7E116;
static u8 D_80B7E118;
static f32 D_80B7E11C;
static u8 D_80B7E120;
static s16 D_80B7E122;
static u8 D_80B7E124;
static Vec3f D_80B7E128;
static f32 D_80B7E134;
static f32 D_80B7E138;
static s16 D_80B7E13C;
static f32 D_80B7E140;
static f32 D_80B7E144;
static f32 D_80B7E148;
static f32 D_80B7E14C;
static s16 D_80B7E150;
static f32 D_80B7E154;
static Vec3f sRodTipPos;
static Vec3f sReelLinePos[LINE_SEG_COUNT];
static Vec3f sReelLineRot[LINE_SEG_COUNT];
static Vec3f sReelLineUnk[LINE_SEG_COUNT];
static Vec3f sLureHookRefPos[2];
static f32 sLureHookRotY[2];
static u8 D_80B7FDA8;
static Vec3f sSinkingLurePos[SINKING_LURE_SEG_COUNT];
static s16 D_80B7FEA0;
static f32 sProjectedW;
static Vec3f sCameraEye;
static Vec3f sCameraAt;
static s16 sCameraId;
static f32 D_80B7FEC8;
static f32 D_80B7FECC;
static f32 D_80B7FED0;
static Vec3f sSinkingLureBasePos;
static f32 D_80B7FEE4;
static s32 sRandSeed0;
static s32 sRandSeed1;
static s32 sRandSeed2;
static FishingProp sPondProps[POND_PROP_COUNT];
static FishingGroupFish sGroupFishes[GROUP_FISH_COUNT];
static f32 sFishGroupAngle1;
static f32 sFishGroupAngle2;
static f32 sFishGroupAngle3;
static FishingEffect sFishingEffects[EFFECT_COUNT];
static Vec3f sStreamSoundProjectedPos;

void Fishing_SetColliderElement(s32 index, ColliderJntSph* collider, Vec3f* pos, f32 scale) {
    collider->elements[index].dim.worldSphere.center.x = pos->x;
    collider->elements[index].dim.worldSphere.center.y = pos->y;
    collider->elements[index].dim.worldSphere.center.z = pos->z;
    collider->elements[index].dim.worldSphere.radius =
        collider->elements[index].dim.modelSphere.radius * collider->elements[index].dim.scale * scale * 1.6f;
}

void Fishing_SeedRand(s32 seed0, s32 seed1, s32 seed2) {
    sRandSeed0 = seed0;
    sRandSeed1 = seed1;
    sRandSeed2 = seed2;
}

f32 Fishing_RandZeroOne(void) {
    f32 rand;

    // Wichmann-Hill algorithm
    sRandSeed0 = (sRandSeed0 * 171) % 30269;
    sRandSeed1 = (sRandSeed1 * 172) % 30307;
    sRandSeed2 = (sRandSeed2 * 170) % 30323;

    rand = (sRandSeed0 / 30269.0f) + (sRandSeed1 / 30307.0f) + (sRandSeed2 / 30323.0f);
    while (rand >= 1.0f) {
        rand -= 1.0f;
    }

    return fabsf(rand);
}

s16 Fishing_SmoothStepToS(s16* pValue, s16 target, s16 scale, s16 step) {
    s16 stepSize;
    s16 diff;

    diff = target - *pValue;
    stepSize = diff / scale;

    if (stepSize > step) {
        stepSize = step;
    }

    if (stepSize < -step) {
        stepSize = -step;
    }

    *pValue += stepSize;

    return stepSize;
}

void Fishing_SpawnRipple(Vec3f* projectedPos, FishingEffect* effect, Vec3f* pos, f32 arg3, f32 arg4, s16 arg5,
                         s16 countLimit) {
    s16 i;

    if ((projectedPos != NULL) && ((projectedPos->z > 500.0f) || (projectedPos->z < 0.0f))) {
        return;
    }

    for (i = 0; i < countLimit; i++) {
        if (effect->type == FS_EFF_NONE) {
            effect->type = FS_EFF_RIPPLE;
            effect->pos = *pos;
            effect->vel = sZeroVec;
            effect->accel = sZeroVec;
            effect->unk_30 = arg3 * 0.0025f;
            effect->unk_34 = arg4 * 0.0025f;

            if (arg3 > 300.0f) {
                effect->alpha = 0;
                effect->unk_2E = arg5;
                effect->unk_2C = 0;
                effect->unk_38 = (effect->unk_34 - effect->unk_30) * 0.05f;
            } else {
                effect->alpha = arg5;
                effect->unk_2C = 1;
                effect->unk_38 = (effect->unk_34 - effect->unk_30) * 0.1f;
            }

            effect->epoch++;
            break;
        }

        effect++;
    }
}

void Fishing_SpawnDustSplash(Vec3f* projectedPos, FishingEffect* effect, Vec3f* pos, Vec3f* vel, f32 scale) {
    s16 i;
    Vec3f accel = { 0.0f, -1.0f, 0.0f };

    if ((projectedPos != NULL) && ((projectedPos->z > 500.0f) || (projectedPos->z < 0.0f))) {
        return;
    }

    for (i = 0; i < 100; i++) {
        if ((effect->type == FS_EFF_NONE) || (effect->type == FS_EFF_RAIN_DROP) ||
            (effect->type == FS_EFF_RAIN_RIPPLE) || (effect->type == FS_EFF_RAIN_SPLASH)) {
            effect->type = FS_EFF_DUST_SPLASH;
            effect->pos = *pos;
            effect->vel = *vel;
            effect->accel = accel;
            effect->alpha = 100 + (s16)Rand_ZeroFloat(100.0f);
            effect->unk_30 = scale;
            effect->epoch++;
            break;
        }

        effect++;
    }
}

void Fishing_SpawnWaterDust(Vec3f* projectedPos, FishingEffect* effect, Vec3f* pos, f32 scale) {
    s16 i;
    Vec3f accel = { 0.0f, 0.05f, 0.0f };

    if ((projectedPos != NULL) && ((projectedPos->z > 500.0f) || (projectedPos->z < 0.0f))) {
        return;
    }

    for (i = 0; i < 90; i++) {
        if (effect->type == FS_EFF_NONE) {
            effect->type = FS_EFF_WATER_DUST;
            effect->pos = *pos;
            effect->vel = sZeroVec;
            effect->accel = accel;
            effect->alpha = 255;
            effect->timer = (s16)Rand_ZeroFloat(100.0f);
            effect->unk_30 = scale;
            effect->unk_34 = 2.0f * scale;
            effect->epoch++;
            break;
        }

        effect++;
    }
}

void Fishing_SpawnBubble(Vec3f* projectedPos, FishingEffect* effect, Vec3f* pos, f32 scale, u8 arg4) {
    s16 i;
    Vec3f vel = { 0.0f, 1.0f, 0.0f };

    if ((projectedPos != NULL) && ((projectedPos->z > 500.0f) || (projectedPos->z < 0.0f))) {
        return;
    }

    for (i = 0; i < 90; i++) {
        if (effect->type == FS_EFF_NONE) {
            effect->type = FS_EFF_BUBBLE;
            effect->pos = *pos;
            effect->vel = vel;
            effect->accel = sZeroVec;
            effect->timer = (s16)Rand_ZeroFloat(100.0f);
            effect->unk_30 = scale;
            effect->unk_2C = arg4;
            effect->epoch++;
            break;
        }

        effect++;
    }
}

void Fishing_SpawnRainDrop(FishingEffect* effect, Vec3f* pos, Vec3f* rot) {
    s16 i;
    Vec3f velSrc;

    velSrc.x = 0.0f;
    velSrc.y = 0.0f;
    velSrc.z = 300.0f;

    effect += 30;

    for (i = 30; i < EFFECT_COUNT; i++) {
        if (effect->type == FS_EFF_NONE) {
            effect->type = FS_EFF_RAIN_DROP;
            effect->pos = *pos;
            effect->accel = sZeroVec;
            effect->unk_34 = rot->x;
            effect->unk_38 = rot->y;
            effect->unk_3C = rot->z;
            Matrix_RotateY(rot->y, MTXMODE_NEW);
            Matrix_RotateX(rot->x, MTXMODE_APPLY);
            Matrix_MultVec3f(&velSrc, &effect->vel);
            effect->epoch++;
            break;
        }

        effect++;
    }
}

static FishingPropInit sPondPropInits[POND_PROP_COUNT + 1] = {
    { FS_PROP_ROCK, { 529, -53, -498 } },
    { FS_PROP_ROCK, { 461, -66, -480 } },
    { FS_PROP_ROCK, { 398, -73, -474 } },
    { FS_PROP_ROCK, { -226, -52, -691 } },
    { FS_PROP_ROCK, { -300, -41, -710 } },
    { FS_PROP_ROCK, { -333, -50, -643 } },
    { FS_PROP_ROCK, { -387, -46, -632 } },
    { FS_PROP_ROCK, { -484, -43, -596 } },
    { FS_PROP_ROCK, { -409, -57, -560 } },
    { FS_PROP_WOOD_POST, { 444, -87, -322 } },
    { FS_PROP_WOOD_POST, { 447, -91, -274 } },
    { FS_PROP_WOOD_POST, { 395, -109, -189 } },
    { FS_PROP_REED, { 617, -29, 646 } },
    { FS_PROP_REED, { 698, -26, 584 } },
    { FS_PROP_REED, { 711, -29, 501 } },
    { FS_PROP_REED, { 757, -28, 457 } },
    { FS_PROP_REED, { 812, -29, 341 } },
    { FS_PROP_REED, { 856, -30, 235 } },
    { FS_PROP_REED, { 847, -31, 83 } },
    { FS_PROP_REED, { 900, -26, 119 } },
    { FS_PROP_LILY_PAD, { 861, -22, 137 } },
    { FS_PROP_LILY_PAD, { 836, -22, 150 } },
    { FS_PROP_LILY_PAD, { 829, -22, 200 } },
    { FS_PROP_LILY_PAD, { 788, -22, 232 } },
    { FS_PROP_LILY_PAD, { 803, -22, 319 } },
    { FS_PROP_LILY_PAD, { 756, -22, 348 } },
    { FS_PROP_LILY_PAD, { 731, -22, 377 } },
    { FS_PROP_LILY_PAD, { 700, -22, 392 } },
    { FS_PROP_LILY_PAD, { 706, -22, 351 } },
    { FS_PROP_LILY_PAD, { 677, -22, 286 } },
    { FS_PROP_LILY_PAD, { 691, -22, 250 } },
    { FS_PROP_LILY_PAD, { 744, -22, 290 } },
    { FS_PROP_LILY_PAD, { 766, -22, 201 } },
    { FS_PROP_LILY_PAD, { 781, -22, 128 } },
    { FS_PROP_LILY_PAD, { 817, -22, 46 } },
    { FS_PROP_LILY_PAD, { 857, -22, -50 } },
    { FS_PROP_LILY_PAD, { 724, -22, 110 } },
    { FS_PROP_LILY_PAD, { 723, -22, 145 } },
    { FS_PROP_LILY_PAD, { 728, -22, 202 } },
    { FS_PROP_LILY_PAD, { 721, -22, 237 } },
    { FS_PROP_LILY_PAD, { 698, -22, 312 } },
    { FS_PROP_LILY_PAD, { 660, -22, 349 } },
    { FS_PROP_LILY_PAD, { 662, -22, 388 } },
    { FS_PROP_LILY_PAD, { 667, -22, 432 } },
    { FS_PROP_LILY_PAD, { 732, -22, 429 } },
    { FS_PROP_LILY_PAD, { 606, -22, 366 } },
    { FS_PROP_LILY_PAD, { 604, -22, 286 } },
    { FS_PROP_LILY_PAD, { 620, -22, 217 } },
    { FS_PROP_LILY_PAD, { 663, -22, 159 } },
    { FS_PROP_LILY_PAD, { 682, -22, 73 } },
    { FS_PROP_LILY_PAD, { 777, -22, 83 } },
    { FS_PROP_LILY_PAD, { 766, -22, 158 } },
    { FS_PROP_REED, { 1073, 0, -876 } },
    { FS_PROP_REED, { 970, 0, -853 } },
    { FS_PROP_REED, { 896, 0, -886 } },
    { FS_PROP_REED, { 646, -27, -651 } },
    { FS_PROP_REED, { 597, -29, -657 } },
    { FS_PROP_REED, { 547, -32, -651 } },
    { FS_PROP_REED, { 690, -29, -546 } },
    { FS_PROP_REED, { 720, -29, -490 } },
    { FS_PROP_REED, { -756, -30, -409 } },
    { FS_PROP_REED, { -688, -34, -458 } },
    { FS_PROP_REED, { -613, -34, -581 } },
    { FS_PROP_LILY_PAD, { -593, -22, -479 } },
    { FS_PROP_LILY_PAD, { -602, -22, -421 } },
    { FS_PROP_LILY_PAD, { -664, -22, -371 } },
    { FS_PROP_LILY_PAD, { -708, -22, -316 } },
    { FS_PROP_LILY_PAD, { -718, -22, -237 } },
    { FS_PROP_REED, { -807, -36, -183 } },
    { FS_PROP_REED, { -856, -29, -259 } },
    { FS_PROP_LILY_PAD, { -814, -22, -317 } },
    { FS_PROP_LILY_PAD, { -759, -22, -384 } },
    { FS_PROP_LILY_PAD, { -718, -22, -441 } },
    { FS_PROP_LILY_PAD, { -474, -22, -567 } },
    { FS_PROP_LILY_PAD, { -519, -22, -517 } },
    { FS_PROP_LILY_PAD, { -539, -22, -487 } },
    { FS_PROP_LILY_PAD, { -575, -22, -442 } },
    { FS_PROP_LILY_PAD, { -594, -22, -525 } },
    { FS_PROP_LILY_PAD, { -669, -22, -514 } },
    { FS_PROP_LILY_PAD, { -653, -22, -456 } },
    { FS_PROP_REED, { -663, -28, -606 } },
    { FS_PROP_REED, { -708, -26, -567 } },
    { FS_PROP_REED, { -739, -27, -506 } },
    { FS_PROP_REED, { -752, -28, -464 } },
    { FS_PROP_REED, { -709, -29, -513 } },
    { FS_PROP_LILY_PAD, { -544, -22, -436 } },
    { FS_PROP_LILY_PAD, { -559, -22, -397 } },
    { FS_PROP_LILY_PAD, { -616, -22, -353 } },
    { FS_PROP_LILY_PAD, { -712, -22, -368 } },
    { FS_PROP_LILY_PAD, { -678, -22, -403 } },
    { FS_PROP_LILY_PAD, { -664, -22, -273 } },
    { FS_PROP_LILY_PAD, { -630, -22, -276 } },
    { FS_PROP_LILY_PAD, { -579, -22, -311 } },
    { FS_PROP_LILY_PAD, { -588, -22, -351 } },
    { FS_PROP_LILY_PAD, { -555, -22, -534 } },
    { FS_PROP_LILY_PAD, { -547, -22, -567 } },
    { FS_PROP_LILY_PAD, { -592, -22, -571 } },
    { FS_PROP_LILY_PAD, { -541, -22, -610 } },
    { FS_PROP_LILY_PAD, { -476, -22, -629 } },
    { FS_PROP_LILY_PAD, { -439, -22, -598 } },
    { FS_PROP_LILY_PAD, { -412, -22, -550 } },
    { FS_PROP_LILY_PAD, { -411, -22, -606 } },
    { FS_PROP_LILY_PAD, { -370, -22, -634 } },
    { FS_PROP_LILY_PAD, { -352, -22, -662 } },
    { FS_PROP_LILY_PAD, { -413, -22, -641 } },
    { FS_PROP_LILY_PAD, { -488, -22, -666 } },
    { FS_PROP_LILY_PAD, { -578, -22, -656 } },
    { FS_PROP_LILY_PAD, { -560, -22, -640 } },
    { FS_PROP_LILY_PAD, { -531, -22, -654 } },
    { FS_PROP_LILY_PAD, { -451, -22, -669 } },
    { FS_PROP_LILY_PAD, { -439, -22, -699 } },
    { FS_PROP_LILY_PAD, { -482, -22, -719 } },
    { FS_PROP_LILY_PAD, { -524, -22, -720 } },
    { FS_PROP_LILY_PAD, { -569, -22, -714 } },
    { FS_PROP_REED, { -520, -27, -727 } },
    { FS_PROP_REED, { -572, -28, -686 } },
    { FS_PROP_REED, { -588, -32, -631 } },
    { FS_PROP_REED, { -622, -34, -571 } },
    { FS_PROP_REED, { -628, -36, -510 } },
    { FS_PROP_REED, { -655, -36, -466 } },
    { FS_PROP_REED, { -655, -41, -393 } },
    { FS_PROP_REED, { -661, -47, -328 } },
    { FS_PROP_REED, { -723, -40, -287 } },
    { FS_PROP_REED, { -756, -33, -349 } },
    { FS_PROP_REED, { -755, -43, -210 } },
    { FS_PROP_LILY_PAD, { -770, -22, -281 } },
    { FS_PROP_LILY_PAD, { -750, -22, -313 } },
    { FS_PROP_LILY_PAD, { -736, -22, -341 } },
    { FS_PROP_LILY_PAD, { -620, -22, -418 } },
    { FS_PROP_LILY_PAD, { -601, -22, -371 } },
    { FS_PROP_LILY_PAD, { -635, -22, -383 } },
    { FS_PROP_LILY_PAD, { -627, -22, -311 } },
    { FS_PROP_LILY_PAD, { -665, -22, -327 } },
    { FS_PROP_LILY_PAD, { -524, -22, -537 } },
    { FS_PROP_LILY_PAD, { -514, -22, -579 } },
    { FS_PROP_LILY_PAD, { -512, -22, -623 } },
    { FS_PROP_LILY_PAD, { -576, -22, -582 } },
    { FS_PROP_LILY_PAD, { -600, -22, -608 } },
    { FS_PROP_LILY_PAD, { -657, -22, -531 } },
    { FS_PROP_LILY_PAD, { -641, -22, -547 } },
    { FS_PROP_INIT_STOP, { 0 } },
};

void Fishing_InitPondProps(Fishing* this, PlayState* play) {
    FishingProp* prop = &sPondProps[0];
    Vec3f colliderPos;
    s16 i;

    Fishing_SeedRand(1, 29100, 9786);

    for (i = 0; i < POND_PROP_COUNT; i++) {
        if (sPondPropInits[i].type == FS_PROP_INIT_STOP) {
            break;
        }

        prop->epoch++;
        prop->type = sPondPropInits[i].type;
        prop->pos.x = sPondPropInits[i].pos.x;
        prop->pos.y = sPondPropInits[i].pos.y;
        prop->pos.z = sPondPropInits[i].pos.z;
        prop->rotX = 0.0f;
        prop->reedAngle = 0.0f;

        prop->timer = Rand_ZeroFloat(100.0f);
        prop->drawDistance = 800.0f;

        if (prop->type == FS_PROP_REED) {
            prop->scale = (Fishing_RandZeroOne() * 0.25f) + 0.75f;
            prop->reedAngle = Rand_ZeroFloat(2 * M_PI);
            if (sLinkAge == 1) {
                prop->scale *= 0.6f;
            }
            prop->drawDistance = 1200.0f;
        } else if (prop->type == FS_PROP_WOOD_POST) {
            prop->scale = 0.08f;
            prop->drawDistance = 1200.0f;
            colliderPos = prop->pos;
            colliderPos.y += 50.0f;
            Fishing_SetColliderElement(i, &sFishingMain->collider, &colliderPos, prop->scale * 3.5f);
        } else if (prop->type == FS_PROP_LILY_PAD) {
            prop->scale = (Fishing_RandZeroOne() * 0.3f) + 0.5f;
            prop->rotY = Rand_ZeroFloat(2 * M_PI);
            if (sLinkAge == 1) {
                if ((i % 4) != 0) {
                    prop->scale *= 0.6f;
                } else {
                    prop->type = FS_PROP_NONE;
                }
            }
        } else {
            prop->scale = (Fishing_RandZeroOne() * 0.1f) + 0.3f;
            prop->rotY = Rand_ZeroFloat(2 * M_PI);
            prop->drawDistance = 1000.0f;
            Fishing_SetColliderElement(i, &sFishingMain->collider, &prop->pos, prop->scale);
        }

        prop++;
    }
}

static FishingFishInit sFishInits[] = {
    { 0, { 666, -45, 354 }, 38, 0.1f },    { 0, { 681, -45, 240 }, 36, 0.1f },   { 0, { 670, -45, 90 }, 41, 0.05f },
    { 0, { 615, -45, -450 }, 35, 0.2f },   { 0, { 500, -45, -420 }, 39, 0.1f },  { 0, { 420, -45, -550 }, 44, 0.05f },
    { 0, { -264, -45, -640 }, 40, 0.1f },  { 0, { -470, -45, -540 }, 34, 0.2f }, { 0, { -557, -45, -430 }, 54, 0.01f },
    { 0, { -260, -60, -330 }, 47, 0.05f }, { 0, { -500, -60, 330 }, 42, 0.06f }, { 0, { 428, -40, -283 }, 33, 0.2f },
    { 0, { 409, -70, -230 }, 57, 0.0f },   { 0, { 450, -67, -300 }, 63, 0.0f },  { 0, { -136, -65, -196 }, 71, 0.0f },
    { 1, { -561, -35, -547 }, 45, 0.0f },  { 1, { 667, -35, 317 }, 43, 0.0f },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 0, ICHAIN_STOP),
};

void Fishing_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Fishing* this = (Fishing*)thisx;
    u16 fishCount;
    s16 i;

    Actor_ProcessInitChain(thisx, sInitChain);
    ActorShape_Init(&thisx->shape, 0.0f, NULL, 0.0f);

    if (KREG(5) != 0) {
        sLinkAge = 1;
    } else {
        sLinkAge = gSaveContext.linkAge;
    }

    if (thisx->params < 100) {
        D_80B7E074 = 0;
        sFishingMain = this;
        Collider_InitJntSph(play, &sFishingMain->collider);
        Collider_SetJntSph(play, &sFishingMain->collider, thisx, &sJntSphInit, sFishingMain->colliderElements);

        thisx->params = 1;

        SkelAnime_InitFlex(play, &this->skelAnime, &gFishingOwnerSkel, &gFishingOwnerAnim, NULL, NULL, 0);
        Animation_MorphToLoop(&this->skelAnime, &gFishingOwnerAnim, 0.0f);

        thisx->update = Fishing_UpdateOwner;
        thisx->draw = Fishing_DrawOwner;

        thisx->shape.rot.y = -0x6000;
        thisx->world.pos.x = 160.0f;
        thisx->world.pos.y = -2.0f;
        thisx->world.pos.z = 1208.0f;

        Actor_SetScale(thisx, 0.011f);

        thisx->focus.pos = thisx->world.pos;
        thisx->focus.pos.y += 75.0f;
        thisx->flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;

        if (sLinkAge != 1) {
            if (HIGH_SCORE(HS_FISHING) & 0x1000) {
                D_80B7A688 = 0;
            } else {
                D_80B7A688 = 1;
            }
        } else {
            D_80B7A688 = 2;
        }

        D_80B7A684 = 20;
        play->specialEffects = sFishingEffects;
        gTimeIncrement = 1;
        D_80B7E0AC = 0;
        D_80B7E0A6 = 10;

        Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x0100FF);

        if (sLinkAge == 1) {
            if ((HIGH_SCORE(HS_FISHING) & 0x7F) != 0) {
                D_80B7E078 = HIGH_SCORE(HS_FISHING) & 0x7F;
            } else {
                D_80B7E078 = 40.0f;
            }
        } else {
            if ((HIGH_SCORE(HS_FISHING) & 0x7F000000) != 0) {
                D_80B7E078 = (HIGH_SCORE(HS_FISHING) & 0x7F000000) >> 0x18;
            } else {
                D_80B7E078 = 45.0f;
            }
        }

        D_80B7E07D = (HIGH_SCORE(HS_FISHING) & 0xFF0000) >> 0x10;
        if ((D_80B7E07D & 7) == 7) {
            play->roomCtx.unk_74[0] = 90;
            D_80B7E076 = 1;
        } else {
            play->roomCtx.unk_74[0] = 40;
            D_80B7E076 = 0;
        }

        if (((D_80B7E07D & 7) == 6) || (KREG(3) != 0)) {
            D_80B7E077 = 100;
            if (KREG(3) != 0) {
                KREG(3) = 0;
                HIGH_SCORE(HS_FISHING) &= 0xFF00FFFF;
                HIGH_SCORE(HS_FISHING) |= 0x60000;
            }
        } else {
            D_80B7E077 = 0;
        }

        for (i = 0; i < EFFECT_COUNT; i++) {
            sFishingEffects[i].type = FS_EFF_NONE;
        }

        for (i = 0; i < POND_PROP_COUNT; i++) {
            sPondProps[i].type = FS_PROP_NONE;
        }

        sFishGroupAngle1 = 0.7f;
        sFishGroupAngle2 = 2.3f;
        sFishGroupAngle3 = 4.6f;

        for (i = 0; i < GROUP_FISH_COUNT; i++) {
            FishingGroupFish* fish = &sGroupFishes[i];

            fish->epoch++;

            fish->type = FS_GROUP_FISH_NORMAL;

            if (i <= 20) {
                fish->unk_10.x = fish->pos.x = sinf(sFishGroupAngle1) * 720.0f;
                fish->unk_10.z = fish->pos.z = cosf(sFishGroupAngle1) * 720.0f;
            } else if (i <= 40) {
                fish->unk_10.x = fish->pos.x = sinf(sFishGroupAngle2) * 720.0f;
                fish->unk_10.z = fish->pos.z = cosf(sFishGroupAngle2) * 720.0f;
            } else {
                fish->unk_10.x = fish->pos.x = sinf(sFishGroupAngle3) * 720.0f;
                fish->unk_10.z = fish->pos.z = cosf(sFishGroupAngle3) * 720.0f;
            }

            fish->unk_10.y = fish->pos.y = -35.0f;

            fish->timer = Rand_ZeroFloat(100.0f);

            fish->unk_3C = 0;
            fish->unk_3E = 0;
            fish->unk_40 = 0;

            if (sLinkAge != 1) {
                if (((i >= 15) && (i < 20)) || ((i >= 35) && (i < 40)) || ((i >= 55) && (i < 60))) {
                    fish->type = FS_GROUP_FISH_NONE;
                }
            }
        }

        Fishing_InitPondProps(this, play);
        Actor_SpawnAsChild(&play->actorCtx, thisx, play, ACTOR_EN_KANBAN, 53.0f, -17.0f, 982.0f, 0, 0, 0,
                           ENKANBAN_FISHING);
        Actor_Spawn(&play->actorCtx, play, ACTOR_FISHING, 0.0f, 0.0f, 0.0f, 0, 0, 0, 200);

        if ((KREG(1) == 1) || ((D_80B7E07D & 3) == 3)) {
            if (sLinkAge != 1) {
                fishCount = 16;
            } else {
                fishCount = 17;
            }
        } else {
            fishCount = 15;
        }

        for (i = 0; i < fishCount; i++) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_FISHING, sFishInits[i].pos.x, sFishInits[i].pos.y,
                        sFishInits[i].pos.z, 0, Rand_ZeroFloat(0x10000), 0, 100 + i);
        }
    } else {
        if ((thisx->params < 115) || (thisx->params == 200)) {
            SkelAnime_InitFlex(play, &this->skelAnime, &gFishingFishSkel, &gFishingFishAnim, NULL, NULL, 0);
            Animation_MorphToLoop(&this->skelAnime, &gFishingFishAnim, 0.0f);
        } else {
            SkelAnime_InitFlex(play, &this->skelAnime, &gFishingLoachSkel, &gFishingLoachAnim, NULL, NULL, 0);
            Animation_MorphToLoop(&this->skelAnime, &gFishingLoachAnim, 0.0f);
        }

        SkelAnime_Update(&this->skelAnime);

        if (thisx->params == 200) {
            this->unk_158 = 100;
            Actor_ChangeCategory(play, &play->actorCtx, thisx, ACTORCAT_PROP);
            thisx->targetMode = 0;
            thisx->flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
            this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
        } else {
            this->unk_158 = 10;
            this->unk_15A = 10;

            this->unk_150 = sFishInits[thisx->params - 100].unk_00;
            this->unk_1A8 = sFishInits[thisx->params - 100].unk_0C;
            this->unk_1AC = sFishInits[thisx->params - 100].unk_08;

            this->unk_1AC += Rand_ZeroFloat(4.99999f);

            if ((this->unk_1AC >= 65.0f) && (Rand_ZeroOne() < 0.05f)) {
                this->unk_1AC += Rand_ZeroFloat(7.99999f);
            }

            if (KREG(6) != 0) {
                this->unk_1AC = KREG(6) + 80.0f;
            }

            if (sLinkAge == 1) {
                this->unk_1AC *= 0.73f;
            }
        }
    }
}

void Fishing_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Fishing* this = (Fishing*)thisx;

    SkelAnime_Free(&this->skelAnime, play);

    if (thisx->params == 200) {
        LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
    } else if (thisx->params == 1) {
        Collider_DestroyJntSph(play, &this->collider);
    }
}

void Fishing_UpdateEffects(FishingEffect* effect, PlayState* play) {
    f32 rippleY;
    s16 i;

    for (i = 0; i < EFFECT_COUNT; i++) {
        if (effect->type) {
            effect->timer++;
            effect->pos.x += effect->vel.x;
            effect->pos.y += effect->vel.y;
            effect->pos.z += effect->vel.z;
            effect->vel.y += effect->accel.y;

            if (effect->type == FS_EFF_RIPPLE) {
                Math_ApproachF(&effect->unk_30, effect->unk_34, 0.2f, effect->unk_38);

                if (effect->unk_2C == 0) {
                    effect->alpha += 20;

                    if (effect->alpha >= effect->unk_2E) {
                        effect->alpha = effect->unk_2E;
                        effect->unk_2C++;
                    }
                } else {
                    effect->alpha -= 8;

                    if (effect->alpha <= 0) {
                        effect->type = FS_EFF_NONE;
                    }
                }
            } else if (effect->type == FS_EFF_WATER_DUST) {
                Math_ApproachF(&effect->unk_30, effect->unk_34, 0.1f, 0.1f);
                effect->alpha -= 10;

                if (effect->pos.y > (WATER_SURFACE_Y(play) - 5.0f)) {
                    effect->accel.y = 0.0f;
                    effect->vel.y = 0.0f;
                    effect->alpha -= 5;
                }

                if (effect->alpha <= 0) {
                    effect->type = FS_EFF_NONE;
                }
            } else if (effect->type == FS_EFF_BUBBLE) {
                if (effect->unk_2C == 0) {
                    rippleY = WATER_SURFACE_Y(play);
                } else {
                    rippleY = 69.0f;
                }

                if (effect->pos.y >= rippleY) {
                    effect->type = FS_EFF_NONE;

                    if (Rand_ZeroOne() < 0.3f) {
                        Vec3f pos = effect->pos;
                        pos.y = rippleY;
                        Fishing_SpawnRipple(NULL, play->specialEffects, &pos, 20.0f, 60.0f, 150, 90);
                    }
                }
            } else if (effect->type == FS_EFF_DUST_SPLASH) {
                if (effect->vel.y < -20.0f) {
                    effect->vel.y = -20.0f;
                    effect->accel.y = 0.0f;
                }

                if (effect->pos.y <= WATER_SURFACE_Y(play)) {
                    effect->type = FS_EFF_NONE;
                    if (Rand_ZeroOne() < 0.5f) {
                        Vec3f pos = effect->pos;
                        pos.y = WATER_SURFACE_Y(play);
                        Fishing_SpawnRipple(NULL, play->specialEffects, &pos, 40.0f, 110.0f, 150, 90);
                    }
                }
            } else if (effect->type == FS_EFF_RAIN_DROP) {
                if (effect->pos.y < WATER_SURFACE_Y(play)) {
                    f32 sqDistXZ = SQ(effect->pos.x) + SQ(effect->pos.z);

                    if (sqDistXZ > SQ(920.0f)) {
                        effect->pos.y = WATER_SURFACE_Y(play) + ((sqrtf(sqDistXZ) - 920.0f) * 0.11f);
                        effect->timer = KREG(17) + 2;
                        effect->type = FS_EFF_RAIN_SPLASH;
                        effect->unk_30 = (KREG(18) + 30) * 0.001f;
                    } else {
                        effect->pos.y = WATER_SURFACE_Y(play) + 3.0f;
                        effect->timer = 0;
                        if (Rand_ZeroOne() < 0.75f) {
                            effect->type = FS_EFF_RAIN_RIPPLE;
                            effect->vel = sZeroVec;
                            effect->unk_30 = (KREG(18) + 30) * 0.001f;
                        } else {
                            effect->type = FS_EFF_NONE;
                        }
                    }

                    effect->vel = sZeroVec;
                }
            } else if (effect->type >= FS_EFF_RAIN_RIPPLE) {
                effect->unk_30 += (KREG(18) + 30) * 0.001f;

                if (effect->timer >= 6) {
                    effect->type = FS_EFF_NONE;
                }
            } else if (effect->type == FS_EFF_OWNER_HAT) {
                f32 sqDistXZ;
                f32 bottomY;

                effect->unk_30 = 0.010000001f;

                Math_ApproachS(&sEffOwnerHatRot.y, 0, 20, 100);
                Math_ApproachS(&sEffOwnerHatRot.x, 0, 20, 100);
                Math_ApproachS(&sEffOwnerHatRot.z, -0x4000, 20, 100);

                sqDistXZ = SQ(effect->pos.x) + SQ(effect->pos.z);
                bottomY = WATER_SURFACE_Y(play) + ((sqrtf(sqDistXZ) - 920.0f) * 0.147f);

                if (effect->pos.y > (bottomY - 10.0f)) {
                    effect->pos.y -= 0.1f;
                }

                if ((effect->timer % 16) == 0) {
                    Vec3f pos = effect->pos;
                    pos.y = WATER_SURFACE_Y(play);
                    Fishing_SpawnRipple(NULL, play->specialEffects, &pos, 30.0f, 300.0f, 150, 90);
                }

                if (effect->unk_2C >= 0) {
                    effect->unk_2C++;
                }

                if (effect->unk_2C == 30) {
                    Message_StartTextbox(play, 0x40B3, NULL);
                }

                if ((effect->unk_2C >= 100) && (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT)) {
                    if (Message_ShouldAdvance(play) || (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                        Message_CloseTextbox(play);
                        Rupees_ChangeBy(-50);
                        effect->unk_2C = -1;
                    }
                }
            }
        }

        effect++;
    }
}

void Fishing_DrawEffects(FishingEffect* effect, PlayState* play) {
    u8 flag = 0;
    f32 rotY;
    s16 i;
    s32 pad;
    FishingEffect* firstEffect = effect;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Push();

    gDPPipeSync(POLY_XLU_DISP++);

    for (i = 0; i < 100; i++) {
        if (effect->type == FS_EFF_RIPPLE) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingRippleMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 155, 155, 155, 0);
                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, effect->alpha);

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(effect->unk_30, 1.0f, effect->unk_30, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            gSPDisplayList(POLY_XLU_DISP++, gFishingRippleModelDL);
            FrameInterpolation_RecordCloseChild();
        }
        effect++;
    }

    effect = firstEffect;
    flag = 0;
    for (i = 0; i < 100; i++) {
        if (effect->type == FS_EFF_DUST_SPLASH) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingDustSplashMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 200, 0);
                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 180, 180, 180, effect->alpha);

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->unk_30, effect->unk_30, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            gSPDisplayList(POLY_XLU_DISP++, gFishingDustSplashModelDL);
            FrameInterpolation_RecordCloseChild();
        }
        effect++;
    }

    effect = firstEffect;
    flag = 0;
    for (i = 0; i < 100; i++) {
        if (effect->type == FS_EFF_WATER_DUST) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (flag == 0) {
                gSPDisplayList(POLY_OPA_DISP++, gFishingWaterDustMaterialDL);
                gDPSetEnvColor(POLY_OPA_DISP++, 40, 90, 80, 128);
                flag++;
            }

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 40, 90, 80, effect->alpha);

            gSPSegment(POLY_OPA_DISP++, 0x08,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, effect->timer + (i * 3),
                                        (effect->timer + (i * 3)) * 5, 32, 64, 1, 0, 0, 32, 32));

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->unk_30, effect->unk_30, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            gSPDisplayList(POLY_OPA_DISP++, gFishingWaterDustModelDL);
            FrameInterpolation_RecordCloseChild();
        }
        effect++;
    }

    effect = firstEffect;
    flag = 0;
    for (i = 0; i < 100; i++) {
        if (effect->type == FS_EFF_BUBBLE) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingBubbleMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 150, 150, 150, 0);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
                flag++;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->unk_30, effect->unk_30, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            gSPDisplayList(POLY_XLU_DISP++, gFishingBubbleModelDL);
            FrameInterpolation_RecordCloseChild();
        }
        effect++;
    }

    effect = firstEffect + 30;
    flag = 0;
    for (i = 30; i < EFFECT_COUNT; i++) {
        if (effect->type == FS_EFF_RAIN_DROP) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (flag == 0) {
                POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0x14);
                gDPSetCombineMode(POLY_XLU_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 150, 255, 255, 30);
                flag++;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateY(effect->unk_38, MTXMODE_APPLY);
            Matrix_RotateX(effect->unk_34, MTXMODE_APPLY);
            Matrix_RotateZ(effect->unk_3C, MTXMODE_APPLY);
            Matrix_Scale(0.002f, 1.0f, 0.1f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            gSPDisplayList(POLY_XLU_DISP++, gFishingRainDropModelDL);
            FrameInterpolation_RecordCloseChild();
        }
        effect++;
    }

    func_80093D84(play->state.gfxCtx);

    effect = firstEffect + 30;
    flag = 0;
    for (i = 30; i < EFFECT_COUNT; i++) {
        if (effect->type == FS_EFF_RAIN_RIPPLE) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingRippleMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 155, 155, 155, 0);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 130);
                flag++;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(effect->unk_30, 1.0f, effect->unk_30, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            gSPDisplayList(POLY_XLU_DISP++, gFishingRippleModelDL);
            FrameInterpolation_RecordCloseChild();
        }
        effect++;
    }

    effect = firstEffect + 30;
    flag = 0;
    for (i = 30; i < EFFECT_COUNT; i++) {
        if (effect->type == FS_EFF_RAIN_SPLASH) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingRainSplashMaterialDL);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, KREG(19) + 80);
                flag++;
            }

            if (Rand_ZeroOne() < 0.5f) {
                rotY = 0.0f;
            } else {
                rotY = M_PI;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_RotateY(rotY, MTXMODE_APPLY);
            Matrix_Scale(effect->unk_30, effect->unk_30, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            gSPDisplayList(POLY_XLU_DISP++, gFishingRainSplashModelDL);
            FrameInterpolation_RecordCloseChild();
        }
        effect++;
    }

    effect = firstEffect;
    if (effect->type == FS_EFF_OWNER_HAT) {
        FrameInterpolation_RecordOpenChild(effect, effect->epoch);
        Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
        Matrix_RotateY((sEffOwnerHatRot.y * M_PI) / 32768, MTXMODE_APPLY);
        Matrix_RotateX((sEffOwnerHatRot.x * M_PI) / 32768, MTXMODE_APPLY);
        Matrix_RotateZ((sEffOwnerHatRot.z * M_PI) / 32768, MTXMODE_APPLY);
        Matrix_Scale(effect->unk_30, effect->unk_30, effect->unk_30, MTXMODE_APPLY);
        Matrix_Translate(-1250.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateX(M_PI / 2, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPDisplayList(POLY_OPA_DISP++, gFishingOwnerHatDL);
        FrameInterpolation_RecordCloseChild();
    }

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

void Fishing_DrawStreamSplash(PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x09,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, play->gameplayFrames * 1,
                                play->gameplayFrames * 8, 32, 64, 1, -(play->gameplayFrames * 2), 0, 16, 16));

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, 50);

    Matrix_Translate(670.0f, -24.0f, -600.0f, MTXMODE_NEW);
    Matrix_Scale(0.02f, 1.0f, 0.02f, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gFishingStreamSplashDL));

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 func_80B6C2EC(Vec3f* vec) {
    if (((vec->x >= 110.0f) && (vec->x <= 150.0f) && (vec->z <= 1400.0f) && (vec->z >= 1160.0f)) ||
        ((vec->x >= 110.0f) && (vec->x <= 210.0f) && (vec->z <= 1200.0f) && (vec->z >= 1160.0f))) {
        if (vec->y <= 42.0f) {
            return true;
        }
    }

    return false;
}

void Fishing_UpdateLine(PlayState* play, Vec3f* basePos, Vec3f* pos, Vec3f* rot, Vec3f* unk) {
    s16 i;
    s16 k;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 rx;
    f32 ry;
    f32 dist;
    f32 spD8;
    s16 temp_s2;
    s32 pad;
    f32 temp_f20;
    Vec3f posSrc = { 0.0f, 0.0f, 0.0f };
    Vec3f posStep;
    f32 phi_f18;
    Vec3f spA4;
    Vec3f sp98;
    f32 sp94;
    f32 sp90;
    f32 sp8C;
    f32 sqDistXZ;
    f32 temp_f18;
    f32 phi_f12;
    f32 phi_f2;

    if (D_80B7A6A4 != 0) {
        spA4 = *basePos;
        sp98 = pos[LINE_SEG_COUNT - 1];

        sp94 = sp98.x - spA4.x;
        sp90 = sp98.y - spA4.y;
        sp8C = sp98.z - spA4.z;

        phi_f18 = sqrtf(SQ(sp94) + SQ(sp90) + SQ(sp8C)) * 0.97f;
        if (phi_f18 > 1000.0f) {
            phi_f18 = 1000.0f;
        }

        D_80B7E144 = 200.0f - (phi_f18 * 200.0f * 0.001f);
    }

    temp_s2 = D_80B7E144;
    posSrc.z = 5.0f;

    for (i = 0; i < LINE_SEG_COUNT; i++) {
        if (i <= temp_s2) {
            pos[i] = *basePos;
        } else if (D_80B7A6A4 != 0) {
            temp_f20 = (f32)(i - temp_s2) / (f32)(LINE_SEG_COUNT - temp_s2 + 1);
            Math_ApproachF(&pos[i].x, (sp94 * temp_f20) + spA4.x, 1.0f, 20.0f);
            Math_ApproachF(&pos[i].y, (sp90 * temp_f20) + spA4.y, 1.0f, 20.0f);
            Math_ApproachF(&pos[i].z, (sp8C * temp_f20) + spA4.z, 1.0f, 20.0f);
        }
    }

    for (i = temp_s2 + 1, k = 0; i < LINE_SEG_COUNT; i++, k++) {
        temp_f18 = 2.0f * D_80B7E148;

        dx = (pos + i)->x - (pos + i - 1)->x;
        spD8 = (pos + i)->y;

        sqDistXZ = SQ((pos + i)->x) + SQ((pos + i)->z);

        if (sqDistXZ > SQ(920.0f)) {
            phi_f12 = ((sqrtf(sqDistXZ) - 920.0f) * 0.11f) + WATER_SURFACE_Y(play);
        } else {
            phi_f12 = WATER_SURFACE_Y(play);
        }

        if (D_80B7E0B6 == 2) {
            if (spD8 < phi_f12) {
                phi_f12 = ((sqrtf(sqDistXZ) - 920.0f) * 0.147f) + WATER_SURFACE_Y(play);
                if (spD8 > phi_f12) {
                    phi_f2 = (spD8 - phi_f12) * 0.05f;
                    if (phi_f2 > 0.29999998f) {
                        phi_f2 = 0.29999998f;
                    }
                    if (i >= 100) {
                        phi_f2 *= (i - 100) * 0.02f;
                        spD8 -= phi_f2;
                    }
                }
            } else {
                spD8 -= temp_f18;
            }
        } else if (i > LINE_SEG_COUNT - 10) {
            if (spD8 > phi_f12) {
                phi_f2 = (spD8 - phi_f12) * 0.2f;
                if (phi_f2 > temp_f18) {
                    phi_f2 = temp_f18;
                }
                spD8 -= phi_f2;
            }
        } else {
            if (spD8 > phi_f12) {
                spD8 -= temp_f18;
            }
        }

        if (func_80B6C2EC(&pos[i])) {
            spD8 = 42.0f;
        }

        dy = spD8 - (pos + i - 1)->y;
        dz = (pos + i)->z - (pos + i - 1)->z;

        ry = Math_Atan2F(dz, dx);
        dist = sqrtf(SQ(dx) + SQ(dz));
        rx = -Math_Atan2F(dist, dy);

        (rot + i - 1)->y = ry;
        (rot + i - 1)->x = rx;

        Matrix_RotateY(ry, MTXMODE_NEW);
        Matrix_RotateX(rx, MTXMODE_APPLY);
        Matrix_MultVec3f(&posSrc, &posStep);

        (pos + i)->x = (pos + i - 1)->x + posStep.x;
        (pos + i)->y = (pos + i - 1)->y + posStep.y;
        (pos + i)->z = (pos + i - 1)->z + posStep.z;
    }
}

void Fishing_UpdateLinePos(Vec3f* pos) {
    s16 i;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 rx;
    f32 ry;
    f32 dist;
    Vec3f posSrc = { 0.0f, 0.0f, 0.0f };
    Vec3f posStep;
    s16 min = D_80B7E144;

    posSrc.z = 5.0f;

    for (i = LINE_SEG_COUNT - 2; i > min; i--) {
        dx = (pos + i)->x - (pos + i + 1)->x;
        dy = (pos + i)->y - (pos + i + 1)->y;
        dz = (pos + i)->z - (pos + i + 1)->z;

        ry = Math_Atan2F(dz, dx);
        dist = sqrtf(SQ(dx) + SQ(dz));
        rx = -Math_Atan2F(dist, dy);

        Matrix_RotateY(ry, MTXMODE_NEW);
        Matrix_RotateX(rx, MTXMODE_APPLY);
        Matrix_MultVec3f(&posSrc, &posStep);

        (pos + i)->x = (pos + i + 1)->x + posStep.x;
        (pos + i)->y = (pos + i + 1)->y + posStep.y;
        (pos + i)->z = (pos + i + 1)->z + posStep.z;
    }
}

void Fishing_DrawLureHook(PlayState* play, Vec3f* pos, Vec3f* refPos, u8 hookIndex) {
    f32 dx;
    f32 dy;
    f32 dz;
    f32 rx;
    f32 ry;
    f32 dist;
    f32 offsetY;
    Vec3f posSrc = { 0.0f, 0.0f, 1.0f };
    Vec3f posStep;
    Player* player = GET_PLAYER(play);

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Push();

    if ((D_80B7A694 == 3) && ((pos->y > WATER_SURFACE_Y(play)) || ((D_80B7A68C != 0) && hookIndex))) {
        offsetY = 0.0f;
    } else if (pos->y < WATER_SURFACE_Y(play)) {
        offsetY = -1.0f;
    } else {
        offsetY = -3.0f;
    }

    dx = refPos->x - pos->x;
    dy = refPos->y - pos->y + offsetY;
    dz = refPos->z - pos->z;

    ry = Math_Atan2F(dz, dx);
    dist = sqrtf(SQ(dx) + SQ(dz));
    rx = -Math_Atan2F(dist, dy);

    Matrix_RotateY(ry, MTXMODE_NEW);
    Matrix_RotateX(rx, MTXMODE_APPLY);
    Matrix_MultVec3f(&posSrc, &posStep);

    refPos->x = pos->x + posStep.x;
    refPos->y = pos->y + posStep.y;
    refPos->z = pos->z + posStep.z;

    Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_NEW);

    if ((player->actor.speedXZ == 0.0f) && (D_80B7E138 == 0.0f)) {
        Math_ApproachF(&sLureHookRotY[hookIndex], ry, 0.1f, 0.3f);
    } else {
        sLureHookRotY[hookIndex] = ry;
    }

    Matrix_RotateY(sLureHookRotY[hookIndex], MTXMODE_APPLY);
    Matrix_RotateX(rx, MTXMODE_APPLY);
    Matrix_Scale(0.0039999997f, 0.0039999997f, 0.005f, MTXMODE_APPLY);
    Matrix_RotateY(M_PI, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gFishingLureHookDL);

    Matrix_RotateZ(M_PI / 2, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gFishingLureHookDL);

    if ((hookIndex == 1) && (D_80B7A68C != 0)) {
        Matrix_Scale(2.0f, 2.0f, 2.0f, MTXMODE_APPLY);
        Matrix_Translate(250.0f, 0.0f, -1400.0f, MTXMODE_APPLY);
        Matrix_Push();

        if (D_80B7A690 != 0) {
            FishingEffect* effect = play->specialEffects;
            MtxF mf;

            Matrix_MultVec3f(&sZeroVec, &effect->pos);
            Matrix_Get(&mf);
            Matrix_MtxFToYXZRotS(&mf, &sEffOwnerHatRot, 0);

            D_80B7A690 = 0;
            D_80B7A68C = 0;

            effect->type = FS_EFF_OWNER_HAT;
            effect->unk_2C = 0;
            effect->vel = sZeroVec;
            effect->accel = sZeroVec;
        }

        Matrix_Pop();
        Matrix_Translate(-1250.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateX(M_PI / 2, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gFishingOwnerHatDL);
    }

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

void Fishing_UpdateSinkingLure(PlayState* play) {
    s16 i;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 rx;
    f32 ry;
    f32 dist;
    f32 offsetY;
    Vec3f posSrc = { 0.0f, 0.0f, 0.0f };
    Vec3f posStep;
    Vec3f sp94;
    Vec3f sp88;
    f32 offsetX;
    f32 offsetZ;
    Player* player = GET_PLAYER(play);

    posSrc.z = 0.85f;

    sSinkingLurePos[0] = sLurePos;

    if (D_80B7A6D4 != 0) {
        offsetY = -1.0f;
    } else if (sLurePos.y < WATER_SURFACE_Y(play)) {
        offsetY = 0.5f;
    } else {
        offsetY = -5.0f;
    }

    if (D_80B7A694 == 5) {
        Matrix_RotateY(player->actor.shape.rot.y * (M_PI / 32768), MTXMODE_NEW);
        sp94.x = 5.0f;
        sp94.y = 0.0f;
        sp94.z = 3.0f;
        Matrix_MultVec3f(&sp94, &sp88);
    }

    for (i = 1; i < SINKING_LURE_SEG_COUNT; i++) {
        Vec3f* pos = sSinkingLurePos;

        if ((i < 10) && (D_80B7A694 == 5)) {
            offsetX = (10 - i) * sp88.x * 0.1f;
            offsetZ = (10 - i) * sp88.z * 0.1f;
        } else {
            offsetX = offsetZ = 0.0f;
        }

        dx = (pos + i)->x - (pos + i - 1)->x + offsetX;
        dy = (pos + i)->y - (pos + i - 1)->y + offsetY;
        dz = (pos + i)->z - (pos + i - 1)->z + offsetZ;

        ry = Math_Atan2F(dz, dx);
        dist = sqrtf(SQ(dx) + SQ(dz));
        rx = -Math_Atan2F(dist, dy);

        Matrix_RotateY(ry, MTXMODE_NEW);
        Matrix_RotateX(rx, MTXMODE_APPLY);
        Matrix_MultVec3f(&posSrc, &posStep);

        (pos + i)->x = (pos + i - 1)->x + posStep.x;
        (pos + i)->y = (pos + i - 1)->y + posStep.y;
        (pos + i)->z = (pos + i - 1)->z + posStep.z;
    }
}

static f32 sSinkingLureSizes[] = {
    1.0f, 1.5f,  1.8f, 2.0f, 1.8f, 1.6f, 1.4f, 1.2f, 1.0f, 1.0f,
    0.9f, 0.85f, 0.8f, 0.7f, 0.8f, 1.0f, 1.2f, 1.1f, 1.0f, 0.8f,
};

void Fishing_DrawSinkingLure(PlayState* play) {
    s16 i;
    f32 scale;

    OPEN_DISPS(play->state.gfxCtx);

    Fishing_UpdateSinkingLure(play);

    if (sLurePos.y < WATER_SURFACE_Y(play)) {
        func_80093D18(play->state.gfxCtx);

        gSPDisplayList(POLY_OPA_DISP++, gFishingSinkingLureSegmentMaterialDL);

        for (i = SINKING_LURE_SEG_COUNT - 1; i >= 0; i--) {
            if ((i + D_80B7FEA0) < SINKING_LURE_SEG_COUNT) {
                FrameInterpolation_RecordOpenChild("Fishing Lures 1", i);
                Matrix_Translate(sSinkingLurePos[i].x, sSinkingLurePos[i].y, sSinkingLurePos[i].z, MTXMODE_NEW);
                scale = sSinkingLureSizes[i + D_80B7FEA0] * 0.04f;
                Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
                Matrix_ReplaceRotation(&play->billboardMtxF);

                gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_OPA_DISP++, gFishingSinkingLureSegmentModelDL);
                FrameInterpolation_RecordCloseChild();
            }
        }
    } else {
        func_80093D84(play->state.gfxCtx);

        gSPDisplayList(POLY_XLU_DISP++, gFishingSinkingLureSegmentMaterialDL);

        for (i = SINKING_LURE_SEG_COUNT - 1; i >= 0; i--) {
            if ((i + D_80B7FEA0) < SINKING_LURE_SEG_COUNT) {
                FrameInterpolation_RecordOpenChild("Fishing Lures 2", i);
                Matrix_Translate(sSinkingLurePos[i].x, sSinkingLurePos[i].y, sSinkingLurePos[i].z, MTXMODE_NEW);
                scale = sSinkingLureSizes[i + D_80B7FEA0] * 0.04f;
                Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
                Matrix_ReplaceRotation(&play->billboardMtxF);

                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, gFishingSinkingLureSegmentModelDL);
                FrameInterpolation_RecordCloseChild();
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Fishing_DrawLureAndLine(PlayState* play, Vec3f* linePos, Vec3f* lineRot) {
    Vec3f posSrc;
    Vec3f posStep;
    Vec3f hookPos[2];
    s16 i;
    s16 spB4 = D_80B7E144;
    s32 pad;
    Player* player = GET_PLAYER(play);

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D18(play->state.gfxCtx);
    Matrix_Push();

    if (D_80B7A6D4 != 0) {
        Vec3f posTemp = sLurePos;
        sLurePos = sSinkingLureBasePos;
        Fishing_DrawSinkingLure(play);
        sLurePos = posTemp;
    }

    if ((D_80B7A694 == 4) || (D_80B7A694 == 5)) {
        sLurePos = sFishingHookedFish->fishMouthPos;

        if ((D_80B7A694 == 5) && (D_80B7E0B6 == 2)) {
            Matrix_RotateY(player->actor.shape.rot.y * (M_PI / 32768), MTXMODE_NEW);
            posSrc.x = 2.0f;
            posSrc.y = 0.0f;
            posSrc.z = 0.0f;
            Matrix_MultVec3f(&posSrc, &posStep);
            sLurePos.x += posStep.x;
            sLurePos.z += posStep.z;
        }
    } else if (D_80B7A694 == 0) {
        sLurePos = sReelLinePos[LINE_SEG_COUNT - 1];
        sLureRot.x = sReelLineRot[LINE_SEG_COUNT - 2].x + M_PI;

        if ((player->actor.speedXZ == 0.0f) && (D_80B7E0B0 == 0)) {
            Math_ApproachF(&sLureRot.y, sReelLineRot[LINE_SEG_COUNT - 2].y, 0.1f, 0.2f);
        } else {
            sLureRot.y = sReelLineRot[LINE_SEG_COUNT - 2].y;
        }
    }

    if (D_80B7E0B6 != 2) {
        Matrix_Translate(sLurePos.x, sLurePos.y, sLurePos.z, MTXMODE_NEW);
        Matrix_RotateY(sLureRot.y + D_80B7E104, MTXMODE_APPLY);
        Matrix_RotateX(sLureRot.x, MTXMODE_APPLY);
        Matrix_Scale(0.0039999997f, 0.0039999997f, 0.0039999997f, MTXMODE_APPLY);
        Matrix_Translate(0.0f, 0.0f, D_80B7E108, MTXMODE_APPLY);
        Matrix_RotateZ(M_PI / 2, MTXMODE_APPLY);
        Matrix_RotateY(M_PI / 2, MTXMODE_APPLY);

        func_80093D18(play->state.gfxCtx);

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gFishingLureFloatDL);

        posSrc.x = -850.0f;
        posSrc.y = 0.0f;
        posSrc.z = 0.0f;
        Matrix_MultVec3f(&posSrc, &D_80B7E0C8);

        posSrc.x = 500.0f;
        posSrc.z = -300.0f;
        Matrix_MultVec3f(&posSrc, &hookPos[0]);
        Fishing_DrawLureHook(play, &hookPos[0], &sLureHookRefPos[0], 0);

        posSrc.x = 2100.0f;
        posSrc.z = -50.0f;
        Matrix_MultVec3f(&posSrc, &hookPos[1]);
        Fishing_DrawLureHook(play, &hookPos[1], &sLureHookRefPos[1], 1);
    }

    POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0x14);

    gDPSetCombineMode(POLY_XLU_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 55);

    if ((D_80B7A694 == 4) && ((D_80B7E124 != 0) || (D_80B7E0B6 != 2))) {
        f32 rx;
        f32 ry;
        f32 dist;
        f32 dx;
        f32 dy;
        f32 dz;

        dx = sLurePos.x - sRodTipPos.x;
        dy = sLurePos.y - sRodTipPos.y;
        dz = sLurePos.z - sRodTipPos.z;

        ry = Math_FAtan2F(dx, dz);
        dist = sqrtf(SQ(dx) + SQ(dz));
        rx = -Math_FAtan2F(dy, dist);

        dist = sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) * 0.001f;

        Matrix_Translate(sRodTipPos.x, sRodTipPos.y, sRodTipPos.z, MTXMODE_NEW);
        Matrix_RotateY(ry, MTXMODE_APPLY);
        Matrix_RotateX(rx, MTXMODE_APPLY);
        Matrix_Scale(D_80B7E14C, 1.0f, dist, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gFishingLineModelDL);
    } else {
        for (i = spB4; i < LINE_SEG_COUNT - 1; i++) {
            if ((i == LINE_SEG_COUNT - 3) && (D_80B7E0B6 == 0) && (D_80B7A694 == 3)) {
                f32 rx;
                f32 ry;
                f32 dist;
                f32 dx;
                f32 dy;
                f32 dz;

                dx = D_80B7E0C8.x - (linePos + i)->x;
                dy = D_80B7E0C8.y - (linePos + i)->y;
                dz = D_80B7E0C8.z - (linePos + i)->z;

                ry = Math_FAtan2F(dx, dz);
                dist = sqrtf(SQ(dx) + SQ(dz));
                rx = -Math_FAtan2F(dy, dist);

                dist = sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) * 0.001f;

                Matrix_Translate((linePos + i)->x, (linePos + i)->y, (linePos + i)->z, MTXMODE_NEW);
                Matrix_RotateY(ry, MTXMODE_APPLY);
                Matrix_RotateX(rx, MTXMODE_APPLY);
                Matrix_Scale(D_80B7E14C, 1.0f, dist, MTXMODE_APPLY);

                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, gFishingLineModelDL);
                break;
            }

            Matrix_Translate((linePos + i)->x, (linePos + i)->y, (linePos + i)->z, MTXMODE_NEW);
            Matrix_RotateY((lineRot + i)->y, MTXMODE_APPLY);
            Matrix_RotateX((lineRot + i)->x, MTXMODE_APPLY);
            Matrix_Scale(D_80B7E14C, 1.0f, 0.005f, MTXMODE_APPLY);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gFishingLineModelDL);
        }
    }

    Matrix_Pop();
    func_80093D84(play->state.gfxCtx);

    CLOSE_DISPS(play->state.gfxCtx);
}

static f32 sRodScales[22] = {
    1.0f,        1.0f,        1.0f,        0.9625f,     0.925f, 0.8875f,     0.85f,       0.8125f,
    0.775f,      0.73749995f, 0.7f,        0.6625f,     0.625f, 0.5875f,     0.54999995f, 0.5125f,
    0.47499996f, 0.4375f,     0.39999998f, 0.36249995f, 0.325f, 0.28749996f,
};

static f32 sRodBendRatios[22] = {
    0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.06f,   0.12f,   0.18f,   0.24f,   0.30f,   0.36f,
    0.42f, 0.48f, 0.54f, 0.60f, 0.60f, 0.5142f, 0.4285f, 0.3428f, 0.2571f, 0.1714f, 0.0857f,
};

static Vec3f sRodTipOffset = { 0.0f, 0.0f, 0.0f };

void Fishing_DrawRod(PlayState* play) {
    s16 i;
    f32 spC8;
    f32 spC4;
    f32 spC0;
    Input* input = &play->state.input[0];
    Player* player = GET_PLAYER(play);
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    if (D_80B7FDA8 != 0) {
        D_80B7FDA8--;

        Math_ApproachF(&D_80B7A6C0, 35.0f, 1.0f, 100.0f);
        Math_ApproachF(&D_80B7A6BC, -0.8f, 1.0f, 0.4f);
        Math_ApproachS(&player->actor.shape.rot.x, -4000, 2, 15000);
    } else {
        s16 target = 0;

        if ((D_80B7A694 == 4) && (D_80B7E124 != 0)) {
            target = Math_SinS(D_80B7E0AE * 25600) * 1500.0f;
        } else {
            Math_ApproachZeroF(&D_80B7A6C0, 0.1f, 10.0f);
            Math_ApproachZeroF(&D_80B7A6BC, 1.0f, 0.05f);
        }

        Math_ApproachS(&player->actor.shape.rot.x, target, 5, 1000);
    }

    if ((D_80B7A694 == 3) || (D_80B7A694 == 4)) {
        if ((input->rel.stick_x == 0) && (D_80B7A6C4 != 0)) {
            D_80B7A6B0 = 0.0f;
        }
        if ((input->rel.stick_y == 0) && (D_80B7A6C8 != 0)) {
            D_80B7A6B4 = 0.0f;
        }

        spC8 = player->unk_85C;
        Math_SmoothStepToF(&player->unk_85C, input->rel.stick_y * 0.02f, 0.3f, 5.0f, 0.0f);
        spC8 = player->unk_85C - spC8;

        spC4 = player->unk_858;
        Math_SmoothStepToF(&player->unk_858, input->rel.stick_x * 0.02f, 0.3f, 5.0f, 0.0f);
        spC4 = player->unk_858 - spC4;

        if (player->unk_858 > 1.0f) {
            player->unk_858 = 1.0f;
        }
        if (player->unk_85C > 1.0f) {
            player->unk_85C = 1.0f;
        }
        if (player->unk_858 < -1.0f) {
            player->unk_858 = -1.0f;
        }
        if (player->unk_85C < -1.0f) {
            player->unk_85C = -1.0f;
        }

        Math_ApproachF(&D_80B7A6A8, spC4 * 70.0f * -0.01f, 1.0f, D_80B7A6B0);
        Math_ApproachF(&D_80B7A6B0, 1.0f, 1.0f, 0.1f);
        Math_ApproachF(&D_80B7A6AC, spC8 * 70.0f * 0.01f, 1.0f, D_80B7A6B4);
        Math_ApproachF(&D_80B7A6B4, 1.0f, 1.0f, 0.1f);
        Math_ApproachZeroF(&D_80B7A6B8, 1.0f, 0.05f);
    } else {
        Math_ApproachZeroF(&player->unk_85C, 1.0f, 0.1f);
        Math_ApproachZeroF(&player->unk_858, 1.0f, 0.1f);
        Math_ApproachF(&D_80B7A6AC, (Math_SinS(D_80B7E0AE * 3000) * 0.025f) + -0.03f, 1.0f, 0.05f);
        Math_ApproachZeroF(&D_80B7A6A8, 1.0f, 0.05f);

        if ((D_80B7E0B4 >= 19) && (D_80B7E0B4 <= 24)) {
            Math_ApproachF(&D_80B7A6B8, 0.8f, 1.0f, 0.2f);
        } else {
            Math_ApproachF(&D_80B7A6B8, 0.0f, 1.0f, 0.4f);
        }
    }

    func_80093D18(play->state.gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gFishingRodMaterialDL);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 155, 0, 255);

    Matrix_Mult(&player->mf_9E0, MTXMODE_NEW);

    if (sLinkAge != 1) {
        Matrix_Translate(0.0f, 400.0f, 0.0f, MTXMODE_APPLY);
    } else {
        Matrix_Translate(0.0f, 230.0f, 0.0f, MTXMODE_APPLY);
    }

    if (D_80B7A694 == 5) {
        Matrix_RotateY(0.56f * M_PI, MTXMODE_APPLY);
    } else {
        Matrix_RotateY(0.41f * M_PI, MTXMODE_APPLY);
    }

    Matrix_RotateX(-M_PI / 5.0000003f, MTXMODE_APPLY);
    Matrix_RotateZ((player->unk_858 * 0.5f) + 3.0f * M_PI / 20.0f, MTXMODE_APPLY);
    Matrix_RotateX((D_80B7A6C0 + 20.0f) * 0.01f * M_PI, MTXMODE_APPLY);
    Matrix_Scale(0.70000005f, 0.70000005f, 0.70000005f, MTXMODE_APPLY);

    spC0 = (D_80B7A6BC * (((player->unk_85C - 1.0f) * -0.25f) + 0.5f)) + (D_80B7A6AC + D_80B7A6B8);

    Matrix_Translate(0.0f, 0.0f, -1300.0f, MTXMODE_APPLY);

    for (i = 0; i < 22; i++) {
        Matrix_RotateY(sRodBendRatios[i] * D_80B7A6A8 * 0.5f, MTXMODE_APPLY);
        Matrix_RotateX(sRodBendRatios[i] * spC0 * 0.5f, MTXMODE_APPLY);

        Matrix_Push();
        Matrix_Scale(sRodScales[i], sRodScales[i], 0.52f, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        if (i < 5) {
            gDPLoadTextureBlock(POLY_OPA_DISP++, gFishingRodSegmentBlackTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 8, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
        } else if ((i < 8) || ((i % 2) == 0)) {
            gDPLoadTextureBlock(POLY_OPA_DISP++, gFishingRodSegmentWhiteTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 8, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
        } else {
            gDPLoadTextureBlock(POLY_OPA_DISP++, gFishingRodSegmentStripTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 8, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
        }

        gSPDisplayList(POLY_OPA_DISP++, gFishingRodSegmentDL);

        Matrix_Pop();
        Matrix_Translate(0.0f, 0.0f, 500.0f, MTXMODE_APPLY);

        if (i == 21) {
            Matrix_MultVec3f(&sRodTipOffset, &sRodTipPos);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

static Vec3f D_80B7AF94 = { 0.0f, 0.0f, 0.0f };

void Fishing_UpdateLure(Fishing* this, PlayState* play) {
    f32 spE4;
    f32 spE0;
    s16 phi_v0;
    s16 spDC;
    f32 spD8;
    f32 spD4;
    f32 spD0;
    f32 phi_f16;
    f32 spC8;
    s16 i;
    Player* player = GET_PLAYER(play);
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f spA8;
    Vec3f sp9C;
    Vec3f sp90;
    Input* input = &play->state.input[0];
    Vec3f sp80;
    f32 sp7C;
    f32 sp78;
    f32 phi_f0;
    f32 sp70;
    Vec3f sp64;
    Vec3f sp58;
    s32 pad;

    D_80B7E0AE++;

    if (D_80B7E0B0 != 0) {
        D_80B7E0B0--;
    }

    if (D_80B7E0B2 != 0) {
        D_80B7E0B2--;
    }

    if (D_80B7E0B4 != 0) {
        D_80B7E0B4--;
    }

    if (D_80B7E122 != 0) {
        D_80B7E122--;
    }

    if (D_80B7E150 != 0) {
        D_80B7E150--;
    }

    if (D_80B7A6A4 != 0) {
        D_80B7A6A4--;
    }

    if (D_80B7E0A4 != 0) {
        D_80B7E0A4--;
    }

    if (D_80B7E114 != 0) {
        D_80B7E114--;
    }

    if (D_80B7E0AC == 1) {
        D_80B7E0AC = 2;
        D_80B7E084 = 0;
        D_80B7E082 = 0;
        D_80B7E0B6 = 0;

        if (((sLinkAge == 1) && (HIGH_SCORE(HS_FISHING) & 0x400)) ||
            ((sLinkAge != 1) && (HIGH_SCORE(HS_FISHING) & 0x800))) {
            sSinkingLureLocation = (u8)Rand_ZeroFloat(3.999f) + 1;
        }

        D_80B7E148 = 520.0f;
        D_80B7E144 = 195.0f;

        D_80B7A694 = D_80B7E0B6 = D_80B7E0AE = D_80B7E0B0 = D_80B7E0B2 = D_80B7E0B4 = D_80B7E120 = D_80B7E114 =
            D_80B7E150 = 0;
        D_80B7E104 = D_80B7E154 = D_80B7E108 = 0.0f;

        D_80B7E128 = zeroVec;

        for (i = 0; i < LINE_SEG_COUNT; i++) {
            sReelLinePos[i] = zeroVec;
            sReelLineRot[i] = zeroVec;
            sReelLineUnk[i] = zeroVec;
        }
    }

    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &sLurePos, &D_80B7AF94, &sProjectedW);

    if (D_80B7A694 == 0) {
        Math_ApproachF(&D_80B7E108, -800.0f, 1.0f, 20.0f);
    } else {
        Math_ApproachF(&D_80B7E108, 300.0f, 1.0f, 20.0f);
    }

    switch (D_80B7A694) {
        case 0:
            D_80B7FEA0 = 0;

            if (KREG(14) != 0) {
                KREG(14) = 0;
                D_80B7E0B6 = 2 - D_80B7E0B6;
                if (D_80B7E0B6 != 0) {
                    D_80B7E082 = 0;
                }
            }

            Math_ApproachF(&D_80B7E144, 195.0f, 1.0f, 1.0f);

            if (player->stateFlags1 & 0x8000000) {
                D_80B7E0B4 = 0;
                player->unk_860 = 0;
            }

            if (D_80B7E0B4 == 0) {
                if ((D_80B7E0B0 == 0) && (player->unk_860 == 1)) {
                    D_80B7E0B4 = 37;
                    Message_CloseTextbox(play);
                }
            } else {
                sLureRot.x = sReelLineRot[LINE_SEG_COUNT - 2].x + M_PI;
                sLureRot.y = sReelLineRot[LINE_SEG_COUNT - 2].y;

                if (D_80B7E0B4 == 18) {
                    D_80B7A694 = 1;
                    sLurePos = sRodTipPos;
                    Matrix_RotateY((player->actor.shape.rot.y / 32768.0f) * M_PI, MTXMODE_NEW);
                    sp90.x = 0.0f;
                    sp90.y = 0.0f;
                    sp90.z = 25.0f;
                    Matrix_MultVec3f(&sp90, &D_80B7E0E8);
                    D_80B7E0E8.y = 15.0f;
                    D_80B7E0F8.x = D_80B7E0F8.z = 0.0f;
                    D_80B7E0F8.y = -1.0f;
                    D_80B7E148 = 0.0f;
                    D_80B7E0B2 = 5;
                    D_80B7E11C = 0.5f;
                    D_80B7E118 = Rand_ZeroFloat(1.9f);
                    sFishMouthOffset.y = 500.0f;
                    func_80078914(&D_80B7AF94, NA_SE_IT_SWORD_SWING_HARD);
                }
            }
            break;

        case 1:
            spE0 = sLurePos.y;

            sLurePos.x += D_80B7E0E8.x;
            sLurePos.y += D_80B7E0E8.y;
            sLurePos.z += D_80B7E0E8.z;

            D_80B7E0E8.x += D_80B7E0F8.x;
            D_80B7E0E8.y += D_80B7E0F8.y;
            D_80B7E0E8.z += D_80B7E0F8.z;

            if (CHECK_BTN_ALL(input->cur.button, BTN_A) || (D_80B7A68C != 0)) {
                D_80B7E0E8.x *= 0.9f;
                D_80B7E0E8.z *= 0.9f;
                if (D_80B7A68C == 0) {
                    func_80078884(NA_SE_IT_FISHING_REEL_HIGH - SFX_FLAG);
                }
            }

            spD8 = sLurePos.x - sRodTipPos.x;
            spD4 = sLurePos.y - sRodTipPos.y;
            spD0 = sLurePos.z - sRodTipPos.z;

            if (D_80B7E0B2 != 0) {
                sLureRot.x = sReelLineRot[LINE_SEG_COUNT - 2].x + M_PI;
                sLureRot.y = sReelLineRot[LINE_SEG_COUNT - 2].y;
            } else {
                sLureRot.x = 0.0f;
                sLureRot.y = Math_Atan2F(spD0, spD8) + M_PI;
            }

            phi_f16 = sqrtf(SQ(spD8) + SQ(spD4) + SQ(spD0));
            if (phi_f16 > 1000.0f) {
                phi_f16 = 1000.0f;
            }
            D_80B7E144 = 200.0f - (phi_f16 * 200.0f * 0.001f);

            spC8 = SQ(sLurePos.x) + SQ(sLurePos.z);
            if (spC8 > SQ(920.0f)) {
                if ((KREG(56) != 0) || (sLurePos.y > 160.0f) || (sLurePos.x < 80.0f) || (sLurePos.x > 180.0f) ||
                    (sLurePos.z > 1350.0f) || (sLurePos.z < 1100.0f) || (sLurePos.y < 45.0f)) {
                    sp80 = this->actor.world.pos;
                    this->actor.prevPos = this->actor.world.pos = sLurePos;
                    Actor_UpdateBgCheckInfo(play, &this->actor, 15.0f, 30.0f, 30.0f, 0x43);
                    this->actor.world.pos = sp80;

                    if (this->actor.bgCheckFlags & 0x10) {
                        D_80B7E0E8.y = -0.5f;
                    }
                    if (this->actor.bgCheckFlags & 8) {
                        if (D_80B7E0E8.y > 0.0f) {
                            D_80B7E0E8.y = 0.0f;
                        }
                        D_80B7E0E8.x = D_80B7E0E8.z = 0.0f;
                    }
                } else {
                    if (func_80B6C2EC(&sLurePos) != 0) {
                        D_80B7A694 = 3;
                        D_80B7E154 = 0.0f;
                    }
                }

                spE4 = ((sqrtf(spC8) - 920.0f) * 0.11f) + WATER_SURFACE_Y(play);
                if (sLurePos.y <= spE4) {
                    sLurePos.y = spE4;
                    D_80B7E0E8.x = D_80B7E0E8.y = D_80B7E0E8.z = 0.0f;
                    D_80B7A694 = 3;
                    D_80B7E154 = 0.0;
                } else {
                    Math_ApproachF(&D_80B7E148, 0.0f, 1.0f, 0.05f);
                    func_80078914(&D_80B7AF94, NA_SE_EN_FANTOM_FLOAT - SFX_FLAG);
                }
            } else {
                spE4 = WATER_SURFACE_Y(play);

                if (sLurePos.y <= spE4) {
                    D_80B7A694 = 2;
                    D_80B7E154 = 0.0f;
                    D_80B7E0E8.x = D_80B7E0E8.z = 0.0f;

                    if (D_80B7E0B6 == 2) {
                        D_80B7E0A2 = 0;
                    } else {
                        D_80B7E0A2 = 10;
                    }

                    if ((sLurePos.y <= spE4) && (spE4 < spE0) && (spE4 == WATER_SURFACE_Y(play))) {
                        D_80B7E114 = 10;
                        func_80078914(&D_80B7AF94, NA_SE_EV_BOMB_DROP_WATER);
                        D_80B7E0F8.y = 0.0f;
                        D_80B7E0E8.y *= 0.2f;

                        for (i = 0; i < 50; i++) {
                            sp7C = Rand_ZeroFloat(1.5f) + 0.5f;
                            sp78 = Rand_ZeroFloat(6.28f);

                            sp9C.x = sinf(sp78) * sp7C;
                            sp9C.z = cosf(sp78) * sp7C;
                            sp9C.y = Rand_ZeroFloat(3.0f) + 3.0f;

                            spA8 = sLurePos;
                            spA8.x += (sp9C.x * 3.0f);
                            spA8.y = WATER_SURFACE_Y(play);
                            spA8.z += (sp9C.z * 3.0f);
                            Fishing_SpawnDustSplash(NULL, play->specialEffects, &spA8, &sp9C,
                                                    Rand_ZeroFloat(0.02f) + 0.025f);
                        }

                        spA8 = sLurePos;
                        spA8.y = WATER_SURFACE_Y(play);
                        Fishing_SpawnRipple(NULL, play->specialEffects, &spA8, 100.0f, 800.0f, 150, 90);
                    }
                } else {
                    Math_ApproachZeroF(&D_80B7E148, 1.0f, 0.05f);
                    func_80078914(&D_80B7AF94, NA_SE_EN_FANTOM_FLOAT - SFX_FLAG);
                }
            }

            sReelLinePos[LINE_SEG_COUNT - 1].x = sLurePos.x;
            sReelLinePos[LINE_SEG_COUNT - 1].y = sLurePos.y;
            sReelLinePos[LINE_SEG_COUNT - 1].z = sLurePos.z;

            D_80B7E140 = 1.0f;
            D_80B7E10C = 0.5f;
            break;

        case 2:
            if (sLurePos.y <= WATER_SURFACE_Y(play)) {
                sLurePos.y += D_80B7E0E8.y;

                Math_ApproachZeroF(&D_80B7E0E8.y, 1.0f, 1.0f);

                if (D_80B7E0B6 != 2) {
                    Math_ApproachF(&sLurePos.y, WATER_SURFACE_Y(play), 0.5f, 1.0f);
                }
            }

            Math_ApproachF(&D_80B7E148, 2.0f, 1.0f, 0.1f);

            if (D_80B7E0A2 == 0) {
                D_80B7A694 = 3;
            } else {
                D_80B7E0A2--;
            }
            break;

        case 3:
            D_80B7FEA0 = 0;

            if ((D_80B7A68C != 0) && ((SQ(sLurePos.x) + SQ(sLurePos.z)) < SQ(500.0f))) {
                D_80B7A690 = 1;
            }

            player->unk_860 = 2;

            if (D_80B7E138 < 3.0f) {
                spD0 = D_80B7E10C * Math_SinS(D_80B7E0AE * 0x1060);
                Math_ApproachF(&sLureRot.x, -M_PI / 6.0f + spD0, 0.3f, D_80B7E110);
                Math_ApproachF(&D_80B7E110, 0.5f, 1.0f, 0.02f);
                Math_ApproachZeroF(&D_80B7E10C, 1.0f, 0.02f);
            } else {
                D_80B7E110 = 0.0f;
            }

            spDC = 0x4000;
            spE4 = WATER_SURFACE_Y(play);

            spC8 = SQ(sLurePos.x) + SQ(sLurePos.z);
            if (spC8 < SQ(920.0f)) {
                if (sLurePos.y <= (spE4 + 4.0f)) {
                    sp70 = 0.0f;

                    if (D_80B7E150 == 0) {
                        if (fabsf(input->rel.stick_x) > 30.0f) {
                            sp70 = fabsf((input->rel.stick_x - D_80B7A6C4) * (1.0f / 60.0f));
                        } else if (fabsf(input->rel.stick_y) > 30.0f) {
                            sp70 = fabsf((input->rel.stick_y - D_80B7A6C8) * (1.0f / 60.0f));
                        }
                    }

                    if (sp70 > 1.0f) {
                        sp70 = 1.0f;
                    }
                    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                        sp70 = 0.5f;
                    }

                    if (D_80B7A68C != 0) {
                        if (sp70 > 0.3f) {
                            sp70 = 0.3f;
                        }
                    }

                    if ((sp70 > 0.2f) && (D_80B7E138 < 4.0f)) {
                        D_80B7E150 = 5;

                        if (sp70 > 0.8f) {
                            D_80B7E120 = 2;
                        } else {
                            D_80B7E120 = 1;
                        }

                        sp90.x = player->actor.world.pos.x - sLurePos.x;
                        sp90.z = player->actor.world.pos.z - sLurePos.z;
                        sp90.y = Math_Atan2F(sp90.z, sp90.x);

                        D_80B7E134 = (sp70 * D_80B7E140) + sp90.y;
                        D_80B7E140 = D_80B7E140 * -1.0f;
                        D_80B7E138 = fabsf(sp70) * 6.0f;
                        sLureRot.x = 0.0f;
                        D_80B7E10C = 0.5f;
                        D_80B7E144 += (fabsf(sp70) * (7.5f + (KREG(25) * 0.1f)));

                        func_800F436C(&D_80B7AF94, NA_SE_EV_LURE_MOVE_W, (sp70 * 1.999f * 0.25f) + 0.75f);

                        if (D_80B7E0B6 == 2) {
                            D_80B7E128.y = 5.0f * sp70;
                            sReelLinePos[LINE_SEG_COUNT - 1].y += D_80B7E128.y;
                            sLurePos.y += D_80B7E128.y;
                        }
                    } else if (CHECK_BTN_ALL(input->cur.button, BTN_A)) {
                        spDC = 0x500;
                        D_80B7E134 = sReelLineRot[LINE_SEG_COUNT - 2].y + M_PI;
                        sLureRot.x = 0.0f;
                        D_80B7E10C = 0.5f;
                        if (D_80B7E0B6 == 2) {
                            D_80B7E128.y = 0.2f;
                            sReelLinePos[LINE_SEG_COUNT - 1].y += D_80B7E128.y;
                            sLurePos.y += D_80B7E128.y;
                        }
                    }
                } else {
                    if (D_80B7E144 > 150.0f) {
                        sLureRot.x = sReelLineRot[LINE_SEG_COUNT - 2].x + M_PI;
                        D_80B7E134 = sReelLineRot[LINE_SEG_COUNT - 2].y + M_PI;
                        D_80B7E144 += 2.0f;
                    }
                }
            } else {
                spE4 = ((sqrtf(spC8) - 920.0f) * 0.11f) + WATER_SURFACE_Y(play);
                if (sLurePos.y <= spE4) {
                    sLurePos.y = spE4;
                    spDC = 0x500;
                    D_80B7E134 = sReelLineRot[LINE_SEG_COUNT - 2].y + M_PI;
                    sLureRot.x = 0.0f;
                    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                        D_80B7E144 += 6.0f;
                        func_80078914(&D_80B7AF94, NA_SE_PL_WALK_SAND);
                    }
                } else {
                    if (D_80B7E144 > 150.0f) {
                        sLureRot.x = sReelLineRot[LINE_SEG_COUNT - 2].x + M_PI;
                        D_80B7E134 = sReelLineRot[LINE_SEG_COUNT - 2].y + M_PI;
                        D_80B7E144 += 2.0f;
                    }
                }
            }

            Math_ApproachZeroF(&D_80B7E138, 1.0f, 0.3f);
            Math_ApproachS(&D_80B7E13C, (D_80B7E134 * 32768.0f) / M_PI, 3, spDC);

            sLureRot.y = (D_80B7E13C / 32768.0f) * M_PI;

            sp90.x = 0.0f;
            sp90.y = 0.0f;
            sp90.z = D_80B7E138;

            Matrix_RotateY(sLureRot.y, MTXMODE_NEW);

            if (D_80B7E0B6 == 2) {
                Matrix_MultVec3f(&sp90, &sp64);
                D_80B7E128.x = sp64.x;
                D_80B7E128.z = sp64.z;
                phi_f0 = 10.0f;
            } else {
                Matrix_MultVec3f(&sp90, &D_80B7E128);
                phi_f0 = 0.0f;
            }

            D_80B7E104 = 0.0f;

            if ((D_80B7E0B6 == 1) && CHECK_BTN_ALL(input->cur.button, BTN_A)) {
                D_80B7E128.y = -2.0f;

                if ((D_80B7E0AE & 1) != 0) {
                    D_80B7E104 = 0.5f;
                } else {
                    D_80B7E104 = -0.5f;
                }
            } else if (sReelLinePos[LINE_SEG_COUNT - 1].y < (WATER_SURFACE_Y(play) + phi_f0)) {
                if (D_80B7E0B6 == 2) {
                    sp58 = this->actor.world.pos;
                    this->actor.prevPos = this->actor.world.pos = sLurePos;
                    Actor_UpdateBgCheckInfo(play, &this->actor, 15.0f, 30.0f, 30.0f, 0x44);
                    this->actor.world.pos = sp58;

                    D_80B7E128.y += -0.5f;
                    if (D_80B7E128.y < -1.0f) {
                        D_80B7E128.y = -1.0f;
                    }

                    if (sLurePos.y < (this->actor.floorHeight + 5.0f)) {
                        sReelLinePos[LINE_SEG_COUNT - 1].y = sLurePos.y = this->actor.floorHeight + 5.0f;
                        D_80B7E128.y = 0.0f;
                    } else {
                        D_80B7E120 = 1;
                    }
                } else {
                    D_80B7E128.y = fabsf(sReelLinePos[LINE_SEG_COUNT - 1].y - WATER_SURFACE_Y(play)) * 0.2f;
                    if (D_80B7E128.y > 1.5f) {
                        D_80B7E128.y = 1.5f;
                    }
                }
            }

            sReelLinePos[LINE_SEG_COUNT - 1].x += D_80B7E128.x;
            sReelLinePos[LINE_SEG_COUNT - 1].y += D_80B7E128.y;
            sReelLinePos[LINE_SEG_COUNT - 1].z += D_80B7E128.z;

            if (sReelLinePos[LINE_SEG_COUNT - 1].y > (spE4 + 6.0f)) {
                sReelLinePos[LINE_SEG_COUNT - 1].y -= 5.0f;
            }

            D_80B7E0E8.x = D_80B7E0E8.y = D_80B7E0E8.z = D_80B7E0F8.y = 0.0f;

            if (CHECK_BTN_ALL(input->cur.button, BTN_A)) {
                if (CHECK_BTN_ALL(input->cur.button, BTN_R)) {
                    D_80B7E144 += 1.5f;
                    func_80078884(NA_SE_IT_FISHING_REEL_HIGH - SFX_FLAG);
                    Math_ApproachF(&D_80B7E154, 1000.0f, 1.0f, 2.0f);
                } else {
                    D_80B7E144 += D_80B7E11C;
                    func_80078884(NA_SE_IT_FISHING_REEL_SLOW - SFX_FLAG);
                    Math_ApproachF(&D_80B7E154, 1000.0f, 1.0f, 0.2f);
                }

                if (sReelLinePos[LINE_SEG_COUNT - 1].y > (WATER_SURFACE_Y(play) + 4.0f)) {
                    Math_ApproachF(&D_80B7E148, 3.0f, 1.0f, 0.2f);
                } else {
                    Math_ApproachF(&D_80B7E148, 1.0f, 1.0f, 0.2f);
                }
            } else {
                Math_ApproachF(&D_80B7E148, 2.0f, 1.0f, 0.2f);
            }

            Math_ApproachF(&sLurePos.x, sReelLinePos[LINE_SEG_COUNT - 1].x, 1.0f, D_80B7E154);
            Math_ApproachF(&sLurePos.y, sReelLinePos[LINE_SEG_COUNT - 1].y, 1.0f, D_80B7E154);
            Math_ApproachF(&sLurePos.z, sReelLinePos[LINE_SEG_COUNT - 1].z, 1.0f, D_80B7E154);

            if (D_80B7E138 > 1.0f) {
                Math_ApproachF(&D_80B7E154, 1000.0f, 1.0f, 1.0f);
            }

            Math_ApproachF(&D_80B7E154, 1000.0f, 1.0f, 0.1f);

            if (D_80B7E144 >= 195.0f) {
                D_80B7E144 = 195.0f;
                D_80B7A694 = 0;
                D_80B7E148 = 520.0f;
                D_80B7A6CC = 3;
            }

            if ((sLurePos.y <= (WATER_SURFACE_Y(play) + 4.0f)) &&
                (sLurePos.y >= (WATER_SURFACE_Y(play) - 4.0f))) {

                phi_v0 = 63;
                if (CHECK_BTN_ALL(input->cur.button, BTN_A) || (D_80B7E138 > 1.0f)) {
                    phi_v0 = 1;
                }

                if ((D_80B7E0AE & phi_v0) == 0) {
                    spA8 = sLurePos;
                    spA8.y = WATER_SURFACE_Y(play);
                    Fishing_SpawnRipple(NULL, play->specialEffects, &spA8, 30.0f, 300.0f, 150, 90);
                }
            }
            break;

        case 4:
            if (this->unk_157 != 0) {
                this->unk_157--;
                D_80B7E144 += D_80B7E11C;
            }

            if (CHECK_BTN_ALL(input->cur.button, BTN_A)) {
                if ((SQ(sLurePos.x) + SQ(sLurePos.z)) > SQ(920.0f)) {
                    D_80B7E144 += (1.0f + (KREG(65) * 0.1f));
                } else {
                    D_80B7E144 += D_80B7E11C;
                }
                func_80078884(NA_SE_IT_FISHING_REEL_SLOW - SFX_FLAG);
            }

            if ((D_80B7E0AE & 0x1F) == 0) {
                if ((D_80B7E124 != 0) || (D_80B7E0B6 != 2)) {
                    D_80B7A6A4 = 5;
                }
            }

            Math_ApproachF(&D_80B7E148, 0.0f, 1.0f, 0.2f);
            break;

        case 5:
            D_80B7E14C = 0.0005000001f;
            sReelLinePos[LINE_SEG_COUNT - 1].x = sLurePos.x;
            sReelLinePos[LINE_SEG_COUNT - 1].y = sLurePos.y;
            sReelLinePos[LINE_SEG_COUNT - 1].z = sLurePos.z;
            D_80B7E148 = 2.0f;
            break;
    }
}

s32 func_80B70A2C(Fishing* this, PlayState* play, u8 ignorePosCheck) {
    s16 i;
    s16 count;
    f32 scale;
    Vec3f pos;
    Vec3f vel;
    f32 speedXZ;
    f32 angle;

    if ((this->actor.world.pos.y < (WATER_SURFACE_Y(play) - 10.0f)) && !ignorePosCheck) {
        return false;
    }

    if (this->unk_1AC >= 40.0f) {
        count = 40;
        scale = 1.2f;
    } else {
        count = 30;
        scale = 1.0f;
    }

    for (i = 0; i < count; i++) {
        speedXZ = (Rand_ZeroFloat(1.5f) + 0.5f) * scale;
        angle = Rand_ZeroFloat(6.28f);

        vel.x = sinf(angle) * speedXZ;
        vel.z = cosf(angle) * speedXZ;
        vel.y = (Rand_ZeroFloat(3.0f) + 3.0f) * scale;

        pos = this->actor.world.pos;
        pos.x += vel.x * 3.0f;
        pos.y = WATER_SURFACE_Y(play);
        pos.z += vel.z * 3.0f;

        Fishing_SpawnDustSplash(&this->actor.projectedPos, play->specialEffects, &pos, &vel,
                                (Rand_ZeroFloat(0.02f) + 0.025f) * scale);
    }

    pos = this->actor.world.pos;
    pos.y = WATER_SURFACE_Y(play);

    Fishing_SpawnRipple(&this->actor.projectedPos, play->specialEffects, &pos, 100.0f, 800.0f, 150, 90);

    this->unk_151 = 30;

    return true;
}

void func_80B70CF0(Fishing* this, PlayState* play) {
    s16 count;
    s16 i;
    f32 scale;
    Vec3f pos;
    Vec3f vel;
    f32 speedXZ;
    f32 angle;

    if (this->unk_1AC >= 45.0f) {
        count = 30;
        scale = 0.5f;
    } else {
        count = 20;
        scale = 0.3f;
    }

    for (i = 0; i < count; i++) {
        speedXZ = (Rand_ZeroFloat(1.5f) + 0.5f) * scale;
        angle = Rand_ZeroFloat(6.28f);

        vel.x = sinf(angle) * speedXZ;
        vel.z = cosf(angle) * speedXZ;
        vel.y = Rand_ZeroFloat(2.0f) + 2.0f;

        pos = this->actor.world.pos;
        pos.x += (vel.x * 3.0f);
        pos.y += (vel.y * 3.0f);
        pos.z += (vel.z * 3.0f);

        Fishing_SpawnDustSplash(&this->actor.projectedPos, play->specialEffects, &pos, &vel,
                                (Rand_ZeroFloat(0.02f) + 0.025f) * scale);
    }
}

void func_80B70ED4(Fishing* this, Input* input) {
    Vec3f sp34;
    Vec3f sp28;
    f32 sp24;

    sp34.x = sLurePos.x - this->actor.world.pos.x;
    sp34.y = sLurePos.y - this->actor.world.pos.y;
    sp34.z = sLurePos.z - this->actor.world.pos.z;

    sp24 = SQ(sp34.x) + SQ(sp34.y) + SQ(sp34.z);

    if ((D_80B7A694 == 3) && (this->unk_1A2 == 0) && (D_80B7A68C == 0)) {
        Matrix_RotateY((-this->actor.shape.rot.y / 32768.0f) * M_PI, MTXMODE_NEW);
        Matrix_MultVec3f(&sp34, &sp28);

        if ((sp28.z > 0.0f) || (this->unk_1AC < 40.0f)) {
            if ((this->unk_158 == 7) && (sp24 < SQ(200.0f))) {
                this->unk_158 = 4;
                this->unk_1B4 = sLurePos;
                this->unk_1B0 = 28672.0f;
                this->unk_188 = 5.0f;
            } else {
                if ((CHECK_BTN_ALL(input->cur.button, BTN_A) || (D_80B7E138 > 1.0f)) && (sp24 < SQ(120.0f))) {
                    this->unk_158 = 2;
                    this->unk_15E = 0;
                    this->unk_17A[0] = 0;
                    this->unk_17A[2] = (s16)Rand_ZeroFloat(100.0f) + 100;
                    this->unk_1A8 = sFishInits[this->actor.params - 100].unk_0C;
                    this->unk_1B0 = 0.0f;
                }

                if ((this->unk_17A[1] == 0) && (sp24 < SQ(70.0f))) {
                    this->unk_158 = 2;
                    this->unk_15E = 0;
                    this->unk_17A[0] = 0;
                    this->unk_17A[2] = (s16)Rand_ZeroFloat(100.0f) + 100;
                    this->unk_1A8 = sFishInits[this->actor.params - 100].unk_0C;
                    this->unk_1B0 = 0.0f;
                }
            }
        }
    } else if ((D_80B7A694 == 4) && (D_80B7E124 != 0) && (sp24 < SQ(100.0f)) && (this->unk_158 >= 10)) {
        this->unk_15A = 0;
        this->unk_158 = 1;
        this->unk_1A4 = 1000;
        this->unk_1A2 = 100;
        this->unk_17A[1] = 50;
    }

    if ((D_80B7E0B6 != 2) && (D_80B7E114 != 0) && (this->unk_1AC > 60.0f) && (sp24 < SQ(30.0f)) &&
        (this->unk_158 >= 10)) {
        this->unk_15A = 0;
        this->unk_158 = 1;
        this->unk_1A4 = 1000;
        this->unk_1A2 = 100;
        this->unk_17A[1] = 50;
    }
}

void func_80B71278(Fishing* this, u8 arg1) {
    s16 sfxId;
    u8 temp;

    if (this->unk_150 == 0) {
        temp = this->unk_1AC;
    } else {
        temp = 2.0f * this->unk_1AC;
    }

    if (arg1 == 0) {
        if (temp >= 50) {
            sfxId = NA_SE_EV_DIVE_INTO_WATER;
        } else if (temp >= 40) {
            sfxId = NA_SE_EV_BOMB_DROP_WATER;
        } else {
            sfxId = NA_SE_EV_BOMB_DROP_WATER;
        }
    } else {
        if (temp >= 50) {
            sfxId = NA_SE_EV_JUMP_OUT_WATER;
        } else if (temp >= 40) {
            sfxId = NA_SE_EV_OUT_OF_WATER;
        } else {
            sfxId = NA_SE_EV_OUT_OF_WATER;
        }
    }

    Audio_PlayActorSound2(&this->actor, sfxId);
}

void Fishing_HandleAquariumDialog(Fishing* this, PlayState* play) {
    if (sLinkAge == 1) {
        if ((HIGH_SCORE(HS_FISHING) & 0x7F) != 0) {
            if (HIGH_SCORE(HS_FISHING) & 0x80) {
                this->actor.textId = 0x40B1;
            } else {
                this->actor.textId = 0x4089;
            }
        } else {
            this->actor.textId = 0x40AE;
        }
    } else {
        if ((HIGH_SCORE(HS_FISHING) & 0x7F000000) != 0) {
            if (HIGH_SCORE(HS_FISHING) & 0x80000000) {
                this->actor.textId = 0x40B1;
            } else {
                this->actor.textId = 0x4089;
            }
        } else {
            this->actor.textId = 0x40AE;
        }
    }

    if (this->unk_1D3 == 0) {
        if (this->unk_1D4 == 0) {
            this->actor.flags |= ACTOR_FLAG_0;

            if (Actor_ProcessTalkRequest(&this->actor, play)) {
                D_80B7A678 = D_80B7E078;
                this->unk_1D3 = 1;
            } else {
                func_8002F2F4(&this->actor, play);
            }
        } else {
            this->unk_1D4--;
            this->actor.flags &= ~ACTOR_FLAG_0;
        }
    } else if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->unk_1D3 = 0;
        this->unk_1D4 = 20;
    }
}

f32 Fishing_GetMinimumRequiredScore() {
    int32_t weight;
    // RANDOTODO: update the enhancement sliders to not allow
    // values above rando fish weight values when rando'd
    if(sLinkAge == 1) {
        weight = CVar_GetS32("gChildMinimumWeightFish", 10);
    } else {
        weight = CVar_GetS32("gAdultMinimumWeightFish", 13);     
    }

    return sqrt(((f32)weight - 0.5f) / 0.0036f);
}

bool getInstantFish() {
    return CVar_GetS32("gInstantFishing", 0);
}

bool getGuaranteeBite() {
    return CVar_GetS32("gGuaranteeFishingBite", 0);
}

bool getFishNeverEscape() {
    return CVar_GetS32("gFishNeverEscape", 0);
}

void Fishing_UpdateFish(Actor* thisx, PlayState* play2) {
    s16 i;
    s16 sp134 = 10;
    f32 sp130;
    f32 sp12C;
    f32 sp128;
    f32 sp124;
    f32 multiplier;
    f32 sp11C;
    f32 sp118;
    Vec3f sp10C;
    Vec3f sp100;
    s16 spFE;
    s16 spFC;
    s16 spFA;
    s16 phi_v0;
    s16 spF6;
    s16 spF4;
    s16 spF2;
    s16 spF0;
    s16 spEE;
    Fishing* this = (Fishing*)thisx;
    PlayState* play = play2;
    Player* player = GET_PLAYER(play);
    Input* input = &play->state.input[0];
    f32 spD8;
    f32 phi_f0;
    f32 phi_f2;
    Vec3f spC4;
    Vec3f spB8;
    u8 phi_v0_2;
    f32 temp_f0;
    f32 temp;
    s32 pad;
    f32 spA4;
    u16 spA2;
    u8 phi_a1;

    this->actor.uncullZoneForward = 700.0f;
    this->actor.uncullZoneScale = 50.0f;

    if (this->unk_150 == 0) {
        sp118 = (player->actor.speedXZ * 0.15f) + 0.25f;
    } else {
        sp118 = (player->actor.speedXZ * 0.3f) + 0.25f;
    }

    if ((D_80B7E0B0 != 0) || (sCameraId != 0) || ((player->actor.world.pos.z > 1150.0f) && (this->unk_158 != 100))) {
        this->actor.flags &= ~ACTOR_FLAG_0;
    } else {
        this->actor.flags |= ACTOR_FLAG_0;
        if (D_80B7A694 != 0) {
            if (D_80B7E0B2 == 0) {
                this->actor.focus.pos = sLurePos;
            } else if (D_80B7E0B2 == 1) {
                D_80B7A6CC = 1;
                D_80B7FED0 = 0.0f;
                D_80B7E088 = 2;
            }
        }
        this->actor.focus.pos = this->actor.world.pos;
    }

    this->unk_15C++;

    for (i = 0; i < 4; i++) {
        if (this->unk_17A[i] != 0) {
            this->unk_17A[i]--;
        }
    }

    if (this->unk_1A4 != 0) {
        this->unk_1A4--;
    }

    if (this->unk_1A2 != 0) {
        this->unk_1A2--;
    }

    if (this->unk_1A0 != 0) {
        this->unk_1A0--;
    }

    if (this->unk_151 != 0) {
        this->unk_151--;
    }

    Math_ApproachF(&this->unk_198, this->unk_190, 1.0f, 0.2f);

    if (this->unk_158 == 6) {
        Math_ApproachF(&this->unk_19C, this->unk_194, 0.2f, 200.0f);
    } else {
        phi_f0 = 1.0f;
        phi_f2 = 1.0f;
        if (this->actor.world.pos.y > WATER_SURFACE_Y(play)) {
            phi_f0 = (KREG(64) * 0.1f) + 1.5f;
            phi_f2 = 3.0f;
        }
        Math_ApproachF(&this->unk_19C, this->unk_194 * phi_f0, 1.0f, 500.0f * phi_f2);
    }

    Math_ApproachS(&this->unk_170, 0, 5, 0x1F4);

    if (this->unk_150 == 0) {
        Actor_SetScale(&this->actor, this->unk_1AC * 15.0f * 0.00001f);

        this->unk_18C += this->unk_198;

        temp = cosf(this->unk_18C);
        this->unk_16C = this->unk_16E + (s16)(temp * this->unk_19C);

        temp = cosf(this->unk_18C + -1.2f);
        this->unk_176 = this->unk_16E + (s16)(temp * this->unk_19C * 1.6f);
    } else {
        Actor_SetScale(&this->actor, this->unk_1AC * 65.0f * 0.000001f);

        this->actor.scale.x = this->actor.scale.z * 1.1f;
        this->actor.scale.y = this->actor.scale.z * 1.1f;

        this->unk_18C += this->unk_198 * 0.8f;

        for (i = 0; i < 3; i++) {
            temp = cosf(this->unk_18C + (i * 2.1f));
            this->unk_1CC[i] = this->unk_16E + (s16)(temp * this->unk_19C * 2.0f);
        }

        temp = cosf(this->unk_18C + 0.4f);
        this->unk_16C = (this->unk_19C * temp * 2.0f) * 0.6f;
    }

    sp130 = this->unk_1B4.x - this->actor.world.pos.x;
    sp12C = this->unk_1B4.y - this->actor.world.pos.y;
    sp128 = this->unk_1B4.z - this->actor.world.pos.z;

    spFC = Math_Atan2S(sp128, sp130);
    sp124 = sqrtf(SQ(sp130) + SQ(sp128));

    spFE = Math_Atan2S(sp124, sp12C);
    sp124 = sqrtf(SQ(sp130) + SQ(sp128) + SQ(sp12C));

    if ((this->unk_1A0 != 0) && (this->unk_158 != 2) && (this->unk_158 != 3) && (this->unk_158 != 4)) {
        if ((this->unk_15C & 0x40) != 0) {
            spFC += 0x4000;
        } else {
            spFC -= 0x4000;
        }
        if (((this->unk_15C + 0x20) & 0x40) != 0) {
            spFE += 0x2000;
        } else {
            spFE -= 0x2000;
        }
    }

    switch (this->unk_158) {
        case 100:
            Fishing_HandleAquariumDialog(this, play);

            this->actor.uncullZoneForward = 500.0f;
            this->actor.uncullZoneScale = 300.0f;

            Lights_PointNoGlowSetInfo(&this->lightInfo, (s16)this->actor.world.pos.x,
                                      (s16)this->actor.world.pos.y + 20.0f, (s16)this->actor.world.pos.z - 50.0f, 255,
                                      255, 255, 255);

            this->unk_1AC = D_80B7E078;
            sp100.y = (f32)Math_SinS(play->gameplayFrames * 300) * 1;
            sp100.z = (f32)Math_SinS(play->gameplayFrames * 230) * 2;
            this->actor.world.pos.x = 130.0f;
            this->actor.world.pos.y = 55.0f + sp100.y;
            this->actor.world.pos.z = 1300.0f + sp100.z;
            this->actor.shape.rot.y = -0x8000;

            if ((this->actor.projectedPos.z < 200.0f) && (this->actor.projectedPos.z > 0.0f)) {
                spC4.x = Rand_CenteredFloat(5.0f) + 130.0f;
                spC4.y = 40.0f;
                spC4.z = Rand_CenteredFloat(5.0f) + 1280.0f;
                Fishing_SpawnBubble(NULL, play->specialEffects, &spC4, Rand_ZeroFloat(0.02f) + 0.03f, 1);
            }

            Math_ApproachS(&this->unk_172, (Math_SinS(this->unk_15C * 0x800) * 2500.0f) + 2500.0f, 2, 0x7D0);
            Math_ApproachS(&this->unk_174, Math_SinS(this->unk_15C * 0xA00) * 1500.0f, 2, 0x7D0);

            this->unk_190 = 0.3f;
            this->unk_194 = 1000.0f / 3.0f;
            return;

        case 10:
            this->unk_1B4 = this->actor.home.pos;

            Math_ApproachF(&this->actor.speedXZ, 2.0f, 1.0f, 0.5f);
            Math_ApproachF(&this->unk_1B0, 4096.0f, 1.0f, 256.0f);

            if (sp124 < 40.0f) {
                this->unk_158 = 11;
                this->unk_190 = 0.4f;
                this->unk_194 = 500.0f;
            }

            func_80B70ED4(this, input);

            if (this->actor.xzDistToPlayer < (250.0f * sp118)) {
                this->unk_15A = this->unk_158 = 0;
                this->unk_1A4 = 1000;
                this->unk_1A2 = 200;
                this->unk_17A[1] = 50;
            }
            break;

        case 11:
            this->unk_1B4 = this->actor.home.pos;

            Math_ApproachF(&this->actor.speedXZ, 0.0f, 1.0f, 0.05f);
            Math_ApproachF(&this->unk_1B0, 0.0f, 1.0f, 256.0f);

            if (sp124 >= 40.0f) {
                this->unk_158 = 10;
                this->unk_190 = 1.0f;
                this->unk_194 = 2000.0f;
            }
            func_80B70ED4(this, input);

            if (this->actor.xzDistToPlayer < (250.0f * sp118)) {
                this->unk_15A = this->unk_158 = 0;
                this->unk_1A4 = 1000;
                this->unk_1A2 = 200;
                this->unk_17A[1] = 50;
            }

            if (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                if ((gSaveContext.dayTime >= 0xC000) && (gSaveContext.dayTime <= 0xC01B)) {
                    this->unk_158 = 7;
                    this->unk_17A[3] = (s16)Rand_ZeroFloat(150.0f) + 200;
                }
                if ((gSaveContext.dayTime >= 0x3AAA) && (gSaveContext.dayTime <= 0x3AC5)) {
                    this->unk_158 = 7;
                    this->unk_17A[3] = (s16)Rand_ZeroFloat(150.0f) + 200;
                }
            }

            if (KREG(15) != 0) {
                KREG(15) = 0;
                this->unk_158 = 7;
                this->unk_17A[3] = (s16)Rand_ZeroFloat(150.0f) + 2000;
            }
            break;

        case 0:
            Math_ApproachF(&this->actor.speedXZ, 1.0f, 1.0f, 0.05f);
            Math_ApproachF(&this->unk_1B0, 0.0f, 1.0f, 256.0f);

            if (this->unk_17A[0] == 0) {
                if (this->unk_1A4 == 0) {
                    this->unk_158 = this->unk_15A = 10;
                } else {
                    this->unk_158 = 1;
                    this->unk_17A[0] = (s16)Rand_ZeroFloat(30.0f) + 10;
                    this->unk_1B4.x = Rand_CenteredFloat(300.0f);
                    this->unk_1B4.y = (WATER_SURFACE_Y(play) - 50.0f) - Rand_ZeroFloat(50.0f);
                    this->unk_1B4.z = Rand_CenteredFloat(300.0f);
                    this->unk_190 = 1.0f;
                    this->unk_194 = 2000.0f;
                }
            }

            if (D_80B7E0B6 == 2) {
                func_80B70ED4(this, input);
            } else {
                this->actor.flags &= ~ACTOR_FLAG_0;
            }
            break;

        case 1:
            if (this->unk_150 == 1) {
                this->unk_158 = -1;
                this->unk_1A4 = 20000;
                this->unk_1A2 = 20000;
                this->unk_1B4.x = 0.0f;
                this->unk_1B4.y = -140.0f;
                this->unk_1B4.z = 0.0f;
            } else {
                Math_ApproachF(&this->unk_1B0, 4096.0f, 1.0f, 256.0f);

                if ((this->actor.xzDistToPlayer < (250.0f * sp118)) || (this->unk_17A[1] != 0)) {
                    Math_ApproachF(&this->unk_1B0, 8192.0f, 1.0f, 768.0f);
                    Math_ApproachF(&this->actor.speedXZ, 4.2f, 1.0f, 0.75);
                    this->unk_190 = 1.2f;
                    this->unk_194 = 4000.0f;
                    this->unk_17A[0] = 20;
                } else {
                    this->unk_190 = 1.0f;
                    this->unk_194 = 2000.0f;
                    Math_ApproachF(&this->actor.speedXZ, 1.5f, 1.0f, 0.1f);
                }

                if ((this->unk_17A[0] == 0) || (sp124 < 50.0f)) {
                    this->unk_158 = 0;
                    this->unk_17A[0] = (s16)Rand_ZeroFloat(30.0f) + 3;
                    this->unk_190 = 1.0f;
                    this->unk_194 = 500.0f;
                }

                if (D_80B7E0B6 == 2) {
                    func_80B70ED4(this, input);
                } else {
                    this->actor.flags &= ~ACTOR_FLAG_0;
                }
            }
            break;

        case -1:
            Math_ApproachS(&this->unk_166, 0, 0x14, 0x20);

            if ((this->actor.xzDistToPlayer < (250.0f * sp118)) || (this->unk_17A[1] != 0)) {
                Math_ApproachF(&this->actor.speedXZ, 3.0f, 1.0f, 0.75);
                this->unk_190 = 1.0f;
                this->unk_17A[0] = 20;
                this->unk_194 = 4000.0f;
                Math_ApproachF(&this->unk_1B0, 4096.0f, 1.0f, 256.0f);

                if ((play->gameplayFrames % 32) == 0) {
                    this->unk_1B4.x = Rand_CenteredFloat(600.0f);
                    this->unk_1B4.z = Rand_CenteredFloat(600.0f);
                    this->unk_1B4.y = -120.0f;
                }
            } else if (sp124 > 50.0f) {
                this->unk_190 = 0.8f;
                this->unk_194 = 1500.0f;
                Math_ApproachF(&this->actor.speedXZ, 1.0f, 1.0f, 0.1f);
                Math_ApproachF(&this->unk_1B0, 2048.0f, 1.0f, 128.0f);
            } else {
                this->unk_190 = 0.4f;
                this->unk_194 = 500.0f;
                Math_ApproachZeroF(&this->actor.speedXZ, 1.0f, 0.02f);
                Math_ApproachF(&this->unk_1B0, 0.0f, 1.0f, 256.0f);
            }

            if (this->unk_1A4 == 0) {
                this->unk_158 = 10;
                this->unk_15A = 10;
            } else if ((KREG(2) != 0) || (((this->unk_1A4 & 0x7FF) == 0) && (this->unk_1A4 < 15000))) {
                KREG(2) = 0;
                this->unk_158 = -2;
                this->actor.world.rot.x = this->actor.shape.rot.x = 0;
                this->unk_1B4.y = WATER_SURFACE_Y(play) + 10.0f;
                this->unk_1B4.x = Rand_ZeroFloat(50.0f);
                this->unk_1B4.z = Rand_ZeroFloat(50.0f);
            }

            this->actor.flags &= ~ACTOR_FLAG_0;
            break;

        case -2:
            if ((this->actor.xzDistToPlayer < (250.0f * sp118)) || (this->unk_17A[1] != 0)) {
                this->unk_158 = -1;
                this->unk_1B4.y = -120.0f;
            } else {
                this->unk_190 = 0.6f;
                this->unk_194 = 1000.0f;

                Math_ApproachS(&this->unk_166, -0x1000, 0x14, 0x100);

                if (this->actor.world.pos.y < (WATER_SURFACE_Y(play) - 20.0f)) {
                    Math_ApproachF(&this->actor.speedXZ, 0.5f, 1.0f, 0.1f);
                } else {
                    Math_ApproachZeroF(&this->actor.speedXZ, 1.0f, 0.01f);

                    if ((this->actor.speedXZ == 0.0f) ||
                        (this->actor.world.pos.y > (WATER_SURFACE_Y(play) - 5.0f))) {
                        this->unk_1B4.x = Rand_ZeroFloat(300.0f);
                        this->unk_1B4.z = Rand_ZeroFloat(300.0f);
                        this->unk_1B4.y = this->actor.floorHeight + 10.0f;
                        this->unk_158 = -25;
                        this->unk_1B0 = 0.0f;

                        spB8 = this->fishMouthPos;
                        spB8.y = WATER_SURFACE_Y(play);
                        Fishing_SpawnRipple(&this->actor.projectedPos, play->specialEffects, &spB8, 10.0f, 300.0f,
                                            150, 90);
                        Fishing_SpawnRipple(&this->actor.projectedPos, play->specialEffects, &spB8, 30.0f, 400.0f,
                                            150, 90);

                        Audio_PlayActorSound2(&this->actor, NA_SE_PL_CATCH_BOOMERANG);
                        break;
                    }
                }

                Math_ApproachF(&this->unk_1B0, 2048.0f, 1.0f, 128.0f);
                this->actor.flags &= ~ACTOR_FLAG_0;
            }
            break;

        case -25:
            if ((this->actor.xzDistToPlayer < (250.0f * sp118)) || (this->unk_17A[1] != 0)) {
                this->unk_158 = -1;
                this->unk_1B4.y = -120.0f;
            } else {
                Math_ApproachS(&this->unk_166, 0x1000, 0x14, 0x6A);

                if (sp124 > 40.0f) {
                    this->unk_190 = 0.7f;
                    this->unk_194 = 1200.0f;
                    Math_ApproachF(&this->actor.speedXZ, 0.5f, 1.0f, 0.01f);
                    Math_ApproachF(&this->unk_1B0, 2048.0f, 1.0f, 128.0f);
                } else {
                    this->unk_158 = -1;
                }
            }
            break;

        case 2:
            if (((this->actor.params + D_80B7E118) & 1) != 0) {
                sp10C.x = 10.0f;
            } else {
                sp10C.x = -10.0f;
            }
            sp10C.y = 0.0f;
            sp10C.z = 0.0f;
            Matrix_RotateY(sLureRot.y, MTXMODE_NEW);
            Matrix_MultVec3f(&sp10C, &sp100);

            this->unk_1B4.x = sLurePos.x + sp100.x;
            this->unk_1B4.z = sLurePos.z + sp100.z;

            if (D_80B7E0B6 == 2) {
                this->unk_1B4.y = sLurePos.y;
            } else if (this->unk_150 == 0) {
                this->unk_1B4.y = sLurePos.y - 15.0f;
            } else {
                this->unk_1B4.y = sLurePos.y - 5.0f;
            }

            if (this->unk_1B4.y <= this->actor.floorHeight) {
                this->unk_1B4.y = this->actor.floorHeight + 3.0f;
            }

            if ((D_80B7E0B6 != 2) && (this->unk_1B4.y < this->actor.world.pos.y)) {
                Math_ApproachF(&this->actor.world.pos.y, this->unk_1B4.y, 0.1f,
                               (this->actor.world.pos.y - this->unk_1B4.y) * 0.1f);
            }

            Math_ApproachF(&this->unk_1B0, 8192.0f, 1.0f, (KREG(16) * 128) + 384.0f);
            if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                this->unk_1A8 += 0.005f;
            }

            if (D_80B7E120 != 0) {
                if (D_80B7E120 == 1) {
                    this->unk_1A8 += 0.01f;
                } else {
                    this->unk_1A8 += 0.05f;
                }
                D_80B7E120 = 0;
            }

            if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                this->unk_1A8 += 0.008f;
            }

            if (sp124 < ((this->unk_1AC * 0.5f) + 20.0f)) {
                if (this->unk_15E == 0) {
                    this->unk_190 = 1.0f;
                    this->unk_194 = 500.0f;
                    this->unk_17A[0] = (s16)Rand_ZeroFloat(10.0f) + 2;
                }
                Math_ApproachF(&this->actor.speedXZ, -0.2f, 1.0f, 0.1f);
                this->unk_15E = 1;
            } else {
                if (this->unk_15E != 0) {
                    this->unk_190 = 1.0f;
                    this->unk_1B0 = 0.0f;
                    this->unk_194 = 3000.0f;
                }
                Math_ApproachF(&this->actor.speedXZ, 3.0f, 1.0f, 0.15f);
                this->unk_15E = 0;
            }

            if (this->unk_1AC >= 60.0f) {
                multiplier = 0.3f;
            } else if (this->unk_1AC >= 45.0f) {
                multiplier = 0.6f;
            } else {
                multiplier = 1.0f;
            }

            if ((gSaveContext.dayTime >= 0xB555) && (gSaveContext.dayTime <= 0xCAAA)) {
                multiplier *= 1.75f;
            } else if ((gSaveContext.dayTime >= 0x3555) && (gSaveContext.dayTime <= 0x4AAA)) {
                multiplier *= 1.5f;
            } else if (D_80B7E076 != 0) {
                multiplier *= 1.5f;
            } else if ((u8)D_80B7A650 != 0) {
                multiplier *= 3.0f;
            }

            sp11C = 0.03f * multiplier;
            if (D_80B7E0B6 == 2) {
                sp11C *= 5.0f;
            }
            if (getGuaranteeBite() == 1 ||
                ((this->unk_17A[0] == 1) || (Rand_ZeroOne() < sp11C)) &&
                ((Rand_ZeroOne() < (this->unk_1A8 * multiplier)) || ((this->unk_150 + 1) == KREG(69)))) {
                if (this->unk_150 == 0) {
                    this->unk_158 = 3;
                    this->unk_190 = 1.2f;
                    this->unk_194 = 5000.0f;
                    this->unk_17A[0] = Rand_ZeroFloat(10.0f);
                } else {
                    this->unk_158 = -3;
                    this->unk_190 = 1.0f;
                    this->unk_194 = 3000.0f;
                    this->unk_17A[0] = 40;
                }
                if (D_80B7E0B6 == 2) {
                    this->unk_188 = Rand_ZeroFloat(1.5f) + 3.0f;
                } else {
                    this->unk_188 = Rand_ZeroFloat(1.5f) + 4.5f;
                }
            }

            if ((D_80B7A694 != 3) || (this->unk_17A[2] == 0) ||
                (sqrtf(SQ(this->actor.world.pos.x) + SQ(this->actor.world.pos.z)) > 800.0f)) {
                this->unk_158 = this->unk_15A;
                this->unk_17A[1] = (s16)Rand_ZeroFloat(30.0f) + 50;
                this->unk_17A[0] = (s16)Rand_ZeroFloat(10.0f) + 5;
                this->unk_190 = 1.0f;
                this->unk_1B0 = 0.0f;
                this->unk_194 = 2000.0f;
            }

            if (this->actor.xzDistToPlayer < (100.0f * sp118)) {
                this->unk_15A = this->unk_158 = 0;
                this->unk_1A4 = 1000;
                this->unk_1A2 = 200;
                this->unk_17A[1] = 50;
            }
            break;

        case 3:
            this->unk_151 = 6;
            sp134 = 2;

            if ((((s16)player->actor.world.pos.x + D_80B7E118) & 1) != 0) {
                sp10C.x = 30.0f;
            } else {
                sp10C.x = -30.0f;
            }
            sp10C.y = 0.0f;
            sp10C.z = 30.0f;

            Matrix_RotateY(sLureRot.y, MTXMODE_NEW);
            Matrix_MultVec3f(&sp10C, &sp100);

            this->unk_1B4.x = sLurePos.x + sp100.x;
            this->unk_1B4.z = sLurePos.z + sp100.z;
            this->unk_1B4.y = sLurePos.y - 10.0f;
            this->unk_1B0 = 4096.0f;
            Math_ApproachF(&this->actor.speedXZ, this->unk_188 * 0.8f, 1.0f, 1.0f);

            if ((D_80B7A694 != 3) || (sLurePos.y > (WATER_SURFACE_Y(play) + 5.0f)) ||
                (sqrtf(SQ(sLurePos.x) + SQ(sLurePos.z)) > 800.0f)) {
                this->unk_158 = this->unk_15A;
                this->unk_17A[0] = 0;
                this->unk_190 = 1.0f;
                this->unk_194 = 2000.0f;
            } else if ((this->unk_17A[0] == 0) || (sp124 < 30.0f)) {
                this->unk_158 = 4;
                this->unk_1B4 = sLurePos;
                this->unk_1B0 = 16384.0f;
                this->unk_190 = 1.2f;
                this->unk_194 = 5000.0f;
                this->unk_17A[0] = 20;
            }
            break;

        case 4:
            Math_ApproachF(&this->unk_1B0, 16384.0f, 1.0f, 4096.0f);
            Math_ApproachS(&this->unk_170, 0x4E20, 4, 0x1388);

            this->unk_151 = 50;
            sp134 = 2;
            this->unk_1B4 = sLurePos;
            Math_ApproachF(&this->actor.speedXZ, this->unk_188, 1.0f, 1.0f);

            if ((D_80B7A694 != 3) || (this->unk_17A[0] == 0) || (sLurePos.y > (WATER_SURFACE_Y(play) + 5.0f)) ||
                (sqrtf(SQ(sLurePos.x) + SQ(sLurePos.z)) > 800.0f)) {

                this->unk_17A[0] = 0;
                this->unk_158 = this->unk_15A;
                this->unk_190 = 1.0f;
                this->unk_194 = 2000.0f;
            } else if (sp124 < 10.0f) {
                if (func_80B70A2C(this, play, false)) {
                    func_80B71278(this, 0);
                }

                this->unk_158 = 5;
                this->unk_190 = 1.2f;
                this->unk_194 = 5000.0f;
                this->unk_17A[1] = 150;
                this->unk_17A[0] = 0;
                this->unk_17A[2] = 0;
                this->unk_17A[3] = 120;

                D_80B7A694 = 4;
                sFishingHookedFish = this;
                sFishMouthOffset.y = 500.0f - Rand_ZeroFloat(400.0f);

                if (D_80B7E0B6 == 2) {
                    if (this->unk_1AC > 70.0f) {
                        phi_v0 = (s16)Rand_ZeroFloat(20.0f) + 10;
                    } else if (this->unk_1AC > 60.0f) {
                        phi_v0 = (s16)Rand_ZeroFloat(30.0f) + 20;
                    } else if (this->unk_1AC > 50.0f) {
                        phi_v0 = (s16)Rand_ZeroFloat(30.0f) + 30;
                    } else {
                        phi_v0 = (s16)Rand_ZeroFloat(40.0f) + 40;
                    }
                    D_80B7E122 = phi_v0;
                    D_80B7E0A4 = phi_v0;
                    func_800A9F6C(0.0f, 60, phi_v0 * 3, 10);
                } else {
                    if (this->unk_1AC > 70.0f) {
                        phi_v0 = (s16)Rand_ZeroFloat(5.0f) + 10;
                    } else if (this->unk_1AC > 60.0f) {
                        phi_v0 = (s16)Rand_ZeroFloat(5.0f) + 15;
                    } else if (this->unk_1AC > 50.0f) {
                        phi_v0 = (s16)Rand_ZeroFloat(5.0f) + 17;
                    } else {
                        phi_v0 = (s16)Rand_ZeroFloat(5.0f) + 25;
                    }
                    D_80B7E122 = phi_v0;
                    D_80B7E0A4 = phi_v0;
                    func_800A9F6C(0.0f, 180, phi_v0 * 3, 10);
                }

                D_80B7E124 = 0;
                D_80B7E116 = 100;
                D_80B7E080 = 0;
            }
            break;

        case -3:
            this->unk_151 = 50;
            this->unk_1B4 = sLurePos;
            Math_ApproachF(&this->actor.speedXZ, 2.0f, 1.0f, 1.0f);

            if ((D_80B7A694 != 3) || (this->unk_17A[0] == 0) || (sLurePos.y > (WATER_SURFACE_Y(play) + 5.0f)) ||
                (sqrtf(SQ(sLurePos.x) + SQ(sLurePos.z)) > 800.0f)) {

                this->unk_17A[0] = 0;
                this->unk_190 = 1.0f;
                this->unk_158 = this->unk_15A;
                this->unk_194 = 2000.0f;
            } else if (sp124 < 10.0f) {
                if (sLurePos.y > (WATER_SURFACE_Y(play) - 10.0f)) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_EV_JUMP_OUT_WATER);
                    func_80078884(NA_SE_PL_CATCH_BOOMERANG);
                }

                func_80B70A2C(this, play, false);
                this->unk_158 = 5;
                this->unk_190 = 1.2f;
                this->unk_194 = 5000.0f;
                this->unk_17A[1] = 150;
                this->unk_17A[0] = 0;
                this->unk_17A[2] = 0;
                this->unk_17A[3] = 120;

                D_80B7A694 = 4;
                sFishingHookedFish = this;

                if (D_80B7E0B6 == 2) {
                    D_80B7E122 = 30;
                    D_80B7E0A4 = 100;
                    func_800A9F6C(0.0f, 60, 90, 10);
                } else {
                    D_80B7E122 = 30;
                    D_80B7E0A4 = 40;
                    func_800A9F6C(0.0f, 180, 90, 10);
                }

                D_80B7E124 = 0;
                D_80B7E116 = 100;
                D_80B7E080 = 0;
            }
            break;

        case 5:
            this->actor.uncullZoneForward = 1200.0f;
            this->actor.uncullZoneScale = 200.0f;

            D_80B7E080++;
            osSyncPrintf("HIT FISH %dcm\n", (u8)this->unk_1AC);

            Math_ApproachS(&this->unk_170, 0x2AF8, 4, 0xBB8);
            sFishingHookedFish = this;
            Math_ApproachS(&player->actor.shape.rot.y, this->actor.yawTowardsPlayer + 0x8000, 5, 0x500);

            if (D_80B7E124 == 0) {
                if ((D_80B7FEA0 < 20) && ((D_80B7E0AE & 3) == 0)) {
                    D_80B7FEA0++;
                        }
            }

            if ((D_80B7E122 != 0) && (D_80B7E124 == 0)) {
                if (((input->rel.stick_y < -50) && (D_80B7A6C8 > -40)) || CHECK_BTN_ALL(input->press.button, BTN_A)) {
                    if (input->rel.stick_y < -50) {
                        temp_f0 = 40.0f - ((this->unk_1AC - 30.0f) * 1.333333f);
                        if (temp_f0 > 0.0f) {
                            this->unk_152 = temp_f0;
                            this->unk_154 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                            this->unk_156 = 1;
                        }
                    }

                    this->unk_198 = 1.7f;
                    this->unk_19C = 7000.0f;
                    D_80B7E124 = 1;
                    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_ENEMY | 0x800);
                    D_80B7E0A6 = 0;

                    if (this->unk_150 == 1) {
                        spA4 = (this->unk_1AC * 3.0f) + 120.0f;
                    } else {
                        spA4 = (2.0f * this->unk_1AC) + 120.0f;
                    }
                    if (spA4 > 255.0f) {
                        spA4 = 255.0f;
                    }

                    func_800A9F6C(0.0f, spA4, 120, 5);
                    D_80B7E0A4 = 40;
                    D_80B7FDA8 = 10;
                    func_80078884(NA_SE_IT_FISHING_HIT);
                }
            }

            if (this->actor.world.pos.y < WATER_SURFACE_Y(play)) {
                if (this->unk_17A[1] > 30) {
                    phi_v0_2 = 7;
                } else {
                    phi_v0_2 = 0xF;
                }

                if (((this->unk_15C & phi_v0_2) == 0) && (Rand_ZeroOne() < 0.75f) && (D_80B7E0A4 == 0)) {
                    if (this->unk_1AC >= 70.0f) {
                        spA4 = 255.0f;
                    } else if (this->unk_1AC >= 60.0f) {
                        spA4 = 230.0f;
                    } else if (this->unk_1AC >= 50.0f) {
                        spA4 = 200.0f;
                    } else if (this->unk_1AC >= 40.0f) {
                        spA4 = 170.0f;
                    } else {
                        spA4 = 140.0f;
                    }

                    if (phi_v0_2 == 0xF) {
                        spA4 *= 3.0f / 4.0f;
                    }

                    func_800A9F6C(0.0f, spA4, (s16)Rand_ZeroFloat(5.0f) + 10, 5);
                }

                if (this->unk_17A[1] > 30) {
                    if (this->unk_17A[0] == 0) {
                        sp10C.x = 0.0f;
                        sp10C.y = 0.0f;
                        sp10C.z = 200.0f;

                        for (spA2 = 0; spA2 < 100; spA2++) {
                            Matrix_RotateY(Rand_CenteredFloat(3.0f * M_PI / 4.0f) +
                                               (((this->actor.yawTowardsPlayer + 0x8000) / 32768.0f) * M_PI),
                                           MTXMODE_NEW);
                            Matrix_MultVec3f(&sp10C, &sp100);

                            this->unk_1B4.x = this->actor.world.pos.x + sp100.x;
                            this->unk_1B4.z = this->actor.world.pos.z + sp100.z;

                            if ((SQ(this->unk_1B4.x) + SQ(this->unk_1B4.z)) < SQ(750.0f)) {
                                break;
                            }
                        }

                        if ((Rand_ZeroOne() < 0.1f) && (this->unk_17A[3] == 0)) {
                            if (this->unk_1AC >= 60.0f) {
                                phi_a1 = 255;
                            } else if (this->unk_1AC >= 50.0f) {
                                phi_a1 = 200;
                            } else {
                                phi_a1 = 180;
                            }
                            func_800A9F6C(0.0f, phi_a1, 90, 2);
                            this->unk_17A[0] = 20;
                            this->unk_17A[1] = 100;
                            this->unk_17A[2] = 20;
                            this->unk_17A[3] = 100;
                            this->unk_1B4.y = 300.0f;
                            D_80B7E0A4 = 0x28;
                            D_80B7E116 = (s16)Rand_ZeroFloat(30.0f) + 20;
                        } else {
                            this->unk_17A[0] = (s16)Rand_ZeroFloat(10.0f) + 3;
                            this->unk_17A[2] = 0;
                            this->unk_1B4.y = -70.0f - Rand_ZeroFloat(150.0f);
                        }
                    }

                    if (this->unk_17A[2] != 0) {
                        D_80B7E11C = 0.0f;
                        this->unk_190 = 1.6f;
                        this->unk_194 = 6000.0f;
                        Math_ApproachF(&this->actor.speedXZ, 7.5f, 1.0f, 1.0f);
                        Math_ApproachS(&this->unk_170, 0x4E20, 2, 0xFA0);
                    } else {
                        if ((D_80B7E124 == 0) && (D_80B7E0B6 == 2)) {
                            this->unk_190 = 1.0f;
                            this->unk_194 = 2000.0f;
                            Math_ApproachF(&this->actor.speedXZ, 3.0f, 1.0f, 0.2f);
                        } else {
                            this->unk_190 = 1.4f;
                            this->unk_194 = 5000.0f;
                            Math_ApproachF(&this->actor.speedXZ, 5.0f, 1.0f, 0.5f);
                        }

                        if (this->unk_150 == 0) {
                            D_80B7E11C = 1.0f - (this->unk_1AC * 0.00899f);
                        } else {
                            D_80B7E11C = 1.0f - (this->unk_1AC * 0.00899f * 1.4f);
                        }
                    }
                } else {
                    if (((this->unk_17A[1] & 0xF) == 0) && CHECK_BTN_ALL(input->cur.button, BTN_A) &&
                        (!(this->unk_1AC >= 60.0f) || (D_80B7E080 >= 2000))) {
                        this->unk_152 = (s16)Rand_ZeroFloat(30.0f) + 15;
                        this->unk_154 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                    }

                    this->unk_190 = 1.0f;
                    this->unk_194 = 4500.0f;

                    if (this->unk_150 == 0) {
                        D_80B7E11C = 1.3f - (this->unk_1AC * 0.00899f);
                    } else {
                        D_80B7E11C = 1.3f - (this->unk_1AC * 0.00899f * 1.4f);
                    }

                    Math_ApproachF(&this->actor.speedXZ, 2.0f, 1.0f, 0.5f);

                    if (this->unk_17A[1] == 0) {
                        this->unk_152 = 0;

                        if (D_80B7E080 < 2000) {
                            this->unk_17A[1] = (s16)Rand_ZeroFloat(50.0f) + 50;
                        } else if (D_80B7E080 < 3000) {
                            this->unk_17A[1] = (s16)Rand_ZeroFloat(20.0f) + 30;
                        } else {
                            this->unk_17A[1] = (s16)Rand_ZeroFloat(10.0f) + 25;
                        }
                    }
                }
            }

            if (D_80B7E074 != 0) {
                D_80B7E11C = 0.0f;
            }

            if (D_80B7E124 || (D_80B7E0B6 != 2)) {
                if (this->actor.speedXZ < 3.0f) {
                    if ((D_80B7E0AE & 8) != 0) {
                        sp100.x = -0.8f;
                    } else {
                        sp100.x = -0.75f;
                    }
                } else {
                    if ((D_80B7E0AE & 4) != 0) {
                        sp100.x = -0.9f;
                    } else {
                        sp100.x = -0.85f;
                    }
                }

                Math_ApproachF(&D_80B7A6C0, 35.0f, 0.1f, 3.5f);
                Math_ApproachF(&D_80B7A6BC, sp100.x, 0.3f, 0.1f);
            }

            sReelLinePos[LINE_SEG_COUNT - 1] = this->fishMouthPos;
            sp10C.x = sReelLinePos[LINE_SEG_COUNT - 1].x - sReelLinePos[LINE_SEG_COUNT - 2].x;
            sp10C.y = sReelLinePos[LINE_SEG_COUNT - 1].y - sReelLinePos[LINE_SEG_COUNT - 2].y;
            sp10C.z = sReelLinePos[LINE_SEG_COUNT - 1].z - sReelLinePos[LINE_SEG_COUNT - 2].z;

            if ((SQ(sp10C.x) + SQ(sp10C.y) + SQ(sp10C.z)) > SQ(20.0f)) {
                Math_ApproachF(&this->actor.world.pos.x, sReelLinePos[LINE_SEG_COUNT - 2].x, 0.2f,
                               2.0f * (this->actor.speedXZ * 1.5f));
                Math_ApproachF(&this->actor.world.pos.y, sReelLinePos[LINE_SEG_COUNT - 2].y, 0.2f,
                               2.0f * (this->actor.speedXZ * 1.5f) * 5.0f * 0.1f);
                Math_ApproachF(&this->actor.world.pos.z, sReelLinePos[LINE_SEG_COUNT - 2].z, 0.2f,
                               2.0f * (this->actor.speedXZ * 1.5f));
            }

            if (CHECK_BTN_ALL(input->cur.button, BTN_A) || (input->rel.stick_y < -30)) {
                if (D_80B7E116 < 100) {
                    D_80B7E116++;
                }
            } else {
                if (D_80B7E116 != 0) {
                    D_80B7E116--;
                }
            }

            if ((D_80B7A694 < 3) || ((D_80B7E074 != 0) && (D_80B7E080 > 50)) || (D_80B7E080 >= 6000) ||
                ((D_80B7E122 == 0) && (D_80B7E124 == 0)) || (D_80B7E116 == 0) ||
                (((D_80B7E0AE & 0x7F) == 0) && (Rand_ZeroOne() < 0.05f) && (D_80B7E0B6 != 2) && (KREG(69) == 0) && (getFishNeverEscape() == 0))) {
                D_80B7A67C = 20;

                if ((D_80B7E122 == 0) && (D_80B7E124 == 0)) {
                    D_80B7E086 = 0x4081;
                    if (((sLinkAge == 1) && (HIGH_SCORE(HS_FISHING) & 0x400)) ||
                        ((sLinkAge != 1) && (HIGH_SCORE(HS_FISHING) & 0x800))) {
                        D_80B7A67C = 0;
                    }
                } else {
                    D_80B7E086 = 0x4082;
                    func_800A9F6C(0.0f, 1, 3, 1);
                    Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x0A00FF);
                }

                this->unk_158 = this->unk_15A = 0;
                this->unk_1A4 = 10000;
                this->unk_1A2 = 500;
                this->unk_17A[1] = 50;
                this->unk_17A[0] = 0;
                this->unk_190 = 1.0f;
                this->unk_194 = 3000.0f;

                if (D_80B7A694 == 4) {
                    D_80B7A694 = 3;
                }

                D_80B7E0A6 = 50;
                D_80B7E11C = 0.5f;
                this->unk_152 = 0;
            } else if (this->actor.xzDistToPlayer < (KREG(59) + 50.0f) || getInstantFish() == 1) {
                this->unk_158 = 6;
                this->unk_17A[0] = 100;
                player->unk_860 = 3;
                func_800A9F6C(0.0f, 1, 3, 1);
                D_80B7E084++;
                func_80064520(play, &play->csCtx);
                D_80B7A6CC = 100;
                D_80B7FEC8 = 45.0f;
                D_80B7A694 = 5;
                this->unk_190 = 1.0f;
                this->unk_194 = 500.0f;
                this->unk_19C = 5000.0f;

                if (this->actor.world.pos.y <= WATER_SURFACE_Y(play)) {
                    func_80B71278(this, 1);
                    func_80B70A2C(this, play, true);
                }
                goto case_6;
            }
            break;

        case_6:
        case 6:
            Math_ApproachS(&this->unk_170, 0x2AF8, 2, 0xFA0);
            Math_ApproachF(&D_80B7FEC8, 15.0f, 0.05f, 0.75f);

            sp10C.x = D_80B7FEC8;
            if (sLinkAge != 1) {
                sp10C.y = 30.0f;
                sp10C.z = 55.0f;
            } else {
                sp10C.y = 10.0f;
                sp10C.z = 50.0f;
            }
            Matrix_RotateY((player->actor.shape.rot.y / 32768.0f) * M_PI, MTXMODE_NEW);
            Matrix_MultVec3f(&sp10C, &sCameraEye);

            sCameraEye.x += player->actor.world.pos.x;
            sCameraEye.y += player->actor.world.pos.y;
            sCameraEye.z += player->actor.world.pos.z;

            sCameraAt = player->actor.world.pos;
            if (sLinkAge != 1) {
                sCameraAt.y += 40.0f;
            } else {
                sCameraAt.y += 25.0f;
            }

            if (this->unk_17A[0] == 90) {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_HEART_GET | 0x900);
                D_80B7A67C = 40;

                if (this->unk_150 == 0) {
                    D_80B7A678 = this->unk_1AC;

                    if (D_80B7A678 >= 75) {
                        D_80B7E086 = 0x409F;
                    } else if (D_80B7A678 >= 50) {
                        D_80B7E086 = 0x4091;
                    } else {
                        D_80B7E086 = 0x4083;
                    }
                } else {
                    D_80B7A678 = 2.0f * this->unk_1AC;
                    D_80B7E086 = 0x4099;
                }

                this->unk_1D5 = 0;
            }

            this->unk_160 = -0x4000;
            this->actor.shape.rot.y = player->actor.shape.rot.y + 0x5000;
            this->actor.shape.rot.x = this->actor.shape.rot.z = this->unk_162 = this->unk_164 = this->unk_16E = 0;

            sp10C.x = 4.0f;
            sp10C.y = -10.0f;
            sp10C.z = 5.0f;
            Matrix_MultVec3f(&sp10C, &sp100);
            if (getInstantFish() == 0) {
                Math_ApproachF(&this->actor.world.pos.x, player->bodyPartsPos[15].x + sp100.x, 1.0f, 6.0f);
                Math_ApproachF(&this->actor.world.pos.y, player->bodyPartsPos[15].y + sp100.y, 1.0f, 6.0f);
                Math_ApproachF(&this->actor.world.pos.z, player->bodyPartsPos[15].z + sp100.z, 1.0f, 6.0f);
            } else {
                this->actor.world.pos.x = player->bodyPartsPos[15].x + sp100.x;
                this->actor.world.pos.y = player->bodyPartsPos[15].y + sp100.y;
                this->actor.world.pos.z = player->bodyPartsPos[15].z + sp100.z;
            }
            D_80B7E144 = 188.0f;

            if (this->unk_17A[0] <= 50) {
                switch (this->unk_1D5) {
                    case 0:
                        if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) ||
                            (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                            if (Message_ShouldAdvance(play)) {
                                Message_CloseTextbox(play);
                                if (play->msgCtx.choiceIndex == 0) {
                                    if (D_80B7A670 == 0.0f) {
                                        D_80B7A670 = this->unk_1AC;
                                        D_80B7E07C = this->unk_150;
                                        D_80B7E07E = D_80B7E0B6;
                                        Actor_Kill(&this->actor);
                                    } else if ((this->unk_150 == 0) && (D_80B7E07C == 0) &&
                                               ((s16)this->unk_1AC < (s16)D_80B7A670)) {
                                        this->unk_1D5 = 1;
                                        this->unk_17A[0] = 0x3C;
                                        Message_StartTextbox(play, 0x4098, NULL);
                                    } else {
                                        f32 temp1 = D_80B7A670;
                                        s16 temp2 = D_80B7E07C;
                                        D_80B7A670 = this->unk_1AC;
                                        D_80B7E07C = this->unk_150;
                                        D_80B7E07E = D_80B7E0B6;
                                        this->unk_1AC = temp1;
                                        this->unk_150 = temp2;
                                    }
                                }
                                if (this->unk_1D5 == 0) {
                                    D_80B7A694 = 0;
                                }
                            }
                        }
                        break;
                    case 1:
                        if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) ||
                            (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                            if (Message_ShouldAdvance(play)) {
                                Message_CloseTextbox(play);
                                if (play->msgCtx.choiceIndex != 0) {
                                    f32 temp1 = D_80B7A670;
                                    s16 temp2 = D_80B7E07C;
                                    D_80B7A670 = this->unk_1AC;
                                    D_80B7E07E = D_80B7E0B6;
                                    this->unk_1AC = temp1;
                                    this->unk_150 = temp2;
                                }
                                D_80B7A694 = 0;
                            }
                        }
                        break;
                }
            }

            if (D_80B7A694 == 0) {
                if (this->actor.update != NULL) {
                    this->unk_158 = this->unk_15A = 0;
                    this->unk_1A4 = 10000;
                    this->unk_1A2 = 500;
                    this->unk_17A[1] = 50;
                    this->unk_17A[0] = 0;
                    this->unk_190 = 1.0f;
                    this->unk_194 = 2000.0f;
                    SkelAnime_Free(&this->skelAnime, play);

                    if (this->unk_150 == 0) {
                        SkelAnime_InitFlex(play, &this->skelAnime, &gFishingFishSkel, &gFishingFishAnim, 0, 0, 0);
                        Animation_MorphToLoop(&this->skelAnime, &gFishingFishAnim, 0.0f);
                    } else {
                        SkelAnime_InitFlex(play, &this->skelAnime, &gFishingLoachSkel, &gFishingLoachAnim, 0, 0,
                                           0);
                        Animation_MorphToLoop(&this->skelAnime, &gFishingLoachAnim, 0.0f);
                    }
                }

                D_80B7E148 = 520.0f;
                D_80B7E144 = 195.0f;
                Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0xA00FF);
                D_80B7E0A6 = 20;
                D_80B7A6CC = 3;
            }
            break;

        case 7:
            this->unk_151 = 50;
            sp134 = 5;
            this->unk_1B0 = 12288.0f;

            if (this->actor.params < 104) {
                this->unk_1B4 = sGroupFishes[this->actor.params - 100].pos;
                D_80B7A898 = 1;
            } else if (this->actor.params < 108) {
                this->unk_1B4 = sGroupFishes[this->actor.params - 100 + 16].pos;
                D_80B7A898 = 2;
            } else {
                this->unk_1B4 = sGroupFishes[this->actor.params - 100 + 32].pos;
                D_80B7A898 = 3;
            }

            Math_ApproachF(&this->actor.speedXZ, 5.0f, 1.0f, 1.0f);

            if (sp124 < 20.0f) {
                Math_ApproachS(&this->unk_170, 0x4E20, 2, 0xFA0);

                if ((this->unk_17A[2] == 0) && func_80B70A2C(this, play, false)) {
                    func_80B71278(this, Rand_ZeroFloat(1.99f));
                    this->unk_17A[2] = (s16)Rand_ZeroFloat(20.0f) + 20;
                }
            }

            if (this->unk_17A[3] == 0) {
                this->unk_158 = 10;
                this->unk_15A = 10;
            } else {
                func_80B70ED4(this, input);
                if (this->actor.xzDistToPlayer < (100.0f * sp118)) {
                    this->unk_15A = this->unk_158 = 0;
                    this->unk_1A4 = 500;
                    this->unk_1A2 = 200;
                    this->unk_17A[1] = 50;
                }
            }
            break;
    }

    Math_ApproachS(&this->unk_172, (Math_SinS(this->unk_15C * 0x1000) * 5000.0f) + 5000.0f, 2, 0x7D0);

    if (this->unk_158 != 6) {
        if (this->actor.world.pos.y > WATER_SURFACE_Y(play)) {
            this->unk_190 = 1.5f;
            this->unk_194 = 5000.0f;

            Math_ApproachS(&this->unk_16E, 0, 5, 0x7D0);

            spF4 = spF0 = spFA = 3;
            spF2 = spEE = 0x2000;

            this->unk_17A[2] = 0;
            this->unk_184 -= 1.0f;
        } else {
            Math_ApproachZeroF(&this->unk_184, 1.0f, 2.0f);
            if ((this->unk_158 != -1) && (this->unk_158 != -2) && (this->unk_158 != -25)) {
                this->unk_166 = 0;
            }

            this->unk_168 = this->unk_16A = 0;
            spF4 = spF0 = spFA = 4;
            spF2 = spEE = 0x2000;

            spF6 = Fishing_SmoothStepToS(&this->actor.world.rot.y, spFC, sp134, this->unk_1B0) * 3.0f;
            Math_ApproachS(&this->actor.world.rot.x, spFE, sp134, this->unk_1B0 * 0.5f);

            if (spF6 > 0x1F40) {
                spF6 = 0x1F40;
            } else if (spF6 < -0x1F40) {
                spF6 = -0x1F40;
            }

            if (this->actor.speedXZ >= 3.2f) {
                Math_ApproachS(&this->unk_16E, spF6, 2, 0x4E20);
            } else {
                Math_ApproachS(&this->unk_16E, spF6, 3, 0xBB8);
            }

            func_8002D908(&this->actor);
        }

        func_8002D7EC(&this->actor);

        this->actor.world.pos.y += (this->unk_184 * 1.5f);

        if (this->unk_152 != 0) {
            this->unk_168 = this->unk_154;
            this->unk_152--;
            if (this->unk_156 != 0) {
                spF0 = 5;
                spEE = 0x4000;
            } else {
                spF0 = 10;
                spEE = 0x800;
            }
            this->unk_166 = -0x500 - this->actor.shape.rot.x;
            spF4 = 5;
            spF2 = 0x4000;
        } else {
            this->unk_156 = 0;
        }

        Math_ApproachS(&this->unk_160, this->unk_166, spF4, spF2);
        Math_ApproachS(&this->unk_162, this->unk_168, spF0, spEE);
        Math_ApproachS(&this->unk_164, this->unk_16A, spFA, 0x2000);

        if (this->actor.speedXZ <= 0.5f) {
            Math_ApproachS(&this->actor.shape.rot.x, 0, 10, this->unk_178);
            Math_ApproachS(&this->unk_178, 0x500, 1, 0x20);
        } else {
            Math_ApproachS(&this->actor.shape.rot.x, -this->actor.world.rot.x, 10, 0x1000);
            this->unk_178 = 0;
        }

        this->actor.shape.rot.y = this->actor.world.rot.y;

        if ((this->unk_158 != -1) && (this->unk_158 != -2) && (this->unk_158 != -25)) {
            if ((this->actor.world.pos.y > WATER_SURFACE_Y(play)) &&
                (this->actor.prevPos.y <= WATER_SURFACE_Y(play))) {
                func_80B70A2C(this, play, true);
                func_80B71278(this, 1);
                this->unk_184 = this->actor.velocity.y;
                this->actor.velocity.y = 0.0f;
                this->unk_16A = Rand_CenteredFloat(32768.0f);
            } else if ((this->actor.world.pos.y < WATER_SURFACE_Y(play)) &&
                       (this->actor.prevPos.y >= WATER_SURFACE_Y(play))) {
                if (this->unk_184 < -5.0f) {
                    this->unk_184 = -5.0f;
                }
                this->actor.world.rot.x = -0xFA0;
                func_80B70A2C(this, play, true);
                this->unk_1D2 = 20;
                func_80B71278(this, 0);
            }
        }

        if ((this->actor.world.pos.y < WATER_SURFACE_Y(play)) &&
            (this->actor.world.pos.y > (WATER_SURFACE_Y(play) - 10.0f)) && ((this->unk_15C & 1) == 0) &&
            (this->actor.speedXZ > 0.0f)) {
            Vec3f pos = this->actor.world.pos;
            pos.y = WATER_SURFACE_Y(play);
            Fishing_SpawnRipple(&this->actor.projectedPos, play->specialEffects, &pos, 80.0f, 500.0f, 150, 90);
        }

        if ((this->actor.speedXZ > 0.0f) || (this->unk_158 == 5)) {
            f32 velocityY = this->actor.velocity.y;

            spD8 = this->unk_1AC * 0.1f;

            this->actor.world.pos.y -= spD8;
            this->actor.prevPos.y -= spD8;
            this->actor.velocity.y = -1.0f;
            Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 30.0f, 100.0f, 0x45);
            this->actor.world.pos.y += spD8;
            this->actor.prevPos.y += spD8;

            this->actor.velocity.y = velocityY;

            if (this->actor.bgCheckFlags & 8) {
                this->unk_1A0 = 20;
            }

            if (this->actor.bgCheckFlags & 1) {
                if (this->actor.world.pos.y > WATER_SURFACE_Y(play)) {
                    this->unk_184 = Rand_ZeroFloat(3.0f) + 3.0f;
                    this->actor.velocity.x = this->actor.world.pos.x * -0.003f;
                    this->actor.velocity.z = this->actor.world.pos.z * -0.003f;

                    Audio_PlayActorSound2(&this->actor, NA_SE_EV_FISH_LEAP);
                    func_80B70CF0(this, play);

                    if (Rand_ZeroOne() < 0.5f) {
                        this->unk_16A = 0x4000;
                    } else {
                        this->unk_16A = -0x4000;
                    }

                    if (Rand_ZeroOne() < 0.5f) {
                        this->unk_166 = 0;
                    } else {
                        this->unk_166 = (s16)Rand_CenteredFloat(32.0f) + 0x8000;
                    }

                    this->unk_168 = (s16)Rand_CenteredFloat(16384.0f);
                    this->unk_190 = 1.0f;
                    this->unk_194 = 5000.0f;
                    this->unk_19C = 5000.0f;
                } else {
                    this->unk_184 = 0.0f;

                    if ((this->unk_158 == 5) && ((this->unk_15C & 1) == 0)) {
                        Vec3f pos;

                        pos.x = Rand_CenteredFloat(10.0f) + this->actor.world.pos.x;
                        pos.z = Rand_CenteredFloat(10.0f) + this->actor.world.pos.z;
                        pos.y = this->actor.floorHeight + 5.0f;
                        Fishing_SpawnWaterDust(&this->actor.projectedPos, play->specialEffects, &pos,
                                               (this->unk_1AC * 0.005f) + 0.15f);
                    }
                }
            }
        }
    }

    if (this->unk_1D2 != 0) {
        s16 i;
        Vec3f pos;
        f32 range = (this->unk_1AC * 0.075f) + 10.0f;

        this->unk_1D2--;

        for (i = 0; i < 2; i++) {
            pos.x = Rand_CenteredFloat(range) + this->actor.world.pos.x;
            pos.y = Rand_CenteredFloat(range) + this->actor.world.pos.y;
            pos.z = Rand_CenteredFloat(range) + this->actor.world.pos.z;
            Fishing_SpawnBubble(&this->actor.projectedPos, play->specialEffects, &pos,
                                Rand_ZeroFloat(0.035f) + 0.04f, 0);
        }
    }
}

s32 Fishing_FishOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                 void* thisx) {
    Fishing* this = (Fishing*)thisx;

    if (limbIndex == 0xD) {
        rot->z -= this->unk_170 - 11000;
    } else if ((limbIndex == 2) || (limbIndex == 3)) {
        rot->y += this->unk_16C;
    } else if (limbIndex == 4) {
        rot->y += this->unk_176;
    } else if (limbIndex == 0xE) {
        rot->y -= this->unk_172;
    } else if (limbIndex == 0xF) {
        rot->y += this->unk_172;
    } else if (limbIndex == 8) {
        rot->y += this->unk_174;
    } else if (limbIndex == 9) {
        rot->y -= this->unk_174;
    }

    return 0;
}

void Fishing_FishPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    Fishing* this = (Fishing*)thisx;

    if (limbIndex == 0xD) {
        Matrix_MultVec3f(&sFishMouthOffset, &this->fishMouthPos);
    }
}

s32 Fishing_LoachOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                  void* thisx) {
    Fishing* this = (Fishing*)thisx;

    if (limbIndex == 3) {
        rot->y += this->unk_1CC[0];
    } else if (limbIndex == 4) {
        rot->y += this->unk_1CC[1];
    } else if (limbIndex == 5) {
        rot->y += this->unk_1CC[2];
    }

    return 0;
}

void Fishing_LoachPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f sLoachMouthOffset = { 500.0f, 500.0f, 0.0f };
    Fishing* this = (Fishing*)thisx;

    if (limbIndex == 0xB) {
        Matrix_MultVec3f(&sLoachMouthOffset, &this->fishMouthPos);
    }
}

void Fishing_DrawFish(Actor* thisx, PlayState* play) {
    Fishing* this = (Fishing*)thisx;

    func_80093D18(play->state.gfxCtx);

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateY(((this->unk_162 + this->actor.shape.rot.y) / 32768.0f) * M_PI, MTXMODE_APPLY);
    Matrix_RotateX(((this->unk_160 + this->actor.shape.rot.x) / 32768.0f) * M_PI, MTXMODE_APPLY);
    Matrix_RotateZ(((this->unk_164 + this->actor.shape.rot.z) / 32768.0f) * M_PI, MTXMODE_APPLY);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);

    if (this->unk_150 == 0) {
        Matrix_RotateY((this->unk_16C * (M_PI / 32768)) - (M_PI / 2), MTXMODE_APPLY);
        Matrix_Translate(0.0f, 0.0f, this->unk_16C * 10.0f * 0.01f, MTXMODE_APPLY);

        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                              this->skelAnime.dListCount, Fishing_FishOverrideLimbDraw, Fishing_FishPostLimbDraw, this);
    } else {
        Matrix_Translate(0.0f, 0.0f, 3000.0f, MTXMODE_APPLY);
        Matrix_RotateY(this->unk_16C * (M_PI / 32768), MTXMODE_APPLY);
        Matrix_Translate(0.0f, 0.0f, -3000.0f, MTXMODE_APPLY);
        Matrix_RotateY(-(M_PI / 2), MTXMODE_APPLY);

        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                              this->skelAnime.dListCount, Fishing_LoachOverrideLimbDraw, Fishing_LoachPostLimbDraw,
                              this);
    }
}

void Fishing_HandleReedContact(FishingProp* prop, Vec3f* entityPos) {
    f32 dx = prop->pos.x - entityPos->x;
    f32 dz = prop->pos.z - entityPos->z;
    f32 distXZ = sqrtf(SQ(dx) + SQ(dz));

    if (distXZ <= 20.0f) {
        prop->rotY = Math_Atan2F(dz, dx);

        Math_ApproachF(&prop->rotX, (20.0f - distXZ) * 0.03f, 0.2f, 0.2f);
    }
}

void Fishing_HandleLilyPadContact(FishingProp* prop, Vec3f* entityPos, u8 fishTimer) {
    f32 dx = prop->pos.x - entityPos->x;
    f32 dz = prop->pos.z - entityPos->z;
    f32 distXZ = sqrtf(SQ(dx) + SQ(dz));

    if (distXZ <= 40.0f) {
        Math_ApproachS(&prop->lilyPadAngle, Math_Atan2S(dz, dx), 10, 0x300);
    }

    if (fishTimer && (distXZ <= 60.0f)) {
        f32 heightTarget = 1.0f;

        if (fishTimer >= 21) {
            heightTarget = 1.5f;
        }

        Math_ApproachF(&prop->lilyPadOffset, heightTarget, 0.1f, 0.2f);
    }
}

void Fishing_UpdatePondProps(PlayState* play) {
    FishingProp* prop = &sPondProps[0];
    Player* player = GET_PLAYER(play);
    Actor* actor;
    s16 i;

    for (i = 0; i < POND_PROP_COUNT; i++) {
        if (prop->type != FS_PROP_NONE) {
            prop->shouldDraw = false;
            prop->timer++;

            SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &prop->pos, &prop->projectedPos, &sProjectedW);

            if ((prop->projectedPos.z < prop->drawDistance) &&
                (fabsf(prop->projectedPos.x) < (100.0f + prop->projectedPos.z))) {
                prop->shouldDraw = true;
            }

            if ((prop->projectedPos.z < 500.0f) && (fabsf(prop->projectedPos.x) < (100.0f + prop->projectedPos.z))) {
                if (prop->type == FS_PROP_REED) {
                    Fishing_HandleReedContact(prop, &player->actor.world.pos);

                    actor = play->actorCtx.actorLists[ACTORCAT_NPC].head;
                    while (actor != NULL) {
                        if (!((actor->id == ACTOR_FISHING) && (actor->params >= 100))) {
                            actor = actor->next;
                        } else {
                            Fishing_HandleReedContact(prop, &actor->world.pos);
                            actor = actor->next;
                        }
                    }

                    Math_ApproachZeroF(&prop->rotX, 0.05f, 0.05f);
                } else if (prop->type == FS_PROP_LILY_PAD) {
                    Fishing_HandleLilyPadContact(prop, &player->actor.world.pos, 0);

                    actor = play->actorCtx.actorLists[ACTORCAT_NPC].head;
                    while (actor != NULL) {
                        if (!((actor->id == ACTOR_FISHING) && (actor->params >= 100))) {
                            actor = actor->next;
                        } else {
                            Fishing_HandleLilyPadContact(prop, &actor->world.pos, ((Fishing*)actor)->unk_151);
                            actor = actor->next;
                        }
                    }

                    Math_ApproachS(&prop->lilyPadAngle, 0, 20, 80);
                    prop->pos.y =
                        (Math_SinS(prop->timer * 0x1000) * prop->lilyPadOffset) + (WATER_SURFACE_Y(play) + 2.0f);
                    Math_ApproachZeroF(&prop->lilyPadOffset, 0.1f, 0.02f);
                }
            }
        }

        prop++;
    }

    if (sCameraId == 0) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &sFishingMain->collider.base);
    }
}

void Fishing_DrawPondProps(PlayState* play) {
    u8 flag = 0;
    FishingProp* prop = &sPondProps[0];
    s16 i;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Push();

    for (i = 0; i < POND_PROP_COUNT; i++) {
        if (prop->type == FS_PROP_REED) {
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingReedMaterialDL);
                flag++;
            }

            if (prop->shouldDraw) {
                FrameInterpolation_RecordOpenChild(prop, prop->epoch);
                Matrix_Translate(prop->pos.x, prop->pos.y, prop->pos.z, MTXMODE_NEW);
                Matrix_Scale(prop->scale, prop->scale, prop->scale, MTXMODE_APPLY);
                Matrix_RotateY(prop->rotY, MTXMODE_APPLY);
                Matrix_RotateX(prop->rotX, MTXMODE_APPLY);
                Matrix_RotateY(prop->reedAngle, MTXMODE_APPLY);

                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, gFishingReedModelDL);
                FrameInterpolation_RecordCloseChild();
            }
        }

        prop++;
    }

    prop = &sPondProps[0];
    flag = 0;
    for (i = 0; i < POND_PROP_COUNT; i++) {
        if (prop->type == FS_PROP_WOOD_POST) {
            if (flag == 0) {
                gSPDisplayList(POLY_OPA_DISP++, gFishingWoodPostMaterialDL);
                flag++;
            }

            if (prop->shouldDraw) {
                FrameInterpolation_RecordOpenChild(prop, prop->epoch);
                Matrix_Translate(prop->pos.x, prop->pos.y, prop->pos.z, MTXMODE_NEW);
                Matrix_Scale(prop->scale, prop->scale, prop->scale, MTXMODE_APPLY);

                gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_OPA_DISP++, gFishingWoodPostModelDL);
                FrameInterpolation_RecordCloseChild();
            }
        }

        prop++;
    }

    prop = &sPondProps[0];
    flag = 0;
    for (i = 0; i < POND_PROP_COUNT; i++) {
        if (prop->type == FS_PROP_LILY_PAD) {
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingLilyPadMaterialDL);
                flag++;
            }

            if (prop->shouldDraw) {
                FrameInterpolation_RecordOpenChild(prop, prop->epoch);
                Matrix_Translate(prop->pos.x, prop->pos.y, prop->pos.z, MTXMODE_NEW);
                Matrix_Scale(prop->scale, 1.0f, prop->scale, MTXMODE_APPLY);
                Matrix_RotateY(prop->lilyPadAngle * (M_PI / 32768), MTXMODE_APPLY);
                Matrix_Translate(0.0f, 0.0f, 20.0f, MTXMODE_APPLY);
                Matrix_RotateY(prop->rotY, MTXMODE_APPLY);

                gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_XLU_DISP++, gFishingLilyPadModelDL);
                FrameInterpolation_RecordCloseChild();
            }
        }

        prop++;
    }

    prop = &sPondProps[0];
    flag = 0;
    for (i = 0; i < POND_PROP_COUNT; i++) {
        if (prop->type == FS_PROP_ROCK) {
            if (flag == 0) {
                gSPDisplayList(POLY_OPA_DISP++, gFishingRockMaterialDL);
                flag++;
            }

            if (prop->shouldDraw) {
                FrameInterpolation_RecordOpenChild(prop, prop->epoch);
                Matrix_Translate(prop->pos.x, prop->pos.y, prop->pos.z, MTXMODE_NEW);
                Matrix_Scale(prop->scale, prop->scale, prop->scale, MTXMODE_APPLY);
                Matrix_RotateY(prop->rotY, MTXMODE_APPLY);

                gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_OPA_DISP++, gFishingRockModelDL);
                FrameInterpolation_RecordCloseChild();
            }
        }

        prop++;
    }

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

void Fishing_UpdateGroupFishes(PlayState* play) {
    s16 groupContactFlags = 0;
    Player* player = GET_PLAYER(play);
    FishingGroupFish* fish = &sGroupFishes[0];
    f32 dy;
    f32 dx;
    f32 dist;
    f32 dz;
    f32 offset;
    s16 groupIndex;
    s16 groupFlag;
    f32 spD8;
    s16 spD6;
    s16 spD4;
    s16 target;
    s16 i;
    Vec3f basePos[3];
    Vec3f ripplePos;
    Vec3f* refPos;
    f32 temp1;
    f32 temp2;

    if ((D_80B7E114 != 0) || (D_80B7A694 == 4)) {
        refPos = &sLurePos;
    } else {
        refPos = &player->actor.world.pos;
    }

    basePos[0].x = sinf(sFishGroupAngle1) * 720.0f;
    basePos[0].y = -35.0f;
    basePos[0].z = cosf(sFishGroupAngle1) * 720.0f;

    temp1 = refPos->x - basePos[0].x;
    temp2 = refPos->z - basePos[0].z;

    if ((SQ(temp1) + SQ(temp2)) < SQ(50.0f)) {
        sFishGroupAngle1 += 0.3f;
        groupContactFlags |= 1;
    } else if (D_80B7A898 != 0.0f) {
        sFishGroupAngle1 += 0.05f;
        basePos[0].y = WATER_SURFACE_Y(play) - 5.0f;
    } else {
        Math_ApproachF(&sFishGroupAngle1, 0.7f, 1.0f, 0.001f);
    }

    basePos[1].x = sinf(sFishGroupAngle2) * 720.0f;
    basePos[1].y = -35.0f;
    basePos[1].z = cosf(sFishGroupAngle2) * 720.0f;

    temp1 = refPos->x - basePos[1].x;
    temp2 = refPos->z - basePos[1].z;

    if ((SQ(temp1) + SQ(temp2)) < SQ(50.0f)) {
        sFishGroupAngle2 -= 0.3f;
        groupContactFlags |= 2;
    } else if (D_80B7A898 != 0.0f) {
        sFishGroupAngle2 -= 0.05f;
        basePos[1].y = WATER_SURFACE_Y(play) - 5.0f;
    } else {
        Math_ApproachF(&sFishGroupAngle2, 2.3f, 1.0f, 0.001f);
    }

    basePos[2].x = sinf(sFishGroupAngle3) * 720.0f;
    basePos[2].y = -35.0f;
    basePos[2].z = cosf(sFishGroupAngle3) * 720.0f;

    temp1 = refPos->x - basePos[2].x;
    temp2 = refPos->z - basePos[2].z;

    if ((SQ(temp1) + SQ(temp2)) < SQ(50.0f)) {
        sFishGroupAngle3 -= 0.3f;
        groupContactFlags |= 4;
    } else if (D_80B7A898 != 0.0f) {
        sFishGroupAngle3 -= 0.05f;
        basePos[2].y = WATER_SURFACE_Y(play) - 5.0f;
    } else {
        Math_ApproachF(&sFishGroupAngle3, 4.6f, 1.0f, 0.001f);
    }

    if (sLinkAge == 1) {
        spD8 = 0.8f;
    } else {
        spD8 = 1.0f;
    }

    for (i = 0; i < GROUP_FISH_COUNT; i++) {
        if (fish->type != FS_GROUP_FISH_NONE) {
            fish->timer++;

            SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &fish->pos, &fish->projectedPos, &sProjectedW);

            if ((fish->projectedPos.z < 400.0f) && (fabsf(fish->projectedPos.x) < (100.0f + fish->projectedPos.z))) {
                fish->shouldDraw = true;
            } else {
                fish->shouldDraw = false;
            }

            if (i <= 20) {
                groupIndex = 0;
                groupFlag = 1;
            } else if (i <= 40) {
                groupIndex = 1;
                groupFlag = 2;
            } else {
                groupIndex = 2;
                groupFlag = 4;
            }

            dx = fish->unk_10.x - fish->pos.x;
            dy = fish->unk_10.y - fish->pos.y;
            dz = fish->unk_10.z - fish->pos.z;
            spD4 = Math_Atan2S(dz, dx);
            dist = sqrtf(SQ(dx) + SQ(dz));
            spD6 = Math_Atan2S(dist, dy);

            if ((dist < 10.0f) || (((fish->timer % 32) == 0) && (Rand_ZeroOne() > 0.5f))) {
                fish->unk_10.y = basePos[groupIndex].y + Rand_CenteredFloat(10.0f);

                if (D_80B7A898 != 0.0f) {
                    fish->unk_10.x = basePos[groupIndex].x + Rand_CenteredFloat(200.0f);
                    fish->unk_10.z = basePos[groupIndex].z + Rand_CenteredFloat(200.0f);
                } else {
                    fish->unk_10.x = basePos[groupIndex].x + Rand_CenteredFloat(100.0f);
                    fish->unk_10.z = basePos[groupIndex].z + Rand_CenteredFloat(100.0f);
                }

                ripplePos = fish->pos;
                ripplePos.y = WATER_SURFACE_Y(play);
                Fishing_SpawnRipple(&fish->projectedPos, play->specialEffects, &ripplePos, 20.0f,
                                    Rand_ZeroFloat(50.0f) + 100.0f, 150, 90);

                if (fish->unk_28 < 1.5f) {
                    fish->unk_28 = 1.5f;
                }

                fish->unk_34 = 1.5f;
                fish->unk_38 = 1.0f;
            }

            target = Fishing_SmoothStepToS(&fish->unk_3E, spD4, 5, 0x4000) * 3.0f;
            if (target > 0x1F40) {
                target = 0x1F40;
            } else if (target < -0x1F40) {
                target = -0x1F40;
            }

            Math_ApproachS(&fish->unk_42, target, 3, 0x1388);

            offset = fish->unk_42 * -0.0001f;
            Math_ApproachS(&fish->unk_3C, spD6, 5, 0x4000);

            if (groupContactFlags & groupFlag) {
                fish->unk_38 = 1.0f;
                fish->unk_28 = 6.0f;
                fish->unk_34 = 2.0f;
            }

            if (D_80B7A898 != 0.0f) {
                fish->unk_38 = 1.0f;
                fish->unk_28 = 4.0f;
                fish->unk_34 = 2.0f;
            }

            Math_ApproachF(&fish->unk_28, 0.75f, 1.0f, 0.05f);

            temp1 = fish->unk_28 * spD8;
            temp2 = Math_CosS(fish->unk_3C) * temp1;

            fish->pos.x += temp2 * Math_SinS(fish->unk_3E);
            fish->pos.y += temp1 * Math_SinS(fish->unk_3C);
            fish->pos.z += temp2 * Math_CosS(fish->unk_3E);

            if (fish->shouldDraw) {
                Math_ApproachF(&fish->unk_34, 1.0f, 1.0f, 0.1f);
                Math_ApproachF(&fish->unk_38, 0.4f, 1.0f, 0.04f);
                fish->unk_30 += fish->unk_34;
                fish->unk_2C = (cosf(fish->unk_30) * fish->unk_38) + offset;
            }
        }

        fish++;
    }

    D_80B7A898 = 0.0f;
}

void Fishing_DrawGroupFishes(PlayState* play) {
    u8 flag = 0;
    FishingGroupFish* fish = &sGroupFishes[0];
    f32 scale;
    s16 i;
    s32 pad;

    if (sLinkAge == 1) {
        scale = 0.003325f;
    } else {
        scale = 0.00475f;
    }

    OPEN_DISPS(play->state.gfxCtx);

    for (i = 0; i < GROUP_FISH_COUNT; i++) {
        if (fish->type != FS_GROUP_FISH_NONE) {
            if (flag == 0) {
                gSPDisplayList(POLY_OPA_DISP++, gFishingGroupFishMaterialDL);
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 155, 155, 155, 255);
                flag++;
            }

            if (fish->shouldDraw) {
                FrameInterpolation_RecordOpenChild(fish, fish->epoch);
                Matrix_Translate(fish->pos.x, fish->pos.y, fish->pos.z, MTXMODE_NEW);
                Matrix_RotateY(((f32)fish->unk_3E * M_PI) / 32768.0f, MTXMODE_APPLY);
                Matrix_RotateX((-(f32)fish->unk_3C * M_PI) / 32768.0f, MTXMODE_APPLY);
                Matrix_Scale(fish->unk_2C * scale, scale, scale, MTXMODE_APPLY);

                gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                          G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPDisplayList(POLY_OPA_DISP++, gFishingGroupFishModelDL);
                FrameInterpolation_RecordCloseChild();
            }
        }
        fish++;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

static u16 D_80B7AFB8[] = { 0x4096, 0x408D, 0x408E, 0x408F, 0x4094, 0x4095 };

void Fishing_HandleOwnerDialog(Fishing* this, PlayState* play) {
    switch (this->unk_15C) {
        case 0:
            if (D_80B7E0AC == 0) {
                if (sLinkAge != 1) {
                    if ((HIGH_SCORE(HS_FISHING) & 0x100) && !(HIGH_SCORE(HS_FISHING) & 0x200)) {
                        this->actor.textId = 0x4093;
                    } else {
                        this->actor.textId = 0x407B;
                    }
                } else {
                    this->actor.textId = 0x407B;
                }
            } else if (D_80B7A68C == 0) {
                this->actor.textId = 0x4084;
            } else {
                this->actor.textId = 0x4097;
            }

            if (Actor_ProcessTalkRequest(&this->actor, play)) {
                if (D_80B7E0AC == 0) {
                    this->unk_15C = 1;
                    if (sLinkAge != 1) {
                        HIGH_SCORE(HS_FISHING) |= 0x200;
                    } else {
                        HIGH_SCORE(HS_FISHING) |= 0x100;
                    }
                } else {
                    this->unk_15C = 10;
                }
            } else {
                func_8002F2CC(&this->actor, play, 100.0f);
            }
            break;

        case 1:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);

                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        if (gSaveContext.rupees >= 20) {
                            Rupees_ChangeBy(-20);
                            if (func_800AA148() == 0) {
                                this->actor.textId = 0x407C;
                            } else {
                                this->actor.textId = 0x407D;
                            }
                            Message_ContinueTextbox(play, this->actor.textId);
                            this->unk_15C = 2;
                        } else {
                            Message_ContinueTextbox(play, 0x407E);
                            this->unk_15C = 3;
                        }
                        break;
                    case 1:
                        Message_ContinueTextbox(play, 0x2D);
                        this->unk_15C = 3;
                        break;
                }
            }
            break;

        case 2:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                Message_ContinueTextbox(play, 0x407F);
                this->unk_15C = 4;
            }
            break;

        case 3:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                this->unk_15C = 0;
            }
            if (Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) {
                this->unk_15C = 0;
            }
            break;

        case 4:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);

                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        D_80B7A678 = D_80B7E078;
                        Message_ContinueTextbox(play, 0x4080);
                        this->unk_15C = 5;
                        break;
                    case 1:
                        Message_ContinueTextbox(play, 0x407F);
                        break;
                }
            }
            break;

        case 5:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);

                play->interfaceCtx.unk_260 = 1;
                play->startPlayerFishing(play);
                D_80B7E0AC = 1;
                D_80B7A684 = 20;
                this->unk_15C = 0;

                if ((HIGH_SCORE(HS_FISHING) & 0xFF0000) < 0xFF0000) {
                    HIGH_SCORE(HS_FISHING) += 0x10000;
                }
            }
            break;

        case 10:
            if (D_80B7A68C != 0) {
                if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                    Message_CloseTextbox(play);

                    switch (play->msgCtx.choiceIndex) {
                        case 0:
                            Message_ContinueTextbox(play, 0x40B2);
                            D_80B7A688 = 1;
                            D_80B7A68C = 0;
                            this->unk_15C = 20;
                            break;
                        case 1:
                            this->unk_15C = 0;
                            break;
                    }
                }
            } else {
                if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                    Message_CloseTextbox(play);

                    switch (play->msgCtx.choiceIndex) {
                        case 0:
                            if (D_80B7A670 == 0.0f) {
                                this->actor.textId = 0x408C;
                                this->unk_15C = 20;
                            } else if (D_80B7E07C == 0 && !gSaveContext.n64ddFlag) {
                                D_80B7A678 = D_80B7A670;
                                if ((s16)D_80B7E078 < (s16)D_80B7A670) {
                                    if (D_80B7E07E == 2) {
                                        this->actor.textId = 0x40B0;
                                    } else {
                                        this->actor.textId = 0x4086;
                                    }
                                    this->unk_15C = 11;
                                } else {
                                    this->actor.textId = 0x408B;
                                    this->unk_15C = 20;
                                }
                            } else if (!gSaveContext.n64ddFlag) {
                                this->actor.textId = 0x409B;
                                this->unk_15C = 11;
                            }
                            else {
                                this->actor.textId = 0x4086;
                                this->unk_15C = 11;
                            }
                            Message_ContinueTextbox(play, this->actor.textId);
                            break;
                        case 1:
                            if (D_80B7A680 > 36000) {
                                D_80B7A680 = 30000;
                                Message_ContinueTextbox(play, 0x4088);
                            } else {
                                if (D_80B7E076 == 0) {
                                    if (D_80B7E082 == 0) {
                                        D_80B7E082++;
                                    }
                                }

                                if ((D_80B7E0B6 == 2) && (D_80B7AFB8[D_80B7E082] == 0x408D)) {
                                    Message_ContinueTextbox(play, 0x40AF);
                                } else {
                                    Message_ContinueTextbox(play, D_80B7AFB8[D_80B7E082]);
                                }

                                D_80B7E082++;

                                if (sLinkAge != 1) {
                                    if (D_80B7E082 >= 6) {
                                        D_80B7E082 = 0;
                                    }
                                } else {
                                    if (D_80B7E082 >= 4) {
                                        D_80B7E082 = 0;
                                    }
                                }
                            }
                            this->unk_15C = 0;
                            break;
                        case 2:
                            if (D_80B7E084 == 0) {
                                Message_ContinueTextbox(play, 0x4085);
                            } else if (sLinkAge == 1) {
                                Message_ContinueTextbox(play, 0x4092);
                            }
                            this->unk_15C = 22;
                            break;
                    }
                }
            }
            break;

        case 11:
            if (((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) ||
                 (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) &&
                Message_ShouldAdvance(play)) {
                s32 getItemId;
                GetItemEntry getItemEntry = (GetItemEntry)GET_ITEM_NONE;

                Message_CloseTextbox(play);

                if (D_80B7E07C == 0) {
                    D_80B7E078 = D_80B7A670;
                    D_80B7A670 = 0.0f;

                    if (sLinkAge == 1) {
                        f32 temp;

                        HIGH_SCORE(HS_FISHING) &= 0xFFFFFF00;
                        HIGH_SCORE(HS_FISHING) |= (s32)D_80B7E078 & 0x7F;

                        temp = (HIGH_SCORE(HS_FISHING) & 0x7F000000) >> 0x18;
                        if (temp < D_80B7E078) {
                            HIGH_SCORE(HS_FISHING) &= 0xFFFFFF;
                            HIGH_SCORE(HS_FISHING) |= ((s32)D_80B7E078 & 0x7F) << 0x18;

                            if (D_80B7E07E == 2) {
                                HIGH_SCORE(HS_FISHING) |= 0x80000000;
                            }
                        }

                        if (D_80B7E07E == 2) {
                            HIGH_SCORE(HS_FISHING) |= 0x80;
                            this->unk_15C = 0;
                            break;
                        }
                    } else {
                        HIGH_SCORE(HS_FISHING) &= 0xFFFFFF;
                        HIGH_SCORE(HS_FISHING) |= ((s32)D_80B7E078 & 0x7F) << 0x18;

                        if (D_80B7E07E == 2) {
                            HIGH_SCORE(HS_FISHING) |= 0x80000000;
                            this->unk_15C = 0;
                            break;
                        }
                    }

                    if (D_80B7E078 >= 60.0f) {
                        getItemId = GI_RUPEE_PURPLE;
                    } else if (D_80B7E078 >= 50.0f) {
                        getItemId = GI_RUPEE_RED;
                    } else if (D_80B7E078 >= 40.0f) {
                        getItemId = GI_RUPEE_BLUE;
                    } else {
                        getItemId = GI_RUPEE_GREEN;
                    }

                    if (sLinkAge == 1) {
                        if (!(HIGH_SCORE(HS_FISHING) & 0x400)) {
                            if (D_80B7E078 >= Fishing_GetMinimumRequiredScore()) {
                                HIGH_SCORE(HS_FISHING) |= 0x400;
                                sSinkingLureLocation = (u8)Rand_ZeroFloat(3.999f) + 1;
                                if (!gSaveContext.n64ddFlag) {
                                    getItemId = GI_HEART_PIECE;
                                } else {
                                    getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LH_CHILD_FISHING, GI_HEART_PIECE);
                                    getItemId = getItemEntry.getItemId;
                                }
                            }
                        }
                    } else {
                        if (!(HIGH_SCORE(HS_FISHING) & 0x800)) {
                            if (D_80B7E078 >= Fishing_GetMinimumRequiredScore()) {
                                HIGH_SCORE(HS_FISHING) |= 0x800;
                                sSinkingLureLocation = (u8)Rand_ZeroFloat(3.999f) + 1;
                                if (!gSaveContext.n64ddFlag) {
                                    getItemId = GI_SCALE_GOLD;
                                } else {
                                    getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LH_ADULT_FISHING, GI_SCALE_GOLD);
                                    getItemId = getItemEntry.getItemId;
                                }
                            }
                        }
                    }
                } else {
                    getItemId = GI_RUPEE_PURPLE;
                    D_80B7A670 = 0.0f;
                }

                this->actor.parent = NULL;
                if (!gSaveContext.n64ddFlag || getItemEntry.getItemId == GI_NONE) {
                    func_8002F434(&this->actor, play, getItemId, 2000.0f, 1000.0f);
                } else {
                    GiveItemEntryFromActor(&this->actor, play, getItemEntry, 2000.0f, 1000.0f);
                }
                this->unk_15C = 23;
            }
            break;

        case 20:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                this->unk_15C = 0;
            }
            break;

        case 21:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);

                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        this->unk_15C = 0;
                        break;
                    case 1:
                        if (D_80B7E084 == 0) {
                            Message_ContinueTextbox(play, 0x4085);
                        } else if (sLinkAge == 1) {
                            Message_ContinueTextbox(play, 0x4092);
                        }
                        this->unk_15C = 22;
                        break;
                }
            }
            break;

        case 22:
            if (play) {}

            if (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                this->unk_15C = 0;

                Player* player = GET_PLAYER(play);

                if (gSaveContext.temporaryWeapon) {
                    player->currentSwordItem = ITEM_NONE;
                    gSaveContext.equips.buttonItems[0] = ITEM_NONE;
                    Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_NONE);
                    gSaveContext.temporaryWeapon = false;
                }

                if (D_80B7A68C != 0) {
                    D_80B7A688 = 1;
                    D_80B7A68C = 0;
                }
                D_80B7E0AC = 0;
                play->interfaceCtx.unk_260 = 0;
            }
            break;

        case 23:
            D_80B7A674 = false;
            if (Actor_HasParent(&this->actor, play)) {
                this->unk_15C = 24;
            } else {
                if (!gSaveContext.n64ddFlag) {
                    func_8002F434(&this->actor, play, GI_SCALE_GOLD, 2000.0f, 1000.0f);
                } else {
                    GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LH_ADULT_FISHING, GI_SCALE_GOLD);
                    GiveItemEntryFromActor(&this->actor, play, getItemEntry, 2000.0f, 1000.0f);
                }
            }
            break;

        case 24:
            D_80B7A674 = false;
            if (((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) ||
                (gSaveContext.n64ddFlag && GET_PLAYER(play)->getItemId == GI_ICE_TRAP)) {
                if (D_80B7E07C == 0) {
                    this->unk_15C = 0;
                } else {
                    Message_StartTextbox(play, 0x409C, NULL);
                    this->unk_15C = 20;
                }
            }
            break;
    }
}

static s16 D_80B7AFC4[] = { 0, 1, 2, 2, 1 };

static Vec3f sStreamSoundPos = { 670.0f, 0.0f, -600.0f };

static Vec3s sSinkingLureLocationPos[] = {
    { -364, -30, -269 },
    { 1129, 3, -855 },
    { -480, 0, -1055 },
    { 553, -48, -508 },
};

void Fishing_UpdateOwner(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Fishing* this = (Fishing*)thisx;
    Vec3f sp114;
    Vec3f sp108;
    Vec3f spFC;
    s16 headRotTarget;
    s16 playerShadowAlpha;
    f32 target;
    f32 camAtFraction;
    f32 lureDistXZ;
    s32 pad;
    Player* player = GET_PLAYER(play);
    Input* input = &play->state.input[0];

    if (0) {
        // Strings existing only in rodata
        osSyncPrintf(VT_FGCOL(GREEN));
        osSyncPrintf(VT_FGCOL(YELLOW));
        osSyncPrintf("plays %x\n");
        osSyncPrintf("ys %x\n");
        osSyncPrintf(VT_RST);
    }

    playerShadowAlpha = player->actor.shape.shadowAlpha;

    if ((SQ(player->actor.world.pos.x) + SQ(player->actor.world.pos.z)) < SQ(920.0f)) {
        Math_ApproachS(&playerShadowAlpha, 0, 1, 40);
    } else {
        Math_ApproachS(&playerShadowAlpha, 200, 1, 40);
    }

    player->actor.shape.shadowAlpha = playerShadowAlpha;

    SkelAnime_Update(&this->skelAnime);

    if ((D_80B7A684 != 0) || (Message_GetState(&play->msgCtx) != TEXT_STATE_NONE)) {
        this->actor.flags &= ~ACTOR_FLAG_0;
    } else {
        this->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_5;
    }

    if ((this->actor.xzDistToPlayer < 120.0f) || (Message_GetState(&play->msgCtx) != TEXT_STATE_NONE)) {
        headRotTarget = this->actor.shape.rot.y - this->actor.yawTowardsPlayer;
    } else {
        headRotTarget = 0;
    }

    if (headRotTarget > 0x2710) {
        headRotTarget = 0x2710;
    } else if (headRotTarget < -0x2710) {
        headRotTarget = -0x2710;
    }

    Math_ApproachS(&this->unk_164, headRotTarget, 3, 0x1388);

    if (((play->gameplayFrames % 32) == 0) && (Rand_ZeroOne() < 0.3f)) {
        this->unk_162 = 4;
    }

    this->unk_160 = D_80B7AFC4[this->unk_162];

    if (this->unk_162 != 0) {
        this->unk_162--;
    }

    if (D_80B7A684 != 0) {
        D_80B7A684--;
    }

    if ((D_80B7A68C == 0) && (D_80B7E0B6 != 2) && (D_80B7A694 > 0) && (D_80B7A688 == 1) && (D_80B7A684 == 0)) {
        f32 dx = sOwnerHeadPos.x - sLurePos.x;
        f32 dy = sOwnerHeadPos.y - sLurePos.y;
        f32 dz = sOwnerHeadPos.z - sLurePos.z;

        if ((sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 25.0f) || (KREG(77) > 0)) {
            KREG(77) = 0;
            D_80B7A688 = 0;
            D_80B7A68C = 1;
            Message_StartTextbox(play, 0x4087, NULL);
        }
    }

    if (D_80B7A688 == 0) {
        HIGH_SCORE(HS_FISHING) |= 0x1000;
    } else if (D_80B7A688 == 1) {
        HIGH_SCORE(HS_FISHING) &= ~0x1000;
    }

    if (KREG(77) < 0) {
        KREG(77) = 0;
        D_80B7A690 = 1;
    }

    if (D_80B7A67C != 0) {
        D_80B7A67C--;
        if (D_80B7A67C == 0) {
            Message_StartTextbox(play, D_80B7E086, NULL);
        }
    }

    Fishing_HandleOwnerDialog(this, play);

    D_80B7E14C = 0.0015f;
    D_80B7A680++;

    if ((D_80B7E0AC != 0) && D_80B7A674) {
        Fishing_UpdateLure(this, play);
    }

    Fishing_UpdateEffects(play->specialEffects, play);
    Fishing_UpdatePondProps(play);
    Fishing_UpdateGroupFishes(play);

    if ((D_80B7E0AC != 0) && (D_80B7A6CC == 0) && (player->actor.world.pos.z > 1360.0f) &&
        (fabsf(player->actor.world.pos.x) < 25.0f)) {
        player->actor.world.pos.z = 1360.0f;
        player->actor.speedXZ = 0.0f;

        if (D_80B7A6D0 == 0) {
            D_80B7A6CC = 10;
        }
    }

    if ((sSinkingLureLocation != 0) &&
        (fabsf(player->actor.world.pos.x - sSinkingLureLocationPos[sSinkingLureLocation - 1].x) < 25.0f) &&
        (fabsf(player->actor.world.pos.y - sSinkingLureLocationPos[sSinkingLureLocation - 1].y) < 10.0f) &&
        (fabsf(player->actor.world.pos.z - sSinkingLureLocationPos[sSinkingLureLocation - 1].z) < 25.0f)) {
        sSinkingLureLocation = 0;
        D_80B7A6CC = 20;
        func_800A9F6C(0.0f, 150, 10, 10);
        func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
        Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x1400FF);
    }

    if (KREG(0) != 0) {
        KREG(0) = 0;
        D_80B7E0B6 = 0;
        D_80B7A6CC = 20;
        func_800A9F6C(0.0f, 150, 10, 10);
        func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
        Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x1400FF);
    }

    if (D_80B7A6D0 != 0) {
        D_80B7A6D0--;
    }

    switch (D_80B7A6CC) {
        case 0:
            break;

        case 1: {
            Camera* camera;

            sCameraId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
            Play_ChangeCameraStatus(play, sCameraId, CAM_STAT_ACTIVE);
            camera = Play_GetCamera(play, MAIN_CAM);
            sCameraEye.x = camera->eye.x;
            sCameraEye.y = camera->eye.y;
            sCameraEye.z = camera->eye.z;
            sCameraAt.x = camera->at.x;
            sCameraAt.y = camera->at.y;
            sCameraAt.z = camera->at.z;
            D_80B7A6CC = 2;
            Interface_ChangeAlpha(12);
            D_80B7FECC = 0.0f;
            // fallthrough
        }

        case 2:
            ShrinkWindow_SetVal(0x1B);

            spFC.x = sLurePos.x - player->actor.world.pos.x;
            spFC.z = sLurePos.z - player->actor.world.pos.z;
            lureDistXZ = sqrtf(SQ(spFC.x) + SQ(spFC.z));
            Matrix_RotateY(Math_Atan2F(spFC.z, spFC.x), MTXMODE_NEW);

            sp114.x = 0.0f;
            sp114.y = 0.0f;
            sp114.z = 100.0f;
            Matrix_MultVec3f(&sp114, &spFC);

            if (D_80B7A694 == 1) {
                camAtFraction = 0.2f;
            } else {
                camAtFraction = 0.1f;
            }

            Math_ApproachF(&sCameraAt.x, sLurePos.x, camAtFraction, fabsf(spFC.x) * D_80B7FECC);
            Math_ApproachF(&sCameraAt.y, sLurePos.y, camAtFraction, 50.0f * D_80B7FECC);
            Math_ApproachF(&sCameraAt.z, sLurePos.z, camAtFraction, fabsf(spFC.z) * D_80B7FECC);

            sp114.x = 0.0f - D_80B7FED0;
            if (sLinkAge != 1) {
                sp114.y = 80.0f;
            } else {
                sp114.y = 55.0f;
            }
            sp114.z = -80.0f;

            Matrix_MultVec3f(&sp114, &sp108);
            sp108.x += player->actor.world.pos.x;
            sp108.y += player->actor.world.pos.y;
            sp108.z += player->actor.world.pos.z;

            Math_ApproachF(&D_80B7FED0, 30.0f, 0.1f, 0.4f);

            if (CHECK_BTN_ALL(input->press.button, BTN_Z)) {
                if ((D_80B7E088 >= 0) && (D_80B7E122 == 0)) {
                    D_80B7E088++;

                    if (D_80B7E088 >= 4) {
                        D_80B7E088 = 0;
                    }

                    if ((D_80B7E088 == 0) || (D_80B7E088 == 3)) {
                        func_80078884(NA_SE_SY_CAMERA_ZOOM_DOWN);
                    } else {
                        func_80078884(NA_SE_SY_CAMERA_ZOOM_UP);
                    }
                }
            }

            if (D_80B7A694 >= 3) {
                if (lureDistXZ < 110.0f) {
                    D_80B7E088 = -1;
                } else if ((lureDistXZ > 300.0f) && (D_80B7E088 < 0)) {
                    D_80B7E088 = 0;
                }
            }

            if (D_80B7E088 > 0) {
                f32 dist;
                f32 offset;
                f32 factor;

                dist = sqrtf(SQ(spFC.x) + SQ(spFC.z)) * 0.001f;
                if (dist > 1.0f) {
                    dist = 1.0f;
                }
                if (D_80B7E088 == 2) {
                    offset = 0.3f;
                } else {
                    offset = 0.1f;
                }
                factor = 0.4f + offset + (dist * 0.4f);

                sp108.x += (sLurePos.x - sp108.x) * factor;
                sp108.y += ((sLurePos.y - sp108.y) * factor) + 20.0f;
                sp108.z += (sLurePos.z - sp108.z) * factor;
                D_80B7E14C = 0.0005000001f;
            }

            sp114.x = 0.0f;
            sp114.y = 0.0f;
            sp114.z = 100.0f;
            Matrix_MultVec3f(&sp114, &spFC);

            Math_ApproachF(&sCameraEye.x, sp108.x, 0.3f, fabsf(spFC.x) * D_80B7FECC);
            Math_ApproachF(&sCameraEye.y, sp108.y, 0.3f, 20.0f * D_80B7FECC);
            Math_ApproachF(&sCameraEye.z, sp108.z, 0.3f, fabsf(spFC.z) * D_80B7FECC);
            break;

        case 3: {
            Camera* camera = Play_GetCamera(play, MAIN_CAM);

            camera->eye = sCameraEye;
            camera->eyeNext = sCameraEye;
            camera->at = sCameraAt;
            func_800C08AC(play, sCameraId, 0);
            func_80064534(play, &play->csCtx);
            D_80B7A6CC = 0;
            sCameraId = 0;
            Environment_EnableUnderwaterLights(play, 0);
            play->envCtx.adjFogNear = 0;
            player->unk_860 = -5;
            D_80B7E0B0 = 5;
            break;
        }

        case 10: {
            Camera* camera;

            func_80064520(play, &play->csCtx);
            sCameraId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
            Play_ChangeCameraStatus(play, sCameraId, CAM_STAT_ACTIVE);
            func_8002DF54(play, &this->actor, 5);
            camera = Play_GetCamera(play, MAIN_CAM);
            sCameraEye.x = camera->eye.x;
            sCameraEye.y = camera->eye.y;
            sCameraEye.z = camera->eye.z;
            sCameraAt.x = camera->at.x;
            sCameraAt.y = camera->at.y;
            sCameraAt.z = camera->at.z;
            Message_StartTextbox(play, 0x409E, NULL);
            D_80B7A6CC = 11;
            func_800A9F6C(0.0f, 150, 10, 10);
            // fallthrough
        }

        case 11:
            player->actor.world.pos.z = 1360.0f;
            player->actor.speedXZ = 0.0f;

            if (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                Camera* camera = Play_GetCamera(play, MAIN_CAM);

                camera->eye = sCameraEye;
                camera->eyeNext = sCameraEye;
                camera->at = sCameraAt;
                func_800C08AC(play, sCameraId, 0);
                func_80064534(play, &play->csCtx);
                func_8002DF54(play, &this->actor, 7);
                D_80B7A6CC = 0;
                sCameraId = 0;
                D_80B7A6D0 = 30;
                Environment_EnableUnderwaterLights(play, 0);
                play->envCtx.adjFogNear = 0;
            }
            break;

        case 20: {
            Camera* camera;

            func_80064520(play, &play->csCtx);
            sCameraId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
            Play_ChangeCameraStatus(play, sCameraId, CAM_STAT_ACTIVE);
            func_8002DF54(play, &this->actor, 5);
            camera = Play_GetCamera(play, MAIN_CAM);
            sCameraEye.x = camera->eye.x;
            sCameraEye.y = camera->eye.y;
            sCameraEye.z = camera->eye.z;
            sCameraAt.x = camera->at.x;
            sCameraAt.y = camera->at.y;
            sCameraAt.z = camera->at.z;
            Message_StartTextbox(play, 0x409A, NULL);
            D_80B7A6CC = 21;
            D_80B7FEC8 = 45.0f;
            D_80B7A6D0 = 10;
            // fallthrough
        }

        case 21:
            if ((D_80B7A6D0 == 0) && Message_ShouldAdvance(play)) {
                D_80B7A6CC = 22;
                D_80B7A6D0 = 40;
                func_8002DF54(play, &this->actor, 0x1C);
                D_80B7FEE4 = 0.0f;
            }
            break;

        case 22:
            if (D_80B7A6D0 == 30) {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_ITEM_GET | 0x900);
            }

            D_80B7A6D4 = 1;

            Math_ApproachF(&D_80B7FEE4, 71.0f, 0.5f, 3.0f);
            Matrix_RotateY((player->actor.shape.rot.y / 32768.0f) * M_PI, MTXMODE_NEW);

            sp114.x = Math_SinS(play->gameplayFrames * 0x1000);
            sp114.y = D_80B7FEE4;
            sp114.z = -5.0f;
            if (sLinkAge == 1) {
                sp114.y -= 20.0f;
            }

            Matrix_MultVec3f(&sp114, &sp108);

            sSinkingLureBasePos.x = player->actor.world.pos.x + sp108.x;
            sSinkingLureBasePos.y = player->actor.world.pos.y + sp108.y;
            sSinkingLureBasePos.z = player->actor.world.pos.z + sp108.z;

            Math_ApproachF(&D_80B7FEC8, 15.0f, 0.1f, 0.75f);

            sp114.x = D_80B7FEC8 - 15.0f;

            if (sLinkAge != 1) {
                sp114.y = 60.0f;
                sp114.z = -30.0f;
            } else {
                sp114.y = 40.0f;
                sp114.z = -35.0f;
            }

            Matrix_MultVec3f(&sp114, &sCameraEye);
            sCameraEye.x += player->actor.world.pos.x;
            sCameraEye.y += player->actor.world.pos.y;
            sCameraEye.z += player->actor.world.pos.z;

            sCameraAt = player->actor.world.pos;
            if (sLinkAge != 1) {
                sCameraAt.y += 62.0f;
            } else {
                sCameraAt.y += 40.0f;
            }

            if (D_80B7A6D0 == 0) {
                if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) ||
                    (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE)) {
                    if (Message_ShouldAdvance(play)) {
                        Camera* camera = Play_GetCamera(play, MAIN_CAM);

                        Message_CloseTextbox(play);
                        if (play->msgCtx.choiceIndex == 0) {
                            D_80B7E0B6 = 2;
                            D_80B7E082 = 0;
                        }

                        camera->eye = sCameraEye;
                        camera->eyeNext = sCameraEye;
                        camera->at = sCameraAt;
                        func_800C08AC(play, sCameraId, 0);
                        func_80064534(play, &play->csCtx);
                        func_8002DF54(play, &this->actor, 7);
                        D_80B7A6CC = 0;
                        sCameraId = 0;
                        player->unk_860 = -5;
                        D_80B7E0B0 = 5;
                        D_80B7A6D4 = 0;
                        D_80B7E0A6 = 20;
                        Environment_EnableUnderwaterLights(play, 0);
                        play->envCtx.adjFogNear = 0;
                    }
                }
            }
            break;

        case 100:
            break;
    }

    if (sCameraId != 0) {
        Play_CameraSetAtEye(play, sCameraId, &sCameraAt, &sCameraEye);
        Math_ApproachF(&D_80B7FECC, 1.0f, 1.0f, 0.02f);

        if (sCameraEye.y <= (WATER_SURFACE_Y(play) + 1.0f)) {
            Environment_EnableUnderwaterLights(play, 1);
            if (D_80B7E076 != 0) {
                play->envCtx.adjFogNear = -0xB2;
            } else {
                play->envCtx.adjFogNear = -0x2E;
            }
        } else {
            Environment_EnableUnderwaterLights(play, 0);
            play->envCtx.adjFogNear = 0;
        }
    }

    if ((player->actor.floorHeight < (WATER_SURFACE_Y(play) - 3.0f)) &&
        (player->actor.world.pos.y < (player->actor.floorHeight + 3.0f)) && (player->actor.speedXZ > 1.0f) &&
        ((play->gameplayFrames % 2) == 0)) {
        Vec3f pos;

        pos.x = Rand_CenteredFloat(20.0f) + player->actor.world.pos.x;
        pos.z = Rand_CenteredFloat(20.0f) + player->actor.world.pos.z;
        pos.y = player->actor.floorHeight + 5.0f;
        Fishing_SpawnWaterDust(NULL, play->specialEffects, &pos, 0.5f);
    }

    if ((player->actor.floorHeight < WATER_SURFACE_Y(play)) &&
        (player->actor.floorHeight > (WATER_SURFACE_Y(play) - 10.0f)) && (player->actor.speedXZ >= 4.0f) &&
        ((play->gameplayFrames % 4) == 0)) {
        s16 i;

        for (i = 0; i < 10; i++) {
            Vec3f pos;
            Vec3f vel;
            f32 speedXZ;
            f32 angle;

            speedXZ = Rand_ZeroFloat(1.5f) + 1.5f;
            angle = Rand_ZeroFloat(6.28f);

            vel.x = sinf(angle) * speedXZ;
            vel.z = cosf(angle) * speedXZ;
            vel.y = Rand_ZeroFloat(3.0f) + 2.0f;

            pos = player->actor.world.pos;
            pos.x += 2.0f * vel.x;
            pos.y = WATER_SURFACE_Y(play);
            pos.z += 2.0f * vel.z;
            Fishing_SpawnDustSplash(NULL, play->specialEffects, &pos, &vel, Rand_ZeroFloat(0.01f) + 0.020000001f);
        }
    }

    if (sREG(15) != 0) {
        if (D_80B7A654 != (sREG(15) - 1)) {
            if (D_80B7A654 == 0) {
                play->envCtx.gloomySkyMode = 1;
            } else {
                play->envCtx.gloomySkyMode = 2;
            }
        }

        D_80B7A654 = sREG(15) - 1;
    }

    if (sREG(14) == 1) {
        play->envCtx.gloomySkyMode = 1;
    }
    if (sREG(14) == -1) {
        play->envCtx.gloomySkyMode = 2;
    }

    sREG(14) = 0;

    osSyncPrintf(VT_FGCOL(GREEN));
    osSyncPrintf("zelda_time %x\n", ((void)0, gSaveContext.dayTime));
    osSyncPrintf(VT_RST);

    if (D_80B7E077 >= 2) {
        D_80B7E077--;
    }

    if ((D_80B7E077 == 1) && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) &&
        ((D_80B7A680 & 0xFFF) == 0xFFF)) {
        D_80B7E077 = 200;

        if (Rand_ZeroOne() < 0.5f) {
            D_80B7A654 = (u8)Rand_ZeroFloat(10.0f) + 5;
            play->envCtx.gloomySkyMode = 1;
        } else {
            D_80B7A654 = 0;
            play->envCtx.gloomySkyMode = 2;
        }
    }

    Math_ApproachF(&D_80B7A650, D_80B7A654, 1.0f, 0.05f);

    if (D_80B7A650 > 0.0f) {
        target = (D_80B7A650 * 0.03f) + 0.8f;
        if (target > 1.2f) {
            target = 1.2f;
        }
        Math_ApproachF(&D_80B7A668, target, 1.0f, 0.01f);
    }

    target = (10.0f - D_80B7A650) * 150.1f;
    if (target < 0.0f) {
        target = 0.0f;
    }
    Math_ApproachF(&D_80B7A65C.z, target, 1.0f, 5.0f);

    if (D_80B7A65C.z < 1500.0f) {
        func_800F436C(&D_80B7A65C, NA_SE_EV_RAIN - SFX_FLAG, D_80B7A668);
    }

    if (D_80B7A654 != 0) {
        Math_ApproachF(&D_80B7A658, -200.0f, 1.0f, 2.0f);
    } else {
        Math_ApproachZeroF(&D_80B7A658, 1.0f, 2.0f);
    }

    play->envCtx.adjLight1Color[0] = play->envCtx.adjLight1Color[1] = play->envCtx.adjLight1Color[2] =
        D_80B7A658;

    if ((u8)D_80B7A650 > 0) {
        s32 pad;
        Camera* camera = Play_GetCamera(play, MAIN_CAM);
        s16 i;
        s32 pad1;
        Vec3f pos;
        Vec3f rot;
        Vec3f projectedPos;
        s32 pad2;

        rot.x = M_PI / 2.0f + 0.1f;
        rot.y = 1.0f;
        rot.z = (Camera_GetInputDirYaw(camera) * -(M_PI / 32768)) + rot.y;

        for (i = 0; i < (u8)D_80B7A650; i++) {
            pos.x = Rand_CenteredFloat(700.0f) + play->view.eye.x;
            pos.y = (Rand_ZeroFloat(100.0f) + 150.0f) - 170.0f;
            pos.z = Rand_CenteredFloat(700.0f) + play->view.eye.z;

            if (pos.z < 1160.0f) {
                SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &pos, &projectedPos, &sProjectedW);

                if (projectedPos.z < 0.0f) {
                    i--;
                } else {
                    Fishing_SpawnRainDrop(play->specialEffects, &pos, &rot);
                }
            }
        }
    }

    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &sStreamSoundPos, &sStreamSoundProjectedPos,
                                 &sProjectedW);

    func_80078914(&sStreamSoundProjectedPos, NA_SE_EV_WATER_WALL - SFX_FLAG);

    gSaveContext.minigameScore = (SQ((f32)D_80B7A678) * 0.0036f) + 0.5f;

    if (BREG(26) != 0) {
        BREG(26) = 0;
        Message_StartTextbox(play, 0x407B + BREG(27), NULL);
    }

    osSyncPrintf("HI_SCORE = %x\n", HIGH_SCORE(HS_FISHING));
}

s32 Fishing_OwnerOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                  void* thisx) {
    Fishing* this = (Fishing*)thisx;

    if (limbIndex == 8) { // Head
        rot->x -= this->unk_164;
    }

    return 0;
}

void Fishing_OwnerPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    if (limbIndex == 8) { // Head
        OPEN_DISPS(play->state.gfxCtx);
        Matrix_MultVec3f(&sZeroVec, &sOwnerHeadPos);

        if (D_80B7A688 == 1) {
            gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gFishingOwnerHatDL));
        } else if (D_80B7A688 == 2) {
            gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gFishingOwnerHairDL));
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

static void* sFishingOwnerEyeTexs[] = {
    gFishingOwnerEyeOpenTex,
    gFishingOwnerEyeHalfTex,
    gFishingOwnerEyeClosedTex,
};

void Fishing_DrawOwner(Actor* thisx, PlayState* play) {
    s32 pad;
    Fishing* this = (Fishing*)thisx;
    Input* input = &play->state.input[0];

    OPEN_DISPS(play->state.gfxCtx);

    func_80093D18(play->state.gfxCtx);
    func_80093D84(play->state.gfxCtx);

    if ((this->actor.projectedPos.z < 1500.0f) &&
        (fabsf(this->actor.projectedPos.x) < (100.0f + this->actor.projectedPos.z))) {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sFishingOwnerEyeTexs[this->unk_160]));

        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                              this->skelAnime.dListCount, Fishing_OwnerOverrideLimbDraw, Fishing_OwnerPostLimbDraw,
                              this);
    }

    Fishing_DrawPondProps(play);
    Fishing_DrawEffects(play->specialEffects, play);
    Fishing_DrawGroupFishes(play);
    Fishing_DrawStreamSplash(play);

    if (D_80B7E0A6 != 0) {
        D_80B7E0A6--;

        if (D_80B7E0A6 == 0) {
            if (sLinkAge != 1) {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_KAKARIKO_ADULT);
            } else {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_KAKARIKO_KID);
            }

            if (sLinkAge != 1) {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_KAKARIKO_ADULT);
            } else {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_KAKARIKO_KID);
            }
        }
    }

    if ((D_80B7E0AC != 0) && D_80B7A674) {
        Fishing_DrawRod(play);
        Fishing_UpdateLinePos(sReelLinePos);
        Fishing_UpdateLine(play, &sRodTipPos, sReelLinePos, sReelLineRot, sReelLineUnk);
        Fishing_DrawLureAndLine(play, sReelLinePos, sReelLineRot);

        D_80B7A6C4 = input->rel.stick_x;
        D_80B7A6C8 = input->rel.stick_y;
    }

    D_80B7A674 = true;

    Matrix_Translate(130.0f, 40.0f, 1300.0f, MTXMODE_NEW);
    Matrix_Scale(0.08f, 0.12f, 0.14f, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, gFishingAquariumBottomDL);
    gSPDisplayList(POLY_XLU_DISP++, gFishingAquariumContainerDL);

    if ((D_80B7E0AC != 0) && (D_80B7E0B6 == 2)) {
        Fishing_DrawSinkingLure(play);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
