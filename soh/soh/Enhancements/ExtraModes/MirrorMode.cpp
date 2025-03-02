#include <libultraship/bridge.h>
#include "soh/Enhancements/cosmetics/authenticGfxPatches.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/randomizer/context.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
#include "macros.h"
}

#define CVAR_MIRRORED_WORLD_MODE_NAME CVAR_ENHANCEMENT("MirroredWorldMode")
#define CVAR_MIRRORED_WORLD_MODE_DEFAULT MIRRORED_WORLD_OFF
#define CVAR_MIRRORED_WORLD_MODE_VALUE CVarGetInteger(CVAR_MIRRORED_WORLD_MODE_NAME, CVAR_MIRRORED_WORLD_MODE_DEFAULT)

#define CVAR_MIRRORED_WORLD_NAME CVAR_ENHANCEMENT("MirroredWorld")
#define CVAR_MIRRORED_WORLD_DEFAULT false
#define CVAR_MIRRORED_WORLD_VALUE CVarGetInteger(CVAR_MIRRORED_WORLD_NAME, CVAR_MIRRORED_WORLD_DEFAULT)

void OnSceneInitMirrorMode(s32 sceneNum) {
    static bool prevMirroredWorld = false;
    bool nextMirroredWorld = false;

    s16 mirroredMode = CVAR_MIRRORED_WORLD_MODE_VALUE;
    s16 inDungeon = (sceneNum >= SCENE_DEKU_TREE && sceneNum <= SCENE_INSIDE_GANONS_CASTLE_COLLAPSE && sceneNum != SCENE_THIEVES_HIDEOUT) ||
                    (sceneNum >= SCENE_DEKU_TREE_BOSS && sceneNum <= SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) ||
                    (sceneNum == SCENE_GANON_BOSS);

    if (mirroredMode == MIRRORED_WORLD_RANDOM_SEEDED || mirroredMode == MIRRORED_WORLD_DUNGEONS_RANDOM_SEEDED) {
        u32 seed = sceneNum + (IS_RANDO ? Rando::Context::GetInstance()->GetSeed() : gSaveContext.ship.stats.fileCreatedAt);
        Random_Init(seed);
    }

    bool randomMirror = Random(0, 2) == 1;

    if (
        mirroredMode == MIRRORED_WORLD_ALWAYS ||
        ((mirroredMode == MIRRORED_WORLD_RANDOM || mirroredMode == MIRRORED_WORLD_RANDOM_SEEDED) && randomMirror) ||
        // Dungeon modes
        (inDungeon && (mirroredMode == MIRRORED_WORLD_DUNGEONS_All ||
         (mirroredMode == MIRRORED_WORLD_DUNGEONS_VANILLA && !ResourceMgr_IsSceneMasterQuest(sceneNum)) ||
         (mirroredMode == MIRRORED_WORLD_DUNGEONS_MQ && ResourceMgr_IsSceneMasterQuest(sceneNum)) ||
         ((mirroredMode == MIRRORED_WORLD_DUNGEONS_RANDOM || mirroredMode == MIRRORED_WORLD_DUNGEONS_RANDOM_SEEDED) && randomMirror)))
    ) {
        nextMirroredWorld = true;
        CVarSetInteger(CVAR_MIRRORED_WORLD_NAME, true);
    } else {
        nextMirroredWorld = false;
        CVarClear(CVAR_MIRRORED_WORLD_NAME);
    }

    if (prevMirroredWorld != nextMirroredWorld) {
        prevMirroredWorld = nextMirroredWorld;
        ApplyMirrorWorldGfxPatches();
    }
}

void RegisterMirrorMode() {
    COND_HOOK(OnSceneInit, CVAR_MIRRORED_WORLD_MODE_VALUE, OnSceneInitMirrorMode);
    if (gPlayState != NULL) {
        OnSceneInitMirrorMode(gPlayState->sceneNum);
    }
}

static RegisterShipInitFunc initFunc(RegisterMirrorMode, { CVAR_MIRRORED_WORLD_MODE_NAME });
