#include "SohMenu.h"

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets2;

void SohMenu::AddMenuDevTools() {
    // Add Dev Tools Menu
    AddMenuEntry("Dev Tools", "gSettings.Menu.DevToolsSidebarSection");

    // Menu 1
    AddSidebarEntry("Dev Tools", "General", 3);
    WidgetPath path = { "Dev Tools", "General", SECTION_COLUMN_1 };

    AddWidget(path, "Popout Menu", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.Menu.Popout")
        .Options(CheckboxOptions().Tooltip("Changes the menu display from overlay to windowed."));

    // dev tools windows
    // path = { "Developer Tools", "Collision Viewer", SECTION_COLUMN_1 };
    // AddSidebarEntry("Developer Tools", "Collision Viewer", 1);
    // AddWidget(path, "Popout Collision Viewer", WIDGET_WINDOW_BUTTON)
    //     .CVar("gWindows.CollisionViewer")
    //     .Options(ButtonOptions().Tooltip("Makes collision visible on screen").Size(Sizes::Inline))
    //     .WindowName("Collision Viewer");

    // path = { "Developer Tools", "Stats", SECTION_COLUMN_1 };
    // AddSidebarEntry("Developer Tools", "Stats", 1);
    // AddWidget(path, "Popout Stats", WIDGET_WINDOW_BUTTON)
    //     .CVar("gOpenWindows.Stats")
    //     .Options(ButtonOptions().Tooltip(
    //         "Shows the stats window, with your FPS and frametimes, and the OS you're playing on"))
    //     .WindowName("Stats");

    // path = { "Developer Tools", "Console", SECTION_COLUMN_1 };
    // AddSidebarEntry("Developer Tools", "Console", 1);
    // AddWidget(path, "Popout Console", WIDGET_WINDOW_BUTTON)
    //     .CVar("gOpenWindows.Console")
    //     .Options(ButtonOptions().Tooltip(
    //         "Enables the console window, allowing you to input commands. Type help for some examples"))
    //     .WindowName("Console");

    // path = { "Developer Tools", "Gfx Debugger", SECTION_COLUMN_1 };
    // AddSidebarEntry("Developer Tools", "Gfx Debugger", 1);
    // AddWidget(path, "Popout Gfx Debugger", WIDGET_WINDOW_BUTTON)
    //     .CVar("gOpenWindows.GfxDebugger")
    //     .Options(ButtonOptions().Tooltip(
    //         "Enables the Gfx Debugger window, allowing you to input commands, type help for some examples"))
    //     .WindowName("GfxDebuggerWindow");

    // path = { "Developer Tools", "Save Editor", SECTION_COLUMN_1 };
    // AddSidebarEntry("Developer Tools", "Save Editor", 1);
    // AddWidget(path, "Popout Save Editor", WIDGET_WINDOW_BUTTON)
    //     .CVar("gWindows.SaveEditor")
    //     .Options(ButtonOptions().Tooltip("Enables the Save Editor window, allowing you to edit your save file"))
    //     .WindowName("Save Editor");

    // path = { "Developer Tools", "Actor Viewer", SECTION_COLUMN_1 };
    // AddSidebarEntry("Developer Tools", "Actor Viewer", 1);
    // AddWidget(path, "Popout Actor Viewer", WIDGET_WINDOW_BUTTON)
    //     .CVar("gWindows.ActorViewer")
    //     .Options(ButtonOptions().Tooltip("Enables the Actor Viewer window, allowing you to view actors in the
    //     world.")) .WindowName("Actor Viewer");

    // path = { "Developer Tools", "Event Log", SECTION_COLUMN_1 };
    // AddSidebarEntry("Developer Tools", "Event Log", 1);
    // AddWidget(path, "Popout Event Log", WIDGET_WINDOW_BUTTON)
    //     .CVar("gWindows.EventLog")
    //     .Options(ButtonOptions().Tooltip("Enables the event log window"))
    //     .WindowName("Event Log");
}

} // namespace SohGui
