/*
 * File: z_bg_mori_hineri.c
 * Overlay: ovl_Bg_Mori_Hineri
 * Description: Twisting hallway in Forest Temple
 */

#include "z_bg_mori_hineri.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_box/object_box.h"
#include "objects/object_mori_hineri1/object_mori_hineri1.h"
#include "objects/object_mori_hineri1a/object_mori_hineri1a.h"
#include "objects/object_mori_hineri2/object_mori_hineri2.h"
#include "objects/object_mori_hineri2a/object_mori_hineri2a.h"
#include "objects/object_mori_tex/object_mori_tex.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_5)

void BgMoriHineri_Init(Actor* thisx, GlobalContext* globalCtx);
void BgMoriHineri_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgMoriHineri_Update(Actor* thisx, GlobalContext* globalCtx);
void BgMoriHineri_DrawHallAndRoom(Actor* thisx, GlobalContext* globalCtx);
void BgMoriHineri_Reset(void);

void func_808A39FC(BgMoriHineri* this, GlobalContext* globalCtx);
void func_808A3E54(BgMoriHineri* this, GlobalContext* globalCtx);
void func_808A3C8C(BgMoriHineri* this, GlobalContext* globalCtx);
void BgMoriHineri_SpawnBossKeyChest(BgMoriHineri* this, GlobalContext* globalCtx);
void BgMoriHineri_DoNothing(BgMoriHineri* this, GlobalContext* globalCtx);
void func_808A3D58(BgMoriHineri* this, GlobalContext* globalCtx);

s16 sBgMoriHineriNextCamIdx = SUBCAM_NONE;

const ActorInit Bg_Mori_Hineri_InitVars = {
    ACTOR_BG_MORI_HINERI,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(BgMoriHineri),
    (ActorFunc)BgMoriHineri_Init,
    (ActorFunc)BgMoriHineri_Destroy,
    (ActorFunc)BgMoriHineri_Update,
    NULL,
    BgMoriHineri_Reset,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

static Gfx* sDLists[] = {
    object_mori_hineri1_DL_0024E0,
    object_mori_hineri1a_DL_001980,
    object_mori_hineri2_DL_0020F0,
    object_mori_hineri2a_DL_002B70,
};

void BgMoriHineri_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgMoriHineri* this = (BgMoriHineri*)thisx;
    s8 moriHineriObjIdx;
    u32 switchFlagParam;
    s32 t6;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_PLAYER);

    switchFlagParam = this->dyna.actor.params & 0x3F;
    t6 = this->dyna.actor.params & 0x4000;

    if (t6 != 0) {
        this->switchFlag = switchFlagParam;
    } else {
        this->switchFlag = ((this->dyna.actor.params >> 8) & 0x3F);
        this->switchFlag = (Flags_GetSwitch(globalCtx, this->switchFlag)) ? 1 : 0;
    }
    this->dyna.actor.params = ((this->dyna.actor.params & 0x8000) >> 0xE);
    if (Flags_GetSwitch(globalCtx, switchFlagParam)) {
        if (this->dyna.actor.params == 0) {
            this->dyna.actor.params = 1;
        } else if (this->dyna.actor.params == 2) {
            this->dyna.actor.params = 3;
        }
    }
    this->boxObjIdx = -1;
    if (this->dyna.actor.params == 0) {
        this->moriHineriObjIdx = Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_HINERI1);
        if (t6 == 0) {
            this->boxObjIdx = Object_GetIndex(&globalCtx->objectCtx, OBJECT_BOX);
        }
    } else {
        if (this->dyna.actor.params == 1) {
            moriHineriObjIdx = Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_HINERI1A);
        } else {
            moriHineriObjIdx = (this->dyna.actor.params == 2)
                                   ? Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_HINERI2)
                                   : Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_HINERI2A);
        }
        this->moriHineriObjIdx = moriHineriObjIdx;
    }
    this->moriTexObjIdx = Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_TEX);
    if (t6 != 0) {
        this->dyna.actor.params += 4;
    }
    if ((this->moriHineriObjIdx < 0) || (this->moriTexObjIdx < 0)) {
        Actor_Kill(&this->dyna.actor);
    } else {
        this->actionFunc = func_808A39FC;
    }
}

void BgMoriHineri_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgMoriHineri* this = (BgMoriHineri*)thisx;
    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
}

void func_808A39FC(BgMoriHineri* this, GlobalContext* globalCtx) {
    CollisionHeader* colHeader;

    if (Object_IsLoaded(&globalCtx->objectCtx, this->moriHineriObjIdx) &&
        Object_IsLoaded(&globalCtx->objectCtx, this->moriTexObjIdx) &&
        ((this->boxObjIdx < 0) || Object_IsLoaded(&globalCtx->objectCtx, this->boxObjIdx))) {
        this->dyna.actor.objBankIndex = this->moriHineriObjIdx;
        if (this->dyna.actor.params >= 4) {
            this->dyna.actor.params -= 4;
            if (this->dyna.actor.params == 0) {
                this->moriHineriObjIdx = Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_HINERI1A);
            } else if (this->dyna.actor.params == 1) {
                this->moriHineriObjIdx = Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_HINERI1);
            } else {
                this->moriHineriObjIdx = (this->dyna.actor.params == 2)
                                             ? Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_HINERI2A)
                                             : Object_GetIndex(&globalCtx->objectCtx, OBJECT_MORI_HINERI2);
            }
            if (this->moriHineriObjIdx < 0) {
                Actor_Kill(&this->dyna.actor);
            } else {
                this->actionFunc = func_808A3D58;
            }
        } else {
            Actor_SetObjectDependency(globalCtx, &this->dyna.actor);
            colHeader = NULL;
            this->dyna.actor.draw = BgMoriHineri_DrawHallAndRoom;
            if (this->dyna.actor.params == 0) {
                this->actionFunc = func_808A3C8C;
                CollisionHeader_GetVirtual(&object_mori_hineri1_Col_0054B8, &colHeader);
            } else if (this->dyna.actor.params == 1) {
                this->actionFunc = BgMoriHineri_SpawnBossKeyChest;
                CollisionHeader_GetVirtual(&object_mori_hineri1a_Col_003490, &colHeader);
            } else if (this->dyna.actor.params == 2) {
                this->actionFunc = BgMoriHineri_DoNothing;
                CollisionHeader_GetVirtual(&object_mori_hineri2_Col_0043D0, &colHeader);
            } else {
                this->actionFunc = func_808A3C8C;
                CollisionHeader_GetVirtual(&object_mori_hineri2a_Col_006078, &colHeader);
            }
            this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);
        }
    }
}

void BgMoriHineri_DoNothing(BgMoriHineri* this, GlobalContext* globalCtx) {
}

void BgMoriHineri_SpawnBossKeyChest(BgMoriHineri* this, GlobalContext* globalCtx) {
    Actor_Spawn(&globalCtx->actorCtx, globalCtx, ACTOR_EN_BOX, this->dyna.actor.world.pos.x + 147.0f,
                this->dyna.actor.world.pos.y + -245.0f, this->dyna.actor.world.pos.z + -453.0f, 0, 0x4000, 0, 0x27EE);
    this->actionFunc = BgMoriHineri_DoNothing;
}

void func_808A3C8C(BgMoriHineri* this, GlobalContext* globalCtx) {
    f32 f0;
    Player* player = GET_PLAYER(globalCtx);

    f0 = 1100.0f - (player->actor.world.pos.z - this->dyna.actor.world.pos.z);
    this->dyna.actor.shape.rot.z = CLAMP(f0, 0.0f, 1000.0f) * 16.384f;
    Camera_ChangeSetting(globalCtx->cameraPtrs[MAIN_CAM], CAM_SET_DUNGEON1);
    if (this->dyna.actor.params != 0) {
        this->dyna.actor.shape.rot.z = -this->dyna.actor.shape.rot.z;
    }
}

void func_808A3D58(BgMoriHineri* this, GlobalContext* globalCtx) {
    s16 mainCamChildIdx;

    if ((Flags_GetSwitch(globalCtx, this->switchFlag) &&
         (this->dyna.actor.params == 0 || this->dyna.actor.params == 2)) ||
        (!Flags_GetSwitch(globalCtx, this->switchFlag) &&
         (this->dyna.actor.params == 1 || this->dyna.actor.params == 3))) {
        this->dyna.actor.draw = BgMoriHineri_DrawHallAndRoom;
        this->actionFunc = func_808A3E54;

        mainCamChildIdx = globalCtx->cameraPtrs[MAIN_CAM]->childCamIdx;
        if ((mainCamChildIdx != SUBCAM_FREE) &&
            (globalCtx->cameraPtrs[mainCamChildIdx]->setting == CAM_SET_CS_TWISTED_HALLWAY)) {
            OnePointCutscene_EndCutscene(globalCtx, mainCamChildIdx);
        }
        OnePointCutscene_Init(globalCtx, 3260, 40, &this->dyna.actor, MAIN_CAM);
        sBgMoriHineriNextCamIdx = OnePointCutscene_Init(globalCtx, 3261, 40, &this->dyna.actor, MAIN_CAM);
    }
}

void func_808A3E54(BgMoriHineri* this, GlobalContext* globalCtx) {
    s8 objBankIndex;

    if (globalCtx->activeCamera == sBgMoriHineriNextCamIdx) {
        if (sBgMoriHineriNextCamIdx != MAIN_CAM) {
            objBankIndex = this->dyna.actor.objBankIndex;
            this->dyna.actor.objBankIndex = this->moriHineriObjIdx;
            this->moriHineriObjIdx = objBankIndex;
            this->dyna.actor.params ^= 1;
            sBgMoriHineriNextCamIdx = MAIN_CAM;
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
        } else {
            this->dyna.actor.draw = NULL;
            this->actionFunc = func_808A3D58;
            sBgMoriHineriNextCamIdx = SUBCAM_NONE;
        }
    }
    if ((sBgMoriHineriNextCamIdx >= SUBCAM_FIRST) &&
        ((GET_ACTIVE_CAM(globalCtx)->eye.z - this->dyna.actor.world.pos.z) < 1100.0f)) {
        func_8002F948(&this->dyna.actor, NA_SE_EV_FLOOR_ROLLING - SFX_FLAG);
    }
}

void BgMoriHineri_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgMoriHineri* this = (BgMoriHineri*)thisx;

    this->actionFunc(this, globalCtx);
}

void BgMoriHineri_DrawHallAndRoom(Actor* thisx, GlobalContext* globalCtx) {
    BgMoriHineri* this = (BgMoriHineri*)thisx;
    s8 objIndex;
    MtxF mtx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_hineri.c", 611);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, globalCtx->objectCtx.status[this->moriTexObjIdx].segment);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_mori_hineri.c", 618),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, sDLists[this->dyna.actor.params]);
    if (this->boxObjIdx > 0) {
        Matrix_Get(&mtx);
    }
    if ((this->actionFunc == func_808A3C8C) && (this->dyna.actor.shape.rot.z != 0)) {
        func_80093D18(globalCtx->state.gfxCtx);
        if (this->dyna.actor.params == 0) {
            Matrix_Translate(-1761.0f, 1278.0f, -1821.0f, MTXMODE_NEW);
        } else {
            Matrix_Translate(1999.0f, 1278.0f, -1821.0f, MTXMODE_NEW);
        }
        Matrix_RotateZYX(0, -0x8000, this->dyna.actor.shape.rot.z, MTXMODE_APPLY);
        Matrix_Translate(0.0f, -50.0f, 0.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_mori_hineri.c", 652),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gDungeonDoorDL);
    }
    if ((this->boxObjIdx > 0) && ((this->boxObjIdx = Object_GetIndex(&globalCtx->objectCtx, OBJECT_BOX)) > 0) &&
        Object_IsLoaded(&globalCtx->objectCtx, this->boxObjIdx)) {
        gSPSegment(POLY_OPA_DISP++, 0x06, globalCtx->objectCtx.status[this->boxObjIdx].segment);
        gSPSegment(POLY_OPA_DISP++, 0x08, &D_80116280[2]);
        Matrix_Put(&mtx);
        Matrix_Translate(147.0f, -245.0f, -453.0f, MTXMODE_APPLY);
        Matrix_RotateY(M_PI / 2, MTXMODE_APPLY);
        Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_mori_hineri.c", 689),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gTreasureChestBossKeyChestFrontDL);
        Matrix_Put(&mtx);
        Matrix_Translate(167.0f, -218.0f, -453.0f, MTXMODE_APPLY);
        if (Flags_GetTreasure(globalCtx, 0xE)) {
            Matrix_RotateZ(0x3500 * (M_PI / 0x8000), MTXMODE_APPLY);
        } else {
            Matrix_RotateZ(M_PI, MTXMODE_APPLY);
        }
        Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_bg_mori_hineri.c", 703),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gTreasureChestBossKeyChestSideAndTopDL);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_bg_mori_hineri.c", 709);
}

void BgMoriHineri_Reset() {
    sBgMoriHineriNextCamIdx = SUBCAM_NONE;
}