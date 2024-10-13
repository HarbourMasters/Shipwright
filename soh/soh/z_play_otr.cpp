#include "OTRGlobals.h"
#include <libultraship/libultraship.h>
#include "soh/resource/type/Scene.h"
#include <utils/StringHelper.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/SceneDB.h"
#include "global.h"
#include "vt.h"
#include <Vertex.h>

extern "C" void Play_InitScene(PlayState * play, s32 spawn);
extern "C" void Play_InitEnvironment(PlayState * play, s16 skyboxId);
void OTRPlay_InitScene(PlayState* play, s32 spawn);
s32 OTRScene_ExecuteCommands(PlayState* play, SOH::Scene* scene);

//LUS::OTRResource* OTRPlay_LoadFile(PlayState* play, RomFile* file) {
Ship::IResource* OTRPlay_LoadFile(PlayState* play, const char* fileName)
{
    auto res = Ship::Context::GetInstance()->GetResourceManager()->LoadResource(fileName);
    return res.get();
}

extern "C" void OTRPlay_SpawnScene(PlayState* play, s32 sceneNum, s32 spawn) {
    SceneDB::Entry& entry = SceneDB::Instance->RetrieveEntry(sceneNum);

    play->sceneNum = sceneNum;
    play->sceneConfig = entry.entry.sceneDrawConfig;

    // Scenes considered "dungeon" with a MQ variant
    int16_t inNonSharedScene = (sceneNum >= SCENE_DEKU_TREE && sceneNum <= SCENE_ICE_CAVERN) ||
                               sceneNum == SCENE_GERUDO_TRAINING_GROUND || sceneNum == SCENE_INSIDE_GANONS_CASTLE;

    std::string sceneVersion = "shared";
    if (inNonSharedScene) {
        sceneVersion = IsGameMasterQuest() ? "mq" : "nonmq";
    }
    std::string scenePath = StringHelper::Sprintf("scenes/%s/%s/%s", sceneVersion.c_str(), entry.name.c_str(), entry.name.c_str());

    play->sceneSegment = OTRPlay_LoadFile(play, scenePath.c_str());

    // Failed to load scene... default to doodongs cavern
    if (play->sceneSegment == nullptr) 
    {
        lusprintf(__FILE__, __LINE__, 2, "Unable to load scene %s... Defaulting to Doodong's Cavern!\n",
                  scenePath.c_str());
        OTRPlay_SpawnScene(play, 0x01, 0);
        return;
    }

    OTRPlay_InitScene(play, spawn);
    uint32_t roomSize = func_80096FE8(play, &play->roomCtx);

    osSyncPrintf("ROOM SIZE=%fK\n", roomSize / 1024.0f);
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
    Play_InitEnvironment(play, play->skyboxId);
    /* auto data = static_cast<LUS::Vertex*>(Ship::Context::GetInstance()
                                               ->GetResourceManager()
                                               ->ResourceLoad("object_link_child\\object_link_childVtx_01FE08")
                                               .get());

    auto data2 = ResourceMgr_LoadVtxByCRC(0x68d4ea06044e228f);*/
    
    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSceneInit>(play->sceneNum);
    SPDLOG_INFO("Scene Init - sceneNum: {0:#x}, entranceIndex: {1:#x}", play->sceneNum, gSaveContext.entranceIndex);

    volatile int a = 0;
}
