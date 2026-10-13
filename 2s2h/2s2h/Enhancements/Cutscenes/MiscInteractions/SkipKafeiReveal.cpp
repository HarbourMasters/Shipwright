#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipMiscInteractions"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// Skips the interaction in which kafei reveals his secret to Link, and gives him the Pendant of Memories.
void RegisterSkipKafeiReveal() {
    // "...Can you keep a secret?"
    COND_ID_HOOK(OnOpenText, 0x296A, CVAR, [](u16* textId, bool* loadFromMessageTable) {
        *textId = 0x2976; // "Keep what we just talked about a secret from everyone."
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = true;

        entry.msg = "Please, deliver this to Anju...";

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;

        // Set flags that are normally set in the experience that this skips
        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_PENDANT_OF_MEMORIES);
        SET_WEEKEVENTREG(WEEKEVENTREG_51_08);
        Message_BombersNotebookQueueEvent(gPlayState, BOMBERS_NOTEBOOK_EVENT_MET_KAFEI);
        Message_BombersNotebookQueueEvent(gPlayState, BOMBERS_NOTEBOOK_EVENT_RECEIVED_PENDANT_OF_MEMORIES);

        if (GameInteractor_Should(VB_GIVE_PENDANT_OF_MEMORIES_FROM_KAFEI, true)) {
            GameInteractor::Instance->events.emplace_back(
                GIEventGiveItem{ .showGetItemCutscene = true,
                                 .param = GID_PENDANT_OF_MEMORIES,
                                 .giveItem = [](Actor* actor, PlayState* play) {
                                     if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                                         CustomMessage::SetActiveCustomMessage("You received the Pendant of Memories!",
                                                                               { .textboxType = 2 });
                                     } else {
                                         CustomMessage::StartTextbox(
                                             "You received the Pendant of Memories!\x1C\x02\x10", { .textboxType = 2 });
                                     }
                                     Item_Give(play, ITEM_PENDANT_OF_MEMORIES);
                                 } });
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipKafeiReveal, { CVAR_NAME });
