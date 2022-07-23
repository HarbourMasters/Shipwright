/*
 * File: z_bg_spot00_hanebasi.c
 * Overlay: ovl_Bg_Spot00_Hanebasi
 * Description: Hyrule Field Drawbridge and Torches
 */

#include "z_bg_spot00_hanebasi.h"
#include "objects/object_spot00_objects/object_spot00_objects.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS ACTOR_FLAG_4

typedef enum {
    /* -1 */ DT_DRAWBRIDGE = -1,
    /*  0 */ DT_CHAIN_1,
    /*  1 */ DT_CHAIN_2
} DrawbridgeType;

void BgSpot00Hanebasi_Init(Actor* thisx, GlobalContext* globalCtx);
void BgSpot00Hanebasi_Destroy(Actor* thisx, GlobalContext* globalCtx);
void BgSpot00Hanebasi_Update(Actor* thisx, GlobalContext* globalCtx);
void BgSpot00Hanebasi_Draw(Actor* thisx, GlobalContext* globalCtx);

void BgSpot00Hanebasi_DrawbridgeWait(BgSpot00Hanebasi* this, GlobalContext* globalCtx);
void BgSpot00Hanebasi_DrawbridgeRiseAndFall(BgSpot00Hanebasi* this, GlobalContext* globalCtx);
void BgSpot00Hanebasi_SetTorchLightInfo(BgSpot00Hanebasi* this, GlobalContext* globalCtx);

const ActorInit Bg_Spot00_Hanebasi_InitVars = {
    ACTOR_BG_SPOT00_HANEBASI,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SPOT00_OBJECTS,
    sizeof(BgSpot00Hanebasi),
    (ActorFunc)BgSpot00Hanebasi_Init,
    (ActorFunc)BgSpot00Hanebasi_Destroy,
    (ActorFunc)BgSpot00Hanebasi_Update,
    (ActorFunc)BgSpot00Hanebasi_Draw,
    NULL,
};

static f32 sTorchFlameScale = 0.0f;

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 550, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 5000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void BgSpot00Hanebasi_Init(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot00Hanebasi* this = (BgSpot00Hanebasi*)thisx;
    s32 pad;
    Vec3f chainPos;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 1);

    if (this->dyna.actor.params == DT_DRAWBRIDGE) {
        CollisionHeader_GetVirtual(&gHyruleFieldCastleDrawbridgeCol, &colHeader);
    } else {
        CollisionHeader_GetVirtual(&gHyruleFieldCastleDrawbridgeChainsCol, &colHeader);
    }

    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);

    if (this->dyna.actor.params == DT_DRAWBRIDGE) {
        if (LINK_IS_ADULT && (gSaveContext.sceneSetupIndex < 4)) {
            Actor_Kill(&this->dyna.actor);
            return;
        }

        if ((gSaveContext.sceneSetupIndex != 6) &&
            ((gSaveContext.sceneSetupIndex == 4) || (gSaveContext.sceneSetupIndex == 5) ||
             (!LINK_IS_ADULT && !IS_DAY))) {
            this->dyna.actor.shape.rot.x = -0x4000;
        } else {
            this->dyna.actor.shape.rot.x = 0;
        }

        if (gSaveContext.sceneSetupIndex != 6) {
            // Don't close the bridge in rando to accomodate hyrule castle exit
            if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) && CHECK_QUEST_ITEM(QUEST_GORON_RUBY) &&
                CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE) && !(gSaveContext.eventChkInf[8] & 1) && !(gSaveContext.n64ddFlag)) {
                this->dyna.actor.shape.rot.x = -0x4000;
            }
        }

        chainPos.y =
            (10.0f * Math_CosS(this->dyna.actor.shape.rot.x)) - (Math_SinS(this->dyna.actor.shape.rot.x) * 400.0f);
        chainPos.z =
            (10.0f * Math_SinS(this->dyna.actor.shape.rot.x)) - (Math_CosS(this->dyna.actor.shape.rot.x) * 400.0f);
        chainPos.x =
            (158.0f * Math_CosS(this->dyna.actor.shape.rot.y)) + (Math_SinS(this->dyna.actor.shape.rot.y) * chainPos.z);
        chainPos.z = (-158.0f * Math_SinS(this->dyna.actor.shape.rot.y)) +
                     (Math_CosS(this->dyna.actor.shape.rot.y) * chainPos.z);

        if (Actor_SpawnAsChild(&globalCtx->actorCtx, &this->dyna.actor, globalCtx, ACTOR_BG_SPOT00_HANEBASI,
                               this->dyna.actor.world.pos.x + chainPos.x, this->dyna.actor.world.pos.y + chainPos.y,
                               this->dyna.actor.world.pos.z + chainPos.z,
                               ((this->dyna.actor.shape.rot.x == 0) ? 0 : 0xF020), this->dyna.actor.shape.rot.y, 0,
                               DT_CHAIN_1) == NULL) {
            Actor_Kill(&this->dyna.actor);
        }

        this->actionFunc = BgSpot00Hanebasi_DrawbridgeWait;
        this->destAngle = 40;
    } else if (this->dyna.actor.params == DT_CHAIN_1) {
        if (Actor_SpawnAsChild(&globalCtx->actorCtx, &this->dyna.actor, globalCtx, ACTOR_BG_SPOT00_HANEBASI,
                               this->dyna.actor.world.pos.x - (Math_CosS(this->dyna.actor.shape.rot.y) * 316.0f),
                               this->dyna.actor.world.pos.y,
                               this->dyna.actor.world.pos.z + (Math_SinS(this->dyna.actor.shape.rot.y) * 316.0f),
                               this->dyna.actor.shape.rot.x, this->dyna.actor.shape.rot.y, 0, DT_CHAIN_2) == NULL) {
            Actor_Kill(&this->dyna.actor);
            Actor_Kill(this->dyna.actor.parent);
        }

        this->actionFunc = BgSpot00Hanebasi_SetTorchLightInfo;
    } else {
        this->actionFunc = BgSpot00Hanebasi_SetTorchLightInfo;
    }

    if (this->dyna.actor.params >= DT_CHAIN_1) {
        this->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &this->lightInfo);
        Lights_PointGlowSetInfo(&this->lightInfo, ((this->dyna.actor.params == DT_CHAIN_1) ? 260.0f : -260.0f), 168,
                                690, 255, 255, 0, 0);
    }
}

void BgSpot00Hanebasi_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot00Hanebasi* this = (BgSpot00Hanebasi*)thisx;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);

    if (this->dyna.actor.params >= DT_CHAIN_1) {
        LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, this->lightNode);
    }
}

void BgSpot00Hanebasi_DrawbridgeWait(BgSpot00Hanebasi* this, GlobalContext* globalCtx) {
    BgSpot00Hanebasi* child = (BgSpot00Hanebasi*)this->dyna.actor.child;

    if ((gSaveContext.sceneSetupIndex >= 4) || !CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) ||
        !CHECK_QUEST_ITEM(QUEST_GORON_RUBY) || !CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE) ||
        (gSaveContext.eventChkInf[8] & 1)) {
        if (this->dyna.actor.shape.rot.x != 0) {
            if (Flags_GetEnv(globalCtx, 0) || ((gSaveContext.sceneSetupIndex < 4) && IS_DAY)) {
                this->actionFunc = BgSpot00Hanebasi_DrawbridgeRiseAndFall;
                this->destAngle = 0;
                child->destAngle = 0;
                return;
            }

            if (this) {} // required to match
        }
        if ((this->dyna.actor.shape.rot.x == 0) && (gSaveContext.sceneSetupIndex < 4) && !LINK_IS_ADULT && !IS_DAY) {
            this->actionFunc = BgSpot00Hanebasi_DrawbridgeRiseAndFall;
            this->destAngle = -0x4000;
            child->destAngle = -0xFE0;
        }
    }
}

void BgSpot00Hanebasi_DoNothing(BgSpot00Hanebasi* this, GlobalContext* globalCtx) {
}

void BgSpot00Hanebasi_DrawbridgeRiseAndFall(BgSpot00Hanebasi* this, GlobalContext* globalCtx) {
    BgSpot00Hanebasi* child;
    Actor* childsChild;
    s16 angle = 80;

    if (Math_ScaledStepToS(&this->dyna.actor.shape.rot.x, this->destAngle, 80)) {
        this->actionFunc = BgSpot00Hanebasi_DrawbridgeWait;
    }

    if (this->dyna.actor.shape.rot.x >= -0x27D8) {
        child = (BgSpot00Hanebasi*)this->dyna.actor.child;
        angle *= 0.4f;
        Math_ScaledStepToS(&child->dyna.actor.shape.rot.x, child->destAngle, angle);
        childsChild = child->dyna.actor.child;
        Math_ScaledStepToS(&childsChild->shape.rot.x, child->destAngle, angle);
    }

    if (this->destAngle < 0) {
        if (this->actionFunc == BgSpot00Hanebasi_DrawbridgeWait) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BRIDGE_CLOSE_STOP);
        } else {
            func_8002F974(&this->dyna.actor, NA_SE_EV_BRIDGE_CLOSE - SFX_FLAG);
        }
    } else {
        if (this->actionFunc == BgSpot00Hanebasi_DrawbridgeWait) {
            Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BRIDGE_OPEN_STOP);
        } else {
            func_8002F974(&this->dyna.actor, NA_SE_EV_BRIDGE_OPEN - SFX_FLAG);
        }
    }
}

void BgSpot00Hanebasi_SetTorchLightInfo(BgSpot00Hanebasi* this, GlobalContext* globalCtx) {
    u8 lightColor = (u8)(Rand_ZeroOne() * 127.0f) + 128; // intensity of the red and green channels

    Lights_PointGlowSetInfo(&this->lightInfo, (this->dyna.actor.params == DT_CHAIN_1) ? 260.0f : -260.0f,
                            (5000.0f * sTorchFlameScale) + 128.0f, 690, lightColor, lightColor, 0,
                            sTorchFlameScale * 37500.0f);
}

void BgSpot00Hanebasi_Update(Actor* thisx, GlobalContext* globalCtx) {
    BgSpot00Hanebasi* this = (BgSpot00Hanebasi*)thisx;
    s32 pad;

    this->actionFunc(this, globalCtx);

    if (this->dyna.actor.params == DT_DRAWBRIDGE) {
        if (globalCtx->sceneNum == SCENE_SPOT00) {
            if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) && CHECK_QUEST_ITEM(QUEST_GORON_RUBY) &&
                CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE) && !(gSaveContext.eventChkInf[8] & 1) && LINK_IS_CHILD) {
                Player* player = GET_PLAYER(globalCtx);

                if ((player->actor.world.pos.x > -450.0f) && (player->actor.world.pos.x < 450.0f) &&
                    (player->actor.world.pos.z > 1080.0f) && (player->actor.world.pos.z < 1700.0f) &&
                    (!(Gameplay_InCsMode(globalCtx)))) {
                    gSaveContext.eventChkInf[8] |= 1;
                    Flags_SetEventChkInf(0x82);
                    this->actionFunc = BgSpot00Hanebasi_DoNothing;
                    func_8002DF54(globalCtx, &player->actor, 8);
                    globalCtx->nextEntranceIndex = 0x00CD;
                    gSaveContext.nextCutsceneIndex = 0xFFF1;
                    globalCtx->sceneLoadFlag = 0x14;
                    globalCtx->fadeTransition = 4;
                } else if (Actor_IsFacingAndNearPlayer(&this->dyna.actor, 3000.0f, 0x7530)) {
                    globalCtx->envCtx.gloomySkyMode = 1;
                }
            }
        }

        if (gSaveContext.sceneSetupIndex == 5) {
            u16 dayTime;
            s32 tmp;

            if (gTimeIncrement == 50) {
                tmp = 0xD556;

                if (gSaveContext.dayTime >= 0xD557) {
                    tmp = 0x1D556;
                }

                gTimeIncrement = (tmp - gSaveContext.dayTime) * (1.0f / 350.0f);
            }

            dayTime = gSaveContext.dayTime;

            if ((dayTime >= 0x2AAC) && (dayTime < 0x3000) && (gSaveContext.sceneSetupIndex == 5)) {
                gTimeIncrement = 0;
            }
        }
    }
}

void BgSpot00Hanebasi_DrawTorches(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    f32 angle;
    s32 i;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D84(globalCtx->state.gfxCtx);

    if (gSaveContext.sceneSetupIndex >= 4) {
        sTorchFlameScale = 0.008f;
    } else {
        sTorchFlameScale = ((thisx->shape.rot.x * -1) - 0x2000) * (1.0f / 1024000.0f);
    }

    angle = (s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) + 0x8000) * (M_PI / 32768.0f);
    gDPSetPrimColor(POLY_XLU_DISP++, 128, 128, 255, 255, 0, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

    for (i = 0; i < 2; i++) {
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0,
                                    ((globalCtx->gameplayFrames + i) * -20) & 0x1FF, 32, 128));

        Matrix_Translate((i == 0) ? 260.0f : -260.0f, 128.0f, 690.0f, MTXMODE_NEW);
        Matrix_RotateY(angle, MTXMODE_APPLY);
        Matrix_Scale(sTorchFlameScale, sTorchFlameScale, sTorchFlameScale, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

void BgSpot00Hanebasi_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Vec3f basePos = { 158.0f, 10.0f, 400.0f };
    Vec3f newPos;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    func_80093D18(globalCtx->state.gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (thisx->params == DT_DRAWBRIDGE) {
        gSPDisplayList(POLY_OPA_DISP++, gHyruleFieldCastleDrawbridgeDL);

        Matrix_MultVec3f(&basePos, &newPos);
        thisx->child->world.pos.x = newPos.x;
        thisx->child->world.pos.y = newPos.y;
        thisx->child->world.pos.z = newPos.z;
        basePos.x *= -1.0f;

        Matrix_MultVec3f(&basePos, &newPos);
        thisx->child->child->world.pos.x = newPos.x;
        thisx->child->child->world.pos.y = newPos.y;
        thisx->child->child->world.pos.z = newPos.z;

        if (gSaveContext.sceneSetupIndex != 12) {
            if ((gSaveContext.sceneSetupIndex >= 4) || (!LINK_IS_ADULT && (thisx->shape.rot.x < -0x2000))) {
                BgSpot00Hanebasi_DrawTorches(thisx, globalCtx);
            } else {
                sTorchFlameScale = 0.0f;
            }
        }
    } else {
        gSPDisplayList(POLY_OPA_DISP++, gHyruleFieldCastleDrawbridgeChainsDL);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}
