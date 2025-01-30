#include "SohMenu.h"

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets2;

void SohMenu::AddMenuEnhancements() {
    // Add Enhancements Menu
    AddMenuEntry("Enhancements", "gSettings.Menu.EnhancementsSidebarSection");


    // Enhancements
    WidgetPath path = { "Enhancements", "Enhancements", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", path.sidebarName, 3);

    AddWidget(path, "filler", WIDGET_TEXT);


    // Cheats
    path.sidebarName = "Cheats";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);

    AddWidget(path, "Cheats filler", WIDGET_TEXT);


    // Cosmetics Editor
    path.sidebarName = "Cosmetics Editor";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);

    AddWidget(path, "Cosmetics filler", WIDGET_TEXT);


    // Audio Editor
    path.sidebarName = "Audio Editor";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);

    AddWidget(path, "Audio filler", WIDGET_TEXT);


    // Gameplay Stats
    path.sidebarName = "Gameplay Stats";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);

    AddWidget(path, "Gameplay Stats filler", WIDGET_TEXT);


    // Time Splits
    path.sidebarName = "Time Splits";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);

    AddWidget(path, "Time Splits filler", WIDGET_TEXT);


    // Timers
    path.sidebarName = "Timers";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);

    AddWidget(path, "Timers filler", WIDGET_TEXT);
}

} // namespace SohGui
