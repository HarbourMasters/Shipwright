#include "ActorBehavior.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/ShipUtils.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Kgy/z_en_kgy.h"
}

void Rando::ActorBehavior::InitEnKgyBehavior() {
    COND_VB_SHOULD(VB_SMITHY_START_UPGRADING_SWORD, IS_RANDO, {
        EnKgy* refActor = va_arg(args, EnKgy*);
        RandoSaveCheck& randoGildedSwordSaveCheck = RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_GILDED_SWORD];
        RandoSaveCheck& randoRazorSwordSaveCheck = RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_RAZOR_SWORD];

        if (randoRazorSwordSaveCheck.cycleObtained) {
            randoGildedSwordSaveCheck.eligible = true;

            // Normally this bit is set to zero when you get your sword back. The DoNotResetRazorSword enhancement uses
            // this bit, so we need to clear it
            gSaveContext.save.saveInfo.permanentSceneFlags[SCENE_KAJIYA].unk_14 &= ~4;
        } else {
            randoRazorSwordSaveCheck.eligible = true;
            // Skip ahead to the textbox that normally plays after receiving the sword
            refActor->actor.textId = 0xC52;
        }

        *should = false;
    });

    // Allow player to get checks without a sword
    COND_VB_SHOULD(VB_SMITHY_CHECK_FOR_SWORD, IS_RANDO, { *should = false; });

    COND_VB_SHOULD(VB_SMITHY_CHECK_FOR_RAZOR_SWORD, IS_RANDO, {
        RandoSaveCheck& randoRazorSwordSaveCheck = RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_RAZOR_SWORD];
        *should = randoRazorSwordSaveCheck.cycleObtained;
    });

    COND_VB_SHOULD(VB_SMITHY_CHECK_FOR_GILDED_SWORD, IS_RANDO, {
        RandoSaveCheck& randoGildedSwordSaveCheck = RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_GILDED_SWORD];
        *should = randoGildedSwordSaveCheck.cycleObtained;
    });

    // "If you want your sword sharpened..." (Razor Sword upgrade)
    COND_ID_HOOK(OnOpenText, 0xc3b, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

        RandoSaveCheck& randoRazorSwordSaveCheck = RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_RAZOR_SWORD];
        if (!randoRazorSwordSaveCheck.cycleObtained) {
            entry.msg = "\nIf you want %y{itemName}%w, it will cost you %p100 Rupees%w.\n\x10";
            entry.msg += "So, do we have a deal?\n\xC2%gI'll buy it\nNo thanks\xBF";
            CustomMessage::Replace(&entry.msg, "{itemName}",
                                   Rando::StaticData::GetItemName(randoRazorSwordSaveCheck.randoItemId, true,
                                                                  RC_MOUNTAIN_VILLAGE_SMITHY_RAZOR_SWORD));
        }

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    // "With gold dust I can forge the strongest of swords"
    COND_ID_HOOK(OnOpenText, 0xc3d, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        std::string itemName = "40 Rupees";

        if (!RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_GILDED_SWORD].eligible) {
            itemName =
                Rando::StaticData::GetItemName(RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_GILDED_SWORD].randoItemId,
                                               true, RC_MOUNTAIN_VILLAGE_SMITHY_GILDED_SWORD);
        }
        entry.msg = "Want to know a secret? If you bring me some gold dust, I can offer you %r{itemName}%w.\xE0";
        CustomMessage::Replace(&entry.msg, "{itemName}", itemName);

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    // "Reforge your sword?"
    COND_ID_HOOK(OnOpenText, 0xc3e, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "Back for more?\n\xC2%gYes\nNo";

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    // "Come back tomorrow morning"
    COND_ID_HOOK(OnOpenText, 0xc42, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "Thanks for your business.\x19";

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    // "Your sword has already been reforged! Unless..."
    COND_ID_HOOK(OnOpenText, 0xc45, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "Did you bring the %rgold dust%w?\x19";

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    // "We can use it to reforge your sword"
    COND_ID_HOOK(OnOpenText, 0xc46, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "That's it, alright. I'll just take that off your hands and give you this. Don't tell anyone!\x19";

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    // "Gold dust is the prize for winning the Goron race in spring?"
    COND_ID_HOOK(OnOpenText, 0xc49, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "Huh? You say that gold dust can be found %r{location}%w?\x19";
        RandoCheckId randoCheckId = Rando::FindItemPlacement(RI_BOTTLE_GOLD_DUST);
        CustomMessage::Replace(&entry.msg, "{location}",
                               Rando::StaticData::GetLocationNameForHint(randoCheckId, false));

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    // "Gold dust happens to be first prize at the racetrack"
    COND_ID_HOOK(OnOpenText, 0xc4b, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "Gold dust can be found %p{location}%w.\x10";
        entry.msg += "Bring me that, and my %r{itemName}%w is all yours.\xE0";
        RandoCheckId randoCheckId = Rando::FindItemPlacement(RI_BOTTLE_GOLD_DUST);
        CustomMessage::Replace(
            &entry.msg, "{itemName}",
            Rando::StaticData::Items[RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_SMITHY_GILDED_SWORD].randoItemId].name);
        CustomMessage::Replace(&entry.msg, "{location}",
                               Rando::StaticData::GetLocationNameForHint(randoCheckId, false));

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    // "Your sword is already as strong as I can make it!"
    COND_ID_HOOK(OnOpenText, 0xc4c, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.msg = "Hey, what is this? I'm not made of Randomizer Checks!\x19";

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}
