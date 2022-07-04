/*
 * File: z_bg_spot17_funen
 * Overlay: ovl_Bg_Spot17_Funen
 * Description: Crater Smoke Cone
 */

#include "z_bg_spot17_funen.h"
#include "objects/object_spot17_obj/object_spot17_obj.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgSpot17Funen_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot17Funen_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot17Funen_Update(Actor* thisx, GlobalContext* globalCtx);
void func_808B746C(Actor* thisx, GlobalContext* globalCtx);
void func_808B7478(Actor* thisx, GlobalContext* globalCtx);

const ActorInit Bg_Spot17_Funen_InitVars = {
    ACTOR_BG_SPOT17_FUNEN,
    ACTORCAT_SWITCH,
    FLAGS,
    OBJECT_SPOT17_OBJ,
    sizeof(BgSpot17Funen),
    (ActorFunc)BgSpot17Funen_Init,
    (ActorFunc)BgSpot17Funen_Destroy,
    (ActorFunc)BgSpot17Funen_Update,
    NULL,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgSpot17Funen_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot17Funen* this = (BgSpot17Funen*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    osSyncPrintf("spot17 obj. 噴煙 (arg_data 0x%04x)\n", this->actor.params);
}

void BgSpot17Funen_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void BgSpot17Funen_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot17Funen* this = (BgSpot17Funen*)thisx;

    this->actor.draw = func_808B7478;
    this->actor.update = func_808B746C;
}

void func_808B746C(Actor* thisx, GlobalContext* globalCtx) {
}

void func_808B7478(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D84(globalCtx->state.gfxCtx);
    Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) - thisx->shape.rot.y + 0x8000) *
                       9.58738019108e-05f,
                   MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, (0 - globalCtx->gameplayFrames) & 0x7F, 0x20, 0x20, 1, 0,
                                (0 - globalCtx->gameplayFrames) & 0x7F, 0x20, 0x20));
    gSPDisplayList(POLY_XLU_DISP++, gCraterSmokeConeDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
