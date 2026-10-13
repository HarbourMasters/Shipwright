/*
 * File: z_bg_iknv_doukutu.c
 * Overlay: ovl_Bg_Iknv_Doukutu
 * Description: Sharp's Cave
 */

#include "z_bg_iknv_doukutu.h"
#include "objects/object_iknv_obj/object_iknv_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void BgIknvDoukutu_Init(Actor* thisx, PlayState* play);
void BgIknvDoukutu_Destroy(Actor* thisx, PlayState* play);
void BgIknvDoukutu_Update(Actor* thisx, PlayState* play);
void BgIknvDoukutu_Draw(Actor* thisx, PlayState* play);

void func_80BD716C(BgIknvDoukutu* this, PlayState* play);
void func_80BD71BC(BgIknvDoukutu* this, PlayState* play);
void func_80BD7250(BgIknvDoukutu* this, PlayState* play);
void func_80BD72BC(BgIknvDoukutu* this, PlayState* play);
void func_80BD7360(BgIknvDoukutu* this, PlayState* play);
void func_80BD73D0(BgIknvDoukutu* this, PlayState* play);
void func_80BD7768(Actor* thisx, PlayState* play);
void func_80BD7820(Actor* thisx, PlayState* play);
void func_80BD78C4(Actor* thisx, PlayState* play);
void func_80BD7538(Actor* thisx, PlayState* play);

ActorProfile Bg_Iknv_Doukutu_Profile = {
    /**/ ACTOR_BG_IKNV_DOUKUTU,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_IKNV_OBJ,
    /**/ sizeof(BgIknvDoukutu),
    /**/ BgIknvDoukutu_Init,
    /**/ BgIknvDoukutu_Destroy,
    /**/ BgIknvDoukutu_Update,
    /**/ BgIknvDoukutu_Draw,
};

void BgIknvDoukutu_Init(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;
    CollisionHeader* colHeader = NULL;
    s32 pad;

    Actor_SetScale(&this->dyna.actor, 0.1f);
    this->actionFunc = func_80BD73D0;
    this->unk_15C = 0;

    switch (BGIKNVDOUKUTU_GET_F(&this->dyna.actor)) {
        case BGIKNVDOUKUTU_F_0:
            this->actionFunc = func_80BD71BC;
            this->cueType = CS_CMD_ACTOR_CUE_516;
            this->unk_160 = 1.0f;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04) || CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE)) {
                this->dyna.actor.draw = func_80BD7768;
                this->actionFunc = func_80BD73D0;
                play->envCtx.lightSettingOverride = 25;
            } else {
                play->envCtx.lightSettingOverride = 24;
            }
            break;

        case BGIKNVDOUKUTU_F_1:
            Actor_SetScale(&this->dyna.actor, 1.0f);
            this->dyna.actor.draw = func_80BD7820;
            this->cueType = CS_CMD_ACTOR_CUE_516;
            DynaPolyActor_Init(&this->dyna, 0);
            CollisionHeader_GetVirtual(&object_iknv_obj_Colheader_012788, &colHeader);
            this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04)) {
                Actor_Kill(&this->dyna.actor);
            }
            break;

        case BGIKNVDOUKUTU_F_2:
            this->cueType = CS_CMD_ACTOR_CUE_516;
            this->dyna.actor.draw = func_80BD78C4;
            DynaPolyActor_Init(&this->dyna, 0);
            CollisionHeader_GetVirtual(&object_iknv_obj_Colheader_0117C8, &colHeader);
            this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_14_04)) {
                this->unk_160 = 1.0f;
                this->dyna.actor.world.pos.y += 68.0f;
            } else {
                this->actionFunc = func_80BD7360;
                this->unk_160 = 0.0f;
            }
            break;

        default:
            Actor_Kill(&this->dyna.actor);
            break;
    }
}

void BgIknvDoukutu_Destroy(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;

    if ((BGIKNVDOUKUTU_GET_F(&this->dyna.actor) == BGIKNVDOUKUTU_F_1) ||
        (BGIKNVDOUKUTU_GET_F(&this->dyna.actor) == BGIKNVDOUKUTU_F_2)) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void func_80BD716C(BgIknvDoukutu* this, PlayState* play) {
    if (this->unk_160 >= 0.05f) {
        this->unk_160 -= 0.05f;
    } else {
        this->actionFunc = func_80BD73D0;
        this->dyna.actor.draw = func_80BD7768;
    }
    play->envCtx.lightSettingOverride = 25;
}

void func_80BD71BC(BgIknvDoukutu* this, PlayState* play) {
    play->envCtx.lightSettingOverride = 24;
    if (Cutscene_IsCueInChannel(play, this->cueType) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id == 2)) {
        this->actionFunc = func_80BD716C;
        this->dyna.actor.draw = func_80BD7538;
    }
}

void func_80BD7250(BgIknvDoukutu* this, PlayState* play) {
    f32 temp_fv0 = this->dyna.actor.home.pos.y + 68.0f;

    this->dyna.actor.world.pos.y += 1.7f;
    if (temp_fv0 < this->dyna.actor.world.pos.y) {
        this->dyna.actor.world.pos.y = temp_fv0;
        this->actionFunc = func_80BD73D0;
    }
    Audio_PlaySfx_2(NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
}

void func_80BD72BC(BgIknvDoukutu* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, this->cueType) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id == 3)) {
        this->actionFunc = func_80BD7250;
    }

    if (this->unk_160 < 0.95f) {
        this->unk_160 += 0.05f;
    }
}

void func_80BD7360(BgIknvDoukutu* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, this->cueType) &&
        (play->csCtx.actorCues[Cutscene_GetCueChannel(play, this->cueType)]->id == 2)) {
        this->actionFunc = func_80BD72BC;
    }
}

void func_80BD73D0(BgIknvDoukutu* this, PlayState* play) {
}

void BgIknvDoukutu_Update(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;

    this->actionFunc(this, play);
}

void BgIknvDoukutu_Draw(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(object_iknv_obj_Matanimheader_00F1C0));
    Scene_SetRenderModeXlu(play, 0, 1);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPDisplayList(POLY_OPA_DISP++, object_iknv_obj_DL_00DDD8);
    gDPSetEnvColor(POLY_XLU_DISP++, 215, 42, 55, 120);
    gSPDisplayList(POLY_XLU_DISP++, object_iknv_obj_DL_00DB60);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80BD7538(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;
    GraphicsContext* gfxCtx;
    f32 sp54;

    func_80BD7768(&this->dyna.actor, play);

    gfxCtx = play->state.gfxCtx;
    OPEN_DISPS(gfxCtx);
    Vec3f sp40;
    s16 sp3E = Math_Vec3f_Yaw(&GET_ACTIVE_CAM(play)->eye, &GET_ACTIVE_CAM(play)->at);
    s16 sp3C = -Math_Vec3f_Pitch(&GET_ACTIVE_CAM(play)->eye, &GET_ACTIVE_CAM(play)->at);

    sp40.x = -100.0f * Math_SinS(sp3E) * Math_CosS(sp3C);
    sp40.y = Math_SinS(sp3C) * -100.0f;
    sp40.z = -100.0f * Math_CosS(sp3E) * Math_CosS(sp3C);
    Matrix_Translate(sp40.x, sp40.y, sp40.z, MTXMODE_APPLY);
    sp54 = this->unk_160;
    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(object_iknv_obj_Matanimheader_00F1C0));

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    Gfx_SetupDL72(POLY_XLU_DISP++);
    Scene_SetRenderModeXlu(play, 1, 2);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, (s32)(255.0f * sp54));
    gSPDisplayList(POLY_XLU_DISP++, object_iknv_obj_DL_00DDD8);
    gDPSetEnvColor(POLY_XLU_DISP++, 215, 42, 55, (s32)(120.0f * sp54));
    gSPDisplayList(POLY_XLU_DISP++, object_iknv_obj_DL_00DB60);

    CLOSE_DISPS(gfxCtx);
}

void func_80BD7768(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Scene_SetRenderModeXlu(play, 0, 1);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPDisplayList(POLY_OPA_DISP++, object_iknv_obj_DL_010D98);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80BD7820(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(object_iknv_obj_Matanimheader_012728));

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, object_iknv_obj_DL_012700);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80BD78C4(Actor* thisx, PlayState* play) {
    BgIknvDoukutu* this = (BgIknvDoukutu*)thisx;
    f32 sp30 = this->unk_160;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(object_iknv_obj_Matanimheader_0117A0));

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, (s32)(140.0f * sp30));

    gSPDisplayList(POLY_XLU_DISP++, object_iknv_obj_DL_0115E0);

    CLOSE_DISPS(play->state.gfxCtx);
}
