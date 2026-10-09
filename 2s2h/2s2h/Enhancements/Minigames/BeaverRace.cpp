#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "CustomMessage/CustomMessage.h"

extern "C" {
#include "overlays/actors/ovl_En_Az/z_en_az.h"

void func_80A979DC(EnAz* thisx, PlayState* play);
void func_80A97F9C(EnAz* thisx, PlayState* play);
}

#define CVAR_RINGS_NAME "gEnhancements.Minigames.BeaverRaceRingsCollected"
#define CVAR_RINGS CVarGetInteger(CVAR_RINGS_NAME, 20)

#define CVAR_SPEEDUP_NAME "gEnhancements.Minigames.SkipLittleBeaver"
#define CVAR_SPEEDUP CVarGetInteger(CVAR_SPEEDUP_NAME, 0)

static bool minigameScoreSet = false; // Flag to track if the score has been set

void RegisterBeaverRaceRings() {
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_AZ, CVAR_RINGS < 20, [](Actor* actor, bool* should) {
        EnAz* enAz = (EnAz*)actor;
        Player* player = GET_PLAYER(gPlayState);

        if (!minigameScoreSet) {
            gSaveContext.minigameScore = CVAR_RINGS;
            minigameScoreSet = true; // Set the flag after assignment
        }

        if (enAz->actionFunc != func_80A97F9C) {
            minigameScoreSet = false;
            return;
        }

        // Check if minigameScore has already been set

        bool allRingsCollected = false;

        // Check WEEKEVENTREG_24_01 and set condition based on the score.
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_24_04)) {
            if (gSaveContext.minigameScore <= 0) {
                allRingsCollected = true;
            }
        } else {
            if (gSaveContext.minigameScore <= 0) {
                allRingsCollected = true;
            }
        }

        // If the condition is met, execute the shared code
        if (allRingsCollected) {
            SET_WEEKEVENTREG(WEEKEVENTREG_24_01);
            gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_STOP;
            enAz->unk_374 &= ~0x10;
            gPlayState->nextEntrance = Entrance_CreateFromSpawn(2);
            gSaveContext.nextCutsceneIndex = 0;
            gPlayState->transitionTrigger = TRANS_TRIGGER_START;
            gPlayState->transitionType = TRANS_TYPE_FADE_WHITE;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
            enAz->actor.speed = 0.0f;
            func_80A979DC(enAz, gPlayState);
        }
    });
}

void RegisterBeaverRaceSpeedup() {
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_AZ, CVAR_SPEEDUP, [](Actor* actor, bool* should) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_24_04)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_24_04);
        }
    });

    COND_ID_HOOK(OnOpenText, 0x10D6, CVAR_SPEEDUP, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "My older brother will show you\x11the way, so follow him and\x11"
                    "don't get separated!";

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, 0x10FA, CVAR_SPEEDUP, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "The time limit is %r1:50%w, let's race.";

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}

static RegisterShipInitFunc initRingsFunc(RegisterBeaverRaceRings, { CVAR_RINGS_NAME });
static RegisterShipInitFunc initSpeedupFunc(RegisterBeaverRaceSpeedup, { CVAR_SPEEDUP_NAME });
