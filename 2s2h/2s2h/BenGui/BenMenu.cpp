#include "BenMenu.h"
#include "BenGui.hpp"
#include "UIWidgets.hpp"
#include "BenPort.h"
#include "BenInputEditorWindow.h"
#include "DeveloperTools/SaveEditor.h"
#include "DeveloperTools/CollisionViewer.h"
#include "2s2h/Enhancements/Enhancements.h"
#include "2s2h/Enhancements/GfxPatcher/AuthenticGfxPatches.h"
#include "2s2h/PresetManager/PresetManager.h"
#include "HudEditor.h"
#include "Notification.h"
#include "2s2h/Enhancements/Trackers/DisplayOverlay.h"
#include <variant>
#include <ship/utils/StringHelper.h>
#include <spdlog/fmt/fmt.h>
#include "variables.h"
#include <variant>
#include <tuple>
#include "ResolutionEditor.h"
#include "2s2h/Rando/Rando.h"
#include "build.h"

#include <fast/Fast3dGui.h>
#include <fast/Fast3dWindow.h>

extern "C" {
#include "z64.h"
#include "functions.h"
#include "assets/archives/icon_item_24_static/icon_item_24_static_yar.h"
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}
extern std::unordered_map<s16, const char*> warpPointSceneList;
extern void Warp();

static std::unordered_map<int32_t, const char*> imguiScaleOptions = {
    { 0, "Small" },
    { 1, "Normal" },
    { 2, "Large" },
    { 3, "X-Large" },
};

static const std::unordered_map<int32_t, const char*> menuThemeOptions = {
    { UIWidgets::Colors::Red, "Red" },
    { UIWidgets::Colors::DarkRed, "Dark Red" },
    { UIWidgets::Colors::Orange, "Orange" },
    { UIWidgets::Colors::Green, "Green" },
    { UIWidgets::Colors::DarkGreen, "Dark Green" },
    { UIWidgets::Colors::LightBlue, "Light Blue" },
    { UIWidgets::Colors::Blue, "Blue" },
    { UIWidgets::Colors::DarkBlue, "Dark Blue" },
    { UIWidgets::Colors::Indigo, "Indigo" },
    { UIWidgets::Colors::Violet, "Violet" },
    { UIWidgets::Colors::Purple, "Purple" },
    { UIWidgets::Colors::Brown, "Brown" },
    { UIWidgets::Colors::Gray, "Gray" },
    { UIWidgets::Colors::DarkGray, "Dark Gray" },
};

static const std::vector<const char*> alwaysWinDoggyraceOptions = {
    "Off",                       // ALWAYS_WIN_DOGGY_RACE_OFF
    "When owning Mask of Truth", // ALWAYS_WIN_DOGGY_RACE_MASKOFTRUTH
    "Always",                    // ALWAYS_WIN_DOGGY_RACE_ALWAYS
};

static const std::unordered_map<int32_t, const char*> bossHealthOptions = {
    { 0, "1x (Default)" }, { 1, "1.25x" }, { 2, "1.50x" }, { 3, "1.75x" }, { 4, "2x" }
};

static const std::vector<const char*> cremiaRewardOptions = {
    "Vanilla", // CREMIA_REWARD_RANDOM
    "Hug",     // CREMIA_REWARD_ALWAYS_HUG
    "Rupee",   // CREMIA_REWARD_ALWAYS_RUPEE
};

static const std::vector<const char*> treasureChestShopMazeOptions = {
    "Off",         // TREASURE_CHEST_SHOP_MAZE_OFF
    "Full Height", // TREASURE_CHEST_SHOP_MAZE_FULL_HEIGHT
    "Tiered",      // TREASURE_CHEST_SHOP_MAZE_TIERED
};

static const std::vector<const char*> ammoBuybackOptions = {
    "Vanilla",    // AMMO_BUYBACK_VANILLA
    "Full Price", // AMMO_BUYBACK_FULL_PRICE
    "Half Price", // AMMO_BUYBACK_HALF_PRICE
};

static const std::vector<const char*> gibdoTradeSequenceOptions = {
    "Vanilla",  // GIBDO_TRADE_SEQUENCE_VANILLA
    "MM3D",     // GIBDO_TRADE_SEQUENCE_MM3D
    "No trade", // GIBDO_TRADE_SEQUENCE_NO_TRADE
};

static const std::vector<const char*> clockTypeOptions = {
    "Original",   // CLOCK_TYPE_ORIGINAL
    "MM3D style", // CLOCK_TYPE_3DS
    "Text only",  // CLOCK_TYPE_TEXT_BASED
};

static const std::vector<const char*> fastTransformationOptions = {
    "Off",              // FAST_TRANSFORM_OFF
    "On",               // FAST_TRANSFORM_ON
    "After First Time", // FAST_TRANSFORM_AFTER_FIRST
};

static const std::vector<const char*> textureFilteringOptions = {
    "Three-Point", // Fast::FILTER_THREE_POINT,
    "Linear",      // Fast::FILTER_LINEAR
    "None",        // Fast::FILTER_NONE
};

static const std::vector<const char*> motionBlurOptions = {
    "Dynamic (default)", // MOTION_BLUR_DYNAMIC
    "Always Off",        // MOTION_BLUR_ALWAYS_OFF
    "Always On",         // MOTION_BLUR_ALWAYS_ON
};
static const std::vector<const char*> debugSaveOptions = {
    "Empty save",         // DEBUG_SAVE_INFO_NONE
    "Vanilla debug save", // DEBUG_SAVE_INFO_VANILLA_DEBUG
    "100% save",          // DEBUG_SAVE_INFO_COMPLETE
};

#ifdef _DEBUG
DebugLogOption defaultLogLevel = DEBUG_LOG_DEBUG;
#else
DebugLogOption defaultLogLevel = DEBUG_LOG_INFO;
#endif

static const std::vector<const char*> logLevels = {
    "Trace",    // DEBUG_LOG_TRACE
    "Debug",    // DEBUG_LOG_DEBUG
    "Info",     // DEBUG_LOG_INFO
    "Warn",     // DEBUG_LOG_WARN
    "Error",    // DEBUG_LOG_ERROR
    "Critical", // DEBUG_LOG_CRITICAL
    "Off",      // DEBUG_LOG_OFF
};

static const std::vector<const char*> timeStopOptions = {
    "Off",                     // TIME_STOP_OFF
    "Temples",                 // TIME_STOP_TEMPLES
    "Temples + Mini Dungeons", // TIME_STOP_TEMPLES_DUNGEONS
};

static const std::vector<const char*> speedModifierModeOptions = {
    "Off",
    "On",
    "Hold Buttons",
    "Toggle Buttons",
};

static const std::vector<const char*> dashAfterRollOptions = {
    "Off",                   // DASH_AFTER_ROLL_OFF
    "On",                    // DASH_AFTER_ROLL_ON
    "Stack with Bunny Hood", // DASH_AFTER_ROLL_STACK
};

static const std::vector<const char*> notificationPosition = {
    "Top Left", "Top Right", "Bottom Left", "Bottom Right", "Hidden",
};

static const std::vector<const char*> dekuGuardSearchBallsOptions = {
    "Night Only", // DEKU_GUARD_SEARCH_BALLS_NIGHT_ONLY
    "Never",      // DEKU_GUARD_SEARCH_BALLS_NEVER
    "Always",     // DEKU_GUARD_SEARCH_BALLS_ALWAYS
};

static const std::vector<const char*> skipGetItemCutscenesOptions = {
    "Never",
    "Junk Items Only",
    "Everything But Major",
    "Always",
};

static const std::vector<const char*> powerCrouchStabOptions = {
    "Patched (US/EU)",
    "Unpatched (JP)",
    "Unpatched (OoT)",
};

static const std::vector<const char*> maskOfTruthGrottoOptions = {
    "Off",                // HIDDEN_GROTTOS_VISIBLITY_OFF
    "Wear Mask of Truth", // HIDDEN_GROTTOS_VISIBLITY_WEAR_MASK_OF_TRUTH
    "Have Mask of Truth", // HIDDEN_GROTTOS_VISIBLITY_HAVE_MASK_OF_TRUTH
    "Always",             // HIDDEN_GROTTOS_VISIBLITY_ALWAYS
};

static const std::vector<const char*> goronRaceDifficultyOptions = {
    "Vanilla",  // GORON_RACE_DIFFICULTY_VANILLA
    "Balanced", // GORON_RACE_DIFFICULTY_BALANCED
    "Skip",     // GORON_RACE_DIFFICULTY_SKIP
};

static const std::vector<const char*> torchTimeLimitOptions = {
    "Normal",   // TORCH_TIME_LIMIT_NORMAL
    "Double",   // TORCH_TIME_LIMIT_DOUBLE
    "Infinity", // TORCH_TIME_LIMIT_INFINITY
};

static const std::vector<const char*> timerDisplayOptions = {
    "Off",          // TIMER_DISPLAY_NONE
    "Real-Time",    // TIMER_DISPLAY_RTA
    "In-Game Time", // TIMER_DISPLAY_IGT
};

static const std::vector<const char*> mirroredWorldModes = {
    "Off",                      // MIRRORED_WORLD_OFF
    "Always",                   // MIRRORED_WORLD_ALWAYS
    "Random",                   // MIRRORED_WORLD_RANDOM
    "Random (Seeded)",          // MIRRORED_WORLD_RANDOM_SEEDED
    "Dungeons (Temples)",       // MIRRORED_WORLD_DUNGEONS_TEMPLES
    "Dungeons (Spider Houses)", // MIRRORED_WORLD_DUNGEONS_SPIDERS
    "Dungeons (All)",           // MIRRORED_WORLD_DUNGEONS_ALL
    "Dungeons Random",          // MIRRORED_WORLD_DUNGEONS_RANDOM
    "Dungeons Random (Seeded)", // MIRRORED_WORLD_DUNGEONS_RANDOM_SEEDED
};

static const std::unordered_map<int32_t, const char*> damageMultiplierOptions = {
    { 0, "1x" }, { 1, "2x" }, { 2, "4x" }, { 3, "8x" }, { 4, "16x" }, { 10, "1 Hit KO" },
};

namespace BenGui {
extern std::shared_ptr<BenMenu> mBenMenu;
void FreeLookPitchMinMax() {
    f32 maxY = CVarGetFloat("gEnhancements.Camera.FreeLook.MaxPitch", 72.0f);
    f32 minY = CVarGetFloat("gEnhancements.Camera.FreeLook.MinPitch", -49.0f);
    CVarSetFloat("gEnhancements.Camera.FreeLook.MaxPitch", std::max(maxY, minY));
    CVarSetFloat("gEnhancements.Camera.FreeLook.MinPitch", std::min(maxY, minY));
}

using namespace UIWidgets;

void BenMenu::AddSidebarEntry(std::string sectionName, std::string sidebarName, uint32_t columnCount) {
    assert(!sectionName.empty());
    assert(!sidebarName.empty());
    menuEntries.at(sectionName).sidebars.emplace(sidebarName, SidebarEntry{ .columnCount = columnCount });
    menuEntries.at(sectionName).sidebarOrder.push_back(sidebarName);
}

WidgetInfo& BenMenu::AddWidget(WidgetPath& pathInfo, std::string widgetName, WidgetType widgetType) {
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
        case WIDGET_CVAR_BTN_SELECTOR:
            widget.options = std::make_shared<BtnSelectorOptions>();
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

// Last generated with the following command on 01/10/2026. The gap in commits is so that cherry-picks from other repos
// do not get mixed in.
// clang-format off
// { git shortlog -sn 2332f63..558f59b ; git shortlog -sn dfcc80e..HEAD; } | awk '{n=$1; $1=""; sub(/^ /,""); c[$0]+=n} END{for(a in c) print c[a],a}' | sort -nr | sed -E 's/^[[:space:]]*[0-9]+[[:space:]]+/"/; s/(.+)/\1",/'
// clang-format on
std::vector<std::string> contributors = {
    "ProxySaw", // "Garrett Cox", manual replacement
    "Archez",   // "Adam Bird", dupe
    "Eblo",
    "balloondude2",
    "louist103",
    "Caladius",
    "mckinlee",
    "sitton76",
    "inspectredc",
    "Jordan Longstaff",
    "ItsHeckinPat", // "Patrick12115", dupe
    "Jameriquiah",  // "Jordyn Hardyman", dupe
    "Malkierian",
    "briaguya",
    "PurpleHato",
    "Joshua Sanchez",
    "Garrett",
    "aMannus",
    "zodiac-ill",
    "OtherBlue",
    "Glought",
    "rachaellama",
    "lightmanLP",
    "Spodi",
    "Sirius902",
    "Revo",
    "J",
    "anthony-barricelli",
    "ReddestDream",
    "Philip Dubé",
    "Nicholas Estelami",
    "Mrlinkwii",
    "lilacLunatic",
    "Liam Scholte",
    "Lars-Christian Selland",
    "GaryOderNichts",
    "enzu.ru",
    "Bradley Sherman",
    "verbes4",
    "Travis",
    "tortugaveloz",
    "stellarkookies",
    "Rozelette",
    "Reinhardt R. Gaming",
    "Ralphie Morell",
    "Quorsor",
    "Qlonever",
    "Mothstery",
    "MegaMech",
    "Marvin Coto",
    "Louis",
    "Kenix3",
    "kaeporagaebora",
    "justawayofthesamurai",
    "Jacob Erly",
    "Hoeloe",
    "Hannah Brown",
    "grande1900",
    "Ghunzor",
    "GhostlyDark",
    "frogssoldseparately",
    "Felix Dietrich",
    "Extloga",
    "ErawanJohnson",
    "djevangelia",
    "cplaster",
    "Corbin Park",
    "Captain Kitty Cat",
    "Ben Willmore",
    "banteg",
    "ammar sadaoui",
    "AltoXorg",
    "Alejandro Asenjo Nitti",
};

void BenMenu::AddSettings() {
    // Add Settings menu
    AddMenuEntry("Settings", "gSettings.Menu.SettingsSidebarSection");
    // General Settings
    AddSidebarEntry("Settings", "General", 2);
    WidgetPath path = { "Settings", "General", SECTION_COLUMN_1 };
    AddWidget(path, "Menu Theme", WIDGET_CVAR_COMBOBOX)
        .CVar("gSettings.Menu.Theme")
        .Options(ComboboxOptions()
                     .Tooltip("Changes the Theme of the Menu Widgets.")
                     .ComboMap(&menuThemeOptions)
                     .DefaultIndex(Colors::LightBlue));
    AddWidget(path, "Menu Background Opacity", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Menu.BackgroundOpacity")
        .Options(FloatSliderOptions().DefaultValue(0.85f).IsPercentage().Tooltip(
            "Sets the opacity of the background of the port menu."));
#if not defined(__SWITCH__) and not defined(__WIIU__)
    AddWidget(path, "Menu Controller Navigation", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_IMGUI_CONTROLLER_NAV)
        .Options(CheckboxOptions().Tooltip(
            "Allows controller navigation of the 2Ship menu (Settings, Enhancements,...)\nCAUTION: "
            "This will disable game inputs while the menu is visible.\n\nD-pad to move between "
            "items, A to select, B to move up in scope."));
    AddWidget(path, "Allow background inputs", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ALLOW_BACKGROUND_INPUTS)
        .Callback([](WidgetInfo& info) {
            SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,
                        CVarGetInteger(CVAR_ALLOW_BACKGROUND_INPUTS, 1) ? "1" : "0");
        })
        .Options(CheckboxOptions()
                     .Tooltip("Allows controller inputs to be picked up by the game even when the game window isn't "
                              "the focused window.")
                     .DefaultValue(true));
    AddWidget(path, "Cursor Always Visible", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.CursorVisibility")
        .Callback([](WidgetInfo& info) {
            Ship::Context::GetRawInstance()->GetWindow()->SetForceCursorVisibility(
                CVarGetInteger("gSettings.CursorVisibility", 0));
        })
        .Options(CheckboxOptions().Tooltip("Makes the cursor always visible, even in full screen."));
#endif
    AddWidget(path, "Search In Sidebar", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.Menu.SidebarSearch")
        .Callback([](WidgetInfo& info) {
            if (CVarGetInteger("gSettings.Menu.SidebarSearch", 0)) {
                mBenMenu->InsertSidebarSearch();
            } else {
                mBenMenu->RemoveSidebarSearch();
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
            CheckboxOptions().Tooltip("Allows pressing the Tab key to toggle alternate assets.").DefaultValue(true));
    AddWidget(path, "Reset Button Combination:", WIDGET_CVAR_BTN_SELECTOR)
        .CVar("gSettings.ResetBtn")
        .Options(BtnSelectorOptions().DefaultValue(BTN_CUSTOM_MODIFIER2));
    AddWidget(path, "Open App Files Folder", WIDGET_BUTTON)
        .Callback([](WidgetInfo& info) {
            std::string filesPath = Ship::Context::GetRawInstance()->GetAppDirectoryPath();
            SDL_OpenURL(std::string("file:///" + std::filesystem::absolute(filesPath).string()).c_str());
        })
        .Options(ButtonOptions().Tooltip("Opens the folder that contains the save and mods folders, etc."));

    AddWidget(path, "ImGui Menu Scaling", WIDGET_CVAR_COMBOBOX)
        .CVar("gSettings.ImGuiScale")
        .Options(ComboboxOptions()
                     .ComboMap(&imguiScaleOptions)
                     .Tooltip("Changes the scaling of the ImGui menu elements.")
                     .DefaultIndex(1)
                     .ComponentAlignment(UIWidgets::Right)
                     .LabelPosition(UIWidgets::Far))
        .Callback([](WidgetInfo& info) { OTRGlobals::Instance->ScaleImGui(); });

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "about", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) {
        ImGui::BeginChild("about");
        ImGui::PushStyleColor(ImGuiCol_Text, ColorValues.at(Colors::Gray));
        if (gGitCommitTag[0] == 0) {
            ImGui::Text("%s | %s | %s", (char*)gBuildVersion, (char*)gGitBranch, (char*)gGitCommitHash);
        } else {
            ImGui::Text("%s", (char*)gBuildVersion);
        }
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.5f, 1.0f));
        ImGui::SeparatorText("Thank You");
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImTextureID heartTextureId =
            std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                ->GetTextureByName((const char*)gQuestIconHeartContainer2Tex);
        ImGui::Image(heartTextureId, ImVec2(25.0f, 25.0f));
        ImGui::TextWrapped("Special thanks to our contributors, playtesters, artists, moderators, helpers, and "
                           "everyone in the larger decomp & N64 communities who make this project possible.\n\n");

        // Draw auto scrolling list of contributors in columns
        ImGui::SetNextWindowSize(ImVec2(0.0f, ImGui::GetMainViewport()->WorkSize.y / 3));
        ImGui::BeginChild("contributors", ImVec2(0, 0), 0,
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        static double scrollSpeed = 1.5f * (ImGui::GetFontSize() / 1000.0f); // Lines to scroll per second
        static int numColumns = 2; // Two columns seem to work best. Some names are too long for more on lower res

        // Calculate the height of one full list iteration
        float lineHeight = ImGui::GetTextLineHeightWithSpacing();
        float singleListHeight =
            (contributors.size() / numColumns + (contributors.size() % numColumns != 0 ? 1 : 0)) * lineHeight;

        // Calculate scroll position that wraps seamlessly
        double scrollPosition = fmod((GetUnixTimestamp() % 18446744000000000000) * scrollSpeed, singleListHeight);
        ImGui::SetScrollY(scrollPosition);

        // Render the contributors list twice for infinite scroll effect
        for (int iteration = 0; iteration < 2; iteration++) {
            for (int column = 0; column < numColumns; column++) {
                if (column > 0)
                    ImGui::SameLine();

                ImGui::BeginGroup();
                for (int i = column; i < contributors.size(); i += numColumns) {
                    ImGui::Text("%s", contributors.at(i).c_str());
                }
                ImGui::EndGroup();
            }
        }
        ImGui::EndChild();

        ImGui::EndChild();
    });

    // Audio Settings
    path.sidebarName = "Audio";
    path.column = SECTION_COLUMN_1;
    AddSidebarEntry("Settings", "Audio", 3);
    AddWidget(path, "Master Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.MasterVolume")
        .Options(FloatSliderOptions()
                     .Tooltip("Adjust the overall sound volume.")
                     .ShowAdjustmentButtons(false)
                     .Format("")
                     .IsPercentage());
    AddWidget(path, "Main Music Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.MainMusicVolume")
        .Callback([](WidgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_BGM_MAIN, CVarGetFloat("gSettings.Audio.MainMusicVolume", 1.0f));
        })
        .Options(FloatSliderOptions()
                     .Tooltip("Adjust the background music volume.")
                     .ShowAdjustmentButtons(false)
                     .Format("")
                     .IsPercentage());
    AddWidget(path, "Sub Music Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.SubMusicVolume")
        .Callback([](WidgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_BGM_SUB, CVarGetFloat("gSettings.Audio.SubMusicVolume", 1.0f));
        })
        .Options(FloatSliderOptions()
                     .Tooltip("Adjust the sub music volume.")
                     .ShowAdjustmentButtons(false)
                     .Format("")
                     .IsPercentage());
    AddWidget(path, "Sound Effects Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.SoundEffectsVolume")
        .Callback([](WidgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_SFX, CVarGetFloat("gSettings.Audio.SoundEffectsVolume", 1.0f));
        })
        .Options(FloatSliderOptions()
                     .Tooltip("Adjust the sound effects volume.")
                     .ShowAdjustmentButtons(false)
                     .Format("")
                     .IsPercentage());
    AddWidget(path, "Fanfare Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.FanfareVolume")
        .Callback([](WidgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_FANFARE, CVarGetFloat("gSettings.Audio.FanfareVolume", 1.0f));
        })
        .Options(FloatSliderOptions()
                     .Tooltip("Adjust the fanfare volume.")
                     .ShowAdjustmentButtons(false)
                     .Format("")
                     .IsPercentage());
    AddWidget(path, "Ambience Volume: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gSettings.Audio.AmbienceVolume")
        .Callback([](WidgetInfo& info) {
            AudioSeq_SetPortVolumeScale(SEQ_PLAYER_AMBIENCE, CVarGetFloat("gSettings.Audio.AmbienceVolume", 1.0f));
        })
        .Options(FloatSliderOptions()
                     .Tooltip("Adjust the ambient sound volume.")
                     .ShowAdjustmentButtons(false)
                     .Format("")
                     .IsPercentage());
    AddWidget(path, "Audio API", WIDGET_AUDIO_BACKEND);

    // Graphics Settings
    path.sidebarName = "Graphics";
    path.column = SECTION_COLUMN_1;
    AddSidebarEntry("Settings", "Graphics", 3);
    AddWidget(path, "Graphics Options", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Toggle Fullscreen", WIDGET_BUTTON)
        .Callback([](WidgetInfo& info) { Ship::Context::GetRawInstance()->GetWindow()->ToggleFullscreen(); })
        .Options(ButtonOptions().Tooltip("Toggles Fullscreen On/Off."));
    AddWidget(path, "Internal Resolution: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_INTERNAL_RESOLUTION)
        .Callback([](WidgetInfo& info) {
            Ship::Context::GetRawInstance()->GetWindow()->SetResolutionMultiplier(
                CVarGetFloat(CVAR_INTERNAL_RESOLUTION, 1));
        })
        .PreFunc([](WidgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_ADVANCED_RESOLUTION_ON).active &&
                mBenMenu->disabledMap.at(DISABLE_FOR_VERTICAL_RES_TOGGLE_ON).active) {
                info.activeDisables.push_back(DISABLE_FOR_ADVANCED_RESOLUTION_ON);
                info.activeDisables.push_back(DISABLE_FOR_VERTICAL_RES_TOGGLE_ON);
            } else if (mBenMenu->disabledMap.at(DISABLE_FOR_LOW_RES_MODE_ON).active) {
                info.activeDisables.push_back(DISABLE_FOR_LOW_RES_MODE_ON);
            }
        })
        .Options(
            FloatSliderOptions()
                .Tooltip("Multiplies your output resolution by the value inputted, as a more intensive but effective "
                         "form of anti-aliasing.")
                .ShowAdjustmentButtons(false)
                .IsPercentage()
                .Format("")
                .Min(0.5f)
                .Max(2.0f));
#ifndef __WIIU__
    AddWidget(path, "Anti-aliasing (MSAA): %d", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_MSAA_VALUE)
        .Callback([](WidgetInfo& info) {
            Ship::Context::GetRawInstance()->GetWindow()->SetMsaaLevel(CVarGetInteger(CVAR_MSAA_VALUE, 1));
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
            if (mBenMenu->disabledMap.at(DISABLE_FOR_MATCH_REFRESH_RATE_ON).active)
                info.activeDisables.push_back(DISABLE_FOR_MATCH_REFRESH_RATE_ON);
        })
        .Options(IntSliderOptions().Min(20).Max(360).DefaultValue(20).Tooltip(
            "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics. "
            "This is purely visual and does not impact game logic, execution of glitches etc.\n\n"
            "A higher target FPS than your monitor's refresh rate will waste resources, and might give a worse "
            "result."));
    AddWidget(path, "Match Refresh Rate", WIDGET_CVAR_CHECKBOX)
        .CVar("gMatchRefreshRate")
        .Options(CheckboxOptions().Tooltip("Matches interpolation value to the refresh rate of your display."));
    AddWidget(path, "Renderer API (Needs reload)", WIDGET_VIDEO_BACKEND);
    AddWidget(path, "Enable Vsync", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_VSYNC_ENABLED)
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NO_VSYNC).active; })
        .Options(CheckboxOptions()
                     .Tooltip("Removes tearing, but clamps your max FPS to your displays refresh rate.")
                     .DefaultValue(true));
    AddWidget(path, "Windowed Fullscreen", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_SDL_WINDOWED_FULLSCREEN)
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NO_WINDOWED_FULLSCREEN).active;
        })
        .Options(CheckboxOptions().Tooltip("Enables Windowed Fullscreen Mode."));
    AddWidget(path, "Allow multi-windows", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENABLE_MULTI_VIEWPORTS)
        .PreFunc(
            [](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NO_MULTI_VIEWPORT).active; })
        .Options(CheckboxOptions()
                     .Tooltip("Allows multiple windows to be opened at once. Requires a reload to take effect.")
                     .DefaultValue(true));
    AddWidget(path, "Texture Filter (Needs reload)", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_TEXTURE_FILTER)
        .Options(ComboboxOptions().Tooltip("Sets the applied Texture Filtering.").ComboVec(&textureFilteringOptions));

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Advanced Graphics Options", WIDGET_SEPARATOR_TEXT);

    path.sidebarName = "Controls";
    AddSidebarEntry("Settings", "Controls", 1);
    AddWidget(path, "Popout Bindings Window", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.BenInputEditor")
        .WindowName("2S2H Input Editor")
        .Options(ButtonOptions().Tooltip("Enables the separate Bindings Window.").Size(Sizes::Inline));

    path.sidebarName = "Overlay";
    path.column = SECTION_COLUMN_1;
    AddSidebarEntry("Settings", "Overlay", 2);
    AddWidget(path, "Notifications", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Position", WIDGET_CVAR_COMBOBOX)
        .CVar("gNotifications.Position")
        .Options(ComboboxOptions()
                     .Tooltip("Which corner of the screen notifications appear in.")
                     .ComboVec(&notificationPosition)
                     .DefaultIndex(3));
    AddWidget(path, "Duration: %.1f seconds", WIDGET_CVAR_SLIDER_FLOAT)
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
    path.column = SECTION_COLUMN_2;
    AddWidget(path, "In-Game Timer", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Display", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_DISPLAY_OVERLAY_MODE)
        .WindowName("Display Overlay")
        .Callback([](WidgetInfo& info) {
            int mode = CVarGetInteger(CVAR_DISPLAY_OVERLAY_MODE, TIMER_DISPLAY_NONE);
            SetDisplayOverlayVisibility(mode != TIMER_DISPLAY_NONE);
        })
        .Options(
            ComboboxOptions()
                .Tooltip(
                    "How the timer should be displayed in the overlay.\n\n"
                    "- Off: Do not display a timer\n"
                    "- Real-Time: Display the time that has elapsed since creating the save file, regardless of play.\n"
                    "- In-Game Time: Display the time spent playing the save file")
                .ComboVec(&timerDisplayOptions));
    AddWidget(path, "Hide Window Background", WIDGET_CVAR_CHECKBOX)
        .CVar("gDisplayOverlay.Background")
        .Options(CheckboxOptions().Tooltip("Hides the background of the Display Overlay window."));
    AddWidget(path, "Scale: %.1fx", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gDisplayOverlay.Scale")
        .Options(FloatSliderOptions()
                     .Tooltip("Adjust the Scale for the Display Overlay window.")
                     .Min(1.0f)
                     .Max(5.0f)
                     .DefaultValue(1.0f)
                     .Format("%.1f")
                     .Step(0.1f));

    path.column = SECTION_COLUMN_1;
    path.sidebarName = "Presets";
    AddSidebarEntry("Settings", "Presets", 1);
    AddWidget(path, "Presets", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) { PresetManager_Draw(); });

    // Input Viewer
    path.sidebarName = "Input Viewer";
    path.column = SECTION_COLUMN_1;
    AddSidebarEntry("Settings", path.sidebarName, 2);
    AddWidget(path, "Input Viewer", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Toggle Input Viewer", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.InputViewer")
        .WindowName("Input Viewer")
        .Options(ButtonOptions().Tooltip("Toggles the Input Viewer."));

    AddWidget(path, "Input Viewer Settings", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Popout Input Viewer Settings", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.InputViewerSettings")
        .WindowName("Input Viewer Settings")
        .Options(ButtonOptions().Tooltip("Enables the separate Input Viewer Settings Window."));

    // Mod Menu
    path.sidebarName = "Mod Menu";
    path.column = SECTION_COLUMN_1;
    AddSidebarEntry("Settings", path.sidebarName, 2);
    AddWidget(path, "Mod Menu", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Popout Mod Menu Window", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.ModMenu")
        .WindowName("Mod Menu")
        .HideInSearch(true)
        .Options(ButtonOptions().Tooltip("Enables the separate Mod Menu Window."));
}
int32_t motionBlurStrength;

void BenMenu::AddEnhancements() {
    AddMenuEntry("Enhancements", "gSettings.Menu.EnhancementsSidebarSection");
    WidgetPath path = { "Enhancements", "Camera", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Camera", 3);
    // Camera Snap Fix
    AddWidget(path, "Fixes", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fix Targeting Camera Snap", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FixTargettingCameraSnap")
        .Options(CheckboxOptions().Tooltip(
            "Fixes the camera snap that occurs when you are moving and press the targeting button."));
    AddWidget(path, "First Person", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Disable Auto-Centering", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.DisableFirstPersonAutoCenterView")
        .PreFunc([](WidgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_ON).active)
                info.activeDisables.push_back(DISABLE_FOR_GYRO_ON);
        })
        .Options(CheckboxOptions().Tooltip("Disables the auto-centering of the camera in first person mode."));
    AddWidget(path, "Invert X Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.InvertX")
        .Options(CheckboxOptions().Tooltip("Inverts the X Axis of the Camera in First Person Mode."));
    AddWidget(path, "Invert Y Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.InvertY")
        .Options(
            CheckboxOptions().Tooltip("Inverts the Y Axis of the Camera in First Person Mode.").DefaultValue(true));
    AddWidget(path, "X Axis Sensitivity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FirstPerson.SensitivityX")
        .Options(FloatSliderOptions()
                     .Tooltip("Adjusts the Sensitivity of the X Axis in First Person Mode.")
                     .DefaultValue(1.0f)
                     .IsPercentage()
                     .Min(0.01f)
                     .Max(2.0f));
    AddWidget(path, "Y Axis Sensitivity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FirstPerson.SensitivityY")
        .Options(FloatSliderOptions()
                     .Tooltip("Adjusts the Sensitivity of the Y Axis in First Person Mode.")
                     .DefaultValue(1.0f)
                     .IsPercentage()
                     .Min(0.01f)
                     .Max(2.0f));
    AddWidget(path, "Gyro Aiming", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.GyroEnabled")
        .Options(CheckboxOptions().Tooltip("Enables Gyro Aiming in First Person Mode."));
    AddWidget(path, "Invert Gyro X Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.GyroInvertX")
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_OFF).active; })
        .Options(CheckboxOptions().Tooltip("Inverts the X Axis of the Gyro in First Person Mode."));
    AddWidget(path, "Invert Gyro Y Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.GyroInvertY")
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_OFF).active; })
        .Options(CheckboxOptions().Tooltip("Inverts the Y Axis of the Gyro in First Person Mode."));
    AddWidget(path, "Gyro X Axis Sensitivity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FirstPerson.GyroSensitivityX")
        .Options(FloatSliderOptions()
                     .Tooltip("Adjusts the Sensitivity of the X Axis of the Gyro in First Person Mode.")
                     .DefaultValue(1.0f)
                     .IsPercentage()
                     .Min(0.01f)
                     .Max(2.0f))
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_OFF).active; });
    AddWidget(path, "Gyro Y Axis Sensitivity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FirstPerson.GyroSensitivityY")
        .Options(FloatSliderOptions()
                     .Tooltip("Adjusts the Sensitivity of the Y Axis of the Gyro in First Person Mode.")
                     .DefaultValue(1.0f)
                     .IsPercentage()
                     .Min(0.01f)
                     .Max(2.0f))
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_GYRO_OFF).active; });
    AddWidget(path, "Right Stick Aiming", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.RightStickEnabled")
        .Options(CheckboxOptions().Tooltip("Enables Right Stick Aiming in First Person Mode."));
    AddWidget(path, "Move while aiming", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.MoveInFirstPerson")
        .PreFunc([](WidgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active)
                info.activeDisables.push_back(DISABLE_FOR_RIGHT_STICK_OFF);
        })
        .Options(CheckboxOptions().Tooltip("Allows movement with the left stick while in first person mode."));
    AddWidget(path, "Invert Right Stick X Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.RightStickInvertX")
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active; })
        .Options(CheckboxOptions().Tooltip("Inverts the X Axis of the Right Stick in First Person Mode."));
    AddWidget(path, "Invert Right Stick Y Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FirstPerson.RightStickInvertY")
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active; })
        .Options(CheckboxOptions()
                     .Tooltip("Inverts the Y Axis of the Right Stick in First Person Mode.")
                     .DefaultValue(true));
    AddWidget(path, "Right Stick X Axis Sensitivity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FirstPerson.RightStickSensitivityX")
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active; })
        .Options(FloatSliderOptions()
                     .Tooltip("Adjusts the Sensitivity of the X Axis of the Right Stick in First Person Mode.")
                     .DefaultValue(1.0f)
                     .IsPercentage()
                     .Min(0.01f)
                     .Max(2.0f));
    AddWidget(path, "Right Stick Y Axis Sensitivity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FirstPerson.RightStickSensitivityY")
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_RIGHT_STICK_OFF).active; })
        .Options(FloatSliderOptions()
                     .Tooltip("Adjusts the Sensitivity of the Y Axis of the Right Stick in First Person Mode.")
                     .DefaultValue(1.0f)
                     .IsPercentage()
                     .Min(0.01f)
                     .Max(2.0f));

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Cameras", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Free Look", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.FreeLook.Enable")
        .PreFunc([](WidgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_CAM_ON).active)
                info.activeDisables.push_back(DISABLE_FOR_DEBUG_CAM_ON);
        })
        .Options(CheckboxOptions().Tooltip(
            "Enables free look camera control.\nNote: You must remap C buttons off of the right "
            "stick in the controller config menu, and map the camera stick to the right stick."));
    AddWidget(path, "Camera Distance: %d", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Camera.FreeLook.MaxCameraDistance")
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active; })
        .Options(
            IntSliderOptions().Tooltip("Maximum Camera Distance for Free Look.").Min(100).Max(900).DefaultValue(185));
    AddWidget(path, "Camera Transition Speed: %d", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Camera.FreeLook.TransitionSpeed")
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active; })
        .Options(IntSliderOptions().Min(1).Max(900).DefaultValue(25));
    AddWidget(path, "Max Camera Height Angle: %.0f\xC2\xB0", WIDGET_CVAR_SLIDER_FLOAT)
        .Callback([](WidgetInfo& info) { FreeLookPitchMinMax(); })
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active; })
        .CVar("gEnhancements.Camera.FreeLook.MaxPitch")
        .Options(FloatSliderOptions()
                     .Tooltip("Maximum Height of the Camera.")
                     .Format("%.0f\xC2\xB0")
                     .Min(-89.0f)
                     .Max(89.0f)
                     .DefaultValue(72.0f));
    AddWidget(path, "Min Camera Height Angle: %.0f\xC2\xB0", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.FreeLook.MinPitch")
        .Callback([](WidgetInfo& info) { FreeLookPitchMinMax(); })
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_OFF).active; })
        .Options(FloatSliderOptions()
                     .Tooltip("Minimum Height of the Camera.")
                     .Format("%.0f\xC2\xB0")
                     .Min(-89.0f)
                     .Max(89.0f)
                     .DefaultValue(-49.0f));
    AddWidget(path, "Debug Camera", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.DebugCam.Enable")
        .PreFunc([](WidgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_FREE_LOOK_ON).active) {
                info.activeDisables.push_back(DISABLE_FOR_FREE_LOOK_ON);
            }
        })
        .Options(CheckboxOptions().Tooltip("Enables debug camera control."));
    AddWidget(path, "Invert Camera X Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.RightStick.InvertXAxis")
        .PreFunc([](WidgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
            }
        })
        .Options(CheckboxOptions().Tooltip("Inverts the Camera X Axis"));
    AddWidget(path, "Invert Camera Y Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.RightStick.InvertYAxis")
        .PreFunc([](WidgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
            }
        })
        .Options(CheckboxOptions().Tooltip("Inverts the Camera Y Axis").DefaultValue(true));
    AddWidget(path, "Third-Person Camera\nHorizontal Sensitivity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.RightStick.CameraSensitivity.X")
        .PreFunc([](WidgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
            }
        })
        .Options(FloatSliderOptions()
                     .Tooltip("Adjust the Sensitivity of the x axis when in Third Person.")
                     .Format("%.0f%%")
                     .DefaultValue(1.0f)
                     .IsPercentage()
                     .Min(0.01f)
                     .Max(5.0f));
    AddWidget(path, "Third-Person Camera\nVertical Sensitivity: %.0f%%", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.RightStick.CameraSensitivity.Y")
        .PreFunc([](WidgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_CAMERAS_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_CAMERAS_OFF);
            }
        })
        .Options(FloatSliderOptions()
                     .Tooltip("Adjust the Sensitivity of the x axis when in Third Person.")
                     .Format("%.0f%%")
                     .DefaultValue(1.0f)
                     .IsPercentage()
                     .Min(0.01f)
                     .Max(5.0f));
    AddWidget(path, "Enable Roll (6\xC2\xB0 of Freedom)", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Camera.DebugCam.6DOF")
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_CAM_OFF).active; })
        .Options(CheckboxOptions().Tooltip(
            "This allows for all six degrees of movement with the camera, NOTE: Yaw will work "
            "differently in this system, instead rotating around the focal point, rather than a polar axis."));
    AddWidget(path, "Camera Speed: %.0f", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Camera.DebugCam.CameraSpeed")
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_CAM_OFF).active; })
        .Options(FloatSliderOptions()
                     .Tooltip("Adjusts the speed of the Camera.")
                     .Format("%.0f%%")
                     .DefaultValue(0.5f)
                     .IsPercentage()
                     .Min(0.1f)
                     .Max(3.0f));

    path = { "Enhancements", "Cheats", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Cheats", 2);
    AddWidget(path, "Infinite Health", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.InfiniteHealth")
        .Options(CheckboxOptions().Tooltip("Always have full Hearts."));
    AddWidget(path, "Infinite Magic", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.InfiniteMagic")
        .Options(CheckboxOptions().Tooltip("Always have full Magic."));
    AddWidget(path, "Infinite Rupees", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.InfiniteRupees")
        .Options(CheckboxOptions().Tooltip("Always have a full Wallet."));
    AddWidget(path, "Infinite Consumables", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.InfiniteConsumables")
        .Options(
            CheckboxOptions().Tooltip("Always have max Consumables, you must have collected the consumables first."));
    AddWidget(path, "Infinite Epona Carrots", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.InfiniteEponaCarrots")
        .Options(CheckboxOptions().Tooltip("Allows Epona to boost without consuming carrots."));
    AddWidget(path, "Easy Frame Advance", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.EasyFrameAdvance")
        .Options(CheckboxOptions().Tooltip(
            "Continue holding START button when unpausing to only advance a single frame and then re-pause."));
    AddWidget(path, "Longer Deku Flower Glide", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.LongerFlowerGlide")
        .Options(CheckboxOptions().Tooltip(
            "Allows Deku Link to glide longer, no longer dropping after a certain distance."));
    AddWidget(path, "No Clip", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.NoClip")
        .Options(CheckboxOptions().Tooltip("Allows Link to phase through collision."));
    AddWidget(path, "Unbreakable Razor Sword", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.UnbreakableRazorSword")
        .Options(CheckboxOptions().Tooltip("Allows to Razor Sword to be used indefinitely without dulling its blade."));
    AddWidget(path, "Unrestricted Items", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.UnrestrictedItems")
        .Options(CheckboxOptions().Tooltip("Allows all Forms to use all Items."));
    AddWidget(path, "Hookshot Anywhere", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.HookshotAnywhere")
        .Options(CheckboxOptions().Tooltip("Allows most surfaces to be hookshot-able."));
    AddWidget(path, "Moon Jump on L", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.MoonJumpOnL")
        .Options(CheckboxOptions().Tooltip("Holding L makes you float into the air."));
    AddWidget(path, "Elegy of Emptiness Anywhere", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.ElegyAnywhere")
        .Options(CheckboxOptions().Tooltip("Allows Elegy of Emptiness outside of Ikana."));
    AddWidget(path, "Climb Anywhere", WIDGET_CVAR_CHECKBOX)
        .CVar("gCheats.ClimbAnywhere")
        .Options(CheckboxOptions().Tooltip("Allows climbing on most walls regardless of vines."));
    AddWidget(path, "Stop Time in Dungeons", WIDGET_CVAR_COMBOBOX)
        .CVar("gCheats.TempleTimeStop")
        .Options(
            ComboboxOptions()
                .Tooltip("Stops time from advancing in selected areas. Requires a room change to update.\n\n"
                         "- Off: Vanilla behaviour.\n"
                         "- Temples: Stops time in Woodfall, Snowhead, Great Bay, and Stone Tower Temples.\n"
                         "- Temples + Mini Dungeons: In addition to the above temples, stops time in both Spider "
                         "Houses, Pirate's Fortress, Beneath the Well, Ancient Castle of Ikana, and Secret Shrine.")
                .ComboVec(&timeStopOptions));

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Speed Modifier", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Speed Modifier Mode", WIDGET_CVAR_COMBOBOX)
        .CVar("gCheats.SpeedModifier.Mode")
        .Options(ComboboxOptions().ComboVec(&speedModifierModeOptions).LabelPosition(LabelPosition::None));
    AddWidget(path, "Multiplier:", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gCheats.SpeedModifier.Value")
        .PreFunc([](WidgetInfo& info) { info.isHidden = !CVarGetInteger("gCheats.SpeedModifier.Mode", 0); })
        .Options(FloatSliderOptions().Format("%.1fx").Min(0.0f).Max(6.0f).DefaultValue(1.0f));
    AddWidget(path, "Button Combination:", WIDGET_CVAR_BTN_SELECTOR)
        .CVar("gCheats.SpeedModifier.Btn")
        .Options(BtnSelectorOptions().DefaultValue(BTN_CUSTOM_MODIFIER1))
        .PreFunc([](WidgetInfo& info) { info.isHidden = CVarGetInteger("gCheats.SpeedModifier.Mode", 0) < 2; });

    //// Gameplay Enhancements
    path = { "Enhancements", "Gameplay", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Gameplay", 3);
    AddWidget(path, "Player", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fast Deku Flower Launch", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Player.FastFlowerLaunch")
        .Options(CheckboxOptions().Tooltip("Speeds up the time it takes to be able to get maximum height from"
                                           "launching out of a Deku Flower."));
    AddWidget(path, "Infinite Deku Hopping", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Player.InfiniteDekuHopping")
        .Options(CheckboxOptions().Tooltip("Allows Deku Link to hop indefinitely in water without drowning. This also "
                                           "prevents the velocity loss while in the air."));
    AddWidget(path, "Instant Putaway", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Player.InstantPutaway")
        .Options(CheckboxOptions().Tooltip("Allows Link to instantly puts away held item without waiting."));
    AddWidget(path, "Unsheathe Sword Without Slashing", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Player.UnsheatheWithoutSlashing")
        .Options(CheckboxOptions().Tooltip("Allows Link to unsheathe sword without slashing automatically."));
    AddWidget(path, "Fierce Deity Putaway", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Player.FierceDeityPutaway")
        .Options(CheckboxOptions().Tooltip("Allows Fierce Deity Link to put away his sword."));
    AddWidget(path, "Climb speed", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Player.ClimbSpeed")
        .Options(IntSliderOptions()
                     .Tooltip("Increases the speed at which Link climbs vines and ladders.")
                     .Min(1)
                     .Max(5)
                     .DefaultValue(1));
    AddWidget(path, "Faster Push/Pull", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Player.FasterPushAndPull")
        .Options(CheckboxOptions().Tooltip("Speeds up the time it takes to push/pull various objects."));
    AddWidget(path, "Prevent Diving Over Water", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Player.PreventDiveOverWater")
        .Options(CheckboxOptions().Tooltip("Prevents Link from automatically diving over bodies of water."));
    AddWidget(path, "Underwater Ocarina", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Player.UnderwaterOcarina")
        .Options(CheckboxOptions().Tooltip("Allows Zora to use the Ocarina of Time when grounded underwater."));
    AddWidget(path, "Manual Jump", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Player.ManualJump")
        .Options(CheckboxOptions().Tooltip("Z + A to Jump and B while midair to Jump Attack."));
    AddWidget(path, "Dash After Roll", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Player.DashAfterRoll")
        .Options(ComboboxOptions()
                     .Tooltip("Keep holding A after a roll to dash.\n\n"
                              "- On: Dashing runs at Bunny Hood speed, wearing the bunny hood adds nothing extra.\n"
                              "- Stack with Bunny Hood: Dashing while wearing the Bunny Hood combines both boosts to "
                              "run even faster.")
                     .ComboVec(&dashAfterRollOptions));
    AddWidget(path, "Dpad Equips", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Dpad.DpadEquips")
        .Options(CheckboxOptions().Tooltip("Allows you to equip items to your D-pad."));
    AddWidget(path, "Unequip Items", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Equipment.ItemUnequip")
        .Options(CheckboxOptions().Tooltip("In the pause menu, press the same C-button or D-pad button an item is "
                                           "equipped to in order to unequip it."));
    AddWidget(path, "Fast Magic Arrow Equip Animation", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Equipment.MagicArrowEquipSpeed")
        .Options(CheckboxOptions().Tooltip("Removes the animation for equipping Magic Arrows."));
    AddWidget(path, "Instant Fin Boomerangs Recall", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.PlayerActions.InstantRecall")
        .Options(CheckboxOptions().Tooltip(
            "Pressing B will instantly recall the fin boomerang back to Zora Link after they are thrown."));
    AddWidget(path, "Two-Handed Sword Spin Attack", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Equipment.TwoHandedSwordSpinAttack")
        .Options(CheckboxOptions().Tooltip(
            "Enables magic spin attacks for the Fierce Deity Sword and Great Fairy's Sword."));
    AddWidget(path, "Better Picto Message", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Equipment.BetterPictoMessage")
        .Options(
            CheckboxOptions().Tooltip("Inform the player what target if any is being captured in the pictograph."));
    AddWidget(path, "Picto Box on C-Up", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Items.PictoBoxOnCUp")
        .Options(CheckboxOptions().Tooltip(
            "Press C-Up to activate the Pictograph Box once acquired, without needing to equip it to a C-button."));
    AddWidget(path, "Arrow Type Cycling", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.PlayerActions.ArrowCycle")
        .Options(CheckboxOptions().Tooltip(
            "While aiming the bow, use R to cycle between Normal, Fire, Ice and Light arrows."));
    AddWidget(path, "Invert Shield Y Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Equipment.InvertShieldY")
        .Options(CheckboxOptions().Tooltip(
            "Invert the Y axis while holding the shield so that it moves up with the left stick."));
    AddWidget(path, "Great Fairy Sword B-Button Attack", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Equipment.GreatFairySwordBButton")
        .Options(CheckboxOptions().Tooltip(
            "When the Great Fairy's Sword is held, pressing B attacks with it instead of drawing "
            "your equipped sword. The sword can still be put away with A as normal."));
    AddWidget(path, "Invert Zora Swim Y Axis", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Player.InvertZoraSwimY")
        .Options(CheckboxOptions().DefaultValue(true).Tooltip(
            "Invert the Y axis while swimming as Zora with the left stick."));

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Modes", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Play as Kafei", WIDGET_CVAR_CHECKBOX)
        .CVar("gModes.PlayAsKafei")
        .Options(CheckboxOptions().Tooltip("Requires scene reload to take effect."));
    AddWidget(path, "Hyrule Warriors Styled Link", WIDGET_CVAR_CHECKBOX)
        .CVar("gModes.HyruleWarriorsStyledLink")
        .Options(CheckboxOptions().Tooltip(
            "When acquired, places the Keaton and Fierce Deity masks on Link similarly to how he "
            "wears them in Hyrule Warriors."));
    AddWidget(path, "Time Moves when you Move", WIDGET_CVAR_CHECKBOX)
        .CVar("gModes.TimeMovesWhenYouMove")
        .Options(CheckboxOptions().Tooltip("Time only moves when Link is not standing still."));
    AddWidget(path, "Mirrored World", WIDGET_CVAR_COMBOBOX)
        .CVar("gModes.MirroredWorld.Mode")
        .Options(
            ComboboxOptions()
                .DefaultIndex(MIRRORED_WORLD_OFF)
                .Tooltip(
                    "Mirrors the world horizontally:\n\n"
                    " - Always: Always mirror the world.\n"
                    " - Random: Randomly decide to mirror the world on each scene change.\n"
                    " - Random (Seeded): Scenes are mirrored based on the current randomizer seed/file.\n"
                    " - Dungeons (Temples): Mirror the world in the four temples.\n"
                    " - Dungeons (Spider Houses): Mirror the world in the two Spider Houses.\n"
                    " - Dungeons (All): Mirror the world in the four temples and the two Spider Houses.\n"
                    " - Dungeons Random: Randomly decide to mirror the world in Dungeons.\n"
                    " - Dungeons Random (Seeded): Dungeons are mirrored based on the current randomizer seed/file.")
                .ComboVec(&mirroredWorldModes));
    AddWidget(path, "Fix Inverted Stone Tower Temple", WIDGET_CVAR_CHECKBOX)
        .CVar("gModes.MirroredWorld.StoneTowerTempleFix")
        .Options(CheckboxOptions().Tooltip(
            "Mirrors (or unmirrors) the inverted Stone Tower Temple to make its layout consistent."));
    AddWidget(path, "Other", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Milk Run Reward Options", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Minigames.CremiaHugs")
        .Options(ComboboxOptions()
                     .Tooltip("Choose what reward you get for winning the Milk Run minigame after the first time. \n"
                              "-Vanilla: Reward is Random\n"
                              "-Hug: Get the hugging cutscene\n"
                              "-Rupee: Get the rupee reward")
                     .ComboVec(&cremiaRewardOptions));
    AddWidget(path, "Ammo Buyback Options", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Items.AmmoBuyback")
        .Options(ComboboxOptions()
                     .Tooltip("Choose whether to allow selling ammo items (Arrows, Bombs, Bombchus, Deku Sticks, Deku "
                              "Nuts, Magic Beans, Powder Keg) "
                              "to the Curiosity Shop owner for Rupees.\n"
                              "-Vanilla: Ammo items cannot be sold\n"
                              "-Full Price: Sell at full value\n"
                              "-Half Price: Sell at half value (rounded up)"
                              "Arrows will always be sold back at Full Price.")
                     .ComboVec(&ammoBuybackOptions));
    AddWidget(path, "Extended Projectile Interaction Distance", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Gameplay.ExtendedProjectileInteractionDistance")
        .Options(CheckboxOptions().Tooltip(
            "Allows projectiles and explosions to hit breakable objects at a distance matching your "
            "Increase Actor Draw Distance setting.\n\n"
            "Does not affect pickup ranges, talk prompts, or physical body collision."));
    AddWidget(path, "Curiosity Shop Refills", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Shops.CuriosityShopRefills")
        .Options(CheckboxOptions().Tooltip(
            "Adds refillable bottles to the Curiosity Shop after completing certain prerequisites:\n"
            "- Seahorse: After obtaining a Bottle, Zora Mask, Pictograph Box & (Rando Only) Swim Ability\n"
            "- Gold Dust: After obtaining the Gold Dust bottle\n"
            "- Chateau Romani: After obtaining Chateau Romani"));
    AddWidget(path, "Accessibility", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Disable Screen Flash for Enemy Kills", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.A11y.NoScreenFlashForEnemyKill")
        .Options(CheckboxOptions().Tooltip("Disables the white screen flash on enemy kill."));
    AddWidget(path, "Disable Final Day Quakes", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.A11y.NoFinalDayQuakes")
        .Options(CheckboxOptions().Tooltip("Earthquakes will not occur on the final day."));
    AddWidget(path, "Disable Screen Distortion", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.A11y.NoScreenDistortion")
        .Options(CheckboxOptions().Tooltip(
            "Disables the wobbling/warping of the screen while swimming underwater, and other various sources"));
    AddWidget(path, "Bow Reticle", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.BowReticle")
        .Options(CheckboxOptions().Tooltip("Gives the bow a reticle when you draw an arrow."));
    AddWidget(path, "Mark Shooting Gallery Octoroks", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Minigames.MarkShootingGalleryOctoroks")
        .Options(CheckboxOptions().Tooltip("Places markers on the Town Shooting Gallery Octoroks, indicating whether "
                                           "they should be hit."));

    AddWidget(path, "Explosives", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Extra Powder Kegs", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Items.ExtraPowderKegs")
        .Options(CheckboxOptions().Tooltip(
            "Allows carrying up to 3 Powder Kegs at once instead of the vanilla limit of 1."));
    AddWidget(path, "Bomb Arrows", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Equipment.BombArrows")
        .Options(CheckboxOptions().Tooltip(
            "Allows equipping Bomb Arrows by equipping Bombs onto a bow button in the pause menu."));
    AddWidget(path, "Remote Bombchu Control", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.PlayerActions.RemoteBombchu")
        .Options(CheckboxOptions().Tooltip(
            "Allows you to control the direction of the Bombchu while it is moving. Press B to detonate. Press A to "
            "stop controlling the Bombchu."));
    AddWidget(path, "Bombchu Drops", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Equipment.ChuDrops")
        .Options(
            CheckboxOptions().Tooltip("When a bomb drop is spawned, it has a 50% chance to be a Bombchu instead."));
    AddWidget(path, "Remove Explosive Limit", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Items.RemoveExplosiveLimit")
        .Options(CheckboxOptions().Tooltip("Removes the cap of 3 active explosives being deployed at once."));

    path.column = SECTION_COLUMN_3;
    AddWidget(path, "Saving", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "3rd Save File Slot", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Saving.FileSlot3")
        .Options(CheckboxOptions().Tooltip("Adds a 3rd file slot that can be used for saves").DefaultValue(true));
    AddWidget(path, "New File Setup Steps", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Saving.NewFileSetup")
        .Options(CheckboxOptions()
                     .Tooltip("After picking an empty file, asks whether it should be a randomizer file and lets "
                              "you apply one of your loaded presets before entering a name.")
                     .DefaultValue(true));
    AddWidget(path, "Persistent Owl Saves", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Saving.PersistentOwlSaves")
        .Options(CheckboxOptions().Tooltip("Continuing a save will not remove the owl save. Playing Song of "
                                           "Time, allowing the moon to crash or finishing the "
                                           "game will remove the owl save and become the new last save."));
    AddWidget(path, "Pause Menu Save", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Saving.PauseSave")
        .Options(CheckboxOptions().Tooltip(
            "Re-introduce the pause menu save system. Pressing B in the pause menu will give you the "
            "option to create a persistent Owl Save from your current location.\n\nWhen loading back "
            "into the game, you will be placed either at the entrance of the dungeon you saved in, or "
            "in South Clock Town, unless Remember Save Location is enabled."));
    AddWidget(path, "Remember Save Location", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Saving.RememberSaveLocation")
        .Options(CheckboxOptions().Tooltip("When loading a save, places Link at the last entrance he went through."));
    AddWidget(path, "Autosave", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Saving.Autosave")
        .Options(CheckboxOptions().Tooltip(
            "Automatically create a persistent Owl Save on the chosen interval.\n\nWhen loading "
            "back into the game, you will be placed either at the entrance of the dungeon you "
            "saved in, or in South Clock Town, unless Remember Save Location is enabled."));
    AddWidget(path, "Autosave Interval: %d minutes", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Saving.AutosaveInterval")
        .PreFunc([](WidgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_AUTO_SAVE_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_AUTO_SAVE_OFF);
            }
        })
        .Options(IntSliderOptions().Tooltip("Sets the interval between Autosaves.").Min(1).Max(60).DefaultValue(5));
    AddWidget(path, "Time Cycle", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Save Game on Moon Crash", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.SaveOnMoonCrash")
        .Options(CheckboxOptions().Tooltip("Moon Crashes will save game data similar to the Song of Time,\n"
                                           "instead of wiping owl save data. Automatically enabled in glitchless \n"
                                           "rando seeds."));
    AddWidget(path, "Do not reset Bottle content", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.DoNotResetBottleContent")
        .Options(CheckboxOptions().Tooltip("Playing the Song of Time will not reset the bottles' content."));
    AddWidget(path, "Do not reset Consumables", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.DoNotResetConsumables")
        .Options(CheckboxOptions().Tooltip("Playing the Song of Time will not reset the consumables."));
    AddWidget(path, "Do not reset Razor Sword", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.DoNotResetRazorSword")
        .Options(CheckboxOptions().Tooltip("Playing the Song of Time will not reset the Sword back to Kokiri Sword."));
    AddWidget(path, "Do not reset Rupees", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.DoNotResetRupees")
        .Options(CheckboxOptions().Tooltip("Playing the Song of Time will not reset the your rupees."));
    AddWidget(path, "Do not reset Time Speed", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.DoNotResetTimeSpeed")
        .Options(CheckboxOptions().Tooltip(
            "Playing the Song of Time will not reset the current time speed set by Inverted Song of Time."));
    AddWidget(path, "Do not reset Chateau status", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.DoNotResetChateau")
        .Options(CheckboxOptions().Tooltip(
            "Playing the Song of Time will not reset the infinite magic status granted by Chateau Romani."));
    AddWidget(path, "Do not reset Scarecrow's Song", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.DoNotResetScarecrowSong")
        .Options(CheckboxOptions().Tooltip("Playing the Song of Time will not reset the Scarecrow's Song."));
    AddWidget(path, "Keep Express Mail", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.KeepExpressMail")
        .Options(CheckboxOptions().Tooltip(
            "Allows the player to keep the Express Mail in their inventory after delivering it "
            "the first time, so that both deliveries can be done within one cycle."));
    AddWidget(path, "Stop Oceanside Spider House squatter", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.StopOceansideSpiderHouseSquatter")
        .Options(
            CheckboxOptions().Tooltip("The Oceanside Spider House squatter will not move in until the player interacts "
                                      "with him. Forced on for randomizers."));
    AddWidget(path, "Oceanside wallet any day", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.OceansideWalletAnyDay")
        .Options(CheckboxOptions().Tooltip("Allows the wallet reward to be collected on any day."));
    AddWidget(path, "Tingle Always in Clock Town", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cycle.TingleAlwaysInClockTown")
        .Options(CheckboxOptions().Tooltip("Tingle will always appear in North Clock Town, not just during the day."));

    //// Graphics Enhancements
    path = { "Enhancements", "Graphics", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Graphics", 3);
    AddWidget(path, "Clock", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Clock Type", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Graphics.ClockType")
        .Options(ComboboxOptions()
                     .Tooltip("Swaps between Graphical and Text only Clock types.")
                     .ComboVec(&clockTypeOptions));
    AddWidget(path, "24 Hours Clock", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.24HoursClock")
        .Options(CheckboxOptions().Tooltip("Changes from a 12 Hour to a 24 Hour Clock."));
    AddWidget(path, "Mods", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Use Alternate Assets", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Mods.AlternateAssets")
        .Options(CheckboxOptions().Tooltip(
            "Toggle between standard assets and alternate assets. Usually mods will indicate if "
            "this setting has to be used or not."));
    AddWidget(path, "Disable Bomb Billboarding", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Mods.DisableBombBillboarding")
        .Options(CheckboxOptions().Tooltip(
            "Disables bombs always rotating to face the camera. To be used in conjunction with mods that want "
            "to replace bombs with 3D objects."));
    AddWidget(path, "Disable Grotto Fixed Rotation", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Mods.DisableGrottoRotation")
        .Options(CheckboxOptions().Tooltip(
            "Disables Grottos rotating with the Camera. To be used in conjuction with mods that want to "
            "replace grottos with 3D objects."));
    AddWidget(path, "Motion Blur", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Motion Blur Mode", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Graphics.MotionBlur.Mode")
        .Options(ComboboxOptions()
                     .Tooltip("Selects the Mode for Motion Blur.")
                     .LabelPosition(LabelPosition::None)
                     .ComboVec(&motionBlurOptions));
    AddWidget(path, "Interpolate", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.MotionBlur.Interpolate")
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value == MOTION_BLUR_ALWAYS_OFF;
        })
        .Options(CheckboxOptions().Tooltip(
            "Change motion blur capture to also happen on interpolated frames instead of only on game frames.\n"
            "This notably reduces the overall motion blur strength but smooths out the trails."));
    AddWidget(path, "On/Off", WIDGET_CHECKBOX)
        .ValuePointer((bool*)&R_MOTION_BLUR_ENABLED)
        .PreFunc([](WidgetInfo& info) {
            info.valuePointer = (bool*)&R_MOTION_BLUR_ENABLED;
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value != MOTION_BLUR_DYNAMIC;
        });
    AddWidget(path, "Strength", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Graphics.MotionBlur.Strength")
        .Options(IntSliderOptions().Tooltip("Motion Blur strength.").Min(0).Max(255).DefaultValue(180))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value != MOTION_BLUR_ALWAYS_ON;
        });
    AddWidget(path, "Strength", WIDGET_SLIDER_INT)
        .Options(IntSliderOptions().Tooltip("Motion Blur strength.").Min(0).Max(255).DefaultValue(180))
        .ValuePointer(&motionBlurStrength)
        .Callback([](WidgetInfo& info) { R_MOTION_BLUR_ALPHA = motionBlurStrength; })
        .PreFunc([](WidgetInfo& info) {
            motionBlurStrength = R_MOTION_BLUR_ALPHA;
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_MODE).value != MOTION_BLUR_DYNAMIC ||
                            mBenMenu->disabledMap.at(DISABLE_FOR_MOTION_BLUR_OFF).active;
        });

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Other", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "3D Item Drops", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.3DItemDrops")
        .Options(CheckboxOptions().Tooltip("Makes item drops 3D"));
    AddWidget(path, "Authentic Logo", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.AuthenticLogo")
        .Options(CheckboxOptions().Tooltip("Hide the game version and build details and display the authentic "
                                           "model and texture on the boot logo start screen."));
    AddWidget(path, "Disable Black Bar Letterboxes", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.DisableBlackBars")
        .Options(CheckboxOptions().Tooltip(
            "Disables Black Bar Letterboxes during cutscenes and Z-targeting.\nNote: There may be "
            "minor visual glitches that were covered up by the black bars.\nPlease disable this "
            "setting before reporting a bug."));
    AddWidget(path, "Enemy Health Bars", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.EnemyHealthBars")
        .Options(CheckboxOptions().Tooltip("Renders a health bar for enemies and bosses when Z-targeted."));
    AddWidget(path, "Fix Scene Geometry Seams", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.FixSceneGeometrySeams")
        .Callback([](WidgetInfo& info) { GfxPatcher_ApplyGeometryIssuePatches(); });
    AddWidget(path, "Disable Scene Geometry Distance Check", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.DisableSceneGeometryDistanceCheck")
        .Callback([](WidgetInfo& info) { GfxPatcher_ApplyGeometryIssuePatches(); })
        .Options(CheckboxOptions().Tooltip(
            "Disables the distance check for scene geometry, allowing it to be drawn no matter how far "
            "away it is from the player. This may have unintended side effects."));
    AddWidget(path, "Widescreen Actor Culling", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.ActorCullingAccountsForWidescreen")
        .Options(CheckboxOptions().Tooltip("Adjusts the culling planes to account for widescreen resolutions. "
                                           "This may have unintended side effects."));
    AddWidget(path, "Increase Actor Draw Distance: %dx", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Graphics.IncreaseActorDrawDistance")
        .Options(IntSliderOptions()
                     .Tooltip("Increase the range in which Actors are drawn. This may have unintended side effects.")
                     .Min(1)
                     .Max(5)
                     .DefaultValue(1));
    AddWidget(path, "N64 Mode", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_LOW_RES_MODE)
        .Options(CheckboxOptions().Tooltip(
            "Sets the aspect ratio to 4:3 and lowers resolution to 240p, the N64's native resolution."));

    path = { "Enhancements", "Items/Songs", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Items/Songs", 3);
    // Mask Enhancements
    AddWidget(path, "Masks", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Equippable While Swimming", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Masks.EquipWhileSwimming")
        .Options(CheckboxOptions().Tooltip("Human Link can equip any non-transformation mask while swimming."));
    AddWidget(path, "Blast Mask has Powder Keg Force", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Masks.BlastMaskKeg")
        .Options(CheckboxOptions().Tooltip("Blast Mask can also destroy objects only the Powder Keg can."));
    AddWidget(path, "Fast Transformation", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Masks.FastTransformation")
        .Options(ComboboxOptions()
                     .Tooltip("Removes the delay when using transformation masks.")
                     .ComboVec(&fastTransformationOptions));
    AddWidget(path, "3DS Style Mask Equipping", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Masks.3DSMaskEquip")
        .Options(CheckboxOptions().Tooltip("Allows equipping masks while in other forms, returning you to human form "
                                           "with the mask immediately equipped, like in MM3D."));
    AddWidget(path, "Easy Mask Equip", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Masks.EasyMaskEquip")
        .Options(CheckboxOptions().Tooltip("In the pause menu, press A on any owned mask to put it on or take it off "
                                           "without assigning it to a button. Mask-specific restrictions still "
                                           "apply."));
    AddWidget(path, "Fierce Deity's Mask Anywhere", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Masks.FierceDeitysAnywhere")
        .Options(CheckboxOptions().Tooltip("Allow using Fierce Deity's mask outside of boss rooms."));
    AddWidget(path, "Persistent Bunny Hood", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Masks.PersistentBunnyHood.Enabled")
        .Options(CheckboxOptions().Tooltip(
            "Permanently toggle a speed boost from the bunny hood by pressing 'A' on it in the mask menu."));
    AddWidget(path, "Blast Mask Cooldown", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Masks.BlastMaskCooldown")
        .Options(FloatSliderOptions()
                     .Tooltip("Customize the Cooldown between Blast Mask usage. Default is 15.5 seconds.")
                     .Min(0.0f)
                     .Max(15.5f)
                     .Format("%.1f seconds")
                     .DefaultValue(15.5f));
    AddWidget(path, "Goron Rolling Ignores Magic", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Masks.GoronRollingIgnoresMagic")
        .Options(CheckboxOptions().Tooltip(
            "Goron rolling will use spikes even when Link doesn't have magic, and doesn't consume any."));
    AddWidget(path, "Goron Rolling Fast Spikes", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Masks.GoronRollingFastSpikes")
        .Options(CheckboxOptions().Tooltip("Speeds up the wind-up towards spiky rolling to be near instant."));
    AddWidget(path, "Goron Rolling Spikes Require Shield Button Press", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Masks.GoronRollingSpikesRequireShield")
        .Options(CheckboxOptions().Tooltip("Goron rolling will only use spikes if the Shield button is pressed, "
                                           "similar to Zora Link's swimming magic shield."));

    // Song Enhancements
    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Ocarina", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Better Song of Double Time", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Songs.BetterSongOfDoubleTime")
        .Options(CheckboxOptions().Tooltip(
            "When playing the Song of Double Time, you can now choose the exact time you want to go "
            "to, similar to the 3DS version.\n\n"
            "Holding Z allows decreasing the time adjustment factor, while holding R will increase the factor."));
    AddWidget(path, "Enable Sun's Song", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Songs.EnableSunsSong")
        .Options(CheckboxOptions().Tooltip(
            "Enables the partially implemented Sun's Song. RIGHT-DOWN-UP-RIGHT-DOWN-UP to play it. "
            "This song will make time move very fast until either Link moves to a different scene, "
            "or when the time switches to a new time period."));
    AddWidget(path, "D-pad Ocarina", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Playback.DpadOcarina")
        .Options(CheckboxOptions().Tooltip("Enables using the D-pad for Ocarina playback."));
    AddWidget(path, "Right Stick Ocarina", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Playback.RightStickOcarina")
        .Options(CheckboxOptions().Tooltip("Enables using the Right Stick for Ocarina playback."));
    AddWidget(path, "Song Items", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Songs.SongItems")
        .Options(CheckboxOptions().Tooltip("Equip songs to C/D-Pad buttons from the Quest Status page. "
                                           "Songs auto-play when used, skipping manual note input."));
    AddWidget(path, "Pause Owl Warp", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Songs.PauseOwlWarp")
        .Options(CheckboxOptions().Tooltip(
            "Allows warping to registered Owl Statues from the pause menu map screen. "
            "Requires that you can play Song of Soaring normally.\n\n"
            "Accounts for Index-Warp being active, by presenting all valid warps for the registered "
            "map points. Great Bay Coast warp is always given for index 0 warp as a convenience."));
    AddWidget(path, "Better Owl Warp Menu", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Songs.BetterOwlWarpMenu")
        .Options(CheckboxOptions().Tooltip(
            "Makes cursor movement conform more to Control Stick direction when choosing an Owl Statue to warp to."));
    AddWidget(path, "Zora Eggs For Bossa Nova", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Songs.ZoraEggCount")
        .Options(IntSliderOptions()
                     .Tooltip("The number of eggs required to unlock New Wave Bossa Nova.")
                     .Min(1)
                     .Max(7)
                     .DefaultValue(7));
    AddWidget(path, "Prevent Dropped Ocarina Inputs", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Playback.NoDropOcarinaInput")
        .Options(CheckboxOptions().Tooltip("Prevent dropping inputs when playing the Ocarina quickly."));
    AddWidget(path, "Skip Scarecrow Song", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Playback.SkipScarecrowSong")
        .Options(CheckboxOptions().Tooltip("Pierre appears when the Ocarina is pulled out."));
    AddWidget(path, "Faster Song Playback", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Songs.FasterSongPlayback")
        .Options(CheckboxOptions().Tooltip("Speeds up the playback of songs."));
    AddWidget(path, "Skip Song of Time cutscenes", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Songs.SkipSoTCutscenes")
        .Options(CheckboxOptions().Tooltip("Skips the cutscenes when playing any of the Song of Time songs."));
    AddWidget(path, "Skip Soaring cutscene", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Songs.SkipSoaringCutscene")
        .Options(CheckboxOptions().Tooltip("Skips the cutscene when using the Song of Soaring to warp."));

    // Item Enhancements
    path.column = SECTION_COLUMN_3;
    AddWidget(path, "Items", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Color Pictograph", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Items.ColorPictograph")
        .Options(CheckboxOptions().Tooltip("Will take and display pictographs in color."));

    // Time Savers
    path = { "Enhancements", "Time Savers", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Time Savers", 3);
    // Cutscene Skips
    AddWidget(path, "Cutscenes", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Hide Title Cards", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.HideTitleCards")
        .Options(CheckboxOptions().Tooltip("Hides Title Cards when entering areas."));
    AddWidget(path, "Skip One Point Cutscenes", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.SkipOnePointCutscenes")
        .Options(CheckboxOptions().Tooltip(
            "Skips freezing Link to focus on various events like chest spawning, door unlocking, switch pressed, etc"));
    AddWidget(path, "Skip Entrance Cutscenes", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.SkipEntranceCutscenes")
        .Options(CheckboxOptions().Tooltip("Skip cutscenes that occur when first entering a new area."));
    AddWidget(path, "Skip to File Select", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.SkipToFileSelect")
        .Options(CheckboxOptions().Tooltip(
            "Skip the opening title sequence and go straight to the file select menu after boot.\n\n"
            "Note: Takes priority over debug warp points with [Boot] enabled."));
    AddWidget(path, "Skip Intro Sequence", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.SkipIntroSequence")
        .Options(CheckboxOptions().Tooltip(
            "When starting a game you will be taken straight to South Clock Town as Deku Link."));
    AddWidget(path, "Skip First Cycle", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.SkipFirstCycle")
        .PreFunc([](WidgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_INTRO_SKIP_OFF).active) {
                info.activeDisables.push_back(DISABLE_FOR_INTRO_SKIP_OFF);
            }
        })
        .Options(CheckboxOptions().Tooltip(
            "When starting a game you will be taken straight to South Clock Town as Human Link "
            "with Deku Mask, Ocarina, Song of Time, and Song of Healing."));
    AddWidget(path, "Skip Story Cutscenes", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.SkipStoryCutscenes")
        .Options(CheckboxOptions().Tooltip("This skips many of the cutscenes associated with the main story."));
    AddWidget(path, "Skip Misc Interactions", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.SkipMiscInteractions")
        .Options(CheckboxOptions().Tooltip("This skips many minor cutscenes and interactions."));
    AddWidget(path, "Skip Enemy Cutscenes", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.SkipEnemyCutscenes")
        .Options(CheckboxOptions().Tooltip("Skips cutscenes specific to enemies and boss battles."));
    AddWidget(path, "Skip Item Get Cutscene", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Cutscenes.SkipGetItemCutscenes")
        .Options(ComboboxOptions()
                     .Tooltip("Note: This only works in Randomizer currently.")
                     .ComboVec(&skipGetItemCutscenesOptions));

    // Dialogue Enhancements
    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Dialogue", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fast Bank Selection", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Dialogue.FastBankSelection")
        .Options(CheckboxOptions().Tooltip(
            "Pressing the Z or R buttons while the Deposit/Withdrawal Rupees dialogue is open will set "
            "the Rupees to Links current Rupees or 0 respectively."));
    AddWidget(path, "Fast Text", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Dialogue.FastText")
        .Options(
            CheckboxOptions().Tooltip("Speeds up text rendering, and enables holding of B progress to next message."));
    AddWidget(path, "Auto Bombers' Code", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Dialogue.AutoBombersCode")
        .Options(CheckboxOptions().Tooltip("Automatically fill in the Bombers' code once you've got the notebook."));
    AddWidget(path, "Skip Bottle Pickup Messages", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Dialogue.SkipBottlePickupMessages")
        .Options(CheckboxOptions().Tooltip("Skip pickup messages for bottle swipes."));
    AddWidget(path, "Auto Advance Ending Text", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Cutscenes.AutoAdvanceEndingText")
        .Options(CheckboxOptions().Tooltip(
            "After completing the game, textboxes shown on the way to the credits advance on their own "
            "once they've been up long enough to read, so the ending plays out without controller input."));

    path.column = SECTION_COLUMN_3;
    AddWidget(path, "Other", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Swamp Boat Timesaver", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Timesavers.SwampBoatSpeed")
        .Options(CheckboxOptions().Tooltip("Hold Z to speed up the boat ride in through the Swamp."));
    AddWidget(path, "Shooting Gallery Both Rewards", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Timesavers.GalleryTwofer")
        .Options(CheckboxOptions().Tooltip("When getting a perfect score at the Shooting Gallery, receive both rewards "
                                           "back to back instead of having to play twice."));
    AddWidget(path, "Fast Marine Lab Fish", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Timesavers.MarineLabHP")
        .Options(CheckboxOptions().Tooltip("Only requires a single fish to be fed for the Piece of Heart to spawn. "
                                           "Requires a Scene Reload to take effect."));
    AddWidget(path, "Fast Dampe Flame Digging", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Timesavers.DampeDiggingSkip")
        .Options(CheckboxOptions().Tooltip("Only requires digging up one flame to spawn the big poe."));
    AddWidget(path, "Always Show Shrine of Truth Feathers", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Timesavers.AlwaysShowShrineFeathers")
        .Options(CheckboxOptions().Tooltip("Always reveals the feather-marked path to the Shrine of Truth."));
    AddWidget(path, "Fast Chests", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Timesavers.FastChests")
        .Options(CheckboxOptions().Tooltip("Uses the quick kick animation for all chests in vanilla gameplay."));
    AddWidget(path, "Faster Scene Transitions", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Timesavers.FasterSceneTransitions")
        .Options(CheckboxOptions().Tooltip("Fade in and out more quickly when moving between areas."));
    AddWidget(path, "Skip Powder Keg Certification", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Timesavers.PowderKegCertification")
        .Options(CheckboxOptions().Tooltip(
            "Skips requiring to take the Powder Keg Test before being given the Certification."));
    AddWidget(path, "Skip Ballad of Windfish", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Timesavers.SkipBalladOfWindfish")
        .Options(CheckboxOptions().Tooltip(
            "Play the complete Ballad after playing in one form if you have all three transformation masks."));
    AddWidget(path, "Auto Bank Deposit", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Timesavers.AutoBankDeposit")
        .Options(CheckboxOptions().Tooltip(
            "Automatically deposits excess Rupees into your bank account when your wallet is full. "
            "Deposits stop when the bank reaches maximum capacity. "
            "Bank rewards are granted automatically. Notifications display deposit amount and new balance."));
    AddWidget(path, "Faster Rupee Accumulator", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Timesavers.FasterRupeeAccumulator")
        .Options(CheckboxOptions().Tooltip("Causes your Wallet to fill and empty faster when you gain or lose money."));
    AddWidget(path, "Faster Bottles", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Timesavers.FasterBottles")
        .Options(CheckboxOptions().Tooltip("Speeds up animation when using a bottle item."));

    // Fixes
    path = { "Enhancements", "Fixes", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Fixes", 3);
    AddWidget(path, "Fixes", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fix Console Crashes", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Fixes.ConsoleCrashes")
        .Options(CheckboxOptions()
                     .Tooltip("Fixes crashes that would typically happen on Console. "
                              "Disabling this option will simply soft reset 2Ship when encountering these "
                              "crashes instead of actually crashing the program.\n\n"
                              "Includes the following:\n"
                              "- HESS/Weirdshot crashes\n"
                              "- Action Swap crash without arrow ammo\n"
                              "- Owl Warp menu crash when moving the cursor with Index-Warp active\n"
                              "- Remote Hookshot Hookslide crashes when over voids in Great Bay Temple")
                     .DefaultValue(true));
    AddWidget(path, "Fix Button Env Color", WIDGET_CVAR_CHECKBOX)
        .CVar("gFixes.FixButtonEnvColor")
        .Options(CheckboxOptions().Tooltip(
            "Fixes a missing gDPSetEnvColor, which causes ammo counts and B button "
            "action labels to be the wrong color prior to obtaining magic or other conditions."));
    AddWidget(path, "Fix Circle Shadow Streaks", WIDGET_CVAR_CHECKBOX)
        .CVar("gFixes.FixCircleShadowStreaks")
        .Options(CheckboxOptions().Tooltip("Fixes faint streaks on round actor shadows").DefaultValue(true));
    AddWidget(path, "Fix Epona stealing Sword", WIDGET_CVAR_CHECKBOX)
        .CVar("gFixes.FixEponaStealingSword")
        .Options(CheckboxOptions().Tooltip(
            "This fixes a bug where Epona can steal your sword when you mount her without a bow in your inventory."));
    AddWidget(path, "Fix Fierce Deity Z-Target movement", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Fixes.FierceDeityZTargetMovement")
        .Options(CheckboxOptions().Tooltip("Fixes Fierce Deity movement being choppy when Z-targeting."));
    AddWidget(path, "Fix Text Control Characters", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Fixes.ControlCharacters")
        .Options(CheckboxOptions().Tooltip("Fixes certain control characters not functioning properly "
                                           "depending on their position within the text."));
    AddWidget(path, "Fix Ikana Great Fairy Fountain Color", WIDGET_CVAR_CHECKBOX)
        .CVar("gFixes.FixIkanaGreatFairyFountainColor")
        .Options(CheckboxOptions().Tooltip(
            "Fixes a bug that results in the Ikana Great Fairy fountain looking green instead of "
            "yellow, this was fixed in the EU version."));
    AddWidget(path, "Fix Texture overflow OOB", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Fixes.FixTexturesOOB")
        .Options(CheckboxOptions()
                     .Tooltip("Fixes textures that normally overflow to be patched with the correct size or format.")
                     .DefaultValue(true))
        .Callback([](WidgetInfo& info) { GfxPatcher_ApplyOverflowTexturePatches(); });
    AddWidget(path, "Fix Completed Heart Container Audio", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Fixes.CompletedHeartContainerAudio")
        .Options(CheckboxOptions().Tooltip(
            "Fixes a bug that results in the wrong audio playing upon receiving a 4th piece of heart to "
            "fill a new heart container."));
    AddWidget(path, "Fix Deku Butler Shock Animation", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Fixes.DekuButlerFixShockLoopAnimation")
        .Options(CheckboxOptions().Tooltip(
            "Fixes a bug where the Deku Butler loops the incorrect animation in the cutscene that plays after "
            "freeing the Deku Princess."));

    // Restorations
    path = { "Enhancements", "Restorations", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Restorations", 3);
    AddWidget(path, "Restorations", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Constant Distance Backflips and Sidehops", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Restorations.ConstantFlipsHops")
        .Options(CheckboxOptions().Tooltip("Backflips and Sidehops travel a constant distance as they did in OoT."));
    AddWidget(path, "Power Crouch Stab", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Restorations.PowerCrouchStab")
        .Options(
            ComboboxOptions()
                .Tooltip("Crouch stabs will use the power of Link's previous melee attack.\n"
                         "- Patched: Crouch stabs will always do the same damage as a slash with your current weapon.\n"
                         "- Unpatched (JP): Glitch restored, and your initial damage is 0 (Can be useful to get ISG on "
                         "  pots).\n"
                         "- Unpatched (OoT): Glitch restored, and your initial damage is 1 (a Kokiri Sword slash).")
                .DefaultIndex(0)
                .ComboVec(&powerCrouchStabOptions));
    AddWidget(path, "Side Rolls", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Restorations.SideRoll")
        .Options(CheckboxOptions().Tooltip("Restores side rolling from OoT."));
    AddWidget(path, "Faster Swim", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Restorations.OoTFasterSwim")
        .Options(CheckboxOptions().Tooltip("Restores the ability to swim faster by spamming the B button, as in OoT."));
    AddWidget(path, "Tatl ISG", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Restorations.TatlISG")
        .Options(CheckboxOptions().Tooltip("Restores Navi ISG from OoT, but now with Tatl."));
    AddWidget(path, "Woodfall Mountain Appearance", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Restorations.WoodfallMountainAppearance")
        .Options(CheckboxOptions().Tooltip("Restores the appearance of Woodfall mountain to not look poisoned "
                                           "when viewed from Termina Field after clearing Woodfall Temple\n\n"
                                           "Requires a scene reload to take effect."));
    AddWidget(path, "JP Deku Palace Grottos", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Restorations.JPGrottos")
        .Options(CheckboxOptions().Tooltip("Restores the Deku Palace Grottos to their original Japanese layout."));
    AddWidget(path, "Day Transition Duration", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Restorations.DayTelopDuration")
        .Options(CheckboxOptions().Tooltip("Restores the day transition title card duration, which was longer on "
                                           "original hardware due to disguising load times."));
    AddWidget(path, "Bonk Collision", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Restorations.BonkCollision")
        .Options(
            CheckboxOptions().Tooltip("Corrects rolls to allow bonking trees near the end of the roll, as in OoT."));
    AddWidget(path, "Soil Patch Burrowed Bugs", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Restorations.SoilPatch")
        .Options(CheckboxOptions().Tooltip("Removes the cutscene lock when bugs burrow into a Skulltula soil patch, "
                                           "allowing the player to re-bottle them, as in OoT."));
    AddWidget(path, "Simulated Input Lag", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_SIMULATED_INPUT_LAG)
        .Options(IntSliderOptions()
                     .Tooltip("Buffers your inputs to be executed a specified amount of frames later.")
                     .Min(0)
                     .Max(6)
                     .DefaultValue(0));
    AddWidget(path, "Pause Buffer Input Window", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Restorations.PauseBufferWindow")
        .Options(IntSliderOptions()
                     .Tooltip("Amount of time in frames you have to buffer an input while unpausing the game. Original "
                              "hardware is around 20.")
                     .Min(0)
                     .Max(40)
                     .DefaultValue(0));

    // Difficulty Options
    path = { "Enhancements", "Difficulty Options", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Difficulty Options", 3);

    AddWidget(path, "Clock Town Minigames", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Bombers Hide-and-Seek Count", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.BombersHideAndSeek")
        .Options(IntSliderOptions()
                     .Tooltip("Sets the number of Bomber Kids you have to find to complete the hide-and-seek game.")
                     .Min(1)
                     .Max(5)
                     .DefaultValue(5));
    AddWidget(path, "Swordsman School Winning Score", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.SwordsmanSchoolScore")
        .Options(IntSliderOptions()
                     .Tooltip("Sets the score required to win the Swordsman School.")
                     .Min(1)
                     .Max(30)
                     .DefaultValue(30));
    AddWidget(path, "Honey & Darling Day 1 (Bombchus)", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.HoneyAndDarlingDay1")
        .Options(IntSliderOptions()
                     .Tooltip("Sets the score required to win the Honey & Darling minigame on Day 1.")
                     .Min(1)
                     .Max(8)
                     .DefaultValue(8));
    AddWidget(path, "Honey & Darling Day 2 (Bombs)", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.HoneyAndDarlingDay2")
        .Options(IntSliderOptions()
                     .Tooltip("Sets the score required to win the Honey & Darling minigame on Day 2.")
                     .Min(1)
                     .Max(8)
                     .DefaultValue(8));
    AddWidget(path, "Honey & Darling Day 3 (Bow)", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.HoneyAndDarlingDay3")
        .Options(IntSliderOptions()
                     .Tooltip("Sets the score required to win the Honey & Darling minigame on Day 3.")
                     .Min(1)
                     .Max(16)
                     .DefaultValue(16));
    AddWidget(path, "Treasure Chest Shop Maze", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Minigames.TreasureChestShopShowFullMaze")
        .Options(ComboboxOptions()
                     .Tooltip("Shows the entire maze layout in the Treasure Chest Shop minigame instead of only "
                              "revealing tiles near Link.\n"
                              "-Off: Only tiles near Link are revealed\n"
                              "-Full Height: The whole maze is raised to the same height\n"
                              "-Tiered: Tiles are raised higher the further back they are, so the front rows "
                              "don't hide the rest")
                     .ComboVec(&treasureChestShopMazeOptions));
    AddWidget(path, "Town Archery Perfect Score", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.TownArcheryScore")
        .Options(IntSliderOptions()
                     .Tooltip("Sets the score required to win the Town Archery minigame. Reaching this score will end "
                              "the minigame.")
                     .Min(1)
                     .Max(50)
                     .DefaultValue(50));
    AddWidget(path, "Randomize Shooting Gallery Octoroks", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Minigames.RandomizeShootingGalleryOctoroks")
        .Options(CheckboxOptions().Tooltip("Randomizes the positions of Octoroks in the Town Shooting Gallery minigame "
                                           "each time they appear."));

    AddWidget(path, "Southern Swamp Minigames", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Swamp Archery Perfect Score", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.SwampArcheryScore")
        .Options(IntSliderOptions()
                     .Tooltip("Sets the score required to win the Swamp Archery minigame, if this is changed it also "
                              "speeds up the final score counting.")
                     .Min(1000)
                     .Max(2180)
                     .DefaultValue(2180));
    AddWidget(path, "Swamp Boat Archery Target Score", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.BoatArcheryScore")
        .Options(IntSliderOptions()
                     .Tooltip("Sets the initial target score of the Swamp Boat Archery minigame. The target score "
                              "gets set the first time you play the minigame in each cycle.")
                     .Min(1)
                     .Max(50)
                     .DefaultValue(20));
    AddWidget(path, "Koume's Health", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.BoatArcheryHealth")
        .PreFunc([](WidgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_KOUME_INVINCIBLE).active) {
                info.activeDisables.push_back(DISABLE_FOR_KOUME_INVINCIBLE);
            }
        })
        .Options(IntSliderOptions()
                     .Tooltip("Sets Koume's health in the Swamp Boat Archery minigame. If Koume is hit this many "
                              "times, the minigame will end.")
                     .Min(1)
                     .Max(30)
                     .DefaultValue(10));
    AddWidget(path, "Invincible", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Minigames.BoatArcheryInvincible")
        .Options(CheckboxOptions().Tooltip("Koume's health does not decrease when hit."));

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Milk Road Minigames", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Romani Target Practice Winning Score", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.RomaniTargetPractice")
        .Options(IntSliderOptions()
                     .Tooltip("Sets the score required to win Romani's Target Practice.")
                     .Min(1)
                     .Max(10)
                     .DefaultValue(10));
    AddWidget(path, "Alien Speed: %.1fx", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Minigames.AlienSpeed")
        .PreFunc([](WidgetInfo& info) {
            if (mBenMenu->disabledMap.at(DISABLE_FOR_INVASION_SKIP).active) {
                info.activeDisables.push_back(DISABLE_FOR_INVASION_SKIP);
            }
        })
        .Options(FloatSliderOptions()
                     .Tooltip("Changes the movement speed of the aliens during the Romani Ranch invasion.")
                     .Format("%.1fx")
                     .Min(0.5f)
                     .Max(5.0f)
                     .Step(0.1f)
                     .DefaultValue(1.0f));
    AddWidget(path, "Skip Invasion", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Minigames.SkipRanchInvasion")
        .Options(CheckboxOptions().Tooltip("Link automatically succeeds in defending Romani Ranch from the invasion. "
                                           "In order for this to happen, you must be present at the ranch between "
                                           "2:30 and 5:15 AM. Automatically advances to 5:15 AM."));
    AddWidget(path, "Always Win Doggy Race", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Minigames.AlwaysWinDoggyRace")
        .Options(ComboboxOptions().Tooltip("Makes the Doggy Race easier to win.").ComboVec(&alwaysWinDoggyraceOptions));

    AddWidget(path, "Cucco Shack Cucco Count", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.CuccoShackCuccoCount")
        .Options(IntSliderOptions()
                     .Tooltip("Choose how many cuccos you need to raise to make Grog happy.")
                     .Min(1)
                     .Max(10)
                     .DefaultValue(10));
    AddWidget(path, "Skip Gorman Horse Race", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Minigames.SkipHorseRace")
        .Options(CheckboxOptions().Tooltip("Instantly win the Gorman Horse Race"));

    AddWidget(path, "Goron Minigames", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Goron Race", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.DifficultyOptions.GoronRace")
        .Options(ComboboxOptions()
                     .Tooltip("Set CPU behavior for the Goron Race:\n"
                              "- Vanilla: Gorons ahead of Link slow down, and Gorons behind speed up.\n"
                              "- Balanced: Gorons ahead of Link slow down, but Gorons behind do not speed up.\n"
                              "- Skip: Instantly win the race.\n")
                     .DefaultIndex(GoronRaceDifficultyOptions::GORON_RACE_DIFFICULTY_VANILLA)
                     .ComboVec(&goronRaceDifficultyOptions));

    AddWidget(path, "Great Bay Minigames", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Beaver Race Rings Collected", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.BeaverRaceRingsCollected")
        .Options(IntSliderOptions()
                     .Tooltip("Sets the number of rings required for both Beavers. If the slider is set to 20, the "
                              "first Beaver will require 20 rings, and the second Beaver will require 25 rings, which "
                              "are their vanilla values.")
                     .Min(1)
                     .Max(20)
                     .DefaultValue(20));
    AddWidget(path, "Skip Little Beaver Brother Races", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Minigames.SkipLittleBeaver")
        .Options(CheckboxOptions().Tooltip("Only Race the Older Beaver."));
    AddWidget(path, "Fisherman's Jumping Game Torch Time Limit", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Minigames.FishermanJumpingGame.TorchTimeLimit")
        .Options(ComboboxOptions()
                     .Tooltip("Sets the time limit for Link to jump to each torch in the Fisherman's Jumping Game."
                              "\n- Normal: 5 seconds."
                              "\n- Double: 10 seconds."
                              "\n- Infinity: No time limit.")
                     .DefaultIndex(TorchTimeLimitOptions::TORCH_TIME_LIMIT_NORMAL)
                     .ComboVec(&torchTimeLimitOptions));
    AddWidget(path, "Fisherman's Jumping Game Target Score", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Minigames.FishermanJumpingGame.TargetScore")
        .Options(IntSliderOptions()
                     .Tooltip("Sets the target score for the Fisherman's Jumping Game.")
                     .Min(1)
                     .Max(40)
                     .DefaultValue(20));
    AddWidget(path, "Win Jumping Game Early", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Minigames.FishermanJumpingGame.EarlyWin")
        .Options(CheckboxOptions().Tooltip(
            "Link wins the Fisherman's Jumping Game immediately upon reaching the target score."));

    path.column = SECTION_COLUMN_3;
    AddWidget(path, "Combat", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Hyper Enemies", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.DifficultyOptions.HyperEnemies")
        .Options(CheckboxOptions().Tooltip("Double the rate at which enemies are updated, making them more difficult"));
    AddWidget(path, "Boss Health Multiplier", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.DifficultyOptions.BossHealthMultiplier")
        .Options(ComboboxOptions()
                     .Tooltip("Multiply the health of all bosses. Requires a Scene Reload to take effect.")
                     .ComboMap(&bossHealthOptions));
    AddWidget(path, "Damage Multiplier", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.DifficultyOptions.DamageMultiplier")
        .Options(ComboboxOptions()
                     .Tooltip("Adjusts the amount of damage Link takes from all sources.")
                     .ComboMap(&damageMultiplierOptions));
    AddWidget(path, "Permanent Heart Loss", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.DifficultyOptions.PermanentHeartLoss")
        .Options(CheckboxOptions().Tooltip(
            "When you lose 4 quarters of a heart you will permanently lose that heart container.\n\nDisabling this "
            "after the fact will not restore any received heart containers."));
    AddWidget(path, "Delete File on Death", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.DifficultyOptions.DeleteFileOnDeath")
        .Options(CheckboxOptions().Tooltip("Dying will delete your file\n\n     " ICON_FA_EXCLAMATION_TRIANGLE
                                           " WARNING " ICON_FA_EXCLAMATION_TRIANGLE
                                           "\nTHIS IS NOT REVERSIBLE\nUSE AT YOUR OWN RISK!"));
    AddWidget(path, "Jinxed Timer: %d seconds", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.DifficultyOptions.JinxedTimer")
        .Options(
            IntSliderOptions()
                .Tooltip("Set the duration of the Jinxed effect. Setting it to 0 will prevent the effect entirely.")
                .Min(0)
                .Max(60)
                .DefaultValue(60));

    AddWidget(path, "Other", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Lower Bank Reward Thresholds", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.DifficultyOptions.LowerBankRewardThresholds")
        .Options(
            CheckboxOptions().Tooltip("Reduces the amount of rupees required to receive the rewards from the bank.\n"
                                      "From: 200 -> 1000 -> 5000\n"
                                      "To:   100 ->  500 -> 1000"));
    AddWidget(path, "Disable Takkuri Steal", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.DifficultyOptions.DisableTakkuriSteal")
        .Options(CheckboxOptions().Tooltip(
            "Prevents the Takkuri from stealing key items like bottles and swords. It may still steal "
            "other items."));
    AddWidget(path, "No Heart Drops", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.DifficultyOptions.NoHeartDrops")
        .Options(CheckboxOptions().Tooltip("Prevents spawning of any hearts or fairies."));
    AddWidget(path, "No Random Drops", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.DifficultyOptions.NoRandomDrops")
        .Options(CheckboxOptions().Tooltip("Prevents spawning of any collectibles."));
    AddWidget(path, "Always Find Rock Sirloin", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.DifficultyOptions.AlwaysFindRockSirloin")
        .Options(CheckboxOptions().Tooltip(
            "Breaking any pot on the Goron Shrine's chandelier will reveal the Rock Sirloin."));
    AddWidget(path, "Deku Guard Search Balls", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.DifficultyOptions.DekuGuardSearchBalls")
        .Options(
            ComboboxOptions()
                .Tooltip("Choose when to show the Deku Palace Guards' search balls:\n"
                         "- Never: Never show the search balls. This matches Majora's Mask 3D behaviour.\n"
                         "- Night Only: Only show the search balls at night. This matches original N64 behaviour.\n"
                         "- Always: Always show the search balls.")
                .DefaultIndex(DekuGuardSearchBallsOptions::DEKU_GUARD_SEARCH_BALLS_NIGHT_ONLY)
                .ComboVec(&dekuGuardSearchBallsOptions));
    AddWidget(path, "Gibdo Trade Sequence Options", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.DifficultyOptions.GibdoTradeSequence")
        .Options(
            ComboboxOptions()
                .Tooltip("Changes the way the Gibdo Trade Sequence works:\n"
                         "- Vanilla: Works normally.\n"
                         "- MM3D: Gibdos will only take one quantity of the item they request, as they do in MM3D. "
                         "  The Gibdo requesting a blue potion will also accept a red potion.\n"
                         "- No trade: Gibdos will vanish without taking items.")
                .DefaultIndex(GibdoTradeSequenceOptions::GIBDO_TRADE_SEQUENCE_VANILLA)
                .ComboVec(&gibdoTradeSequenceOptions));
    AddWidget(path, "Hidden Grottos Visibility", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.DifficultyOptions.HiddenGrottosVisibility")
        .Options(
            ComboboxOptions()
                .Tooltip(
                    "Enable visual markers for hidden grottos:\n"
                    "- Off: No visual markers for hidden grottos. Vanilla behavior.\n"
                    "- Wear Mask of Truth: Hidden grottos are visible when wearing the Mask of Truth. MM3D behavior.\n"
                    "- Have Mask of Truth: Hidden grottos are visible once the Mask of Truth is obtained.\n"
                    "- Always: Hidden grottos always have a visual marker.\n")
                .DefaultIndex(HiddenGrottosVisibilityOptions::HIDDEN_GROTTOS_VISIBLITY_OFF)
                .ComboVec(&maskOfTruthGrottoOptions));
    AddWidget(path, "Frog Choir Count", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.DifficultyOptions.FrogChoirCount")
        .Options(IntSliderOptions()
                     .Tooltip("Choose how many frogs you need to save for the choir performance.")
                     .Min(1)
                     .Max(5)
                     .DefaultValue(5));

    // HUD Editor
    path = { "Enhancements", "HUD Editor", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "HUD Editor", 1);
    AddWidget(path, "Popout HUD Editor", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.HudEditor")
        .WindowName("HUD Editor")
        .Options(ButtonOptions()
                     .Tooltip("Enables the HUD Editor window, allowing you to modify your HUD.")
                     .Size(Sizes::Inline));

    // Cosmetics Editor
    path = { "Enhancements", "Cosmetic Editor", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Cosmetic Editor", 1);
    AddWidget(path, "Popout Cosmetic Editor", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.CosmeticEditor")
        .WindowName("Cosmetic Editor")
        .Options(ButtonOptions()
                     .Tooltip("Enables the Cosmetic Editor window, allowing you to modify various colors in the game.")
                     .Size(Sizes::Inline));

    // Timesplit Settings
    path = { "Enhancements", "Time Splits", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Time Splits", 1);
    AddWidget(path, "Popout Timesplits Settings", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.Timesplits.Settings")
        .WindowName("Time Splits Settings Window");

    // Audio Editor
    path = { "Enhancements", "Audio Editor", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Audio Editor", 1);
    AddWidget(path, "Popout Audio Editor", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.AudioEditor")
        .WindowName("Audio Editor");
}

void BenMenu::AddDevTools() {
    AddMenuEntry("Dev Tools", "gSettings.Menu.DevToolsSidebarSection");
    AddSidebarEntry("Dev Tools", "General", 3);
    WidgetPath path = { "Dev Tools", "General", SECTION_COLUMN_1 };
    AddWidget(path, "Popout Menu", WIDGET_CVAR_CHECKBOX)
        .CVar("gSettings.Menu.Popout")
        .Options(CheckboxOptions().Tooltip("Changes the menu display from overlay to windowed."));
    AddWidget(path, "Debug Mode", WIDGET_CVAR_CHECKBOX)
        .CVar("gDeveloperTools.DebugEnabled")
        .Options(CheckboxOptions().Tooltip("Enables Debug Mode, allowing the following:\n\n"
                                           "- Open debug warp menu with L + R + Z\n"
                                           "- Enable debug no-clip mode with L + D-Right\n"
                                           "- Open built-in debug inventory editor when paused with L\n"
                                           "- Saves created will inherit inventory from \"Debug Save File Mode\""));
    AddWidget(path, "Better Map Select", WIDGET_CVAR_CHECKBOX)
        .CVar("gDeveloperTools.BetterMapSelect.Enabled")
        .Options(CheckboxOptions().Tooltip(
            "Overrides the original map select with a translated, more user-friendly version."))
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Map Select Button Combination:", WIDGET_CVAR_BTN_SELECTOR)
        .CVar("gDeveloperTools.MapSelectBtn")
        .Options(BtnSelectorOptions().DefaultValue(BTN_R | BTN_L | BTN_Z))
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "No Clip Button Combination:", WIDGET_CVAR_BTN_SELECTOR)
        .CVar("gDeveloperTools.NoClipBtn")
        .Options(BtnSelectorOptions().DefaultValue(BTN_L | BTN_DRIGHT))
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Debug Save File Mode", WIDGET_CVAR_COMBOBOX)
        .CVar("gDeveloperTools.DebugSaveFileMode")
        .Options(ComboboxOptions()
                     .Tooltip("Change the behavior of creating saves while debug mode is enabled:\n\n"
                              "- Empty Save: The default 3 heart save file in first cycle.\n"
                              "- Vanilla Debug Save: Uses the title screen save info (8 hearts, all items and masks).\n"
                              "- 100\% Save: All items, equipment, mask, quest status and Bombers' Notebook complete.")
                     .ComboVec(&debugSaveOptions))
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Prevent Actor Update", WIDGET_CVAR_CHECKBOX)
        .CVar("gDeveloperTools.PreventActorUpdate")
        .Options(CheckboxOptions().Tooltip("Prevents Actors from updating."))
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Prevent Actor Draw", WIDGET_CVAR_CHECKBOX)
        .CVar("gDeveloperTools.PreventActorDraw")
        .Options(CheckboxOptions().Tooltip("Prevents Actors from drawing."))
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Prevent Actor Init", WIDGET_CVAR_CHECKBOX)
        .CVar("gDeveloperTools.PreventActorInit")
        .Options(CheckboxOptions().Tooltip("Prevents Actors from initializing."))
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Disable Object Dependency", WIDGET_CVAR_CHECKBOX)
        .CVar("gDeveloperTools.DisableObjectDependency")
        .Options(CheckboxOptions().Tooltip("Disables dependencies when loading objects."))
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Log Level", WIDGET_CVAR_COMBOBOX)
        .CVar("gDeveloperTools.LogLevel")
        .Options(ComboboxOptions()
                     .Tooltip("The log level determines which messages are printed to the "
                              "console. This does not affect the log file output.")
                     .ComboVec(&logLevels)
                     .DefaultIndex(defaultLogLevel))
        .Callback([](WidgetInfo& info) {
            Ship::Context::GetRawInstance()->GetLogger()->set_level(
                (spdlog::level::level_enum)CVarGetInteger("gDeveloperTools.LogLevel", defaultLogLevel));
        })
        .PreFunc([](WidgetInfo& info) { info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active; });
    AddWidget(path, "Frame Advance", WIDGET_CHECKBOX)
        .Options(CheckboxOptions().Tooltip(
            "This allows you to advance through the game one frame at a time on command. "
            "To advance a frame, hold Z and tap R on the second controller. Holding Z "
            "and R will advance a frame every half second. You can also use the buttons below."))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_NULL_PLAY_STATE).active ||
                            mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
            if (gPlayState != nullptr) {
                info.valuePointer = (bool*)&gPlayState->frameAdvCtx.enabled;
            } else {
                info.valuePointer = (bool*)nullptr;
            }
        });
    AddWidget(path, "Advance 1", WIDGET_BUTTON)
        .Options(ButtonOptions().Tooltip("Advance 1 frame.").Size(Sizes::Inline))
        .Callback([](WidgetInfo& info) { CVarSetInteger("gDeveloperTools.FrameAdvanceTick", 1); })
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FRAME_ADVANCE_OFF).active ||
                            mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
        });
    AddWidget(path, "Advance (Hold)", WIDGET_BUTTON)
        .Options(ButtonOptions().Tooltip("Advance frames while the button is held.").Size(Sizes::Inline))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = mBenMenu->disabledMap.at(DISABLE_FOR_FRAME_ADVANCE_OFF).active ||
                            mBenMenu->disabledMap.at(DISABLE_FOR_DEBUG_MODE_OFF).active;
        })
        .PostFunc([](WidgetInfo& info) {
            if (ImGui::IsItemActive()) {
                CVarSetInteger("gDeveloperTools.FrameAdvanceTick", 1);
            }
        })
        .SameLine(true);
    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Warp Point", WIDGET_CUSTOM).CustomFunction([](WidgetInfo& info) { RenderWarpPointSection(); });

    // dev tools windows
    path = { "Dev Tools", "Collision Viewer", SECTION_COLUMN_1 };
    AddSidebarEntry("Dev Tools", "Collision Viewer", 1);
    AddWidget(path, "Popout Collision Viewer", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.CollisionViewer")
        .Options(ButtonOptions().Tooltip("Makes collision visible on screen.").Size(Sizes::Inline))
        .WindowName("Collision Viewer");

    path = { "Dev Tools", "Stats", SECTION_COLUMN_1 };
    AddSidebarEntry("Dev Tools", "Stats", 1);
    AddWidget(path, "Popout Stats", WIDGET_WINDOW_BUTTON)
        .CVar("gOpenWindows.Stats")
        .Options(ButtonOptions().Tooltip(
            "Shows the Stats window, with your FPS and frametimes, and the OS you're playing on."))
        .WindowName("Stats");

    path = { "Dev Tools", "Console", SECTION_COLUMN_1 };
    AddSidebarEntry("Dev Tools", "Console", 1);
    AddWidget(path, "Popout Console", WIDGET_WINDOW_BUTTON)
        .CVar("gOpenWindows.Console")
        .Options(ButtonOptions().Tooltip(
            "Enables the Console window, allowing you to input commands. Type help for some examples."))
        .WindowName("Console");

    path = { "Dev Tools", "Gfx Debugger", SECTION_COLUMN_1 };
    AddSidebarEntry("Dev Tools", "Gfx Debugger", 1);
    AddWidget(path, "Popout Gfx Debugger", WIDGET_WINDOW_BUTTON)
        .CVar("gOpenWindows.GfxDebugger")
        .Options(ButtonOptions().Tooltip(
            "Enables the Gfx Debugger window, allowing you to input commands, type help for some examples."))
        .WindowName("Gfx Debugger");

    path = { "Dev Tools", "Hook Debugger", SECTION_COLUMN_1 };
    AddSidebarEntry("Dev Tools", "Hook Debugger", 1);
    AddWidget(path, "Popout Hook Debugger", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.HookDebugger")
        .Options(ButtonOptions().Tooltip("Enables the Hook Debugger window, for viewing info about registered hooks."))
        .WindowName("Hook Debugger");

    path = { "Dev Tools", "Save Editor", SECTION_COLUMN_1 };
    AddSidebarEntry("Dev Tools", "Save Editor", 1);
    AddWidget(path, "Popout Save Editor", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.SaveEditor")
        .Options(ButtonOptions().Tooltip("Enables the Save Editor window, allowing you to edit your save file."))
        .WindowName("Save Editor");

    path = { "Dev Tools", "Actor Viewer", SECTION_COLUMN_1 };
    AddSidebarEntry("Dev Tools", "Actor Viewer", 1);
    AddWidget(path, "Popout Actor Viewer", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.ActorViewer")
        .Options(ButtonOptions().Tooltip("Enables the Actor Viewer window, allowing you to view actors in the world."))
        .WindowName("Actor Viewer");

    path = { "Dev Tools", "Event Log", SECTION_COLUMN_1 };
    AddSidebarEntry("Dev Tools", "Event Log", 1);
    AddWidget(path, "Popout Event Log", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.EventLog")
        .Options(ButtonOptions().Tooltip("Enables the Event Log window."))
        .WindowName("Event Log");

    path = { "Dev Tools", "DL Viewer", SECTION_COLUMN_1 };
    AddSidebarEntry("Dev Tools", "DL Viewer", 1);
    AddWidget(path, "Popout DL Viewer", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.DLViewer")
        .Options(ButtonOptions().Tooltip("Enables the DL Viewer window for inspecting and editing display lists."))
        .WindowName("DL Viewer");
    path = { "Dev Tools", "Message Viewer", SECTION_COLUMN_1 };
    AddSidebarEntry("Dev Tools", "Message Viewer", 1);
    AddWidget(path, "Popout Message Viewer", WIDGET_WINDOW_BUTTON)
        .CVar("gWindows.MessageViewer")
        .Options(ButtonOptions().Tooltip("Enables the Message Viewer window for testing in-game messages."))
        .WindowName("Message Viewer");
}

BenMenu::BenMenu(const std::string& consoleVariable, const std::string& name)
    : Menu(consoleVariable, name, 0, UIWidgets::Colors::LightBlue) {
}

void BenMenu::InitElement() {
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
        { DISABLE_FOR_CAMERAS_OFF,
          { [](disabledInfo& info) -> bool {
               return !CVarGetInteger("gEnhancements.Camera.DebugCam.Enable", 0) &&
                      !CVarGetInteger("gEnhancements.Camera.FreeLook.Enable", 0);
           },
            "Both Debug Camera and Free Look are Disabled" } },
        { DISABLE_FOR_DEBUG_CAM_ON,
          { [](disabledInfo& info) -> bool { return CVarGetInteger("gEnhancements.Camera.DebugCam.Enable", 0); },
            "Debug Camera is Enabled" } },
        { DISABLE_FOR_DEBUG_CAM_OFF,
          { [](disabledInfo& info) -> bool { return !CVarGetInteger("gEnhancements.Camera.DebugCam.Enable", 0); },
            "Debug Camera is Disabled" } },
        { DISABLE_FOR_FREE_LOOK_ON,
          { [](disabledInfo& info) -> bool { return CVarGetInteger("gEnhancements.Camera.FreeLook.Enable", 0); },
            "Free Look is Enabled" } },
        { DISABLE_FOR_FREE_LOOK_OFF,
          { [](disabledInfo& info) -> bool { return !CVarGetInteger("gEnhancements.Camera.FreeLook.Enable", 0); },
            "Free Look is Disabled" } },
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
          { [](disabledInfo& info) -> bool { return gPlayState == NULL; }, "Not in game" } },
        { DISABLE_FOR_DEBUG_MODE_OFF,
          { [](disabledInfo& info) -> bool { return !CVarGetInteger("gDeveloperTools.DebugEnabled", 0); },
            "Debug Mode is Disabled" } },
        { DISABLE_FOR_NO_VSYNC,
          { [](disabledInfo& info) -> bool {
               return !Ship::Context::GetRawInstance()->GetWindow()->CanDisableVerticalSync();
           },
            "Disabling VSync not supported" } },
        { DISABLE_FOR_NO_WINDOWED_FULLSCREEN,
          { [](disabledInfo& info) -> bool {
               return !Ship::Context::GetRawInstance()->GetWindow()->SupportsWindowedFullscreen();
           },
            "Windowed Fullscreen not supported" } },
        { DISABLE_FOR_NO_MULTI_VIEWPORT,
          { [](disabledInfo& info) -> bool {
               return !Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SupportsViewports();
           },
            "Multi-viewports not supported" } },
        { DISABLE_FOR_NOT_DIRECTX,
          { [](disabledInfo& info) -> bool {
               return Ship::Context::GetRawInstance()->GetWindow()->GetWindowBackend() !=
                      Fast::WindowBackend::FAST3D_DXGI_DX11;
           },
            "Available Only on DirectX" } },
        { DISABLE_FOR_DIRECTX,
          { [](disabledInfo& info) -> bool {
               return Ship::Context::GetRawInstance()->GetWindow()->GetWindowBackend() ==
                      Fast::WindowBackend::FAST3D_DXGI_DX11;
           },
            "Not Available on DirectX" } },
        { DISABLE_FOR_MATCH_REFRESH_RATE_ON,
          { [](disabledInfo& info) -> bool { return CVarGetInteger("gMatchRefreshRate", 0); },
            "Match Refresh Rate is Enabled" } },
        { DISABLE_FOR_MOTION_BLUR_MODE,
          { [](disabledInfo& info) -> bool {
               info.value = CVarGetInteger("gEnhancements.Graphics.MotionBlur.Mode", 0);
               return !info.value;
           },
            "Motion Blur Mode mismatch" } },
        { DISABLE_FOR_MOTION_BLUR_OFF,
          { [](disabledInfo& info) -> bool { return !R_MOTION_BLUR_ENABLED; }, "Motion Blur is disabled" } },
        { DISABLE_FOR_FRAME_ADVANCE_OFF,
          { [](disabledInfo& info) -> bool { return !(gPlayState != nullptr && gPlayState->frameAdvCtx.enabled); },
            "Frame Advance is Disabled" } },
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
          { [](disabledInfo& info) -> bool { return CVarGetInteger(CVAR_LOW_RES_MODE, 0); }, "N64 Mode is enabled" } },
        { DISABLE_FOR_ADVANCED_RESOLUTION_OFF,
          { [](disabledInfo& info) -> bool { return !CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".Enabled", 0); },
            "Advanced Resolution is Disabled" } },
        { DISABLE_FOR_VERTICAL_RESOLUTION_OFF,
          { [](disabledInfo& info) -> bool {
               return !CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".VerticalResolutionToggle", 0);
           },
            "Vertical Resolution Toggle is Off" } },
        { DISABLE_FOR_LINKS_VOICE_PITCH_MULTIPLIER_OFF,
          { [](disabledInfo& info) -> bool {
               return !CVarGetInteger("gAudioEditor.LinkVoiceFreqMultiplier.Enable", 0);
           },
            "Enable Link's Voice Pitch Multiplier is Disabled" } },
        { DISABLE_FOR_KOUME_INVINCIBLE,
          { [](disabledInfo& info) -> bool {
               return CVarGetInteger("gEnhancements.Minigames.BoatArcheryInvincible", 0);
           },
            "Koume is Invincible" } },
        { DISABLE_FOR_INVASION_SKIP,
          { [](disabledInfo& info) -> bool { return CVarGetInteger("gEnhancements.Minigames.SkipRanchInvasion", 0); },
            "Alien Invasion Skipped" } }
    };
}

void BenMenu::UpdateElement() {
    Ship::Menu::UpdateElement();
}

void BenMenu::Draw() {
    Ship::Menu::Draw();
}

void BenMenu::DrawElement() {
    Ship::Menu::DrawElement();
}
} // namespace BenGui
