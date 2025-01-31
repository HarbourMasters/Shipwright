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
    AddSidebarEntry("Enhancements", path.sidebarName, 1);
    AddWidget(path, "Cosmetics Editor", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Popout Cosmetics Editor Window", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.CosmeticsEditor")
        .WindowName("Cosmetics Editor")
        .Options(ButtonOptions().Tooltip("Enables the separate Cosmetics Editor Window.").Size(Sizes::Inline));


    // Audio Editor
    path.sidebarName = "Audio Editor";
    AddSidebarEntry("Enhancements", path.sidebarName, 1);
    AddWidget(path, "Audio Editor", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Popout Audio Editor Window", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.AudioEditor")
        .WindowName("Audio Editor")
        .Options(ButtonOptions().Tooltip("Enables the separate Audio Editor Window.").Size(Sizes::Inline));


    // Gameplay Stats
    path.sidebarName = "Gameplay Stats";
    AddSidebarEntry("Enhancements", path.sidebarName, 2);
    AddWidget(path, "Gameplay Stats", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Popout Gameplay Stats Window", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.GameplayStats")
        .WindowName("Gameplay Stats")
        .Options(ButtonOptions().Tooltip("Enables the separate Gameplay Stats Window.").Size(Sizes::Inline));


    // Time Splits
    path.sidebarName = "Time Splits";
    AddSidebarEntry("Enhancements", path.sidebarName, 1);
    AddWidget(path, "Time Splits", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Popout Time Splits Window", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.TimeSplits")
        .WindowName("Time Splits")
        .Options(ButtonOptions().Tooltip("Enables the separate Time Splits Window.").Size(Sizes::Inline));


    // Timers
    path.sidebarName = "Timers";
    AddSidebarEntry("Enhancements", path.sidebarName, 1);
    AddWidget(path, "Timers", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Popout Timers Window", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.AdditionalTimers")
        .WindowName("Additional Timers")
        .Options(ButtonOptions().Tooltip("Enables the separate Additional Timers Window.").Size(Sizes::Inline));
}

} // namespace SohGui
