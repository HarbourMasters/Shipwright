/*
 * File: z_dm_char01.c
 * Overlay: ovl_Dm_Char01
 * Description: Woodfall scene objects (temple, water, walls, etc)
 */

#include "prevent_bss_reordering.h"
#include "z_dm_char01.h"
#include "objects/object_mtoride/object_mtoride.h"
#include "overlays/actors/ovl_Obj_Etcetera/z_obj_etcetera.h"
#include "2s2h/BenPort.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void DmChar01_Init(Actor* thisx, PlayState* play);
void DmChar01_Destroy(Actor* thisx, PlayState* play);
void DmChar01_Update(Actor* thisx, PlayState* play2);
void DmChar01_Draw(Actor* thisx, PlayState* play);

void func_80AA8698(DmChar01* this, PlayState* play);
void func_80AA884C(DmChar01* this, PlayState* play);
void func_80AA88A8(DmChar01* this, PlayState* play);
void func_80AA892C(DmChar01* this, PlayState* play);
void func_80AA8C28(DmChar01* this, PlayState* play);
void func_80AA8F1C(DmChar01* this, PlayState* play);
void func_80AA8F2C(DmChar01* this, PlayState* play);
void func_80AA9020(DmChar01* this, PlayState* play);
void func_80AA90AC(DmChar01* this, PlayState* play);
void func_80AA90F4(DmChar01* this, PlayState* play);

s16 D_80AAAE20;
s16 D_80AAAE22;
s16 D_80AAAE24;
s16 D_80AAAE26;

#include "overlays/ovl_Dm_Char01/ovl_Dm_Char01.h"

ActorProfile Dm_Char01_Profile = {
    ACTOR_DM_CHAR01,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_MTORIDE,
    sizeof(DmChar01),
    (ActorFunc)DmChar01_Init,
    (ActorFunc)DmChar01_Destroy,
    (ActorFunc)DmChar01_Update,
    (ActorFunc)DmChar01_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 300, ICHAIN_STOP),
};

s16 D_80AAAAB4 = false;
Vtx* gWoodfallSceneryDynamicPoisonWaterVtxData;

void DmChar01_Init(Actor* thisx, PlayState* play) {
    DmChar01* this = (DmChar01*)thisx;
    s32 i;
    gWoodfallSceneryDynamicPoisonWaterVtxData = ResourceMgr_LoadVtxByName(gWoodfallSceneryDynamicPoisonWaterVtx);

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    Actor_SetScale(&this->dyna.actor, 1.0f);

    this->unk_346 = 0;
    this->unk_34D = false;
    D_80AAAE24 = 0;
    D_80AAAE26 = 0;

    switch (DMCHAR01_GET(&this->dyna.actor)) {
        case DMCHAR01_0:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)) {
                this->unk_34C = 2;
                this->actionFunc = func_80AA8F1C;
                break;
            }

            if (gSaveContext.sceneLayer == 0) {
                play->envCtx.lightConfig = 5;
                play->envCtx.changeLightNextConfig = 5;
            }
            this->unk_348 = 255.0f;

            for (i = 0; i < ARRAY_COUNT(this->unk_1AC); i++) {
                this->unk_1AC[i] = gWoodfallSceneryDynamicPoisonWaterVtxData[i].v.ob[1] * 409.6f;
            }

            DynaPolyActor_Init(&this->dyna, 0);
            DynaPolyActor_LoadMesh(play, &this->dyna, &gWoodfallSceneryPoisonWaterDamageCol);

            this->unk_34D = true;
            if (gSaveContext.sceneLayer == 1) {
                this->unk_34C = 1;
                this->actionFunc = func_80AA8C28;
            } else {
                this->unk_34C = 0;
                this->actionFunc = func_80AA892C;
            }
            break;

        case DMCHAR01_1:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE) || (gSaveContext.sceneLayer == 1)) {
                this->unk_34C = 1;
                this->actionFunc = func_80AA8F1C;
            } else {
                this->actionFunc = func_80AA8698;
            }
            break;

        case DMCHAR01_2:
            this->unk_34C = 0;
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_20_01)) {
                this->unk_34C = 1;
                this->dyna.actor.world.pos.y -= 400.0f;
            }
            this->dyna.actor.world.rot.y += 0x8000;
            this->dyna.actor.shape.rot.y += 0x8000;
            DynaPolyActor_Init(&this->dyna, 0);
            DynaPolyActor_LoadMesh(play, &this->dyna, &gWoodfallSceneryTempleCol);
            this->unk_34D = true;
            this->unk_348 = 200.0f;
            this->actionFunc = func_80AA8F2C;
            break;

        case DMCHAR01_3:
            this->dyna.actor.world.rot.y += 0x8000;
            this->dyna.actor.shape.rot.y += 0x8000;
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_20_01)) {
                Actor_Kill(&this->dyna.actor);
                return;
            }

            Actor_Spawn(&play->actorCtx, play, ACTOR_OBJ_ETCETERA, 5.0f, 202.0f, 294.0f, 0, 0, 0,
                        DEKU_FLOWER_PARAMS(DEKU_FLOWER_TYPE_PINK_WITH_INITIAL_BOUNCE));
            DynaPolyActor_Init(&this->dyna, 0);
            DynaPolyActor_LoadMesh(play, &this->dyna, &gWoodfallSceneryTempleRampAndPlatformCol);

            this->unk_34D = true;
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)) {
                this->actionFunc = func_80AA9020;
                this->dyna.actor.world.pos.y -= 120.0f;
            } else {
                this->actionFunc = func_80AA8F1C;
            }
            break;

        default:
            this->actionFunc = func_80AA88A8;
            break;
    }
}

void DmChar01_Destroy(Actor* thisx, PlayState* play) {
    DmChar01* this = (DmChar01*)thisx;

    if (this->unk_34D) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void func_80AA8698(DmChar01* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Player* player2 = GET_PLAYER(play);

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_20_01)) {
        return;
    }

    if ((player->stateFlags2 & PLAYER_STATE2_USING_OCARINA) && (player2->actor.world.pos.x > -40.0f) &&
        (player2->actor.world.pos.x < 40.0f) && (player2->actor.world.pos.z > 1000.0f) &&
        (player2->actor.world.pos.z < 1078.0f)) {
        if (!D_80AAAAB4) {
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
            D_80AAAAB4 = true;
        }
    } else {
        D_80AAAAB4 = false;
    }

    if (GameInteractor_Should(VB_OPEN_WOODFALL_FROM_SONG,
                              (player->transformation == PLAYER_FORM_DEKU) &&
                                  (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) &&
                                  (play->msgCtx.lastPlayedSong == OCARINA_SONG_SONATA),
                              this)) {

        if ((player2->actor.world.pos.x > -40.0f) && (player2->actor.world.pos.x < 40.0f) &&
            (player2->actor.world.pos.z > 1000.0f) && (player2->actor.world.pos.z < 1078.0f)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_20_01);
            this->actionFunc = func_80AA884C;
        }
    }
}

void func_80AA884C(DmChar01* this, PlayState* play) {
    s16 csId = this->dyna.actor.csId;

    if (CutsceneManager_IsNext(csId)) {
        CutsceneManager_Start(csId, &this->dyna.actor);
        this->actionFunc = func_80AA88A8;
    } else {
        CutsceneManager_Queue(csId);
    }
}

void func_80AA88A8(DmChar01* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_135)) {
        if (play->csCtx.curFrame ==
            play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_135)]->startFrame) {
            D_80AAAE24 = 1;
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_FORT_RISING);
        }
    } else {
        D_80AAAE24 = 0;
    }
}

void func_80AA892C(DmChar01* this, PlayState* play) {
    f32 temp_f18;
    s32 temp_v0_4;
    s32 i;

    switch (this->unk_346) {
        case 0:
            D_80AAAE22 = -2200;
            D_80AAAE20 = 380;
            this->unk_346++;
            break;

        case 1:
            if (D_80AAAE24 != 0) {
                this->unk_34C = 1;
                if (D_80AAAE22 > -2000) {
                    D_80AAAE26 = 1;
                }

                D_80AAAE22 += 40;
                if (D_80AAAE22 > 3400) {
                    if (D_80AAAE20 > 100) {
                        D_80AAAE20 -= 230;
                    }
                    this->unk_346++;
                }
            }
            break;

        case 2:
            D_80AAAE22 -= 40;
            if (D_80AAAE22 <= -2200) {
                if (D_80AAAE20 > 100) {
                    D_80AAAE20 -= 70;
                }
                this->unk_346--;
            }
            break;
    }

    if (D_80AAAE24 == 0) {
        this->unk_34C = 0;
    }

    for (i = 0; i < ResourceMgr_GetVtxArraySizeByName(gWoodfallSceneryDynamicPoisonWaterVtx); i++) {
        s32 temp_s2 = sqrtf(SQ((f32)gWoodfallSceneryDynamicPoisonWaterVtxData[i].v.ob[2]) +
                            SQ((f32)gWoodfallSceneryDynamicPoisonWaterVtxData[i].v.ob[0]));
        f32 cos = Math_CosS((temp_s2 / 1892.0f) * 0x4000);
        f32 temp_f20 = (1.0f - (ABS_ALT(temp_s2 - D_80AAAE22) / 1892.0f)) * D_80AAAE20 * cos;

        if (temp_f20 < 0.0f) {
            temp_f20 = 0.0f;
        }

        temp_f18 = Math_SinS(this->unk_1AC[i]) * 15.0f;
        temp_f20 += temp_f18;

        this->unk_1AC[i] += 1600;
        gWoodfallSceneryDynamicPoisonWaterVtxData[i].v.ob[1] = temp_f20;
    }
}

void func_80AA8C28(DmChar01* this, PlayState* play) {
    f32 temp_f18;
    s32 temp_v0_4;
    s32 i;

    switch (this->unk_346) {
        case 0:
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WATER_PURIFICATION);
            D_80AAAE22 = -2200;
            D_80AAAE20 = 100;
            this->unk_346++;
            break;

        case 1:
            D_80AAAE22 += 40;
            if (D_80AAAE22 > 3400) {
                this->unk_346++;
            }
            break;

        case 2:
            D_80AAAE22 -= 40;
            if (D_80AAAE22 <= -2200) {
                this->unk_346--;
            }
            break;
    }

    for (i = 0; i < ResourceMgr_GetVtxArraySizeByName(gWoodfallSceneryDynamicPoisonWaterVtx); i++) {
        s32 temp_s2 = sqrtf(SQ((f32)gWoodfallSceneryDynamicPoisonWaterVtxData[i].v.ob[2]) +
                            SQ((f32)gWoodfallSceneryDynamicPoisonWaterVtxData[i].v.ob[0]));
        f32 cos = Math_CosS((temp_s2 / 1892.0f) * 0x4000);
        f32 temp_f20 = (1.0f - (ABS_ALT(temp_s2 - D_80AAAE22) / 1892.0f)) * D_80AAAE20 * cos;

        if (temp_f20 < 0.0f) {
            temp_f20 = 0.0f;
        }

        temp_f18 = Math_SinS(this->unk_1AC[i]) * 15.0f;
        temp_f20 += temp_f18;

        this->unk_1AC[i] += 1600;
        gWoodfallSceneryDynamicPoisonWaterVtxData[i].v.ob[1] = temp_f20;
    }

    Math_SmoothStepToF(&this->unk_348, 0.0f, 0.02f, 0.6f, 0.4f);

    if (this->unk_348 < 0.01f) {
        this->unk_34C = 2;
        this->actionFunc = func_80AA8F1C;
    }
}

void func_80AA8F0C(DmChar01* this, PlayState* play) {
}

void func_80AA8F1C(DmChar01* this, PlayState* play) {
}

void func_80AA8F2C(DmChar01* this, PlayState* play) {
    if (D_80AAAE26 != 0) {
        Math_SmoothStepToF(&this->dyna.actor.world.pos.y, 0.0f, 0.05f, 6.0f, 0.001f);
        Math_SmoothStepToF(&this->unk_348, 0.0f, 0.01f, 0.5f, 0.4f);
        if ((s32)this->dyna.actor.world.pos.y >= 0) {
            D_80AAAE26 = 2;
            Actor_Spawn(&play->actorCtx, play, ACTOR_OBJ_ETCETERA, 5.0f, 202.0f, 294.0f, 0, 0, 0,
                        DEKU_FLOWER_PARAMS(DEKU_FLOWER_TYPE_PINK_WITH_INITIAL_BOUNCE));
            this->actionFunc = func_80AA90F4;
        }
    }
    func_80AA8F0C(this, play);
}

void func_80AA9020(DmChar01* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_135)) {
        CsCmdActorCue* cue = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_135)];

        if ((cue->startFrame == play->csCtx.curFrame) && (cue->id == 2)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE);
            this->actionFunc = func_80AA90AC;
        }
    }
}

void func_80AA90AC(DmChar01* this, PlayState* play) {
    Math_SmoothStepToF(&this->dyna.actor.world.pos.y, 0.0f, 0.05f, 2.0f, 0.001f);
}

void func_80AA90F4(DmChar01* this, PlayState* play) {
    Math_SmoothStepToF(&this->unk_348, 0.0f, 0.02f, 0.8f, 0.4f);
}

void DmChar01_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DmChar01* this = (DmChar01*)thisx;

    this->actionFunc(this, play);

    if (this->unk_34D) {
        if (DMCHAR01_GET(&this->dyna.actor) == DMCHAR01_0) {
            Player* player = GET_PLAYER(play);

            if (player->actor.world.pos.y > 5.0f) {
                DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
            } else {
                DynaPoly_EnableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
            }
        }

        if (DMCHAR01_GET(&this->dyna.actor) == DMCHAR01_2) {
            if (this->dyna.actor.xzDistToPlayer > 600.0f) {
                DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
            } else {
                DynaPoly_EnableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
            }
        }
    }
}

void DmChar01_Draw(Actor* thisx, PlayState* play) {
    static f32 D_80AAAAB8 = 0.0f;
    static f32 D_80AAAABC = 0.0f;
    static s16 D_80AAAAC0 = 0;
    static s16 D_80AAAAC4 = 0;
    static s16 D_80AAAAC8 = 0;
    static s16 D_80AAAACC = 0;
    DmChar01* this = (DmChar01*)thisx;
    f32 temp_f12;
    f32 spBC;
    s32 i;
    u8 spB7 = false;

    switch (DMCHAR01_GET(thisx)) {
        case DMCHAR01_0:
            switch (this->unk_34C) {
                case 0:
                    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gWoodfallSceneryPoisonWaterTexAnim));
                    Gfx_DrawDListOpa(play, gWoodfallSceneryPoisonWaterDL);
                    break;

                case 1:
                    if (gSaveContext.sceneLayer == 1) {
                        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gWoodfallSceneryPurifiedWaterTexAnim));
                        Gfx_DrawDListOpa(play, gWoodfallSceneryFloorDL);
                        Gfx_DrawDListXlu(play, gWoodfallSceneryPurifiedWaterDL);
                        Matrix_Translate(0.0f, 10.0f, 0.0f, MTXMODE_APPLY);
                    }
                    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gWoodfallSceneryDynamicPoisonWaterTexAnim));

                    OPEN_DISPS(play->state.gfxCtx);

                    if ((u8)this->unk_348 == 255) {
                        Gfx_SetupDL25_Opa(play->state.gfxCtx);

                        gDPSetRenderMode(POLY_OPA_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
                        gDPPipeSync(POLY_OPA_DISP++);
                        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x96, 255, 255, 255, 255);
                        gSPSegment(POLY_OPA_DISP++, 0x0B,
                                   Lib_SegmentedToVirtual(gWoodfallSceneryDynamicPoisonWaterVtxData));
                        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                        gSPDisplayList(POLY_OPA_DISP++, gWoodfallSceneryDynamicPoisonWaterDL);
                    } else {
                        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

                        gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
                        gDPPipeSync(POLY_XLU_DISP++);
                        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, (u8)this->unk_348);
                        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x96, 255, 255, 255, (u8)this->unk_348);
                        gSPSegment(POLY_XLU_DISP++, 0x0B,
                                   Lib_SegmentedToVirtual(gWoodfallSceneryDynamicPoisonWaterVtxData));
                        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                        gSPDisplayList(POLY_XLU_DISP++, gWoodfallSceneryDynamicPoisonWaterDL);
                    }

                    CLOSE_DISPS(play->state.gfxCtx);
                    break;

                case 2:
                    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gWoodfallSceneryPurifiedWaterTexAnim));
                    Gfx_DrawDListOpa(play, gWoodfallSceneryFloorDL);
                    Gfx_DrawDListXlu(play, gWoodfallSceneryPurifiedWaterDL);
                    break;
            }
            break;

        case DMCHAR01_1:
            switch (this->unk_34C) {
                case 0:
                    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gWoodfallSceneryPoisonWallsTexAnim));
                    Gfx_DrawDListOpa(play, gWoodfallSceneryPoisonWallsDL);
                    break;

                case 1:
                    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gWoodfallSceneryPurifiedWallsTexAnim));
                    Gfx_DrawDListOpa(play, gWoodfallSceneryPurifiedWallsDL);
                    break;
            }
            break;

        case DMCHAR01_2:
            AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gWoodfallSceneryTempleTexAnim));
            Gfx_DrawDListOpa(play, gWoodfallSceneryTempleDL);

            if ((this->unk_34C != 0) && ((u8)this->unk_348 != 0)) {
                AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gWoodfallSceneryWaterFlowingOverTempleTexAnim));

                OPEN_DISPS(play->state.gfxCtx);

                Gfx_SetupDL25_Xlu(play->state.gfxCtx);

                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, (u8)this->unk_348);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, (u8)this->unk_348);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gWoodfallSceneryWaterFlowingOverTempleDL);

                CLOSE_DISPS(play->state.gfxCtx);
            }

            if (D_80AAAE24 != 0) {
                if ((D_80AAAE22 > -1800) && (D_80AAAE22 < 3000)) {
                    temp_f12 = D_80AAAE22 - 640.0f;
                    if ((D_80AAAE20 == 380) && (D_80AAAE22 > 640)) {
                        D_80AAAAC0 = 2;
                        D_80AAAAC4 = 0;
                        D_80AAAAC8 = 900;
                        D_80AAAACC = 700;
                        spB7 = true;
                        if (D_80AAAE22 < 1350) {
                            f32 temp_f0 = temp_f12 / 2000.0f;

                            D_80AAAAB8 = 420.0f - (420.0f * temp_f0);
                            D_80AAAABC = (200.0f * temp_f0) + 200.0f;
                        } else {
                            f32 temp_f0 = temp_f12 / 2000.0f;

                            D_80AAAAB8 = 420.0f - (420.0f * temp_f0);
                            D_80AAAABC = 400.0f;
                        }
                    }
                }

                if (spB7) {
                    for (i = 0; i < D_80AAAAC0 * 2; i++) {
                        Vec3f sp44;
                        f32 phi_f2 = D_80AAAABC;
                        s16 temp;

                        spBC = Rand_ZeroOne() * D_80AAAAC8;
                        if ((play->state.frames % 2) != 0) {
                            sp44.x = (Rand_ZeroOne() - 0.5f) * (2.0f * phi_f2);
                            sp44.y = D_80AAAAB8;
                            sp44.z = (Rand_ZeroOne() * D_80AAAAC4) + phi_f2;
                            temp = TRUNCF_BINANG(spBC) + D_80AAAACC;
                            EffectSsGSplash_Spawn(play, &sp44, NULL, NULL, 0, temp);
                        } else {
                            sp44.x = -phi_f2 - (Rand_ZeroOne() * D_80AAAAC4);
                            sp44.y = D_80AAAAB8;
                            sp44.z = (Rand_ZeroOne() - 0.5f) * (2.0f * phi_f2);
                            temp = TRUNCF_BINANG(spBC) + D_80AAAACC;
                            EffectSsGSplash_Spawn(play, &sp44, NULL, NULL, 0, temp);
                        }
                    }
                }
            }

            Gfx_DrawDListXlu(play, gWoodfallSceneryTempleEntrancesDL);
            break;

        case DMCHAR01_3:
            if (thisx->world.pos.y > -120.0f) {
                Gfx_DrawDListOpa(play, gWoodfallSceneryTempleRampAndPlatformDL);
            }
            break;
    }
}
