/*
 * File: z_en_fishing.c
 * Overlay: ovl_En_Fishing
 * Description: Fishing Pond Elements (Owner, Fish, Props, Effects...)
 */

#include "z_en_fishing.h"
#include "z64rumble.h"
#include "z64shrink_window.h"
#include "objects/object_fish/object_fish.h"
#include "overlays/actors/ovl_En_Kanban/z_en_kanban.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

#define WATER_SURFACE_Y(play) play->colCtx.colHeader->waterBoxes->minPos.y

void EnFishing_Init(Actor* thisx, PlayState* play2);
void EnFishing_Destroy(Actor* thisx, PlayState* play2);
void EnFishing_UpdateFish(Actor* thisx, PlayState* play2);
void EnFishing_DrawFish(Actor* thisx, PlayState* play);

void EnFishing_UpdateOwner(Actor* thisx, PlayState* play2);
void EnFishing_DrawOwner(Actor* thisx, PlayState* play);

typedef struct {
    /* 0x0 */ u8 unk_00;
    /* 0x2 */ Vec3s pos;
    /* 0x8 */ u8 unk_08;
    /* 0xC */ f32 unk_0C;
} FishingFishInit; // size = 0x10

#define EFFECT_COUNT 130

typedef enum {
    /* 0 */ FS_EFF_NONE,
    /* 1 */ FS_EFF_RIPPLE,
    /* 2 */ FS_EFF_DUST_SPLASH,
    /* 3 */ FS_EFF_WATER_DUST,
    /* 4 */ FS_EFF_BUBBLE,
    /* 5 */ FS_EFF_RAIN_DROP,
    /* 6 */ FS_EFF_OWNER_HAT,
    /* 7 */ FS_EFF_RAIN_RIPPLE,
    /* 8 */ FS_EFF_RAIN_SPLASH
} FishingEffectType;

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ u8 type;
    /* 0x25 */ u8 timer;
    /* 0x26 */ UNK_TYPE1 unk_26[0x04];
    /* 0x2A */ s16 alpha;
    /* 0x2C */ s16 unk_2C;
    /* 0x2E */ s16 unk_2E;
    /* 0x30 */ f32 unk_30;
    /* 0x34 */ f32 unk_34;
    /* 0x38 */ f32 unk_38;
    /* 0x3C */ f32 unk_3C;
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
    /* 0x0 */ u8 type;
    /* 0x2 */ Vec3s pos;
} FishingPropInit; // size = 0x8

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
} FishingProp; // size = 0x3C

typedef enum {
    /* 0 */ FS_GROUP_FISH_NONE,
    /* 1 */ FS_GROUP_FISH_NORMAL
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
} FishingGroupFish; // size = 0x48

#define LINE_SEG_COUNT 200
#define SINKING_LURE_SEG_COUNT 20

f32 D_809101B0;
f32 D_809101B4;
s16 D_809101B8;
f32 D_809101BC;
f32 D_809101C0;
f32 D_809101C4;
f32 D_809101C8;
s16 D_809101CC;
f32 D_809101D0;
Vec3f sRodTipPos;
Vec3f sReelLinePos[LINE_SEG_COUNT];
Vec3f sReelLineRot[LINE_SEG_COUNT];
Vec3f sReelLineUnk[LINE_SEG_COUNT];
Vec3f sLureHookRefPos[2];
f32 sLureHookRotY[2];
u8 D_80911E28;
Vec3f sSinkingLurePos[SINKING_LURE_SEG_COUNT];
s16 D_80911F20;
f32 sProjectedW;
Vec3f sSubCamEye;
Vec3f sSubCamAt;
s16 sSubCamId;
f32 D_80911F48;
f32 sSubCamVelFactor;
f32 D_80911F50;
Vec3f sSinkingLureBasePos;
f32 D_80911F64;
s32 sFishingRandSeed1;
s32 sFishingRandSeed2;
s32 sFishingRandSeed3;
FishingProp sPondProps[POND_PROP_COUNT];
FishingGroupFish sGroupFishes[GROUP_FISH_COUNT];
f32 sFishGroupAngle1;
f32 sFishGroupAngle2;
f32 sFishGroupAngle3;
FishingEffect sFishingEffects[EFFECT_COUNT];
Vec3f sStreamSoundProjectedPos;
EnFishing* sFishingMain;
u8 D_809171C8;
u8 sLinkAge;
u8 D_809171CA;
u8 D_809171CB;
f32 D_809171CC;
u8 D_809171D0;
u8 D_809171D1;
u8 D_809171D2;
s16 D_809171D4;
u8 D_809171D6;
u16 D_809171D8;
u16 D_809171DA;
s8 D_809171DC;
Vec3f sOwnerHeadPos;
Vec3s sEffOwnerHatRot;
u8 D_809171F2;
s16 D_809171F4;
s16 D_809171F6;
EnFishing* sFishingHookedFish;
s16 D_809171FC;
s16 D_809171FE;
s16 D_80917200;
s16 D_80917202;
s16 D_80917204;
u8 D_80917206;
Vec3f sLurePos;
Vec3f D_80917218;
Vec3f sLureRot;
Vec3f D_80917238;
Vec3f D_80917248;
f32 D_80917254;
f32 D_80917258;
f32 D_8091725C;
f32 D_80917260;
s8 D_80917264;
s16 D_80917266;
u8 D_80917268;
f32 D_8091726C;
u8 D_80917270;
s16 D_80917272;
u8 D_80917274;
Vec3f D_80917278;

ActorProfile En_Fishing_Profile = {
    /**/ ACTOR_EN_FISHING,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_FISH,
    /**/ sizeof(EnFishing),
    /**/ EnFishing_Init,
    /**/ EnFishing_Destroy,
    /**/ EnFishing_UpdateFish,
    /**/ EnFishing_DrawFish,
};

f32 D_8090CCD0 = 0.0f;
u8 D_8090CCD4 = 0;
f32 D_8090CCD8 = 0.0f;
Vec3f D_8090CCDC = { 0.0f, 0.0f, 0.0f };
f32 D_8090CCE8 = 0.0f;
u8 sSinkingLureLocation = 0;
f32 D_8090CCF0 = 0.0f;
u8 D_8090CCF4 = true;
u16 D_8090CCF8 = 0;
u8 D_8090CCFC = 0;
s32 D_8090CD00 = 0;
s16 D_8090CD04 = 0;
u8 D_8090CD08 = 0;
u8 D_8090CD0C = false;
u8 D_8090CD10 = false;
s16 D_8090CD14 = 0;
Vec3f sFishMouthOffset = { 500.0f, 500.0f, 0.0f };
u8 D_8090CD24 = 0;
f32 D_8090CD28 = 0;
f32 D_8090CD2C = 0;
f32 D_8090CD30 = 0.0f;
f32 D_8090CD34 = 0.0f;
f32 D_8090CD38 = 0.0f;
f32 D_8090CD3C = 0.0f;
f32 D_8090CD40 = 0.0f;
s16 D_8090CD44 = 0;
s16 D_8090CD48 = 0;
u8 D_8090CD4C = 0;
u8 D_8090CD50 = 0;
u8 D_8090CD54 = 0;

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE | AT_TYPE_ENEMY,
        AC_NONE | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

u8 D_8090CF18 = 0;
static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };
Vec3f D_8090CF28 = { 0.0f, 0.0f, 2000.0f }; // Unused

void EnFishing_SetColliderElement(s32 index, ColliderJntSph* collider, Vec3f* pos, f32 scale) {
    collider->elements[index].dim.worldSphere.center.x = pos->x;
    collider->elements[index].dim.worldSphere.center.y = pos->y;
    collider->elements[index].dim.worldSphere.center.z = pos->z;
    collider->elements[index].dim.worldSphere.radius =
        collider->elements[index].dim.modelSphere.radius * collider->elements[index].dim.scale * scale * 1.6f;
}

void EnFishing_InitRand(s32 seedInit1, s32 seedInit2, s32 seedInit3) {
    sFishingRandSeed1 = seedInit1;
    sFishingRandSeed2 = seedInit2;
    sFishingRandSeed3 = seedInit3;
}

f32 EnFishing_RandZeroOne(void) {
    // Wichmann-Hill algorithm
    f32 randFloat;

    sFishingRandSeed1 = (sFishingRandSeed1 * 171) % 30269;
    sFishingRandSeed2 = (sFishingRandSeed2 * 172) % 30307;
    sFishingRandSeed3 = (sFishingRandSeed3 * 170) % 30323;

    randFloat = (sFishingRandSeed1 / 30269.0f) + (sFishingRandSeed2 / 30307.0f) + (sFishingRandSeed3 / 30323.0f);
    while (randFloat >= 1.0f) {
        randFloat -= 1.0f;
    }

    return fabsf(randFloat);
}

s16 EnFishing_SmoothStepToS(s16* pValue, s16 target, s16 scale, s16 step) {
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

void EnFishing_SpawnRipple(Vec3f* projectedPos, FishingEffect* effect, Vec3f* pos, f32 arg3, f32 arg4, s16 arg5,
                           s16 countLimit) {
    s16 i;

    if ((projectedPos != NULL) && ((projectedPos->z > 500.0f) || (projectedPos->z < 0.0f))) {
        return;
    }

    for (i = 0; i < countLimit; i++) {
        if (effect->type == FS_EFF_NONE) {
            effect->type = FS_EFF_RIPPLE;
            effect->pos = *pos;
            effect->velocity = sZeroVec;
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
            break;
        }

        effect++;
    }
}

void EnFishing_SpawnDustSplash(Vec3f* projectedPos, FishingEffect* effect, Vec3f* pos, Vec3f* velocity, f32 scale) {
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
            effect->velocity = *velocity;
            effect->accel = accel;
            effect->alpha = 100 + Rand_ZeroFloat(100.0f);
            effect->unk_30 = scale;
            break;
        }

        effect++;
    }
}

void EnFishing_SpawnWaterDust(Vec3f* projectedPos, FishingEffect* effect, Vec3f* pos, f32 scale) {
    s16 i;
    Vec3f accel = { 0.0f, 0.05f, 0.0f };

    if ((projectedPos != NULL) && ((projectedPos->z > 500.0f) || (projectedPos->z < 0.0f))) {
        return;
    }

    for (i = 0; i < 90; i++) {
        if (effect->type == FS_EFF_NONE) {
            effect->type = FS_EFF_WATER_DUST;
            effect->pos = *pos;
            effect->velocity = sZeroVec;
            effect->accel = accel;
            effect->alpha = 255;
            effect->timer = Rand_ZeroFloat(100.0f);
            effect->unk_30 = scale;
            effect->unk_34 = 2.0f * scale;
            break;
        }

        effect++;
    }
}

void EnFishing_SpawnBubble(Vec3f* projectedPos, FishingEffect* effect, Vec3f* pos, f32 scale, u8 arg4) {
    s16 i;
    Vec3f velocity = { 0.0f, 1.0f, 0.0f };

    if ((projectedPos != NULL) && ((projectedPos->z > 500.0f) || (projectedPos->z < 0.0f))) {
        return;
    }

    for (i = 0; i < 90; i++) {
        if (effect->type == FS_EFF_NONE) {
            effect->type = FS_EFF_BUBBLE;
            effect->pos = *pos;
            effect->velocity = velocity;
            effect->accel = sZeroVec;
            effect->timer = Rand_ZeroFloat(100.0f);
            effect->unk_30 = scale;
            effect->unk_2C = arg4;
            break;
        }

        effect++;
    }
}

void EnFishing_SpawnRainDrop(FishingEffect* effect, Vec3f* pos, Vec3f* rot) {
    s16 i;
    Vec3f velocity;

    velocity.x = 0.0f;
    velocity.y = 0.0f;
    velocity.z = 300.0f;

    effect += 30;

    for (i = 30; i < EFFECT_COUNT; i++) {
        if (effect->type == FS_EFF_NONE) {
            effect->type = FS_EFF_RAIN_DROP;
            effect->pos = *pos;
            effect->accel = sZeroVec;
            effect->unk_34 = rot->x;
            effect->unk_38 = rot->y;
            effect->unk_3C = rot->z;
            Matrix_RotateYF(rot->y, MTXMODE_NEW);
            Matrix_RotateXFApply(rot->x);
            Matrix_MultVec3f(&velocity, &effect->velocity);
            break;
        }

        effect++;
    }
}

FishingPropInit sPondPropInits[POND_PROP_COUNT + 1] = {
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

void EnFishing_InitPondProps(EnFishing* this, PlayState* play) {
    FishingProp* prop = &sPondProps[0];
    Vec3f colliderPos;
    s16 i;

    EnFishing_InitRand(1, 29100, 9786);

    for (i = 0; i < POND_PROP_COUNT; i++) {
        if (sPondPropInits[i].type == FS_PROP_INIT_STOP) {
            break;
        }

        prop->type = sPondPropInits[i].type;
        prop->pos.x = sPondPropInits[i].pos.x;
        prop->pos.y = sPondPropInits[i].pos.y;
        prop->pos.z = sPondPropInits[i].pos.z;
        prop->rotX = 0.0f;
        prop->reedAngle = 0.0f;

        prop->timer = Rand_ZeroFloat(100.0f);
        prop->drawDistance = 800.0f;

        if (prop->type == FS_PROP_REED) {
            prop->scale = (EnFishing_RandZeroOne() * 0.25f) + 0.75f;
            prop->reedAngle = Rand_ZeroFloat(2 * M_PIf);
            if (sLinkAge == 1) {
                prop->scale *= 0.6f;
            }
            prop->drawDistance = 1200.0f;
        } else if (prop->type == FS_PROP_WOOD_POST) {
            prop->scale = 0.08f;
            prop->drawDistance = 1200.0f;
            colliderPos = prop->pos;
            colliderPos.y += 50.0f;
            EnFishing_SetColliderElement(i, &sFishingMain->collider, &colliderPos, prop->scale * 3.5f);
        } else if (prop->type == FS_PROP_LILY_PAD) {
            prop->scale = (EnFishing_RandZeroOne() * 0.3f) + 0.5f;
            prop->rotY = Rand_ZeroFloat(2 * M_PIf);
            if (sLinkAge == 1) {
                if ((i % 4) != 0) {
                    prop->scale *= 0.6f;
                } else {
                    prop->type = FS_PROP_NONE;
                }
            }
        } else {
            prop->scale = (EnFishing_RandZeroOne() * 0.1f) + 0.3f;
            prop->rotY = Rand_ZeroFloat(2 * M_PIf);
            prop->drawDistance = 1000.0f;
            EnFishing_SetColliderElement(i, &sFishingMain->collider, &prop->pos, prop->scale);
        }

        prop++;
    }
}

FishingFishInit sFishInits[] = {
    { 0, { 666, -45, 354 }, 38, 0.1f },    { 0, { 681, -45, 240 }, 36, 0.1f },   { 0, { 670, -45, 90 }, 41, 0.05f },
    { 0, { 615, -45, -450 }, 35, 0.2f },   { 0, { 500, -45, -420 }, 39, 0.1f },  { 0, { 420, -45, -550 }, 44, 0.05f },
    { 0, { -264, -45, -640 }, 40, 0.1f },  { 0, { -470, -45, -540 }, 34, 0.2f }, { 0, { -557, -45, -430 }, 54, 0.01f },
    { 0, { -260, -60, -330 }, 47, 0.05f }, { 0, { -500, -60, 330 }, 42, 0.06f }, { 0, { 428, -40, -283 }, 33, 0.2f },
    { 0, { 409, -70, -230 }, 57, 0.0f },   { 0, { 450, -67, -300 }, 63, 0.0f },  { 0, { -136, -65, -196 }, 71, 0.0f },
    { 1, { -561, -35, -547 }, 45, 0.0f },  { 1, { 667, -35, 317 }, 43, 0.0f },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 0, ICHAIN_STOP),
};

void EnFishing_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnFishing* this = (EnFishing*)thisx;
    u16 fishCount;

    Actor_ProcessInitChain(thisx, sInitChain);
    ActorShape_Init(&thisx->shape, 0.0f, NULL, 0.0f);

    sLinkAge = gSaveContext.save.linkAge;

    if (thisx->params < 100) {
        s16 i;
        FishingGroupFish* fish;

        D_809171C8 = false;
        sFishingMain = this;
        Collider_InitJntSph(play, &sFishingMain->collider);
        Collider_SetJntSph(play, &sFishingMain->collider, thisx, &sJntSphInit, sFishingMain->colliderElements);

        thisx->params = 1;

        SkelAnime_InitFlex(play, &this->skelAnime, &gFishingOwnerSkel, &gFishingOwnerAnim, NULL, NULL, 0);
        Animation_MorphToLoop(&this->skelAnime, &gFishingOwnerAnim, 0.0f);

        thisx->update = EnFishing_UpdateOwner;
        thisx->draw = EnFishing_DrawOwner;

        thisx->shape.rot.y = -0x6000;
        thisx->world.pos.x = 160.0f;
        thisx->world.pos.y = -2.0f;
        thisx->world.pos.z = 1208.0f;

        Actor_SetScale(thisx, 0.011f);

        thisx->focus.pos = thisx->world.pos;
        thisx->focus.pos.y += 75.0f;
        thisx->flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);

        if (sLinkAge != 1) {
            if (HIGH_SCORE(HS_FISHING) & 0x1000) {
                D_8090CD08 = 0;
            } else {
                D_8090CD08 = 1;
            }
        } else {
            D_8090CD08 = 2;
        }

        D_8090CD04 = 20;
        play->specialEffects = sFishingEffects;
        R_TIME_SPEED = 1;
        D_809171FC = 0;
        D_809171F6 = 10;

        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);

        if (sLinkAge == 1) {
            if (HIGH_SCORE(HS_FISHING) & 0x7F) {
                D_809171CC = HIGH_SCORE(HS_FISHING) & 0x7F;
            } else {
                D_809171CC = 40.0f;
            }
        } else if (HIGH_SCORE(HS_FISHING) & 0x7F000000) {
            D_809171CC = (HIGH_SCORE(HS_FISHING) & 0x7F000000) >> 0x18;
        } else {
            D_809171CC = 45.0f;
        }

        D_809171D1 = (HIGH_SCORE(HS_FISHING) & 0xFF0000) >> 0x10;
        if ((D_809171D1 & 7) == 7) {
            play->roomCtx.unk7A[0] = 90;
            D_809171CA = true;
        } else {
            play->roomCtx.unk7A[0] = 40;
            D_809171CA = false;
        }

        if ((D_809171D1 & 7) == 6) {
            D_809171CB = 100;
        } else {
            D_809171CB = 0;
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
            fish = &sGroupFishes[i];

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

        EnFishing_InitPondProps(this, play);
        Actor_SpawnAsChild(&play->actorCtx, thisx, play, ACTOR_EN_KANBAN, 53.0f, -17.0f, 982.0f, 0, 0, 0,
                           ENKANBAN_FISHING);
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_FISHING, 0.0f, 0.0f, 0.0f, 0, 0, 0, 200);

        if ((D_809171D1 & 3) == 3) {
            if (sLinkAge != 1) {
                fishCount = 16;
            } else {
                fishCount = 17;
            }
        } else {
            fishCount = 15;
        }

        for (i = 0; i < fishCount; i++) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_FISHING, sFishInits[i].pos.x, sFishInits[i].pos.y,
                        sFishInits[i].pos.z, 0, Rand_ZeroFloat(0x10000), 0, 100 + i);
        }

        return;
    }

    thisx->bgCheckFlags |= BGCHECKFLAG_PLAYER_800; // Added in MM

    if ((thisx->params < 115) || (thisx->params == 200)) {
        SkelAnime_InitFlex(play, &this->skelAnime, &gFishingBassSkel, &gFishingBassAnim, NULL, NULL, 0);
        Animation_MorphToLoop(&this->skelAnime, &gFishingBassAnim, 0.0f);
    } else {
        SkelAnime_InitFlex(play, &this->skelAnime, &gFishingLoachSkel, &gFishingLoachAnim, NULL, NULL, 0);
        Animation_MorphToLoop(&this->skelAnime, &gFishingLoachAnim, 0.0f);
    }

    SkelAnime_Update(&this->skelAnime);

    if (thisx->params == 200) {
        this->unk_150 = 100;
        Actor_ChangeCategory(play, &play->actorCtx, thisx, ACTORCAT_PROP);
        thisx->attentionRangeType = ATTENTION_RANGE_0;
        thisx->flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
    } else {
        this->unk_150 = 10;
        this->unk_152 = 10;

        this->unk_148 = sFishInits[thisx->params - 100].unk_00;
        this->unk_1A0 = sFishInits[thisx->params - 100].unk_0C;
        this->unk_1A4 = sFishInits[thisx->params - 100].unk_08;

        this->unk_1A4 += Rand_ZeroFloat(4.99999f);

        if ((this->unk_1A4 >= 65.0f) && (Rand_ZeroOne() < 0.05f)) {
            this->unk_1A4 += Rand_ZeroFloat(7.99999f);
        }

        if (sLinkAge == 1) {
            this->unk_1A4 *= 0.73f;
        }
    }
}

void EnFishing_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnFishing* this = (EnFishing*)thisx;

    SkelAnime_Free(&this->skelAnime, play);

    if (thisx->params == 200) {
        LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
    } else if (thisx->params == 1) {
        Collider_DestroyJntSph(play, &this->collider);
    }
}

void EnFishing_UpdateEffects(FishingEffect* effect, PlayState* play) {
    f32 rippleY;
    s16 i;

    for (i = 0; i < EFFECT_COUNT; i++) {
        if (effect->type) {
            effect->timer++;
            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;
            effect->velocity.y += effect->accel.y;

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
                    effect->velocity.y = 0.0f;
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
                        EnFishing_SpawnRipple(NULL, play->specialEffects, &pos, 20.0f, 60.0f, 150, 90);
                    }
                }
            } else if (effect->type == FS_EFF_DUST_SPLASH) {
                if (effect->velocity.y < -20.0f) {
                    effect->velocity.y = -20.0f;
                    effect->accel.y = 0.0f;
                }

                if (effect->pos.y <= WATER_SURFACE_Y(play)) {
                    effect->type = FS_EFF_NONE;
                    if (Rand_ZeroOne() < 0.5f) {
                        Vec3f pos = effect->pos;

                        pos.y = WATER_SURFACE_Y(play);
                        EnFishing_SpawnRipple(NULL, play->specialEffects, &pos, 40.0f, 110.0f, 150, 90);
                    }
                }
            } else if (effect->type == FS_EFF_RAIN_DROP) {
                if (effect->pos.y < WATER_SURFACE_Y(play)) {
                    f32 sqDistXZ = SQXZ(effect->pos);

                    if (sqDistXZ > SQ(920.0f)) {
                        effect->pos.y = WATER_SURFACE_Y(play) + ((sqrtf(sqDistXZ) - 920.0f) * 0.11f);
                        effect->timer = 2;
                        effect->type = FS_EFF_RAIN_SPLASH;
                        effect->unk_30 = (KREG(18) + 30) * 0.001f;
                    } else {
                        effect->pos.y = WATER_SURFACE_Y(play) + 3.0f;
                        effect->timer = 0;
                        if (Rand_ZeroOne() < 0.75f) {
                            effect->type = FS_EFF_RAIN_RIPPLE;
                            effect->velocity = sZeroVec;
                            effect->unk_30 = 30 * 0.001f;
                        } else {
                            effect->type = FS_EFF_NONE;
                        }
                    }

                    effect->velocity = sZeroVec;
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

                sqDistXZ = SQXZ(effect->pos);
                bottomY = WATER_SURFACE_Y(play) + ((sqrtf(sqDistXZ) - 920.0f) * 0.147f);

                if (effect->pos.y > (bottomY - 10.0f)) {
                    effect->pos.y -= 0.1f;
                }

                if ((effect->timer % 16) == 0) {
                    Vec3f pos = effect->pos;

                    pos.y = WATER_SURFACE_Y(play);
                    EnFishing_SpawnRipple(NULL, play->specialEffects, &pos, 30.0f, 300.0f, 150, 90);
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

void EnFishing_DrawEffects(FishingEffect* effect, PlayState* play) {
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
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingRippleMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 155, 155, 155, 0);
                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, effect->alpha);

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(effect->unk_30, 1.0f, effect->unk_30, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

            gSPDisplayList(POLY_XLU_DISP++, gFishingRippleModelDL);
        }
        effect++;
    }

    effect = firstEffect;
    flag = 0;
    for (i = 0; i < 100; i++) {
        if (effect->type == FS_EFF_DUST_SPLASH) {
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingDustSplashMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 200, 0);
                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 180, 180, 180, effect->alpha);

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->unk_30, effect->unk_30, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

            gSPDisplayList(POLY_XLU_DISP++, gFishingDustSplashModelDL);
        }
        effect++;
    }

    effect = firstEffect;
    flag = 0;
    for (i = 0; i < 100; i++) {
        if (effect->type == FS_EFF_WATER_DUST) {
            if (flag == 0) {
                gSPDisplayList(POLY_OPA_DISP++, gFishingWaterDustMaterialDL);
                gDPSetEnvColor(POLY_OPA_DISP++, 40, 90, 80, 128);
                flag++;
            }

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 40, 90, 80, effect->alpha);

            gSPSegment(POLY_OPA_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, effect->timer + (i * 3), (effect->timer + (i * 3)) * 5,
                                          32, 64, 1, 0, 0, 32, 32, 1, 5, 0, 0));

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->unk_30, effect->unk_30, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

            gSPDisplayList(POLY_OPA_DISP++, gFishingWaterDustModelDL);
        }
        effect++;
    }

    effect = firstEffect;
    flag = 0;
    for (i = 0; i < 100; i++) {
        if (effect->type == FS_EFF_BUBBLE) {
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingBubbleMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 150, 150, 150, 0);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
                flag++;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->unk_30, effect->unk_30, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

            gSPDisplayList(POLY_XLU_DISP++, gFishingBubbleModelDL);
        }
        effect++;
    }

    effect = firstEffect + 30;
    flag = 0;
    for (i = 30; i < EFFECT_COUNT; i++) {
        if (effect->type == FS_EFF_RAIN_DROP) {
            if (flag == 0) {
                POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_20);
                gDPSetCombineMode(POLY_XLU_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 150, 255, 255, 30);
                flag++;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateYF(effect->unk_38, MTXMODE_APPLY);
            Matrix_RotateXFApply(effect->unk_34);
            Matrix_RotateZF(effect->unk_3C, MTXMODE_APPLY);
            Matrix_Scale(0.002f, 1.0f, 0.1f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

            gSPDisplayList(POLY_XLU_DISP++, gFishingRainDropModelDL);
        }
        effect++;
    }

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    effect = firstEffect + 30;
    flag = 0;
    for (i = 30; i < EFFECT_COUNT; i++) {
        if (effect->type == FS_EFF_RAIN_RIPPLE) {
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingRippleMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 155, 155, 155, 0);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 130);
                flag++;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(effect->unk_30, 1.0f, effect->unk_30, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

            gSPDisplayList(POLY_XLU_DISP++, gFishingRippleModelDL);
        }
        effect++;
    }

    effect = firstEffect + 30;
    flag = 0;
    for (i = 30; i < EFFECT_COUNT; i++) {
        if (effect->type == FS_EFF_RAIN_SPLASH) {
            if (flag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingRainSplashMaterialDL);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, KREG(19) + 80);
                flag++;
            }

            if (Rand_ZeroOne() < 0.5f) {
                rotY = 0.0f;
            } else {
                rotY = M_PIf;
            }

            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_RotateYF(rotY, MTXMODE_APPLY);
            Matrix_Scale(effect->unk_30, effect->unk_30, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

            gSPDisplayList(POLY_XLU_DISP++, gFishingRainSplashModelDL);
        }
        effect++;
    }

    effect = firstEffect;
    if (effect->type == FS_EFF_OWNER_HAT) {
        Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
        Matrix_RotateYF(BINANG_TO_RAD_ALT2(sEffOwnerHatRot.y), MTXMODE_APPLY);
        Matrix_RotateXFApply(BINANG_TO_RAD_ALT2(sEffOwnerHatRot.x));
        Matrix_RotateZF(BINANG_TO_RAD_ALT2(sEffOwnerHatRot.z), MTXMODE_APPLY);
        Matrix_Scale(effect->unk_30, effect->unk_30, effect->unk_30, MTXMODE_APPLY);
        Matrix_Translate(-1250.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateXFApply(M_PIf / 2);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        gSPDisplayList(POLY_OPA_DISP++, gFishingOwnerHatDL);
    }

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFishing_DrawStreamSplash(PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x09,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, play->gameplayFrames * 1, play->gameplayFrames * 8, 32, 64, 1,
                                  -play->gameplayFrames * 2, 0, 16, 16, 1, 8, -2, 0));

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, 50);

    Matrix_Translate(670.0f, -24.0f, -600.0f, MTXMODE_NEW);
    Matrix_Scale(0.02f, 1.0f, 0.02f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gFishingStreamSplashDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 func_808FEF70(Vec3f* vec) {
    if (((vec->x >= 110.0f) && (vec->x <= 150.0f) && (vec->z <= 1400.0f) && (vec->z >= 1160.0f)) ||
        ((vec->x >= 110.0f) && (vec->x <= 210.0f) && (vec->z <= 1200.0f) && (vec->z >= 1160.0f))) {
        if (vec->y <= 42.0f) {
            return true;
        }
    }

    return false;
}

void EnFishing_UpdateLine(PlayState* play, Vec3f* basePos, Vec3f* pos, Vec3f* rot, Vec3f* unk) {
    s32 i;
    s32 k;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 rx;
    f32 ry;
    f32 dist;
    f32 spD8;
    s32 temp_s2;
    s32 pad;
    f32 temp_f20;
    Vec3f posSrc = { 0.0f, 0.0f, 0.0f };
    Vec3f posStep;
    f32 phi_f12;
    Vec3f spA4;
    Vec3f sp98;
    f32 sp94;
    f32 sp90;
    f32 sp8C;
    f32 sqDistXZ;
    f32 temp_f18;

    if (D_8090CD24 != 0) {
        spA4 = *basePos;
        sp98 = pos[LINE_SEG_COUNT - 1];

        sp94 = sp98.x - spA4.x;
        sp90 = sp98.y - spA4.y;
        sp8C = sp98.z - spA4.z;

        temp_f20 = sqrtf(SQ(sp94) + SQ(sp90) + SQ(sp8C)) * 0.97f;
        if (temp_f20 > 1000.0f) {
            temp_f20 = 1000.0f;
        }

        D_809101C0 = 200.0f - (temp_f20 * 200.0f * 0.001f);
    }

    temp_s2 = D_809101C0;
    posSrc.z = 5.0f;

    for (i = 0; i < LINE_SEG_COUNT; i++) {
        if (i <= temp_s2) {
            pos[i] = *basePos;
        } else if (D_8090CD24 != 0) {
            temp_f20 = (f32)(i - temp_s2) / (f32)(LINE_SEG_COUNT - temp_s2 + 1);
            Math_ApproachF(&pos[i].x, (sp94 * temp_f20) + spA4.x, 1.0f, 20.0f);
            Math_ApproachF(&pos[i].y, (sp90 * temp_f20) + spA4.y, 1.0f, 20.0f);
            Math_ApproachF(&pos[i].z, (sp8C * temp_f20) + spA4.z, 1.0f, 20.0f);
        }
    }

    for (i = temp_s2 + 1, k = 0; i < LINE_SEG_COUNT; i++, k++) {
        temp_f18 = 2.0f * D_809101C4;

        dx = (pos + i)->x - (pos + i - 1)->x;
        spD8 = (pos + i)->y;

        sqDistXZ = SQ((pos + i)->x) + SQ((pos + i)->z);

        if (sqDistXZ > SQ(920.0f)) {
            phi_f12 = ((sqrtf(sqDistXZ) - 920.0f) * 0.11f) + WATER_SURFACE_Y(play);
        } else {
            phi_f12 = WATER_SURFACE_Y(play);
        }

        if (D_80917206 == 2) {
            f32 phi_f2;

            if (spD8 < phi_f12) {
                phi_f12 = ((sqrtf(sqDistXZ) - 920.0f) * 0.147f) + WATER_SURFACE_Y(play);
                if (spD8 > phi_f12) {
                    u8 temp;

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
                f32 phi_f2 = (spD8 - phi_f12) * 0.2f;

                phi_f2 = CLAMP_MAX(phi_f2, temp_f18);
                spD8 -= phi_f2;
            }
        } else {
            if (spD8 > phi_f12) {
                spD8 -= temp_f18;
            }
        }

        if (func_808FEF70(&pos[i])) {
            spD8 = 42.0f;
        }

        dy = spD8 - (pos + i - 1)->y;
        dz = (pos + i)->z - (pos + i - 1)->z;

        ry = Math_Atan2F_XY(dz, dx);
        dist = sqrtf(SQ(dx) + SQ(dz));
        rx = -Math_Atan2F_XY(dist, dy);

        (rot + i - 1)->y = ry;
        (rot + i - 1)->x = rx;

        Matrix_RotateYF(ry, MTXMODE_NEW);
        Matrix_RotateXFApply(rx);
        Matrix_MultVec3f(&posSrc, &posStep);

        (pos + i)->x = (pos + i - 1)->x + posStep.x;
        (pos + i)->y = (pos + i - 1)->y + posStep.y;
        (pos + i)->z = (pos + i - 1)->z + posStep.z;
    }
}

void EnFishing_UpdateLinePos(Vec3f* pos) {
    s32 i;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 rx;
    f32 ry;
    f32 dist;
    Vec3f posSrc = { 0.0f, 0.0f, 0.0f };
    Vec3f posStep;
    s32 min = D_809101C0;

    posSrc.z = 5.0f;

    for (i = LINE_SEG_COUNT - 2; i > min; i--) {
        dx = (pos + i)->x - (pos + i + 1)->x;
        dy = (pos + i)->y - (pos + i + 1)->y;
        dz = (pos + i)->z - (pos + i + 1)->z;

        ry = Math_Atan2F_XY(dz, dx);
        dist = sqrtf(SQ(dx) + SQ(dz));
        rx = -Math_Atan2F_XY(dist, dy);

        Matrix_RotateYF(ry, MTXMODE_NEW);
        Matrix_RotateXFApply(rx);
        Matrix_MultVec3f(&posSrc, &posStep);

        (pos + i)->x = (pos + i + 1)->x + posStep.x;
        (pos + i)->y = (pos + i + 1)->y + posStep.y;
        (pos + i)->z = (pos + i + 1)->z + posStep.z;
    }
}

void EnFishing_DrawLureHook(PlayState* play, Vec3f* pos, Vec3f* refPos, u8 hookIndex) {
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

    if ((D_8090CD14 == 3) && ((pos->y > WATER_SURFACE_Y(play)) || (D_8090CD0C && ((u32)hookIndex != 0)))) {
        offsetY = 0.0f;
    } else if (pos->y < WATER_SURFACE_Y(play)) {
        offsetY = -1.0f;
    } else {
        offsetY = -3.0f;
    }

    dx = refPos->x - pos->x;
    dy = refPos->y - pos->y + offsetY;
    dz = refPos->z - pos->z;

    ry = Math_Atan2F_XY(dz, dx);
    dist = sqrtf(SQ(dx) + SQ(dz));
    rx = -Math_Atan2F_XY(dist, dy);

    Matrix_RotateYF(ry, MTXMODE_NEW);
    Matrix_RotateXFApply(rx);
    Matrix_MultVec3f(&posSrc, &posStep);

    refPos->x = pos->x + posStep.x;
    refPos->y = pos->y + posStep.y;
    refPos->z = pos->z + posStep.z;

    Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_NEW);

    if ((player->actor.speed == 0.0f) && (D_809101B4 == 0.0f)) {
        Math_ApproachF(&sLureHookRotY[hookIndex], ry, 0.1f, 0.3f);
    } else {
        sLureHookRotY[hookIndex] = ry;
    }

    Matrix_RotateYF(sLureHookRotY[hookIndex], MTXMODE_APPLY);
    Matrix_RotateXFApply(rx);
    Matrix_Scale(0.0039999997f, 0.0039999997f, 0.005f, MTXMODE_APPLY);
    Matrix_RotateYF(M_PIf, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gFishingLureHookDL);

    Matrix_RotateZF(M_PIf / 2, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gFishingLureHookDL);

    if ((hookIndex == 1) && D_8090CD0C) {
        Matrix_Scale(2.0f, 2.0f, 2.0f, MTXMODE_APPLY);
        Matrix_Translate(250.0f, 0.0f, -1400.0f, MTXMODE_APPLY);
        Matrix_Push();

        if (D_8090CD10) {
            FishingEffect* effect = play->specialEffects;
            MtxF mf;

            Matrix_MultVec3f(&sZeroVec, &effect->pos);
            Matrix_Get(&mf);
            Matrix_MtxFToYXZRot(&mf, &sEffOwnerHatRot, false);

            D_8090CD10 = false;
            D_8090CD0C = false;

            effect->type = FS_EFF_OWNER_HAT;
            effect->unk_2C = 0;
            effect->velocity = sZeroVec;
            effect->accel = sZeroVec;
        }

        Matrix_Pop();
        Matrix_Translate(-1250.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateXFApply(M_PIf / 2);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gFishingOwnerHatDL);
    }

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFishing_UpdateSinkingLure(PlayState* play) {
    s32 i;
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
    Vec3f* pos;
    s32 pad;

    posSrc.z = 0.85f;

    sSinkingLurePos[0] = sLurePos;

    if (D_8090CD54 != 0) {
        offsetY = -1.0f;
    } else if (sLurePos.y < WATER_SURFACE_Y(play)) {
        offsetY = 0.5f;
    } else {
        offsetY = -5.0f;
    }

    if (D_8090CD14 == 5) {
        Matrix_RotateYF(BINANG_TO_RAD(player->actor.shape.rot.y), MTXMODE_NEW);
        sp94.x = 5.0f;
        sp94.y = 0.0f;
        sp94.z = 3.0f;
        Matrix_MultVec3f(&sp94, &sp88);
    }

    for (i = 1; i < SINKING_LURE_SEG_COUNT; i++) {
        pos = sSinkingLurePos;

        if ((i < 10) && (D_8090CD14 == 5)) {
            offsetX = (10 - i) * sp88.x * 0.1f;
            offsetZ = (10 - i) * sp88.z * 0.1f;
        } else {
            offsetX = offsetZ = 0.0f;
        }

        dx = (pos + i)->x - (pos + i - 1)->x + offsetX;
        dy = (pos + i)->y - (pos + i - 1)->y + offsetY;
        dz = (pos + i)->z - (pos + i - 1)->z + offsetZ;

        ry = Math_Atan2F_XY(dz, dx);
        dist = sqrtf(SQ(dx) + SQ(dz));
        rx = -Math_Atan2F_XY(dist, dy);

        Matrix_RotateYF(ry, MTXMODE_NEW);
        Matrix_RotateXFApply(rx);
        Matrix_MultVec3f(&posSrc, &posStep);

        (pos + i)->x = (pos + i - 1)->x + posStep.x;
        (pos + i)->y = (pos + i - 1)->y + posStep.y;
        (pos + i)->z = (pos + i - 1)->z + posStep.z;
    }
}

f32 sSinkingLureSizes[] = {
    1.0f, 1.5f,  1.8f, 2.0f, 1.8f, 1.6f, 1.4f, 1.2f, 1.0f, 1.0f,
    0.9f, 0.85f, 0.8f, 0.7f, 0.8f, 1.0f, 1.2f, 1.1f, 1.0f, 0.8f,
};

void EnFishing_DrawSinkingLure(PlayState* play) {
    s16 i;
    f32 scale;

    OPEN_DISPS(play->state.gfxCtx);

    EnFishing_UpdateSinkingLure(play);

    if (sLurePos.y < WATER_SURFACE_Y(play)) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        gSPDisplayList(POLY_OPA_DISP++, gFishingSinkingLureSegmentMaterialDL);

        for (i = SINKING_LURE_SEG_COUNT - 1; i >= 0; i--) {
            if ((i + D_80911F20) < SINKING_LURE_SEG_COUNT) {
                Matrix_Translate(sSinkingLurePos[i].x, sSinkingLurePos[i].y, sSinkingLurePos[i].z, MTXMODE_NEW);
                scale = sSinkingLureSizes[i + D_80911F20] * 0.04f;
                Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
                Matrix_ReplaceRotation(&play->billboardMtxF);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gFishingSinkingLureSegmentModelDL);
            }
        }
    } else {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        gSPDisplayList(POLY_XLU_DISP++, gFishingSinkingLureSegmentMaterialDL);

        for (i = SINKING_LURE_SEG_COUNT - 1; i >= 0; i--) {
            if ((i + D_80911F20) < SINKING_LURE_SEG_COUNT) {
                Matrix_Translate(sSinkingLurePos[i].x, sSinkingLurePos[i].y, sSinkingLurePos[i].z, MTXMODE_NEW);
                scale = sSinkingLureSizes[i + D_80911F20] * 0.04f;
                Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
                Matrix_ReplaceRotation(&play->billboardMtxF);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gFishingSinkingLureSegmentModelDL);
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFishing_DrawLureAndLine(PlayState* play, Vec3f* linePos, Vec3f* lineRot) {
    Vec3f posSrc;
    Vec3f posStep;
    Vec3f hookPos[2];
    s32 i;
    s32 spB4 = D_809101C0;
    s32 pad;
    Player* player = GET_PLAYER(play);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Matrix_Push();

    if (D_8090CD54 != 0) {
        Vec3f posTemp = sLurePos;

        sLurePos = sSinkingLureBasePos;
        EnFishing_DrawSinkingLure(play);
        sLurePos = posTemp;
    }

    if ((D_8090CD14 == 4) || (D_8090CD14 == 5)) {
        sLurePos = sFishingHookedFish->fishMouthPos;

        if ((D_8090CD14 == 5) && (D_80917206 == 2)) {
            Matrix_RotateYF(BINANG_TO_RAD(player->actor.shape.rot.y), MTXMODE_NEW);
            posSrc.x = 2.0f;
            posSrc.y = 0.0f;
            posSrc.z = 0.0f;
            Matrix_MultVec3f(&posSrc, &posStep);
            sLurePos.x += posStep.x;
            sLurePos.z += posStep.z;
        }
    } else if (D_8090CD14 == 0) {
        sLurePos = sReelLinePos[LINE_SEG_COUNT - 1];
        sLureRot.x = sReelLineRot[LINE_SEG_COUNT - 2].x + M_PIf;

        if ((player->actor.speed == 0.0f) && (D_80917200 == 0)) {
            Math_ApproachF(&sLureRot.y, sReelLineRot[LINE_SEG_COUNT - 2].y, 0.1f, 0.2f);
        } else {
            sLureRot.y = sReelLineRot[LINE_SEG_COUNT - 2].y;
        }
    }

    if (D_80917206 != 2) {
        Matrix_Translate(sLurePos.x, sLurePos.y, sLurePos.z, MTXMODE_NEW);
        Matrix_RotateYF(sLureRot.y + D_80917254, MTXMODE_APPLY);
        Matrix_RotateXFApply(sLureRot.x);
        Matrix_Scale(0.0039999997f, 0.0039999997f, 0.0039999997f, MTXMODE_APPLY);
        Matrix_Translate(0.0f, 0.0f, D_80917258, MTXMODE_APPLY);
        Matrix_RotateZF(M_PIf / 2, MTXMODE_APPLY);
        Matrix_RotateYF(M_PIf / 2, MTXMODE_APPLY);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gFishingLureFloatDL);

        posSrc.x = -850.0f;
        posSrc.y = 0.0f;
        posSrc.z = 0.0f;
        Matrix_MultVec3f(&posSrc, &D_80917218);

        posSrc.x = 500.0f;
        posSrc.z = -300.0f;
        Matrix_MultVec3f(&posSrc, &hookPos[0]);
        EnFishing_DrawLureHook(play, &hookPos[0], &sLureHookRefPos[0], 0);

        posSrc.x = 2100.0f;
        posSrc.z = -50.0f;
        Matrix_MultVec3f(&posSrc, &hookPos[1]);
        EnFishing_DrawLureHook(play, &hookPos[1], &sLureHookRefPos[1], 1);
    }

    POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_20);

    gDPSetCombineMode(POLY_XLU_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 55);

    if ((D_8090CD14 == 4) && (D_80917274 || (D_80917206 != 2))) {
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
        Matrix_RotateYF(ry, MTXMODE_APPLY);
        Matrix_RotateXFApply(rx);
        Matrix_Scale(D_809101C8, 1.0f, dist, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gFishingLineModelDL);
    } else {
        for (i = spB4; i < LINE_SEG_COUNT - 1; i++) {
            if ((i == LINE_SEG_COUNT - 3) && (D_80917206 == 0) && (D_8090CD14 == 3)) {
                f32 rx;
                f32 ry;
                f32 dist;
                f32 dx = D_80917218.x - (linePos + i)->x;
                f32 dy = D_80917218.y - (linePos + i)->y;
                f32 dz = D_80917218.z - (linePos + i)->z;

                ry = Math_FAtan2F(dx, dz);
                dist = sqrtf(SQ(dx) + SQ(dz));
                rx = -Math_FAtan2F(dy, dist);

                dist = sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) * 0.001f;

                Matrix_Translate((linePos + i)->x, (linePos + i)->y, (linePos + i)->z, MTXMODE_NEW);
                Matrix_RotateYF(ry, MTXMODE_APPLY);
                Matrix_RotateXFApply(rx);
                Matrix_Scale(D_809101C8, 1.0f, dist, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gFishingLineModelDL);
                break;
            }

            Matrix_Translate((linePos + i)->x, (linePos + i)->y, (linePos + i)->z, MTXMODE_NEW);
            Matrix_RotateYF((lineRot + i)->y, MTXMODE_APPLY);
            Matrix_RotateXFApply((lineRot + i)->x);
            Matrix_Scale(D_809101C8, 1.0f, 0.005f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gFishingLineModelDL);
        }
    }

    Matrix_Pop();
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    CLOSE_DISPS(play->state.gfxCtx);
}

f32 sRodScales[22] = {
    1.0f,        1.0f,        1.0f,        0.9625f,     0.925f, 0.8875f,     0.85f,       0.8125f,
    0.775f,      0.73749995f, 0.7f,        0.6625f,     0.625f, 0.5875f,     0.54999995f, 0.5125f,
    0.47499996f, 0.4375f,     0.39999998f, 0.36249995f, 0.325f, 0.28749996f,
};

f32 sRodBendRatios[22] = {
    0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.06f,   0.12f,   0.18f,   0.24f,   0.30f,   0.36f,
    0.42f, 0.48f, 0.54f, 0.60f, 0.60f, 0.5142f, 0.4285f, 0.3428f, 0.2571f, 0.1714f, 0.0857f,
};

Vec3f sRodTipOffset = { 0.0f, 0.0f, 0.0f };

void EnFishing_DrawRod(PlayState* play) {
    s16 i;
    f32 spC8;
    f32 spC4;
    f32 spC0;
    Input* input = CONTROLLER1(&play->state);
    Player* player = GET_PLAYER(play);
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    if (D_80911E28 != 0) {
        D_80911E28--;

        Math_ApproachF(&D_8090CD40, 35.0f, 1.0f, 100.0f);
        Math_ApproachF(&D_8090CD3C, -0.8f, 1.0f, 0.4f);
        Math_ApproachS(&player->actor.shape.rot.x, -4000, 2, 15000);
    } else {
        s16 target = 0;

        if ((D_8090CD14 == 4) && D_80917274) {
            target = Math_SinS(D_809171FE * 25600) * 1500.0f;
        } else {
            Math_ApproachZeroF(&D_8090CD40, 0.1f, 10.0f);
            Math_ApproachZeroF(&D_8090CD3C, 1.0f, 0.05f);
        }

        Math_ApproachS(&player->actor.shape.rot.x, target, 5, 1000);
    }

    if ((D_8090CD14 == 3) || (D_8090CD14 == 4)) {
        if ((input->rel.stick_x == 0) && (D_8090CD44 != 0)) {
            D_8090CD30 = 0.0f;
        }
        if ((input->rel.stick_y == 0) && (D_8090CD48 != 0)) {
            D_8090CD34 = 0.0f;
        }

        spC8 = player->unk_B0C;
        Math_SmoothStepToF(&player->unk_B0C, input->rel.stick_y * 0.02f, 0.3f, 5.0f, 0.0f);
        spC8 = player->unk_B0C - spC8;

        spC4 = player->unk_B08;
        Math_SmoothStepToF(&player->unk_B08, input->rel.stick_x * 0.02f, 0.3f, 5.0f, 0.0f);
        spC4 = player->unk_B08 - spC4;

        if (player->unk_B08 > 1.0f) {
            player->unk_B08 = 1.0f;
        }
        if (player->unk_B0C > 1.0f) {
            player->unk_B0C = 1.0f;
        }
        if (player->unk_B08 < -1.0f) {
            player->unk_B08 = -1.0f;
        }
        if (player->unk_B0C < -1.0f) {
            player->unk_B0C = -1.0f;
        }

        Math_ApproachF(&D_8090CD28, spC4 * 70.0f * -0.01f, 1.0f, D_8090CD30);
        Math_ApproachF(&D_8090CD30, 1.0f, 1.0f, 0.1f);
        Math_ApproachF(&D_8090CD2C, spC8 * 70.0f * 0.01f, 1.0f, D_8090CD34);
        Math_ApproachF(&D_8090CD34, 1.0f, 1.0f, 0.1f);
        Math_ApproachZeroF(&D_8090CD38, 1.0f, 0.05f);
    } else {
        Math_ApproachZeroF(&player->unk_B0C, 1.0f, 0.1f);
        Math_ApproachZeroF(&player->unk_B08, 1.0f, 0.1f);
        Math_ApproachF(&D_8090CD2C, (Math_SinS(D_809171FE * 3000) * 0.025f) + -0.03f, 1.0f, 0.05f);
        Math_ApproachZeroF(&D_8090CD28, 1.0f, 0.05f);

        if ((D_80917204 >= 19) && (D_80917204 <= 24)) {
            Math_ApproachF(&D_8090CD38, 0.8f, 1.0f, 0.2f);
        } else {
            Math_ApproachF(&D_8090CD38, 0.0f, 1.0f, 0.4f);
        }
    }

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gFishingRodSetupDL);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 155, 0, 255);

    Matrix_Mult(&player->leftHandMf, MTXMODE_NEW);

    if (sLinkAge != 1) {
        Matrix_Translate(0.0f, 400.0f, 0.0f, MTXMODE_APPLY);
    } else {
        Matrix_Translate(0.0f, 230.0f, 0.0f, MTXMODE_APPLY);
    }

    if (D_8090CD14 == 5) {
        Matrix_RotateYF(1.7592919f, MTXMODE_APPLY);
    } else {
        Matrix_RotateYF(1.288053f, MTXMODE_APPLY);
    }

    Matrix_RotateXFApply(-0.6283185f);
    Matrix_RotateZF((player->unk_B08 * 0.5f) + 0.4712389f, MTXMODE_APPLY);
    Matrix_RotateXFApply((D_8090CD40 + 20.0f) * 0.01f * M_PIf);
    Matrix_Scale(0.70000005f, 0.70000005f, 0.70000005f, MTXMODE_APPLY);

    spC0 = (D_8090CD3C * (((player->unk_B0C - 1.0f) * -0.25f) + 0.5f)) + (D_8090CD2C + D_8090CD38);

    Matrix_Translate(0.0f, 0.0f, -1300.0f, MTXMODE_APPLY);

    for (i = 0; i < 22; i++) {
        Matrix_RotateYF(sRodBendRatios[i] * D_8090CD28 * 0.5f, MTXMODE_APPLY);
        Matrix_RotateXFApply(sRodBendRatios[i] * spC0 * 0.5f);

        Matrix_Push();
        Matrix_Scale(sRodScales[i], sRodScales[i], 0.52f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        if (i < 5) {
            gDPLoadTextureBlock(POLY_OPA_DISP++, &gFishingRodSegmentBlackTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 8, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
        } else if ((i < 8) || ((i % 2) == 0)) {
            gDPLoadTextureBlock(POLY_OPA_DISP++, &gFishingRodSegmentWhiteTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 8, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
        } else {
            gDPLoadTextureBlock(POLY_OPA_DISP++, &gFishingRodSegmentStripTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 8, 0,
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

Vec3f D_8090D614 = { 0.0f, 0.0f, 0.0f };

void EnFishing_UpdateLure(EnFishing* this, PlayState* play) {
    f32 spE4;
    f32 spE0;
    s16 i;
    s16 spDC;
    f32 spD8;
    f32 spD4;
    f32 spD0;
    f32 phi_f16;
    f32 spC8;
    f32 phi_f0;
    Player* player = GET_PLAYER(play);
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f spA8;
    Vec3f sp9C;
    Vec3f sp90;
    Input* input = CONTROLLER1(&play->state);

    D_809171FE++;

    if (D_80917200 != 0) {
        D_80917200--;
    }

    if (D_80917202 != 0) {
        D_80917202--;
    }

    if (D_80917204 != 0) {
        D_80917204--;
    }

    if (D_80917272 != 0) {
        D_80917272--;
    }

    if (D_809101CC != 0) {
        D_809101CC--;
    }

    if (D_8090CD24 != 0) {
        D_8090CD24--;
    }

    if (D_809171F4 != 0) {
        D_809171F4--;
    }

    if (D_80917264 != 0) {
        D_80917264--;
    }

    if (D_809171FC == 1) {
        D_809171FC = 2;
        D_809171D8 = 0;
        D_809171D6 = 0;
        D_80917206 = 0;

        // Age and high score check removed in MM
        sSinkingLureLocation = Rand_ZeroFloat(3.999f) + 1.0f;

        D_809101C4 = 520.0f;
        D_809101C0 = 195.0f;

        D_8090CD14 = 0;
        D_80917206 = 0;
        D_809171FE = 0;
        D_80917200 = 0;
        D_80917202 = 0;
        D_80917204 = 0;
        D_80917270 = 0;
        D_80917264 = 0;
        D_809101CC = 0;

        D_80917254 = D_809101D0 = D_80917258 = 0.0f;

        D_80917278 = zeroVec;

        for (i = 0; i < LINE_SEG_COUNT; i++) {
            sReelLinePos[i] = zeroVec;
            sReelLineRot[i] = zeroVec;
            sReelLineUnk[i] = zeroVec;
        }
    }

    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &sLurePos, &D_8090D614, &sProjectedW);

    if (D_8090CD14 == 0) {
        Math_ApproachF(&D_80917258, -800.0f, 1.0f, 20.0f);
    } else {
        Math_ApproachF(&D_80917258, 300.0f, 1.0f, 20.0f);
    }

    switch (D_8090CD14) {
        case 0:
            D_80911F20 = 0;

            Math_ApproachF(&D_809101C0, 195.0f, 1.0f, 1.0f);

            if (player->stateFlags1 & PLAYER_STATE1_8000000) {
                D_80917204 = 0;
                player->unk_B28 = 0;
            }

            if (D_80917204 == 0) {
                if ((D_80917200 == 0) && (player->unk_B28 == 1)) {
                    D_80917204 = 37;
                    Message_CloseTextbox(play);
                }
            } else {
                sLureRot.x = sReelLineRot[LINE_SEG_COUNT - 2].x + M_PIf;
                sLureRot.y = sReelLineRot[LINE_SEG_COUNT - 2].y;

                if (D_80917204 == 18) {
                    D_8090CD14 = 1;
                    sLurePos = sRodTipPos;
                    Matrix_RotateYF(BINANG_TO_RAD_ALT(player->actor.shape.rot.y), MTXMODE_NEW);
                    sp90.x = 0.0f;
                    sp90.y = 0.0f;
                    sp90.z = 25.0f;
                    Matrix_MultVec3f(&sp90, &D_80917238);
                    D_80917238.y = 15.0f;
                    D_80917248.x = D_80917248.z = 0.0f;
                    D_80917248.y = -1.0f;
                    D_809101C4 = 0.0f;
                    D_80917202 = 5;
                    D_8091726C = 0.5f;
                    D_80917268 = Rand_ZeroFloat(1.9f);
                    sFishMouthOffset.y = 500.0f;
                    Audio_PlaySfx_AtPos(&D_8090D614, NA_SE_IT_SWORD_SWING_HARD);
                }
            }
            break;

        case 1:
            spE0 = sLurePos.y;

            sLurePos.x += D_80917238.x;
            sLurePos.y += D_80917238.y;
            sLurePos.z += D_80917238.z;

            D_80917238.x += D_80917248.x;
            D_80917238.y += D_80917248.y;
            D_80917238.z += D_80917248.z;

            if (CHECK_BTN_ALL(input->cur.button, BTN_A) || D_8090CD0C) {
                D_80917238.x *= 0.9f;
                D_80917238.z *= 0.9f;
                if (!D_8090CD0C) {
                    Audio_PlaySfx(NA_SE_IT_FISHING_REEL_HIGH - SFX_FLAG);
                }
            }

            spD8 = sLurePos.x - sRodTipPos.x;
            spD4 = sLurePos.y - sRodTipPos.y;
            spD0 = sLurePos.z - sRodTipPos.z;

            if (D_80917202 != 0) {
                sLureRot.x = sReelLineRot[LINE_SEG_COUNT - 2].x + M_PIf;
                sLureRot.y = sReelLineRot[LINE_SEG_COUNT - 2].y;
            } else {
                sLureRot.x = 0.0f;
                sLureRot.y = Math_Atan2F_XY(spD0, spD8) + M_PIf;
            }

            phi_f16 = sqrtf(SQ(spD8) + SQ(spD4) + SQ(spD0));
            if (phi_f16 > 1000.0f) {
                phi_f16 = 1000.0f;
            }
            D_809101C0 = 200.0f - (phi_f16 * 200.0f * 0.001f);

            spC8 = SQ(sLurePos.x) + SQ(sLurePos.z);
            if (spC8 > SQ(920.0f)) {
                if ((sLurePos.y > 160.0f) || (sLurePos.x < 80.0f) || (sLurePos.x > 180.0f) || (sLurePos.z > 1350.0f) ||
                    (sLurePos.z < 1100.0f) || (sLurePos.y < 45.0f)) {
                    Vec3f sp80 = this->actor.world.pos;

                    this->actor.prevPos = this->actor.world.pos = sLurePos;
                    Actor_UpdateBgCheckInfo(play, &this->actor, 15.0f, 30.0f, 30.0f,
                                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_40);
                    this->actor.world.pos = sp80;

                    if (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) {
                        D_80917238.y = -0.5f;
                    }
                    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                        if (D_80917238.y > 0.0f) {
                            D_80917238.y = 0.0f;
                        }
                        D_80917238.x = D_80917238.z = 0.0f;
                    }
                } else {
                    if (func_808FEF70(&sLurePos)) {
                        D_8090CD14 = 3;
                        D_809101D0 = 0.0f;
                    }
                }

                spE0 = ((sqrtf(spC8) - 920.0f) * 0.11f) + WATER_SURFACE_Y(play);
                if (sLurePos.y <= spE0) {
                    sLurePos.y = spE0;
                    D_80917238.x = D_80917238.y = D_80917238.z = 0.0f;
                    D_8090CD14 = 3;
                    D_809101D0 = 0.0;
                } else {
                    Math_ApproachF(&D_809101C4, 0.0f, 1.0f, 0.05f);
                    Audio_PlaySfx_AtPos(&D_8090D614,
                                        NA_SE_EN_WIZ_UNARI - SFX_FLAG); // changed from NA_SE_EN_FANTOM_FLOAT in OoT
                }
            } else {
                f32 sp7C = WATER_SURFACE_Y(play);
                f32 sp78;

                if (sLurePos.y <= sp7C) {
                    D_8090CD14 = 2;
                    D_809101D0 = 0.0f;
                    D_80917238.x = D_80917238.z = 0.0f;

                    if (D_80917206 == 2) {
                        D_809171F2 = 0;
                    } else {
                        D_809171F2 = 10;
                    }

                    if ((sLurePos.y <= sp7C) && (sp7C < spE0) && (sp7C == WATER_SURFACE_Y(play))) {
                        D_80917264 = 10;
                        Audio_PlaySfx_AtPos(&D_8090D614, NA_SE_EV_BOMB_DROP_WATER);
                        D_80917248.y = 0.0f;
                        D_80917238.y *= 0.2f;

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
                            EnFishing_SpawnDustSplash(NULL, play->specialEffects, &spA8, &sp9C,
                                                      Rand_ZeroFloat(0.02f) + 0.025f);
                        }

                        spA8 = sLurePos;
                        spA8.y = WATER_SURFACE_Y(play);
                        EnFishing_SpawnRipple(NULL, play->specialEffects, &spA8, 100.0f, 800.0f, 150, 90);
                    }
                } else {
                    Math_ApproachZeroF(&D_809101C4, 1.0f, 0.05f);
                    Audio_PlaySfx_AtPos(&D_8090D614,
                                        NA_SE_EN_WIZ_UNARI - SFX_FLAG); // changed from NA_SE_EN_FANTOM_FLOAT in OoT
                }
            }

            sReelLinePos[LINE_SEG_COUNT - 1].x = sLurePos.x;
            sReelLinePos[LINE_SEG_COUNT - 1].y = sLurePos.y;
            sReelLinePos[LINE_SEG_COUNT - 1].z = sLurePos.z;

            D_809101BC = 1.0f;
            D_8091725C = 0.5f;
            break;

        case 2:
            if (sLurePos.y <= WATER_SURFACE_Y(play)) {
                sLurePos.y += D_80917238.y;

                Math_ApproachZeroF(&D_80917238.y, 1.0f, 1.0f);

                if (D_80917206 != 2) {
                    Math_ApproachF(&sLurePos.y, WATER_SURFACE_Y(play), 0.5f, 1.0f);
                }
            }

            Math_ApproachF(&D_809101C4, 2.0f, 1.0f, 0.1f);

            if (D_809171F2 == 0) {
                D_8090CD14 = 3;
            } else {
                D_809171F2--;
            }
            break;

        case 3: {
            s32 pad;
            f32 sp70;
            Vec3f sp64;
            Vec3f sp58;

            D_80911F20 = 0;

            if (D_8090CD0C && ((SQ(sLurePos.x) + SQ(sLurePos.z)) < SQ(500.0f))) {
                D_8090CD10 = true;
            }

            player->unk_B28 = 2;

            if (D_809101B4 < 3.0f) {
                spD0 = D_8091725C * Math_SinS(D_809171FE * 0x1060);
                Math_ApproachF(&sLureRot.x, -0.5235988f + spD0, 0.3f, D_80917260);
                Math_ApproachF(&D_80917260, 0.5f, 1.0f, 0.02f);
                Math_ApproachZeroF(&D_8091725C, 1.0f, 0.02f);
            } else {
                D_80917260 = 0.0f;
            }

            spDC = 0x4000;
            spE4 = WATER_SURFACE_Y(play);

            spC8 = SQ(sLurePos.x) + SQ(sLurePos.z);
            if (spC8 < SQ(920.0f)) {
                if (sLurePos.y <= (spE4 + 4.0f)) {
                    sp70 = 0.0f;

                    if (D_809101CC == 0) {
                        if (fabsf(input->rel.stick_x) > 30.0f) {
                            sp70 = fabsf((input->rel.stick_x - D_8090CD44) * (1.0f / 60.0f));
                        } else if (fabsf(input->rel.stick_y) > 30.0f) {
                            sp70 = fabsf((input->rel.stick_y - D_8090CD48) * (1.0f / 60.0f));
                        }
                    }

                    if (sp70 > 1.0f) {
                        sp70 = 1.0f;
                    }
                    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                        sp70 = 0.5f;
                    }

                    if (D_8090CD0C) {
                        if (sp70 > 0.3f) {
                            sp70 = 0.3f;
                        }
                    }

                    if ((sp70 > 0.2f) && (D_809101B4 < 4.0f)) {
                        D_809101CC = 5;

                        if (sp70 > 0.8f) {
                            D_80917270 = 2;
                        } else {
                            D_80917270 = 1;
                        }

                        sp90.x = player->actor.world.pos.x - sLurePos.x;
                        sp90.z = player->actor.world.pos.z - sLurePos.z;
                        sp90.y = Math_Atan2F_XY(sp90.z, sp90.x);

                        D_809101B0 = (sp70 * D_809101BC) + sp90.y;
                        D_809101BC *= -1.0f;
                        D_809101B4 = fabsf(sp70) * 6.0f;
                        sLureRot.x = 0.0f;
                        D_8091725C = 0.5f;
                        D_809101C0 += (fabsf(sp70) * 7.5f);

                        Audio_PlaySfx_AtPosWithFreq(&D_8090D614, NA_SE_EV_LURE_MOVE_W, (sp70 * 1.999f * 0.25f) + 0.75f);

                        if (D_80917206 == 2) {
                            D_80917278.y = 5.0f * sp70;
                            sReelLinePos[LINE_SEG_COUNT - 1].y += D_80917278.y;
                            sLurePos.y += D_80917278.y;
                        }
                    } else if (CHECK_BTN_ALL(input->cur.button, BTN_A)) {
                        s8 requiredScopeTemp;

                        spDC = 0x500;
                        D_809101B0 = sReelLineRot[LINE_SEG_COUNT - 2].y + M_PIf;
                        sLureRot.x = 0.0f;
                        D_8091725C = 0.5f;
                        if (D_80917206 == 2) {
                            D_80917278.y = 0.2f;
                            sReelLinePos[LINE_SEG_COUNT - 1].y += D_80917278.y;
                            sLurePos.y += D_80917278.y;
                        }
                    }
                } else {
                    if (D_809101C0 > 150.0f) {
                        sLureRot.x = sReelLineRot[LINE_SEG_COUNT - 2].x + M_PIf;
                        D_809101B0 = sReelLineRot[LINE_SEG_COUNT - 2].y + M_PIf;
                        D_809101C0 += 2.0f;
                    }
                }
            } else {
                spE4 = ((sqrtf(spC8) - 920.0f) * 0.11f) + WATER_SURFACE_Y(play);
                if (sLurePos.y <= spE4) {
                    sLurePos.y = spE4;
                    spDC = 0x500;
                    D_809101B0 = sReelLineRot[LINE_SEG_COUNT - 2].y + M_PIf;
                    sLureRot.x = 0.0f;
                    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                        D_809101C0 += 6.0f;
                        Audio_PlaySfx_AtPos(&D_8090D614, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_SAND);
                    }
                } else {
                    if (D_809101C0 > 150.0f) {
                        sLureRot.x = sReelLineRot[LINE_SEG_COUNT - 2].x + M_PIf;
                        D_809101B0 = sReelLineRot[LINE_SEG_COUNT - 2].y + M_PIf;
                        D_809101C0 += 2.0f;
                    }
                }
            }

            Math_ApproachZeroF(&D_809101B4, 1.0f, 0.3f);
            Math_ApproachS(&D_809101B8, RAD_TO_BINANG_ALT2(D_809101B0), 3, spDC);

            sLureRot.y = BINANG_TO_RAD_ALT(D_809101B8);

            sp90.x = 0.0f;
            sp90.y = 0.0f;
            sp90.z = D_809101B4;

            Matrix_RotateYF(sLureRot.y, MTXMODE_NEW);

            if (D_80917206 == 2) {
                s8 requiredScopeTemp;

                Matrix_MultVec3f(&sp90, &sp64);
                D_80917278.x = sp64.x;
                D_80917278.z = sp64.z;
                phi_f0 = 10.0f;
            } else {
                Matrix_MultVec3f(&sp90, &D_80917278);
                phi_f0 = 0.0f;
            }

            D_80917254 = 0.0f;

            if ((D_80917206 == 1) && CHECK_BTN_ALL(input->cur.button, BTN_A)) {
                D_80917278.y = -2.0f;

                if (D_809171FE & 1) {
                    D_80917254 = 0.5f;
                } else {
                    D_80917254 = -0.5f;
                }
            } else if (sReelLinePos[LINE_SEG_COUNT - 1].y < (WATER_SURFACE_Y(play) + phi_f0)) {
                if (D_80917206 == 2) {
                    s8 requiredScopeTemp;

                    sp58 = this->actor.world.pos;
                    this->actor.prevPos = this->actor.world.pos = sLurePos;
                    Actor_UpdateBgCheckInfo(play, &this->actor, 15.0f, 30.0f, 30.0f,
                                            UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40);
                    this->actor.world.pos = sp58;

                    D_80917278.y += -0.5f;
                    if (D_80917278.y < -1.0f) {
                        D_80917278.y = -1.0f;
                    }

                    if (sLurePos.y < (this->actor.floorHeight + 5.0f)) {
                        sReelLinePos[LINE_SEG_COUNT - 1].y = sLurePos.y = this->actor.floorHeight + 5.0f;
                        D_80917278.y = 0.0f;
                    } else {
                        D_80917270 = 1;
                    }
                } else {
                    D_80917278.y = fabsf(sReelLinePos[LINE_SEG_COUNT - 1].y - WATER_SURFACE_Y(play)) * 0.2f;
                    if (D_80917278.y > 1.5f) {
                        D_80917278.y = 1.5f;
                    }
                }
            }

            sReelLinePos[LINE_SEG_COUNT - 1].x += D_80917278.x;
            sReelLinePos[LINE_SEG_COUNT - 1].y += D_80917278.y;
            sReelLinePos[LINE_SEG_COUNT - 1].z += D_80917278.z;

            if (sReelLinePos[LINE_SEG_COUNT - 1].y > (spE4 + 6.0f)) {
                sReelLinePos[LINE_SEG_COUNT - 1].y -= 5.0f;
            }

            D_80917238.x = D_80917238.y = D_80917238.z = D_80917248.y = 0.0f;

            if (CHECK_BTN_ALL(input->cur.button, BTN_A)) {
                if (CHECK_BTN_ALL(input->cur.button, BTN_R)) {
                    D_809101C0 += 1.5f;
                    Audio_PlaySfx(NA_SE_IT_FISHING_REEL_HIGH - SFX_FLAG);
                    Math_ApproachF(&D_809101D0, 1000.0f, 1.0f, 2.0f);
                } else {
                    D_809101C0 += D_8091726C;
                    Audio_PlaySfx(NA_SE_IT_FISHING_REEL_SLOW - SFX_FLAG);
                    Math_ApproachF(&D_809101D0, 1000.0f, 1.0f, 0.2f);
                }

                if (sReelLinePos[LINE_SEG_COUNT - 1].y > (WATER_SURFACE_Y(play) + 4.0f)) {
                    Math_ApproachF(&D_809101C4, 3.0f, 1.0f, 0.2f);
                } else {
                    Math_ApproachF(&D_809101C4, 1.0f, 1.0f, 0.2f);
                }
            } else {
                Math_ApproachF(&D_809101C4, 2.0f, 1.0f, 0.2f);
            }

            Math_ApproachF(&sLurePos.x, sReelLinePos[LINE_SEG_COUNT - 1].x, 1.0f, D_809101D0);
            Math_ApproachF(&sLurePos.y, sReelLinePos[LINE_SEG_COUNT - 1].y, 1.0f, D_809101D0);
            Math_ApproachF(&sLurePos.z, sReelLinePos[LINE_SEG_COUNT - 1].z, 1.0f, D_809101D0);

            if (D_809101B4 > 1.0f) {
                Math_ApproachF(&D_809101D0, 1000.0f, 1.0f, 1.0f);
            }

            Math_ApproachF(&D_809101D0, 1000.0f, 1.0f, 0.1f);

            if (D_809101C0 >= 195.0f) {
                D_809101C0 = 195.0f;
                D_8090CD14 = 0;
                D_809101C4 = 520.0f;
                D_8090CD4C = 3;
            }

            if ((sLurePos.y <= (WATER_SURFACE_Y(play) + 4.0f)) && (sLurePos.y >= (WATER_SURFACE_Y(play) - 4.0f))) {
                s8 phi_v0 = 63;

                if (CHECK_BTN_ALL(input->cur.button, BTN_A) || (D_809101B4 > 1.0f)) {
                    phi_v0 = 1;
                }

                if ((D_809171FE & phi_v0) == 0) {
                    spA8 = sLurePos;
                    spA8.y = WATER_SURFACE_Y(play);
                    EnFishing_SpawnRipple(NULL, play->specialEffects, &spA8, 30.0f, 300.0f, 150, 90);
                }
            }
        } break; // must be outside of the block

        case 4:
            if (this->unk_14F != 0) {
                this->unk_14F--;
                D_809101C0 += D_8091726C;
            }

            if (CHECK_BTN_ALL(input->cur.button, BTN_A)) {
                if ((SQ(sLurePos.x) + SQ(sLurePos.z)) > SQ(920.0f)) {
                    D_809101C0 += 1.0f;
                } else {
                    D_809101C0 += D_8091726C;
                }
                Audio_PlaySfx(NA_SE_IT_FISHING_REEL_SLOW - SFX_FLAG);
            }

            if ((D_809171FE & 0x1F) == 0) {
                if (D_80917274 || (D_80917206 != 2)) {
                    D_8090CD24 = 5;
                }
            }

            Math_ApproachF(&D_809101C4, 0.0f, 1.0f, 0.2f);
            break;

        case 5:
            D_809101C8 = 0.0005000001f;
            sReelLinePos[LINE_SEG_COUNT - 1].x = sLurePos.x;
            sReelLinePos[LINE_SEG_COUNT - 1].y = sLurePos.y;
            sReelLinePos[LINE_SEG_COUNT - 1].z = sLurePos.z;
            D_809101C4 = 2.0f;
            break;

        default:
            break;
    }
}

s32 func_809033F0(EnFishing* this, PlayState* play, u8 ignorePosCheck) {
    s16 i;
    s16 count;
    f32 scale;
    Vec3f pos;
    Vec3f velocity;
    f32 speedXZ;
    f32 angle;

    if ((this->actor.world.pos.y < (WATER_SURFACE_Y(play) - 10.0f)) && !ignorePosCheck) {
        return false;
    }

    // Necessary to match
    if (this->unk_1A4) {}

    if (this->unk_1A4 >= 40.0f) {
        count = 40;
        scale = 1.2f;
    } else {
        count = 30;
        scale = 1.0f;
    }

    for (i = 0; i < count; i++) {
        speedXZ = (Rand_ZeroFloat(1.5f) + 0.5f) * scale;
        angle = Rand_ZeroFloat(6.28f);

        velocity.x = sinf(angle) * speedXZ;
        velocity.z = cosf(angle) * speedXZ;
        velocity.y = (Rand_ZeroFloat(3.0f) + 3.0f) * scale;

        pos = this->actor.world.pos;
        pos.x += velocity.x * 3.0f;
        pos.y = WATER_SURFACE_Y(play);
        pos.z += velocity.z * 3.0f;

        EnFishing_SpawnDustSplash(&this->actor.projectedPos, play->specialEffects, &pos, &velocity,
                                  (Rand_ZeroFloat(0.02f) + 0.025f) * scale);
    }

    pos = this->actor.world.pos;
    pos.y = WATER_SURFACE_Y(play);

    EnFishing_SpawnRipple(&this->actor.projectedPos, play->specialEffects, &pos, 100.0f, 800.0f, 150, 90);

    this->unk_149 = 30;

    return true;
}

void func_809036BC(EnFishing* this, PlayState* play) {
    s16 count;
    s16 i;
    f32 scale;
    Vec3f pos;
    Vec3f velocity;
    f32 speedXZ;
    f32 angle;

    // Necessary to match
    if (this->unk_1A4) {}

    if (this->unk_1A4 >= 45.0f) {
        count = 30;
        scale = 0.5f;
    } else {
        count = 20;
        scale = 0.3f;
    }

    for (i = 0; i < count; i++) {
        speedXZ = (Rand_ZeroFloat(1.5f) + 0.5f) * scale;
        angle = Rand_ZeroFloat(6.28f);

        velocity.x = sinf(angle) * speedXZ;
        velocity.z = cosf(angle) * speedXZ;
        velocity.y = Rand_ZeroFloat(2.0f) + 2.0f;

        pos = this->actor.world.pos;
        pos.x += (velocity.x * 3.0f);
        pos.y += (velocity.y * 3.0f);
        pos.z += (velocity.z * 3.0f);

        EnFishing_SpawnDustSplash(&this->actor.projectedPos, play->specialEffects, &pos, &velocity,
                                  (Rand_ZeroFloat(0.02f) + 0.025f) * scale);
    }
}

void func_809038A4(EnFishing* this, Input* input) {
    Vec3f sp34;
    Vec3f sp28;
    f32 sp24;

    sp34.x = sLurePos.x - this->actor.world.pos.x;
    sp34.y = sLurePos.y - this->actor.world.pos.y;
    sp34.z = sLurePos.z - this->actor.world.pos.z;

    sp24 = SQ(sp34.x) + SQ(sp34.y) + SQ(sp34.z);

    if ((D_8090CD14 == 3) && (this->unk_19A == 0) && !D_8090CD0C) {
        Matrix_RotateYF(BINANG_TO_RAD_ALT(-this->actor.shape.rot.y), MTXMODE_NEW);
        Matrix_MultVec3f(&sp34, &sp28);

        if ((sp28.z > 0.0f) || (this->unk_1A4 < 40.0f)) {
            if ((this->unk_150 == 7) && (sp24 < SQ(200.0f))) {
                this->unk_150 = 4;
                this->unk_1AC = sLurePos;
                this->unk_1A8 = 28672.0f;
                this->unk_180 = 5.0f;
            } else {
                if ((CHECK_BTN_ALL(input->cur.button, BTN_A) || (D_809101B4 > 1.0f)) && (sp24 < SQ(120.0f))) {
                    this->unk_150 = 2;
                    this->unk_156 = 0;
                    this->unk_172[0] = 0;
                    this->unk_172[2] = Rand_ZeroFloat(100.0f) + 100.0f;
                    this->unk_1A0 = sFishInits[this->actor.params - 100].unk_0C;
                    this->unk_1A8 = 0.0f;
                }

                if ((this->unk_172[1] == 0) && (sp24 < SQ(70.0f))) {
                    this->unk_150 = 2;
                    this->unk_156 = 0;
                    this->unk_172[0] = 0;
                    this->unk_172[2] = Rand_ZeroFloat(100.0f) + 100.0f;
                    this->unk_1A0 = sFishInits[this->actor.params - 100].unk_0C;
                    this->unk_1A8 = 0.0f;
                }
            }
        }
    } else if ((D_8090CD14 == 4) && D_80917274 && (sp24 < SQ(100.0f)) && (this->unk_150 >= 10)) {
        this->unk_152 = 0;
        this->unk_150 = 1;
        this->unk_19C = 1000;
        this->unk_19A = 100;
        this->unk_172[1] = 50;
    }

    if ((D_80917206 != 2) && (D_80917264 != 0) && (this->unk_1A4 > 60.0f) && (sp24 < SQ(30.0f)) &&
        (this->unk_150 >= 10)) {
        this->unk_152 = 0;
        this->unk_150 = 1;
        this->unk_19C = 1000;
        this->unk_19A = 100;
        this->unk_172[1] = 50;
    }
}

void func_80903C60(EnFishing* this, u8 arg1) {
    u16 sfxId;
    u8 temp;

    if (this->unk_148 == 0) {
        temp = this->unk_1A4;
    } else {
        temp = 2.0f * this->unk_1A4;
    }

    if (!arg1) {
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

    Actor_PlaySfx(&this->actor, sfxId);
}

void EnFishing_HandleAquariumDialog(EnFishing* this, PlayState* play) {
    if (sLinkAge == 1) {
        if (HIGH_SCORE(HS_FISHING) & 0x7F) {
            if (HIGH_SCORE(HS_FISHING) & 0x80) {
                this->actor.textId = 0x40B1;
            } else {
                this->actor.textId = 0x4089;
            }
        } else {
            this->actor.textId = 0x40AE;
        }
    } else if (HIGH_SCORE(HS_FISHING) & 0x7F000000) {
        if (HIGH_SCORE(HS_FISHING) & 0x80000000) {
            this->actor.textId = 0x40B1;
        } else {
            this->actor.textId = 0x4089;
        }
    } else {
        this->actor.textId = 0x40AE;
    }

    if (this->unk_1CB == 0) {
        if (this->unk_1CC == 0) {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;

            if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
                D_8090CCF8 = D_809171CC;
                this->unk_1CB = 1;
            } else {
                Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
            }
        } else {
            this->unk_1CC--;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        }
    } else if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->unk_1CB = 0;
        this->unk_1CC = 20;
    }
}

void EnFishing_UpdateFish(Actor* thisx, PlayState* play2) {
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
    s16 spF8;
    s16 spF6;
    s16 spF4;
    s16 spF2;
    s16 spF0;
    s16 spEE;
    EnFishing* this = (EnFishing*)thisx;
    PlayState* play = play2;
    Player* player = GET_PLAYER(play);
    Input* input = CONTROLLER1(&play->state);
    f32 spD8;
    f32 phi_f0;
    f32 phi_f2;
    Vec3f spC4;
    Vec3f spB8;
    f32 temp_f0;
    f32 temp;
    f32 temp2;

    this->actor.cullingVolumeDistance = 700.0f;
    this->actor.cullingVolumeScale = 50.0f;

    if (this->unk_148 == 0) {
        sp118 = (player->actor.speed * 0.15f) + 0.25f;
    } else {
        sp118 = (player->actor.speed * 0.3f) + 0.25f;
    }

    if ((D_80917200 != 0) || (sSubCamId != SUB_CAM_ID_DONE) ||
        ((player->actor.world.pos.z > 1150.0f) && (this->unk_150 != 100))) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        if (D_8090CD14 != 0) {
            if (D_80917202 == 0) {
                this->actor.focus.pos = sLurePos;
            } else if (D_80917202 == 1) {
                D_8090CD4C = 1;
                D_80911F50 = 0.0f;
                D_809171DC = 2;
            }
        }
        this->actor.focus.pos = this->actor.world.pos;
    }

    this->unk_154++;

    for (i = 0; i < ARRAY_COUNT(this->unk_172); i++) {
        if (this->unk_172[i] != 0) {
            this->unk_172[i]--;
        }
    }

    if (this->unk_19C != 0) {
        this->unk_19C--;
    }

    if (this->unk_19A != 0) {
        this->unk_19A--;
    }

    if (this->unk_198 != 0) {
        this->unk_198--;
    }

    if (this->unk_149 != 0) {
        this->unk_149--;
    }

    Math_ApproachF(&this->unk_190, this->unk_188, 1.0f, 0.2f);

    if (this->unk_150 == 6) {
        Math_ApproachF(&this->unk_194, this->unk_18C, 0.2f, 200.0f);
    } else {
        phi_f0 = 1.0f;
        phi_f2 = 1.0f;
        if (this->actor.world.pos.y > WATER_SURFACE_Y(play)) {
            phi_f0 = 1.5f;
            phi_f2 = 3.0f;
        }
        Math_ApproachF(&this->unk_194, this->unk_18C * phi_f0, 1.0f, 500.0f * phi_f2);
    }

    Math_ApproachS(&this->unk_168, 0, 5, 500);

    if (this->unk_148 == 0) {
        Actor_SetScale(&this->actor, this->unk_1A4 * 15.0f * 0.00001f);

        this->unk_184 += this->unk_190;

        temp = cosf(this->unk_184);
        this->unk_164 = this->unk_166 + (temp * this->unk_194);

        temp = cosf(this->unk_184 + -1.2f);
        this->unk_16E = this->unk_166 + (temp * this->unk_194 * 1.6f);
    } else {
        Actor_SetScale(&this->actor, this->unk_1A4 * 65.0f * 0.000001f);

        this->actor.scale.x = this->actor.scale.z * 1.1f;
        this->actor.scale.y = this->actor.scale.z * 1.1f;

        this->unk_184 += this->unk_190 * 0.8f;

        for (i = 0; i < 3; i++) {
            temp = cosf(this->unk_184 + (i * 2.1f));
            this->unk_1C4[i] = this->unk_166 + (temp * this->unk_194 * 2.0f);
        }

        temp = cosf(this->unk_184 + 0.4f);
        this->unk_164 = (this->unk_194 * temp * 2.0f) * 0.6f;
    }

    sp130 = this->unk_1AC.x - this->actor.world.pos.x;
    sp12C = this->unk_1AC.y - this->actor.world.pos.y;
    sp128 = this->unk_1AC.z - this->actor.world.pos.z;

    spFC = Math_Atan2S_XY(sp128, sp130);
    sp124 = sqrtf(SQ(sp130) + SQ(sp128));

    spFE = Math_Atan2S_XY(sp124, sp12C);
    sp124 = sqrtf(SQ(sp130) + SQ(sp128) + SQ(sp12C));

    if ((this->unk_198 != 0) && (this->unk_150 != 2) && (this->unk_150 != 3) && (this->unk_150 != 4)) {
        if (this->unk_154 & 0x40) {
            spFC += 0x4000;
        } else {
            spFC -= 0x4000;
        }
        if ((this->unk_154 + 0x20) & 0x40) {
            spFE += 0x2000;
        } else {
            spFE -= 0x2000;
        }
    }

    switch (this->unk_150) {
        case 100:
            EnFishing_HandleAquariumDialog(this, play);

            this->actor.cullingVolumeDistance = 500.0f;
            this->actor.cullingVolumeScale = 300.0f;

            Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y + 20.0f,
                                      this->actor.world.pos.z - 50.0f, 255, 255, 255, 255);

            this->unk_1A4 = D_809171CC;
            sp100.y = Math_SinS(play->gameplayFrames * 300);
            sp100.z = Math_SinS(play->gameplayFrames * 230) * 2.0f;
            this->actor.world.pos.x = 130.0f;
            this->actor.world.pos.y = 55.0f + sp100.y;
            this->actor.world.pos.z = 1300.0f + sp100.z;
            this->actor.shape.rot.y = -0x8000;

            if ((this->actor.projectedPos.z < 200.0f) && (this->actor.projectedPos.z > 0.0f)) {
                spC4.x = Rand_CenteredFloat(5.0f) + 130.0f;
                spC4.y = 40.0f;
                spC4.z = Rand_CenteredFloat(5.0f) + 1280.0f;
                EnFishing_SpawnBubble(NULL, play->specialEffects, &spC4, Rand_ZeroFloat(0.02f) + 0.03f, 1);
            }

            Math_ApproachS(&this->unk_16A, (Math_SinS(this->unk_154 * 0x800) * 2500.0f) + 2500.0f, 2, 0x7D0);
            Math_ApproachS(&this->unk_16C, Math_SinS(this->unk_154 * 0xA00) * 1500.0f, 2, 0x7D0);

            this->unk_188 = 0.3f;
            this->unk_18C = 333.33334f;
            return;

        case 10:
            this->unk_1AC = this->actor.home.pos;

            Math_ApproachF(&this->actor.speed, 2.0f, 1.0f, 0.5f);
            Math_ApproachF(&this->unk_1A8, 4096.0f, 1.0f, 256.0f);

            if (sp124 < 40.0f) {
                this->unk_150 = 11;
                this->unk_188 = 0.4f;
                this->unk_18C = 500.0f;
            }

            func_809038A4(this, input);

            if (this->actor.xzDistToPlayer < (250.0f * sp118)) {
                this->unk_152 = this->unk_150 = 0;
                this->unk_19C = 1000;
                this->unk_19A = 200;
                this->unk_172[1] = 50;
            }
            break;

        case 11:
            this->unk_1AC = this->actor.home.pos;

            Math_ApproachF(&this->actor.speed, 0.0f, 1.0f, 0.05f);
            Math_ApproachF(&this->unk_1A8, 0.0f, 1.0f, 256.0f);

            if (sp124 >= 40.0f) {
                this->unk_150 = 10;
                this->unk_188 = 1.0f;
                this->unk_18C = 2000.0f;
            }
            func_809038A4(this, input);

            if (this->actor.xzDistToPlayer < (250.0f * sp118)) {
                this->unk_152 = this->unk_150 = 0;
                this->unk_19C = 1000;
                this->unk_19A = 200;
                this->unk_172[1] = 50;
            }

            if (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                if ((CURRENT_TIME >= CLOCK_TIME(18, 0)) && (CURRENT_TIME <= (CLOCK_TIME(18, 1) - 0x12))) {
                    this->unk_150 = 7;
                    this->unk_172[3] = Rand_ZeroFloat(150.0f) + 200.0f;
                }
                if ((CURRENT_TIME >= CLOCK_TIME(5, 30)) && (CURRENT_TIME <= (CLOCK_TIME(5, 31) - 0x13))) {
                    this->unk_150 = 7;
                    this->unk_172[3] = Rand_ZeroFloat(150.0f) + 200.0f;
                }
            }
            break;

        case 0:
            Math_ApproachF(&this->actor.speed, 1.0f, 1.0f, 0.05f);
            Math_ApproachF(&this->unk_1A8, 0.0f, 1.0f, 256.0f);

            if (this->unk_172[0] == 0) {
                if (this->unk_19C == 0) {
                    this->unk_150 = this->unk_152 = 10;
                } else {
                    this->unk_150 = 1;
                    this->unk_172[0] = Rand_ZeroFloat(30.0f) + 10.0f;
                    this->unk_1AC.x = Rand_CenteredFloat(300.0f);
                    this->unk_1AC.y = (WATER_SURFACE_Y(play) - 50.0f) - Rand_ZeroFloat(50.0f);
                    this->unk_1AC.z = Rand_CenteredFloat(300.0f);
                    this->unk_188 = 1.0f;
                    this->unk_18C = 2000.0f;
                }
            }

            if (D_80917206 == 2) {
                func_809038A4(this, input);
            } else {
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            }
            break;

        case 1:
            if (this->unk_148 == 1) {
                this->unk_150 = -1;
                this->unk_19C = 20000;
                this->unk_19A = 20000;
                this->unk_1AC.x = 0.0f;
                this->unk_1AC.y = -140.0f;
                this->unk_1AC.z = 0.0f;
            } else {
                Math_ApproachF(&this->unk_1A8, 4096.0f, 1.0f, 256.0f);

                if ((this->actor.xzDistToPlayer < (250.0f * sp118)) || (this->unk_172[1] != 0)) {
                    Math_ApproachF(&this->unk_1A8, 8192.0f, 1.0f, 768.0f);
                    Math_ApproachF(&this->actor.speed, 4.2f, 1.0f, 0.75f);
                    this->unk_188 = 1.2f;
                    this->unk_18C = 4000.0f;
                    this->unk_172[0] = 20;
                } else {
                    this->unk_188 = 1.0f;
                    this->unk_18C = 2000.0f;
                    Math_ApproachF(&this->actor.speed, 1.5f, 1.0f, 0.1f);
                }

                if ((this->unk_172[0] == 0) || (sp124 < 50.0f)) {
                    this->unk_150 = 0;
                    this->unk_172[0] = Rand_ZeroFloat(30.0f) + 3.0f;
                    this->unk_188 = 1.0f;
                    this->unk_18C = 500.0f;
                }

                if (D_80917206 == 2) {
                    func_809038A4(this, input);
                } else {
                    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                }
            }
            break;

        case -1:
            Math_ApproachS(&this->unk_15E, 0, 0x14, 0x20);

            if ((this->actor.xzDistToPlayer < (250.0f * sp118)) || (this->unk_172[1] != 0)) {
                Math_ApproachF(&this->actor.speed, 3.0f, 1.0f, 0.75f);
                this->unk_188 = 1.0f;
                this->unk_172[0] = 20;
                this->unk_18C = 4000.0f;
                Math_ApproachF(&this->unk_1A8, 4096.0f, 1.0f, 256.0f);

                if ((play->gameplayFrames % 32) == 0) {
                    this->unk_1AC.x = Rand_CenteredFloat(600.0f);
                    this->unk_1AC.z = Rand_CenteredFloat(600.0f);
                    this->unk_1AC.y = -120.0f;
                }
            } else if (sp124 > 50.0f) {
                this->unk_188 = 0.8f;
                this->unk_18C = 1500.0f;
                Math_ApproachF(&this->actor.speed, 1.0f, 1.0f, 0.1f);
                Math_ApproachF(&this->unk_1A8, 2048.0f, 1.0f, 128.0f);
            } else {
                this->unk_188 = 0.4f;
                this->unk_18C = 500.0f;
                Math_ApproachZeroF(&this->actor.speed, 1.0f, 0.02f);
                Math_ApproachF(&this->unk_1A8, 0.0f, 1.0f, 256.0f);
            }

            if (this->unk_19C == 0) {
                this->unk_150 = 10;
                this->unk_152 = 10;
            } else if (((this->unk_19C & 0x7FF) == 0) && (this->unk_19C < 15000)) {
                this->unk_150 = -2;
                this->actor.world.rot.x = this->actor.shape.rot.x = 0;
                this->unk_1AC.y = WATER_SURFACE_Y(play) + 10.0f;
                this->unk_1AC.x = Rand_ZeroFloat(50.0f);
                this->unk_1AC.z = Rand_ZeroFloat(50.0f);
            }

            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            break;

        case -2:
            if ((this->actor.xzDistToPlayer < (250.0f * sp118)) || (this->unk_172[1] != 0)) {
                this->unk_150 = -1;
                this->unk_1AC.y = -120.0f;
            } else {
                this->unk_188 = 0.6f;
                this->unk_18C = 1000.0f;

                Math_ApproachS(&this->unk_15E, -0x1000, 0x14, 0x100);

                if (this->actor.world.pos.y < (WATER_SURFACE_Y(play) - 20.0f)) {
                    Math_ApproachF(&this->actor.speed, 0.5f, 1.0f, 0.1f);
                } else {
                    Math_ApproachZeroF(&this->actor.speed, 1.0f, 0.01f);

                    if ((this->actor.speed == 0.0f) || (this->actor.world.pos.y > (WATER_SURFACE_Y(play) - 5.0f))) {
                        this->unk_1AC.x = Rand_ZeroFloat(300.0f);
                        this->unk_1AC.z = Rand_ZeroFloat(300.0f);
                        this->unk_1AC.y = this->actor.floorHeight + 10.0f;
                        this->unk_150 = -25;
                        this->unk_1A8 = 0.0f;

                        spB8 = this->fishMouthPos;
                        spB8.y = WATER_SURFACE_Y(play);
                        EnFishing_SpawnRipple(&this->actor.projectedPos, play->specialEffects, &spB8, 10.0f, 300.0f,
                                              150, 90);
                        EnFishing_SpawnRipple(&this->actor.projectedPos, play->specialEffects, &spB8, 30.0f, 400.0f,
                                              150, 90);

                        Actor_PlaySfx(&this->actor, NA_SE_PL_CATCH_BOOMERANG);
                        break;
                    }
                }

                Math_ApproachF(&this->unk_1A8, 2048.0f, 1.0f, 128.0f);
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            }
            break;

        case -25:
            if ((this->actor.xzDistToPlayer < (250.0f * sp118)) || (this->unk_172[1] != 0)) {
                this->unk_150 = -1;
                this->unk_1AC.y = -120.0f;
            } else {
                Math_ApproachS(&this->unk_15E, 0x1000, 0x14, 0x6A);

                if (sp124 > 40.0f) {
                    this->unk_188 = 0.7f;
                    this->unk_18C = 1200.0f;
                    Math_ApproachF(&this->actor.speed, 0.5f, 1.0f, 0.01f);
                    Math_ApproachF(&this->unk_1A8, 2048.0f, 1.0f, 128.0f);
                } else {
                    this->unk_150 = -1;
                }
            }
            break;

        case 2:
            if ((this->actor.params + D_80917268) & 1) {
                sp10C.x = 10.0f;
            } else {
                sp10C.x = -10.0f;
            }
            sp10C.y = 0.0f;
            sp10C.z = 0.0f;
            Matrix_RotateYF(sLureRot.y, MTXMODE_NEW);
            Matrix_MultVec3f(&sp10C, &sp100);

            this->unk_1AC.x = sLurePos.x + sp100.x;
            this->unk_1AC.z = sLurePos.z + sp100.z;

            if (D_80917206 == 2) {
                this->unk_1AC.y = sLurePos.y;
            } else if (this->unk_148 == 0) {
                this->unk_1AC.y = sLurePos.y - 15.0f;
            } else {
                this->unk_1AC.y = sLurePos.y - 5.0f;
            }

            if (this->unk_1AC.y <= this->actor.floorHeight) {
                this->unk_1AC.y = this->actor.floorHeight + 3.0f;
            }

            if ((D_80917206 != 2) && (this->unk_1AC.y < this->actor.world.pos.y)) {
                Math_ApproachF(&this->actor.world.pos.y, this->unk_1AC.y, 0.1f,
                               (this->actor.world.pos.y - this->unk_1AC.y) * 0.1f);
            }

            Math_ApproachF(&this->unk_1A8, 8192.0f, 1.0f, 384.0f);
            if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                this->unk_1A0 += 0.005f;
            }

            if (D_80917270 != 0) {
                if (D_80917270 == 1) {
                    this->unk_1A0 += 0.01f;
                } else {
                    this->unk_1A0 += 0.05f;
                }
                D_80917270 = 0;
            }

            if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                this->unk_1A0 += 0.008f;
            }

            if (sp124 < ((this->unk_1A4 * 0.5f) + 20.0f)) {
                if (this->unk_156 == 0) {
                    this->unk_188 = 1.0f;
                    this->unk_18C = 500.0f;
                    this->unk_172[0] = Rand_ZeroFloat(10.0f) + 2.0f;
                }
                Math_ApproachF(&this->actor.speed, -0.2f, 1.0f, 0.1f);
                this->unk_156 = 1;
            } else {
                if (this->unk_156 != 0) {
                    this->unk_188 = 1.0f;
                    this->unk_1A8 = 0.0f;
                    this->unk_18C = 3000.0f;
                }
                Math_ApproachF(&this->actor.speed, 3.0f, 1.0f, 0.15f);
                this->unk_156 = 0;
            }

            if (this->unk_1A4 >= 60.0f) {
                multiplier = 0.3f;
            } else if (this->unk_1A4 >= 45.0f) {
                multiplier = 0.6f;
            } else {
                multiplier = 1.0f;
            }

            if ((CURRENT_TIME >= CLOCK_TIME(17, 0)) && (CURRENT_TIME <= CLOCK_TIME(19, 0))) {
                multiplier *= 1.75f;
            } else if ((CURRENT_TIME >= CLOCK_TIME(5, 0)) && (CURRENT_TIME <= CLOCK_TIME(7, 0))) {
                multiplier *= 1.5f;
            } else if (D_809171CA) {
                multiplier *= 1.5f;
            } else if ((u8)D_8090CCD0 != 0) {
                multiplier *= 3.0f;
            }

            sp11C = 0.03f * multiplier;
            if (D_80917206 == 2) {
                sp11C *= 5.0f;
            }

            if (((this->unk_172[0] == 1) || (Rand_ZeroOne() < sp11C)) &&
                (Rand_ZeroOne() < (this->unk_1A0 * multiplier))) {
                if (this->unk_148 == 0) {
                    this->unk_150 = 3;
                    this->unk_188 = 1.2f;
                    this->unk_18C = 5000.0f;
                    this->unk_172[0] = Rand_ZeroFloat(10.0f);
                } else {
                    this->unk_150 = -3;
                    this->unk_188 = 1.0f;
                    this->unk_18C = 3000.0f;
                    this->unk_172[0] = 40;
                }
                if (D_80917206 == 2) {
                    this->unk_180 = Rand_ZeroFloat(1.5f) + 3.0f;
                } else {
                    this->unk_180 = Rand_ZeroFloat(1.5f) + 4.5f;
                }
            }

            if ((D_8090CD14 != 3) || (this->unk_172[2] == 0) ||
                (sqrtf(SQ(this->actor.world.pos.x) + SQ(this->actor.world.pos.z)) > 800.0f)) {
                this->unk_150 = this->unk_152;
                this->unk_172[1] = Rand_ZeroFloat(30.0f) + 50.0f;
                this->unk_172[0] = Rand_ZeroFloat(10.0f) + 5.0f;
                this->unk_188 = 1.0f;
                this->unk_1A8 = 0.0f;
                this->unk_18C = 2000.0f;
            }

            if (this->actor.xzDistToPlayer < (100.0f * sp118)) {
                this->unk_152 = this->unk_150 = 0;
                this->unk_19C = 1000;
                this->unk_19A = 200;
                this->unk_172[1] = 50;
            }
            break;

        case 3:
            this->unk_149 = 6;
            sp134 = 2;

            if ((TRUNCF_BINANG(player->actor.world.pos.x) + D_80917268) & 1) {
                sp10C.x = 30.0f;
            } else {
                sp10C.x = -30.0f;
            }
            sp10C.y = 0.0f;
            sp10C.z = 30.0f;

            Matrix_RotateYF(sLureRot.y, MTXMODE_NEW);
            Matrix_MultVec3f(&sp10C, &sp100);

            this->unk_1AC.x = sLurePos.x + sp100.x;
            this->unk_1AC.z = sLurePos.z + sp100.z;
            this->unk_1AC.y = sLurePos.y - 10.0f;
            this->unk_1A8 = 4096.0f;
            Math_ApproachF(&this->actor.speed, this->unk_180 * 0.8f, 1.0f, 1.0f);

            if ((D_8090CD14 != 3) || (sLurePos.y > (WATER_SURFACE_Y(play) + 5.0f)) ||
                (sqrtf(SQ(sLurePos.x) + SQ(sLurePos.z)) > 800.0f)) {
                this->unk_150 = this->unk_152;
                this->unk_172[0] = 0;
                this->unk_188 = 1.0f;
                this->unk_18C = 2000.0f;
            } else if ((this->unk_172[0] == 0) || (sp124 < 30.0f)) {
                this->unk_150 = 4;
                this->unk_1AC = sLurePos;
                this->unk_1A8 = 16384.0f;
                this->unk_188 = 1.2f;
                this->unk_18C = 5000.0f;
                this->unk_172[0] = 20;
            }
            break;

        case 4:
            Math_ApproachF(&this->unk_1A8, 16384.0f, 1.0f, 4096.0f);
            Math_ApproachS(&this->unk_168, 0x4E20, 4, 0x1388);

            this->unk_149 = 50;
            sp134 = 2;
            this->unk_1AC = sLurePos;
            Math_ApproachF(&this->actor.speed, this->unk_180, 1.0f, 1.0f);

            if ((D_8090CD14 != 3) || (this->unk_172[0] == 0) || (sLurePos.y > (WATER_SURFACE_Y(play) + 5.0f)) ||
                (sqrtf(SQ(sLurePos.x) + SQ(sLurePos.z)) > 800.0f)) {

                this->unk_172[0] = 0;
                this->unk_150 = this->unk_152;
                this->unk_188 = 1.0f;
                this->unk_18C = 2000.0f;
            } else if (sp124 < 10.0f) {
                s16 phi_v0;

                if (func_809033F0(this, play, false)) {
                    func_80903C60(this, false);
                }

                this->unk_150 = 5;
                this->unk_188 = 1.2f;
                this->unk_18C = 5000.0f;
                this->unk_172[1] = 150;
                this->unk_172[0] = 0;
                this->unk_172[2] = 0;
                this->unk_172[3] = 120;

                D_8090CD14 = 4;
                sFishingHookedFish = this;
                sFishMouthOffset.y = 500.0f - Rand_ZeroFloat(400.0f);

                if (D_80917206 == 2) {
                    if (this->unk_1A4 > 70.0f) {
                        phi_v0 = Rand_ZeroFloat(20.0f) + 10.0f;
                    } else if (this->unk_1A4 > 60.0f) {
                        phi_v0 = Rand_ZeroFloat(30.0f) + 20.0f;
                    } else if (this->unk_1A4 > 50.0f) {
                        phi_v0 = Rand_ZeroFloat(30.0f) + 30.0f;
                    } else {
                        phi_v0 = Rand_ZeroFloat(40.0f) + 40.0f;
                    }
                    D_80917272 = phi_v0;
                    D_809171F4 = phi_v0;
                    Rumble_Override(0.0f, 60, phi_v0 * 3, 10);
                } else {
                    if (this->unk_1A4 > 70.0f) {
                        phi_v0 = Rand_ZeroFloat(5.0f) + 10.0f;
                    } else if (this->unk_1A4 > 60.0f) {
                        phi_v0 = Rand_ZeroFloat(5.0f) + 15.0f;
                    } else if (this->unk_1A4 > 50.0f) {
                        phi_v0 = Rand_ZeroFloat(5.0f) + 17.0f;
                    } else {
                        phi_v0 = Rand_ZeroFloat(5.0f) + 25.0f;
                    }
                    D_80917272 = phi_v0;
                    D_809171F4 = phi_v0;
                    Rumble_Override(0.0f, 180, phi_v0 * 3, 10);
                }

                D_80917274 = false;
                D_80917266 = 100;
                D_809171D4 = 0;
            }
            break;

        case -3:
            this->unk_149 = 50;
            this->unk_1AC = sLurePos;
            Math_ApproachF(&this->actor.speed, 2.0f, 1.0f, 1.0f);

            if ((D_8090CD14 != 3) || (this->unk_172[0] == 0) || (sLurePos.y > (WATER_SURFACE_Y(play) + 5.0f)) ||
                (sqrtf(SQ(sLurePos.x) + SQ(sLurePos.z)) > 800.0f)) {

                this->unk_172[0] = 0;
                this->unk_188 = 1.0f;
                this->unk_150 = this->unk_152;
                this->unk_18C = 2000.0f;
            } else if (sp124 < 10.0f) {
                if (sLurePos.y > (WATER_SURFACE_Y(play) - 10.0f)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EV_JUMP_OUT_WATER);
                    Audio_PlaySfx(NA_SE_PL_CATCH_BOOMERANG);
                }

                func_809033F0(this, play, false);
                this->unk_150 = 5;
                this->unk_188 = 1.2f;
                this->unk_18C = 5000.0f;
                this->unk_172[1] = 150;
                this->unk_172[0] = 0;
                this->unk_172[2] = 0;
                this->unk_172[3] = 120;

                D_8090CD14 = 4;
                sFishingHookedFish = this;

                if (D_80917206 == 2) {
                    D_80917272 = 30;
                    D_809171F4 = 100;
                    Rumble_Override(0.0f, 60, 90, 10);
                } else {
                    D_80917272 = 30;
                    D_809171F4 = 40;
                    Rumble_Override(0.0f, 180, 90, 10);
                }

                D_80917274 = false;
                D_80917266 = 100;
                D_809171D4 = 0;
            }
            break;

        case 5:
            this->actor.cullingVolumeDistance = 1200.0f;
            this->actor.cullingVolumeScale = 200.0f;

            D_809171D4++;

            Math_ApproachS(&this->unk_168, 0x2AF8, 4, 0xBB8);
            sFishingHookedFish = this;
            Math_ApproachS(&player->actor.shape.rot.y, this->actor.yawTowardsPlayer + 0x8000, 5, 0x500);

            if (D_80917274 == 0) {
                if ((D_80911F20 < 20) && ((D_809171FE & 3) == 0)) {
                    D_80911F20++;
                }
            }

            if ((D_80917272 != 0) && (D_80917274 == 0)) {
                if (((input->rel.stick_y < -50) && (D_8090CD48 > -40)) || CHECK_BTN_ALL(input->press.button, BTN_A)) {
                    if (input->rel.stick_y < -50) {
                        temp_f0 = 40.0f - ((this->unk_1A4 - 30.0f) * 1.333333f);
                        if (temp_f0 > 0.0f) {
                            this->unk_14A = temp_f0;
                            this->unk_14C = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                            this->unk_14E = 1;
                        }
                    }

                    this->unk_190 = 1.7f;
                    this->unk_194 = 7000.0f;
                    D_80917274 = true;
                    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_ENEMY | 0x800 | SEQ_FLAG_ASYNC);
                    D_809171F6 = 0;

                    if (this->unk_148 == 1) {
                        temp2 = (this->unk_1A4 * 3.0f) + 120.0f;
                    } else {
                        temp2 = (2.0f * this->unk_1A4) + 120.0f;
                    }
                    if (temp2 > 255.0f) {
                        temp2 = 255.0f;
                    }

                    Rumble_Override(0.0f, temp2, 120, 5);
                    D_809171F4 = 40;
                    D_80911E28 = 10;
                    Audio_PlaySfx(NA_SE_IT_FISHING_HIT);
                }
            }

            if (this->actor.world.pos.y < WATER_SURFACE_Y(play)) {
                u8 phi_v0_2;
                f32 spA4;

                if (this->unk_172[1] > 30) {
                    phi_v0_2 = 7;
                } else {
                    phi_v0_2 = 0xF;
                }

                if (((this->unk_154 & phi_v0_2) == 0) && (Rand_ZeroOne() < 0.75f) && (D_809171F4 == 0)) {
                    if (this->unk_1A4 >= 70.0f) {
                        spA4 = 255.0f;
                    } else if (this->unk_1A4 >= 60.0f) {
                        spA4 = 230.0f;
                    } else if (this->unk_1A4 >= 50.0f) {
                        spA4 = 200.0f;
                    } else if (this->unk_1A4 >= 40.0f) {
                        spA4 = 170.0f;
                    } else {
                        spA4 = 140.0f;
                    }

                    if (phi_v0_2 == 0xF) {
                        spA4 *= 3.0f / 4.0f;
                    }

                    Rumble_Override(0.0f, spA4, Rand_ZeroFloat(5.0f) + 10.0f, 5);
                }

                if (this->unk_172[1] > 30) {
                    if (this->unk_172[0] == 0) {
                        u16 spA2;

                        sp10C.x = 0.0f;
                        sp10C.y = 0.0f;
                        sp10C.z = 200.0f;

                        for (spA2 = 0; spA2 < 100; spA2++) {

                            Matrix_RotateYF(Rand_CenteredFloat(0.75f * M_PIf) +
                                                BINANG_TO_RAD_ALT(this->actor.yawTowardsPlayer + 0x8000),
                                            MTXMODE_NEW);
                            Matrix_MultVec3f(&sp10C, &sp100);

                            this->unk_1AC.x = this->actor.world.pos.x + sp100.x;
                            this->unk_1AC.z = this->actor.world.pos.z + sp100.z;

                            if ((SQ(this->unk_1AC.x) + SQ(this->unk_1AC.z)) < SQ(750.0f)) {
                                break;
                            }
                        }

                        if ((Rand_ZeroOne() < 0.1f) && (this->unk_172[3] == 0)) {
                            u8 phi_a1;

                            if (this->unk_1A4 >= 60.0f) {
                                phi_a1 = 255;
                            } else if (this->unk_1A4 >= 50.0f) {
                                phi_a1 = 200;
                            } else {
                                phi_a1 = 180;
                            }
                            Rumble_Override(0.0f, phi_a1, 90, 2);
                            this->unk_172[0] = 20;
                            this->unk_172[1] = 100;
                            this->unk_172[2] = 20;
                            this->unk_172[3] = 100;
                            this->unk_1AC.y = 300.0f;
                            D_809171F4 = 0x28;
                            D_80917266 = Rand_ZeroFloat(30.0f) + 20.0f;
                        } else {
                            this->unk_172[0] = Rand_ZeroFloat(10.0f) + 3.0f;
                            this->unk_172[2] = 0;
                            this->unk_1AC.y = -70.0f - Rand_ZeroFloat(150.0f);
                        }
                    }

                    if (this->unk_172[2] != 0) {
                        D_8091726C = 0.0f;
                        this->unk_188 = 1.6f;
                        this->unk_18C = 6000.0f;
                        Math_ApproachF(&this->actor.speed, 7.5f, 1.0f, 1.0f);
                        Math_ApproachS(&this->unk_168, 0x4E20, 2, 0xFA0);
                    } else {
                        if (!D_80917274 && (D_80917206 == 2)) {
                            this->unk_188 = 1.0f;
                            this->unk_18C = 2000.0f;
                            Math_ApproachF(&this->actor.speed, 3.0f, 1.0f, 0.2f);
                        } else {
                            this->unk_188 = 1.4f;
                            this->unk_18C = 5000.0f;
                            Math_ApproachF(&this->actor.speed, 5.0f, 1.0f, 0.5f);
                        }

                        if (this->unk_148 == 0) {
                            D_8091726C = 1.0f - (this->unk_1A4 * 0.00899f);
                        } else {
                            D_8091726C = 1.0f - (this->unk_1A4 * 0.00899f * 1.4f);
                        }
                    }
                } else {
                    if (((this->unk_172[1] & 0xF) == 0) && CHECK_BTN_ALL(input->cur.button, BTN_A) &&
                        (!(this->unk_1A4 >= 60.0f) || (D_809171D4 >= 2000))) {
                        this->unk_14A = Rand_ZeroFloat(30.0f) + 15.0f;
                        this->unk_14C = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                    }

                    this->unk_188 = 1.0f;
                    this->unk_18C = 4500.0f;

                    if (this->unk_148 == 0) {
                        D_8091726C = 1.3f - (this->unk_1A4 * 0.00899f);
                    } else {
                        D_8091726C = 1.3f - (this->unk_1A4 * 0.00899f * 1.4f);
                    }

                    Math_ApproachF(&this->actor.speed, 2.0f, 1.0f, 0.5f);

                    if (this->unk_172[1] == 0) {
                        this->unk_14A = 0;

                        if (D_809171D4 < 2000) {
                            this->unk_172[1] = Rand_ZeroFloat(50.0f) + 50.0f;
                        } else if (D_809171D4 < 3000) {
                            this->unk_172[1] = Rand_ZeroFloat(20.0f) + 30.0f;
                        } else {
                            this->unk_172[1] = Rand_ZeroFloat(10.0f) + 25.0f;
                        }
                    }
                }
            }

            if (D_809171C8) {
                D_8091726C = 0.0f;
            }

            if (D_80917274 || (D_80917206 != 2)) {
                if (this->actor.speed < 3.0f) {
                    if (D_809171FE & 8) {
                        sp100.x = -0.8f;
                    } else {
                        sp100.x = -0.75f;
                    }
                } else if (D_809171FE & 4) {
                    sp100.x = -0.9f;
                } else {
                    sp100.x = -0.85f;
                }

                Math_ApproachF(&D_8090CD40, 35.0f, 0.1f, 3.5f);
                Math_ApproachF(&D_8090CD3C, sp100.x, 0.3f, 0.1f);
            }

            sReelLinePos[LINE_SEG_COUNT - 1] = this->fishMouthPos;
            sp10C.x = sReelLinePos[LINE_SEG_COUNT - 1].x - sReelLinePos[LINE_SEG_COUNT - 2].x;
            sp10C.y = sReelLinePos[LINE_SEG_COUNT - 1].y - sReelLinePos[LINE_SEG_COUNT - 2].y;
            sp10C.z = sReelLinePos[LINE_SEG_COUNT - 1].z - sReelLinePos[LINE_SEG_COUNT - 2].z;

            if ((SQ(sp10C.x) + SQ(sp10C.y) + SQ(sp10C.z)) > SQ(20.0f)) {
                Math_ApproachF(&this->actor.world.pos.x, sReelLinePos[LINE_SEG_COUNT - 2].x, 0.2f,
                               2.0f * (this->actor.speed * 1.5f));
                Math_ApproachF(&this->actor.world.pos.y, sReelLinePos[LINE_SEG_COUNT - 2].y, 0.2f,
                               2.0f * (this->actor.speed * 1.5f) * 5.0f * 0.1f);
                Math_ApproachF(&this->actor.world.pos.z, sReelLinePos[LINE_SEG_COUNT - 2].z, 0.2f,
                               2.0f * (this->actor.speed * 1.5f));
            }

            if (CHECK_BTN_ALL(input->cur.button, BTN_A) || (input->rel.stick_y < -30)) {
                if (D_80917266 < 100) {
                    D_80917266++;
                }
            } else {
                if (D_80917266 != 0) {
                    D_80917266--;
                }
            }

            if ((D_8090CD14 < 3) || (D_809171C8 && (D_809171D4 > 50)) || (D_809171D4 >= 6000) ||
                ((D_80917272 == 0) && !D_80917274) || (D_80917266 == 0) ||
                (((D_809171FE & 0x7F) == 0) && (Rand_ZeroOne() < 0.05f) && (D_80917206 != 2))) {
                // Assignment of OoT's D_80B7A67C here removed in MM

                if ((D_80917272 == 0) && !D_80917274) {
                    // Assignment of OoT's D_80B7E086 here removed in MM
                    if (((sLinkAge == 1) && (HIGH_SCORE(HS_FISHING) & 0x400)) ||
                        ((sLinkAge != 1) && (HIGH_SCORE(HS_FISHING) & 0x800))) {
                        // Assignment of OoT's D_80B7A67C here removed in MM, this is now an empty branch
                    }
                } else {
                    // Assignment of OoT's D_80B7E086 here removed in MM
                    Rumble_Override(0.0f, 1, 3, 1);
                    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 10);
                }

                this->unk_150 = this->unk_152 = 0;
                this->unk_19C = 10000;
                this->unk_19A = 500;
                this->unk_172[1] = 50;
                this->unk_172[0] = 0;
                this->unk_188 = 1.0f;
                this->unk_18C = 3000.0f;

                if (D_8090CD14 == 4) {
                    D_8090CD14 = 3;
                }

                D_809171F6 = 50;
                D_8091726C = 0.5f;
                this->unk_14A = 0;
            } else if (this->actor.xzDistToPlayer < 50.0f) {
                this->unk_150 = 6;
                this->unk_172[0] = 100;
                player->unk_B28 = 3;
                Rumble_Override(0.0f, 1, 3, 1);
                D_809171D8++;
                Cutscene_StartManual(play, &play->csCtx);
                D_8090CD4C = 100;
                D_80911F48 = 45.0f;
                D_8090CD14 = 5;
                this->unk_188 = 1.0f;
                this->unk_18C = 500.0f;
                this->unk_194 = 5000.0f;

                if (this->actor.world.pos.y <= WATER_SURFACE_Y(play)) {
                    func_80903C60(this, true);
                    func_809033F0(this, play, true);
                }
                goto case_6;
            }
            break;

        case_6:
        case 6:
            Math_ApproachS(&this->unk_168, 0x2AF8, 2, 0xFA0);
            Math_ApproachF(&D_80911F48, 15.0f, 0.05f, 0.75f);

            sp10C.x = D_80911F48;
            if (sLinkAge != 1) {
                sp10C.y = 30.0f;
                sp10C.z = 55.0f;
            } else {
                sp10C.y = 10.0f;
                sp10C.z = 50.0f;
            }
            Matrix_RotateYF(BINANG_TO_RAD_ALT(player->actor.shape.rot.y), MTXMODE_NEW);
            Matrix_MultVec3f(&sp10C, &sSubCamEye);

            sSubCamEye.x += player->actor.world.pos.x;
            sSubCamEye.y += player->actor.world.pos.y;
            sSubCamEye.z += player->actor.world.pos.z;

            sSubCamAt = player->actor.world.pos;
            if (sLinkAge != 1) {
                sSubCamAt.y += 40.0f;
            } else {
                sSubCamAt.y += 25.0f;
            }

            if (this->unk_172[0] == 90) {
                // BENTODO This had | 0x900 which interfered with the 16 bit sequence IDs. Removing it doesn't seem to
                // do anything bad.
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_GET_HEART | SEQ_FLAG_ASYNC);
                D_8090CCFC = 40;

                if (this->unk_148 == 0) {
                    D_8090CCF8 = this->unk_1A4;

                    if (D_8090CCF8 >= 75) {
                        D_809171DA = 0x409F;
                    } else if (D_8090CCF8 >= 50) {
                        D_809171DA = 0x4091;
                    } else {
                        D_809171DA = 0x4083;
                    }
                } else {
                    D_8090CCF8 = 2.0f * this->unk_1A4;
                    D_809171DA = 0x4099;
                }

                this->unk_1CD = 0;
            }

            this->unk_158 = -0x4000;
            this->actor.shape.rot.y = player->actor.shape.rot.y + 0x5000;
            this->actor.shape.rot.x = this->actor.shape.rot.z = this->unk_15A = this->unk_15C = this->unk_166 = 0;

            sp10C.x = 4.0f;
            sp10C.y = -10.0f;
            sp10C.z = 5.0f;
            Matrix_MultVec3f(&sp10C, &sp100);
            Math_ApproachF(&this->actor.world.pos.x, player->bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND].x + sp100.x, 1.0f,
                           6.0f);
            Math_ApproachF(&this->actor.world.pos.y, player->bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND].y + sp100.y, 1.0f,
                           6.0f);
            Math_ApproachF(&this->actor.world.pos.z, player->bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND].z + sp100.z, 1.0f,
                           6.0f);

            D_809101C0 = 188.0f;

            if (this->unk_172[0] <= 50) {
                switch (this->unk_1CD) {
                    case 0:
                        if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) ||
                            Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                            if (Message_ShouldAdvance(play)) {
                                Message_CloseTextbox(play);
                                if (play->msgCtx.choiceIndex == 0) {
                                    if (D_8090CCF0 == 0.0f) {
                                        D_8090CCF0 = this->unk_1A4;
                                        D_809171D0 = this->unk_148;
                                        D_809171D2 = D_80917206;
                                        Actor_Kill(&this->actor);
                                    } else if ((this->unk_148 == 0) && (D_809171D0 == 0) &&
                                               (TRUNCF_BINANG(this->unk_1A4) < TRUNCF_BINANG(D_8090CCF0))) {
                                        this->unk_1CD = 1;
                                        this->unk_172[0] = 0x3C;
                                        Message_StartTextbox(play, 0x4098, NULL);
                                    } else {
                                        f32 temp1 = D_8090CCF0;
                                        s16 temp2 = D_809171D0;

                                        D_8090CCF0 = this->unk_1A4;
                                        D_809171D0 = this->unk_148;
                                        D_809171D2 = D_80917206;
                                        this->unk_1A4 = temp1;
                                        this->unk_148 = temp2;
                                    }
                                }
                                if (this->unk_1CD == 0) {
                                    D_8090CD14 = 0;
                                }
                            }
                        }
                        break;

                    case 1:
                        if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) ||
                            Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                            if (Message_ShouldAdvance(play)) {
                                Message_CloseTextbox(play);
                                if (play->msgCtx.choiceIndex != 0) {
                                    f32 temp1 = D_8090CCF0;
                                    s16 temp2 = D_809171D0;

                                    D_8090CCF0 = this->unk_1A4;
                                    D_809171D2 = D_80917206;
                                    this->unk_1A4 = temp1;
                                    this->unk_148 = temp2;
                                }
                                D_8090CD14 = 0;
                            }
                        }
                        break;

                    default:
                        break;
                }
            }

            if (D_8090CD14 == 0) {
                if (this->actor.update != NULL) {
                    this->unk_150 = this->unk_152 = 0;
                    this->unk_19C = 10000;
                    this->unk_19A = 500;
                    this->unk_172[1] = 50;
                    this->unk_172[0] = 0;
                    this->unk_188 = 1.0f;
                    this->unk_18C = 2000.0f;
                    SkelAnime_Free(&this->skelAnime, play);

                    if (this->unk_148 == 0) {
                        SkelAnime_InitFlex(play, &this->skelAnime, &gFishingBassSkel, &gFishingBassAnim, NULL, NULL, 0);
                        Animation_MorphToLoop(&this->skelAnime, &gFishingBassAnim, 0.0f);
                    } else {
                        SkelAnime_InitFlex(play, &this->skelAnime, &gFishingLoachSkel, &gFishingLoachAnim, NULL, NULL,
                                           0);
                        Animation_MorphToLoop(&this->skelAnime, &gFishingLoachAnim, 0.0f);
                    }
                }

                D_809101C4 = 520.0f;
                D_809101C0 = 195.0f;
                SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 10);
                D_809171F6 = 20;
                D_8090CD4C = 3;
            }
            break;

        case 7:
            this->unk_149 = 50;
            sp134 = 5;
            this->unk_1A8 = 12288.0f;

            if (this->actor.params < 104) {
                this->unk_1AC = sGroupFishes[this->actor.params - 100].pos;
                D_8090CF18 = 1;
            } else if (this->actor.params < 108) {
                this->unk_1AC = sGroupFishes[this->actor.params - 100 + 16].pos;
                D_8090CF18 = 2;
            } else {
                this->unk_1AC = sGroupFishes[this->actor.params - 100 + 32].pos;
                D_8090CF18 = 3;
            }

            Math_ApproachF(&this->actor.speed, 5.0f, 1.0f, 1.0f);

            if (sp124 < 20.0f) {
                Math_ApproachS(&this->unk_168, 0x4E20, 2, 0xFA0);

                if ((this->unk_172[2] == 0) && func_809033F0(this, play, false)) {
                    func_80903C60(this, Rand_ZeroFloat(1.99f));
                    this->unk_172[2] = Rand_ZeroFloat(20.0f) + 20.0f;
                }
            }

            if (this->unk_172[3] == 0) {
                this->unk_150 = 10;
                this->unk_152 = 10;
            } else {
                func_809038A4(this, input);
                if (this->actor.xzDistToPlayer < (100.0f * sp118)) {
                    this->unk_152 = this->unk_150 = 0;
                    this->unk_19C = 500;
                    this->unk_19A = 200;
                    this->unk_172[1] = 50;
                }
            }
            break;
    }

    Math_ApproachS(&this->unk_16A, (Math_SinS(this->unk_154 * 0x1000) * 5000.0f) + 5000.0f, 2, 0x7D0);

    if (this->unk_150 != 6) {
        if (this->actor.world.pos.y > WATER_SURFACE_Y(play)) {
            this->unk_188 = 1.5f;
            this->unk_18C = 5000.0f;

            Math_ApproachS(&this->unk_166, 0, 5, 0x7D0);

            spF4 = spF0 = spFA = 3;
            spF2 = spEE = 0x2000;

            this->unk_172[2] = 0;
            this->unk_17C -= 1.0f;
        } else {
            Math_ApproachZeroF(&this->unk_17C, 1.0f, 2.0f);
            if ((this->unk_150 != -1) && (this->unk_150 != -2) && (this->unk_150 != -25)) {
                this->unk_15E = 0;
            }

            this->unk_160 = this->unk_162 = 0;
            spF4 = spF0 = spFA = 4;
            spF2 = spEE = 0x2000;

            spF6 = EnFishing_SmoothStepToS(&this->actor.world.rot.y, spFC, sp134, this->unk_1A8) * 3.0f;
            Math_ApproachS(&this->actor.world.rot.x, spFE, sp134, this->unk_1A8 * 0.5f);

            if (spF6 > 0x1F40) {
                spF6 = 0x1F40;
            } else if (spF6 < -0x1F40) {
                spF6 = -0x1F40;
            }

            if (this->actor.speed >= 3.2f) {
                Math_ApproachS(&this->unk_166, spF6, 2, 0x4E20);
            } else {
                Math_ApproachS(&this->unk_166, spF6, 3, 0xBB8);
            }

            Actor_UpdateVelocityWithoutGravity(&this->actor);
        }

        Actor_UpdatePos(&this->actor);

        this->actor.world.pos.y += (this->unk_17C * 1.5f);

        if (this->unk_14A != 0) {
            this->unk_160 = this->unk_14C;
            this->unk_14A--;
            if (this->unk_14E != 0) {
                spF0 = 5;
                spEE = 0x4000;
            } else {
                spF0 = 10;
                spEE = 0x800;
            }
            this->unk_15E = -0x500 - this->actor.shape.rot.x;
            spF4 = 5;
            spF2 = 0x4000;
        } else {
            this->unk_14E = 0;
        }

        Math_ApproachS(&this->unk_158, this->unk_15E, spF4, spF2);
        Math_ApproachS(&this->unk_15A, this->unk_160, spF0, spEE);
        Math_ApproachS(&this->unk_15C, this->unk_162, spFA, 0x2000);

        if (this->actor.speed <= 0.5f) {
            Math_ApproachS(&this->actor.shape.rot.x, 0, 10, this->unk_170);
            Math_ApproachS(&this->unk_170, 0x500, 1, 0x20);
        } else {
            Math_ApproachS(&this->actor.shape.rot.x, -this->actor.world.rot.x, 10, 0x1000);
            this->unk_170 = 0;
        }

        this->actor.shape.rot.y = this->actor.world.rot.y;

        if ((this->unk_150 != -1) && (this->unk_150 != -2) && (this->unk_150 != -25)) {
            if ((this->actor.world.pos.y > WATER_SURFACE_Y(play)) && (this->actor.prevPos.y <= WATER_SURFACE_Y(play))) {
                func_809033F0(this, play, true);
                func_80903C60(this, true);
                this->unk_17C = this->actor.velocity.y;
                this->actor.velocity.y = 0.0f;
                this->unk_162 = TRUNCF_BINANG(Rand_CenteredFloat(0x8000));
            } else if ((this->actor.world.pos.y < WATER_SURFACE_Y(play)) &&
                       (this->actor.prevPos.y >= WATER_SURFACE_Y(play))) {
                if (this->unk_17C < -5.0f) {
                    this->unk_17C = -5.0f;
                }
                this->actor.world.rot.x = -0xFA0;
                func_809033F0(this, play, true);
                this->unk_1CA = 20;
                func_80903C60(this, false);
            }
        }

        if ((this->actor.world.pos.y < WATER_SURFACE_Y(play)) &&
            (this->actor.world.pos.y > (WATER_SURFACE_Y(play) - 10.0f)) && !(this->unk_154 & 1) &&
            (this->actor.speed > 0.0f)) {
            Vec3f pos = this->actor.world.pos;

            pos.y = WATER_SURFACE_Y(play);
            EnFishing_SpawnRipple(&this->actor.projectedPos, play->specialEffects, &pos, 80.0f, 500.0f, 150, 90);
        }

        if ((this->actor.speed > 0.0f) || (this->unk_150 == 5)) {
            f32 velocityY = this->actor.velocity.y;

            spD8 = this->unk_1A4 * 0.1f;

            this->actor.world.pos.y -= spD8;
            this->actor.prevPos.y -= spD8;
            this->actor.velocity.y = -1.0f;
            if (KREG(90) == 0) { // Check added in MM
                Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 30.0f, 100.0f,
                                        UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40);
            }
            this->actor.world.pos.y += spD8;
            this->actor.prevPos.y += spD8;

            this->actor.velocity.y = velocityY;

            if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                this->unk_198 = 20;
            }

            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                if (this->actor.world.pos.y > WATER_SURFACE_Y(play)) {
                    this->unk_17C = Rand_ZeroFloat(3.0f) + 3.0f;
                    this->actor.velocity.x = this->actor.world.pos.x * -0.003f;
                    this->actor.velocity.z = this->actor.world.pos.z * -0.003f;

                    Actor_PlaySfx(&this->actor, NA_SE_EV_FISH_LEAP);
                    func_809036BC(this, play);

                    if (Rand_ZeroOne() < 0.5f) {
                        this->unk_162 = 0x4000;
                    } else {
                        this->unk_162 = -0x4000;
                    }

                    if (Rand_ZeroOne() < 0.5f) {
                        this->unk_15E = 0;
                    } else {
                        this->unk_15E = TRUNCF_BINANG(Rand_CenteredFloat(0x20)) + 0x8000;
                    }

                    this->unk_160 = TRUNCF_BINANG(Rand_CenteredFloat(0x4000));
                    this->unk_188 = 1.0f;
                    this->unk_18C = 5000.0f;
                    this->unk_194 = 5000.0f;
                } else {
                    this->unk_17C = 0.0f;

                    if ((this->unk_150 == 5) && !(this->unk_154 & 1)) {
                        Vec3f pos;

                        pos.x = Rand_CenteredFloat(10.0f) + this->actor.world.pos.x;
                        pos.z = Rand_CenteredFloat(10.0f) + this->actor.world.pos.z;
                        pos.y = this->actor.floorHeight + 5.0f;
                        EnFishing_SpawnWaterDust(&this->actor.projectedPos, play->specialEffects, &pos,
                                                 (this->unk_1A4 * 0.005f) + 0.15f);
                    }
                }
            }
        }
    }

    if (this->unk_1CA != 0) {
        s16 i;
        Vec3f pos;
        f32 range = (this->unk_1A4 * 0.075f) + 10.0f;

        this->unk_1CA--;

        for (i = 0; i < 2; i++) {
            pos.x = Rand_CenteredFloat(range) + this->actor.world.pos.x;
            pos.y = Rand_CenteredFloat(range) + this->actor.world.pos.y;
            pos.z = Rand_CenteredFloat(range) + this->actor.world.pos.z;
            EnFishing_SpawnBubble(&this->actor.projectedPos, play->specialEffects, &pos, Rand_ZeroFloat(0.035f) + 0.04f,
                                  0);
        }
    }
}

s32 EnFishing_BassOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnFishing* this = (EnFishing*)thisx;

    if (limbIndex == FISHING_BASS_LIMB_JAW) {
        rot->z -= this->unk_168 - 11000;
    } else if ((limbIndex == FISHING_BASS_LIMB_MIDDLE_SEGMENT_ROOT) ||
               (limbIndex == FISHING_BASS_LIMB_BACK_SEGMENT_ROOT)) {
        rot->y += this->unk_164;
    } else if (limbIndex == FISHING_BASS_LIMB_END_ROOT) {
        rot->y += this->unk_16E;
    } else if (limbIndex == FISHING_BASS_LIMB_RIGHT_PECTORAL_FIN) {
        rot->y -= this->unk_16A;
    } else if (limbIndex == FISHING_BASS_LIMB_LEFT_PECTORAL_FIN) {
        rot->y += this->unk_16A;
    } else if (limbIndex == FISHING_BASS_LIMB_TOP_REAR_FIN) {
        rot->y += this->unk_16C;
    } else if (limbIndex == FISHING_BASS_LIMB_BOTTOM_REAR_FIN) {
        rot->y -= this->unk_16C;
    }

    return false;
}

void EnFishing_BassPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnFishing* this = (EnFishing*)thisx;

    if (limbIndex == FISHING_BASS_LIMB_JAW) {
        Matrix_MultVec3f(&sFishMouthOffset, &this->fishMouthPos);
    }
}

s32 EnFishing_LoachOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnFishing* this = (EnFishing*)thisx;

    if (limbIndex == FISHING_LOACH_LIMB_MIDDLE_SEGMENT) {
        rot->y += this->unk_1C4[0];
    } else if (limbIndex == FISHING_LOACH_LIMB_LOWER_BACK_FIN) {
        rot->y += this->unk_1C4[1];
    } else if (limbIndex == FISHING_LOACH_LIMB_TAIL_FIN) {
        rot->y += this->unk_1C4[2];
    }

    return false;
}

void EnFishing_LoachPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sLoachMouthOffset = { 500.0f, 500.0f, 0.0f };
    EnFishing* this = (EnFishing*)thisx;

    if (limbIndex == FISHING_LOACH_LIMB_JAW) {
        Matrix_MultVec3f(&sLoachMouthOffset, &this->fishMouthPos);
    }
}

void EnFishing_DrawFish(Actor* thisx, PlayState* play) {
    EnFishing* this = (EnFishing*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateYF(BINANG_TO_RAD_ALT(this->unk_15A + this->actor.shape.rot.y), MTXMODE_APPLY);
    Matrix_RotateXFApply(BINANG_TO_RAD_ALT(this->unk_158 + this->actor.shape.rot.x));
    Matrix_RotateZF(BINANG_TO_RAD_ALT(this->unk_15C + this->actor.shape.rot.z), MTXMODE_APPLY);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);

    if (this->unk_148 == 0) {
        Matrix_RotateYF(BINANG_TO_RAD(this->unk_164) - (M_PIf / 2), MTXMODE_APPLY);
        Matrix_Translate(0.0f, 0.0f, this->unk_164 * 10.0f * 0.01f, MTXMODE_APPLY);

        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              EnFishing_BassOverrideLimbDraw, EnFishing_BassPostLimbDraw, &this->actor);
    } else {
        Matrix_Translate(0.0f, 0.0f, 3000.0f, MTXMODE_APPLY);
        Matrix_RotateYF(BINANG_TO_RAD(this->unk_164), MTXMODE_APPLY);
        Matrix_Translate(0.0f, 0.0f, -3000.0f, MTXMODE_APPLY);
        Matrix_RotateYF(-(M_PIf / 2), MTXMODE_APPLY);

        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              EnFishing_LoachOverrideLimbDraw, EnFishing_LoachPostLimbDraw, &this->actor);
    }
}

void EnFishing_HandleReedContact(FishingProp* prop, Vec3f* entityPos) {
    f32 dx = prop->pos.x - entityPos->x;
    f32 dz = prop->pos.z - entityPos->z;
    f32 distXZ = sqrtf(SQ(dx) + SQ(dz));

    if (distXZ <= 20.0f) {
        prop->rotY = Math_Atan2F_XY(dz, dx);

        Math_ApproachF(&prop->rotX, (20.0f - distXZ) * 0.03f, 0.2f, 0.2f);
    }
}

void EnFishing_HandleLilyPadContact(FishingProp* prop, Vec3f* entityPos, u8 fishTimer) {
    f32 dx = prop->pos.x - entityPos->x;
    f32 dz = prop->pos.z - entityPos->z;
    f32 distXZ = sqrtf(SQ(dx) + SQ(dz));

    if (distXZ <= 40.0f) {
        Math_ApproachS(&prop->lilyPadAngle, Math_Atan2S_XY(dz, dx), 10, 0x300);
    }

    if (fishTimer && (distXZ <= 60.0f)) {
        f32 heightTarget = 1.0f;

        if (fishTimer >= 21) {
            heightTarget = 1.5f;
        }

        Math_ApproachF(&prop->lilyPadOffset, heightTarget, 0.1f, 0.2f);
    }
}

void EnFishing_UpdatePondProps(PlayState* play) {
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
                    EnFishing_HandleReedContact(prop, &player->actor.world.pos);

                    actor = play->actorCtx.actorLists[ACTORCAT_NPC].first;
                    while (actor != NULL) {
                        if ((actor->id != ACTOR_EN_FISHING) || (actor->params < 100)) {
                            actor = actor->next;
                        } else {
                            EnFishing_HandleReedContact(prop, &actor->world.pos);
                            actor = actor->next;
                        }
                    }

                    Math_ApproachZeroF(&prop->rotX, 0.05f, 0.05f);
                } else if (prop->type == FS_PROP_LILY_PAD) {
                    EnFishing_HandleLilyPadContact(prop, &player->actor.world.pos, 0);

                    actor = play->actorCtx.actorLists[ACTORCAT_NPC].first;
                    while (actor != NULL) {
                        if ((actor->id != ACTOR_EN_FISHING) || (actor->params < 100)) {
                            actor = actor->next;
                        } else {
                            EnFishing_HandleLilyPadContact(prop, &actor->world.pos, ((EnFishing*)actor)->unk_149);
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

    if (sSubCamId == SUB_CAM_ID_DONE) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &sFishingMain->collider.base);
    }
}

void EnFishing_DrawPondProps(PlayState* play) {
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
                Matrix_Translate(prop->pos.x, prop->pos.y, prop->pos.z, MTXMODE_NEW);
                Matrix_Scale(prop->scale, prop->scale, prop->scale, MTXMODE_APPLY);
                Matrix_RotateYF(prop->rotY, MTXMODE_APPLY);
                Matrix_RotateXFApply(prop->rotX);
                Matrix_RotateYF(prop->reedAngle, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gFishingReedModelDL);
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
                Matrix_Translate(prop->pos.x, prop->pos.y, prop->pos.z, MTXMODE_NEW);
                Matrix_Scale(prop->scale, prop->scale, prop->scale, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gFishingWoodPostModelDL);
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
                Matrix_Translate(prop->pos.x, prop->pos.y, prop->pos.z, MTXMODE_NEW);
                Matrix_Scale(prop->scale, 1.0f, prop->scale, MTXMODE_APPLY);
                Matrix_RotateYF(BINANG_TO_RAD(prop->lilyPadAngle), MTXMODE_APPLY);
                Matrix_Translate(0.0f, 0.0f, 20.0f, MTXMODE_APPLY);
                Matrix_RotateYF(prop->rotY, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gFishingLilyPadModelDL);
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
                Matrix_Translate(prop->pos.x, prop->pos.y, prop->pos.z, MTXMODE_NEW);
                Matrix_Scale(prop->scale, prop->scale, prop->scale, MTXMODE_APPLY);
                Matrix_RotateYF(prop->rotY, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gFishingRockModelDL);
            }
        }

        prop++;
    }

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnFishing_UpdateGroupFishes(PlayState* play) {
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

    if ((D_80917264 != 0) || (D_8090CD14 == 4)) {
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
    } else if (D_8090CF18 != 0) {
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
    } else if (D_8090CF18 != 0) {
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
    } else if (D_8090CF18 != 0) {
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
            spD4 = Math_Atan2S_XY(dz, dx);
            dist = sqrtf(SQ(dx) + SQ(dz));
            spD6 = Math_Atan2S_XY(dist, dy);

            if ((dist < 10.0f) || (((fish->timer % 32) == 0) && (Rand_ZeroOne() > 0.5f))) {
                fish->unk_10.y = basePos[groupIndex].y + Rand_CenteredFloat(10.0f);

                if (D_8090CF18 != 0) {
                    fish->unk_10.x = basePos[groupIndex].x + Rand_CenteredFloat(200.0f);
                    fish->unk_10.z = basePos[groupIndex].z + Rand_CenteredFloat(200.0f);
                } else {
                    fish->unk_10.x = basePos[groupIndex].x + Rand_CenteredFloat(100.0f);
                    fish->unk_10.z = basePos[groupIndex].z + Rand_CenteredFloat(100.0f);
                }

                ripplePos = fish->pos;
                ripplePos.y = WATER_SURFACE_Y(play);
                EnFishing_SpawnRipple(&fish->projectedPos, play->specialEffects, &ripplePos, 20.0f,
                                      Rand_ZeroFloat(50.0f) + 100.0f, 150, 90);

                if (fish->unk_28 < 1.5f) {
                    fish->unk_28 = 1.5f;
                }

                fish->unk_34 = 1.5f;
                fish->unk_38 = 1.0f;
            }

            target = EnFishing_SmoothStepToS(&fish->unk_3E, spD4, 5, 0x4000) * 3.0f;
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

            if (D_8090CF18 != 0) {
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

    D_8090CF18 = 0;
}

void EnFishing_DrawGroupFishes(PlayState* play) {
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
                Matrix_Translate(fish->pos.x, fish->pos.y, fish->pos.z, MTXMODE_NEW);
                Matrix_RotateYF(BINANG_TO_RAD_ALT2(fish->unk_3E), MTXMODE_APPLY);
                Matrix_RotateXFApply(BINANG_TO_RAD_ALT2(-(f32)fish->unk_3C));
                Matrix_Scale(fish->unk_2C * scale, scale, scale, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gFishingGroupFishModelDL);
            }
        }
        fish++;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

u16 D_8090D638[] = { 0x4096, 0x408D, 0x408E, 0x408F, 0x4094, 0x4095 };

void EnFishing_HandleOwnerDialog(EnFishing* this, PlayState* play) {
    switch (this->unk_154) {
        case 0:
            if (D_809171FC == 0) {
                if (sLinkAge != 1) {
                    if ((HIGH_SCORE(HS_FISHING) & 0x100) && !(HIGH_SCORE(HS_FISHING) & 0x200)) {
                        this->actor.textId = 0x4093;
                    } else {
                        this->actor.textId = 0x407B;
                    }
                } else {
                    this->actor.textId = 0x407B;
                }
            } else if (!D_8090CD0C) {
                this->actor.textId = 0x4084;
            } else {
                this->actor.textId = 0x4097;
            }

            if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
                if (D_809171FC == 0) {
                    this->unk_154 = 1;
                    if (sLinkAge != 1) {
                        HIGH_SCORE(HS_FISHING) |= 0x200;
                    } else {
                        HIGH_SCORE(HS_FISHING) |= 0x100;
                    }
                } else {
                    this->unk_154 = 10;
                }
            } else {
                Actor_OfferTalk(&this->actor, play, 100.0f);
            }
            break;

        case 1:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);

                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        if (gSaveContext.save.saveInfo.playerData.rupees >= 20) {
                            Rupees_ChangeBy(-20);
                            if (!Rumble_ControllerOneHasRumblePak()) {
                                this->actor.textId = 0x407C;
                            } else {
                                this->actor.textId = 0x407D;
                            }
                            Message_ContinueTextbox(play, this->actor.textId);
                            this->unk_154 = 2;
                        } else {
                            Message_ContinueTextbox(play, 0x407E);
                            this->unk_154 = 3;
                        }
                        break;

                    case 1:
                        Message_ContinueTextbox(play, 0x2D);
                        this->unk_154 = 3;
                        break;

                    default:
                        break;
                }
            }
            break;

        case 2:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                Message_ContinueTextbox(play, 0x407F);
                this->unk_154 = 4;
            }
            break;

        case 3:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                this->unk_154 = 0;
            }
            if (Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) {
                this->unk_154 = 0;
            }
            break;

        case 4:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);

                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        D_8090CCF8 = D_809171CC;
                        Message_ContinueTextbox(play, 0x4080);
                        this->unk_154 = 5;
                        break;

                    case 1:
                        Message_ContinueTextbox(play, 0x407F);
                        break;

                    default:
                        break;
                }
            }
            break;

        case 5:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);

                play->interfaceCtx.fishingHUDControl = 1;
                play->startPlayerFishing(play);
                D_809171FC = 1;
                D_8090CD04 = 20;
                this->unk_154 = 0;

                if ((HIGH_SCORE(HS_FISHING) & 0xFF0000) < 0xFF0000) {
                    HIGH_SCORE(HS_FISHING) += 0x10000;
                }
            }
            break;

        case 10:
            if (D_8090CD0C) {
                if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                    Message_CloseTextbox(play);

                    switch (play->msgCtx.choiceIndex) {
                        case 0:
                            Message_ContinueTextbox(play, 0x40B2);
                            D_8090CD08 = 1;
                            D_8090CD0C = false;
                            this->unk_154 = 20;
                            break;

                        case 1:
                            this->unk_154 = 0;
                            break;

                        default:
                            break;
                    }
                }
            } else {
                if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                    Message_CloseTextbox(play);

                    switch (play->msgCtx.choiceIndex) {
                        case 0:
                            if (D_8090CCF0 == 0.0f) {
                                this->actor.textId = 0x408C;
                                this->unk_154 = 20;
                            } else if (D_809171D0 == 0) {
                                D_8090CCF8 = D_8090CCF0;
                                if (TRUNCF_BINANG(D_809171CC) < TRUNCF_BINANG(D_8090CCF0)) {
                                    if (D_809171D2 == 2) {
                                        this->actor.textId = 0x40B0;
                                    } else {
                                        this->actor.textId = 0x4086;
                                    }
                                    this->unk_154 = 11;
                                } else {
                                    this->actor.textId = 0x408B;
                                    this->unk_154 = 20;
                                }
                            } else {
                                this->actor.textId = 0x409B;
                                this->unk_154 = 11;
                            }
                            Message_ContinueTextbox(play, this->actor.textId);
                            break;

                        case 1:
                            if (D_8090CD00 > 36000) {
                                D_8090CD00 = 30000;
                                Message_ContinueTextbox(play, 0x4088);
                            } else {
                                if (!D_809171CA) {
                                    if (D_809171D6 == 0) {
                                        D_809171D6++;
                                    }
                                }

                                if ((D_80917206 == 2) && (D_8090D638[D_809171D6] == 0x408D)) {
                                    Message_ContinueTextbox(play, 0x40AF);
                                } else {
                                    Message_ContinueTextbox(play, D_8090D638[D_809171D6]);
                                }

                                D_809171D6++;

                                if (sLinkAge != 1) {
                                    if (D_809171D6 >= 6) {
                                        D_809171D6 = 0;
                                    }
                                } else {
                                    if (D_809171D6 >= 4) {
                                        D_809171D6 = 0;
                                    }
                                }
                            }
                            this->unk_154 = 0;
                            break;

                        case 2:
                            if (D_809171D8 == 0) {
                                Message_ContinueTextbox(play, 0x4085);
                            } else if (sLinkAge == 1) {
                                Message_ContinueTextbox(play, 0x4092);
                            }
                            this->unk_154 = 22;
                            break;

                        default:
                            break;
                    }
                }
            }
            break;

        case 11:
            if (((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) ||
                 Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) &&
                Message_ShouldAdvance(play)) {
                s32 getItemId;

                Message_CloseTextbox(play);

                if (D_809171D0 == 0) {
                    D_809171CC = D_8090CCF0;
                    D_8090CCF0 = 0.0f;

                    if (sLinkAge == 1) {
                        f32 temp;

                        HIGH_SCORE(HS_FISHING) &= 0xFFFFFF00;
                        HIGH_SCORE(HS_FISHING) |= (TRUNCF_BINANG(D_809171CC) & 0x7F);
                        temp = (HIGH_SCORE(HS_FISHING) & 0x7F000000) >> 0x18;
                        if (temp < D_809171CC) {
                            HIGH_SCORE(HS_FISHING) &= 0xFFFFFF;
                            HIGH_SCORE(HS_FISHING) |= (TRUNCF_BINANG(D_809171CC) & 0x7F) << 0x18;

                            if (D_809171D2 == 2) {
                                HIGH_SCORE(HS_FISHING) |= 0x80000000;
                            }
                        }

                        if (D_809171D2 == 2) {
                            HIGH_SCORE(HS_FISHING) |= 0x80;
                            this->unk_154 = 0;
                            break;
                        }
                    } else {
                        HIGH_SCORE(HS_FISHING) &= 0xFFFFFF;
                        HIGH_SCORE(HS_FISHING) |= (TRUNCF_BINANG(D_809171CC) & 0x7F) << 0x18;

                        if (D_809171D2 == 2) {
                            HIGH_SCORE(HS_FISHING) |= 0x80000000;
                            this->unk_154 = 0;
                            break;
                        }
                    }

                    if (D_809171CC >= 60.0f) {
                        getItemId = GI_RUPEE_PURPLE;
                    } else if (D_809171CC >= 50.0f) {
                        getItemId = GI_RUPEE_10;
                    } else if (D_809171CC >= 40.0f) {
                        getItemId = GI_RUPEE_BLUE;
                    } else {
                        getItemId = GI_RUPEE_GREEN;
                    }

                    if (sLinkAge == 1) {
                        if ((D_809171CC >= 50.0f) && !(HIGH_SCORE(HS_FISHING) & 0x400)) {
                            HIGH_SCORE(HS_FISHING) |= 0x400;
                            getItemId = GI_HEART_PIECE;
                            sSinkingLureLocation = Rand_ZeroFloat(3.999f) + 1.0f;
                        }
                    } else {
                        if ((D_809171CC >= 60.0f) && !(HIGH_SCORE(HS_FISHING) & 0x800)) {
                            HIGH_SCORE(HS_FISHING) |= 0x800;
                            getItemId = GI_SKULL_TOKEN;
                            sSinkingLureLocation = Rand_ZeroFloat(3.999f) + 1.0f;
                        }
                    }
                } else {
                    getItemId = GI_RUPEE_PURPLE;
                    D_8090CCF0 = 0.0f;
                }

                this->actor.parent = NULL;
                Actor_OfferGetItem(&this->actor, play, getItemId, 2000.0f, 1000.0f);
                this->unk_154 = 23;
            }
            break;

        case 20:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                this->unk_154 = 0;
            }
            break;

        case 21:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);

                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        this->unk_154 = 0;
                        break;

                    case 1:
                        if (D_809171D8 == 0) {
                            Message_ContinueTextbox(play, 0x4085);
                        } else if (sLinkAge == 1) {
                            Message_ContinueTextbox(play, 0x4092);
                        }
                        this->unk_154 = 22;
                        break;

                    default:
                        break;
                }
            }
            break;

        case 22:
            if (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                this->unk_154 = 0;
                if (D_8090CD0C) {
                    D_8090CD08 = 1;
                    D_8090CD0C = false;
                }
                D_809171FC = 0;
                play->interfaceCtx.fishingHUDControl = 0;
            }
            break;

        case 23:
            D_8090CCF4 = false;
            if (Actor_HasParent(&this->actor, play)) {
                this->unk_154 = 24;
            } else {
                Actor_OfferGetItem(&this->actor, play, GI_SKULL_TOKEN, 2000.0f, 1000.0f);
            }
            break;

        case 24:
            D_8090CCF4 = false;
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
                if (D_809171D0 == 0) {
                    this->unk_154 = 0;
                } else {
                    Message_StartTextbox(play, 0x409C, NULL);
                    this->unk_154 = 20;
                }
            }
            break;

        default:
            break;
    }
}

s16 D_8090D644[] = { 0, 1, 2, 2, 1 };

Vec3f sStreamSoundPos = { 670.0f, 0.0f, -600.0f };

Vec3s sSinkingLureLocationPos[] = {
    { -364, -30, -269 },
    { 1129, 3, -855 },
    { -480, 0, -1055 },
    { 553, -48, -508 },
};

void EnFishing_UpdateOwner(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnFishing* this = (EnFishing*)thisx;
    Vec3f sp114;
    Vec3f sp108;
    Vec3f spFC;
    s16 headRotTarget;
    s16 playerShadowAlpha;
    f32 target;
    f32 camAtFraction;
    f32 lureDistXZ;
    Camera* mainCam;
    Player* player = GET_PLAYER(play);
    Input* input = CONTROLLER1(&play->state);

    playerShadowAlpha = player->actor.shape.shadowAlpha;

    if ((SQ(player->actor.world.pos.x) + SQ(player->actor.world.pos.z)) < SQ(920.0f)) {
        Math_ApproachS(&playerShadowAlpha, 0, 1, 40);
    } else {
        Math_ApproachS(&playerShadowAlpha, 200, 1, 40);
    }

    player->actor.shape.shadowAlpha = playerShadowAlpha;

    SkelAnime_Update(&this->skelAnime);

    if ((D_8090CD04 != 0) || Message_GetState(&play->msgCtx) != TEXT_STATE_NONE) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else {
        this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED);
    }

    if ((this->actor.xzDistToPlayer < 120.0f) || Message_GetState(&play->msgCtx) != TEXT_STATE_NONE) {
        headRotTarget = this->actor.shape.rot.y - this->actor.yawTowardsPlayer;
    } else {
        headRotTarget = 0;
    }

    if (headRotTarget > 0x2710) {
        headRotTarget = 0x2710;
    } else if (headRotTarget < -0x2710) {
        headRotTarget = -0x2710;
    }

    Math_ApproachS(&this->unk_15C, headRotTarget, 3, 0x1388);

    if (((play->gameplayFrames % 32) == 0) && (Rand_ZeroOne() < 0.3f)) {
        this->unk_15A = 4;
    }

    this->unk_158 = D_8090D644[this->unk_15A];

    if (this->unk_15A != 0) {
        this->unk_15A--;
    }

    if (D_8090CD04 != 0) {
        D_8090CD04--;
    }

    if (!D_8090CD0C && (D_80917206 != 2) && (D_8090CD14 > 0) && (D_8090CD08 == 1) && (D_8090CD04 == 0)) {
        f32 dx = sOwnerHeadPos.x - sLurePos.x;
        f32 dy = sOwnerHeadPos.y - sLurePos.y;
        f32 dz = sOwnerHeadPos.z - sLurePos.z;

        if ((sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 25.0f)) {
            D_8090CD08 = 0;
            D_8090CD0C = true;
            Message_StartTextbox(play, 0x4087, NULL);
        }
    }

    if (D_8090CD08 == 0) {
        HIGH_SCORE(HS_FISHING) |= 0x1000;
    } else if (D_8090CD08 == 1) {
        HIGH_SCORE(HS_FISHING) &= ~0x1000;
    }

    if (D_8090CCFC != 0) {
        D_8090CCFC--;
        if (D_8090CCFC == 0) {
            Message_StartTextbox(play, D_809171DA, NULL);
        }
    }

    EnFishing_HandleOwnerDialog(this, play);

    D_809101C8 = 0.0015f;
    D_8090CD00++;

    if ((D_809171FC != 0) && D_8090CCF4) {
        EnFishing_UpdateLure(this, play);
    }

    EnFishing_UpdateEffects(play->specialEffects, play);
    EnFishing_UpdatePondProps(play);
    EnFishing_UpdateGroupFishes(play);

    if ((D_809171FC != 0) && (D_8090CD4C == 0) && (player->actor.world.pos.z > 1360.0f) &&
        (fabsf(player->actor.world.pos.x) < 25.0f)) {
        player->actor.world.pos.z = 1360.0f;
        player->actor.speed = 0.0f;

        if (D_8090CD50 == 0) {
            D_8090CD4C = 10;
        }
    }

    if ((sSinkingLureLocation != 0) &&
        (fabsf(player->actor.world.pos.x - sSinkingLureLocationPos[sSinkingLureLocation - 1].x) < 25.0f) &&
        (fabsf(player->actor.world.pos.y - sSinkingLureLocationPos[sSinkingLureLocation - 1].y) < 10.0f) &&
        (fabsf(player->actor.world.pos.z - sSinkingLureLocationPos[sSinkingLureLocation - 1].z) < 25.0f)) {
        sSinkingLureLocation = 0;
        D_8090CD4C = 20;
        Rumble_Override(0.0f, 150, 10, 10);
        Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 20);
    }

    if (D_8090CD50 != 0) {
        D_8090CD50--;
    }

    switch (D_8090CD4C) {
        case 0:
            break;

        case 1:
            sSubCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, sSubCamId, CAM_STATUS_ACTIVE);
            mainCam = Play_GetCamera(play, CAM_ID_MAIN);
            sSubCamEye.x = mainCam->eye.x;
            sSubCamEye.y = mainCam->eye.y;
            sSubCamEye.z = mainCam->eye.z;
            sSubCamAt.x = mainCam->at.x;
            sSubCamAt.y = mainCam->at.y;
            sSubCamAt.z = mainCam->at.z;
            D_8090CD4C = 2;
            Interface_SetHudVisibility(HUD_VISIBILITY_A_B_MINIMAP);
            sSubCamVelFactor = 0.0f;
            // fallthrough
        case 2:
            ShrinkWindow_Letterbox_SetSizeTarget(27);

            spFC.x = sLurePos.x - player->actor.world.pos.x;
            spFC.z = sLurePos.z - player->actor.world.pos.z;
            lureDistXZ = sqrtf(SQXZ(spFC));
            Matrix_RotateYF(Math_Atan2F_XY(spFC.z, spFC.x), MTXMODE_NEW);

            sp114.x = 0.0f;
            sp114.y = 0.0f;
            sp114.z = 100.0f;
            Matrix_MultVec3f(&sp114, &spFC);

            if (D_8090CD14 == 1) {
                camAtFraction = 0.2f;
            } else {
                camAtFraction = 0.1f;
            }

            Math_ApproachF(&sSubCamAt.x, sLurePos.x, camAtFraction, fabsf(spFC.x) * sSubCamVelFactor);
            Math_ApproachF(&sSubCamAt.y, sLurePos.y, camAtFraction, 50.0f * sSubCamVelFactor);
            Math_ApproachF(&sSubCamAt.z, sLurePos.z, camAtFraction, fabsf(spFC.z) * sSubCamVelFactor);

            sp114.x = 0.0f - D_80911F50;
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

            Math_ApproachF(&D_80911F50, 30.0f, 0.1f, 0.4f);

            if (CHECK_BTN_ALL(input->press.button, BTN_Z)) {
                if ((D_809171DC >= 0) && (D_80917272 == 0)) {
                    D_809171DC++;

                    if (D_809171DC >= 4) {
                        D_809171DC = 0;
                    }

                    if ((D_809171DC == 0) || (D_809171DC == 3)) {
                        Audio_PlaySfx(NA_SE_SY_CAMERA_ZOOM_DOWN);
                    } else {
                        Audio_PlaySfx(NA_SE_SY_CAMERA_ZOOM_UP);
                    }
                }
            }

            if (D_8090CD14 >= 3) {
                if (lureDistXZ < 110.0f) {
                    D_809171DC = -1;
                } else if ((lureDistXZ > 300.0f) && (D_809171DC < 0)) {
                    D_809171DC = 0;
                }
            }

            if (D_809171DC > 0) {
                f32 dist;
                f32 offset;
                f32 factor;

                dist = sqrtf(SQ(spFC.x) + SQ(spFC.z)) * 0.001f;
                if (dist > 1.0f) {
                    dist = 1.0f;
                }
                if (D_809171DC == 2) {
                    offset = 0.3f;
                } else {
                    offset = 0.1f;
                }
                factor = 0.4f + offset + (dist * 0.4f);

                sp108.x += (sLurePos.x - sp108.x) * factor;
                sp108.y += ((sLurePos.y - sp108.y) * factor) + 20.0f;
                sp108.z += (sLurePos.z - sp108.z) * factor;
                D_809101C8 = 0.0005000001f;
            }

            sp114.x = 0.0f;
            sp114.y = 0.0f;
            sp114.z = 100.0f;
            Matrix_MultVec3f(&sp114, &spFC);

            Math_ApproachF(&sSubCamEye.x, sp108.x, 0.3f, fabsf(spFC.x) * sSubCamVelFactor);
            Math_ApproachF(&sSubCamEye.y, sp108.y, 0.3f, 20.0f * sSubCamVelFactor);
            Math_ApproachF(&sSubCamEye.z, sp108.z, 0.3f, fabsf(spFC.z) * sSubCamVelFactor);
            break;

        case 3: {
            Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);

            mainCam->eye = sSubCamEye;
            mainCam->eyeNext = sSubCamEye;
            mainCam->at = sSubCamAt;
        }
            func_80169AFC(play, sSubCamId, 0);
            Cutscene_StopManual(play, &play->csCtx);
            D_8090CD4C = 0;
            sSubCamId = SUB_CAM_ID_DONE;
            Environment_EnableUnderwaterLights(play, 0);
            play->envCtx.adjLightSettings.fogNear = 0;
            player->unk_B28 = -5;
            D_80917200 = 5;
            break;

        case 10:
            Cutscene_StartManual(play, &play->csCtx);
            sSubCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, sSubCamId, CAM_STATUS_ACTIVE);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_4);

            mainCam = Play_GetCamera(play, CAM_ID_MAIN);
            sSubCamEye.x = mainCam->eye.x;
            sSubCamEye.y = mainCam->eye.y;
            sSubCamEye.z = mainCam->eye.z;
            sSubCamAt.x = mainCam->at.x;
            sSubCamAt.y = mainCam->at.y;
            sSubCamAt.z = mainCam->at.z;

            Message_StartTextbox(play, 0x409E, NULL);
            D_8090CD4C = 11;
            Rumble_Override(0.0f, 150, 10, 10);
            // fallthrough
        case 11:
            player->actor.world.pos.z = 1360.0f;
            player->actor.speed = 0.0f;

            if (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);

                mainCam->eye = sSubCamEye;
                mainCam->eyeNext = sSubCamEye;
                mainCam->at = sSubCamAt;

                func_80169AFC(play, sSubCamId, 0);
                Cutscene_StopManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                D_8090CD4C = 0;
                sSubCamId = SUB_CAM_ID_DONE;
                D_8090CD50 = 30;
                Environment_EnableUnderwaterLights(play, 0);
                play->envCtx.adjLightSettings.fogNear = 0;
            }
            break;

        case 20:
            Cutscene_StartManual(play, &play->csCtx);
            sSubCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, sSubCamId, CAM_STATUS_ACTIVE);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_4);

            mainCam = Play_GetCamera(play, CAM_ID_MAIN);
            sSubCamEye.x = mainCam->eye.x;
            sSubCamEye.y = mainCam->eye.y;
            sSubCamEye.z = mainCam->eye.z;
            sSubCamAt.x = mainCam->at.x;
            sSubCamAt.y = mainCam->at.y;
            sSubCamAt.z = mainCam->at.z;
            Message_StartTextbox(play, 0x409A, NULL);
            D_8090CD4C = 21;
            D_80911F48 = 45.0f;
            D_8090CD50 = 10;
            // fallthrough
        case 21:
            if ((D_8090CD50 == 0) && Message_ShouldAdvance(play)) {
                D_8090CD4C = 22;
                D_8090CD50 = 40;
                // Player_SetCsActionWithHaltedActors call removed in MM
                D_80911F64 = 0.0f;
            }
            break;

        case 22:
            if (D_8090CD50 == 30) {
                // BENTODO This had | 0x900 which interfered with the 16 bit sequence IDs. Removing it doesn't seem to
                // do anything bad.
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_GET_ITEM | SEQ_FLAG_ASYNC);
            }

            D_8090CD54 = 1;

            Math_ApproachF(&D_80911F64, 71.0f, 0.5f, 3.0f);
            Matrix_RotateYF(BINANG_TO_RAD_ALT(player->actor.shape.rot.y), MTXMODE_NEW);

            sp114.x = Math_SinS(play->gameplayFrames * 0x1000);
            sp114.y = D_80911F64;
            sp114.z = -5.0f;
            if (sLinkAge == 1) {
                sp114.y -= 20.0f;
            }

            Matrix_MultVec3f(&sp114, &sp108);

            sSinkingLureBasePos.x = player->actor.world.pos.x + sp108.x;
            sSinkingLureBasePos.y = player->actor.world.pos.y + sp108.y;
            sSinkingLureBasePos.z = player->actor.world.pos.z + sp108.z;

            Math_ApproachF(&D_80911F48, 15.0f, 0.1f, 0.75f);

            sp114.x = D_80911F48 - 15.0f;

            if (sLinkAge != 1) {
                sp114.y = 60.0f;
                sp114.z = -30.0f;
            } else {
                sp114.y = 40.0f;
                sp114.z = -35.0f;
            }

            Matrix_MultVec3f(&sp114, &sSubCamEye);
            sSubCamEye.x += player->actor.world.pos.x;
            sSubCamEye.y += player->actor.world.pos.y;
            sSubCamEye.z += player->actor.world.pos.z;

            sSubCamAt = player->actor.world.pos;
            if (sLinkAge != 1) {
                sSubCamAt.y += 62.0f;
            } else {
                sSubCamAt.y += 40.0f;
            }

            if (D_8090CD50 == 0) {
                if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) ||
                    Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) {
                    if (Message_ShouldAdvance(play)) {
                        Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);

                        Message_CloseTextbox(play);
                        if (play->msgCtx.choiceIndex == 0) {
                            D_80917206 = 2;
                            D_809171D6 = 0;
                        }

                        mainCam->eye = sSubCamEye;
                        mainCam->eyeNext = sSubCamEye;
                        mainCam->at = sSubCamAt;
                        func_80169AFC(play, sSubCamId, 0);
                        Cutscene_StopManual(play, &play->csCtx);
                        Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                        D_8090CD4C = 0;
                        sSubCamId = SUB_CAM_ID_DONE;
                        player->unk_B28 = -5;
                        D_80917200 = 5;
                        D_8090CD54 = 0;
                        D_809171F6 = 20;
                        Environment_EnableUnderwaterLights(play, 0);
                        play->envCtx.adjLightSettings.fogNear = 0;
                    }
                }
            }
            break;

        case 100:
            break;
    }

    if (sSubCamId != SUB_CAM_ID_DONE) {
        Play_SetCameraAtEye(play, sSubCamId, &sSubCamAt, &sSubCamEye);
        Math_ApproachF(&sSubCamVelFactor, 1.0f, 1.0f, 0.02f);

        if (sSubCamEye.y <= (WATER_SURFACE_Y(play) + 1.0f)) {
            Environment_EnableUnderwaterLights(play, 1);
            if (D_809171CA) {
                play->envCtx.adjLightSettings.fogNear = -178;
            } else {
                play->envCtx.adjLightSettings.fogNear = -46;
            }
        } else {
            Environment_EnableUnderwaterLights(play, 0);
            play->envCtx.adjLightSettings.fogNear = 0;
        }
    }

    if ((player->actor.floorHeight < (WATER_SURFACE_Y(play) - 3.0f)) &&
        (player->actor.world.pos.y < (player->actor.floorHeight + 3.0f)) && (player->actor.speed > 1.0f) &&
        ((play->gameplayFrames % 2) == 0)) {
        Vec3f pos;

        pos.x = Rand_CenteredFloat(20.0f) + player->actor.world.pos.x;
        pos.z = Rand_CenteredFloat(20.0f) + player->actor.world.pos.z;
        pos.y = player->actor.floorHeight + 5.0f;
        EnFishing_SpawnWaterDust(NULL, play->specialEffects, &pos, 0.5f);
    }

    if ((player->actor.floorHeight < WATER_SURFACE_Y(play)) &&
        (player->actor.floorHeight > (WATER_SURFACE_Y(play) - 10.0f)) && (player->actor.speed >= 4.0f) &&
        ((play->gameplayFrames % 4) == 0)) {
        s16 i;

        for (i = 0; i < 10; i++) {
            Vec3f pos;
            Vec3f velocity;
            f32 speedXZ;
            f32 angle;

            speedXZ = Rand_ZeroFloat(1.5f) + 1.5f;
            angle = Rand_ZeroFloat(6.28f);

            velocity.x = sinf(angle) * speedXZ;
            velocity.z = cosf(angle) * speedXZ;
            velocity.y = Rand_ZeroFloat(3.0f) + 2.0f;

            pos = player->actor.world.pos;
            pos.x += 2.0f * velocity.x;
            pos.y = WATER_SURFACE_Y(play);
            pos.z += 2.0f * velocity.z;
            EnFishing_SpawnDustSplash(NULL, play->specialEffects, &pos, &velocity,
                                      Rand_ZeroFloat(0.01f) + 0.020000001f);
        }
    }

    if (D_809171CB >= 2) {
        D_809171CB--;
    }

    if ((D_809171CB == 1) && (Message_GetState(&play->msgCtx) == TEXT_STATE_NONE) && ((D_8090CD00 & 0xFFF) == 0xFFF)) {
        D_809171CB = 200;

        if (Rand_ZeroOne() < 0.5f) {
            D_8090CCD4 = Rand_ZeroFloat(10.0f) + 5.0f;
            play->envCtx.stormRequest = STORM_REQUEST_START;
        } else {
            D_8090CCD4 = 0;
            play->envCtx.stormRequest = STORM_REQUEST_STOP;
        }
    }

    Math_ApproachF(&D_8090CCD0, D_8090CCD4, 1.0f, 0.05f);

    if (D_8090CCD0 > 0.0f) {
        f32 target = (D_8090CCD0 * 0.03f) + 0.8f;

        if (target > 1.2f) {
            target = 1.2f;
        }
        Math_ApproachF(&D_8090CCE8, target, 1.0f, 0.01f);
    }

    target = (10.0f - D_8090CCD0) * 150.1f;
    if (target < 0.0f) {
        target = 0.0f;
    }
    Math_ApproachF(&D_8090CCDC.z, target, 1.0f, 5.0f);

    if (D_8090CCDC.z < 1500.0f) {
        Audio_PlaySfx_AtPosWithFreq(&D_8090CCDC, NA_SE_EV_RAIN - SFX_FLAG, D_8090CCE8);
    }

    if (D_8090CCD4 != 0) {
        Math_ApproachF(&D_8090CCD8, -200.0f, 1.0f, 2.0f);
    } else {
        Math_ApproachZeroF(&D_8090CCD8, 1.0f, 2.0f);
    }

    play->envCtx.adjLightSettings.light1Color[0] = play->envCtx.adjLightSettings.light1Color[1] =
        play->envCtx.adjLightSettings.light1Color[2] = D_8090CCD8;

    if ((u8)D_8090CCD0 > 0) {
        s32 pad;
        s16 i;
        Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);
        Vec3f pos;
        Vec3f rot;
        Vec3f projectedPos;
        s32 pad2;

        rot.x = 1.6707964f;
        rot.y = 1.0f;
        rot.z = (Camera_GetInputDirYaw(mainCam) * -(M_PIf / 0x8000)) + rot.y;

        for (i = 0; i < (u8)D_8090CCD0; i++) {
            pos.x = Rand_CenteredFloat(700.0f) + play->view.eye.x;
            pos.y = (Rand_ZeroFloat(100.0f) + 150.0f) - 170.0f;
            pos.z = Rand_CenteredFloat(700.0f) + play->view.eye.z;

            if (pos.z < 1160.0f) {
                SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &pos, &projectedPos, &sProjectedW);

                if (projectedPos.z < 0.0f) {
                    i--;
                } else {
                    EnFishing_SpawnRainDrop(play->specialEffects, &pos, &rot);
                }
            }
        }
    }

    SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &sStreamSoundPos, &sStreamSoundProjectedPos, &sProjectedW);

    Audio_PlaySfx_AtPos(&sStreamSoundProjectedPos, NA_SE_EV_WATER_WALL - SFX_FLAG);

    if (gSaveContext.options.language == LANGUAGE_JPN) { // Added in MM
        gSaveContext.minigameScore = D_8090CCF8;
    } else {
        gSaveContext.minigameScore = (SQ((f32)D_8090CCF8) * 0.0036f) + 0.5f;
    }
}

s32 EnFishing_OwnerOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnFishing* this = (EnFishing*)thisx;

    if (limbIndex == FISHING_OWNER_LIMB_HEAD) {
        rot->x -= this->unk_15C;
    }

    return false;
}

void EnFishing_OwnerPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    if (limbIndex == FISHING_OWNER_LIMB_HEAD) {
        OPEN_DISPS(play->state.gfxCtx);

        Matrix_MultVec3f(&sZeroVec, &sOwnerHeadPos);

        if (D_8090CD08 == 1) {
            gSPDisplayList(POLY_OPA_DISP++, gFishingOwnerHatDL);
        } else if (D_8090CD08 == 2) {
            gSPDisplayList(POLY_OPA_DISP++, gFishingOwnerHairDL);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

TexturePtr sFishingOwnerEyeTexs[] = {
    gFishingOwnerEyeOpenTex,
    gFishingOwnerEyeHalfTex,
    gFishingOwnerEyeClosedTex,
};

void EnFishing_DrawOwner(Actor* thisx, PlayState* play) {
    s32 pad;
    EnFishing* this = (EnFishing*)thisx;
    Input* input = CONTROLLER1(&play->state);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    if ((this->actor.projectedPos.z < 1500.0f) &&
        (fabsf(this->actor.projectedPos.x) < (100.0f + this->actor.projectedPos.z))) {
        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sFishingOwnerEyeTexs[this->unk_158]));

        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              EnFishing_OwnerOverrideLimbDraw, EnFishing_OwnerPostLimbDraw, &this->actor);
    }

    EnFishing_DrawPondProps(play);
    EnFishing_DrawEffects(play->specialEffects, play);
    EnFishing_DrawGroupFishes(play);
    EnFishing_DrawStreamSplash(play);

    if (D_809171F6 != 0) {
        D_809171F6--;

        if (D_809171F6 == 0) {
            if (sLinkAge != 1) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_CLEAR_EVENT | SEQ_FLAG_ASYNC);
            } else {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_MUSIC_BOX_HOUSE | SEQ_FLAG_ASYNC);
            }

            if (sLinkAge != 1) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_CLEAR_EVENT | SEQ_FLAG_ASYNC);
            } else {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_MUSIC_BOX_HOUSE | SEQ_FLAG_ASYNC);
            }
        }
    }

    if ((D_809171FC != 0) && D_8090CCF4) {
        EnFishing_DrawRod(play);
        EnFishing_UpdateLinePos(sReelLinePos);
        EnFishing_UpdateLine(play, &sRodTipPos, sReelLinePos, sReelLineRot, sReelLineUnk);
        EnFishing_DrawLureAndLine(play, sReelLinePos, sReelLineRot);

        D_8090CD44 = input->rel.stick_x;
        D_8090CD48 = input->rel.stick_y;
    }

    D_8090CCF4 = true;

    Matrix_Translate(130.0f, 40.0f, 1300.0f, MTXMODE_NEW);
    Matrix_Scale(0.08f, 0.12f, 0.14f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gFishingAquariumBottomDL);
    gSPDisplayList(POLY_XLU_DISP++, gFishingAquariumContainerDL);

    if ((D_809171FC != 0) && (D_80917206 == 2)) {
        EnFishing_DrawSinkingLure(play);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
