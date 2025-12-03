#include "soh/Enhancements/cosmetics/authenticGfxPatches.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/randomizer/3drando/random.hpp"
#include "soh/Enhancements/randomizer/context.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

static constexpr MirroredWorldMode CVAR_MIRRORED_WORLD_DEFAULT = MIRRORED_WORLD_OFF;
#define CVAR_MIRRORED_WORLD_NAME CVAR_ENHANCEMENT("MirroredWorld")
#define CVAR_MIRRORED_WORLD_MODE_NAME CVAR_ENHANCEMENT("MirroredWorldMode")
#define CVAR_MIRRORED_WORLD_MODE_VALUE CVarGetInteger(CVAR_MIRRORED_WORLD_MODE_NAME, CVAR_MIRRORED_WORLD_DEFAULT)

static bool prevMirroredWorld = false;

static bool MirroredWorld_IsInDungeon(int32_t sceneNum) {
    return (sceneNum >= SCENE_DEKU_TREE && sceneNum <= SCENE_INSIDE_GANONS_CASTLE_COLLAPSE &&
            sceneNum != SCENE_THIEVES_HIDEOUT) ||
           (sceneNum >= SCENE_DEKU_TREE_BOSS && sceneNum <= SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) ||
           (sceneNum == SCENE_GANON_BOSS);
}

static void MirroredWorld_InitRandomSeed(int32_t sceneNum) {
    uint32_t seed =
        sceneNum + (IS_RANDO ? Rando::Context::GetInstance()->GetSeed() : gSaveContext.ship.stats.fileCreatedAt);
    Random_Init(seed);
}

static bool MirroredWorld_ShouldApply(int32_t sceneNum) {
    switch (CVAR_MIRRORED_WORLD_MODE_VALUE) {
        case MIRRORED_WORLD_ALWAYS:
            return true;
        case MIRRORED_WORLD_RANDOM_SEEDED:
            MirroredWorld_InitRandomSeed(sceneNum);
        case MIRRORED_WORLD_RANDOM:
            return Random(0, 2) == 1;
        case MIRRORED_WORLD_DUNGEONS_ALL:
            return MirroredWorld_IsInDungeon(sceneNum);
        case MIRRORED_WORLD_DUNGEONS_VANILLA:
            return MirroredWorld_IsInDungeon(sceneNum) && !ResourceMgr_IsSceneMasterQuest(sceneNum);
        case MIRRORED_WORLD_DUNGEONS_MQ:
            return MirroredWorld_IsInDungeon(sceneNum) && ResourceMgr_IsSceneMasterQuest(sceneNum);
        case MIRRORED_WORLD_DUNGEONS_RANDOM_SEEDED:
            MirroredWorld_InitRandomSeed(sceneNum);
        case MIRRORED_WORLD_DUNGEONS_RANDOM:
            return MirroredWorld_IsInDungeon(sceneNum) && (Random(0, 2) == 1);
        default:
            return false;
    }
}

static void UpdateMirrorModeState(int32_t sceneNum) {
    bool nextMirroredWorld = MirroredWorld_ShouldApply(sceneNum);

    if (prevMirroredWorld == nextMirroredWorld) {
        return;
    }
    prevMirroredWorld = nextMirroredWorld;

    if (nextMirroredWorld) {
        CVarSetInteger(CVAR_MIRRORED_WORLD_NAME, 1);
    } else {
        CVarClear(CVAR_MIRRORED_WORLD_NAME);
    }

    ApplyMirrorWorldGfxPatches();
}

static void RegisterMirroredWorld() {
    if (gPlayState != NULL) {
        UpdateMirrorModeState(gPlayState->sceneNum);
    }

    COND_HOOK(OnSceneInit, CVAR_MIRRORED_WORLD_MODE_VALUE, UpdateMirrorModeState);
}

static RegisterShipInitFunc initFunc(RegisterMirroredWorld, { CVAR_MIRRORED_WORLD_MODE_NAME });
