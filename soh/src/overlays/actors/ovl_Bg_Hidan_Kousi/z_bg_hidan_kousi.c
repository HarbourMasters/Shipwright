/*
 * File: z_bg_hidan_kousi.c
 * Overlay: ovl_Bg_Hidan_Kousi
 * Description:
 */

#include "z_bg_hidan_kousi.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS ACTOR_FLAG_4

void BgHidanKousi_Init(Actor* thisx, PlayState* play);
void BgHidanKousi_Destroy(Actor* thisx, PlayState* play);
void BgHidanKousi_Update(Actor* thisx, PlayState* play);
void BgHidanKousi_Draw(Actor* thisx, PlayState* play);

void func_80889ACC(BgHidanKousi* this);
void func_80889B5C(BgHidanKousi* this, PlayState* play);
void func_80889BC0(BgHidanKousi* this, PlayState* play);
void func_80889C18(BgHidanKousi* this, PlayState* play);
void func_80889C90(BgHidanKousi* this, PlayState* play);
void func_80889D28(BgHidanKousi* this, PlayState* play);

static f32 D_80889E40[] = { 120.0f, 150.0f, 150.0f };

const ActorInit Bg_Hidan_Kousi_InitVars = {
    ACTOR_BG_HIDAN_KOUSI,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanKousi),
    (ActorFunc)BgHidanKousi_Init,
    (ActorFunc)BgHidanKousi_Destroy,
    (ActorFunc)BgHidanKousi_Update,
    (ActorFunc)BgHidanKousi_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static CollisionHeader* sMetalFencesCollisions[] = {
    &gFireTempleMetalFenceWithSlantCol,
    &gFireTempleMetalFenceCol,
    &gFireTempleMetalFence2Col,
};

static s16 D_80889E7C[] = {
    0x4000,
    0xC000,
    0xC000,
    0x0000,
};

static Gfx* sMetalFencesDLs[] = {
    gFireTempleMetalFenceWithSlantDL,
    gFireTempleMetalFenceDL,
    gFireTempleMetalFence2DL,
};

void BgHidanKousi_SetupAction(BgHidanKousi* this, BgHidanKousiActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void BgHidanKousi_Init(Actor* thisx, PlayState* play) {
    BgHidanKousi* this = (BgHidanKousi*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    Actor_SetFocus(thisx, 50.0f);
    osSyncPrintf("◯◯◯炎の神殿オブジェクト【格子(arg_data : %0x)】出現 (%d %d)\n", thisx->params, thisx->params & 0xFF,
                 ((s32)thisx->params >> 8) & 0xFF);

    Actor_ProcessInitChain(thisx, sInitChain);
    if (((thisx->params & 0xFF) < 0) || ((thisx->params & 0xFF) >= 3)) {
        osSyncPrintf("arg_data おかしい 【格子】\n");
    }

    CollisionHeader_GetVirtual(sMetalFencesCollisions[thisx->params & 0xFF], &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, thisx, colHeader);
    thisx->world.rot.y = D_80889E7C[this->dyna.actor.params & 0xFF] + thisx->shape.rot.y;
    if (Flags_GetSwitch(play, (thisx->params >> 8) & 0xFF)) {
        func_80889ACC(this);
        BgHidanKousi_SetupAction(this, func_80889D28);
    } else {
        BgHidanKousi_SetupAction(this, func_80889B5C);
    }
}

void BgHidanKousi_Destroy(Actor* thisx, PlayState* play) {
    BgHidanKousi* this = (BgHidanKousi*)thisx;
    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80889ACC(BgHidanKousi* this) {
    s32 pad[2];
    Vec3s* rot = &this->dyna.actor.world.rot;
    f32 temp1 = D_80889E40[this->dyna.actor.params & 0xFF] * Math_SinS(rot->y);
    f32 temp2 = D_80889E40[this->dyna.actor.params & 0xFF] * Math_CosS(rot->y);

    this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x + temp1;
    this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z + temp2;
}

void func_80889B5C(BgHidanKousi* this, PlayState* play) {
    if (Flags_GetSwitch(play, (this->dyna.actor.params >> 8) & 0xFF)) {
        BgHidanKousi_SetupAction(this, func_80889BC0);
        OnePointCutscene_Attention(play, &this->dyna.actor);
        this->unk_168 = 0xC8;
    }
}

void func_80889BC0(BgHidanKousi* this, PlayState* play) {
    this->unk_168 -= 1;
    if (this->dyna.actor.category == func_8005B198() || (this->unk_168 <= 0)) {
        BgHidanKousi_SetupAction(this, func_80889C18);
    }
}

void func_80889C18(BgHidanKousi* this, PlayState* play) {
    this->dyna.actor.speedXZ += 0.2f;
    if (this->dyna.actor.speedXZ > 2.0f) {
        this->dyna.actor.speedXZ = 2.0f;
        BgHidanKousi_SetupAction(this, func_80889C90);
    }
    Actor_MoveForward(&this->dyna.actor);
    func_8002F974(&this->dyna.actor, NA_SE_EV_METALDOOR_SLIDE - SFX_FLAG);
}

void func_80889C90(BgHidanKousi* this, PlayState* play) {
    func_8002D7EC(&this->dyna.actor);
    if (D_80889E40[this->dyna.actor.params & 0xFF] <
        Math_Vec3f_DistXYZ(&this->dyna.actor.home.pos, &this->dyna.actor.world.pos)) {
        func_80889ACC(this);
        BgHidanKousi_SetupAction(this, func_80889D28);
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_METALDOOR_STOP);
    } else {
        func_8002F974(&this->dyna.actor, NA_SE_EV_METALDOOR_SLIDE - SFX_FLAG);
    }
}

void func_80889D28(BgHidanKousi* this, PlayState* play) {
}

void BgHidanKousi_Update(Actor* thisx, PlayState* play) {
    BgHidanKousi* this = (BgHidanKousi*)thisx;

    this->actionFunc(this, play);
}

void BgHidanKousi_Draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, sMetalFencesDLs[thisx->params & 0xFF]);

    CLOSE_DISPS(play->state.gfxCtx);
}
