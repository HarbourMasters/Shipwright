#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/resource/type/scenecommand/SetTimeSettings.h"

bool Scene_CommandTimeSettings(PlayState* play, SOH::ISceneCommand* cmd);

extern "C" {
#include "variables.h"
#include "z64save.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

#define CVAR_NAME CVAR_ENHANCEMENT("3DSceneRender")
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, 0)

std::set<SceneID> fogControlList = {
    SCENE_MARKET_ENTRANCE_DAY,
    SCENE_MARKET_ENTRANCE_NIGHT,
    SCENE_MARKET_ENTRANCE_RUINS,
    SCENE_BACK_ALLEY_DAY,
    SCENE_BACK_ALLEY_NIGHT,
    SCENE_MARKET_DAY,
    SCENE_MARKET_NIGHT,
    SCENE_MARKET_RUINS,
    SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY,
    SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT,
    SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS,
};

std::set<SceneID> skyboxSceneControlList = {
    SCENE_MARKET_ENTRANCE_DAY,
    SCENE_MARKET_ENTRANCE_NIGHT,
    SCENE_MARKET_ENTRANCE_RUINS,
    SCENE_BACK_ALLEY_DAY,
    SCENE_BACK_ALLEY_NIGHT,
    SCENE_MARKET_DAY,
    SCENE_MARKET_NIGHT,
    SCENE_MARKET_RUINS,
    SCENE_CASTLE_COURTYARD_ZELDA,
    SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY,
    SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT,
    SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS,
    SCENE_FOREST_TEMPLE,
};

std::set<SkyboxId> skyboxIdControlList = {
    SKYBOX_BAZAAR,
    SKYBOX_HOUSE_LINK,
    SKYBOX_MARKET_ADULT,
    SKYBOX_MARKET_CHILD_DAY,
    SKYBOX_MARKET_CHILD_NIGHT,
    SKYBOX_HAPPY_MASK_SHOP,
    SKYBOX_HOUSE_KNOW_IT_ALL_BROTHERS,
    SKYBOX_HOUSE_OF_TWINS,
    SKYBOX_STABLES,
    SKYBOX_HOUSE_KAKARIKO,
    SKYBOX_KOKIRI_SHOP,
    SKYBOX_GORON_SHOP,
    SKYBOX_ZORA_SHOP,
    SKYBOX_POTION_SHOP_KAKARIKO,
    SKYBOX_POTION_SHOP_MARKET,
    SKYBOX_HOUSE_RICHARD,
    SKYBOX_HOUSE_IMPA,
    SKYBOX_TENT,
    SKYBOX_HOUSE_MIDO,
    SKYBOX_HOUSE_SARIA,
    SKYBOX_HOUSE_ALLEY,
};

static void SyncSkyboxTimeToCurrentTime() {
    SOH::SetTimeSettings cmd;
    cmd.settings = { 0xFF, 0xFF, static_cast<uint8_t>(gPlayState->envCtx.timeIncrement) };
    Scene_CommandTimeSettings(gPlayState, &cmd);
}

void Register3DPreRenderedScenes() {
    // Runs after the scene commands have set play->skyboxId, but before Play_InitEnvironment builds the
    // skybox. Overriding the id here means Skybox_Setup loads a real sky; overriding it any later would
    // leave the display lists pointing at the pre-rendered skybox's texture slots.
    COND_HOOK(AfterSceneCommands, CVAR_VALUE, [](int16_t sceneNum) {
        if (!skyboxSceneControlList.contains(static_cast<SceneID>(sceneNum)) &&
            !skyboxIdControlList.contains(static_cast<SkyboxId>(gPlayState->skyboxId))) {
            return;
        }

        // Add a skybox on scenes from skyboxSceneControlList
        gPlayState->envCtx.skyboxDisabled = false;

        // Replace skybox with normal sky
        SyncSkyboxTimeToCurrentTime();
        gPlayState->skyboxId = SKYBOX_NORMAL_SKY;
        // Apply the always cloudy skybox as an adult for Temple of Time and the Market
        if (sceneNum == SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS || sceneNum == SCENE_MARKET_RUINS ||
            sceneNum == SCENE_MARKET_ENTRANCE_RUINS) {
            gWeatherMode = 3;
            gSaveContext.retainWeatherMode = 1;
        }
    });

    COND_HOOK(OnPlayDrawBegin, CVAR_VALUE, []() {
        if (!fogControlList.contains(static_cast<SceneID>(gPlayState->sceneNum))) {
            return;
        }

        if ((HREG(80) != 10) || (HREG(82) != 0)) {
            // Furthest possible fog and zFar
            gPlayState->view.zFar = 12800;
            gPlayState->lightCtx.fogNear = 1000;
            gPlayState->lightCtx.fogFar = 12800;
        }
    });

    COND_VB_SHOULD(VB_DRAW_2D_BACKGROUND, CVAR_VALUE, { *should = false; });
}

static RegisterShipInitFunc initFunc(Register3DPreRenderedScenes, { CVAR_NAME });
