/*
 * File: z_bg_spot15_saku.c
 * Overlay: ovl_Bg_Spot15_Saku
 * Description: Hyrule Castle Gate
 */

#include "z_bg_spot15_saku.h"
#include "objects/object_spot15_obj/object_spot15_obj.h"

#define FLAGS 0

void BgSpot15Saku_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot15Saku_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot15Saku_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot15Saku_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_808B4930(BgSpot15Saku* this, GlobalContext* globalCtx);
void func_808B4978(BgSpot15Saku* this, GlobalContext* globalCtx);
void func_808B4A04(BgSpot15Saku* this, GlobalContext* globalCtx);

const ActorInit Bg_Spot15_Saku_InitVars = {
    ACTOR_BG_SPOT15_SAKU,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_SPOT15_OBJ,
    sizeof(BgSpot15Saku),
    (ActorFunc)BgSpot15Saku_Init,
    (ActorFunc)BgSpot15Saku_Destroy,
    (ActorFunc)BgSpot15Saku_Update,
    (ActorFunc)BgSpot15Saku_Draw,
    NULL,
};

void BgSpot15Saku_Init(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    BgSpot15Saku* this = (BgSpot15Saku*)thisx;
    s32 pad2;
    CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gLonLonCorralFenceCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);
    this->dyna.actor.scale.x = 0.1f;
    this->dyna.actor.scale.y = 0.1f;
    this->dyna.actor.scale.z = 0.1f;
    this->unk_170.x = this->dyna.actor.world.pos.x;
    this->unk_170.y = this->dyna.actor.world.pos.y;
    this->unk_170.z = this->dyna.actor.world.pos.z;
    if (gSaveContext.infTable[7] & 2) {
        this->dyna.actor.world.pos.z = 2659.0f;
    }
    this->actionFunc = func_808B4930;
}

void BgSpot15Saku_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot15Saku* this = (BgSpot15Saku*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
}

void func_808B4930(BgSpot15Saku* this, GlobalContext* globalCtx) {
    if (this->unk_168 && !(gSaveContext.infTable[7] & 2)) {
        this->timer = 2;
        this->actionFunc = func_808B4978;
    }
}

void func_808B4978(BgSpot15Saku* this, GlobalContext* globalCtx) {
    if (this->timer == 0) {
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_METALGATE_OPEN - SFX_FLAG);
        this->dyna.actor.world.pos.z -= 2.0f;
        if (this->dyna.actor.world.pos.z < 2660.0f) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BRIDGE_OPEN_STOP);
            this->timer = 30;
            this->actionFunc = func_808B4A04;
        }
    }
}

void func_808B4A04(BgSpot15Saku* this, GlobalContext* globalCtx) {
    if (this->timer == 0) {
        this->unk_168 = 0;
        this->actionFunc = func_808B4930;
    }
}

void BgSpot15Saku_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot15Saku* this = (BgSpot15Saku*)thisx;

    if (this->timer != 0) {
        this->timer--;
    }

    this->actionFunc(this, globalCtx);
}

void BgSpot15Saku_Draw(Actor* thisx, GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gLonLonCorralFenceDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
