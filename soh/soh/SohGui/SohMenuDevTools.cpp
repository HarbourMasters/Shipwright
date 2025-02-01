#include "SohMenu.h"

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets2;

void SohMenu::AddMenuDevTools() {
    // Add Dev Tools Menu
    AddMenuEntry("Dev Tools", CVAR_SETTING("Menu.DevToolsSidebarSection"));

    // General
    AddSidebarEntry("Dev Tools", "General", 3);
    WidgetPath path = { "Dev Tools", "General", SECTION_COLUMN_1 };

    AddWidget(path, "Popout Menu", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.Menu.Popout")
        .Options(CheckboxOptions().Tooltip("Changes the menu display from overlay to windowed."));

    // Stats
    path.sidebarName = "Stats";
    AddSidebarEntry("Dev Tools", path.sidebarName, 1);
    AddWidget(path, "Popout Stats Window", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("Stats"))
        .WindowName("Stats")
        .Options(ButtonOptions().Tooltip("Enables the separate Stats Window.").Size(Sizes::Inline));

    // Console
    path.sidebarName = "Console";
    AddSidebarEntry("Dev Tools", path.sidebarName, 1);
    AddWidget(path, "Popout Console", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("Console"))
        .WindowName("Console")
        .Options(ButtonOptions().Tooltip("Enables the separate Console Window.").Size(Sizes::Inline));

    // Save Editor
    path.sidebarName = "Save Editor";
    AddSidebarEntry("Dev Tools", path.sidebarName, 1);
    AddWidget(path, "Popout Save Editor", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("SaveEditor"))
        .WindowName("Save Editor")
        .Options(ButtonOptions().Tooltip("Enables the separate Save Editor Window.").Size(Sizes::Inline));

    // Hook Debugger
    path.sidebarName = "Hook Debugger";
    AddSidebarEntry("Dev Tools", path.sidebarName, 1);
    AddWidget(path, "Popout Hook Debugger", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("HookDebugger"))
        .WindowName("Hook Debugger")
        .Options(ButtonOptions().Tooltip("Enables the separate Hook Debugger Window.").Size(Sizes::Inline));

    // Collision Viewer
    path.sidebarName = "Collision Viewer";
    AddSidebarEntry("Dev Tools", path.sidebarName, 2);
    AddWidget(path, "Popout Collision Viewer", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("CollisionViewer"))
        .WindowName("Collision Viewer")
        .Options(ButtonOptions().Tooltip("Enables the separate Collision Viewer Window.").Size(Sizes::Inline));

    // Actor Viewer
    path.sidebarName = "Actor Viewer";
    AddSidebarEntry("Dev Tools", path.sidebarName, 1);
    AddWidget(path, "Popout Actor Viewer", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("ActorViewer"))
        .WindowName("Actor Viewer")
        .Options(ButtonOptions().Tooltip("Enables the separate Actor Viewer Window.").Size(Sizes::Inline));

    // Display List Viewer
    path.sidebarName = "DList Viewer";
    AddSidebarEntry("Dev Tools", path.sidebarName, 1);
    AddWidget(path, "Popout Display List Viewer", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("DisplayListViewer"))
        .WindowName("Display List Viewer")
        .Options(ButtonOptions().Tooltip("Enables the separate Display List Viewer Window.").Size(Sizes::Inline));

    // Value Viewer
    path.sidebarName = "Value Viewer";
    AddSidebarEntry("Dev Tools", path.sidebarName, 1);
    AddWidget(path, "Popout Value Viewer", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("ValueViewer"))
        .WindowName("Value Viewer")
        .Options(ButtonOptions().Tooltip("Enables the separate Value Viewer Window.").Size(Sizes::Inline));

    // Message Viewer
    path.sidebarName = "Message Viewer";
    AddSidebarEntry("Dev Tools", path.sidebarName, 1);
    AddWidget(path, "Popout Message Viewer", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("MessageViewer"))
        .WindowName("Message Viewer")
        .Options(ButtonOptions().Tooltip("Enables the separate Message Viewer Window.").Size(Sizes::Inline));

    // Gfx Debugger
    path.sidebarName = "Gfx Debugger";
    AddSidebarEntry("Dev Tools", path.sidebarName, 1);
    AddWidget(path, "Popout Gfx Debugger", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("GfxDebugger"))
        .WindowName("Gfx Debugger")
        .Options(ButtonOptions().Tooltip("Enables the separate Gfx Debugger Window.").Size(Sizes::Inline));
    
}

} // namespace SohGui
