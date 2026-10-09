/*
 * File: z_en_karebaba.c
 * Overlay: ovl_En_Karebaba
 * Description: Wilted Deku Baba and Mini Baba
 */

#include "z_en_karebaba.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void EnKarebaba_Init(Actor* thisx, PlayState* play);
void EnKarebaba_Destroy(Actor* thisx, PlayState* play);
void EnKarebaba_Update(Actor* thisx, PlayState* play2);
void EnKarebaba_Draw(Actor* thisx, PlayState* play);

void EnKarebaba_SetupGrow(EnKarebaba* this);
void EnKarebaba_Grow(EnKarebaba* this, PlayState* play);
void EnKarebaba_SetupIdle(EnKarebaba* this);
void EnKarebaba_Idle(EnKarebaba* this, PlayState* play);
void EnKarebaba_SetupAwaken(EnKarebaba* this);
void EnKarebaba_Awaken(EnKarebaba* this, PlayState* play);
void EnKarebaba_SetupUpright(EnKarebaba* this);
void EnKarebaba_Upright(EnKarebaba* this, PlayState* play);
void EnKarebaba_SetupSpin(EnKarebaba* this);
void EnKarebaba_Spin(EnKarebaba* this, PlayState* play);
void EnKarebaba_SetupDying(EnKarebaba* this);
void EnKarebaba_Dying(EnKarebaba* this, PlayState* play);
void EnKarebaba_SetupShrinkDie(EnKarebaba* this);
void EnKarebaba_ShrinkDie(EnKarebaba* this, PlayState* play);
void EnKarebaba_SetupDeadItemDrop(EnKarebaba* this, PlayState* play);
void EnKarebaba_DeadItemDrop(EnKarebaba* this, PlayState* play);
void EnKarebaba_SetupRetract(EnKarebaba* this);
void EnKarebaba_Retract(EnKarebaba* this, PlayState* play);
void EnKarebaba_SetupDead(EnKarebaba* this);
void EnKarebaba_Dead(EnKarebaba* this, PlayState* play);

ActorProfile En_Karebaba_Profile = {
    /**/ ACTOR_EN_KAREBABA,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_DEKUBABA,
    /**/ sizeof(EnKarebaba),
    /**/ EnKarebaba_Init,
    /**/ EnKarebaba_Destroy,
    /**/ EnKarebaba_Update,
    /**/ EnKarebaba_Draw,
};

static ColliderCylinderInit sHurtCylinderInit = {
    {
        COL_MATERIAL_HARD,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 7, 25, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sAttackCylinderInit = {
    {
        COL_MATERIAL_HARD,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_WOOD,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 4, 25, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 15, 80, MASS_HEAVY };

typedef enum {
    /* 0 */ KAREBABA_DMGEFF_NONE,
    /* 2 */ KAREBABA_DMGEFF_FIRE = 2,
    /* 3 */ KAREBABA_DMGEFF_ICE,
    /* 4 */ KAREBABA_DMGEFF_LIGHT,
    /* 5 */ KAREBABA_DMGEFF_ELECTRIC
} DekuBabaDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, KAREBABA_DMGEFF_NONE),
    /* Deku Stick     */ DMG_ENTRY(3, KAREBABA_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(1, KAREBABA_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, KAREBABA_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, KAREBABA_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, KAREBABA_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, KAREBABA_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(1, KAREBABA_DMGEFF_NONE),
    /* Goron punch    */ DMG_ENTRY(2, KAREBABA_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, KAREBABA_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, KAREBABA_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(1, KAREBABA_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, KAREBABA_DMGEFF_ICE),
    /* Light arrow    */ DMG_ENTRY(2, KAREBABA_DMGEFF_LIGHT),
    /* Goron spikes   */ DMG_ENTRY(1, KAREBABA_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(1, KAREBABA_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, KAREBABA_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, KAREBABA_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, KAREBABA_DMGEFF_NONE),
    /* Zora barrier   */ DMG_ENTRY(1, KAREBABA_DMGEFF_ELECTRIC),
    /* Normal shield  */ DMG_ENTRY(0, KAREBABA_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, KAREBABA_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, KAREBABA_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, KAREBABA_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, KAREBABA_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, KAREBABA_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, KAREBABA_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, KAREBABA_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, KAREBABA_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, KAREBABA_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, KAREBABA_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, KAREBABA_DMGEFF_NONE),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 2500, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_1, ICHAIN_STOP),
};

void EnKarebaba_Init(Actor* thisx, PlayState* play) {
    EnKarebaba* this = (EnKarebaba*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 22.0f);
    SkelAnime_Init(play, &this->skelAnime, &gDekuBabaSkel, &gDekuBabaFastChompAnim, this->jointTable, this->morphTable,
                   DEKUBABA_LIMB_MAX);

    Collider_InitAndSetCylinder(play, &this->hurtCollider, &this->actor, &sHurtCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->hurtCollider);
    Collider_InitAndSetCylinder(play, &this->attackCollider, &this->actor, &sAttackCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->attackCollider);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    this->boundFloor = NULL;
    if (this->actor.params == KAREBABA_MINI) {
        this->actor.hintId = TATL_HINT_ID_MINI_BABA;
    } else {
        this->actor.hintId = TATL_HINT_ID_WILTED_DEKU_BABA;
    }

    if (this->actor.params == ENKAREBABA_0) {
        EnKarebaba_SetupGrow(this);
    } else {
        EnKarebaba_SetupIdle(this);
    }
}

void EnKarebaba_Destroy(Actor* thisx, PlayState* play) {
    EnKarebaba* this = (EnKarebaba*)thisx;

    Collider_DestroyCylinder(play, &this->hurtCollider);
    Collider_DestroyCylinder(play, &this->attackCollider);
}

void EnKarebaba_SpawnIceEffects(EnKarebaba* this, PlayState* play) {
    s32 bodyPartsCount;

    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffAlpha = 0.0f;

        if (this->actor.params == KAREBABA_MINI) {
            bodyPartsCount = 1;
        } else {
            bodyPartsCount = KAREBABA_BODYPART_MAX;
        }
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, bodyPartsCount, 4, 0.3f, 0.2f);
    }
}

void EnKarebaba_SetDamageEffects(EnKarebaba* this, PlayState* play) {
    if (this->actor.colChkInfo.damageEffect == KAREBABA_DMGEFF_FIRE) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffScale = 0.75f;
        this->drawDmgEffAlpha = 3.0f;
    } else if (this->actor.colChkInfo.damageEffect == KAREBABA_DMGEFF_LIGHT) {
        this->drawDmgEffScale = 0.75f;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
        this->drawDmgEffAlpha = 3.0f;

        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->hurtCollider.elem.acDmgInfo.hitPos.x,
                    this->hurtCollider.elem.acDmgInfo.hitPos.y, this->hurtCollider.elem.acDmgInfo.hitPos.z, 0, 0, 0,
                    CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_LIGHT_RAYS));
    } else if (this->actor.colChkInfo.damageEffect == KAREBABA_DMGEFF_ICE) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
        this->drawDmgEffScale = 0.75f;
        this->drawDmgEffAlpha = 1.0f;
        this->drawDmgEffFrozenSteamScale = 1.125f;
    } else if (this->actor.colChkInfo.damageEffect == KAREBABA_DMGEFF_ELECTRIC) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_MEDIUM;
        this->drawDmgEffScale = 0.75f;
        this->drawDmgEffAlpha = 3.0f;
    }
}

void EnKarebaba_ResetColliders(EnKarebaba* this) {
    this->hurtCollider.dim.radius = 7;
    this->hurtCollider.dim.height = 25;
    this->hurtCollider.base.colMaterial = COL_MATERIAL_HARD;
    this->hurtCollider.base.acFlags |= AC_HARD;
    this->attackCollider.dim.height = 25;
}

void EnKarebaba_SetupGrow(EnKarebaba* this) {
    Actor_SetScale(&this->actor, 0.0f);
    this->actor.shape.rot.x = -0x4000;
    this->timer = 0;
    this->actor.world.pos.y = this->actor.home.pos.y + 14.0f;
    this->actionFunc = EnKarebaba_Grow;
}

void EnKarebaba_Grow(EnKarebaba* this, PlayState* play) {
    f32 scale;

    this->timer++;
    scale = this->timer * 0.05f;
    Actor_SetScale(&this->actor, 0.005f * scale);
    this->actor.world.pos.y = this->actor.home.pos.y + (14.0f * scale);
    if (this->timer == 20) {
        EnKarebaba_SetupIdle(this);
    }
}

void EnKarebaba_SetupIdle(EnKarebaba* this) {
    Actor_SetScale(&this->actor, 0.005f);
    this->actor.shape.rot.x = -0x4000;
    this->actor.world.pos.y = this->actor.home.pos.y + 14.0f;
    this->actionFunc = EnKarebaba_Idle;
}

void EnKarebaba_Idle(EnKarebaba* this, PlayState* play) {
    if ((this->actor.xzDistToPlayer < 200.0f) && (fabsf(this->actor.playerHeightRel) < 30.0f)) {
        EnKarebaba_SetupAwaken(this);
    }
}

void EnKarebaba_SetupAwaken(EnKarebaba* this) {
    Animation_Change(&this->skelAnime, &gDekuBabaFastChompAnim, 4.0f, 0.0f,
                     Animation_GetLastFrame(&gDekuBabaFastChompAnim), ANIMMODE_LOOP, -3.0f);
    Actor_PlaySfx(&this->actor, NA_SE_EN_DEKU_WAKEUP);
    this->actionFunc = EnKarebaba_Awaken;
}

void EnKarebaba_Awaken(EnKarebaba* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Math_StepToF(&this->actor.scale.x, 0.01f, 0.0005f) && (this->actor.params == KAREBABA_MINI)) {
        EnKarebaba_SetupUpright(this);
    }

    this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;

    if ((this->actor.params == ENKAREBABA_1) &&
        Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y + 60.0f, 5.0f)) {
        EnKarebaba_SetupUpright(this);
    }

    this->actor.shape.rot.y += 0x10000 / 10;
    EffectSsHahen_SpawnBurst(play, &this->actor.home.pos, 3.0f, 0, 12, 5, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);
}

void EnKarebaba_SetupUpright(EnKarebaba* this) {
    if (this->actionFunc != EnKarebaba_Spin) {
        Actor_SetScale(&this->actor, 0.01f);

        this->hurtCollider.base.colMaterial = COL_MATERIAL_HIT6;
        this->hurtCollider.base.acFlags &= ~AC_HARD;
        this->hurtCollider.dim.radius = 15;

        if (this->actor.params == ENKAREBABA_1) {
            this->hurtCollider.dim.height = 80;
            this->attackCollider.dim.height = 80;
        } else {
            this->hurtCollider.dim.height = 35;
            this->attackCollider.dim.height = 35;
        }
    }

    this->timer = 40;
    this->actionFunc = EnKarebaba_Upright;
}

void EnKarebaba_Upright(EnKarebaba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }

    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 12.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_MIZUBABA1_MOUTH);
    }

    if (this->hurtCollider.base.acFlags & AC_HIT) {
        EnKarebaba_SetDamageEffects(this, play);
        if (this->actor.params == ENKAREBABA_1) {
            EnKarebaba_SetupDying(this);
        } else {
            EnKarebaba_SetupShrinkDie(this);
        }
        Enemy_StartFinishingBlow(play, &this->actor);
    } else if (Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) > 240.0f) {
        EnKarebaba_SetupRetract(this);
    } else if ((this->timer == 0) && (this->actor.params == ENKAREBABA_1)) {
        EnKarebaba_SetupSpin(this);
    }
}

void EnKarebaba_SetupSpin(EnKarebaba* this) {
    this->timer = 40;
    this->actionFunc = EnKarebaba_Spin;
}

void EnKarebaba_Spin(EnKarebaba* this, PlayState* play) {
    s32 temp; // Used for cylinder radius and shape rotations
    f32 headDistHorizontal;

    if (this->timer != 0) {
        this->timer--;
    }

    SkelAnime_Update(&this->skelAnime);

    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 12.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_MIZUBABA1_MOUTH);
    }

    temp = 20 - this->timer;
    temp = 20 - ABS_ALT(temp);
    if (temp > 10) {
        temp = 10;
    }

    this->attackCollider.dim.radius = sAttackCylinderInit.dim.radius + (temp * 2);
    this->actor.shape.rot.x = 0xC000 - temp * 0x100;
    this->actor.shape.rot.y += temp * 0x2C0;
    this->actor.world.pos.y = (Math_SinS(this->actor.shape.rot.x) * -60.0f) + this->actor.home.pos.y;

    headDistHorizontal = Math_CosS(this->actor.shape.rot.x) * 60.0f;
    this->actor.world.pos.x = (Math_SinS(this->actor.shape.rot.y) * headDistHorizontal) + this->actor.home.pos.x;
    this->actor.world.pos.z = (Math_CosS(this->actor.shape.rot.y) * headDistHorizontal) + this->actor.home.pos.z;

    if (this->hurtCollider.base.acFlags & AC_HIT) {
        EnKarebaba_SetDamageEffects(this, play);
        EnKarebaba_SetupDying(this);
        Enemy_StartFinishingBlow(play, &this->actor);
    } else if (this->timer == 0) {
        EnKarebaba_SetupUpright(this);
    }
}

void EnKarebaba_SetupDying(EnKarebaba* this) {
    if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->timer = 0;
        this->actor.gravity = -0.8f;
        this->actor.velocity.y = 4.0f;
        this->actor.world.rot.y = BINANG_ROT180(this->actor.shape.rot.y);
        this->actor.speed = 3.0f;
    } else {
        this->timer = 3;
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_DEKU_JR_DEAD);
    this->actor.flags |= (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED);
    this->actionFunc = EnKarebaba_Dying;
}

void EnKarebaba_Dying(EnKarebaba* this, PlayState* play) {
    s32 i;
    Vec3f dustPos;
    f32 xIncr;
    f32 zIncr;
    f32 yIncr;

    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->timer--;
        if (this->timer == 0) {
            this->actor.gravity = -0.8f;
            this->actor.speed = 3.0f;
            this->actor.velocity.y = 4.0f;
            this->actor.world.rot.y = BINANG_ROT180(this->actor.shape.rot.y);
            EnKarebaba_SpawnIceEffects(this, play);
        }
    } else {
        Math_StepToF(&this->actor.speed, 0.0f, 0.1f);

        if (this->timer == 0) {
            Math_ScaledStepToS(&this->actor.shape.rot.x, 0x4800, 0x71C);
            EffectSsHahen_SpawnBurst(play, &this->actor.world.pos, 3.0f, 0, 12, 5, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);

            if ((this->actor.scale.x > 0.005f) && ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) ||
                                                   (this->actor.bgCheckFlags & BGCHECKFLAG_WALL))) {
                this->actor.scale.z = 0.0f;
                this->actor.scale.y = 0.0f;
                this->actor.scale.x = 0.0f;
                this->actor.speed = 0.0f;
                this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
                EffectSsHahen_SpawnBurst(play, &this->actor.world.pos, 3.0f, 0, 12, 5, 15, HAHEN_OBJECT_DEFAULT, 10,
                                         NULL);
            }

            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_ATTACK);
                this->timer = 1;
            }
        } else if (this->timer == 1) {
            Math_Vec3f_Copy(&dustPos, &this->actor.world.pos);

            yIncr = Math_SinS(this->actor.shape.rot.x) * 20.0f;
            xIncr = -20.0f * Math_CosS(this->actor.shape.rot.x) * Math_SinS(this->actor.shape.rot.y);
            zIncr = -20.0f * Math_CosS(this->actor.shape.rot.x) * Math_CosS(this->actor.shape.rot.y);

            for (i = 0; i < 4; i++) {
                func_800B1210(play, &dustPos, &gZeroVec3f, &gZeroVec3f, 500, 50);
                dustPos.x += xIncr;
                dustPos.y += yIncr;
                dustPos.z += zIncr;
            }

            func_800B1210(play, &this->actor.home.pos, &gZeroVec3f, &gZeroVec3f, 500, 100);
            EnKarebaba_SetupDeadItemDrop(this, play);
        }
    }
}

void EnKarebaba_SetupShrinkDie(EnKarebaba* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_DEKU_JR_DEAD);
    this->actor.flags |= (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->timer = 3;
    }
    this->actionFunc = EnKarebaba_ShrinkDie;
}

void EnKarebaba_ShrinkDie(EnKarebaba* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->timer--;
        if (this->timer == 0) {
            EnKarebaba_SpawnIceEffects(this, play);
        }
    } else {
        if (Math_StepToF(&this->actor.scale.x, 0.0f, 0.0005f)) {
            Item_DropCollectible(play, &this->actor.world.pos, ITEM00_DEKU_NUTS_1);
            EnKarebaba_SetupDead(this);
        } else {
            EffectSsHahen_SpawnBurst(play, &this->actor.world.pos, 3.0f, 0, 12, 5, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);
            Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y, 1.0f);
        }

        this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
    }
}

void EnKarebaba_SetupDeadItemDrop(EnKarebaba* this, PlayState* play) {
    Actor_SetScale(&this->actor, 0.03f);
    this->actor.shape.rot.x -= 0x4000;
    this->actor.shape.yOffset = 1000.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.shape.shadowScale = 3.0f;
    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_MISC);
    this->timer = 200;
    this->actor.flags &= ~ACTOR_FLAG_DRAW_CULLING_DISABLED;
    this->drawDmgEffAlpha = 0.0f;
    this->actionFunc = EnKarebaba_DeadItemDrop;
}

void EnKarebaba_DeadItemDrop(EnKarebaba* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (Actor_HasParent(&this->actor, play) || (this->timer == 0)) {
        EnKarebaba_SetupDead(this);
    } else {
        Actor_OfferGetItemNearby(&this->actor, play, GI_DEKU_STICKS_1);
    }
}

void EnKarebaba_SetupRetract(EnKarebaba* this) {
    Animation_Change(&this->skelAnime, &gDekuBabaFastChompAnim, -3.0f, Animation_GetLastFrame(&gDekuBabaFastChompAnim),
                     0.0f, ANIMMODE_ONCE, -3.0f);
    EnKarebaba_ResetColliders(this);
    this->actionFunc = EnKarebaba_Retract;
}

void EnKarebaba_Retract(EnKarebaba* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Math_StepToF(&this->actor.scale.x, 0.005f, 0.0005f) && (this->actor.params == KAREBABA_MINI)) {
        EnKarebaba_SetupIdle(this);
    }

    this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;

    if ((this->actor.params == ENKAREBABA_1) &&
        Math_StepToF(&this->actor.world.pos.y, this->actor.home.pos.y + 14.0f, 5.0f)) {
        EnKarebaba_SetupIdle(this);
    }

    this->actor.shape.rot.y += 0x10000 / 10;
    EffectSsHahen_SpawnBurst(play, &this->actor.home.pos, 3.0f, 0, 12, 5, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);
}

void EnKarebaba_SetupDead(EnKarebaba* this) {
    Animation_Change(&this->skelAnime, &gDekuBabaFastChompAnim, 0.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
    EnKarebaba_ResetColliders(this);
    this->actor.shape.rot.x = -0x4000;
    this->timer = 200;
    this->actor.parent = NULL;
    this->actor.shape.shadowScale = 0.0f;
    this->drawDmgEffAlpha = 0.0f;
    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
    this->actionFunc = EnKarebaba_Dead;
}

void EnKarebaba_Regrow(EnKarebaba* this, PlayState* play) {
    f32 scale;

    this->timer++;
    scale = this->timer * 0.05f;
    Actor_SetScale(&this->actor, 0.005f * scale);
    this->actor.world.pos.y = this->actor.home.pos.y + (14.0f * scale);

    if (this->timer == 20) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
        if (this->actor.params == ENKAREBABA_1) {
            Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
        }
        EnKarebaba_SetupIdle(this);
    }
}

void EnKarebaba_SetupRegrow(EnKarebaba* this) {
    this->actor.shape.yOffset = 0.0f;
    this->actor.shape.shadowScale = 22.0f;
    this->attackCollider.dim.radius = sAttackCylinderInit.dim.radius;
    Actor_SetScale(&this->actor, 0.0f);
    this->actionFunc = EnKarebaba_Regrow;
}

void EnKarebaba_Dead(EnKarebaba* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        EnKarebaba_SetupRegrow(this);
    }
}

void EnKarebaba_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnKarebaba* this = (EnKarebaba*)thisx;
    f32 max;

    this->actionFunc(this, play);

    if (this->drawDmgEffAlpha > 0.0f) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.375f;
            this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.75f);
        } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.75f, 0.75f / 40)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }

    if (this->actionFunc != EnKarebaba_Dead) {
        if (this->actionFunc == EnKarebaba_Dying) {
            Actor_MoveWithGravity(&this->actor);
            Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 15.0f, 10.0f,
                                    UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
        } else {
            Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
            if (this->boundFloor == 0) {
                this->boundFloor = this->actor.floorPoly;
            }
        }

        if ((this->actionFunc != EnKarebaba_Dying) && (this->actionFunc != EnKarebaba_ShrinkDie) &&
            (this->actionFunc != EnKarebaba_DeadItemDrop)) {
            if ((this->actionFunc != EnKarebaba_Regrow) && (this->actionFunc != EnKarebaba_Grow)) {
                CollisionCheck_SetAT(play, &play->colChkCtx, &this->attackCollider.base);
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->hurtCollider.base);
            }

            CollisionCheck_SetOC(play, &play->colChkCtx, &this->attackCollider.base);
            Actor_SetFocus(&this->actor, (this->actor.scale.x * 10.0f) / 0.01f);

            max = this->actor.home.pos.y + 40.0f;
            this->actor.focus.pos.x = this->actor.home.pos.x;
            this->actor.focus.pos.y = CLAMP_MAX(this->actor.focus.pos.y, max);
            this->actor.focus.pos.z = this->actor.home.pos.z;
        }
    }
}

void EnKarebaba_DrawShadow(EnKarebaba* this, PlayState* play) {
    MtxF mf;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL44_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);

    func_800C0094(this->boundFloor, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, &mf);
    Matrix_Mult(&mf, MTXMODE_NEW);
    Matrix_Scale(0.15f, 1.0f, 0.15f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gCircleShadowDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnKarebaba_Draw(Actor* thisx, PlayState* play) {
    static Color_RGBA8 sFogColor = { 0, 0, 0, 0 };
    static Gfx* sStemDLists[] = { gDekuBabaStemTopDL, gDekuBabaStemMiddleDL, gDekuBabaStemBaseDL };
    EnKarebaba* this = (EnKarebaba*)thisx;
    s32 i;
    s32 stemSections;
    s16 bodyPartsCount;
    f32 scale = 0.01f;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Math_Vec3f_Copy(this->bodyPartsPos, &this->actor.world.pos);

    if (this->actionFunc == EnKarebaba_DeadItemDrop) {
        if ((this->timer > 40) || (this->timer & 1)) {
            FrameInterpolation_RecordOpenChild(this, 0);
            Matrix_Translate(0.0f, 0.0f, 200.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStickDropDL);
            FrameInterpolation_RecordCloseChild();
        }
    } else if (this->actionFunc != EnKarebaba_Dead) {
        func_800AE2A0(play, &sFogColor, 1, 2);
        SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, NULL);
        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);

        if ((this->actionFunc == EnKarebaba_Regrow) || (this->actionFunc == EnKarebaba_ShrinkDie) ||
            (this->actionFunc == EnKarebaba_Grow)) {
            scale = this->timer * 0.0005f;
        }

        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        Matrix_RotateZYX(this->actor.shape.rot.x, this->actor.shape.rot.y, 0, MTXMODE_APPLY);

        if (this->actor.params == KAREBABA_MINI) {
            stemSections = 1;
        } else if (this->actionFunc == EnKarebaba_Dying) {
            stemSections = 2;
        } else {
            stemSections = 3;
        }

        for (i = 0; i < stemSections; i++) {
            Matrix_Translate(0.0f, 0.0f, -2000.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, sStemDLists[i]);

            Matrix_MultZero(&this->bodyPartsPos[KAREBABA_BODYPART_1 + i]);
            if ((i == 0) && (this->actionFunc == EnKarebaba_Dying)) {
                Matrix_MultZero(&this->actor.focus.pos);
            }
        }

        func_800AE5A0(play);
    }

    func_800AE2A0(play, &sFogColor, 1, 2);
    Matrix_Translate(this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, MTXMODE_NEW);

    if (this->actionFunc != EnKarebaba_Grow) {
        scale = 0.01f;
    }

    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_RotateYS(this->actor.home.rot.y, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gDekuBabaBaseLeavesDL);

    if (this->actionFunc == EnKarebaba_Dying) {
        Matrix_RotateZYX(-0x4000, this->actor.shape.rot.y - this->actor.home.rot.y, 0, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStemBaseDL);

        Matrix_MultZero(&this->bodyPartsPos[KAREBABA_BODYPART_3]);
    }

    func_800AE5A0(play);

    if (this->actor.params == KAREBABA_MINI) {
        bodyPartsCount = 1;
    } else {
        bodyPartsCount = KAREBABA_BODYPART_MAX;
    }

    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, bodyPartsCount, this->drawDmgEffScale,
                            this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);

    if (this->boundFloor != 0) {
        EnKarebaba_DrawShadow(this, play);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
