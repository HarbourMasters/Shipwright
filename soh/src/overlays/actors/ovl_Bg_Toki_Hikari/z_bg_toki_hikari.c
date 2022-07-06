/*
 * File: z_bg_toki_hikari.c
 * Overlay: ovl_Toki_Hikari
 * Description: Temple of Time Windows
 */

#include "z_bg_toki_hikari.h"
#include "objects/object_toki_objects/object_toki_objects.h"

#define FLAGS ACTOR_FLAG_5

void BgTokiHikari_Init(Actor* thisx, GlobalContext* globalCtx);
void BgTokiHikari_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgTokiHikari_Update(Actor* thisx, GlobalContext* globalCtx);
void BgTokiHikari_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgTokiHikari_DoNothing(BgTokiHikari* this, GlobalContext* globalCtx);
void func_808BA018(BgTokiHikari* this, GlobalContext* globalCtx);
void func_808BA204(BgTokiHikari* this, GlobalContext* globalCtx);
void func_808BA22C(BgTokiHikari* this, GlobalContext* globalCtx);
void func_808BA274(BgTokiHikari* this, GlobalContext* globalCtx);
void func_808BA2CC(BgTokiHikari* this, GlobalContext* globalCtx);

const ActorInit Bg_Toki_Hikari_InitVars = {
    ACTOR_BG_TOKI_HIKARI,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_TOKI_OBJECTS,
    sizeof(BgTokiHikari),
    (ActorFunc)BgTokiHikari_Init,
    (ActorFunc)BgTokiHikari_Destroy,
    (ActorFunc)BgTokiHikari_Update,
    (ActorFunc)BgTokiHikari_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void BgTokiHikari_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgTokiHikari* this = (BgTokiHikari*)thisx;

    switch (this->actor.params) {
        case 0:
            Actor_ProcessInitChain(&this->actor, sInitChain);
            this->actionFunc = BgTokiHikari_DoNothing;
            break;
        case 1:
            if (!(gSaveContext.eventChkInf[4] & 0x800)) {
                this->actionFunc = func_808BA204;
                this->unk_14C = 0.0f;
            } else {
                Actor_Kill(&this->actor);
            }
            break;
    }
}

void BgTokiHikari_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void BgTokiHikari_DoNothing(BgTokiHikari* this, GlobalContext* globalCtx) {
}

void BgTokiHikari_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgTokiHikari* this = (BgTokiHikari*)thisx;

    this->actionFunc(this, globalCtx);
}

void BgTokiHikari_Draw(Actor* thisx, GlobalContext* globalCtx) {
    BgTokiHikari* this = (BgTokiHikari*)thisx;

    switch (this->actor.params) {
        case 0:
            func_808BA018(this, globalCtx);
            break;
        case 1:
            func_808BA2CC(this, globalCtx);
            break;
    }
}

void func_808BA018(BgTokiHikari* this, GlobalContext* globalCtx) {
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx);
    func_80093D18(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (LINK_IS_ADULT) {
        gSPDisplayList(POLY_OPA_DISP++, object_toki_objects_DL_008190);
    } else {
        gSPDisplayList(POLY_OPA_DISP++, object_toki_objects_DL_007E20);
        func_80093D84(globalCtx->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 8,
                   Gfx_TexScroll(globalCtx->state.gfxCtx, 0, globalCtx->gameplayFrames % 128, 64, 32));

        gSPSegment(POLY_XLU_DISP++, 9,
                   Gfx_TexScroll(globalCtx->state.gfxCtx, 0, globalCtx->gameplayFrames % 128, 64, 32));

        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPDisplayList(POLY_XLU_DISP++, object_toki_objects_DL_007EE0);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void func_808BA204(BgTokiHikari* this, GlobalContext* globalCtx) {
    if (globalCtx->roomCtx.unk_74[1] != 0) {
        this->actionFunc = func_808BA22C;
    }
}

void func_808BA22C(BgTokiHikari* this, GlobalContext* globalCtx) {
    if (this->unk_14C < 1.0f) {
        this->unk_14C += 0.05f;
    } else {
        this->unk_14C = 1.0f;
        this->actionFunc = func_808BA274;
    }
}

void func_808BA274(BgTokiHikari* this, GlobalContext* globalCtx) {
    if (this->unk_14C > 0.2f) {
        this->unk_14C -= 0.025f;
    } else {
        this->unk_14C = 0.0f;
        Actor_Kill(&this->actor);
    }
}

void func_808BA2CC(BgTokiHikari* this, GlobalContext* globalCtx) {
    s32 pad[2];

    OPEN_DISPS(globalCtx->state.gfxCtx);
    Matrix_Translate(0.0f, 276.0f, 1122.0f, MTXMODE_NEW);
    Matrix_Scale(0.32f, 0.32f, this->unk_14C * 7.0f, MTXMODE_APPLY);
    Matrix_RotateZ(M_PI, MTXMODE_APPLY);
    func_80093D18(globalCtx->state.gfxCtx);
    Matrix_Push();
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, this->unk_14C * 255.0f, (u8)(155.0f * this->unk_14C) + 100,
                    this->unk_14C * 255.0f, this->unk_14C * 255.0f);

    gDPSetEnvColor(POLY_XLU_DISP++, (u8)(this->unk_14C * 155.0f) + 100, (u8)(255.0f * this->unk_14C), 0, 128);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, -2 * (globalCtx->gameplayFrames & 0x7F), 0, 0x20, 0x40, 1,
                                (globalCtx->gameplayFrames & 0x7F) * 4, 0, 0x20, 0x40));

    gSPDisplayList(POLY_XLU_DISP++, object_toki_objects_DL_000880);
    Matrix_Pop();
    Matrix_Push();
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (u8)(this->unk_14C * 200.0f));

    gDPSetEnvColor(POLY_XLU_DISP++, (u8)(this->unk_14C * 255.0f), (u8)(this->unk_14C * 255.0f),
                   (u8)(this->unk_14C * 255.0f), (u8)(200.0f * this->unk_14C));

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_XLU_DISP++, object_toki_objects_DL_0009C0);
    Matrix_Pop();
    Matrix_Push();
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (u8)(this->unk_14C * 200.0f));

    gDPSetEnvColor(POLY_XLU_DISP++, (u8)(this->unk_14C * 255.0f), (u8)(this->unk_14C * 255.0f),
                   (u8)(this->unk_14C * 255.0f), (u8)(200.0f * this->unk_14C));

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayListOffset(POLY_XLU_DISP++, object_toki_objects_DL_0009C0, 10);
    Matrix_Pop();
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
