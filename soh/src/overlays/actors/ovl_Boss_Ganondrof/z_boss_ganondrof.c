/*
 * File: z_boss_ganondrof.c
 * Overlay: ovl_Boss_Ganondrof
 * Description: Phantom Ganon
 */

#include "z_boss_ganondrof.h"
#include "objects/object_gnd/object_gnd.h"
#include "overlays/actors/ovl_En_fHG/z_en_fhg.h"
#include "overlays/actors/ovl_En_Fhg_Fire/z_en_fhg_fire.h"
#include "overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

typedef enum {
    /* 0 */ NOT_DEAD,
    /* 1 */ DEATH_START,
    /* 2 */ DEATH_THROES,
    /* 3 */ DEATH_WARP,
    /* 4 */ DEATH_SCREAM,
    /* 5 */ DEATH_DISINTEGRATE,
    /* 6 */ DEATH_FINISH
} BossGanondrofDeathState;

typedef enum {
    /* 0 */ THROW_NORMAL,
    /* 1 */ THROW_SLOW
} BossGanondrofThrowAction;

typedef enum {
    /* 0 */ STUNNED_FALL,
    /* 1 */ STUNNED_GROUND
} BossGanondrofStunnedAction;

typedef enum {
    /* 0 */ CHARGE_WINDUP,
    /* 1 */ CHARGE_START,
    /* 2 */ CHARGE_ATTACK,
    /* 3 */ CHARGE_FINISH
} BossGanondrofChargeAction;

typedef enum {
    /* 0 */ DEATH_SPASM,
    /* 1 */ DEATH_LIMP,
    /* 2 */ DEATH_HUNCHED
} BossGanondrofDeathAction;

void BossGanondrof_Init(Actor* thisx, PlayState* play);
void BossGanondrof_Destroy(Actor* thisx, PlayState* play);
void BossGanondrof_Update(Actor* thisx, PlayState* play);
void BossGanondrof_Draw(Actor* thisx, PlayState* play);

void BossGanondrof_SetupIntro(BossGanondrof* this, PlayState* play);
void BossGanondrof_Intro(BossGanondrof* this, PlayState* play);
void BossGanondrof_SetupPaintings(BossGanondrof* this);
void BossGanondrof_Paintings(BossGanondrof* this, PlayState* play);
void BossGanondrof_SetupNeutral(BossGanondrof* this, f32 arg1);
void BossGanondrof_Neutral(BossGanondrof* this, PlayState* play);
void BossGanondrof_SetupThrow(BossGanondrof* this, PlayState* play);
void BossGanondrof_Throw(BossGanondrof* this, PlayState* play);
void BossGanondrof_SetupBlock(BossGanondrof* this, PlayState* play);
void BossGanondrof_Block(BossGanondrof* this, PlayState* play);
void BossGanondrof_SetupReturn(BossGanondrof* this, PlayState* play);
void BossGanondrof_Return(BossGanondrof* this, PlayState* play);
void BossGanondrof_SetupCharge(BossGanondrof* this, PlayState* play);
void BossGanondrof_Charge(BossGanondrof* this, PlayState* play);
void BossGanondrof_Stunned(BossGanondrof* this, PlayState* play);
void BossGanondrof_Death(BossGanondrof* this, PlayState* play);

const ActorInit Boss_Ganondrof_InitVars = {
    ACTOR_BOSS_GANONDROF,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_GND,
    sizeof(BossGanondrof),
    (ActorFunc)BossGanondrof_Init,
    (ActorFunc)BossGanondrof_Destroy,
    (ActorFunc)BossGanondrof_Update,
    (ActorFunc)BossGanondrof_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInitBody = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0xFFCFFFFE, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 30, 90, -50, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInitSpear = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x30 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 20, 30, -20, { 0, 0, 0 } },
};

// clang-format off
static u8 sDecayMaskHigh[16 * 16] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,
    1,0,1,1,0,0,0,0,1,1,1,1,1,1,0,1,
    1,0,1,1,1,0,0,0,0,1,1,1,1,1,0,1,
    1,0,0,1,1,1,1,0,0,0,1,1,1,0,0,1,
    1,0,0,1,1,1,1,1,0,0,0,1,1,0,0,1,
    1,0,1,1,1,1,0,0,0,0,0,1,0,0,0,1,
    1,1,1,1,1,1,1,1,0,0,0,0,1,1,0,1,
    1,0,1,1,1,1,1,0,0,0,0,1,1,1,0,1,
    1,0,0,1,1,1,0,0,0,1,1,1,1,1,0,1,
    1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,
    1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,
    1,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,
    1,0,1,1,1,1,1,0,0,1,1,1,1,1,0,1,
    1,1,1,1,1,1,1,0,0,1,1,1,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

static u8 sDecayMaskLow[16 * 16] = {
    1,1,1,0,1,0,0,1,0,0,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,1,1,1,0,0,1,1,0,
    1,0,1,1,0,0,0,0,0,1,1,0,0,1,0,0,
    1,0,0,1,1,0,0,0,0,0,1,1,1,0,0,0,
    0,0,0,1,1,1,0,0,0,0,0,1,1,0,0,1,
    0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,
    1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
    1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
    1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,
    0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
    0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
    1,0,0,0,0,1,0,0,0,0,0,1,1,1,0,0,
    1,0,0,0,0,1,0,0,0,0,1,0,1,1,0,0,
    0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,
    1,0,0,0,1,0,1,0,0,0,1,1,0,0,0,1,
    1,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,
};

static u8 sDecayMaskTotal[16 * 16] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};
// clang-format on

// These are Phantom Ganon's body textures, but I don't know which is which.
static void* sLimbTex_rgba16_8x8[] = {
    gPhantomGanonLimbTex_00A800, gPhantomGanonLimbTex_00AE80, gPhantomGanonLimbTex_00AF00,
    gPhantomGanonLimbTex_00C180, gPhantomGanonLimbTex_00C400,
};
static void* sLimbTex_rgba16_16x8[] = {
    gPhantomGanonLimbTex_00B980, gPhantomGanonLimbTex_00C480, gPhantomGanonLimbTex_00BC80,
    gPhantomGanonLimbTex_00BD80, gPhantomGanonLimbTex_00C080,
};
static void* sLimbTex_rgba16_16x16[] = {
    gPhantomGanonLimbTex_00C200, gPhantomGanonLimbTex_00A000, gPhantomGanonLimbTex_00A200,
    gPhantomGanonLimbTex_00A400, gPhantomGanonLimbTex_00A600, gPhantomGanonLimbTex_00A880,
    gPhantomGanonLimbTex_00B780, gPhantomGanonLimbTex_00BA80, gPhantomGanonLimbTex_00BE80,
};
static void* sLimbTex_rgba16_16x32[] = { gPhantomGanonLimbTex_00AA80, gPhantomGanonLimbTex_00AF80 };

static void* sMouthTex_ci8_16x16[] = { gPhantomGanonMouthTex, gPhantomGanonSmileTex };

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(targetMode, 5, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, 0x2B, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 0, ICHAIN_STOP),
};

static Vec3f sAudioVec = { 0.0f, 0.0f, 50.0f };

void BossGanondrof_ClearPixels8x8(s16* texture, u8* mask, s16 index)
{
    if (mask[index]) {
        ResourceMgr_WriteTexS16ByName(texture, index / 4, 0);
    }
}

void BossGanondrof_ClearPixels16x8(s16* texture, u8* mask, s16 index) {
    if (mask[index]) {
        ResourceMgr_WriteTexS16ByName(texture, index / 2, 0);

    }
}

void BossGanondrof_ClearPixels16x16(s16* texture, u8* mask, s16 index, s16 bpp) {
    if (mask[index]) {
        ResourceMgr_WriteTexS16ByName(texture, index, 0);
    }
}

void BossGanondrof_ClearPixels32x16(s16* texture, u8* mask, s16 index) {
    if (mask[index]) {
        s16 i = (index & 0xF) + ((index & 0xF0) << 1);

        ResourceMgr_WriteTexS16ByName(texture, i + 0x10, 0);
        ResourceMgr_WriteTexS16ByName(texture, i, 0);
    }
}

void BossGanondrof_ClearPixels16x32(s16* texture, u8* mask, s16 index) {
    if (mask[index]) {
        s16 i = ((index & 0xF) * 2) + ((index & 0xF0) * 2);

        ResourceMgr_WriteTexS16ByName(texture, i + 1, 0);
        ResourceMgr_WriteTexS16ByName(texture, i, 0);
    }

}

void BossGanondrof_ClearPixels(u8* mask, s16 index) {
    s16 i;

    for (i = 0; i < 5; i++) {
        // ARRAY_COUNT can't be used here because the arrays aren't guaranteed to be the same size.
        BossGanondrof_ClearPixels8x8(SEGMENTED_TO_VIRTUAL(sLimbTex_rgba16_8x8[i]), mask, index);
        BossGanondrof_ClearPixels16x8(SEGMENTED_TO_VIRTUAL(sLimbTex_rgba16_16x8[i]), mask, index);
    }

    for (i = 0; i < ARRAY_COUNT(sLimbTex_rgba16_16x16); i++) {
        BossGanondrof_ClearPixels16x16(SEGMENTED_TO_VIRTUAL(sLimbTex_rgba16_16x16[i]), mask, index, 2);
    }

    for (i = 0; i < ARRAY_COUNT(sLimbTex_rgba16_16x32); i++) {
        BossGanondrof_ClearPixels16x32(SEGMENTED_TO_VIRTUAL(sLimbTex_rgba16_16x32[i]), mask, index);
    }

    BossGanondrof_ClearPixels32x16(SEGMENTED_TO_VIRTUAL(gPhantomGanonLimbTex_00B380), mask, index);
    BossGanondrof_ClearPixels16x32(SEGMENTED_TO_VIRTUAL(gPhantomGanonEyeTex), mask, index);
    for (i = 0; i < ARRAY_COUNT(sMouthTex_ci8_16x16); i++) {
        BossGanondrof_ClearPixels16x16(SEGMENTED_TO_VIRTUAL(sMouthTex_ci8_16x16[i]), mask, index, 1);
    }
}

void BossGanondrof_SetColliderPos(Vec3f* pos, ColliderCylinder* collider) {
    collider->dim.pos.x = pos->x;
    collider->dim.pos.y = pos->y;
    collider->dim.pos.z = pos->z;
}

void BossGanondrof_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BossGanondrof* this = (BossGanondrof*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    Actor_SetScale(&this->actor, 0.01f);
    SkelAnime_Init(play, &this->skelAnime, &gPhantomGanonSkel, &gPhantomGanonRideAnim, NULL, NULL, 0);
    if (this->actor.params < GND_FAKE_BOSS) {
        this->actor.params = GND_REAL_BOSS;
        this->actor.colChkInfo.health = 30;
        this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
        Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                  this->actor.world.pos.z, 255, 255, 255, 255);
        BossGanondrof_SetupIntro(this, play);
    } else {
        BossGanondrof_SetupPaintings(this);
    }

    Collider_InitCylinder(play, &this->colliderBody);
    Collider_InitCylinder(play, &this->colliderSpear);
    Collider_SetCylinder(play, &this->colliderBody, &this->actor, &sCylinderInitBody);
    Collider_SetCylinder(play, &this->colliderSpear, &this->actor, &sCylinderInitSpear);
    this->actor.flags &= ~ACTOR_FLAG_0;
    if (Flags_GetClear(play, play->roomCtx.curRoom.num)) {
        Actor_Kill(&this->actor);
        Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, GND_BOSSROOM_CENTER_X, GND_BOSSROOM_CENTER_Y,
                    GND_BOSSROOM_CENTER_Z, 0, 0, 0, WARP_DUNGEON_ADULT);
        Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, 200.0f + GND_BOSSROOM_CENTER_X,
                    GND_BOSSROOM_CENTER_Y, GND_BOSSROOM_CENTER_Z, 0, 0, 0, 0);
    } else {
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, this->actor.params);
    }
}

void BossGanondrof_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    BossGanondrof* this = (BossGanondrof*)thisx;

    osSyncPrintf("DT1\n");
    SkelAnime_Free(&this->skelAnime, play);
    Collider_DestroyCylinder(play, &this->colliderBody);
    Collider_DestroyCylinder(play, &this->colliderSpear);
    if (this->actor.params == GND_REAL_BOSS) {
        LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
    }

    osSyncPrintf("DT2\n");
}

void BossGanondrof_SetupIntro(BossGanondrof* this, PlayState* play) {
    Animation_PlayLoop(&this->skelAnime, &gPhantomGanonRidePoseAnim);
    this->actionFunc = BossGanondrof_Intro;
    this->work[GND_MASK_OFF] = true;
}

void BossGanondrof_Intro(BossGanondrof* this, PlayState* play) {
    s16 i;
    s32 pad;
    EnfHG* horse = (EnfHG*)this->actor.child;

    SkelAnime_Update(&this->skelAnime);
    this->actor.world.pos = horse->actor.world.pos;
    this->actor.shape.rot.y = this->actor.world.rot.y = horse->actor.world.rot.y;

    osSyncPrintf("SW %d------------------------------------------------\n", horse->bossGndSignal);

    if ((this->timers[1] != 0) && (this->timers[1] < 25)) {
        Vec3f pos;
        Vec3f vel = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };

        pos.x = this->bodyPartsPos[14].x + Rand_CenteredFloat(10.0f);
        pos.y = this->bodyPartsPos[14].y + Rand_ZeroFloat(-5.0f);
        pos.z = this->bodyPartsPos[14].z + Rand_CenteredFloat(10.0f) + 5.0f;
        accel.y = 0.03f;
        EffectSsKFire_Spawn(play, &pos, &vel, &accel, (s16)Rand_ZeroFloat(10.0f) + 5, 0);
    }

    if (this->timers[1] == 20) {
        this->work[GND_MASK_OFF] = false;
    }

    if (this->timers[1] == 30) {
        func_80078914(&sAudioVec, NA_SE_EN_FANTOM_TRANSFORM);
    }

    if (horse->bossGndSignal == FHG_LIGHTNING) {
        Animation_Change(&this->skelAnime, &gPhantomGanonMaskOnAnim, 0.5f, 0.0f,
                         Animation_GetLastFrame(&gPhantomGanonMaskOnAnim), ANIMMODE_ONCE_INTERP, 0.0f);
        this->timers[1] = 40;
    }

    if (horse->bossGndSignal == FHG_REAR) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gPhantomGanonHorseRearingAnim, -3.0f);
    }

    if (horse->bossGndSignal == FHG_RIDE) {
        Animation_MorphToLoop(&this->skelAnime, &gPhantomGanonRidePoseAnim, -13.0f);
    }

    if (horse->bossGndSignal == FHG_SPUR) {
        EnfHG* horseTemp;

        Animation_MorphToPlayOnce(&this->skelAnime, &gPhantomGanonRideSpearRaiseAnim, -7.0f);
        horseTemp = (EnfHG*)this->actor.child;
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE, this->spearTip.x,
                           this->spearTip.y, this->spearTip.z, 50, FHGFIRE_LIGHT_GREEN, 0, FHGFIRE_SPEAR_LIGHT);
        this->actor.child = &horseTemp->actor;
    }

    if (horse->bossGndSignal == FHG_FINISH) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gPhantomGanonRideSpearResetAnim, -5.0f);
    }

    switch (this->work[GND_EYE_STATE]) {
        case GND_EYESTATE_FADE:
            this->fwork[GND_EYE_ALPHA] += 40.0f;
            if (this->fwork[GND_EYE_ALPHA] >= 255.0f) {
                this->fwork[GND_EYE_ALPHA] = 255.0f;
            }
            break;

        case GND_EYESTATE_BRIGHTEN:
            this->fwork[GND_EYE_BRIGHTNESS] += 20.0f;
            if (this->fwork[GND_EYE_BRIGHTNESS] > 255.0f) {
                this->fwork[GND_EYE_BRIGHTNESS] = 255.0f;
            }
            break;
    }

    this->armRotY = Math_SinS(this->work[GND_VARIANCE_TIMER] * 0x6E8) * 0;
    this->armRotZ = Math_CosS(this->work[GND_VARIANCE_TIMER] * 0x8DC) * 300.0f;
    for (i = 0; i < 30; i++) {
        this->rideRotY[i] = Math_SinS(this->work[GND_VARIANCE_TIMER] * ((i * 50) + 0x7B0)) * 100.0f;
        this->rideRotZ[i] = Math_CosS(this->work[GND_VARIANCE_TIMER] * ((i * 50) + 0x8DC)) * 100.0f;
    }

    if (horse->bossGndSignal == FHG_START_FIGHT) {
        BossGanondrof_SetupPaintings(this);
        for (i = 0; i < 30; i++) {
            this->rideRotY[i] = this->rideRotZ[i] = 0.0f;
        }
    }

    horse->bossGndSignal = FHG_NO_SIGNAL;
}

void BossGanondrof_SetupPaintings(BossGanondrof* this) {
    Animation_MorphToLoop(&this->skelAnime, &gPhantomGanonRideAnim, -5.0f);
    this->actionFunc = BossGanondrof_Paintings;
}

void BossGanondrof_Paintings(BossGanondrof* this, PlayState* play) {
    EnfHG* horse = (EnfHG*)this->actor.child;

    osSyncPrintf("RUN 1\n");
    SkelAnime_Update(&this->skelAnime);
    osSyncPrintf("RUN 2\n");

    if (horse->bossGndSignal == FHG_RAISE_SPEAR) {
        EnfHG* horseTemp;

        Animation_MorphToPlayOnce(&this->skelAnime, &gPhantomGanonRideSpearRaiseAnim, -2.0f);
        this->actor.flags |= ACTOR_FLAG_0;
        horseTemp = (EnfHG*)this->actor.child;
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE, this->spearTip.x,
                           this->spearTip.y, this->spearTip.z, 30, FHGFIRE_LIGHT_GREEN, 0, FHGFIRE_SPEAR_LIGHT);
        this->actor.child = &horseTemp->actor;
    } else if (horse->bossGndSignal == FHG_LIGHTNING) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gPhantomGanonRideSpearStrikeAnim, -2.0f);
    } else if (horse->bossGndSignal == FHG_RESET) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gPhantomGanonRideSpearResetAnim, -2.0f);
    } else if (horse->bossGndSignal == FHG_RIDE) {
        Animation_MorphToLoop(&this->skelAnime, &gPhantomGanonRideAnim, -2.0f);
        this->actor.flags &= ~ACTOR_FLAG_0;
    }

    osSyncPrintf("RUN 3\n");
    this->actor.world.pos = horse->actor.world.pos;
    this->actor.world.pos.y = horse->actor.world.pos.y;
    this->actor.shape.rot.y = this->actor.world.rot.y = horse->actor.world.rot.y;
    if (this->flyMode != GND_FLY_PAINTING) {
        BossGanondrof_SetupNeutral(this, -20.0f);
        this->timers[0] = 100;
        this->colliderBody.dim.radius = 20;
        this->colliderBody.dim.height = 60;
        this->colliderBody.dim.yShift = -33;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_LAUGH);
        this->actor.naviEnemyId = 0x1A;
    } else {
        horse->bossGndSignal = FHG_NO_SIGNAL;
        this->actor.scale.x = horse->actor.scale.x / 1.15f;
        this->actor.scale.y = horse->actor.scale.y / 1.15f;
        this->actor.scale.z = horse->actor.scale.z / 1.15f;
        osSyncPrintf("RUN 4\n");
    }
}

void BossGanondrof_SetupNeutral(BossGanondrof* this, f32 arg1) {
    Animation_MorphToLoop(&this->skelAnime, &gPhantomGanonNeutralAnim, arg1);
    this->actionFunc = BossGanondrof_Neutral;
    this->actor.flags |= ACTOR_FLAG_0;
    this->fwork[GND_FLOAT_SPEED] = 0.0f;
    this->timers[0] = (s16)(Rand_ZeroOne() * 64.0f) + 30;
}

void BossGanondrof_Neutral(BossGanondrof* this, PlayState* play) {
    f32 targetX;
    f32 targetY;
    f32 targetZ;
    Player* player = GET_PLAYER(play);
    Actor* playerx = &player->actor;
    Actor* thisx = &this->actor;
    f32 rand01;
    s16 i;

    SkelAnime_Update(&this->skelAnime);
    switch (this->flyMode) {
        case GND_FLY_NEUTRAL:
            if (this->timers[0] == 0) {
                this->timers[0] = (s16)(Rand_ZeroOne() * 64.0f) + 30;
                rand01 = Rand_ZeroOne();
                if (thisx->colChkInfo.health < 5) {
                    if (rand01 < 0.25f) {
                        BossGanondrof_SetupThrow(this, play);
                    } else if (rand01 >= 0.8f) {
                        this->flyMode = GND_FLY_CHARGE;
                        this->timers[0] = 60;
                        this->fwork[GND_FLOAT_SPEED] = 0.0f;
                        Audio_PlayActorSound2(thisx, NA_SE_EN_FANTOM_LAUGH);
                    } else {
                        this->flyMode = GND_FLY_VOLLEY;
                        this->timers[0] = 60;
                        this->fwork[GND_FLOAT_SPEED] = 0.0f;
                        Audio_PlayActorSound2(thisx, NA_SE_EN_FANTOM_LAUGH);
                    }
                } else if ((rand01 < 0.5f) || (this->work[GND_THROW_COUNT] < 5)) {
                    BossGanondrof_SetupThrow(this, play);
                } else {
                    this->flyMode = GND_FLY_VOLLEY;
                    this->timers[0] = 60;
                    this->fwork[GND_FLOAT_SPEED] = 0.0f;
                    Audio_PlayActorSound2(thisx, NA_SE_EN_FANTOM_LAUGH);
                }
            }

            if (this->timers[1] != 0) {
                targetX = GND_BOSSROOM_CENTER_X;
                targetZ = GND_BOSSROOM_CENTER_Z;
            } else {
                targetX = playerx->world.pos.x + (180.0f * Math_SinS(playerx->shape.rot.y));
                targetZ = playerx->world.pos.z + (180.0f * Math_CosS(playerx->shape.rot.y));
                if (sqrtf(SQ(targetX - GND_BOSSROOM_CENTER_X) + SQ(targetZ - GND_BOSSROOM_CENTER_Z)) > 280.0f) {
                    this->timers[1] = 50;
                    this->fwork[GND_FLOAT_SPEED] = 0.0f;
                }
            }

            targetY = playerx->world.pos.y + 100.0f + 0.0f;
            targetX += Math_SinS(this->work[GND_VARIANCE_TIMER] * 0x500) * 100.0f;
            targetZ += Math_CosS(this->work[GND_VARIANCE_TIMER] * 0x700) * 100.0f;
            break;
        case GND_FLY_VOLLEY:
            targetX = GND_BOSSROOM_CENTER_X - 14.0f;
            targetZ = GND_BOSSROOM_CENTER_Z + 265.0f;

            targetY = playerx->world.pos.y + 100.0f + 100.0f;
            targetX += Math_SinS(this->work[GND_VARIANCE_TIMER] * 0x500) * 100.0f;
            targetZ += Math_CosS(this->work[GND_VARIANCE_TIMER] * 0x700) * 100.0f;
            if (this->timers[0] == 0) {
                this->flyMode = GND_FLY_RETURN;
                this->returnSuccess = false;
                BossGanondrof_SetupThrow(this, play);
                this->timers[0] = 80;
            }
            break;
        case GND_FLY_RETURN:
            targetX = GND_BOSSROOM_CENTER_X - 14.0f;
            targetZ = GND_BOSSROOM_CENTER_Z + 265.0f;

            targetY = playerx->world.pos.y + 100.0f + 100.0f;
            targetX += Math_SinS(this->work[GND_VARIANCE_TIMER] * 0x500) * 50.0f;
            targetZ += Math_CosS(this->work[GND_VARIANCE_TIMER] * 0x700) * 50.0f;
            if (this->returnSuccess) {
                this->returnSuccess = false;
                BossGanondrof_SetupReturn(this, play);
                this->timers[0] = 80;
            }

            if (this->timers[0] == 0) {
                this->flyMode = GND_FLY_NEUTRAL;
            }
            break;
        case GND_FLY_CHARGE:
            targetX = GND_BOSSROOM_CENTER_X - 14.0f;
            targetZ = GND_BOSSROOM_CENTER_Z + 215.0f;

            targetY = playerx->world.pos.y + 100.0f + 50.0f;
            targetX += Math_SinS(this->work[GND_VARIANCE_TIMER] * 0x500) * 100.0f;
            targetZ += Math_CosS(this->work[GND_VARIANCE_TIMER] * 0x700) * 100.0f;
            if (this->timers[0] == 0) {
                BossGanondrof_SetupCharge(this, play);
            }
            break;
    }

    Math_ApproachF(&thisx->world.pos.x, targetX, 0.05f, this->fwork[GND_FLOAT_SPEED]);
    if (this->timers[2] != 0) {
        Math_ApproachF(&thisx->world.pos.y, targetY + 100.0f, 0.1f, 50.0f);
    } else {
        Math_ApproachF(&thisx->world.pos.y, targetY, 0.05f, 10.0f);
    }

    Math_ApproachF(&thisx->world.pos.z, targetZ, 0.05f, this->fwork[GND_FLOAT_SPEED]);
    Math_ApproachF(&this->fwork[GND_FLOAT_SPEED], 50.0f, 1.0f, 0.5f);
    thisx->velocity.x = thisx->world.pos.x - thisx->prevPos.x;
    thisx->velocity.z = thisx->world.pos.z - thisx->prevPos.z;
    thisx->world.pos.y += 2.0f * Math_SinS(this->work[GND_VARIANCE_TIMER] * 1500);
    Math_ApproachS(&thisx->shape.rot.y, thisx->yawTowardsPlayer, 5, 0xBB8);
    if ((this->work[GND_VARIANCE_TIMER] & 1) == 0) {
        Vec3f pos;
        Vec3f vel = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };

        for (i = 0; i < 3; i++) {
            pos.x = Rand_CenteredFloat(20.0f) + this->spearTip.x;
            pos.y = Rand_CenteredFloat(20.0f) + this->spearTip.y;
            pos.z = Rand_CenteredFloat(20.0f) + this->spearTip.z;
            accel.y = -0.08f;
            EffectSsFhgFlash_SpawnLightBall(play, &pos, &vel, &accel, (s16)(Rand_ZeroOne() * 80.0f) + 150,
                                            FHGFLASH_LIGHTBALL_GREEN);
        }
    }

    if (player->unk_A73 != 0) {
        BossGanondrof_SetupBlock(this, play);
    }

    Audio_PlayActorSound2(thisx, NA_SE_EN_FANTOM_FLOAT - SFX_FLAG);
}

void BossGanondrof_SetupThrow(BossGanondrof* this, PlayState* play) {
    EnfHG* horseTemp;
    s16 lightTime;

    this->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonThrowAnim);
    Animation_MorphToPlayOnce(&this->skelAnime, &gPhantomGanonThrowAnim, -5.0f);
    this->actionFunc = BossGanondrof_Throw;
    if ((Rand_ZeroOne() <= 0.1f) && (this->work[GND_THROW_COUNT] >= 10) && (this->flyMode == GND_FLY_NEUTRAL)) {
        this->work[GND_ACTION_STATE] = THROW_SLOW;
        this->work[GND_THROW_FRAME] = 1000;
        lightTime = 32;
    } else {
        this->work[GND_ACTION_STATE] = THROW_NORMAL;
        this->work[GND_THROW_FRAME] = 25;
        lightTime = 25;
    }

    horseTemp = (EnfHG*)this->actor.child;
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE, this->spearTip.x,
                       this->spearTip.y, this->spearTip.z, lightTime, FHGFIRE_LIGHT_GREEN, 0, FHGFIRE_SPEAR_LIGHT);
    this->actor.child = &horseTemp->actor;
    this->work[GND_THROW_COUNT]++;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_STICK);
}

void BossGanondrof_Throw(BossGanondrof* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    osSyncPrintf("this->fwork[GND_END_FRAME] = %d\n", (s16)this->fwork[GND_END_FRAME]);
    osSyncPrintf("this->work[GND_SHOT_FRAME] = %d\n", this->work[GND_THROW_FRAME]);
    if (Animation_OnFrame(&this->skelAnime, this->fwork[GND_END_FRAME])) {
        BossGanondrof_SetupNeutral(this, -6.0f);
    }

    if ((this->work[GND_ACTION_STATE] != THROW_NORMAL) && Animation_OnFrame(&this->skelAnime, 21.0f)) {
        this->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonThrowEndAnim);
        Animation_MorphToPlayOnce(&this->skelAnime, &gPhantomGanonThrowEndAnim, 0.0f);
        this->work[GND_THROW_FRAME] = 10;
    }

    if (Animation_OnFrame(&this->skelAnime, this->work[GND_THROW_FRAME])) {
        if (this->flyMode <= GND_FLY_NEUTRAL) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_MASIC2);
        } else {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_MASIC1);
        }

        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_VOICE);
    }

    if (Animation_OnFrame(&this->skelAnime, this->work[GND_THROW_FRAME])) {
        EnfHG* horseTemp = (EnfHG*)this->actor.child;

        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE, this->spearTip.x,
                           this->spearTip.y, this->spearTip.z, this->work[GND_ACTION_STATE], 0, 0, FHGFIRE_ENERGY_BALL);
        this->actor.child = &horseTemp->actor;
    }

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x7D0);
    this->actor.world.pos.x += this->actor.velocity.x;
    this->actor.world.pos.z += this->actor.velocity.z;
    Math_ApproachZeroF(&this->actor.velocity.x, 1.0f, 0.5f);
    Math_ApproachZeroF(&this->actor.velocity.z, 1.0f, 0.5f);
    this->actor.world.pos.y += 2.0f * Math_SinS(this->work[GND_VARIANCE_TIMER] * 1500);
}

void BossGanondrof_SetupReturn(BossGanondrof* this, PlayState* play) {
    static AnimationHeader* returnAnim[] = { &gPhantomGanonReturn1Anim, &gPhantomGanonReturn2Anim };
    s16 rand = Rand_ZeroOne() * 1.99f;

    this->fwork[GND_END_FRAME] = Animation_GetLastFrame(returnAnim[rand]);
    Animation_MorphToPlayOnce(&this->skelAnime, returnAnim[rand], 0.0f);
    this->actionFunc = BossGanondrof_Return;
}

void BossGanondrof_Return(BossGanondrof* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_VOICE);
        osSyncPrintf("VOISE               2  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        osSyncPrintf("VOISE               2  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }

    if (Animation_OnFrame(&this->skelAnime, this->fwork[GND_END_FRAME])) {
        BossGanondrof_SetupNeutral(this, 0.0f);
    }

    this->actor.world.pos.x += this->actor.velocity.x;
    this->actor.world.pos.z += this->actor.velocity.z;
    Math_ApproachZeroF(&this->actor.velocity.x, 1.0f, 0.5f);
    Math_ApproachZeroF(&this->actor.velocity.z, 1.0f, 0.5f);
    this->actor.world.pos.y += 2.0f * Math_SinS(this->work[GND_VARIANCE_TIMER] * 1500);
    if (this->returnSuccess) {
        this->returnSuccess = false;
        BossGanondrof_SetupReturn(this, play);
        this->timers[0] = 80;
    }
}

void BossGanondrof_SetupStunned(BossGanondrof* this, PlayState* play) {
    if (this->actionFunc != BossGanondrof_Stunned) {
        this->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonAirDamageAnim);
        Animation_MorphToLoop(&this->skelAnime, &gPhantomGanonAirDamageAnim, 0.0f);
        this->timers[0] = 50;
        this->shockTimer = 60;
    } else {
        this->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonGroundDamageAnim);
        Animation_MorphToLoop(&this->skelAnime, &gPhantomGanonGroundDamageAnim, 0.0f);
    }

    this->actionFunc = BossGanondrof_Stunned;
    this->work[GND_ACTION_STATE] = STUNNED_FALL;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.z = 0.0f;
}

void BossGanondrof_Stunned(BossGanondrof* this, PlayState* play) {
    osSyncPrintf("DAMAGE   .................................\n");
    SkelAnime_Update(&this->skelAnime);
    this->actor.gravity = -0.2f;
    if (this->actor.world.pos.y <= 5.0f) {
        if (this->work[GND_ACTION_STATE] == STUNNED_FALL) {
            this->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonStunnedAnim);
            Animation_MorphToLoop(&this->skelAnime, &gPhantomGanonStunnedAnim, -10.0f);
            this->work[GND_ACTION_STATE] = STUNNED_GROUND;
        }

        this->actor.velocity.y = 0.0f;
        this->actor.gravity = 0.0f;
        if (Animation_OnFrame(&this->skelAnime, this->fwork[GND_END_FRAME])) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_DAMAGE2);
        }

        this->actor.flags |= ACTOR_FLAG_10;
    }

    osSyncPrintf("TIME0 %d ********************************************\n", this->timers[0]);
    if (this->timers[0] == 0) {
        BossGanondrof_SetupNeutral(this, -5.0f);
        this->timers[0] = 30;
        this->timers[2] = 30;
        this->flyMode = GND_FLY_NEUTRAL;
        this->actor.velocity.y = 0.0f;
        this->actor.gravity = 0.0f;
    }

    Actor_MoveForward(&this->actor);
}

void BossGanondrof_SetupBlock(BossGanondrof* this, PlayState* play) {
    this->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonBlockAnim);
    Animation_MorphToLoop(&this->skelAnime, &gPhantomGanonBlockAnim, -3.0f);
    this->actionFunc = BossGanondrof_Block;
    this->timers[0] = 10;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_STICK);
}

void BossGanondrof_Block(BossGanondrof* this, PlayState* play) {
    this->colliderBody.base.colType = COLTYPE_METAL;
    SkelAnime_Update(&this->skelAnime);
    this->actor.world.pos.x += this->actor.velocity.x;
    this->actor.world.pos.z += this->actor.velocity.z;
    Math_ApproachZeroF(&this->actor.velocity.x, 1.0f, 0.5f);
    Math_ApproachZeroF(&this->actor.velocity.z, 1.0f, 0.5f);
    this->actor.world.pos.y += 2.0f * Math_SinS(this->work[GND_VARIANCE_TIMER] * 1500);
    if (this->timers[0] == 0) {
        BossGanondrof_SetupNeutral(this, -5.0f);
        this->timers[0] = 10;
        this->flyMode = GND_FLY_NEUTRAL;
    }
}

void BossGanondrof_SetupCharge(BossGanondrof* this, PlayState* play) {
    this->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonChargeWindupAnim);
    Animation_MorphToLoop(&this->skelAnime, &gPhantomGanonChargeWindupAnim, -3.0f);
    this->actionFunc = BossGanondrof_Charge;
    this->timers[0] = 20;
    this->work[GND_ACTION_STATE] = CHARGE_WINDUP;
}

void BossGanondrof_Charge(BossGanondrof* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* playerx = &player->actor;
    Actor* thisx = &this->actor;
    f32 dxCenter = thisx->world.pos.x - GND_BOSSROOM_CENTER_X;
    f32 dzCenter = thisx->world.pos.z - GND_BOSSROOM_CENTER_Z;

    this->colliderBody.base.colType = COLTYPE_METAL;
    SkelAnime_Update(&this->skelAnime);
    switch (this->work[GND_ACTION_STATE]) {
        case CHARGE_WINDUP:
            if (this->timers[0] == 218) {
                Audio_PlayActorSound2(thisx, NA_SE_EN_FANTOM_STICK);
            }

            if (this->timers[0] == 19) {
                Audio_PlayActorSound2(thisx, NA_SE_EN_FANTOM_ATTACK);
            }

            thisx->world.pos.x += thisx->velocity.x;
            thisx->world.pos.z += thisx->velocity.z;
            Math_ApproachZeroF(&thisx->velocity.x, 1.0f, 0.5f);
            Math_ApproachZeroF(&thisx->velocity.z, 1.0f, 0.5f);
            if (this->timers[0] == 0) {
                this->work[GND_ACTION_STATE] = CHARGE_START;
                this->timers[0] = 10;
                thisx->speedXZ = 0.0f;
                this->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonChargeStartAnim);
                Animation_MorphToPlayOnce(&this->skelAnime, &gPhantomGanonChargeStartAnim, 0.0f);
            }

            Math_ApproachS(&thisx->shape.rot.y, thisx->yawTowardsPlayer, 5, 0x7D0);
            break;
        case CHARGE_START:
            if (Animation_OnFrame(&this->skelAnime, this->fwork[GND_END_FRAME])) {
                this->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonChargeAnim);
                Animation_MorphToLoop(&this->skelAnime, &gPhantomGanonChargeAnim, 0.0f);
                this->work[GND_ACTION_STATE] = CHARGE_ATTACK;
            }
        case CHARGE_ATTACK:
            if (this->timers[0] != 0) {
                Vec3f vecToLink;

                Math_ApproachS(&thisx->shape.rot.y, thisx->yawTowardsPlayer, 5, 0x7D0);
                vecToLink.x = playerx->world.pos.x - thisx->world.pos.x;
                vecToLink.y = playerx->world.pos.y + 40.0f - thisx->world.pos.y;
                vecToLink.z = playerx->world.pos.z - thisx->world.pos.z;
                thisx->world.rot.y = thisx->shape.rot.y;
                thisx->world.rot.x =
                    Math_FAtan2F(vecToLink.y, sqrtf(SQ(vecToLink.x) + SQ(vecToLink.z))) * (0x8000 / M_PI);
            }

            func_8002D908(thisx);
            func_8002D7EC(thisx);
            Math_ApproachF(&thisx->speedXZ, 10.0f, 1.0f, 0.5f);
            if ((sqrtf(SQ(dxCenter) + SQ(dzCenter)) > 280.0f) || (thisx->xyzDistToPlayerSq < SQ(100.0f))) {
                this->work[GND_ACTION_STATE] = CHARGE_FINISH;
                this->timers[0] = 20;
            }
            break;
        case CHARGE_FINISH:
            thisx->gravity = 0.2f;
            Actor_MoveForward(thisx);
            osSyncPrintf("YP %f @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n", thisx->world.pos.y);
            if (thisx->world.pos.y < 5.0f) {
                thisx->world.pos.y = 5.0f;
                thisx->velocity.y = 0.0f;
            }

            if (sqrtf(SQ(dxCenter) + SQ(dzCenter)) > 280.0f) {
                Math_ApproachZeroF(&thisx->speedXZ, 1.0f, 2.0f);
                this->timers[0] = 0;
            }

            if (this->timers[0] == 0) {
                Math_ApproachZeroF(&thisx->speedXZ, 1.0f, 2.0f);
                Math_ApproachZeroF(&thisx->velocity.y, 1.0f, 2.0f);
                Math_ApproachS(&thisx->shape.rot.y, thisx->yawTowardsPlayer, 5, 0x7D0);
                if ((thisx->speedXZ <= 0.5f) && (fabsf(thisx->velocity.y) <= 0.1f)) {
                    BossGanondrof_SetupNeutral(this, -10.0f);
                    this->timers[0] = 30;
                    this->flyMode = GND_FLY_NEUTRAL;
                }
            }
            break;
    }

    if (thisx->world.pos.y > (GND_BOSSROOM_CENTER_Y + 83.0f)) {
        thisx->world.pos.y += 2.0f * Math_SinS(this->work[GND_VARIANCE_TIMER] * 1500);
    }
    {
        s16 i;
        Vec3f pos;
        Vec3f vel = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };
        Vec3f baseOffset = { 0.0f, 50.0f, 0.0f };
        Vec3f offset;

        baseOffset.y = 10.0f;
        for (i = 0; i < 10; i++) {
            Matrix_Push();
            Matrix_RotateY((thisx->shape.rot.y / (f32)0x8000) * M_PI, MTXMODE_NEW);
            Matrix_RotateX((thisx->shape.rot.x / (f32)0x8000) * M_PI, MTXMODE_APPLY);
            Matrix_RotateZ((this->work[GND_PARTICLE_ANGLE] / (f32)0x8000) * M_PI, MTXMODE_APPLY);
            Matrix_MultVec3f(&baseOffset, &offset);
            Matrix_Pop();
            pos.x = this->spearTip.x + offset.x;
            pos.y = this->spearTip.y + offset.y;
            pos.z = this->spearTip.z + offset.z;
            vel.x = (offset.x * 500.0f) / 1000.0f;
            vel.y = (offset.y * 500.0f) / 1000.0f;
            vel.z = (offset.z * 500.0f) / 1000.0f;
            accel.x = (offset.x * -50.0f) / 1000.0f;
            accel.y = (offset.y * -50.0f) / 1000.0f;
            accel.z = (offset.z * -50.0f) / 1000.0f;
            EffectSsFhgFlash_SpawnLightBall(play, &pos, &vel, &accel, 150, i % 7);
            this->work[GND_PARTICLE_ANGLE] += 0x1A5C;
        }
    }

    if (!(this->work[GND_VARIANCE_TIMER] & 7)) {
        EnfHG* horse = (EnfHG*)thisx->child;

        Actor_SpawnAsChild(&play->actorCtx, thisx, play, ACTOR_EN_FHG_FIRE, this->spearTip.x,
                           this->spearTip.y, this->spearTip.z, 8, FHGFIRE_LIGHT_BLUE, 0, FHGFIRE_SPEAR_LIGHT);
        thisx->child = &horse->actor;
    }
}

void BossGanondrof_SetupDeath(BossGanondrof* this, PlayState* play) {
    Animation_PlayOnce(&this->skelAnime, &gPhantomGanonDeathBlowAnim);
    this->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonDeathBlowAnim);
    this->actionFunc = BossGanondrof_Death;
    Audio_QueueSeqCmd(0x1 << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0x100FF);
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_DEAD);
    this->deathState = DEATH_START;
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->work[GND_VARIANCE_TIMER] = 0;
    this->shockTimer = 50;
}

void BossGanondrof_Death(BossGanondrof* this, PlayState* play) {
    u8 holdCamera = false;
    u8 bodyDecayLevel = 0;
    f32 camX;
    f32 camZ;
    f32 pad;
    Player* player = GET_PLAYER(play);
    Camera* camera = Play_GetCamera(play, 0);

    osSyncPrintf("PYP %f\n", player->actor.floorHeight);
    SkelAnime_Update(&this->skelAnime);
    this->work[GND_DEATH_SFX_TIMER]++;
    if (((60 < this->work[GND_DEATH_SFX_TIMER]) && (this->work[GND_DEATH_SFX_TIMER] < 500)) ||
        ((501 < this->work[GND_DEATH_SFX_TIMER]) && (this->work[GND_DEATH_SFX_TIMER] < 620))) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_LAST - SFX_FLAG);
    }

    switch (this->deathState) {
        case DEATH_START:
            func_80064520(play, &play->csCtx);
            func_8002DF54(play, &this->actor, 1);
            this->deathCamera = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
            osSyncPrintf("7\n");
            Play_ChangeCameraStatus(play, this->deathCamera, CAM_STAT_ACTIVE);
            osSyncPrintf("8\n");
            this->deathState = DEATH_THROES;
            player->actor.speedXZ = 0.0f;
            this->timers[0] = 50;
            this->cameraEye = camera->eye;
            this->cameraAt = camera->at;
            this->cameraNextEye.x = this->targetPos.x;
            this->cameraNextEye.y = GND_BOSSROOM_CENTER_Y + 83.0f;
            this->cameraNextEye.z = (this->targetPos.z + 100.0f) + 50;
            this->cameraNextAt.x = this->targetPos.x;
            this->cameraNextAt.y = this->targetPos.y - 10.0f;
            this->cameraNextAt.z = this->targetPos.z;
            this->cameraEyeVel.x = fabsf(camera->eye.x - this->cameraNextEye.x);
            this->cameraEyeVel.y = fabsf(camera->eye.y - this->cameraNextEye.y);
            this->cameraEyeVel.z = fabsf(camera->eye.z - this->cameraNextEye.z);
            this->cameraAtVel.x = fabsf(camera->at.x - this->cameraNextAt.x);
            this->cameraAtVel.y = fabsf(camera->at.y - this->cameraNextAt.y);
            this->cameraAtVel.z = fabsf(camera->at.z - this->cameraNextAt.z);
            this->cameraAccel = 0.02f;
            this->cameraEyeMaxVel.x = this->cameraEyeMaxVel.y = this->cameraEyeMaxVel.z = 0.05f;
            this->work[GND_ACTION_STATE] = DEATH_SPASM;
            this->timers[0] = 150;
            this->cameraAtMaxVel.x = 0.2f;
            this->cameraAtMaxVel.y = 0.2f;
            this->cameraAtMaxVel.z = 0.2f;
        case DEATH_THROES:
            switch (this->work[GND_ACTION_STATE]) {
                case DEATH_SPASM:
                    if (Animation_OnFrame(&this->skelAnime, this->fwork[GND_END_FRAME]) && !gSaveContext.n64ddFlag) {
                        this->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonAirDamageAnim);
                        Animation_Change(&this->skelAnime, &gPhantomGanonAirDamageAnim, 0.5f, 0.0f,
                                            this->fwork[GND_END_FRAME], ANIMMODE_ONCE_INTERP, 0.0f);
                        this->work[GND_ACTION_STATE] = DEATH_LIMP;
                    } else if (gSaveContext.n64ddFlag) {
                        // Skip to death scream animation and move ganondrof to middle
                        this->deathState = DEATH_SCREAM;
                        this->timers[0] = 50;
                        Animation_MorphToLoop(&this->skelAnime, &gPhantomGanonScreamAnim, -10.0f);
                        this->actor.world.pos.x = GND_BOSSROOM_CENTER_X;
                        this->actor.world.pos.y = GND_BOSSROOM_CENTER_Y + 83.0f;
                        this->actor.world.pos.z = GND_BOSSROOM_CENTER_Z;
                        this->actor.shape.rot.y = 0;
                        this->work[GND_BODY_DECAY_INDEX] = 0;
                        Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_LAST);

                        // Move Player out of the center of the room
                        player->actor.world.pos.x = GND_BOSSROOM_CENTER_X - 200.0f;
                        player->actor.world.pos.z = GND_BOSSROOM_CENTER_Z;
                    }
                    break;
                case DEATH_LIMP:
                    if (Animation_OnFrame(&this->skelAnime, this->fwork[GND_END_FRAME])) {
                        this->fwork[GND_END_FRAME] = Animation_GetLastFrame(&gPhantomGanonLimpAnim);
                        Animation_MorphToLoop(&this->skelAnime, &gPhantomGanonLimpAnim, -20.0f);
                        this->work[GND_ACTION_STATE] = DEATH_HUNCHED;
                    }
                case DEATH_HUNCHED:
                    bodyDecayLevel = 1;
                    break;
            }
            if (gSaveContext.n64ddFlag) {
                break;
            }
            Math_ApproachS(&this->actor.shape.rot.y, this->work[GND_VARIANCE_TIMER] * -100, 5, 0xBB8);
            Math_ApproachF(&this->cameraNextEye.z, this->targetPos.z + 60.0f, 0.02f, 0.5f);
            Math_ApproachF(&this->actor.world.pos.y, GND_BOSSROOM_CENTER_Y + 133.0f, 0.05f, 100.0f);
            this->actor.world.pos.y += Math_SinS(this->work[GND_VARIANCE_TIMER] * 1500);
            this->cameraNextAt.x = this->targetPos.x;
            this->cameraNextAt.y = this->targetPos.y - 10.0f;
            this->cameraNextAt.z = this->targetPos.z;
            if (this->timers[0] == 0) {
                this->deathState = DEATH_WARP;
                this->timers[0] = 350;
                this->timers[1] = 50;
                this->fwork[GND_CAMERA_ZOOM] = 300.0f;
                this->cameraNextEye.y = GND_BOSSROOM_CENTER_Y + 233.0f;
                player->actor.world.pos.x = GND_BOSSROOM_CENTER_X - 200.0f;
                player->actor.world.pos.z = GND_BOSSROOM_CENTER_Z;
                holdCamera = true;
                bodyDecayLevel = 1;
            }
            break;
        case DEATH_WARP:
            if (this->timers[1] == 1) {
                EnfHG* horseTemp = (EnfHG*)this->actor.child;

                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE,
                                   GND_BOSSROOM_CENTER_X, GND_BOSSROOM_CENTER_Y + 3.0f, GND_BOSSROOM_CENTER_Z, 0x4000,
                                   0, 0, FHGFIRE_WARP_DEATH);
                this->actor.child = &horseTemp->actor;
                Message_StartTextbox(play, 0x108E, NULL);
            }

            this->actor.shape.rot.y -= 0xC8;
            this->actor.world.pos.y += Math_SinS(this->work[GND_VARIANCE_TIMER] * 1500);
            this->fwork[GND_CAMERA_ANGLE] += 0x78;
            camX = Math_SinS(this->fwork[GND_CAMERA_ANGLE]) * this->fwork[GND_CAMERA_ZOOM];
            camZ = Math_CosS(this->fwork[GND_CAMERA_ANGLE]) * this->fwork[GND_CAMERA_ZOOM];
            this->cameraEye.x = GND_BOSSROOM_CENTER_X + camX;
            this->cameraEye.y = this->cameraNextEye.y;
            this->cameraEye.z = GND_BOSSROOM_CENTER_Z + camZ;
            this->cameraAt.x = GND_BOSSROOM_CENTER_X;
            this->cameraAt.y = GND_BOSSROOM_CENTER_Y + 23.0f;
            this->cameraAt.z = GND_BOSSROOM_CENTER_Z;
            Math_ApproachF(&this->cameraNextEye.y, GND_BOSSROOM_CENTER_Y + 33.0f, 0.05f, 0.5f);
            Math_ApproachF(&this->fwork[GND_CAMERA_ZOOM], 170.0f, 0.05f, 1.0f);
            Math_ApproachF(&this->actor.world.pos.x, GND_BOSSROOM_CENTER_X, 0.05f, 1.5f);
            Math_ApproachF(&this->actor.world.pos.y, GND_BOSSROOM_CENTER_Y + 83.0f, 0.05f, 1.0f);
            Math_ApproachF(&this->actor.world.pos.z, GND_BOSSROOM_CENTER_Z, 0.05f, 1.5f);
            if (this->timers[0] == 0) {
                this->deathState = DEATH_SCREAM;
                this->timers[0] = 50;
                Animation_MorphToLoop(&this->skelAnime, &gPhantomGanonScreamAnim, -10.0f);
                this->actor.world.pos.x = GND_BOSSROOM_CENTER_X;
                this->actor.world.pos.y = GND_BOSSROOM_CENTER_Y + 83.0f;
                this->actor.world.pos.z = GND_BOSSROOM_CENTER_Z;
                this->actor.shape.rot.y = 0;
                this->work[GND_BODY_DECAY_INDEX] = 0;
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_LAST);
            }

            holdCamera = true;
            bodyDecayLevel = 1;
            break;
        case DEATH_SCREAM:
            holdCamera = true;
            bodyDecayLevel = 2;
            this->actor.world.pos.y = GND_BOSSROOM_CENTER_Y + 83.0f;
            this->cameraEye.x = GND_BOSSROOM_CENTER_X;
            this->cameraEye.y = GND_BOSSROOM_CENTER_Y + 83.0f;
            this->cameraEye.z = GND_BOSSROOM_CENTER_Z + 50.0f;
            this->cameraAt.x = GND_BOSSROOM_CENTER_X;
            this->cameraAt.y = GND_BOSSROOM_CENTER_Y + 103.0f;
            this->cameraAt.z = GND_BOSSROOM_CENTER_Z;
            if (this->timers[0] == 0) {
                this->deathState = DEATH_DISINTEGRATE;
                Animation_MorphToPlayOnce(&this->skelAnime, &gPhantomGanonLastPoseAnim, -10.0f);
                this->work[GND_BODY_DECAY_INDEX] = 0;
                this->timers[0] = 40;
            }
            break;
        case DEATH_DISINTEGRATE:
            holdCamera = true;
            bodyDecayLevel = 3;
            Math_ApproachZeroF(&this->cameraEye.y, 0.05f, 1.0f); // approaches GND_BOSSROOM_CENTER_Y + 33.0f
            Math_ApproachF(&this->cameraEye.z, GND_BOSSROOM_CENTER_Z + 170.0f, 0.05f, 2.0f);
            Math_ApproachF(&this->cameraAt.y, GND_BOSSROOM_CENTER_Y + 53.0f, 0.05f, 1.0f);
            if (this->timers[0] == 0) {
                this->timers[0] = 250;
                this->deathState = DEATH_FINISH;
            }
            break;
        case DEATH_FINISH:
            holdCamera = true;
            bodyDecayLevel = 10;
            if (this->timers[0] == 150) {
                Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS_CLEAR);
                Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, GND_BOSSROOM_CENTER_X,
                            GND_BOSSROOM_CENTER_Y, GND_BOSSROOM_CENTER_Z, 0, 0, 0, WARP_DUNGEON_ADULT);
            }

            Math_ApproachZeroF(&this->cameraEye.y, 0.05f, 1.0f); // GND_BOSSROOM_CENTER_Y + 33.0f
            Math_ApproachF(&this->cameraEye.z, GND_BOSSROOM_CENTER_Z + 170.0f, 0.05f, 2.0f);
            Math_ApproachF(&this->cameraAt.y, GND_BOSSROOM_CENTER_Y + 53.0f, 0.05f, 1.0f);
            if (this->timers[0] == 0) {
                EnfHG* horse = (EnfHG*)this->actor.child;

                camera->eye = this->cameraEye;
                camera->eyeNext = this->cameraEye;
                camera->at = this->cameraAt;
                func_800C08AC(play, this->deathCamera, 0);
                this->deathCamera = 0;
                func_80064534(play, &play->csCtx);
                func_8002DF54(play, &this->actor, 7);
                Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, GND_BOSSROOM_CENTER_X,
                            GND_BOSSROOM_CENTER_Y, GND_BOSSROOM_CENTER_Z + 200.0f, 0, 0, 0, 0);
                this->actor.child = &horse->actor;
                this->killActor = true;
                horse->killActor = true;
                Flags_SetClear(play, play->roomCtx.curRoom.num);
                Flags_SetSwitch(play, 0x22);
            }
            break;
    }

    if (bodyDecayLevel) {
        Vec3f pos;
        Vec3f vel = { 0.0f, 0.0f, 0.0f };
        Vec3f accelKFire = { 0.0f, 0.0f, 0.0f };
        Vec3f accelHahen = { 0.0f, -0.5f, 0.0f };
        s16 limbDecayIndex;
        s16 i;

        vel.x = this->actor.world.pos.x - this->actor.prevPos.x;
        vel.z = this->actor.world.pos.z - this->actor.prevPos.z;
        if (bodyDecayLevel < 10) {
            if (this->work[GND_DEATH_ENV_TIMER] == 0) {
                if (play->envCtx.unk_BF == 0) {
                    play->envCtx.unk_BF = 3;
                    this->work[GND_DEATH_ENV_TIMER] = (s16)Rand_ZeroFloat(5.0f) + 4.0f;
                    play->envCtx.unk_D6 = 0x28;
                } else {
                    play->envCtx.unk_BF = 0;
                    this->work[GND_DEATH_ENV_TIMER] = (s16)Rand_ZeroFloat(2.0f) + 2.0f;
                    play->envCtx.unk_D6 = 0x14;
                }
            } else {
                this->work[GND_DEATH_ENV_TIMER]--;
            }

            for (i = 0; i <= 0; i++) {
                limbDecayIndex = this->work[GND_LIMB_DECAY_INDEX];
                this->work[GND_LIMB_DECAY_INDEX]++;
                this->work[GND_LIMB_DECAY_INDEX] %= 25;
                pos.x = this->bodyPartsPos[limbDecayIndex].x + Rand_CenteredFloat(5.0f);
                pos.y = this->bodyPartsPos[limbDecayIndex].y + Rand_CenteredFloat(5.0f);
                pos.z = this->bodyPartsPos[limbDecayIndex].z + Rand_CenteredFloat(5.0f);
                accelKFire.y = 0.0f;

                if (bodyDecayLevel == 3) {
                    accelKFire.y = -0.2f;
                    accelKFire.x = (GND_BOSSROOM_CENTER_X - pos.x) * 0.002f;
                    accelKFire.z = (GND_BOSSROOM_CENTER_Z - pos.z) * 0.002f;
                    accelHahen.x = (GND_BOSSROOM_CENTER_X - pos.x) * 0.001f;
                    accelHahen.y = -1.0f;
                    accelHahen.z = (GND_BOSSROOM_CENTER_Z - pos.z) * 0.001f;
                }

                EffectSsKFire_Spawn(play, &pos, &vel, &accelKFire, (s16)Rand_ZeroFloat(20.0f) + 15,
                                    bodyDecayLevel);
                if ((Rand_ZeroOne() < 0.5f) || (bodyDecayLevel == 3)) {
                    EffectSsHahen_Spawn(play, &pos, &vel, &accelHahen, 0, (s16)Rand_ZeroFloat(4.0f) + 7,
                                        HAHEN_OBJECT_DEFAULT, 10, NULL);
                }
            }
        } else {
            play->envCtx.unk_BF = 0;
            play->envCtx.unk_D6 = 0x14;
        }

        this->work[GND_BODY_DECAY_FLAG] = true;
        for (i = 0; i < 5; i++) {
            if (bodyDecayLevel == 1) {
                BossGanondrof_ClearPixels(sDecayMaskLow, this->work[GND_BODY_DECAY_INDEX]);
            } else if (bodyDecayLevel == 2) {
                BossGanondrof_ClearPixels(sDecayMaskHigh, this->work[GND_BODY_DECAY_INDEX]);
            } else {
                BossGanondrof_ClearPixels(sDecayMaskTotal, this->work[GND_BODY_DECAY_INDEX]);
            }

            if (this->work[GND_BODY_DECAY_INDEX] < 0xFF) {
                this->work[GND_BODY_DECAY_INDEX]++;
            }
        }
    }

    if (this->deathCamera != 0) {
        if (!holdCamera) {
            Math_ApproachF(&this->cameraEye.x, this->cameraNextEye.x, this->cameraEyeMaxVel.x,
                           this->cameraEyeVel.x * this->cameraSpeedMod);
            Math_ApproachF(&this->cameraEye.y, this->cameraNextEye.y, this->cameraEyeMaxVel.y,
                           this->cameraEyeVel.y * this->cameraSpeedMod);
            Math_ApproachF(&this->cameraEye.z, this->cameraNextEye.z, this->cameraEyeMaxVel.z,
                           this->cameraEyeVel.z * this->cameraSpeedMod);
            Math_ApproachF(&this->cameraAt.x, this->cameraNextAt.x, this->cameraAtMaxVel.x,
                           this->cameraAtVel.x * this->cameraSpeedMod);
            Math_ApproachF(&this->cameraAt.y, this->cameraNextAt.y, this->cameraAtMaxVel.y,
                           this->cameraAtVel.y * this->cameraSpeedMod);
            Math_ApproachF(&this->cameraAt.z, this->cameraNextAt.z, this->cameraAtMaxVel.z,
                           this->cameraAtVel.z * this->cameraSpeedMod);
            Math_ApproachF(&this->cameraSpeedMod, 1.0f, 1.0f, this->cameraAccel);
        }

        Play_CameraSetAtEye(play, this->deathCamera, &this->cameraAt, &this->cameraEye);
    }
}

void BossGanondrof_CollisionCheck(BossGanondrof* this, PlayState* play) {
    s32 acHit;
    EnfHG* horse = (EnfHG*)this->actor.child;
    ColliderInfo* hurtbox;

    if (this->work[GND_INVINC_TIMER] != 0) {
        this->work[GND_INVINC_TIMER]--;
        this->returnCount = 0;
        this->colliderBody.base.acFlags &= ~AC_HIT;
    } else {
        acHit = this->colliderBody.base.acFlags & AC_HIT;
        if ((acHit && ((s8)this->actor.colChkInfo.health > 0)) || (this->returnCount != 0)) {
            if (acHit) {
                this->colliderBody.base.acFlags &= ~AC_HIT;
                hurtbox = this->colliderBody.info.acHitInfo;
            }
            if (this->flyMode != GND_FLY_PAINTING) {
                if (acHit && (this->actionFunc != BossGanondrof_Stunned) && (hurtbox->toucher.dmgFlags & 0x0001F8A4)) {
                    Audio_PlayActorSound2(&this->actor, NA_SE_PL_WALK_GROUND - SFX_FLAG);
                    osSyncPrintf("hit != 0 \n");
                } else if (this->actionFunc != BossGanondrof_Charge) {
                    if (this->returnCount == 0) {
                        u8 dmg;
                        u8 canKill = false;
                        s32 dmgFlags = hurtbox->toucher.dmgFlags;

                        if (dmgFlags & 0x80) {
                            return;
                        }
                        dmg = CollisionCheck_GetSwordDamage(dmgFlags);
                        (dmg == 0) ? (dmg = 2) : (canKill = true);
                        if (((s8)this->actor.colChkInfo.health > 2) || canKill) {
                            this->actor.colChkInfo.health -= dmg;
                        }

                        if ((s8)this->actor.colChkInfo.health <= 0) {
                            BossGanondrof_SetupDeath(this, play);
                            Enemy_StartFinishingBlow(play, &this->actor);
                            gSaveContext.sohStats.timestamp[TIMESTAMP_DEFEAT_PHANTOM_GANON] = GAMEPLAYSTAT_TOTAL_TIME;
                            return;
                        }
                    }
                    BossGanondrof_SetupStunned(this, play);
                    if (this->returnCount >= 2) {
                        this->timers[0] = 120;
                    }
                    this->work[GND_INVINC_TIMER] = 10;
                    horse->hitTimer = 20;
                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_DAMAGE);
                } else {
                    Audio_PlayActorSound2(&this->actor, NA_SE_PL_WALK_GROUND - SFX_FLAG);
                }
            } else if (acHit && (hurtbox->toucher.dmgFlags & 0x0001F8A4)) {
                this->work[GND_INVINC_TIMER] = 10;
                this->actor.colChkInfo.health -= 2;
                horse->hitTimer = 20;
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_DAMAGE);
            }
            this->returnCount = 0;
        }
    }
}

void BossGanondrof_Update(Actor* thisx, PlayState* play) {
    f32 cs;
    f32 sn;
    f32 legRotTargetY;
    f32 legRotTargetZ;
    f32 legSplitTarget;
    s32 pad2;
    s16 i;
    s32 pad;
    BossGanondrof* this = (BossGanondrof*)thisx;
    EnfHG* horse;

    osSyncPrintf("MOVE START %d\n", this->actor.params);
    this->actor.flags &= ~ACTOR_FLAG_10;
    this->colliderBody.base.colType = COLTYPE_HIT3;
    if (this->killActor) {
        Actor_Kill(&this->actor);
        return;
    }
    this->work[GND_VARIANCE_TIMER]++;
    horse = (EnfHG*)this->actor.child;
    osSyncPrintf("MOVE START EEEEEEEEEEEEEEEEEEEEEE%d\n", this->actor.params);

    this->actionFunc(this, play);

    for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
        if (this->timers[i]) {
            this->timers[i]--;
        }
    }
    if (this->work[GND_UNKTIMER_1]) {
        this->work[GND_UNKTIMER_1]--;
    }
    if (this->work[GND_UNKTIMER_2]) {
        this->work[GND_UNKTIMER_2]--;
    }

    if (this->actionFunc != BossGanondrof_Death) {
        BossGanondrof_CollisionCheck(this, play);
    }

    osSyncPrintf("MOVE END\n");
    BossGanondrof_SetColliderPos(&this->targetPos, &this->colliderBody);
    BossGanondrof_SetColliderPos(&this->spearTip, &this->colliderSpear);
    if ((this->flyMode == GND_FLY_PAINTING) && !horse->bossGndInPainting) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderBody.base);
    }
    if ((this->actionFunc == BossGanondrof_Stunned) && (this->timers[0] > 1)) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderBody.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderBody.base);
    } else if (this->actionFunc == BossGanondrof_Block) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderBody.base);
    } else if (this->actionFunc == BossGanondrof_Charge) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderBody.base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderBody.base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderSpear.base);
    }

    this->actor.focus.pos = this->targetPos;

    sn = Math_SinS(-this->actor.shape.rot.y);
    cs = Math_CosS(-this->actor.shape.rot.y);
    legRotTargetY = ((sn * this->actor.velocity.z) + (cs * this->actor.velocity.x)) * 300.0f;
    legRotTargetZ = ((-sn * this->actor.velocity.x) + (cs * this->actor.velocity.z)) * 300.0f;
    Math_ApproachF(&this->legRotY, legRotTargetY, 1.0f, 600.0f);
    Math_ApproachF(&this->legRotZ, legRotTargetZ, 1.0f, 600.0f);
    if ((this->flyMode != GND_FLY_PAINTING) && (this->actionFunc != BossGanondrof_Stunned) &&
        (this->deathState == NOT_DEAD)) {
        legSplitTarget = (Math_SinS(this->work[GND_VARIANCE_TIMER] * 0x8DC) * -500.0f) - 500.0f;
    } else {
        legSplitTarget = 0.0f;
    }

    Math_ApproachF(&this->legSplitY, legSplitTarget, 1.0f, 100.0f);
    if (this->shockTimer != 0) {
        s16 j;

        this->shockTimer--;
        osSyncPrintf("F 1\n");
        for (j = 0; j < 7; j++) {
            osSyncPrintf("F 15\n");
            EffectSsFhgFlash_SpawnShock(play, &this->actor, &this->actor.world.pos, 45, FHGFLASH_SHOCK_PG);
        }
        osSyncPrintf("F 2\n");
    }

    if (this->actor.params == GND_REAL_BOSS) {
        Lights_PointNoGlowSetInfo(&this->lightInfo, this->spearTip.x, this->spearTip.y, this->spearTip.z, 255, 255, 255,
                                  200);
    }
}

s32 BossGanondrof_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                   void* thisx) {
    BossGanondrof* this = (BossGanondrof*)thisx;

    switch (limbIndex) {
        case 15:
            if ((this->actionFunc == BossGanondrof_Intro) && this->work[GND_MASK_OFF]) {
                *dList = gPhantomGanonFaceDL;
            }
            rot->y += this->rideRotY[limbIndex];
            rot->z += this->rideRotZ[limbIndex];
            break;

        case 19:
            rot->y += this->legRotY + this->legSplitY;
            rot->z += this->legRotZ;
            break;

        case 20:
            rot->y += this->legRotY + this->legSplitY;
            rot->z += this->legRotZ;
            break;

        case 21:
            rot->y += this->legRotY + this->legSplitY;
            rot->z += this->legRotZ;
            break;

        case 23:
            rot->y += this->legRotY - this->legSplitY;
            rot->z += this->legRotZ;
            break;

        case 24:
            rot->y += this->legRotY - this->legSplitY;
            rot->z += this->legRotZ;
            break;

        case 25:
            rot->y += this->legRotY - this->legSplitY;
            rot->z += this->legRotZ;
            break;

        case 5:
        case 6:
        case 7:
            rot->y += this->armRotY;
            rot->z += this->armRotZ;
            break;

        case 8:
        case 9:
        case 10:
            rot->y += this->armRotY;
            rot->z += this->armRotZ;
            break;

        case 13:
            if (this->deathState != NOT_DEAD) {
                *dList = NULL;
            }
        default:
            rot->y += this->rideRotY[limbIndex];
            rot->z += this->rideRotZ[limbIndex];
            break;
    }

    return 0;
}

void BossGanondrof_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    static Vec3f spearVec = { 0.0f, 0.0f, 6000.0f };

    BossGanondrof* this = (BossGanondrof*)thisx;

    if (limbIndex == 14) {
        Matrix_MultVec3f(&zeroVec, &this->targetPos);
    } else if (limbIndex == 13) {
        Matrix_MultVec3f(&spearVec, &this->spearTip);
    }

    if (((this->flyMode != GND_FLY_PAINTING) || (this->actionFunc == BossGanondrof_Intro)) && (limbIndex <= 25)) {
        Matrix_MultVec3f(&zeroVec, &this->bodyPartsPos[limbIndex - 1]);
    }
}

Gfx* BossGanondrof_GetClearPixelDList(GraphicsContext* gfxCtx) {
    Gfx* dList = (Gfx*)Graph_Alloc(gfxCtx, sizeof(Gfx) * 4);
    Gfx* dListHead = dList;

    gDPPipeSync(dListHead++);
    gDPSetRenderMode(dListHead++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_TEX_EDGE2);
    gSPClearGeometryMode(dListHead++, G_CULL_BACK);
    gSPEndDisplayList(dListHead++);

    return dList;
}

Gfx* BossGanondrof_GetNullDList(GraphicsContext* gfxCtx) {
    Gfx* dList = (Gfx*)Graph_Alloc(gfxCtx, sizeof(Gfx) * 1);
    Gfx* dListHead = dList;

    gSPEndDisplayList(dListHead++);
    return dList;
}

void BossGanondrof_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BossGanondrof* this = (BossGanondrof*)thisx;
    EnfHG* horse;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->work[GND_BODY_DECAY_FLAG]) {
        for (int i = 0; i < 5; i++) {
            gSPInvalidateTexCache(POLY_OPA_DISP++, sLimbTex_rgba16_8x8[i]);
            gSPInvalidateTexCache(POLY_OPA_DISP++, sLimbTex_rgba16_16x8[i]);
        }
        for (int i = 0; i < ARRAY_COUNT(sLimbTex_rgba16_16x16); i++) {
            gSPInvalidateTexCache(POLY_OPA_DISP++, sLimbTex_rgba16_16x16[i]);
        }
        for (int i = 0; i < ARRAY_COUNT(sLimbTex_rgba16_16x32); i++) {
            gSPInvalidateTexCache(POLY_OPA_DISP++, sLimbTex_rgba16_16x32[i]);
        }
        gSPInvalidateTexCache(POLY_OPA_DISP++, gPhantomGanonLimbTex_00B380);
        gSPInvalidateTexCache(POLY_OPA_DISP++, gPhantomGanonEyeTex);
        for (int i = 0; i < ARRAY_COUNT(sMouthTex_ci8_16x16); i++) {
            gSPInvalidateTexCache(POLY_OPA_DISP++, sMouthTex_ci8_16x16[i]);
        }
    }

    osSyncPrintf("MOVE P = %x\n", this->actor.update);
    osSyncPrintf("STOP TIMER = %d ==============\n", this->actor.freezeTimer);
    horse = (EnfHG*)this->actor.child;
    if (this->flyMode == GND_FLY_PAINTING) {
        Matrix_RotateY((horse->turnRot * 3.1416f) / (f32)0x8000, MTXMODE_APPLY);
    }

    osSyncPrintf("YP %f\n", this->actor.world.pos.y);
    func_80093D18(play->state.gfxCtx);
    if (this->work[GND_INVINC_TIMER] & 4) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 50, 0, 0, 900, 1099);
    } else {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, (u32)horse->warpColorFilterR, (u32)horse->warpColorFilterG,
                                   (u32)horse->warpColorFilterB, 0, (s32)horse->warpColorFilterUnk1 + 995,
                                   (s32)horse->warpColorFilterUnk2 + 1000);
    }

    osSyncPrintf("DRAW 11\n");
    osSyncPrintf("EYE_COL %d\n", (s16)this->fwork[GND_EYE_BRIGHTNESS]);
    gDPSetEnvColor(POLY_OPA_DISP++, (s16)this->fwork[GND_EYE_BRIGHTNESS], (s16)this->fwork[GND_EYE_BRIGHTNESS],
                   (s16)this->fwork[GND_EYE_BRIGHTNESS], (s16)this->fwork[GND_EYE_ALPHA]);
    if (this->work[GND_BODY_DECAY_FLAG]) {
        gSPSegment(POLY_OPA_DISP++, 0x08, BossGanondrof_GetClearPixelDList(play->state.gfxCtx));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, BossGanondrof_GetNullDList(play->state.gfxCtx));
    }

    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, BossGanondrof_OverrideLimbDraw,
                      BossGanondrof_PostLimbDraw, this);
    osSyncPrintf("DRAW 22\n");
    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
    CLOSE_DISPS(play->state.gfxCtx);
    osSyncPrintf("DRAW END %d\n", this->actor.params);
}
