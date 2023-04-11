//
//  GameMenuBar.cpp
//  soh
//
//  Created by David Chavez on 24.08.22.
//

#include "GameMenuBar.hpp"

#include <spdlog/spdlog.h>
#include <ImGui/imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui_internal.h>
#include <ImGuiImpl.h>
#include <libultraship/bridge.h>
#include <Hooks.h>
#include <libultraship/libultra/types.h>
#include <libultraship/libultra/pi.h>
#include <libultraship/libultra/sptask.h>

#ifdef __SWITCH__
#include <port/switch/SwitchImpl.h>
#endif

#include "UIWidgets.hpp"
#include "include/global.h"
#include "include/z64audio.h"
#include "soh/SaveManager.h"
#include "OTRGlobals.h"
#include "soh/Enhancements/presets.h"

#ifdef ENABLE_CROWD_CONTROL
#include "Enhancements/crowd-control/CrowdControl.h"
#endif

#include "Enhancements/game-interactor/GameInteractor.h"

#define EXPERIMENTAL() \
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 50, 50, 255)); \
    UIWidgets::Spacer(3.0f); \
    ImGui::Text("Experimental"); \
    ImGui::PopStyleColor(); \
    UIWidgets::PaddedSeparator(false, true);

bool isBetaQuestEnabled = false;

extern "C" {
    void enableBetaQuest() { isBetaQuestEnabled = true; }
    void disableBetaQuest() { isBetaQuestEnabled = false; }
}

enum SeqPlayers {
    /* 0 */ SEQ_BGM_MAIN,
    /* 1 */ SEQ_FANFARE,
    /* 2 */ SEQ_SFX,
    /* 3 */ SEQ_BGM_SUB,
    /* 4 */ SEQ_MAX
};

namespace GameMenuBar {

    // MARK: - Properties

    static const char* chestSizeAndTextureMatchesContentsOptions[4] = { "Disabled", "Both", "Texture Only", "Size Only" };
    static const char* bunnyHoodOptions[3] = { "Disabled", "Faster Run & Longer Jump", "Faster Run" };
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
    static const char* autosaveLabels[6] = { "Off", "New Location + Major Item", "New Location + Any Item", "New Location", "Major Item", "Any Item" };
    static const char* FastFileSelect[5] = { "File N.1", "File N.2", "File N.3", "Zelda Map Select (require OoT Debug Mode)", "File select" };
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

    // MARK: - Helpers

    std::string GetWindowButtonText(const char* text, bool menuOpen) {
        char buttonText[100] = "";
        if(menuOpen) { strcat(buttonText,"> "); }
        strcat(buttonText, text);
        if (!menuOpen) { strcat(buttonText, "  "); }
        return buttonText;
    }

    void UpdateAudio() {
        Audio_SetGameVolume(SEQ_BGM_MAIN, CVarGetFloat("gMainMusicVolume", 1.0f));
        Audio_SetGameVolume(SEQ_BGM_SUB, CVarGetFloat("gSubMusicVolume", 1.0f));
        Audio_SetGameVolume(SEQ_FANFARE, CVarGetFloat("gFanfareVolume", 1.0f));
        Audio_SetGameVolume(SEQ_SFX, CVarGetFloat("gSFXMusicVolume", 1.0f));
    }

    // MARK: - Delegates

    void SetupHooks() {
        Ship::RegisterHook<Ship::AudioInit>(UpdateAudio);
        Ship::RegisterHook<Ship::GfxInit>(UpdateAudio);
    }

    void Draw() {
        if (ImGui::BeginMenu("Settings"))
        {
            if (ImGui::BeginMenu("Audio")) {
                UIWidgets::PaddedEnhancementSliderFloat("Master Volume: %d %%", "##Master_Vol", "gGameMasterVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true);
                if (UIWidgets::PaddedEnhancementSliderFloat("Main Music Volume: %d %%", "##Main_Music_Vol", "gMainMusicVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                    Audio_SetGameVolume(SEQ_BGM_MAIN, CVarGetFloat("gMainMusicVolume", 1.0f));
                }
                if (UIWidgets::PaddedEnhancementSliderFloat("Sub Music Volume: %d %%", "##Sub_Music_Vol", "gSubMusicVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                    Audio_SetGameVolume(SEQ_BGM_SUB, CVarGetFloat("gSubMusicVolume", 1.0f));
                }
                if (UIWidgets::PaddedEnhancementSliderFloat("Sound Effects Volume: %d %%", "##Sound_Effect_Vol", "gSFXMusicVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                    Audio_SetGameVolume(SEQ_SFX, CVarGetFloat("gSFXMusicVolume", 1.0f));
                }
                if (UIWidgets::PaddedEnhancementSliderFloat("Fanfare Volume: %d %%", "##Fanfare_Vol", "gFanfareVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                    Audio_SetGameVolume(SEQ_FANFARE, CVarGetFloat("gFanfareVolume", 1.0f));
                }

                ImGui::Text("Audio API (Needs reload)");
                auto audioBackends = SohImGui::GetAvailableAudioBackends();
                auto currentAudioBackend = SohImGui::GetCurrentAudioBackend();

                if (audioBackends.size() <= 1) {
                    UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
                }
                if (ImGui::BeginCombo("##AApi", currentAudioBackend.second)) {
                    for (uint8_t i = 0; i < audioBackends.size(); i++) {
                        if (ImGui::Selectable(audioBackends[i].second, audioBackends[i] == currentAudioBackend)) {
                            SohImGui::SetCurrentAudioBackend(i, audioBackends[i]);
                        }
                    }

                    ImGui::EndCombo();
                }
                if (audioBackends.size() <= 1) {
                    UIWidgets::ReEnableComponent("");
                }

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Controller")) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 (12.0f, 6.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                if (ImGui::Button(GetWindowButtonText("Controller Configuration", CVarGetInteger("gControllerConfigurationEnabled", 0)).c_str(), ImVec2 (-1.0f, 0.0f)))
                {
                    bool currentValue = CVarGetInteger("gControllerConfigurationEnabled", 0);
                    CVarSetInteger("gControllerConfigurationEnabled", !currentValue);
                    SohImGui::RequestCvarSaveOnNextTick();
                    SohImGui::ToggleInputEditorWindow(CVarGetInteger("gControllerConfigurationEnabled", 0));
                }
                UIWidgets::PaddedSeparator();
                ImGui::PopStyleColor(1);
                ImGui::PopStyleVar(3);
            #ifndef __SWITCH__
                UIWidgets::EnhancementCheckbox("Menubar Controller Navigation", "gControlNav");
                UIWidgets::Tooltip("Allows controller navigation of the SOH menu bar (Settings, Enhancements,...)\nCAUTION: This will disable game inputs while the menubar is visible.\n\nD-pad to move between items, A to select, and X to grab focus on the menu bar");
            #endif
                UIWidgets::PaddedEnhancementCheckbox("Show Inputs", "gInputEnabled", true, false);
                UIWidgets::Tooltip("Shows currently pressed inputs on the bottom right of the screen");
                UIWidgets::PaddedEnhancementSliderFloat("Input Scale: %.1f", "##Input", "gInputScale", 1.0f, 3.0f, "", 1.0f, false, true, true, false);
                UIWidgets::Tooltip("Sets the on screen size of the displayed inputs from the Show Inputs setting");
                UIWidgets::PaddedEnhancementSliderInt("Simulated Input Lag: %d frames", "##SimulatedInputLag", "gSimulatedInputLag", 0, 6, "", 0, true, true, false);
                UIWidgets::Tooltip("Buffers your inputs to be executed a specified amount of frames later");

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Graphics")) {
            #ifndef __APPLE__
                UIWidgets::EnhancementSliderFloat("Internal Resolution: %d %%", "##IMul", "gInternalResolution", 0.5f, 2.0f, "", 1.0f, true);
                UIWidgets::Tooltip("Multiplies your output resolution by the value inputted, as a more intensive but effective form of anti-aliasing");
                SohImGui::SetResolutionMultiplier(CVarGetFloat("gInternalResolution", 1));
            #endif
            #ifndef __WIIU__
                UIWidgets::PaddedEnhancementSliderInt("MSAA: %d", "##IMSAA", "gMSAAValue", 1, 8, "", 1, true, true, false);
                UIWidgets::Tooltip("Activates multi-sample anti-aliasing when above 1x up to 8x for 8 samples for every pixel");
                SohImGui::SetMSAALevel(CVarGetInteger("gMSAAValue", 1));
            #endif

                if (SohImGui::WindowBackend() == SohImGui::Backend::DX11) {
                    UIWidgets::PaddedEnhancementSliderInt(CVarGetInteger("gExtraLatencyThreshold", 80) == 0 ? "Jitter fix: Off" : "Jitter fix: >= %d FPS",
                        "##ExtraLatencyThreshold", "gExtraLatencyThreshold", 0, 360, "", 80, true, true, false);
                    UIWidgets::Tooltip("When Interpolation FPS setting is at least this threshold, add one frame of input lag (e.g. 16.6 ms for 60 FPS) in order to avoid jitter. This setting allows the CPU to work on one frame while GPU works on the previous frame.\nThis setting should be used when your computer is too slow to do CPU + GPU work in time.");
                }

                ImGui::Text("Renderer API (Needs reload)");
                auto renderingBackends = SohImGui::GetAvailableRenderingBackends();
                auto currentRenderingBackend = SohImGui::GetCurrentRenderingBackend();

                if (renderingBackends.size() <= 1) {
                    UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
                }
                if (ImGui::BeginCombo("##RApi", currentRenderingBackend.second)) {
                    for (uint8_t i = 0; i < renderingBackends.size(); i++) {
                        if (ImGui::Selectable(renderingBackends[i].second, renderingBackends[i] == currentRenderingBackend)) {
                            SohImGui::SetCurrentRenderingBackend(i, renderingBackends[i]);
                        }
                    }

                    ImGui::EndCombo();
                }
                if (renderingBackends.size() <= 1) {
                    UIWidgets::ReEnableComponent("");
                }

                if (SohImGui::SupportsWindowedFullscreen()) {
                    UIWidgets::PaddedEnhancementCheckbox("Windowed fullscreen", "gSdlWindowedFullscreen", true, false);
                }

                if (SohImGui::SupportsViewports()) {
                    UIWidgets::PaddedEnhancementCheckbox("Allow multi-windows", "gEnableMultiViewports", true, false);
                    UIWidgets::Tooltip("Allows windows to be able to be dragged off of the main game window. Requires a reload to take effect.");
                }

                EXPERIMENTAL();

                // If more filters are added to LUS, make sure to add them to the filters list here
                ImGui::Text("Texture Filter (Needs reload)");
                const char* filters[] = { SohImGui::GetSupportedTextureFilters()[0],
                                          SohImGui::GetSupportedTextureFilters()[1],
                                          SohImGui::GetSupportedTextureFilters()[2] };
                UIWidgets::EnhancementCombobox("gTextureFilter", filters, 0);

                UIWidgets::Spacer(0);

                SohImGui::DrawSettings();

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Languages")) {
                UIWidgets::PaddedEnhancementCheckbox("Translate Title Screen", "gTitleScreenTranslation");
                if (UIWidgets::EnhancementRadioButton("English", "gLanguages", LANGUAGE_ENG)) {
                    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
                }
                if (UIWidgets::EnhancementRadioButton("German", "gLanguages", LANGUAGE_GER)) {
                    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
                }
                if (UIWidgets::EnhancementRadioButton("French", "gLanguages", LANGUAGE_FRA)) {
                    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
                }
                ImGui::EndMenu();
            }
            
            UIWidgets::Spacer(0);
            
            if (ImGui::BeginMenu("Accessibility")) {
            #if defined(_WIN32) || defined(__APPLE__)
                UIWidgets::PaddedEnhancementCheckbox("Text to Speech", "gA11yTTS");
                UIWidgets::Tooltip("Enables text to speech for in game dialog");
            #endif
                UIWidgets::PaddedEnhancementCheckbox("Disable Idle Camera Re-Centering", "gA11yDisableIdleCam");
                UIWidgets::Tooltip("Disables the automatic re-centering of the camera when idle.");
                
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        ImGui::SetCursorPosY(0.0f);

        if (ImGui::BeginMenu("Enhancements"))
        {
            DrawPresetSelector(PRESET_TYPE_ENHANCEMENTS);

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Gameplay"))
            {
                if (ImGui::BeginMenu("Time Savers"))
                {
                    UIWidgets::PaddedEnhancementSliderInt("Text Speed: %dx", "##TEXTSPEED", "gTextSpeed", 1, 5, "", 1, true, false, true);
                    UIWidgets::PaddedEnhancementSliderInt("King Zora Speed: %dx", "##MWEEPSPEED", "gMweepSpeed", 1, 5, "", 1, true, false, true);
                    UIWidgets::PaddedEnhancementSliderInt("Biggoron Forge Time: %d days", "##FORGETIME", "gForgeTime", 0, 3, "", 3, true, false, true);
                    UIWidgets::Tooltip("Allows you to change the number of days it takes for Biggoron to forge the Biggoron Sword");
                    UIWidgets::PaddedEnhancementSliderInt("Vine/Ladder Climb speed +%d", "##CLIMBSPEED", "gClimbSpeed", 0, 12, "", 0, true, false, true);
                    UIWidgets::PaddedEnhancementSliderInt("Block pushing speed +%d", "##BLOCKSPEED", "gFasterBlockPush", 0, 5, "", 0, true, false, true);
                    UIWidgets::PaddedEnhancementCheckbox("Faster Heavy Block Lift", "gFasterHeavyBlockLift", true, false);
                    UIWidgets::Tooltip("Speeds up lifting silver rocks and obelisks");
                    UIWidgets::PaddedEnhancementCheckbox("Link as default file name", "gLinkDefaultName", true, false);
                    UIWidgets::Tooltip("Allows you to have \"Link\" as a premade file name");
                    UIWidgets::PaddedEnhancementCheckbox("No Forced Navi", "gNoForcedNavi", true, false);
                    UIWidgets::Tooltip("Prevent forced Navi conversations");
                    UIWidgets::PaddedEnhancementCheckbox("No Skulltula Freeze", "gSkulltulaFreeze", true, false);
                    UIWidgets::Tooltip("Stops the game from freezing the player when picking up Gold Skulltulas");
                    UIWidgets::PaddedEnhancementCheckbox("Fast Chests", "gFastChests", true, false);
                    UIWidgets::Tooltip("Kick open every chest");
                    UIWidgets::PaddedText("Chest size & texture matches contents", true, false);
                    if (UIWidgets::EnhancementCombobox("gChestSizeAndTextureMatchesContents", chestSizeAndTextureMatchesContentsOptions, 0)) {
                        if (CVarGetInteger("gChestSizeAndTextureMatchesContents", 0) == 0) {
                            CVarSetInteger("gChestSizeDependsStoneOfAgony", 0);
                        }
                    }
                    UIWidgets::Tooltip(
                        "Chest sizes and textures are changed to help identify the item inside.\n"
                        " - Major items: Large gold chests\n"
                        " - Lesser items: Large brown chests\n"
                        " - Junk items: Small brown chests\n"
                        " - Small keys: Small silver chest\n"
                        " - Boss keys: Vanilla size and texture\n"
                        " - Skulltula Tokens: Small skulltula chest\n"
                    );
                    if (CVarGetInteger("gChestSizeAndTextureMatchesContents", 0) > 0) {
                        UIWidgets::PaddedEnhancementCheckbox("Chests of Agony", "gChestSizeDependsStoneOfAgony", true, false);
                        UIWidgets::Tooltip("Only change the size/texture of chests if you have the Stone of Agony.");
                    }
                    UIWidgets::PaddedEnhancementCheckbox("Skip Pickup Messages", "gFastDrops", true, false);
                    UIWidgets::Tooltip("Skip pickup messages for new consumable items and bottle swipes");
                    UIWidgets::PaddedEnhancementCheckbox("Ask to Equip New Items", "gAskToEquip", true, false);
                    UIWidgets::Tooltip("Adds a prompt to equip newly-obtained swords, shields and tunics");
                    UIWidgets::PaddedEnhancementCheckbox("Better Owl", "gBetterOwl", true, false);
                    UIWidgets::Tooltip("The default response to Kaepora Gaebora is always that you understood what he said");
                    UIWidgets::PaddedEnhancementCheckbox("Fast Ocarina Playback", "gFastOcarinaPlayback", true, false);
                    bool forceSkipScarecrow = gSaveContext.n64ddFlag &&
                        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SKIP_SCARECROWS_SONG);
                    static const char* forceSkipScarecrowText =
                        "This setting is forcefully enabled because a savefile\nwith \"Skip Scarecrow Song\" is loaded";
                    UIWidgets::Tooltip("Skip the part where the Ocarina playback is called when you play a song");
                    UIWidgets::PaddedEnhancementCheckbox("Skip Scarecrow Song", "gSkipScarecrow", true, false,
                                                         forceSkipScarecrow, forceSkipScarecrowText, UIWidgets::CheckboxGraphics::Checkmark);
                    UIWidgets::Tooltip("Pierre appears when Ocarina is pulled out. Requires learning scarecrow song.");
                    UIWidgets::PaddedEnhancementCheckbox("Remember Save Location", "gRememberSaveLocation", true, false);
                    UIWidgets::Tooltip("When loading a save, places Link at the last entrance he went through.\n"
                            "This doesn't work if the save was made in a grotto.");
                    UIWidgets::PaddedEnhancementCheckbox("Skip Magic Arrow Equip Animation", "gSkipArrowAnimation", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Skip save confirmation", "gSkipSaveConfirmation", true, false);
                    UIWidgets::Tooltip("Skip the \"Game saved.\" confirmation screen");
                    UIWidgets::PaddedEnhancementCheckbox("Exit Market at Night", "gMarketSneak", true, false);
                    UIWidgets::Tooltip("Allows exiting Hyrule Castle Market Town to Hyrule Field at night by speaking "
                    "to the guard next to the gate.");
                    UIWidgets::PaddedEnhancementCheckbox("Faster Farore's Wind", "gFastFarores", true, false);
                    UIWidgets::Tooltip("Greatly decreases cast time of Farore's Wind magic spell.");
                    UIWidgets::PaddedEnhancementCheckbox("Nighttime GS Always Spawn", "gNightGSAlwaysSpawn", true, false);
                    UIWidgets::Tooltip("Nighttime Skulltulas will spawn during both day and night.");
                    UIWidgets::PaddedEnhancementCheckbox("Dampe Appears All Night", "gDampeAllNight", true, false);
                    UIWidgets::Tooltip("Makes Dampe appear anytime during the night, not just his usual working hours.");
                    UIWidgets::PaddedEnhancementCheckbox("Time Travel with the Song of Time", "gTimeTravel", true, false);
                    UIWidgets::Tooltip("Allows Link to freely change age by playing the Song of Time.\n"
                        "Time Blocks can still be used properly.\n\n"
                        "Requirements:\n"
                        "- Obtained the Ocarina of Time\n"
                        "- Obtained the Song of Time\n"
                        "- Obtained the Master Sword\n"
                        "- Not within range of Time Block\n"
                        "- Not within range of Ocarina playing spots");
                    ImGui::EndMenu();
                }

                UIWidgets::Spacer(0);

                if (ImGui::BeginMenu("Items"))
                {
                    UIWidgets::PaddedEnhancementCheckbox("Instant Putaway", "gInstantPutaway", true, false);
                    UIWidgets::Tooltip("Allow Link to put items away without having to wait around");
                    UIWidgets::PaddedEnhancementCheckbox("Instant Boomerang Recall", "gFastBoomerang", true, false);
                    UIWidgets::Tooltip("Instantly return the boomerang to Link by pressing its item button while it's in the air");
                    UIWidgets::PaddedEnhancementCheckbox("Prevent Dropped Ocarina Inputs", "gDpadNoDropOcarinaInput", true, false);
                    UIWidgets::Tooltip("Prevent dropping inputs when playing the ocarina quickly");
                    UIWidgets::PaddedText("Bunny Hood Effect", true, false);
                    UIWidgets::EnhancementCombobox("gMMBunnyHood", bunnyHoodOptions, 0);
                    UIWidgets::Tooltip(
                        "Wearing the Bunny Hood grants a speed increase like in Majora's Mask. The longer jump option is not accounted for in randomizer logic.\n\n"
                        "Also disables NPC's reactions to wearing the Bunny Hood."
                    );
                    UIWidgets::PaddedEnhancementCheckbox("Mask Select in Inventory", "gMaskSelect", true, false);
                    UIWidgets::Tooltip("After completing the mask trading sub-quest, press A and any direction on the mask slot to change masks");
                    UIWidgets::PaddedEnhancementCheckbox("Nuts explode bombs", "gNutsExplodeBombs", true, false);
                    UIWidgets::Tooltip("Makes nuts explode bombs, similar to how they interact with bombchus. This does not affect bombflowers.");
                    UIWidgets::PaddedEnhancementCheckbox("Equip Multiple Arrows at Once", "gSeparateArrows", true, false);
                    UIWidgets::Tooltip("Allow the bow and magic arrows to be equipped at the same time on different slots");
                    UIWidgets::PaddedEnhancementCheckbox("Bow as Child/Slingshot as Adult", "gBowSlingShotAmmoFix", true, false);
                    UIWidgets::Tooltip("Allows child to use bow with arrows.\nAllows adult to use slingshot with seeds.\n\nRequires glitches or 'Timeless Equipment' cheat to equip.");
                    UIWidgets::PaddedEnhancementCheckbox("Better Farore's Wind", "gBetterFW", true, false);
                    UIWidgets::Tooltip("Helps FW persist between ages, gives child and adult separate FW points, and can be used in more places.");
                    UIWidgets::PaddedEnhancementCheckbox("Static Explosion Radius", "gStaticExplosionRadius", true, false);
                    UIWidgets::Tooltip("Explosions are now a static size, like in Majora's Mask and OoT3D. Makes bombchu hovering much easier.");
                    UIWidgets::PaddedEnhancementCheckbox("Prevent Bombchus Forcing First-Person", "gDisableFirstPersonChus", true, false);
                    UIWidgets::Tooltip("Prevent bombchus from forcing the camera into first-person mode when released.");
                    ImGui::EndMenu();
                }

                UIWidgets::Spacer(0);

                if (ImGui::BeginMenu("Difficulty Options"))
                {
                    ImGui::Text("Damage Multiplier");
                    UIWidgets::EnhancementCombobox("gDamageMul", allPowers, 0);
                    UIWidgets::Tooltip(
                        "Modifies all sources of damage not affected by other sliders\n"
                        "2x: Can survive all common attacks from the start of the game\n"
                        "4x: Dies in 1 hit to any substantial attack from the start of the game\n"
                        "8x: Can only survive trivial damage from the start of the game\n"
                        "16x: Can survive all common attacks with max health without double defense\n"
                        "32x: Can survive all common attacks with max health and double defense\n"
                        "64x: Can survive trivial damage with max health without double defense\n"
                        "128x: Can survive trivial damage with max health and double defense\n"
                        "256x: Cannot survive damage"
                    );
                    UIWidgets::PaddedText("Fall Damage Multiplier", true, false);
                    UIWidgets::EnhancementCombobox("gFallDamageMul", subPowers, 0);
                    UIWidgets::Tooltip(
                        "Modifies all fall damage\n"
                        "2x: Can survive all fall damage from the start of the game\n"
                        "4x: Can only survive short fall damage from the start of the game\n"
                        "8x: Cannot survive any fall damage from the start of the game\n"
                        "16x: Can survive all fall damage with max health without double defense\n"
                        "32x: Can survive all fall damage with max health and double defense\n"
                        "64x: Can survive short fall damage with double defense\n"
                        "128x: Cannot survive fall damage"
                    );
                    UIWidgets::PaddedText("Void Damage Multiplier", true, false);
                    UIWidgets::EnhancementCombobox("gVoidDamageMul", subSubPowers, 0);
                    UIWidgets::Tooltip(
                        "Modifies damage taken after falling into a void\n"
                        "2x: Can survive void damage from the start of the game\n"
                        "4x: Cannot survive void damage from the start of the game\n"
                        "8x: Can survive void damage twice with max health without double defense\n"
                        "16x: Can survive void damage with max health without double defense\n"
                        "32x: Can survive void damage with max health and double defense\n"
                        "64x: Cannot survive void damage"
                    );
                    UIWidgets::PaddedText("Bonk Damage Multiplier", true, false);
                    UIWidgets::EnhancementCombobox("gBonkDamageMul", bonkDamageValues, 0);
                    UIWidgets::Tooltip("Modifies damage taken after bonking.");
                    UIWidgets::PaddedEnhancementCheckbox("Spawn with full health", "gFullHealthSpawn", true, false);
                    UIWidgets::Tooltip("Respawn with full health instead of 3 Hearts");
                    UIWidgets::PaddedEnhancementCheckbox("No Random Drops", "gNoRandomDrops", true, false);
                    UIWidgets::Tooltip("Disables random drops, except from the Goron Pot, Dampe, and bosses");
                    bool forceEnableBombchuDrops = gSaveContext.n64ddFlag &&
                        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_ENABLE_BOMBCHU_DROPS) == 1;
                    static const char* forceEnableBombchuDropsText =
                        "This setting is forcefully enabled because a savefile\nwith \"Enable Bombchu Drops\" is loaded.";
                    UIWidgets::PaddedEnhancementCheckbox("Enable Bombchu Drops", "gBombchuDrops", true, false,
                                                         forceEnableBombchuDrops, forceEnableBombchuDropsText, UIWidgets::CheckboxGraphics::Checkmark);
                    UIWidgets::Tooltip("Bombchus will sometimes drop in place of bombs");
                    UIWidgets::PaddedEnhancementCheckbox("No Heart Drops", "gNoHeartDrops", true, false);
                    UIWidgets::Tooltip("Disables heart drops, but not heart placements, like from a Deku Scrub running off\nThis simulates Hero Mode from other games in the series");
                    UIWidgets::PaddedEnhancementCheckbox("Hyper Bosses", "gHyperBosses", true, false);
                    UIWidgets::Tooltip("All major bosses move and act twice as fast.");
                    UIWidgets::PaddedEnhancementCheckbox("Always Win Goron Pot", "gGoronPot", true, false);
                    UIWidgets::Tooltip("Always get the heart piece/purple rupee from the spinning Goron pot");
                    UIWidgets::PaddedEnhancementCheckbox("Always Win Dampe Digging Game", "gDampeWin", true, false, SaveManager::Instance->IsRandoFile(),
                                                         "This setting is always enabled in randomizer files", UIWidgets::CheckboxGraphics::Checkmark);
                    UIWidgets::Tooltip("Always win the heart piece/purple rupee on the first dig in Dampe's grave digging game, just like in rando\nIn a rando file, this is unconditionally enabled");
                    UIWidgets::Spacer(0);

                    if (ImGui::BeginMenu("Potion Values"))
                    {
                        UIWidgets::EnhancementCheckbox("Change Red Potion Effect", "gRedPotionEffect");
                        UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Red Potions");
                        bool disabledRedPotion = !CVarGetInteger("gRedPotionEffect", 0);
                        static const char* disabledTooltipRedPotion = "This option is disabled because \"Change Red Potion Effect\" is turned off";
                        UIWidgets::EnhancementSliderInt("Red Potion Health: %d", "##REDPOTIONHEALTH", "gRedPotionHealth", 1, 100, "", 0, true, disabledRedPotion, disabledTooltipRedPotion);
                        UIWidgets::Tooltip("Changes the amount of health restored by Red Potions");
                        UIWidgets::EnhancementCheckbox("Red Potion Percent Restore", "gRedPercentRestore", disabledRedPotion, disabledTooltipRedPotion);
                        UIWidgets::Tooltip("Toggles from Red Potions restoring a fixed amount of health to a percent of the player's current max health");

                        ImGui::Separator();

                        UIWidgets::EnhancementCheckbox("Change Green Potion Effect", "gGreenPotionEffect");
                        UIWidgets::Tooltip("Enable the following changes to the amount of mana restored by Green Potions");
                        bool disabledGreenPotion = !CVarGetInteger("gGreenPotionEffect", 0);
                        static const char* disabledTooltipGreenPotion = "This option is disabled because \"Change Green Potion Effect\" is turned off";
                        UIWidgets::EnhancementSliderInt("Green Potion Mana: %d", "##GREENPOTIONMANA", "gGreenPotionMana", 1, 100, "", 0, true, disabledGreenPotion, disabledTooltipGreenPotion);
                        UIWidgets::Tooltip("Changes the amount of mana restored by Green Potions, base max mana is 48, max upgraded mana is 96");
                        UIWidgets::EnhancementCheckbox("Green Potion Percent Restore", "gGreenPercentRestore", disabledGreenPotion, disabledTooltipGreenPotion);
                        UIWidgets::Tooltip("Toggles from Green Potions restoring a fixed amount of mana to a percent of the player's current max mana");

                        ImGui::Separator();

                        UIWidgets::EnhancementCheckbox("Change Blue Potion Effects", "gBluePotionEffects");
                        UIWidgets::Tooltip("Enable the following changes to the amount of health and mana restored by Blue Potions");
                        bool disabledBluePotion = !CVarGetInteger("gBluePotionEffects", 0);
                        static const char* disabledTooltipBluePotion = "This option is disabled because \"Change Blue Potion Effects\" is turned off";
                        UIWidgets::EnhancementSliderInt("Blue Potion Health: %d", "##BLUEPOTIONHEALTH", "gBluePotionHealth", 1, 100, "", 0, true, disabledBluePotion, disabledTooltipBluePotion);
                        UIWidgets::Tooltip("Changes the amount of health restored by Blue Potions");
                        UIWidgets::EnhancementCheckbox("Blue Potion Health Percent Restore", "gBlueHealthPercentRestore", disabledBluePotion, disabledTooltipBluePotion);
                        UIWidgets::Tooltip("Toggles from Blue Potions restoring a fixed amount of health to a percent of the player's current max health");

                        ImGui::Separator();

                        UIWidgets::EnhancementSliderInt("Blue Potion Mana: %d", "##BLUEPOTIONMANA", "gBluePotionMana", 1, 100, "", 0, true, disabledBluePotion, disabledTooltipBluePotion);
                        UIWidgets::Tooltip("Changes the amount of mana restored by Blue Potions, base max mana is 48, max upgraded mana is 96");
                        UIWidgets::EnhancementCheckbox("Blue Potion Mana Percent Restore", "gBlueManaPercentRestore", disabledBluePotion, disabledTooltipBluePotion);
                        UIWidgets::Tooltip("Toggles from Blue Potions restoring a fixed amount of mana to a percent of the player's current max mana");

                        ImGui::Separator();

                        UIWidgets::EnhancementCheckbox("Change Milk Effect", "gMilkEffect");
                        UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Milk");
                        bool disabledMilk = !CVarGetInteger("gMilkEffect", 0);
                        static const char* disabledTooltipMilk = "This option is disabled because \"Change Milk Effect\" is turned off";
                        UIWidgets::EnhancementSliderInt("Milk Health: %d", "##MILKHEALTH", "gMilkHealth", 1, 100, "", 0, true, disabledMilk, disabledTooltipMilk);
                        UIWidgets::Tooltip("Changes the amount of health restored by Milk");
                        UIWidgets::EnhancementCheckbox("Milk Percent Restore", "gMilkPercentRestore", disabledMilk, disabledTooltipMilk);
                        UIWidgets::Tooltip("Toggles from Milk restoring a fixed amount of health to a percent of the player's current max health");

                        ImGui::Separator();

                        UIWidgets::EnhancementCheckbox("Separate Half Milk Effect", "gSeparateHalfMilkEffect", disabledMilk, disabledTooltipMilk);
                        UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Half Milk\nIf this is disabled, Half Milk will behave the same as Full Milk.");
                        bool disabledHalfMilk = disabledMilk || !CVarGetInteger("gSeparateHalfMilkEffect", 0);
                        static const char* disabledTooltipHalfMilk = "This option is disabled because \"Separate Half Milk Effect\" is turned off";
                        UIWidgets::EnhancementSliderInt("Half Milk Health: %d", "##HALFMILKHEALTH", "gHalfMilkHealth", 1, 100, "", 0, true, disabledHalfMilk, disabledTooltipHalfMilk);
                        UIWidgets::Tooltip("Changes the amount of health restored by Half Milk");
                        UIWidgets::EnhancementCheckbox("Half Milk Percent Restore", "gHalfMilkPercentRestore", disabledHalfMilk, disabledTooltipHalfMilk);
                        UIWidgets::Tooltip("Toggles from Half Milk restoring a fixed amount of health to a percent of the player's current max health");

                        ImGui::Separator();

                        UIWidgets::EnhancementCheckbox("Change Fairy Effect", "gFairyEffect");
                        UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Fairies");
                        bool disabledFairy = !CVarGetInteger("gFairyEffect", 0);
                        static const char* disabledTooltipFairy = "This option is disabled because \"Change Fairy Effect\" is turned off";
                        UIWidgets::EnhancementSliderInt("Fairy: %d", "##FAIRYHEALTH", "gFairyHealth", 1, 100, "", 0, true, disabledFairy, disabledTooltipFairy);
                        UIWidgets::Tooltip("Changes the amount of health restored by Fairies");
                        UIWidgets::EnhancementCheckbox("Fairy Percent Restore", "gFairyPercentRestore", disabledFairy, disabledTooltipFairy);
                        UIWidgets::Tooltip("Toggles from Fairies restoring a fixed amount of health to a percent of the player's current max health");

                        ImGui::Separator();

                        UIWidgets::EnhancementCheckbox("Change Fairy Revive Effect", "gFairyReviveEffect");
                        UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Fairy Revivals");
                        bool disabledFairyRevive = !CVarGetInteger("gFairyReviveEffect", 0);
                        static const char* disabledTooltipFairyRevive = "This option is disabled because \"Change Fairy Revive Effect\" is turned off";
                        UIWidgets::EnhancementSliderInt("Fairy Revival: %d", "##FAIRYREVIVEHEALTH", "gFairyReviveHealth", 1, 100, "", 0, true, disabledFairyRevive, disabledTooltipFairyRevive);
                        UIWidgets::Tooltip("Changes the amount of health restored by Fairy Revivals");
                        UIWidgets::EnhancementCheckbox("Fairy Revive Percent Restore", "gFairyRevivePercentRestore", disabledFairyRevive, disabledTooltipFairyRevive);
                        UIWidgets::Tooltip("Toggles from Fairy Revivals restoring a fixed amount of health to a percent of the player's current max health");

                        ImGui::EndMenu();
                    }

                    UIWidgets::Spacer(0);

                    if (ImGui::BeginMenu("Shooting Gallery")) {
                        UIWidgets::EnhancementCheckbox("Customize Behavior", "gCustomizeShootingGallery");
                        UIWidgets::Tooltip("Turn on/off changes to the shooting gallery behavior");
                        bool disabled = !CVarGetInteger("gCustomizeShootingGallery", 0);
                        static const char* disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off";
                        UIWidgets::PaddedEnhancementCheckbox("Instant Win", "gInstantShootingGalleryWin", true, false, disabled, disabledTooltip);
                        UIWidgets::Tooltip("Skips the shooting gallery minigame");
                        UIWidgets::PaddedEnhancementCheckbox("No Rupee Randomization", "gConstantAdultGallery", true, false, disabled, disabledTooltip);
                        UIWidgets::Tooltip("Forces the rupee order to not be randomized as adult, making it the same as chlid");
                        UIWidgets::PaddedEnhancementSliderInt("Child Starting Ammunition: %d", "##cShootingGalleryAmmunition", "gChildShootingGalleryAmmunition", 10, 30, "", 15, true, true, false, disabled, disabledTooltip);
                        UIWidgets::Tooltip("The ammunition at the start of the shooting gallery minigame as a child");
                        UIWidgets::PaddedEnhancementSliderInt("Adult Starting Ammunition: %d", "##aShootingGalleryAmmunition", "gAdultShootingGalleryAmmunition", 10, 30, "", 15, true, true, false, disabled, disabledTooltip);
                        UIWidgets::Tooltip("The ammunition at the start of the shooting gallery minigame as an adult");
                        ImGui::EndMenu();
                    }

                    UIWidgets::Spacer(0);

                    if (ImGui::BeginMenu("Bombchu Bowling")) {
                        UIWidgets::EnhancementCheckbox("Customize Behavior", "gCustomizeBombchuBowling");
                        UIWidgets::Tooltip("Turn on/off changes to the bombchu bowling behavior");
                        bool disabled = CVarGetInteger("gCustomizeBombchuBowling", 0) == 0;
                        static const char* disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off";
                        UIWidgets::PaddedEnhancementCheckbox("Remove Small Cucco", "gBombchuBowlingNoSmallCucco", true, false, disabled, disabledTooltip);
                        UIWidgets::Tooltip("Prevents the small cucco from appearing in the bombchu bowling minigame");
                        UIWidgets::PaddedEnhancementCheckbox("Remove Big Cucco", "gBombchuBowlingNoBigCucco", true, false, disabled, disabledTooltip);
                        UIWidgets::Tooltip("Prevents the big cucco from appearing in the bombchu bowling minigame");
                        UIWidgets::PaddedEnhancementSliderInt("Bombchu Count: %d", "##cBombchuBowlingAmmunition", "gBombchuBowlingAmmunition", 3, 20, "", 10, true, true, false, disabled, disabledTooltip);
                        UIWidgets::Tooltip("The number of bombchus available at the start of the bombchu bowling minigame");
                        ImGui::EndMenu();
                    }

                    UIWidgets::Spacer(0);

                    if (ImGui::BeginMenu("Fishing")) {
                        UIWidgets::EnhancementCheckbox("Customize Behavior", "gCustomizeFishing");
                        UIWidgets::Tooltip("Turn on/off changes to the fishing behavior");
                        bool disabled = !CVarGetInteger("gCustomizeFishing", 0);
                        static const char* disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off";
                        UIWidgets::PaddedEnhancementCheckbox("Instant Fishing", "gInstantFishing", true, false, disabled, disabledTooltip);
                        UIWidgets::Tooltip("All fish will be caught instantly");
                        UIWidgets::PaddedEnhancementCheckbox("Guarantee Bite", "gGuaranteeFishingBite", true, false, disabled, disabledTooltip);
                        UIWidgets::Tooltip("When a line is stable, guarantee bite. Otherwise use default logic");
                        UIWidgets::PaddedEnhancementCheckbox("Fish Never Escape", "gFishNeverEscape", true, false, disabled, disabledTooltip);
                        UIWidgets::Tooltip("Once a hook has been set, fish will never let go while being reeled in.");
                        UIWidgets::PaddedEnhancementSliderInt("Child Minimum Weight: %d", "##cMinimumWeight", "gChildMinimumWeightFish", 3, 10, "", 10, true, true, false, disabled, disabledTooltip);
                        UIWidgets::Tooltip("The minimum weight for the unique fishing reward as a child");
                        UIWidgets::PaddedEnhancementSliderInt("Adult Minimum Weight: %d", "##aMinimumWeight", "gAdultMinimumWeightFish", 6, 13, "", 13, true, true, false, disabled, disabledTooltip);
                        UIWidgets::Tooltip("The minimum weight for the unique fishing reward as an adult");
                        ImGui::EndMenu();
                    }

                    UIWidgets::Spacer(0);

                    UIWidgets::PaddedEnhancementCheckbox("Rupee Dash Mode", "gRupeeDash", true, false);
                    UIWidgets::Tooltip("Rupees reduced over time, Link suffers damage when the count hits 0.");
                    UIWidgets::PaddedEnhancementSliderInt("Rupee Dash Interval: %d", "##DashInterval", "gDashInterval", 3, 5, "", 5, true, true, false,
                        !CVarGetInteger("gRupeeDash", 0), "This option is disabled because \"Rupee Dash Mode\" is turned off");
                    UIWidgets::Tooltip("Interval between Rupee reduction in Rupee Dash Mode");

                    ImGui::EndMenu();
                }

                UIWidgets::Spacer(0);

                if (ImGui::BeginMenu("Reduced Clutter"))
                {
                    UIWidgets::EnhancementCheckbox("Mute Low HP Alarm", "gLowHpAlarm");
                    UIWidgets::Tooltip("Disable the low HP beeping sound");
                    UIWidgets::PaddedEnhancementCheckbox("Minimal UI", "gMinimalUI", true, false);
                    UIWidgets::Tooltip("Hides most of the UI when not needed\nNote: Doesn't activate until after loading a new scene");
                    UIWidgets::PaddedEnhancementCheckbox("Disable Navi Call Audio", "gDisableNaviCallAudio", true, false);
                    UIWidgets::Tooltip("Disables the voice audio when Navi calls you");
                    UIWidgets::PaddedEnhancementCheckbox("Disable Hot/Underwater Warning Text", "gDisableWarningText", true, false);
                    UIWidgets::Tooltip("Disables warning text when you don't have on the Goron/Zora Tunic in Hot/Underwater conditions.");

                    ImGui::EndMenu();
                }

                UIWidgets::Spacer(0);

                UIWidgets::EnhancementCheckbox("Visual Stone of Agony", "gVisualAgony");
                UIWidgets::Tooltip("Displays an icon and plays a sound when Stone of Agony should be activated, for those without rumble");
                UIWidgets::PaddedEnhancementCheckbox("Assignable Tunics and Boots", "gAssignableTunicsAndBoots", true, false);
                UIWidgets::Tooltip("Allows equipping the tunic and boots to c-buttons");
                UIWidgets::PaddedEnhancementCheckbox("Equipment Toggle", "gEquipmentCanBeRemoved", true, false);
                UIWidgets::Tooltip("Allows equipment to be removed by toggling it off on\nthe equipment subscreen.");
                UIWidgets::PaddedEnhancementCheckbox("Link's Cow in Both Time Periods", "gCowOfTime", true, false);
                UIWidgets::Tooltip("Allows the Lon Lon Ranch obstacle course reward to be shared across time periods");
                UIWidgets::PaddedEnhancementCheckbox("Enable visible guard vision", "gGuardVision", true, false);
                UIWidgets::PaddedEnhancementCheckbox("Enable passage of time on file select", "gTimeFlowFileSelect", true, false);
                UIWidgets::PaddedEnhancementCheckbox("Item counts in messages", "gInjectItemCounts", true, false);
                UIWidgets::Tooltip("Injects item counts in pickup messages, like golden skulltula tokens and heart pieces");
                UIWidgets::PaddedEnhancementCheckbox("Pull grave during the day", "gDayGravePull", true, false);
                UIWidgets::Tooltip("Allows graves to be pulled when child during the day");
                UIWidgets::PaddedEnhancementCheckbox("Dogs follow you everywhere", "gDogFollowsEverywhere", true, false);
                UIWidgets::Tooltip("Allows dogs to follow you anywhere you go, even if you leave the market");
                UIWidgets::PaddedEnhancementCheckbox("Don't require input for Credits sequence", "gNoInputForCredits", true, false);
                UIWidgets::Tooltip("Removes the input requirement on textboxes after defeating Ganon, allowing Credits sequence to continue to progress");

                // Blue Fire Arrows
                bool forceEnableBlueFireArrows = gSaveContext.n64ddFlag &&
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BLUE_FIRE_ARROWS);
                static const char* forceEnableBlueFireArrowsText =
                    "This setting is forcefully enabled because a savefile\nwith \"Blue Fire Arrows\" is loaded.";
                UIWidgets::PaddedEnhancementCheckbox("Blue Fire Arrows", "gBlueFireArrows", true, false, 
                    forceEnableBlueFireArrows, forceEnableBlueFireArrowsText, UIWidgets::CheckboxGraphics::Checkmark);
                UIWidgets::Tooltip("Allows Ice Arrows to melt red ice.\nMay require a room reload if toggled during gameplay.");

                // Sunlight Arrows
                bool forceEnableSunLightArrows = gSaveContext.n64ddFlag &&
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SUNLIGHT_ARROWS);
                static const char* forceEnableSunLightArrowsText =
                    "This setting is forcefully enabled because a savefile\nwith \"Sunlight Arrows\" is loaded.";
                UIWidgets::PaddedEnhancementCheckbox("Sunlight Arrows", "gSunlightArrows", true, false, 
                    forceEnableSunLightArrows, forceEnableSunLightArrowsText, UIWidgets::CheckboxGraphics::Checkmark);
                UIWidgets::Tooltip("Allows Light Arrows to activate sun switches.\nMay require a room reload if toggled during gameplay.");

                UIWidgets::PaddedEnhancementCheckbox("Disable Crit wiggle", "gDisableCritWiggle", true, false);
                UIWidgets::Tooltip("Disable random camera wiggle at low health");

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Graphics"))
            {
                if (ImGui::BeginMenu("Animated Link in Pause Menu")) {
                    ImGui::Text("Rotation");
                    UIWidgets::EnhancementRadioButton("Disabled", "gPauseLiveLinkRotation", 0);
                    UIWidgets::EnhancementRadioButton("Rotate Link with D-pad", "gPauseLiveLinkRotation", 1);
                    UIWidgets::Tooltip("Allow you to rotate Link on the Equipment menu with the D-pad\nUse D-pad Up or D-pad Down to reset Link's rotation");
                    UIWidgets::EnhancementRadioButton("Rotate Link with C-buttons", "gPauseLiveLinkRotation", 2);
                    UIWidgets::Tooltip("Allow you to rotate Link on the Equipment menu with the C-buttons\nUse C-Up or C-Down to reset Link's rotation");
                    UIWidgets::EnhancementRadioButton("Rotate Link with Right Stick", "gPauseLiveLinkRotation", 3);
                    UIWidgets::Tooltip("Allow you to rotate Link on the Equipment menu with the Right Stick\nYou can zoom in by pointing up and reset Link's rotation by pointing down");
                    if (CVarGetInteger("gPauseLiveLinkRotation", 0) != 0) {
                        UIWidgets::EnhancementSliderInt("Rotation Speed: %d", "##MinRotationSpeed", "gPauseLiveLinkRotationSpeed", 1, 20, "", 1);
                    }
                    UIWidgets::PaddedSeparator();
                    ImGui::Text("Static loop");
                    UIWidgets::EnhancementRadioButton("Disabled", "gPauseLiveLink", 0);
                    UIWidgets::EnhancementRadioButton("Idle (standing)", "gPauseLiveLink", 1);
                    UIWidgets::EnhancementRadioButton("Idle (look around)", "gPauseLiveLink", 2);
                    UIWidgets::EnhancementRadioButton("Idle (belt)", "gPauseLiveLink", 3);
                    UIWidgets::EnhancementRadioButton("Idle (shield)", "gPauseLiveLink", 4);
                    UIWidgets::EnhancementRadioButton("Idle (test sword)", "gPauseLiveLink", 5);
                    UIWidgets::EnhancementRadioButton("Idle (yawn)", "gPauseLiveLink", 6);
                    UIWidgets::EnhancementRadioButton("Battle Stance", "gPauseLiveLink", 7);
                    UIWidgets::EnhancementRadioButton("Walking (no shield)", "gPauseLiveLink", 8);
                    UIWidgets::EnhancementRadioButton("Walking (holding shield)", "gPauseLiveLink", 9);
                    UIWidgets::EnhancementRadioButton("Running (no shield)", "gPauseLiveLink", 10);
                    UIWidgets::EnhancementRadioButton("Running (holding shield)", "gPauseLiveLink", 11);
                    UIWidgets::EnhancementRadioButton("Hand on hip", "gPauseLiveLink", 12);
                    UIWidgets::EnhancementRadioButton("Spin attack charge", "gPauseLiveLink", 13);
                    UIWidgets::EnhancementRadioButton("Look at hand", "gPauseLiveLink", 14);
                    UIWidgets::PaddedSeparator();
                    ImGui::Text("Randomize");
                    UIWidgets::EnhancementRadioButton("Random", "gPauseLiveLink", 15);
                    UIWidgets::Tooltip("Randomize the animation played each time you open the menu");
                    UIWidgets::EnhancementRadioButton("Random cycle", "gPauseLiveLink", 16);
                    UIWidgets::Tooltip("Randomize the animation played on the menu after a certain time");
                    UIWidgets::EnhancementRadioButton("Random cycle (Idle)", "gPauseLiveLink", 17);
                    UIWidgets::Tooltip("Randomize the animation played on the menu after a certain time (Idle animations only)");
                    if (CVarGetInteger("gPauseLiveLink", 0) >= 16) {
                        UIWidgets::EnhancementSliderInt("Frame to wait: %d", "##MinFrameCount", "gMinFrameCount", 1, 1000, "", 0);
                    }

                    ImGui::EndMenu();
                }
                UIWidgets::PaddedEnhancementCheckbox("N64 Mode", "gLowResMode", true, false);
                UIWidgets::Tooltip("Sets aspect ratio to 4:3 and lowers resolution to 240p, the N64's native resolution");
                UIWidgets::PaddedEnhancementCheckbox("Glitch line-up tick", "gDrawLineupTick", true, false);
                UIWidgets::Tooltip("Displays a tick in the top center of the screen to help with glitch line-ups in SoH, as traditional UI based line-ups do not work outside of 4:3");
                UIWidgets::PaddedEnhancementCheckbox("Enable 3D Dropped items/projectiles", "gNewDrops", true, false);
                UIWidgets::Tooltip("Change most 2D items and projectiles on the overworld to their 3D versions");
                UIWidgets::PaddedEnhancementCheckbox("Disable Black Bar Letterboxes", "gDisableBlackBars", true, false);
                UIWidgets::Tooltip("Disables Black Bar Letterboxes during cutscenes and Z-targeting\nNote: there may be minor visual glitches that were covered up by the black bars\nPlease disable this setting before reporting a bug");
                UIWidgets::PaddedEnhancementCheckbox("Dynamic Wallet Icon", "gDynamicWalletIcon", true, false);
                UIWidgets::Tooltip("Changes the rupee in the wallet icon to match the wallet size you currently have");
                UIWidgets::PaddedEnhancementCheckbox("Always show dungeon entrances", "gAlwaysShowDungeonMinimapIcon", true, false);
                UIWidgets::Tooltip("Always shows dungeon entrance icons on the minimap");
                UIWidgets::PaddedText("Fix Vanishing Paths", true, false);
                UIWidgets::EnhancementCombobox("gDirtPathFix", zFightingOptions, 0);
                UIWidgets::Tooltip("Disabled: Paths vanish more the higher the resolution (Z-fighting is based on resolution)\n"
                                   "Consistent: Certain paths vanish the same way in all resolutions\n"
                                   "No Vanish: Paths do not vanish, Link seems to sink in to some paths\n"
                                   "This might affect other decal effects\n");

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Fixes"))
            {
                UIWidgets::EnhancementCheckbox("Fix L&R Pause menu", "gUniformLR");
                UIWidgets::Tooltip("Makes the L and R buttons in the pause menu the same color");
                UIWidgets::PaddedEnhancementCheckbox("Fix L&Z Page switch in Pause menu", "gNGCKaleidoSwitcher", true, false);
                UIWidgets::Tooltip("Makes L and R switch pages like on the GameCube\nZ opens the Debug Menu instead");
                UIWidgets::PaddedEnhancementCheckbox("Fix Dungeon entrances", "gFixDungeonMinimapIcon", true, false);
                UIWidgets::Tooltip("Removes the dungeon entrance icon on the top-left corner of the screen when no dungeon is present on the current map");
                UIWidgets::PaddedEnhancementCheckbox("Fix Two Handed idle animations", "gTwoHandedIdle", true, false);
                UIWidgets::Tooltip("Re-enables the two-handed idle animation, a seemingly finished animation that was disabled on accident in the original game");
                UIWidgets::PaddedEnhancementCheckbox("Fix the Gravedigging Tour Glitch", "gGravediggingTourFix", true, false);
                UIWidgets::Tooltip("Fixes a bug where the Gravedigging Tour Heart Piece disappears if the area reloads");
                UIWidgets::PaddedEnhancementCheckbox("Fix Deku Nut upgrade", "gDekuNutUpgradeFix", true, false);
                UIWidgets::Tooltip("Prevents the Forest Stage Deku Nut upgrade from becoming unobtainable after receiving the Poacher's Saw");
                UIWidgets::PaddedEnhancementCheckbox("Fix Navi text HUD position", "gNaviTextFix", true, false);
                UIWidgets::Tooltip("Correctly centers the Navi text prompt on the HUD's C-Up button");
                UIWidgets::PaddedEnhancementCheckbox("Fix Anubis fireballs", "gAnubisFix", true, false);
                UIWidgets::Tooltip("Make Anubis fireballs do fire damage when reflected back at them with the Mirror Shield");
                UIWidgets::PaddedEnhancementCheckbox("Fix Megaton Hammer crouch stab", "gCrouchStabHammerFix", true, false);
                UIWidgets::Tooltip("Make the Megaton Hammer's crouch stab able to destroy rocks without first swinging it normally");
                if (CVarGetInteger("gCrouchStabHammerFix", 0) == 0) {
                    CVarSetInteger("gCrouchStabFix", 0);
                } else {
                    UIWidgets::PaddedEnhancementCheckbox("Remove power crouch stab", "gCrouchStabFix", true, false);
                    UIWidgets::Tooltip("Make crouch stabbing always do the same damage as a regular slash");
                }
                UIWidgets::PaddedEnhancementCheckbox("Fix credits timing", "gCreditsFix", true, false);
                UIWidgets::Tooltip("Extend certain credits scenes so the music lines up properly with the visuals");
                UIWidgets::PaddedEnhancementCheckbox("Fix Gerudo Warrior's clothing colors", "gGerudoWarriorClothingFix", true, false);
                UIWidgets::Tooltip("Prevent the Gerudo Warrior's clothes changing color when changing Link's tunic or using bombs in front of her");
                UIWidgets::PaddedEnhancementCheckbox("Fix Camera Drift", "gFixCameraDrift", true, false);
                UIWidgets::Tooltip("Fixes camera slightly drifting to the left when standing still due to a math error");
                UIWidgets::PaddedEnhancementCheckbox("Fix Camera Swing", "gFixCameraSwing", true, false);
                UIWidgets::Tooltip("Fixes camera getting stuck on collision when standing still, also fixes slight shift back in camera when stop moving");
                UIWidgets::PaddedEnhancementCheckbox("Fix Hanging Ledge Swing Rate", "gFixHangingLedgeSwingRate", true, false);
                UIWidgets::Tooltip("Fixes camera swing rate when player falls off a ledge and camera swings around");
                UIWidgets::PaddedEnhancementCheckbox("Fix Missing Jingle after 5 Silver Rupees", "gSilverRupeeJingleExtend", true, false);
                UIWidgets::Tooltip(
                    "Adds 5 higher pitches for the Silver Rupee Jingle for the rooms with more than 5 Silver Rupees. "
                    "Currently only relevant in Master Quest.");

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Restoration"))
            {
                UIWidgets::EnhancementCheckbox("Red Ganon blood", "gRedGanonBlood");
                UIWidgets::Tooltip("Restore the original red blood from NTSC 1.0/1.1. Disable for green blood");
                UIWidgets::PaddedEnhancementCheckbox("Fish while hovering", "gHoverFishing", true, false);
                UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows casting the Fishing Rod while using the Hover Boots");
                UIWidgets::PaddedEnhancementCheckbox("N64 Weird Frames", "gN64WeirdFrames", true, false);
                UIWidgets::Tooltip("Restores N64 Weird Frames allowing weirdshots to behave the same as N64");
                UIWidgets::PaddedEnhancementCheckbox("Bombchus out of bounds", "gBombchusOOB", true, false);
                UIWidgets::Tooltip("Allows bombchus to explode out of bounds\nSimilar to GameCube and Wii VC");
                UIWidgets::PaddedEnhancementCheckbox("Quick Putaway", "gQuickPutaway", true, false);
                UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows putting away an item without an animation and performing Putaway Ocarina Items");
                UIWidgets::PaddedEnhancementCheckbox("Restore old Gold Skulltula cutscene", "gGsCutscene", true, false);
                UIWidgets::PaddedEnhancementCheckbox("Quick Bongo Kill", "gQuickBongoKill", true, false);
                UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows bypassing Bongo Bongo's intro cutscene to quickly kill him");

                ImGui::EndMenu();
            }

            UIWidgets::PaddedSeparator(false, true);

            // Autosave enum value of 1 is the default in presets and the old checkbox "on" state for backwards compatibility
            UIWidgets::PaddedText("Autosave", false, true);
            UIWidgets::EnhancementCombobox("gAutosave", autosaveLabels, 0);
            UIWidgets::Tooltip("Automatically save the game when changing locations and/or obtaining items\n"
                "Major items exclude rupees and health/magic/ammo refills (but include bombchus unless bombchu drops are enabled)");

            UIWidgets::Spacer(0);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));

            if (ImGui::Button(GetWindowButtonText("Customize Game Controls", CVarGetInteger("gGameControlEditorEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVarGetInteger("gGameControlEditorEnabled", 0);
                CVarSetInteger("gGameControlEditorEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Game Control Editor", CVarGetInteger("gGameControlEditorEnabled", 0));
            }
            if (ImGui::Button(GetWindowButtonText("Cosmetics Editor", CVarGetInteger("gCosmeticsEditorEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVarGetInteger("gCosmeticsEditorEnabled", 0);
                CVarSetInteger("gCosmeticsEditorEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Cosmetics Editor", CVarGetInteger("gCosmeticsEditorEnabled", 0));
            }
            if (ImGui::Button(GetWindowButtonText("Audio Editor", CVarGetInteger("gAudioEditor.WindowOpen", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVarGetInteger("gAudioEditor.WindowOpen", 0);
                CVarSetInteger("gAudioEditor.WindowOpen", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Audio Editor", CVarGetInteger("gAudioEditor.WindowOpen", 0));
            }
            if (ImGui::Button(GetWindowButtonText("Gameplay Stats", CVarGetInteger("gGameplayStatsEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                bool currentValue = CVarGetInteger("gGameplayStatsEnabled", 0);
                CVarSetInteger("gGameplayStatsEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Gameplay Stats", CVarGetInteger("gGameplayStatsEnabled", 0));
            }
            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor(1);

            EXPERIMENTAL();

            { // FPS Slider
                const int minFps = 20;
                const int maxFps = Ship::Window::GetInstance()->GetCurrentRefreshRate();
                int currentFps = fmax(fmin(OTRGlobals::Instance->GetInterpolationFPS(), maxFps), minFps);
            #ifdef __WIIU__
                // only support divisors of 60 on the Wii U
                if (currentFps > 60) {
                    currentFps = 60;
                } 
                else {
                    currentFps = 60 / (60 / currentFps);
                }

                int fpsSlider = 1;
                if (currentFps == 20) {
                    ImGui::Text("Frame interpolation: Off");
                } 
                else {
                    ImGui::Text("Frame interpolation: %d FPS", currentFps);
                    if (currentFps == 30) {
                        fpsSlider = 2;
                    }
                    else { // currentFps == 60
                        fpsSlider = 3;
                    }
                }
                if (CVarGetInteger("gMatchRefreshRate", 0)) {
                    UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
                }

                if (ImGui::Button(" - ##WiiUFPS")) {
                    fpsSlider--;
                }
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);

                ImGui::SliderInt("##WiiUFPSSlider", &fpsSlider, 1, 3, "", ImGuiSliderFlags_AlwaysClamp);

                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                if (ImGui::Button(" + ##WiiUFPS")) {
                    fpsSlider++;
                }

                if (CVarGetInteger("gMatchRefreshRate", 0)) {
                    UIWidgets::ReEnableComponent("");
                }
                if (fpsSlider > 3) {
                    fpsSlider = 3;
                } 
                else if (fpsSlider < 1) {
                    fpsSlider = 1;
                }

                if (fpsSlider == 1) {
                    currentFps = 20;
                }
                else if (fpsSlider == 2) {
                    currentFps = 30;
                }
                else if (fpsSlider == 3) {
                    currentFps = 60;
                }
                CVarSetInteger("gInterpolationFPS", currentFps);
                SohImGui::RequestCvarSaveOnNextTick();
            #else
                UIWidgets::EnhancementSliderInt((currentFps == 20) ? "Frame interpolation: Off" : "Frame interpolation: %d FPS", 
                    "##FPSInterpolation", "gInterpolationFPS", minFps, maxFps, "", 20, true, CVarGetInteger("gMatchRefreshRate", 0));
            #endif
                UIWidgets::Tooltip("Interpolate extra frames to get smoother graphics\n"
                    "Set to match your monitor's refresh rate, or a divisor of it\n"
                    "A higher target FPS than your monitor's refresh rate will just waste resources, and might give a worse result.\n"
                    "For consistent input lag, set this value and your monitor's refresh rate to a multiple of 20\n"
                    "Ctrl+Click for keyboard input");
            } // END FPS Slider
            
            UIWidgets::PaddedEnhancementCheckbox("Match Refresh Rate", "gMatchRefreshRate", true, false);
            UIWidgets::Tooltip("Matches interpolation value to the current game's window refresh rate");

            UIWidgets::PaddedEnhancementCheckbox("Disable LOD", "gDisableLOD", true, false);
            UIWidgets::Tooltip("Turns off the Level of Detail setting, making models use their higher-poly variants at any distance");
            if (UIWidgets::PaddedEnhancementCheckbox("Disable Draw Distance", "gDisableDrawDistance", true, false)) {
                if (CVarGetInteger("gDisableDrawDistance", 0) == 0) {
                    CVarSetInteger("gDisableKokiriDrawDistance", 0);
                }
            }
            UIWidgets::Tooltip("Turns off the objects draw distance, making objects being visible from a longer range");
            if (CVarGetInteger("gDisableDrawDistance", 0) == 1) {
                UIWidgets::PaddedEnhancementCheckbox("Kokiri Draw Distance", "gDisableKokiriDrawDistance", true, false);
                UIWidgets::Tooltip("The Kokiri are mystical beings that fade into view when approached\nEnabling this will remove their draw distance");
            }
            UIWidgets::PaddedEnhancementCheckbox("Skip Text", "gSkipText", true, false);
            UIWidgets::Tooltip("Holding down B skips text");

         #ifdef __SWITCH__
            UIWidgets::Spacer(0);
            ImGui::Text("Switch performance mode");
            if (UIWidgets::EnhancementCombobox("gSwitchPerfMode", SWITCH_CPU_PROFILES, (int)Ship::SwitchProfiles::STOCK)) {
                SPDLOG_INFO("Profile:: %s", SWITCH_CPU_PROFILES[CVarGetInteger("gSwitchPerfMode", (int)Ship::SwitchProfiles::STOCK)]);
                Ship::Switch::ApplyOverclock();
            }
         #endif

            ImGui::EndMenu();
        }

        ImGui::SetCursorPosY(0.0f);

        if (ImGui::BeginMenu("Cheats"))
        {
            if (ImGui::BeginMenu("Infinite...")) {
                UIWidgets::EnhancementCheckbox("Money", "gInfiniteMoney");
                UIWidgets::PaddedEnhancementCheckbox("Health", "gInfiniteHealth", true, false);
                UIWidgets::PaddedEnhancementCheckbox("Ammo", "gInfiniteAmmo", true, false);
                UIWidgets::PaddedEnhancementCheckbox("Magic", "gInfiniteMagic", true, false);
                UIWidgets::PaddedEnhancementCheckbox("Nayru's Love", "gInfiniteNayru", true, false);
                UIWidgets::PaddedEnhancementCheckbox("Epona Boost", "gInfiniteEpona", true, false);

                ImGui::EndMenu();
            }

            UIWidgets::PaddedEnhancementCheckbox("No Clip", "gNoClip", true, false);
            UIWidgets::Tooltip("Allows you to walk through walls");
            UIWidgets::PaddedEnhancementCheckbox("Climb Everything", "gClimbEverything", true, false);
            UIWidgets::Tooltip("Makes every surface in the game climbable");
            UIWidgets::PaddedEnhancementCheckbox("Hookshot Everything", "gHookshotEverything", true, false);
            UIWidgets::Tooltip("Makes every surface in the game hookshot-able");
            UIWidgets::EnhancementSliderFloat("Hookshot Reach Multiplier: %.1fx", "##gCheatHookshotReachMultiplier", "gCheatHookshotReachMultiplier", 1.0f, 5.0f, "", 1.0f, false);
            UIWidgets::PaddedEnhancementCheckbox("Moon Jump on L", "gMoonJumpOnL", true, false);
            UIWidgets::Tooltip("Holding L makes you float into the air");
            UIWidgets::PaddedEnhancementCheckbox("Super Tunic", "gSuperTunic", true, false);
            UIWidgets::Tooltip("Makes every tunic have the effects of every other tunic");
            UIWidgets::PaddedEnhancementCheckbox("Easy ISG", "gEzISG", true, false);
            UIWidgets::Tooltip("Passive Infinite Sword Glitch\nIt makes your sword's swing effect and hitbox stay active indefinitely");
            UIWidgets::PaddedEnhancementCheckbox("Timeless Equipment", "gTimelessEquipment", true, false);
            UIWidgets::Tooltip("Allows any item to be equipped, regardless of age\nAlso allows Child to use Adult strength upgrades");
            UIWidgets::PaddedEnhancementCheckbox("Easy Frame Advancing", "gCheatEasyPauseBufferEnabled", true, false);
            UIWidgets::Tooltip("Continue holding START button when unpausing to only advance a single frame and then re-pause");
            const bool bEasyFrameAdvanceEnabled = CVarGetInteger("gCheatEasyPauseBufferEnabled", 0);
            UIWidgets::PaddedEnhancementCheckbox("Easy Input Buffering", "gCheatEasyInputBufferingEnabled", true, false, bEasyFrameAdvanceEnabled, "Forced enabled when Easy Frame Advancing is enabled");
            UIWidgets::Tooltip("Inputs that are held down while the Subscreen is closing will be pressed when the game is resumed");
            UIWidgets::PaddedEnhancementCheckbox("Unrestricted Items", "gNoRestrictItems", true, false);
            UIWidgets::Tooltip("Allows you to use any item at any location");
            UIWidgets::PaddedEnhancementCheckbox("Freeze Time", "gFreezeTime", true, false);
            UIWidgets::Tooltip("Freezes the time of day");
            UIWidgets::PaddedEnhancementCheckbox("Drops Don't Despawn", "gDropsDontDie", true, false);
            UIWidgets::Tooltip("Drops from enemies, grass, etc. don't disappear after a set amount of time");
            UIWidgets::PaddedEnhancementCheckbox("Fireproof Deku Shield", "gFireproofDekuShield", true, false);
            UIWidgets::Tooltip("Prevents the Deku Shield from burning on contact with fire");
            UIWidgets::PaddedEnhancementCheckbox("Shield with Two-Handed Weapons", "gShieldTwoHanded", true, false);
            UIWidgets::Tooltip("This allows you to put up your shield with any two-handed weapon in hand except for Deku Sticks");
            UIWidgets::PaddedEnhancementCheckbox("Time Sync", "gTimeSync", true, false);
            UIWidgets::Tooltip("This syncs the ingame time with the real world time");
            UIWidgets::PaddedEnhancementCheckbox("No ReDead/Gibdo Freeze", "gNoRedeadFreeze", true, false);
            UIWidgets::Tooltip("Prevents ReDeads and Gibdos from being able to freeze you with their scream");

            {
                static int32_t betaQuestEnabled = CVarGetInteger("gEnableBetaQuest", 0);
                static int32_t lastBetaQuestEnabled = betaQuestEnabled;
                static int32_t betaQuestWorld = CVarGetInteger("gBetaQuestWorld", 0xFFEF);
                static int32_t lastBetaQuestWorld = betaQuestWorld;

                if (!isBetaQuestEnabled) {
                    UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
                }

                UIWidgets::PaddedEnhancementCheckbox("Enable Beta Quest", "gEnableBetaQuest", true, false);
                UIWidgets::Tooltip("Turns on OoT Beta Quest. *WARNING* This will reset your game.");
                betaQuestEnabled = CVarGetInteger("gEnableBetaQuest", 0);
                if (betaQuestEnabled) {
                    if (betaQuestEnabled != lastBetaQuestEnabled) {
                        betaQuestWorld = 0;
                    }

                    ImGui::Text("Beta Quest World: %d", betaQuestWorld);

                    if (ImGui::Button(" - ##BetaQuest")) {
                        betaQuestWorld--;
                    }
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);

                    ImGui::SliderInt("##BetaQuest", &betaQuestWorld, 0, 8, "", ImGuiSliderFlags_AlwaysClamp);
                    UIWidgets::Tooltip("Set the Beta Quest world to explore. *WARNING* Changing this will reset your game.\nCtrl+Click to type in a value.");

                    ImGui::SameLine();
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                    if (ImGui::Button(" + ##BetaQuest")) {
                        betaQuestWorld++;
                    }

                    if (betaQuestWorld > 8) {
                        betaQuestWorld = 8;
                    }
                    else if (betaQuestWorld < 0) {
                        betaQuestWorld = 0;
                    }
                }
                else {
                    lastBetaQuestWorld = betaQuestWorld = 0xFFEF;
                    CVarSetInteger("gBetaQuestWorld", betaQuestWorld);
                }
                if (betaQuestEnabled != lastBetaQuestEnabled || betaQuestWorld != lastBetaQuestWorld)
                {
                    // Reset the game if the beta quest state or world changed because beta quest happens on redirecting the title screen cutscene.
                    lastBetaQuestEnabled = betaQuestEnabled;
                    lastBetaQuestWorld = betaQuestWorld;
                    CVarSetInteger("gEnableBetaQuest", betaQuestEnabled);
                    CVarSetInteger("gBetaQuestWorld", betaQuestWorld);

                    SohImGui::DispatchConsoleCommand("reset");

                    SohImGui::RequestCvarSaveOnNextTick();
                }

                if (!isBetaQuestEnabled) {
                    UIWidgets::ReEnableComponent("");
                }
            }

            if (ImGui::Button("Change Age")) {
                CVarSetInteger("gSwitchAge", 1);
            }
            UIWidgets::Tooltip("Switches Link's age and reloads the area.");   

            ImGui::EndMenu();
        }

        ImGui::SetCursorPosY(0.0f);

        if (ImGui::BeginMenu("Developer Tools"))
        {
            UIWidgets::EnhancementCheckbox("OoT Debug Mode", "gDebugEnabled");
            UIWidgets::Tooltip("Enables Debug Mode, allowing you to select maps with L + R + Z, noclip with L + D-pad Right, and open the debug menu with L on the pause screen");
            UIWidgets::PaddedEnhancementCheckbox("OoT Skulltula Debug", "gSkulltulaDebugEnabled", true, false);
            UIWidgets::Tooltip("Enables Skulltula Debug, when moving the cursor in the menu above various map icons (boss key, compass, map screen locations, etc) will set the GS bits in that area.\nUSE WITH CAUTION AS IT DOES NOT UPDATE THE GS COUNT.");
            UIWidgets::PaddedEnhancementCheckbox("Fast File Select", "gSkipLogoTitle", true, false);
            UIWidgets::Tooltip("Load the game to the selected menu or file\n\"Zelda Map Select\" require debug mode else you will fallback to File choose menu\nUsing a file number that don't have save will create a save file only if you toggle on \"Create a new save if none ?\" else it will bring you to the File choose menu");
            if (CVarGetInteger("gSkipLogoTitle", 0)) {
                ImGui::Text("Loading :");
                UIWidgets::EnhancementCombobox("gSaveFileID", FastFileSelect, 0);
            };
            UIWidgets::PaddedEnhancementCheckbox("Hide Build Info", "gHideBuildInfo", true, false);
            UIWidgets::Tooltip("Hides the game version and build details in the boot logo start screen");
            UIWidgets::PaddedEnhancementCheckbox("Better Debug Warp Screen", "gBetterDebugWarpScreen", true, false);
            UIWidgets::Tooltip("Optimized debug warp screen, with the added ability to chose entrances and time of day");
            UIWidgets::PaddedEnhancementCheckbox("Debug Warp Screen Translation", "gDebugWarpScreenTranslation", true, false);
            UIWidgets::Tooltip("Translate the Debug Warp Screen based on the game language");
            UIWidgets::PaddedSeparator();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0,0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
            if (ImGui::Button(GetWindowButtonText("Stats", CVarGetInteger("gStatsEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVarGetInteger("gStatsEnabled", 0);
                CVarSetInteger("gStatsEnabled", !currentValue);
                SohImGui::ToggleStatisticsWindow(true);
                SohImGui::RequestCvarSaveOnNextTick();
            }
            UIWidgets::Tooltip("Shows the stats window, with your FPS and frametimes, and the OS you're playing on");
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Console", CVarGetInteger("gConsoleEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVarGetInteger("gConsoleEnabled", 0);
                CVarSetInteger("gConsoleEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::ToggleConsoleWindow(!currentValue);
            }
            UIWidgets::Tooltip("Enables the console window, allowing you to input commands, type help for some examples");
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Save Editor", CVarGetInteger("gSaveEditorEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVarGetInteger("gSaveEditorEnabled", 0);
                CVarSetInteger("gSaveEditorEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Save Editor", CVarGetInteger("gSaveEditorEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Collision Viewer", CVarGetInteger("gCollisionViewerEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVarGetInteger("gCollisionViewerEnabled", 0);
                CVarSetInteger("gCollisionViewerEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Collision Viewer", CVarGetInteger("gCollisionViewerEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Actor Viewer", CVarGetInteger("gActorViewerEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVarGetInteger("gActorViewerEnabled", 0);
                CVarSetInteger("gActorViewerEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Actor Viewer", CVarGetInteger("gActorViewerEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Display List Viewer", CVarGetInteger("gDLViewerEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVarGetInteger("gDLViewerEnabled", 0);
                CVarSetInteger("gDLViewerEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Display List Viewer", CVarGetInteger("gDLViewerEnabled", 0));
            }
            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor(1);

            ImGui::EndMenu();
        }

        ImGui::SetCursorPosY(0.0f);

        if (ImGui::BeginMenu("Randomizer"))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
        #ifdef __WIIU__
            static ImVec2 buttonSize(200.0f * 2.0f, 0.0f);
        #else
            static ImVec2 buttonSize(200.0f, 0.0f);
        #endif
            if (ImGui::Button(GetWindowButtonText("Randomizer Settings", CVarGetInteger("gRandomizerSettingsEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVarGetInteger("gRandomizerSettingsEnabled", 0);
                CVarSetInteger("gRandomizerSettingsEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Randomizer Settings", CVarGetInteger("gRandomizerSettingsEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Item Tracker", CVarGetInteger("gItemTrackerEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVarGetInteger("gItemTrackerEnabled", 0);
                CVarSetInteger("gItemTrackerEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Item Tracker", CVarGetInteger("gItemTrackerEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Item Tracker Settings", CVarGetInteger("gItemTrackerSettingsEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVarGetInteger("gItemTrackerSettingsEnabled", 0);
                CVarSetInteger("gItemTrackerSettingsEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Item Tracker Settings", CVarGetInteger("gItemTrackerSettingsEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Entrance Tracker", CVarGetInteger("gEntranceTrackerEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVarGetInteger("gEntranceTrackerEnabled", 0);
                CVarSetInteger("gEntranceTrackerEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Entrance Tracker", CVarGetInteger("gEntranceTrackerEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Check Tracker", CVarGetInteger("gCheckTrackerEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVarGetInteger("gCheckTrackerEnabled", 0);
                CVarSetInteger("gCheckTrackerEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Check Tracker", CVarGetInteger("gCheckTrackerEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Check Tracker Settings", CVarGetInteger("gCheckTrackerSettingsEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVarGetInteger("gCheckTrackerSettingsEnabled", 0);
                CVarSetInteger("gCheckTrackerSettingsEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Check Tracker Settings", CVarGetInteger("gCheckTrackerSettingsEnabled", 0));
            }
            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor(1);

            UIWidgets::PaddedSeparator();

            if (ImGui::BeginMenu("Rando Enhancements"))
            {
                UIWidgets::EnhancementCheckbox("Rando-Relevant Navi Hints", "gRandoRelevantNavi");
                UIWidgets::Tooltip(
                    "Replace Navi's overworld quest hints with rando-related gameplay hints."
                );
                UIWidgets::PaddedEnhancementCheckbox("Random Rupee Names", "gRandomizeRupeeNames", true, false);
                UIWidgets::Tooltip(
                    "When obtaining rupees, randomize what the rupee is called in the textbox."
                );

                // Only disable the key colors checkbox when none of the keysanity settings are set to "Any Dungeon", "Overworld" or "Anywhere"
                bool disableKeyColors = true;

                if (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANY_DUNGEON ||
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_OVERWORLD ||
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANYWHERE ||
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GERUDO_KEYS) != RO_GERUDO_KEYS_VANILLA ||
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANY_DUNGEON ||
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_OVERWORLD ||
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANYWHERE ||
                    (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GANONS_BOSS_KEY) != RO_GANON_BOSS_KEY_VANILLA &&
                     OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GANONS_BOSS_KEY) != RO_GANON_BOSS_KEY_OWN_DUNGEON &&
                     OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GANONS_BOSS_KEY) != RO_GANON_BOSS_KEY_STARTWITH) || 
                    !gSaveContext.n64ddFlag) {
                    disableKeyColors = false;
                }

                static const char* disableKeyColorsText = 
                    "This setting is disabled because a savefile is loaded without any key\n"
                    "shuffle settings set to \"Any Dungeon\", \"Overworld\" or \"Anywhere\"";

                UIWidgets::PaddedEnhancementCheckbox("Key Colors Match Dungeon", "gRandoMatchKeyColors", true, false,
                                                     disableKeyColors, disableKeyColorsText);
                UIWidgets::Tooltip(
                    "Matches the color of small keys and boss keys to the dungeon they belong to. "
                    "This helps identify keys from afar and adds a little bit of flair.\n\nThis only "
                    "applies to seeds with keys and boss keys shuffled to Any Dungeon, Overworld, or Anywhere.");
                UIWidgets::PaddedEnhancementCheckbox("Quest Item Fanfares", "gRandoQuestItemFanfares", true, false);
                UIWidgets::Tooltip(
                    "Play unique fanfares when obtaining quest items "
                    "(medallions/stones/songs). Note that these fanfares are longer than usual."
                );
                ImGui::EndMenu();
            }

            UIWidgets::PaddedSeparator();

        #ifdef ENABLE_CROWD_CONTROL
            UIWidgets::EnhancementCheckbox("Crowd Control", "gCrowdControl");
            UIWidgets::Tooltip("Will attempt to connect to the Crowd Control server. Check out crowdcontrol.live for more information.");

            if (CVarGetInteger("gCrowdControl", 0)) {
                CrowdControl::Instance->Enable();
            } else {
                CrowdControl::Instance->Disable();
            }

            UIWidgets::Spacer(0);
        #endif

            UIWidgets::EnhancementCheckbox("Enemy Randomizer", "gRandomizedEnemies");
            UIWidgets::Tooltip(
                "Randomizes regular enemies every time you load a room. Bosses, mini-bosses and a few specific regular enemies are excluded.\n\n"
                "Enemies that need more than Deku Nuts + either Deku Sticks or a sword to kill are excluded from spawning in \"clear enemy\" rooms."
            );

            if (CVarGetInteger("gRandomizedEnemies", 0)) {

                bool disableSeededEnemies = !gSaveContext.n64ddFlag && gSaveContext.fileNum >= 0 && gSaveContext.fileNum <= 2;
                static const char* disableSeededEnemiesText = "This setting is disabled because it relies on a randomizer savefile.";

                UIWidgets::PaddedEnhancementCheckbox("Seeded Enemy Spawns", "gSeededRandomizedEnemies", true, false, disableSeededEnemies, disableSeededEnemiesText);
                UIWidgets::Tooltip(
                    "Enemy spawns will stay consistent throughout room reloads. Enemy spawns are based on randomizer seeds, so this only works with randomizer savefiles."
                );
            }

            ImGui::EndMenu();
        }
    }
}
