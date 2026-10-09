/*
 * File: z_boss_03.c
 * Overlay: ovl_Boss_03
 * Description: Gyorg
 */

/**
 * Some notes:
 *
 * Seaweed:
 * - Refers to the seaweed at the bottom of the stage where Player fights Gyorg
 * - The default Gyorg actor will spawn 5 other Gyorg instances using the parameter GYORG_PARAM_SEAWEED to spawn them
 * - Seaweed can bend when the main Gyorg instance (the actual boss) or Player are near
 *
 * This actor makes some heavy assumptions about the positions of the room where it is spawned
 *
 * Cutscenes:
 * - There are 3 cutscenes:
 *   - IntroCutscene: The cs which is played when Player falls into the main room. It also shows Gyorg's titlecard
 *   - SpawnSmallFishesCutscene: The short cs which is played when Gyorg is spawning the small fishes (EnTanron3). This
 * is triggered when Gyorg reaches half of its life
 *   - DeathCutscene: Played when Gyorg dies. Showing it splashing and becoming smaller each time until it disappears
 * - This actor mainly handles the 3 cutscenes it has manually (instead of relying on existing systems for it)
 *
 * Main behaviour:
 * - Gyorg has two types of behaviour depending on Player's state:
 *   - If Player is standing on the main platform, then Gyorg follows the PrepareCharge -> Charge -> JumpOverPlatform
 * branch
 *   - Otherwise, Gyorg follows the ChasePlayer -> CatchPlayer -> ChewPlayer branch
 * - The main actionFunc which decides which branch should be taken is func_809E34B8
 * - Most of the actions of those two branches are constantly checking for the WORK_TIMER_CURRENT_ACTION timer. If it
 * runs out, then the behaviour resets back to func_809E34B8
 * - Either branch behaviour can be interrupted at any time by a hit from Player
 *   - Being hit once makes Gyorg Stunned
 *   - When Gyorg is Stunned, it is vulnerable to be Damaged by Player
 *   - When Gyorg's health drops below half, it spawns the small fishes
 *
 * The collision logic is handled manually
 *
 * This file is more or less sectioned like this:
 * - SpawnEffect
 * - RNG
 * - Gyorg
 *   - Init and actionFuncs
 *   - Cutscenes
 *   - More actionFuncs and Update
 *   - Draw
 * - Effect Update/Draw
 * - Seaweed
 */

#include "prevent_bss_reordering.h"
#include "z_boss_03.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "overlays/actors/ovl_En_Water_Effect/z_en_water_effect.h"
#include "overlays/actors/ovl_Item_B_Heart/z_item_b_heart.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_water_effect/object_water_effect.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

#define WORK_TIMER_UNK0_A 0 // used in func_809E34B8
#define WORK_TIMER_CURRENT_ACTION 0
#define WORK_TIMER_UNK0_C 0 // used in DeathCutscene

#define WORK_TIMER_UNK1_A 1 // used in func_809E34B8
#define WORK_TIMER_UNK1_B 1 // used in PrepareCharge

#define WORK_TIMER_UNK2_A 2 // used in func_809E34B8
#define WORK_TIMER_STUNNED 2

// The Y position of the top of the platform
#define PLATFORM_HEIGHT 440.0f
#define WATER_HEIGHT 430.0f

void Boss03_Init(Actor* thisx, PlayState* play2);
void Boss03_Destroy(Actor* thisx, PlayState* play);
void Boss03_Update(Actor* thisx, PlayState* play2);
void Boss03_Draw(Actor* thisx, PlayState* play);
void Boss03_Reset(void);

void func_809E344C(Boss03* this, PlayState* play);
void func_809E34B8(Boss03* this, PlayState* play);
void Boss03_SetupChasePlayer(Boss03* this, PlayState* play);
void Boss03_ChasePlayer(Boss03* this, PlayState* play);
void Boss03_SetupCatchPlayer(Boss03* this, PlayState* play, u8 arg2);
void Boss03_CatchPlayer(Boss03* this, PlayState* play);
void Boss03_SetupChewPlayer(Boss03* this, PlayState* play);
void Boss03_ChewPlayer(Boss03* this, PlayState* play);
void Boss03_SetupPrepareCharge(Boss03* this, PlayState* play);
void Boss03_PrepareCharge(Boss03* this, PlayState* play);
void Boss03_SetupCharge(Boss03* this, PlayState* play);
void Boss03_Charge(Boss03* this, PlayState* play);
void Boss03_SetupJumpOverPlatform(Boss03* this, PlayState* play);
void Boss03_JumpOverPlatform(Boss03* this, PlayState* play);
void Boss03_SetupIntroCutscene(Boss03* this, PlayState* play);
void Boss03_IntroCutscene(Boss03* this, PlayState* play);
void Boss03_DeathCutscene(Boss03* this, PlayState* play);
void Boss03_SpawnSmallFishesCutscene(Boss03* this, PlayState* play);
void Boss03_Stunned(Boss03* this, PlayState* play);
void Boss03_Damaged(Boss03* this, PlayState* play);

void Boss03_UpdateEffects(PlayState* play);
void Boss03_DrawEffects(PlayState* play);

void Boss03_SeaweedUpdate(Actor* thisx, PlayState* play);
void Boss03_SeaweedDraw(Actor* thisx, PlayState* play);

u8 D_809E9840;
// Timer used to start playing the boss background music if the intro cutscene was skipped (because it was already
// watched)
u8 D_809E9841;
// Used to set the timer D_809E9841 in case the intro cutscene was skipped
u8 D_809E9842;

// Set to Gyorg's projectedPos. sGyorgProjectedPos?
Vec3f D_809E9848;

GyorgEffect sGyorgEffects[GYORG_EFFECT_COUNT];

// Used by the Seaweed variant to interact with the "real" Gyorg boss
Boss03* sGyorgBossInstance;

void Boss03_PlayUnderwaterSfx(Vec3f* projectedPos, u16 sfxId) {
    Audio_PlaySfx_Underwater(projectedPos, sfxId);
}

/* Start of SpawnEffect section */

void Boss03_SpawnEffectWetSpot(PlayState* play, Vec3f* pos) {
    s16 i;
    GyorgEffect* eff = play->specialEffects;

    for (i = 0; i < GYORG_EFFECT_COUNT; i++) {
        if ((eff->type == GYORG_EFFECT_NONE) || (eff->type == GYORG_EFFECT_BUBBLE)) {
            eff->type = GYORG_EFFECT_WET_SPOT;
            eff->pos = *pos;
            eff->unk_34.x = 0.1f;
            eff->unk_34.y = 0.4f;
            eff->velocity = gZeroVec3f;
            eff->accel = gZeroVec3f;
            eff->alpha = 150;
            eff->alphaDelta = Rand_ZeroFloat(4.0f) + 5.0f;
            return;
        }

        eff++;
    }
}

void Boss03_SpawnEffectDroplet(PlayState* play, Vec3f* pos) {
    s16 i;
    GyorgEffect* eff = play->specialEffects;

    for (i = 0; i < GYORG_EFFECT_COUNT; i++) {
        if ((eff->type == GYORG_EFFECT_NONE) || (eff->type == GYORG_EFFECT_BUBBLE)) {
            eff->type = GYORG_EFFECT_DROPLET;
            eff->pos = *pos;
            eff->velocity = gZeroVec3f;
            eff->accel = gZeroVec3f;
            eff->accel.y = -2.0f;
            eff->unk_34.x = 0.1f;
            eff->unk_34.y = 0.0f;
            eff->unk_34.z = Rand_ZeroFloat(2 * M_PIf);
            eff->unk_02 = Rand_ZeroFloat(100.0f);
            eff->velocity.x = Rand_CenteredFloat(25.0f);
            eff->velocity.z = Rand_CenteredFloat(25.0f);
            return;
        }

        eff++;
    }
}

void Boss03_SpawnEffectSplash(PlayState* play, Vec3f* pos, Vec3f* velocity) {
    Vec3f accel = { 0.0f, -1.0f, 0.0f };
    f32 temp_f2;
    GyorgEffect* eff = play->specialEffects;
    s16 i;

    for (i = 0; i < GYORG_EFFECT_COUNT; i++) {
        if ((eff->type == GYORG_EFFECT_NONE) || (eff->type == GYORG_EFFECT_BUBBLE)) {
            eff->type = GYORG_EFFECT_SPLASH;
            eff->pos = *pos;
            eff->velocity = *velocity;
            eff->accel = accel;
            temp_f2 = Rand_ZeroFloat(0.02f) + 0.02f;
            eff->unk_34.y = temp_f2;
            eff->unk_34.x = temp_f2;
            eff->unk_34.z = Rand_ZeroFloat(2 * M_PIf);
            eff->unk_02 = Rand_ZeroFloat(100.0f);
            return;
        }

        eff++;
    }
}

void Boss03_SpawnEffectBubble(PlayState* play, Vec3f* pos) {
    s16 i;
    GyorgEffect* eff = play->specialEffects;

    for (i = 0; i < GYORG_EFFECT_COUNT; i++) {
        if (eff->type == GYORG_EFFECT_NONE) {
            eff->type = GYORG_EFFECT_BUBBLE;
            eff->pos = *pos;
            eff->velocity = gZeroVec3f;
            eff->accel = gZeroVec3f;
            eff->accel.y = 0.2f;
            eff->unk_34.x = Rand_ZeroFloat(0.3f) + 0.2f;
            eff->unk_02 = 0;
            return;
        }

        eff++;
    }
}

/* End of SpawnEffect section */

void Boss03_UpdateSphereElement(s32 index, ColliderJntSph* collider, Vec3f* sphereCenter) {
    ColliderJntSphElement* jntSphElem;

    collider->elements[index].dim.worldSphere.center.x = sphereCenter->x;
    collider->elements[index].dim.worldSphere.center.y = sphereCenter->y;
    collider->elements[index].dim.worldSphere.center.z = sphereCenter->z;

    jntSphElem = &collider->elements[index];
    jntSphElem->dim.worldSphere.radius = jntSphElem->dim.scale * jntSphElem->dim.modelSphere.radius;
}

/* Start of RNG section */

s32 sGyorgRandSeed1;
s32 sGyorgRandSeed2;
s32 sGyorgRandSeed3;

void Boss03_InitRand(s32 seedInit1, s32 seedInit2, s32 seedInit3) {
    sGyorgRandSeed1 = seedInit1;
    sGyorgRandSeed2 = seedInit2;
    sGyorgRandSeed3 = seedInit3;
}

f32 Boss03_RandZeroOne(void) {
    // Wichmann-Hill algorithm
    f32 randFloat;

    sGyorgRandSeed1 = (sGyorgRandSeed1 * 171) % 30269;
    sGyorgRandSeed2 = (sGyorgRandSeed2 * 172) % 30307;
    sGyorgRandSeed3 = (sGyorgRandSeed3 * 170) % 30323;

    randFloat = (sGyorgRandSeed1 / 30269.0f) + (sGyorgRandSeed2 / 30307.0f) + (sGyorgRandSeed3 / 30323.0f);
    while (randFloat >= 1.0f) {
        randFloat -= 1.0f;
    }

    return fabsf(randFloat);
}

/* End of RNG section */

// ACTOR_BG_DBLUE_MOVEBG is never spawned naturally on Gyorg's fight
Actor* Boss03_FindActorDblueMovebg(PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_BG].first;

    while (actor != NULL) {
        if (actor->id == ACTOR_BG_DBLUE_MOVEBG) {
            return actor;
        }
        actor = actor->next;
    }

    return NULL;
}

/* Start of Gyorg's Init and actionFuncs section */

ActorProfile Boss_03_Profile = {
    /**/ ACTOR_BOSS_03,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_BOSS03,
    /**/ sizeof(Boss03),
    /**/ Boss03_Init,
    /**/ Boss03_Destroy,
    /**/ Boss03_Update,
    /**/ Boss03_Draw,
    /**/ Boss03_Reset,
};

// The limbs referenced here are not used. The spheres are positioned manually by Boss03_PostLimbDraw
static ColliderJntSphElementInit sHeadJntSphElementsInit[] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { GYORG_LIMB_NONE, { { 0, 0, 0 }, 70 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { GYORG_LIMB_ROOT, { { 0, 0, 0 }, 50 }, 100 },
    },
};

static ColliderJntSphInit sHeadJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sHeadJntSphElementsInit),
    sHeadJntSphElementsInit,
};

// The limbs referenced here are not used. The spheres are positioned manually by Boss03_PostLimbDraw
static ColliderJntSphElementInit sBodyJntSphElementsInit[] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { GYORG_LIMB_HEAD, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { GYORG_LIMB_LOWER_TRUNK, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { GYORG_LIMB_TAIL, { { 0, 0, 0 }, 70 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { GYORG_LIMB_RIGHT_FIN_ROOT, { { 0, 0, 0 }, 70 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { GYORG_LIMB_UPPER_RIGHT_FIN, { { 0, 0, 0 }, 30 }, 100 },
    },
};

static ColliderJntSphInit sBodyJntSphInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sBodyJntSphElementsInit),
    sBodyJntSphElementsInit,
};

Vec3f sGyorgInitialPos = { 1216.0f, 140.0f, -1161.0f };

// unused
Vec3f D_809E8FF4[] = {
    { 261.0f, 140.0f, -1000.0f }, { -504.0f, 120.0f, -1276.0f }, { -1157.0f, 80.0f, -836.0f },
    { -1020.0f, 120.0f, 381.0f }, { -34.0f, 120.0f, 1213.0f },   { 803.0f, 80.0f, 1250.0f },
    { 1143.0f, 120.0f, 605.0f },  { 1228.0f, 140.0f, -244.0f },  { 1216.0f, 140.0f, -1161.0f },
    { 261.0f, 140.0f, -1000.0f }, { -504.0f, 120.0f, -890.0f },  { -1157.0f, 80.0f, -836.0f },
    { -1020.0f, 120.0f, 381.0f }, { -34.0f, 120.0f, 1213.0f },   { 803.0f, 80.0f, 1250.0f },
    { 1143.0f, 120.0f, 605.0f },  { 1228.0f, 140.0f, -244.0f },  { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },         { 0.0f, 0.0f, 0.0f },          { 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },
};

Color_RGBA8 sGyorgDustPrimColor = { 60, 50, 20, 255 };
Color_RGBA8 sGyorgDustEnvColor = { 40, 30, 30, 255 };

/**
 * Used when chasing Player and Gyorg is near the bottom
 */
void Boss03_SpawnDust(Boss03* this, PlayState* play) {
    if (this->insideJawPos.y < 80.0f) {
        u8 i;
        Vec3f pos;
        Vec3f velocity;
        Vec3f accel;

        for (i = 0; i < 5; i++) {
            velocity.x = Rand_CenteredFloat(10.0f);
            velocity.y = Rand_ZeroFloat(2.0f) + 2.0f;
            velocity.z = Rand_CenteredFloat(10.0f);

            accel.y = -0.075f;
            accel.z = 0.0f;
            accel.x = 0.0f;

            pos.y = Rand_ZeroFloat(20.0f) + 5.0f;
            pos.z = Rand_CenteredFloat(150.0f) + this->insideJawPos.z;
            pos.x = Rand_CenteredFloat(150.0f) + this->insideJawPos.x;

            func_800B0EB0(play, &pos, &velocity, &accel, &sGyorgDustPrimColor, &sGyorgDustEnvColor,
                          Rand_ZeroFloat(200.0f) + 400.0f, 10, Rand_ZeroFloat(10.0f) + 25.0f);
        }
    }
}

void Boss03_Init(Actor* thisx, PlayState* play2) {
    Boss03* this = (Boss03*)thisx;
    s32 i;
    PlayState* play = play2;
    Vec3f sp70;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, 0.0f, PLATFORM_HEIGHT, 200.0f, 0, 0,
                           0, ENDOORWARP1_FF_1);
        Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, 0.0f, PLATFORM_HEIGHT, 0.0f, 0, 0, 0,
                    BHEART_PARAM_NORMAL);
        Actor_Kill(&this->actor);
        return;
    }

    if (this->actor.params == GYORG_PARAM_SEAWEED) {
        this->actor.update = Boss03_SeaweedUpdate;
        this->actor.draw = Boss03_SeaweedDraw;

        this->unk_240 = Rand_ZeroFloat(1000.0f);

        this->actor.scale.x = Rand_ZeroFloat(0.0075f) + 0.027f;
        this->actor.scale.y = 0.02f;
        this->actor.scale.z = 0.015f;

        // Manually set the joint table of seaweed
        for (i = 0; i < 6; i++) {
            this->jointTable[i].x = Math_SinS(this->unk_240 * 0x100 + i * 15000) * 3000.0f;
            this->jointTable[i].y = Math_SinS(this->unk_240 * 0x180 + i * 20000) * 2000.0f;
            this->jointTable[i].z = Math_SinS(this->unk_240 * 0x10 + i * 19000) * 4000.0f;
        }

        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        return;
    }

    this->actor.world.pos = sGyorgInitialPos;

    // Since Boss03_RandZeroOne is only used on this Init function, the resulting values end up being deterministic
    Boss03_InitRand(1, 29093, 9786);

    for (i = 0; i < 5; i++) {
        f32 rand;

        rand = Boss03_RandZeroOne();
        Matrix_RotateYF((rand * M_PIf * 0.2f) + ((2.0f * M_PIf / 5.0f) * i), MTXMODE_NEW);

        rand = Boss03_RandZeroOne();
        Matrix_MultVecZ((rand * 800.0f) + 400.0f, &sp70);

        rand = Boss03_RandZeroOne();
        Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_03, sp70.x, sp70.y, sp70.z, 0, rand * 0x10000, 0,
                    GYORG_PARAM_SEAWEED);
    }

    sGyorgBossInstance = this;

    play->specialEffects = sGyorgEffects;

    for (i = 0; i < ARRAY_COUNT(sGyorgEffects); i++) {
        sGyorgEffects[i].type = GYORG_EFFECT_NONE;
    }

    this->actor.attentionRangeType = ATTENTION_RANGE_5;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.health = 10;

    Collider_InitAndSetJntSph(play, &this->headCollider, &this->actor, &sHeadJntSphInit, this->headColliderElements);
    Collider_InitAndSetJntSph(play, &this->bodyCollider, &this->actor, &sBodyJntSphInit, this->bodyColliderElements);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGyorgSkel, &gGyorgCrawlingAnim, this->jointTable, this->morphTable,
                       GYORG_LIMB_MAX);
    Actor_SetScale(&this->actor, 0.2f);

    if ((KREG(64) != 0) || CHECK_EVENTINF(EVENTINF_INTRO_CS_WATCHED_GYORG)) {
        this->actionFunc = func_809E344C;
        D_809E9842 = false;
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
    } else {
        Boss03_SetupIntroCutscene(this, play);
        D_809E9842 = true;
    }

    this->numSpawnedSmallFish = -1;
    this->waterHeight = WATER_HEIGHT;
}

void Boss03_Destroy(Actor* thisx, PlayState* play) {
    Boss03* this = (Boss03*)thisx;
}

/* Start of ActionFuncs section */

void func_809E344C(Boss03* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    // Mid-air
    if (player->actor.world.pos.y < 540.0f) {
        this->actionFunc = func_809E34B8;
        Animation_MorphToLoop(&this->skelAnime, &gGyorgFastSwimmingAnim, -15.0f);
        this->unk_274 = 0;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

/**
 * Swims randomly until WORK_TIMER_UNK1_A runs out
 */
void func_809E34B8(Boss03* this, PlayState* play) {
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 bodyYRotTarget;
    s32 pad;
    s16 i;
    Player* player = GET_PLAYER(play);

    Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_WAIT_OLD - SFX_FLAG);

    this->unk_276 = 0x800;
    this->skelAnime.playSpeed = 1.0f;
    this->unk_27C = 1.0f;
    this->unk_278 = 10.0f;

    SkelAnime_Update(&this->skelAnime);

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_APPLY);

    xDiff = this->unk_268.x - this->actor.world.pos.x;
    yDiff = this->unk_268.y - this->actor.world.pos.y;
    zDiff = this->unk_268.z - this->actor.world.pos.z;

    Math_ApproachS(&this->actor.world.rot.x, Math_Atan2S_XY(sqrtf(SQ(xDiff) + SQ(zDiff)), -yDiff), 0xA, this->unk_274);

    bodyYRotTarget =
        Math_SmoothStepToS(&this->actor.world.rot.y, Math_Atan2S_XY(zDiff, xDiff), 0xA, this->unk_274, 0) * -0.5f;
    Math_ApproachS(&this->bodyYRot, bodyYRotTarget, 5, 0x100);

    Math_ApproachS(&this->unk_274, this->unk_276, 1, 0x100);
    Math_ApproachF(&this->actor.speed, this->unk_278, 1.0f, this->unk_27C);
    Math_ApproachF(&this->unk_260, sinf(this->skelAnime.curFrame * (M_PIf / 5.0f)) * 10.0f * 0.01f, 0.5f, 1.0f);

    if ((this->workTimer[WORK_TIMER_UNK2_A] == 0) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
        Matrix_MultVecZ(-500.0f, &this->unk_268);
        this->unk_268.y = Rand_ZeroFloat(100.0f) + 150.0f;
        this->workTimer[WORK_TIMER_UNK2_A] = 60;
        this->workTimer[WORK_TIMER_UNK0_A] = Rand_ZeroFloat(60.0f) + 60.0f;
        this->unk_274 = 0x100;
    }

    if (this->workTimer[WORK_TIMER_UNK2_A] == 0) {
        if ((sqrtf(SQ(xDiff) + SQ(zDiff)) < 100.0f) || (this->workTimer[WORK_TIMER_UNK0_A] == 0)) {
            for (i = 0; i < 200; i++) {
                this->unk_268.x = Rand_CenteredFloat(2500.0f);
                this->unk_268.y = Rand_ZeroFloat(100.0f) + 150.0f;
                this->unk_268.z = Rand_CenteredFloat(2500.0f);

                xDiff = this->unk_268.x - this->actor.world.pos.x;
                zDiff = this->unk_268.z - this->actor.world.pos.z;

                if (sqrtf(SQ(xDiff) + SQ(zDiff)) > 300.0f) {
                    break;
                }
            }

            this->unk_274 = 0x100;
            this->workTimer[WORK_TIMER_UNK0_A] = Rand_ZeroFloat(60.0f) + 60.0f;
        }
    }

    Actor_MoveWithoutGravityReverse(&this->actor);
    Math_ApproachS(&this->actor.shape.rot.x, this->actor.world.rot.x, 2, this->unk_274 * 2);
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, this->unk_274 * 2);

    if (this->workTimer[WORK_TIMER_UNK1_A] == 0) {
        // Player is above water && Player is standing on ground
        if ((this->waterHeight < player->actor.world.pos.y) && (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            Boss03_SetupPrepareCharge(this, play);
        } else if ((player->transformation != PLAYER_FORM_GORON) && (player->transformation != PLAYER_FORM_DEKU)) {
            if (KREG(70) == 0) {
                Boss03_SetupChasePlayer(this, play);
            } else if (this->numSpawnedSmallFish <= 0) {
                Boss03_SetupChasePlayer(this, play);
            }
        }
    }
}

void Boss03_SetupChasePlayer(Boss03* this, PlayState* play) {
    this->actionFunc = Boss03_ChasePlayer;
    Animation_MorphToLoop(&this->skelAnime, &gGyorgFastSwimmingAnim, -10.0f);
    this->workTimer[WORK_TIMER_CURRENT_ACTION] = 100;
    this->unk_276 = 0x1000;
    this->skelAnime.playSpeed = 1.5f;
    this->unk_278 = 10.0f;
    this->unk_27C = 1.0f;
}

/**
 * Approach Player until near enough, then try to catch Player, unless it has since got back on the platform
 */
void Boss03_ChasePlayer(Boss03* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 bodyYRotTarget;
    Vec3f sp50;
    s32 pad;
    f32 phi_f2;
    f32 sp44;
    u8 sp43;

    this->unk_2BD = true;

    SkelAnime_Update(&this->skelAnime);

    xDiff = player->actor.world.pos.x - this->actor.world.pos.x;
    yDiff = (player->actor.world.pos.y - this->actor.world.pos.y) + 50.0f;
    zDiff = player->actor.world.pos.z - this->actor.world.pos.z;

    Math_ApproachS(&this->actor.world.rot.x, Math_Atan2S_XY(sqrtf(SQ(xDiff) + SQ(zDiff)), -yDiff), 0xA, this->unk_274);
    bodyYRotTarget =
        Math_SmoothStepToS(&this->actor.world.rot.y, Math_Atan2S_XY(zDiff, xDiff), 0xA, this->unk_274, 0) * -0.5f;
    Math_ApproachS(&this->bodyYRot, bodyYRotTarget, 5, 0x100);

    Math_ApproachS(&this->unk_274, this->unk_276, 1, 0x100);
    Math_ApproachF(&this->actor.speed, this->unk_278, 1.0f, this->unk_27C);
    Math_ApproachF(&this->unk_260, sinf(this->skelAnime.curFrame * (M_PIf / 5.0f)) * 10.0f * 0.01f, 0.5f, 1.0f);
    Actor_MoveWithoutGravityReverse(&this->actor);

    Math_ApproachS(&this->actor.shape.rot.x, this->actor.world.rot.x, 2, this->unk_274 * 2);
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, this->unk_274 * 2);

    // If either (Player is standing on ground && Player is above water) or (WORK_TIMER_CURRENT_ACTION timer runs out),
    // then stop chasing
    if (((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
         (player->actor.shape.feetPos[0].y >= WATER_HEIGHT + 8.0f)) ||
        (this->workTimer[WORK_TIMER_CURRENT_ACTION] == 0)) {
        if (&this->actor == player->actor.parent) {
            player->av2.actionVar2 = 101;
            player->actor.parent = NULL;
            player->csAction = PLAYER_CSACTION_NONE;
        }

        func_809E344C(this, play);
        this->workTimer[WORK_TIMER_UNK1_A] = 100;
    } else {
        if ((this->waterHeight - 80.0f) < player->actor.world.pos.y) {
            sp43 = 1;
            phi_f2 = 100.0f;
            sp44 = 50.0f;
        } else {
            sp43 = 0;
            phi_f2 = 200.0f;
            sp44 = 100.0f;
        }

        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_APPLY);
        Matrix_MultVecZ(sp44, &sp50);

        xDiff = sp50.x - player->actor.world.pos.x;
        zDiff = sp50.z - player->actor.world.pos.z;

        if (sqrtf(SQ(xDiff) + SQ(zDiff)) < (2.0f * phi_f2)) {
            Math_ApproachS(&this->jawZRot, 0x3200, 2, 0x1800);
            this->unk_278 = 25.0f;
            this->unk_27C = 5.0f;
            this->skelAnime.playSpeed = 2.5f;
        }

        // Near enough to Player?
        if (sqrtf(SQ(xDiff) + SQ(zDiff)) < phi_f2) {
            Boss03_SetupCatchPlayer(this, play, sp43);

            if (sp43 != 0) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_WATER_EFFECT, player->actor.world.pos.x, this->waterHeight,
                            player->actor.world.pos.z, 0, 0, 0x78, ENWATEREFFECT_TYPE_GYORG_RIPPLES);
                Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_SINK_OLD);
            }

            Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_UTSUBO_EAT);
        }
    }

    Boss03_SpawnDust(this, play);
}

void Boss03_SetupCatchPlayer(Boss03* this, PlayState* play, u8 arg2) {
    this->actionFunc = Boss03_CatchPlayer;
    Animation_MorphToLoop(&this->skelAnime, &gGyorgFastSwimmingAnim, -15.0f);
    this->workTimer[WORK_TIMER_CURRENT_ACTION] = 100;
    this->unk_2C4 = 0.0f;
    this->unk_2B8 = 0.0f;
    this->unk_242 = arg2;
}

void Boss03_CatchPlayer(Boss03* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    s32 pad;

    this->skelAnime.playSpeed = 2.0f;
    this->unk_27C = 2.0f;
    this->unk_276 = 0x1000;
    this->unk_2BD = true;
    this->unk_278 = 15.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    SkelAnime_Update(&this->skelAnime);

    xDiff = player->actor.world.pos.x - this->actor.world.pos.x;
    yDiff = player->actor.world.pos.y - this->actor.world.pos.y;
    zDiff = player->actor.world.pos.z - this->actor.world.pos.z;

    Math_ApproachS(&this->actor.world.rot.x, Math_Atan2S_XY(sqrtf(SQ(xDiff) + SQ(zDiff)), -yDiff), 0xA, this->unk_274);
    Math_ApproachS(&this->bodyYRot,
                   Math_SmoothStepToS(&this->actor.world.rot.y, Math_Atan2S_XY(zDiff, xDiff), 0xA, this->unk_274, 0) *
                       -0.5f,
                   5, 0x100);
    Math_ApproachS(&this->unk_274, this->unk_276, 1, 0x100);
    Math_ApproachF(&this->actor.speed, this->unk_278, 1.0f, this->unk_27C);
    Math_ApproachF(&this->unk_260, sinf(this->skelAnime.curFrame * (M_PIf / 5.0f)) * 10.0f * 0.01f, 0.5f, 1.0f);
    Actor_MoveWithoutGravityReverse(&this->actor);
    Math_ApproachS(&this->actor.shape.rot.x, this->actor.world.rot.x, 2, this->unk_274 * 2);
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, this->unk_274 * 2);

    // If either (Player is standing on ground && Player is above water) or (WORK_TIMER_CURRENT_ACTION timer runs out)
    // then stop trying to catch Player
    if (((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
         (player->actor.shape.feetPos[FOOT_LEFT].y >= WATER_HEIGHT + 8.0f)) ||
        (this->workTimer[WORK_TIMER_CURRENT_ACTION] == 0)) {
        if (&this->actor == player->actor.parent) {
            player->av2.actionVar2 = 101;
            player->actor.parent = NULL;
            player->csAction = PLAYER_CSACTION_NONE;
            Play_DisableMotionBlur();
        }

        func_809E344C(this, play);
        // WORK_TIMER_UNK1_A wasn't set
    } else {
        f32 phi_f0;

        Math_ApproachF(&player->actor.world.pos.x, this->insideJawPos.x, 1.0f, this->unk_2B8);
        Math_ApproachF(&player->actor.world.pos.y, this->insideJawPos.y, 1.0f, this->unk_2B8);
        Math_ApproachF(&player->actor.world.pos.z, this->insideJawPos.z, 1.0f, this->unk_2B8);

        if (this->unk_242 != 0) {
            phi_f0 = 10.0f;
        } else {
            phi_f0 = 2.0f;
        }

        Math_ApproachF(&this->unk_2B8, 100.0f, 1.0f, phi_f0);

        if (this->unk_2B8 > 30.0f) {
            if ((&this->actor != player->actor.parent) && (play->grabPlayer(play, player) != 0)) {
                player->actor.parent = &this->actor;
                AudioSfx_PlaySfx(NA_SE_VO_LI_DAMAGE_S, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                                 &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                Boss03_SetupChewPlayer(this, play);
            }
        } else {
            Math_ApproachS(&this->jawZRot, 0x3200, 2, 0xC00 * phi_f0);
        }

        Math_ApproachS(&player->actor.world.rot.x, 0x4000, 1, 0x400);
        Math_ApproachS(&player->actor.shape.rot.x, 0x4000, 1, 0x400);
        Math_ApproachS(&player->actor.world.rot.y, this->unk_2A2.y, 1, 0x400);
        Math_ApproachS(&player->actor.shape.rot.y, this->unk_2A2.y, 1, 0x400);
    }

    Boss03_SpawnDust(this, play);
}

void Boss03_SetupChewPlayer(Boss03* this, PlayState* play) {
    s16 pitchAngle;
    Vec3f out;

    this->actionFunc = Boss03_ChewPlayer;

    pitchAngle = Math_Atan2S_XY(this->actor.world.pos.z, this->actor.world.pos.x);
    Matrix_RotateYS(pitchAngle, MTXMODE_NEW);

    out.x = 0.0f;
    out.y = 200.0f;
    out.z = 700.0f;
    Matrix_MultVec3f(&out, &this->unk_268);

    this->unk_276 = 0x800;
    this->unk_242 = 0;
    this->workTimer[WORK_TIMER_CURRENT_ACTION] = 100;
    this->skelAnime.playSpeed = 1.0f;
}

void Boss03_ChewPlayer(Boss03* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Input* input = CONTROLLER1(&play->state);
    f32 jawZRotTarget;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;

    this->unk_2BD = true;
    this->unk_25C = 15;

    if (this->workTimer[WORK_TIMER_CURRENT_ACTION] == 90) {
        Play_EnableMotionBlur(150);
    }

    SkelAnime_Update(&this->skelAnime);

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_APPLY);

    xDiff = this->unk_268.x - this->actor.world.pos.x;
    yDiff = this->unk_268.y - this->actor.world.pos.y;
    zDiff = this->unk_268.z - this->actor.world.pos.z;

    Math_ApproachS(&this->actor.world.rot.x, Math_Atan2S_XY(sqrtf(SQ(xDiff) + SQ(zDiff)), -yDiff), 0xA, this->unk_274);
    Math_ApproachS(&this->bodyYRot,
                   Math_SmoothStepToS(&this->actor.world.rot.y, Math_Atan2S_XY(zDiff, xDiff), 0xA, this->unk_274, 0) *
                       -0.5f,
                   5, 0x100);
    Math_ApproachS(&this->unk_274, this->unk_276, 1, 0x100);
    Math_ApproachF(&this->unk_260, sinf(this->skelAnime.curFrame * (M_PIf / 5.0f)) * 10.0f * 0.01f, 0.5f, 1.0f);

    switch (this->unk_242) {
        case 0:
            Math_ApproachF(&this->actor.speed, 10.0f, 1.0f, 1.0f);
            if (sqrtf(SQ(xDiff) + SQ(zDiff)) < 100.0f) {
                this->unk_242 = 1;
                Animation_MorphToLoop(&this->skelAnime, &gGyorgBackingUpAnim, -15.0f);
            }
            break;

        case 1:
            Math_ApproachF(&this->actor.speed, 0.0f, 1.0f, 0.5f);
            Math_ApproachF(&this->actor.world.pos.y, 200.0f, 0.05f, 5.0f);
            break;
    }

    Actor_MoveWithoutGravityReverse(&this->actor);

    Math_ApproachS(&this->actor.shape.rot.x, this->actor.world.rot.x, 2, this->unk_274 * 2);
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, this->unk_274 * 2);

    // Mashing A or B reduces the time Gyorg is chewing Player
    if (CHECK_BTN_ALL(input->press.button, BTN_A) || CHECK_BTN_ALL(input->press.button, BTN_B)) {
        if (this->workTimer[WORK_TIMER_CURRENT_ACTION] != 0) {
            this->workTimer[WORK_TIMER_CURRENT_ACTION]--;
        }
        if (this->workTimer[WORK_TIMER_CURRENT_ACTION] != 0) {
            this->workTimer[WORK_TIMER_CURRENT_ACTION]--;
        }
    }

    // Stop chewing when the timer runs out
    if (this->workTimer[WORK_TIMER_CURRENT_ACTION] == 0) {
        if (&this->actor == player->actor.parent) {
            player->av2.actionVar2 = 101;
            player->actor.parent = NULL;
            player->csAction = PLAYER_CSACTION_NONE;
            Play_DisableMotionBlur();
            func_800B8D50(play, NULL, 10.0f, this->actor.shape.rot.y, 0.0f, 0x20);
        }

        func_809E344C(this, play);
        this->workTimer[WORK_TIMER_UNK1_A] = Rand_ZeroFloat(100.0f) + 200.0f;

        return;
    }

    player->actor.world.pos = this->insideJawPos;

    jawZRotTarget = Math_SinS(this->unk_240 * 0x2000);
    Math_ApproachS(&this->jawZRot, jawZRotTarget * 2000.0f, 2, 0x3000);

    player->actor.shape.rot.x = 0x4000;
    player->actor.world.rot.x = player->actor.shape.rot.x;

    player->actor.world.rot.y = player->actor.shape.rot.y = this->unk_2A2.y;

    if (this->workTimer[WORK_TIMER_CURRENT_ACTION] < 5) {
        Math_ApproachS(&this->jawZRot, 0x3200, 2, 0x1800);
        Math_ApproachF(&this->unk_2C4, 100.0f, 1.0f, 100.0f);
    } else {
        Math_ApproachF(&this->unk_2C4, -300.0f, 1.0f, 5.0f);
        if ((this->unk_240 % 8) == 0) {
            Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_BITE_OLD);
        }
    }
}

void Boss03_SetupPrepareCharge(Boss03* this, PlayState* play) {
    this->actionFunc = Boss03_PrepareCharge;
    Animation_MorphToLoop(&this->skelAnime, &gGyorgBackingUpAnim, -15.0f);
    this->workTimer[WORK_TIMER_CURRENT_ACTION] = Rand_ZeroFloat(30.0f) + 80.0f;
    this->workTimer[WORK_TIMER_UNK1_B] = 50;
    this->unk_274 = 0;

    if (sqrtf(SQXZ(this->actor.world.pos)) > 600.0f) {
        if (Rand_ZeroOne() < 0.5f) {
            this->unk_242 = 1;
        } else {
            this->unk_242 = 0;
        }
    } else {
        this->unk_242 = 0;
    }
}

/**
 * Slowly turns back while looking at Player during WORK_TIMER_CURRENT_ACTION frames, then prepares to charge
 */
void Boss03_PrepareCharge(Boss03* this, PlayState* play) {
    f32 posYTarget;
    Player* player = GET_PLAYER(play);

    if (this->workTimer[WORK_TIMER_UNK1_B] != 0) {
        Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_WAIT_OLD - SFX_FLAG);
    }

    // Rotate towards Player
    Math_ApproachS(&this->bodyYRot,
                   Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0xA, this->unk_274, 0) *
                       -0.7f,
                   5, 0x200);
    Math_ApproachS(&this->unk_274, 0x800, 1, 0x100);
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 2, 0x1000);
    Math_ApproachS(&this->actor.world.rot.x, 0, 0xA, 0x200);
    Math_ApproachS(&this->actor.shape.rot.x, 0, 0xA, 0x200);

    posYTarget = this->waterHeight - 70.0f;
    if (posYTarget < this->actor.world.pos.y) {
        Math_ApproachF(&this->actor.world.pos.y, posYTarget, 0.05f, 5.0f);
    }

    SkelAnime_Update(&this->skelAnime);

    // Player is above water && Player is standing on ground
    if ((this->waterHeight < player->actor.world.pos.y) && (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (this->workTimer[WORK_TIMER_CURRENT_ACTION] == 0) {
            Boss03_SetupCharge(this, play);
        }
    } else if (player->actor.world.pos.y <= this->waterHeight) {
        func_809E344C(this, play);
        this->workTimer[WORK_TIMER_UNK1_A] = 20;
    }

    // Turns back slowly
    Math_ApproachF(&this->actor.speed, -3.0f, 1.0f, 0.5f);
    Actor_MoveWithoutGravityReverse(&this->actor);
}

void Boss03_SetupCharge(Boss03* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actionFunc = Boss03_Charge;
    Animation_MorphToLoop(&this->skelAnime, &gGyorgFastSwimmingAnim, -15.0f);
    this->unk_268 = player->actor.world.pos;
}

/**
 * Charge against the platform, either by clashing against the platform or by preparing to jump over it
 */
void Boss03_Charge(Boss03* this, PlayState* play) {
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    Player* player = GET_PLAYER(play);
    s16 rotXTarget;

    this->skelAnime.playSpeed = 2.0f;
    SkelAnime_Update(&this->skelAnime);
    Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_PREATTACK_OLD - SFX_FLAG);

    xDiff = this->unk_268.x - this->actor.world.pos.x;
    yDiff = (this->unk_268.y - this->actor.world.pos.y) - 50.0f;
    zDiff = this->unk_268.z - this->actor.world.pos.z;

    Math_ApproachS(&this->actor.world.rot.y, Math_Atan2S_XY(zDiff, xDiff), 0xA, 0x1000);

    rotXTarget = Math_Atan2S_XY(sqrtf(SQ(xDiff) + SQ(zDiff)), -yDiff);
    Math_ApproachS(&this->actor.world.rot.x, rotXTarget, 0xA, 0x1000);

    this->actor.shape.rot = this->actor.world.rot;

    Math_ApproachF(&this->actor.speed, 25.0f, 1.0f, 3.0f);
    Math_ApproachF(&this->unk_260, sinf(this->skelAnime.curFrame * (M_PIf / 5.0f)) * 10.0f * 0.01f, 0.5f, 1.0f);
    Actor_MoveWithoutGravityReverse(&this->actor);

    if (this->actor.speed >= 20.0f) {
        // Jump over platform
        if (this->unk_242 == 1) {
            if (sqrtf(SQXZ(this->actor.world.pos)) < 700.0f) {
                Boss03_SetupJumpOverPlatform(this, play);
                return;
            }
        }

        // Attack platform
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            Audio_PlaySfx(NA_SE_IT_BIG_BOMB_EXPLOSION);
            Actor_RequestQuakeAndRumble(&this->actor, play, 20, 15);
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_WATER_EFFECT, 0.0f, this->waterHeight, 0.0f, 0, 0, 0x96,
                        ENWATEREFFECT_TYPE_GYORG_SHOCKWAVE);

            // Player is above water && Player is standing on ground
            if ((this->waterHeight < player->actor.world.pos.y) && (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                func_800B8D50(play, NULL, 7.0f, Math_Atan2S_XY(player->actor.world.pos.z, player->actor.world.pos.x),
                              7.0f, 0);
            }

            func_809E344C(this, play);
            this->workTimer[WORK_TIMER_UNK1_A] = 50;
        }
    }
}

void Boss03_SetupJumpOverPlatform(Boss03* this, PlayState* play) {
    this->actionFunc = Boss03_JumpOverPlatform;
    this->actor.gravity = -2.0f;
    this->actor.velocity.y = 30.0f;
    this->actor.speed = 25.0f;
    Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_JUMP_OLD);
}

void Boss03_JumpOverPlatform(Boss03* this, PlayState* play) {
    this->bubbleEffectSpawnCount = 0;
    Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_JUMP_LEV_OLD - SFX_FLAG);

    this->skelAnime.playSpeed = 2.0f;
    SkelAnime_Update(&this->skelAnime);

    Math_ApproachS(&this->actor.world.rot.x, this->actor.velocity.y * -300.0f, 2, 0x2000);
    this->actor.shape.rot.x = this->actor.world.rot.x;

    Actor_MoveWithGravity(&this->actor);
    if ((this->actor.velocity.y < 0.0f) && (this->actor.world.pos.y < this->waterHeight + 50.0f)) {
        this->bubbleEffectSpawnCount = 2;
        this->actor.gravity = 0.0f;
        Math_ApproachZeroF(&this->actor.velocity.y, 1.0f, 1.0f);
        if (this->actor.velocity.y == 0.0f) {
            func_809E344C(this, play);
            this->workTimer[WORK_TIMER_UNK1_A] = 50;
        }
    } else {
        s16 i;
        Vec3f sp30;

        for (i = 0; i < 3; i++) {
            sp30.x = this->actor.world.pos.x + Rand_CenteredFloat(150.0f);
            sp30.y = this->actor.world.pos.y;
            sp30.z = this->actor.world.pos.z + Rand_CenteredFloat(150.0f);
            Boss03_SpawnEffectDroplet(play, &sp30);
        }
    }
}

/* End of Gyorg's Init and actionFuncs section */

/* Start of Gyorg's Cutscenes section */

void Boss03_SetupIntroCutscene(Boss03* this, PlayState* play) {
    this->actionFunc = Boss03_IntroCutscene;
    Animation_MorphToLoop(&this->skelAnime, &gGyorgFastSwimmingAnim, -10.0f);
    this->skelAnime.playSpeed = 2.0f;
}

Vec3f D_809E9104[] = {
    { 770.0f, 200.0f, 720.0f },
    { 831.0f, 200.0f, -570.0f },
    { 0.0f, 450.0f, 0.0f },
};

void Boss03_IntroCutscene(Boss03* this, PlayState* play) {
    s16 i;
    Vec3f effectPos;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    s32 pad;
    f32 sp5C;
    s16 sp5A;
    s16 pad2;
    s16 bubblesToSpawnNum = 0;
    f32 phi_f2;
    Player* player = GET_PLAYER(play);

    this->bubbleEffectSpawnCount = 0;
    this->csTimer++;
    this->unk_290 = 0;

    sp5A = 0x4BC;

    switch (this->csState) {
        case 0:
            if (player->actor.world.pos.y < 1350.0f) {
                Cutscene_StartManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
                this->subCamId = Play_CreateSubCamera(play);
                Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
                Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);

                this->actor.world.rot.y = -0x7B30;
                this->prevPlayerPos.y = 1850.0f;

                this->actor.world.pos.x = 1400.0f;
                this->actor.world.pos.y = 130.0f;
                this->actor.world.pos.z = 1400.0f;

                player->actor.shape.rot.y = 0;
                player->actor.world.pos.y = 1850.0f;
                player->actor.world.rot.y = player->actor.shape.rot.y;

                this->subCamAt.y = player->actor.world.pos.y + 30.0f;
                this->csState = 1;
                this->csTimer = 0;
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                this->unk_2D5 = true;

                this->subCamFov = KREG(14) + 60.0f;

                case 1:
                    player->actor.world.pos.z = 0.0f;
                    player->actor.world.pos.x = 0.0f;
                    player->actor.speed = 0.0f;

                    this->subCamEye.x = 100.0f;
                    this->subCamEye.y = 540.0f;

                    this->subCamEye.z = player->actor.world.pos.z + 100.0f;
                    this->subCamAt.x = player->actor.world.pos.x;

                    Math_ApproachF(&this->subCamAt.y, player->actor.world.pos.y + 30.0f, 0.5f, 100.0f);
                    this->subCamAt.z = player->actor.world.pos.z;

                    if (this->csTimer > 105) {
                        this->csState = 2;
                        this->csTimer = 0;
                        this->unk_240 = 0;
                        Play_EnableMotionBlur(150);
                        this->subCamFov = 80.0f;

                        case 2:
                            Actor_PlaySfx(&this->actor, NA_SE_EN_KONB_DEMO_MOVE_OLD - SFX_FLAG);

                            xDiff = D_809E9104[this->unk_242].x - this->actor.world.pos.x;
                            yDiff = D_809E9104[this->unk_242].y - this->actor.world.pos.y;
                            zDiff = D_809E9104[this->unk_242].z - this->actor.world.pos.z;

                            Math_ApproachS(&this->actor.world.rot.x,
                                           Math_Atan2S_XY(sqrtf(SQ(xDiff) + SQ(zDiff)), -yDiff), 0xA, this->unk_274);
                            Math_ApproachS(&this->actor.world.rot.y, Math_Atan2S_XY(zDiff, xDiff), 0xA, this->unk_274);
                            Math_ApproachS(&this->unk_274, 0x200, 1, 0x10);

                            if ((this->csTimer > 30) && (this->csTimer < 50)) {
                                bubblesToSpawnNum = 2;
                            }

                            if ((this->csTimer == 40) || (this->csTimer == (u32)(KREG(91) + 270))) {
                                Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_WATER_DEEP);
                            }

                            if (this->csTimer > 50) {
                                Math_ApproachF(&this->actor.speed, this->unk_278, 1.0f, 0.1f);
                            }

                            if (this->unk_242 < 2) {
                                if (sqrtf(SQ(xDiff) + SQ(zDiff)) < 100.0f) {
                                    this->unk_242++;
                                    this->unk_274 = 0;
                                    if (this->unk_242 >= 2) {
                                        this->csTimer = 100;
                                    }
                                }
                                this->unk_278 = 5.0f;
                            } else {
                                this->unk_278 = 0.0f;
                                bubblesToSpawnNum = 1;
                                if ((this->actor.speed == 0.0f) && (this->csTimer > 230)) {
                                    this->csState = 3;
                                    this->csTimer = 0;
                                }
                                if (this->csTimer == 165) {
                                    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_WATER_MID);
                                }
                                if (this->csTimer == 180) {
                                    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_WATER_DEEP);
                                }
                            }
                    }
            }
            break;

        case 3:
            Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_PREATTACK_OLD - SFX_FLAG);
            sp5A = 0x1970;
            Math_ApproachF(&this->actor.speed, 15.0f, 1.0f, 2.0f);
            if (this->csTimer > 20) {
                this->csState = 4;
                this->csTimer = 0;
            }
            break;

        case 4:
            player->actor.world.rot.y = player->actor.shape.rot.y = 0;

            if (this->csTimer == 5) {
                // Rotates Player towards Gyorg
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_8);
            }

            this->subCamEye.x = player->actor.world.pos.x + 30.0f;
            this->subCamEye.y = player->actor.world.pos.y + Player_GetHeight(player) - 4.0f + BREG(17);
            this->subCamEye.z = player->actor.world.pos.z - 30.0f;

            this->subCamAt.x = player->actor.world.pos.x;
            this->subCamAt.y = player->actor.world.pos.y + Player_GetHeight(player) - 18.0f + 6.0f + BREG(18);
            this->subCamAt.z = player->actor.world.pos.z;

            if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
                this->subCamEye.y -= 60.0f;
                this->subCamAt.y -= 35.0f;
            }

            this->subCamFov = 60.0f;
            if (this->csTimer == 16) {
                this->csState = 5;
                this->csTimer = 0;
                this->unk_2D5 = false;
                this->actor.speed = -200.0f;
                Actor_MoveWithoutGravityReverse(&this->actor);
                this->actor.world.pos.y = this->waterHeight - 150.0f;
                Play_DisableMotionBlur();

                case 5:
                    SkelAnime_Update(&this->skelAnime);
                    this->actor.speed = 20.0f;
                    Actor_MoveWithoutGravityReverse(&this->actor);
                    player->actor.shape.rot.y = -0x1470;
                    player->actor.world.rot.y = player->actor.shape.rot.y;

                    this->subCamEye.x = player->actor.world.pos.x + 30.0f - 90.0f + 300.0f;
                    this->subCamEye.y = player->actor.world.pos.y + 40.0f + 10.0f;
                    this->subCamEye.z = player->actor.world.pos.z - 30.0f + 160.0f + 300.0f;

                    this->subCamAt.x = this->actor.world.pos.x;
                    this->subCamAt.y = this->actor.world.pos.y - 100.0f;
                    this->subCamAt.z = this->actor.world.pos.z;

                    if (this->csTimer == 10) {
                        this->actor.velocity.y = 30.0f;
                        this->csState = 6;
                        this->csTimer = 0;
                        this->actor.gravity = -1.5f;
                        this->actor.speed = 20.0f;

                        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_BOSS | SEQ_FLAG_ASYNC);
                        Actor_PlaySfx(&this->actor, NA_SE_EN_KONB_JUMP_OLD);
                        this->skelAnime.playSpeed = 1.0f;
                    }
            }
            break;

        case 6:
            Actor_PlaySfx(&this->actor, NA_SE_EN_KONB_JUMP_LEV_OLD - SFX_FLAG);

            if (this->csTimer == 30) {
                TitleCard_InitBossName(&play->state, &play->actorCtx.titleCtx,
                                       Lib_SegmentedToVirtual(gGyorgTitleCardTex), 160, 180, 128, 40);
            }

            if ((this->csTimer < 24) || (this->csTimer >= 90)) {
                SkelAnime_Update(&this->skelAnime);
                Math_ApproachS(&this->actor.world.rot.x, this->actor.velocity.y * -300.0f, 3, 0x1000);
                Actor_MoveWithGravity(&this->actor);
                if ((this->actor.velocity.y <= 0.0f) && (this->actor.world.pos.y < (this->waterHeight + 50.0f))) {
                    this->bubbleEffectSpawnCount = 2;
                    this->actor.gravity = 0.0f;
                    Math_ApproachZeroF(&this->actor.velocity.y, 1.0f, 1.0f);
                    Math_ApproachZeroF(&this->actor.speed, 1.0f, 0.5f);
                } else {
                    if (1) {}
                    if (1) {}
                    if (1) {}

                    for (i = 0; i < 3; i++) {
                        effectPos.x = Rand_CenteredFloat(150.0f) + this->actor.world.pos.x;
                        effectPos.y = this->actor.world.pos.y;
                        effectPos.z = Rand_CenteredFloat(150.0f) + this->actor.world.pos.z;

                        Boss03_SpawnEffectDroplet(play, &effectPos);
                    }

                    this->subCamAtNext.x = this->actor.world.pos.x;
                    this->subCamAtNext.y = this->actor.world.pos.y - 100.0f;
                    this->subCamAtNext.z = this->actor.world.pos.z;
                }
            } else {
                Math_ApproachF(&this->subCamEye.x, player->actor.world.pos.x + 30.0f - 90.0f + 300.0f - 90.0f, 0.05f,
                               3.0f);
                Math_ApproachF(&this->subCamEye.y, player->actor.world.pos.y + 40.0f + 10.0f + 90.0f, 0.05f, 3.0f);
                Math_ApproachF(&this->subCamEye.z, player->actor.world.pos.z - 30.0f + 160.0f + 300.0f - 90.0f, 0.05f,
                               3.0f);
                Math_ApproachF(&this->unk_568, 90.0f, 0.05f, 3.0f);
            }

            Math_ApproachF(&this->subCamAt.x, this->subCamAtNext.x, 0.5f, 100.0f);
            Math_ApproachF(&this->subCamAt.y, this->subCamAtNext.y + this->unk_568, 0.5f, 100.0f);
            Math_ApproachF(&this->subCamAt.z, this->subCamAtNext.z, 0.5f, 100.0f);

            if (this->csTimer == 145) {
                Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);

                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;

                func_80169AFC(play, this->subCamId, 0);
                Cutscene_StopManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                this->subCamId = SUB_CAM_ID_DONE;

                func_809E344C(this, play);
                this->workTimer[WORK_TIMER_UNK1_A] = 50;

                SET_EVENTINF(EVENTINF_INTRO_CS_WATCHED_GYORG);
            }
            break;
    }

    this->actor.shape.rot = this->actor.world.rot;

    if ((this->csState == 2) || (this->csState == 3)) {
        Actor_MoveWithoutGravityReverse(&this->actor);

        phi_f2 = this->actor.speed * 0.02f;
        phi_f2 = CLAMP_MAX(phi_f2, 0.12f);

        sp5C = Math_SinS(this->unk_240 * sp5A) * phi_f2;
        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_APPLY);
        Matrix_RotateYF(sp5C, MTXMODE_APPLY);
        Matrix_RotateXS(this->actor.world.rot.x, MTXMODE_APPLY);
        Matrix_MultVecZ(100.0f, &this->subCamAt);

        this->subCamEye = this->actor.world.pos;

        for (i = 0; i < bubblesToSpawnNum; i++) {
            effectPos.x = Rand_CenteredFloat(100.0f) + this->subCamAt.x;
            effectPos.y = (Rand_CenteredFloat(100.0f) + this->subCamAt.y) - 150.0f;
            effectPos.z = Rand_CenteredFloat(100.0f) + this->subCamAt.z;

            Boss03_SpawnEffectBubble(play, &effectPos);
        }
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
        Play_SetCameraFov(play, this->subCamId, this->subCamFov);
    }
}

void Boss03_SetupDeathCutscene(Boss03* this, PlayState* play) {
    this->actionFunc = Boss03_DeathCutscene;
    Animation_MorphToLoop(&this->skelAnime, &gGyorgFloppingAnim, -10.0f);
    this->floppingAnimLastFrame = Animation_GetLastFrame(&gGyorgFloppingAnim);
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
    this->workTimer[WORK_TIMER_UNK0_C] = 0;
    this->unk_242 = 0;
    this->csState = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void Boss03_DeathCutscene(Boss03* this, PlayState* play) {
    s16 i;
    Vec3f sp90;
    Vec3f sp84;
    Vec3f sp78;
    f32 aux;
    f32 aux2;
    s32 pad;
    f32 pad2;
    f32 sp64;
    Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);
    Player* player;
    f32 sp4C;

    aux2 = 0.0f;
    player = GET_PLAYER(play);
    sp64 = this->actor.scale.x * 5.0f;

    this->csTimer++;

    switch (this->csState) {
        case 0:
            if (CutsceneManager_GetCurrentCsId() == CS_ID_NONE) {
                Cutscene_StartManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_1);
                this->subCamId = Play_CreateSubCamera(play);
                Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
                Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
                this->unk_2BE = Math_Atan2S_XY(this->actor.world.pos.z, this->actor.world.pos.x);

                // Player is above water && Player is standing on ground
                if ((this->waterHeight < player->actor.world.pos.y) &&
                    (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                    player->actor.world.pos.x = 0.0f;
                    player->actor.world.pos.z = -200.0f;
                }

                case 1:
                    this->csTimer = 0;
                    this->csState = 2;
                    this->subCamEye.x = mainCam->eye.x;
                    this->subCamEye.y = mainCam->eye.y;
                    this->subCamEye.z = mainCam->eye.z;
                    this->subCamAt.x = mainCam->at.x;
                    this->subCamAt.y = mainCam->at.y;
                    this->subCamAt.z = mainCam->at.z;

                    aux = this->subCamEye.x - this->actor.world.pos.x;
                    this->unk_568 = Math_Atan2F_XY(this->subCamEye.z - this->actor.world.pos.z, aux);

                    this->unk_570 = 0.0f;
                    this->unk_56C = 0.0f;

                case 2:
                    sp90.x = 0.0f;
                    sp90.y = (50.0f * sp64) + 150.0f;
                    sp90.z = (400.0f * sp64) + 300.0f;

                    this->subCamAtNext.x = this->actor.world.pos.x;
                    this->subCamAtNext.y = this->actor.world.pos.y;
                    this->subCamAtNext.z = this->actor.world.pos.z;

                    this->unk_568 += this->unk_56C;
                    Matrix_RotateYF(this->unk_568, MTXMODE_NEW);
                    Matrix_MultVec3f(&sp90, &this->subCamEyeNext);

                    this->subCamEyeNext.x += this->actor.world.pos.x;
                    this->subCamEyeNext.y += this->waterHeight;
                    this->subCamEyeNext.z += this->actor.world.pos.z;

                    sp4C = 40.0f + aux2;
                    Math_ApproachF(&this->subCamEye.x, this->subCamEyeNext.x, 0.1f, sp4C);
                    Math_ApproachF(&this->subCamEye.y, this->subCamEyeNext.y, 0.1f, sp4C);
                    Math_ApproachF(&this->subCamEye.z, this->subCamEyeNext.z, 0.1f, sp4C);
                    sp4C = 70.0f + aux2;
                    Math_ApproachF(&this->subCamAt.x, this->subCamAtNext.x, 0.1f, sp4C);
                    Math_ApproachF(&this->subCamAt.y, this->subCamAtNext.y, 0.1f, sp4C);
                    Math_ApproachF(&this->subCamAt.z, this->subCamAtNext.z, 0.1f, sp4C);
            }
            break;
    }

    SkelAnime_Update(&this->skelAnime);

    switch (this->unk_242) {
        case 0:
            Math_ApproachF(&this->actor.world.pos.y, Math_SinS(this->unk_240 * 0x1000) * 80.0f + this->waterHeight,
                           1.0f, 10.0f);
            this->actor.shape.rot.z += 0x100;
            Matrix_RotateYS(this->unk_2BE, MTXMODE_NEW);
            Matrix_MultVecZ(500.0f, &sp84);
            Math_ApproachF(&this->actor.world.pos.x, sp84.x, 0.1f, 5.0f);
            Math_ApproachF(&this->actor.world.pos.z, sp84.z, 0.1f, 5.0f);

            if (Animation_OnFrame(&this->skelAnime, this->floppingAnimLastFrame)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_KONB_DEAD_JUMP2_OLD);
            }
            if (Animation_OnFrame(&this->skelAnime, this->floppingAnimLastFrame * 0.5f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_KONB_DEAD_JUMP_OLD);
            }

            if ((this->workTimer[WORK_TIMER_UNK0_C] == 0) && ((this->waterHeight - 100.0f) < this->actor.world.pos.y)) {
                this->workTimer[WORK_TIMER_UNK0_C] = Rand_ZeroFloat(15.0f) + 15.0f;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_WATER_EFFECT, this->actor.world.pos.x, this->waterHeight,
                            this->actor.world.pos.z, 0, 0, 0x78, ENWATEREFFECT_TYPE_GYORG_RIPPLES);

                if (this->actionFunc == Boss03_DeathCutscene) {
                    if ((D_809E9840 % 2) != 0) {
                        Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_JUMP_OLD);
                    } else {
                        Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_SINK_OLD);
                    }
                    D_809E9840++;
                }

                this->unk_284 = this->actor.world.pos.x;
                this->unk_288 = this->waterHeight;
                this->unk_28C = this->actor.world.pos.z;

                this->unk_280 = 27;
                if ((fabsf(this->actor.world.pos.x - sp84.x) < 5.0f) &&
                    (fabsf(this->actor.world.pos.z - sp84.z) < 5.0f)) {
                    this->unk_242 = 1;
                    this->actor.gravity = -2.0f;
                    this->actor.velocity.y = 25.0f;
                    this->actor.speed = 10.0f;
                    this->actor.world.rot.y = this->unk_2BE + 0x8000;
                    this->unk_240 = 0;
                    Actor_PlaySfx(&this->actor, NA_SE_EN_KONB_DEAD_JUMP2_OLD);
                }
            }
            Math_ApproachF(&this->unk_56C, 0.01f, 1.0f, 0.0005f);
            break;

        case 1:
            if (this->unk_240 == 0x96) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_CLEAR_BOSS | SEQ_FLAG_ASYNC);
            }
            Math_ApproachF(&this->unk_56C, 0.01f, 1.0f, 0.0005f);
            Math_ApproachF(&this->actor.scale.x, 0.01f, 0.05f, 0.001f);
            Actor_SetScale(&this->actor, this->actor.scale.x);

            if (this->actor.velocity.y < 0.0f) {
                if (this->actor.world.pos.y < PLATFORM_HEIGHT + (100.0f * sp64)) {
                    this->actor.world.pos.y = PLATFORM_HEIGHT + (100.0f * sp64);
                    this->actor.velocity.y = ((Rand_ZeroFloat(10.0f) + 7.5f) * sp64) + 7.5f;
                    this->actor.speed = ((Rand_ZeroFloat(5.0f) + 2.5f) * sp64) + 2.5f;

                    if (Rand_ZeroOne() < 0.5f) {
                        this->shapeRotTargetX =
                            (TRUNCF_BINANG(Rand_CenteredFloat(0x1F4)) + this->shapeRotTargetX) + 0x8000;
                    }

                    if (Rand_ZeroOne() < 0.5f) {
                        this->shapeRotTargetZ =
                            (TRUNCF_BINANG(Rand_CenteredFloat(0x1F4)) + this->shapeRotTargetZ) + 0x8000;
                    }

                    if (Rand_ZeroOne() < 0.5f) {
                        this->shapeRotTargetY = Rand_ZeroFloat(0x10000);
                    }

                    this->actor.world.rot.y = Math_Atan2S_XY(-this->actor.world.pos.z, -this->actor.world.pos.x);

                    Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_BOUND_OLD);
                    Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_BOUND2_OLD);
                }
            }

            Math_ApproachS(&this->actor.shape.rot.y, this->shapeRotTargetY, 3, 0x500);
            Math_ApproachS(&this->actor.shape.rot.x, this->shapeRotTargetX, 3, 0xA00);
            Math_ApproachS(&this->actor.shape.rot.z, this->shapeRotTargetZ, 3, 0xA00);

            sp4C = 150.0f * sp64;

            for (i = 0; i < 1; i++) {
                sp78.x = Rand_CenteredFloat(sp4C) + this->actor.world.pos.x;
                sp78.y = this->actor.world.pos.y;
                sp78.z = Rand_CenteredFloat(sp4C) + this->actor.world.pos.z;
                Boss03_SpawnEffectDroplet(play, &sp78);
            }

            Actor_MoveWithGravity(&this->actor);
            if (this->actor.scale.x <= 0.0111f) {
                this->unk_242 = 2;
                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, 0.0f, PLATFORM_HEIGHT, 200.0f,
                                   0, 0, 0, ENDOORWARP1_FF_1);
                Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, this->actor.focus.pos.x, PLATFORM_HEIGHT,
                            this->actor.focus.pos.z, 0, 0, 0, BHEART_PARAM_NORMAL);
                this->csTimer = 0;
                Actor_SetScale(&this->actor, 0.0f);
                AudioSfx_StopByPos(&this->actor.projectedPos);
            }
            break;

        case 2:
            Math_ApproachZeroF(&this->unk_56C, 1.0f, 0.0005f);
            if (this->csTimer == 60) {
                mainCam = Play_GetCamera(play, CAM_ID_MAIN);
                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                func_80169AFC(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Cutscene_StopManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                this->csState = 3;
                Play_DisableMotionBlur();
                Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_INIT_OLD);
                Boss03_PlayUnderwaterSfx(&D_809E9848, NA_SE_EN_KONB_INIT_OLD);
            }
            break;

        case 3:
            this->actor.world.pos.y = 5000.0f;
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }
}

void Boss03_SetupSpawnSmallFishesCutscene(Boss03* this, PlayState* play) {
    this->actionFunc = Boss03_SpawnSmallFishesCutscene;
    Animation_MorphToLoop(&this->skelAnime, &gGyorgBackingUpAnim, -15.0f);
    this->csState = 0;
    this->csTimer = 0;
}

void Boss03_SpawnSmallFishesCutscene(Boss03* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    this->csTimer++;
    switch (this->csState) {
        case 0:
            if (CutsceneManager_GetCurrentCsId() == CS_ID_NONE) {
                Cutscene_StartManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_1);
                this->subCamId = Play_CreateSubCamera(play);
                Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
                Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
                this->csState = 1;
                this->unk_2BE = 0xBB8;

                case 1:
                    if (player->actor.world.pos.y < 437.0f) {
                        player->actor.world.pos.z = 500.0f;
                        player->actor.world.pos.x = 500.0f;
                    }

                    this->actor.shape.rot.z = 0;
                    this->actor.world.pos.z = 1000.0f;
                    this->actor.world.pos.x = 1000.0f;
                    this->actor.world.pos.y = 200.0f;
                    this->actor.shape.rot.x = this->actor.shape.rot.z;

                    this->actor.shape.rot.y = this->actor.world.rot.y =
                        Math_Atan2S_XY(-this->actor.world.pos.x, -this->actor.world.pos.x);
                    this->unk_260 = 0.0f;

                    player->actor.shape.rot.y = player->actor.world.rot.y = this->actor.world.rot.y + 0x8000;

                    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                     MTXMODE_NEW);
                    Matrix_RotateYS(this->actor.shape.rot.y + this->unk_2BE, MTXMODE_APPLY);
                    Matrix_MultVecZ(340.0f, &this->subCamEye);

                    this->subCamAt.x = this->actor.world.pos.x;
                    this->subCamAt.y = this->actor.world.pos.y;
                    this->subCamAt.z = this->actor.world.pos.z;

                    Math_ApproachS(&this->unk_2BE, -4000, 10, 70);

                    if (this->csTimer >= 61) {
                        Math_ApproachS(&this->jawZRot, 0x3200, 5, 0x500);
                        if ((this->csTimer >= 90) && (this->csTimer < 130)) {
                            if ((this->csTimer % 2) != 0) {
                                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_TANRON3,
                                                   this->actor.world.pos.x - 110.0f, this->actor.world.pos.y - 20.0f,
                                                   this->actor.world.pos.z - 110.0f, 0, this->actor.shape.rot.y, 0, 0);
                                this->numSpawnedSmallFish++;
                            }
                            if ((this->csTimer % 8) == 0) {
                                Actor_PlaySfx(&this->actor, NA_SE_EN_KONB_MINI_APPEAR);
                            }
                        }
                    }

                    if (this->csTimer >= 150) {
                        Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);

                        mainCam->eye = this->subCamEye;
                        mainCam->eyeNext = this->subCamEye;
                        mainCam->at = this->subCamAt;

                        func_80169AFC(play, this->subCamId, 0);
                        this->subCamId = SUB_CAM_ID_DONE;
                        Cutscene_StopManual(play, &play->csCtx);
                        Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);

                        func_809E344C(this, play);
                        this->workTimer[WORK_TIMER_UNK1_A] = 50;
                    }
            }
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }
}

/* End of Gyorg's Cutscenes section */

/* Start of Gyorg's More actionFuncs and Update section */

void Boss03_SetupStunned(Boss03* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actionFunc != Boss03_Stunned) {
        Animation_MorphToLoop(&this->skelAnime, &gGyorgStunnedAnim, -15.0f);
        this->workTimer[WORK_TIMER_STUNNED] = 200;
        this->actionFunc = Boss03_Stunned;
    }

    if (&this->actor == player->actor.parent) {
        player->av2.actionVar2 = 101;
        player->actor.parent = NULL;
        player->csAction = PLAYER_CSACTION_NONE;
        Play_DisableMotionBlur();
    }

    this->unk_240 = 0;
}

void Boss03_Stunned(Boss03* this, PlayState* play) {
    this->actor.hintId = TATL_HINT_ID_GYORG_STUNNED;

    if (this->unk_240 >= 16) {
        Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_COMMON_WEAKENED - SFX_FLAG);
    }

    SkelAnime_Update(&this->skelAnime);

    if ((this->waterHeight + 30.0f) < this->actor.world.pos.y) {
        this->actor.gravity = -2.0f;
        Actor_MoveWithGravity(&this->actor);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            Audio_PlaySfx(NA_SE_IT_WALL_HIT_HARD);
            Actor_RequestQuakeAndRumble(&this->actor, play, 10, 10);
        }
    } else {
        Math_ApproachS(&this->actor.shape.rot.z, -0x6000, 0xA, 0x900);
        Math_ApproachS(&this->actor.world.rot.x, 0x800, 0xA, 0x1000);
        this->actor.shape.rot.x = this->actor.world.rot.x;

        Math_ApproachF(&this->actor.world.pos.y, 100.0f, 0.05f, 5.0f);
        Math_ApproachF(&this->actor.speed, 0.0f, 1.0f, 1.5f);
        Actor_MoveWithoutGravityReverse(&this->actor);
    }

    if (this->workTimer[WORK_TIMER_STUNNED] == 0) {
        func_809E344C(this, play);
        // WORK_TIMER_UNK1_A wasn't set
    }
}

void Boss03_SetupDamaged(Boss03* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gGyorgFloppingAnim, -10.0f);
    this->actionFunc = Boss03_Damaged;
    this->workTimer[WORK_TIMER_CURRENT_ACTION] = 30;
}

/**
 * Invulnerability period by being damaged by Player
 */
void Boss03_Damaged(Boss03* this, PlayState* play) {
    this->unk_25C = 15;

    SkelAnime_Update(&this->skelAnime);

    Math_ApproachS(&this->jawZRot, ((Math_SinS(this->unk_240 * 0x2000) * 3000.0f) + 0x3000), 2, 0x3000);
    Math_ApproachF(&this->actor.world.pos.y, 200.0f, 0.05f, 10.0f);

    if (this->workTimer[WORK_TIMER_CURRENT_ACTION] == 0) {
        if ((s8)this->actor.colChkInfo.health <= 5) {
            if (!this->hasSpwanedSmallFishes) {
                this->hasSpwanedSmallFishes++;
                Boss03_SetupSpawnSmallFishesCutscene(this, play);
                return;
            }
        }

        func_809E344C(this, play);
        this->workTimer[WORK_TIMER_UNK1_A] = 100;
    }
}

/* End of ActionFuncs section */

void Boss03_UpdateCollision(Boss03* this, PlayState* play) {
    ColliderElement* acHitElem;
    u8 sp4B = true;
    Player* player = GET_PLAYER(play);
    s32 i;
    s32 phi_v1;
    u32 phi_v0;
    Boss03ActionFunc stunnedActionFunc = Boss03_Stunned;

    if (((KREG(20) + (this->waterHeight - 50.0f)) < player->actor.world.pos.y) &&
        (player->transformation != PLAYER_FORM_FIERCE_DEITY)) {
        sp4B = false;
    }

    if (this->waterHeight < player->actor.world.pos.y) {
        for (i = 0; i < ARRAY_COUNT(sHeadJntSphElementsInit); i++) {
            if (this->headCollider.elements[i].base.atElemFlags & ATELEM_HIT) {
                this->headCollider.elements[i].base.atElemFlags &= ~ATELEM_HIT;
                player->pushedYaw = this->actor.shape.rot.y;
                player->pushedSpeed = 20.0f;
            }
        }

        for (i = 0; i < ARRAY_COUNT(sBodyJntSphElementsInit); i++) {
            if (this->bodyCollider.elements[i].base.atElemFlags & ATELEM_HIT) {
                this->bodyCollider.elements[i].base.atElemFlags &= ~ATELEM_HIT;
                player->pushedYaw = this->actor.shape.rot.y;
                player->pushedSpeed = 20.0f;
            }
        }
    }

    if (this->unk_25C == 0) {
        if ((this->actionFunc == stunnedActionFunc) && sp4B) {
            for (i = 0; i < ARRAY_COUNT(sBodyJntSphElementsInit); i++) {
                if (this->bodyCollider.elements[i].base.acElemFlags & ACELEM_HIT) {
                    acHitElem = this->bodyCollider.elements[i].base.acHitElem;
                    this->bodyCollider.elements[i].base.acElemFlags &= ~ACELEM_HIT;
                    this->unk_25C = 15;
                    this->unk_25E = 15;

                    // (DMG_SWORD_BEAM | DMG_SPIN_ATTACK | DMG_ZORA_PUNCH | DMG_ZORA_BARRIER | DMG_DEKU_LAUNCH |
                    // DMG_DEKU_SPIN | DMG_GORON_SPIKES | DMG_SWORD | DMG_GORON_PUNCH | DMG_DEKU_STICK)
                    phi_v0 = (acHitElem->atDmgInfo.dmgFlags & 0x038AC302)
                                 ? this->bodyCollider.elements[i].base.acHitElem->atDmgInfo.damage
                                 : 0;

                    phi_v1 = phi_v0;
                    if (phi_v0 < 1) {
                        phi_v1 = 1;
                    }
                    this->actor.colChkInfo.health -= phi_v1;

                    if ((s8)this->actor.colChkInfo.health <= 0) {
                        Boss03_PlayUnderwaterSfx(&D_809E9848, NA_SE_EN_KONB_DEAD_OLD);
                        Boss03_PlayUnderwaterSfx(&D_809E9848, NA_SE_EN_KONB_DEAD2_OLD);
                        Boss03_SetupDeathCutscene(this, play);
                        GameInteractor_ExecuteOnBossDefeated(this->actor.id); // 2S2H Time Splits
                        Enemy_StartFinishingBlow(play, &this->actor);
                    } else {
                        Boss03_SetupDamaged(this, play);
                        Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_DAMAGE_OLD);
                    }
                    return;
                }
            }
        }

        for (i = 0; i < ARRAY_COUNT(sHeadJntSphElementsInit); i++) {
            if (this->headCollider.elements[i].base.acElemFlags & ACELEM_HIT) {
                acHitElem = this->headCollider.elements[i].base.acHitElem;
                this->headCollider.elements[i].base.acElemFlags &= ~ACELEM_HIT;
                this->unk_25C = 15;

                if (this->actionFunc != stunnedActionFunc) {
                    Boss03_SetupStunned(this, play);
                    Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_DAMAGE_OLD);

                    if (&this->actor == player->actor.parent) {
                        player->av2.actionVar2 = 101;
                        player->actor.parent = NULL;
                        player->csAction = PLAYER_CSACTION_NONE;
                        Play_DisableMotionBlur();
                    }

                    continue;
                }

                if (sp4B) {
                    this->unk_25E = 15;

                    // (DMG_SWORD_BEAM | DMG_SPIN_ATTACK | DMG_ZORA_PUNCH | DMG_ZORA_BARRIER | DMG_DEKU_LAUNCH |
                    // DMG_DEKU_SPIN | DMG_GORON_SPIKES | DMG_SWORD | DMG_GORON_PUNCH | DMG_DEKU_STICK)
                    phi_v0 = (acHitElem->atDmgInfo.dmgFlags & 0x038AC302)
                                 ? (this->headCollider.elements[i].base.acHitElem->atDmgInfo.damage)
                                 : 0;

                    phi_v1 = phi_v0;
                    if (phi_v0 < 1) {
                        phi_v1 = 1;
                    }
                    this->actor.colChkInfo.health -= phi_v1;
                    if ((s8)this->actor.colChkInfo.health <= 0) {
                        Boss03_PlayUnderwaterSfx(&D_809E9848, NA_SE_EN_KONB_DEAD_OLD);
                        Boss03_PlayUnderwaterSfx(&D_809E9848, NA_SE_EN_KONB_DEAD2_OLD);
                        Enemy_StartFinishingBlow(play, &this->actor);
                        Boss03_SetupDeathCutscene(this, play);
                    } else {
                        Boss03_SetupDamaged(this, play);
                        Boss03_PlayUnderwaterSfx(&this->actor.projectedPos, NA_SE_EN_KONB_DAMAGE_OLD);
                    }
                }
                return;
            }
        }
    }
}

void Boss03_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Boss03* this = (Boss03*)thisx;
    Actor* dblueActor;
    Player* player = GET_PLAYER(play);
    s32 i;
    Vec3f wetSpotPos;
    Vec3f bubblePos;
    Vec3f dropletPos;
    s16 j;
    f32 yRot;

    this->actor.hintId = TATL_HINT_ID_GYORG;

    if (!D_809E9842 && (player->actor.world.pos.y < (PLATFORM_HEIGHT + 5.0f))) {
        D_809E9842 = true;
        D_809E9841 = 5;
    }

    if (KREG(63) == 0) {
        this->unk_290 = 1;
        this->unk_240++;
        this->unk_2BC = 1;

        this->unk_2BD = false;

        Math_Vec3f_Copy(&D_809E9848, &this->actor.projectedPos);

        for (i = 0; i < ARRAY_COUNT(this->workTimer); i++) {
            if (this->workTimer[i] != 0) {
                this->workTimer[i]--;
            }
        }

        if (this->unk_324 != 0) {
            this->unk_324--;
        }

        if (this->unk_25C != 0) {
            this->unk_25C--;
        }

        if (this->unk_25E != 0) {
            this->unk_25E--;
        }

        this->actionFunc(this, play);

        if (this->actionFunc != Boss03_DeathCutscene) {
            Math_ApproachS(&this->actor.shape.rot.z, 0, 0xA, 0x800);
            this->actor.world.pos.y -= 100.0f;
            this->actor.prevPos.y -= 100.0f;
            Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 150.0f, 100.0f,
                                    UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
            this->actor.world.pos.y += 100.0f;
            this->actor.prevPos.y += 100.0f;
        }
    }

    if ((this->actionFunc != Boss03_DeathCutscene) && (!this->unk_2D5)) {
        // Going into or out of the water
        if (((this->actor.world.pos.y < this->waterHeight + 50.0f) &&
             (this->waterHeight + 50.0f <= this->actor.prevPos.y)) ||
            ((this->waterHeight - 50.0f < this->actor.world.pos.y) &&
             (this->actor.prevPos.y <= this->waterHeight - 50.0f))) {
            if ((this->actor.velocity.y < 0.0f) && (this->actionFunc != Boss03_DeathCutscene)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_KONB_SINK_OLD);
            }

            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_WATER_EFFECT, this->actor.world.pos.x, this->waterHeight,
                        this->actor.world.pos.z, 0, 0, 0x78, ENWATEREFFECT_TYPE_GYORG_RIPPLES);

            this->unk_280 = 27;
            this->unk_284 = this->actor.world.pos.x;
            this->unk_288 = this->waterHeight;
            this->unk_28C = this->actor.world.pos.z;
        }
    }

    if (this->actionFunc != Boss03_DeathCutscene) {
        if ((this->actionFunc == Boss03_Stunned) || (this->actionFunc == Boss03_Damaged)) {
            this->bodyCollider.base.colMaterial = COL_MATERIAL_HIT3;
        } else {
            this->bodyCollider.base.colMaterial = COL_MATERIAL_METAL;
        }

        Boss03_UpdateCollision(this, play);

        CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);

        if (player->transformation == PLAYER_FORM_HUMAN) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->headCollider.base);
        }

        if (!this->unk_2BD) {
            if (player->transformation != PLAYER_FORM_HUMAN) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->headCollider.base);
            }
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->headCollider.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
        }

        if ((this->actionFunc != Boss03_Stunned) && (!this->unk_2BD)) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->headCollider.base);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->bodyCollider.base);
        }
    }

    dblueActor = Boss03_FindActorDblueMovebg(play);
    if (dblueActor != NULL) {
        dblueActor->world.pos.y = this->waterHeight;
    }

    Math_ApproachZeroF(&this->unk_260, 0.1f, 0.05f);

    if (this->unk_290 != 0) {
        this->unk_294 += 0.1f;
        this->unk_298 += 0.12f;
        this->leftFinYRot = sinf(this->unk_294) * 1280.0f;
        this->rightFinYRot = sinf(this->unk_298) * 1280.0f;
    } else {
        Math_ApproachS(&this->rightFinYRot, 0, 0xA, 0x100);
        Math_ApproachS(&this->leftFinYRot, 0, 0xA, 0x100);
    }

    Math_ApproachS(&this->bodyYRot, 0, 0xA, 0x100);
    Math_ApproachS(&this->jawZRot, 0, 0xA, 0x200);

    if ((this->unk_240 % 2) == 0) {
        for (i = 0; i < this->bubbleEffectSpawnCount; i++) {
            bubblePos.x = Rand_CenteredFloat(100.0f) + this->actor.world.pos.x;
            bubblePos.y = Rand_CenteredFloat(100.0f) + this->actor.world.pos.y;
            bubblePos.z = Rand_CenteredFloat(100.0f) + this->actor.world.pos.z;

            Boss03_SpawnEffectBubble(play, &bubblePos);
        }
    }

    this->bubbleEffectSpawnCount = 1;
    Boss03_UpdateEffects(play);

    if (D_809E9841 != 0) {
        D_809E9841--;
        if (D_809E9841 == 0) {
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_BOSS | SEQ_FLAG_ASYNC);
        }
    }

    if (this->actionFunc != Boss03_CatchPlayer) {
        Math_ApproachS(&player->actor.world.rot.x, 0, 1, 0x80);
        Math_ApproachS(&player->actor.shape.rot.x, 0, 1, 0x80);
    }

    if ((this->waterHeight < player->actor.world.pos.y) && (this->prevPlayerPos.y <= this->waterHeight)) {
        this->wetSpotEffectSpawnCount = 20;
    }

    // Player is standing on ground && Player is above water
    if ((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        (player->actor.shape.feetPos[FOOT_LEFT].y >= WATER_HEIGHT + 8.0f)) {
        if (this->wetSpotEffectSpawnCount != 0) {
            this->wetSpotEffectSpawnCount--;

            wetSpotPos.x = Rand_CenteredFloat(50.0f) + player->actor.world.pos.x;
            wetSpotPos.y = PLATFORM_HEIGHT;
            wetSpotPos.z = Rand_CenteredFloat(50.0f) + player->actor.world.pos.z;
            Boss03_SpawnEffectWetSpot(play, &wetSpotPos);
        }
    }

    this->prevPlayerPos = player->actor.world.pos;

    if (this->waterHeight < this->actor.world.pos.y) {
        Audio_SetSfxUnderwaterReverb(false);
    } else {
        Audio_SetSfxUnderwaterReverb(true);
    }

    if (this->unk_280 != 0) {
        this->unk_280--;
    }

    if ((this->unk_280 == 1) || (this->unk_280 == 5) || (this->unk_280 == 9)) {
        yRot = 0.0f;

        for (j = 0, i = 0; i < 20; j++) {
            Matrix_RotateYF(yRot, MTXMODE_NEW);
            Matrix_MultVecZ(Rand_ZeroFloat(60.000004f) + 312.0f, &dropletPos);
            dropletPos.x += this->unk_284 + Rand_CenteredFloat(40.0f);
            dropletPos.y = PLATFORM_HEIGHT;
            dropletPos.z += this->unk_28C + Rand_CenteredFloat(40.0f);

            if (sqrtf(SQ(dropletPos.x) + SQ(dropletPos.z)) < 355.0f) {
                Boss03_SpawnEffectDroplet(play, &dropletPos);
                i++;
            }

            yRot += ((2.0f * M_PIf) / 50.0f);
            if (j >= 50) {
                break;
            }
        }
    }
}

/* End of Gyorg's More actionFuncs and Update section */

/* Start of Gyorg's Draw section */

void Boss03_SetObject(PlayState* play, s16 objectId) {
    s32 objectSlot = Object_GetSlot(&play->objectCtx, objectId);

    OPEN_DISPS(play->state.gfxCtx);

    gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);

    gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[objectSlot].segment);
    gSPSegment(POLY_XLU_DISP++, 0x06, play->objectCtx.slots[objectSlot].segment);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 Boss03_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    Boss03* this = (Boss03*)thisx;

    if ((limbIndex == GYORG_LIMB_UPPER_TRUNK) || (limbIndex == GYORG_LIMB_LOWER_TRUNK) ||
        (limbIndex == GYORG_LIMB_TAIL)) {
        rot->y += this->bodyYRot;
    }
    if (limbIndex == GYORG_LIMB_UPPER_RIGHT_FIN) {
        rot->y += this->rightFinYRot;
    }
    if (limbIndex == GYORG_LIMB_LOWER_RIGHT_FIN) {
        rot->y += (s16)(2 * this->rightFinYRot);
    }
    if (limbIndex == GYORG_LIMB_UPPER_LEFT_FIN) {
        rot->y -= this->leftFinYRot;
    }
    if (limbIndex == GYORG_LIMB_LOWER_LEFT_FIN) {
        rot->y -= (s16)(2 * this->leftFinYRot);
    }
    if (limbIndex == GYORG_LIMB_JAW) {
        rot->z += this->jawZRot;
    }

    return false;
}

/**
 * Used to manually index the spheres elements of the two colliders
 * Since there are two sets of ColliderJntSph, indices < 2 (ARRAY_COUNT(sHeadJntSphElementsInit)) refers to the first
 * collider and indices >= 2 refers to the second one
 */
static s8 sLimbToSphere[2][GYORG_LIMB_MAX] = {
    {
        -1, // GYORG_LIMB_NONE
        -1, // GYORG_LIMB_ROOT
        0,  // GYORG_LIMB_HEAD
        -1, // GYORG_LIMB_BODY_ROOT
        4,  // GYORG_LIMB_UPPER_TRUNK
        5,  // GYORG_LIMB_LOWER_TRUNK
        6,  // GYORG_LIMB_TAIL
        -1, // GYORG_LIMB_RIGHT_FIN_ROOT
        2,  // GYORG_LIMB_UPPER_RIGHT_FIN
        -1, // GYORG_LIMB_LOWER_RIGHT_FIN
        -1, // GYORG_LIMB_LEFT_FIN_ROOT
        3,  // GYORG_LIMB_UPPER_LEFT_FIN
        -1, // GYORG_LIMB_LOWER_LEFT_FIN
        -1, // GYORG_LIMB_JAW_ROOT
        1,  // GYORG_LIMB_JAW
    },
    // unused
    {
        -1, // GYORG_LIMB_NONE
        -1, // GYORG_LIMB_ROOT
        -1, // GYORG_LIMB_HEAD
        -1, // GYORG_LIMB_BODY_ROOT
        -1, // GYORG_LIMB_UPPER_TRUNK
        -1, // GYORG_LIMB_LOWER_TRUNK
        -1, // GYORG_LIMB_TAIL
        -1, // GYORG_LIMB_RIGHT_FIN_ROOT
        -1, // GYORG_LIMB_UPPER_RIGHT_FIN
        -1, // GYORG_LIMB_LOWER_RIGHT_FIN
        -1, // GYORG_LIMB_LEFT_FIN_ROOT
        -1, // GYORG_LIMB_UPPER_LEFT_FIN
        -1, // GYORG_LIMB_LOWER_LEFT_FIN
        -1, // GYORG_LIMB_JAW_ROOT
        -1, // GYORG_LIMB_JAW
    },
};

Vec3f D_809E9148 = { 600.0f, -100.0f, 0.0f };
Vec3f D_809E9154[] = {
    { 450.0f, 0.0f, 0.0f }, { 140.0f, -60.0f, 0.0f }, { 350.0f, 0.0f, 0.0f }, { 350.0f, 0.0f, 0.0f },
    { 100.0f, 0.0f, 0.0f }, { 150.0f, 0.0f, 0.0f },   { 500.0f, 0.0f, 0.0f },
};
Vec3f D_809E91A8 = { 100000.0f, 100000.0f, 100000.0f };
Vec3f D_809E91B4 = { 300.0f, -100.0f, -200.0f };

void Boss03_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    Boss03* this = (Boss03*)thisx;
    s32 pad;
    s8 sphereElementIndex;
    Vec3f spherePos;
    Player* player = GET_PLAYER(play);

    if (limbIndex == GYORG_LIMB_HEAD) {
        Matrix_MultVec3f(&D_809E9148, &this->actor.focus.pos);
    }

    sphereElementIndex = sLimbToSphere[0][limbIndex];
    if (sphereElementIndex > -1) {
        Matrix_MultVec3f(&D_809E9154[sphereElementIndex], &spherePos);

        if (sphereElementIndex < 2) {
            if ((this->actionFunc == Boss03_Stunned) && (this->waterHeight < player->actor.world.pos.y)) {
                Boss03_UpdateSphereElement(sphereElementIndex, &this->headCollider, &D_809E91A8);
            } else {
                Boss03_UpdateSphereElement(sphereElementIndex, &this->headCollider, &spherePos);
            }
        } else {
            Boss03_UpdateSphereElement(sphereElementIndex - 2, &this->bodyCollider, &spherePos);
        }
    }

    if (limbIndex == GYORG_LIMB_JAW) {
        MtxF mf;

        D_809E91B4.x = this->unk_2C4 + 300.0f;
        Matrix_MultVec3f(&D_809E91B4, &this->insideJawPos);
        Matrix_Get(&mf);
        Matrix_MtxFToYXZRot(&mf, &this->unk_2A2, false);
    }
}

void Boss03_Draw(Actor* thisx, PlayState* play) {
    Boss03* this = (Boss03*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (!this->unk_2D5) {
        if ((this->unk_25E % 2) != 0) {
            POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 255, 900, 1099);
        }

        Matrix_RotateYF(this->unk_260, MTXMODE_APPLY);
        Matrix_Translate(0.0f, -600.0f, 0.0f, MTXMODE_APPLY);
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              Boss03_OverrideLimbDraw, Boss03_PostLimbDraw, &this->actor);
        POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
    }

    this->unk_2BC = 0;

    Boss03_DrawEffects(play);

    CLOSE_DISPS(play->state.gfxCtx);
}

/* End of Gyorg's Draw section */

/* Start of Effect Update/Draw section */

void Boss03_UpdateEffects(PlayState* play) {
    GyorgEffect* eff = play->specialEffects;
    s16 i;
    Vec3f sp94;
    Vec3f velocity;

    for (i = 0; i < GYORG_EFFECT_COUNT; i++, eff++) {
        f32 phi_f0;

        if (eff->type == GYORG_EFFECT_NONE) {
            continue;
        }

        eff->unk_02++;

        eff->pos.x += eff->velocity.x;
        eff->pos.y += eff->velocity.y;
        eff->pos.z += eff->velocity.z;

        eff->velocity.y += eff->accel.y;

        if ((eff->unk_02 & 6) != 0) {
            phi_f0 = 80.0f;
        } else {
            phi_f0 = 200.0f;
        }

        Math_ApproachF(&eff->unk_40, phi_f0, 1.0f, 80.0f);

        if (eff->type == GYORG_EFFECT_DROPLET) {
            eff->unk_34.z += 0.15f;

            Math_ApproachF(&eff->unk_34.x, 0.03f, 0.5f, 0.005f);
            Math_ApproachF(&eff->unk_34.y, 0.5f, 0.5f, 0.02f);

            if (eff->pos.y <= WATER_HEIGHT) {
                eff->type = GYORG_EFFECT_NONE;
                eff->pos.y = WATER_HEIGHT;
                EffectSsGRipple_Spawn(play, &eff->pos, 0, 80, 0);
            } else if (eff->pos.y <= PLATFORM_HEIGHT) {
                s16 j;

                eff->type = GYORG_EFFECT_WET_SPOT;
                eff->pos.y = PLATFORM_HEIGHT;
                eff->unk_34.x = 0.1f;
                eff->unk_34.y = 0.6f;
                eff->velocity = gZeroVec3f;
                eff->accel = gZeroVec3f;
                eff->alpha = 150;
                eff->alphaDelta = Rand_ZeroFloat(4.0f) + 5.0f;

                for (j = 0; j < 4; j++) {
                    Matrix_RotateYF((2.0f * (j * M_PIf)) / 6.0f, MTXMODE_NEW);
                    sp94.x = 0.0f;
                    sp94.y = Rand_ZeroFloat(4.0f) + 2.0f;
                    sp94.z = Rand_ZeroFloat(1.5f) + 1.5f;
                    Matrix_MultVec3f(&sp94, &velocity);
                    Boss03_SpawnEffectSplash(play, &eff->pos, &velocity);
                }
            }
        } else if (eff->type == GYORG_EFFECT_SPLASH) {
            eff->unk_34.z += 0.15f;

            if (eff->velocity.y < -8.0f) {
                eff->velocity.y = -8.0f;
            }

            if ((eff->velocity.y < 0.0f) && (eff->pos.y <= PLATFORM_HEIGHT)) {
                eff->type = GYORG_EFFECT_WET_SPOT;
                eff->pos.y = PLATFORM_HEIGHT;
                eff->unk_34.x = 0.05f;
                eff->unk_34.y = 0.2f;
                eff->velocity = gZeroVec3f;
                eff->accel = gZeroVec3f;
                eff->alpha = 150;
                eff->alphaDelta = Rand_ZeroFloat(4.0f) + 5.0f;
            }
        } else if (eff->type == GYORG_EFFECT_WET_SPOT) {
            Math_ApproachF(&eff->unk_34.x, eff->unk_34.y, 0.1f, 0.6f);

            eff->alpha -= eff->alphaDelta;
            if (eff->alpha <= 0) {
                eff->alpha = 0;
                eff->type = GYORG_EFFECT_NONE;
            }
        } else if (eff->type == GYORG_EFFECT_BUBBLE) {
            if (eff->velocity.y > 5.0f) {
                eff->velocity.y = 5.0f;
            }
            if (sGyorgBossInstance->waterHeight < eff->pos.y) {
                eff->type = GYORG_EFFECT_NONE;
            }
        }
    }
}

void Boss03_DrawEffects(PlayState* play) {
    u8 flag = false;
    s16 i;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 pad;
    GyorgEffect* eff = play->specialEffects;
    GyorgEffect* effFirst = eff;

    OPEN_DISPS(gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Gfx_SetupDL25_Opa(gfxCtx);

    for (i = 0; i < GYORG_EFFECT_COUNT; i++, eff++) {
        if (eff->type == GYORG_EFFECT_BUBBLE) {
            FrameInterpolation_RecordOpenChild(eff, eff->type);
            FrameInterpolation_IgnoreActorMtx();
            if (!flag) {
                gSPDisplayList(POLY_OPA_DISP++, gGyorgBubbleMaterialDL);
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
                gDPSetEnvColor(POLY_OPA_DISP++, 150, 150, 150, 0);

                flag = true;
            }

            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_Scale(eff->unk_34.x, eff->unk_34.x, 1.0f, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&play->billboardMtxF);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gGyorgBubbleModelDL);
            FrameInterpolation_RecordCloseChild();
        }
    }

    eff = effFirst;

    Boss03_SetObject(play, OBJECT_WATER_EFFECT);

    flag = false;

    for (i = 0; i < GYORG_EFFECT_COUNT; i++, eff++) {
        if ((eff->type == GYORG_EFFECT_DROPLET) || (eff->type == GYORG_EFFECT_SPLASH)) {
            FrameInterpolation_RecordOpenChild(eff, eff->type);
            FrameInterpolation_IgnoreActorMtx();
            if (!flag) {
                POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_0);

                gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(gEffDust1Tex));
                gSPDisplayList(POLY_XLU_DISP++, object_water_effect_DL_004260);
                gDPSetEnvColor(POLY_XLU_DISP++, 250, 250, 255, 0);

                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, (u8)eff->unk_40, (u8)((((void)0, eff->unk_40) + 55.0f)), 225, 150);

            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);

            if (eff->type == GYORG_EFFECT_DROPLET) {
                Matrix_RotateYF(BINANG_TO_RAD(Camera_GetInputDirYaw(GET_ACTIVE_CAM(play))), MTXMODE_APPLY);
            } else { // GYORG_EFFECT_SPLASH
                Matrix_ReplaceRotation(&play->billboardMtxF);
            }

            Matrix_Scale(eff->unk_34.x, eff->unk_34.y, 1.0f, MTXMODE_APPLY);
            Matrix_RotateZF(eff->unk_34.z, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, object_water_effect_DL_0042B0);
            FrameInterpolation_RecordCloseChild();
        }
    }

    eff = effFirst;

    flag = false;

    for (i = 0; i < GYORG_EFFECT_COUNT; i++, eff++) {
        if (eff->type == GYORG_EFFECT_WET_SPOT) {
            FrameInterpolation_RecordOpenChild(eff, eff->type);
            FrameInterpolation_IgnoreActorMtx();
            if (!flag) {
                Gfx_SetupDL44_Xlu(gfxCtx);

                gSPSegment(POLY_XLU_DISP++, 0x08, Lib_SegmentedToVirtual(gEffDust1Tex));
                gDPSetEnvColor(POLY_XLU_DISP++, 250, 250, 255, 0);
                gSPDisplayList(POLY_XLU_DISP++, object_water_effect_DL_004260);

                flag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, TRUNCF_BINANG(eff->unk_40),
                            ((void)0, TRUNCF_BINANG(eff->unk_40) + 55), 225, eff->alpha);

            Matrix_Translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);

            Matrix_Scale(eff->unk_34.x, 1.0f, eff->unk_34.x, MTXMODE_APPLY);
            Matrix_RotateYF(eff->unk_34.z, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, object_water_effect_DL_0042F8);
            FrameInterpolation_RecordCloseChild();
        }
    }

    Boss03_SetObject(play, OBJECT_BOSS03);

    CLOSE_DISPS(gfxCtx);
}

/* End of Effect Update/Draw section */

/* Start of Seaweed section */

#define SEAWEED_FLAG_INTERACT_NONE 0
#define SEAWEED_FLAG_INTERACT_PLAYER 1
#define SEAWEED_FLAG_INTERACT_GYORG 2

void Boss03_SeaweedUpdate(Actor* thisx, PlayState* play) {
    Boss03* this = (Boss03*)thisx;
    s16 i;
    s16 pad;
    s16 maxBendSpeed;
    u8 flag;
    Player* player = GET_PLAYER(play);
    f32 distanceBetweenSeaweedAndDisturbance;
    f32 disturbanceFactor;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;

    flag = SEAWEED_FLAG_INTERACT_NONE;
    this->unk_240++;

    for (i = 0; i < ARRAY_COUNT(this->seaweedSegmentPositions); i++) {
        xDiff = player->actor.world.pos.x - this->seaweedSegmentPositions[i].x;
        yDiff = player->actor.world.pos.y - this->seaweedSegmentPositions[i].y;
        zDiff = player->actor.world.pos.z - this->seaweedSegmentPositions[i].z;
        distanceBetweenSeaweedAndDisturbance = sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff));
        disturbanceFactor = player->actor.speed * 3.0f + 70.0f;

        // Player is standing on ground
        if (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            maxBendSpeed = 0;
        } else {
            maxBendSpeed = player->actor.speed * 16.0f;
            if (maxBendSpeed > 0x1000) {
                maxBendSpeed = 0x1000;
            } else if (maxBendSpeed < 0x100) {
                maxBendSpeed = 0x100;
            }
        }

        if (distanceBetweenSeaweedAndDisturbance < disturbanceFactor) {
            Math_ApproachS(&this->morphTable[i].x, (disturbanceFactor - distanceBetweenSeaweedAndDisturbance) * 200.0f,
                           0xA, maxBendSpeed);
            if (maxBendSpeed != 0) {
                flag |= SEAWEED_FLAG_INTERACT_PLAYER;
            }
        }
    }

    if (flag & SEAWEED_FLAG_INTERACT_PLAYER) {
        Math_ApproachS(&this->actor.shape.rot.y, Math_Atan2S_XY(zDiff, xDiff), 0x14, 0x800);
    }

    if (sGyorgBossInstance->actor.world.pos.y - 40.0f < sGyorgBossInstance->waterHeight) {
        for (i = 0; i < 6; i++) {
            xDiff = sGyorgBossInstance->actor.world.pos.x - this->seaweedSegmentPositions[i].x;
            yDiff = sGyorgBossInstance->actor.world.pos.y - this->seaweedSegmentPositions[i].y;
            zDiff = sGyorgBossInstance->actor.world.pos.z - this->seaweedSegmentPositions[i].z;

            distanceBetweenSeaweedAndDisturbance = sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff));

            if ((i == 0) && (distanceBetweenSeaweedAndDisturbance > 400.0f)) {
                break;
            }

            maxBendSpeed = sGyorgBossInstance->actor.speed * 16.0f;
            disturbanceFactor = sGyorgBossInstance->actor.speed * 5.0f + 150.0f;
            if (maxBendSpeed > 0x1000) {
                maxBendSpeed = 0x1000;
            } else if (maxBendSpeed < 0x100) {
                maxBendSpeed = 0x0100;
            }

            if (distanceBetweenSeaweedAndDisturbance < disturbanceFactor) {
                Math_ApproachS(&this->morphTable[i].x,
                               (disturbanceFactor - distanceBetweenSeaweedAndDisturbance) * 200.0f, 0xA, maxBendSpeed);
                if (maxBendSpeed != 0) {
                    flag |= SEAWEED_FLAG_INTERACT_GYORG;
                }
            }
        }

        if (flag & SEAWEED_FLAG_INTERACT_GYORG) {
            Math_ApproachS(&this->actor.shape.rot.y, Math_Atan2S_XY(zDiff, xDiff), 0x14, 0x800);
        }
    }

    if (flag == SEAWEED_FLAG_INTERACT_NONE) {
        for (i = 0; i < 6; i++) {
            Math_ApproachS(&this->morphTable[i].x, 0, 0x14, 0x80);
        }
    }
}

Gfx* sGyorgSeaweedDLs[] = {
    gGyorgSeaweedTopDL,    gGyorgSeaweedPiece5DL, gGyorgSeaweedPiece4DL,
    gGyorgSeaweedPiece3DL, gGyorgSeaweedPiece2DL, gGyorgSeaweedPiece1DL,
};

void Boss03_SeaweedDraw(Actor* thisx, PlayState* play) {
    Boss03* this = (Boss03*)thisx;
    s16 i;
    // Why 10 Mtxs? This seems to only use the first 6 elements
    Mtx* mtx = GRAPH_ALLOC(play->state.gfxCtx, 10 * sizeof(Mtx));

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0D, mtx);

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_RotateZS(0x4000, MTXMODE_APPLY);
    // The indices looks a bit random...
    Matrix_RotateYS(this->jointTable[5].x * -5.0f * 0.1f, MTXMODE_APPLY);
    Matrix_RotateXS(this->jointTable[3].y * -5.0f * 0.1f, MTXMODE_APPLY);
    Matrix_RotateZS(this->jointTable[2].z * 6.0f * 0.1f, MTXMODE_APPLY);

    for (i = 0; i < ARRAY_COUNT(sGyorgSeaweedDLs); i++, mtx++) {
        Matrix_RotateYS(this->jointTable[i].x + this->morphTable[i].x, MTXMODE_APPLY);
        Matrix_RotateXS(this->jointTable[i].y + this->morphTable[i].y, MTXMODE_APPLY);
        Matrix_RotateZS(this->jointTable[i].z + this->morphTable[i].z, MTXMODE_APPLY);

        Matrix_ToMtx(mtx);
        gSPMatrix(POLY_OPA_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPDisplayList(POLY_OPA_DISP++, sGyorgSeaweedDLs[i]);

        Matrix_MultZero(&this->seaweedSegmentPositions[i]);
        Matrix_Translate(4000.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/* End of Seaweed section */

void Boss03_Reset(void) {
    D_809E9840 = 0;
    D_809E9841 = 0;
    D_809E9842 = 0;
    sGyorgBossInstance = NULL;

    for (int i = 0; i < GYORG_EFFECT_COUNT; i++) {
        sGyorgEffects[i].type = GYORG_EFFECT_NONE;
    }
}
