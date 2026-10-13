/*
 * File: z_obj_comb.c
 * Overlay: ovl_Obj_Comb
 * Description: Beehive
 */

#include "z_obj_comb.h"
#include "objects/object_comb/object_comb.h"
#include "GameInteractor/GameInteractor.h"

#define FLAGS 0x00000000

void ObjComb_Init(Actor* thisx, PlayState* play);
void ObjComb_Destroy(Actor* thisx, PlayState* play2);
void ObjComb_Update(Actor* thisx, PlayState* play);
void ObjComb_Draw(Actor* thisx, PlayState* play);

void func_8098DC44(ObjComb* this);
void func_8098DC60(ObjComb* this, PlayState* play);
void func_8098DEA0(ObjComb* this, PlayState* play);
void func_8098E098(ObjComb* this);
void func_8098E0B8(ObjComb* this, PlayState* play);

ActorProfile Obj_Comb_Profile = {
    /**/ ACTOR_OBJ_COMB,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_COMB,
    /**/ sizeof(ObjComb),
    /**/ ObjComb_Init,
    /**/ ObjComb_Destroy,
    /**/ ObjComb_Update,
    /**/ ObjComb_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x05CBFFBE, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 15 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_STOP),
};

bool func_8098CE40(ObjComb* this, PlayState* play) {
    s32 phi_a2 = -1;
    s32 temp_v0 = (OBJCOMB_GET_1F(&this->actor) << 2) | 0xFF01;

    if ((u8)temp_v0 & 3) {
        phi_a2 = ((temp_v0 & 0x3FC) >> 2) & 0xFF;
    }

    return ((phi_a2 < 0) == 1) || !Flags_GetTreasure(play, phi_a2);
}

void func_8098CEAC(ObjComb* this, PlayState* play) {
    Vec3f spDC;
    Vec3f spD0;
    Vec3f spC4;
    s32 i;
    f32 temp_f20;
    s16 temp_s2 = 0;
    s16 temp_f10;
    s32 phi_s0;
    s32 phi_v0;
    s32 phi_s1;

    for (i = 0; i < 31; i++) {
        temp_s2 += 0x4E20;
        temp_f20 = Rand_ZeroOne() * 10.0f;

        spDC.x = Math_SinS(temp_s2) * temp_f20;
        spDC.y = (i - 15) * 0.7f;
        spDC.z = Math_CosS(temp_s2) * temp_f20;

        Math_Vec3f_Sum(&spDC, &this->actor.world.pos, &spD0);

        spC4.x = (Rand_ZeroOne() - 0.5f) + (spDC.x * 0.5f);
        spC4.y = (Rand_ZeroOne() - 0.5f) + (spDC.y * 0.6f);
        spC4.z = (Rand_ZeroOne() - 0.5f) + (spDC.z * 0.5f);

        temp_f10 = (Rand_ZeroOne() * 72.0f) + 25.0f;

        if (temp_f10 < 40) {
            phi_s0 = -200;
            phi_s1 = 40;
        } else if (temp_f10 < 70) {
            phi_s0 = -280;
            phi_s1 = 30;
        } else {
            phi_s0 = -340;
            phi_s1 = 20;
        }

        temp_f20 = Rand_ZeroOne();
        if (temp_f20 < 0.1f) {
            phi_v0 = 0x60;
        } else if (temp_f20 < 0.8f) {
            phi_v0 = 0x40;
        } else {
            phi_v0 = 0x20;
        }
        EffectSsKakera_Spawn(play, &spD0, &spC4, &spD0, phi_s0, phi_v0, phi_s1, 4, 0, temp_f10, 0, 0, 60, -1,
                             OBJECT_COMB, gBeehiveFragmentDL);
    }

    spD0.x = this->actor.world.pos.x;
    spD0.y = this->actor.world.pos.y - 10.0f;
    spD0.z = this->actor.world.pos.z;

    func_800BBFB0(play, &spD0, 40.0f, 6, 70, 60, 1);
}

void func_8098D19C(ObjComb* this, PlayState* play) {
    Vec3f spDC;
    Vec3f spD0;
    Vec3f spC4;
    f32 temp_f20;
    s16 temp_s2 = 0;
    s16 temp_f8;
    s32 phi_s0;
    s32 phi_v0;
    s32 phi_s1;
    s32 i;

    for (i = 0; i < 31; i++) {
        temp_s2 += 0x4E20;
        temp_f20 = Rand_ZeroOne() * 10.0f;

        spDC.x = Math_SinS(temp_s2) * temp_f20;
        spDC.y = i * 0.7f;
        spDC.z = Math_CosS(temp_s2) * temp_f20;

        Math_Vec3f_Sum(&spDC, &this->actor.world.pos, &spD0);

        spC4.x = (Rand_ZeroOne() - 0.5f) + (spDC.x * 0.5f);
        spC4.y = (Rand_ZeroOne() * 0.5f) + (spDC.y * 0.5f);
        spC4.z = (Rand_ZeroOne() - 0.5f) + (spDC.z * 0.5f);

        temp_f8 = (Rand_ZeroOne() * 72.0f) + 25.0f;
        if (temp_f8 < 40) {
            phi_s0 = -200;
            phi_s1 = 40;
        } else if (temp_f8 < 70) {
            phi_s0 = -280;
            phi_s1 = 30;
        } else {
            phi_s0 = -340;
            phi_s1 = 20;
        }

        temp_f20 = Rand_ZeroOne();
        if (temp_f20 < 0.1f) {
            phi_v0 = 0x60;
        } else if (temp_f20 < 0.8f) {
            phi_v0 = 0x40;
        } else {
            phi_v0 = 0x20;
        }

        EffectSsKakera_Spawn(play, &spD0, &spC4, &spD0, phi_s0, phi_v0, phi_s1, 4, 0, temp_f8, 0, 0, 60, -1,
                             OBJECT_COMB, gBeehiveFragmentDL);
    }

    spD0.x = this->actor.world.pos.x;
    spD0.y = this->actor.world.pos.y + 20.0f;
    spD0.z = this->actor.world.pos.z;

    func_800BBFB0(play, &spD0, 40.0f, 6, 70, 60, 1);
}

void func_8098D47C(ObjComb* this, PlayState* play) {
    Vec3f spD4;
    Vec3f spC8;
    Vec3f spBC;
    f32 temp_f20;
    s16 temp_s1 = 0;
    s16 temp_f10;
    s32 phi_s0;
    s32 phi_v0;
    s32 i;

    for (i = 0; i < 21; i++) {
        temp_s1 += 0x4E20;
        temp_f20 = Rand_ZeroOne() * 10.0f;

        spD4.x = Math_SinS(temp_s1) * temp_f20;
        spD4.y = i * 0.25f;
        spD4.z = Math_CosS(temp_s1) * temp_f20;

        Math_Vec3f_Sum(&spD4, &this->actor.world.pos, &spC8);

        spBC.x = (Rand_ZeroOne() - 0.5f) + (spD4.x * 0.5f);
        spBC.y = Rand_ZeroOne() + (spD4.y * 0.6f) + 6.0f;
        spBC.z = (Rand_ZeroOne() - 0.5f) + (spD4.z * 0.5f);

        temp_f10 = (Rand_ZeroOne() * 72.0f) + 25.0f;
        if (temp_f10 < 40) {
            phi_s0 = -100;
        } else if (temp_f10 < 70) {
            phi_s0 = -180;
        } else {
            phi_s0 = -240;
        }

        if (Rand_ZeroOne() < 0.2f) {
            phi_v0 = 0x40;
        } else {
            phi_v0 = 0x20;
        }

        EffectSsKakera_Spawn(play, &spC8, &spBC, &spC8, phi_s0, phi_v0, 40, 0, 0, temp_f10, 0, 0, 80, -1, OBJECT_COMB,
                             gBeehiveFragmentDL);
    }
}

void func_8098D6E0(ObjComb* this, PlayState* play) {
    s32 i;
    Vec3f sp70;
    f32 temp_f0;
    f32 temp_f20;
    s32 temp;

    sp70.y = this->actor.world.pos.y + this->actor.depthInWater;

    for (temp = 0, i = 0; i < 8; i++, temp += 0x2000) {
        temp_f0 = Rand_ZeroOne();
        temp_f20 = ((1.0f - SQ(temp_f0)) * 14.0f) + 4.0f;
        sp70.x = this->actor.world.pos.x + (Math_SinS((s32)(Rand_ZeroOne() * 8000.0f) + temp) * temp_f20);
        sp70.z = this->actor.world.pos.z + (Math_CosS((s32)(Rand_ZeroOne() * 8000.0f) + temp) * temp_f20);
        EffectSsGSplash_Spawn(play, &sp70, NULL, NULL, 0, 200);
    }

    sp70.x = this->actor.world.pos.x;
    sp70.z = this->actor.world.pos.z;
    EffectSsGSplash_Spawn(play, &sp70, NULL, NULL, 0, 300);
}

void func_8098D870(ObjComb* this, PlayState* play) {
    s32 temp_v0 = func_800A8150(OBJCOMB_GET_3F(&this->actor));

    if (temp_v0 >= 0) {
        Item_DropCollectible(play, &this->actor.world.pos, ((OBJCOMB_GET_7F00(&this->actor)) << 8) | temp_v0);
    }
}

void func_8098D8C8(ObjComb* this, PlayState* play) {
    s32 params = OBJCOMB_GET_F(&this->actor);
    s32 i;
    s32 phi_s5;

    if (this->actionFunc == func_8098DC60) {
        phi_s5 = 2;
    } else {
        phi_s5 = 1;
    }
    this->unk_1B6 = 0;

    for (i = 0; i < params; i++) {
        if (Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BEE, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, 0, 0, phi_s5)) {
            this->unk_1B6 = 1;
        }
    }
}

void func_8098D99C(ObjComb* this, PlayState* play) {
    s32 params;
    Actor* temp_v0;

    if (func_8098CE40(this, play)) {
        params = (OBJCOMB_GET_1F(&this->actor) << 2) | 0xFF01;
        temp_v0 = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_SW, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 0, this->actor.home.rot.y, 0, params);

        if (temp_v0 != NULL) {
            temp_v0->parent = &this->actor;
            if (this->actionFunc == func_8098DC60) {
                temp_v0->velocity.y = 8.0f;
                temp_v0->speed = 2.0f;
            } else {
                temp_v0->velocity.y = 10.0f;
                temp_v0->speed = 2.0f;
            }
            this->unk_1B6 = 1;
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
        }
    }
}

void func_8098DA74(ObjComb* this, PlayState* play) {
    s32 temp_v0 = OBJCOMB_GET_8000(&this->actor) | OBJCOMB_GET_80(&this->actor);

    if (GameInteractor_Should(VB_BEEHIVE_SPAWN_ACTOR, true, this)) {
        if (temp_v0 == 0) {
            func_8098D870(this, play);
        } else if (temp_v0 == 1) {
            func_8098D8C8(this, play);
        } else {
            func_8098D99C(this, play);
        }
    }
}

void ObjComb_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjComb* this = (ObjComb*)thisx;
    s32 sp2C = OBJCOMB_GET_8000(&this->actor) | OBJCOMB_GET_80(&this->actor);

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitJntSph(play, &this->collider);

    if (GameInteractor_Should(VB_BEEHIVE_ALREADY_BROKEN,
                              (sp2C == 1) && OBJCOMB_GET_10(&this->actor) && CHECK_WEEKEVENTREG(WEEKEVENTREG_83_02),
                              this)) {
        Actor_Kill(&this->actor);
        return;
    }

    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);

    if ((sp2C == 0) && Item_CanDropBigFairy(play, OBJCOMB_GET_3F(&this->actor), OBJCOMB_GET_7F00(&this->actor))) {
        this->unk_1B7 = 1;
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    }

    if ((sp2C != 2) || !func_8098CE40(this, play)) {
        this->unk_1B8 = -1;
    }
    func_8098DC44(this);
}

void ObjComb_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjComb* this = (ObjComb*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void func_8098DC44(ObjComb* this) {
    this->unk_1AC = 0x2EE0;
    this->actionFunc = func_8098DC60;
}

void func_8098DC60(ObjComb* this, PlayState* play) {
    s32 temp_a0;
    s8 temp_v0;
    u32 temp_v1;

    this->unk_1AA += this->unk_1AC;
    if (this->unk_1B3) {
        this->collider.base.acFlags &= ~AC_HIT;
    }

    this->unk_1A8 -= 0x32;
    if (this->unk_1A8 < 0) {
        this->unk_1A8 = 0;
    }

    if (this->unk_1B3) {
        if (this->collider.elements[0].base.acHitElem->atDmgInfo.dmgFlags & 0x0182C29C) {
            func_8098CEAC(this, play);
            func_8098DA74(this, play);
            Actor_Kill(&this->actor);
        } else {
            s32 dmgFlags = this->collider.elements[0].base.acHitElem->atDmgInfo.dmgFlags;

            if (dmgFlags & 0x13820) {
                this->unk_1A8 = 0xDAC;
                this->unk_1AC = 0x36B0;
            } else {
                this->unk_1A8 = 0x5DC;
                this->unk_1AC = 0x2EE0;
            }

            if ((this->unk_1B2 <= 0) && (dmgFlags & 0x13820)) {
                if (this->unk_1B5 == 0) {
                    this->unk_1B5 = 1;
                    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
                }
                this->unk_1B2 = 20;
            }
        }
    } else {
        if (this->unk_1B8 >= 0) {
            if (this->unk_1B8 == 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALGOLD_ROLL);
                if (Rand_ZeroOne() < 0.1f) {
                    this->unk_1B8 = Rand_S16Offset(40, 80);
                } else {
                    this->unk_1B8 = 8;
                }
            } else {
                this->unk_1B8--;
            }
        }
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->actor.update != NULL) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }

    this->actor.shape.rot.x = (s32)(Math_SinS(this->unk_1AA) * this->unk_1A8) + this->actor.home.rot.x;
}

void func_8098DE58(ObjComb* this) {
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->unk_1B4 = 100;
    this->actor.terminalVelocity = -20.0f;
    this->actor.gravity = -1.5f;
    this->actor.speed = 0.0f;
    this->actionFunc = func_8098DEA0;
}

void func_8098DEA0(ObjComb* this, PlayState* play) {
    this->unk_1B4--;
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->unk_1B4 <= 0)) {
        func_8098DA74(this, play);
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER) && (this->actor.depthInWater > 30.0f)) {
            func_8098D47C(this, play);
        } else {
            func_8098D19C(this, play);
        }
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EV_HONEYCOMB_BROKEN);
        func_8098E098(this);
    } else {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) {
            func_8098D6E0(this, play);
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
            this->actor.gravity = -0.5f;
            this->actor.velocity.y *= 0.8f;
            this->unk_1AE >>= 1;
            this->unk_1B0 >>= 1;
        } else {
            this->actor.gravity = -1.5f;
            this->actor.velocity.y *= 0.96f;

            this->unk_1AE += 0x1F4;
            if (this->unk_1AE > 0x7D0) {
                this->unk_1AE = 0x7D0;
            }

            this->unk_1B0 += 0xC8;
            if (this->unk_1B0 > 0x258) {
                this->unk_1B0 = 0x258;
            }
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_HONEYCOMB_FALL - SFX_FLAG);
        }

        Actor_MoveWithGravity(&this->actor);
        this->actor.shape.rot.x += this->unk_1AE;
        this->actor.shape.rot.y += this->unk_1B0;
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void func_8098E098(ObjComb* this) {
    this->actor.draw = NULL;
    this->unk_1B4 = 20;
    this->actionFunc = func_8098E0B8;
}

void func_8098E0B8(ObjComb* this, PlayState* play) {
    this->unk_1B4--;
    if (this->unk_1B4 <= 0) {
        Actor_Kill(&this->actor);
        return;
    }

    if ((this->unk_1B4 == 10) && (this->unk_1B6 != 0) && (this->unk_1B5 == 2) && (this->actor.csId > CS_ID_NONE)) {
        if (CutsceneManager_GetCurrentCsId() == this->actor.csId) {
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_4);
        }
    }
}

void ObjComb_Update(Actor* thisx, PlayState* play) {
    ObjComb* this = (ObjComb*)thisx;

    this->unk_1B3 = (this->collider.base.acFlags & AC_HIT) != 0;
    if (this->unk_1B3) {
        this->collider.base.acFlags &= ~AC_HIT;
    }

    if (this->unk_1B2 > 0) {
        this->unk_1B2--;
        if (this->unk_1B2 == 0) {
            func_8098DE58(this);
        }
    }

    this->actionFunc(this, play);

    if (this->actor.update == NULL) {
        if ((this->unk_1B5 == 2) && (CutsceneManager_GetCutsceneScriptIndex(this->actor.csId) == -1)) {
            CutsceneManager_Stop(this->actor.csId);
            this->unk_1B5 = 0;
        }
    } else {
        if (this->unk_1B5 != 0) {
            Actor_SetFocus(&this->actor, 0.0f);
            if (this->unk_1B5 == 1) {
                if (CutsceneManager_IsNext(this->actor.csId)) {
                    CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
                    if (this->actor.csId > CS_ID_NONE) {
                        Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_1);
                    }

                    if (((OBJCOMB_GET_8000(&this->actor) | OBJCOMB_GET_80(&this->actor)) == 1) &&
                        OBJCOMB_GET_10(&this->actor)) {
                        SET_WEEKEVENTREG(WEEKEVENTREG_83_02);
                    }

                    this->unk_1B5 = 2;
                } else {
                    CutsceneManager_Queue(this->actor.csId);
                }
            }
        }

        if (this->unk_1B7 != 0) {
            play->actorCtx.flags |= ACTORCTX_FLAG_3;
            this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        }
    }
}

void ObjComb_Draw(Actor* thisx, PlayState* play) {
    ObjComb* this = (ObjComb*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y + (118.0f * this->actor.scale.y),
                     this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_RotateXS(this->actor.shape.rot.x, MTXMODE_APPLY);
    Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_Translate(0.0f, -(this->actor.scale.y * 118.0f), 0.0f, MTXMODE_APPLY);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gBeehiveDL);

    Collider_UpdateSpheres(0, &this->collider);

    CLOSE_DISPS(play->state.gfxCtx);
}
