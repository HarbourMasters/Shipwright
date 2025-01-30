#include "SohMenu.h"

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets2;

void SohMenu::AddMenuEnhancements() {
    // Add Enhancements Menu
    AddMenuEntry("Enhancements", "gSettings.Menu.EnhancementsSidebarSection");

    // Menu 1
    WidgetPath path = { "Enhancements", "Menu 1", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Menu 1", 3);

    AddWidget(path, "Menu 1 filler", WIDGET_TEXT);

    // Cheats
    path.sidebarName = "Cheats";
    AddSidebarEntry("Enhancements", "Cheats", 3);

    AddWidget(path, "Cheats filler", WIDGET_TEXT);

    // HUD Editor
    // path = { "Enhancements", "HUD Editor", SECTION_COLUMN_1 };
    // AddSidebarEntry("Enhancements", "HUD Editor", 1);
    // AddWidget(path, "Popout HUD Editor", WIDGET_WINDOW_BUTTON)
    //    .CVar("gWindows.HudEditor")
    //    .WindowName("HUD Editor")
    //    .Options(ButtonOptions()
    //                 .Tooltip("Enables the HUD Editor window, allowing you to modify your HUD")
    //                 .Size(Sizes::Inline));

    //// Item Tracker Settings
    // path = { "Enhancements", "Item Tracker", SECTION_COLUMN_1 };
    // AddSidebarEntry("Enhancements", "Item Tracker", 1);
    // AddWidget(path, "Popout Item Tracker", WIDGET_WINDOW_BUTTON)
    //     .CVar("gWindows.ItemTracker")
    //     .WindowName("Item Tracker Settings");
}

} // namespace SohGui
