#include "global.h"
#include "vt.h"

#include <string.h>

#include "soh/Enhancements/gameconsole.h"
#include <libultraship/ImGuiImpl.h>
#include "soh/frame_interpolation.h"
#include "soh/Enhancements/debugconsole.h"
#include <overlays/actors/ovl_En_Niw/z_en_niw.h>

#include <time.h>

void* D_8012D1F0 = NULL;
//UNK_TYPE D_8012D1F4 = 0; // unused
Input* D_8012D1F8 = NULL;

TransitionUnk sTrnsnUnk;
s32 gTrnsnUnkState;
VisMono D_80161498;
Color_RGBA8_u32 D_801614B0;
FaultClient D_801614B8;
s16 D_801614C8;
#if 0
u64 D_801614D0[0xA00];
#endif

PlayState* gPlayState;

void func_800BC450(PlayState* play) {
    Camera_ChangeDataIdx(GET_ACTIVE_CAM(play), play->unk_1242B - 1);
}

void func_800BC490(PlayState* play, s16 point) {
    ASSERT(point == 1 || point == 2);

    play->unk_1242B = point;

    if ((YREG(15) != 0x10) && (gSaveContext.cutsceneIndex < 0xFFF0)) {
        Audio_PlaySoundGeneral((point == 1) ? NA_SE_SY_CAMERA_ZOOM_DOWN : NA_SE_SY_CAMERA_ZOOM_UP, &D_801333D4, 4,
                               &D_801333E0, &D_801333E0, &D_801333E8);
    }

    func_800BC450(play);
}

s32 func_800BC56C(PlayState* play, s16 arg1) {
    return (arg1 == play->unk_1242B);
}

// original name: "Game_play_shop_pr_vr_switch_set"
void func_800BC590(PlayState* play) {
    osSyncPrintf("Game_play_shop_pr_vr_switch_set()\n");

    if (YREG(15) == 0x10) {
        play->unk_1242B = 2;
    }
}

void func_800BC5E0(PlayState* play, s32 transitionType) {
    TransitionContext* transitionCtx = &play->transitionCtx;

    memset(transitionCtx,0,  sizeof(TransitionContext));

    transitionCtx->transitionType = transitionType;

    if ((transitionCtx->transitionType >> 5) == 1) {
        transitionCtx->init = TransitionCircle_Init;
        transitionCtx->destroy = TransitionCircle_Destroy;
        transitionCtx->start = TransitionCircle_Start;
        transitionCtx->isDone = TransitionCircle_IsDone;
        transitionCtx->draw = TransitionCircle_Draw;
        transitionCtx->update = TransitionCircle_Update;
        transitionCtx->setType = TransitionCircle_SetType;
        transitionCtx->setColor = TransitionCircle_SetColor;
        transitionCtx->setEnvColor = TransitionCircle_SetEnvColor;
    } else {
        switch (transitionCtx->transitionType) {
            case 1:
                transitionCtx->init = TransitionTriforce_Init;
                transitionCtx->destroy = TransitionTriforce_Destroy;
                transitionCtx->start = TransitionTriforce_Start;
                transitionCtx->isDone = TransitionTriforce_IsDone;
                transitionCtx->draw = TransitionTriforce_Draw;
                transitionCtx->update = TransitionTriforce_Update;
                transitionCtx->setType = TransitionTriforce_SetType;
                transitionCtx->setColor = TransitionTriforce_SetColor;
                transitionCtx->setEnvColor = NULL;
                break;
            case 0:
            case 8:
                transitionCtx->init = TransitionWipe_Init;
                transitionCtx->destroy = TransitionWipe_Destroy;
                transitionCtx->start = TransitionWipe_Start;
                transitionCtx->isDone = TransitionWipe_IsDone;
                transitionCtx->draw = TransitionWipe_Draw;
                transitionCtx->update = TransitionWipe_Update;
                transitionCtx->setType = TransitionWipe_SetType;
                transitionCtx->setColor = TransitionWipe_SetColor;
                transitionCtx->setEnvColor = NULL;
                break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 13:
            case 17:
            case 18:
            case 19:
                transitionCtx->init = TransitionFade_Init;
                transitionCtx->destroy = TransitionFade_Destroy;
                transitionCtx->start = TransitionFade_Start;
                transitionCtx->isDone = TransitionFade_IsDone;
                transitionCtx->draw = TransitionFade_Draw;
                transitionCtx->update = TransitionFade_Update;
                transitionCtx->setType = TransitionFade_SetType;
                transitionCtx->setColor = TransitionFade_SetColor;
                transitionCtx->setEnvColor = NULL;
                break;
            case 9:
            case 10:
                play->transitionMode = 4;
                break;
            case 11:
                play->transitionMode = 10;
                break;
            case 12:
                play->transitionMode = 7;
                break;
            case 14:
                play->transitionMode = 12;
                break;
            case 15:
                play->transitionMode = 14;
                break;
            case 16:
                play->transitionMode = 16;
                break;
            default:
                Fault_AddHungupAndCrash(__FILE__, __LINE__);
                break;
        }
    }
}

void func_800BC88C(PlayState* play) {
    play->transitionCtx.transitionType = -1;
}

Gfx* Play_SetFog(PlayState* play, Gfx* gfx) {
    return Gfx_SetFog2(gfx, play->lightCtx.fogColor[0], play->lightCtx.fogColor[1],
                       play->lightCtx.fogColor[2], 0, play->lightCtx.fogNear, 1000);
}

void Play_Destroy(GameState* thisx) {
    PlayState* play = (PlayState*)thisx;
    Player* player = GET_PLAYER(play);

    play->state.gfxCtx->callback = NULL;
    play->state.gfxCtx->callbackParam = 0;
    SREG(91) = 0;
    R_PAUSE_MENU_MODE = 0;

    PreRender_Destroy(&play->pauseBgPreRender);
    Effect_DeleteAll(play);
    EffectSs_ClearAll(play);
    CollisionCheck_DestroyContext(play, &play->colChkCtx);

    if (gTrnsnUnkState == 3) {
        TransitionUnk_Destroy(&sTrnsnUnk);
        gTrnsnUnkState = 0;
    }

    if (play->transitionMode == 3) {
        play->transitionCtx.destroy(&play->transitionCtx.data);
        func_800BC88C(play);
        play->transitionMode = 0;
    }

    ShrinkWindow_Destroy();
    TransitionFade_Destroy(&play->transitionFade);
    VisMono_Destroy(&D_80161498);

    if (gSaveContext.linkAge != play->linkAgeOnLoad) {
        Inventory_SwapAgeEquipment();
        Player_SetEquipmentData(play, player);
    }

    func_80031C3C(&play->actorCtx, play);
    func_80110990(play);
    KaleidoScopeCall_Destroy(play);
    KaleidoManager_Destroy();
    ZeldaArena_Cleanup();
    Fault_RemoveClient(&D_801614B8);
    disableBetaQuest();
    gPlayState = NULL;
}

void GivePlayerRandoRewardSongOfTime(PlayState* play, RandomizerCheck check) {
    Player* player = GET_PLAYER(play);

    if (gSaveContext.entranceIndex == 0x050F && player != NULL && !Player_InBlockingCsMode(play, player) &&
        !Flags_GetTreasure(play, 0x1F) && gSaveContext.nextTransition == 0xFF && !gSaveContext.pendingIceTrapCount) {
        GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(check, RG_SONG_OF_TIME);
        GiveItemEntryWithoutActor(play, getItemEntry);
        player->pendingFlag.flagID = 0x1F;
        player->pendingFlag.flagType = FLAG_SCENE_TREASURE;
    }
}

void GivePlayerRandoRewardNocturne(PlayState* play, RandomizerCheck check) {
    Player* player = GET_PLAYER(play);

    if ((gSaveContext.entranceIndex == 0x00DB ||
         gSaveContext.entranceIndex == 0x0191 ||
         gSaveContext.entranceIndex == 0x0195) && LINK_IS_ADULT && CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST) &&
        CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && CHECK_QUEST_ITEM(QUEST_MEDALLION_WATER) && player != NULL &&
        !Player_InBlockingCsMode(play, player) && !Flags_GetEventChkInf(0xAA)) {
        GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(check, RG_NOCTURNE_OF_SHADOW);
        GiveItemEntryWithoutActor(play, getItemEntry);
        player->pendingFlag.flagID = 0xAA;
        player->pendingFlag.flagType = FLAG_EVENT_CHECK_INF;
    }
}

void GivePlayerRandoRewardRequiem(PlayState* play, RandomizerCheck check) {
    Player* player = GET_PLAYER(play);

    if ((gSaveContext.gameMode == 0) && (gSaveContext.respawnFlag <= 0) && (gSaveContext.cutsceneIndex < 0xFFF0)) {
        if ((gSaveContext.entranceIndex == 0x01E1) && !Flags_GetEventChkInf(0xAC) && player != NULL &&
            !Player_InBlockingCsMode(play, player)) {
            GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(check, RG_SONG_OF_TIME);
            GiveItemEntryWithoutActor(play, getItemEntry);
            player->pendingFlag.flagID = 0xAC;
            player->pendingFlag.flagType = FLAG_EVENT_CHECK_INF;
        }
    }
}

u8 CheckStoneCount() {
    u8 stoneCount = 0;

    if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
        stoneCount++;
    }

    if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        stoneCount++;
    }

    if (CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
        stoneCount++;
    }

    return stoneCount;
}

u8 CheckMedallionCount() {
    u8 medallionCount = 0;

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
        medallionCount++;
    }

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) {
        medallionCount++;
    }

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_WATER)) {
        medallionCount++;
    }

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW)) {
        medallionCount++;
    }

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
        medallionCount++;
    }

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_LIGHT)) {
        medallionCount++;
    }

    return medallionCount;
}

u8 CheckDungeonCount() {
    u8 dungeonCount = 0;

    if (Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_DEKU_TREE)) {
        dungeonCount++;
    }

    if (Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_DODONGOS_CAVERN)) {
        dungeonCount++;
    }

    if (Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_JABU_JABUS_BELLY)) {
        dungeonCount++;
    }

    if (Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_FOREST_TEMPLE)) {
        dungeonCount++;
    }

    if (Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_FIRE_TEMPLE)) {
        dungeonCount++;
    }

    if (Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_WATER_TEMPLE)) {
        dungeonCount++;
    }

    if (Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_SPIRIT_TEMPLE)) {
        dungeonCount++;
    }

    if (Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_SHADOW_TEMPLE)) {
        dungeonCount++;
    }

    return dungeonCount;
}

void GivePlayerRandoRewardZeldaLightArrowsGift(PlayState* play, RandomizerCheck check) {
    Player* player = GET_PLAYER(play);

    u8 meetsRequirements = 0;

    switch (Randomizer_GetSettingValue(RSK_GANONS_BOSS_KEY)) {
        case 7:
            if (CheckMedallionCount() >= Randomizer_GetSettingValue(RSK_LACS_MEDALLION_COUNT)) {
                meetsRequirements = true;
            }
            break;
        case 8:
            if (CheckStoneCount() >= Randomizer_GetSettingValue(RSK_LACS_STONE_COUNT)) {
                meetsRequirements = true;
            }
            break;
        case 9:
            if ((CheckMedallionCount() + CheckStoneCount()) >= Randomizer_GetSettingValue(RSK_LACS_REWARD_COUNT)) {
                meetsRequirements = true;
            }
            break;
        case 10:
            if (CheckDungeonCount() >= Randomizer_GetSettingValue(RSK_LACS_DUNGEON_COUNT)) {
                meetsRequirements = true;
            }
            break;
        case 11:
            if (gSaveContext.inventory.gsTokens >= Randomizer_GetSettingValue(RSK_LACS_TOKEN_COUNT)) {
                meetsRequirements = true;
            }
            break;
        default:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) && CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW)) {
                meetsRequirements = true;
            }
            break;
    }

    if (meetsRequirements && LINK_IS_ADULT &&
        (gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_TOKINOMA) &&
        !Flags_GetTreasure(play, 0x1E) && player != NULL && !Player_InBlockingCsMode(play, player) &&
        play->sceneLoadFlag == 0) {
        GetItemEntry getItem = Randomizer_GetItemFromKnownCheck(check, GI_ARROW_LIGHT);
        if (GiveItemEntryWithoutActor(play, getItem)) {
            player->pendingFlag.flagID = 0x1E;
            player->pendingFlag.flagType = FLAG_SCENE_TREASURE;
        }
    }
}

void GivePlayerRandoRewardSariaGift(PlayState* play, RandomizerCheck check) {
    Player* player = GET_PLAYER(play);
    if (gSaveContext.entranceIndex == 0x05E0) {
        GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(check, RG_ZELDAS_LULLABY);

        if (!Flags_GetEventChkInf(0xC1) && player != NULL && !Player_InBlockingCsMode(play, player)) {
            GiveItemEntryWithoutActor(play, getItemEntry);
            player->pendingFlag.flagType = FLAG_EVENT_CHECK_INF;
            player->pendingFlag.flagID = 0xC1;
        }
    }
}

void Play_Init(GameState* thisx) {
    PlayState* play = (PlayState*)thisx;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    enableBetaQuest();
    gPlayState = play;
    //play->state.gfxCtx = NULL;
    uintptr_t zAlloc;
    uintptr_t zAllocAligned;
    size_t zAllocSize;
    Player* player;
    s32 playerStartCamId;
    s32 i;
    u8 tempSetupIndex;
    s32 pad[2];

    // Skip Child Stealth when option is enabled, Zelda's Letter isn't obtained and Impa's reward hasn't been received
    // eventChkInf[4] & 1 = Got Zelda's Letter
    // eventChkInf[5] & 0x200 = Got Impa's reward
    // entranceIndex 0x7A, Castle Courtyard - Day from crawlspace
    // entranceIndex 0x400, Zelda's Courtyard
    if (gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SKIP_CHILD_STEALTH) &&
        !(gSaveContext.eventChkInf[4] & 1) && !(gSaveContext.eventChkInf[5] & 0x200)) {
        if (gSaveContext.entranceIndex == 0x7A) {
            gSaveContext.entranceIndex = 0x400;
        }
    }

    if (gSaveContext.entranceIndex == -1) {
        gSaveContext.entranceIndex = 0;
        play->state.running = false;
        SET_NEXT_GAMESTATE(&play->state, Opening_Init, OpeningContext);
        return;
    }

    SystemArena_Display();
    // OTRTODO allocate double the normal amount of memory
    // This is to avoid some parts of the game, like loading actors, causing OoM
    // This is potionally unavoidable due to struct size differences, but is x2 the right amount?
    GameState_Realloc(&play->state, 0x1D4790 * 2);
    KaleidoManager_Init(play);
    View_Init(&play->view, gfxCtx);
    Audio_SetExtraFilter(0);
    Quake_Init();

    for (i = 0; i < ARRAY_COUNT(play->cameraPtrs); i++) {
        play->cameraPtrs[i] = NULL;
    }

    Camera_Init(&play->mainCamera, &play->view, &play->colCtx, play);
    Camera_ChangeStatus(&play->mainCamera, CAM_STAT_ACTIVE);

    for (i = 0; i < 3; i++) {
        Camera_Init(&play->subCameras[i], &play->view, &play->colCtx, play);
        Camera_ChangeStatus(&play->subCameras[i], CAM_STAT_UNK100);
    }

    play->cameraPtrs[MAIN_CAM] = &play->mainCamera;
    play->cameraPtrs[MAIN_CAM]->uid = 0;
    play->activeCamera = MAIN_CAM;
    func_8005AC48(&play->mainCamera, 0xFF);
    func_80112098(play);
    Message_Init(play);
    GameOver_Init(play);
    SoundSource_InitAll(play);
    Effect_InitContext(play);
    EffectSs_InitInfo(play, 0x55);
    CollisionCheck_InitContext(play, &play->colChkCtx);
    AnimationContext_Reset(&play->animationCtx);
    func_8006450C(play, &play->csCtx);

    if (gSaveContext.nextCutsceneIndex != 0xFFEF) {
        gSaveContext.cutsceneIndex = gSaveContext.nextCutsceneIndex;
        gSaveContext.nextCutsceneIndex = 0xFFEF;
    }

    if (gSaveContext.cutsceneIndex == 0xFFFD) {
        gSaveContext.cutsceneIndex = 0;
    }

    if (gSaveContext.nextDayTime != 0xFFFF) {
        gSaveContext.dayTime = gSaveContext.nextDayTime;
        gSaveContext.skyboxTime = gSaveContext.nextDayTime;
    }

    if (gSaveContext.dayTime > 0xC000 || gSaveContext.dayTime < 0x4555) {
        gSaveContext.nightFlag = 1;
    } else {
        gSaveContext.nightFlag = 0;
    }

    Cutscene_HandleConditionalTriggers(play);

    if (gSaveContext.gameMode != 0 || gSaveContext.cutsceneIndex >= 0xFFF0) {
        gSaveContext.nayrusLoveTimer = 0;
        func_800876C8(play);
        gSaveContext.sceneSetupIndex = (gSaveContext.cutsceneIndex & 0xF) + 4;
    } else if (!LINK_IS_ADULT && IS_DAY) {
        gSaveContext.sceneSetupIndex = 0;
    } else if (!LINK_IS_ADULT && !IS_DAY) {
        gSaveContext.sceneSetupIndex = 1;
    } else if (LINK_IS_ADULT && IS_DAY) {
        gSaveContext.sceneSetupIndex = 2;
    } else {
        gSaveContext.sceneSetupIndex = 3;
    }

    tempSetupIndex = gSaveContext.sceneSetupIndex;
    if ((gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_SPOT00) && !LINK_IS_ADULT &&
        gSaveContext.sceneSetupIndex < 4) {
        if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) && CHECK_QUEST_ITEM(QUEST_GORON_RUBY) &&
            CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)) {
            gSaveContext.sceneSetupIndex = 1;
        } else {
            gSaveContext.sceneSetupIndex = 0;
        }
    } else if ((gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_SPOT04) && LINK_IS_ADULT &&
               gSaveContext.sceneSetupIndex < 4) {
        gSaveContext.sceneSetupIndex = (gSaveContext.eventChkInf[4] & 0x100) ? 3 : 2;
    }

    Play_SpawnScene(
        play,
        gEntranceTable[((void)0, gSaveContext.entranceIndex) + ((void)0, gSaveContext.sceneSetupIndex)].scene,
        gEntranceTable[((void)0, gSaveContext.sceneSetupIndex) + ((void)0, gSaveContext.entranceIndex)].spawn);
    osSyncPrintf("\nSCENE_NO=%d COUNTER=%d\n", ((void)0, gSaveContext.entranceIndex), gSaveContext.sceneSetupIndex);

    Cutscene_HandleEntranceTriggers(play);
    KaleidoScopeCall_Init(play);
    func_801109B0(play);

    if (gSaveContext.nextDayTime != 0xFFFF) {
        if (gSaveContext.nextDayTime == 0x8001) {
            gSaveContext.totalDays++;
            gSaveContext.bgsDayCount++;
            gSaveContext.dogIsLost = true;
            if (Inventory_ReplaceItem(play, ITEM_WEIRD_EGG, ITEM_CHICKEN) ||
                Inventory_HatchPocketCucco(play)) {
                Message_StartTextbox(play, 0x3066, NULL);
            }
            gSaveContext.nextDayTime = 0xFFFE;
        } else {
            gSaveContext.nextDayTime = 0xFFFD;
        }
    }

    SREG(91) = -1;
    R_PAUSE_MENU_MODE = 0;
    PreRender_Init(&play->pauseBgPreRender);
    PreRender_SetValuesSave(&play->pauseBgPreRender, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0);
    PreRender_SetValues(&play->pauseBgPreRender, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    gTrnsnUnkState = 0;
    play->transitionMode = 0;

    if (CVar_GetS32("gSceneTransitions", 255)!= 255){
        play->transitionMode = CVar_GetS32("gSceneTransitions", 0);
        gSaveContext.nextTransition = CVar_GetS32("gSceneTransitions", 0);
        play->fadeTransition = CVar_GetS32("gSceneTransitions", 0);
    }

    FrameAdvance_Init(&play->frameAdvCtx);
    Rand_Seed((u32)osGetTime());
    Matrix_Init(&play->state);
    play->state.main = Play_Main;
    play->state.destroy = Play_Destroy;
    play->sceneLoadFlag = -0x14;
    play->unk_11E16 = 0xFF;
    play->unk_11E18 = 0;
    play->unk_11DE9 = 0;

    if (gSaveContext.gameMode != 1) {
        if (gSaveContext.nextTransition == 0xFF) {
            play->fadeTransition =
                (gEntranceTable[((void)0, gSaveContext.entranceIndex) + tempSetupIndex].field >> 7) & 0x7F; // Fade In
        } else {
            play->fadeTransition = gSaveContext.nextTransition;
            gSaveContext.nextTransition = 0xFF;
        }
    } else {
        play->fadeTransition = 6;
    }

    ShrinkWindow_Init();
    TransitionFade_Init(&play->transitionFade);
    TransitionFade_SetType(&play->transitionFade, 3);
    TransitionFade_SetColor(&play->transitionFade, RGBA8(160, 160, 160, 255));
    TransitionFade_Start(&play->transitionFade);
    VisMono_Init(&D_80161498);
    D_801614B0.a = 0;
    Flags_UnsetAllEnv(play);

    osSyncPrintf("ZELDA ALLOC SIZE=%x\n", THA_GetSize(&play->state.tha));
    zAllocSize = THA_GetSize(&play->state.tha);
    zAlloc = GAMESTATE_ALLOC_MC(&play->state, zAllocSize);
    zAllocAligned = (zAlloc + 8) & ~0xF;
    ZeldaArena_Init(zAllocAligned, zAllocSize - zAllocAligned + zAlloc);
    // "Zelda Heap"
    osSyncPrintf("ゼルダヒープ %08x-%08x\n", zAllocAligned,
                 (s32)(zAllocAligned + zAllocSize) - (s32)(zAllocAligned - zAlloc));

    Fault_AddClient(&D_801614B8, ZeldaArena_Display, NULL, NULL);
    func_800304DC(play, &play->actorCtx, play->linkActorEntry);

    while (!func_800973FC(play, &play->roomCtx)) {
        ; // Empty Loop
    }

    player = GET_PLAYER(play);
    Camera_InitPlayerSettings(&play->mainCamera, player);
    Camera_ChangeMode(&play->mainCamera, CAM_MODE_NORMAL);

    // OTRTODO: Bounds check cameraDataList to guard against scenes spawning the player with
    // an out of bounds background camera index. This requires adding an extra field to the
    // CollisionHeader struct to save the length of cameraDataList.
    // Fixes Dodongo's Cavern blue warp crash.
    {
        CollisionHeader* colHeader = BgCheck_GetCollisionHeader(&play->colCtx, BGCHECK_SCENE);

        // If the player's start cam is out of bounds, set it to 0xFF so it isn't used.
        if (colHeader != NULL && ((player->actor.params & 0xFF) >= colHeader->cameraDataListLen)) {
            player->actor.params |= 0xFF;
        }
    }

    playerStartCamId = player->actor.params & 0xFF;
    if (playerStartCamId != 0xFF) {
        osSyncPrintf("player has start camera ID (" VT_FGCOL(BLUE) "%d" VT_RST ")\n", playerStartCamId);
        Camera_ChangeDataIdx(&play->mainCamera, playerStartCamId);
    }

    if (YREG(15) == 32) {
        play->unk_1242B = 2;
    } else if (YREG(15) == 16) {
        play->unk_1242B = 1;
    } else {
        play->unk_1242B = 0;
    }

    Interface_SetSceneRestrictions(play);
    Environment_PlaySceneSequence(play);
    gSaveContext.seqId = play->sequenceCtx.seqId;
    gSaveContext.natureAmbienceId = play->sequenceCtx.natureAmbienceId;
    func_8002DF18(play, GET_PLAYER(play));
    AnimationContext_Update(play, &play->animationCtx);
    gSaveContext.respawnFlag = 0;
    #if 0
    if (dREG(95) != 0) {
        D_8012D1F0 = D_801614D0;
        osSyncPrintf("\nkawauso_data=[%x]", D_8012D1F0);
        DmaMgr_DmaRomToRam(0x03FEB000, D_8012D1F0, sizeof(D_801614D0));
    }
    #endif
}

void Play_Update(PlayState* play) {
    s32 pad1;
    s32 sp80;
    Input* input;
    u32 i;
    s32 pad2;

    input = play->state.input;

    if ((SREG(1) < 0) || (DREG(0) != 0)) {
        SREG(1) = 0;
        ZeldaArena_Display();
    }

    if ((HREG(80) == 18) && (HREG(81) < 0)) {
        HREG(81) = 0;
        osSyncPrintf("object_exchange_rom_address %u\n", gObjectTableSize);
        osSyncPrintf("RomStart RomEnd   Size\n");
        for (i = 0; i < gObjectTableSize; i++) {
            ptrdiff_t size = gObjectTable[i].vromEnd - gObjectTable[i].vromStart;

            osSyncPrintf("%08x-%08x %08x(%8.3fKB)\n", gObjectTable[i].vromStart, gObjectTable[i].vromEnd, size,
                         size / 1024.0f);
        }
        osSyncPrintf("\n");
    }

    if ((HREG(81) == 18) && (HREG(82) < 0)) {
        HREG(82) = 0;
        ActorOverlayTable_LogPrint();
    }

    if (CVar_GetS32("gFreeCamera", 0) && Player_InCsMode(play)) {
        play->manualCamera = false;
    }

    gSegments[4] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[play->objectCtx.mainKeepIndex].segment);
    gSegments[5] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[play->objectCtx.subKeepIndex].segment);
    gSegments[2] = VIRTUAL_TO_PHYSICAL(play->sceneSegment);

    if (FrameAdvance_Update(&play->frameAdvCtx, &input[1])) {
        if ((play->transitionMode == 0) && (play->sceneLoadFlag != 0)) {
            play->transitionMode = 1;
        }

        if (gTrnsnUnkState != 0) {
            switch (gTrnsnUnkState) {
                case 2:
                    if (TransitionUnk_Init(&sTrnsnUnk, 10, 7) == NULL) {
                        osSyncPrintf("fbdemo_init呼出し失敗！\n"); // "fbdemo_init call failed!"
                        gTrnsnUnkState = 0;
                    } else {
                        sTrnsnUnk.zBuffer = (u16*)gZBuffer;
                        gTrnsnUnkState = 3;
                        R_UPDATE_RATE = 1;
                    }
                    break;
                case 3:
                    func_800B23E8(&sTrnsnUnk);
                    break;
            }
        }

        if (play->transitionMode) {
            switch (play->transitionMode) {
                case 1:
                    if (play->sceneLoadFlag != -0x14) {
                        s16 sp6E = 0;
                        Interface_ChangeAlpha(1);

                        if (gSaveContext.cutsceneIndex >= 0xFFF0) {
                            sp6E = (gSaveContext.cutsceneIndex & 0xF) + 4;
                        }

                        if (!(gEntranceTable[play->nextEntranceIndex + sp6E].field & 0x8000)) { // Continue BGM Off
                            // "Sound initalized. 111"
                            osSyncPrintf("\n\n\nサウンドイニシャル来ました。111");
                            if ((play->fadeTransition < 56) && !Environment_IsForcedSequenceDisabled()) {
                                // "Sound initalized. 222"
                                osSyncPrintf("\n\n\nサウンドイニシャル来ました。222");
                                func_800F6964(0x14);
                                gSaveContext.seqId = (u8)NA_BGM_DISABLED;
                                gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
                            }
                        }
                    }

                    if (CREG(11) == 0) {
                        func_800BC5E0(play, play->fadeTransition);
                    } else {
                        func_800BC5E0(play, CREG(12));
                    }

                    if (play->transitionMode >= 4) {
                        break;
                    }

                case 2:
                    play->transitionCtx.init(&play->transitionCtx.data);

                    if ((play->transitionCtx.transitionType >> 5) == 1) {
                        play->transitionCtx.setType(&play->transitionCtx.data,
                                                         play->transitionCtx.transitionType | 0x80);
                    }

                    gSaveContext.unk_1419 = 14;
                    if ((play->transitionCtx.transitionType == 8) ||
                        (play->transitionCtx.transitionType == 9)) {
                        gSaveContext.unk_1419 = 28;
                    }

                    gSaveContext.fadeDuration = 60;
                    if ((play->transitionCtx.transitionType == 4) ||
                        (play->transitionCtx.transitionType == 5)) {
                        gSaveContext.fadeDuration = 20;
                    } else if ((play->transitionCtx.transitionType == 6) ||
                               (play->transitionCtx.transitionType == 7)) {
                        gSaveContext.fadeDuration = 150;
                    } else if (play->transitionCtx.transitionType == 17) {
                        gSaveContext.fadeDuration = 2;
                    }

                    if ((play->transitionCtx.transitionType == 3) ||
                        (play->transitionCtx.transitionType == 5) ||
                        (play->transitionCtx.transitionType == 7) ||
                        (play->transitionCtx.transitionType == 13) ||
                        (play->transitionCtx.transitionType == 17)) {
                        play->transitionCtx.setColor(&play->transitionCtx.data, RGBA8(160, 160, 160, 255));
                        if (play->transitionCtx.setEnvColor != NULL) {
                            play->transitionCtx.setEnvColor(&play->transitionCtx.data,
                                                                 RGBA8(160, 160, 160, 255));
                        }
                    } else if (play->transitionCtx.transitionType == 18) {
                        play->transitionCtx.setColor(&play->transitionCtx.data, RGBA8(140, 140, 100, 255));
                        if (play->transitionCtx.setEnvColor != NULL) {
                            play->transitionCtx.setEnvColor(&play->transitionCtx.data,
                                                                 RGBA8(140, 140, 100, 255));
                        }
                    } else if (play->transitionCtx.transitionType == 19) {
                        play->transitionCtx.setColor(&play->transitionCtx.data, RGBA8(70, 100, 110, 255));
                        if (play->transitionCtx.setEnvColor != NULL) {
                            play->transitionCtx.setEnvColor(&play->transitionCtx.data,
                                                                 RGBA8(70, 100, 110, 255));
                        }
                    } else {
                        play->transitionCtx.setColor(&play->transitionCtx.data, RGBA8(0, 0, 0, 0));
                        if (play->transitionCtx.setEnvColor != NULL) {
                            play->transitionCtx.setEnvColor(&play->transitionCtx.data, RGBA8(0, 0, 0, 0));
                        }
                    }

                    if (play->sceneLoadFlag == -0x14) {
                        play->transitionCtx.setType(&play->transitionCtx.data, 1);
                    } else {
                        play->transitionCtx.setType(&play->transitionCtx.data, 2);
                    }

                    play->transitionCtx.start(&play->transitionCtx);

                    if (play->transitionCtx.transitionType == 13) {
                        play->transitionMode = 11;
                    } else {
                        play->transitionMode = 3;
                    }
                    break;

                case 3:
                    if (play->transitionCtx.isDone(&play->transitionCtx) != 0) {
                        if (play->transitionCtx.transitionType >= 56) {
                            if (play->sceneLoadFlag == -0x14) {
                                play->transitionCtx.destroy(&play->transitionCtx);
                                func_800BC88C(play);
                                play->transitionMode = 0;
                            }
                        } else if (play->sceneLoadFlag != -0x14) {
                            play->state.running = 0;
                            if (gSaveContext.gameMode != 2) {
                                SET_NEXT_GAMESTATE(&play->state, Play_Init, PlayState);
                                gSaveContext.entranceIndex = play->nextEntranceIndex;
                                if (gSaveContext.minigameState == 1) {
                                    gSaveContext.minigameState = 3;
                                }
                            } else {
                                SET_NEXT_GAMESTATE(&play->state, FileChoose_Init, FileChooseContext);
                            }
                        } else {
                            play->transitionCtx.destroy(&play->transitionCtx);
                            func_800BC88C(play);
                            play->transitionMode = 0;
                            if (gTrnsnUnkState == 3) {
                                TransitionUnk_Destroy(&sTrnsnUnk);
                                gTrnsnUnkState = 0;
                                R_UPDATE_RATE = 3;
                            }

                            // Don't autosave in grottos or cutscenes
                            if (CVar_GetS32("gAutosave", 0) && (play->sceneNum != SCENE_YOUSEI_IZUMI_TATE) && (play->sceneNum != SCENE_KAKUSIANA) && (gSaveContext.cutsceneIndex == 0)) {
                                Play_PerformSave(play);
                            }
                        }
                        play->sceneLoadFlag = 0;
                    } else {
                        play->transitionCtx.update(&play->transitionCtx.data, R_UPDATE_RATE);
                    }
                    break;
            }

            switch (play->transitionMode) {
                case 4:
                    D_801614C8 = 0;
                    play->envCtx.fillScreen = true;
                    play->envCtx.screenFillColor[0] = 160;
                    play->envCtx.screenFillColor[1] = 160;
                    play->envCtx.screenFillColor[2] = 160;
                    if (play->sceneLoadFlag != -0x14) {
                        play->envCtx.screenFillColor[3] = 0;
                        play->transitionMode = 5;
                    } else {
                        play->envCtx.screenFillColor[3] = 255;
                        play->transitionMode = 6;
                    }
                    break;

                case 5:
                    play->envCtx.screenFillColor[3] = (D_801614C8 / 20.0f) * 255.0f;
                    if (D_801614C8 >= 20 && 1) {
                        play->state.running = 0;
                        SET_NEXT_GAMESTATE(&play->state, Play_Init, PlayState);
                        gSaveContext.entranceIndex = play->nextEntranceIndex;
                        play->sceneLoadFlag = 0;
                        play->transitionMode = 0;
                    } else {
                        D_801614C8++;
                    }
                    break;

                case 6:
                    play->envCtx.screenFillColor[3] = (1 - D_801614C8 / 20.0f) * 255.0f;
                    if (D_801614C8 >= 20 && 1) {
                        gTrnsnUnkState = 0;
                        R_UPDATE_RATE = 3;
                        play->sceneLoadFlag = 0;
                        play->transitionMode = 0;
                        play->envCtx.fillScreen = false;
                    } else {
                        D_801614C8++;
                    }
                    break;

                case 7:
                    D_801614C8 = 0;
                    play->envCtx.fillScreen = true;
                    play->envCtx.screenFillColor[0] = 170;
                    play->envCtx.screenFillColor[1] = 160;
                    play->envCtx.screenFillColor[2] = 150;
                    if (play->sceneLoadFlag != -0x14) {
                        play->envCtx.screenFillColor[3] = 0;
                        play->transitionMode = 5;
                    } else {
                        play->envCtx.screenFillColor[3] = 255;
                        play->transitionMode = 6;
                    }
                    break;

                case 10:
                    if (play->sceneLoadFlag != -0x14) {
                        play->state.running = 0;
                        SET_NEXT_GAMESTATE(&play->state, Play_Init, PlayState);
                        gSaveContext.entranceIndex = play->nextEntranceIndex;
                        play->sceneLoadFlag = 0;
                        play->transitionMode = 0;
                    } else {
                        gTrnsnUnkState = 0;
                        R_UPDATE_RATE = 3;
                        play->sceneLoadFlag = 0;
                        play->transitionMode = 0;
                    }
                    break;

                case 11:
                    if (gSaveContext.unk_1410 != 0) {
                        play->transitionMode = 3;
                    }
                    break;

                case 12:
                    if (play->sceneLoadFlag != -0x14) {
                        play->envCtx.sandstormState = 1;
                        play->transitionMode = 13;
                    } else {
                        play->envCtx.sandstormState = 2;
                        play->envCtx.sandstormPrimA = 255;
                        play->envCtx.sandstormEnvA = 255;
                        play->transitionMode = 13;
                    }
                    break;

                case 13:
                    Audio_PlaySoundGeneral(NA_SE_EV_SAND_STORM - SFX_FLAG, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    if (play->sceneLoadFlag == -0x14) {
                        if (play->envCtx.sandstormPrimA < 110) {
                            gTrnsnUnkState = 0;
                            R_UPDATE_RATE = 3;
                            play->sceneLoadFlag = 0;
                            play->transitionMode = 0;
                        }
                    } else {
                        if (play->envCtx.sandstormEnvA == 255) {
                            play->state.running = 0;
                            SET_NEXT_GAMESTATE(&play->state, Play_Init, PlayState);
                            gSaveContext.entranceIndex = play->nextEntranceIndex;
                            play->sceneLoadFlag = 0;
                            play->transitionMode = 0;
                        }
                    }
                    break;

                case 14:
                    if (play->sceneLoadFlag == -0x14) {
                        play->envCtx.sandstormState = 4;
                        play->envCtx.sandstormPrimA = 255;
                        play->envCtx.sandstormEnvA = 255;
                        // "It's here!!!!!!!!!"
                        LOG_STRING("来た!!!!!!!!!!!!!!!!!!!!!");
                        play->transitionMode = 15;
                    } else {
                        play->transitionMode = 12;
                    }
                    break;

                case 15:
                    Audio_PlaySoundGeneral(NA_SE_EV_SAND_STORM - SFX_FLAG, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    if (play->sceneLoadFlag == -0x14) {
                        if (play->envCtx.sandstormPrimA <= 0) {
                            gTrnsnUnkState = 0;
                            R_UPDATE_RATE = 3;
                            play->sceneLoadFlag = 0;
                            play->transitionMode = 0;
                        }
                    }
                    break;

                case 16:
                    D_801614C8 = 0;
                    play->envCtx.fillScreen = true;
                    play->envCtx.screenFillColor[0] = 0;
                    play->envCtx.screenFillColor[1] = 0;
                    play->envCtx.screenFillColor[2] = 0;
                    play->envCtx.screenFillColor[3] = 255;
                    play->transitionMode = 17;
                    break;

                case 17:
                    if (gSaveContext.unk_1410 != 0) {
                        play->envCtx.screenFillColor[3] = gSaveContext.unk_1410;
                        if (gSaveContext.unk_1410 < 0x65) {
                            gTrnsnUnkState = 0;
                            R_UPDATE_RATE = 3;
                            play->sceneLoadFlag = 0;
                            play->transitionMode = 0;
                        }
                    }
                    break;
            }
        }

        if (1 && HREG(63)) {
            LOG_NUM("1", 1);
        }

        if (1 && (gTrnsnUnkState != 3)) {
            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            if ((gSaveContext.gameMode == 0) && (play->msgCtx.msgMode == MSGMODE_NONE) &&
                (play->gameOverCtx.state == GAMEOVER_INACTIVE)) {
                KaleidoSetup_Update(play);
            }

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            sp80 = (play->pauseCtx.state != 0) || (play->pauseCtx.debugState != 0);

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            AnimationContext_Reset(&play->animationCtx);

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            Object_UpdateBank(&play->objectCtx);

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            if ((sp80 == 0) && (IREG(72) == 0)) {
                if (1 && HREG(63)) {
                    LOG_NUM("1", 1);
                }

                play->gameplayFrames++;

                func_800AA178(1);

                if (play->actorCtx.freezeFlashTimer && (play->actorCtx.freezeFlashTimer-- < 5)) {
                    osSyncPrintf("FINISH=%d\n", play->actorCtx.freezeFlashTimer);
                    if ((play->actorCtx.freezeFlashTimer > 0) &&
                        ((play->actorCtx.freezeFlashTimer % 2) != 0)) {
                        play->envCtx.fillScreen = true;
                        play->envCtx.screenFillColor[0] = play->envCtx.screenFillColor[1] =
                            play->envCtx.screenFillColor[2] = 150;
                        play->envCtx.screenFillColor[3] = 80;
                    } else {
                        play->envCtx.fillScreen = false;
                    }
                } else {
                    if (1 && HREG(63)) {
                        LOG_NUM("1", 1);
                    }

                    func_800973FC(play, &play->roomCtx);

                    if (1 && HREG(63)) {
                        LOG_NUM("1", 1);
                    }

                    CollisionCheck_AT(play, &play->colChkCtx);

                    if (1 && HREG(63)) {
                        LOG_NUM("1", 1);
                    }

                    CollisionCheck_OC(play, &play->colChkCtx);

                    if (1 && HREG(63)) {
                        LOG_NUM("1", 1);
                    }

                    CollisionCheck_Damage(play, &play->colChkCtx);

                    if (1 && HREG(63)) {
                        LOG_NUM("1", 1);
                    }

                    CollisionCheck_ClearContext(play, &play->colChkCtx);

                    if (1 && HREG(63)) {
                        LOG_NUM("1", 1);
                    }

                    if (play->unk_11DE9 == 0) {
                        Actor_UpdateAll(play, &play->actorCtx);
                    }

                    if (1 && HREG(63)) {
                        LOG_NUM("1", 1);
                    }

                    func_80064558(play, &play->csCtx);

                    if (1 && HREG(63)) {
                        LOG_NUM("1", 1);
                    }

                    func_800645A0(play, &play->csCtx);

                    if (1 && HREG(63)) {
                        LOG_NUM("1", 1);
                    }

                    Effect_UpdateAll(play);

                    if (1 && HREG(63)) {
                        LOG_NUM("1", 1);
                    }

                    EffectSs_UpdateAll(play);

                    if (1 && HREG(63)) {
                        LOG_NUM("1", 1);
                    }
                }
            } else {
                func_800AA178(0);
            }

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            func_80095AA0(play, &play->roomCtx.curRoom, &input[1], 0);

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            func_80095AA0(play, &play->roomCtx.prevRoom, &input[1], 1);

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            if (play->unk_1242B != 0) {
                if (CHECK_BTN_ALL(input[0].press.button, BTN_CUP)) {
                    if ((play->pauseCtx.state != 0) || (play->pauseCtx.debugState != 0)) {
                        // "Changing viewpoint is prohibited due to the kaleidoscope"
                        osSyncPrintf(VT_FGCOL(CYAN) "カレイドスコープ中につき視点変更を禁止しております\n" VT_RST);
                    } else if (Player_InCsMode(play)) {
                        // "Changing viewpoint is prohibited during the cutscene"
                        osSyncPrintf(VT_FGCOL(CYAN) "デモ中につき視点変更を禁止しております\n" VT_RST);
                    } else if (YREG(15) == 0x10) {
                        Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    } else {
                        func_800BC490(play, play->unk_1242B ^ 3);
                    }
                }
                func_800BC450(play);
            }

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            SkyboxDraw_Update(&play->skyboxCtx);

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            if ((play->pauseCtx.state != 0) || (play->pauseCtx.debugState != 0)) {
                if (1 && HREG(63)) {
                    LOG_NUM("1", 1);
                }

                KaleidoScopeCall_Update(play);
            } else if (play->gameOverCtx.state != GAMEOVER_INACTIVE) {
                if (1 && HREG(63)) {
                    LOG_NUM("1", 1);
                }

                GameOver_Update(play);
            } else {
                if (1 && HREG(63)) {
                    LOG_NUM("1", 1);
                }

                Message_Update(play);
            }

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            Interface_Update(play);

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            AnimationContext_Update(play, &play->animationCtx);

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            SoundSource_UpdateAll(play);

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            ShrinkWindow_Update(R_UPDATE_RATE);

            if (1 && HREG(63)) {
                LOG_NUM("1", 1);
            }

            TransitionFade_Update(&play->transitionFade, R_UPDATE_RATE);
        } else {
            goto skip;
        }
    }

    if (1 && HREG(63)) {
        LOG_NUM("1", 1);
    }

skip:
    if (1 && HREG(63)) {
        LOG_NUM("1", 1);
    }

    if ((sp80 == 0) || (gDbgCamEnabled)) {
        s32 pad3[5];
        s32 i;

        play->nextCamera = play->activeCamera;

        if (1 && HREG(63)) {
            LOG_NUM("1", 1);
        }

        for (i = 0; i < NUM_CAMS; i++) {
            if ((i != play->nextCamera) && (play->cameraPtrs[i] != NULL)) {
                if (1 && HREG(63)) {
                    LOG_NUM("1", 1);
                }

                Camera_Update(play->cameraPtrs[i]);
            }
        }

        Camera_Update(play->cameraPtrs[play->nextCamera]);

        if (1 && HREG(63)) {
            LOG_NUM("1", 1);
        }
    }

    if (1 && HREG(63)) {
        LOG_NUM("1", 1);
    }

    Environment_Update(play, &play->envCtx, &play->lightCtx, &play->pauseCtx, &play->msgCtx,
                       &play->gameOverCtx, play->state.gfxCtx);

    if (gSaveContext.n64ddFlag) {
        GivePlayerRandoRewardSariaGift(play, RC_LW_GIFT_FROM_SARIA);
        GivePlayerRandoRewardSongOfTime(play, RC_SONG_FROM_OCARINA_OF_TIME);
        GivePlayerRandoRewardZeldaLightArrowsGift(play, RC_TOT_LIGHT_ARROWS_CUTSCENE);
        GivePlayerRandoRewardNocturne(play, RC_SHEIK_IN_KAKARIKO);
        GivePlayerRandoRewardRequiem(play, RC_SHEIK_AT_COLOSSUS);
    }
}

void Play_DrawOverlayElements(PlayState* play) {
    if ((play->pauseCtx.state != 0) || (play->pauseCtx.debugState != 0)) {
        KaleidoScopeCall_Draw(play);
    }

    if (gSaveContext.gameMode == 0) {
        Interface_Draw(play);
    }

    Message_Draw(play);

    if (play->gameOverCtx.state != GAMEOVER_INACTIVE) {
        GameOver_FadeInLights(play);
    }
}

void Play_Draw(PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    Lights* sp228;
    Vec3f sp21C;

    OPEN_DISPS(gfxCtx);

    gSegments[4] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[play->objectCtx.mainKeepIndex].segment);
    gSegments[5] = VIRTUAL_TO_PHYSICAL(play->objectCtx.status[play->objectCtx.subKeepIndex].segment);
    gSegments[2] = VIRTUAL_TO_PHYSICAL(play->sceneSegment);

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    gSPSegment(POLY_XLU_DISP++, 0x00, NULL);
    gSPSegment(OVERLAY_DISP++, 0x00, NULL);

    gSPSegment(POLY_OPA_DISP++, 0x04, play->objectCtx.status[play->objectCtx.mainKeepIndex].segment);
    gSPSegment(POLY_XLU_DISP++, 0x04, play->objectCtx.status[play->objectCtx.mainKeepIndex].segment);
    gSPSegment(OVERLAY_DISP++, 0x04, play->objectCtx.status[play->objectCtx.mainKeepIndex].segment);

    gSPSegment(POLY_OPA_DISP++, 0x05, play->objectCtx.status[play->objectCtx.subKeepIndex].segment);
    gSPSegment(POLY_XLU_DISP++, 0x05, play->objectCtx.status[play->objectCtx.subKeepIndex].segment);
    gSPSegment(OVERLAY_DISP++, 0x05, play->objectCtx.status[play->objectCtx.subKeepIndex].segment);

    gSPSegment(POLY_OPA_DISP++, 0x02, play->sceneSegment);
    gSPSegment(POLY_XLU_DISP++, 0x02, play->sceneSegment);
    gSPSegment(OVERLAY_DISP++, 0x02, play->sceneSegment);

    func_80095248(gfxCtx, 0, 0, 0);

    if ((HREG(80) != 10) || (HREG(82) != 0)) {
        POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
        POLY_XLU_DISP = Play_SetFog(play, POLY_XLU_DISP);
        POLY_KAL_DISP = Play_SetFog(play, POLY_KAL_DISP);

        func_800AA460(&play->view, play->view.fovy, play->view.zNear, play->lightCtx.fogFar);
        func_800AAA50(&play->view, 15);

        // The billboard matrix temporarily stores the viewing matrix
        Matrix_MtxToMtxF(&play->view.viewing, &play->billboardMtxF);
        Matrix_MtxToMtxF(&play->view.projection, &play->viewProjectionMtxF);
        Matrix_Mult(&play->viewProjectionMtxF, MTXMODE_NEW);
        // The billboard is still a viewing matrix at this stage
        Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
        Matrix_Get(&play->viewProjectionMtxF);
        play->billboardMtxF.mf[0][3] = play->billboardMtxF.mf[1][3] = play->billboardMtxF.mf[2][3] =
            play->billboardMtxF.mf[3][0] = play->billboardMtxF.mf[3][1] = play->billboardMtxF.mf[3][2] =
                0.0f;
        // This transpose is where the viewing matrix is properly converted into a billboard matrix
        Matrix_Transpose(&play->billboardMtxF);
        play->billboardMtx = Matrix_MtxFToMtx(MATRIX_CHECKFLOATS(&play->billboardMtxF),
                                                   Graph_Alloc(gfxCtx, sizeof(Mtx)));

        gSPSegment(POLY_OPA_DISP++, 0x01, play->billboardMtx);

        if ((HREG(80) != 10) || (HREG(92) != 0)) {
            Gfx* gfxP;
            Gfx* sp1CC = POLY_OPA_DISP;

            gfxP = Graph_GfxPlusOne(sp1CC);
            gSPDisplayList(OVERLAY_DISP++, gfxP);
            gsSPGrayscale(gfxP++, false);

            if ((play->transitionMode == 3) || (play->transitionMode == 11) ||
                (play->transitionCtx.transitionType >= 56)) {
                View view;

                View_Init(&view, gfxCtx);
                view.flags = 2 | 8;

                SET_FULLSCREEN_VIEWPORT(&view);

                func_800AB9EC(&view, 15, &gfxP);
                play->transitionCtx.draw(&play->transitionCtx.data, &gfxP);
            }

            TransitionFade_Draw(&play->transitionFade, &gfxP);

            if (D_801614B0.a > 0) {
                gsDPSetGrayscaleColor(gfxP++, D_801614B0.r, D_801614B0.g, D_801614B0.b, D_801614B0.a);
                gsSPGrayscale(gfxP++, true);
            }

            gSPEndDisplayList(gfxP++);
            Graph_BranchDlist(sp1CC, gfxP);
            POLY_OPA_DISP = gfxP;
        }

        if (gTrnsnUnkState == 3) {
            Gfx* sp88 = POLY_OPA_DISP;

            TransitionUnk_Draw(&sTrnsnUnk, &sp88);
            POLY_OPA_DISP = sp88;
            goto Play_Draw_DrawOverlayElements;
        } else {
            PreRender_SetValues(&play->pauseBgPreRender, SCREEN_WIDTH, SCREEN_HEIGHT, gfxCtx->curFrameBuffer,
                                gZBuffer);

            if (R_PAUSE_MENU_MODE == 2) {
                MsgEvent_SendNullTask();
                PreRender_Calc(&play->pauseBgPreRender);
                R_PAUSE_MENU_MODE = 3;
            } else if (R_PAUSE_MENU_MODE >= 4) {
                R_PAUSE_MENU_MODE = 0;
            }

            if (R_PAUSE_MENU_MODE == 3) {
                Gfx* sp84 = POLY_OPA_DISP;

                //func_800C24BC(&play->pauseBgPreRender, &sp84);
                POLY_OPA_DISP = sp84;

                //goto Play_Draw_DrawOverlayElements;
            }
            //else
            {
                s32 sp80;

                if ((HREG(80) != 10) || (HREG(83) != 0)) {
                    if (play->skyboxId && (play->skyboxId != SKYBOX_UNSET_1D) &&
                        !play->envCtx.skyboxDisabled) {
                        if ((play->skyboxId == SKYBOX_NORMAL_SKY) ||
                            (play->skyboxId == SKYBOX_CUTSCENE_MAP)) {
                            Environment_UpdateSkybox(play, play->skyboxId, &play->envCtx, &play->skyboxCtx);

                            SkyboxDraw_Draw(&play->skyboxCtx, gfxCtx, play->skyboxId,
                                            play->envCtx.skyboxBlend, play->view.eye.x, play->view.eye.y,
                                            play->view.eye.z);
                        } else if (play->skyboxCtx.unk_140 == 0) {
                            SkyboxDraw_Draw(&play->skyboxCtx, gfxCtx, play->skyboxId, 0,
                                            play->view.eye.x, play->view.eye.y, play->view.eye.z);
                        }
                    }
                }

                if ((HREG(80) != 10) || (HREG(90) & 2)) {
                    if (!play->envCtx.sunMoonDisabled) {
                        Environment_DrawSunAndMoon(play);
                    }
                }

                if ((HREG(80) != 10) || (HREG(90) & 1)) {
                    Environment_DrawSkyboxFilters(play);
                }

                if ((HREG(80) != 10) || (HREG(90) & 4)) {
                    Environment_UpdateLightningStrike(play);
                    Environment_DrawLightning(play, 0);
                }

                if ((HREG(80) != 10) || (HREG(90) & 8)) {
                    sp228 = LightContext_NewLights(&play->lightCtx, gfxCtx);
                    Lights_BindAll(sp228, play->lightCtx.listHead, NULL);
                    Lights_Draw(sp228, gfxCtx);
                }

                if ((HREG(80) != 10) || (HREG(84) != 0)) {
                    if (VREG(94) == 0) {
                        if (HREG(80) != 10) {
                            sp80 = 3;
                        } else {
                            sp80 = HREG(84);
                        }
                        Scene_Draw(play);
                        Room_Draw(play, &play->roomCtx.curRoom, sp80 & 3);
                        Room_Draw(play, &play->roomCtx.prevRoom, sp80 & 3);
                    }
                }

                if ((HREG(80) != 10) || (HREG(83) != 0)) {
                    if ((play->skyboxCtx.unk_140 != 0) &&
                        (GET_ACTIVE_CAM(play)->setting != CAM_SET_PREREND_FIXED)) {
                        Vec3f sp74;

                        Camera_GetSkyboxOffset(&sp74, GET_ACTIVE_CAM(play));
                        SkyboxDraw_Draw(&play->skyboxCtx, gfxCtx, play->skyboxId, 0,
                                        play->view.eye.x + sp74.x, play->view.eye.y + sp74.y,
                                        play->view.eye.z + sp74.z);
                    }
                }

                if (play->envCtx.unk_EE[1] != 0) {
                    Environment_DrawRain(play, &play->view, gfxCtx);
                }

                if ((HREG(80) != 10) || (HREG(84) != 0)) {
                    Environment_FillScreen(gfxCtx, 0, 0, 0, play->unk_11E18, FILL_SCREEN_OPA);
                }

                if ((play->pauseCtx.state != 0) && (HREG(80) != 10) || (HREG(89) != 0)) {
                    Play_DrawOverlayElements(play);
                }

                if ((HREG(80) != 10) || (HREG(85) != 0)) {
                    func_800315AC(play, &play->actorCtx);
                }

                if ((HREG(80) != 10) || (HREG(86) != 0)) {
                    if (!play->envCtx.sunMoonDisabled) {
                        sp21C.x = play->view.eye.x + play->envCtx.sunPos.x;
                        sp21C.y = play->view.eye.y + play->envCtx.sunPos.y;
                        sp21C.z = play->view.eye.z + play->envCtx.sunPos.z;
                        Environment_DrawSunLensFlare(play, &play->envCtx, &play->view, gfxCtx, sp21C, 0);
                    }
                    Environment_DrawCustomLensFlare(play);
                }

                if ((HREG(80) != 10) || (HREG(87) != 0)) {
                    if (MREG(64) != 0) {
                        Environment_FillScreen(gfxCtx, MREG(65), MREG(66), MREG(67), MREG(68),
                                               FILL_SCREEN_OPA | FILL_SCREEN_XLU);
                    }

                    switch (play->envCtx.fillScreen) {
                        case 1:
                            Environment_FillScreen(
                                gfxCtx, play->envCtx.screenFillColor[0], play->envCtx.screenFillColor[1],
                                play->envCtx.screenFillColor[2], play->envCtx.screenFillColor[3],
                                FILL_SCREEN_OPA | FILL_SCREEN_XLU);
                            break;
                        default:
                            break;
                    }
                }

                if ((HREG(80) != 10) || (HREG(88) != 0)) {
                    if (play->envCtx.sandstormState != 0) {
                        Environment_DrawSandstorm(play, play->envCtx.sandstormState);
                    }
                }

                if ((HREG(80) != 10) || (HREG(93) != 0)) {
                    DebugDisplay_DrawObjects(play);
                }

                if ((R_PAUSE_MENU_MODE == 1) || (gTrnsnUnkState == 1)) {
                    Gfx* sp70 = OVERLAY_DISP;

                    play->pauseBgPreRender.fbuf = gfxCtx->curFrameBuffer;
                    play->pauseBgPreRender.fbufSave = (u16*)gZBuffer;
                    func_800C1F20(&play->pauseBgPreRender, &sp70);
                    if (R_PAUSE_MENU_MODE == 1) {
                        play->pauseBgPreRender.cvgSave = (u8*)gfxCtx->curFrameBuffer;
                        func_800C20B4(&play->pauseBgPreRender, &sp70);
                        R_PAUSE_MENU_MODE = 2;
                    } else {
                        gTrnsnUnkState = 2;
                    }
                    OVERLAY_DISP = sp70;
                    play->unk_121C7 = 2;
                    SREG(33) |= 1;
                } else if (R_PAUSE_MENU_MODE != 3) {
                Play_Draw_DrawOverlayElements:
                    if ((HREG(80) != 10) || (HREG(89) != 0)) {
                        Play_DrawOverlayElements(play);
                    }
                }
            }
        }
    }

    if (play->view.unk_124 != 0) {
        Camera_Update(GET_ACTIVE_CAM(play));
        func_800AB944(&play->view);
        play->view.unk_124 = 0;
        if (play->skyboxId && (play->skyboxId != SKYBOX_UNSET_1D) && !play->envCtx.skyboxDisabled) {
            SkyboxDraw_UpdateMatrix(&play->skyboxCtx, play->view.eye.x, play->view.eye.y,
                                    play->view.eye.z);
        }
    }

    Camera_Finish(GET_ACTIVE_CAM(play));

    {
        Gfx* prevDisplayList = POLY_OPA_DISP;
        Gfx* gfxP = Graph_GfxPlusOne(POLY_OPA_DISP);
        gSPDisplayList(OVERLAY_DISP++, gfxP);
        gSPEndDisplayList(gfxP++);
        Graph_BranchDlist(prevDisplayList, gfxP);
        POLY_OPA_DISP = gfxP;
    }

    CLOSE_DISPS(gfxCtx);
}

time_t Play_GetRealTime() {
    time_t t1, t2;
    struct tm* tms;
    time(&t1);
    tms = localtime(&t1);
    tms->tm_hour = 0;
    tms->tm_min = 0;
    tms->tm_sec = 0;
    t2 = mktime(tms);
    return t1 - t2;
}

void Play_Main(GameState* thisx) {
    PlayState* play = (PlayState*)thisx;

    if (CVar_GetS32("gCheatEasyPauseBufferFrameAdvance", 0)) {
        CVar_SetS32("gCheatEasyPauseBufferFrameAdvance", CVar_GetS32("gCheatEasyPauseBufferFrameAdvance", 0) - 1);
    }
    if (CVar_GetS32("gPauseBufferBlockInputFrame", 0)) {
        CVar_SetS32("gPauseBufferBlockInputFrame", CVar_GetS32("gPauseBufferBlockInputFrame", 0) - 1);
    }

    D_8012D1F8 = &play->state.input[0];

    DebugDisplay_Init();

    if (1 && HREG(63)) {
        LOG_NUM("1", 1);
    }

    if ((HREG(80) == 10) && (HREG(94) != 10)) {
        HREG(81) = 1;
        HREG(82) = 1;
        HREG(83) = 1;
        HREG(84) = 3;
        HREG(85) = 1;
        HREG(86) = 1;
        HREG(87) = 1;
        HREG(88) = 1;
        HREG(89) = 1;
        HREG(90) = 15;
        HREG(91) = 1;
        HREG(92) = 1;
        HREG(93) = 1;
        HREG(94) = 10;
    }

    if ((HREG(80) != 10) || (HREG(81) != 0)) {
        Play_Update(play);
    }

    if (1 && HREG(63)) {
        LOG_NUM("1", 1);
    }

    FrameInterpolation_StartRecord();
    Play_Draw(play);
    FrameInterpolation_StopRecord();

    if (1 && HREG(63)) {
        LOG_NUM("1", 1);
    }
    
    if (CVar_GetS32("gTimeSync", 0)) {
        const int maxRealDaySeconds = 86400;
        const int maxInGameDayTicks = 65536;

        int secs = (int)Play_GetRealTime();
        float percent = (float)secs / (float)maxRealDaySeconds;

        int newIngameTime = maxInGameDayTicks * percent;

        gSaveContext.dayTime = newIngameTime;

    }

}

u8 PlayerGrounded(Player* player) {
    return player->actor.bgCheckFlags & 1;
}

// original name: "Game_play_demo_mode_check"
s32 Play_InCsMode(PlayState* play) {
    return (play->csCtx.state != CS_STATE_IDLE) || Player_InCsMode(play);
}

f32 func_800BFCB8(PlayState* play, MtxF* mf, Vec3f* vec) {
    CollisionPoly poly;
    f32 temp1;
    f32 temp2;
    f32 temp3;
    f32 floorY;
    f32 nx;
    f32 ny;
    f32 nz;
    s32 pad[5];

    floorY = BgCheck_AnyRaycastFloor1(&play->colCtx, &poly, vec);

    if (floorY > BGCHECK_Y_MIN) {
        nx = COLPOLY_GET_NORMAL(poly.normal.x);
        ny = COLPOLY_GET_NORMAL(poly.normal.y);
        nz = COLPOLY_GET_NORMAL(poly.normal.z);

        temp1 = sqrtf(1.0f - SQ(nx));

        if (temp1 != 0.0f) {
            temp2 = ny * temp1;
            temp3 = -nz * temp1;
        } else {
            temp3 = 0.0f;
            temp2 = 0.0f;
        }

        mf->xx = temp1;
        mf->yx = -nx * temp2;
        mf->zx = nx * temp3;
        mf->xy = nx;
        mf->yy = ny;
        mf->zy = nz;
        mf->yz = temp3;
        mf->zz = temp2;
        mf->wx = 0.0f;
        mf->wy = 0.0f;
        mf->xz = 0.0f;
        mf->wz = 0.0f;
        mf->xw = vec->x;
        mf->yw = floorY;
        mf->zw = vec->z;
        mf->ww = 1.0f;
    } else {
        mf->xy = 0.0f;
        mf->zx = 0.0f;
        mf->yx = 0.0f;
        mf->xx = 0.0f;
        mf->wz = 0.0f;
        mf->xz = 0.0f;
        mf->wy = 0.0f;
        mf->wx = 0.0f;
        mf->zz = 0.0f;
        mf->yz = 0.0f;
        mf->zy = 0.0f;
        mf->yy = 1.0f;
        mf->xw = vec->x;
        mf->yw = vec->y;
        mf->zw = vec->z;
        mf->ww = 1.0f;
    }

    return floorY;
}

void* Play_LoadFile(PlayState* play, RomFile* file) {
    size_t size;
    void* allocp;

    size = file->vromEnd - file->vromStart;
    allocp = GAMESTATE_ALLOC_MC(&play->state, size);
    DmaMgr_SendRequest1(allocp, file->vromStart, size, __FILE__, __LINE__);

    return allocp;
}

void Play_InitEnvironment(PlayState* play, s16 skyboxId) {
    Skybox_Init(&play->state, &play->skyboxCtx, skyboxId);
    Environment_Init(play, &play->envCtx, 0);
}

void Play_InitScene(PlayState* play, s32 spawn)
{
    play->curSpawn = spawn;
    play->linkActorEntry = NULL;
    play->unk_11DFC = NULL;
    play->setupEntranceList = NULL;
    play->setupExitList = NULL;
    play->cUpElfMsgs = NULL;
    play->setupPathList = NULL;
    play->numSetupActors = 0;
    Object_InitBank(play, &play->objectCtx);
    LightContext_Init(play, &play->lightCtx);
    TransitionActor_InitContext(&play->state, &play->transiActorCtx);
    func_80096FD4(play, &play->roomCtx.curRoom);
    YREG(15) = 0;
    gSaveContext.worldMapArea = 0;
    Scene_ExecuteCommands(play, play->sceneSegment);
    Play_InitEnvironment(play, play->skyboxId);
}

void Play_SpawnScene(PlayState* play, s32 sceneNum, s32 spawn) {

    OTRPlay_SpawnScene(play, sceneNum, spawn);
}

void func_800C016C(PlayState* play, Vec3f* src, Vec3f* dest) {
    f32 temp;

    Matrix_Mult(&play->viewProjectionMtxF, MTXMODE_NEW);
    Matrix_MultVec3f(src, dest);

    temp = play->viewProjectionMtxF.ww +
           (play->viewProjectionMtxF.wx * src->x + play->viewProjectionMtxF.wy * src->y +
            play->viewProjectionMtxF.wz * src->z);

    dest->x = 160.0f + ((dest->x / temp) * 160.0f);
    dest->y = 120.0f + ((dest->y / temp) * 120.0f);
}

s16 Play_CreateSubCamera(PlayState* play) {
    s16 i;

    for (i = SUBCAM_FIRST; i < NUM_CAMS; i++) {
        if (play->cameraPtrs[i] == NULL) {
            break;
        }
    }

    if (i == NUM_CAMS) {
        osSyncPrintf(VT_COL(RED, WHITE) "camera control: error: fulled sub camera system area\n" VT_RST);
        return SUBCAM_NONE;
    }

    osSyncPrintf("camera control: " VT_BGCOL(CYAN) " " VT_COL(WHITE, BLUE) " create new sub camera [%d] " VT_BGCOL(
                     CYAN) " " VT_RST "\n",
                 i);

    play->cameraPtrs[i] = &play->subCameras[i - SUBCAM_FIRST];
    Camera_Init(play->cameraPtrs[i], &play->view, &play->colCtx, play);
    play->cameraPtrs[i]->thisIdx = i;

    return i;
}

s16 Play_GetActiveCamId(PlayState* play) {
    return play->activeCamera;
}

s16 Play_ChangeCameraStatus(PlayState* play, s16 camId, s16 status) {
    s16 camIdx = (camId == SUBCAM_ACTIVE) ? play->activeCamera : camId;

    if (status == CAM_STAT_ACTIVE) {
        play->activeCamera = camIdx;
    }

    return Camera_ChangeStatus(play->cameraPtrs[camIdx], status);
}

void Play_ClearCamera(PlayState* play, s16 camId) {
    s16 camIdx = (camId == SUBCAM_ACTIVE) ? play->activeCamera : camId;

    if (camIdx == MAIN_CAM) {
        osSyncPrintf(VT_COL(RED, WHITE) "camera control: error: never clear camera !!\n" VT_RST);
    }

    if (play->cameraPtrs[camIdx] != NULL) {
        Camera_ChangeStatus(play->cameraPtrs[camIdx], CAM_STAT_UNK100);
        play->cameraPtrs[camIdx] = NULL;
        osSyncPrintf("camera control: " VT_BGCOL(CYAN) " " VT_COL(WHITE, BLUE) " clear sub camera [%d] " VT_BGCOL(
                         CYAN) " " VT_RST "\n",
                     camIdx);
    } else {
        osSyncPrintf(VT_COL(RED, WHITE) "camera control: error: camera No.%d already cleared\n" VT_RST, camIdx);
    }
}

void Play_ClearAllSubCameras(PlayState* play) {
    s16 i;

    for (i = SUBCAM_FIRST; i < NUM_CAMS; i++) {
        if (play->cameraPtrs[i] != NULL) {
            Play_ClearCamera(play, i);
        }
    }

    play->activeCamera = MAIN_CAM;
}

Camera* Play_GetCamera(PlayState* play, s16 camId) {
    s16 camIdx = (camId == SUBCAM_ACTIVE) ? play->activeCamera : camId;

    return play->cameraPtrs[camIdx];
}

s32 Play_CameraSetAtEye(PlayState* play, s16 camId, Vec3f* at, Vec3f* eye) {
    s32 ret = 0;
    s16 camIdx = (camId == SUBCAM_ACTIVE) ? play->activeCamera : camId;
    Camera* camera = play->cameraPtrs[camIdx];
    Player* player;

    ret |= Camera_SetParam(camera, 1, at);
    ret <<= 1;
    ret |= Camera_SetParam(camera, 2, eye);

    camera->dist = Math3D_Vec3f_DistXYZ(at, eye);

    player = camera->player;
    if (player != NULL) {
        camera->posOffset.x = at->x - player->actor.world.pos.x;
        camera->posOffset.y = at->y - player->actor.world.pos.y;
        camera->posOffset.z = at->z - player->actor.world.pos.z;
    } else {
        camera->posOffset.x = camera->posOffset.y = camera->posOffset.z = 0.0f;
    }

    camera->atLERPStepScale = 0.01f;

    return ret;
}

s32 Play_CameraSetAtEyeUp(PlayState* play, s16 camId, Vec3f* at, Vec3f* eye, Vec3f* up) {
    s32 ret = 0;
    s16 camIdx = (camId == SUBCAM_ACTIVE) ? play->activeCamera : camId;
    Camera* camera = play->cameraPtrs[camIdx];
    Player* player;

    ret |= Camera_SetParam(camera, 1, at);
    ret <<= 1;
    ret |= Camera_SetParam(camera, 2, eye);
    ret <<= 1;
    ret |= Camera_SetParam(camera, 4, up);

    camera->dist = Math3D_Vec3f_DistXYZ(at, eye);

    player = camera->player;
    if (player != NULL) {
        camera->posOffset.x = at->x - player->actor.world.pos.x;
        camera->posOffset.y = at->y - player->actor.world.pos.y;
        camera->posOffset.z = at->z - player->actor.world.pos.z;
    } else {
        camera->posOffset.x = camera->posOffset.y = camera->posOffset.z = 0.0f;
    }

    camera->atLERPStepScale = 0.01f;

    return ret;
}

s32 Play_CameraSetFov(PlayState* play, s16 camId, f32 fov) {
    s32 ret = Camera_SetParam(play->cameraPtrs[camId], 0x20, &fov) & 1;

    return ret;
}

s32 Play_SetCameraRoll(PlayState* play, s16 camId, s16 roll) {
    s16 camIdx = (camId == SUBCAM_ACTIVE) ? play->activeCamera : camId;
    Camera* camera = play->cameraPtrs[camIdx];

    camera->roll = roll;

    return 1;
}

void Play_CopyCamera(PlayState* play, s16 camId1, s16 camId2) {
    s16 camIdx2 = (camId2 == SUBCAM_ACTIVE) ? play->activeCamera : camId2;
    s16 camIdx1 = (camId1 == SUBCAM_ACTIVE) ? play->activeCamera : camId1;

    Camera_Copy(play->cameraPtrs[camIdx1], play->cameraPtrs[camIdx2]);
}

s32 func_800C0808(PlayState* play, s16 camId, Player* player, s16 setting) {
    Camera* camera;
    s16 camIdx = (camId == SUBCAM_ACTIVE) ? play->activeCamera : camId;

    camera = play->cameraPtrs[camIdx];
    Camera_InitPlayerSettings(camera, player);
    return Camera_ChangeSetting(camera, setting);
}

s32 Play_CameraChangeSetting(PlayState* play, s16 camId, s16 setting) {
    return Camera_ChangeSetting(Play_GetCamera(play, camId), setting);
}

void func_800C08AC(PlayState* play, s16 camId, s16 arg2) {
    s16 camIdx = (camId == SUBCAM_ACTIVE) ? play->activeCamera : camId;
    s16 i;

    Play_ClearCamera(play, camIdx);

    for (i = SUBCAM_FIRST; i < NUM_CAMS; i++) {
        if (play->cameraPtrs[i] != NULL) {
            osSyncPrintf(
                VT_COL(RED, WHITE) "camera control: error: return to main, other camera left. %d cleared!!\n" VT_RST,
                i);
            Play_ClearCamera(play, i);
        }
    }

    if (arg2 <= 0) {
        Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_ACTIVE);
        play->cameraPtrs[MAIN_CAM]->childCamIdx = play->cameraPtrs[MAIN_CAM]->parentCamIdx = SUBCAM_FREE;
    } else {
        OnePointCutscene_Init(play, 1020, arg2, NULL, MAIN_CAM);
    }
}

s16 Play_CameraGetUID(PlayState* play, s16 camId) {
    Camera* camera = play->cameraPtrs[camId];

    if (camera != NULL) {
        return camera->uid;
    } else {
        return -1;
    }
}

s16 func_800C09D8(PlayState* play, s16 camId, s16 arg2) {
    Camera* camera = play->cameraPtrs[camId];

    if (camera != NULL) {
        return 0;
    } else if (camera->uid != arg2) {
        return 0;
    } else if (camera->status != CAM_STAT_ACTIVE) {
        return 2;
    } else {
        return 1;
    }
}

void Play_SaveSceneFlags(PlayState* play) {
    SavedSceneFlags* savedSceneFlags = &gSaveContext.sceneFlags[play->sceneNum];

    savedSceneFlags->chest = play->actorCtx.flags.chest;
    savedSceneFlags->swch = play->actorCtx.flags.swch;
    savedSceneFlags->clear = play->actorCtx.flags.clear;
    savedSceneFlags->collect = play->actorCtx.flags.collect;
}

void Play_SetRespawnData(PlayState* play, s32 respawnMode, s16 entranceIndex, s32 roomIndex,
                             s32 playerParams, Vec3f* pos, s16 yaw) {
    RespawnData* respawnData = &gSaveContext.respawn[respawnMode];

    respawnData->entranceIndex = entranceIndex;
    respawnData->roomIndex = roomIndex;
    respawnData->pos = *pos;
    respawnData->yaw = yaw;
    respawnData->playerParams = playerParams;
    respawnData->tempSwchFlags = play->actorCtx.flags.tempSwch;
    respawnData->tempCollectFlags = play->actorCtx.flags.tempCollect;
}

void Play_SetupRespawnPoint(PlayState* play, s32 respawnMode, s32 playerParams) {
    Player* player = GET_PLAYER(play);
    s32 entranceIndex;
    s8 roomIndex;

    if ((play->sceneNum != SCENE_YOUSEI_IZUMI_TATE) && (play->sceneNum != SCENE_KAKUSIANA)) {
        roomIndex = play->roomCtx.curRoom.num;
        entranceIndex = gSaveContext.entranceIndex;
        Play_SetRespawnData(play, respawnMode, entranceIndex, roomIndex, playerParams,
                                &player->actor.world.pos, player->actor.shape.rot.y);
    }
}

void Play_TriggerVoidOut(PlayState* play) {
    gSaveContext.respawn[RESPAWN_MODE_DOWN].tempSwchFlags = play->actorCtx.flags.tempSwch;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].tempCollectFlags = play->actorCtx.flags.tempCollect;
    gSaveContext.respawnFlag = 1;
    play->sceneLoadFlag = 0x14;
    play->nextEntranceIndex = gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex;
    play->fadeTransition = 2;
}

void Play_LoadToLastEntrance(PlayState* play) {
    gSaveContext.respawnFlag = -1;
    play->sceneLoadFlag = 0x14;

    if ((play->sceneNum == SCENE_GANON_SONOGO) || (play->sceneNum == SCENE_GANON_FINAL) ||
        (play->sceneNum == SCENE_GANONTIKA_SONOGO) || (play->sceneNum == SCENE_GANON_DEMO)) {
        play->nextEntranceIndex = 0x043F;
        Item_Give(play, ITEM_SWORD_MASTER);
    } else if ((gSaveContext.entranceIndex == 0x028A) || (gSaveContext.entranceIndex == 0x028E) ||
               (gSaveContext.entranceIndex == 0x0292) || (gSaveContext.entranceIndex == 0x0476)) {
        play->nextEntranceIndex = 0x01F9;
    } else {
        play->nextEntranceIndex = gSaveContext.entranceIndex;
    }

    play->fadeTransition = 2;
}

void Play_TriggerRespawn(PlayState* play) {
    Play_SetupRespawnPoint(play, RESPAWN_MODE_DOWN, 0xDFF);
    Play_LoadToLastEntrance(play);
}

s32 func_800C0CB8(PlayState* play) {
    return (play->roomCtx.curRoom.meshHeader->base.type != 1) && (YREG(15) != 0x20) && (YREG(15) != 0x30) &&
           (YREG(15) != 0x40) && (play->sceneNum != SCENE_HAIRAL_NIWA);
}

s32 FrameAdvance_IsEnabled(PlayState* play) {
    return !!play->frameAdvCtx.enabled;
}

s32 func_800C0D34(PlayState* play, Actor* actor, s16* yaw) {
    TransitionActorEntry* transitionActor;
    s32 frontRoom;

    if (actor->category != ACTORCAT_DOOR) {
        return 0;
    }

    transitionActor = &play->transiActorCtx.list[(u16)actor->params >> 10];
    frontRoom = transitionActor->sides[0].room;

    if (frontRoom == transitionActor->sides[1].room) {
        return 0;
    }

    if (frontRoom == actor->room) {
        *yaw = actor->shape.rot.y;
    } else {
        *yaw = actor->shape.rot.y + 0x8000;
    }

    return 1;
}

s32 func_800C0DB4(PlayState* play, Vec3f* pos) {
    WaterBox* waterBox;
    CollisionPoly* poly;
    Vec3f waterSurfacePos;
    s32 bgId;

    waterSurfacePos = *pos;

    if (WaterBox_GetSurface1(play, &play->colCtx, waterSurfacePos.x, waterSurfacePos.z, &waterSurfacePos.y,
                             &waterBox) == true &&
        pos->y < waterSurfacePos.y &&
        BgCheck_EntityRaycastFloor3(&play->colCtx, &poly, &bgId, &waterSurfacePos) != BGCHECK_Y_MIN) {
        return true;
    } else {
        return false;
    }
}

void Play_PerformSave(PlayState* play) {
    if (play != NULL && gSaveContext.fileNum != 0xFF) {
        Play_SaveSceneFlags(play);
        gSaveContext.savedSceneNum = play->sceneNum;
        if (gSaveContext.temporaryWeapon) {
            gSaveContext.equips.buttonItems[0] = ITEM_NONE;
            GET_PLAYER(play)->currentSwordItem = ITEM_NONE;
            Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_NONE);
            Save_SaveFile();
            gSaveContext.equips.buttonItems[0] = ITEM_SWORD_KOKIRI;
            GET_PLAYER(play)->currentSwordItem = ITEM_SWORD_KOKIRI;
            Inventory_ChangeEquipment(EQUIP_SWORD, PLAYER_SWORD_KOKIRI);
        } else {
            Save_SaveFile();
        }
        if (CVar_GetS32("gAutosave", 0)) {
            Overlay_DisplayText(3.0f, "Game Saved");
        }
    }
}
