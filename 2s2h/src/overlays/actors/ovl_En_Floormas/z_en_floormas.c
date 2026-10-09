/*
 * File: z_en_floormas.c
 * Overlay: ovl_En_Floormas
 * Description: Floormaster
 */

#include "z_en_floormas.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

void EnFloormas_Init(Actor* thisx, PlayState* play2);
void EnFloormas_Destroy(Actor* thisx, PlayState* play);
void EnFloormas_Update(Actor* thisx, PlayState* play);
void EnFloormas_Draw(Actor* thisx, PlayState* play);

void func_808D0B08(EnFloormas* this);
void func_808D0B50(EnFloormas* this, PlayState* play);
void func_808D0C14(EnFloormas* this);
void func_808D0C58(EnFloormas* this, PlayState* play);
void func_808D0CE4(EnFloormas* this);
void func_808D0D70(EnFloormas* this, PlayState* play);
void func_808D0ECC(EnFloormas* this);
void func_808D0F14(EnFloormas* this, PlayState* play);
void func_808D0F50(EnFloormas* this);
void func_808D0F80(EnFloormas* this, PlayState* play);
void func_808D108C(EnFloormas* this);
void func_808D11BC(EnFloormas* this, PlayState* play);
void func_808D1380(EnFloormas* this, PlayState* play);
void func_808D1458(EnFloormas* this, PlayState* play);
void func_808D161C(EnFloormas* this);
void func_808D1650(EnFloormas* this, PlayState* play);
void func_808D1740(EnFloormas* this);
void func_808D17EC(EnFloormas* this, PlayState* play);
void func_808D19D4(EnFloormas* this);
void func_808D1B44(EnFloormas* this, PlayState* play);
void func_808D1BCC(EnFloormas* this);
void func_808D1C1C(EnFloormas* this, PlayState* play);
void func_808D1D0C(EnFloormas* this);
void func_808D1D6C(EnFloormas* this, PlayState* play);
void func_808D1ED4(EnFloormas* this, PlayState* play);
void func_808D1F7C(EnFloormas* this, PlayState* play);
void func_808D1FD4(EnFloormas* this);
void func_808D2040(EnFloormas* this, PlayState* play);
void func_808D217C(EnFloormas* this, Player* player);
void func_808D22C8(EnFloormas* this, PlayState* play);
void func_808D2484(EnFloormas* this);
void func_808D24F0(EnFloormas* this, PlayState* play);
void func_808D2700(EnFloormas* this);
void func_808D2764(EnFloormas* this, PlayState* play);
void func_808D2A20(EnFloormas* this);
void func_808D2AA8(EnFloormas* this, PlayState* play);
void func_808D2AB8(EnFloormas* this);
void func_808D2AF4(EnFloormas* this, PlayState* play);
void func_808D2C08(EnFloormas* this, PlayState* play);
void func_808D2CDC(EnFloormas* this);
void func_808D2D30(EnFloormas* this, PlayState* play);
void func_808D2DC0(EnFloormas* this, PlayState* play);
void func_808D3754(Actor* thisx, PlayState* play);

ActorProfile En_Floormas_Profile = {
    /**/ ACTOR_EN_FLOORMAS,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_WALLMASTER,
    /**/ sizeof(EnFloormas),
    /**/ EnFloormas_Init,
    /**/ EnFloormas_Destroy,
    /**/ EnFloormas_Update,
    /**/ EnFloormas_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x04, 0x10 },
        { 0xF7EFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_HARD,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 25, 40, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 3, 30, 40, 150 };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x1),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0xF),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(2, 0x2),
    /* Ice arrow      */ DMG_ENTRY(1, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x1),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(2, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x1),
    /* Zora barrier   */ DMG_ENTRY(0, 0x5),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0xE),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_FLOORMASTER, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 5500, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_STOP),
};

void EnFloormas_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnFloormas* this = (EnFloormas*)thisx;
    s32 pad;
    s32 params;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 50.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gWallmasterSkel, &gWallmasterIdleAnim, this->jointTable,
                       this->morphTable, WALLMASTER_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    this->unk_192 = -1600;
    params = ENFLOORMAS_GET_8000(&this->actor);
    this->actor.params = ENFLOORMAS_GET_7FFF(&this->actor);

    if (params != 0) {
        this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
        this->actor.draw = func_808D3754;
    }

    if (this->actor.params == ENFLOORMAS_GET_7FFF_10) {
        this->actor.draw = NULL;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->actionFunc = func_808D2AA8;
        return;
    }

    this->actor.parent = Actor_SpawnAsChildAndCutscene(
        &play->actorCtx, play, ACTOR_EN_FLOORMAS, this->actor.world.pos.x, this->actor.world.pos.y,
        this->actor.world.pos.z, 0, 0, 0, params + 0x10, CS_ID_NONE, this->actor.halfDaysBits, NULL);
    if (this->actor.parent == NULL) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.child = Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_EN_FLOORMAS, this->actor.world.pos.x,
                                                      this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0,
                                                      params + 0x10, CS_ID_NONE, this->actor.halfDaysBits, NULL);
    if (this->actor.child == NULL) {
        Actor_Kill(this->actor.parent);
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.parent->child = &this->actor;
    this->actor.parent->parent = this->actor.child;
    this->actor.child->parent = &this->actor;
    this->actor.child->child = this->actor.parent;
    func_808D0B08(this);
}

void EnFloormas_Destroy(Actor* thisx, PlayState* play) {
    EnFloormas* this = (EnFloormas*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_808D08D0(EnFloormas* this) {
    this->collider.base.colMaterial = COL_MATERIAL_HARD;
    this->collider.base.acFlags |= AC_HARD;
    this->collider.elem.acDmgInfo.dmgFlags &= ~0x8000;
    this->unk_190 = 40;
}

void func_808D0908(EnFloormas* this) {
    this->collider.base.colMaterial = COL_MATERIAL_HIT0;
    this->collider.base.acFlags &= ~AC_HARD;
    this->collider.elem.acDmgInfo.dmgFlags |= 0x8000;
    this->unk_190 = 0;
}

void func_808D0930(EnFloormas* this, PlayState* play) {
    if (this->actor.scale.x > 0.009f) {
        this->actor.shape.rot.y = BINANG_ROT180(this->actor.yawTowardsPlayer);
        func_808D19D4((EnFloormas*)this->actor.child);
        func_808D19D4((EnFloormas*)this->actor.parent);
        func_808D19D4(this);
        Actor_PlaySfx(&this->actor, NA_SE_EN_KUSAMUSHI_HIDE);
    } else {
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x90);
        func_808D1ED4(this, play);
    }
}

void func_808D09CC(EnFloormas* this) {
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->drawDmgEffScale = 0.55f;
    this->drawDmgEffFrozenSteamScale = 825.0f * 0.001f;
    this->drawDmgEffAlpha = 1.0f;
    this->collider.base.colMaterial = COL_MATERIAL_HIT3;
    this->unk_18E = 80;
    this->actor.flags &= ~(ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 80);
}

void func_808D0A48(EnFloormas* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->collider.base.colMaterial = COL_MATERIAL_HIT0;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, ENFLOORMAS_BODYPART_MAX, 2,
                              this->actor.scale.x * 30.000002f, this->actor.scale.x * 20.0f);
        if (this->actor.scale.x > 0.009f) {
            this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
        } else {
            this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
        }
    }
}

void func_808D0B08(EnFloormas* this) {
    Animation_PlayOnce(&this->skelAnime, &gWallmasterIdleAnim);
    this->actor.speed = 0.0f;
    this->actionFunc = func_808D0B50;
}

void func_808D0B50(EnFloormas* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if ((this->actor.xzDistToPlayer < 400.0f) && !Actor_IsFacingPlayer(&this->actor, 0x4000)) {
            this->unk_190 = this->actor.yawTowardsPlayer;
            func_808D108C(this);
        } else if ((this->actor.xzDistToPlayer < 280.0f) && Actor_IsFacingPlayer(&this->actor, 0x2000)) {
            func_808D1380(this, play);
        } else {
            func_808D0C14(this);
        }
    }
}

void func_808D0C14(EnFloormas* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gWallmasterStandUpAnim, -3.0f);
    this->actionFunc = func_808D0C58;
}

void func_808D0C58(EnFloormas* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->actor.scale.x > 0.009f) {
            func_808D0CE4(this);
        } else if (this->actor.params == ENFLOORMAS_GET_7FFF_20) {
            func_808D1D0C(this);
        } else {
            func_808D1BCC(this);
        }
    }
}

void func_808D0CE4(EnFloormas* this) {
    if (this->actionFunc != func_808D0F80) {
        Animation_PlayLoopSetSpeed(&this->skelAnime, &gWallmasterWalkAnim, 1.5f);
    } else {
        this->skelAnime.playSpeed = 1.5f;
    }

    this->unk_18E = Rand_S16Offset(2, 4);
    this->actor.speed = 1.5f;
    this->actionFunc = func_808D0D70;
}

void func_808D0D70(EnFloormas* this, PlayState* play) {
    s32 onAnimFirstFrame;

    SkelAnime_Update(&this->skelAnime);
    onAnimFirstFrame = Animation_OnFrame(&this->skelAnime, 0.0f);
    if (onAnimFirstFrame && (this->unk_18E != 0)) {
        this->unk_18E--;
    }

    if (onAnimFirstFrame || Animation_OnFrame(&this->skelAnime, 12.0f) || Animation_OnFrame(&this->skelAnime, 24.0f) ||
        Animation_OnFrame(&this->skelAnime, 36.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_FALL_WALK);
    }

    if ((this->actor.xzDistToPlayer < 320.0f) && Actor_IsFacingPlayer(&this->actor, 0x4000)) {
        func_808D0F50(this);
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->unk_190 = this->actor.wallYaw;
        func_808D108C(this);
    } else if ((this->actor.xzDistToPlayer < 400.0f) && !Actor_IsFacingPlayer(&this->actor, 0x4000)) {
        this->unk_190 = this->actor.yawTowardsPlayer;
        func_808D108C(this);
    } else if (this->unk_18E == 0) {
        func_808D0ECC(this);
    }
}

void func_808D0ECC(EnFloormas* this) {
    Animation_PlayOnce(&this->skelAnime, &gWallmasterStopWalkAnim);
    this->actor.speed = 0.0f;
    this->actionFunc = func_808D0F14;
}

void func_808D0F14(EnFloormas* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        func_808D0B08(this);
    }
}

void func_808D0F50(EnFloormas* this) {
    this->unk_18E = 0;
    this->actor.speed = 5.0f;
    this->skelAnime.playSpeed = 3.0f;
    this->actionFunc = func_808D0F80;
}

void func_808D0F80(EnFloormas* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 12.0f) ||
        Animation_OnFrame(&this->skelAnime, 24.0f) || Animation_OnFrame(&this->skelAnime, 36.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_FALL_WALK);
    }

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x71C);
    if ((this->actor.xzDistToPlayer < 280.0f) && Actor_IsFacingPlayer(&this->actor, 0x2000) &&
        !(this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
        func_808D1380(this, play);
    } else if (this->actor.xzDistToPlayer > 400.0f) {
        func_808D0CE4(this);
    }
}

void func_808D108C(EnFloormas* this) {
    s16 sp36 = this->unk_190 - this->actor.shape.rot.y;

    this->actor.speed = 0.0f;
    if (sp36 > 0) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gFloormasterTurnAnim, -3.0f);
    } else {
        Animation_Change(&this->skelAnime, &gFloormasterTurnAnim, -1.0f, Animation_GetLastFrame(&gFloormasterTurnAnim),
                         0.0f, ANIMMODE_ONCE, -3.0f);
    }

    if (this->actor.scale.x > 0.009f) {
        this->unk_190 = sp36 * (1.0f / 15.0f);
    } else {
        this->skelAnime.playSpeed *= 2.0f;
        this->unk_190 = sp36 * (2.0f / 15.0f);
    }

    this->actionFunc = func_808D11BC;
}

void func_808D11BC(EnFloormas* this, PlayState* play) {
    f32 curFrame;
    f32 sp30;
    f32 sp2C;

    if (SkelAnime_Update(&this->skelAnime)) {
        func_808D0C14(this);
    }

    if (((this->skelAnime.playSpeed > 0.0f) && Animation_OnFrame(&this->skelAnime, 21.0f)) ||
        ((this->skelAnime.playSpeed < 0.0f) && Animation_OnFrame(&this->skelAnime, 6.0f))) {
        if (this->actor.scale.x > 0.009f) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_FALL_WALK);
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_EN_FLOORMASTER_SM_WALK);
        }
    }

    curFrame = this->skelAnime.curFrame;
    if ((curFrame >= 7.0f) && (curFrame < 22.0f)) {
        sp30 = Math_SinS(this->actor.shape.rot.y + 0x4268);
        sp2C = Math_CosS(this->actor.shape.rot.y + 0x4268);
        this->actor.shape.rot.y += this->unk_190;
        this->actor.world.pos.x -= this->actor.scale.x * 2700.0f * (Math_SinS(this->actor.shape.rot.y + 0x4268) - sp30);
        this->actor.world.pos.z -= this->actor.scale.x * 2700.0f * (Math_CosS(this->actor.shape.rot.y + 0x4268) - sp2C);
    }
}

void func_808D1380(EnFloormas* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gWallmasterHoverAnim, 3.0f, 0.0f, Animation_GetLastFrame(&gWallmasterHoverAnim),
                     ANIMMODE_ONCE, -3.0f);
    this->actor.speed = 0.0f;
    this->actor.gravity = 0.0f;
    func_808D08D0(this);
    Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 15.0f, 6, 20.0f, 300, 100, true);
    Actor_PlaySfx(&this->actor, NA_SE_EN_FLOORMASTER_ATTACK);
    this->actionFunc = func_808D1458;
}

void func_808D1458(EnFloormas* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        func_808D161C(this);
    }

    this->actor.shape.rot.x += 0x140;
    this->actor.world.pos.y += 10.0f;
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0xAAA);
    Math_StepToS(&this->unk_192, 1200, 100);
}

void func_808D14DC(EnFloormas* this, PlayState* play) {
    Vec3f sp34;
    Vec3f sp28;

    sp34.x = this->actor.world.pos.x;
    sp34.z = this->actor.world.pos.z;
    sp34.y = this->actor.floorHeight;

    sp28.y = 2.0f;
    sp28.x = Math_SinS(this->actor.shape.rot.y + 0x6000) * 7.0f;
    sp28.z = Math_CosS(this->actor.shape.rot.y + 0x6000) * 7.0f;

    func_800B1210(play, &sp34, &sp28, &gZeroVec3f, 0x1C2, 0x64);
    sp28.x = Math_SinS(this->actor.shape.rot.y - 0x6000) * 7.0f;
    sp28.z = Math_CosS(this->actor.shape.rot.y - 0x6000) * 7.0f;
    func_800B1210(play, &sp34, &sp28, &gZeroVec3f, 0x1C2, 0x64);
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FLOORMASTER_SLIDING - SFX_FLAG);
}

void func_808D161C(EnFloormas* this) {
    this->unk_18E = 25;
    this->actor.gravity = -0.15f;
    this->actor.speed = 0.5f;
    this->actionFunc = func_808D1650;
}

void func_808D1650(EnFloormas* this, PlayState* play) {
    f32 temp_f0_2;

    if (this->unk_18E != 0) {
        this->unk_18E--;
    }

    Math_StepToF(&this->actor.speed, 15.0f, SQ(this->actor.speed) * (1.0f / 3.0f));
    Math_ScaledStepToS(&this->actor.shape.rot.x, -0x1680, 0x140);

    temp_f0_2 = this->actor.world.pos.y - this->actor.floorHeight;
    if (temp_f0_2 < 10.0f) {
        this->actor.world.pos.y = this->actor.floorHeight + 10.0f;
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = 0.0f;
    }

    if (temp_f0_2 < 12.0f) {
        func_808D14DC(this, play);
    }

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->unk_18E == 0)) {
        func_808D1740(this);
    }
}

void func_808D1740(EnFloormas* this) {
    Animation_Change(&this->skelAnime, &gWallmasterJumpAnim, 1.0f, 41.0f, 42.0f, ANIMMODE_ONCE, 5.0f);
    if ((this->actor.speed < 0.0f) || (this->actionFunc != func_808D1650)) {
        this->unk_18E = 30;
    } else {
        this->unk_18E = 45;
    }
    this->actor.gravity = -1.0f;
    this->actionFunc = func_808D17EC;
}

void func_808D17EC(EnFloormas* this, PlayState* play) {
    s32 sp24 = this->actor.bgCheckFlags & BGCHECKFLAG_GROUND;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        if (this->actor.params != ENFLOORMAS_GET_7FFF_40) {
            func_808D0908(this);
        }

        if (this->actor.velocity.y < -4.0f) {
            if (this->actor.scale.x > 0.009f) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_FALL_LAND);
            } else {
                Actor_PlaySfx(&this->actor, NA_SE_EN_FLOORMASTER_SM_LAND);
            }
        }
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->actor.speed = 0.0f;
    }

    if (sp24 != 0) {
        Math_StepToF(&this->actor.speed, 0.0f, 2.0f);
    }

    if ((this->actor.speed > 0.0f) && ((this->actor.world.pos.y - this->actor.floorHeight) < 12.0f)) {
        func_808D14DC(this, play);
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->unk_18E != 0) {
            this->unk_18E--;
        }

        if ((this->unk_18E == 0) && (sp24 != 0)) {
            if (this->skelAnime.endFrame < 45.0f) {
                this->skelAnime.endFrame = Animation_GetLastFrame(&gWallmasterJumpAnim);
            } else if (this->actor.params == ENFLOORMAS_GET_7FFF_40) {
                func_808D2700(this);
            } else {
                func_808D0C14(this);
                this->unk_194 = 50;
            }
        }
    }

    Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x140);
    Math_StepToS(&this->unk_192, -1600, 100);
}

void func_808D19D4(EnFloormas* this) {
    Actor* parent;

    this->actor.colorFilterTimer = 0;
    this->drawDmgEffAlpha = 0.0f;
    Actor_SetScale(&this->actor, 0.004f);
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
        this->actor.draw = func_808D3754;
    } else {
        this->actor.draw = EnFloormas_Draw;
    }

    parent = this->actor.parent;
    this->actor.shape.rot.y = parent->shape.rot.y + 0x5555;
    this->actor.world.pos = parent->world.pos;
    this->actor.params = ENFLOORMAS_GET_7FFF_10;
    Animation_Change(&this->skelAnime, &gWallmasterJumpAnim, 1.0f, 41.0f, Animation_GetLastFrame(&gWallmasterJumpAnim),
                     ANIMMODE_ONCE, 0.0f);
    this->collider.dim.radius = sCylinderInit.dim.radius * 0.6f;
    this->collider.dim.height = sCylinderInit.dim.height * 0.6f;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
    this->actor.colChkInfo.health = 1;
    this->actor.speed = 4.0f;
    this->actor.velocity.y = 7.0f;
    this->actionFunc = func_808D1B44;
}

void func_808D1B44(EnFloormas* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (SkelAnime_Update(&this->skelAnime)) {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->unk_194 = 50;
            func_808D0C14(this);
        }
        Math_StepToF(&this->actor.speed, 0.0f, 1.0f);
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_FLOORMASTER_SM_LAND);
    }
}

void func_808D1BCC(EnFloormas* this) {
    Animation_PlayLoopSetSpeed(&this->skelAnime, &gWallmasterWalkAnim, 4.5f);
    this->actor.speed = 5.0f;
    this->actionFunc = func_808D1C1C;
}

void func_808D1C1C(EnFloormas* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->unk_194 != 0) {
        this->unk_194--;
    }

    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 18.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_FLOORMASTER_SM_WALK);
    }

    if (this->unk_194 == 0) {
        func_808D1D0C(this);
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->unk_190 = this->actor.wallYaw;
        func_808D108C(this);
    } else if (this->actor.xzDistToPlayer < 120.0f) {
        Math_ScaledStepToS(&this->actor.shape.rot.y, BINANG_ROT180(this->actor.yawTowardsPlayer), 0x38E);
    }
}

void func_808D1D0C(EnFloormas* this) {
    if (this->actionFunc != func_808D1C1C) {
        Animation_PlayLoopSetSpeed(&this->skelAnime, &gWallmasterWalkAnim, 4.5f);
    }
    this->actor.speed = 5.0f;
    this->actionFunc = func_808D1D6C;
}

void func_808D1D6C(EnFloormas* this, PlayState* play) {
    Actor* sp2C;
    s32 flags;

    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 18.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_FLOORMASTER_SM_WALK);
    }

    flags = this->actor.bgCheckFlags & BGCHECKFLAG_WALL;
    if (flags) {
        this->unk_190 = this->actor.wallYaw;
        func_808D108C(this);
    } else if (this->actor.params == ENFLOORMAS_GET_7FFF_20) {
        if (this->actor.parent->params == ENFLOORMAS_GET_7FFF_40) {
            sp2C = this->actor.parent;
        } else if (this->actor.child->params == ENFLOORMAS_GET_7FFF_40) {
            sp2C = this->actor.child;
        } else {
            this->actor.params = ENFLOORMAS_GET_7FFF_10;
            return;
        }

        Math_ScaledStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardActor(&this->actor, sp2C), 0x38E);

        if (Actor_WorldDistXZToActor(&this->actor, sp2C) < 80.0f) {
            func_808D2484(this);
        }
    } else {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x71C);
        if (this->actor.xzDistToPlayer < 80.0f) {
            func_808D1FD4(this);
        }
    }
}

void func_808D1ED4(EnFloormas* this, PlayState* play) {
    Vec3f sp34;

    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    sp34.x = this->actor.world.pos.x;
    sp34.y = this->actor.world.pos.y + 15.0f;
    sp34.z = this->actor.world.pos.z;
    func_800B3030(play, &sp34, &gZeroVec3f, &gZeroVec3f, 150, -10, 2);
    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 11, NA_SE_EN_EXTINCT);
    this->actionFunc = func_808D1F7C;
}

void func_808D1F7C(EnFloormas* this, PlayState* play) {
    if (Math_StepToF(&this->actor.scale.x, 0.0f, 0.0015f)) {
        func_808D2A20(this);
    }
    this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;
}

void func_808D1FD4(EnFloormas* this) {
    Animation_Change(&this->skelAnime, &gWallmasterJumpAnim, 2.0f, 0.0f, 41.0f, ANIMMODE_ONCE, 0.0f);
    this->actor.speed = 0.0f;
    this->actionFunc = func_808D2040;
}

void func_808D2040(EnFloormas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    SkelAnime_Update(&this->skelAnime);
    if (this->skelAnime.curFrame < 20.0f) {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    } else if (Animation_OnFrame(&this->skelAnime, 20.0f)) {
        this->actor.speed = 5.0f;
        this->actor.velocity.y = 7.0f;
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->unk_18E = 50;
        this->actor.speed = 0.0f;
        Actor_PlaySfx(&this->actor, NA_SE_EN_FLOORMASTER_SM_LAND);
        func_808D1740(this);
    } else if ((this->actor.playerHeightRel < -10.0f) && (this->collider.base.ocFlags1 & OC1_HIT) &&
               (&player->actor == this->collider.base.oc)) {
        play->grabPlayer(play, player);
        func_808D217C(this, player);
    }
}

static Vec3f D_808D3900[PLAYER_FORM_MAX] = {
    { 25.0f, 45.0f, -7.0f },  // PLAYER_FORM_FIERCE_DEITY
    { 25.0f, 40.0f, -12.0f }, // PLAYER_FORM_GORON
    { 25.0f, 35.0f, -2.0f },  // PLAYER_FORM_ZORA
    { 10.0f, 20.0f, -4.0f },  // PLAYER_FORM_DEKU
    { 15.0f, 25.0f, -2.0f },  // PLAYER_FORM_HUMAN
};

void func_808D217C(EnFloormas* this, Player* player) {
    Vec3f* ptr;

    Animation_Change(&this->skelAnime, &gWallmasterJumpAnim, 1.0f, 36.0f, 45.0f, ANIMMODE_ONCE, -3.0f);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    func_808D08D0(this);
    ptr = &D_808D3900[GET_PLAYER_FORM];
    this->actor.home.pos.x = ptr->z * Math_SinS(this->actor.shape.rot.y);
    this->actor.home.pos.y = CLAMP(-this->actor.playerHeightRel, ptr->x, ptr->y);
    this->actor.home.pos.z = ptr->z * Math_CosS(this->actor.shape.rot.y);
    Math_Vec3f_Sum(&player->actor.world.pos, &this->actor.home.pos, &this->actor.world.pos);
    this->actor.shape.rot.x = -0x4CE0;
    this->actionFunc = func_808D22C8;
}

void func_808D22C8(EnFloormas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->skelAnime.playSpeed > 0.0f) {
            this->skelAnime.playSpeed = -1.0f;
            this->skelAnime.endFrame = 36.0f;
            this->skelAnime.startFrame = 45.0f;
        } else {
            this->skelAnime.playSpeed = 1.0f;
            this->skelAnime.endFrame = 45.0f;
            this->skelAnime.startFrame = 36.0f;
        }
    }

    Math_Vec3f_Sum(&player->actor.world.pos, &this->actor.home.pos, &this->actor.world.pos);

    if (!(player->stateFlags2 & PLAYER_STATE2_80) || (player->invincibilityTimer < 0)) {
        EnFloormas* parent = (EnFloormas*)this->actor.parent;
        EnFloormas* child = (EnFloormas*)this->actor.child;

        if (((parent->actionFunc == func_808D22C8) || (parent->actionFunc == func_808D2AA8)) &&
            ((child->actionFunc == func_808D22C8) || (child->actionFunc == func_808D2AA8))) {
            parent->actor.params = ENFLOORMAS_GET_7FFF_20;
            child->actor.params = ENFLOORMAS_GET_7FFF_20;
            this->actor.params = ENFLOORMAS_GET_7FFF_40;
        }

        this->actor.shape.rot.x = 0;
        this->actor.velocity.y = 6.0f;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.speed = -3.0f;
        func_808D1740(this);
    } else if ((this->unk_190 % 20) == 0) {
        Player_PlaySfx(player, NA_SE_VO_LI_DAMAGE_S + player->ageProperties->voiceSfxIdOffset);
        play->damagePlayer(play, -4);
    }
    Actor_PlaySfx(&this->actor, NA_SE_EN_FLOORMASTER_SM_STICK - SFX_FLAG);
}

void func_808D2484(EnFloormas* this) {
    Animation_Change(&this->skelAnime, &gWallmasterJumpAnim, 2.0f, 0.0f, 41.0f, ANIMMODE_ONCE, 0.0f);
    this->actor.speed = 0.0f;
    this->actionFunc = func_808D24F0;
}

void func_808D24F0(EnFloormas* this, PlayState* play) {
    Actor* phi_s1;

    SkelAnime_Update(&this->skelAnime);
    if (this->actor.parent->params == ENFLOORMAS_GET_7FFF_40) {
        phi_s1 = this->actor.parent;
    } else if (this->actor.child->params == ENFLOORMAS_GET_7FFF_40) {
        phi_s1 = this->actor.child;
    } else {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            this->actor.params = ENFLOORMAS_GET_7FFF_10;
            func_808D1740(this);
        }
        return;
    }

    if (Animation_OnFrame(&this->skelAnime, 20.0f)) {
        this->actor.speed = 5.0f;
        this->actor.velocity.y = 7.0f;
    } else if (this->skelAnime.curFrame < 20.0f) {
        Math_ApproachS(&this->actor.shape.rot.y, Actor_WorldYawTowardActor(&this->actor, phi_s1), 2, 0xE38);
    } else if (((phi_s1->world.pos.y - this->actor.world.pos.y) < -10.0f) &&
               (fabsf(this->actor.world.pos.x - phi_s1->world.pos.x) < 10.0f) &&
               (fabsf(this->actor.world.pos.z - phi_s1->world.pos.z) < 10.0f)) {
        func_808D2A20(this);
        this->collider.base.ocFlags1 |= OC1_ON;
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actor.speed = 0.0f;
        Actor_PlaySfx(&this->actor, NA_SE_EN_FLOORMASTER_SM_LAND);
        func_808D1740(this);
    }

    if ((fabsf(this->actor.world.pos.x - phi_s1->world.pos.x) < 5.0f) &&
        (fabsf(this->actor.world.pos.z - phi_s1->world.pos.z) < 5.0f)) {
        Math_StepToF(&this->actor.speed, 0.0f, 2.0f);
    }
}

void func_808D2700(EnFloormas* this) {
    Animation_PlayOnce(&this->skelAnime, &gWallmasterIdleAnim);
    this->unk_18E = 0;
    this->unk_194 = 1500;
    this->actor.params = ENFLOORMAS_GET_7FFF_40;
    this->actor.speed = 0.0f;
    func_808D08D0(this);
    this->actionFunc = func_808D2764;
}

void func_808D2764(EnFloormas* this, PlayState* play) {
    EnFloormas* parent;
    EnFloormas* child;
    s32 phi_a2 = 0;
    f32 temp_f2;

    if (this->unk_194 != 0) {
        this->unk_194--;
    }

    parent = (EnFloormas*)this->actor.parent;
    child = (EnFloormas*)this->actor.child;
    if (this->unk_194 == 0) {
        if (parent->actionFunc != func_808D2AA8) {
            func_808D1ED4(parent, play);
        }
        if (child->actionFunc != func_808D2AA8) {
            func_808D1ED4(child, play);
        }
    } else {
        if ((parent->actionFunc != func_808D2AA8) && (parent->actionFunc != func_808D1F7C)) {
            phi_a2 = 1;
        }

        if ((child->actionFunc != func_808D2AA8) && (child->actionFunc != func_808D1F7C)) {
            phi_a2++;
        }
    }

    temp_f2 = this->actor.scale.x;

    if (phi_a2 == 1) {
        Math_StepToF(&this->actor.scale.x, 0.007f, 0.0005f);
    } else if (phi_a2 == 0) {
        Math_StepToF(&this->actor.scale.x, 0.01f, 0.0005f);
    }

    this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;

    if (((temp_f2 == 0.007f) || (temp_f2 == 0.004f)) && (temp_f2 != this->actor.scale.x)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_FLOORMASTER_EXPAND);
    }

    this->collider.dim.radius = sCylinderInit.dim.radius * 100.0f * this->actor.scale.x;
    this->collider.dim.height = sCylinderInit.dim.height * 100.0f * this->actor.scale.x;

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->actor.scale.x >= 0.01f) {
            this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            func_808D0908(this);
            this->actor.params = ENFLOORMAS_GET_7FFF_0;
            this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
            this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
            this->actor.colChkInfo.health = sColChkInfoInit.health;
            func_808D0C14(this);
        } else if (this->unk_18E == 0) {
            Animation_PlayOnce(&this->skelAnime, &gFloormasterTapFingerAnim);
            this->unk_18E = 1;
        } else {
            Animation_PlayOnce(&this->skelAnime, &gWallmasterIdleAnim);
            this->unk_18E = 0;
        }
    }

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FLOORMASTER_RESTORE - SFX_FLAG);
}

void func_808D2A20(EnFloormas* this) {
    EnFloormas* parent = (EnFloormas*)this->actor.parent;
    EnFloormas* child = (EnFloormas*)this->actor.child;

    this->drawDmgEffAlpha = 0.0f;

    if ((parent->actionFunc == func_808D2AA8) && (child->actionFunc == func_808D2AA8)) {
        func_808D2AB8(parent);
        func_808D2AB8(child);
        Actor_Kill(&this->actor);
    } else {
        this->actor.draw = NULL;
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED);
        this->actionFunc = func_808D2AA8;
    }
}

void func_808D2AA8(EnFloormas* this, PlayState* play) {
}

void func_808D2AB8(EnFloormas* this) {
    this->collider.base.atFlags &= ~(AT_HIT | AT_ON);
    this->collider.base.acFlags &= ~(AC_HIT | AC_ON);
    this->collider.base.ocFlags1 &= ~(OC1_HIT | OC1_ON);
    this->actionFunc = func_808D2AF4;
}

void func_808D2AF4(EnFloormas* this, PlayState* play) {
    Actor_Kill(&this->actor);
}

void func_808D2B18(EnFloormas* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gWallmasterDamageAnim, -3.0f);
    func_800BE504(&this->actor, &this->collider);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 20);
    this->actor.speed = 5.0f;
    this->actor.velocity.y = 5.5f;
    if (this->actor.params == ENFLOORMAS_GET_7FFF_40) {
        EnFloormas* parent = (EnFloormas*)this->actor.parent;

        if ((parent->actionFunc != func_808D2AA8) && (parent->actor.colChkInfo.health > 0)) {
            func_808D2700(parent);
        } else {
            EnFloormas* child = (EnFloormas*)this->actor.child;

            if ((child->actionFunc != func_808D2AA8) && (child->actor.colChkInfo.health > 0)) {
                func_808D2700(child);
            }
        }
        this->actor.params = ENFLOORMAS_GET_7FFF_20;
    }
    this->actionFunc = func_808D2C08;
}

void func_808D2C08(EnFloormas* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (this->actor.colChkInfo.health == 0) {
            func_808D0930(this, play);
        } else {
            func_808D2CDC(this);
        }
    }

    if (Animation_OnFrame(&this->skelAnime, 13.0f)) {
        if (this->actor.scale.x > 0.009f) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_ATTACK);
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_ATTACK);
        }
    }

    Math_StepToF(&this->actor.speed, 0.0f, 0.2f);
}

void func_808D2CDC(EnFloormas* this) {
    Animation_PlayOnce(&this->skelAnime, &gWallmasterRecoverFromDamageAnim);
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = func_808D2D30;
}

void func_808D2D30(EnFloormas* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        func_808D0C14(this);
    }
}

void func_808D2D6C(EnFloormas* this) {
    this->actor.speed = 0.0f;
    if (this->actor.velocity.y > 0.0f) {
        this->actor.velocity.y = 0.0f;
    }
    func_800BE504(&this->actor, &this->collider);
    this->actionFunc = func_808D2DC0;
}

void func_808D2DC0(EnFloormas* this, PlayState* play) {
    if (this->unk_18E != 0) {
        this->unk_18E--;
    }

    if (this->unk_18E == 0) {
        func_808D0A48(this, play);
        if (this->actor.colChkInfo.health == 0) {
            this->collider.base.acFlags |= AC_ON;
            func_808D0930(this, play);
        } else {
            func_808D0C14(this);
        }
    }
}

void func_808D2E34(EnFloormas* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.elem);
        if ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) ||
            !(this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3)) {
            if (this->actor.colChkInfo.damageEffect == 0xE) {
                func_808D0908(this);
                this->actor.colorFilterTimer = 0;
                this->actor.colChkInfo.damage = 4;
                this->unk_192 = -1600;
                this->actor.gravity = -1.0f;
                this->drawDmgEffAlpha = 4.0f;
                this->drawDmgEffScale = 0.55f;
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
            }

            if (this->collider.base.colMaterial != COL_MATERIAL_HARD) {
                if (!Actor_ApplyDamage(&this->actor)) {
                    if (this->actor.scale.x > 0.009f) {
                        Actor_PlaySfx(&this->actor, NA_SE_EN_DAIOCTA_REVERSE);
                    } else {
                        Actor_PlaySfx(&this->actor, NA_SE_EN_FLOORMASTER_SM_DEAD);
                    }
                    Enemy_StartFinishingBlow(play, &this->actor);
                    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                } else if (this->actor.colChkInfo.damage != 0) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_FALL_DAMAGE);
                }

                func_808D0A48(this, play);

                if (this->actor.colChkInfo.damageEffect != 0xF) {
                    if (this->actor.colChkInfo.damageEffect == 3) {
                        func_808D09CC(this);
                        if (this->actor.colChkInfo.health == 0) {
                            this->unk_18E = 3;
                            this->collider.base.acFlags &= ~AC_ON;
                        }
                        func_808D2D6C(this);
                    } else if (this->actor.colChkInfo.damageEffect == 1) {
                        this->unk_18E = 40;
                        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA,
                                             40);
                        Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                        func_808D2D6C(this);
                    } else if (this->actor.colChkInfo.damageEffect == 5) {
                        this->unk_18E = 40;
                        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA,
                                             40);
                        Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                        this->drawDmgEffScale = 0.55f;
                        this->drawDmgEffAlpha = 2.0f;
                        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_MEDIUM;
                        func_808D2D6C(this);
                    } else {
                        if (this->actor.colChkInfo.damageEffect == 2) {
                            this->drawDmgEffAlpha = 4.0f;
                            this->drawDmgEffScale = 0.55f;
                            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                        } else if (this->actor.colChkInfo.damageEffect == 4) {
                            this->drawDmgEffAlpha = 4.0f;
                            this->drawDmgEffScale = 0.55f;
                            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG,
                                        this->collider.elem.acDmgInfo.hitPos.x, this->collider.elem.acDmgInfo.hitPos.y,
                                        this->collider.elem.acDmgInfo.hitPos.z, 0, 0, 0,
                                        (this->actor.scale.x > 0.009f) ? CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS)
                                                                       : CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_LIGHT_RAYS));
                        }
                        func_808D2B18(this);
                    }
                }
            }
        }
    }
}

void EnFloormas_Update(Actor* thisx, PlayState* play) {
    EnFloormas* this = (EnFloormas*)thisx;
    s32 pad;

    if (this->actionFunc != func_808D2AA8) {
        if (this->collider.base.atFlags & AT_HIT) {
            this->collider.base.atFlags &= ~AT_HIT;
            this->actor.speed *= -0.5f;
            this->actor.speed = CLAMP_MAX(this->actor.speed, -5.0f);
            this->actor.velocity.y = 5.0f;
            func_808D1740(this);
        }

        func_808D2E34(this, play);
        this->actionFunc(this, play);
        if (this->actionFunc != func_808D2AF4) {
            if (this->actionFunc != func_808D2C08) {
                this->actor.world.rot.y = this->actor.shape.rot.y;
            }

            if (this->actionFunc != func_808D22C8) {
                Actor_MoveWithGravity(&this->actor);
            }

            Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, this->actor.scale.x * 3000.0f, 0.0f,
                                    UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                        UPDBGCHECKINFO_FLAG_10);
            Collider_UpdateCylinder(&this->actor, &this->collider);
            if (this->actionFunc == func_808D1650) {
                this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
                CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
            }

            if (this->actionFunc != func_808D22C8) {
                if ((this->actionFunc != func_808D1B44) && (this->actionFunc != func_808D2C08) &&
                    (this->actionFunc != func_808D1F7C)) {
                    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
                }

                if ((this->actionFunc != func_808D24F0) || (this->skelAnime.curFrame < 20.0f)) {
                    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
                }
            }

            Actor_SetFocus(&this->actor, this->actor.scale.x * 2500.0f);
            if (this->collider.base.colMaterial == COL_MATERIAL_HARD) {
                if (this->unk_190 != 0) {
                    this->unk_190--;
                }

                if (this->unk_190 == 0) {
                    this->unk_190 = 40;
                }
            }

            if (this->drawDmgEffAlpha > 0.0f) {
                if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
                    Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
                    this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.275f;
                    this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.55f);
                } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.55f, 0.01375f)) {
                    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
                }
            }
        }
    }
}

s32 EnFloormas_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                                Gfx** gfx) {
    EnFloormas* this = (EnFloormas*)thisx;

    if (limbIndex == WALLMASTER_LIMB_ROOT) {
        pos->z += this->unk_192;
    }

    return false;
}

static s8 sLimbToBodyParts[WALLMASTER_LIMB_MAX] = {
    BODYPART_NONE,         // WALLMASTER_LIMB_NONE
    BODYPART_NONE,         // WALLMASTER_LIMB_ROOT
    BODYPART_NONE,         // WALLMASTER_LIMB_HAND
    BODYPART_NONE,         // WALLMASTER_LIMB_INDEX_FINGER_ROOT
    ENFLOORMAS_BODYPART_0, // WALLMASTER_LIMB_INDEX_FINGER_PROXIMAL
    BODYPART_NONE,         // WALLMASTER_LIMB_INDEX_FINGER_DISTAL_ROOT
    BODYPART_NONE,         // WALLMASTER_LIMB_INDEX_FINGER_MIDDLE
    ENFLOORMAS_BODYPART_1, // WALLMASTER_LIMB_INDEX_FINGER_DISTAL
    BODYPART_NONE,         // WALLMASTER_LIMB_RING_FINGER_ROOT
    ENFLOORMAS_BODYPART_2, // WALLMASTER_LIMB_RING_FINGER_PROXIMAL
    BODYPART_NONE,         // WALLMASTER_LIMB_RING_FINGER_DISTAL_ROOT
    BODYPART_NONE,         // WALLMASTER_LIMB_RING_FINGER_MIDDLE
    ENFLOORMAS_BODYPART_3, // WALLMASTER_LIMB_RING_FINGER_DISTAL
    BODYPART_NONE,         // WALLMASTER_LIMB_MIDDLE_FINGER_ROOT
    ENFLOORMAS_BODYPART_4, // WALLMASTER_LIMB_MIDDLE_FINGER_PROXIMAL
    BODYPART_NONE,         // WALLMASTER_LIMB_MIDDLE_FINGER_DISTAL_ROOT
    BODYPART_NONE,         // WALLMASTER_LIMB_MIDDLE_FINGER_MIDDLE
    ENFLOORMAS_BODYPART_5, // WALLMASTER_LIMB_MIDDLE_FINGER_DISTAL
    BODYPART_NONE,         // WALLMASTER_LIMB_WRIST_ROOT
    BODYPART_NONE,         // WALLMASTER_LIMB_WRIST
    BODYPART_NONE,         // WALLMASTER_LIMB_THUMB_ROOT
    ENFLOORMAS_BODYPART_6, // WALLMASTER_LIMB_THUMB_PROXIMAL
    ENFLOORMAS_BODYPART_7, // WALLMASTER_LIMB_THUMB_DISTAL_ROOT
    BODYPART_NONE,         // WALLMASTER_LIMB_THUMB_MIDDLE
    ENFLOORMAS_BODYPART_8, // WALLMASTER_LIMB_THUMB_DISTAL
};

void EnFloormas_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    EnFloormas* this = (EnFloormas*)thisx;

    if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
        Matrix_MultZero(&this->bodyPartsPos[sLimbToBodyParts[limbIndex]]);
    }

    if (limbIndex == WALLMASTER_LIMB_WRIST) {
        Matrix_MultVecX(1000.0f, &this->bodyPartsPos[ENFLOORMAS_BODYPART_9]);
        Matrix_MultVecX(-1000.0f, &this->bodyPartsPos[ENFLOORMAS_BODYPART_10]);
    } else if (limbIndex == WALLMASTER_LIMB_HAND) {
        Matrix_Push();
        Matrix_Translate(1600.0f, -700.0f, -1700.0f, MTXMODE_APPLY);
        Matrix_RotateYF(M_PIf / 3, MTXMODE_APPLY);
        Matrix_RotateZF(M_PIf / 12, MTXMODE_APPLY);
        Matrix_Scale(2.0f, 2.0f, 2.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD((*gfx)++, play->state.gfxCtx);
        gSPDisplayList((*gfx)++, gWallmasterLittleFingerDL);

        Matrix_Pop();
    }
}

static Color_RGBA8 D_808D3958 = { 0, 255, 0, 0 };

void EnFloormas_Draw(Actor* thisx, PlayState* play) {
    EnFloormas* this = (EnFloormas*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    if (this->collider.base.colMaterial == COL_MATERIAL_HARD) {
        func_800AE2A0(play, &D_808D3958, this->unk_190 % 40, 40);
    }

    POLY_OPA_DISP =
        SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                           EnFloormas_OverrideLimbDraw, EnFloormas_PostLimbDraw, &this->actor, POLY_OPA_DISP);
    if (this->collider.base.colMaterial == COL_MATERIAL_HARD) {
        func_800AE5A0(play);
    }

    CLOSE_DISPS(play->state.gfxCtx);

    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, ENFLOORMAS_BODYPART_MAX,
                            100.0f * (this->drawDmgEffScale * this->actor.scale.x),
                            100.0f * (this->drawDmgEffFrozenSteamScale * this->actor.scale.x), this->drawDmgEffAlpha,
                            this->drawDmgEffType);
}

void func_808D3754(Actor* thisx, PlayState* play) {
    EnFloormas* this = (EnFloormas*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    if (this->collider.base.colMaterial == COL_MATERIAL_HARD) {
        func_800AE5E4(play, &D_808D3958, this->unk_190 % 40, 40);
    }

    POLY_XLU_DISP =
        SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                           EnFloormas_OverrideLimbDraw, EnFloormas_PostLimbDraw, &this->actor, POLY_XLU_DISP);
    if (this->collider.base.colMaterial == COL_MATERIAL_HARD) {
        func_800AE8EC(play);
    }

    CLOSE_DISPS(play->state.gfxCtx);

    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, ENFLOORMAS_BODYPART_MAX,
                            this->drawDmgEffScale * this->actor.scale.x * 100.0f,
                            this->drawDmgEffFrozenSteamScale * this->actor.scale.x * 100.0f, this->drawDmgEffAlpha,
                            this->drawDmgEffType);
}
