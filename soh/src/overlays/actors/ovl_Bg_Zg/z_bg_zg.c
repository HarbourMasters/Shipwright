/*
 * File: z_bg_zg.c
 * Overlay: ovl_Bg_Zg
 * Description: Metal bars (Ganon's Castle)
 */

#include "z_bg_zg.h"
#include "objects/object_zg/object_zg.h"
#include "vt.h"

#define FLAGS ACTOR_FLAG_4

void BgZg_Init(Actor* thisx, PlayState* play);
void BgZg_Destroy(Actor* thisx, PlayState* play);
void BgZg_Update(Actor* thisx, PlayState* play);
void BgZg_Draw(Actor* thisx, PlayState* play);

void func_808C0C50(BgZg* this);
s32 func_808C0C98(BgZg* this, PlayState* play);
s32 func_808C0CC8(BgZg* this);
void func_808C0CD4(BgZg* this, PlayState* play);
void func_808C0D08(BgZg* this, PlayState* play);
void func_808C0EEC(BgZg* this, PlayState* play);

static BgZgActionFunc sActionFuncs[] = {
    func_808C0CD4,
    func_808C0D08,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

static BgZgDrawFunc sDrawFuncs[] = {
    func_808C0EEC,
};

const ActorInit Bg_Zg_InitVars = {
    ACTOR_BG_ZG,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_ZG,
    sizeof(BgZg),
    (ActorFunc)BgZg_Init,
    (ActorFunc)BgZg_Destroy,
    (ActorFunc)BgZg_Update,
    (ActorFunc)BgZg_Draw,
    NULL,
};

void BgZg_Destroy(Actor* thisx, PlayState* play) {
    BgZg* this = (BgZg*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_808C0C50(BgZg* this) {
    Audio_PlaySoundGeneral(NA_SE_EV_METALDOOR_OPEN, &this->dyna.actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                           &D_801333E8);
}

s32 func_808C0C98(BgZg* this, PlayState* play) {
    s32 flag = (this->dyna.actor.params >> 8) & 0xFF;

    return Flags_GetSwitch(play, flag);
}

s32 func_808C0CC8(BgZg* this) {
    s32 flag = this->dyna.actor.params & 0xFF;

    return flag;
}

void func_808C0CD4(BgZg* this, PlayState* play) {
    if (func_808C0C98(this, play) != 0) {
        this->action = 1;
        func_808C0C50(this);
    }
}

void func_808C0D08(BgZg* this, PlayState* play) {
    this->dyna.actor.world.pos.y += (kREG(16) + 20.0f) * 1.2f;
    if ((((kREG(17) + 200.0f) * 1.2f) + this->dyna.actor.home.pos.y) <= this->dyna.actor.world.pos.y) {
        Actor_Kill(&this->dyna.actor);
    }
}

void BgZg_Update(Actor* thisx, PlayState* play) {
    BgZg* this = (BgZg*)thisx;
    s32 action = this->action;

    if (((action < 0) || (1 < action)) || (sActionFuncs[action] == NULL)) {
        // "Main Mode is wrong!!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        sActionFuncs[action](this, play);
    }
}

void BgZg_Init(Actor* thisx, PlayState* play) {
    s32 pad[2];
    BgZg* this = (BgZg*)thisx;
    CollisionHeader* colHeader;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    colHeader = NULL;
    CollisionHeader_GetVirtual(&gTowerCollapseBarsCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    if ((func_808C0CC8(this) == 8) || (func_808C0CC8(this) == 9)) {
        this->dyna.actor.scale.x = this->dyna.actor.scale.x * 1.3f;
        this->dyna.actor.scale.z = this->dyna.actor.scale.z * 1.3f;
        this->dyna.actor.scale.y = this->dyna.actor.scale.y * 1.2f;
    }

    this->action = 0;
    this->drawConfig = 0;
    if (func_808C0C98(this, play)) {
        Actor_Kill(&this->dyna.actor);
    }
}

void func_808C0EEC(BgZg* this, PlayState* play) {
    GraphicsContext* localGfxCtx = play->state.gfxCtx;

    OPEN_DISPS(localGfxCtx);

    Gfx_SetupDL_25Opa(localGfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(localGfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseBarsDL);

    CLOSE_DISPS(localGfxCtx);
}

void BgZg_Draw(Actor* thisx, PlayState* play) {
    BgZg* this = (BgZg*)thisx;
    s32 drawConfig = this->drawConfig;

    if (((drawConfig < 0) || (drawConfig > 0)) || sDrawFuncs[drawConfig] == NULL) {
        // "Drawing mode is wrong !!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        sDrawFuncs[drawConfig](this, play);
    }
}
