/*
 * File: z_obj_armos.c
 * Overlay: ovl_Obj_Armos
 * Description: Non-hostile Armos statues
 */

#include "z_obj_armos.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_CAN_PRESS_SWITCHES)

void ObjArmos_Init(Actor* thisx, PlayState* play);
void ObjArmos_Destroy(Actor* thisx, PlayState* play);
void ObjArmos_Update(Actor* thisx, PlayState* play2);
void ObjArmos_Draw(Actor* thisx, PlayState* play);

void func_809A54B4(ObjArmos* this);
void func_809A54E0(ObjArmos* this, PlayState* play);
void func_809A5610(ObjArmos* this);
void func_809A562C(ObjArmos* this, PlayState* play);
void func_809A57D8(ObjArmos* this);
void func_809A57F4(ObjArmos* this, PlayState* play);

ActorProfile Obj_Armos_Profile = {
    /**/ ACTOR_OBJ_ARMOS,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_AM,
    /**/ sizeof(ObjArmos),
    /**/ ObjArmos_Init,
    /**/ ObjArmos_Destroy,
    /**/ ObjArmos_Update,
    /**/ ObjArmos_Draw,
};

s16 D_809A5BB0[] = { 1, -1, 0, 0 };
s16 D_809A5BB8[] = { 0, 0, 1, -1 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 120, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 250, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -4000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

bool func_809A4E00(ObjArmos* this, PlayState* play, s16 arg2) {
    return !DynaPolyActor_ValidateMove(play, &this->dyna, 30, arg2, 1) ||
           !DynaPolyActor_ValidateMove(play, &this->dyna, 30, arg2, 28);
}

s32 func_809A4E68(ObjArmos* this) {
    s16 temp_v0;

    if (fabsf(this->dyna.pushForce) > 0.1f) {
        temp_v0 = BINANG_ADD(this->dyna.yRotation, 0x2000);
        if (this->dyna.pushForce < 0.0f) {
            temp_v0 = BINANG_ROT180(temp_v0);
        }

        if (temp_v0 < -0x4000) {
            return 3;
        }

        if (temp_v0 < 0) {
            return 1;
        }

        if (temp_v0 < 0x4000) {
            return 2;
        }

        return 0;
    }
    return -1;
}

bool func_809A4F00(ObjArmos* this, s32 arg1) {
    s32 temp_v0 = OBJARMOS_GET_ROTZ_7(&this->dyna.actor);

    if (temp_v0 == OBJARMOS_ROT_7_0) {
        return arg1 == 0;
    }

    if (temp_v0 == OBJARMOS_ROT_7_1) {
        return arg1 == 1;
    }

    if (temp_v0 == OBJARMOS_ROT_7_2) {
        return arg1 == 2;
    }

    if (temp_v0 == OBJARMOS_ROT_7_3) {
        return arg1 == 3;
    }

    if (temp_v0 == OBJARMOS_ROT_7_4) {
        return (arg1 == 0) || (arg1 == 1);
    }

    if (temp_v0 == OBJARMOS_ROT_7_5) {
        return (arg1 == 2) || (arg1 == 3);
    }

    if (temp_v0 == OBJARMOS_ROT_7_6) {
        if (this->unk_26E != 0) {
            return (arg1 == 0) || (arg1 == 1);
        }

        if (this->unk_270 != 0) {
            return (arg1 == 2) || (arg1 == 3);
        }

        return true;
    }

    return false;
}

bool func_809A500C(ObjArmos* this, s32 arg1) {
    s32 temp_v0 = OBJARMOS_GET_ROTZ_7(&this->dyna.actor);
    s32 temp_v1 = OBJARMOS_GET_ROTX_F(&this->dyna.actor);
    s32 temp;
    s32 temp2;

    if (temp_v0 == OBJARMOS_ROT_7_0) {
        return this->unk_26E < temp_v1;
    }

    if (temp_v0 == OBJARMOS_ROT_7_1) {
        return -temp_v1 < this->unk_26E;
    }

    if (temp_v0 == OBJARMOS_ROT_7_2) {
        return this->unk_270 < temp_v1;
    }

    if (temp_v0 == OBJARMOS_ROT_7_3) {
        return -temp_v1 < this->unk_270;
    }

    if (temp_v0 == OBJARMOS_ROT_7_4) {
        temp = D_809A5BB0[arg1] + this->unk_26E;
        return (temp <= temp_v1) && (-temp_v1 <= temp);
    }

    if (temp_v0 == OBJARMOS_ROT_7_5) {
        temp2 = D_809A5BB8[arg1] + this->unk_270;
        return (temp2 <= temp_v1) && (-temp_v1 <= temp2);
    }

    if (temp_v0 == OBJARMOS_ROT_7_6) {
        if ((arg1 == 0) || (arg1 == 1)) {
            temp = D_809A5BB0[arg1] + this->unk_26E;
            return (temp <= temp_v1) && (-temp_v1 <= temp);
        }

        temp2 = D_809A5BB8[arg1] + this->unk_270;
        return (temp2 <= temp_v1) && (-temp_v1 <= temp2);
    }

    return false;
}

void func_809A518C(ObjArmos* this, s32 arg1) {
    this->unk_26E += D_809A5BB0[arg1];
    this->unk_270 += D_809A5BB8[arg1];

    if ((arg1 == 0) || (arg1 == 1)) {
        this->unk_25C = &this->dyna.actor.world.pos.x;
        this->unk_260 = this->dyna.actor.home.pos.x + (this->unk_26E * 60);
    } else {
        this->unk_25C = &this->dyna.actor.world.pos.z;
        this->unk_260 = this->dyna.actor.home.pos.z + (this->unk_270 * 60);
    }
    this->unk_264 = arg1;
}

void ObjArmos_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjArmos* this = (ObjArmos*)thisx;
    s32 sp44 = OBJARMOS_GET_ROTZ_7(&this->dyna.actor);
    s32 sp40 = OBJARMOS_GET_ROTX_F(&this->dyna.actor);
    f32 endFrame = Animation_GetLastFrame(&gArmosPushedBackAnim);

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);

    this->dyna.actor.home.rot.y = this->dyna.actor.world.rot.x = this->dyna.actor.world.rot.y =
        this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.x = this->dyna.actor.shape.rot.z = 0;

    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_am_Colheader_005CF8);

    SkelAnime_Init(play, &this->skelAnime, &object_am_Skel_005948, &gArmosPushedBackAnim, this->jointTable,
                   this->morphTable, OBJECT_AM_LIMB_MAX);

    Animation_Change(&this->skelAnime, &gArmosPushedBackAnim, 0.0f, endFrame, endFrame, ANIMMODE_ONCE, 0.0f);

    if (sp40 == 0) {
        func_809A57D8(this);
    } else if (Flags_GetSwitch(play, OBJARMOS_GET_SWITCH_FLAG(&this->dyna.actor))) {
        if (sp44 == OBJARMOS_ROT_7_0) {
            this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x + (sp40 * 60);
            func_809A57D8(this);
        } else if (sp44 == OBJARMOS_ROT_7_1) {
            this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x - (sp40 * 60);
            func_809A57D8(this);
        } else if (sp44 == OBJARMOS_ROT_7_2) {
            this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z + (sp40 * 60);
            func_809A57D8(this);
        } else if (sp44 == OBJARMOS_ROT_7_3) {
            this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z - (sp40 * 60);
            func_809A57D8(this);
        } else {
            func_809A54B4(this);
        }
    } else {
        func_809A54B4(this);
    }
}

void ObjArmos_Destroy(Actor* thisx, PlayState* play) {
    ObjArmos* this = (ObjArmos*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_809A54B4(ObjArmos* this) {
    this->actionFunc = func_809A54E0;
    this->updBgCheckInfoFlags = UPDBGCHECKINFO_FLAG_4;
    this->unk_266[1] = 0;
    this->unk_266[2] = 0;
    this->unk_266[3] = 0;
    this->unk_266[0] = 0;
}

void func_809A54E0(ObjArmos* this, PlayState* play) {
    s32 i;
    s32 sp20 = func_809A4E68(this);

    for (i = 0; i < ARRAY_COUNT(this->unk_266); i++) {
        if (sp20 == i) {
            this->unk_266[i]++;
        } else if (this->unk_266[i] > 0) {
            this->unk_266[i]--;
        }
    }

    if (sp20 != -1) {
        if ((this->unk_266[sp20] >= 9) && func_809A4F00(this, sp20) && func_809A500C(this, sp20) &&
            !func_809A4E00(this, play, (this->dyna.pushForce > 0.0f) ? 90 : 120)) {
            func_809A518C(this, sp20);
            func_809A5610(this);
        } else {
            GET_PLAYER(play)->stateFlags2 &= ~PLAYER_STATE2_10;
            this->dyna.pushForce = 0.0f;
        }
    }
}

void func_809A5610(ObjArmos* this) {
    this->actionFunc = func_809A562C;
    this->updBgCheckInfoFlags = UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4;
}

void func_809A562C(ObjArmos* this, PlayState* play) {
    s32 pad[2];
    Player* player;
    f32 temp2 = this->unk_260 - *this->unk_25C;
    s32 temp;
    s32 sp20;

    if (Math_StepToF(this->unk_25C, this->unk_260, (Math_SinS(fabsf(temp2) * 546.13336f) * 1.6f) + 1.0f)) {
        player = GET_PLAYER(play);
        temp = OBJARMOS_GET_ROTZ_7(&this->dyna.actor);
        sp20 = false;

        if ((temp == OBJARMOS_ROT_7_4) || (temp == OBJARMOS_ROT_7_5) || (temp == OBJARMOS_ROT_7_6)) {
            if (!func_809A500C(this, this->unk_264) || func_809A4E00(this, play, 0x5A)) {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
            }
        } else if (func_809A500C(this, this->unk_264)) {
            if (func_809A4E00(this, play, 0x5A)) {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
            }
        } else {
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
            Flags_SetSwitch(play, OBJARMOS_GET_SWITCH_FLAG(&this->dyna.actor));
            sp20 = true;
        }

        player->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;

        if (!sp20) {
            func_809A54B4(this);
        } else {
            func_809A57D8(this);
        }
    } else {
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
    }
}

void func_809A57D8(ObjArmos* this) {
    this->actionFunc = func_809A57F4;
    this->updBgCheckInfoFlags = UPDBGCHECKINFO_FLAG_4;
}

void func_809A57F4(ObjArmos* this, PlayState* play) {
    if (fabsf(this->dyna.pushForce) > 0.1f) {
        GET_PLAYER(play)->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;
    }
}

void ObjArmos_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjArmos* this = (ObjArmos*)thisx;
    s32 bgId;

    this->actionFunc(this, play);
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;

    if (this->updBgCheckInfoFlags != 0) {
        Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 20.0f, 30.0f, 0.0f, this->updBgCheckInfoFlags);

        if ((this->actionFunc == func_809A54E0) && (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
            (DynaPoly_GetActor(&play->colCtx, this->dyna.actor.floorBgId) == NULL)) {
            this->updBgCheckInfoFlags = 0;
        }

        this->unk_250.x = (Math_SinS(this->dyna.actor.shape.rot.y) * -9.0f) + this->dyna.actor.world.pos.x;
        this->unk_250.y = this->dyna.actor.world.pos.y + 20.0f;
        this->unk_250.z = (Math_CosS(this->dyna.actor.shape.rot.y) * -9.0f) + this->dyna.actor.world.pos.z;

        this->dyna.actor.floorHeight = BgCheck_EntityRaycastFloor5(&play->colCtx, &this->dyna.actor.floorPoly, &bgId,
                                                                   &this->dyna.actor, &this->unk_250);
    }
}

void func_809A5960(ObjArmos* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);
    Vec3f sp28;

    sp28.x = Math_SinS(this->dyna.actor.shape.rot.y);
    Matrix_SetTranslateRotateYXZ((sp28.x * -9.0f) + this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 (Math_CosS(this->dyna.actor.shape.rot.y) * -9.0f) + this->dyna.actor.world.pos.z,
                                 &this->dyna.actor.shape.rot);
    Matrix_Scale(0.014f, 0.014f, 0.014f, MTXMODE_APPLY);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, &this->dyna.actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_809A5A3C(ObjArmos* this, PlayState* play) {
    s32 pad[2];
    MtxF sp48;

    if (this->dyna.actor.floorPoly != NULL) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL44_Xlu(play->state.gfxCtx);

        gDPSetCombineLERP(POLY_XLU_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED, 0, 0, 0,
                          COMBINED);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);

        func_800C0094(this->dyna.actor.floorPoly, this->unk_250.x, this->dyna.actor.floorHeight, this->unk_250.z,
                      &sp48);
        Matrix_Put(&sp48);
        Matrix_Scale(0.6f, 1.0f, 0.6f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gCircleShadowDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void ObjArmos_Draw(Actor* thisx, PlayState* play) {
    ObjArmos* this = (ObjArmos*)thisx;

    func_809A5960(this, play);
    func_809A5A3C(this, play);
}
