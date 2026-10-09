#include "ActorBehavior.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/GameInteractor/GameInteractor.h"

extern "C" {
#include "variables.h"
}

void ApplyRemainsHint(u16* textId, bool* loadFromMessageTable) {
    static int remainsHintIndex = 0;

    if (remainsHintIndex > 4) {
        remainsHintIndex = 0;
    }

    u8 icon = 0xFE;
    std::string msg;
    RandoItemId randoItemId = RI_NONE;

    if (remainsHintIndex == 0) {
        msg = "        Witcher Wanted:\n"
              "These monsters are tormenting the "
              "local townfolk, will pay good money "
              "for their remains.";
    } else {
        msg = "         %g{{boss}}%w:\n"
              "Last seen %y{{location}}%w.";

        switch (remainsHintIndex) {
            case 1:
                CustomMessage::Replace(&msg, "{{boss}}", " Odolwa");
                randoItemId = RI_REMAINS_ODOLWA;
                break;
            case 2:
                CustomMessage::Replace(&msg, "{{boss}}", "  Goht");
                randoItemId = RI_REMAINS_GOHT;
                break;
            case 3:
                CustomMessage::Replace(&msg, "{{boss}}", "  Gyorg");
                randoItemId = RI_REMAINS_GYORG;
                break;
            case 4:
                CustomMessage::Replace(&msg, "{{boss}}", "Twinmold");
                randoItemId = RI_REMAINS_TWINMOLD;
                break;
        }

        icon = Rando::StaticData::GetIconForZMessage(randoItemId);
        RandoCheckId randoCheckId = Rando::FindItemPlacement(randoItemId);
        CustomMessage::Replace(&msg, "{{location}}", Rando::StaticData::GetLocationNameForHint(randoCheckId, false));
    }

    CustomMessage::Entry entry = {
        .icon = icon,
        .nextMessageID = remainsHintIndex >= 4 ? (u16)0xFFFF : (u16)0x1C06,
        .msg = msg,
    };

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
    remainsHintIndex++;
}

void ApplyTransformationHints(u16* textId, bool* loadFromMessageTable) {
    static std::string placeholderMsg = "Last seen in";
    static int transformHintIndex = 0;

    if (transformHintIndex > 3) {
        transformHintIndex = 0;
    }

    u8 icon = 0xFE;
    std::string msg;
    RandoItemId randoItemId = RI_NONE;

    if (transformHintIndex == 0) {
        msg = "   %yDeparted Soul Alert%w!\n"
              "The souls of the departed lay restless, "
              "find and heal them!";
    } else {
        msg = "%g{{mask}}%w:\n"
              "%y{{locations}}%w.";

        switch (transformHintIndex) {
            case 1:
                CustomMessage::Replace(&msg, "{{mask}}", " Butler's Son");
                randoItemId = RI_MASK_DEKU;
                break;
            case 2:
                CustomMessage::Replace(&msg, "{{mask}}", "  Darmani");
                randoItemId = RI_MASK_GORON;
                break;
            case 3:
                CustomMessage::Replace(&msg, "{{mask}}", "  Mikau");
                randoItemId = RI_MASK_ZORA;
                break;
            default:
                break;
        }

        icon = Rando::StaticData::GetIconForZMessage(randoItemId);
        std::vector<RandoCheckId> itemPlacements = Rando::FindMultiItemPlacement(randoItemId);
        std::string locationStr = "";
        if (!itemPlacements.empty()) {
            for (int i = 0; i < itemPlacements.size(); i++) {
                if (RANDO_SAVE_CHECKS[itemPlacements[i]].obtained) {
                    itemPlacements[i] = RC_UNKNOWN;
                }
            }
            for (auto& location : itemPlacements) {
                if (location == RC_UNKNOWN) {
                    locationStr = "%gLink's pocket%w";
                    break;
                }

                if (locationStr != "") {
                    locationStr += " %w&%y\n";
                }

                locationStr += Rando::StaticData::GetLocationNameForHint(location, false);
            }
            CustomMessage::Replace(&msg, "{{locations}}", locationStr);
        } else {
            CustomMessage::Replace(&msg, "{{locations}}", "%gLink's pocket%w");
        }
    }

    CustomMessage::Entry entry = {
        .icon = icon,
        .nextMessageID = transformHintIndex >= 3 ? (u16)0xFFFF : (u16)0x1C18,
        .msg = msg,
    };

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
    transformHintIndex++;
}

void Rando::ActorBehavior::InitEnTalkBehavior() {
    // "Recruiting Soldiers..." Posters around Clock Town
    COND_ID_HOOK(OnOpenText, 0x1C06, IS_RANDO && RANDO_SAVE_OPTIONS[RO_HINTS_BOSS_REMAINS], ApplyRemainsHint);

    COND_ID_HOOK(OnOpenText, 0x1C18, IS_RANDO && RANDO_SAVE_OPTIONS[RO_HINTS_TRANSFORMATIONS],
                 ApplyTransformationHints);
}
