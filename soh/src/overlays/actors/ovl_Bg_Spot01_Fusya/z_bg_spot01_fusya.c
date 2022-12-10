/*
 * File: z_bg_spot01_fusya.c
 * Overlay: Bg_Spot01_Fusya
 * Description: Windmill Sails
 */

#include "z_bg_spot01_fusya.h"
#include "objects/object_spot01_objects/object_spot01_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgSpot01Fusya_Init(Actor* thisx, PlayState* play);
void BgSpot01Fusya_Destroy(Actor* thisx, PlayState* play);
void BgSpot01Fusya_Update(Actor* thisx, PlayState* play);
void BgSpot01Fusya_Draw(Actor* thisx, PlayState* play);

void func_808AAA50(BgSpot01Fusya* this, PlayState* play);

const ActorInit Bg_Spot01_Fusya_InitVars = {
    ACTOR_BG_SPOT01_FUSYA,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT01_OBJECTS,
    sizeof(BgSpot01Fusya),
    (ActorFunc)BgSpot01Fusya_Init,
    (ActorFunc)BgSpot01Fusya_Destroy,
    (ActorFunc)BgSpot01Fusya_Update,
    (ActorFunc)BgSpot01Fusya_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 12800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 1300, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1300, ICHAIN_STOP),
};

void BgSpot01Fusya_SetupAction(BgSpot01Fusya* this, BgSpot01FusyaActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void BgSpot01Fusya_Init(Actor* thisx, PlayState* play) {
    BgSpot01Fusya* this = (BgSpot01Fusya*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->unk_154 = 100.0f;
    this->unk_158 = 100.0f;
    this->unk_15C = 0.5f;
    if (gSaveContext.sceneSetupIndex < 4) {
        gSaveContext.eventChkInf[6] &= 0xFFDF;
    }
    BgSpot01Fusya_SetupAction(this, func_808AAA50);
}

void BgSpot01Fusya_Destroy(Actor* thisx, PlayState* play) {
}

void func_808AAA50(BgSpot01Fusya* this, PlayState* play) {
    f32 temp;
    Actor* thisx = &this->actor;

    if (CVar_GetS32("gCosmetics.Kak_Windmill_Speed.Changed", 0)) {
        this->unk_158 = CVar_GetFloat("gCosmetics.Kak_Windmill_Speed.Value", 100.0f);
    } else {
        this->unk_158 = 100.0f;
    }

    if (gSaveContext.eventChkInf[6] & 0x20) {
        this->unk_158 = 1800.0f;
    }
    thisx->shape.rot.z += this->unk_154;
    temp = ((this->unk_154 - 100.0f) / 1700.0f) + 1.0f;
    func_800F436C(&thisx->projectedPos, 0x2085, temp);
    Math_ApproachF(&this->unk_154, this->unk_158, this->unk_15C, 100.0f);
}

void BgSpot01Fusya_Update(Actor* thisx, PlayState* play) {
    BgSpot01Fusya* this = (BgSpot01Fusya*)thisx;

    this->actionFunc(this, play);
}

void BgSpot01Fusya_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gKakarikoWindmillSailsDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
