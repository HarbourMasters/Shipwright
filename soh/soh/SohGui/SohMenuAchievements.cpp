#include "SohMenu.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include "variables.h"
}

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets;

void SohMenu::AddMenuAchievements() {
    // Add Achievements section
    AddMenuEntry("Achievements", CVAR_SETTING("Menu.AchievementsSidebarSection"));
    WidgetPath path = {"Achievements", "Test 1", SECTION_COLUMN_1};
    AddSidebarEntry("Achievements", path.sidebarName, 2);

    //Test 1
    AddWidget(path, "Obtained Achievements", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Test", WIDGET_TEXT);
    AddWidget(path, "Locked Achievements", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Test", WIDGET_TEXT);

    //Test 2
    path.sidebarName = "Test 2";
    AddSidebarEntry("Achievements", path.sidebarName, 2);
    AddWidget(path, "Test", WIDGET_TEXT);
} // namespace SohGui

}