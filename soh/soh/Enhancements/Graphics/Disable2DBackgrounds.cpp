#include <libultraship/bridge.h>
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
#include "macros.h"
#include "variables.h"
}

#define CVAR_NAME CVAR_SETTING("3DSceneRender")
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, 0)

std::vector<SceneID> fogControlList = {
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
    SCENE_KNOW_IT_ALL_BROS_HOUSE,
    SCENE_TWINS_HOUSE,
    SCENE_MIDOS_HOUSE,
    SCENE_SARIAS_HOUSE,
    SCENE_BACK_ALLEY_HOUSE,
    SCENE_BAZAAR,
    SCENE_KOKIRI_SHOP,
    SCENE_GORON_SHOP,
    SCENE_ZORA_SHOP,
    SCENE_POTION_SHOP_KAKARIKO,
    SCENE_POTION_SHOP_MARKET,
    SCENE_BOMBCHU_SHOP,
    SCENE_HAPPY_MASK_SHOP,
    SCENE_LINKS_HOUSE,
    SCENE_DOG_LADY_HOUSE,
    SCENE_STABLE,
    SCENE_IMPAS_HOUSE,
    SCENE_CARPENTERS_TENT,
    SCENE_GRAVEKEEPERS_HUT,
};

std::vector<SceneID> skyboxControlList = { 
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
    SCENE_FOREST_TEMPLE
};


void Register3DPreRenderedScenes() {
    COND_HOOK(AfterSceneCommands, CVAR_VALUE, [](int16_t sceneNum) {
        // Check if this scene is in the skyboxControlList
        bool shouldControlSkybox = false;
        for (const auto& scene : skyboxControlList) {
            if (sceneNum == scene) {
                shouldControlSkybox = true;
                break;
            }
        }

        if (shouldControlSkybox) {
            // Add a skybox on scenes from skyboxControlList
            gPlayState->envCtx.skyboxDisabled = false;

            // Replace skybox with normal sky
            gPlayState->skyboxId = SKYBOX_NORMAL_SKY;
            // Apply the always cloudy skybox as an adult for Temple of Time and the Market
            if (LINK_IS_ADULT && sceneNum == SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS || sceneNum == SCENE_MARKET_RUINS ||
                sceneNum == SCENE_MARKET_ENTRANCE_RUINS) {
                gWeatherMode = 3;
            }
        }
    });

    COND_HOOK(OnPlayDrawBegin, CVAR_VALUE, []() {
        if (!CVarGetInteger(CVAR_SETTING("3DSceneRender"), 0)) {
            return;
        }
        for (auto& scene : fogControlList) {
            if (scene == gPlayState->sceneNum) {
                if ((HREG(80) != 10) || (HREG(82) != 0)) {
                    // Furthest possible fog and zFar
                    gPlayState->view.zFar = 12800;
                    gPlayState->lightCtx.fogNear = 996; // Set to 1000 to complete disable fog entirely
                    gPlayState->lightCtx.fogFar = 12800;
                    // General gray fog color
                    gPlayState->lightCtx.fogColor[0] = 100;
                    gPlayState->lightCtx.fogColor[1] = 100;
                    gPlayState->lightCtx.fogColor[2] = 100;
                }
                break;
            }
        }
    });
    REGISTER_VB_SHOULD(VB_DRAW_2D_BACKGROUND, {
        if (CVAR_VALUE) {
            *should = false;
            return;
        }
    });
}

static RegisterShipInitFunc initFunc(Register3DPreRenderedScenes, { CVAR_NAME });