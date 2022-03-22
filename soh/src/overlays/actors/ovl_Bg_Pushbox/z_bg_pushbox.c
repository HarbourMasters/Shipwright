/*
 * File: z_bg_pushbox.c
 * Overlay: ovl_Bg_Pushbox
 * Description: Unused (and non functional) pushable block
 */

#include "z_bg_pushbox.h"
#include "objects/object_pu_box/object_pu_box.h"

#define FLAGS 0

void BgPushbox_Init(Actor* thisx, GlobalContext* globalCtx);
void BgPushbox_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgPushbox_Update(Actor* thisx, GlobalContext* globalCtx);
void BgPushbox_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgPushbox_UpdateImpl(BgPushbox* this, GlobalContext* globalCtx);

const ActorInit Bg_Pushbox_InitVars = {
    ACTOR_BG_PUSHBOX,
    ACTORCAT_BG,
    FLAGS,
    //! @bug fixing this actor would involve using OBJECT_PU_BOX
    OBJECT_GAMEPLAY_DANGEON_KEEP,
    sizeof(BgPushbox),
    (ActorFunc)BgPushbox_Init,
    (ActorFunc)BgPushbox_Destroy,
    (ActorFunc)BgPushbox_Update,
    (ActorFunc)BgPushbox_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_STOP),
};

void BgPushbox_SetupAction(BgPushbox* this, BgPushboxActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void BgPushbox_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgPushbox* this = (BgPushbox*)thisx;
    CollisionHeader* colHeader = NULL;
    s32 pad2;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gBlockSmallCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);
    ActorShape_Init(&this->dyna.actor.shape, 0.0f, NULL, 0.0f);
    BgPushbox_SetupAction(this, BgPushbox_UpdateImpl);
}

void BgPushbox_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgPushbox* this = (BgPushbox*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
}

void BgPushbox_UpdateImpl(BgPushbox* this, GlobalContext* globalCtx) {
    this->dyna.actor.speedXZ += this->dyna.unk_150 * 0.2f;
    this->dyna.actor.speedXZ = (this->dyna.actor.speedXZ < -1.0f)
                                   ? -1.0f
                                   : ((this->dyna.actor.speedXZ > 1.0f) ? 1.0f : this->dyna.actor.speedXZ);
    Math_StepToF(&this->dyna.actor.speedXZ, 0.0f, 0.2f);
    this->dyna.actor.world.rot.y = this->dyna.unk_158;
    Actor_MoveForward(&this->dyna.actor);
    Actor_UpdateBgCheckInfo(globalCtx, &this->dyna.actor, 20.0f, 40.0f, 40.0f, 0x1D);
}

void BgPushbox_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgPushbox* this = (BgPushbox*)thisx;

    this->actionFunc(this, globalCtx);
    func_8002DF90(&this->dyna);
}

void BgPushbox_Draw(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_pushbox.c", 263);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_pushbox.c", 269),
              G_MTX_NOPUSH | G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, gBlockSmallDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_pushbox.c", 272);
}
