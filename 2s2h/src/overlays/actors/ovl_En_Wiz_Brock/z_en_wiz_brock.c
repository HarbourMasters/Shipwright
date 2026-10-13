/*
 * File: z_en_wiz_brock.c
 * Overlay: ovl_En_Wiz_Brock
 * Description: Wizrobe Warp Platform
 */

#include "z_en_wiz_brock.h"
#include "objects/object_wiz/object_wiz.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_LOCK_ON_DISABLED)

void EnWizBrock_Init(Actor* thisx, PlayState* play);
void EnWizBrock_Destroy(Actor* thisx, PlayState* play);
void EnWizBrock_Update(Actor* thisx, PlayState* play);
void EnWizBrock_Draw(Actor* thisx, PlayState* play);

void EnWizBrock_SetupUpdateStatus(EnWizBrock* this, PlayState* play);
void EnWizBrock_UpdateStatus(EnWizBrock* this, PlayState* play);

s16 sPlatformIndex = 0;

ActorProfile En_Wiz_Brock_Profile = {
    /**/ ACTOR_EN_WIZ_BROCK,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_WIZ,
    /**/ sizeof(EnWizBrock),
    /**/ EnWizBrock_Init,
    /**/ EnWizBrock_Destroy,
    /**/ EnWizBrock_Update,
    /**/ EnWizBrock_Draw,
};

void EnWizBrock_Init(Actor* thisx, PlayState* play) {
    EnWizBrock* this = (EnWizBrock*)thisx;
    CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&this->dyna, 0);
    CollisionHeader_GetVirtual(&gWizrobePlatformCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->dyna.actor.colChkInfo.health = 3;
    this->unk_1A6 = 0;
    Actor_SetScale(&this->dyna.actor, 0.01f);
    this->platformIndex = sPlatformIndex++;
    this->actionFunc = EnWizBrock_SetupUpdateStatus;
    this->dyna.actor.scale.x = this->dyna.actor.scale.y = this->dyna.actor.scale.z = 0.01f;
    this->alpha = 255.0f;
}

void EnWizBrock_Destroy(Actor* thisx, PlayState* play) {
    EnWizBrock* this = (EnWizBrock*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void EnWizBrock_SetupUpdateStatus(EnWizBrock* this, PlayState* play) {
    this->actionFunc = EnWizBrock_UpdateStatus;
}

/**
 * @brief Checks the platform status, when the Wizrobe is defeated, which triggers timer to
 *  count up to 30 at which point the platforms are despawned.
 */
void EnWizBrock_UpdateStatus(EnWizBrock* this, PlayState* play) {
    if (this->platformType == EN_WIZ_BROCK_PLATFORM_TYPE_INACTIVE) {
        if (this->dyna.actor.colChkInfo.health != EN_WIZ_BROCK_PLATFORM_TYPE_MAX) {
            this->platformType = this->dyna.actor.colChkInfo.health;
        }
    }

    if (this->dyna.actor.colChkInfo.health == 0) {
        this->timer++;
        if (this->timer > (BREG(49) + 30)) {
            Math_ApproachZeroF(&this->dyna.actor.scale.y, (BREG(50) / 10.0f) + 0.3f, (BREG(51) / 10000.0f) + 0.003f);
            Math_ApproachZeroF(&this->alpha, (BREG(52) / 10.0f) + 1.0f, (BREG(53) / 10.0f) + 35.0f);
            Math_ApproachF(&this->dyna.actor.scale.x, (BREG(54) / 100.0f) + 0.02f, (BREG(55) / 100.0f) + 0.2f,
                           (BREG(56) / 1000.0f) + 0.002f);
            this->dyna.actor.scale.z = this->dyna.actor.scale.x;
            if (this->dyna.actor.scale.y < 0.001f) {
                Actor_Kill(&this->dyna.actor);
            }
        }
    }
}

void EnWizBrock_Update(Actor* thisx, PlayState* play) {
    EnWizBrock* this = (EnWizBrock*)thisx;

    this->actionFunc(this, play);
}

void EnWizBrock_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnWizBrock* this = (EnWizBrock*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    OPEN_DISPS(play->state.gfxCtx);

    Matrix_Translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_Scale(this->dyna.actor.scale.x, this->dyna.actor.scale.y, this->dyna.actor.scale.z, MTXMODE_APPLY);
    if (this->dyna.actor.colChkInfo.health != 0) {
        Scene_SetRenderModeXlu(play, 0, 1);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
        Gfx_DrawDListOpa(play, gWizrobePlatformDL);

    } else {
        Scene_SetRenderModeXlu(play, 1, 2);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, TRUNCF_BINANG(this->alpha));
        Gfx_DrawDListXlu(play, gWizrobePlatformDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);

    if (this->platformType != EN_WIZ_BROCK_PLATFORM_TYPE_INACTIVE) {
        OPEN_DISPS(play->state.gfxCtx);

        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gWizrobePlatformCenterTexAnim));
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, 255);
        if (this->platformType == EN_WIZ_BROCK_PLATFORM_TYPE_FIRE) {
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 00, 100, TRUNCF_BINANG(this->alpha));
        } else {
            gDPSetEnvColor(POLY_XLU_DISP++, 50, 00, 255, TRUNCF_BINANG(this->alpha));
        }

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, &gWizrobePlatformCenterDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
