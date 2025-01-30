#include "SohMenu.h"
#include "UIWidgets2.hpp"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/controls/SohInputEditorWindow.h"
#include "window/gui/GuiMenuBar.h"
#include "window/gui/GuiElement.h"
#include "soh/Notification/Notification.h"
#include <variant>
#include "StringHelper.h"
#include <spdlog/fmt/fmt.h>
#include "variables.h"
#include <variant>
#include <tuple>
//#include "ResolutionEditor.h"

extern "C" {
#include "z64.h"
#include "functions.h"
extern PlayState* gPlayState;
}
extern std::unordered_map<s16, const char*> warpPointSceneList;
extern void Warp();

namespace SohGui {
extern std::shared_ptr<SohMenu> mSohMenu;
void FreeLookPitchMinMax() {
    f32 maxY = CVarGetFloat("gEnhancements.Camera.FreeLook.MaxPitch", 72.0f);
    f32 minY = CVarGetFloat("gEnhancements.Camera.FreeLook.MinPitch", -49.0f);
    CVarSetFloat("gEnhancements.Camera.FreeLook.MaxPitch", std::max(maxY, minY));
    CVarSetFloat("gEnhancements.Camera.FreeLook.MinPitch", std::min(maxY, minY));
}

using namespace UIWidgets2;

void SohMenu::AddSidebarEntry(std::string sectionName, std::string sidebarName, uint32_t columnCount) {
    assert(!sectionName.empty());
    assert(!sidebarName.empty());
    menuEntries.at(sectionName).sidebars.emplace(sidebarName, SidebarEntry{ .columnCount = columnCount });
    menuEntries.at(sectionName).sidebarOrder.push_back(sidebarName);
}

WidgetInfo& SohMenu::AddWidget(WidgetPath& pathInfo, std::string widgetName, WidgetType widgetType) {
    assert(!widgetName.empty());                        // Must be unique
    assert(menuEntries.contains(pathInfo.sectionName)); // Section/header must already exist
    assert(menuEntries.at(pathInfo.sectionName).sidebars.contains(pathInfo.sidebarName)); // Sidebar must already exist
    std::unordered_map<std::string, SidebarEntry>& sidebar = menuEntries.at(pathInfo.sectionName).sidebars;
    uint8_t column = pathInfo.column;
    if (sidebar.contains(pathInfo.sidebarName)) {
        while (sidebar.at(pathInfo.sidebarName).columnWidgets.size() < column + 1) {
            sidebar.at(pathInfo.sidebarName).columnWidgets.push_back({});
        }
    }
    SidebarEntry& entry = sidebar.at(pathInfo.sidebarName);
    entry.columnWidgets.at(column).push_back({ .name = widgetName, .type = widgetType });
    WidgetInfo& widget = entry.columnWidgets.at(column).back();
    switch (widgetType) {
        case WIDGET_CHECKBOX:
        case WIDGET_CVAR_CHECKBOX:
            widget.options = std::make_shared<CheckboxOptions>();
            break;
        case WIDGET_SLIDER_FLOAT:
        case WIDGET_CVAR_SLIDER_FLOAT:
            widget.options = std::make_shared<FloatSliderOptions>();
            break;
        case WIDGET_SLIDER_INT:
        case WIDGET_CVAR_SLIDER_INT:
            widget.options = std::make_shared<IntSliderOptions>();
            break;
        case WIDGET_COMBOBOX:
        case WIDGET_CVAR_COMBOBOX:
        case WIDGET_AUDIO_BACKEND:
        case WIDGET_VIDEO_BACKEND:
            widget.options = std::make_shared<ComboboxOptions>();
            break;
        case WIDGET_BUTTON:
            widget.options = std::make_shared<ButtonOptions>();
            break;
        case WIDGET_WINDOW_BUTTON:
            widget.options = std::make_shared<ButtonOptions>(ButtonOptions{ .size = Sizes::Inline });
            break;
        case WIDGET_COLOR_24:
        case WIDGET_COLOR_32:
            break;
        case WIDGET_SEARCH:
        case WIDGET_SEPARATOR:
        case WIDGET_SEPARATOR_TEXT:
        case WIDGET_TEXT:
        default:
            widget.options = std::make_shared<WidgetOptions>();
    }
    return widget;
}

void SohMenu::AddSettings() {
    // Add Settings menu
    AddMenuEntry("Settings", "gSettings.Menu.SettingsSidebarSection");
    // General Settings
    AddSidebarEntry("Settings", "General", 3);
    WidgetPath path = { "Settings", "General", SECTION_COLUMN_1 };
    AddWidget(path, "Menu Theme", WIDGET_CVAR_COMBOBOX)
        .CVar("gSettings.Menu.Theme")
        .Options(ComboboxOptions()
                     .Tooltip("Changes the Theme of the Menu Widgets.")
                     .ComboMap(menuThemeOptions)
                     .DefaultIndex(Colors::LightBlue));
#if not defined(__SWITCH__) and not defined(__WIIU__)
    AddWidget(path, "Menu Controller Navigation", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_IMGUI_CONTROLLER_NAV)
        .Options(CheckboxOptions().Tooltip(
            "Allows controller navigation of the 2Ship menu (Settings, Enhancements,...)\nCAUTION: "
            "This will disable game inputs while the menu is visible.\n\nD-pad to move between "
            "items, A to select, B to move up in scope."));
    AddWidget(path, "Cursor Always Visible", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.CursorVisibility")
        .Callback([](WidgetInfo& info) {
            Ship::Context::GetInstance()->GetWindow()->SetForceCursorVisibility(
                CVarGetInteger("gSettings.CursorVisibility", 0));
        })
        .Options(CheckboxOptions().Tooltip("Makes the cursor always visible, even in full screen."));
#endif
    AddWidget(path, "Search In Sidebar", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.Menu.SidebarSearch")
        .Callback([](WidgetInfo& info) {
            if (CVarGetInteger("gSettings.Menu.SidebarSearch", 0)) {
                mSohMenu->InsertSidebarSearch();
            } else {
                mSohMenu->RemoveSidebarSearch();
            }
        })
        .Options(CheckboxOptions().Tooltip(
            "Displays the Search menu as a sidebar entry in Settings instead of in the header."));
    AddWidget(path, "Search Input Autofocus", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.Menu.SearchAutofocus")
        .Options(CheckboxOptions().Tooltip(
            "Search input box gets autofocus when visible. Does not affect using other widgets."));
    AddWidget(path, "Alt Assets Tab hotkey", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Mods.AlternateAssetsHotkey")
        .Options(
            CheckboxOptions().Tooltip("Allows pressing the Tab key to toggle alternate assets").DefaultValue(true));
    AddWidget(path, "Open App Files Folder", WIDGET_BUTTON)
        .Callback([](WidgetInfo& info) {
            std::string filesPath = Ship::Context::GetInstance()->GetAppDirectoryPath();
            SDL_OpenURL(std::string("file:///" + std::filesystem::absolute(filesPath).string()).c_str());
        })
        .Options(ButtonOptions().Tooltip("Opens the folder that contains the save and mods folders, etc."));

    // Audio Settings
    path.sidebarName = "Audio";
    AddSidebarEntry("Settings", "Audio", 3);
    AddWidget(path, "Master Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.MasterVolume")
        .Options(FloatSliderOptions()
                     .Tooltip("Adjust the overall sound volume.")
                     .ShowButtons(false)
                     .Format("")
                     .IsPercentage());
    /*AddWidget(path, "Main Music Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.MainMusicVolume")
        .Callback([](WidgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_BGM_MAIN, CVarGetFloat("gSettings.Audio.MainMusicVolume", 1.0f));
        })
        .Options(FloatSliderOptions()
                     .Tooltip("Adjust the background music volume.")
                     .ShowButtons(false)
                     .Format("")
                     .IsPercentage());
    AddWidget(path, "Sub Music Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.SubMusicVolume")
        .Callback([](WidgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_BGM_SUB, CVarGetFloat("gSettings.Audio.SubMusicVolume", 1.0f));
        })
        .Options(
            FloatSliderOptions().Tooltip("Adjust the sub music volume.").ShowButtons(false).Format("").IsPercentage());
    AddWidget(path, "Sound Effects Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.SoundEffectsVolume")
        .Callback([](WidgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_SFX, CVarGetFloat("gSettings.Audio.SoundEffectsVolume", 1.0f));
        })
        .Options(FloatSliderOptions()
                     .Tooltip("Adjust the sound effects volume.")
                     .ShowButtons(false)
                     .Format("")
                     .IsPercentage());
    AddWidget(path, "Fanfare Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.FanfareVolume")
        .Callback([](WidgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_FANFARE, CVarGetFloat("gSettings.Audio.FanfareVolume", 1.0f));
        })
        .Options(
            FloatSliderOptions().Tooltip("Adjust the fanfare volume.").ShowButtons(false).Format("").IsPercentage());
    AddWidget(path, "Ambience Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.AmbienceVolume")
        .Callback([](WidgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_AMBIENCE, CVarGetFloat("gSettings.Audio.AmbienceVolume", 1.0f));
        })
        .Options(FloatSliderOptions()
                     .Tooltip("Adjust the ambient sound volume.")
                     .ShowButtons(false)
                     .Format("")
                     .IsPercentage());*/
    AddWidget(path, "Audio API", WIDGET_AUDIO_BACKEND);

    // Graphics Settings
    static int32_t maxFps;
    const char* tooltip = "";
    if (Ship::Context::GetInstance()->GetWindow()->GetWindowBackend() == Ship::WindowBackend::FAST3D_DXGI_DX11) {
        maxFps = 360;
        tooltip = "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics. This is "
                  "purely visual and does not impact game logic, execution of glitches etc.\n\nA higher target "
                  "FPS than your monitor's refresh rate will waste resources, and might give a worse result.";
    } else {
        maxFps = Ship::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
        tooltip = "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics. This is "
                  "purely visual and does not impact game logic, execution of glitches etc.";
    }
    path.sidebarName = "Graphics";
    AddSidebarEntry("Settings", "Graphics", 3);
    AddWidget(path, "Toggle Fullscreen", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.Fullscreen")
        .Callback([](WidgetInfo& info) { Ship::Context::GetInstance()->GetWindow()->ToggleFullscreen(); })
        .Options(CheckboxOptions().Tooltip("Toggles Fullscreen On/Off."));
#ifndef __APPLE__
    AddWidget(path, "Internal Resolution: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_INTERNAL_RESOLUTION)
        .Callback([](WidgetInfo& info) {
            Ship::Context::GetInstance()->GetWindow()->SetResolutionMultiplier(
                CVarGetFloat(CVAR_INTERNAL_RESOLUTION, 1));
        })
        .PreFunc([](WidgetInfo& info) {
            if (mSohMenu->disabledMap.at(DISABLE_FOR_ADVANCED_RESOLUTION_ON).active &&
                mSohMenu->disabledMap.at(DISABLE_FOR_VERTICAL_RES_TOGGLE_ON).active) {
                info.activeDisables.push_back(DISABLE_FOR_ADVANCED_RESOLUTION_ON);
                info.activeDisables.push_back(DISABLE_FOR_VERTICAL_RES_TOGGLE_ON);
            } else if (mSohMenu->disabledMap.at(DISABLE_FOR_LOW_RES_MODE_ON).active) {
                info.activeDisables.push_back(DISABLE_FOR_LOW_RES_MODE_ON);
            }
        })
        .Options(
            FloatSliderOptions()
                .Tooltip("Multiplies your output resolution by the value inputted, as a more intensive but effective "
                         "form of anti-aliasing.")
                .ShowButtons(false)
                .IsPercentage()
                .Format("")
                .Min(0.5f)
                .Max(2.0f));
#endif
#ifndef __WIIU__
    AddWidget(path, "Anti-aliasing (MSAA): %d", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_MSAA_VALUE)
        .Callback([](WidgetInfo& info) {
            Ship::Context::GetInstance()->GetWindow()->SetMsaaLevel(CVarGetInteger(CVAR_MSAA_VALUE, 1));
        })
        .Options(
            IntSliderOptions()
                .Tooltip("Activates MSAA (multi-sample anti-aliasing) from 2x up to 8x, to smooth the edges of "
                         "rendered geometry.\n"
                         "Higher sample count will result in smoother edges on models, but may reduce performance.")
                .Min(1)
                .Max(8)
                .DefaultValue(1));
#endif

    AddWidget(path, "Current FPS: %d", WIDGET_CVAR_SLIDER_INT)
        .CVar("gInterpolationFPS")
        .Callback([](WidgetInfo& info) {
            int32_t defaultValue = std::static_pointer_cast<IntSliderOptions>(info.options)->defaultValue;
            if (CVarGetInteger(info.cVar, defaultValue) == defaultValue) {
                info.name = "Current FPS: Original (%d)";
            } else {
                info.name = "Current FPS: %d";
            }
        })
        .PreFunc([](WidgetInfo& info) {
            if (mSohMenu->disabledMap.at(DISABLE_FOR_MATCH_REFRESH_RATE_ON).active)
                info.activeDisables.push_back(DISABLE_FOR_MATCH_REFRESH_RATE_ON);
        })
        .Options(IntSliderOptions().Tooltip(tooltip).Min(20).Max(maxFps).DefaultValue(20));
    AddWidget(path, "Match Refresh Rate", WIDGET_BUTTON)
        .Callback([](WidgetInfo& info) {
            int hz = Ship::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
            if (hz >= 20 && hz <= 360) {
                CVarSetInteger("gInterpolationFPS", hz);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            }
        })
        .PreFunc([](WidgetInfo& info) { info.isHidden = mSohMenu->disabledMap.at(DISABLE_FOR_NOT_DIRECTX).active; })
        .Options(ButtonOptions().Tooltip("Matches interpolation value to the current game's window refresh rate."));
    AddWidget(path, "Match Refresh Rate", WIDGET_CVAR_CHECKBOX)
        .CVar("gMatchRefreshRate")
        .PreFunc([](WidgetInfo& info) { info.isHidden = mSohMenu->disabledMap.at(DISABLE_FOR_DIRECTX).active; })
        .Options(CheckboxOptions().Tooltip("Matches interpolation value to the current game's window refresh rate."));
    AddWidget(path, "Jitter fix : >= % d FPS", WIDGET_CVAR_SLIDER_INT)
        .CVar("gExtraLatencyThreshold")
        .PreFunc([](WidgetInfo& info) { info.isHidden = mSohMenu->disabledMap.at(DISABLE_FOR_NOT_DIRECTX).active; })
        .Options(IntSliderOptions()
                     .Tooltip("When Interpolation FPS setting is at least this threshold, add one frame of input "
                              "lag (e.g. 16.6 ms for 60 FPS) in order to avoid jitter. This setting allows the "
                              "CPU to work on one frame while GPU works on the previous frame.\nThis setting "
                              "should be used when your computer is too slow to do CPU + GPU work in time.")
                     .Min(0)
                     .Max(360)
                     .DefaultValue(80));
    AddWidget(path, "Renderer API (Needs reload)", WIDGET_VIDEO_BACKEND);
    AddWidget(path, "Enable Vsync", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_VSYNC_ENABLED)
        .PreFunc([](WidgetInfo& info) { info.isHidden = mSohMenu->disabledMap.at(DISABLE_FOR_NO_VSYNC).active; })
        .Options(CheckboxOptions().Tooltip("Enables Vsync."));
    AddWidget(path, "Windowed Fullscreen", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_SDL_WINDOWED_FULLSCREEN)
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = mSohMenu->disabledMap.at(DISABLE_FOR_NO_WINDOWED_FULLSCREEN).active;
        })
        .Options(CheckboxOptions().Tooltip("Enables Windowed Fullscreen Mode."));
    AddWidget(path, "Allow multi-windows", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENABLE_MULTI_VIEWPORTS)
        .PreFunc(
            [](WidgetInfo& info) { info.isHidden = mSohMenu->disabledMap.at(DISABLE_FOR_NO_MULTI_VIEWPORT).active; })
        .Options(CheckboxOptions().Tooltip(
            "Allows multiple windows to be opened at once. Requires a reload to take effect."));
    AddWidget(path, "Texture Filter (Needs reload)", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_TEXTURE_FILTER)
        .Options(ComboboxOptions().Tooltip("Sets the applied Texture Filtering.").ComboMap(textureFilteringMap));

    path.sidebarName = "Controls";
    AddSidebarEntry("Settings", "Controls", 1);
    AddWidget(path,
              "This interface can be a little daunting. Please bear with us as we work to improve the experience "
              "and address some known issues.\n"
              "\n"
              "At first glance, you may notice several input devices displayed below the 'Clear All' button. "
              "Some of these might be other controllers connected to your computer, while others may be "
              "duplicated controllers (a known issue). We recommend clicking on the box with the " ICON_FA_EYE
              " icon and the name of any disconnected or unused controllers to hide their inputs. Make sure the "
              "target controller remains visible.\n"
              "\n"
              "If you encounter issues connecting your controller or registering inputs, try closing Steam or "
              "any other external input software. Alternatively, test a different controller to determine if "
              "it's a compatibility issue.\n",
              WIDGET_TEXT);
    AddWidget(path, "Bindings", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Popout Bindings Window", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.BenInputEditor")
        .WindowName("2S2H Input Editor")
        .Options(ButtonOptions().Tooltip("Enables the separate Bindings Window.").Size(Sizes::Inline));

    path.sidebarName = "Notifications";
    AddSidebarEntry("Settings", "Notifications", 1);
    AddWidget(path, "Position", WIDGET_CVAR_COMBOBOX)
        .CVar("gNotifications.Position")
        .Options(ComboboxOptions()
                     .Tooltip("Which corner of the screen notifications appear in.")
                     .ComboMap(notificationPosition)
                     .DefaultIndex(3));
    AddWidget(path, "Duration: %.0f seconds", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gNotifications.Duration")
        .Options(FloatSliderOptions()
                     .Tooltip("How long notifications are displayed for.")
                     .Format("%.1f")
                     .Step(0.1f)
                     .Min(3.0f)
                     .Max(30.0f)
                     .DefaultValue(10.0f));
    AddWidget(path, "Background Opacity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gNotifications.BgOpacity")
        .Options(FloatSliderOptions()
                     .Tooltip("How opaque the background of notifications is.")
                     .DefaultValue(0.5f)
                     .IsPercentage());
    AddWidget(path, "Size %.1f", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gNotifications.Size")
        .Options(FloatSliderOptions()
                     .Tooltip("How large notifications are.")
                     .Format("%.1f")
                     .Step(0.1f)
                     .Min(1.0f)
                     .Max(5.0f)
                     .DefaultValue(1.8f));
    AddWidget(path, "Test Notification", WIDGET_BUTTON)
        .Callback([](WidgetInfo& info) {
            Notification::Emit({
                .itemIcon = "__OTR__icon_item_24_static_yar/gQuestIconGoldSkulltulaTex",
                .prefix = "This",
                .message = "is a",
                .suffix = "test.",
            });
        })
        .Options(ButtonOptions().Tooltip("Displays a test notification."));
}

void SohMenu::AddEnhancements() {
    AddMenuEntry("Enhancements", "gSettings.Menu.EnhancementsSidebarSection");
    WidgetPath path = { "Enhancements", "Camera", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Camera", 3);
    AddWidget(path, "Filler1", WIDGET_TEXT);
    
    // HUD Editor
    //path = { "Enhancements", "HUD Editor", SECTION_COLUMN_1 };
    //AddSidebarEntry("Enhancements", "HUD Editor", 1);
    //AddWidget(path, "Popout HUD Editor", WIDGET_WINDOW_BUTTON)
    //    .CVar("gWindows.HudEditor")
    //    .WindowName("HUD Editor")
    //    .Options(ButtonOptions()
    //                 .Tooltip("Enables the HUD Editor window, allowing you to modify your HUD")
    //                 .Size(Sizes::Inline));

    //// Item Tracker Settings
    //path = { "Enhancements", "Item Tracker", SECTION_COLUMN_1 };
    //AddSidebarEntry("Enhancements", "Item Tracker", 1);
    //AddWidget(path, "Popout Item Tracker", WIDGET_WINDOW_BUTTON)
    //    .CVar("gWindows.ItemTracker")
    //    .WindowName("Item Tracker Settings");
}

void SohMenu::AddDevTools() {
    AddMenuEntry("Developer Tools", "gSettings.Menu.DevToolsSidebarSection");
    AddSidebarEntry("Developer Tools", "General", 3);
    WidgetPath path = { "Developer Tools", "General", SECTION_COLUMN_1 };
    AddWidget(path, "Filler1", WIDGET_TEXT);
    //AddWidget(path, "Popout Menu", WIDGET_CVAR_CHECKBOX)
    //    .CVar("gSettings.Menu.Popout")
    //    .Options(CheckboxOptions().Tooltip("Changes the menu display from overlay to windowed."));
    //
    //// dev tools windows
    //path = { "Developer Tools", "Collision Viewer", SECTION_COLUMN_1 };
    //AddSidebarEntry("Developer Tools", "Collision Viewer", 1);
    //AddWidget(path, "Popout Collision Viewer", WIDGET_WINDOW_BUTTON)
    //    .CVar("gWindows.CollisionViewer")
    //    .Options(ButtonOptions().Tooltip("Makes collision visible on screen").Size(Sizes::Inline))
    //    .WindowName("Collision Viewer");

    //path = { "Developer Tools", "Stats", SECTION_COLUMN_1 };
    //AddSidebarEntry("Developer Tools", "Stats", 1);
    //AddWidget(path, "Popout Stats", WIDGET_WINDOW_BUTTON)
    //    .CVar("gOpenWindows.Stats")
    //    .Options(ButtonOptions().Tooltip(
    //        "Shows the stats window, with your FPS and frametimes, and the OS you're playing on"))
    //    .WindowName("Stats");

    //path = { "Developer Tools", "Console", SECTION_COLUMN_1 };
    //AddSidebarEntry("Developer Tools", "Console", 1);
    //AddWidget(path, "Popout Console", WIDGET_WINDOW_BUTTON)
    //    .CVar("gOpenWindows.Console")
    //    .Options(ButtonOptions().Tooltip(
    //        "Enables the console window, allowing you to input commands. Type help for some examples"))
    //    .WindowName("Console");

    //path = { "Developer Tools", "Gfx Debugger", SECTION_COLUMN_1 };
    //AddSidebarEntry("Developer Tools", "Gfx Debugger", 1);
    //AddWidget(path, "Popout Gfx Debugger", WIDGET_WINDOW_BUTTON)
    //    .CVar("gOpenWindows.GfxDebugger")
    //    .Options(ButtonOptions().Tooltip(
    //        "Enables the Gfx Debugger window, allowing you to input commands, type help for some examples"))
    //    .WindowName("GfxDebuggerWindow");

    //path = { "Developer Tools", "Save Editor", SECTION_COLUMN_1 };
    //AddSidebarEntry("Developer Tools", "Save Editor", 1);
    //AddWidget(path, "Popout Save Editor", WIDGET_WINDOW_BUTTON)
    //    .CVar("gWindows.SaveEditor")
    //    .Options(ButtonOptions().Tooltip("Enables the Save Editor window, allowing you to edit your save file"))
    //    .WindowName("Save Editor");

    //path = { "Developer Tools", "Actor Viewer", SECTION_COLUMN_1 };
    //AddSidebarEntry("Developer Tools", "Actor Viewer", 1);
    //AddWidget(path, "Popout Actor Viewer", WIDGET_WINDOW_BUTTON)
    //    .CVar("gWindows.ActorViewer")
    //    .Options(ButtonOptions().Tooltip("Enables the Actor Viewer window, allowing you to view actors in the world."))
    //    .WindowName("Actor Viewer");

    //path = { "Developer Tools", "Event Log", SECTION_COLUMN_1 };
    //AddSidebarEntry("Developer Tools", "Event Log", 1);
    //AddWidget(path, "Popout Event Log", WIDGET_WINDOW_BUTTON)
    //    .CVar("gWindows.EventLog")
    //    .Options(ButtonOptions().Tooltip("Enables the event log window"))
    //    .WindowName("Event Log");
}

SohMenu::SohMenu(const std::string& consoleVariable, const std::string& name)
    : Menu(consoleVariable, name, 0, UIWidgets2::Colors::LightBlue) {
}

void SohMenu::InitElement() {
    Ship::Menu::InitElement();
    AddSettings();
    AddEnhancements();
    AddDevTools();

    if (CVarGetInteger("gSettings.Menu.SidebarSearch", 0)) {
        InsertSidebarSearch();
    }

    for (auto& initFunc : MenuInit::GetInitFuncs()) {
        initFunc();
    }

    disabledMap = {
        { DISABLE_FOR_GYRO_OFF,
          { [](disabledInfo& info) -> bool {
               return !CVarGetInteger("gEnhancements.Camera.FirstPerson.GyroEnabled", 0);
           },
            "Gyro Aiming is Disabled" } },
        { DISABLE_FOR_GYRO_ON,
          { [](disabledInfo& info) -> bool {
               return CVarGetInteger("gEnhancements.Camera.FirstPerson.GyroEnabled", 0);
           },
            "Gyro Aiming is Enabled" } },
        { DISABLE_FOR_RIGHT_STICK_OFF,
          { [](disabledInfo& info) -> bool {
               return !CVarGetInteger("gEnhancements.Camera.FirstPerson.RightStickEnabled", 0);
           },
            "Right Stick Aiming is Disabled" } },
        { DISABLE_FOR_AUTO_SAVE_OFF,
          { [](disabledInfo& info) -> bool { return !CVarGetInteger("gEnhancements.Saving.Autosave", 0); },
            "AutoSave is Disabled" } },
        { DISABLE_FOR_NULL_PLAY_STATE,
          { [](disabledInfo& info) -> bool { return gPlayState == NULL; }, "Save Not Loaded" } },
        { DISABLE_FOR_DEBUG_MODE_OFF,
          { [](disabledInfo& info) -> bool { return !CVarGetInteger("gDeveloperTools.DebugEnabled", 0); },
            "Debug Mode is Disabled" } },
        { DISABLE_FOR_NO_VSYNC,
          { [](disabledInfo& info) -> bool {
               return !Ship::Context::GetInstance()->GetWindow()->CanDisableVerticalSync();
           },
            "Disabling VSync not supported" } },
        { DISABLE_FOR_NO_WINDOWED_FULLSCREEN,
          { [](disabledInfo& info) -> bool {
               return !Ship::Context::GetInstance()->GetWindow()->SupportsWindowedFullscreen();
           },
            "Windowed Fullscreen not supported" } },
        { DISABLE_FOR_NO_MULTI_VIEWPORT,
          { [](disabledInfo& info) -> bool {
               return !Ship::Context::GetInstance()->GetWindow()->GetGui()->SupportsViewports();
           },
            "Multi-viewports not supported" } },
        { DISABLE_FOR_NOT_DIRECTX,
          { [](disabledInfo& info) -> bool {
               return Ship::Context::GetInstance()->GetWindow()->GetWindowBackend() !=
                      Ship::WindowBackend::FAST3D_DXGI_DX11;
           },
            "Available Only on DirectX" } },
        { DISABLE_FOR_DIRECTX,
          { [](disabledInfo& info) -> bool {
               return Ship::Context::GetInstance()->GetWindow()->GetWindowBackend() ==
                      Ship::WindowBackend::FAST3D_DXGI_DX11;
           },
            "Not Available on DirectX" } },
        { DISABLE_FOR_MATCH_REFRESH_RATE_ON,
          { [](disabledInfo& info) -> bool { return CVarGetInteger("gMatchRefreshRate", 0); },
            "Match Refresh Rate is Enabled" } },
        /*{ DISABLE_FOR_FRAME_ADVANCE_OFF,
          { [](disabledInfo& info) -> bool { return !(gPlayState != nullptr && gPlayState->frameAdvCtx.enabled); },
            "Frame Advance is Disabled" } },*/
        { DISABLE_FOR_INTRO_SKIP_OFF,
          { [](disabledInfo& info) -> bool { return !CVarGetInteger("gEnhancements.Cutscenes.SkipIntroSequence", 0); },
            "Intro Skip Not Selected" } },
        { DISABLE_FOR_ADVANCED_RESOLUTION_ON,
          { [](disabledInfo& info) -> bool { return CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".Enabled", 0); },
            "Advanced Resolution Enabled" } },
        { DISABLE_FOR_VERTICAL_RES_TOGGLE_ON,
          { [](disabledInfo& info) -> bool {
               return CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".VerticalResolutionToggle", 0);
           },
            "Vertical Resolution Toggle Enabled" } },
        { DISABLE_FOR_LOW_RES_MODE_ON,
          { [](disabledInfo& info) -> bool { return CVarGetInteger(CVAR_LOW_RES_MODE, 0); }, "N64 Mode Enabled" } },
    };
}

void SohMenu::UpdateElement() {
    Ship::Menu::UpdateElement();
}

void SohMenu::Draw() {
    Ship::Menu::Draw();
}

void SohMenu::DrawElement() {
    Ship::Menu::DrawElement();
}
} // namespace BenGui
