#include "FileSelectEnhancements.h"

#include <ship/Context.h>

#include "soh/OTRGlobals.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/SohGui/SohModals.h"
#include "soh/SohGui/SohGui.hpp"
#include "soh/SohGui/UIWidgets.hpp"

#include <string>

// Quests are hidden when their O2R is missing or their File Select "Hide" option is on
bool SohFileSelect_IsQuestHidden(u8 quest) {
    switch (quest) {
        case QUEST_NORMAL:
            return !ResourceMgr_GameHasOriginal() || CVarGetInteger(CVAR_ENHANCEMENT("FileSelect.HideNormalQuest"), 0);
        case QUEST_MASTER:
            return !ResourceMgr_GameHasMasterQuest() ||
                   CVarGetInteger(CVAR_ENHANCEMENT("FileSelect.HideMasterQuest"), 0);
        case QUEST_RANDOMIZER:
            return CVarGetInteger(CVAR_ENHANCEMENT("FileSelect.HideRandomizerQuest"), 0);
        case QUEST_BOSSRUSH:
            return CVarGetInteger(CVAR_ENHANCEMENT("FileSelect.HideBossRushQuest"), 0);
        case QUEST_SPEEDRUN:
            return !ResourceMgr_GameHasOriginal() ||
                   CVarGetInteger(CVAR_ENHANCEMENT("FileSelect.HideSpeedrunQuest"), 0);
        case QUEST_SPEEDRUN_MASTER:
            return !ResourceMgr_GameHasMasterQuest() ||
                   CVarGetInteger(CVAR_ENHANCEMENT("FileSelect.HideSpeedrunMasterQuest"), 0);
        default:
            return false;
    }
}

u8 SohFileSelect_CountVisibleQuests() {
    u8 count = 0;
    for (u8 quest = QUEST_NORMAL; quest <= QUEST_SPEEDRUN_MASTER; ++quest) {
        if (!SohFileSelect_IsQuestHidden(quest)) {
            count++;
        }
    }

    return count;
}

void SohFileSelect_ShowPresetMenu() {
    SohGui::ShowEscMenu();
    CVarSetString(CVAR_SETTING("Menu.ActiveHeader"), "Settings");
    CVarSetString(CVAR_SETTING("Menu.SettingsSidebarSection"), "Presets");
    CVarSetInteger(CVAR_GENERAL("HasSeenPresetModal"), 1);
}

void SohFileSelect_DismissPresetModal() {
    CVarSetInteger(CVAR_GENERAL("HasSeenPresetModal"), 1);
}

void SohFileSelect_ShowPresetModal() {
    if (CVarGetInteger(CVAR_GENERAL("HasSeenPresetModal"), 0)) {
        return;
    }
    std::shared_ptr<SohModalWindow> modal = static_pointer_cast<SohModalWindow>(
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGuiWindow("Modal Window"));
    if (modal->IsPopupOpen("FileSelectPresets")) {
        modal->DismissPopup();
    } else {
        // The stable identifier remains the queue identity; display text is
        // resolved each frame so a UI language change also updates an open modal.
        modal->RegisterPopup("FileSelectPresets", "", "", "", SohFileSelect_ShowPresetMenu,
                             SohFileSelect_DismissPresetModal, "ui.file_select.presets");
    }
}
