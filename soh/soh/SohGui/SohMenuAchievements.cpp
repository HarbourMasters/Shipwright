#include "SohMenu.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/achievements.h"
#include <fast/Fast3dGui.h>

extern "C" {
#include "variables.h"
}

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets;

void DrawAchievementEntry(const AchievementInfo& achievement, bool unlocked) {
    auto gui = std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui());
    const char* icon = unlocked ? achievement.icon : achievement.grayedIcon;
    auto texture = gui->GetTextureByName(icon);

    ImGui::Image(texture, ImVec2(64, 64), ImVec2(0, 0), ImVec2(1, 1));
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", achievement.description);
    }
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16.0f);
    ImGui::Text("%s", achievement.name);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", achievement.description);
    }
}

void DrawAchievementsWidget(WidgetInfo& widget) {
    ImGui::SeparatorText("Obtained achievements");

    for (size_t i = 0; i < gAchievementCount; i++) {
        if (Achievements_IsUnlocked(gAchievements[i].id)) {
            DrawAchievementEntry(gAchievements[i], true);
        }
    }

    ImGui::SeparatorText("Locked achievements");

    for (size_t i = 0; i < gAchievementCount; i++) {
        if (!Achievements_IsUnlocked(gAchievements[i].id)) {
            DrawAchievementEntry(gAchievements[i], false);
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