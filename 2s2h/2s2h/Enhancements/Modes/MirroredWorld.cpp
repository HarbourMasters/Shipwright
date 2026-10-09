#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/ShipUtils.h"

extern "C" {
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

static constexpr MirroredWorldModeOptions CVAR_MODE_DEFAULT = MIRRORED_WORLD_OFF;
#define CVAR_MODE_NAME "gModes.MirroredWorld.Mode"
#define CVAR_MODE CVarGetInteger(CVAR_MODE_NAME, CVAR_MODE_DEFAULT)
#define CVAR_INISIE_R_NAME "gModes.MirroredWorld.StoneTowerTempleFix"
#define CVAR_INISIE_R CVarGetInteger(CVAR_INISIE_R_NAME, 0)
#define CVAR_STATE_NAME "gModes.MirroredWorld.State"

static bool prevMirroredWorld = false;

static bool MirroredWorld_IsInTemple(int32_t sceneId) {
    switch (sceneId) {
        case SCENE_MITURIN:    // Woodfall Temple
        case SCENE_MITURIN_BS: // Woodfall Temple Boss Room
        case SCENE_HAKUGIN:    // Snowhead Temple
        case SCENE_HAKUGIN_BS: // Snowhead Temple Boss Room
        case SCENE_SEA:        // Great Bay Temple
        case SCENE_SEA_BS:     // Great Bay Temple Boss Room
        case SCENE_INISIE_N:   // Stone Tower Temple
        case SCENE_INISIE_BS:  // Stone Tower Temple Boss Room
        case SCENE_INISIE_R:   // Inverted Stone Tower Temple
            return true;
        default:
            return false;
    }
}

static bool MirroredWorld_IsInSpiderHouse(int32_t sceneId) {
    return (sceneId == SCENE_KINSTA1) || (sceneId == SCENE_KINDAN2);
}

static void MirroredWorld_InitRandomSeed(int32_t sceneId) {
    uint32_t seed = sceneId + (IS_RANDO ? gSaveContext.save.shipSaveInfo.rando.finalSeed
                                        : gSaveContext.save.shipSaveInfo.fileCreatedAt);
    Ship_Random_Seed(seed);
}

static bool MirroredWorld_ShouldApply(int32_t sceneId) {
    switch (CVAR_MODE) {
        case MIRRORED_WORLD_ALWAYS:
            return true;
        case MIRRORED_WORLD_RANDOM_SEEDED:
            MirroredWorld_InitRandomSeed(sceneId);
        case MIRRORED_WORLD_RANDOM:
            return Ship_Random(0, 2) == 1;
        case MIRRORED_WORLD_DUNGEONS_TEMPLES:
            return MirroredWorld_IsInTemple(sceneId);
        case MIRRORED_WORLD_DUNGEONS_SPIDERS:
            return MirroredWorld_IsInSpiderHouse(sceneId);
        case MIRRORED_WORLD_DUNGEONS_ALL:
            return MirroredWorld_IsInTemple(sceneId) || MirroredWorld_IsInSpiderHouse(sceneId);
        case MIRRORED_WORLD_DUNGEONS_RANDOM_SEEDED:
            MirroredWorld_InitRandomSeed(sceneId);
        case MIRRORED_WORLD_DUNGEONS_RANDOM:
            return (MirroredWorld_IsInTemple(sceneId) || MirroredWorld_IsInSpiderHouse(sceneId)) &&
                   (Ship_Random(0, 2) == 1);
        default:
            return false;
    }
}

static void UpdateMirrorModeState(int32_t sceneId, uint8_t spawnNum) {
    bool nextMirroredWorld = MirroredWorld_ShouldApply(sceneId);

    if (sceneId == SCENE_INISIE_R && CVAR_INISIE_R) {
        nextMirroredWorld = !nextMirroredWorld;
    }

    if (prevMirroredWorld == nextMirroredWorld) {
        return;
    }
    prevMirroredWorld = nextMirroredWorld;

    if (nextMirroredWorld) {
        CVarSetInteger(CVAR_STATE_NAME, 1);
    } else {
        CVarClear(CVAR_STATE_NAME);
    }
}

static void RegisterMirroredWorld() {
    if (gPlayState != NULL) {
        UpdateMirrorModeState(gPlayState->sceneId, gPlayState->curSpawn);
    }

    COND_HOOK(OnSceneInit, CVAR_MODE || CVAR_INISIE_R, UpdateMirrorModeState);
}

static RegisterShipInitFunc initFunc(RegisterMirroredWorld, { CVAR_MODE_NAME, CVAR_INISIE_R_NAME });
