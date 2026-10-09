#include "MiscBehavior.h"
#include "2s2h/Rando/Logic/Logic.h"

extern "C" {
#include "variables.h"
#include "functions.h"
}

// Entry point for the module, run once on game boot
void Rando::MiscBehavior::Init() {
    Rando::MiscBehavior::InitFileSelect();
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveInit>(Rando::MiscBehavior::OnFileCreate);
}

void Rando::MiscBehavior::OnFileLoad() {
    Rando::MiscBehavior::CheckQueueReset();
    Rando::MiscBehavior::InitKaleidoItemPage();
    Rando::MiscBehavior::InitOfferGetItemBehavior();
    Rando::MiscBehavior::SariasSongHint();
    Rando::MiscBehavior::BankSignHint();
    Rando::MiscBehavior::InitTycoonWallet();

    COND_HOOK(OnFlagSet, IS_RANDO, Rando::MiscBehavior::OnFlagSet);
    COND_HOOK(OnSceneFlagSet, IS_RANDO, Rando::MiscBehavior::OnSceneFlagSet);
    COND_HOOK(BeforeEndOfCycleSave, IS_RANDO, Rando::MiscBehavior::BeforeEndOfCycleSave);
    COND_HOOK(AfterEndOfCycleSave, IS_RANDO, Rando::MiscBehavior::AfterEndOfCycleSave);
    COND_HOOK(OnSceneInit, IS_RANDO, Rando::MiscBehavior::OnSceneInit);
    COND_ID_HOOK(OnActorUpdate, ACTOR_PLAYER, IS_RANDO, [](Actor* actor) { Rando::MiscBehavior::CheckQueue(); });

    // This overrides the ocarina condition for Termina Field
    COND_VB_SHOULD(VB_TERMINA_FIELD_BE_EMPTY, IS_RANDO, { *should = false; });

    // Override faster first-cycle time speed if you don't have the Ocarina
    COND_VB_SHOULD(VB_FASTER_FIRST_CYCLE, IS_RANDO, { *should = false; });

    // The game normally only checks the trade slot for the room key directly, which would mean the player could be
    // denied entry to the Stock Pot Inn if they have the room key but it isn't assigned as the active item for the
    // slot. In rando, use this flag instead.
    COND_VB_SHOULD(VB_CHECK_FOR_ROOM_KEY, IS_RANDO, { *should = Flags_GetRandoInf(RANDO_INF_OBTAINED_ROOM_KEY); });

    // In the case of receiving a sword, we only want to equip it to the Human's B button. Vanilla avoids this issue by
    // never letting you be other forms when you get a sword from the smithy or curiosity shop.
    COND_VB_SHOULD(VB_ITEM_GIVE_SWORD_SET_FORM_EQUIP, IS_RANDO, {
        *should = false;
        // FD and human share equip slots, so do not change the equip slot if the player is FD.
        if (GET_PLAYER_FORM != PLAYER_FORM_FIERCE_DEITY) {
            u8* item = va_arg(args, u8*);
            BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = *item;
        }
    });

    // Fix vanilla bug where the player can often use magic before it's acquired.
    COND_VB_SHOULD(VB_GRANT_MAGIC_UPON_REQUEST, IS_RANDO, {
        if (!gSaveContext.save.saveInfo.playerData.isMagicAcquired) {
            *should = false;
            gSaveContext.isMagicRequested = false;
            gSaveContext.save.saveInfo.playerData.magic = 0;
            gSaveContext.magicToAdd = 0;
        }
    });

    COND_VB_SHOULD(VB_MEET_MOON_REQUIREMENTS, IS_RANDO, { *should = Rando::Logic::MeetsMoonRequirements(); });

    // Fix issue where bombchus/bombs can't be used in Honey and Darling if the player has no sword equipped and no bow.
    COND_VB_SHOULD(VB_CLEAR_B_BUTTON_FOR_NO_BOW, IS_RANDO, {
        // Playing Honey and Darling
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_08_01) && (gPlayState->sceneId == SCENE_BOWLING)) {
            *should = false;
        }
    });
}
