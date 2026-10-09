/*
 * File: z_en_test7.c
 * Overlay: ovl_En_Test7
 * Description: Soaring effects (wings, sphere, etc)
 */

#include "prevent_bss_reordering.h"
#include "z_en_test7.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS                                                                                              \
    (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_FREEZE_EXCEPTION | \
     ACTOR_FLAG_UPDATE_DURING_SOARING_AND_SOT_CS | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnTest7_Init(Actor* thisx, PlayState* play2);
void EnTest7_Destroy(Actor* thisx, PlayState* play);
void EnTest7_Update(Actor* thisx, PlayState* play);
void EnTest7_Draw(Actor* thisx, PlayState* play);

void EnTest7_StartWarpCs(EnTest7* this, PlayState* play);
void EnTest7_WarpCsPart1(EnTest7* this, PlayState* play);
void EnTest7_WarpCsPart2(EnTest7* this, PlayState* play);
void EnTest7_WarpCsPart3(EnTest7* this, PlayState* play);
void EnTest7_WarpCsPart4(EnTest7* this, PlayState* play);
void EnTest7_WarpCsPart5(EnTest7* this, PlayState* play);
void EnTest7_WarpCsPart6(EnTest7* this, PlayState* play);
void EnTest7_WarpCsWait(EnTest7* this, PlayState* play);
void EnTest7_WarpCsWarp(EnTest7* this, PlayState* play);

void EnTest7_PlayerAndSubCamAction(EnTest7* this, PlayState* play);

void EnTest7_SetupArriveCs(EnTest7* this, PlayState* play);
void EnTest7_StartArriveCs(EnTest7* this, PlayState* play);
void EnTest7_ArriveCsPart1(EnTest7* this, PlayState* play);
void EnTest7_StartArriveCsSkip(EnTest7* this, PlayState* play);
void EnTest7_ArriveCsPart2(EnTest7* this, PlayState* play);
void EnTest7_ArriveCsPart3(EnTest7* this, PlayState* play);

ActorProfile En_Test7_Profile = {
    /**/ ACTOR_EN_TEST7,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnTest7),
    /**/ EnTest7_Init,
    /**/ EnTest7_Destroy,
    /**/ EnTest7_Update,
    /**/ EnTest7_Draw,
};

void EnTest7_SetupPlayerCamFunc(EnTest7* this, EnTest7PlayerCamFunc playerCamFunc) {
    this->playerCamFunc = playerCamFunc;
}

void EnTest7_SetupAction(EnTest7* this, EnTest7ActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnTest7_InitFeathers(OwlWarpFeather* feathers) {
    OwlWarpFeather* feather = feathers;
    s32 i;

    for (i = 0, feather = feathers; i < OWL_WARP_NUM_FEATHERS; i++, feather++) {
        s32 pad;

        feather->type = OWL_WARP_FEATHER_TYPE_DISABLED;
        feather->unk_04 = 0;
        feather->pos.x = 0.0f;
        feather->pos.y = 0.0f;
        feather->pos.z = 0.0f;
        feather->velocity.x = 0.0f;
        feather->velocity.y = 0.0f;
        feather->velocity.z = 0.0f;
        feather->accel.x = 0.0f;
        feather->accel.y = 0.0f;
        feather->accel.z = 0.0f;
        feather->scale = 0.00001f;
        feather->rot.x = 0;
        feather->rot.y = 0;
        feather->rot.z = 0;
        feather->angularVelocity.x = 0;
        feather->angularVelocity.y = 0;
        feather->angularVelocity.z = 0;
    }
}

void EnTest7_AddFeather(OwlWarpFeather* feather, Vec3f* pos, s32 isPosYRand) {
    s16 yaw = Rand_ZeroOne() * 0xFFFF;
    f32 velocityXZ;

    feather->pos = *pos;

    if (isPosYRand) {
        feather->pos.x += Math_CosS(yaw) * 0.0f;
        feather->pos.y += (Rand_ZeroOne() * 100.0f) - 20.0f;
        feather->pos.z += Math_SinS(yaw) * 0.0f;
    }

    velocityXZ = (Rand_ZeroOne() * 4.0f) + 2.0f;

    feather->velocity.x = Math_CosS(yaw) * velocityXZ;
    feather->velocity.y = Rand_ZeroOne();
    feather->velocity.z = Math_SinS(yaw) * velocityXZ;

    feather->accel.x = 0.0f;
    feather->accel.y = 0.0f;
    feather->accel.z = 0.0f;

    feather->scale = 0.25f;
    feather->rot.x = Rand_ZeroOne() * 0x10000;
    feather->rot.y = Rand_ZeroOne() * 0x10000;
    feather->rot.z = Rand_ZeroOne() * 0x10000;
    feather->unk_04 = 60;

    if (Rand_ZeroOne() < 0.9f) {
        feather->type = OWL_WARP_FEATHER_TYPE_1;
        feather->rot.x = Rand_ZeroOne() * 0x10000;
        feather->rot.y = Rand_ZeroOne() * 0x10000;
        feather->rot.z = Rand_ZeroOne() * 0x10000;
        feather->angularVelocity.x = 0;
        feather->angularVelocity.y = 0;
        feather->angularVelocity.z = 0;
    } else {
        feather->type = OWL_WARP_FEATHER_TYPE_2;
        feather->rot.x = 0;
        feather->rot.y = 0;
        feather->rot.z = Rand_ZeroOne() * 5000.0f;
        feather->angularVelocity.x = 0;
        feather->angularVelocity.y = (Rand_ZeroOne() * 8000.0f) + 2000.0f;
        if (Rand_ZeroOne() > 0.5f) {
            feather->angularVelocity.y = -feather->angularVelocity.y;
        }
        feather->angularVelocity.z = 0;
    }
}

void EnTest7_AddAndChooseFeather(OwlWarpFeather* feathers, Vec3f* pos, s32 isPosYRand) {
    static s32 sFeatherIndex = 0;
    s32 i;
    s32 featherAdded = false;
    OwlWarpFeather* feather;
    s32 featherIndex;

    for (i = 0, feather = feathers; i < OWL_WARP_NUM_FEATHERS; i++, feather++) {
        if (feather->type == OWL_WARP_FEATHER_TYPE_DISABLED) {
            EnTest7_AddFeather(feather, pos, isPosYRand);
            sFeatherIndex = i;
            featherAdded = true;
            break;
        }
    }

    if (!featherAdded) {
        featherIndex = sFeatherIndex + 1;
        if (featherIndex >= OWL_WARP_NUM_FEATHERS) {
            featherIndex = 0;
        }

        EnTest7_AddFeather(&feathers[featherIndex], pos, isPosYRand);
    }
}

void EnTest7_UpdateFeatherType1(PlayState* play, OwlWarpFeather* feather) {
    static Vec3f sUnitVecY = { 0.0f, 1.0f, 0.0f };
    static Vec3f sUnitVecZ = { 0.0f, 0.0f, 1.0f };
    static MtxF sFeatherRotMf;
    static Vec3f sFeatherAccel;
    static Vec3f sFeatherUnused;
    s32 swayAngleX = feather->unk_04 % 41;
    s32 swayAngleY = (feather->unk_04 + 7000) % 41;
    s32 sp24 = false;

    SkinMatrix_SetRotateRPY(&sFeatherRotMf, feather->rot.x, feather->rot.y, feather->rot.z);
    SkinMatrix_Vec3fMtxFMultXYZ(&sFeatherRotMf, &sUnitVecY, &sFeatherAccel);
    SkinMatrix_Vec3fMtxFMultXYZ(&sFeatherRotMf, &sUnitVecZ, &sFeatherUnused);

    if (feather->rot.x < 0x3448) {
        feather->rot.x += 0x384;
    } else if (feather->rot.x >= 0x4BB9) {
        feather->rot.x -= 0x384;
    } else {
        feather->rot.x = (Math_SinS(((f32)swayAngleX * 0xFFFF) / 41.0f) * 2000.0f) + 0x4000;
    }

    if (feather->rot.y < -0xBB8) {
        feather->rot.y += 0x384;
    } else if (feather->rot.y > 0xBB8) {
        feather->rot.y -= 0x384;
    } else {
        sp24 = true;
        feather->rot.y = Math_SinS(((f32)swayAngleY * 0xFFFF) / 41.0f) * 2000.0f;
    }

    if (sp24 == true) {
        if (sFeatherAccel.y < 0.0f) {
            feather->velocity.x += sFeatherAccel.x * 0.5f;
            feather->velocity.y += sFeatherAccel.y * 0.5f + 0.08f;
            feather->velocity.z += sFeatherAccel.z * 0.5f;
        } else {
            feather->velocity.x += -sFeatherAccel.x * 0.5f;
            feather->velocity.y += -sFeatherAccel.y * 0.5f + 0.08f;
            feather->velocity.z += -sFeatherAccel.z * 0.5f;
        }
    } else if (sFeatherAccel.y < 0.0f) {
        feather->velocity.x += sFeatherAccel.x * 0.2f;
        feather->velocity.y += sFeatherAccel.y * 0.2f + 0.08f;
        feather->velocity.z += sFeatherAccel.z * 0.2f;
    } else {
        feather->velocity.x += -sFeatherAccel.x * 0.2f;
        feather->velocity.y += -sFeatherAccel.y * 0.2f + 0.08f;
        feather->velocity.z += -sFeatherAccel.z * 0.2f;
    }

    feather->pos.x += feather->velocity.x;
    feather->pos.y += feather->velocity.y;
    feather->pos.z += feather->velocity.z;
}

void EnTest7_UpdateFeatherType2(PlayState* play, OwlWarpFeather* feather) {
    feather->rot.y += feather->angularVelocity.y;

    feather->accel.x = Rand_Centered();
    feather->accel.y = Rand_Centered() + -0.01f;
    feather->accel.z = Rand_Centered();

    feather->velocity.x += feather->accel.x;
    feather->velocity.y += feather->accel.y;
    feather->velocity.z += feather->accel.z;

    feather->pos.x += feather->velocity.x;
    feather->pos.y += feather->velocity.y;
    feather->pos.z += feather->velocity.z;
}

void EnTest7_UpdateFeathers(PlayState* play, OwlWarpFeather* feathers, EnTest7* this, s32 arg3, s32 arg4) {
    s32 pad;
    s32 i;
    f32 temp_f28;
    f32 temp_f0;
    f32 temp_f22;
    f32 temp_f24;
    f32 temp_f26;
    f32 temp_f2;
    Vec3f sp8C;
    OwlWarpFeather* feather;
    s16 phi_s1;

    for (i = 0; i < OWL_WARP_NUM_FEATHERS; i++) {
        feather = &feathers[i];

        if (feather->type == OWL_WARP_FEATHER_TYPE_DISABLED) {
            continue;
        }

        if (feather->type == OWL_WARP_FEATHER_TYPE_1) {
            EnTest7_UpdateFeatherType1(play, feather);
        } else {
            EnTest7_UpdateFeatherType2(play, feather);
        }

        if (arg3) {
            temp_f22 = feather->pos.x - this->actor.world.pos.x;
            temp_f24 = feather->pos.z - this->actor.world.pos.z;
            temp_f0 = SQ(temp_f22) + SQ(temp_f24);

            phi_s1 = -0x2710;
            if (temp_f0 > SQ(20.0f)) {
                phi_s1 /= ((temp_f0 - SQ(20.0f)) * 0.00125f) + 1.0f;
            }

            temp_f26 = (temp_f22 * Math_CosS(phi_s1)) - (temp_f24 * Math_SinS(phi_s1));
            temp_f28 = (temp_f22 * Math_SinS(phi_s1)) + (temp_f24 * Math_CosS(phi_s1));

            feather->pos.x = this->actor.world.pos.x + temp_f26;
            feather->pos.z = this->actor.world.pos.z + temp_f28;

            temp_f22 = feather->velocity.x;
            temp_f24 = feather->velocity.z;

            feather->velocity.x = (temp_f22 * Math_CosS(phi_s1)) - (temp_f24 * Math_SinS(phi_s1));
            feather->velocity.z = (temp_f22 * Math_SinS(phi_s1)) + (temp_f24 * Math_CosS(phi_s1));

            temp_f22 = feather->accel.x;
            temp_f24 = feather->accel.z;

            feather->accel.x = (temp_f22 * Math_CosS(phi_s1)) - (temp_f24 * Math_SinS(phi_s1));
            feather->accel.z = (temp_f22 * Math_SinS(phi_s1)) + (temp_f24 * Math_CosS(phi_s1));
        }

        if (arg4) {
            sp8C.x = feather->pos.x - this->actor.world.pos.x;
            sp8C.y = feather->pos.y - (this->actor.world.pos.y + 40.0f);
            sp8C.z = feather->pos.z - this->actor.world.pos.z;

            temp_f2 = 1.0f - (0.5f / ((Math3D_Vec3fMagnitude(&sp8C) / 500.0f) + 1.0f));

            sp8C.x *= temp_f2;
            sp8C.y *= temp_f2;
            sp8C.z *= temp_f2;

            feather->pos.x = this->actor.world.pos.x + sp8C.x;
            feather->pos.y = this->actor.world.pos.y + sp8C.y + 40.0f;
            feather->pos.z = this->actor.world.pos.z + sp8C.z;
        }
    }
}

void EnTest7_DrawFeathers(PlayState* play2, OwlWarpFeather* feathers) {
    PlayState* play = play2;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL26_Opa(play->state.gfxCtx);

    Matrix_Push();

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x80, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);

    for (i = 0; i < OWL_WARP_NUM_FEATHERS; i++) {
        Mtx* mtx;
        MtxF sp6C;
        OwlWarpFeather* feather = &feathers[i];

        if (feather->type == OWL_WARP_FEATHER_TYPE_DISABLED) {
            continue;
        }

        if ((feather->pos.x > 30000.0f) || (feather->pos.x < -30000.0f) || (feather->pos.y > 30000.0f) ||
            (feather->pos.y < -30000.0f) || (feather->pos.z > 30000.0f) || (feather->pos.z < -30000.0f)) {
            feather->type = OWL_WARP_FEATHER_TYPE_DISABLED;
            continue;
        }

        Matrix_Translate(feather->pos.x, feather->pos.y, feather->pos.z, MTXMODE_NEW);

        if (feather->type == OWL_WARP_FEATHER_TYPE_1) {
            Matrix_RotateZYX(feather->rot.x, feather->rot.y, feather->rot.z, MTXMODE_APPLY);
        } else {
            SkinMatrix_SetRotateYRP(&sp6C, feather->rot.x, feather->rot.y, feather->rot.z);
            Matrix_Mult(&sp6C, MTXMODE_APPLY);
        }

        Matrix_Scale(feather->scale, feather->scale, feather->scale, MTXMODE_APPLY);
        if (feather->type == OWL_WARP_FEATHER_TYPE_2) {
            Matrix_Translate(0.0f, 30.0f, 0.0f, MTXMODE_APPLY);
        }

        mtx = Matrix_Finalize(play->state.gfxCtx);
        if (mtx == NULL) {
            continue;
        }
        gSPMatrix(POLY_OPA_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_OPA_DISP++, gSoaringWarpCsFeatherDL);
    }

    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnTest7_InitWindCapsule(OwlWarpWindCapsule* windCapsule) {
    windCapsule->unk_00 = 0.0f;
    windCapsule->unk_04 = 0.0f;
    windCapsule->xzScale = 1.0f;
    windCapsule->yScale = 1.0f;
    windCapsule->yaw = 0;
}

void EnTest7_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTest7* this = (EnTest7*)thisx;
    Player* player = GET_PLAYER(play);
    Player* player2 = GET_PLAYER(play);

    this->actor.world.rot.y = this->actor.shape.rot.y = player->actor.shape.rot.y;
    this->flags = 0;
    this->timer = 0;
    this->playerYaw = player->actor.shape.rot.y;
    this->playerScaleX = player->actor.scale.x;
    this->playerScaleZ = player->actor.scale.z;

    Keyframe_InitFlex(&this->kfSkelAnime, gGameplayKeepKFSkel_85640, gGameplayKeepKFAnim_83534, this->jointTable,
                      this->morphTable, NULL);
    Keyframe_FlexPlayOnce(&this->kfSkelAnime, gGameplayKeepKFAnim_83534);

    EnTest7_InitFeathers(this->feathers);
    EnTest7_InitWindCapsule(&this->windCapsule);

    if (OWL_WARP_CS_GET_OCARINA_MODE(&this->actor) == ENTEST7_ARRIVE) {
        EnTest7_SetupAction(this, EnTest7_SetupArriveCs);
        EnTest7_SetupPlayerCamFunc(this, NULL);
    } else {
        EnTest7_SetupAction(this, EnTest7_StartWarpCs);
        EnTest7_SetupPlayerCamFunc(this, EnTest7_PlayerAndSubCamAction);
        Audio_PlayBgm_StorePrevBgm(NA_BGM_SONG_OF_SOARING);
    }

    if (play->playerCsIds[PLAYER_CS_ID_SONG_WARP] == CS_ID_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    CutsceneManager_Queue(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);
    player2->stateFlags1 |= PLAYER_STATE1_20;
    Lights_PointNoGlowSetInfo(&this->lightInfo, (Math_SinS(this->playerYaw) * 90.0f) + player->actor.world.pos.x,
                              player->actor.world.pos.y + 10.0f,
                              (Math_CosS(this->playerYaw) * 90.0f) + player->actor.world.pos.z, 255, 255, 255, 255);
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
}

void EnTest7_Destroy(Actor* thisx, PlayState* play) {
    EnTest7* this = (EnTest7*)thisx;

    CutsceneManager_Stop(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);
    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
}

void EnTest7_StartWarpCs(EnTest7* this, PlayState* play) {
    if (!CutsceneManager_IsNext(play->playerCsIds[PLAYER_CS_ID_SONG_WARP])) {
        CutsceneManager_Queue(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);
    } else {
        CutsceneManager_Start(play->playerCsIds[PLAYER_CS_ID_SONG_WARP], NULL);
        EnTest7_SetupAction(this, EnTest7_WarpCsPart1);
        play->soaringCsOrSoTCsPlaying = true;
    }
}

void EnTest7_WarpCsPart1(EnTest7* this, PlayState* play) {
    Color_RGB8 fogColor = { 64, 0, 0 };
    Color_RGB8 ambientColor = { 220, 220, 255 };
    f32 envLerp = this->timer / (f32)(10 - 0);

    Environment_LerpAmbientColor(play, &ambientColor, envLerp);
    Environment_LerpFogColor(play, &fogColor, envLerp);
    Environment_LerpFog(play, 2000, 4000, envLerp);

    if (this->timer >= 10) {
        Camera* subCam =
            Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]));

        this->subCamEye = subCam->eye;
        this->subCamAt = subCam->at;
        this->subCamFov = subCam->fov;

        EnTest7_SetupAction(this, EnTest7_WarpCsPart2);
        this->flags |= OWL_WARP_FLAGS_20;
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_PL_WARP_WING_OPEN);
        Play_EnableMotionBlur(120);
    }
}

void EnTest7_UpdateGrowingWindCapsule(EnTest7* this, PlayState* play) {
    this->flags |= OWL_WARP_FLAGS_DRAW_WIND_CAPSULE;

    if (this->windCapsule.unk_04 < 11.0f) {
        this->windCapsule.unk_04 += 0.3f;
        this->windCapsule.xzScale = ((this->windCapsule.unk_00 * -0.45f) / 11.0f) + 0.7f;
        this->windCapsule.yScale = ((this->windCapsule.unk_00 * -0.29999998f) / 11.0f) + 0.7f;
    }

    if (this->windCapsule.unk_00 < 11.0f) {
        this->windCapsule.unk_00 += 1.0f;
        if (this->windCapsule.unk_00 > 6.0f) {
            Player* player = GET_PLAYER(play);

            this->flags &= ~OWL_WARP_FLAGS_DRAW_WINGS;
            player->actor.draw = NULL;
        }
    } else if (this->timer > 86) {
        EnTest7_SetupAction(this, EnTest7_WarpCsPart4);
        this->flags &= ~OWL_WARP_FLAGS_8;
        this->windCapsule.yaw -= 0x2EE0;
    } else {
        this->windCapsule.yaw -= 0x2EE0;
    }
}

void EnTest7_WarpCsPart2(EnTest7* this, PlayState* play) {
    Vec3f featherPos;

    if (Keyframe_UpdateFlex(&this->kfSkelAnime)) {
        EnTest7_SetupAction(this, EnTest7_WarpCsPart3);
    }

    if (this->kfSkelAnime.frameCtrl.curTime > 60.0f) {
        EnTest7_UpdateGrowingWindCapsule(this, play);
    }

    if ((this->kfSkelAnime.frameCtrl.curTime > 20.0f) && !(this->flags & OWL_WARP_FLAGS_40)) {
        this->flags |= OWL_WARP_FLAGS_40;
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_PL_WARP_WING_CLOSE);
    }

    if (this->kfSkelAnime.frameCtrl.curTime > 42.0f) {
        if (!(this->flags & OWL_WARP_FLAGS_80)) {
            this->flags |= OWL_WARP_FLAGS_80;
            Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_PL_WARP_WING_ROLL);
        }

        if (Rand_ZeroOne() < 0.3f) {
            Camera* subCam =
                Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]));
            f32 randLerp = Rand_ZeroOne();

            featherPos.x = LERPIMP_ALT(this->actor.world.pos.x, subCam->eye.x, randLerp);
            featherPos.y = LERPIMP_ALT(this->actor.world.pos.y, subCam->eye.y, randLerp);
            featherPos.z = LERPIMP_ALT(this->actor.world.pos.z, subCam->eye.z, randLerp);

            EnTest7_AddAndChooseFeather(this->feathers, &featherPos, true);
            this->flags |= OWL_WARP_FLAGS_8;
        }
    } else {
        this->flags |= OWL_WARP_FLAGS_DRAW_WINGS;
    }
}

void EnTest7_WarpCsPart3(EnTest7* this, PlayState* play) {
    Vec3f featherPos;
    Camera* subCam;
    f32 randLerp;

    EnTest7_UpdateGrowingWindCapsule(this, play);

    if (Rand_ZeroOne() < 0.3f) {
        subCam = Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]));
        randLerp = Rand_ZeroOne();

        featherPos.x = LERPIMP_ALT(this->actor.world.pos.x, subCam->eye.x, randLerp);
        featherPos.y = LERPIMP_ALT(this->actor.world.pos.y, subCam->eye.y, randLerp);
        featherPos.z = LERPIMP_ALT(this->actor.world.pos.z, subCam->eye.z, randLerp);

        EnTest7_AddAndChooseFeather(this->feathers, &featherPos, true);
    }

    Math_Vec3f_Copy(&featherPos, &this->actor.world.pos);
    EnTest7_AddAndChooseFeather(this->feathers, &featherPos, true);
    this->jointTable[1].y += 0x2EE0;
}

void EnTest7_WarpCsPart4(EnTest7* this, PlayState* play) {
    s32 pad;
    s32 temp = this->timer - 86;
    f32 temp_f0 = temp / (f32)(96 - 86);
    Vec3f featherPos;

    this->flags |= OWL_WARP_FLAGS_10;

    this->windCapsule.xzScale = (-0.15f * temp_f0) + 0.25f;
    this->windCapsule.yScale = (-0.3f * temp_f0) + 0.4f;
    this->windCapsule.yaw -= 0x2EE0;

    this->flags |= OWL_WARP_FLAGS_DRAW_LENS_FLARE;

    if (this->timer >= 96) {
        EnTest7_SetupAction(this, EnTest7_WarpCsPart5);
        this->flags &= ~OWL_WARP_FLAGS_10;
    }
    Math_Vec3f_Copy(&featherPos, &this->actor.world.pos);
    EnTest7_AddAndChooseFeather(this->feathers, &featherPos, true);
}

void EnTest7_WarpCsPart5(EnTest7* this, PlayState* play) {
    s32 pad;
    s32 temp = this->timer - 96;
    f32 lerp = 1.0f - (temp / (f32)(100 - 96));
    Camera* subCam;
    f32 temp_f2;
    f32 temp_f4;

    this->windCapsule.xzScale = ((temp * -0.1f) / 4) + 0.1f;
    this->windCapsule.yScale = ((temp * 5.9f) / 4) + 0.1f;
    this->windCapsule.yaw -= 0x2EE0;

    this->actor.world.pos.y += 100.0f;

    subCam = Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]));
    subCam->focalActor = NULL;

    subCam->eye.x = LERPIMP_ALT(this->subCamEye.x, subCam->eye.x, lerp);
    subCam->eye.y = LERPIMP_ALT(this->subCamEye.y, subCam->eye.y, lerp);
    subCam->eye.z = LERPIMP_ALT(this->subCamEye.z, subCam->eye.z, lerp);

    subCam->fov = LERPIMP_ALT(this->subCamFov, subCam->fov, lerp);

    if (this->timer >= 100) {
        R_PLAY_FILL_SCREEN_ON = true;
        R_PLAY_FILL_SCREEN_R = 255;
        R_PLAY_FILL_SCREEN_G = 255;
        R_PLAY_FILL_SCREEN_B = 255;
        R_PLAY_FILL_SCREEN_ALPHA = 255;
        play->soaringCsOrSoTCsPlaying = false;
        this->flags &= ~OWL_WARP_FLAGS_DRAW_LENS_FLARE;
        EnTest7_SetupAction(this, EnTest7_WarpCsPart6);
        Play_DisableMotionBlur();
    }
}

void EnTest7_WarpCsPart6(EnTest7* this, PlayState* play) {
    s32 sp2C = this->timer - 100;
    f32 envLerp;
    Color_RGB8 fogColor = { 64, 0, 0 };
    Color_RGB8 ambientColor = { 220, 220, 255 };

    if (R_PLAY_FILL_SCREEN_ON) {
        Audio_PlaySfx_AtPos(&this->actor.projectedPos, NA_SE_PL_WARP_WING_VANISH);
        R_PLAY_FILL_SCREEN_ON = false;
        R_PLAY_FILL_SCREEN_R = 0;
        R_PLAY_FILL_SCREEN_G = 0;
        R_PLAY_FILL_SCREEN_B = 0;
        R_PLAY_FILL_SCREEN_ALPHA = 0;
    }

    envLerp = 1.0f - (sp2C / (f32)(110 - 100));
    Environment_LerpAmbientColor(play, &ambientColor, envLerp);
    Environment_LerpFogColor(play, &fogColor, envLerp);
    Environment_LerpFog(play, 2000, 4000, envLerp);

    if (this->timer >= 110) {
        EnTest7_SetupAction(this, EnTest7_WarpCsWait);
    }
}

void EnTest7_WarpCsWait(EnTest7* this, PlayState* play) {
    if (this->timer >= 130) {
        EnTest7_SetupAction(this, EnTest7_WarpCsWarp);
    }
}

static u16 sOwlWarpEntrances[OWL_WARP_MAX - 1] = {
    ENTRANCE(GREAT_BAY_COAST, 11),         // OWL_WARP_GREAT_BAY_COAST
    ENTRANCE(ZORA_CAPE, 6),                // OWL_WARP_ZORA_CAPE
    ENTRANCE(SNOWHEAD, 3),                 // OWL_WARP_SNOWHEAD
    ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 8),  // OWL_WARP_MOUNTAIN_VILLAGE
    ENTRANCE(SOUTH_CLOCK_TOWN, 9),         // OWL_WARP_CLOCK_TOWN
    ENTRANCE(MILK_ROAD, 4),                // OWL_WARP_MILK_ROAD
    ENTRANCE(WOODFALL, 4),                 // OWL_WARP_WOODFALL
    ENTRANCE(SOUTHERN_SWAMP_POISONED, 10), // OWL_WARP_SOUTHERN_SWAMP
    ENTRANCE(IKANA_CANYON, 4),             // OWL_WARP_IKANA_CANYON
    ENTRANCE(STONE_TOWER, 3),              // OWL_WARP_STONE_TOWER
};

void EnTest7_WarpCsWarp(EnTest7* this, PlayState* play) {
    Vec3f featherPos;

    //! FAKE:
    if (1) {}

    Math_Vec3f_Copy(&featherPos, &this->actor.world.pos);

    if (Rand_ZeroOne() < 0.1f) {
        EnTest7_AddAndChooseFeather(this->feathers, &featherPos, true);
    }

    this->windCapsule.yaw -= 0x2EE0;

    if (play->sceneId == SCENE_SECOM) {
        play->nextEntrance = ENTRANCE(IKANA_CANYON, 6);
    } else if (OWL_WARP_CS_GET_OCARINA_MODE(&this->actor) == OCARINA_MODE_WARP_TO_ENTRANCE) {
        func_80169F78(play);
        gSaveContext.respawn[RESPAWN_MODE_TOP].playerParams =
            PLAYER_PARAMS(gSaveContext.respawn[RESPAWN_MODE_TOP].playerParams, PLAYER_START_MODE_OWL);
        gSaveContext.respawnFlag = -6;
    } else {
        play->nextEntrance =
            sOwlWarpEntrances[OWL_WARP_CS_GET_OCARINA_MODE(&this->actor) - OCARINA_MODE_WARP_TO_GREAT_BAY_COAST];
        if ((play->nextEntrance == ENTRANCE(SOUTHERN_SWAMP_POISONED, 10)) &&
            CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)) {
            play->nextEntrance = ENTRANCE(SOUTHERN_SWAMP_CLEARED, 10);
        } else if ((play->nextEntrance == ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 8)) &&
                   CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE)) {
            play->nextEntrance = ENTRANCE(MOUNTAIN_VILLAGE_SPRING, 8);
        }
    }

    play->transitionTrigger = TRANS_TRIGGER_START;
    play->transitionType = TRANS_TYPE_FADE_BLACK;
    gSaveContext.seqId = NA_BGM_DISABLED;
    gSaveContext.ambienceId = AMBIENCE_ID_DISABLED;
}

void EnTest7_UpdateSubCamWarpCs2(EnTest7* this, PlayState* play, f32 lerp) {
    Vec3f eyeNext;
    Vec3f* playerPos;
    Player* player = GET_PLAYER(play);
    Camera* subCam =
        Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]));

    playerPos = &player->actor.world.pos;
    subCam->focalActor = NULL;

    eyeNext.x = playerPos->x + 180.0f * Math_SinS(this->playerYaw) * Math_CosS(0xFA0);
    eyeNext.y = playerPos->y + 180.0f * Math_SinS(0xFA0);
    eyeNext.z = playerPos->z + 180.0f * Math_CosS(this->playerYaw) * Math_CosS(0xFA0);

    subCam->eye.x = LERPIMP_ALT(subCam->eye.x, eyeNext.x, lerp);
    subCam->eye.y = LERPIMP_ALT(subCam->eye.y, eyeNext.y, lerp);
    subCam->eye.z = LERPIMP_ALT(subCam->eye.z, eyeNext.z, lerp);

    subCam->fov = LERPIMP_ALT(subCam->fov, this->subCamFov, lerp);

    subCam->at.y += 1.4444444f;
}

void EnTest7_UpdateSubCamWarpCs1(EnTest7* this, PlayState* play, f32 lerp) {
    Vec3f* playerPos;
    Player* player = GET_PLAYER(play);
    Camera* subCam =
        Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]));
    Vec3f eyeNext;

    subCam->focalActor = NULL;

    playerPos = &player->actor.world.pos;

    eyeNext.x = playerPos->x + 80.0f * Math_SinS(this->playerYaw) * Math_CosS(0xBB8);
    eyeNext.y = playerPos->y + 80.0f * Math_SinS(0xBB8);
    eyeNext.z = playerPos->z + 80.0f * Math_CosS(this->playerYaw) * Math_CosS(0xBB8);

    subCam->eye.x = LERPIMP_ALT(subCam->eye.x, eyeNext.x, lerp);
    subCam->eye.y = LERPIMP_ALT(subCam->eye.y, eyeNext.y, lerp);
    subCam->eye.z = LERPIMP_ALT(subCam->eye.z, eyeNext.z, lerp);

    subCam->at.x = LERPIMP_ALT(subCam->at.x, playerPos->x, lerp);
    subCam->at.y = LERPIMP_ALT(subCam->at.y, playerPos->y + 40.0f, lerp);
    subCam->at.z = LERPIMP_ALT(subCam->at.z, playerPos->z, lerp);

    subCam->fov = LERPIMP_ALT(subCam->fov, this->subCamFov, lerp);
}

void EnTest7_SpinAndSquishPlayer(EnTest7* this, PlayState* play, f32 lerp) {
    Player* player = GET_PLAYER(play);

    // Spin Player
    player->actor.shape.rot.y += 0x2EE0;

    // Squish Player thin
    player->actor.scale.x = LERPIMP_ALT(this->playerScaleX, 0.0f, lerp);
    player->actor.scale.z = LERPIMP_ALT(this->playerScaleZ, 0.0f, lerp);
}

void EnTest7_PlayerAndSubCamAction(EnTest7* this, PlayState* play) {
    f32 lerp;

    if ((this->timer >= 12) && (this->timer <= 30)) {
        lerp = (this->timer - 12) / (f32)(30 - 12);
        EnTest7_UpdateSubCamWarpCs2(this, play, lerp);
    } else if ((this->timer >= 79) && (this->timer <= 95)) {
        lerp = (this->timer - 79) / (f32)(95 - 79);
        EnTest7_UpdateSubCamWarpCs1(this, play, lerp);
    }

    if ((this->timer >= 42) && (this->timer <= 68)) {
        lerp = (this->timer - 42) / (f32)(68 - 42);
        EnTest7_SpinAndSquishPlayer(this, play, lerp);
    }
}

void EnTest7_SetupArriveCs(EnTest7* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->playerDrawFunc = player->actor.draw;
    player->actor.draw = NULL;
    player->stateFlags2 |= PLAYER_STATE2_20000000;
    this->flags |= OWL_WARP_FLAGS_DRAW_WIND_CAPSULE;
    this->windCapsule.unk_04 = 30.0f;

    if (play->roomCtx.curRoom.type != ROOM_TYPE_DUNGEON) {
        EnTest7_SetupAction(this, EnTest7_StartArriveCs);
    } else {
        EnTest7_SetupAction(this, EnTest7_StartArriveCsSkip);
    }
}

typedef struct {
    /* 0x0 */ s16 yaw;
    /* 0x2 */ s16 pitch;
    /* 0x4 */ f32 r;
} OwlWarpVecGeo; // size = 0x8

void EnTest7_UpdateSubCamArrivalCs1(EnTest7* this, PlayState* play) {
    static OwlWarpVecGeo sSubCamAtOffset = { 0, 0x31C7, 3500.0f };
    s32 pad;
    Player* player = GET_PLAYER(play);
    Vec3f* pos = &player->actor.world.pos;
    Camera* subCam =
        Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]));

    subCam->at.x = pos->x + (sSubCamAtOffset.r * Math_SinS(sSubCamAtOffset.yaw) * Math_CosS(sSubCamAtOffset.pitch));
    subCam->at.y = pos->y + (sSubCamAtOffset.r * Math_SinS(sSubCamAtOffset.pitch));
    subCam->at.z = pos->z + (sSubCamAtOffset.r * Math_CosS(sSubCamAtOffset.yaw)) * Math_CosS(sSubCamAtOffset.pitch);

    this->actor.world.pos.x = subCam->at.x;
    this->actor.world.pos.y = subCam->at.y - 40.0f;
    this->actor.world.pos.z = subCam->at.z;
}

void EnTest7_StartArriveCs(EnTest7* this, PlayState* play) {
    Camera* subCam;

    if (!CutsceneManager_IsNext(play->playerCsIds[PLAYER_CS_ID_SONG_WARP])) {
        CutsceneManager_Queue(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);
        return;
    }

    CutsceneManager_Start(play->playerCsIds[PLAYER_CS_ID_SONG_WARP], NULL);
    EnTest7_SetupAction(this, EnTest7_ArriveCsPart1);

    subCam = Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]));
    this->subCamEye = subCam->eye;
    this->subCamAt = subCam->at;

    EnTest7_UpdateSubCamArrivalCs1(this, play);
}

void EnTest7_UpdateSubCamArrivalCs2(EnTest7* this, PlayState* play, Vec3f* atNext, f32 lerp) {
    Vec3f subCamAt;
    Camera* subCam =
        Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]));

    subCam->focalActor = NULL;

    subCamAt.x = LERPIMP_ALT(atNext->x, subCam->at.x, lerp);
    subCamAt.y = LERPIMP_ALT(atNext->y, subCam->at.y, lerp);
    subCamAt.z = LERPIMP_ALT(atNext->z, subCam->at.z, lerp);

    subCam->at.x = subCamAt.x;
    subCam->at.y = subCamAt.y;
    subCam->at.z = subCamAt.z;
}

void EnTest7_ArriveCsPart1(EnTest7* this, PlayState* play) {
    f32 lerp = (40 - this->timer) / (f32)(40 - 0);
    Camera* subCam;

    this->flags |= OWL_WARP_FLAGS_DRAW_LENS_FLARE;

    this->windCapsule.unk_00 = 11.0f;
    this->windCapsule.xzScale = 0.05f;
    this->windCapsule.yScale = 0.05f;
    this->windCapsule.yaw += 0x2EE0;

    this->actor.world.pos.x = LERPIMP_ALT(this->actor.home.pos.x, this->actor.world.pos.x, lerp);
    this->actor.world.pos.y = LERPIMP_ALT(this->actor.home.pos.y, this->actor.world.pos.y, lerp);
    this->actor.world.pos.z = LERPIMP_ALT(this->actor.home.pos.z, this->actor.world.pos.z, lerp);

    subCam = Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]));
    EnTest7_UpdateSubCamArrivalCs2(this, play, &this->actor.home.pos, lerp);

    subCam->at.x = this->actor.world.pos.x;
    subCam->at.y = this->actor.world.pos.y + 40.0f;
    subCam->at.z = this->actor.world.pos.z;

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_PL_WARP_WING_ROLL_2 - SFX_FLAG);
    if (this->timer >= 40) {
        this->flags &= ~OWL_WARP_FLAGS_DRAW_LENS_FLARE;
        EnTest7_SetupAction(this, EnTest7_ArriveCsPart2);
    }
}

void EnTest7_UpdateSubCamArrivalCs3(EnTest7* this, PlayState* play) {
    Camera* subCam;
    Player* player = GET_PLAYER(play);
    Vec3f* pos = &player->actor.world.pos;

    subCam = Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]));

    // pitch is 147.042044 degrees
    subCam->eye.x = pos->x + 200.0f * Math_SinS(-player->actor.shape.rot.y) * -0.83907f;
    subCam->eye.y = pos->y + 200.0f * 0.544021f;
    subCam->eye.z = pos->z + 200.0f * Math_CosS(-player->actor.shape.rot.y) * -0.83907f;

    subCam->at.x = pos->x;
    subCam->at.y = pos->y + 40.0f;
    subCam->at.z = pos->z;
}

void EnTest7_StartArriveCsSkip(EnTest7* this, PlayState* play) {
    Camera* subCam;

    if (!CutsceneManager_IsNext(play->playerCsIds[PLAYER_CS_ID_SONG_WARP])) {
        CutsceneManager_Queue(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]);
    } else {
        CutsceneManager_Start(play->playerCsIds[PLAYER_CS_ID_SONG_WARP], NULL);
        EnTest7_SetupAction(this, EnTest7_ArriveCsPart2);

        subCam = Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(play->playerCsIds[PLAYER_CS_ID_SONG_WARP]));
        this->subCamEye = subCam->eye;
        this->subCamAt = subCam->at;
        this->timer = 40;

        EnTest7_UpdateSubCamArrivalCs3(this, play);
    }
}

void EnTest7_ArriveCsPart2(EnTest7* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Player* player2 = GET_PLAYER(play);
    Vec3f featherPos;

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_PL_WARP_WING_ROLL_2 - SFX_FLAG);

    featherPos = this->actor.world.pos;

    EnTest7_AddAndChooseFeather(this->feathers, &featherPos, true);
    EnTest7_AddAndChooseFeather(this->feathers, &featherPos, true);

    this->windCapsule.unk_04 = 70 - this->timer;
    if (this->timer > 70) {
        EnTest7_SetupAction(this, EnTest7_ArriveCsPart3);
    }

    if (this->windCapsule.unk_04 > 11.0f) {
        f32 temp = this->windCapsule.unk_04 - 11.0f;

        this->windCapsule.xzScale = ((-0.35f * temp) / 19.0f) + 0.4f;
        this->windCapsule.yScale = ((-0.35f * temp) / 19.0f) + 0.4f;
        this->windCapsule.yaw += 0x2EE0;
    } else {
        player2->actor.draw = this->playerDrawFunc;
        this->windCapsule.unk_00 = this->windCapsule.unk_04;
        this->windCapsule.xzScale = ((this->windCapsule.unk_04 * -0.29999998f) / 11.0f) + 0.7f;
        this->windCapsule.yScale = ((this->windCapsule.unk_04 * -0.29999998f) / 11.0f) + 0.7f;
        player->stateFlags2 &= ~PLAYER_STATE2_20000000;
    }
}

void EnTest7_ArriveCsPart3(EnTest7* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->timer > 90) {
        player->stateFlags1 &= ~PLAYER_STATE1_20;
        player->stateFlags1 &= ~PLAYER_STATE1_20000000;
        Actor_Kill(&this->actor);
    }
}

void EnTest7_Update(Actor* thisx, PlayState* play) {
    EnTest7* this = (EnTest7*)thisx;

    this->actionFunc(this, play);

    if (this->playerCamFunc != NULL) {
        this->playerCamFunc(this, play);
    }

    this->timer++;

    EnTest7_UpdateFeathers(play, this->feathers, this, (this->flags & OWL_WARP_FLAGS_8) != 0,
                           (this->flags & OWL_WARP_FLAGS_10) != 0);
}

s32 EnTest7_OverrideLimbDraw(PlayState* play, KFSkelAnimeFlex* kfSkelAnime, s32 limbIndex, Gfx** dList, u8* flags,
                             void* thisx, Vec3f* scale, Vec3s* rot, Vec3f* pos) {
    EnTest7* this = (EnTest7*)thisx;
    Vec3f featherPos;

    if ((*dList != NULL) && (Rand_ZeroOne() < 0.03f)) {
        Matrix_MultVec3f(&gZeroVec3f, &featherPos);
        EnTest7_AddAndChooseFeather(this->feathers, &featherPos, false);
    }
    return true;
}

void EnTest7_Draw(Actor* thisx, PlayState* play) {
    s32 pad[2];
    EnTest7* this = (EnTest7*)thisx;
    s32 sp40;

    // Draw wings
    if (this->flags & OWL_WARP_FLAGS_DRAW_WINGS) {
        Mtx* mtxStack = GRAPH_ALLOC(play->state.gfxCtx, this->kfSkelAnime.skeleton->dListCount * sizeof(Mtx));

        if (mtxStack == NULL) {
            return;
        }
        Keyframe_DrawFlex(play, &this->kfSkelAnime, mtxStack, EnTest7_OverrideLimbDraw, NULL, &this->actor);
    }

    // Draw windCapsule encasing that surrounds player after wings
    if (this->flags & OWL_WARP_FLAGS_DRAW_WIND_CAPSULE) {
        Matrix_Push();
        Matrix_Translate(0.0f, 4000.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateZYX(0, this->windCapsule.yaw, 0, MTXMODE_APPLY);
        Matrix_Scale(this->windCapsule.xzScale * 100.0f, this->windCapsule.yScale * 100.0f,
                     this->windCapsule.xzScale * 100.0f, MTXMODE_APPLY);
        sp40 = this->windCapsule.unk_00;
        AnimatedMat_DrawStep(play, Lib_SegmentedToVirtual(&gSoaringWarpCsWindCapsuleTexAnim), sp40);
        Gfx_DrawDListXlu(play, gSoaringWarpCsWindCapsuleDL);
        Matrix_Pop();
    }

    EnTest7_DrawFeathers(play, this->feathers);

    if (this->flags & OWL_WARP_FLAGS_DRAW_LENS_FLARE) {
        Environment_DrawLensFlare(play, &play->envCtx, &play->view, play->state.gfxCtx, this->actor.world.pos, 70.0f,
                                  5.0f, 0, false);
    }
}
