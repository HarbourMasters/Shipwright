/*
 * File: z_door_warp1.c
 * Overlay: ovl_Door_Warp1
 * Description: Blue warp portal and crystal, and the Majora's Mask-shaped boss warp platform
 */

#include "prevent_bss_reordering.h"
#include "z_door_warp1.h"
#include "objects/object_warp1/object_warp1.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS 0x00000000

void DoorWarp1_Init(Actor* thisx, PlayState* play);
void DoorWarp1_Destroy(Actor* thisx, PlayState* play);
void DoorWarp1_Update(Actor* thisx, PlayState* play);
void DoorWarp1_Draw(Actor* thisx, PlayState* play);
void DoorWarp1_Reset(void);

void DoorWarp1_SetupAction(DoorWarp1* this, DoorWarp1ActionFunc actionFunc);
void func_808B8924(DoorWarp1* this, PlayState* play);
void func_808B8A7C(DoorWarp1* this, PlayState* play);
void func_808B8C48(DoorWarp1* this, PlayState* play);
void func_808B8E78(DoorWarp1* this, PlayState* play);
void func_808B9094(DoorWarp1* this, PlayState* play);
void func_808B90CC(DoorWarp1* this, PlayState* play);
void func_808B921C(DoorWarp1* this, PlayState* play);
void func_808B93A0(DoorWarp1* this, PlayState* play);
void func_808B94A4(DoorWarp1* this, PlayState* play);
void func_808B9524(DoorWarp1* this, PlayState* play);
void func_808B958C(DoorWarp1* this, PlayState* play);
void func_808B96A0(DoorWarp1* this, PlayState* play);
void func_808B96B0(DoorWarp1* this, PlayState* play);
void func_808B977C(DoorWarp1* this, PlayState* play);
void func_808B9840(DoorWarp1* this, PlayState* play);
void func_808B98A8(DoorWarp1* this, PlayState* play);
void func_808B9B30(DoorWarp1* this, PlayState* play);
void func_808B9BE8(DoorWarp1* this, PlayState* play);
void func_808B9CE8(DoorWarp1* this, PlayState* play);
void func_808B9E94(DoorWarp1* this, PlayState* play);
void func_808B9ED8(DoorWarp1* this, PlayState* play);
void func_808B9F10(DoorWarp1* this, PlayState* play);
void func_808B9FD0(DoorWarp1* this, PlayState* play);
void func_808BA10C(DoorWarp1* this, PlayState* play);
void func_808BA550(DoorWarp1* this, PlayState* play);
void func_808BAAF4(DoorWarp1* this, PlayState* play);
void func_808BABF4(DoorWarp1* this, PlayState* play);
void func_808BB8D4(DoorWarp1* this, PlayState* play, s32 arg2);

s16 D_808BC000;
f32 D_808BC004;

ActorProfile Door_Warp1_Profile = {
    /**/ ACTOR_DOOR_WARP1,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_WARP1,
    /**/ sizeof(DoorWarp1),
    /**/ DoorWarp1_Init,
    /**/ DoorWarp1_Destroy,
    /**/ DoorWarp1_Update,
    /**/ DoorWarp1_Draw,
    /**/ DoorWarp1_Reset,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 4000, ICHAIN_STOP),
};

void DoorWarp1_SetupAction(DoorWarp1* this, DoorWarp1ActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

s32 func_808B849C(DoorWarp1* this, PlayState* play) {
    s32 ret = 0;

    if (GameInteractor_Should(VB_SPAWN_BOSS_REMAINS, true, &ret)) {
        if ((play->sceneId == SCENE_MITURIN_BS) && !CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA)) {
            ret = 1;
        } else if ((play->sceneId == SCENE_HAKUGIN_BS) && !CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT)) {
            ret = 2;
        } else if ((play->sceneId == SCENE_SEA_BS) && !CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG)) {
            ret = 3;
        } else if ((play->sceneId == SCENE_INISIE_BS) && !CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD)) {
            ret = 4;
        }
    }
    return ret;
}

void func_808B8568(DoorWarp1* this, PlayState* play) {
    s32 pad[2];

    Lights_PointNoGlowSetInfo(&this->unk_1E0, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                              this->dyna.actor.world.pos.z, 0, 0, 0, 0);
    this->unk_1DC = LightContext_InsertLight(play, &play->lightCtx, &this->unk_1E0);
    Lights_PointNoGlowSetInfo(&this->unk_1F4, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                              this->dyna.actor.world.pos.z, 0, 0, 0, 0);
    this->unk_1F0 = LightContext_InsertLight(play, &play->lightCtx, &this->unk_1F4);
}

s32 func_808B866C(DoorWarp1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 ret = false;

    if ((fabsf(this->dyna.actor.xzDistToPlayer) < 60.0f) &&
        (((player->actor.world.pos.y - 20.0f) < this->dyna.actor.world.pos.y) &&
         (this->dyna.actor.world.pos.y < (player->actor.world.pos.y + 20.0f)))) {
        ret = true;
    }
    return ret;
}

void DoorWarp1_Init(Actor* thisx, PlayState* play) {
    DoorWarp1* this = (DoorWarp1*)thisx;

    this->unk_1CC = 0;
    this->unk_202 = 0;
    this->unk_203 = 0;
    this->unk_1A0 = NULL;
    this->unk_1C0 = 0.0f;
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    ActorShape_Init(&this->dyna.actor.shape, 0.0f, NULL, 0.0f);

    this->unk_1D3 = 0;
    this->unk_1D4 = 0;
    this->unk_203 = 0;
    this->unk_204 = 1.0f;

    switch (DOORWARP1_GET_FF(&this->dyna.actor)) {
        case ENDOORWARP1_FF_0:
        case ENDOORWARP1_FF_1:
        case ENDOORWARP1_FF_2:
        case ENDOORWARP1_FF_3:
        case ENDOORWARP1_FF_4:
        case ENDOORWARP1_FF_5:
            func_808B8568(this, play);
            break;
    }

    switch (DOORWARP1_GET_FF(&this->dyna.actor)) {
        case ENDOORWARP1_FF_0:
            func_808B8924(this, play);
            break;

        case ENDOORWARP1_FF_1:
            func_808B8A7C(this, play);
            break;

        case ENDOORWARP1_FF_2:
        case ENDOORWARP1_FF_3:
        case ENDOORWARP1_FF_4:
        case ENDOORWARP1_FF_5:
            this->unk_1D3 = 1;
            DynaPolyActor_Init(&this->dyna, 0);
            DynaPolyActor_LoadMesh(play, &this->dyna, &gWarpBossWarpPlatformCol);
            func_808B8C48(this, play);
            break;

        case ENDOORWARP1_FF_6:
            func_808B8E78(this, play);
            break;
    }

    if ((play->sceneId == SCENE_MITURIN_BS) || (play->sceneId == SCENE_HAKUGIN_BS) ||
        (play->sceneId == SCENE_INISIE_BS) || (play->sceneId == SCENE_SEA_BS)) {
        Environment_StopTime();
        play->interfaceCtx.restrictions.songOfTime = 1;
        play->interfaceCtx.restrictions.songOfSoaring = 1;
    }
}

void DoorWarp1_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    DoorWarp1* this = (DoorWarp1*)thisx;
    s16 i;

    LightContext_RemoveLight(play, &play->lightCtx, this->unk_1DC);
    LightContext_RemoveLight(play, &play->lightCtx, this->unk_1F0);

    for (i = 0; i < ARRAY_COUNT(play->envCtx.adjLightSettings.light1Color); i++) {
        play->envCtx.adjLightSettings.ambientColor[i] = play->envCtx.adjLightSettings.fogColor[i] =
            play->envCtx.adjLightSettings.light1Color[i] = 0;
    }

    if (this->unk_1D3 != 0) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void func_808B8924(DoorWarp1* this, PlayState* play) {
    this->unk_1C4 = 0;
    this->unk_1C6 = -140;
    this->unk_1C8 = -80;
    this->unk_1BC = 1.0f;
    this->unk_1B0 = 0.0f;
    this->unk_1B4 = 0.0f;
    this->unk_1B8 = 0.0f;
    this->unk_1A4 = 0.3f;
    this->unk_1A8 = 0.3f;
    this->unk_1AC = 0.0f;
    this->dyna.actor.shape.yOffset = 1.0f;
    this->unk_1D0 = 0;
    D_808BC000 = 100;
    Lights_PointNoGlowSetInfo(&this->unk_1E0, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                              this->dyna.actor.world.pos.z, 200, 255, 255, 255);
    Lights_PointNoGlowSetInfo(&this->unk_1F4, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                              this->dyna.actor.world.pos.z, 200, 255, 255, 255);
    DoorWarp1_SetupAction(this, func_808B96B0);
}

void func_808B8A7C(DoorWarp1* this, PlayState* play) {
    SkelAnime_Init(play, &this->skelAnime, &gWarpCrystalSkel, &gWarpCrystalAnim, NULL, NULL, 0);
    Animation_ChangeImpl(&this->skelAnime, &gWarpCrystalAnim, 1.0f, 1.0f, 1.0f, 2, 40.0f, 1);
    this->unk_1C4 = 0;
    this->unk_1C6 = -140;
    this->unk_1C8 = -80;
    this->unk_1D0 = 0;
    this->unk_1BC = 1.0f;
    this->unk_1A4 = 0.3f;
    this->unk_1A8 = 0.3f;
    this->unk_1B0 = 0.0f;
    this->unk_1B4 = 0.0f;
    this->unk_1B8 = 0.0f;
    this->unk_1AC = 0.0f;
    this->dyna.actor.shape.yOffset = -400.0f;
    D_808BC000 = 160;
    Lights_PointNoGlowSetInfo(&this->unk_1E0, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                              this->dyna.actor.world.pos.z, 200, 255, 255, 255);
    Lights_PointNoGlowSetInfo(&this->unk_1F4, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                              this->dyna.actor.world.pos.z, 200, 255, 255, 255);
    DoorWarp1_SetupAction(this, func_808B9B30);
}

void func_808B8C48(DoorWarp1* this, PlayState* play) {
    this->dyna.actor.shape.yOffset = 0.0f;
    Actor_SetScale(&this->dyna.actor, 0.1f);
    Lights_PointNoGlowSetInfo(&this->unk_1E0, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                              this->dyna.actor.world.pos.z, 200, 255, 255, 255);
    Lights_PointNoGlowSetInfo(&this->unk_1F4, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                              this->dyna.actor.world.pos.z, 200, 255, 255, 255);
    bool vanillaCondition =
        ((DOORWARP1_GET_FF(&this->dyna.actor) == ENDOORWARP1_FF_2) && CHECK_QUEST_ITEM(QUEST_REMAINS_ODOLWA)) ||
        ((DOORWARP1_GET_FF(&this->dyna.actor) == ENDOORWARP1_FF_3) && CHECK_QUEST_ITEM(QUEST_REMAINS_GOHT)) ||
        ((DOORWARP1_GET_FF(&this->dyna.actor) == ENDOORWARP1_FF_4) && CHECK_QUEST_ITEM(QUEST_REMAINS_GYORG)) ||
        ((DOORWARP1_GET_FF(&this->dyna.actor) == ENDOORWARP1_FF_5) && CHECK_QUEST_ITEM(QUEST_REMAINS_TWINMOLD));
    if (GameInteractor_Should(VB_ACTIVATE_BOSS_WARP_PAD, vanillaCondition, this)) {
        s16 params = DOORWARP1_GET_FF00_2(&this->dyna.actor);

        params |= 6;
        Actor_SpawnAsChild(&play->actorCtx, &this->dyna.actor, play, ACTOR_DOOR_WARP1, this->dyna.actor.world.pos.x,
                           this->dyna.actor.world.pos.y + 10.0f, this->dyna.actor.world.pos.z,
                           this->dyna.actor.world.rot.x, this->dyna.actor.world.rot.y, this->dyna.actor.world.rot.z,
                           params);
        DoorWarp1_SetupAction(this, func_808BAAF4);
    } else {
        DoorWarp1_SetupAction(this, func_808BABF4);
    }
}

void func_808B8E78(DoorWarp1* this, PlayState* play) {
    Actor_SetScale(&this->dyna.actor, 0.1f);
    Lights_PointNoGlowSetInfo(&this->unk_1E0, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                              this->dyna.actor.world.pos.z, 200, 255, 255, 0);
    Lights_PointNoGlowSetInfo(&this->unk_1F4, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                              this->dyna.actor.world.pos.z, 200, 255, 255, 0);
    Actor_SetScale(&this->dyna.actor, 3.0f);
    this->unk_1D4 = 1;
    this->unk_1D0 = 0;
    this->unk_1A8 = 0.0f;
    this->unk_1AC = 0.0f;
    this->unk_1A4 = 700.0f;
    if (play->sceneId == SCENE_INISIE_N) {
        DoorWarp1_SetupAction(this, func_808B96A0);
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_86_80)) {
        this->unk_1D4 = 0;
        DoorWarp1_SetupAction(this, func_808B921C);
    } else {
        DoorWarp1_SetupAction(this, func_808B9094);
    }
}

s32 func_808B900C(DoorWarp1* this, PlayState* play) {
    s32 cueChannel;
    u8 ret = false;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_569)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_569);

        if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
            this->cueId = play->csCtx.actorCues[cueChannel]->id;
            if (play->csCtx.actorCues[cueChannel]->id == 2) {
                ret = true;
            }
        }
    }

    return ret;
}

void func_808B9094(DoorWarp1* this, PlayState* play) {
    if (func_808B900C(this, play)) {
        DoorWarp1_SetupAction(this, func_808B90CC);
    }
}

void func_808B90CC(DoorWarp1* this, PlayState* play) {
    s32 pad[2];
    s16 sp2E = 0;
    f32 phi_f0 = 0.0f;

    if (play->sceneId == SCENE_MITURIN) {
        sp2E = -10;
        phi_f0 = -5.0f;
    } else if (play->sceneId == SCENE_HAKUGIN) {
        sp2E = -20;
    } else if (play->sceneId == SCENE_SEA) {
        sp2E = -20;
    }

    Math_SmoothStepToF(&this->unk_1AC, 40.0f, 0.5f, 1.2f + phi_f0, 0.01f);
    Math_SmoothStepToF(&this->unk_1A4, 0.0f, 0.5f, 12.0f, 0.1f);
    if ((this->unk_1A4 < 460.0f) && (this->unk_1A4 > 100.0f)) {
        func_808BB8D4(this, play, 6);
    }

    if (this->unk_1A4 < 0.1f) {
        this->unk_1D0 = sp2E + 30;
        this->unk_1D4 = 0;
        DoorWarp1_SetupAction(this, func_808B921C);
    }

    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WARP_HOLE_ENERGY - SFX_FLAG);
}

void func_808B921C(DoorWarp1* this, PlayState* play) {
    if (this->unk_1D0 != 0) {
        this->unk_1D0--;
        return;
    }

    Math_SmoothStepToF(&this->unk_1A8, 1.0f, 0.2f, 0.05f, 0.01f);
    this->unk_203 = (u8)(this->unk_1A8 * 200.0f);
    if (this->unk_1A8 > 0.8f) {
        func_808BB8D4(this, play, 1);
    }

    if (func_808B866C(this, play) && !Play_InCsMode(play)) {
        Player_SetCsActionWithHaltedActors(play, &this->dyna.actor, PLAYER_CSACTION_WAIT);
        Message_StartTextbox(play, 0xF2, &this->dyna.actor);
        DoorWarp1_SetupAction(this, func_808B93A0);
    }

    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BOSS_WARP_HOLE - SFX_FLAG);
}

void func_808B93A0(DoorWarp1* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        if (play->msgCtx.choiceIndex == 0) {
            Audio_PlaySfx_MessageDecide();
            Player_SetCsActionWithHaltedActors(play, &this->dyna.actor, PLAYER_CSACTION_9);
            player->unk_3A0.x = this->dyna.actor.world.pos.x;
            player->unk_3A0.z = this->dyna.actor.world.pos.z;
            this->unk_1CA = 1;
            DoorWarp1_SetupAction(this, func_808B9524);
        } else {
            Audio_PlaySfx_MessageCancel();
            Player_SetCsActionWithHaltedActors(play, &this->dyna.actor, PLAYER_CSACTION_END);
            DoorWarp1_SetupAction(this, func_808B94A4);
        }
    }
    func_808BB8D4(this, play, 1);
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BOSS_WARP_HOLE - SFX_FLAG);
}

void func_808B94A4(DoorWarp1* this, PlayState* play) {
    if (!func_808B866C(this, play) && (CutsceneManager_GetCurrentCsId() != play->playerCsIds[PLAYER_CS_ID_SONG_WARP])) {
        DoorWarp1_SetupAction(this, func_808B921C);
    }
    func_808BB8D4(this, play, 1);
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BOSS_WARP_HOLE - SFX_FLAG);
}

void func_808B9524(DoorWarp1* this, PlayState* play) {
    if (!CutsceneManager_IsNext(play->playerCsIds[PLAYER_CS_ID_WARP_PAD_ENTRANCE])) {
        CutsceneManager_Queue(play->playerCsIds[PLAYER_CS_ID_WARP_PAD_ENTRANCE]);
    } else {
        CutsceneManager_Start(play->playerCsIds[PLAYER_CS_ID_WARP_PAD_ENTRANCE], NULL);
        DoorWarp1_SetupAction(this, func_808B958C);
    }
}

void func_808B958C(DoorWarp1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    func_808BB8D4(this, play, 1);

    if (this->unk_1CA > 100) {
        if (player->actor.velocity.y < 10.0f) {
            player->actor.gravity = 0.1f;
        } else {
            player->actor.gravity = 0.0f;
        }
    } else {
        this->unk_1CA++;
    }

    this->unk_1D0++;
    if ((this->unk_1D0 > 120) && (gSaveContext.nextCutsceneIndex == 0xFFEF)) {
        func_808BA10C(this, play);
        play->transitionType = TRANS_TYPE_FADE_WHITE;
        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
    }

    Math_SmoothStepToF(&this->unk_1A8, 6.0f, 0.2f, 0.02f, 0.01f);
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_LINK_WARP - SFX_FLAG);
}

void func_808B96A0(DoorWarp1* this, PlayState* play) {
}

void func_808B96B0(DoorWarp1* this, PlayState* play) {
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WARP_HOLE - SFX_FLAG);
    Math_SmoothStepToF(&this->unk_1B0, 255.0f, 0.4f, 10.0f, 0.01f);
    Math_SmoothStepToF(&this->unk_1B4, 255.0f, 0.4f, 10.0f, 0.01f);

    if (this->unk_1C4 < 100) {
        this->unk_1C4 += 2;
    }

    if (this->unk_1C6 < 120) {
        this->unk_1C6 += 4;
    }

    if (this->unk_1C8 < 230) {
        this->unk_1C8 += 4;
    } else {
        DoorWarp1_SetupAction(this, func_808B977C);
    }
}

void func_808B977C(DoorWarp1* this, PlayState* play) {
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WARP_HOLE - SFX_FLAG);
    if (func_808B866C(this, play) && !Play_InCsMode(play)) {
        Player* player = GET_PLAYER(play);

        AudioSfx_PlaySfx(NA_SE_EV_LINK_WARP, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                         &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        Player_SetCsActionWithHaltedActors(play, &this->dyna.actor, PLAYER_CSACTION_9);
        player->unk_3A0.x = this->dyna.actor.world.pos.x;
        player->unk_3A0.z = this->dyna.actor.world.pos.z;
        this->unk_1CA = 1;
        DoorWarp1_SetupAction(this, func_808B9840);
    }
}

void func_808B9840(DoorWarp1* this, PlayState* play) {
    if (!CutsceneManager_IsNext(play->playerCsIds[PLAYER_CS_ID_WARP_PAD_ENTRANCE])) {
        CutsceneManager_Queue(play->playerCsIds[PLAYER_CS_ID_WARP_PAD_ENTRANCE]);
    } else {
        CutsceneManager_Start(play->playerCsIds[PLAYER_CS_ID_WARP_PAD_ENTRANCE], NULL);
        DoorWarp1_SetupAction(this, func_808B98A8);
    }
}

void func_808B98A8(DoorWarp1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->unk_1CA > 100) {
        if (player->actor.velocity.y < 10.0f) {
            player->actor.gravity = 0.1f;
        } else {
            player->actor.gravity = 0.0f;
        }
    } else {
        this->unk_1CA++;
    }

    Math_SmoothStepToF(&this->unk_1B0, 0.0f, 0.2f, 6.0f, 0.01f);

    this->unk_1D0++;
    if (D_808BC000 < this->unk_1D0) {
        if ((gSaveContext.nextCutsceneIndex == 0xFFEF) && (D_808BC000 < this->unk_1D0) &&
            (gSaveContext.nextCutsceneIndex == 0xFFEF)) {
            if (DOORWARP1_GET_FF00_1(&this->dyna.actor) != 0xFF) {
                play->nextEntrance = play->setupExitList[DOORWARP1_GET_FF00_3(&this->dyna.actor)];
                Scene_SetExitFade(play);
                play->transitionTrigger = TRANS_TRIGGER_START;
            } else {
                func_80169FDC(play);
            }
        }
    }

    Math_StepToF(&this->unk_1A4, 2.0f, 0.01f);
    Math_StepToF(&this->unk_1A8, 10.0f, 0.02f);
    Lights_PointNoGlowSetInfo(&this->unk_1E0, player->actor.world.pos.x + 10, player->actor.world.pos.y + 10,
                              player->actor.world.pos.z + 10, 235, 255, 255, 255);
    Lights_PointNoGlowSetInfo(&this->unk_1F4, player->actor.world.pos.x - 10, player->actor.world.pos.y - 10,
                              player->actor.world.pos.z - 10, 235, 255, 255, 255);
    Math_SmoothStepToF(&this->dyna.actor.shape.yOffset, 0.0f, 0.5f, 2.0f, 0.1f);
}

void func_808B9B30(DoorWarp1* this, PlayState* play) {
    if (fabsf(this->dyna.actor.xzDistToPlayer) >= 60.0f) {
        if (func_808B849C(this, play)) {
            this->unk_1A0 = (DmHina*)Actor_SpawnAsChild(
                &play->actorCtx, &this->dyna.actor, play, ACTOR_DM_HINA, this->dyna.actor.world.pos.x,
                this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, 0, 0, 0, func_808B849C(this, play) - 1);
        }
        DoorWarp1_SetupAction(this, func_808B9BE8);
    }
}

void func_808B9BE8(DoorWarp1* this, PlayState* play) {
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WARP_HOLE - SFX_FLAG);
    Math_SmoothStepToF(&this->unk_1B0, 255.0f, 0.2f, 2.0f, 0.1f);
    Math_SmoothStepToF(&this->unk_1B4, 255.0f, 0.2f, 2.0f, 0.1f);
    if (this->unk_1C4 < 10) {
        this->unk_1C4 += 2;
    }

    if (this->unk_1C6 < 120) {
        this->unk_1C6 += 4;
    }

    if (this->unk_1C8 < 230) {
        this->unk_1C8 += 4;
        return;
    }

    this->dyna.actor.parent = NULL;
    if (func_808B849C(this, play)) {
        this->unk_202 = 1;
        DoorWarp1_SetupAction(this, func_808B9CE8);
    } else {
        DoorWarp1_SetupAction(this, func_808B9F10);
    }
}

void func_808B9CE8(DoorWarp1* this, PlayState* play) {
    if (this->unk_203 != 0) {
        if (1) {}
        return;
    }

    if (!Actor_HasParent(&this->dyna.actor, play)) {
        Actor_OfferGetItem(&this->dyna.actor, play, (GI_REMAINS_ODOLWA - 1) + func_808B849C(this, play), 30.0f, 80.0f);
        return;
    }

    switch (play->sceneId) {
        case SCENE_MITURIN_BS:
            gSaveContext.save.saveInfo.unk_EA8[0] = (((void)0, gSaveContext.save.saveInfo.unk_EA8[0]) & 0xFFFFFF00) |
                                                    (((u8)gSaveContext.save.saveInfo.unk_EA8[1]) & 0xFF);
            break;

        case SCENE_HAKUGIN_BS:
            gSaveContext.save.saveInfo.unk_EA8[0] = (((void)0, gSaveContext.save.saveInfo.unk_EA8[0]) & 0xFFFF00FF) |
                                                    ((((u8)gSaveContext.save.saveInfo.unk_EA8[1]) & 0xFF) << 8);
            break;

        case SCENE_INISIE_BS:
            gSaveContext.save.saveInfo.unk_EA8[0] = (((void)0, gSaveContext.save.saveInfo.unk_EA8[0]) & 0xFF00FFFF) |
                                                    ((((u8)gSaveContext.save.saveInfo.unk_EA8[1]) & 0xFF) << 0x10);
            break;

        case SCENE_SEA_BS:
            gSaveContext.save.saveInfo.unk_EA8[0] = (((void)0, gSaveContext.save.saveInfo.unk_EA8[0]) & 0x00FFFFFF) |
                                                    ((((u8)gSaveContext.save.saveInfo.unk_EA8[1]) & 0xFF) << 0x18);
            break;

        default:
            break;
    }

    gSaveContext.save.saveInfo.unk_EA8[1] = (gSaveContext.save.saveInfo.unk_EA8[1] & 0xFFFFFF00) |
                                            ((((u8)gSaveContext.save.saveInfo.unk_EA8[1]) + 1) & 0xFF);
    Item_Give(play, func_808B849C(this, play) + (ITEM_REMAINS_ODOLWA - 1));
    DoorWarp1_SetupAction(this, func_808B9E94);
}

void func_808B9E94(DoorWarp1* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->unk_1CE = 110;
        DoorWarp1_SetupAction(this, func_808B9ED8);
    }
}

void func_808B9ED8(DoorWarp1* this, PlayState* play) {
    this->unk_1CE--;
    if (this->unk_1CE <= 0) {
        func_808BA10C(this, play);
    }
}

void func_808B9F10(DoorWarp1* this, PlayState* play) {
    Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WARP_HOLE - SFX_FLAG);
    if ((this->unk_203 == 0) && func_808B866C(this, play) && !Play_InCsMode(play) && (this->unk_203 == 0)) {
        Player* player = GET_PLAYER(play);

        Interface_SetHudVisibility(HUD_VISIBILITY_NONE);
        Player_SetCsActionWithHaltedActors(play, &this->dyna.actor, PLAYER_CSACTION_9);
        player->unk_3A0.x = this->dyna.actor.world.pos.x;
        player->unk_3A0.z = this->dyna.actor.world.pos.z;
        this->unk_1CA = 20;
        DoorWarp1_SetupAction(this, func_808B9FD0);
    }
}

void func_808B9FD0(DoorWarp1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Player* player2 = GET_PLAYER(play);

    if (this->unk_1CA != 0) {
        this->unk_1CA--;
        return;
    }

    if (!CutsceneManager_IsNext(play->playerCsIds[PLAYER_CS_ID_WARP_PAD_ENTRANCE])) {
        CutsceneManager_Queue(play->playerCsIds[PLAYER_CS_ID_WARP_PAD_ENTRANCE]);
    } else {
        CutsceneManager_Start(play->playerCsIds[PLAYER_CS_ID_WARP_PAD_ENTRANCE], NULL);
        AudioSfx_PlaySfx(NA_SE_EV_LINK_WARP, &player->actor.projectedPos, 4, &gSfxDefaultFreqAndVolScale,
                         &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        Animation_ChangeImpl(&this->skelAnime, &gWarpCrystalAnim, 1.0f, Animation_GetLastFrame(&gWarpCrystalAnim),
                             Animation_GetLastFrame(&gWarpCrystalAnim), 2, 40.0f, 1);
        this->unk_1CA = 50;
        D_808BC004 = player2->actor.world.pos.y;
        DoorWarp1_SetupAction(this, func_808BA550);
    }
}

void func_808BA10C(DoorWarp1* this, PlayState* play) {
    s32 phi_v0_2;
    u8 phi_a0;
    s32 phi_v0_3;

    if ((play->sceneId == SCENE_MITURIN_BS) || (play->sceneId == SCENE_HAKUGIN_BS) ||
        (play->sceneId == SCENE_INISIE_BS) || (play->sceneId == SCENE_SEA_BS)) {
        gDungeonBossWarpSceneId = play->sceneId;
        if (play->sceneId == SCENE_MITURIN_BS) {
            phi_v0_2 = 0;
        } else if (play->sceneId == SCENE_HAKUGIN_BS) {
            phi_v0_2 = 1;
        } else if (play->sceneId == SCENE_INISIE_BS) {
            phi_v0_2 = 2;
        } else if (play->sceneId == SCENE_SEA_BS) {
            phi_v0_2 = 3;
        } else {
            phi_v0_2 = 0;
        }

        if (this->unk_202 != 0) {
            if (phi_v0_2 > 0) {
                SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WOODFALL_TEMPLE_PRISON);
            }

            switch (phi_v0_2) {
                case 0:
                    phi_a0 = gSaveContext.save.saveInfo.unk_EA8[0] & 0xFF;
                    break;

                case 1:
                    phi_a0 = (gSaveContext.save.saveInfo.unk_EA8[0] & 0xFF00) >> 8;
                    break;

                case 2:
                    phi_a0 = (gSaveContext.save.saveInfo.unk_EA8[0] & 0xFF0000) >> 0x10;
                    break;

                case 3:
                    phi_a0 = (gSaveContext.save.saveInfo.unk_EA8[0] & 0xFF000000) >> 0x18;
                    break;

                default:
                    phi_a0 = 0;
                    break;
            }

            switch (phi_a0) {
                case 0:
                    phi_v0_3 = 10;
                    break;

                case 1:
                    phi_v0_3 = 0;
                    break;

                case 2:
                    phi_v0_3 = 1;
                    break;

                case 3:
                    phi_v0_3 = 2;
                    break;

                default:
                    phi_v0_3 = 10;
                    break;
            }

            play->nextEntrance = ENTRANCE(GIANTS_CHAMBER, 0);
            if (phi_v0_3 < 10) {
                gSaveContext.nextCutsceneIndex = phi_v0_3 + 0xFFF0;
            }

            play->transitionTrigger = TRANS_TRIGGER_START;
            play->transitionType = TRANS_TYPE_FADE_WHITE;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
        } else {
            switch (phi_v0_2) {
                case 0:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)) {
                        // Skips the entrance cutscene as this flag is attached to `ENTRANCE(WOODFALL_TEMPLE, 1)`
                        SET_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WOODFALL_TEMPLE_PRISON);
                        play->nextEntrance = ENTRANCE(WOODFALL_TEMPLE, 1);
                        play->transitionTrigger = TRANS_TRIGGER_START;
                        play->transitionType = TRANS_TYPE_FADE_WHITE;
                        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                    } else {
                        play->nextEntrance = ENTRANCE(WOODFALL, 0);
                        gSaveContext.nextCutsceneIndex = 0xFFF0;
                        play->transitionTrigger = TRANS_TRIGGER_START;
                        play->transitionType = TRANS_TYPE_FADE_WHITE;
                        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                    }
                    break;

                case 1:
                    SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE);
                    play->nextEntrance = ENTRANCE(MOUNTAIN_VILLAGE_SPRING, 7);
                    play->transitionTrigger = TRANS_TRIGGER_START;
                    play->transitionType = TRANS_TYPE_FADE_WHITE;
                    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                    break;

                case 3:
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE)) {
                        play->nextEntrance = ENTRANCE(ZORA_CAPE, 9);
                        gSaveContext.nextCutsceneIndex = 0xFFF0;
                        play->transitionTrigger = TRANS_TRIGGER_START;
                        play->transitionType = TRANS_TYPE_FADE_WHITE;
                        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                    } else {
                        SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_GREAT_BAY_TEMPLE);
                        play->nextEntrance = ENTRANCE(ZORA_CAPE, 8);
                        gSaveContext.nextCutsceneIndex = 0xFFF0;
                        play->transitionTrigger = TRANS_TRIGGER_START;
                        play->transitionType = TRANS_TYPE_FADE_WHITE;
                        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                    }
                    break;

                case 2:
                    SET_WEEKEVENTREG(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE);
                    play->nextEntrance = ENTRANCE(IKANA_CANYON, 15);
                    gSaveContext.nextCutsceneIndex = 0xFFF2;
                    play->transitionTrigger = TRANS_TRIGGER_START;
                    play->transitionType = TRANS_TYPE_FADE_WHITE;
                    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                    break;
            }
        }
    } else if (DOORWARP1_GET_FF00_1(&this->dyna.actor) != 0xFF) {
        if (DOORWARP1_GET_FF(&this->dyna.actor) == ENDOORWARP1_FF_6) {
            gSaveContext.respawnFlag = -2;
        }
        play->nextEntrance = play->setupExitList[DOORWARP1_GET_FF00_3(&this->dyna.actor)];
        Scene_SetExitFade(play);
        play->transitionTrigger = TRANS_TRIGGER_START;
    } else {
        func_80169FDC(play);
    }
}

void func_808BA550(DoorWarp1* this, PlayState* play) {
    Player* player2 = GET_PLAYER(play);
    Player* player = GET_PLAYER(play);
    s16 i;
    f32 temp_f0;
    f32 temp_f2;
    s32 temp;
    s32 tempS;

    if (this->unk_1CA != 0) {
        this->unk_1CA--;
    }

    if (this->unk_1CA < 0x1F) {
        temp_f0 = 0.0f;
        switch (player2->transformation) {
            case PLAYER_FORM_FIERCE_DEITY:
                temp_f0 = -37.0f;
                break;

            case PLAYER_FORM_GORON:
                temp_f0 = -37.0f;
                break;

            case PLAYER_FORM_ZORA:
                temp_f0 = -37.0f;
                break;

            case PLAYER_FORM_DEKU:
                temp_f0 = -20.0f;
                break;

            case PLAYER_FORM_HUMAN:
                temp_f0 = -25.0f;
                break;

            default:
                break;
        }

        temp_f2 = this->dyna.actor.world.pos.y + (this->dyna.actor.shape.yOffset * 0.1f) + temp_f0;

        if (D_808BC004 < temp_f2) {
            player->actor.world.pos.y = temp_f2;
        }

        if (this->unk_1CA <= 0) {
            if (this->unk_1CC < 4000) {
                this->unk_1CC += 40;
            }
            player->actor.world.rot.y -= this->unk_1CC;
            player->actor.shape.rot.y -= this->unk_1CC;
        }
        Math_SmoothStepToF(&player->actor.world.pos.x, this->dyna.actor.world.pos.x, 0.5f, 0.1f, 0.01f);
        Math_SmoothStepToF(&player->actor.world.pos.z, this->dyna.actor.world.pos.z, 0.5f, 0.1f, 0.01f);
    }

    this->unk_1D0++;
    if ((D_808BC000 < this->unk_1D0) && (gSaveContext.nextCutsceneIndex == 0xFFEF)) {
        func_808BA10C(this, play);
    }

    if (this->unk_1D0 > 140) {
        play->envCtx.fillScreen = true;
        temp_f0 = (this->unk_1D0 - 140) / 20.0f;
        if (temp_f0 > 1.0f) {
            temp_f0 = 1.0f;
        }
        play->envCtx.screenFillColor[0] = 160;
        play->envCtx.screenFillColor[1] = 160;
        play->envCtx.screenFillColor[2] = 160;
        play->envCtx.screenFillColor[3] = 255.0f * temp_f0;
    }

    Lights_PointNoGlowSetInfo(&this->unk_1E0, player->actor.world.pos.x + 10.0f, player->actor.world.pos.y + 10.0f,
                              player->actor.world.pos.z + 10.0f, 235, 255, 255, 255);
    Lights_PointNoGlowSetInfo(&this->unk_1F4, player->actor.world.pos.x - 10.0f, player->actor.world.pos.y - 10.0f,
                              player->actor.world.pos.z - 10.0f, 235, 255, 255, 255);
    Math_SmoothStepToF(&this->dyna.actor.shape.yOffset, 800.0f, 0.2f, 15.0f, 0.01f);
    this->dyna.actor.shape.rot.y += 0x320;
    Math_SmoothStepToF(&this->unk_1BC, 1.13f, 0.2f, 0.1f, 0.01f);
    Math_StepToF(&this->unk_1A4, 2.0f, 0.003f);
    Math_StepToF(&this->unk_1A8, 10.0f, 0.006f);
    Math_SmoothStepToF(&this->unk_1B0, 0.0f, 0.2f, 3.0f, 0.01f);
    Math_SmoothStepToF(&this->unk_1B4, 0.0f, 0.2f, 2.0f, 0.01f);
    Math_SmoothStepToF(&this->unk_1B8, 255.0f, 0.1f, 1.0f, 0.01f);

    tempS = D_808BC000;
    if (1) {}
    temp_f0 = 1.0f - ((f32)(D_808BC000 - this->unk_1D0) / ((D_808BC000 - tempS) + 100));
    if (temp_f0 > 0.0f) {
        for (i = 0; i < ARRAY_COUNT(play->envCtx.adjLightSettings.light1Color); i++) {
            play->envCtx.adjLightSettings.ambientColor[i] = play->envCtx.adjLightSettings.fogColor[i] =
                play->envCtx.adjLightSettings.light1Color[i] = -255.0f * temp_f0;
        }

        play->envCtx.adjLightSettings.fogNear = -500.0f * temp_f0;
        if (play->envCtx.adjLightSettings.fogNear < -300) {
            play->roomCtx.curRoom.segment = NULL;
        }
    }
}

void func_808BAAF4(DoorWarp1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 csId;
    f32 phi_f2;

    phi_f2 = 200.0f;
    if (play->sceneId == SCENE_SEA) {
        phi_f2 = 85.0f;
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_86_80) && (fabsf(this->dyna.actor.xzDistToPlayer) < phi_f2) &&
        ((player->actor.world.pos.y - 20.0f) < this->dyna.actor.world.pos.y) &&
        (this->dyna.actor.world.pos.y < (player->actor.world.pos.y + 20.0f))) {
        csId = this->dyna.actor.csId;

        if (CutsceneManager_IsNext(csId)) {
            CutsceneManager_Start(csId, &this->dyna.actor);
            SET_WEEKEVENTREG(WEEKEVENTREG_86_80);
            DoorWarp1_SetupAction(this, func_808BABF4);
        } else {
            CutsceneManager_Queue(csId);
        }
    }
}

void func_808BABF4(DoorWarp1* this, PlayState* play) {
}

void DoorWarp1_Update(Actor* thisx, PlayState* play) {
    DoorWarp1* this = (DoorWarp1*)thisx;

    if (this->unk_203 == 0) {
        this->unk_204 = 1.0f;
    }

    if ((this->unk_1A0 != NULL) && (this->unk_1A0->unk15C != this->unk_204)) {
        this->unk_1A0->actor.world.pos.y = this->dyna.actor.world.pos.y;
        this->unk_1A0->unk158 = this->dyna.actor.world.pos.y;
        this->unk_1A0->unk15C = this->unk_204;
    }

    this->actionFunc(this, play);

    if (DOORWARP1_GET_FF(&this->dyna.actor) < ENDOORWARP1_FF_2) {
        Actor_SetScale(&this->dyna.actor, this->unk_1C4 / 100.0f);
    }
}

void func_808BACCC(DoorWarp1* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0xFF, 0xFF, 200, 255, 255, (u8)this->unk_1B8);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 100, 255, (u8)this->unk_1B8);

    POLY_XLU_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL,
                                   &this->dyna.actor, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);

    SkelAnime_Update(&this->skelAnime);
}

void func_808BAE9C(DoorWarp1* this, PlayState* play) {
    s32 pad;
    s32 pad2;
    s32 sp94 = play->state.frames * 10;
    f32 sp90;
    f32 sp8C;
    f32 phi_f12;
    f32 sp84;

    if (this->unk_1A4 >= 1.0f) {
        sp90 = 0.0f;
    } else {
        sp90 = 1.0f - this->unk_1A4;
    }

    if (this->unk_1A8 >= 1.0f) {
        sp8C = 0.0f;
    } else {
        sp8C = 1.0f - this->unk_1A8;
    }

    OPEN_DISPS(play->state.gfxCtx);

    sp84 = 1.0f - ((2.0f - this->unk_1A4) / 1.7f);
    this->unk_1AC += sp84 * 15.0f;
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255.0f * sp84, 255, 255, (u8)this->unk_1B4);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 255.0f * sp84, 255, 255);

    Matrix_Translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y + 1.0f, this->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    phi_f12 = 1.0f;
    if (this->unk_203 != 0) {
        phi_f12 = this->unk_204 * phi_f12;
    }
    Matrix_Scale(phi_f12, phi_f12, phi_f12, MTXMODE_APPLY);

    gSPSegment(POLY_XLU_DISP++, 0x0A, Matrix_Finalize(play->state.gfxCtx));
    Matrix_Push();
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, sp94 & 0xFF, -(TRUNCF_BINANG(2.0f * this->unk_1AC) & 0x1FF),
                                  0x100, 0x100, 1, sp94 & 0xFF, -(TRUNCF_BINANG(2.0f * this->unk_1AC) & 0x1FF), 0x100,
                                  0x100, 10, -2, 10, -2));

    Matrix_Translate(0.0f, this->unk_1A4 * 230.0f, 0.0f, MTXMODE_APPLY);
    Matrix_Scale(((this->unk_1C6 * sp90) / 100.0f) + 1.0f, 1.0f, ((this->unk_1C6 * sp90) / 100.0f) + 1.0f,
                 MTXMODE_APPLY);

    gSPSegment(POLY_XLU_DISP++, 0x09, Matrix_Finalize(play->state.gfxCtx));
    gSPDisplayList(POLY_XLU_DISP++, gWarpPortalDL);

    Matrix_Pop();

    if (this->unk_1B0 > 0.0f) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255.0f * sp84, 255, 255, (u8)this->unk_1B0);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 255.0f * sp84, 255, 255);

        sp94 *= 2;
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, sp94 & 0xFF, -(TRUNCF_BINANG(this->unk_1AC) & 0x1FF),
                                      0x100, 0x100, 1, sp94 & 0xFF, -(TRUNCF_BINANG(this->unk_1AC) & 0x1FF), 0x100,
                                      0x100, 10, -1, 10, -1));

        Matrix_Translate(0.0f, this->unk_1A8 * 60.0f, 0.0f, MTXMODE_APPLY);
        Matrix_Scale(((this->unk_1C8 * sp8C) / 100.0f) + 1.0f, 1.0f, ((this->unk_1C8 * sp8C) / 100.0f) + 1.0f,
                     MTXMODE_APPLY);

        gSPSegment(POLY_XLU_DISP++, 0x09, Matrix_Finalize(play->state.gfxCtx));
        gSPDisplayList(POLY_XLU_DISP++, gWarpPortalDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_808BB4C4(DoorWarp1* this, PlayState* play) {
    Gfx_DrawDListOpa(play, gWarpBossWarpPlatformDL);
}

void func_808BB4F4(DoorWarp1* this, PlayState* play2) {
    PlayState* play = play2;
    s32 pad;
    Color_RGB8 sp64[] = {
        { 50, 150, 0 },
        { 100, 150, 200 },
        { 0, 50, 200 },
        { 120, 150, 0 },
    };
    s32 sp60 = 0;

    if (this->unk_1D4 != 0) {
        Matrix_Translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y + this->unk_1A4,
                         this->dyna.actor.world.pos.z, MTXMODE_NEW);
        Matrix_Scale(4.0f, this->unk_1AC, 4.0f, MTXMODE_APPLY);
        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gWarpBossWarpActivationBeamTexAnim));
        Gfx_DrawDListXlu(play, gWarpBossWarpActivationBeamDL);
        return;
    }

    if (play->sceneId != SCENE_MITURIN) {
        if (play->sceneId == SCENE_HAKUGIN) {
            sp60 = 1;
        } else if (play->sceneId == SCENE_SEA) {
            sp60 = 2;
        } else if (play->sceneId == SCENE_INISIE_R) {
            sp60 = 3;
        }
    }

    Matrix_Translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateYS(this->dyna.actor.world.rot.y, MTXMODE_APPLY);
    Matrix_Scale(1.0f, this->unk_1A8, 1.0f, MTXMODE_APPLY);
    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gWarpBossWarpLightShaftsTexAnim));

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetEnvColor(POLY_XLU_DISP++, sp64[sp60].r, sp64[sp60].g, sp64[sp60].b, 255);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, 255);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gWarpBossWarpLightShaftsDL);

    CLOSE_DISPS(play->state.gfxCtx);

    OPEN_DISPS(play->state.gfxCtx);

    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gWarpBossWarpGlowTexAnim));
    Matrix_Translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateYS(this->dyna.actor.world.rot.y, MTXMODE_APPLY);
    Matrix_Scale(1.0f, 0.0f, 1.0f, MTXMODE_APPLY);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gDPSetEnvColor(POLY_XLU_DISP++, sp64[sp60].r, sp64[sp60].g, sp64[sp60].b, 255);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, this->unk_203);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gWarpBossWarpGlowDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void DoorWarp1_Draw(Actor* thisx, PlayState* play) {
    DoorWarp1* this = (DoorWarp1*)thisx;

    switch (DOORWARP1_GET_FF(&this->dyna.actor)) {
        case ENDOORWARP1_FF_0:
            func_808BAE9C(this, play);
            break;

        case ENDOORWARP1_FF_1:
            func_808BACCC(this, play);
            func_808BAE9C(this, play);
            break;

        case ENDOORWARP1_FF_2:
        case ENDOORWARP1_FF_3:
        case ENDOORWARP1_FF_4:
        case ENDOORWARP1_FF_5:
            func_808BB4C4(this, play);
            break;

        case ENDOORWARP1_FF_6:
            func_808BB4F4(this, play);
            break;
    }
}

void func_808BB8D4(DoorWarp1* this, PlayState* play, s32 arg2) {
    static Vec3f sVelocity = { 0.0f, 0.05f, 0.0f };
    static Vec3f sAccel = { 0.0f, 0.25f, 0.0f };
    Vec3f pos;
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;
    s32 i;
    s32 rand;
    s16 scale;
    s32 life;

    for (i = 0; i < arg2; i++) {
        rand = (Rand_ZeroOne() * 0x10000);

        pos.x = (-Math_SinS(rand - 0x8000) * 35.0f) + this->dyna.actor.world.pos.x;
        pos.y = this->dyna.actor.world.pos.y;
        pos.z = (Math_CosS(rand - 0x8000) * 35.0f) + this->dyna.actor.world.pos.z;

        primColor.r = 255;
        primColor.g = 255;
        primColor.b = 255;
        envColor.r = 200;
        envColor.g = 200;
        envColor.b = 255;

        sAccel.y = 0.25f;
        sVelocity.x = 0.0f;
        sVelocity.z = 0.0f;

        scale = (Rand_ZeroOne() * 1000.0f) + 800.0f;

        life = 27;
        if (arg2 >= 2) {
            life = 8;
            sAccel.y = 1.8f;
            sVelocity.x = -Math_SinS(rand - 0x8000) * 5.0f;
            sVelocity.z = Math_CosS(rand - 0x8000) * 5.0f;
        }
        EffectSsKirakira_SpawnDispersed(play, &pos, &sVelocity, &sAccel, &primColor, &envColor, scale, life);
    }
}

void DoorWarp1_Reset(void) {
    D_808BC000 = 0;
    D_808BC004 = 0;
}
