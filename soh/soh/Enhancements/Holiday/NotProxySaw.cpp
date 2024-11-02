#include "Holiday.hpp"
#include <libultraship/libultraship.h>
#include "soh/UIWidgets.hpp"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/custom-message/CustomMessageManager.h"
#include "include/message_data_fmt.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
}

#define AUTHOR "NotProxySaw"
#define CVAR(v) "gHoliday." AUTHOR "." v

int dialogIndex = 0;
int affection = 0;
int TARGET_AFFECTION = 10;
struct DialogOption {
    std::string text;
    int affectionChange;
    int nextDialogIndex;
};
struct Dialog {
    std::string ganonText;
    std::vector<DialogOption> options;
};

const std::vector<Dialog> dialogs = {
    { // 0
        "Think you can defeat me? Foolish!",
        {
            {"You're lonely. You don't have to be.", 3, 1},
            {"I'm here to end this... peacefully.", 1, 2},
            {"I respect a man with ambition.", -1, 3},
        }
    },
    { // 1
        "Lonely? Power's my only ally.",
        {
            {"There's more to you than that.", 3, 4},
            {"I get it more than you think.", 5, 4},
            {"Why not let someone in?", 2, 4},
        }
    },
    { // 2
        "Peace? It's meaningless here.",
        {
            {"Maybe you've forgotten peace.", 2, 4},
            {"Power isn't everything.", 4, 4},
            {"Ally, not enemy.. that's my goal.", 1, 4},
        }
    },
    { // 3
        "Respect? From you?",
        {
            {"We're not so different.", 3, 4},
            {"Maybe I admire your strength.", 5, 4},
            {"True power must be earned.", -1, 4},
        }
    },
    { // 4
        "You're... different than I thought.",
        {
            {"Together, we'd be unstoppable.", 5, 5},
            {"Power won't bring fulfillment.", 3, 5},
            {"Let's change the world.", 4, 5},
        }
    },
    { // 5
        "What if I trusted you with power?",
        {
            {"I'll protect it. And you.", 10, -1},
            {"Together, we're unstoppable.", 5, -1},
            {"Our way. Our history.", -3, -1},
        }
    }
};

static void ConfigurationChanged() {
    COND_ID_HOOK(OnActorInit, ACTOR_BOSS_GANON, CVarGetInteger(CVAR("GanonDatingSim"), 0), [](void* actorRef) {
        dialogIndex = 0;
        affection = 0;
    });

    COND_VB_SHOULD(VB_GANONDORF_DECIDE_TO_FIGHT, CVarGetInteger(CVAR("GanonDatingSim"), 0), {
        MessageContext* msgCtx = &gPlayState->msgCtx;

        if (dialogIndex == -1) {
            if (affection >= TARGET_AFFECTION) {
                gPlayState->nextEntranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;
                gSaveContext.nextCutsceneIndex = 0xFFF2;
                gPlayState->transitionTrigger = TRANS_TRIGGER_START;
                gPlayState->transitionType = TRANS_TYPE_FADE_WHITE;
                GameInteractor::State::TriforceHuntCreditsWarpActive = 0;
                *should = false;
                return;
            }
            return;
        }

        affection += dialogs[dialogIndex].options[msgCtx->choiceIndex].affectionChange;
        dialogIndex = dialogs[dialogIndex].options[msgCtx->choiceIndex].nextDialogIndex;

        *should = false;
        Message_StartTextbox(gPlayState, 0x70CB, NULL);
    });

    COND_ID_HOOK(OnOpenText, 0x70CB, CVarGetInteger(CVAR("GanonDatingSim"), 0), [](u16 * textId, bool* loadFromMessageTable) {
        std::string message;
        if (dialogIndex == -1) {
            if (affection >= TARGET_AFFECTION) {
                message = "I've never felt this way before...\x01Take my power, Link. I trust you.";
            } else {
                message = "Liar! You're just like the rest of\x01them! Now I must destroy you!";
            }
        } else {
            message = dialogs[dialogIndex].ganonText + "\x01\x1C"
                + dialogs[dialogIndex].options[0].text + "\x01"
                + dialogs[dialogIndex].options[1].text + "\x01"
                + dialogs[dialogIndex].options[2].text;
        }

        auto messageEntry = CustomMessage(message);
        messageEntry.Format();
        messageEntry.LoadIntoFont();
        *loadFromMessageTable = false;
    });
}

static void DrawMenu() {
    ImGui::SeparatorText(AUTHOR);
    if (UIWidgets::EnhancementCheckbox("Ganon Dating Sim", CVAR("GanonDatingSim"))) {
        ConfigurationChanged();
    }
}

static void RegisterMod() {
    // #region Leave this alone unless you know what you are doing
    ConfigurationChanged();
    // #endregion
}

static Holiday holiday(DrawMenu, RegisterMod);
