#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Rando/Rando.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/CustomMessage/CustomMessage.h"

extern "C" {
#include "overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.h"
}

#define CVAR_NAME "gEnhancements.Timesavers.GalleryTwofer"
#define CVAR CVarGetInteger(CVAR_NAME, 0)
#define SWAMP_CVAR_NAME "gEnhancements.Minigames.SwampArcheryScore"
#define SWAMP_CVAR CVarGetInteger(SWAMP_CVAR_NAME, 2180)
#define TOWN_CVAR_NAME "gEnhancements.Minigames.TownArcheryScore"
#define TOWN_CVAR CVarGetInteger(TOWN_CVAR_NAME, 50)

static s16 highestScore = 0;

void RegisterGalleryTwofer() {
    COND_HOOK(OnFlagSet, CVAR, [](FlagType flagType, u32 flag) {
        bool queueHeartPiece = false;

        if (flagType == FLAG_WEEK_EVENT_REG && flag == WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_QUIVER_UPGRADE &&
            !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_HEART_PIECE) &&
            HS_GET_SWAMP_SHOOTING_GALLERY_HIGH_SCORE() >= SWAMP_CVAR) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_HEART_PIECE);
            queueHeartPiece = true;
        }

        if (flagType == FLAG_WEEK_EVENT_REG && flag == WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_QUIVER_UPGRADE &&
            !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_HEART_PIECE) &&
            HS_GET_TOWN_SHOOTING_GALLERY_HIGH_SCORE() >= TOWN_CVAR) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_HEART_PIECE);
            queueHeartPiece = true;
        }

        if (!IS_RANDO && queueHeartPiece) {
            GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                .showGetItemCutscene = true,
                .param = GID_HEART_PIECE,
                .giveItem =
                    [](Actor* actor, PlayState* play) {
                        if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                            CustomMessage::SetActiveCustomMessage("You received a Piece of Heart!",
                                                                  { .textboxType = 2 });
                        } else {
                            CustomMessage::StartTextbox("You received a Piece of Heart!\x1C\x02\x10",
                                                        { .textboxType = 2 });
                        }
                        Item_Give(gPlayState, ITEM_HEART_PIECE);
                    },
            });
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterGalleryTwofer, { CVAR_NAME });