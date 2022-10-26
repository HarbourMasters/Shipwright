/*
 * File: z_bg_spot01_idosoko.c
 * Overlay: ovl_Bg_Spot01_Idosoko
 * Description: Stone that blocks the entrance to Bottom of the Well
 */

#include "z_bg_spot01_idosoko.h"
#include "objects/object_spot01_matoya/object_spot01_matoya.h"

#define FLAGS ACTOR_FLAG_4

void BgSpot01Idosoko_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot01Idosoko_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot01Idosoko_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot01Idosoko_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_808ABF54(BgSpot01Idosoko* this, GlobalContext* globalCtx);

const ActorInit Bg_Spot01_Idosoko_InitVars = {
    ACTOR_BG_SPOT01_IDOSOKO,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT01_MATOYA,
    sizeof(BgSpot01Idosoko),
    (ActorFunc)BgSpot01Idosoko_Init,
    (ActorFunc)BgSpot01Idosoko_Destroy,
    (ActorFunc)BgSpot01Idosoko_Update,
    (ActorFunc)BgSpot01Idosoko_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgSpot01Idosoko_SetupAction(BgSpot01Idosoko* this, BgSpot01IdosokoActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void BgSpot01Idosoko_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgSpot01Idosoko* this = (BgSpot01Idosoko*)thisx;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    DynaPolyActor_Init(&this->dyna, DPM_PLAYER);
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    CollisionHeader_GetVirtual(&gKakarikoBOTWStoneCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);
    // If dungeon entrance randomizer is on, remove the well stone as adult Link when
    // child Link has drained the water to the well
    if (!LINK_IS_ADULT || gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_DUNGEONS_ENTRANCES) &&
        gSaveContext.eventChkInf[6] &0x80) {
        Actor_Kill(&this->dyna.actor);
    } else {
        BgSpot01Idosoko_SetupAction(this, func_808ABF54);
    }
}

void BgSpot01Idosoko_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot01Idosoko* this = (BgSpot01Idosoko*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
}

void func_808ABF54(BgSpot01Idosoko* this, GlobalContext* globalCtx) {
}

void BgSpot01Idosoko_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot01Idosoko* this = (BgSpot01Idosoko*)thisx;

    this->actionFunc(this, globalCtx);
}

void BgSpot01Idosoko_Draw(Actor* thisx, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gKakarikoBOTWStoneDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
