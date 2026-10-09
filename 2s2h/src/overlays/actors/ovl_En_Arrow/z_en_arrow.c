/*
 * File: z_en_arrow.c
 * Overlay: ovl_En_Arrow
 * Description: Arrows and other projectiles
 */

#include "z_en_arrow.h"
#include "overlays/effects/ovl_Effect_Ss_Hitmark/z_eff_ss_hitmark.h"
#include "overlays/effects/ovl_Effect_Ss_Sbn/z_eff_ss_sbn.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#include "2s2h/framebuffer_effects.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnArrow_Init(Actor* thisx, PlayState* play);
void EnArrow_Destroy(Actor* thisx, PlayState* play);
void EnArrow_Update(Actor* thisx, PlayState* play);
void EnArrow_Draw(Actor* thisx, PlayState* play);

void func_8088A594(EnArrow* this, PlayState* play);
void func_8088ACE0(EnArrow* this, PlayState* play);
void func_8088B630(EnArrow* this, PlayState* play);
void func_8088B6B0(EnArrow* this, PlayState* play);

ActorProfile En_Arrow_Profile = {
    /**/ ACTOR_EN_ARROW,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnArrow),
    /**/ EnArrow_Init,
    /**/ EnArrow_Destroy,
    /**/ EnArrow_Update,
    /**/ EnArrow_Draw,
};

static ColliderQuadInit sQuadInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000020, 0x00, 0x02 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_NEAREST | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(terminalVelocity, -150, ICHAIN_STOP),
};

void EnArrow_Init(Actor* thisx, PlayState* play) {
    static EffectBlureInit2 D_8088C234 = {
        0,
        EFFECT_BLURE_ELEMENT_FLAG_4,
        0,
        { 0, 255, 200, 255 },
        { 0, 255, 255, 255 },
        { 0, 255, 200, 0 },
        { 0, 255, 255, 0 },
        16,
        0,
        EFF_BLURE_DRAW_MODE_SIMPLE_ALT_COLORS,
        0,
        { 255, 255, 170, 255 },
        { 0, 150, 0, 0 },
    };
    static EffectBlureInit2 D_8088C258 = {
        0,
        EFFECT_BLURE_ELEMENT_FLAG_4,
        0,
        { 0, 255, 200, 255 },
        { 0, 255, 255, 255 },
        { 0, 255, 200, 0 },
        { 0, 255, 255, 0 },
        16,
        0,
        EFF_BLURE_DRAW_MODE_SIMPLE_ALT_COLORS,
        0,
        { 255, 200, 0, 255 },
        { 255, 0, 0, 0 },
    };
    static EffectBlureInit2 D_8088C27C = {
        0,
        EFFECT_BLURE_ELEMENT_FLAG_4,
        0,
        { 0, 255, 200, 255 },
        { 0, 255, 255, 255 },
        { 0, 255, 200, 0 },
        { 0, 255, 255, 0 },
        16,
        0,
        EFF_BLURE_DRAW_MODE_SIMPLE_ALT_COLORS,
        0,
        { 170, 255, 255, 255 },
        { 0, 0x64, 255, 0 },
    };
    static EffectBlureInit2 D_8088C2A0 = {
        0,
        EFFECT_BLURE_ELEMENT_FLAG_4,
        0,
        { 0, 255, 200, 255 },
        { 0, 255, 255, 255 },
        { 0, 255, 200, 0 },
        { 0, 255, 255, 0 },
        16,
        0,
        EFF_BLURE_DRAW_MODE_SIMPLE_ALT_COLORS,
        0,
        { 255, 255, 170, 255 },
        { 255, 255, 0, 0 },
    };
    EnArrow* this = (EnArrow*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    if (this->actor.params == -ARROW_TYPE_DEKU_NUT) {
        this->unk_263 = 1;
        this->actor.params = ARROW_TYPE_DEKU_NUT;
    }

    if (ARROW_IS_ARROW(this->actor.params)) {
        SkelAnime_Init(play, &this->arrow.skelAnime, &gameplay_keep_Skel_014560, &gameplay_keep_Anim_0128BC,
                       this->arrow.jointTable, this->arrow.jointTable, ARROW_LIMB_MAX);
        if (this->actor.params < ARROW_TYPE_FIRE) {
            if (this->actor.params == ARROW_TYPE_NORMAL_HORSE) {
                D_8088C234.elemDuration = 4;
            } else {
                D_8088C234.elemDuration = 16;
            }
            Effect_Add(play, &this->unk_240, EFFECT_BLURE2, 0, 0, &D_8088C234);
        } else if (this->actor.params == ARROW_TYPE_FIRE) {
            Effect_Add(play, &this->unk_240, EFFECT_BLURE2, 0, 0, &D_8088C258);
        } else if (this->actor.params == ARROW_TYPE_ICE) {
            Effect_Add(play, &this->unk_240, EFFECT_BLURE2, 0, 0, &D_8088C27C);
        } else if (this->actor.params == ARROW_TYPE_LIGHT) {
            Effect_Add(play, &this->unk_240, EFFECT_BLURE2, 0, 0, &D_8088C2A0);
        }
    }

    Collider_InitQuad(play, &this->collider);
    Collider_SetQuad(play, &this->collider, &this->actor, &sQuadInit);

    if (ARROW_IS_ARROW(this->actor.params)) {
        this->collider.elem.atElemFlags &= ~(ATELEM_SFX_WOOD | ATELEM_SFX_HARD);
        this->collider.elem.atElemFlags |= 0;
    }

    if (this->actor.params < ARROW_TYPE_NORMAL_LIT) {
        this->collider.base.atFlags = (AT_TYPE_ENEMY | AT_ON);
    } else {
        this->collider.elem.atDmgInfo.dmgFlags = Actor_GetArrowDmgFlags(this->actor.params);
        if (this->actor.params == ARROW_TYPE_DEKU_NUT) {
            this->collider.elem.atDmgInfo.damage = 1;
        }

        if (this->actor.params == ARROW_TYPE_DEKU_BUBBLE) {
            Actor_SetScale(&this->actor, 1.0f);
        }
    }
    this->actionFunc = func_8088A594;
}

void EnArrow_Destroy(Actor* thisx, PlayState* play) {
    EnArrow* this = (EnArrow*)thisx;

    if (ARROW_IS_ARROW(this->actor.params)) {
        Effect_Destroy(play, this->unk_240);
    }

    Collider_DestroyQuad(play, &this->collider);

    if ((this->unk_264 != NULL) && (this->unk_264->update != NULL)) {
        this->unk_264->flags &= ~ACTOR_FLAG_ATTACHED_TO_ARROW;
    }

    if (ARROW_IS_MAGICAL(this->actor.params) && (this->actor.child == NULL)) {
        Magic_Reset(play);
    }
}

void func_8088A514(EnArrow* this) {
    f32 temp_f0 = 16.0f - this->bubble.unk_144;

    Actor_SetSpeeds(&this->actor, CLAMP(temp_f0, 1.0f, 80.0f));
}

void func_8088A594(EnArrow* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.parent != NULL) {
        if (this->actor.params == ARROW_TYPE_DEKU_BUBBLE) {
            if (Math_SmoothStepToF(&this->bubble.unk_144, 16.0f, 0.07f, 1.8f, 0.0f) > 0.5f) {
                Actor_PlaySfx_Flagged(&this->actor, NA_SE_PL_DEKUNUTS_BUBLE_BREATH - SFX_FLAG);
                return;
            }

            this->bubble.unk_148++;
            if (this->bubble.unk_148 > 20) {
                this->actionFunc = func_8088ACE0;
                Magic_Reset(play);
            }
        }
    } else {
        if ((this->actor.params != ARROW_TYPE_DEKU_NUT) && (player->unk_D57 == 0)) {
            if (this->actor.params == ARROW_TYPE_DEKU_BUBBLE) {
                Magic_Reset(play);
            }
            Actor_Kill(&this->actor);
            return;
        }

        switch (this->actor.params) {
            case ARROW_TYPE_SLINGSHOT:
                Player_PlaySfx(player, NA_SE_IT_SLING_SHOT);
                break;

            case ARROW_TYPE_NORMAL_LIT:
            case ARROW_TYPE_NORMAL_HORSE:
            case ARROW_TYPE_NORMAL:
                Player_PlaySfx(player, NA_SE_IT_ARROW_SHOT);
                break;

            case ARROW_TYPE_FIRE:
            case ARROW_TYPE_ICE:
            case ARROW_TYPE_LIGHT:
                Player_PlaySfx(player, NA_SE_IT_MAGIC_ARROW_SHOT);

            case ARROW_TYPE_DEKU_BUBBLE:
                Player_PlaySfx(player, NA_SE_PL_DEKUNUTS_FIRE);
                break;

            default:
                break;
        }

        this->actionFunc = func_8088ACE0;
        Math_Vec3f_Copy(&this->unk_228, &this->actor.world.pos);

        if (this->actor.params == ARROW_TYPE_DEKU_BUBBLE) {
            this->bubble.unk_144 = CLAMP_MIN(this->bubble.unk_144, 3.5f);
            func_8088A514(this);
            this->unk_260 = 99;
            Magic_Reset(play);
        } else if (!ARROW_IS_ARROW(this->actor.params)) {
            if ((this->actor.params == ARROW_TYPE_DEKU_NUT) && (this->actor.world.rot.x < 0)) {
                Actor_SetScale(&this->actor, 0.009f);
                this->unk_260 = 40;
            } else {
                Actor_SetSpeeds(&this->actor, 80.0f);
                this->unk_260 = 15;
            }
            this->actor.shape.rot.x = 0;
            this->actor.shape.rot.y = 0;
            this->actor.shape.rot.z = 0;
        } else {
            Actor_SetSpeeds(&this->actor, 150.0f);
            this->unk_260 = 16;
        }
    }
}

void func_8088A7D8(PlayState* play, EnArrow* this) {
    this->actionFunc = func_8088B6B0;
    Animation_PlayOnce(&this->arrow.skelAnime, &gameplay_keep_Anim_012860);
    this->actor.world.rot.y += (s32)(0x6000 * (Rand_ZeroOne() - 0.5f)) + 0x8000;
    this->actor.speed *= 0.02f + (0.02f * Rand_ZeroOne());
    this->actor.gravity = -1.5f;
    this->unk_260 = 50;
    this->unk_263 = 1;
}

void func_8088A894(EnArrow* this, PlayState* play) {
    CollisionPoly* sp74;
    Vec3f sp68;
    Vec3f sp5C;
    Vec3f sp50;
    f32 sp4C;
    f32 temp_f0;
    s32 bgId;

    Math_Vec3f_Diff(&this->actor.world.pos, &this->unk_228, &sp68);
    sp4C = ((this->actor.world.pos.x - this->unk_264->world.pos.x) * sp68.x) +
           ((this->actor.world.pos.y - this->unk_264->world.pos.y) * sp68.y) +
           ((this->actor.world.pos.z - this->unk_264->world.pos.z) * sp68.z);
    if (sp4C < 0.0f) {
        return;
    }

    temp_f0 = Math3D_Vec3fMagnitudeSq(&sp68);
    if (temp_f0 < 1.0f) {
        return;
    }

    temp_f0 = sp4C / temp_f0;
    Math_Vec3f_Scale(&sp68, temp_f0);
    Math_Vec3f_Sum(&this->unk_264->world.pos, &sp68, &sp5C);
    if (BgCheck_EntityLineTest1(&play->colCtx, &this->unk_264->world.pos, &sp5C, &sp50, &sp74, true, true, true, true,
                                &bgId)) {
        this->unk_264->world.pos.x = ((sp5C.x <= sp50.x) ? 1.0f : -1.0f) + sp50.x;
        this->unk_264->world.pos.y = ((sp5C.y <= sp50.y) ? 1.0f : -1.0f) + sp50.y;
        this->unk_264->world.pos.z = ((sp5C.z <= sp50.z) ? 1.0f : -1.0f) + sp50.z;
    } else {
        Math_Vec3f_Copy(&this->unk_264->world.pos, &sp5C);
    }
}

void func_8088AA98(EnArrow* this, PlayState* play) {
    WaterBox* waterBox;
    f32 sp50 = this->actor.world.pos.y;
    Vec3f sp44;
    f32 temp_f0;

    if (WaterBox_GetSurface1(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &sp50, &waterBox) &&
        (this->actor.world.pos.y < sp50) && !(this->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
        this->actor.bgCheckFlags |= BGCHECKFLAG_WATER;

        Math_Vec3f_Diff(&this->actor.world.pos, &this->actor.home.pos, &sp44);

        if (sp44.y != 0.0f) {
            temp_f0 = sqrtf(SQ(sp44.x) + SQ(sp44.z));
            if (temp_f0 != 0.0f) {
                temp_f0 = (((sp50 - this->actor.home.pos.y) / sp44.y) * temp_f0) / temp_f0;
            }
            sp44.x = this->actor.home.pos.x + (sp44.x * temp_f0);
            sp44.y = sp50;
            sp44.z = this->actor.home.pos.z + (sp44.z * temp_f0);
            EffectSsGSplash_Spawn(play, &sp44, NULL, NULL, 0, 300);
        }

        Actor_PlaySfx(&this->actor, NA_SE_EV_DIVE_INTO_WATER_L);

        EffectSsGRipple_Spawn(play, &sp44, 100, 500, 0);
        EffectSsGRipple_Spawn(play, &sp44, 100, 500, 4);
        EffectSsGRipple_Spawn(play, &sp44, 100, 500, 8);

        if ((this->actor.params == ARROW_TYPE_ICE) || (this->actor.params == ARROW_TYPE_FIRE)) {
            if ((this->actor.params == ARROW_TYPE_ICE) && (func_8088B6B0 != this->actionFunc)) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_BG_ICEFLOE, sp44.x, sp44.y, sp44.z, 0, 0, 0, 300);
                Actor_Kill(&this->actor);
                return;
            }

            this->actor.params = ARROW_TYPE_NORMAL;
            this->collider.elem.atDmgInfo.dmgFlags = 0x20;

            if (this->actor.child != NULL) {
                Actor_Kill(this->actor.child);
                return;
            }

            Magic_Reset(play);
        }
    }
}

void func_8088ACE0(EnArrow* this, PlayState* play) {
    CollisionPoly* spAC;
    s32 bgId;
    Vec3f sp9C;
    s32 phi_a2 = 0;
    EffectSsSbnInitParams sp84;
    u16 sp82;
    Actor* sp7C;
    f32 sp78;
    f32 sp74;
    f32 temp_f12_2;
    s32 sp50;

    if ((DECR(this->unk_260) == 0) ||
        ((this->actor.params == ARROW_TYPE_DEKU_BUBBLE) &&
         ((this->unk_262 != 0) || (phi_a2 = (this->collider.base.atFlags & AT_HIT) != 0)))) {
        if (this->actor.params == ARROW_TYPE_DEKU_BUBBLE) {
            if (phi_a2 && (this->collider.elem.atHitElem->elemMaterial != ELEM_MATERIAL_UNK4) &&
                (this->collider.base.atFlags & AT_BOUNCED)) {
                if ((this->collider.base.at != NULL) && (this->collider.base.at->id != ACTOR_OBJ_SYOKUDAI)) {
                    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.prevPos);
                    this->actor.world.rot.y += BINANG_ROT180(TRUNCF_BINANG(Rand_CenteredFloat(0x1F40)));
                    this->actor.velocity.y = -this->actor.velocity.y;
                    this->bubble.unk_149 = -1;
                    return;
                }
            }

            if ((this->unk_262 != 0) || phi_a2) {
                EffectSsStone1_Spawn(play, &this->actor.world.pos, 0);
            }

            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_IT_DEKUNUTS_BUBLE_VANISH);

            if ((this->unk_262 != 0) && (this->actor.wallBgId == BG_ACTOR_MAX)) {

                Math_Vec3f_Copy(&sp84.pos, &this->actor.world.pos);
                sp84.colPoly = this->actor.wallPoly;
                sp84.scale = this->bubble.unk_144;
                EffectSs_Spawn(play, EFFECT_SS_SBN, 128, &sp84);
            }
        }
        Actor_Kill(&this->actor);
        return;
    }

    sp50 = (this->actor.params != ARROW_TYPE_NORMAL_LIT) && (this->actor.params < ARROW_TYPE_DEKU_NUT) &&
           (this->collider.base.atFlags & AT_HIT);

    if (sp50 || (this->unk_262 != 0)) {
        if (!ARROW_IS_ARROW(this->actor.params)) {
            if (sp50) {
                this->actor.world.pos.x = (this->actor.world.pos.x + this->actor.prevPos.x) * 0.5f;
                this->actor.world.pos.y = (this->actor.world.pos.y + this->actor.prevPos.y) * 0.5f;
                this->actor.world.pos.z = (this->actor.world.pos.z + this->actor.prevPos.z) * 0.5f;
            }

            if (this->actor.params == ARROW_TYPE_DEKU_NUT) {
                R_TRANS_FADE_FLASH_ALPHA_STEP = -1;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_M_FIRE1, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, 0, 0, 0, this->actor.speed == 0.0f);
                sp82 = NA_SE_IT_DEKU;
            } else {
                sp82 = NA_SE_IT_SLING_REFLECT;
            }
            EffectSsStone1_Spawn(play, &this->actor.world.pos, 0);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, sp82);
            Actor_Kill(&this->actor);
        } else {
            EffectSsHitmark_SpawnCustomScale(play, EFFECT_HITMARK_WHITE, 150, &this->actor.world.pos);

            if (sp50 && (this->collider.elem.atHitElem->elemMaterial != ELEM_MATERIAL_UNK4)) {
                sp7C = this->collider.base.at;

                if ((sp7C->update != NULL) && !(this->collider.base.atFlags & AT_BOUNCED) &&
                    (sp7C->flags & ACTOR_FLAG_CAN_ATTACH_TO_ARROW)) {
                    this->unk_264 = sp7C;
                    func_8088A894(this, play);
                    Math_Vec3f_Diff(&sp7C->world.pos, &this->actor.world.pos, &this->unk_268);
                    sp7C->flags |= ACTOR_FLAG_ATTACHED_TO_ARROW;
                    this->collider.base.atFlags &= ~AT_HIT;
                    this->actor.speed *= 0.5f;
                    this->actor.velocity.y *= 0.5f;
                } else {
                    this->unk_261 |= 1;
                    this->unk_261 |= 2;
                    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.prevPos);
                    func_8088A7D8(play, this);
                    Actor_PlaySfx(&this->actor, NA_SE_IT_HOOKSHOT_STICK_CRE);
                }
            } else if (this->unk_262 != 0) {
                this->actionFunc = func_8088B630;
                Animation_PlayOnce(&this->arrow.skelAnime, &gameplay_keep_Anim_0128BC);
                if (this->actor.params >= ARROW_TYPE_NORMAL_LIT) {
                    this->unk_260 = 60;
                } else {
                    this->unk_260 = 20;
                }
                if (ARROW_IS_MAGICAL(this->actor.params)) {
                    this->actor.draw = NULL;
                }
                Actor_PlaySfx(&this->actor, NA_SE_IT_ARROW_STICK_OBJ);
                this->unk_261 |= 1;
            }
        }
    } else {
        func_8088AA98(this, play);
        if (this->actor.params == ARROW_TYPE_DEKU_BUBBLE) {
            if (this->bubble.unk_149 == 0) {
                sp78 = sqrtf(SQ(this->actor.speed) + SQ(this->actor.velocity.y));
                sp74 = Math_SinS(this->actor.world.rot.y) * this->actor.speed;
                temp_f12_2 = Math_CosS(this->actor.world.rot.y) * this->actor.speed;

                this->actor.prevPos.x = this->actor.world.pos.x - (sp74 * (10.0f / sp78));
                this->actor.prevPos.y = this->actor.world.pos.y - (this->actor.velocity.y * (10.0f / sp78));
                this->actor.prevPos.z = this->actor.world.pos.z - (temp_f12_2 * (10.0f / sp78));

                this->bubble.unk_149 = 1;
            }

            if (Math_StepToF(&this->bubble.unk_144, 1.0f, 0.4f)) {
                this->unk_260 = 0;
            } else {
                this->bubble.unk_14A += TRUNCF_BINANG(this->bubble.unk_144 * (500.0f + Rand_ZeroFloat(1400.0f)));
                this->actor.world.rot.x += TRUNCF_BINANG(500.0f * Math_SinS(this->bubble.unk_14A));
                this->actor.shape.rot.x = this->actor.world.rot.x;

                this->bubble.unk_14C += TRUNCF_BINANG(this->bubble.unk_144 * (500.0f + Rand_ZeroFloat(1400.0f)));
                this->actor.world.rot.y += TRUNCF_BINANG(500.0f * Math_SinS(this->bubble.unk_14C));

                this->actor.shape.rot.y = this->actor.world.rot.y;

                func_8088A514(this);
            }
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_IT_DEKUNUTS_BUBLE_SHOT_LEVEL - SFX_FLAG);
        } else if (this->unk_260 < 7) {
            this->actor.gravity = -0.4f;
        }

        Math_Vec3f_Copy(&this->unk_228, &this->actor.world.pos);

        if (this->actor.speed == 0.0f) {
            this->actor.velocity.y -= 1.0f;
            if (this->actor.velocity.y < this->actor.terminalVelocity) {
                this->actor.velocity.y = this->actor.terminalVelocity;
            }
            Actor_UpdatePos(&this->actor);
        } else {
            Actor_MoveWithGravity(&this->actor);
        }

        if ((this->unk_262 = BgCheck_ProjectileLineTest(&play->colCtx, &this->actor.prevPos, &this->actor.world.pos,
                                                        &sp9C, &this->actor.wallPoly, true, true, true, true, &bgId))) {
            // `func_800B90AC` only returns a boolean, and does not process any code
            func_800B90AC(play, &this->actor, this->actor.wallPoly, bgId, &sp9C);
            Math_Vec3f_Copy(&this->actor.world.pos, &sp9C);
            this->actor.wallBgId = bgId;
        }

        if (ARROW_IS_ARROW(this->actor.params)) {
            this->actor.shape.rot.x = Math_Atan2S_XY(this->actor.speed, -this->actor.velocity.y);
        }
    }

    if (this->unk_264 != NULL) {
        if (this->unk_264->update != NULL) {
            Vec3f sp60;
            Vec3f sp54;

            Math_Vec3f_Sum(&this->unk_228, &this->unk_268, &sp60);
            Math_Vec3f_Sum(&this->actor.world.pos, &this->unk_268, &sp54);

            if (BgCheck_EntityLineTest1(&play->colCtx, &sp60, &sp54, &sp9C, &spAC, true, true, true, true, &bgId)) {
                this->unk_264->world.pos.x = ((sp54.x <= sp9C.x) ? 1.0f : -1.0f) + sp9C.x;
                this->unk_264->world.pos.y = ((sp54.y <= sp9C.y) ? 1.0f : -1.0f) + sp9C.y;
                this->unk_264->world.pos.z = ((sp54.z <= sp9C.z) ? 1.0f : -1.0f) + sp9C.z;

                Math_Vec3f_Diff(&this->unk_264->world.pos, &this->actor.world.pos, &this->unk_268);
                this->unk_264->flags &= ~ACTOR_FLAG_ATTACHED_TO_ARROW;
                this->unk_264 = NULL;
            } else {
                Math_Vec3f_Sum(&this->actor.world.pos, &this->unk_268, &this->unk_264->world.pos);
            }

            if ((this->unk_262 != 0) && (this->unk_264 != NULL)) {
                this->unk_264->flags &= ~ACTOR_FLAG_ATTACHED_TO_ARROW;
                this->unk_264 = NULL;
            }
        } else {
            this->unk_264 = NULL;
        }
    }
}

void func_8088B630(EnArrow* this, PlayState* play) {
    SkelAnime_Update(&this->arrow.skelAnime);

    if (this->actor.wallBgId != BG_ACTOR_MAX) {
        DynaPolyActor_TransformCarriedActor(&play->colCtx, this->actor.wallBgId, &this->actor);
    }

    if (DECR(this->unk_260) == 0) {
        Actor_Kill(&this->actor);
    }
}

void func_8088B6B0(EnArrow* this, PlayState* play) {
    SkelAnime_Update(&this->arrow.skelAnime);
    Actor_MoveWithGravity(&this->actor);
    func_8088AA98(this, play);

    if (DECR(this->unk_260) == 0) {
        Actor_Kill(&this->actor);
    }
}

void EnArrow_Update(Actor* thisx, PlayState* play) {
    static Vec3f sVelocity = { 0.0f, 0.5f, 0.0f };
    static Vec3f sAccel = { 0.0f, 0.5f, 0.0f };
    static Color_RGBA8 sPrimColor = { 255, 255, 100, 255 };
    static Color_RGBA8 sEnvColor = { 255, 50, 0, 0 };
    s32 pad;
    EnArrow* this = (EnArrow*)thisx;
    Player* player = GET_PLAYER(play);

    if ((this->unk_263 != 0) ||
        ((this->actor.params >= ARROW_TYPE_NORMAL_LIT) &&
         ((this->actor.params == ARROW_TYPE_DEKU_BUBBLE) || (player->unk_D57 != 0))) ||
        !Player_InBlockingCsMode(play, player)) {
        this->actionFunc(this, play);
    }

    if (ARROW_IS_MAGICAL(this->actor.params)) {
        s16 sp44[] = {
            ACTOR_ARROW_FIRE,  // ARROW_MAGIC_FIRE
            ACTOR_ARROW_ICE,   // ARROW_MAGIC_ICE
            ACTOR_ARROW_LIGHT, // ARROW_MAGIC_LIGHT
        };

        if (this->actor.child == NULL) {
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, sp44[ARROW_GET_MAGIC_FROM_TYPE(this->actor.params)],
                               this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
        }
    } else if (this->actor.params == ARROW_TYPE_NORMAL_LIT) {
        func_800B0EB0(play, &this->unk_234, &sVelocity, &sAccel, &sPrimColor, &sEnvColor, 100, 0, 8);
    }

    Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.prevPos);
}

typedef struct {
    /* 0x00 */ Vec3f unk_00[2];
    /* 0x18 */ Vec3f unk_18[2][2];
    /* 0x48 */ Vec3f unk_48;
} EnArrowUnkStruct; // size = 0x54

void func_8088B88C(PlayState* play, EnArrow* this, EnArrowUnkStruct* arg2) {
    Vec3f* sp4C;
    Vec3f sp40;
    Vec3f sp34;
    s32 sp30;

    Matrix_MultVec3f(&arg2->unk_48, &this->unk_234);
    if (func_8088ACE0 == this->actionFunc) {
        if (!this->unk_244.active) {
            sp4C = arg2->unk_00;
        } else {
            sp4C = arg2->unk_18[play->gameplayFrames % 2];
        }
        Matrix_MultVec3f(&sp4C[0], &sp40);
        Matrix_MultVec3f(&sp4C[1], &sp34);
        if (this->actor.params < ARROW_TYPE_DEKU_NUT) {
            sp30 = ARROW_IS_ARROW(this->actor.params);
            if (this->unk_264 == NULL) {
                sp30 &= func_80126440(play, &this->collider, &this->unk_244, &sp40, &sp34);
            } else if (sp30 && (sp40.x == this->unk_244.tip.x) && (sp40.y == this->unk_244.tip.y) &&
                       (sp40.z == this->unk_244.tip.z) && (sp34.x == this->unk_244.base.x) &&
                       (sp34.y == this->unk_244.base.y) && (sp34.z == this->unk_244.base.z)) {
                sp30 = false;
            }
            if (sp30) {
                EffectBlure_AddVertex(Effect_GetByIndex(this->unk_240), &sp40, &sp34);
            }
        }
    }
}

void EnArrow_Draw(Actor* thisx, PlayState* play) {
    static EnArrowUnkStruct D_8088C2EC[] = {
        {
            {
                { 400.0f, 400.0f, 0.0f },
                { -400.0f, 400.0f, 0.0f },
            },
            {
                {
                    { 400.0f, 400.0f, 1500.0f },
                    { -400.0f, 400.0f, 1500.0f },
                },
                {
                    { 400.0f, 400.0f, 1500.0f },
                    { -400.0f, 400.0f, 1500.0f },
                },
            },
            { 0.0f, 0.0f, -300.0f },
        },
        {
            {
                { 20.0f, -20.0f, 20.0f },
                { 20.0f, -20.0f, 20.0f },
            },
            {
                {
                    { 20.0f, -20.0f, 20.0f },
                    { -20.0f, 20.0f, 20.0f },
                },
                {
                    { -20.0f, -20.0f, 20.0f },
                    { 20.0f, 20.0f, 20.0f },
                },
            },
            { 0.0f, 0.0f, 20.0f },
        },
    };
    EnArrow* this = (EnArrow*)thisx;
    s32 phi_v0;

    if (ARROW_IS_ARROW(this->actor.params)) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        SkelAnime_DrawLod(play, this->arrow.skelAnime.skeleton, this->arrow.skelAnime.jointTable, NULL, NULL,
                          &this->actor, this->actor.projectedPos.z < 160.0f ? 0 : 1);
    } else if (this->actor.params == ARROW_TYPE_DEKU_BUBBLE) {
        s32 spA4 = 255 - (s32)(this->bubble.unk_144 * 4.0f);
        f32 spA0 = (this->actor.speed * 0.1f) + 1.0f;
        f32 sp9C = (1.0f / spA0);

        OPEN_DISPS(play->state.gfxCtx);

        if (this->bubble.unk_149 >= 0) {
            func_8088B88C(play, this, &D_8088C2EC[1]);
        } else {
            Collider_ResetQuadAT(play, &this->collider.base);
        }

        sp9C *= 0.002f;
        spA0 *= 0.002f;

        Matrix_Scale(this->bubble.unk_144 * sp9C, this->bubble.unk_144 * sp9C, this->bubble.unk_144 * spA0,
                     MTXMODE_APPLY);
        Matrix_Translate(0.0f, 0.0f, 460.0f, MTXMODE_APPLY);

        if (this->actor.speed == 0.0f) {
            func_800B8118(&this->actor, play, 0);

            // #region 2S2H [Port] Capture framebuffer to current frame, byteswaped, so that it will
            // be read from seg 0x0F in gameplay_keep_DL_06F380
            Gfx* gfx = POLY_XLU_DISP;
            FB_WriteFramebufferSliceToCPU(&gfx, play->state.gfxCtx->curFrameBuffer, true);
            POLY_XLU_DISP = gfx;
            // Only a portion of the buffer is used as a texture, so we need to invalidate by the same offset
            // Offset derived from ((ult * width + uls) * 2), with values taken from the DL directly
            gSPInvalidateTexCache(POLY_XLU_DISP++,
                                  (uintptr_t)(play->state.gfxCtx->curFrameBuffer) + ((104 * 320 + 144) * 2));
            // #endregion

            gSPDisplayList(POLY_XLU_DISP++, gameplay_keep_DL_06F380);
            gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
            gDPSetCombineLERP(POLY_XLU_DISP++, TEXEL1, 0, PRIM_LOD_FRAC, TEXEL0, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0,
                              COMBINED, 0, ENVIRONMENT, 0, COMBINED, 0, ENVIRONMENT, 0);
            gDPSetEnvColor(POLY_XLU_DISP++, 230, 225, 150, spA4);

            Matrix_ReplaceRotation(&gIdentityMtxF);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPMatrix(POLY_XLU_DISP++, D_01000000_TO_SEGMENTED, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gameplay_keep_DL_06F9F0);
        } else {
            func_800B8050(&this->actor, play, 0);

            // #region 2S2H [Port] Capture framebuffer to current frame, byteswaped, so that it will
            // be read from seg 0x0F in gameplay_keep_DL_06F380
            Gfx* gfx = POLY_OPA_DISP;
            FB_WriteFramebufferSliceToCPU(&gfx, play->state.gfxCtx->curFrameBuffer, true);
            POLY_OPA_DISP = gfx;
            // Only a portion of the buffer is used as a texture, so we need to invalidate by the same offset
            // Offset derived from ((ult * width + uls) * 2), with values taken from the DL directly
            gSPInvalidateTexCache(POLY_OPA_DISP++,
                                  (uintptr_t)(play->state.gfxCtx->curFrameBuffer) + ((104 * 320 + 144) * 2));
            // #endregion

            gSPDisplayList(POLY_OPA_DISP++, gameplay_keep_DL_06F380);
            gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL1, 0, PRIM_LOD_FRAC, TEXEL0, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0,
                              COMBINED, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x7F, 230, 225, 150, 255);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gameplay_keep_DL_06FAE0);
        }

        CLOSE_DISPS(play->state.gfxCtx);

        return;
    } else if (this->actor.speed != 0.0f) {
        u8 sp63 = (Math_CosS(this->unk_260 * 5000) * 127.5f) + 127.5f;
        f32 sp5C;

        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Xlu2(play->state.gfxCtx);

        gSPClearGeometryMode(POLY_XLU_DISP++, G_FOG | G_LIGHTING);

        if (this->actor.params == ARROW_TYPE_SLINGSHOT) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, sp63);
            sp5C = 50.0f;
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 12, 0, 0, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 250, 250, 0, sp63);
            sp5C = 150.0f;
        }

        Matrix_Push();
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

        if (this->actor.speed == 0.0f) {
            phi_v0 = 0;
        } else {
            phi_v0 = (play->gameplayFrames % 256) * 4000;
        }

        Matrix_RotateZS(phi_v0, MTXMODE_APPLY);
        Matrix_Scale(sp5C, sp5C, sp5C, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gEffSparklesDL);

        Matrix_Pop();
        Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_APPLY);

        CLOSE_DISPS(play->state.gfxCtx);
    } else if (this->actor.velocity.y != 0.0f) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gameplay_keep_DL_058BA0);

        CLOSE_DISPS(play->state.gfxCtx);
    }

    func_8088B88C(play, this, &D_8088C2EC[0]);
}
