#include <libultraship/bridge.h>
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/enhancementTypes.h"

extern "C" {
#include "macros.h"
#include "variables.h"

extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

void CheatsOnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs) {
    switch (id) {
        case VB_DEKU_STICK_BREAK: {
            if (CVarGetInteger(CVAR_CHEAT("DekuStick"), DEKU_STICK_NORMAL) != DEKU_STICK_NORMAL) {
                *should = false;
            }
            break;
        }
        case VB_DEKU_STICK_BE_ON_FIRE: {
            if (CVarGetInteger(CVAR_CHEAT("DekuStick"), DEKU_STICK_NORMAL) == DEKU_STICK_UNBREAKABLE_AND_ALWAYS_ON_FIRE) {
                Player* player = GET_PLAYER(gPlayState);
                player->unk_860 = 200;    // Keeps the stick's flame lit
                player->unk_85C = 1.0f;   // Ensures the stick is the proper length
                *should = true;
            }
            break;
        }
        case VB_DEKU_STICK_BURN_OUT: {
            if (CVarGetInteger(CVAR_CHEAT("DekuStick"), DEKU_STICK_NORMAL) != DEKU_STICK_NORMAL) {
                *should = false;
            }
            break;
        }
        case VB_DEKU_STICK_BURN_DOWN: {
            if (CVarGetInteger(CVAR_CHEAT("DekuStick"), DEKU_STICK_NORMAL) != DEKU_STICK_NORMAL) {
                *should = false;
            }
            break;
        }
        default:
            break;
    }
}

static uint32_t onVanillaBehaviorHook = 0;
void CheatsRegisterHooks() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) mutable {

        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnVanillaBehavior>(onVanillaBehaviorHook);
        onVanillaBehaviorHook = 0;
        onVanillaBehaviorHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnVanillaBehavior>(CheatsOnVanillaBehaviorHandler);

    });
}
