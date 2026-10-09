/*
 * File: z_dm_hina.c
 * Overlay: ovl_Dm_Hina
 * Description: Boss mask cutscene objects
 */

#include "z_dm_hina.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DmHina_Init(Actor* thisx, PlayState* play);
void DmHina_Destroy(Actor* thisx, PlayState* play);
void DmHina_Update(Actor* thisx, PlayState* play);
void DmHina_Draw(Actor* thisx, PlayState* play);

void func_80A1F470(DmHina* this, PlayState* play);
void func_80A1F56C(DmHina* this, PlayState* play);
void func_80A1F5AC(DmHina* this, PlayState* play);
void func_80A1F63C(DmHina* this, PlayState* play);

ActorProfile Dm_Hina_Profile = {
    /**/ ACTOR_DM_HINA,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_BSMASK,
    /**/ sizeof(DmHina),
    /**/ DmHina_Init,
    /**/ DmHina_Destroy,
    /**/ DmHina_Update,
    /**/ DmHina_Draw,
};

void DmHina_Init(Actor* thisx, PlayState* play) {
    DmHina* this = (DmHina*)thisx;

    this->isDrawn = true;
    this->actionFunc = func_80A1F470;
    this->unk158 = this->actor.world.pos.y;
    this->unk148 = 0.0f;
    this->unk15C = 1.0f;
    this->actor.focus.pos.x = this->actor.world.pos.x;
    this->actor.focus.pos.y = this->actor.world.pos.y;
    this->actor.focus.pos.z = this->actor.world.pos.z;
}

void DmHina_Destroy(Actor* thisx, PlayState* play) {
}

void func_80A1F470(DmHina* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_SmoothStepToF(&this->unk148, 0.6f, 0.5f, 0.05f, 0.001f);
    this->unk154 = Math_SinS(play->gameplayFrames * 0x708) * 8.0f;
    if ((player->stateFlags1 & PLAYER_STATE1_400) && (this->actor.xzDistToPlayer < 80.0f)) {
        this->isDrawn = false;
        this->unk154 = 0.0f;
        this->actor.world.pos.y += 40.0f;
        this->actionFunc = func_80A1F56C;
    }
}

void func_80A1F56C(DmHina* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->unk17C = 2;
        this->actionFunc = func_80A1F5AC;
    }
}

void func_80A1F5AC(DmHina* this, PlayState* play) {
    this->unk17C--;
    if (this->unk17C == 0) {
        this->isDrawn = true;
        Cutscene_StartManual(play, &play->csCtx);
        this->subCamId = Play_CreateSubCamera(play);
        Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
        Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
        this->actionFunc = func_80A1F63C;
    }
}

void func_80A1F63C(DmHina* this, PlayState* play) {
    this->subCamEye.x = this->actor.world.pos.x + 100.0f;
    this->subCamEye.y = this->unk158 + 260.0f;
    this->subCamEye.z = this->actor.world.pos.z + 100.0f;
    this->subCamAt.x = this->actor.world.pos.x;
    this->subCamAt.y = this->actor.world.pos.y + this->unk154 * this->unk15C + 40.0f * this->unk15C;
    this->subCamAt.z = this->actor.world.pos.z;
    Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    Math_SmoothStepToF(&this->actor.world.pos.y, this->unk158 + 300.0f, 0.5f, 2.0f, 0.1f);
    if (((this->unk158 + 240.0f) < this->actor.world.pos.y) && (this->unk17E != 1)) {
        this->unk17E = 1;
        Actor_PlaySfx(&this->actor, NA_SE_OC_WHITE_OUT_INTO_KYOJIN);
    }
    Actor_PlaySfx(&this->actor, NA_SE_EV_MASK_RISING - SFX_FLAG);
}

void func_80A1F75C(DmHina* this, PlayState* play) {
    s32 i;

    switch (this->unk17E) {
        case 0:
            this->unk17F = 0;
            this->unk14C = 0.0f;
            break;

        case 1:
            Math_SmoothStepToF(&this->unk14C, 1.0f, 0.4f, 0.05f, 0.001f);
            this->unk17F = this->unk14C * 255.0f;
            this->unk150 = Math_SinS(play->state.frames * 0x1F40);
            for (i = 0; i < ARRAY_COUNT(play->envCtx.adjLightSettings.light1Color); i++) {
                play->envCtx.adjLightSettings.ambientColor[i] = play->envCtx.adjLightSettings.fogColor[i] =
                    play->envCtx.adjLightSettings.light1Color[i] = -255.0f * this->unk14C;
            }

            play->envCtx.adjLightSettings.fogNear = -500.0f * this->unk14C;
            if (play->envCtx.adjLightSettings.fogNear < -300) {
                play->roomCtx.curRoom.segment = NULL;
            }
            break;

        default:
            break;
    }
}

void DmHina_Update(Actor* thisx, PlayState* play) {
    DmHina* this = (DmHina*)thisx;

    this->actionFunc(this, play);
    func_80A1F75C(this, play);
}

void func_80A1F9AC(DmHina* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    if (this->unk17E != 0) {
        OPEN_DISPS(gfxCtx);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, (u8)(this->unk14C * 100.0f) + 155, this->unk17F);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, (u8)(this->unk150 * 100.0f) + 50, 0);
        gSPDisplayList(POLY_XLU_DISP++, gLightOrbMaterial1DL);

        Matrix_Translate(this->actor.world.pos.x,
                         this->actor.world.pos.y + (this->unk154 * this->unk15C) + (40.0f * this->unk15C),
                         this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        Matrix_Scale(this->unk14C * 20.0f, this->unk14C * 20.0f, this->unk14C * 20.0f, MTXMODE_APPLY);
        Matrix_RotateZF(Rand_ZeroFloat(2 * M_PIf), MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gLightOrbModelDL);

        CLOSE_DISPS(gfxCtx);
    }
}

void DmHina_Draw(Actor* thisx, PlayState* play) {
    DmHina* this = (DmHina*)thisx;
    f32 scale;

    if (this->isDrawn) {
        Matrix_Translate(this->actor.world.pos.x,
                         this->actor.world.pos.y + (this->unk154 * this->unk15C) + (40.0f * this->unk15C),
                         this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_RotateZYX(0, play->gameplayFrames * 0x3E8, 0, MTXMODE_APPLY);
        scale = this->unk148 * (1.0f - this->unk14C) * this->unk15C;
        Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
        if (GameInteractor_Should(VB_DRAW_BOSS_REMAINS, true, this)) {
            switch (this->actor.params) {
                case 0:
                    GetItem_Draw(play, GID_REMAINS_ODOLWA);
                    break;

                case 1:
                    GetItem_Draw(play, GID_REMAINS_GOHT);
                    break;

                case 2:
                    GetItem_Draw(play, GID_REMAINS_GYORG);
                    break;

                case 3:
                    GetItem_Draw(play, GID_REMAINS_TWINMOLD);
                    break;

                default:
                    break;
            }
        }
        func_80A1F9AC(this, play);
    }
}
