/*
 * File: z_en_dekunuts.c
 * Overlay: ovl_En_Dekunuts
 * Description: Mad Scrub
 */

#include "z_en_dekunuts.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_Obj_Etcetera/z_obj_etcetera.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void EnDekunuts_Init(Actor* thisx, PlayState* play);
void EnDekunuts_Destroy(Actor* thisx, PlayState* play);
void EnDekunuts_Update(Actor* thisx, PlayState* play);
void EnDekunuts_Draw(Actor* thisx, PlayState* play);

void func_808BD428(EnDekunuts* this);
void func_808BD49C(EnDekunuts* this, PlayState* play);
void func_808BD78C(EnDekunuts* this);
void func_808BD7D4(EnDekunuts* this, PlayState* play);
void func_808BD870(EnDekunuts* this);
void func_808BD8D8(EnDekunuts* this, PlayState* play);
void func_808BDA08(EnDekunuts* this);
void func_808BDA4C(EnDekunuts* this, PlayState* play);
void func_808BDC9C(EnDekunuts* this);
void func_808BDD54(EnDekunuts* this, PlayState* play);
void func_808BDEF8(EnDekunuts* this, PlayState* play);
void func_808BDF60(EnDekunuts* this);
void func_808BDFB8(EnDekunuts* this, PlayState* play);
void func_808BE1CC(EnDekunuts* this);
void func_808BE22C(EnDekunuts* this, PlayState* play);
void func_808BE358(EnDekunuts* this, PlayState* play);
void func_808BE3FC(EnDekunuts* this, PlayState* play);
void func_808BE484(EnDekunuts* this);
void func_808BE4D4(EnDekunuts* this, PlayState* play);
void func_808BE680(EnDekunuts* this);
void func_808BE6C4(EnDekunuts* this, PlayState* play);

ActorProfile En_Dekunuts_Profile = {
    /**/ ACTOR_EN_DEKUNUTS,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_DEKUNUTS,
    /**/ sizeof(EnDekunuts),
    /**/ EnDekunuts_Init,
    /**/ EnDekunuts_Destroy,
    /**/ EnDekunuts_Update,
    /**/ EnDekunuts_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT6,
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
        ACELEM_ON,
        OCELEM_ON,
    },
    { 18, 32, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 18, 32, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
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
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x5),
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

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_MAD_SCRUB, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2600, ICHAIN_STOP),
};

void EnDekunuts_Init(Actor* thisx, PlayState* play) {
    EnDekunuts* this = (EnDekunuts*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
    SkelAnime_Init(play, &this->skelAnime, &gDekuScrubSkel, &gDekuScrubIdleAnim, this->jointTable, this->morphTable,
                   DEKU_SCRUB_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    this->unk_194 = ENDEKUNUTS_GET_FF00(&this->actor);
    thisx->params &= 0xFF;
    if ((this->unk_194 == ENDEKUNUTS_GET_FF00_FF) || (this->unk_194 == ENDEKUNUTS_GET_FF00_0)) {
        this->unk_194 = ENDEKUNUTS_GET_FF00_1;
    }

    if (this->actor.params == ENDEKUNUTS_GET_FF00_1) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->collider.base.colMaterial = COL_MATERIAL_NONE;
        this->collider.elem.acElemFlags |=
            (ACELEM_NO_HITMARK | ACELEM_NO_SWORD_SFX | ACELEM_NO_DAMAGE | ACELEM_NO_AT_INFO);
    } else if (this->actor.params == ENDEKUNUTS_GET_FF00_2) {
        this->actor.attentionRangeType = ATTENTION_RANGE_0;
    }

    func_808BD428(this);
}

void EnDekunuts_Destroy(Actor* thisx, PlayState* play) {
    EnDekunuts* this = (EnDekunuts*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_808BD348(EnDekunuts* this) {
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->drawDmgEffScale = 0.55f;
    this->drawDmgEffFrozenSteamScale = 825.0f * 0.001f;
    this->drawDmgEffAlpha = 1.0f;
    this->collider.base.colMaterial = COL_MATERIAL_HIT3;
    this->unk_190 = 80;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 80);
}

void func_808BD3B4(EnDekunuts* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->collider.base.colMaterial = COL_MATERIAL_HIT6;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, ENDEKUNUTS_BODYPART_MAX, 2, 0.2f, 0.2f);
    }
}

void func_808BD428(EnDekunuts* this) {
    Animation_PlayOnceSetSpeed(&this->skelAnime, &gDekuScrubUpAnim, 0.0f);
    this->unk_190 = Rand_S16Offset(100, 50);
    this->collider.dim.height = 5;
    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = func_808BD49C;
}

void func_808BD49C(EnDekunuts* this, PlayState* play) {
    s32 phi_v1 = false;

    if (this->skelAnime.playSpeed < 0.5f) {
        phi_v1 = true;
    }

    if (phi_v1 && (this->unk_190 != 0)) {
        this->unk_190--;
    }

    if (Animation_OnFrame(&this->skelAnime, 9.0f)) {
        this->collider.base.acFlags |= AC_ON;
    } else if (Animation_OnFrame(&this->skelAnime, 8.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_UP);
    }

    this->collider.dim.height = (s32)((CLAMP(this->skelAnime.curFrame, 9.0f, 12.0f) - 9.0f) * 9.0f) + 5;

    if (!phi_v1 && (this->actor.params == ENDEKUNUTS_GET_FF00_0) && (Player_GetMask(play) != PLAYER_MASK_STONE) &&
        (this->actor.xzDistToPlayer < 120.0f)) {
        func_808BDC9C(this);
    } else if (SkelAnime_Update(&this->skelAnime)) {
        if (((this->unk_190 == 0) && (this->actor.xzDistToPlayer > 320.0f)) ||
            (Player_GetMask(play) == PLAYER_MASK_STONE)) {
            func_808BD78C(this);
        } else {
            if (this->actor.params == ENDEKUNUTS_GET_FF00_1) {
                func_808BE680(this);
            } else if ((this->actor.params == ENDEKUNUTS_GET_FF00_0) && (this->actor.xzDistToPlayer < 120.0f)) {
                func_808BDC9C(this);
            } else {
                func_808BD870(this);
            }
        }
    }

    if (phi_v1 && ((this->actor.xzDistToPlayer > 160.0f) || (this->actor.params != ENDEKUNUTS_GET_FF00_0)) &&
        (((this->actor.params == ENDEKUNUTS_GET_FF00_0) && (fabsf(this->actor.playerHeightRel) < 120.0f)) ||
         ((this->actor.params == ENDEKUNUTS_GET_FF00_2) && (this->actor.playerHeightRel > -60.0f)) ||
         (this->actor.params == ENDEKUNUTS_GET_FF00_1)) &&
        ((this->unk_190 == 0) || (this->actor.xzDistToPlayer < 480.0f))) {
        this->skelAnime.playSpeed = 1.0f;
    }
}

void func_808BD78C(EnDekunuts* this) {
    Animation_PlayLoop(&this->skelAnime, &gDekuScrubLookAroundAnim);
    this->unk_190 = 2;
    this->actionFunc = func_808BD7D4;
}

void func_808BD7D4(EnDekunuts* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
        if (this->unk_190 != 0) {
            this->unk_190--;
        }
    }

    if ((this->unk_190 == 0) || ((this->actor.xzDistToPlayer < 120.0f) && Player_GetMask(play) != PLAYER_MASK_STONE)) {
        func_808BDC9C(this);
    }
}

void func_808BD870(EnDekunuts* this) {
    Animation_MorphToLoop(&this->skelAnime, &gDekuScrubIdleAnim, -3.0f);
    if (this->actionFunc == func_808BDA4C) {
        this->unk_190 = 4098;
    } else {
        this->unk_190 = 1;
    }
    this->actionFunc = func_808BD8D8;
}

void func_808BD8D8(EnDekunuts* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
        if (this->unk_190 != 0) {
            this->unk_190--;
        }
    }

    if (!(this->unk_190 & 0x1000)) {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    }

    if (this->unk_190 == 0x1000) {
        if ((this->actor.xzDistToPlayer > 480.0f) ||
            ((this->actor.params == ENDEKUNUTS_GET_FF00_0) && (this->actor.xzDistToPlayer < 120.0f)) ||
            (Player_GetMask(play) == PLAYER_MASK_STONE)) {
            func_808BDC9C(this);
        } else {
            func_808BDA08(this);
        }
    } else if (this->unk_190 == 0) {
        if (Player_GetMask(play) == PLAYER_MASK_STONE) {
            func_808BDC9C(this);
        } else {
            func_808BDA08(this);
        }
    }
}

void func_808BDA08(EnDekunuts* this) {
    Animation_PlayOnce(&this->skelAnime, &gDekuScrubSpitAnim);
    this->unk_190 = this->unk_194;
    this->actionFunc = func_808BDA4C;
}

void func_808BDA4C(EnDekunuts* this, PlayState* play) {
    Player* player;
    Vec3f sp58;
    s16 pitch;
    Vec3f pos;
    f32 val;
    s16 params;

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    if (this->actor.params == ENDEKUNUTS_GET_FF00_2) {
        player = GET_PLAYER(play);

        sp58.x = player->actor.world.pos.x;
        sp58.z = player->actor.world.pos.z;
        sp58.y = player->actor.world.pos.y + 40.0f;
        pitch = Actor_WorldPitchTowardPoint(&this->actor, &sp58);
        pitch = CLAMP(pitch, -0x3800, -0x2000);
        if (this->skelAnime.curFrame < 7.0f) {
            Math_ScaledStepToS(&this->actor.world.rot.x, pitch, 0x800);
        } else {
            Math_ScaledStepToS(&this->actor.world.rot.x, 0, 0x800);
        }
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        this->actor.world.rot.x = 0;
        func_808BD870(this);
    } else if (Animation_OnFrame(&this->skelAnime, 7.0f)) {
        val = Math_CosS(this->actor.world.rot.x) * 15.0f;
        pos.x = (Math_SinS(this->actor.shape.rot.y) * val) + this->actor.world.pos.x;
        pos.y = (this->actor.world.pos.y + 12.0f) - (Math_SinS(this->actor.world.rot.x) * 15.0f);
        pos.z = (Math_CosS(this->actor.shape.rot.y) * val) + this->actor.world.pos.z;
        params = (this->actor.params == ENDEKUNUTS_GET_FF00_2) ? ENDEKUNUTS_GET_FF00_2 : ENDEKUNUTS_GET_FF00_0;

        if (Actor_Spawn(&play->actorCtx, play, ACTOR_EN_NUTSBALL, pos.x, pos.y, pos.z, this->actor.world.rot.x,
                        this->actor.shape.rot.y, 0, params) != NULL) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_THROW);
        }
    } else if ((this->unk_190 >= 2) && Animation_OnFrame(&this->skelAnime, 12.0f)) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gDekuScrubSpitAnim, -3.0f);
        if (this->unk_190 != 0) {
            this->unk_190--;
        }
    }
}

void func_808BDC9C(EnDekunuts* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDekuScrubBurrowAnim, -5.0f);
    this->unk_190 = 0;
    Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_DOWN);
    this->actionFunc = func_808BDD54;
}

void func_808BDCF0(EnDekunuts* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDekuScrubBurrowAnim, -5.0f);
    this->collider.base.acFlags &= ~AC_ON;
    this->unk_190 = 80;
    Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_DOWN);
    this->actionFunc = func_808BDD54;
}

void func_808BDD54(EnDekunuts* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->unk_190 != 0) {
            this->unk_190--;
        }

        if (this->unk_190 == 0) {
            func_808BD428(this);
        }
    } else {
        this->collider.dim.height = (s32)((3.0f - CLAMP(this->skelAnime.curFrame, 1.0f, 3.0f)) * 12.0f) + 5;
    }

    if (Animation_OnFrame(&this->skelAnime, 4.0f)) {
        this->collider.base.acFlags &= ~AC_ON;
    }
    Math_ApproachF(&this->actor.world.pos.x, this->actor.home.pos.x, 0.5f, 3.0f);
    Math_ApproachF(&this->actor.world.pos.z, this->actor.home.pos.z, 0.5f, 3.0f);
}

void func_808BDE7C(EnDekunuts* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDekuScrubUnburrowAnim, -3.0f);
    this->collider.dim.height = 37;
    this->actor.colChkInfo.mass = 50;
    Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_DAMAGE);
    this->actor.world.rot.x = 0;
    this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = func_808BDEF8;
}

void func_808BDEF8(EnDekunuts* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        this->unk_192 = BINANG_ROT180(this->actor.yawTowardsPlayer);
        this->unk_18D = 3;
        func_808BDF60(this);
    }
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
}

void func_808BDF60(EnDekunuts* this) {
    Animation_PlayLoop(&this->skelAnime, &gDekuScrubRunAnim);
    this->unk_190 = 2;
    this->unk_18C = 0;
    this->collider.base.acFlags |= AC_ON;
    this->actionFunc = func_808BDFB8;
}

void func_808BDFB8(EnDekunuts* this, PlayState* play) {
    s16 yaw;
    s16 yaw2;

    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
        if (this->unk_190 != 0) {
            this->unk_190--;
        }
    }

    if (this->unk_18C != 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_WALK);
        this->unk_18C = 0;
    } else {
        this->unk_18C = 1;
    }

    Math_StepToF(&this->actor.speed, 7.5f, 1.0f);
    if (!Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_192, 1, 0xE38, 0xB6)) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
            this->unk_192 = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
        } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            this->unk_192 = this->actor.wallYaw;
        } else if (this->unk_18D == 0) {
            yaw = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
            yaw2 = yaw - this->actor.yawTowardsPlayer;
            if (ABS_ALT(yaw2) > 0x2000) {
                this->unk_192 = yaw;
            } else {
                this->unk_192 = (((yaw2 >= 0) ? 1 : -1) * -0x2000) + this->actor.yawTowardsPlayer;
            }
        } else {
            this->unk_192 = BINANG_ROT180(this->actor.yawTowardsPlayer);
        }
    }

    this->actor.shape.rot.y = BINANG_ROT180(this->actor.world.rot.y);
    if ((this->unk_18D == 0) && (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) < 20.0f) &&
        (fabsf(this->actor.world.pos.y - this->actor.home.pos.y) < 2.0f)) {
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->actor.flags &= ~ACTOR_FLAG_DRAW_CULLING_DISABLED;
        this->actor.speed = 0.0f;
        func_808BDC9C(this);
    } else if (this->unk_190 == 0) {
        func_808BE1CC(this);
    }
}

void func_808BE1CC(EnDekunuts* this) {
    Animation_PlayLoop(&this->skelAnime, &gDekuScrubPantingAnim);
    this->unk_190 = 3;
    this->actor.speed = 0.0f;
    if (this->unk_18D != 0) {
        this->unk_18D--;
    }
    this->actionFunc = func_808BE22C;
}

void func_808BE22C(EnDekunuts* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
        if (this->unk_190 != 0) {
            this->unk_190--;
        }
    }

    if (this->unk_190 == 0) {
        func_808BDF60(this);
    }
}

void func_808BE294(EnDekunuts* this, s32 arg1) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDekuScrubDamageAnim, -3.0f);
    if (this->actor.params == ENDEKUNUTS_GET_FF00_0) {
        this->actor.speed = 10.0f;
        if (arg1) {
            func_800BE504(&this->actor, &this->collider);
        }
    }

    this->actor.world.rot.x = 0;
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = func_808BE358;
    Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_DAMAGE);
    Actor_PlaySfx(&this->actor, NA_SE_EN_CUTBODY);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA,
                         Animation_GetLastFrame(&gDekuScrubDamageAnim));
}

void func_808BE358(EnDekunuts* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 0.0f, 1.0f);
    if (SkelAnime_Update(&this->skelAnime)) {
        func_808BE484(this);
    }
}

void func_808BE3A8(EnDekunuts* this) {
    this->actor.speed = 0.0f;
    if (this->actor.velocity.y > 0.0f) {
        this->actor.velocity.y = 0.0f;
    }

    func_800BE504(&this->actor, &this->collider);
    this->actionFunc = func_808BE3FC;
}

void func_808BE3FC(EnDekunuts* this, PlayState* play) {
    if (this->unk_190 != 0) {
        this->unk_190--;
    }

    if (this->unk_190 == 0) {
        func_808BD3B4(this, play);
        if (this->actor.params == ENDEKUNUTS_GET_FF00_1) {
            func_808BDCF0(this);
        } else if (this->actor.colChkInfo.health == 0) {
            func_808BE294(this, false);
        } else {
            this->actor.world.rot.y = this->actor.shape.rot.y;
            func_808BDF60(this);
        }
    }
}

void func_808BE484(EnDekunuts* this) {
    Animation_PlayOnce(&this->skelAnime, &gDekuScrubDieAnim);
    this->actionFunc = func_808BE4D4;
    this->actor.speed = 0.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_DEAD);
}

void func_808BE4D4(EnDekunuts* this, PlayState* play) {
    static Color_RGBA8 D_808BEF90 = { 255, 255, 255, 255 };
    static Color_RGBA8 D_808BEF94 = { 150, 150, 150, 0 };
    s32 pad;
    Vec3f sp40;

    if (SkelAnime_Update(&this->skelAnime)) {
        sp40.x = this->actor.world.pos.x;
        sp40.y = this->actor.world.pos.y + 18.0f;
        sp40.z = this->actor.world.pos.z;
        EffectSsDeadDb_Spawn(play, &sp40, &gZeroVec3f, &gZeroVec3f, &D_808BEF90, &D_808BEF94, 200, 0, 13);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 11, NA_SE_EN_EXTINCT);
        sp40.y = this->actor.world.pos.y + 10.0f;
        EffectSsHahen_SpawnBurst(play, &sp40, 3.0f, 0, 12, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0xE0);
        Actor_Spawn(&play->actorCtx, play, ACTOR_OBJ_ETCETERA, this->actor.home.pos.x, this->actor.home.pos.y,
                    this->actor.home.pos.z, 0, this->actor.home.rot.y, 0,
                    DEKU_FLOWER_PARAMS(DEKU_FLOWER_TYPE_PINK_WITH_INITIAL_BOUNCE));
        EffectSsHahen_SpawnBurst(play, &this->actor.home.pos, 6.0f, 0, 6, 2, 15, OBJECT_DEKUNUTS, 10,
                                 gDekuScrubFlowerFragmentDL);
        Actor_Kill(&this->actor);
    }
}

void func_808BE680(EnDekunuts* this) {
    Animation_MorphToLoop(&this->skelAnime, &gDekuScrubIdleAnim, -3.0f);
    this->actionFunc = func_808BE6C4;
}

void func_808BE6C4(EnDekunuts* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Player_GetMask(play) != PLAYER_MASK_STONE) {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    }

    if (this->actor.xzDistToPlayer > 480.0f) {
        func_808BDC9C(this);
    }
}

void func_808BE73C(EnDekunuts* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.elem);
        if ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) ||
            !(this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3)) {
            func_808BD3B4(this, play);
            if ((this->actor.colChkInfo.mass == 50) || (this->actor.params != ENDEKUNUTS_GET_FF00_0)) {
                if ((this->actor.params != ENDEKUNUTS_GET_FF00_1) && !Actor_ApplyDamage(&this->actor)) {
                    Enemy_StartFinishingBlow(play, &this->actor);
                }

                if (this->actor.params == ENDEKUNUTS_GET_FF00_1) {
                    func_808BDCF0(this);
                    return;
                }

                if (this->actor.colChkInfo.damageEffect == 3) {
                    func_808BD348(this);
                    if (this->actor.colChkInfo.health == 0) {
                        this->unk_190 = 3;
                        this->collider.base.acFlags &= ~AC_ON;
                    }
                    func_808BE3A8(this);
                    return;
                }

                if (this->actor.colChkInfo.damageEffect == 1) {
                    this->unk_190 = 40;
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                    func_808BE3A8(this);
                    return;
                }

                if (this->actor.colChkInfo.damageEffect == 2) {
                    this->drawDmgEffAlpha = 4.0f;
                    this->drawDmgEffScale = 0.55f;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                } else if (this->actor.colChkInfo.damageEffect == 4) {
                    this->drawDmgEffAlpha = 4.0f;
                    this->drawDmgEffScale = 0.55f;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->collider.elem.acDmgInfo.hitPos.x,
                                this->collider.elem.acDmgInfo.hitPos.y, this->collider.elem.acDmgInfo.hitPos.z, 0, 0, 0,
                                CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_LIGHT_RAYS));
                } else if (this->actor.colChkInfo.damageEffect == 5) {
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_LARGE;
                    this->drawDmgEffAlpha = 4.0f;
                    this->drawDmgEffScale = 0.55f;
                }

                func_808BE294(this, true);
            } else if (this->actor.params == ENDEKUNUTS_GET_FF00_0) {
                func_808BDE7C(this);
            }
        }
    } else if ((this->actor.colChkInfo.mass == MASS_IMMOVABLE) && (play->actorCtx.unk2 != 0) &&
               (this->actor.xyzDistToPlayerSq < SQ(200.0f))) {
        if (this->actor.params == ENDEKUNUTS_GET_FF00_1) {
            func_808BDCF0(this);
        } else if (this->actor.params == ENDEKUNUTS_GET_FF00_0) {
            func_808BDE7C(this);
        } else if (this->actor.colChkInfo.health != 0) {
            this->actor.colChkInfo.health = 0;
            Enemy_StartFinishingBlow(play, &this->actor);
            func_808BE294(this, false);
        }
    }
}

void EnDekunuts_Update(Actor* thisx, PlayState* play) {
    EnDekunuts* this = (EnDekunuts*)thisx;
    s32 pad;

    func_808BE73C(this, play);
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, this->collider.dim.radius, this->collider.dim.height,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    Collider_UpdateCylinder(&this->actor, &this->collider);

    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    if (this->drawDmgEffAlpha > 0.0f) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.275f;
            this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.55f);
        } else if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) &&
                   !Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.55f, (33.0f / 1600.0f))) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
}

s32 EnDekunuts_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnDekunuts* this = (EnDekunuts*)thisx;
    f32 arg1;
    f32 arg2;
    f32 arg3;
    f32 curFrame;

    if (this->actionFunc == func_808BDA4C) {
        if (limbIndex == DEKU_SCRUB_LIMB_SNOUT) {
            curFrame = this->skelAnime.curFrame;
            if (curFrame <= 6.0f) {
                arg2 = 1.0f - (curFrame * 0.0833f);
                arg1 = (curFrame * 0.1167f) + 1.0f;
                arg3 = (curFrame * 0.1167f) + 1.0f;
            } else if (curFrame <= 7.0f) {
                curFrame -= 6.0f;
                arg2 = 0.5f + curFrame;
                arg1 = 1.7f - (curFrame * 0.7f);
                arg3 = 1.7f - (curFrame * 0.7f);
            } else if (curFrame <= 10.0f) {
                arg2 = 1.5f - ((curFrame - 7.0f) * 0.1667f);
                arg1 = 1.0f;
                arg3 = 1.0f;
            } else {
                return 0;
            }
            Matrix_Scale(arg1, arg2, arg3, MTXMODE_APPLY);
        } else if ((limbIndex == DEKU_SCRUB_LIMB_HEAD) && (this->actor.params == ENDEKUNUTS_GET_FF00_2)) {
            rot->z = this->actor.world.rot.x;
        }
    }
    return false;
}

static s8 sLimbToBodyParts[DEKU_SCRUB_LIMB_MAX] = {
    BODYPART_NONE,         // DEKU_SCRUB_LIMB_NONE
    BODYPART_NONE,         // DEKU_SCRUB_LIMB_BODY
    BODYPART_NONE,         // DEKU_SCRUB_LIMB_HEAD
    ENDEKUNUTS_BODYPART_3, // DEKU_SCRUB_LIMB_HEADDRESS
    BODYPART_NONE,         // DEKU_SCRUB_LIMB_CREST
    ENDEKUNUTS_BODYPART_0, // DEKU_SCRUB_LIMB_SNOUT
    BODYPART_NONE,         // DEKU_SCRUB_LIMB_LEFT_LEG
    ENDEKUNUTS_BODYPART_1, // DEKU_SCRUB_LIMB_LEFT_FOOT
    BODYPART_NONE,         // DEKU_SCRUB_LIMB_RIGHT_LEG
    ENDEKUNUTS_BODYPART_2, // DEKU_SCRUB_LIMB_RIGHT_FOOT
};

static Vec3f D_808BEFA4[] = {
    { -1500.0f, 0.0f, -1700.0f }, // ENDEKUNUTS_BODYPART_4
    { -1500.0f, 0.0f, 1700.0f },  // ENDEKUNUTS_BODYPART_5
    { -2500.0f, -2000.0f, 0.0f }, // ENDEKUNUTS_BODYPART_6
    { -1000.0f, 1000.0f, 0.0f },  // ENDEKUNUTS_BODYPART_7
};

void EnDekunuts_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnDekunuts* this = (EnDekunuts*)thisx;
    s32 i;
    Vec3f* ptr1;
    Vec3f* ptr2;
    s32 bodyPartIndex = sLimbToBodyParts[limbIndex];

    if (bodyPartIndex != BODYPART_NONE) {
        if (bodyPartIndex <= ENDEKUNUTS_BODYPART_2) {
            Matrix_MultVecX(1000.0f, &this->bodyPartsPos[bodyPartIndex]);
        } else {
            // ENDEKUNUTS_BODYPART_3
            Matrix_MultZero(&this->bodyPartsPos[bodyPartIndex]);
            ptr1 = &D_808BEFA4[0];
            ptr2 = &this->bodyPartsPos[bodyPartIndex + 1];
            for (i = bodyPartIndex + 1; i < ENDEKUNUTS_BODYPART_MAX; i++) {
                Matrix_MultVec3f(ptr1, ptr2);
                ptr1++, ptr2++;
            }
        }
    }

    if (limbIndex == DEKU_SCRUB_LIMB_HEAD) {
        Matrix_MultZero(&this->actor.focus.pos);
    }
}

void EnDekunuts_Draw(Actor* thisx, PlayState* play) {
    EnDekunuts* this = (EnDekunuts*)thisx;

    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnDekunuts_OverrideLimbDraw,
                      EnDekunuts_PostLimbDraw, &this->actor);
    Matrix_SetTranslateRotateYXZ(this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z,
                                 &this->actor.home.rot);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    if (this->actor.colorFilterTimer != 0) {
        func_800AE5A0(play);
    }
    Gfx_DrawDListOpa(play, gDekuScrubFlowerDL);
    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, ENDEKUNUTS_BODYPART_MAX, this->drawDmgEffScale,
                            this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);
}
