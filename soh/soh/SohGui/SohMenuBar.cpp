#include "SohMenuBar.h"
#include <imgui.h>
#include "regex"
#include "public/bridge/consolevariablebridge.h"
#include <libultraship/libultraship.h>
#include "UIWidgets.hpp"
#include "include/z64audio.h"
#include "graphic/Fast3D/gfx_rendering_api.h"
#include "soh/OTRGlobals.h"
#include "soh/SaveManager.h"
#include "z64.h"
#include "soh/cvar_prefixes.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/presets.h"
#include "soh/Enhancements/mods.h"
#include "soh/Notification/Notification.h"
#include "soh/Enhancements/cosmetics/authenticGfxPatches.h"
#ifdef ENABLE_REMOTE_CONTROL
#include "soh/Network/CrowdControl/CrowdControl.h"
#include "soh/Network/Sail/Sail.h"
#endif


#include "soh/Enhancements/audio/AudioEditor.h"
#include "soh/Enhancements/controls/InputViewer.h"
#include "soh/Enhancements/cosmetics/CosmeticsEditor.h"
#include "soh/Enhancements/debugger/actorViewer.h"
#include "soh/Enhancements/debugger/colViewer.h"
#include "soh/Enhancements/debugger/debugSaveEditor.h"
#include "soh/Enhancements/debugger/hookDebugger.h"
#include "soh/Enhancements/debugger/dlViewer.h"
#include "soh/Enhancements/debugger/valueViewer.h"
#include "soh/Enhancements/gameplaystatswindow.h"
#include "soh/Enhancements/debugger/MessageViewer.h"
#include "soh/Enhancements/randomizer/randomizer_check_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_entrance_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_item_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_settings_window.h"
#include "soh/Enhancements/resolution-editor/ResolutionEditor.h"
#include "soh/Enhancements/enemyrandomizer.h"
#include "soh/Enhancements/timesplits/TimeSplits.h"
#include "soh/Enhancements/randomizer/Plandomizer.h"
#include "soh/Enhancements/TimeDisplay/TimeDisplay.h"
#include "soh/Enhancements/Autosave.h"

// FA icons are kind of wonky, if they worked how I expected them to the "+ 2.0f" wouldn't be needed, but
// they don't work how I expect them to so I added that because it looked good when I eyeballed it
#define FA_ICON_BUTTON_FRAME_PADDING_X(icon) (((optionsButtonSize.x - ImGui::CalcTextSize(icon).x) / 2) + 2.0f)

extern bool isBetaQuestEnabled;

extern "C" PlayState* gPlayState;

std::string GetWindowButtonText(const char* text, bool menuOpen) {
    char buttonText[100] = "";
    if (menuOpen) {
        strcat(buttonText, ICON_FA_CHEVRON_RIGHT " ");
    }
    strcat(buttonText, text);
    if (!menuOpen) { strcat(buttonText, "  "); }
    return buttonText;
}

static std::unordered_map<Ship::WindowBackend, const char*> windowBackendNames = {
    { Ship::WindowBackend::FAST3D_DXGI_DX11, "DirectX" },
    { Ship::WindowBackend::FAST3D_SDL_OPENGL, "OpenGL" },
    { Ship::WindowBackend::FAST3D_SDL_METAL, "Metal" },
};

static const char* imguiScaleOptions[4] = { "Small", "Normal", "Large", "X-Large" };

    static const char* filters[3] = {
#ifdef __WIIU__
            "",
#else
            "Three-Point",
#endif
            "Linear", "None"
    };

    static const char* chestStyleMatchesContentsOptions[4] = { "Disabled", "Both", "Texture Only", "Size Only" };
    static const char* skipGetItemAnimationOptions[3] = { "Disabled", "Junk Items", "All Items" };
    static const char* skipForcedDialogOptions[4] = { "None", "Navi Only", "NPCs Only", "All" };
    static const char* sleepingWaterfallOptions[3] = { "Always", "Once", "Never" };
    static const char* bunnyHoodOptions[3] = { "Disabled", "Faster Run & Longer Jump", "Faster Run" };
    static const char* mirroredWorldModes[9] = {
        "Disabled",           "Always",        "Random",          "Random (Seeded)",          "Dungeons",
        "Dungeons (Vanilla)", "Dungeons (MQ)", "Dungeons Random", "Dungeons Random (Seeded)",
    };
    static const char* enemyRandomizerModes[3] = { "Disabled", "Random", "Random (Seeded)" };
    static const char* allPowers[9] = {
                        "Vanilla (1x)",
                        "Double (2x)",
                        "Quadruple (4x)",
                        "Octuple (8x)",
                        "Foolish (16x)",
                        "Ridiculous (32x)",
                        "Merciless (64x)",
                        "Pure Torture (128x)",
                        "OHKO (256x)" };
    static const char* subPowers[8] = { allPowers[0], allPowers[1], allPowers[2], allPowers[3], allPowers[4], allPowers[5], allPowers[6], allPowers[7] };
    static const char* subSubPowers[7] = { allPowers[0], allPowers[1], allPowers[2], allPowers[3], allPowers[4], allPowers[5], allPowers[6] };
    static const char* zFightingOptions[3] = { "Disabled", "Consistent Vanish", "No Vanish" };
    static const char* bootSequenceLabels[3] = { "Default", "Authentic", "File Select" };
    static const char* DekuStickCheat[3] = { "Normal", "Unbreakable", "Unbreakable + Always on Fire" };
    static const char* bonkDamageValues[8] = {
        "No Damage",
        "0.25 Heart",
        "0.5 Heart",
        "1 Heart",
        "2 Hearts",
        "4 Hearts",
        "8 Hearts",
        "OHKO"
    };
    static const char* timeTravelOptions[3] = { "Disabled", "Ocarina of Time", "Any Ocarina" };
    static const char* swordToggleModes[3] = { "Disabled", "Child Toggle", "Both Ages (May lead to unintended behaviour)"};
    static const char* itemCountMessageCVars[3] = {
        CVAR_ENHANCEMENT("InjectItemCounts.GoldSkulltula"),
        CVAR_ENHANCEMENT("InjectItemCounts.HeartPiece"),
        CVAR_ENHANCEMENT("InjectItemCounts.HeartContainer"),
    };
    static const char* itemCountMessageOptions[ARRAY_COUNT(itemCountMessageCVars)] = {
        "Gold Skulltula Tokens",
        "Pieces of Heart",
        "Heart Containers",
    };

extern "C" SaveContext gSaveContext;

namespace SohGui {

std::unordered_map<Ship::WindowBackend, const char*> availableWindowBackendsMap;
Ship::WindowBackend configWindowBackend;

extern std::shared_ptr<Ship::GuiWindow> mGfxDebuggerWindow;
extern std::shared_ptr<AdvancedResolutionSettings::AdvancedResolutionSettingsWindow> mAdvancedResolutionSettingsWindow;

void DrawSettingsMenu() {
    if (ImGui::BeginMenu("Settings"))
    {
        if (ImGui::BeginMenu("Graphics")) {
        #ifndef __APPLE__
            if (mAdvancedResolutionSettingsWindow) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                UIWidgets::Spacer(0);
                if (ImGui::Button(GetWindowButtonText("Advanced Resolution", CVarGetInteger(CVAR_WINDOW("AdvancedResolutionEditor"), 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                    mAdvancedResolutionSettingsWindow->ToggleVisibility();
                }
                ImGui::PopStyleColor(1);
                ImGui::PopStyleVar(3);
            }
        #endif
            ImGui::Text("ImGui Menu Scale");
            ImGui::SameLine();
            ImGui::TextColored({ 0.85f, 0.35f, 0.0f, 1.0f }, "(Experimental)");
            if (UIWidgets::EnhancementCombobox(CVAR_SETTING("ImGuiScale"), imguiScaleOptions, 1)) {
                OTRGlobals::Instance->ScaleImGui();
            }
            UIWidgets::Tooltip("Changes the scaling of the ImGui menu elements.");

            // Draw LUS settings menu (such as Overlays Text Font)
            Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGameOverlay()->DrawSettings();

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Languages")) {
            UIWidgets::PaddedEnhancementCheckbox("Translate Title Screen", CVAR_SETTING("TitleScreenTranslation"));
            if (UIWidgets::EnhancementRadioButton("English", CVAR_SETTING("Languages"), LANGUAGE_ENG)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            if (UIWidgets::EnhancementRadioButton("German", CVAR_SETTING("Languages"), LANGUAGE_GER)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            if (UIWidgets::EnhancementRadioButton("French", CVAR_SETTING("Languages"), LANGUAGE_FRA)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            ImGui::EndMenu();
        }
        
        UIWidgets::Spacer(0);
        
        if (ImGui::BeginMenu("Accessibility")) {
        #if defined(_WIN32) || defined(__APPLE__)
            UIWidgets::PaddedEnhancementCheckbox("Text to Speech", CVAR_SETTING("A11yTTS"));
            UIWidgets::Tooltip("Enables text to speech for in game dialog");
        #endif
            UIWidgets::PaddedEnhancementCheckbox("Disable Idle Camera Re-Centering", CVAR_SETTING("A11yDisableIdleCam"));
            UIWidgets::Tooltip("Disables the automatic re-centering of the camera when idle.");
            
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

extern std::shared_ptr<TimeDisplayWindow> mTimeDisplayWindow;

void DrawEnhancementsMenu() {
    if (ImGui::BeginMenu("Enhancements"))
    {
        ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));

        DrawPresetSelector(PRESET_TYPE_ENHANCEMENTS);

        UIWidgets::PaddedSeparator();

        if (ImGui::BeginMenu("Gameplay"))
        {
            UIWidgets::EnhancementCheckbox("Visual Stone of Agony", CVAR_ENHANCEMENT("VisualAgony"));
            UIWidgets::Tooltip("Displays an icon and plays a sound when Stone of Agony should be activated, for those without rumble");
            static const char* cursorOnAnySlot[3] = { "Only in Rando", "Always", "Never" };
            UIWidgets::PaddedText("Allow the cursor to be on any slot", true, false);
            UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("PauseAnyCursor"), cursorOnAnySlot, PAUSE_ANY_CURSOR_RANDO_ONLY);
            UIWidgets::Tooltip("Allows the cursor on the pause menu to be over any slot. Sometimes required in rando to select certain items.");
            UIWidgets::PaddedEnhancementCheckbox("Assignable Tunics and Boots", CVAR_ENHANCEMENT("AssignableTunicsAndBoots"), true, false);
            UIWidgets::Tooltip("Allows equipping the tunic and boots to c-buttons");
            UIWidgets::PaddedEnhancementCheckbox("Equipment Toggle", CVAR_ENHANCEMENT("EquipmentCanBeRemoved"), true, false);
            UIWidgets::Tooltip("Allows equipment to be removed by toggling it off on\nthe equipment subscreen.");
            if (CVarGetInteger(CVAR_ENHANCEMENT("EquipmentCanBeRemoved"), 0)) {
                UIWidgets::PaddedText("Sword Toggle Options", true, false);
                UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("SwordToggle"), swordToggleModes, SWORD_TOGGLE_NONE);
                UIWidgets::Tooltip(
                    "Introduces Options for unequipping Link's sword\n\n"
                    "None: Only Biggoron's Sword/Giant's Knife can be toggled. Doing so will equip the Master Sword.\n\n"
                    "Child Toggle: This will allow for completely unequipping any sword as child link.\n\n"
                    "Both Ages: Any sword can be unequipped as either age. This may lead to swordless glitches as Adult.\n"
                );
            }

            UIWidgets::PaddedEnhancementCheckbox("Link's Cow in Both Time Periods", CVAR_ENHANCEMENT("CowOfTime"), true, false);
            UIWidgets::Tooltip("Allows the Lon Lon Ranch obstacle course reward to be shared across time periods");
            UIWidgets::PaddedEnhancementCheckbox("Enable visible guard vision", CVAR_ENHANCEMENT("GuardVision"), true, false);
            UIWidgets::PaddedEnhancementCheckbox("Enable passage of time on file select", CVAR_ENHANCEMENT("TimeFlowFileSelect"), true, false);
            UIWidgets::PaddedEnhancementCheckbox("Pull grave during the day", CVAR_ENHANCEMENT("DayGravePull"), true, false);
            UIWidgets::Tooltip("Allows graves to be pulled when child during the day");
            UIWidgets::PaddedEnhancementCheckbox("Dogs follow you everywhere", CVAR_ENHANCEMENT("DogFollowsEverywhere"), true, false);
            UIWidgets::Tooltip("Allows dogs to follow you anywhere you go, even if you leave the market");
            UIWidgets::PaddedEnhancementCheckbox("Don't require input for Credits sequence", CVAR_ENHANCEMENT("NoInputForCredits"), true, false);
            UIWidgets::Tooltip("Removes the input requirement on textboxes after defeating Ganon, allowing Credits sequence to continue to progress");
            UIWidgets::PaddedEnhancementCheckbox("Answer Navi Prompt with L Button", CVAR_ENHANCEMENT("NaviOnL"), true, false);
            UIWidgets::Tooltip("Speak to Navi with L but enter first-person camera with C-Up");

            // Blue Fire Arrows
            bool forceEnableBlueFireArrows = IS_RANDO &&
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BLUE_FIRE_ARROWS);
            static const char* forceEnableBlueFireArrowsText =
                "This setting is forcefully enabled because a savefile\nwith \"Blue Fire Arrows\" is loaded.";
            UIWidgets::PaddedEnhancementCheckbox("Blue Fire Arrows", CVAR_ENHANCEMENT("BlueFireArrows"), true, false, 
                forceEnableBlueFireArrows, forceEnableBlueFireArrowsText, UIWidgets::CheckboxGraphics::Checkmark);
            UIWidgets::Tooltip("Allows Ice Arrows to melt red ice.\nMay require a room reload if toggled during gameplay.");

            // Sunlight Arrows
            bool forceEnableSunLightArrows = IS_RANDO &&
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SUNLIGHT_ARROWS);
            static const char* forceEnableSunLightArrowsText =
                "This setting is forcefully enabled because a savefile\nwith \"Sunlight Arrows\" is loaded.";
            UIWidgets::PaddedEnhancementCheckbox("Sunlight Arrows", CVAR_ENHANCEMENT("SunlightArrows"), true, false, 
                forceEnableSunLightArrows, forceEnableSunLightArrowsText, UIWidgets::CheckboxGraphics::Checkmark);
            UIWidgets::Tooltip("Allows Light Arrows to activate sun switches.\nMay require a room reload if toggled during gameplay.");

            UIWidgets::PaddedEnhancementCheckbox("Disable Crit wiggle", CVAR_ENHANCEMENT("DisableCritWiggle"), true, false);
            UIWidgets::Tooltip("Disable random camera wiggle at low health");
            UIWidgets::PaddedEnhancementCheckbox("Enemy Health Bars", CVAR_ENHANCEMENT("EnemyHealthBar"), true, false);
            UIWidgets::Tooltip("Renders a health bar for enemies when Z-Targeted");

            UIWidgets::PaddedEnhancementCheckbox("Targetable Hookshot Reticle", CVAR_ENHANCEMENT("HookshotableReticle"), true, false);
            UIWidgets::Tooltip("Use a different color when aiming at hookshotable collision");

            UIWidgets::PaddedEnhancementCheckbox("Faster Rupee Accumulator", CVAR_ENHANCEMENT("TimeSavers.FasterRupeeAccumulator"), true, false);
            UIWidgets::Tooltip("Causes your wallet to fill and empty faster when you gain or lose money.");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Graphics"))
        {
            if (ImGui::BeginMenu("Mods")) {
                UIWidgets::PaddedEnhancementCheckbox("Use Alternate Assets", CVAR_ENHANCEMENT("AltAssets"), false, false);
                UIWidgets::Tooltip("Toggle between standard assets and alternate assets. Usually mods will indicate if this setting has to be used or not.");
                UIWidgets::PaddedEnhancementCheckbox("Disable Bomb Billboarding", CVAR_ENHANCEMENT("DisableBombBillboarding"), true, false);
                UIWidgets::Tooltip("Disables bombs always rotating to face the camera. To be used in conjunction with mods that want to replace bombs with 3D objects.");
                UIWidgets::PaddedEnhancementCheckbox("Disable Grotto Fixed Rotation", CVAR_ENHANCEMENT("DisableGrottoRotation"), true, false);
                UIWidgets::Tooltip("Disables grottos rotating with the camera. To be used in conjunction with mods that want to replace grottos with 3D objects.");
                UIWidgets::PaddedEnhancementCheckbox("Invisible Bunny Hood", CVAR_ENHANCEMENT("HideBunnyHood"), true, false);
                UIWidgets::Tooltip("Turns Bunny Hood invisible while still maintaining its effects.");
                UIWidgets::PaddedEnhancementCheckbox("Disable HUD Heart animations", CVAR_ENHANCEMENT("NoHUDHeartAnimation"), true, false);
                UIWidgets::Tooltip("Disables the beating animation of the hearts on the HUD.");

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            UIWidgets::PaddedEnhancementCheckbox("Animated Link in Pause Menu", CVAR_ENHANCEMENT("PauseMenuAnimatedLink"), false, false);
            UIWidgets::PaddedEnhancementCheckbox("Disable LOD", CVAR_ENHANCEMENT("DisableLOD"), true, false);
            UIWidgets::Tooltip(
                "Turns off the Level of Detail setting, making models use their higher-poly variants at any distance");
            if (UIWidgets::EnhancementSliderInt("Increase Actor Draw Distance: %dx", "##IncreaseActorDrawDistance",
                                                CVAR_ENHANCEMENT("DisableDrawDistance"), 1, 5, "", 1, true, false)) {
                if (CVarGetInteger(CVAR_ENHANCEMENT("DisableDrawDistance"), 1) <= 1) {
                    CVarSetInteger(CVAR_ENHANCEMENT("DisableKokiriDrawDistance"), 0);
                }
            }
            UIWidgets::Tooltip("Increases the range in which actors/objects are drawn");
            if (CVarGetInteger(CVAR_ENHANCEMENT("DisableDrawDistance"), 1) > 1) {
                UIWidgets::PaddedEnhancementCheckbox("Kokiri Draw Distance",
                                                     CVAR_ENHANCEMENT("DisableKokiriDrawDistance"), true, false);
                UIWidgets::Tooltip("The Kokiri are mystical beings that fade into view when approached\nEnabling this "
                                   "will remove their draw distance");
            }
            UIWidgets::PaddedEnhancementCheckbox("Widescreen Actor Culling", CVAR_ENHANCEMENT("WidescreenActorCulling"),
                                                 true, false);
            UIWidgets::Tooltip("Adjusts the horizontal culling plane to account for widescreen resolutions");
            UIWidgets::PaddedEnhancementCheckbox(
                "Cull Glitch Useful Actors", CVAR_ENHANCEMENT("ExtendedCullingExcludeGlitchActors"), true, false,
                !CVarGetInteger(CVAR_ENHANCEMENT("WidescreenActorCulling"), 0) &&
                    CVarGetInteger(CVAR_ENHANCEMENT("DisableDrawDistance"), 1) <= 1,
                "Requires Actor Draw Distance to be increased or Widescreen Actor Culling enabled");
            UIWidgets::Tooltip(
                "Exclude actors that are useful for glitches from the extended culling ranges.\n"
                "Some actors may still draw in the extended ranges, but will not \"update\" so that certain "
                "glitches that leverage the original culling requirements will still work.\n"
                "\n"
                "The following actors are excluded:\n"
                "- White clothed Gerudos\n"
                "- King Zora\n"
                "- Gossip Stones\n"
                "- Boulders\n"
                "- Blue Warps\n"
                "- Darunia\n"
                "- Gold Skulltulas");
            if (UIWidgets::PaddedEnhancementCheckbox("Show Age-Dependent Equipment", CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), true,
                                                     false)) {
                UpdatePatchHand();
            }
            UIWidgets::Tooltip("Makes all equipment visible, regardless of Age.");
            if (CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0) == 1) {
				UIWidgets::PaddedEnhancementCheckbox("Scale Adult Equipment as Child", CVAR_ENHANCEMENT("ScaleAdultEquipmentAsChild"), true, false);
				UIWidgets::Tooltip("Scales all of the Adult Equipment, as well and moving some a bit, to fit on Child Link Better. May not work properly with some mods.");
			}
            UIWidgets::PaddedEnhancementCheckbox("N64 Mode", CVAR_LOW_RES_MODE, true, false);
            UIWidgets::Tooltip("Sets aspect ratio to 4:3 and lowers resolution to 240p, the N64's native resolution");
            UIWidgets::PaddedEnhancementCheckbox("Glitch line-up tick", CVAR_ENHANCEMENT("DrawLineupTick"), true, false);
            UIWidgets::Tooltip("Displays a tick in the top center of the screen to help with glitch line-ups in SoH, as traditional UI based line-ups do not work outside of 4:3");
            UIWidgets::PaddedEnhancementCheckbox("Enable 3D Dropped items/projectiles", CVAR_ENHANCEMENT("NewDrops"), true, false);
            UIWidgets::Tooltip("Change most 2D items and projectiles on the overworld to their 3D versions");
            UIWidgets::PaddedEnhancementCheckbox("Disable Black Bar Letterboxes", CVAR_ENHANCEMENT("DisableBlackBars"), true, false);
            UIWidgets::Tooltip("Disables Black Bar Letterboxes during cutscenes and Z-targeting\nNote: there may be minor visual glitches that were covered up by the black bars\nPlease disable this setting before reporting a bug");
            UIWidgets::PaddedEnhancementCheckbox("Dynamic Wallet Icon", CVAR_ENHANCEMENT("DynamicWalletIcon"), true, false);
            UIWidgets::Tooltip("Changes the rupee in the wallet icon to match the wallet size you currently have");
            UIWidgets::PaddedEnhancementCheckbox("Always show dungeon entrances", CVAR_ENHANCEMENT("AlwaysShowDungeonMinimapIcon"), true, false);
            UIWidgets::Tooltip("Always shows dungeon entrance icons on the minimap");
            UIWidgets::PaddedEnhancementCheckbox("Show Gauntlets in First Person", CVAR_ENHANCEMENT("FirstPersonGauntlets"), true, false);
            UIWidgets::Tooltip("Renders Gauntlets when using the Bow and Hookshot like in OOT3D");
            if (UIWidgets::PaddedEnhancementCheckbox("Color Temple of Time's Medallions", CVAR_ENHANCEMENT("ToTMedallionsColors"), true, false)) {
                PatchToTMedallions();
            }
            UIWidgets::Tooltip("When medallions are collected, the medallion imprints around the Master Sword pedestal in the Temple of Time will become colored");
            UIWidgets::PaddedEnhancementCheckbox("Show locked door chains on both sides of locked doors", CVAR_ENHANCEMENT("ShowDoorLocksOnBothSides"), true, false);
            UIWidgets::PaddedText("Fix Vanishing Paths", true, false);
            if (UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("SceneSpecificDirtPathFix"), zFightingOptions, ZFIGHT_FIX_DISABLED) && gPlayState != NULL) {
                UpdateDirtPathFixState(gPlayState->sceneNum);
            }
            UIWidgets::Tooltip("Disabled: Paths vanish more the higher the resolution (Z-fighting is based on resolution)\n"
                                "Consistent: Certain paths vanish the same way in all resolutions\n"
                                "No Vanish: Paths do not vanish, Link seems to sink in to some paths\n"
                                "This might affect other decal effects\n");
            UIWidgets::PaddedEnhancementSliderInt("Text Spacing: %d", "##TEXTSPACING", CVAR_ENHANCEMENT("TextSpacing"), 4, 6, "", 6, true, true, true);
            UIWidgets::Tooltip("Space between text characters (useful for HD font textures)");
            UIWidgets::PaddedEnhancementCheckbox("More info in file select", CVAR_ENHANCEMENT("FileSelectMoreInfo"), true, false);
            UIWidgets::Tooltip("Shows what items you have collected in the file select screen, like in N64 randomizer");
            UIWidgets::PaddedEnhancementCheckbox("Better ammo rendering in pause menu", CVAR_ENHANCEMENT("BetterAmmoRendering"), true, false);
            UIWidgets::Tooltip("Ammo counts in the pause menu will work correctly regardless of the position of items in the inventory");
            UIWidgets::PaddedEnhancementCheckbox("Remove spin attack darkness", CVAR_ENHANCEMENT("RemoveSpinAttackDarkness"), true, false);
            UIWidgets::Tooltip("Remove the darkness that appears when charging a spin attack");
            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Fixes"))
        {
            UIWidgets::EnhancementCheckbox("Fix L&R Pause menu", CVAR_ENHANCEMENT("FixMenuLR"));
            UIWidgets::Tooltip("Makes the L and R buttons in the pause menu the same color");
            UIWidgets::PaddedEnhancementCheckbox("Fix L&Z Page switch in Pause menu", CVAR_ENHANCEMENT("NGCKaleidoSwitcher"), true, false);
            UIWidgets::Tooltip("Makes L and R switch pages like on the GameCube\nZ opens the Debug Menu instead");
            UIWidgets::PaddedEnhancementCheckbox("Fix Dungeon entrances", CVAR_ENHANCEMENT("FixDungeonMinimapIcon"), true, false);
            UIWidgets::Tooltip("Removes the dungeon entrance icon on the top-left corner of the screen when no dungeon is present on the current map");
            UIWidgets::PaddedEnhancementCheckbox("Fix Two Handed idle animations", CVAR_ENHANCEMENT("TwoHandedIdle"), true, false);
            UIWidgets::Tooltip("Re-enables the two-handed idle animation, a seemingly finished animation that was disabled on accident in the original game");
            UIWidgets::PaddedEnhancementCheckbox("Fix the Gravedigging Tour Glitch", CVAR_ENHANCEMENT("GravediggingTourFix"), true, false, SaveManager::Instance->IsRandoFile(),
                                                        "This setting is always enabled in randomizer files", UIWidgets::CheckboxGraphics::Checkmark);
            UIWidgets::Tooltip("Fixes a bug where the Gravedigging Tour Heart Piece disappears if the area reloads");
            UIWidgets::PaddedEnhancementCheckbox("Fix Deku Nut upgrade", CVAR_ENHANCEMENT("DekuNutUpgradeFix"), true, false);
            UIWidgets::Tooltip("Prevents the Forest Stage Deku Nut upgrade from becoming unobtainable after receiving the Poacher's Saw");
            UIWidgets::PaddedEnhancementCheckbox("Fix Navi text HUD position", CVAR_ENHANCEMENT("NaviTextFix"), true, false);
            UIWidgets::Tooltip("Correctly centers the Navi text prompt on the HUD's C-Up button");
            UIWidgets::PaddedEnhancementCheckbox("Fix Anubis fireballs", CVAR_ENHANCEMENT("AnubisFix"), true, false);
            UIWidgets::Tooltip("Make Anubis fireballs do fire damage when reflected back at them with the Mirror Shield");
            if (UIWidgets::PaddedEnhancementCheckbox("Fix Megaton Hammer crouch stab", CVAR_ENHANCEMENT("CrouchStabHammerFix"), true, false)) {
                if (!CVarGetInteger(CVAR_ENHANCEMENT("CrouchStabHammerFix"), 0)) {
                    CVarClear(CVAR_ENHANCEMENT("CrouchStabFix"));
                }
            }
            UIWidgets::Tooltip("Make the Megaton Hammer's crouch stab able to destroy rocks without first swinging it normally");
            if (CVarGetInteger(CVAR_ENHANCEMENT("CrouchStabHammerFix"), 0)) {
                UIWidgets::PaddedEnhancementCheckbox("Remove power crouch stab", CVAR_ENHANCEMENT("CrouchStabFix"), true, false);
                UIWidgets::Tooltip("Make crouch stabbing always do the same damage as a regular slash");
            }
            UIWidgets::PaddedEnhancementCheckbox("Fix credits timing", CVAR_ENHANCEMENT("CreditsFix"), true, false);
            UIWidgets::Tooltip("Extend certain credits scenes so the music lines up properly with the visuals");
            UIWidgets::PaddedEnhancementCheckbox("Fix Gerudo Warrior's clothing colors", CVAR_ENHANCEMENT("GerudoWarriorClothingFix"), true, false);
            UIWidgets::Tooltip("Prevent the Gerudo Warrior's clothes changing color when changing Link's tunic or using bombs in front of her");
            UIWidgets::PaddedEnhancementCheckbox("Fix Camera Drift", CVAR_ENHANCEMENT("FixCameraDrift"), true, false);
            UIWidgets::Tooltip("Fixes camera slightly drifting to the left when standing still due to a math error");
            UIWidgets::PaddedEnhancementCheckbox("Fix Camera Swing", CVAR_ENHANCEMENT("FixCameraSwing"), true, false);
            UIWidgets::Tooltip("Fixes camera getting stuck on collision when standing still, also fixes slight shift back in camera when stop moving");
            UIWidgets::PaddedEnhancementCheckbox("Fix Hanging Ledge Swing Rate", CVAR_ENHANCEMENT("FixHangingLedgeSwingRate"), true, false);
            UIWidgets::Tooltip("Fixes camera swing rate when player falls off a ledge and camera swings around");
            UIWidgets::PaddedEnhancementCheckbox("Fix Missing Jingle after 5 Silver Rupees", CVAR_ENHANCEMENT("SilverRupeeJingleExtend"), true, false);
            UIWidgets::Tooltip(
                "Adds 5 higher pitches for the Silver Rupee Jingle for the rooms with more than 5 Silver Rupees. "
                "Currently only relevant in Master Quest.");
            if (UIWidgets::PaddedEnhancementCheckbox("Fix out of bounds textures", CVAR_ENHANCEMENT("FixTexturesOOB"), true, false)) {
                ApplyAuthenticGfxPatches();
            }
            UIWidgets::Tooltip("Fixes authentic out of bounds texture reads, instead loading textures with the correct size");
            UIWidgets::PaddedEnhancementCheckbox("Fix Poacher's Saw Softlock", CVAR_ENHANCEMENT("FixSawSoftlock"), true, false, CVarGetInteger(CVAR_ENHANCEMENT("SkipText"), 0),
                "This is disabled because it is forced on when Skip Text is enabled.", UIWidgets::CheckboxGraphics::Checkmark);
            UIWidgets::Tooltip("Prevents the Poacher's Saw softlock from mashing through the text, or with Skip Text enabled.");
            UIWidgets::PaddedEnhancementCheckbox("Fix enemies not spawning near water", CVAR_ENHANCEMENT("EnemySpawnsOverWaterboxes"), true, false);
            UIWidgets::Tooltip("Causes respawning enemies, like stalchildren, to appear on land near bodies of water. "
                                "Fixes an incorrect calculation that acted like water underneath ground was above it.");
            UIWidgets::PaddedEnhancementCheckbox("Fix Bush Item Drops", CVAR_ENHANCEMENT("BushDropFix"), true, false);
            UIWidgets::Tooltip("Fixes the bushes to drop items correctly rather than spawning undefined items.");
            UIWidgets::PaddedEnhancementCheckbox("Fix falling from vine edges", CVAR_ENHANCEMENT("FixVineFall"), true, false); 
            UIWidgets::Tooltip("Prevents immediately falling off climbable surfaces if climbing on the edges."); 
            UIWidgets::PaddedEnhancementCheckbox("Fix Link's eyes open while sleeping", CVAR_ENHANCEMENT("FixEyesOpenWhileSleeping"), true, false);
            UIWidgets::Tooltip("Fixes Link's eyes being open in the opening cutscene when he is supposed to be sleeping.");
            UIWidgets::PaddedEnhancementCheckbox("Fix Darunia dancing too fast", CVAR_ENHANCEMENT("FixDaruniaDanceSpeed"),
                                                 true, false, false, "", UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip("Fixes Darunia's dancing speed so he dances to the beat of Saria's Song, like in vanilla.");
            UIWidgets::PaddedEnhancementCheckbox("Fix raised Floor Switches", CVAR_ENHANCEMENT("FixFloorSwitches"), true, false);
            UIWidgets::Tooltip("Fixes the two raised floor switches, the one in Forest Temple Basement and the one at the top of Fire Temple. \n"
                "This will lower them, making activating them easier");
            UIWidgets::PaddedEnhancementCheckbox("Fix Zora hint dialogue", CVAR_ENHANCEMENT("FixZoraHintDialogue"), true, false);
            UIWidgets::Tooltip("Fixes one Zora's dialogue giving a hint about bringing Ruto's Letter to King Zora to properly occur before moving King Zora rather than after");
            if (UIWidgets::PaddedEnhancementCheckbox("Fix hand holding Hammer", CVAR_ENHANCEMENT("FixHammerHand"), true, false)) {
                UpdatePatchHand();
            }
            UIWidgets::Tooltip("Fixes Adult Link having a backwards left hand when holding the Megaton Hammer.");
            if (UIWidgets::PaddedEnhancementCheckbox(
                "Fix Broken Giant's Knife bug", CVAR_ENHANCEMENT("FixBrokenGiantsKnife"), true, false, IS_RANDO,
                "This setting is forcefully enabled when you are playing a randomizer.",
                UIWidgets::CheckboxGraphics::Checkmark)) {
                bool hasGiantsKnife = CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BIGGORON);
                bool hasBrokenKnife = CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE);
                bool knifeIsBroken = gSaveContext.swordHealth == 0.0f;

                if (hasGiantsKnife && (hasBrokenKnife != knifeIsBroken)) {
                    func_800849EC(gPlayState);
                }
            }
            UIWidgets::Tooltip("Fixes the Broken Giant's Knife flag not being reset when Medigoron fixes it");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Restoration"))
        {
            UIWidgets::PaddedEnhancementCheckbox("Red Ganon blood", CVAR_ENHANCEMENT("RedGanonBlood"), true, false);
            UIWidgets::Tooltip("Restore the original red blood from NTSC 1.0/1.1. Disable for green blood");
            UIWidgets::PaddedEnhancementCheckbox("Fish while hovering", CVAR_ENHANCEMENT("HoverFishing"), true, false);
            UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows casting the Fishing Rod while using the Hover Boots");
            UIWidgets::PaddedEnhancementCheckbox("N64 Weird Frames", CVAR_ENHANCEMENT("N64WeirdFrames"), true, false);
            UIWidgets::Tooltip("Restores N64 Weird Frames allowing weirdshots to behave the same as N64");
            UIWidgets::PaddedEnhancementCheckbox("Bombchus out of bounds", CVAR_ENHANCEMENT("BombchusOOB"), true, false);
            UIWidgets::Tooltip("Allows bombchus to explode out of bounds\nSimilar to GameCube and Wii VC");
            UIWidgets::PaddedEnhancementCheckbox("Quick Putaway", CVAR_ENHANCEMENT("QuickPutaway"), true, false);
            UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows putting away an item without an animation and performing Putaway Ocarina Items");
            UIWidgets::PaddedEnhancementCheckbox("Restore old Gold Skulltula cutscene", CVAR_ENHANCEMENT("GSCutscene"), true, false);
            UIWidgets::Tooltip("Restore pre-release behavior where defeating a Gold Skulltula will play a cutscene showing it die.");
            UIWidgets::PaddedEnhancementCheckbox("Quick Bongo Kill", CVAR_ENHANCEMENT("QuickBongoKill"), true, false);
            UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows bypassing Bongo Bongo's intro cutscene to quickly kill him");
            UIWidgets::PaddedEnhancementCheckbox("Original RBA Values", CVAR_ENHANCEMENT("RestoreRBAValues"), true, false);
            UIWidgets::Tooltip("Restores the original outcomes when performing Reverse Bottle Adventure.");
            UIWidgets::PaddedEnhancementCheckbox("Early Eyeball Frog", CVAR_ENHANCEMENT("EarlyEyeballFrog"), true, false);
            UIWidgets::Tooltip("Restores a bug from NTSC 1.0/1.1 that allows you to obtain the eyeball frog from King Zora instead of the Zora Tunic by holding shield.");
            UIWidgets::PaddedEnhancementCheckbox("Pulsate boss icon", CVAR_ENHANCEMENT("PulsateBossIcon"), true, false);
            UIWidgets::Tooltip("Restores an unfinished feature to pulsate the boss room icon when you are in the boss room.");
            UIWidgets::PaddedEnhancementSliderInt("Pause Buffer Input Window: %d", "##PauseBufferWindow", CVAR_ENHANCEMENT("PauseBufferWindow"), 0, 40, "", 0, true, true, false);
            UIWidgets::PaddedEnhancementCheckbox("Include held inputs at start of Buffer Input Window", CVAR_ENHANCEMENT("IncludeHeldInputsBufferWindow"), true, false);
            UIWidgets::Tooltip("Typically, inputs that are held prior to the buffer window are not included in the buffer. This setting changes that behavior to include them. This may cause some inputs to be re-triggered undesireably, for instance Z-Targetting something you might not want to.");
            UIWidgets::PaddedEnhancementSliderInt("Simulated Input Lag: %d frames", "##SimulatedInputLag", CVAR_SIMULATED_INPUT_LAG, 0, 6, "", 0, true, true, false);
            UIWidgets::Tooltip("Buffers your inputs to be executed a specified amount of frames later");
            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Extra Modes")) {
            UIWidgets::PaddedText("Mirrored World", true, false);
            if (UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("MirroredWorldMode"), mirroredWorldModes, MIRRORED_WORLD_OFF) && gPlayState != NULL) {
                UpdateMirrorModeState(gPlayState->sceneNum);
            }
            UIWidgets::Tooltip(
                "Mirrors the world horizontally\n\n"
                "- Always: Always mirror the world\n"
                "- Random: Randomly decide to mirror the world on each scene change\n"
                "- Random (Seeded): Scenes are mirrored based on the current randomizer seed/file\n"
                "- Dungeons: Mirror the world in Dungeons\n"
                "- Dungeons (Vanilla): Mirror the world in vanilla Dungeons\n"
                "- Dungeons (MQ): Mirror the world in MQ Dungeons\n"
                "- Dungeons Random: Randomly decide to mirror the world in Dungeons\n"
                "- Dungeons Random (Seeded): Dungeons are mirrored based on the current randomizer seed/file\n"
            );

            UIWidgets::PaddedText("Enemy Randomizer", true, false);
            if (UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("RandomizedEnemies"), enemyRandomizerModes, ENEMY_RANDOMIZER_OFF)) {
                GetSelectedEnemies();
            }
            UIWidgets::Tooltip(
                "Replaces fixed enemies throughout the game with a random enemy. Bosses, mini-bosses and a few specific regular enemies are excluded.\n"
                "Enemies that need more than Deku Nuts + either Deku Sticks or a sword to kill are excluded from spawning in \"clear enemy\" rooms.\n\n"
                "- Random: Enemies are randomized every time you load a room\n"
                "- Random (Seeded): Enemies are randomized based on the current randomizer seed/file\n"
            );
            if (CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemies"), 0) == ENEMY_RANDOMIZER_RANDOM) {
                ImGui::Separator();
                if (ImGui::BeginMenu("Enemy List")) {
                    UIWidgets::PaddedEnhancementCheckbox("Select All Enemies", CVAR_ENHANCEMENT("RandomizedEnemyList.All"), true, false);
                    bool disabledEnemyList = CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemyList.All"), 0);
                    ImGui::Separator();

                    ImGui::BeginDisabled(CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemyList.All"), 0));

                    ImGui::BeginTable("Enemy Table", 2);
                    ImGui::TableNextColumn();
                    for (int i = 0; i < RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE; i++) {
                        if (UIWidgets::PaddedEnhancementCheckbox(enemyNameList[i], enemyCVarList[i], true, false, disabledEnemyList, 
                            "These options are disabled because \"Select All Enemies\" is enabled.",
                            UIWidgets::CheckboxGraphics::Checkmark)) { 
                            GetSelectedEnemies();
                        }
                        if (i == RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE / 2) {
                            ImGui::TableNextColumn();
                        }
                    }
                    ImGui::EndTable();
                    ImGui::EndDisabled();

                    ImGui::EndMenu();
                }
            };
            ImGui::Separator();

            UIWidgets::PaddedEnhancementCheckbox("Randomized Enemy Sizes", CVAR_ENHANCEMENT("RandomizedEnemySizes"), true, false);
            UIWidgets::Tooltip("Enemies and Bosses spawn with random sizes.");

            if (CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemySizes"), 0)) {
                UIWidgets::EnhancementCheckbox("Scale Health with Size", CVAR_ENHANCEMENT("EnemySizeScalesHealth"));
                UIWidgets::Tooltip("Scales normal enemies health with their randomized size. *This will NOT affect bosses*");
            }

            UIWidgets::PaddedEnhancementCheckbox("Ivan the Fairy (Coop Mode)", CVAR_ENHANCEMENT("IvanCoopModeEnabled"), true, false);
            UIWidgets::Tooltip("Enables Ivan the Fairy upon the next map change. Player 2 can control Ivan and "
                                "press the C-Buttons to use items and mess with Player 1!");

            UIWidgets::PaddedEnhancementCheckbox("Rupee Dash Mode", CVAR_ENHANCEMENT("RupeeDash"), true, false);
            UIWidgets::Tooltip("Rupees reduced over time, Link suffers damage when the count hits 0.");

            if (CVarGetInteger(CVAR_ENHANCEMENT("RupeeDash"), 0)) {
                UIWidgets::PaddedEnhancementSliderInt(
                    "Rupee Dash Interval: %d", "##DashInterval", CVAR_ENHANCEMENT("RupeeDashInterval"), 1, 10, "", 5, true, true, false,
                    !CVarGetInteger(CVAR_ENHANCEMENT("RupeeDash"), 0),
                    "This option is disabled because \"Rupee Dash Mode\" is turned off");
                UIWidgets::Tooltip("Interval between Rupee reduction in Rupee Dash Mode");
            }

            UIWidgets::PaddedEnhancementCheckbox("Shadow Tag Mode", CVAR_ENHANCEMENT("ShadowTag"), true, false);
            UIWidgets::Tooltip("A wallmaster follows Link everywhere, don't get caught!");

            UIWidgets::PaddedEnhancementCheckbox("Additional Traps", CVAR_ENHANCEMENT("ExtraTraps.Enabled"), true, false);
            UIWidgets::Tooltip("Enables additional Trap variants.");

            if (CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0)) {
                UIWidgets::PaddedSeparator();
                if (ImGui::BeginMenu("Trap Options")) {
                    ImGui::Text("Tier 1 Traps:");
                    UIWidgets::Spacer(0);
                    UIWidgets::PaddedEnhancementCheckbox("Freeze Traps", CVAR_ENHANCEMENT("ExtraTraps.Ice"), true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Burn Traps", CVAR_ENHANCEMENT("ExtraTraps.Burn"), true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Shock Traps", CVAR_ENHANCEMENT("ExtraTraps.Shock"), true, false);

                    UIWidgets::PaddedSeparator();
                    ImGui::Text("Tier 2 Traps:");
                    UIWidgets::Spacer(0);
                    UIWidgets::PaddedEnhancementCheckbox("Knockback Traps", CVAR_ENHANCEMENT("ExtraTraps.Knockback"), true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Speed Traps", CVAR_ENHANCEMENT("ExtraTraps.Speed"), true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Bomb Traps", CVAR_ENHANCEMENT("ExtraTraps.Bomb"), true, false);

                    UIWidgets::PaddedSeparator();
                    ImGui::Text("Tier 3 Traps:");
                    UIWidgets::Spacer(0);
                    UIWidgets::PaddedEnhancementCheckbox("Void Traps", CVAR_ENHANCEMENT("ExtraTraps.Void"), true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Ammo Traps", CVAR_ENHANCEMENT("ExtraTraps.Ammo"), true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Death Traps", CVAR_ENHANCEMENT("ExtraTraps.Kill"), true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Teleport Traps", CVAR_ENHANCEMENT("ExtraTraps.Teleport"), true, false);

                    ImGui::EndMenu();
                }
            }

            UIWidgets::Spacer(0);
            if (UIWidgets::PaddedEnhancementCheckbox("Hurt Container Mode", CVAR_ENHANCEMENT("HurtContainer"), true, false)) {
                UpdateHurtContainerModeState(CVarGetInteger(CVAR_ENHANCEMENT("HurtContainer"), 0));
            }
            UIWidgets::Tooltip("Changes Heart Piece and Heart Container functionality.\n\n"
                "- Each Heart Container or full Heart Piece reduces Links hearts by 1.\n"
                "- Can be enabled retroactively after a File has already started.");

            ImGui::EndMenu();
        }

        UIWidgets::PaddedSeparator();

        UIWidgets::EnhancementCheckbox("Autosave", CVAR_ENHANCEMENT("Autosave"));
        UIWidgets::Tooltip("Save the game automatically on a 3 minute interval and when soft-resetting the game.\n\n"
                           "The interval autosave will wait if the game is paused in any way (dialogue, pause screen up, cutscenes).");

        UIWidgets::PaddedSeparator(true, true, 2.0f, 2.0f);

        UIWidgets::PaddedText("Boot Sequence", false, true);
        UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("BootSequence"), bootSequenceLabels, BOOTSEQUENCE_DEFAULT);
        UIWidgets::Tooltip("Configure what happens when starting or resetting the game\n\n"
                           "Default: LUS logo -> N64 logo\n"
                           "Authentic: N64 logo only\n"
                           "File Select: Skip to file select menu"
        );

        UIWidgets::PaddedSeparator(true, true, 2.0f, 2.0f);

        ImGui::EndDisabled();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));

        if (mTimeDisplayWindow) {
            if (ImGui::Button(GetWindowButtonText("Additional Timers", CVarGetInteger(CVAR_WINDOW("TimeDisplayEnabled"), 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mTimeDisplayWindow->ToggleVisibility();
            }
        }
        if (mTimeDisplayWindow->IsVisible()) {
            ImGui::SeparatorText("Timer Display Options");

            if (!gPlayState) {
                ImGui::Text("Additional Timer options\n"
                            "available when a file is\n"
                            "loaded...");
            } else {
                if (UIWidgets::PaddedEnhancementSliderFloat("Font Scale: %.2fx", "##FontScale", CVAR_ENHANCEMENT("TimeDisplay.FontScale"), 
                    1.0f, 5.0f, "", 1.0f, false, true, false, true)) {
                    TimeDisplayInitSettings();
                }
                if (UIWidgets::PaddedEnhancementCheckbox("Hide Background", CVAR_ENHANCEMENT("TimeDisplay.ShowWindowBG"), 
                    false, false)) {
                    TimeDisplayInitSettings();
                }
                ImGui::Separator();
                for (auto& timer : timeDisplayList) {
                    if (UIWidgets::PaddedEnhancementCheckbox(timer.timeLabel.c_str(), timer.timeEnable, false, false)) {
                        TimeDisplayUpdateDisplayOptions();
                    }
                }
            }
        }
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(1);

        ImGui::EndMenu();
    }
}

#ifdef ENABLE_REMOTE_CONTROL
void DrawRemoteControlMenu() {
    if (ImGui::BeginMenu("Network")) {
        Sail::Instance->DrawMenu();
        CrowdControl::Instance->DrawMenu();
        ImGui::EndMenu();
    }
}
#endif

extern "C" u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);

void DrawRandomizerMenu() {
    if (ImGui::BeginMenu("Randomizer")) {

        if (ImGui::BeginMenu("Rando Enhancements"))
        {
            UIWidgets::EnhancementCheckbox("Rando-Relevant Navi Hints", CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), false, "", UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip(
                "Replace Navi's overworld quest hints with rando-related gameplay hints."
            );
            UIWidgets::PaddedEnhancementCheckbox("Random Rupee Names", CVAR_RANDOMIZER_ENHANCEMENT("RandomizeRupeeNames"), true, false, false, "", UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip(
                "When obtaining rupees, randomize what the rupee is called in the textbox."
            );
            
            UIWidgets::PaddedEnhancementCheckbox("Use Custom Key Models", CVAR_RANDOMIZER_ENHANCEMENT("CustomKeyModels"), true, false);
            UIWidgets::Tooltip("Use Custom graphics for dungeon keys, Big and Small, so that they can be easily told apart");

            bool disableCompassColors = !DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_SHUFFLE_MAPANDCOMPASS);

            static const char* disableCompassColorsText =
                "This setting is disabled because a savefile is loaded without the compass\n"
                "shuffle settings set to \"Any Dungeon\", \"Overworld\" or \"Anywhere\"";

            if (UIWidgets::PaddedEnhancementCheckbox("Compass Colors Match Dungeon", CVAR_RANDOMIZER_ENHANCEMENT("MatchCompassColors"), true, false,
                                                  disableCompassColors, disableCompassColorsText, UIWidgets::CheckboxGraphics::Cross, true)) {
                PatchCompasses();
            }
            UIWidgets::Tooltip(
                "Matches the color of compasses to the dungeon they belong to. "
                "This helps identify compasses from afar and adds a little bit of flair.\n\nThis only "
                "applies to seeds with compasses shuffled to \"Any Dungeon\", \"Overworld\", or \"Anywhere\".");

            UIWidgets::PaddedEnhancementCheckbox("Quest Item Fanfares", CVAR_RANDOMIZER_ENHANCEMENT("QuestItemFanfares"), true, false);
            UIWidgets::Tooltip(
                "Play unique fanfares when obtaining quest items "
                "(medallions/stones/songs). Note that these fanfares are longer than usual."
            );
            UIWidgets::PaddedEnhancementCheckbox("Mysterious Shuffled Items", CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), true, false);
            UIWidgets::Tooltip(
                "Displays a \"Mystery Item\" model in place of any freestanding/GS/shop items that were shuffled, "
                "and replaces item names for them and scrubs and merchants, regardless of hint settings, "
                "so you never know what you're getting.");
            UIWidgets::PaddedEnhancementCheckbox("Simpler Boss Soul Models",
                                                 CVAR_RANDOMIZER_ENHANCEMENT("SimplerBossSoulModels"), true, false);
            UIWidgets::Tooltip(
                "When shuffling boss souls, they'll appear as a simpler model instead of showing the boss' models."
                "This might make boss souls more distinguishable from a distance, and can help with performance."
            );
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

void SohMenuBar::InitElement() {
    
}

void SohMenuBar::DrawElement() {
    if (ImGui::BeginMenuBar()) {
        static ImVec2 sWindowPadding(8.0f, 8.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, sWindowPadding);

        DrawSettingsMenu();

        ImGui::SetCursorPosY(0.0f);

        DrawEnhancementsMenu();

        ImGui::SetCursorPosY(0.0f);

        #ifdef ENABLE_REMOTE_CONTROL
        DrawRemoteControlMenu();

        ImGui::SetCursorPosY(0.0f);
        #endif

        DrawRandomizerMenu();

        ImGui::PopStyleVar(1);
        ImGui::EndMenuBar();
    }
}
} // namespace SohGui
