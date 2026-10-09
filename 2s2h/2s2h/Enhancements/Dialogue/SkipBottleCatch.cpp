#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/BenGui/Notification.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

#define CVAR_NAME "gEnhancements.Dialogue.SkipBottlePickupMessages"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipBottleCatchCutscene() {
    COND_VB_SHOULD(VB_PLAY_BOTTLE_CATCH_TEXT, CVAR, {
        if (*should) {
            Player* player = va_arg(args, Player*);
            Actor* interactRangeActor = va_arg(args, Actor*);
            ItemId itemId = (ItemId)va_arg(args, int);
            PlayerItemAction itemAction = (PlayerItemAction)va_arg(args, int);

            interactRangeActor->parent = &player->actor;
            Player_UpdateBottleHeld(gPlayState, player, itemId, itemAction);
            Audio_PlayFanfare(NA_BGM_GET_ITEM);

            if (itemAction == PLAYER_IA_BOTTLE_DEKU_PRINCESS) {
                // Kill Deku Princess actor immediately upon capture
                Actor_Kill(interactRangeActor);
            }

            Notification::Emit({
                .itemIcon = (const char*)gItemIcons[itemId],
                .message = "Caught!",
            });

            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipBottleCatchCutscene, { CVAR_NAME });