/*
 * File: z_magic_wind.c
 * Overlay: ovl_Magic_Wind
 * Description: Farore's Wind
 */

#include "z_magic_wind.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void MagicWind_Init(Actor* thisx, GlobalContext* globalCtx);
void MagicWind_Destroy(Actor* thisx, GlobalContext* globalCtx);
void MagicWind_Update(Actor* thisx, GlobalContext* globalCtx);
void MagicWind_Draw(Actor* thisx, GlobalContext* globalCtx);

void MagicWind_Shrink(MagicWind* this, GlobalContext* globalCtx);
void MagicWind_WaitForTimer(MagicWind* this, GlobalContext* globalCtx);
void MagicWind_FadeOut(MagicWind* this, GlobalContext* globalCtx);
void MagicWind_WaitAtFullSize(MagicWind* this, GlobalContext* globalCtx);
void MagicWind_Grow(MagicWind* this, GlobalContext* globalCtx);

const ActorInit Magic_Wind_InitVars = {
    ACTOR_MAGIC_WIND,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(MagicWind),
    (ActorFunc)MagicWind_Init,
    (ActorFunc)MagicWind_Destroy,
    (ActorFunc)MagicWind_Update,
    (ActorFunc)MagicWind_Draw,
    NULL,
};

#include "overlays/ovl_Magic_Wind/ovl_Magic_Wind.h"

static u8 sAlphaUpdVals[] = {
    0x00, 0x03, 0x04, 0x07, 0x09, 0x0A, 0x0D, 0x0F, 0x11, 0x12, 0x15, 0x16, 0x19, 0x1B, 0x1C, 0x1F, 0x21, 0x23,
};

void MagicWind_SetupAction(MagicWind* this, MagicWindFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void MagicWind_Init(Actor* thisx, GlobalContext* globalCtx) {
    MagicWind* this = (MagicWind*)thisx;
    Player* player = GET_PLAYER(globalCtx);

    if (SkelCurve_Init(globalCtx, &this->skelCurve, &sSkel, &sAnim) == 0) {
        // "Magic_Wind_Actor_ct (): Construct failed"
        osSyncPrintf("Magic_Wind_Actor_ct():コンストラクト失敗\n");
    }
    this->actor.room = -1;
    switch (this->actor.params) {
        case 0:
            SkelCurve_SetAnim(&this->skelCurve, &sAnim, 0.0f, 60.0f, 0.0f, 1.0f);
            this->timer = 29;
            MagicWind_SetupAction(this, MagicWind_WaitForTimer);
            break;
        case 1:
            SkelCurve_SetAnim(&this->skelCurve, &sAnim, 60.0f, 0.0f, 60.0f, -1.0f);
            MagicWind_SetupAction(this, MagicWind_Shrink);
            // "Means start"
            LOG_STRING("表示開始");
            func_8002F7DC(&player->actor, NA_SE_PL_MAGIC_WIND_WARP);
            break;
    }
}

void MagicWind_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    MagicWind* this = (MagicWind*)thisx;
    SkelCurve_Destroy(globalCtx, &this->skelCurve);
    func_800876C8(globalCtx);
    // "wipe out"
    LOG_STRING("消滅");
}

void MagicWind_UpdateAlpha(f32 alpha) {
    s32 i;

    Vtx* vtx = ResourceMgr_LoadVtxByName(sCylinderVtx);

    for (i = 0; i < ARRAY_COUNT(sAlphaUpdVals); i++) {
        vtx[sAlphaUpdVals[i]].n.a = alpha * 255.0f;
    }
}

void MagicWind_WaitForTimer(MagicWind* this, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);

    if (this->timer > 0) {
        this->timer--;
        return;
    }

    // "Means start"
    LOG_STRING("表示開始");
    func_8002F7DC(&player->actor, NA_SE_PL_MAGIC_WIND_NORMAL);
    MagicWind_UpdateAlpha(1.0f);
    MagicWind_SetupAction(this, MagicWind_Grow);
    SkelCurve_Update(globalCtx, &this->skelCurve);
}

void MagicWind_Grow(MagicWind* this, GlobalContext* globalCtx) {
    if (SkelCurve_Update(globalCtx, &this->skelCurve)) {
        MagicWind_SetupAction(this, MagicWind_WaitAtFullSize);
        this->timer = 50;
    }
}

void MagicWind_WaitAtFullSize(MagicWind* this, GlobalContext* globalCtx) {
    if (this->timer > 0) {
        this->timer--;
    } else {
        MagicWind_SetupAction(this, MagicWind_FadeOut);
        this->timer = 30;
    }
}

void MagicWind_FadeOut(MagicWind* this, GlobalContext* globalCtx) {
    if (this->timer > 0) {
        MagicWind_UpdateAlpha((f32)this->timer * (1.0f / 30.0f));
        this->timer--;
    } else {
        Actor_Kill(&this->actor);
    }
}

void MagicWind_Shrink(MagicWind* this, GlobalContext* globalCtx) {
    if (SkelCurve_Update(globalCtx, &this->skelCurve)) {
        Actor_Kill(&this->actor);
    }
}

void MagicWind_Update(Actor* thisx, GlobalContext* globalCtx) {
    MagicWind* this = (MagicWind*)thisx;
    if (globalCtx->msgCtx.msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK ||
        globalCtx->msgCtx.msgMode == MSGMODE_SONG_PLAYED) {
        Actor_Kill(thisx);
        return;
    }

    this->actionFunc(this, globalCtx);
}

s32 MagicWind_OverrideLimbDraw(GlobalContext* globalCtx, SkelAnimeCurve* skelCurve, s32 limbIndex, void* thisx) {
    MagicWind* this = (MagicWind*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    if (limbIndex == 1) {
        gSPSegment(POLY_XLU_DISP++, 8,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (globalCtx->state.frames * 9) & 0xFF,
                                    0xFF - ((globalCtx->state.frames * 0xF) & 0xFF), 0x40, 0x40, 1,
                                    (globalCtx->state.frames * 0xF) & 0xFF,
                                    0xFF - ((globalCtx->state.frames * 0x1E) & 0xFF), 0x40, 0x40));

    } else if (limbIndex == 2) {
        gSPSegment(POLY_XLU_DISP++, 9,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, (globalCtx->state.frames * 3) & 0xFF,
                                    0xFF - ((globalCtx->state.frames * 5) & 0xFF), 0x40, 0x40, 1,
                                    (globalCtx->state.frames * 6) & 0xFF,
                                    0xFF - ((globalCtx->state.frames * 0xA) & 0xFF), 0x40, 0x40));
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx);

    return true;
}

void MagicWind_Draw(Actor* thisx, GlobalContext* globalCtx) {
    GraphicsContext* gfxCtx = globalCtx->state.gfxCtx;
    MagicWind* this = (MagicWind*)thisx;

    OPEN_DISPS(gfxCtx);

    if (this->actionFunc != MagicWind_WaitForTimer) {
        POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 25);
        SkelCurve_Draw(thisx, globalCtx, &this->skelCurve, MagicWind_OverrideLimbDraw, NULL, 1, NULL);
    }

    CLOSE_DISPS(gfxCtx);
}
