#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
}

#define CVAR_NAME "gCheats.TempleTimeStop"
#define CVAR CVarGetInteger(CVAR_NAME, TIME_STOP_OFF)

void RegisterTimeStopInTemples() {
    COND_HOOK(AfterRoomSceneCommands, CVAR != TIME_STOP_OFF, [](s16 sceneId, s8 roomNum) {
        uint8_t selectedOption = CVarGetInteger("gCheats.TempleTimeStop", TIME_STOP_OFF);

        switch (selectedOption) {
            case TIME_STOP_TEMPLES_DUNGEONS:
                switch (gPlayState->sceneId) {
                    // Swamp + Ocean Spider House
                    case SCENE_KINSTA1:
                    case SCENE_KINDAN2:
                    // Pirates' Fortress
                    case SCENE_KAIZOKU:
                    case SCENE_PIRATE:
                    case SCENE_TORIDE:
                    // Beneath the Well
                    case SCENE_REDEAD:
                    // Ancient Castle of Ikana + Igos's Lair
                    case SCENE_CASTLE:
                    case SCENE_IKNINSIDE:
                    // Secret Shrine
                    case SCENE_RANDOM:
                        R_TIME_SPEED = 0;
                        break;
                    default:
                        break;
                }
                // fallthrough
            case TIME_STOP_TEMPLES:
                switch (gPlayState->sceneId) {
                    // Woodfall Temple + Odolwa
                    case SCENE_MITURIN:
                    case SCENE_MITURIN_BS:
                    // Snowhead Temple + Goht
                    case SCENE_HAKUGIN:
                    case SCENE_HAKUGIN_BS:
                    // Great Bay Temple + Gyorg
                    case SCENE_SEA:
                    case SCENE_SEA_BS:
                    // Stone Tower Temple (+ inverted) + Twinmold
                    case SCENE_INISIE_N:
                    case SCENE_INISIE_R:
                    case SCENE_INISIE_BS:
                        R_TIME_SPEED = 0;
                        break;
                    default:
                        break;
                }
                break;
            case TIME_STOP_OFF:
                break;
            default:
                break;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterTimeStopInTemples, { CVAR_NAME });
