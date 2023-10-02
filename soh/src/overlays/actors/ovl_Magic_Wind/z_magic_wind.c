/*
 * File: z_magic_wind.c
 * Overlay: ovl_Magic_Wind
 * Description: Farore's Wind
 */

#include "z_magic_wind.h"

#define FLAGS (ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_NO_FREEZE_OCARINA)

void MagicWind_Init(Actor* thisx, PlayState* play);
void MagicWind_Destroy(Actor* thisx, PlayState* play);
void MagicWind_Update(Actor* thisx, PlayState* play);
void MagicWind_Draw(Actor* thisx, PlayState* play);

void MagicWind_Shrink(MagicWind* this, PlayState* play);
void MagicWind_WaitForTimer(MagicWind* this, PlayState* play);
void MagicWind_FadeOut(MagicWind* this, PlayState* play);
void MagicWind_WaitAtFullSize(MagicWind* this, PlayState* play);
void MagicWind_Grow(MagicWind* this, PlayState* play);

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

void MagicWind_Init(Actor* thisx, PlayState* play) {
    MagicWind* this = (MagicWind*)thisx;
    Player* player = GET_PLAYER(play);

    if (SkelCurve_Init(play, &this->skelCurve, &sSkel, &sAnim) == 0) {
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
            Player_PlaySfx(&player->actor, NA_SE_PL_MAGIC_WIND_WARP);
            break;
    }
}

void MagicWind_Destroy(Actor* thisx, PlayState* play) {
    MagicWind* this = (MagicWind*)thisx;
    SkelCurve_Destroy(play, &this->skelCurve);
    func_800876C8(play);
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

void MagicWind_WaitForTimer(MagicWind* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->timer > 0) {
        this->timer--;
        return;
    }

    // "Means start"
    LOG_STRING("表示開始");
    Player_PlaySfx(&player->actor, NA_SE_PL_MAGIC_WIND_NORMAL);
    MagicWind_UpdateAlpha(1.0f);
    MagicWind_SetupAction(this, MagicWind_Grow);
    SkelCurve_Update(play, &this->skelCurve);
}

void MagicWind_Grow(MagicWind* this, PlayState* play) {
    if (SkelCurve_Update(play, &this->skelCurve)) {
        MagicWind_SetupAction(this, MagicWind_WaitAtFullSize);
        this->timer = 50;
    }
}

void MagicWind_WaitAtFullSize(MagicWind* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    } else {
        MagicWind_SetupAction(this, MagicWind_FadeOut);
        this->timer = 30;
    }
}

void MagicWind_FadeOut(MagicWind* this, PlayState* play) {
    if (this->timer > 0) {
        MagicWind_UpdateAlpha((f32)this->timer * (1.0f / 30.0f));
        this->timer--;
    } else {
        Actor_Kill(&this->actor);
    }
}

void MagicWind_Shrink(MagicWind* this, PlayState* play) {
    if (SkelCurve_Update(play, &this->skelCurve)) {
        Actor_Kill(&this->actor);
    }
}

void MagicWind_Update(Actor* thisx, PlayState* play) {
    MagicWind* this = (MagicWind*)thisx;
    if (play->msgCtx.msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK ||
        play->msgCtx.msgMode == MSGMODE_SONG_PLAYED) {
        Actor_Kill(thisx);
        return;
    }

    this->actionFunc(this, play);
}

s32 MagicWind_OverrideLimbDraw(PlayState* play, SkelAnimeCurve* skelCurve, s32 limbIndex, void* thisx) {
    MagicWind* this = (MagicWind*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (limbIndex == 1) {
        gSPSegment(POLY_XLU_DISP++, 8,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, (play->state.frames * 9) & 0xFF,
                                    0xFF - ((play->state.frames * 0xF) & 0xFF), 0x40, 0x40, 1,
                                    (play->state.frames * 0xF) & 0xFF,
                                    0xFF - ((play->state.frames * 0x1E) & 0xFF), 0x40, 0x40));

    } else if (limbIndex == 2) {
        gSPSegment(POLY_XLU_DISP++, 9,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, (play->state.frames * 3) & 0xFF,
                                    0xFF - ((play->state.frames * 5) & 0xFF), 0x40, 0x40, 1,
                                    (play->state.frames * 6) & 0xFF,
                                    0xFF - ((play->state.frames * 0xA) & 0xFF), 0x40, 0x40));
    }

    CLOSE_DISPS(play->state.gfxCtx);

    return true;
}

void MagicWind_Draw(Actor* thisx, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MagicWind* this = (MagicWind*)thisx;

    OPEN_DISPS(gfxCtx);

    if (this->actionFunc != MagicWind_WaitForTimer) {
        POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, 25);
        SkelCurve_Draw(thisx, play, &this->skelCurve, MagicWind_OverrideLimbDraw, NULL, 1, NULL);
    }

    CLOSE_DISPS(gfxCtx);
}
