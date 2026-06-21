#include "SohMenu.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/achievements.h"

extern "C" {
#include "variables.h"
}

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets;

void DrawAchievementsWidget(WidgetInfo& widget) {
    ImGui::SeparatorText("Obtained achievements");

    for (size_t i = 0; i < gAchievementCount; i++) {
        if (Achievements_IsUnlocked(gAchievements[i].id)) {
            ImGui::Text("%s", gAchievements[i].name);
        }
    }

    ImGui::SeparatorText("Locked achievements");

    for (size_t i = 0; i < gAchievementCount; i++) {
        if (!Achievements_IsUnlocked(gAchievements[i].id)) {
            ImGui::Text("%s", gAchievements[i].name);
        }
    }
}

void SohMenu::AddMenuAchievements() {
    // Add Achievements section
    AddMenuEntry("Achievements", CVAR_SETTING("Menu.AchievementsSidebarSection"));
    WidgetPath path = {"Achievements", "Progress", SECTION_COLUMN_1};
    AddSidebarEntry("Achievements", path.sidebarName, 2);

    AddWidget(path, "Achievements", WIDGET_CUSTOM).CustomFunction(DrawAchievementsWidget).HideInSearch(true);
} // namespace SohGui

}