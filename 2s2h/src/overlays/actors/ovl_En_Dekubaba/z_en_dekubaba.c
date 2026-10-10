/*
 * File: z_en_dekubaba.c
 * Overlay: ovl_En_Dekubaba
 * Description: Deku Baba
 */

#include "z_en_dekubaba.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

void EnDekubaba_Init(Actor* thisx, PlayState* play);
void EnDekubaba_Destroy(Actor* thisx, PlayState* play);
void EnDekubaba_Update(Actor* thisx, PlayState* play);
void EnDekubaba_Draw(Actor* thisx, PlayState* play);

void EnDekubaba_SetupWait(EnDekubaba* this);
void EnDekubaba_Wait(EnDekubaba* this, PlayState* play);
void EnDekubaba_SetupGrow(EnDekubaba* this);
void EnDekubaba_Grow(EnDekubaba* this, PlayState* play);
void EnDekubaba_SetupRetract(EnDekubaba* this);
void EnDekubaba_Retract(EnDekubaba* this, PlayState* play);
void EnDekubaba_DecideLunge(EnDekubaba* this, PlayState* play);
void EnDekubaba_SetupPrepareLunge(EnDekubaba* this);
void EnDekubaba_PrepareLunge(EnDekubaba* this, PlayState* play);
void EnDekubaba_SetupLunge(EnDekubaba* this);
void EnDekubaba_Lunge(EnDekubaba* this, PlayState* play);
void EnDekubaba_SetupPullBack(EnDekubaba* this);
void EnDekubaba_PullBack(EnDekubaba* this, PlayState* play);
void EnDekubaba_Recover(EnDekubaba* this, PlayState* play);
void EnDekubaba_Hit(EnDekubaba* this, PlayState* play);
void EnDekubaba_PrunedSomersaultDie(EnDekubaba* this, PlayState* play);
void EnDekubaba_SetupShrinkDie(EnDekubaba* this);
void EnDekubaba_ShrinkDie(EnDekubaba* this, PlayState* play);
void EnDekubaba_SetupStunnedVertical(EnDekubaba* this);
void EnDekubaba_StunnedVertical(EnDekubaba* this, PlayState* play);
void EnDekubaba_Sway(EnDekubaba* this, PlayState* play);
void EnDekubaba_Frozen(EnDekubaba* this, PlayState* play);
void EnDekubaba_SetupDeadStickDrop(EnDekubaba* this, PlayState* play);
void EnDekubaba_DeadStickDrop(EnDekubaba* this, PlayState* play);

ActorProfile En_Dekubaba_Profile = {
    /**/ ACTOR_EN_DEKUBABA,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_DEKUBABA,
    /**/ sizeof(EnDekubaba),
    /**/ EnDekubaba_Init,
    /**/ EnDekubaba_Destroy,
    /**/ EnDekubaba_Update,
    /**/ EnDekubaba_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[7] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 1, { { 0, 100, 1000 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 51, { { 0, 0, 1500 }, 8 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 52, { { 0, 0, 500 }, 8 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 53, { { 0, 0, 1500 }, 8 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 54, { { 0, 0, 500 }, 8 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 55, { { 0, 0, 1500 }, 8 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 56, { { 0, 0, 500 }, 8 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 2, 25, 25, MASS_IMMOVABLE };

typedef enum {
    /* 0x0 */ DEKUBABA_DMGEFF_NONE,
    /* 0x1 */ DEKUBABA_DMGEFF_NUT,
    /* 0x2 */ DEKUBABA_DMGEFF_FIRE,
    /* 0x3 */ DEKUBABA_DMGEFF_ICE,
    /* 0x4 */ DEKUBABA_DMGEFF_LIGHT,
    /* 0x5 */ DEKUBABA_DMGEFF_ELECTRIC,
    /* 0xD */ DEKUBABA_DMGEFF_HOOKSHOT = 0xD,
    /* 0xF */ DEKUBABA_DMGEFF_CUT = 0xF
} DekuBabaDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NUT),
    /* Deku Stick     */ DMG_ENTRY(3, DEKUBABA_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(1, DEKUBABA_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, DEKUBABA_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, DEKUBABA_DMGEFF_CUT),
    /* Normal arrow   */ DMG_ENTRY(3, DEKUBABA_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, DEKUBABA_DMGEFF_HOOKSHOT),
    /* Goron punch    */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, DEKUBABA_DMGEFF_CUT),
    /* Goron pound    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(1, DEKUBABA_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(3, DEKUBABA_DMGEFF_ICE),
    /* Light arrow    */ DMG_ENTRY(3, DEKUBABA_DMGEFF_LIGHT),
    /* Goron spikes   */ DMG_ENTRY(1, DEKUBABA_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(1, DEKUBABA_DMGEFF_CUT),
    /* Deku bubble    */ DMG_ENTRY(3, DEKUBABA_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NUT),
    /* Zora barrier   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_ELECTRIC),
    /* Normal shield  */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, DEKUBABA_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, DEKUBABA_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, DEKUBABA_DMGEFF_CUT),
    /* Sword beam     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, DEKUBABA_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 1500, ICHAIN_STOP),
};

void EnDekubaba_Init(Actor* thisx, PlayState* play) {
    EnDekubaba* this = (EnDekubaba*)thisx;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 22.0f);
    SkelAnime_Init(play, &this->skelAnime, &gDekuBabaSkel, &gDekuBabaFastChompAnim, this->jointTable, this->morphTable,
                   DEKUBABA_LIMB_MAX);
    Collider_InitAndSetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    if (this->actor.params == DEKUBABA_BIG) {
        this->size = 2.5f;

        for (i = 0; i < sJntSphInit.count; i++) {
            this->collider.elements[i].dim.worldSphere.radius = this->collider.elements[i].dim.modelSphere.radius =
                (sJntSphElementsInit[i].dim.modelSphere.radius * 2.50f);
        }

        this->actor.colChkInfo.health = 4;
        // Big Deku Baba hint does not exist in MM
        this->actor.hintId = TATL_HINT_ID_BIO_DEKU_BABA;
        this->actor.attentionRangeType = ATTENTION_RANGE_2;
    } else {
        this->size = 1.0f;

        for (i = 0; i < sJntSphInit.count; i++) {
            this->collider.elements[i].dim.worldSphere.radius = this->collider.elements[i].dim.modelSphere.radius;
        }
        this->actor.hintId = TATL_HINT_ID_DEKU_BABA;
        this->actor.attentionRangeType = ATTENTION_RANGE_1;
    }

    EnDekubaba_SetupWait(this);
    this->timer = 0;
    this->boundFloor = NULL;
}

void EnDekubaba_Destroy(Actor* thisx, PlayState* play) {
    EnDekubaba* this = (EnDekubaba*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void EnDekubaba_DisableHitboxes(EnDekubaba* this) {
    s32 i;

    for (i = 1; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].base.acElemFlags &= ~ACELEM_ON;
    }
}

void EnDekubaba_UpdateHeadPosition(EnDekubaba* this) {
    f32 horizontalHeadShift = (Math_CosS(this->stemSectionAngle[0]) + Math_CosS(this->stemSectionAngle[1]) +
                               Math_CosS(this->stemSectionAngle[2])) *
                              20.0f;

    this->actor.world.pos.x =
        this->actor.home.pos.x + Math_SinS(this->actor.shape.rot.y) * (horizontalHeadShift * this->size);
    this->actor.world.pos.y =
        this->actor.home.pos.y - (Math_SinS(this->stemSectionAngle[0]) + Math_SinS(this->stemSectionAngle[1]) +
                                  Math_SinS(this->stemSectionAngle[2])) *
                                     20.0f * this->size;
    this->actor.world.pos.z =
        this->actor.home.pos.z + Math_CosS(this->actor.shape.rot.y) * (horizontalHeadShift * this->size);
}

void EnDekubaba_SetFireLightEffects(EnDekubaba* this, PlayState* play, s32 index) {
    ColliderJntSphElement* jntSphElem;

    if (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_FIRE) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffScale = 0.75f;
        this->drawDmgEffAlpha = 4.0f;
    } else if (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_LIGHT) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
        this->drawDmgEffScale = 0.75f;
        this->drawDmgEffAlpha = 4.0f;
        jntSphElem = &this->collider.elements[index];
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, jntSphElem->base.acDmgInfo.hitPos.x,
                    jntSphElem->base.acDmgInfo.hitPos.y, jntSphElem->base.acDmgInfo.hitPos.z, 0, 0, 0,
                    CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_LIGHT_RAYS));
    }
}

void EnDekubaba_SetFrozenEffects(EnDekubaba* this) {
    this->drawDmgEffScale = 0.75f;
    this->drawDmgEffFrozenSteamScale = 1.125f;
    this->drawDmgEffAlpha = 1.0f;
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->collider.base.colMaterial = COL_MATERIAL_HIT3;
    this->timer = 80;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 80);
}

void EnDekubaba_SpawnIceEffects(EnDekubaba* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->collider.base.colMaterial = COL_MATERIAL_HIT6;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, DEKUBABA_BODYPART_MAX, 4, this->size * 0.3f,
                              this->size * 0.2f);
        this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    }
}

void EnDekubaba_SetupWait(EnDekubaba* this) {
    s32 i;
    ColliderJntSphElement* jntSphElem;

    this->actor.shape.rot.x = -0x4000;
    this->stemSectionAngle[0] = this->stemSectionAngle[1] = this->stemSectionAngle[2] = this->actor.shape.rot.x;

    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.z = this->actor.home.pos.z;
    this->actor.world.pos.y = this->actor.home.pos.y + 14.0f * this->size;

    Actor_SetScale(&this->actor, this->size * 0.01f * 0.5f);

    this->collider.base.colMaterial = COL_MATERIAL_HARD;
    this->collider.base.acFlags |= AC_HARD;
    this->timer = 45;

    for (i = 1; i < ARRAY_COUNT(this->colliderElements); i++) {
        jntSphElem = &this->collider.elements[i];
        jntSphElem->dim.worldSphere.center.x = this->actor.world.pos.x;
        jntSphElem->dim.worldSphere.center.y = (s32)this->actor.world.pos.y - 7;
        jntSphElem->dim.worldSphere.center.z = this->actor.world.pos.z;
    }

    this->actionFunc = EnDekubaba_Wait;
}

void EnDekubaba_Wait(EnDekubaba* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.z = this->actor.home.pos.z;
    this->actor.world.pos.y = this->actor.home.pos.y + 14.0f * this->size;

    if ((this->timer == 0) && (this->actor.xzDistToPlayer < (200.0f * this->size)) &&
        (fabsf(this->actor.playerHeightRel) < (30.0f * this->size))) {
        EnDekubaba_SetupGrow(this);
    }
}

void EnDekubaba_SetupGrow(EnDekubaba* this) {
    s32 i;

    Animation_Change(&this->skelAnime, &gDekuBabaFastChompAnim,
                     Animation_GetLastFrame(&gDekuBabaFastChompAnim) * (1.0f / 15), 0.0f,
                     Animation_GetLastFrame(&gDekuBabaFastChompAnim), ANIMMODE_ONCE, 0.0f);

    this->timer = 15;

    for (i = 2; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].base.ocElemFlags |= OCELEM_ON;
    }

    this->collider.base.colMaterial = COL_MATERIAL_HIT6;
    this->collider.base.acFlags &= ~AC_HARD;
    Actor_PlaySfx(&this->actor, NA_SE_EN_DEKU_WAKEUP);
    this->actionFunc = EnDekubaba_Grow;
}

void EnDekubaba_Grow(EnDekubaba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 headDistHorizontal;
    f32 headDistVertical;
    f32 headShiftX;
    f32 headShiftZ;

    if (this->timer != 0) {
        this->timer--;
    }

    SkelAnime_Update(&this->skelAnime);

    this->actor.scale.x = this->actor.scale.y = this->actor.scale.z =
        this->size * 0.01f * (0.5f + (15 - this->timer) * 0.5f / 15.0f);
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0x1800, 0x800);

    headDistVertical = Math_SinF(CLAMP_MAX((15 - this->timer) * (1.0f / 15), 0.7f) * M_PIf) * 32.0f + 14.0f;

    if (this->actor.shape.rot.x < -0x38E3) {
        headDistHorizontal = 0.0f;
    } else if (this->actor.shape.rot.x < -0x238E) {
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0x5555, 0x38E);
        headDistHorizontal = Math_CosS(this->stemSectionAngle[0]) * 20.0f;
    } else if (this->actor.shape.rot.x < -0xE38) {
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0xAAA, 0x38E);
        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x5555, 0x38E);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x5555, 0x222);

        headDistHorizontal = 20.0f * (Math_CosS(this->stemSectionAngle[0]) + Math_CosS(this->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-Math_SinS(this->stemSectionAngle[0]) - Math_SinS(this->stemSectionAngle[1]))) *
                                 Math_CosS(this->stemSectionAngle[2]) / -Math_SinS(this->stemSectionAngle[2]);
    } else {
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0xAAA, 0x38E);
        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x31C7, 0x222);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x5555, 0x222);

        headDistHorizontal = 20.0f * (Math_CosS(this->stemSectionAngle[0]) + Math_CosS(this->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-Math_SinS(this->stemSectionAngle[0]) - Math_SinS(this->stemSectionAngle[1]))) *
                                 Math_CosS(this->stemSectionAngle[2]) / -Math_SinS(this->stemSectionAngle[2]);
    }

    if (this->timer < 10) {
        Math_ApproachS(&this->actor.shape.rot.y, Math_Vec3f_Yaw(&this->actor.home.pos, &player->actor.world.pos), 2,
                       0xE38);
    }

    this->actor.world.pos.y = this->actor.home.pos.y + (headDistVertical * this->size);
    //! FAKE:
    headShiftX = headDistHorizontal;
    headShiftX = headShiftX * this->size * Math_SinS(this->actor.shape.rot.y);
    headShiftZ = headDistHorizontal;
    headShiftZ = headShiftZ * this->size * Math_CosS(this->actor.shape.rot.y);
    this->actor.world.pos.x = this->actor.home.pos.x + headShiftX;
    this->actor.world.pos.z = this->actor.home.pos.z + headShiftZ;

    EffectSsHahen_SpawnBurst(play, &this->actor.home.pos, this->size * 3.0f, 0, this->size * 12.0f, this->size * 5.0f,
                             1, HAHEN_OBJECT_DEFAULT, 10, NULL);

    if (this->timer == 0) {
        if (Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < (240.0f * this->size)) {
            EnDekubaba_SetupPrepareLunge(this);
        } else {
            EnDekubaba_SetupRetract(this);
        }
    }
}

void EnDekubaba_SetupRetract(EnDekubaba* this) {
    s32 i;

    Animation_Change(&this->skelAnime, &gDekuBabaFastChompAnim, -1.5f, Animation_GetLastFrame(&gDekuBabaFastChompAnim),
                     0.0f, ANIMMODE_ONCE, -3.0f);

    this->timer = 15;

    for (i = 2; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].base.ocElemFlags &= ~OCELEM_ON;
    }

    this->actionFunc = EnDekubaba_Retract;
}

void EnDekubaba_Retract(EnDekubaba* this, PlayState* play) {
    f32 headDistHorizontal;
    f32 headDistVertical;
    f32 xShift;
    f32 zShift;

    if (this->timer != 0) {
        this->timer--;
    }

    SkelAnime_Update(&this->skelAnime);

    this->actor.scale.x = this->actor.scale.y = this->actor.scale.z =
        this->size * 0.01f * (0.5f + this->timer * (1.0f / 30));
    Math_ScaledStepToS(&this->actor.shape.rot.x, -0x4000, 0x300);

    headDistVertical = (Math_SinF(CLAMP_MAX(this->timer * 0.033f, 0.7f) * M_PIf) * 32.0f) + 14.0f;

    if (this->actor.shape.rot.x < -0x38E3) {
        headDistHorizontal = 0.0f;
    } else if (this->actor.shape.rot.x < -0x238E) {
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0x4000, 0x555);
        headDistHorizontal = Math_CosS(this->stemSectionAngle[0]) * 20.0f;
    } else if (this->actor.shape.rot.x < -0xE38) {
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0x5555, 0x555);
        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x4000, 0x555);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x4000, 0x333);

        headDistHorizontal = 20.0f * (Math_CosS(this->stemSectionAngle[0]) + Math_CosS(this->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-Math_SinS(this->stemSectionAngle[0]) - Math_SinS(this->stemSectionAngle[1]))) *
                                 Math_CosS(this->stemSectionAngle[2]) / -Math_SinS(this->stemSectionAngle[2]);
    } else {
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0x5555, 0x555);
        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x5555, 0x333);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x4000, 0x333);

        headDistHorizontal = 20.0f * (Math_CosS(this->stemSectionAngle[0]) + Math_CosS(this->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-Math_SinS(this->stemSectionAngle[0]) - Math_SinS(this->stemSectionAngle[1]))) *
                                 Math_CosS(this->stemSectionAngle[2]) / -Math_SinS(this->stemSectionAngle[2]);
    }

    this->actor.world.pos.y = this->actor.home.pos.y + (headDistVertical * this->size);
    xShift = headDistHorizontal * this->size * Math_SinS(this->actor.shape.rot.y);
    zShift = headDistHorizontal * this->size * Math_CosS(this->actor.shape.rot.y);
    this->actor.world.pos.x = this->actor.home.pos.x + xShift;
    this->actor.world.pos.z = this->actor.home.pos.z + zShift;

    EffectSsHahen_SpawnBurst(play, &this->actor.home.pos, this->size * 3.0f, 0, this->size * 12.0f, this->size * 5.0f,
                             1, HAHEN_OBJECT_DEFAULT, 10, NULL);
    if (this->timer == 0) {
        EnDekubaba_SetupWait(this);
    }
}

void EnDekubaba_SetupDecideLunge(EnDekubaba* this) {
    this->timer = Animation_GetLastFrame(&gDekuBabaFastChompAnim) * 2;
    Animation_MorphToLoop(&this->skelAnime, &gDekuBabaFastChompAnim, -3.0f);
    this->actionFunc = EnDekubaba_DecideLunge;
}

void EnDekubaba_DecideLunge(EnDekubaba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 12.0f)) {
        if (this->actor.params == DEKUBABA_BIG) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_DEKU_MOUTH);
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_EN_MIZUBABA1_MOUTH);
        }
    }

    if (this->timer != 0) {
        this->timer--;
    }

    Math_ApproachS(&this->actor.shape.rot.y, Math_Vec3f_Yaw(&this->actor.home.pos, &player->actor.world.pos), 2,
                   (this->timer % 5) * 0x222);

    if (this->timer < 10) {
        this->stemSectionAngle[0] += 0x16C;
        this->stemSectionAngle[1] += 0x16C;
        this->stemSectionAngle[2] += 0xB6;
        this->actor.shape.rot.x += 0x222;
    } else if (this->timer < 20) {
        this->stemSectionAngle[0] -= 0x16C;
        this->stemSectionAngle[1] += 0x111;
        this->actor.shape.rot.x += 0x16C;
    } else if (this->timer < 30) {
        this->stemSectionAngle[1] -= 0x111;
        this->actor.shape.rot.x -= 0xB6;
    } else {
        this->stemSectionAngle[1] -= 0xB6;
        this->stemSectionAngle[2] += 0xB6;
        this->actor.shape.rot.x -= 0x16C;
    }

    EnDekubaba_UpdateHeadPosition(this);

    if ((240.0f * this->size) < Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos)) {
        EnDekubaba_SetupRetract(this);
    } else if ((this->timer == 0) || (this->actor.xzDistToPlayer < (80.0f * this->size))) {
        EnDekubaba_SetupPrepareLunge(this);
    }
}

void EnDekubaba_SetupPrepareLunge(EnDekubaba* this) {
    this->timer = 8;
    this->skelAnime.playSpeed = 0.0f;
    this->actionFunc = EnDekubaba_PrepareLunge;
}

void EnDekubaba_PrepareLunge(EnDekubaba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->timer != 0) {
        this->timer--;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.x, 0x1800, 2, 0xE38, 0x71C);
    Math_ApproachS(&this->actor.shape.rot.y, Math_Vec3f_Yaw(&this->actor.home.pos, &player->actor.world.pos), 2, 0xE38);
    Math_ScaledStepToS(this->stemSectionAngle, 0xAAA, 0x444);
    Math_ScaledStepToS(&this->stemSectionAngle[1], -0x4718, 0x888);
    Math_ScaledStepToS(&this->stemSectionAngle[2], -0x6AA4, 0x888);

    if (this->timer == 0) {
        EnDekubaba_SetupLunge(this);
    }

    EnDekubaba_UpdateHeadPosition(this);
}

void EnDekubaba_SetupLunge(EnDekubaba* this) {
    Animation_PlayOnce(&this->skelAnime, &gDekuBabaPauseChompAnim);
    this->timer = 0;
    this->actionFunc = EnDekubaba_Lunge;
}

void EnDekubaba_Lunge(EnDekubaba* this, PlayState* play) {
    static Color_RGBA8 sDustPrimColor = { 105, 255, 105, 255 };
    static Color_RGBA8 sDustEnvColor = { 150, 250, 150, 0 };
    s32 allStepsDone;
    s16 curFrame10;
    Vec3f velocity;

    SkelAnime_Update(&this->skelAnime);

    if (this->timer == 0) {
        if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
            if (this->actor.params == DEKUBABA_BIG) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_DEKU_ATTACK);
            } else {
                Actor_PlaySfx(&this->actor, NA_SE_EN_MIZUBABA1_ATTACK);
            }
        }

        Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x222);

        curFrame10 = (s32)(this->skelAnime.curFrame * 10.0f);

        allStepsDone = true;
        allStepsDone &= Math_ScaledStepToS(&this->stemSectionAngle[0], -0xE38, curFrame10 + 0x38E);
        allStepsDone &= Math_ScaledStepToS(&this->stemSectionAngle[1], -0xE38, curFrame10 + 0x71C);
        allStepsDone &= Math_ScaledStepToS(&this->stemSectionAngle[2], -0xE38, curFrame10 + 0xE38);
        if (allStepsDone) {
            Animation_PlayLoopSetSpeed(&this->skelAnime, &gDekuBabaFastChompAnim, 4.0f);
            velocity.x = Math_SinS(this->actor.shape.rot.y) * 5.0f;
            velocity.y = 0.0f;
            velocity.z = Math_CosS(this->actor.shape.rot.y) * 5.0f;
            func_800B0DE0(play, &this->actor.world.pos, &velocity, &gZeroVec3f, &sDustPrimColor, &sDustEnvColor, 1,
                          (s32)(this->size * 100.0f));
            this->timer = 1;
        }
    } else if (this->timer > 10) {
        EnDekubaba_SetupPullBack(this);
    } else {
        this->timer++;

        if ((this->timer >= 4) && !Actor_IsFacingPlayer(&this->actor, 0x16C)) {
            Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xF, 0x71C);
        }

        if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 12.0f)) {
            if (this->actor.params == DEKUBABA_BIG) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_DEKU_MOUTH);
            } else {
                Actor_PlaySfx(&this->actor, NA_SE_EN_MIZUBABA1_MOUTH);
            }
        }
    }

    EnDekubaba_UpdateHeadPosition(this);
}

void EnDekubaba_SetupPullBack(EnDekubaba* this) {
    Animation_Change(&this->skelAnime, &gDekuBabaPauseChompAnim, 1.0f, 15.0f,
                     Animation_GetLastFrame(&gDekuBabaPauseChompAnim), ANIMMODE_ONCE, -3.0f);
    this->timer = 0;
    this->actionFunc = EnDekubaba_PullBack;
}

void EnDekubaba_PullBack(EnDekubaba* this, PlayState* play) {
    Vec3f dustPos;
    f32 xIncr;
    f32 zIncr;
    s32 i;

    SkelAnime_Update(&this->skelAnime);

    if (this->timer == 0) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, -0x93E, 0x38E);
        Math_ScaledStepToS(&this->stemSectionAngle[0], -0x888, 0x16C);
        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x888, 0x16C);

        if (Math_ScaledStepToS(&this->stemSectionAngle[2], -0x888, 0x16C)) {
            xIncr = Math_SinS(this->actor.shape.rot.y) * 30.0f * this->size;
            zIncr = Math_CosS(this->actor.shape.rot.y) * 30.0f * this->size;
            dustPos = this->actor.home.pos;

            for (i = 0; i < 3; i++) {
                func_800B1210(play, &dustPos, &gZeroVec3f, &gZeroVec3f, (s32)(this->size * 500.0f),
                              (s32)(this->size * 50.0f));
                dustPos.x += xIncr;
                dustPos.z += zIncr;
            }

            this->timer = 1;
        }
    } else if (this->timer == 11) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, -0x93E, 0x200);
        Math_ScaledStepToS(this->stemSectionAngle, -0xAAA, 0x200);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x5C71, 0x200);

        if (Math_ScaledStepToS(&this->stemSectionAngle[1], 0x238C, 0x200)) {
            this->timer = 12;
        }
    } else if (this->timer == 18) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, 0x2AA8, 0xAAA);

        if (Math_ScaledStepToS(&this->stemSectionAngle[0], 0x1554, 0x5B0)) {
            this->timer = 25;
        }

        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x38E3, 0xAAA);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x5C71, 0x2D8);
    } else if (this->timer == 25) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, -0x5550, 0xAAA);

        if (Math_ScaledStepToS(&this->stemSectionAngle[0], -0x6388, 0x93E)) {
            this->timer = 26;
        }

        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x3FFC, 0x4FA);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x238C, 0x444);
    } else if (this->timer == 26) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, 0x1800, 0x93E);

        if (Math_ScaledStepToS(&this->stemSectionAngle[0], -0x1555, 0x71C)) {
            this->timer = 27;
        }

        Math_ScaledStepToS(&this->stemSectionAngle[1], -0x38E3, 0x2D8);
        Math_ScaledStepToS(&this->stemSectionAngle[2], -0x5C71, 0x5B0);
    } else if (this->timer >= 27) {
        this->timer++;
        if (this->timer > 30) {
            if (this->actor.xzDistToPlayer < (80.0f * this->size)) {
                EnDekubaba_SetupPrepareLunge(this);
            } else {
                EnDekubaba_SetupDecideLunge(this);
            }
        }
    } else {
        this->timer++;
        if (this->timer == 10) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_DEKU_SCRAPE);
        }

        if (this->timer >= 12) {
            Math_ScaledStepToS(&this->stemSectionAngle[2], -0x5C71, 0x88);
        }
    }

    EnDekubaba_UpdateHeadPosition(this);
}

void EnDekubaba_SetupRecover(EnDekubaba* this) {
    this->timer = 9;
    this->collider.base.acFlags |= AC_ON;
    this->skelAnime.playSpeed = -1.0f;
    this->actionFunc = EnDekubaba_Recover;
}

void EnDekubaba_Recover(EnDekubaba* this, PlayState* play) {
    s32 anyStepsDone;

    SkelAnime_Update(&this->skelAnime);

    if (this->timer > 8) {
        anyStepsDone = Math_SmoothStepToS(&this->actor.shape.rot.x, 0x1800, 1, 0x11C6, 0x71C);
        anyStepsDone |= Math_SmoothStepToS(&this->stemSectionAngle[0], -0x1555, 1, 0xAAA, 0x71C);
        anyStepsDone |= Math_SmoothStepToS(&this->stemSectionAngle[1], -0x38E3, 1, 0xE38, 0x71C);
        anyStepsDone |= Math_SmoothStepToS(&this->stemSectionAngle[2], -0x5C71, 1, 0x11C6, 0x71C);
        if (!anyStepsDone) {
            this->timer = 8;
        }
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            EnDekubaba_SetupDecideLunge(this);
        }
    }

    EnDekubaba_UpdateHeadPosition(this);
}

typedef enum {
    /* 0 */ DEKUBABA_HIT_NO_STUN,
    /* 1 */ DEKUBABA_HIT_STUN_OTHER,
    /* 2 */ DEKUBABA_HIT_STUN_NUT,
    /* 3 */ DEKUBABA_HIT_STUN_ELECTRIC
} DekuBabaHitType;

void EnDekubaba_SetupHit(EnDekubaba* this, s32 hitType) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDekuBabaPauseChompAnim, -5.0f);
    this->timer = hitType;
    this->collider.base.acFlags &= ~AC_ON;
    Actor_SetScale(&this->actor, this->size * 0.01f);

    if (hitType == DEKUBABA_HIT_STUN_NUT) {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 155, COLORFILTER_BUFFLAG_OPA, 42);
    } else if (hitType == DEKUBABA_HIT_STUN_ELECTRIC) {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 155, COLORFILTER_BUFFLAG_OPA, 42);
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_LARGE;
        this->drawDmgEffScale = 0.75f;
        this->drawDmgEffAlpha = 2.0f;
    } else {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 42);
    }
    this->actionFunc = EnDekubaba_Hit;
}

/**
 * Hit by a weapon or hit something when lunging.
 */
void EnDekubaba_Hit(EnDekubaba* this, PlayState* play) {
    s32 allStepsDone;

    SkelAnime_Update(&this->skelAnime);
    allStepsDone = true;
    allStepsDone &= Math_ScaledStepToS(&this->actor.shape.rot.x, -0x4000, 0xE38);
    allStepsDone &= Math_ScaledStepToS(this->stemSectionAngle, -0x4000, 0xE38);
    allStepsDone &= Math_ScaledStepToS(&this->stemSectionAngle[1], -0x4000, 0xE38);
    allStepsDone &= Math_ScaledStepToS(&this->stemSectionAngle[2], -0x4000, 0xE38);
    if (allStepsDone) {
        if (this->actor.colChkInfo.health == 0) {
            EnDekubaba_SetupShrinkDie(this);
        } else {
            this->collider.base.acFlags |= AC_ON;

            if (this->timer == DEKUBABA_HIT_NO_STUN) {
                if (this->actor.xzDistToPlayer < (80.0f * this->size)) {
                    EnDekubaba_SetupPrepareLunge(this);
                } else {
                    EnDekubaba_SetupRecover(this);
                }
            } else {
                EnDekubaba_SetupStunnedVertical(this);
            }
        }
    }

    EnDekubaba_UpdateHeadPosition(this);
}

void EnDekubaba_SetupPrunedSomersaultDie(EnDekubaba* this) {
    this->timer = 0;
    this->skelAnime.playSpeed = 0.0f;
    this->actor.gravity = -0.8f;
    this->actor.velocity.y = 4.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x8000;
    this->actor.speed = this->size * 3.0f;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;
    this->actionFunc = EnDekubaba_PrunedSomersaultDie;
}

void EnDekubaba_PrunedSomersaultDie(EnDekubaba* this, PlayState* play) {
    s32 i;
    Vec3f dustPos;
    f32 deltaX;
    f32 deltaY;
    f32 deltaZ;

    Math_StepToF(&this->actor.speed, 0.0f, this->size * 0.1f);

    if (this->timer == 0) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, 0x4800, 0x71C);
        Math_ScaledStepToS(this->stemSectionAngle, 0x4800, 0x71C);
        Math_ScaledStepToS(&this->stemSectionAngle[1], 0x4800, 0x71C);

        EffectSsHahen_SpawnBurst(play, &this->actor.world.pos, this->size * 3.0f, 0, (s32)(this->size * 12.0f),
                                 (s32)(this->size * 5.0f), 1, HAHEN_OBJECT_DEFAULT, 10, NULL);

        if ((this->actor.scale.x > 0.005f) &&
            ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) || (this->actor.bgCheckFlags & BGCHECKFLAG_WALL))) {
            this->actor.scale.z = 0.0f;
            this->actor.scale.y = 0.0f;
            this->actor.scale.x = 0.0f;
            this->actor.speed = 0.0f;
            this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
            EffectSsHahen_SpawnBurst(play, &this->actor.world.pos, this->size * 3.0f, 0, (s32)(this->size * 12.0f),
                                     (s32)(this->size * 5.0f), 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_ATTACK);
            this->timer = 1;
        }
    } else if (this->timer == 1) {
        dustPos = this->actor.world.pos;

        deltaY = 20.0f * Math_SinS(this->actor.shape.rot.x);
        deltaX = -20.0f * Math_CosS(this->actor.shape.rot.x) * Math_SinS(this->actor.shape.rot.y);
        deltaZ = -20.0f * Math_CosS(this->actor.shape.rot.x) * Math_CosS(this->actor.shape.rot.y);

        for (i = 0; i < 4; i++) {
            func_800B1210(play, &dustPos, &gZeroVec3f, &gZeroVec3f, 500, 50);
            dustPos.x += deltaX;
            dustPos.y += deltaY;
            dustPos.z += deltaZ;
        }

        func_800B1210(play, &this->actor.home.pos, &gZeroVec3f, &gZeroVec3f, (s32)(this->size * 500.0f),
                      (s32)(this->size * 100.0f));
        EnDekubaba_SetupDeadStickDrop(this, play);
    }
}

void EnDekubaba_SetupShrinkDie(EnDekubaba* this) {
    Animation_Change(&this->skelAnime, &gDekuBabaFastChompAnim, -1.5f, Animation_GetLastFrame(&gDekuBabaFastChompAnim),
                     0.0f, ANIMMODE_ONCE, -3.0f);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnDekubaba_ShrinkDie;
}

/**
 * Die and drop Deku Nuts (Deku Stick drop is handled elsewhere).
 */
void EnDekubaba_ShrinkDie(EnDekubaba* this, PlayState* play) {
    Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y, this->size * 5.0f);

    if (Math_StepToF(&this->actor.scale.x, this->size * 0.1f * 0.01f, this->size * 0.1f * 0.01f)) {
        func_800B1210(play, &this->actor.home.pos, &gZeroVec3f, &gZeroVec3f, (s32)(this->size * 500.0f),
                      (s32)(this->size * 100.0f));

        if (!this->actor.dropFlag) {
            Item_DropCollectible(play, &this->actor.world.pos, ITEM00_DEKU_NUTS_1);
            if (this->actor.params == DEKUBABA_BIG) {
                Item_DropCollectible(play, &this->actor.world.pos, ITEM00_DEKU_NUTS_1);
                Item_DropCollectible(play, &this->actor.world.pos, ITEM00_DEKU_NUTS_1);
            }
        } else {
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x30);
        }
        Actor_Kill(&this->actor);
    }

    this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;
    this->actor.shape.rot.z += 0x1C70;
    EffectSsHahen_SpawnBurst(play, &this->actor.home.pos, this->size * 3.0f, 0, (s32)(this->size * 12.0f),
                             (s32)(this->size * 5.0f), 1, HAHEN_OBJECT_DEFAULT, 10, NULL);
}

void EnDekubaba_SetupStunnedVertical(EnDekubaba* this) {
    s32 i;

    for (i = 1; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].base.acElemFlags |= ACELEM_ON;
    }

    if (this->timer == 1) {
        Animation_Change(&this->skelAnime, &gDekuBabaFastChompAnim, 4.0f, 0.0f,
                         Animation_GetLastFrame(&gDekuBabaFastChompAnim), ANIMMODE_LOOP, -3.0f);
        this->timer = 40;
    } else {
        Animation_Change(&this->skelAnime, &gDekuBabaFastChompAnim, 0.0f, 0.0f,
                         Animation_GetLastFrame(&gDekuBabaFastChompAnim), ANIMMODE_LOOP, -3.0f);
        if (this->timer == 2) {
            this->timer = 40;
        } else {
            this->timer = 40;
        }
    }

    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.y = this->actor.home.pos.y + (60.0f * this->size);
    this->actor.world.pos.z = this->actor.home.pos.z;
    this->actionFunc = EnDekubaba_StunnedVertical;
}

void EnDekubaba_StunnedVertical(EnDekubaba* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        EnDekubaba_DisableHitboxes(this);

        if (this->actor.xzDistToPlayer < (80.0f * this->size)) {
            EnDekubaba_SetupPrepareLunge(this);
        } else {
            EnDekubaba_SetupRecover(this);
        }
    }
}

void EnDekubaba_SetupSway(EnDekubaba* this) {
    this->targetSwayAngle = -0x6000;
    this->stemSectionAngle[2] = -0x5000;
    this->stemSectionAngle[1] = -0x4800;
    EnDekubaba_DisableHitboxes(this);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 35);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnDekubaba_Sway;
}

/**
 * Sway back and forth with decaying amplitude until close enough to vertical.
 */
void EnDekubaba_Sway(EnDekubaba* this, PlayState* play) {
    s16 angleToVertical;

    SkelAnime_Update(&this->skelAnime);
    Math_ScaledStepToS(&this->actor.shape.rot.x, this->stemSectionAngle[0], 0x71C);
    Math_ScaledStepToS(this->stemSectionAngle, this->stemSectionAngle[1], 0x71C);
    Math_ScaledStepToS(&this->stemSectionAngle[1], this->stemSectionAngle[2], 0x71C);
    if (Math_ScaledStepToS(&this->stemSectionAngle[2], this->targetSwayAngle, 0x71C)) {
        this->targetSwayAngle = -0x4000 - (s32)((this->targetSwayAngle + 0x4000) * 0.8f);
    }

    angleToVertical = this->targetSwayAngle + 0x4000;

    if (ABS_ALT(angleToVertical) < 0x100) {
        this->collider.base.acFlags |= AC_ON;
        if (this->actor.xzDistToPlayer < (80.0f * this->size)) {
            EnDekubaba_SetupPrepareLunge(this);
        } else {
            EnDekubaba_SetupRecover(this);
        }
    }
    EnDekubaba_UpdateHeadPosition(this);
}

void EnDekubaba_SetupFrozen(EnDekubaba* this) {
    EnDekubaba_SetFrozenEffects(this);
    this->actionFunc = EnDekubaba_Frozen;
}

void EnDekubaba_Frozen(EnDekubaba* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        EnDekubaba_SpawnIceEffects(this, play);

        if (this->actor.colChkInfo.health == 0) {
            EnDekubaba_SetupShrinkDie(this);
        } else if (this->actor.xzDistToPlayer < (80.0f * this->size)) {
            EnDekubaba_SetupPrepareLunge(this);
        } else {
            EnDekubaba_SetupRecover(this);
        }
    }
}

void EnDekubaba_SetupDeadStickDrop(EnDekubaba* this, PlayState* play) {
    Actor_SetScale(&this->actor, 0.03f);
    this->actor.shape.rot.x -= 0x4000;
    this->actor.shape.yOffset = 1000.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.shape.shadowScale = 3.0f;
    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_MISC);
    this->actor.flags &= ~ACTOR_FLAG_DRAW_CULLING_DISABLED;
    this->timer = 200;
    this->drawDmgEffAlpha = 0.0f;
    this->actionFunc = EnDekubaba_DeadStickDrop;
}

void EnDekubaba_DeadStickDrop(EnDekubaba* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (Actor_HasParent(&this->actor, play) || (this->timer == 0)) {
        Actor_Kill(&this->actor);
        return;
    }

    Actor_OfferGetItemNearby(&this->actor, play, GI_DEKU_STICKS_1);
}

/* Update and associated functions */

void EnDekubaba_UpdateDamage(EnDekubaba* this, PlayState* play) {
    s32 newHealth;
    s32 i;
    ColliderJntSphElement* jntSphElem;

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlagJntSph(&this->actor, &this->collider);

        if ((this->collider.base.colMaterial != COL_MATERIAL_HARD) &&
            (this->actor.colChkInfo.damageEffect != DEKUBABA_DMGEFF_HOOKSHOT)) {
            jntSphElem = &this->collider.elements[0];
            for (i = 0; i < ARRAY_COUNT(this->colliderElements); i++, jntSphElem++) {
                if (jntSphElem->base.acElemFlags & ACELEM_HIT) {
                    break;
                }
            }

            if ((i != ARRAY_COUNT(this->colliderElements)) &&
                ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) ||
                 !(jntSphElem->base.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3))) {
                EnDekubaba_SpawnIceEffects(this, play);
                newHealth = this->actor.colChkInfo.health - this->actor.colChkInfo.damage;

                if (this->actionFunc != EnDekubaba_StunnedVertical) {
                    if (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_ICE) {
                        EnDekubaba_SetupFrozen(this);
                    } else if (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_ELECTRIC) {
                        EnDekubaba_SetupHit(this, DEKUBABA_HIT_STUN_ELECTRIC);
                    } else if (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_NUT) {
                        EnDekubaba_SetupHit(this, DEKUBABA_HIT_STUN_NUT);
                    } else {
                        EnDekubaba_SetFireLightEffects(this, play, i);

                        if (this->actionFunc == EnDekubaba_PullBack) {
                            if (newHealth <= 0) {
                                newHealth = 1;
                            }

                            EnDekubaba_SetupHit(this, DEKUBABA_HIT_STUN_OTHER);
                        } else {
                            EnDekubaba_SetupHit(this, DEKUBABA_HIT_NO_STUN);
                        }
                    }
                } else {
                    if (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_CUT) {
                        if (newHealth > 0) {
                            EnDekubaba_SetupSway(this);
                        } else {
                            EnDekubaba_SetupPrunedSomersaultDie(this);
                        }
                    } else if (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_ELECTRIC) {
                        EnDekubaba_SetupHit(this, DEKUBABA_HIT_STUN_ELECTRIC);
                    } else if (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_NUT) {
                        EnDekubaba_SetupHit(this, DEKUBABA_HIT_STUN_NUT);
                    } else if (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_ICE) {
                        EnDekubaba_SetupFrozen(this);
                    } else {
                        EnDekubaba_SetFireLightEffects(this, play, i);
                        EnDekubaba_SetupHit(this, DEKUBABA_HIT_NO_STUN);
                    }
                }

                if (newHealth < 0) {
                    this->actor.colChkInfo.health = 0;
                } else {
                    this->actor.colChkInfo.health = newHealth;
                }
            } else {
                return;
            }
        } else {
            return;
        }
    } else if ((play->actorCtx.unk2 != 0) && (this->actor.xyzDistToPlayerSq < SQ(200.0f)) &&
               (this->collider.base.colMaterial != COL_MATERIAL_HARD) &&
               (this->actionFunc != EnDekubaba_StunnedVertical) && (this->actionFunc != EnDekubaba_Hit) &&
               (this->actor.colChkInfo.health != 0)) {
        this->actor.colChkInfo.health--;
        this->actor.dropFlag = 0;
        EnDekubaba_SetupHit(this, DEKUBABA_HIT_STUN_OTHER);
    } else {
        return;
    }

    if (this->actor.colChkInfo.health != 0) {
        if ((this->timer == 2) || (this->timer == 3)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_EN_DEKU_DAMAGE);
        }
    } else {
        Enemy_StartFinishingBlow(play, &this->actor);

        if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            this->timer = 3;
            this->collider.base.acFlags &= ~AC_ON;
        } else if (this->drawDmgEffAlpha > 1.5f) {
            this->drawDmgEffAlpha = 1.5f;
        }

        if (this->actor.params == DEKUBABA_BIG) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_DEKU_DEAD);
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_EN_DEKU_JR_DEAD);
        }
    }
}

void EnDekubaba_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDekubaba* this = (EnDekubaba*)thisx;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        EnDekubaba_SetupRecover(this);
    }

    EnDekubaba_UpdateDamage(this, play);
    this->actionFunc(this, play);

    if (this->actionFunc == EnDekubaba_PrunedSomersaultDie) {
        Actor_MoveWithGravity(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, this->size * 15.0f, 10.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    } else if (this->actionFunc != EnDekubaba_DeadStickDrop) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
        if (this->boundFloor == NULL) {
            this->boundFloor = this->actor.floorPoly;
        }
    }

    if (this->actionFunc == EnDekubaba_Lunge) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
    }

    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->actionFunc != EnDekubaba_DeadStickDrop) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->drawDmgEffAlpha > 0.0f) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.375f;
            this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.75f);
        } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.75f, 0.75f / 40)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
}

/* Draw functions */

void EnDekubaba_DrawStemRetracted(EnDekubaba* this, PlayState* play) {
    f32 horizontalScale;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    horizontalScale = this->size * 0.01f;

    Matrix_Translate(this->actor.home.pos.x, this->actor.home.pos.y + (-6.0f * this->size), this->actor.home.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateZYX(this->stemSectionAngle[0], this->actor.shape.rot.y, 0, MTXMODE_APPLY);
    Matrix_Scale(horizontalScale, horizontalScale, horizontalScale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStemTopDL);

    for (i = DEKUBABA_BODYPART_1; i < DEKUBABA_BODYPART_MAX; i++) {
        Matrix_MultZero(&this->bodyPartsPos[i]);
    }

    Actor_SetFocus(&this->actor, 0.0f);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDekubaba_DrawStemExtended(EnDekubaba* this, PlayState* play) {
    static Gfx* sStemDLists[] = { gDekuBabaStemTopDL, gDekuBabaStemMiddleDL, gDekuBabaStemBaseDL };
    MtxF mf;
    s32 i;
    f32 scale;
    f32 horizontalStepSize;
    s32 stemSections;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->actionFunc == EnDekubaba_PrunedSomersaultDie) {
        stemSections = 2;
    } else {
        stemSections = 3;
    }

    scale = this->size * 0.01f;
    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_Get(&mf);

    for (i = 0; i < stemSections; i++) {
        mf.yw += 20.0f * Math_SinS(this->stemSectionAngle[i]) * this->size;
        horizontalStepSize = Math_CosS(this->stemSectionAngle[i]) * 20.0f * this->size;
        mf.xw -= horizontalStepSize * Math_SinS(this->actor.shape.rot.y);
        mf.zw -= horizontalStepSize * Math_CosS(this->actor.shape.rot.y);

        Matrix_Put(&mf);
        Matrix_RotateZYX(this->stemSectionAngle[i], this->actor.shape.rot.y, 0, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, sStemDLists[i]);

        Collider_UpdateSpheres(51 + 2 * i, &this->collider);
        Collider_UpdateSpheres(52 + 2 * i, &this->collider);

        if (i == 0) {
            if (this->actionFunc != EnDekubaba_Sway) {
                this->actor.focus.pos.x = mf.xw;
                this->actor.focus.pos.y = mf.yw;
                this->actor.focus.pos.z = mf.zw;
            } else {
                this->actor.focus.pos.x = this->actor.home.pos.x;
                this->actor.focus.pos.y = this->actor.home.pos.y + (40.0f * this->size);
                this->actor.focus.pos.z = this->actor.home.pos.z;
            }
        }

        Matrix_MultZero(&this->bodyPartsPos[DEKUBABA_BODYPART_1 + i]);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDekubaba_DrawStemBasePruned(EnDekubaba* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Matrix_RotateZYX(this->stemSectionAngle[2], this->actor.shape.rot.y, 0, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStemBaseDL);

    Collider_UpdateSpheres(55, &this->collider);
    Collider_UpdateSpheres(56, &this->collider);

    Matrix_MultZero(&this->bodyPartsPos[DEKUBABA_BODYPART_3]);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDekubaba_DrawShadow(EnDekubaba* this, PlayState* play) {
    MtxF mf;
    f32 horizontalScale;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL44_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);

    func_800C0094(this->boundFloor, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, &mf);
    Matrix_Mult(&mf, MTXMODE_NEW);
    horizontalScale = this->size * 0.15f;
    Matrix_Scale(horizontalScale, 1.0f, horizontalScale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gCircleShadowDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDekubaba_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnDekubaba* this = (EnDekubaba*)thisx;

    if (limbIndex == DEKUBABA_LIMB_ROOT) {
        Collider_UpdateSpheres(limbIndex, &this->collider);
    }
}

void EnDekubaba_Draw(Actor* thisx, PlayState* play) {
    EnDekubaba* this = (EnDekubaba*)thisx;
    f32 scale;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Math_Vec3f_Copy(&this->bodyPartsPos[DEKUBABA_BODYPART_0], &this->actor.world.pos);

    if (this->actionFunc != EnDekubaba_DeadStickDrop) {
        SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, EnDekubaba_PostLimbDraw,
                          &this->actor);
        if (this->actionFunc == EnDekubaba_Wait) {
            EnDekubaba_DrawStemRetracted(this, play);
        } else {
            EnDekubaba_DrawStemExtended(this, play);
        }

        scale = this->size * 0.01f;
        Matrix_Translate(this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, MTXMODE_NEW);
        Matrix_RotateYS(this->actor.home.rot.y, MTXMODE_APPLY);
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gDekuBabaBaseLeavesDL);

        if (this->actionFunc == EnDekubaba_PrunedSomersaultDie) {
            EnDekubaba_DrawStemBasePruned(this, play);
        }
        if (this->boundFloor != NULL) {
            EnDekubaba_DrawShadow(this, play);
        }

        // Display solid until 40 frames left, then blink until killed.
    } else if ((this->timer > 40) || ((this->timer % 2) != 0)) {
        Matrix_Translate(0.0f, 0.0f, 200.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStickDropDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, DEKUBABA_BODYPART_MAX,
                            this->drawDmgEffScale * this->size, this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha,
                            this->drawDmgEffType);
}
