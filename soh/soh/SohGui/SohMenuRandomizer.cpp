#include "SohMenu.h"

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets2;

void SohMenu::AddMenuRandomizer() {
    // Add Randomizer Menu
    AddMenuEntry("Randomizer", CVAR_SETTING("Menu.RandomizerSidebarSection"));

    // Seed Settings
    WidgetPath path = { "Randomizer", "Seed Settings", SECTION_COLUMN_1 };
    AddSidebarEntry("Randomizer", path.sidebarName, 1);
    AddWidget(path, "Popout Randomizer Settings Window", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("RandomizerSettings"))
        .WindowName("Randomizer Settings")
        .Options(ButtonOptions().Tooltip("Enables the separate Randomizer Settings Window.").Size(Sizes::Inline));

    // Plandomizer
    path.sidebarName = "Plandomizer";
    AddSidebarEntry("Randomizer", path.sidebarName, 1);
    AddWidget(path, "Popout Plandomizer Window", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("PlandomizerEditor"))
        .WindowName("Plandomizer Editor")
        .Options(ButtonOptions().Tooltip("Enables the separate Randomizer Settings Window.").Size(Sizes::Inline));

    // Item Tracker
    path.sidebarName = "Item Tracker";
    AddSidebarEntry("Randomizer", path.sidebarName, 1);

    AddWidget(path, "Item Tracker", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Toggle Item Tracker", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("ItemTracker"))
        .WindowName("Item Tracker")
        .Options(ButtonOptions().Tooltip("Toggles the Item Tracker.").Size(Sizes::Inline));

    AddWidget(path, "Item Tracker Settings", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Popout Item Tracker Settings", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("ItemTrackerSettings"))
        .WindowName("Item Tracker Settings")
        .Options(ButtonOptions().Tooltip("Enables the separate Item Tracker Settings Window.").Size(Sizes::Inline));

    // Entrance Tracker
    path.sidebarName = "Entrance Tracker";
    AddSidebarEntry("Randomizer", path.sidebarName, 1);

    AddWidget(path, "Entrance Tracker", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Toggle Entrance Tracker", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("EntranceTracker"))
        .WindowName("Entrance Tracker")
        .Options(ButtonOptions().Tooltip("Toggles the Entrance Tracker.").Size(Sizes::Inline));

    AddWidget(path, "Entrance Tracker Settings", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Popout Entrance Tracker Settings", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("EntranceTrackerSettings"))
        .WindowName("Entrance Tracker Settings")
        .Options(ButtonOptions().Tooltip("Enables the separate Entrance Tracker Settings Window.").Size(Sizes::Inline));

    // Check Tracker
    path.sidebarName = "Check Tracker";
    AddSidebarEntry("Randomizer", path.sidebarName, 1);

    AddWidget(path, "Check Tracker", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Toggle Check Tracker", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("CheckTracker"))
        .WindowName("Check Tracker")
        .Options(ButtonOptions().Tooltip("Toggles the Check Tracker.").Size(Sizes::Inline));

    AddWidget(path, "Check Tracker Settings", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Popout Check Tracker Settings", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("CheckTrackerSettings"))
        .WindowName("Check Tracker Settings")
        .Options(ButtonOptions().Tooltip("Enables the separate Check Tracker Settings Window.").Size(Sizes::Inline));
}

} // namespace SohGui
