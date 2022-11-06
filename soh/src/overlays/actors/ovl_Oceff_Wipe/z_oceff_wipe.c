/*
 * File: z_oceff_wipe.c
 * Overlay: ovl_Oceff_Wipe
 * Description: Zelda's Lullaby and Song of Time Ocarina Effect
 */

#include "z_oceff_wipe.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void OceffWipe_Init(Actor* thisx, PlayState* play);
void OceffWipe_Destroy(Actor* thisx, PlayState* play);
void OceffWipe_Update(Actor* thisx, PlayState* play);
void OceffWipe_Draw(Actor* thisx, PlayState* play);

const ActorInit Oceff_Wipe_InitVars = {
    ACTOR_OCEFF_WIPE,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(OceffWipe),
    (ActorFunc)OceffWipe_Init,
    (ActorFunc)OceffWipe_Destroy,
    (ActorFunc)OceffWipe_Update,
    (ActorFunc)OceffWipe_Draw,
    NULL,
};

void OceffWipe_Init(Actor* thisx, PlayState* play) {
    OceffWipe* this = (OceffWipe*)thisx;

    Actor_SetScale(&this->actor, 0.1f);
    this->timer = 0;
    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    osSyncPrintf(VT_FGCOL(CYAN) " WIPE arg_data = %d\n" VT_RST, this->actor.params);
}

void OceffWipe_Destroy(Actor* thisx, PlayState* play) {
    OceffWipe* this = (OceffWipe*)thisx;
    Player* player = GET_PLAYER(play);

    func_800876C8(play);
    if (gSaveContext.nayrusLoveTimer != 0) {
        player->stateFlags3 |= 0x40;
    }
}

void OceffWipe_Update(Actor* thisx, PlayState* play) {
    OceffWipe* this = (OceffWipe*)thisx;

    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    if (this->timer < 100) {
        this->timer++;
    } else {
        Actor_Kill(&this->actor);
    }
}

#include "overlays/ovl_Oceff_Wipe/ovl_Oceff_Wipe.h"

static u8 sAlphaIndices[] = {
    0x01, 0x10, 0x22, 0x01, 0x20, 0x12, 0x01, 0x20, 0x12, 0x01,
    0x10, 0x22, 0x01, 0x20, 0x12, 0x01, 0x12, 0x21, 0x01, 0x02,
};

void OceffWipe_Draw(Actor* thisx, PlayState* play) {
    u32 scroll = play->state.frames & 0xFF;
    OceffWipe* this = (OceffWipe*)thisx;
    f32 z;
    s32 pad;
    u8 alphaTable[3];
    s32 i;
    Vec3f eye;
    Vtx* vtxPtr;
    Vec3f vec;

    eye = GET_ACTIVE_CAM(play)->eye;
    Camera_GetSkyboxOffset(&vec, GET_ACTIVE_CAM(play));

    OPEN_DISPS(play->state.gfxCtx);

    if (this->timer < 32) {
        z = Math_SinS(this->timer << 9) * 1400;
    } else {
        z = 1400;
    }

    if (this->timer >= 80) {
        alphaTable[0] = 0;
        alphaTable[1] = (0x64 - this->timer) * 8;
        alphaTable[2] = (0x64 - this->timer) * 12;
    } else {
        alphaTable[0] = 0;
        alphaTable[1] = 0xA0;
        alphaTable[2] = 0xFF;
    }

    for (i = 0; i < 20; i++) {
        vtxPtr = ResourceMgr_LoadVtxByName(sFrustumVtx);
        vtxPtr[i * 2 + 0].v.cn[3] = alphaTable[(sAlphaIndices[i] & 0xF0) >> 4];
        vtxPtr[i * 2 + 1].v.cn[3] = alphaTable[sAlphaIndices[i] & 0xF];
    }

    func_80093D84(play->state.gfxCtx);

    Matrix_Translate(eye.x + vec.x, eye.y + vec.y, eye.z + vec.z, MTXMODE_NEW);
    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_Translate(0.0f, 0.0f, -z, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (this->actor.params != OCEFF_WIPE_ZL) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 170, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 150, 255, 128);
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 200, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 100, 0, 255, 128);
    }

    gSPDisplayList(POLY_XLU_DISP++, sMaterialDL);
    gSPDisplayList(POLY_XLU_DISP++, Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0 - scroll, scroll * (-2), 32, 32, 1,
                                                     0 - scroll, scroll * (-2), 32, 32));
    gSPDisplayList(POLY_XLU_DISP++, sFrustumDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
