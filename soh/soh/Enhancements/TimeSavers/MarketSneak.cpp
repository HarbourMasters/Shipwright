#include <soh/OTRGlobals.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/randomizer/randomizer.h"
#include "soh/Enhancements/randomizer/randomizer_entrance.h"

extern "C" {
#include <functions.h>
#include <variables.h>
#include "overlays/actors/ovl_En_Heishi4/z_en_heishi4.h"
extern PlayState* gPlayState;

// the guard's idle action func, restored once the sneak prompt is answered
void func_80A56614(EnHeishi4* heishi, PlayState* play);

u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);
}

void BuildNightGuardMessage(uint16_t* textId, bool* loadFromMessageTable) {
    // Other guards should not have their text overridden
    if (gPlayState->sceneNum != SCENE_MARKET_ENTRANCE_NIGHT) {
        return;
    }

    CustomMessage msg = CustomMessage("You look bored. Wanna go out for a walk?\x1B%gYes&No%w",
                                      "Du siehst gelangweilt aus. Willst Du einen Spaziergang machen?\x1B%gJa&Nein%w",
                                      "Tu as l'air de t'ennuyer. Tu veux aller faire un tour?\x1B%gOui&Non%w");
    msg.AutoFormat();
    msg.LoadIntoFont();
    *loadFromMessageTable = false;
}

// Answers the choice from BuildNightGuardMessage, letting child Link exit from the Market
// entrance to Hyrule Field at night.
extern "C" void EnHeishi4_MarketSneak(EnHeishi4* heishi, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // yes
                if (IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_ENTRANCES) != RO_GENERIC_OFF) {
                    play->nextEntranceIndex =
                        Entrance_OverrideNextIndex(ENTR_HYRULE_FIELD_ON_BRIDGE_SPAWN); // Market Entrance -> HF
                } else {
                    play->nextEntranceIndex = ENTR_HYRULE_FIELD_PAST_BRIDGE_SPAWN; // HF Near bridge (OoT cutscene
                                                                                   // entrance) to not fall in the water
                }
                play->transitionTrigger = TRANS_TRIGGER_START;
                play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
                gSaveContext.nextTransitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
                heishi->actionFunc = func_80A56614;
                break;
            case 1: // no
                heishi->actionFunc = func_80A56614;
                break;
        }
    }
}

void MarketSneak_Register() {
    COND_ID_HOOK(OnOpenText, TEXT_MARKET_GUARD_NIGHT, CVarGetInteger(CVAR_ENHANCEMENT("MarketSneak"), 0),
                 BuildNightGuardMessage);

    COND_VB_SHOULD(VB_MARKET_NIGHT_GUARD_SET_ACTION_AFTER_TALK, CVarGetInteger(CVAR_ENHANCEMENT("MarketSneak"), 0), {
        EnHeishi4* heishi = va_arg(args, EnHeishi4*);
        PlayState* play = va_arg(args, PlayState*);
        Player* player = GET_PLAYER(play);

        // Only allow sneaking when not wearing a mask as that triggers different dialogue. A mask NPCs
        // don't react to (MM Bunny hood) is fine in that case.
        if (player->currentMask != PLAYER_MASK_NONE &&
            GameInteractor_Should(VB_NPC_REACT_TO_MASK, true, player->currentMask)) {
            return;
        }

        heishi->actionFunc = EnHeishi4_MarketSneak;
        *should = false;
    });
}

static RegisterShipInitFunc initFunc(MarketSneak_Register, { CVAR_ENHANCEMENT("MarketSneak") });
