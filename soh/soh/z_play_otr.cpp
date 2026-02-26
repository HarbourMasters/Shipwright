#include "OTRGlobals.h"
#include "ResourceManagerHelpers.h"
#include <libultraship/libultraship.h>
#include "soh/resource/type/Scene.h"
#include <ship/utils/StringHelper.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "global.h"
#include "vt.h"
#include <fast/resource/type/Vertex.h>
#include "soh/Enhancements/external-mods/ExternalModManager.h"

extern "C" void Play_InitScene(PlayState* play, s32 spawn);
extern "C" void Play_InitEnvironment(PlayState* play, s16 skyboxId);
void OTRPlay_InitScene(PlayState* play, s32 spawn);
s32 OTRScene_ExecuteCommands(PlayState* play, SOH::Scene* scene);

// LUS::OTRResource* OTRPlay_LoadFile(PlayState* play, RomFile* file) {
Ship::IResource* OTRPlay_LoadFile(PlayState* play, const char* fileName) {
    auto res = Ship::Context::GetInstance()->GetResourceManager()->LoadResource(fileName);
    return res.get();
}

extern "C" void OTRPlay_SpawnScene(PlayState* play, s32 sceneId, s32 spawn) {
    SceneTableEntry* scene = &gSceneTable[sceneId];

    scene->unk_13 = 0;
    play->loadedScene = scene;
    play->sceneNum = sceneId;
    play->sceneConfig = scene->config;

    // osSyncPrintf("\nSCENE SIZE %fK\n", (scene->sceneFile.vromEnd - scene->sceneFile.vromStart) / 1024.0f);

    // Scenes considered "dungeon" with a MQ variant
    int16_t inNonSharedScene = (sceneId >= SCENE_DEKU_TREE && sceneId <= SCENE_ICE_CAVERN) ||
                               sceneId == SCENE_GERUDO_TRAINING_GROUND || sceneId == SCENE_INSIDE_GANONS_CASTLE;

    std::string sceneVersion = "shared";
    if (inNonSharedScene) {
        sceneVersion = ResourceMgr_IsGameMasterQuest() ? "mq" : "nonmq";
    }
    const std::string defaultScenePath = StringHelper::Sprintf("scenes/%s/%s/%s", sceneVersion.c_str(),
                                                                scene->sceneFile.fileName, scene->sceneFile.fileName);
    std::string scenePath = defaultScenePath;
    s32 resolvedSpawn = spawn;

    SOH::ExternalModPendingSceneLoadRequest pendingSceneRequest;
    const bool hasPendingSceneRequest =
        SOH::ExternalModManager::Instance().TryConsumePendingSceneLoadRequest(static_cast<int16_t>(sceneId), pendingSceneRequest);
    if (hasPendingSceneRequest) {
        scenePath = pendingSceneRequest.sceneResourcePath;
        resolvedSpawn = pendingSceneRequest.spawnId;
        SPDLOG_INFO("[ExternalMods] Attempting namespaced scene load for {}.{}: hostScene={:#x} resource={} spawn={}",
                    pendingSceneRequest.modId, pendingSceneRequest.sceneId, sceneId, scenePath, resolvedSpawn);
    }

    play->sceneSegment = OTRPlay_LoadFile(play, scenePath.c_str());
    const bool loadedNamespacedScene = hasPendingSceneRequest && play->sceneSegment != nullptr;

    // Failed to load scene... default to doodongs cavern
    if (play->sceneSegment == nullptr) {
        if (hasPendingSceneRequest) {
            SOH::ExternalModManager::Instance().HandlePendingSceneLoadFailure(
                pendingSceneRequest, "resource not found: " + pendingSceneRequest.sceneResourcePath);

            auto resolveFallbackEntranceTableIndex = [&pendingSceneRequest]() -> int32_t {
                const int32_t setupAdjusted = static_cast<int32_t>(pendingSceneRequest.fallbackEntranceIndex) +
                                              static_cast<int32_t>(gSaveContext.sceneSetupIndex);
                if (setupAdjusted >= 0 && setupAdjusted < static_cast<int32_t>(ARRAY_COUNT(gEntranceTable))) {
                    return setupAdjusted;
                }
                const int32_t direct = static_cast<int32_t>(pendingSceneRequest.fallbackEntranceIndex);
                if (direct >= 0 && direct < static_cast<int32_t>(ARRAY_COUNT(gEntranceTable))) {
                    return direct;
                }
                return -1;
            };

            if (pendingSceneRequest.fallbackPlayable && pendingSceneRequest.hasFallbackEntrance) {
                const int32_t fallbackEntranceIndex = resolveFallbackEntranceTableIndex();
                if (fallbackEntranceIndex >= 0) {
                    const auto fallbackSceneId = static_cast<s32>(gEntranceTable[fallbackEntranceIndex].scene);
                    const auto fallbackSpawn = static_cast<s32>(gEntranceTable[fallbackEntranceIndex].spawn);
                    if (fallbackSceneId != sceneId || fallbackSpawn != spawn) {
                        SPDLOG_WARN(
                            "[ExternalMods] Redirecting namespaced scene fallback for {}.{} to fallback entrance scene={:#x} spawn={}",
                            pendingSceneRequest.modId, pendingSceneRequest.sceneId, fallbackSceneId, fallbackSpawn);
                        OTRPlay_SpawnScene(play, fallbackSceneId, fallbackSpawn);
                        return;
                    }
                }
            }

            play->sceneSegment = OTRPlay_LoadFile(play, defaultScenePath.c_str());
            if (play->sceneSegment != nullptr) {
                scenePath = defaultScenePath;
                if (pendingSceneRequest.fallbackPlayable) {
                    SPDLOG_WARN("[ExternalMods] Falling back to host scene path for {}.{}: {}", pendingSceneRequest.modId,
                                pendingSceneRequest.sceneId, scenePath);
                } else {
                    SPDLOG_WARN(
                        "[ExternalMods] Namespaced scene strict mode failed for {}.{}; runtime disabled and host scene loaded: {}",
                        pendingSceneRequest.modId, pendingSceneRequest.sceneId, scenePath);
                }
            }
        }
    }

    // Failed to load scene... default to doodongs cavern
    if (play->sceneSegment == nullptr) {
        lusprintf(__FILE__, __LINE__, 2, "Unable to load scene %s... Defaulting to Doodong's Cavern!\n",
                  scenePath.c_str());
        OTRPlay_SpawnScene(play, 0x01, 0);
        return;
    }

    if (loadedNamespacedScene) {
        SOH::ExternalModManager::Instance().HandlePendingSceneLoadSuccess(pendingSceneRequest);
    }

    scene->unk_13 = 0;

    // gSegments[2] = VIRTUAL_TO_PHYSICAL(play->sceneSegment);

    OTRPlay_InitScene(play, resolvedSpawn);
    auto roomSize = func_80096FE8(play, &play->roomCtx);

    osSyncPrintf("ROOM SIZE=%fK\n", roomSize / 1024.0f);

    GameInteractor_ExecuteOnSceneInit(play->sceneNum);
    SPDLOG_INFO("Scene Init - sceneNum: {0:#x}, entranceIndex: {1:#x}", play->sceneNum, gSaveContext.entranceIndex);
}

void OTRPlay_InitScene(PlayState* play, s32 spawn) {
    play->curSpawn = spawn;
    play->linkActorEntry = nullptr;
    play->unk_11DFC = nullptr;
    play->setupEntranceList = nullptr;
    play->setupExitList = nullptr;
    play->cUpElfMsgs = nullptr;
    play->setupPathList = nullptr;
    play->numSetupActors = 0;
    Object_InitBank(play, &play->objectCtx);
    LightContext_Init(play, &play->lightCtx);
    TransitionActor_InitContext(&play->state, &play->transiActorCtx);
    func_80096FD4(play, &play->roomCtx.curRoom);
    YREG(15) = 0;
    gSaveContext.worldMapArea = 0;
    OTRScene_ExecuteCommands(play, (SOH::Scene*)play->sceneSegment);

    GameInteractor_ExecuteAfterSceneCommands(play->sceneNum);
    Play_InitEnvironment(play, play->skyboxId);
    /* auto data = static_cast<LUS::Vertex*>(Ship::Context::GetInstance()
                                               ->GetResourceManager()
                                               ->ResourceLoad("object_link_child\\object_link_childVtx_01FE08")
                                               .get());

    auto data2 = ResourceMgr_LoadVtxByCRC(0x68d4ea06044e228f);*/

    volatile int a = 0;
}
