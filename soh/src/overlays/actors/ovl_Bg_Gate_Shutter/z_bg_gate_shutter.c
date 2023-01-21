/*
 * File: z_bg_gate_shutter.c
 * Overlay: Bg_Gate_Shutter
 * Description: Death Mountain Trail Gate
 */

#include "z_bg_gate_shutter.h"
#include "objects/object_spot01_matoyab/object_spot01_matoyab.h"
#include "vt.h"

#define FLAGS 0

void BgGateShutter_Init(Actor* thisx, PlayState* play);
void BgGateShutter_Destroy(Actor* thisx, PlayState* play);
void BgGateShutter_Update(Actor* thisx, PlayState* play);
void BgGateShutter_Draw(Actor* thisx, PlayState* play);

void func_8087828C(BgGateShutter* this, PlayState* play);
void func_80878300(BgGateShutter* this, PlayState* play);
void func_808783AC(BgGateShutter* this, PlayState* play);
void func_808783D4(BgGateShutter* this, PlayState* play);

const ActorInit Bg_Gate_Shutter_InitVars = {
    ACTOR_BG_GATE_SHUTTER,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_SPOT01_MATOYAB,
    sizeof(BgGateShutter),
    (ActorFunc)BgGateShutter_Init,
    (ActorFunc)BgGateShutter_Destroy,
    (ActorFunc)BgGateShutter_Update,
    (ActorFunc)BgGateShutter_Draw,
    NULL,
};

void BgGateShutter_Init(Actor* thisx, PlayState* play) {
    BgGateShutter* this = (BgGateShutter*)thisx;
    s32 pad[2];
    CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&gKakarikoGuardGateCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);
    this->somePos.x = thisx->world.pos.x;
    this->somePos.y = thisx->world.pos.y;
    this->somePos.z = thisx->world.pos.z;
    if (((gSaveContext.infTable[7] & 0x40) ||
         (!gSaveContext.n64ddFlag && (gSaveContext.eventChkInf[4] & 0x20)) ||
         (gSaveContext.n64ddFlag && (Randomizer_GetSettingValue(RSK_KAK_GATE) == RO_KAK_GATE_OPEN))) &&
        (play->sceneNum == SCENE_SPOT01)) {
        thisx->world.pos.x = -89.0f;
        thisx->world.pos.z = -1375.0f;
    }
    thisx->scale.x = 1.0f;
    thisx->scale.y = 1.0f;
    thisx->scale.z = 1.0f;
    osSyncPrintf("\n\n");
    osSyncPrintf(VT_FGCOL(GREEN) " ☆☆☆☆☆ 柵でたなぁ ☆☆☆☆☆ \n" VT_RST);
    this->actionFunc = func_8087828C;
}

void BgGateShutter_Destroy(Actor* thisx, PlayState* play) {
    BgGateShutter* this = (BgGateShutter*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_8087828C(BgGateShutter* this, PlayState* play) {
    if (this->openingState == 1 && !(gSaveContext.infTable[7] & 0x40)) {
        this->unk_178 = 2;
        this->actionFunc = func_80878300;
    } else if (this->openingState == 2) {
        this->unk_178 = 2;
        this->actionFunc = func_80878300;
    } else if (this->openingState < 0) {
        this->unk_178 = 2;
        this->actionFunc = func_808783D4;
    }
}

void func_80878300(BgGateShutter* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    if (this->unk_178 == 0) {
        Audio_PlayActorSound2(thisx, NA_SE_EV_METALGATE_OPEN - SFX_FLAG);
        thisx->world.pos.x -= 2.0f;
        Math_ApproachF(&thisx->world.pos.z, -1375.0f, 0.8f, 0.3f);
        if (thisx->world.pos.x < -89.0f) {
            Audio_PlayActorSound2(thisx, NA_SE_EV_BRIDGE_OPEN_STOP);
            this->unk_178 = 0x1E;
            this->actionFunc = func_808783AC;
        }
    }
}

void func_808783AC(BgGateShutter* this, PlayState* play) {
    if (this->unk_178 == 0) {
        this->openingState = 0;
        this->actionFunc = func_8087828C;
    }
}

void func_808783D4(BgGateShutter* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    if (this->unk_178 == 0) {
        Audio_PlayActorSound2(thisx, NA_SE_EV_METALGATE_OPEN - SFX_FLAG);
        thisx->world.pos.x += 2.0f;
        Math_ApproachF(&thisx->world.pos.z, -1350.0f, 0.8f, 0.3f);
        if (thisx->world.pos.x > 90.0f) {
            thisx->world.pos.x = 91.0f;
            Audio_PlayActorSound2(thisx, NA_SE_EV_BRIDGE_OPEN_STOP);
            this->unk_178 = 30;
            this->actionFunc = func_808783AC;
        }
    }
}

void BgGateShutter_Update(Actor* thisx, PlayState* play) {
    BgGateShutter* this = (BgGateShutter*)thisx;

    if (this->unk_178 != 0) {
        this->unk_178 -= 1;
    }
    this->actionFunc(this, play);
}

void BgGateShutter_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gKakarikoGuardGateDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
