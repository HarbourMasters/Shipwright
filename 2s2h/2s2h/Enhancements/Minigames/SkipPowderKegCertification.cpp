#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Rando/Rando.h"
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "overlays/actors/ovl_En_Go/z_en_go.h"
}

#define CVAR_NAME "gEnhancements.Timesavers.PowderKegCertification"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterPowderKegCertification() {
    COND_VB_SHOULD(VB_EXEC_MSG_EVENT, CVAR, {
        u32 cmdId = va_arg(args, u32);
        Actor* actor = va_arg(args, Actor*);
        Player* player = GET_PLAYER(gPlayState);

        if (actor->id == ACTOR_EN_GO && ENGO_GET_TYPE(actor) == ENGO_MEDIGORON &&
            player->transformation == PLAYER_FORM_GORON && !CHECK_WEEKEVENTREG(WEEKEVENTREG_19_02)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_19_01); // Started Test
            SET_WEEKEVENTREG(WEEKEVENTREG_19_02); // Succeeded Test

            // TODO: Probably migrate this to some unified flag system.
            gSaveContext.cycleSceneFlags[SCENE_17SETUGEN].switch0 |= 1 << (0x34 & 0x1f);
        }
    });

    // "Looks like you succeeded..."
    COND_ID_HOOK(OnOpenText, 0x0C86, CVAR && !IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "Take one on the house, don't tell your parents.";

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterPowderKegCertification, { CVAR_NAME, "IS_RANDO" });
