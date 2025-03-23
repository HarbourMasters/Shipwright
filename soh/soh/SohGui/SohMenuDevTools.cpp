#include "SohMenu.h"

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets;

void SohMenu::AddMenuDevTools() {
    // Add Dev Tools Menu
    AddMenuEntry("Dev Tools", CVAR_SETTING("Menu.DevToolsSidebarSection"));

    // General
    AddSidebarEntry("Dev Tools", "General", 3);
    WidgetPath path = { "Dev Tools", "General", SECTION_COLUMN_1 };

    AddWidget(path, "Popout Menu", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.Menu.Popout")
        .Options(CheckboxOptions().Tooltip("Changes the menu display from overlay to windowed."));
    AddWidget(path, "Debug Mode", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_DEVELOPER_TOOLS("DebugEnabled"))
        .Options(CheckboxOptions().Tooltip("Enables Debug Mode, allowing you to select maps with L + R + Z, noclip "
                                           "with L + D-pad Right, and open the debug menu with L on the pause screen."));
    AddWidget(path, "OoT Registry Editor", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_DEVELOPER_TOOLS("RegEditEnabled"))
        .PreFunc([](WidgetInfo& info) { info.isHidden = !CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugEnabled"), 0); })
        .Options(CheckboxOptions().Tooltip("Enables the registry editor."));
    AddWidget(path, "Debug Save File Mode", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_DEVELOPER_TOOLS("DebugSaveFileMode"))
        .PreFunc([](WidgetInfo& info) { info.isHidden = !CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugEnabled"), 0); })
        .Options(ComboboxOptions()
                     .Tooltip("Changes the behaviour of debug file select creation (creating a save file on slot 1 "
                              "with debug mode on)\n"
                              "- Off: The debug save file will be a normal savefile\n"
                              "- Vanilla: The debug save file will be the debug save file from the original game\n"
                              "- Maxed: The debug save file will be a save file with all of the items & upgrades")
                     .ComboMap(debugSaveFileModes));
    AddWidget(path, "OoT Skulltula Debug", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_DEVELOPER_TOOLS("SkulltulaDebugEnabled"))
        .Options(CheckboxOptions().Tooltip("Enables Skulltula Debug, when moving the cursor in the menu above various "
                                           "map icons (boss key, compass, map screen locations, etc) will set the GS "
                                           "bits in that area.\nUSE WITH CAUTION AS IT DOES NOT UPDATE THE GS COUNT."));
    AddWidget(path, "Better Debug Warp Screen", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_DEVELOPER_TOOLS("BetterDebugWarpScreen"))
        .Options(CheckboxOptions().Tooltip(
            "Optimized debug warp screen, with the added ability to chose entrances and time of day").DefaultValue(true));
    AddWidget(path, "Debug Warp Screen Translation", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"))
        .Options(CheckboxOptions()
            .Tooltip("Translate the Debug Warp Screen based on the game language.")
            .DefaultValue(true));
    AddWidget(path, "Resource logging", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_DEVELOPER_TOOLS("ResourceLogging"))
        .Options(CheckboxOptions().Tooltip("Logs some resources as XML when they're loaded in binary format"));

    AddWidget(path, "Frame Advance", WIDGET_CHECKBOX)
        .Options(CheckboxOptions().Tooltip(
            "This allows you to advance through the game one frame at a time on command. "
            "To advance a frame, hold Z and tap R on the second controller. Holding Z "
            "and R will advance a frame every half second. You can also use the buttons below."))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = mSohMenu->disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active ||
                            mSohMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
            if (gPlayState != nullptr) {
                info.valuePointer = (bool*)&gPlayState->frameAdvCtx.enabled;
            } else {
                info.valuePointer = (bool*)nullptr;
            }
        });
    AddWidget(path, "Advance 1", WIDGET_BUTTON)
        .Options(ButtonOptions().Tooltip("Advance 1 frame.").Size(Sizes::Inline))
        .Callback([](WidgetInfo& info) { CVarSetInteger(CVAR_DEVELOPER_TOOLS("FrameAdvanceTick"), 1); })
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = mSohMenu->disabledMap.at(DISABLE_FOR_FRAME_ADVANCE_OFF).active ||
                            mSohMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
        });
    AddWidget(path, "Advance (Hold)", WIDGET_BUTTON)
        .Options(ButtonOptions().Tooltip("Advance frames while the button is held.").Size(Sizes::Inline))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = mSohMenu->disabledMap.at(DISABLE_FOR_FRAME_ADVANCE_OFF).active ||
                            mSohMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
        })
        .PostFunc([](WidgetInfo& info) {
            if (ImGui::IsItemActive()) {
                CVarSetInteger(CVAR_DEVELOPER_TOOLS("FrameAdvanceTick"), 1);
            }
        })
        .SameLine(true);

    // Stats
    path.sidebarName = "Stats";
    AddSidebarEntry("Dev Tools", path.sidebarName, 1);
    AddWidget(path, "Popout Stats Window", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("Stats"))
        .WindowName("Stats")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Stats Window."));

    // Console
    path.sidebarName = "Console";
    AddSidebarEntry("Dev Tools", path.sidebarName, 1);
    AddWidget(path, "Popout Console", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("Console"))
        .WindowName("Console##SoH")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Console Window."));

    // Save Editor
    path.sidebarName = "Save Editor";
    AddSidebarEntry("Dev Tools", path.sidebarName, 1);
    AddWidget(path, "Popout Save Editor", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("SaveEditor"))
        .WindowName("Save Editor")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Save Editor Window."));

    // Hook Debugger
    path.sidebarName = "Hook Debugger";
    AddSidebarEntry("Dev Tools", path.sidebarName, 1);
    AddWidget(path, "Popout Hook Debugger", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("HookDebugger"))
        .WindowName("Hook Debugger")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Hook Debugger Window."));

    // Collision Viewer
    path.sidebarName = "Collision Viewer";
    AddSidebarEntry("Dev Tools", path.sidebarName, 2);
    AddWidget(path, "Popout Collision Viewer", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("CollisionViewer"))
        .WindowName("Collision Viewer")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Collision Viewer Window."));

    // Actor Viewer
    path.sidebarName = "Actor Viewer";
    AddSidebarEntry("Dev Tools", path.sidebarName, 2);
    AddWidget(path, "Popout Actor Viewer", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("ActorViewer"))
        .WindowName("Actor Viewer")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Actor Viewer Window."));

    // Display List Viewer
    path.sidebarName = "DList Viewer";
    AddSidebarEntry("Dev Tools", path.sidebarName, 2);
    AddWidget(path, "Popout Display List Viewer", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("DisplayListViewer"))
        .WindowName("Display List Viewer")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Display List Viewer Window."));

    // Value Viewer
    path.sidebarName = "Value Viewer";
    AddSidebarEntry("Dev Tools", path.sidebarName, 2);
    AddWidget(path, "Popout Value Viewer", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("ValueViewer"))
        .WindowName("Value Viewer")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Value Viewer Window."));

    // Message Viewer
    path.sidebarName = "Message Viewer";
    AddSidebarEntry("Dev Tools", path.sidebarName, 2);
    AddWidget(path, "Popout Message Viewer", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("MessageViewer"))
        .WindowName("Message Viewer")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Message Viewer Window."));

    // Gfx Debugger
    path.sidebarName = "Gfx Debugger";
    AddSidebarEntry("Dev Tools", path.sidebarName, 1);
    AddWidget(path, "Popout Gfx Debugger", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("GfxDebugger"))
        .WindowName("GfxDebugger##SoH")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Gfx Debugger Window."));
}

} // namespace SohGui
