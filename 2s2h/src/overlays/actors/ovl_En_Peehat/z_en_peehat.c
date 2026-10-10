/*
 * File: z_en_peehat.c
 * Overlay: ovl_En_Peehat
 * Description: Peahat
 */

#include "z_en_peehat.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnPeehat_Init(Actor* thisx, PlayState* play);
void EnPeehat_Destroy(Actor* thisx, PlayState* play);
void EnPeehat_Update(Actor* thisx, PlayState* play2);
void EnPeehat_Draw(Actor* thisx, PlayState* play);

void func_80897498(EnPeehat* this);
void func_80897520(EnPeehat* this, PlayState* play);
void func_80897648(EnPeehat* this);
void func_808976DC(EnPeehat* this, PlayState* play);
void func_80897910(EnPeehat* this, PlayState* play);
void func_80897A34(EnPeehat* this);
void func_80897A94(EnPeehat* this, PlayState* play);
void func_80897D48(EnPeehat* this, PlayState* play);
void func_80897EAC(EnPeehat* this);
void func_80897F44(EnPeehat* this, PlayState* play);
void func_80898124(EnPeehat* this);
void func_80898144(EnPeehat* this, PlayState* play);
void func_808982E0(EnPeehat* this);
void func_80898338(EnPeehat* this, PlayState* play);
void func_80898454(EnPeehat* this, PlayState* play);
void func_808984E0(EnPeehat* this);
void func_80898594(EnPeehat* this, PlayState* play);
void func_80898654(EnPeehat* this);
void func_808986A4(EnPeehat* this, PlayState* play);

ActorProfile En_Peehat_Profile = {
    /**/ ACTOR_EN_PEEHAT,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_PH,
    /**/ sizeof(EnPeehat),
    /**/ EnPeehat_Init,
    /**/ EnPeehat_Destroy,
    /**/ EnPeehat_Update,
    /**/ EnPeehat_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_WOOD,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 50, 120, -20, { 0, 0, 0 } },
};

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 0, { { 0, 0, 0 }, 40 }, 100 },
};

static ColliderTrisElementInit sTrisElementsInit[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COL_MATERIAL_METAL,
        AT_NONE | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(sTrisElementsInit),
    sTrisElementsInit,
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0x1),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(2, 0x2),
    /* Ice arrow      */ DMG_ENTRY(2, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(2, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x5),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
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

static CollisionCheckInfoInit2 sColChkInfoInit1 = { 15, 50, 120, -20, MASS_HEAVY };

static CollisionCheckInfoInit2 sColChkInfoInit2 = { 1, 20, 15, -5, 30 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 700, ICHAIN_STOP),
};

void EnPeehat_Init(Actor* thisx, PlayState* play) {
    EnPeehat* this = (EnPeehat*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_Init(play, &this->skelAnime, &object_ph_Skel_001C80, &object_ph_Anim_0009C4, this->jointTable,
                   this->morphTable, OBJECT_PH_LIMB_MAX);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 27.0f);
    this->unk_2B0 = 0;
    Math_Vec3f_Copy(&this->actor.focus.pos, &this->actor.world.pos);
    this->actor.floorHeight = this->actor.world.pos.y;
    Collider_InitAndSetCylinder(play, &this->colliderCylinder, &this->actor, &sCylinderInit);
    Collider_InitAndSetSphere(play, &this->colliderSphere, &this->actor, &sSphereInit);
    Collider_InitAndSetTris(play, &this->colliderTris, &this->actor, &sTrisInit, this->colliderTriElements);

    if (this->actor.params == 0) {
        CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit1);
        if (gSaveContext.save.isNight) {
            this->actor.shape.yOffset = -1000.0f;
        }
        Actor_SetScale(&this->actor, 0.036f);
        this->actor.hintId = TATL_HINT_ID_PEAHAT;
        func_80897498(this);
    } else {
        CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit2);
        this->actor.scale.z = this->actor.scale.x = 0.006f;
        this->actor.scale.y = 0.003f;
        this->actor.velocity.y = 6.0f;

        this->colliderCylinder.dim.radius = 20;
        this->colliderCylinder.dim.height = 15;
        this->colliderCylinder.dim.yShift = -5;
        this->actor.hintId = TATL_HINT_ID_PEAHAT_LARVA;
        this->colliderCylinder.base.ocFlags1 &= ~OC1_ON;

        func_80897A34(this);
    }
}

void EnPeehat_Destroy(Actor* thisx, PlayState* play) {
    EnPeehat* this = (EnPeehat*)thisx;

    Collider_DestroyCylinder(play, &this->colliderCylinder);
    Collider_DestroySphere(play, &this->colliderSphere);
    Collider_DestroyTris(play, &this->colliderTris);
    if (this->actor.params != 0) {
        EnPeehat* parent = (EnPeehat*)this->actor.parent;

        if ((parent != NULL) && (parent->actor.update != NULL)) {
            parent->unk_2AC--;
        }
    }
}

void func_80897170(EnPeehat* this) {
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->drawDmgEffScale = 1.1f;
    this->drawDmgEffFrozenSteamScale = 1650.0f * 0.001f;
    this->drawDmgEffAlpha = 1.0f;
    this->colliderSphere.base.colMaterial = COL_MATERIAL_HIT3;
    this->unk_2B0 = 80;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 80);
}

void func_808971DC(EnPeehat* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->colliderSphere.base.colMaterial = COL_MATERIAL_HIT6;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, PEEHAT_BODYPART_MAX, 2, 0.5f, 0.35f);
    }
}

void func_80897258(PlayState* play, EnPeehat* this, Vec3f* arg2, f32 arg3, f32 arg4) {
    static Vec3f D_80899558 = { 0.0f, 8.0f, 0.0f };
    static Vec3f D_80899564 = { 0.0f, -1.5f, 0.0f };
    Vec3f sp44;
    s16 sp42 = (s32)Rand_Next() >> 0x10;
    s32 temp_v1;

    sp44.y = this->actor.floorHeight;
    sp44.x = (Math_SinS(sp42) * arg3) + arg2->x;
    sp44.z = (Math_CosS(sp42) * arg3) + arg2->z;

    D_80899564.x = Rand_CenteredFloat(1.05f);
    D_80899564.z = Rand_CenteredFloat(1.05f);
    D_80899558.y = Rand_CenteredFloat(4.0f) + 8.0f;

    EffectSsHahen_Spawn(play, &sp44, &D_80899558, &D_80899564, 0, (Rand_ZeroFloat(5.0f) + 12.0f) * arg4,
                        HAHEN_OBJECT_DEFAULT, 10, NULL);
}

void func_80897390(EnPeehat* this, PlayState* play) {
    s32 i;
    s16 phi_s2 = BINANG_ROT180(this->actor.yawTowardsPlayer);
    Actor* actor;

    this->colliderCylinder.base.acFlags &= ~AC_HIT;

    for (i = 3 - this->unk_2AC; i > 0; i--) {
        actor = Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_PEEHAT, this->actor.world.pos.x,
                                   this->actor.world.pos.y + 50.0f, this->actor.world.pos.z, 0, phi_s2, 0, 1);
        phi_s2 += 0x5555;

        if (actor != NULL) {
            this->unk_2AC++;
        }
    }

    this->unk_2B0 = 8;
    Actor_PlaySfx(&this->actor, NA_SE_EN_PIHAT_DAMAGE);
}

void func_80897498(EnPeehat* this) {
    Animation_Change(&this->skelAnime, &object_ph_Anim_0009C4, 0.0f, 3.0f,
                     Animation_GetLastFrame(&object_ph_Anim_0009C4), ANIMMODE_ONCE, 0.0f);
    this->unk_2B0 = 0;
    this->unk_2AD = 1;
    this->colliderCylinder.base.acFlags &= ~AC_HIT;
    this->actionFunc = func_80897520;
}

void func_80897520(EnPeehat* this, PlayState* play) {
    if (!gSaveContext.save.isNight) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->colliderSphere.base.acFlags |= AC_ON;
        if (this->actor.xzDistToPlayer < 740.0f) {
            func_80897648(this);
        } else {
            Math_StepToF(&this->actor.shape.yOffset, -1000.0f, 10.0f);
        }
    } else {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->colliderSphere.base.acFlags &= ~AC_ON;
        Math_StepToF(&this->actor.shape.yOffset, -1000.0f, 50.0f);
        if (this->unk_2B0 != 0) {
            this->unk_2B0--;
            if (this->unk_2B0 & 4) {
                Math_StepToF(&this->unk_2C4, 0.205f, 0.235f);
            } else {
                Math_StepToF(&this->unk_2C4, 0.0f, 0.005f);
            }
        } else if (this->colliderCylinder.base.acFlags & AC_HIT) {
            func_80897390(this, play);
        }
    }
}

void func_80897648(EnPeehat* this) {
    if (this->actionFunc != func_80898454) {
        Animation_Change(&this->skelAnime, &object_ph_Anim_0009C4, 0.0f, 3.0f,
                         Animation_GetLastFrame(&object_ph_Anim_0009C4), ANIMMODE_ONCE, 0.0f);
    }
    this->unk_2B0 = 16;
    Actor_PlaySfx(&this->actor, NA_SE_EN_PIHAT_UP);
    this->actionFunc = func_808976DC;
}

void func_808976DC(EnPeehat* this, PlayState* play) {
    Vec3f sp34;

    Math_StepToF(&this->actor.shape.yOffset, 0.0f, 50.0f);

    if (Math_ScaledStepToS(&this->unk_2B2, 0xFA0, 0x320)) {
        if (this->unk_2B0 != 0) {
            this->unk_2B0--;
            if ((this->unk_2B0 == 0) && (this->skelAnime.playSpeed < 0.5f)) {
                this->unk_2B0 = -1;
                this->skelAnime.playSpeed = 1.0f;
            }
        }

        if (SkelAnime_Update(&this->skelAnime) || (this->unk_2B0 == 0)) {
            func_80897EAC(this);
        } else {
            this->actor.world.pos.y += 6.5f;
        }

        if ((this->actor.world.pos.y - this->actor.floorHeight) < 80.0f) {
            Math_Vec3f_Copy(&sp34, &this->actor.world.pos);
            sp34.y = this->actor.floorHeight;
            func_800BBFB0(play, &sp34, 90.0f, 1, 150, 100, 1);
        }
    }

    func_80897258(play, this, &this->actor.world.pos, 75.0f, 2.0f);
    Math_StepToF(&this->unk_2C4, 0.075f, 0.005f);
    this->unk_2B4 += this->unk_2B2;
}

void func_80897864(EnPeehat* this) {
    Animation_PlayLoop(&this->skelAnime, &object_ph_Anim_0005C4);
    this->unk_2B8 = 0.0f;
    if (this->actionFunc == func_80898338) {
        this->unk_2AD = -this->unk_2AD;
    } else {
        this->unk_2AD = (Rand_ZeroOne() < 0.5f) ? 1 : -1;
    }
    this->colliderTris.base.atFlags |= AT_ON;
    this->actionFunc = func_80897910;
}

void func_80897910(EnPeehat* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_StepToF(&this->actor.speed, 3.0f, 0.25f);
    Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight + 80.0f, 3.0f);
    SkelAnime_Update(&this->skelAnime);

    if (!gSaveContext.save.isNight && (Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < 1200.0f)) {
        Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x3E8);
        this->actor.shape.rot.y += (s16)(this->unk_2AD * 450);
    } else {
        func_80898124(this);
    }

    Math_ScaledStepToS(&this->unk_2B2, 0xFA0, 0x1F4);
    this->unk_2B4 += this->unk_2B2;
    Math_StepToF(&this->unk_2C4, 0.075f, 0.005f);
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void func_80897A34(EnPeehat* this) {
    Animation_PlayLoop(&this->skelAnime, &object_ph_Anim_0005C4);
    this->unk_2B0 = 30;
    this->actor.speed = 5.3f;
    this->colliderTris.base.atFlags |= AT_ON;
    this->actionFunc = func_80897A94;
}

void func_80897A94(EnPeehat* this, PlayState* play) {
    s32 pad;

    if ((this->actor.parent != NULL) && (this->actor.parent->update == NULL)) {
        this->actor.parent = NULL;
    }

    if ((this->actor.world.pos.y - this->actor.floorHeight) >= 70.0f) {
        Math_StepToF(&this->actor.velocity.y, -1.3f, 0.5f);
    } else {
        Math_StepToF(&this->actor.velocity.y, -0.135f, 0.05f);
    }

    if (this->unk_2B0 > 0) {
        this->unk_2B0--;
    } else {
        Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x33E);
        this->colliderCylinder.base.ocFlags1 |= OC1_ON;
    }

    this->actor.shape.rot.y += 0x15E;
    SkelAnime_Update(&this->skelAnime);
    Math_ScaledStepToS(&this->unk_2B2, 0xFA0, 0x1F4);
    this->unk_2B4 += this->unk_2B2;
    Math_StepToF(&this->unk_2C4, 0.075f, 0.005f);
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_PIHAT_SM_FLY - SFX_FLAG);

    if (this->colliderTris.base.atFlags & AT_BOUNCED) {
        this->colliderTris.base.atFlags &= ~(AT_BOUNCED | AT_ON);
        this->actor.colChkInfo.health = 0;
        func_808982E0(this);
    } else if ((this->colliderCylinder.base.acFlags & AC_HIT) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        func_800B3030(play, &this->actor.world.pos, &gZeroVec3f, &gZeroVec3f, 40, 7, 0);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 11, NA_SE_EN_EXTINCT);
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_PIHAT_SM_DEAD);
        }
        Actor_Kill(&this->actor);
    } else if (this->colliderTris.base.atFlags & AT_HIT) {
        this->colliderTris.base.atFlags &= ~AT_HIT;
        if (BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.world.rot.y) > 0) {
            this->actor.world.rot.y -= 0x2000;
        } else {
            this->actor.world.rot.y += 0x2000;
        }
        this->unk_2B0 = 40;
    } else if (this->colliderCylinder.base.ocFlags1 & OC1_HIT) {
        this->colliderCylinder.base.ocFlags1 &= ~OC1_HIT;
        if ((BINANG_SUB(Actor_WorldYawTowardActor(&this->actor, this->colliderCylinder.base.oc),
                        this->actor.world.rot.y)) > 0) {
            this->actor.world.rot.y -= 0x2000;
        } else {
            this->actor.world.rot.y += 0x2000;
        }
        this->unk_2B0 = 10;
    }
}

void func_80897D00(EnPeehat* this) {
    Animation_PlayOnce(&this->skelAnime, &object_ph_Anim_000350);
    this->colliderTris.base.atFlags &= ~AT_ON;
    this->actionFunc = func_80897D48;
}

void func_80897D48(EnPeehat* this, PlayState* play) {
    Vec3f sp34;

    Math_StepToF(&this->actor.shape.yOffset, -1000.0f, 50.0f);
    Math_StepToF(&this->actor.speed, 0.0f, 1.0f);
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0, 0x32);

    if (SkelAnime_Update(&this->skelAnime)) {
        func_80897498(this);
        this->actor.world.pos.y = this->actor.floorHeight;
        Actor_PlaySfx(&this->actor, NA_SE_EN_PIHAT_LAND);
    } else if (this->actor.world.pos.y > this->actor.floorHeight) {
        Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.floorHeight, 0.3f, 3.5f, 0.25f);
        if ((this->actor.world.pos.y - this->actor.floorHeight) < 60.0f) {
            Math_Vec3f_Copy(&sp34, &this->actor.world.pos);
            sp34.y = this->actor.floorHeight;
            func_800BBFB0(play, &sp34, 80.0f, 1, 150, 100, 1);
            func_80897258(play, this, &sp34, 75.0f, 2.0f);
        }
    }
    Math_ScaledStepToS(&this->unk_2B2, 0, 0x64);
    this->unk_2B4 += this->unk_2B2;
}

void func_80897EAC(EnPeehat* this) {
    Animation_PlayLoop(&this->skelAnime, &object_ph_Anim_0005C4);
    this->actor.speed = Rand_ZeroFloat(0.5f) + 2.5f;
    this->unk_2B0 = Rand_ZeroFloat(10.0f) + 10.0f;
    this->colliderTris.base.atFlags |= AT_ON;
    this->colliderSphere.base.acFlags |= AC_ON;
    this->actionFunc = func_80897F44;
}

void func_80897F44(EnPeehat* this, PlayState* play) {
    s32 pad;
    f32 cos;
    Player* player = GET_PLAYER(play);

    if ((this->actor.world.pos.y - this->actor.floorHeight) > 75.0f) {
        this->actor.world.pos.y -= 1.0f;
    }

    cos = Math_CosF(this->unk_2B8);
    this->actor.world.pos.y += cos * 1.4f;
    this->unk_2B8 += fabsf(cos * 0.18f) + 0.07f;
    this->unk_2B0--;

    if (this->unk_2B0 <= 0) {
        this->actor.speed = Rand_ZeroFloat(0.5f) + 2.5f;
        this->unk_2B0 = Rand_ZeroFloat(10.0f) + 10.0f;
        this->unk_2B6 = Rand_CenteredFloat(1000.0f);
    }

    SkelAnime_Update(&this->skelAnime);
    this->actor.world.rot.y += this->unk_2B6;
    this->actor.shape.rot.y += 0x15E;

    if (!gSaveContext.save.isNight && (Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < 1200.0f)) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        func_80897864(this);
    } else {
        func_80898124(this);
    }

    Math_ScaledStepToS(&this->unk_2B2, 0xFA0, 0x1F4);
    this->unk_2B4 += this->unk_2B2;
    Math_StepToF(&this->unk_2C4, 0.075f, 0.005f);
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void func_80898124(EnPeehat* this) {
    this->actionFunc = func_80898144;
    this->actor.speed = 2.5f;
}

void func_80898144(EnPeehat* this, PlayState* play) {
    s32 step;
    f32 cos;
    Player* player = GET_PLAYER(play);

    if ((this->actor.world.pos.y - this->actor.floorHeight) > 75.0f) {
        this->actor.world.pos.y -= 1.0f;
    } else {
        this->actor.world.pos.y += 1.0f;
    }
    cos = Math_CosF(this->unk_2B8);
    this->actor.world.pos.y += cos * 1.4f;

    cos = Math_CosF(this->unk_2B8);
    this->unk_2B8 += fabsf(cos * 0.18f) + 0.07f;

    step = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
    Math_ScaledStepToS(&this->actor.world.rot.y, step, 0x258);
    Math_ScaledStepToS(&this->actor.shape.rot.x, 0x1194, 0x258);

    this->actor.shape.rot.y += 0x15E;
    this->unk_2B4 += this->unk_2B2;

    if (Math_Vec3f_DistXZ(&this->actor.world.pos, &this->actor.home.pos) < 2.0f) {
        func_80897D00(this);
    }

    if (!gSaveContext.save.isNight && (Math_Vec3f_DistXZ(&this->actor.home.pos, &player->actor.world.pos) < 1200.0f)) {
        func_80897864(this);
    }
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void func_808982E0(EnPeehat* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &object_ph_Anim_000844, -4.0f);
    this->actor.speed = -9.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->actionFunc = func_80898338;
}

void func_80898338(EnPeehat* this, PlayState* play) {
    this->unk_2B4 += this->unk_2B2;
    SkelAnime_Update(&this->skelAnime);

    if (Math_StepToF(&this->actor.speed, 0.0f, 0.5f)) {
        if (this->actor.params != 0) {
            func_800B3030(play, &this->actor.world.pos, &gZeroVec3f, &gZeroVec3f, 40, 7, 0);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_EN_EXTINCT);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_PIHAT_SM_DEAD);
            Actor_Kill(&this->actor);
        } else {
            func_80897864(this);
        }
    }
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void func_80898414(EnPeehat* this) {
    func_800BE568(&this->actor, &this->colliderSphere);
    this->unk_2B2 = 0;
    this->actor.speed = 0.0f;
    this->actionFunc = func_80898454;
}

void func_80898454(EnPeehat* this, PlayState* play) {
    if (this->unk_2B0 != 0) {
        this->unk_2B0--;
    }

    Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight, 8.0f);

    if (this->unk_2B0 == 0) {
        func_808971DC(this, play);
        if (this->actor.colChkInfo.health == 0) {
            func_808984E0(this);
        } else {
            func_80897648(this);
        }
    }
}

void func_808984E0(EnPeehat* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &object_ph_Anim_000844, -4.0f);
    Actor_PlaySfx(&this->actor, NA_SE_EN_PIHAT_DAMAGE);
    this->unk_2B2 = 0xFA0;
    this->unk_2B0 = 14;
    this->actor.speed = 10.0f;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 14);
    this->colliderSphere.base.acFlags &= ~AC_ON;
    this->unk_2C4 = 0.0f;
    if (this->actor.colChkInfo.health == 0) {
        this->actor.velocity.y = 6.0f;
    }
    this->actionFunc = func_80898594;
}

void func_80898594(EnPeehat* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->unk_2B4 += this->unk_2B2;
    Math_ScaledStepToS(&this->unk_2B2, 0xFA0, 0xFA);
    Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight + 88.5f, 3.0f);
    Math_StepToF(&this->actor.speed, 0.0f, 0.5f);
    this->unk_2B0--;
    if (this->unk_2B0 <= 0) {
        if (this->actor.colChkInfo.health == 0) {
            func_80898654(this);
        } else {
            func_80897EAC(this);
        }
    }
}

void func_80898654(EnPeehat* this) {
    Animation_PlayLoop(&this->skelAnime, &object_ph_Anim_0005C4);
    this->unk_2B0 = 5;
    this->unk_2B8 = 0.0f;
    this->actionFunc = func_808986A4;
}

void func_808986A4(EnPeehat* this, PlayState* play) {
    if (this->unk_2B0 == 5) {
        EnBom* bomb =
            (EnBom*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x, this->actor.world.pos.y,
                                this->actor.world.pos.z, BOMB_EXPLOSIVE_TYPE_BOMB, 0, 0x602, BOMB_TYPE_BODY);

        if (bomb != NULL) {
            bomb->timer = 0;
        }
    }

    this->unk_2B0--;

    if (this->unk_2B0 == 0) {
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xE0);
        Actor_Kill(&this->actor);
    }
}

void func_8089874C(EnPeehat* this, PlayState* play) {
    if (this->colliderTris.base.acFlags & AC_BOUNCED) {
        this->colliderTris.base.acFlags &= ~AC_BOUNCED;
        this->colliderSphere.base.acFlags &= ~AC_HIT;
        return;
    }

    if (this->colliderSphere.base.acFlags & AC_HIT) {
        this->colliderSphere.base.acFlags &= ~AC_HIT;
        if ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) ||
            !(this->colliderSphere.elem.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3)) {
            if (!Actor_ApplyDamage(&this->actor)) {
                Enemy_StartFinishingBlow(play, &this->actor);
            }

            this->colliderTris.base.atFlags &= ~(AT_HIT | AT_ON);
            Actor_SetDropFlag(&this->actor, &this->colliderSphere.elem);
            func_808971DC(this, play);

            if (this->actor.colChkInfo.damageEffect == 5) {
                this->unk_2B0 = 40;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
                Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                this->drawDmgEffScale = 1.1f;
                this->drawDmgEffAlpha = 2.0f;
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_LARGE;
                func_80898414(this);
            } else if (this->actor.colChkInfo.damageEffect == 1) {
                this->unk_2B0 = 40;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 200, COLORFILTER_BUFFLAG_OPA, 40);
                Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                func_80898414(this);
            } else if (this->actor.colChkInfo.damageEffect == 3) {
                func_80897170(this);
                if (this->actor.colChkInfo.health == 0) {
                    this->unk_2B0 = 3;
                    this->colliderSphere.base.acFlags &= ~AC_ON;
                }
                func_80898414(this);
            } else {
                if (this->actor.colChkInfo.damageEffect == 2) {
                    this->drawDmgEffAlpha = 4.0f;
                    this->drawDmgEffScale = 2.1f;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                } else if (this->actor.colChkInfo.damageEffect == 4) {
                    this->drawDmgEffAlpha = 4.0f;
                    this->drawDmgEffScale = 1.1f;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->colliderSphere.elem.acDmgInfo.hitPos.x,
                                this->colliderSphere.elem.acDmgInfo.hitPos.y,
                                this->colliderSphere.elem.acDmgInfo.hitPos.z, 0, 0, 0,
                                CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                }
                func_800BE568(&this->actor, &this->colliderSphere);
                func_808984E0(this);
            }
        }
    } else if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) &&
               (this->colliderCylinder.base.acFlags & AC_HIT) &&
               ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) ||
                !(this->colliderCylinder.elem.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3))) {
        func_808971DC(this, play);
        this->actor.colorFilterTimer = 0;
        func_80897648(this);
    }
}

void EnPeehat_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnPeehat* this = (EnPeehat*)thisx;

    if (thisx->params == 0) {
        func_8089874C(this, play);
    }
    Actor_MoveWithGravity(thisx);
    Actor_UpdateBgCheckInfo(play, thisx, 25.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

    this->actionFunc(this, play);

    if ((play->gameplayFrames % 128) == 0) {
        this->unk_2C0 = Rand_ZeroFloat(0.25f) + 0.5f;
    }

    this->unk_2BC += this->unk_2C0;
    if (thisx->params == 0) {
        if (this->actionFunc == func_80897910) {
            Math_ScaledStepToS(&thisx->shape.rot.x, 0x1770, 0x12C);
        } else {
            Math_ScaledStepToS(&thisx->shape.rot.x, 0, 0x12C);
        }
    } else {
        Actor_SetFocus(thisx, 0.0f);
    }

    Collider_UpdateCylinder(thisx, &this->colliderCylinder);
    this->colliderCylinder.dim.pos.y += TRUNCF_BINANG(thisx->shape.yOffset * thisx->scale.y);
    if (this->colliderCylinder.base.ocFlags1 & OC1_ON) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder.base);
    }

    if (thisx->params == 0) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderSphere.base);
        if (this->colliderSphere.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderSphere.base);
        }
    }

    if (this->colliderTris.base.atFlags & AT_HIT) {
        this->colliderTris.base.atFlags &= ~AT_HIT;
        func_808982E0(this);
    }

    if (this->colliderTris.base.atFlags & AT_ON) {
        thisx->flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderTris.base);
        if (thisx->params == 0) {
            Vec3f sp74;
            CollisionPoly* poly = NULL;
            s32 bgId;
            s32 i;

            CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderTris.base);

            for (i = 1; i >= 0; i--) {
                if (BgCheck_EntityLineTest1(&play->colCtx, &thisx->world.pos, &this->unk_2D4[i], &sp74, &poly, true,
                                            true, false, true, &bgId)) {
                    func_800BBFB0(play, &sp74, 0.0f, 1, 300, 150, 1);
                    func_80897258(play, this, &sp74, 0.0f, 1.5f);
                }
            }
        }
    }

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderCylinder.base);

    if (this->actionFunc != func_80898454) {
        Math_StepToF(&this->unk_2C4, 0.0f, 0.001f);
    }

    if (this->drawDmgEffAlpha > 0.0f) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FIRE) {
                this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 1.05f;
                this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 2.1f);
            } else {
                this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.55f;
                this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 1.1f);
            }
        } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 1.1f, 0.0275f)) {
            Actor_PlaySfx_Flagged(thisx, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
}

s32 EnPeehat_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnPeehat* this = (EnPeehat*)thisx;
    s32 pad;

    if (limbIndex == OBJECT_PH_LIMB_04) {
        rot->x = -this->unk_2B4;
    } else if ((limbIndex == OBJECT_PH_LIMB_03) ||
               ((limbIndex == OBJECT_PH_LIMB_17) &&
                ((this->actionFunc == func_80898594) || (this->actionFunc == func_80897520)))) {
        OPEN_DISPS(play->state.gfxCtx);
        Gfx* gfx = POLY_OPA_DISP;

        Matrix_Push();
        Matrix_RotateXFApply(this->unk_2BC * 0.115f);
        Matrix_RotateYF(this->unk_2BC * 0.13f, MTXMODE_APPLY);
        Matrix_RotateZF(this->unk_2BC * 0.1f, MTXMODE_APPLY);
        Matrix_Scale(1.0f - this->unk_2C4, this->unk_2C4 + 1.0f, 1.0f - this->unk_2C4, MTXMODE_APPLY);
        Matrix_RotateZF(-(this->unk_2BC * 0.1f), MTXMODE_APPLY);
        Matrix_RotateYF(-(this->unk_2BC * 0.13f), MTXMODE_APPLY);
        Matrix_RotateXFApply(-(this->unk_2BC * 0.115f));

        MATRIX_FINALIZE_AND_LOAD(&gfx[0], play->state.gfxCtx);
        gSPDisplayList(&gfx[1], *dList);

        Matrix_Pop();

        POLY_OPA_DISP = &gfx[2];

        CLOSE_DISPS(play->state.gfxCtx);
        return true;
    }
    return false;
}

static Vec3f D_80899570[] = {
    { 1300.0f, 1200.0f, 0.0f },  // PEEHAT_BODYPART_12
    { 1300.0f, -1200.0f, 0.0f }, // PEEHAT_BODYPART_13
    { 1300.0f, 0.0f, 1200.0f },  // PEEHAT_BODYPART_14
    { 1300.0f, 0.0f, -1200.0f }, // PEEHAT_BODYPART_15
};

static s8 sLimbToBodyParts[OBJECT_PH_LIMB_MAX] = {
    BODYPART_NONE,      // OBJECT_PH_LIMB_NONE
    BODYPART_NONE,      // OBJECT_PH_LIMB_01
    BODYPART_NONE,      // OBJECT_PH_LIMB_02
    BODYPART_NONE,      // OBJECT_PH_LIMB_03
    BODYPART_NONE,      // OBJECT_PH_LIMB_04
    BODYPART_NONE,      // OBJECT_PH_LIMB_05
    BODYPART_NONE,      // OBJECT_PH_LIMB_06
    PEEHAT_BODYPART_0,  // OBJECT_PH_LIMB_07
    BODYPART_NONE,      // OBJECT_PH_LIMB_08
    BODYPART_NONE,      // OBJECT_PH_LIMB_09
    PEEHAT_BODYPART_2,  // OBJECT_PH_LIMB_0A
    BODYPART_NONE,      // OBJECT_PH_LIMB_0B
    BODYPART_NONE,      // OBJECT_PH_LIMB_0C
    PEEHAT_BODYPART_4,  // OBJECT_PH_LIMB_0D
    BODYPART_NONE,      // OBJECT_PH_LIMB_0E
    BODYPART_NONE,      // OBJECT_PH_LIMB_0F
    PEEHAT_BODYPART_6,  // OBJECT_PH_LIMB_10
    BODYPART_NONE,      // OBJECT_PH_LIMB_11
    BODYPART_NONE,      // OBJECT_PH_LIMB_12
    PEEHAT_BODYPART_8,  // OBJECT_PH_LIMB_13
    BODYPART_NONE,      // OBJECT_PH_LIMB_14
    BODYPART_NONE,      // OBJECT_PH_LIMB_15
    PEEHAT_BODYPART_10, // OBJECT_PH_LIMB_16
    BODYPART_NONE,      // OBJECT_PH_LIMB_17
};

void EnPeehat_PostLimbDraw(PlayState* play2, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    PlayState* play = play2;
    EnPeehat* this = (EnPeehat*)thisx;
    s32 i;
    s32 bodyPartIndex = sLimbToBodyParts[limbIndex];
    Gfx* gfx;

    if (bodyPartIndex != BODYPART_NONE) {
        Matrix_MultVecX(2000.0f, &this->bodyPartsPos[bodyPartIndex]);
        Matrix_MultVecX(4000.0f, &this->bodyPartsPos[bodyPartIndex + 1]);
    }

    if (limbIndex == OBJECT_PH_LIMB_04) {
        Matrix_MultVecZ(5500.0f, &this->unk_2D4[0]);
        Matrix_MultVecZ(-5500.0f, &this->unk_2D4[1]);
    } else if ((limbIndex == OBJECT_PH_LIMB_03) && (thisx->params == 0)) {
        Vec3f* vec = &D_80899570[0];
        Vec3f* bodyPartPosPtr = &this->bodyPartsPos[PEEHAT_BODYPART_12];

        for (i = 0; i < ARRAY_COUNT(D_80899570); i++, vec++, bodyPartPosPtr++) {
            Matrix_MultVec3f(vec, bodyPartPosPtr);
        }

        Matrix_MultVecX(3000.0f, bodyPartPosPtr++); // PEEHAT_BODYPART_16
        Matrix_MultVecX(-400.0f, bodyPartPosPtr);   // PEEHAT_BODYPART_17

        OPEN_DISPS(play->state.gfxCtx);

        gfx = POLY_OPA_DISP;

        Matrix_Translate(-1000.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Collider_UpdateSphere(0, &this->colliderSphere);
        Matrix_Translate(500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateYF(3.2f, MTXMODE_APPLY);
        Matrix_Scale(0.3f, 0.2f, 0.2f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(&gfx[0], play->state.gfxCtx);
        gSPDisplayList(&gfx[1], *dList);

        POLY_OPA_DISP = &gfx[2];
        Math_Vec3s_ToVec3f(&this->actor.focus.pos, &this->colliderSphere.dim.worldSphere.center);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnPeehat_Draw(Actor* thisx, PlayState* play) {
    EnPeehat* this = (EnPeehat*)thisx;
    Vec3f sp58;
    Vec3f sp4C;
    Vec3f sp40;
    s32 i;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnPeehat_OverrideLimbDraw,
                      (this->actor.params == 0) ? EnPeehat_PostLimbDraw : NULL, &this->actor);

    if ((this->actor.speed != 0.0f) || (this->actor.velocity.y != 0.0f)) {
        Matrix_MultVecZ(4500.0f, &sp40);
        Matrix_MultVecZ(-4500.0f, &sp4C);
        Matrix_MultVecX(4500.0f, &sp58);
        Collider_SetTrisVertices(&this->colliderTris, 0, &sp40, &sp4C, &sp58);
        Matrix_MultVecX(-4500.0f, &sp58);
        Collider_SetTrisVertices(&this->colliderTris, 1, &sp40, &sp58, &sp4C);
    }

    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FIRE) {
        for (i = 0; i < PEEHAT_BODYPART_MAX; i++) {
            this->bodyPartsPos[i].y -= 50.0f;
        }
    }

    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, PEEHAT_BODYPART_MAX, this->drawDmgEffScale,
                            this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);
}
