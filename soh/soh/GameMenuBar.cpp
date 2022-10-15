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
#include <libultraship/ImGuiImpl.h>
#include <libultraship/Cvar.h>
#include <libultraship/Hooks.h>
#include <ultra64/types.h>
#include <ultra64/pi.h>
#include <ultra64/sptask.h>

#ifdef __SWITCH__
#include <libultraship/SwitchImpl.h>
#endif

#include "UIWidgets.hpp"
#include "include/global.h"
#include "include/z64audio.h"
#include "soh/SaveManager.h"

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

    const char* powers[9] = {
        "Vanilla (1x)",
        "Double (2x)",
        "Quadruple (4x)",
        "Octuple (8x)",
        "Hexadecuple (16x)",
        "Duotrigintuple (32x)",
        "Quattuorsexagintuple (64x)",
        "Octoviginticentuple (128x)",
        "Hexaquinquagintiducentuple (256x)"
    };

    // MARK: - Helpers

    std::string GetWindowButtonText(const char* text, bool menuOpen) {
        char buttonText[100] = "";
        if(menuOpen) { strcat(buttonText,"> "); }
        strcat(buttonText, text);
        if (!menuOpen) { strcat(buttonText, "  "); }
        return buttonText;
    }

    void BindAudioSlider(const char* name, const char* key, float defaultValue, SeqPlayers playerId) {
        float value = CVar_GetFloat(key, defaultValue);

        ImGui::Text(name, static_cast<int>(100 * value));
        if (ImGui::SliderFloat((std::string("##") + key).c_str(), &value, 0.0f, 1.0f, "")) {
            const float volume = floorf(value * 100) / 100;
            CVar_SetFloat(key, volume);
            SohImGui::RequestCvarSaveOnNextTick();
            Audio_SetGameVolume(playerId, volume);
        }
    }

    void UpdateAudio() {
        Audio_SetGameVolume(SEQ_BGM_MAIN, CVar_GetFloat("gMainMusicVolume", 1));
        Audio_SetGameVolume(SEQ_BGM_SUB, CVar_GetFloat("gSubMusicVolume", 1));
        Audio_SetGameVolume(SEQ_FANFARE, CVar_GetFloat("gSFXMusicVolume", 1));
        Audio_SetGameVolume(SEQ_SFX, CVar_GetFloat("gFanfareVolume", 1));
    }

    void applyEnhancementPresetDefault(void) {
        // D-pad Support on Pause
        CVar_SetS32("gDpadPause", 0);
        // D-pad Support in text and file select
        CVar_SetS32("gDpadText", 0);
        // Play Ocarina with D-pad
        CVar_SetS32("gDpadOcarina", 0);
        // Play Ocarina with Right Stick
        CVar_SetS32("gRStickOcarina", 0);
        // D-pad as Equip Items
        CVar_SetS32("gDpadEquips", 0);
        // Allow the cursor to be on any slot
        CVar_SetS32("gPauseAnyCursor", 0);
        // Prevent Dropped Ocarina Inputs
        CVar_SetS32("gDpadNoDropOcarinaInput", 0);
        // Answer Navi Prompt with L Button
        CVar_SetS32("gNaviOnL", 0);
        // Invert Camera X Axis
        CVar_SetS32("gInvertXAxis", 0);
        // Invert Camera Y Axis
        CVar_SetS32("gInvertYAxis", 1);
        // Right Stick Aiming
        CVar_SetS32("gRightStickAiming", 0);
        // Disable Auto-Center First Person View
        CVar_SetS32("gDisableAutoCenterView", 0);

        // Text Speed (1 to 5)
        CVar_SetS32("gTextSpeed", 1);
        // King Zora Speed (1 to 5)
        CVar_SetS32("gMweepSpeed", 1);
        // Biggoron Forge Time (0 to 3)
        CVar_SetS32("gForgeTime", 3);
        // Vine/Ladder Climb speed (+0 to +12)
        CVar_SetS32("gClimbSpeed", 0);
        // Faster Block Push (+0 to +5)
        CVar_SetS32("gFasterBlockPush", 0);
        // Faster Heavy Block Lift
        CVar_SetS32("gFasterHeavyBlockLift", 0);
        // No Forced Navi
        CVar_SetS32("gNoForcedNavi", 0);
        // No Skulltula Freeze
        CVar_SetS32("gSkulltulaFreeze", 0);
        // MM Bunny Hood
        CVar_SetS32("gMMBunnyHood", 0);
        // Fast Chests
        CVar_SetS32("gFastChests", 0);
        // Fast Drops
        CVar_SetS32("gFastDrops", 0);
        // Better Owl
        CVar_SetS32("gBetterOwl", 0);
        // Fast Ocarina Playback
        CVar_SetS32("gFastOcarinaPlayback", 0);
        // Instant Putaway
        CVar_SetS32("gInstantPutaway", 0);
        // Instant Boomerang Recall
        CVar_SetS32("gFastBoomerang", 0);
        // Mask Select in Inventory
        CVar_SetS32("gMaskSelect", 0);
        // Remember Save Location
        CVar_SetS32("gRememberSaveLocation", 0);

        // Damage Multiplier (0 to 8)
        CVar_SetS32("gDamageMul", 0);
        // Fall Damage Multiplier (0 to 7)
        CVar_SetS32("gFallDamageMul", 0);
        // Void Damage Multiplier (0 to 6)
        CVar_SetS32("gVoidDamageMul", 0);
        // No Random Drops
        CVar_SetS32("gNoRandomDrops", 0);
        // No Heart Drops
        CVar_SetS32("gNoHeartDrops", 0);
        // Enable Bombchu Drops
        CVar_SetS32("gBombchuDrops", 0);
        // Always Win Goron Pot
        CVar_SetS32("gGoronPot", 0);
        // Always Win Dampe Digging First Try
        CVar_SetS32("gDampeWin", 0);

        // Change Red Potion Effect
        CVar_SetS32("gRedPotionEffect", 0);
        // Red Potion Health (1 to 100)
        CVar_SetS32("gRedPotionHealth", 1);
        // Red Potion Percent Restore
        CVar_SetS32("gRedPercentRestore", 0);
        // Change Green Potion Effect
        CVar_SetS32("gGreenPotionEffect", 0);
        // Green Potion Mana (1 to 100)
        CVar_SetS32("gGreenPotionMana", 1);
        // Green Potion Percent Restore
        CVar_SetS32("gGreenPercentRestore", 0);
        // Change Blue Potion Effects
        CVar_SetS32("gBluePotionEffects", 0);
        // Blue Potion Health (1 to 100)
        CVar_SetS32("gBluePotionHealth", 1);
        // Blue Potion Health Percent Restore
        CVar_SetS32("gBlueHealthPercentRestore", 0);
        // Blue Potion Mana (1 to 100)
        CVar_SetS32("gBluePotionMana", 1);
        // Blue Potion Mana Percent Restore
        CVar_SetS32("gBlueManaPercentRestore", 0);
        // Change Milk Effect
        CVar_SetS32("gMilkEffect", 0);
        // Milk Health (1 to 100)
        CVar_SetS32("gMilkHealth", 1);
        // Milk Percent Restore
        CVar_SetS32("gMilkPercentRestore", 0);
        // Separate Half Milk Effect
        CVar_SetS32("gSeparateHalfMilkEffect", 0);
        // Half Milk Health (1 to 100)
        CVar_SetS32("gHalfMilkHealth", 0);
        // Half Milk Percent Restore
        CVar_SetS32("gHalfMilkPercentRestore", 0);
        // Change Fairy Effect
        CVar_SetS32("gFairyEffect", 0);
        // Fairy (1 to 100)
        CVar_SetS32("gFairyHealth", 1);
        // Fairy Percent Restore
        CVar_SetS32("gFairyPercentRestore", 0);
        // Change Fairy Revive Effect
        CVar_SetS32("gFairyReviveEffect", 0);
        // Fairy Revival (1 to 100)
        CVar_SetS32("gFairyReviveHealth", 1);
        // Fairy Revive Percent Restore
        CVar_SetS32("gFairyRevivePercentRestore", 0);

        // Instant Fishing
        CVar_SetS32("gInstantFishing", 0);
        // Guarantee Bite
        CVar_SetS32("gGuaranteeFishingBite", 0);
        // Fish Never Escape
        CVar_SetS32("gFishNeverEscape", 0);
        // Child Minimum Weight (6 to 10)
        CVar_SetS32("gChildMinimumWeightFish", 10);
        // Adult Minimum Weight (8 to 13)
        CVar_SetS32("gAdultMinimumWeightFish", 13);

        // Mute Low HP Alarm
        CVar_SetS32("gLowHpAlarm", 0);
        // Minimal UI
        CVar_SetS32("gMinimalUI", 0);
        // Disable Navi Call Audio
        CVar_SetS32("gDisableNaviCallAudio", 0);

        // Visual Stone of Agony
        CVar_SetS32("gVisualAgony", 0);
        // Assignable Tunics and Boots
        CVar_SetS32("gAssignableTunicsAndBoots", 0);
        // Equipment Toggle
        CVar_SetS32("gEquipmentCanBeRemoved", 0);
        // Link's Cow in Both Time Periods
        CVar_SetS32("gCowOfTime", 0);
        // Enable visible guard vision
        CVar_SetS32("gGuardVision", 0);
        // Enable passage of time on file select
        CVar_SetS32("gTimeFlowFileSelect", 0);
        // Count Golden Skulltulas
        CVar_SetS32("gInjectSkulltulaCount", 0);
        // Pull grave during the day
        CVar_SetS32("gDayGravePull", 0);
        // Pull out Ocarina to Summon Scarecrow
        CVar_SetS32("gSkipScarecrow", 0);
        // Blue Fire Arrows
        CVar_SetS32("gBlueFireArrows", 0);
        // Sunlight Arrows
        CVar_SetS32("gSunlightArrows", 0);

        // Rotate link (0 to 2)
        CVar_SetS32("gPauseLiveLinkRotation", 0);
        // Pause link animation (0 to 16)
        CVar_SetS32("gPauseLiveLink", 0);
        // Frames to wait
        CVar_SetS32("gMinFrameCount", 1);

        // N64 Mode
        CVar_SetS32("gN64Mode", 0);
        // Enable 3D Dropped items/projectiles
        CVar_SetS32("gNewDrops", 0);
        // Disable Black Bar Letterboxes
        CVar_SetS32("gDisableBlackBars", 0);
        // Dynamic Wallet Icon
        CVar_SetS32("gDynamicWalletIcon", 0);
        // Always show dungeon entrances
        CVar_SetS32("gAlwaysShowDungeonMinimapIcon", 0);

        // Fix L&R Pause menu
        CVar_SetS32("gUniformLR", 0);
        // Fix L&Z Page switch in Pause menu
        CVar_SetS32("gNGCKaleidoSwitcher", 0);
        // Fix Dungeon entrances
        CVar_SetS32("gFixDungeonMinimapIcon", 0);
        // Fix Two Handed idle animations
        CVar_SetS32("gTwoHandedIdle", 0);
        // Fix the Gravedigging Tour Glitch
        CVar_SetS32("gGravediggingTourFix", 0);
        // Fix Deku Nut upgrade
        CVar_SetS32("gDekuNutUpgradeFix", 0);
        // Fix Navi text HUD position
        CVar_SetS32("gNaviTextFix", 0);
        // Fix Anubis fireballs
        CVar_SetS32("gAnubisFix", 0);
        // Fix Megaton Hammer crouch stab
        CVar_SetS32("gCrouchStabHammerFix", 0);
        // Fix all crouch stab
        CVar_SetS32("gCrouchStabFix", 0);
        // Fix Gerudo Warrior's clothing colors
        CVar_SetS32("gGerudoWarriorClothingFix", 0);

        // Red Ganon blood
        CVar_SetS32("gRedGanonBlood", 0);
        // Fish while hovering
        CVar_SetS32("gHoverFishing", 0);
        // N64 Weird Frames
        CVar_SetS32("gN64WeirdFrames", 0);
        // Bombchus out of bounds
        CVar_SetS32("gBombchusOOB", 0);

        // Restore old Gold Skulltula cutscene
        CVar_SetS32("gGsCutscene", 0);
        // Autosave
        CVar_SetS32("gAutosave", 0);

        //Crit wiggle disable
        CVar_SetS32("gDisableCritWiggle", 0);
    }

    void applyEnhancementPresetVanillaPlus(void) {
        // D-pad Support in text and file select
        CVar_SetS32("gDpadText", 1);
        // Play Ocarina with D-pad
        CVar_SetS32("gDpadOcarina", 1);
        // Play Ocarina with Right Stick
        CVar_SetS32("gRStickOcarina", 1);
        // D-pad as Equip Items
        CVar_SetS32("gDpadEquips", 1);
        // Prevent Dropped Ocarina Inputs
        CVar_SetS32("gDpadNoDropOcarinaInput", 1);
        // Right Stick Aiming
        CVar_SetS32("gRightStickAiming", 1);

        // Text Speed (1 to 5)
        CVar_SetS32("gTextSpeed", 5);
        // King Zora Speed (1 to 5)
        CVar_SetS32("gMweepSpeed", 2);
        // Faster Block Push (+0 to +5)
        CVar_SetS32("gFasterBlockPush", 5);
        // Better Owl
        CVar_SetS32("gBetterOwl", 1);

        // Assignable Tunics and Boots
        CVar_SetS32("gAssignableTunicsAndBoots", 1);
        // Enable passage of time on file select
        CVar_SetS32("gTimeFlowFileSelect", 1);
        // Count Golden Skulltulas
        CVar_SetS32("gInjectSkulltulaCount", 1);

        // Pause link animation (0 to 16)
        CVar_SetS32("gPauseLiveLink", 1);

        // Dynamic Wallet Icon
        CVar_SetS32("gDynamicWalletIcon", 1);
        // Always show dungeon entrances
        CVar_SetS32("gAlwaysShowDungeonMinimapIcon", 1);

        // Fix L&R Pause menu
        CVar_SetS32("gUniformLR", 1);
        // Fix Dungeon entrances
        CVar_SetS32("gFixDungeonMinimapIcon", 1);
        // Fix Two Handed idle animations
        CVar_SetS32("gTwoHandedIdle", 1);
        // Fix the Gravedigging Tour Glitch
        CVar_SetS32("gGravediggingTourFix", 1);
        // Fix Deku Nut upgrade
        CVar_SetS32("gDekuNutUpgradeFix", 1);
        // Fix Navi text HUD position
        CVar_SetS32("gNaviTextFix", 1);

        // Red Ganon blood
        CVar_SetS32("gRedGanonBlood", 1);
        // Fish while hovering
        CVar_SetS32("gHoverFishing", 1);
        // N64 Weird Frames
        CVar_SetS32("gN64WeirdFrames", 1);
        // Bombchus out of bounds
        CVar_SetS32("gBombchusOOB", 1);
    }

    void applyEnhancementPresetEnhanced(void) {
        // King Zora Speed (1 to 5)
        CVar_SetS32("gMweepSpeed", 5);
        // Biggoron Forge Time (0 to 3)
        CVar_SetS32("gForgeTime", 0);
        // Vine/Ladder Climb speed (+0 to +12)
        CVar_SetS32("gClimbSpeed", 3);
        // Faster Heavy Block Lift
        CVar_SetS32("gFasterHeavyBlockLift", 1);
        // No Forced Navi
        CVar_SetS32("gNoForcedNavi", 1);
        // No Skulltula Freeze
        CVar_SetS32("gSkulltulaFreeze", 1);
        // MM Bunny Hood
        CVar_SetS32("gMMBunnyHood", 1);
        // Fast Chests
        CVar_SetS32("gFastChests", 1);
        // Fast Drops
        CVar_SetS32("gFastDrops", 1);
        // Fast Ocarina Playback
        CVar_SetS32("gFastOcarinaPlayback", 1);
        // Instant Putaway
        CVar_SetS32("gInstantPutaway", 1);
        // Instant Boomerang Recall
        CVar_SetS32("gFastBoomerang", 1);
        // Mask Select in Inventory
        CVar_SetS32("gMaskSelect", 1);
        // Always Win Goron Pot
        CVar_SetS32("gGoronPot", 1);
        // Always Win Dampe Digging
        CVar_SetS32("gDampeWin", 1);

        // Disable Navi Call Audio
        CVar_SetS32("gDisableNaviCallAudio", 1);

        // Equipment Toggle
        CVar_SetS32("gEquipmentCanBeRemoved", 1);
        // Link's Cow in Both Time Periods
        CVar_SetS32("gCowOfTime", 1);

        // Enable 3D Dropped items/projectiles
        CVar_SetS32("gNewDrops", 1);

        // Fix Anubis fireballs
        CVar_SetS32("gAnubisFix", 1);

        // Autosave
        CVar_SetS32("gAutosave", 1);
    }

    void applyEnhancementPresetRandomizer(void) {
        // Allow the cursor to be on any slot
        CVar_SetS32("gPauseAnyCursor", 1);

        // Guarantee Bite
        CVar_SetS32("gGuaranteeFishingBite", 1);
        // Fish Never Escape
        CVar_SetS32("gFishNeverEscape", 1);
        // Child Minimum Weight (6 to 10)
        CVar_SetS32("gChildMinimumWeightFish", 3);
        // Adult Minimum Weight (8 to 13)
        CVar_SetS32("gAdultMinimumWeightFish", 6);

        // Visual Stone of Agony
        CVar_SetS32("gVisualAgony", 1);
        // Pull grave during the day
        CVar_SetS32("gDayGravePull", 1);
        // Pull out Ocarina to Summon Scarecrow
        CVar_SetS32("gSkipScarecrow", 1);

        // Pause link animation (0 to 16)
        CVar_SetS32("gPauseLiveLink", 16);
        // Frames to wait
        CVar_SetS32("gMinFrameCount", 200);
    }

    void applyEnhancementPresets(void) {
        switch (CVar_GetS32("gSelectEnhancementPresets", 0)) {
            // Default
        case 0:
            applyEnhancementPresetDefault();
            break;

            // Vanilla Plus
        case 1:
            applyEnhancementPresetDefault();
            applyEnhancementPresetVanillaPlus();
            break;

            // Enhanced
        case 2:
            applyEnhancementPresetDefault();
            applyEnhancementPresetVanillaPlus();
            applyEnhancementPresetEnhanced();
            break;

            // Randomizer
        case 3:
            applyEnhancementPresetDefault();
            applyEnhancementPresetVanillaPlus();
            applyEnhancementPresetEnhanced();
            applyEnhancementPresetRandomizer();
            break;
        }
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
                UIWidgets::EnhancementSliderFloat("Master Volume: %d %%", "##Master_Vol", "gGameMasterVolume", 0.0f, 1.0f, "", 1.0f, true);
                UIWidgets::Spacer(0);
                BindAudioSlider("Main Music Volume: %d %%", "gMainMusicVolume", 1.0f, SEQ_BGM_MAIN);
                UIWidgets::Spacer(0);
                BindAudioSlider("Sub Music Volume: %d %%", "gSubMusicVolume", 1.0f, SEQ_BGM_SUB);
                UIWidgets::Spacer(0);
                BindAudioSlider("Sound Effects Volume: %d %%", "gSFXMusicVolume", 1.0f, SEQ_SFX);
                UIWidgets::Spacer(0);
                BindAudioSlider("Fanfare Volume: %d %%", "gFanfareVolume", 1.0f, SEQ_FANFARE);

                ImGui::Text("Audio API (Needs reload)");
                auto audioBackends = SohImGui::GetAvailableAudioBackends();
                auto currentAudioBackend = SohImGui::GetCurrentAudioBackend();

                if (ImGui::BeginCombo("##AApi", currentAudioBackend.second)) {
                    if (audioBackends.size() > 1) {
                        for (uint8_t i = 0; i < audioBackends.size(); i++) {
                            if (ImGui::Selectable(audioBackends[i].second, audioBackends[i] == currentAudioBackend)) {
                                SohImGui::SetCurrentAudioBackend(i, audioBackends[i]);
                            }
                        }
                    }

                    ImGui::EndCombo();
                }

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Controller")) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 (12.0f, 6.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                if (ImGui::Button(GetWindowButtonText("Controller Configuration", CVar_GetS32("gControllerConfigurationEnabled", 0)).c_str()))
                {
                    bool currentValue = CVar_GetS32("gControllerConfigurationEnabled", 0);
                    CVar_SetS32("gControllerConfigurationEnabled", !currentValue);
                    SohImGui::RequestCvarSaveOnNextTick();
                    SohImGui::ToggleInputEditorWindow(CVar_GetS32("gControllerConfigurationEnabled", 0));
                }
                ImGui::PopStyleColor(1);
                ImGui::PopStyleVar(3);
            #ifndef __SWITCH__
                UIWidgets::PaddedEnhancementCheckbox("Use Controller Navigation", "gControlNav", true, false);
                UIWidgets::Tooltip("Allows controller navigation of the menu bar\nD-pad to move between items, A to select, and X to grab focus on the menu bar");
            #endif
                UIWidgets::PaddedEnhancementCheckbox("Show Inputs", "gInputEnabled", true, false);
                UIWidgets::Tooltip("Shows currently pressed inputs on the bottom right of the screen");
                UIWidgets::Spacer(0);
                ImGui::PushItemWidth(ImGui::GetWindowSize().x - 20.0f);
                UIWidgets::EnhancementSliderFloat("Input Scale: %.1f", "##Input", "gInputScale", 1.0f, 3.0f, "", 1.0f, false);
                UIWidgets::Tooltip("Sets the on screen size of the displayed inputs from the Show Inputs setting");
                ImGui::PopItemWidth();
                UIWidgets::Spacer(0);
                ImGui::PushItemWidth(ImGui::GetWindowSize().x - 20.0f);
                UIWidgets::EnhancementSliderInt("Simulated Input Lag: %d frames", "##SimulatedInputLag", "gSimulatedInputLag", 0, 6, "", 0, false);
                UIWidgets::Tooltip("Buffers your inputs to be executed a specified amount of frames later");
                ImGui::PopItemWidth();

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Graphics")) {
            #ifndef __APPLE__
                UIWidgets::EnhancementSliderFloat("Internal Resolution: %d %%", "##IMul", "gInternalResolution", 0.5f, 2.0f, "", 1.0f, true, true);
                UIWidgets::Tooltip("Multiplies your output resolution by the value inputted, as a more intensive but effective form of anti-aliasing");
                SohImGui::SetResolutionMultiplier(CVar_GetFloat("gInternalResolution", 1));
            #endif
            #ifndef __WIIU__
                UIWidgets::PaddedEnhancementSliderInt("MSAA: %d", "##IMSAA", "gMSAAValue", 1, 8, "", 1, false, true, false);
                UIWidgets::Tooltip("Activates multi-sample anti-aliasing when above 1x up to 8x for 8 samples for every pixel");
                SohImGui::SetMSAALevel(CVar_GetS32("gMSAAValue", 1));
            #endif

                if (SohImGui::WindowBackend() == SohImGui::Backend::DX11)
                {
                    const char* cvar = "gExtraLatencyThreshold";
                    int val = CVar_GetS32(cvar, 80);
                    val = fmax(fmin(val, 360), 0);
                    int fps = val;

                    UIWidgets::Spacer(0);

                    if (fps == 0)
                    {
                        ImGui::Text("Jitter fix: Off");
                    }
                    else
                    {
                        ImGui::Text("Jitter fix: >= %d FPS", fps);
                    }

                    std::string MinusBTNELT = " - ##ExtraLatencyThreshold";
                    std::string PlusBTNELT = " + ##ExtraLatencyThreshold";
                    if (ImGui::Button(MinusBTNELT.c_str())) {
                        val--;
                        CVar_SetS32(cvar, val);
                        SohImGui::RequestCvarSaveOnNextTick();
                    }
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                #ifdef __SWITCH__
                    ImGui::PushItemWidth(ImGui::GetWindowSize().x - 110.0f);
                #elif defined(__WIIU__)
                    ImGui::PushItemWidth(ImGui::GetWindowSize().x - 79.0f * 2);
                #else
                    ImGui::PushItemWidth(ImGui::GetWindowSize().x - 79.0f);
                #endif
                    if (ImGui::SliderInt("##ExtraLatencyThreshold", &val, 0, 360, "", ImGuiSliderFlags_AlwaysClamp))
                    {
                        CVar_SetS32(cvar, val);
                        SohImGui::RequestCvarSaveOnNextTick();
                    }
                    ImGui::PopItemWidth();
                    UIWidgets::Tooltip("When Interpolation FPS setting is at least this threshold, add one frame of input lag (e.g. 16.6 ms for 60 FPS) in order to avoid jitter. This setting allows the CPU to work on one frame while GPU works on the previous frame.\nThis setting should be used when your computer is too slow to do CPU + GPU work in time.");

                    ImGui::SameLine();
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                    if (ImGui::Button(PlusBTNELT.c_str())) {
                        val++;
                        CVar_SetS32(cvar, val);
                        SohImGui::RequestCvarSaveOnNextTick();
                    }

                    UIWidgets::Spacer(0);
                }

                ImGui::Text("Renderer API (Needs reload)");
                auto renderingBackends = SohImGui::GetAvailableRenderingBackends();
                auto currentRenderingBackend = SohImGui::GetCurrentRenderingBackend();

                if (ImGui::BeginCombo("##RApi", currentRenderingBackend.second)) {
                    if (renderingBackends.size() > 1) {
                        for (uint8_t i = 0; i < renderingBackends.size(); i++) {
                            if (ImGui::Selectable(renderingBackends[i].second, renderingBackends[i] == currentRenderingBackend)) {
                                SohImGui::SetCurrentRenderingBackend(i, renderingBackends[i]);
                            }
                        }
                    }

                    ImGui::EndCombo();
                }

                EXPERIMENTAL();

                ImGui::Text("Texture Filter (Needs reload)");
                UIWidgets::EnhancementCombobox("gTextureFilter", SohImGui::GetSupportedTextureFilters(), 3, 0);

                UIWidgets::Spacer(0);

                SohImGui::DrawSettings();

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Languages")) {
                UIWidgets::EnhancementRadioButton("English", "gLanguages", 0);
                UIWidgets::EnhancementRadioButton("German", "gLanguages", 1);
                UIWidgets::EnhancementRadioButton("French", "gLanguages", 2);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        ImGui::SetCursorPosY(0.0f);

        if (ImGui::BeginMenu("Enhancements"))
        {

            const char* enhancementPresets[4] = { "Default", "Vanilla Plus", "Enhanced", "Randomizer"};
            UIWidgets::PaddedText("Enhancement Presets", false, true);
            UIWidgets::EnhancementCombobox("gSelectEnhancementPresets", enhancementPresets, 4, 0);
            UIWidgets::Tooltip(
                "Default - Set all enhancements to their default values. The true vanilla SoH experience.\n"
                "\n"
                "Vanilla Plus - Adds Quality of Life features that enhance your experience, but don't alter gameplay. Recommended for a first playthrough of OoT.\n"
                "\n"
                "Enhanced - The \"Vanilla Plus\" preset, but with more quality of life enhancements that might alter gameplay slightly. Recommended for returning players.\n"
                "\n"
                "Randomizer - The \"Enhanced\" preset, plus any other enhancements that are recommended for playing Randomizer."
            );

            UIWidgets::Spacer(0);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));
            if (ImGui::Button("Apply Preset")) {
                applyEnhancementPresets();
                SohImGui::RequestCvarSaveOnNextTick();
            }
            ImGui::PopStyleVar(1);

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Controls")) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                float availableWidth = ImGui::GetContentRegionAvail().x;
                if (ImGui::Button(
                    GetWindowButtonText("Customize Game Controls", CVar_GetS32("gGameControlEditorEnabled", 0)).c_str(),
                    ImVec2(availableWidth, 0)
                )) {
                    bool currentValue = CVar_GetS32("gGameControlEditorEnabled", 0);
                    CVar_SetS32("gGameControlEditorEnabled", !currentValue);
                    SohImGui::RequestCvarSaveOnNextTick();
                    SohImGui::EnableWindow("Game Control Editor", CVar_GetS32("gGameControlEditorEnabled", 0));
                }
                ImGui::PopStyleVar(3);
                ImGui::PopStyleColor(1);

                UIWidgets::PaddedEnhancementCheckbox("D-pad Support on Pause Screen", "gDpadPause", true, false);
                UIWidgets::Tooltip("Navigate Pause with the D-pad\nIf used with D-pad as Equip Items, you must hold C-Up to equip instead of navigate\n"
                    "To make the cursor only move a single space no matter how long a direction is held, manually set gDpadHoldChange to 0");
                UIWidgets::PaddedEnhancementCheckbox("D-pad Support in Text Boxes", "gDpadText", true, false);
                UIWidgets::Tooltip("Navigate choices in text boxes, shop item selection, and the file select / name entry screens with the D-pad\n"
                    "To make the cursor only move a single space during name entry no matter how long a direction is held, manually set gDpadHoldChange to 0");
                UIWidgets::PaddedEnhancementCheckbox("D-pad as Equip Items", "gDpadEquips", true, false);
                UIWidgets::Tooltip("Equip items and equipment on the D-pad\nIf used with D-pad on Pause Screen, you must hold C-Up to equip instead of navigate");
                UIWidgets::PaddedEnhancementCheckbox("Allow the cursor to be on any slot", "gPauseAnyCursor", true, false);
                UIWidgets::Tooltip("Allows the cursor on the pause menu to be over any slot\nSimilar to Rando and Spaceworld 97");
                UIWidgets::PaddedEnhancementCheckbox("Answer Navi Prompt with L Button", "gNaviOnL", true, false);
                UIWidgets::Tooltip("Speak to Navi with L but enter first-person camera with C-Up");
                UIWidgets::PaddedEnhancementCheckbox("Enable walk speed modifiers", "gEnableWalkModify", true, false);
                UIWidgets::Tooltip("Hold the assigned button to change the maximum walking speed\nTo change the assigned button, click Customize Game Controls");
                if (CVar_GetS32("gEnableWalkModify", 0)) {
                    UIWidgets::EnhancementSliderFloat("Modifier 1: %d %%", "##WalkMod1", "gWalkModifierOne", 0.0f, 5.0f, "", 1.0f, true);
                    UIWidgets::EnhancementSliderFloat("Modifier 2: %d %%", "##WalkMod2", "gWalkModifierTwo", 0.0f, 5.0f, "", 1.0f, true);
                }
                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Gameplay"))
            {
                if (ImGui::BeginMenu("Time Savers"))
                {
                    UIWidgets::PaddedEnhancementSliderInt("Text Speed: %dx", "##TEXTSPEED", "gTextSpeed", 1, 5, "", 1, false, false, true);
                    UIWidgets::PaddedEnhancementSliderInt("King Zora Speed: %dx", "##MWEEPSPEED", "gMweepSpeed", 1, 5, "", 1, false, false, true);
                    UIWidgets::EnhancementSliderInt("Biggoron Forge Time: %d days", "##FORGETIME", "gForgeTime", 0, 3, "", 3);
                    UIWidgets::Tooltip("Allows you to change the number of days it takes for Biggoron to forge the Biggoron Sword");
                    UIWidgets::PaddedEnhancementSliderInt("Vine/Ladder Climb speed +%d", "##CLIMBSPEED", "gClimbSpeed", 0, 12, "", 0, false, false, true);
                    UIWidgets::PaddedEnhancementSliderInt("Block pushing speed +%d", "##BLOCKSPEED", "gFasterBlockPush", 0, 5, "", 0, false, false, true);
                    UIWidgets::PaddedEnhancementCheckbox("Faster Heavy Block Lift", "gFasterHeavyBlockLift", true, false);
                    UIWidgets::Tooltip("Speeds up lifting silver rocks and obelisks");
                    UIWidgets::PaddedEnhancementCheckbox("No Forced Navi", "gNoForcedNavi", true, false);
                    UIWidgets::Tooltip("Prevent forced Navi conversations");
                    UIWidgets::PaddedEnhancementCheckbox("No Skulltula Freeze", "gSkulltulaFreeze", true, false);
                    UIWidgets::Tooltip("Stops the game from freezing the player when picking up Gold Skulltulas");
                    UIWidgets::PaddedEnhancementCheckbox("Fast Chests", "gFastChests", true, false);
                    UIWidgets::Tooltip("Kick open every chest");
                    UIWidgets::PaddedEnhancementCheckbox("Skip Pickup Messages", "gFastDrops", true, false);
                    UIWidgets::Tooltip("Skip pickup messages for new consumable items and bottle swipes");
                    UIWidgets::PaddedEnhancementCheckbox("Better Owl", "gBetterOwl", true, false);
                    UIWidgets::Tooltip("The default response to Kaepora Gaebora is always that you understood what he said");
                    UIWidgets::PaddedEnhancementCheckbox("Fast Ocarina Playback", "gFastOcarinaPlayback", true, false);
                    bool forceSkipScarecrow = gSaveContext.n64ddFlag &&
                        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SKIP_SCARECROWS_SONG);
                    const char* forceSkipScarecrowText =
                        "This setting is forcefully enabled because a savefile\nwith \"Skip Scarecrow Song\" is loaded";
                    UIWidgets::Tooltip("Skip the part where the Ocarina playback is called when you play a song");
                    UIWidgets::PaddedEnhancementCheckbox("Skip Scarecrow Song", "gSkipScarecrow", true, false,
                                                         forceSkipScarecrow, forceSkipScarecrowText, UIWidgets::CheckboxGraphics::Checkmark);
                    UIWidgets::Tooltip("Pierre appears when Ocarina is pulled out. Requires learning scarecrow song.");
                    UIWidgets::PaddedEnhancementCheckbox("Remember Save Location", "gRememberSaveLocation", true, false);
                    UIWidgets::Tooltip("When loading a save, places Link at the last entrance he went through.\n"
                            "This doesn't work if the save was made in a grotto.");
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
                    UIWidgets::PaddedEnhancementCheckbox("MM Bunny Hood", "gMMBunnyHood", true, false);
                    UIWidgets::Tooltip("Wearing the Bunny Hood grants a speed increase like in Majora's Mask");
                    UIWidgets::PaddedEnhancementCheckbox("Mask Select in Inventory", "gMaskSelect", true, false);
                    UIWidgets::Tooltip("After completing the mask trading sub-quest, press A and any direction on the mask slot to change masks");
                    ImGui::EndMenu();
                }

                UIWidgets::Spacer(0);

                if (ImGui::BeginMenu("Difficulty Options"))
                {
                    ImGui::Text("Damage Multiplier");
                    UIWidgets::EnhancementCombobox("gDamageMul", powers, 9, 0);
                    UIWidgets::Tooltip(
                        "Modifies all sources of damage not affected by other sliders\n\
                        2x: Can survive all common attacks from the start of the game\n\
                        4x: Dies in 1 hit to any substantial attack from the start of the game\n\
                        8x: Can only survive trivial damage from the start of the game\n\
                        16x: Can survive all common attacks with max health without double defense\n\
                        32x: Can survive all common attacks with max health and double defense\n\
                        64x: Can survive trivial damage with max health without double defense\n\
                        128x: Can survive trivial damage with max health and double defense\n\
                        256x: Cannot survive damage"
                    );
                    UIWidgets::PaddedText("Fall Damage Multiplier", true, false);
                    UIWidgets::EnhancementCombobox("gFallDamageMul", powers, 8, 0);
                    UIWidgets::Tooltip(
                        "Modifies all fall damage\n\
                        2x: Can survive all fall damage from the start of the game\n\
                        4x: Can only survive short fall damage from the start of the game\n\
                        8x: Cannot survive any fall damage from the start of the game\n\
                        16x: Can survive all fall damage with max health without double defense\n\
                        32x: Can survive all fall damage with max health and double defense\n\
                        64x: Can survive short fall damage with double defense\n\
                        128x: Cannot survive fall damage"
                    );
                    UIWidgets::PaddedText("Void Damage Multiplier", true, false);
                    UIWidgets::EnhancementCombobox("gVoidDamageMul", powers, 7, 0);
                    UIWidgets::Tooltip(
                        "Modifies damage taken after falling into a void\n\
                        2x: Can survive void damage from the start of the game\n\
                        4x: Cannot survive void damage from the start of the game\n\
                        8x: Can survive void damage twice with max health without double defense\n\
                        16x: Can survive void damage with max health without double defense\n\
                        32x: Can survive void damage with max health and double defense\n\
                        64x: Cannot survive void damage"
                    );
                    UIWidgets::PaddedEnhancementCheckbox("No Random Drops", "gNoRandomDrops", true, false);
                    UIWidgets::Tooltip("Disables random drops, except from the Goron Pot, Dampe, and bosses");
                    bool forceEnableBombchuDrops = gSaveContext.n64ddFlag &&
                        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_ENABLE_BOMBCHU_DROPS) == 1;
                    const char* forceEnableBombchuDropsText =
                        "This setting is forcefully enabled because a savefile\nwith \"Enable Bombchu Drops\" is loaded.";
                    UIWidgets::PaddedEnhancementCheckbox("Enable Bombchu Drops", "gBombchuDrops", true, false,
                                                         forceEnableBombchuDrops, forceEnableBombchuDropsText, UIWidgets::CheckboxGraphics::Checkmark);
                    UIWidgets::Tooltip("Bombchus will sometimes drop in place of bombs");
                    UIWidgets::PaddedEnhancementCheckbox("No Heart Drops", "gNoHeartDrops", true, false);
                    UIWidgets::Tooltip("Disables heart drops, but not heart placements, like from a Deku Scrub running off\nThis simulates Hero Mode from other games in the series");
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
                        UIWidgets::EnhancementSliderInt("Red Potion Health: %d", "##REDPOTIONHEALTH", "gRedPotionHealth", 1, 100, "", 0, true);
                        UIWidgets::Tooltip("Changes the amount of health restored by Red Potions");
                        UIWidgets::EnhancementCheckbox("Red Potion Percent Restore", "gRedPercentRestore");
                        UIWidgets::Tooltip("Toggles from Red Potions restoring a fixed amount of health to a percent of the player's current max health");

                        ImGui::Separator();

                        UIWidgets::EnhancementCheckbox("Change Green Potion Effect", "gGreenPotionEffect");
                        UIWidgets::Tooltip("Enable the following changes to the amount of mana restored by Green Potions");
                        UIWidgets::EnhancementSliderInt("Green Potion Mana: %d", "##GREENPOTIONMANA", "gGreenPotionMana", 1, 100, "", 0, true);
                        UIWidgets::Tooltip("Changes the amount of mana restored by Green Potions, base max mana is 48, max upgraded mana is 96");
                        UIWidgets::EnhancementCheckbox("Green Potion Percent Restore", "gGreenPercentRestore");
                        UIWidgets::Tooltip("Toggles from Green Potions restoring a fixed amount of mana to a percent of the player's current max mana");

                        ImGui::Separator();

                        UIWidgets::EnhancementCheckbox("Change Blue Potion Effects", "gBluePotionEffects");
                        UIWidgets::Tooltip("Enable the following changes to the amount of health and mana restored by Blue Potions");
                        UIWidgets::EnhancementSliderInt("Blue Potion Health: %d", "##BLUEPOTIONHEALTH", "gBluePotionHealth", 1, 100, "", 0, true);
                        UIWidgets::Tooltip("Changes the amount of health restored by Blue Potions");
                        UIWidgets::EnhancementCheckbox("Blue Potion Health Percent Restore", "gBlueHealthPercentRestore");
                        UIWidgets::Tooltip("Toggles from Blue Potions restoring a fixed amount of health to a percent of the player's current max health");

                        ImGui::Separator();

                        UIWidgets::EnhancementSliderInt("Blue Potion Mana: %d", "##BLUEPOTIONMANA", "gBluePotionMana", 1, 100, "", 0, true);
                        UIWidgets::Tooltip("Changes the amount of mana restored by Blue Potions, base max mana is 48, max upgraded mana is 96");
                        UIWidgets::EnhancementCheckbox("Blue Potion Mana Percent Restore", "gBlueManaPercentRestore");
                        UIWidgets::Tooltip("Toggles from Blue Potions restoring a fixed amount of mana to a percent of the player's current max mana");

                        ImGui::Separator();

                        UIWidgets::EnhancementCheckbox("Change Milk Effect", "gMilkEffect");
                        UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Milk");
                        UIWidgets::EnhancementSliderInt("Milk Health: %d", "##MILKHEALTH", "gMilkHealth", 1, 100, "", 0, true);
                        UIWidgets::Tooltip("Changes the amount of health restored by Milk");
                        UIWidgets::EnhancementCheckbox("Milk Percent Restore", "gMilkPercentRestore");
                        UIWidgets::Tooltip("Toggles from Milk restoring a fixed amount of health to a percent of the player's current max health");

                        ImGui::Separator();

                        UIWidgets::EnhancementCheckbox("Separate Half Milk Effect", "gSeparateHalfMilkEffect");
                        UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Half Milk\nIf this is disabled, Half Milk will behave the same as Full Milk.");
                        UIWidgets::EnhancementSliderInt("Half Milk Health: %d", "##HALFMILKHEALTH", "gHalfMilkHealth", 1, 100, "", 0, true);
                        UIWidgets::Tooltip("Changes the amount of health restored by Half Milk");
                        UIWidgets::EnhancementCheckbox("Half Milk Percent Restore", "gHalfMilkPercentRestore");
                        UIWidgets::Tooltip("Toggles from Half Milk restoring a fixed amount of health to a percent of the player's current max health");

                        ImGui::Separator();

                        UIWidgets::EnhancementCheckbox("Change Fairy Effect", "gFairyEffect");
                        UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Fairies");
                        UIWidgets::EnhancementSliderInt("Fairy: %d", "##FAIRYHEALTH", "gFairyHealth", 1, 100, "", 0, true);
                        UIWidgets::Tooltip("Changes the amount of health restored by Fairies");
                        UIWidgets::EnhancementCheckbox("Fairy Percent Restore", "gFairyPercentRestore");
                        UIWidgets::Tooltip("Toggles from Fairies restoring a fixed amount of health to a percent of the player's current max health");

                        ImGui::Separator();

                        UIWidgets::EnhancementCheckbox("Change Fairy Revive Effect", "gFairyReviveEffect");
                        UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Fairy Revivals");
                        UIWidgets::EnhancementSliderInt("Fairy Revival: %d", "##FAIRYREVIVEHEALTH", "gFairyReviveHealth", 1, 100, "", 0, true);
                        UIWidgets::Tooltip("Changes the amount of health restored by Fairy Revivals");
                        UIWidgets::EnhancementCheckbox("Fairy Revive Percent Restore", "gFairyRevivePercentRestore");
                        UIWidgets::Tooltip("Toggles from Fairy Revivals restoring a fixed amount of health to a percent of the player's current max health");

                        ImGui::EndMenu();
                    }

                    UIWidgets::Spacer(0);

                    if (ImGui::BeginMenu("Fishing")) {
                        UIWidgets::EnhancementCheckbox("Instant Fishing", "gInstantFishing");
                        UIWidgets::Tooltip("All fish will be caught instantly");
                        UIWidgets::PaddedEnhancementCheckbox("Guarantee Bite", "gGuaranteeFishingBite", true, false);
                        UIWidgets::Tooltip("When a line is stable, guarantee bite. Otherwise use default logic");
                        UIWidgets::PaddedEnhancementCheckbox("Fish Never Escape", "gFishNeverEscape", true, false);
                        UIWidgets::Tooltip("Once a hook has been set, fish will never let go while being reeled in.");
                        UIWidgets::PaddedEnhancementSliderInt("Child Minimum Weight: %d", "##cMinimumWeight", "gChildMinimumWeightFish", 3, 10, "", 10, false, true, false);
                        UIWidgets::Tooltip("The minimum weight for the unique fishing reward as a child");
                        UIWidgets::PaddedEnhancementSliderInt("Adult Minimum Weight: %d", "##aMinimumWeight", "gAdultMinimumWeightFish", 6, 13, "", 13, false, true, false);
                        UIWidgets::Tooltip("The minimum weight for the unique fishing reward as an adult");
                        ImGui::EndMenu();
                    }

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
                UIWidgets::PaddedEnhancementCheckbox("Count Golden Skulltulas", "gInjectSkulltulaCount", true, false);
                UIWidgets::Tooltip("Injects Golden Skulltula total count in pickup messages");
                UIWidgets::PaddedEnhancementCheckbox("Pull grave during the day", "gDayGravePull", true, false);
                UIWidgets::Tooltip("Allows graves to be pulled when child during the day");

                // Blue Fire Arrows
                bool forceEnableBlueFireArrows = gSaveContext.n64ddFlag &&
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BLUE_FIRE_ARROWS);
                const char* forceEnableBlueFireArrowsText =
                    "This setting is forcefully enabled because a savefile\nwith \"Blue Fire Arrows\" is loaded.";
                UIWidgets::PaddedEnhancementCheckbox("Blue Fire Arrows", "gBlueFireArrows", true, false, 
                    forceEnableBlueFireArrows, forceEnableBlueFireArrowsText, UIWidgets::CheckboxGraphics::Checkmark);
                UIWidgets::Tooltip("Allows Ice Arrows to melt red ice.\nMay require a room reload if toggled during gameplay.");

                // Sunlight Arrows
                bool forceEnableSunLightArrows = gSaveContext.n64ddFlag &&
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SUNLIGHT_ARROWS);
                const char* forceEnableSunLightArrowsText =
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
                    if (CVar_GetS32("gPauseLiveLinkRotation", 0) != 0) {
                        UIWidgets::EnhancementSliderInt("Rotation Speed: %d", "##MinRotationSpeed", "gPauseLiveLinkRotationSpeed", 1, 20, "");
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
                    if (CVar_GetS32("gPauseLiveLink", 0) >= 16) {
                        UIWidgets::EnhancementSliderInt("Frame to wait: %d", "##MinFrameCount", "gMinFrameCount", 1, 1000, "", 0, true);
                    }

                    ImGui::EndMenu();
                }
                UIWidgets::PaddedEnhancementCheckbox("N64 Mode", "gN64Mode", true, false);
                UIWidgets::Tooltip("Sets aspect ratio to 4:3 and lowers resolution to 240p, the N64's native resolution");
                UIWidgets::PaddedEnhancementCheckbox("Enable 3D Dropped items/projectiles", "gNewDrops", true, false);
                UIWidgets::Tooltip("Change most 2D items and projectiles on the overworld to their 3D versions");
                UIWidgets::PaddedEnhancementCheckbox("Disable Black Bar Letterboxes", "gDisableBlackBars", true, false);
                UIWidgets::Tooltip("Disables Black Bar Letterboxes during cutscenes and Z-targeting\nNote: there may be minor visual glitches that were covered up by the black bars\nPlease disable this setting before reporting a bug");
                UIWidgets::PaddedEnhancementCheckbox("Dynamic Wallet Icon", "gDynamicWalletIcon", true, false);
                UIWidgets::Tooltip("Changes the rupee in the wallet icon to match the wallet size you currently have");
                UIWidgets::PaddedEnhancementCheckbox("Always show dungeon entrances", "gAlwaysShowDungeonMinimapIcon", true, false);
                UIWidgets::Tooltip("Always shows dungeon entrance icons on the minimap");

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
                if (CVar_GetS32("gCrouchStabHammerFix", 0) == 0) {
                    CVar_SetS32("gCrouchStabFix", 0);
                } else {
                    UIWidgets::PaddedEnhancementCheckbox("Remove power crouch stab", "gCrouchStabFix", true, false);
                    UIWidgets::Tooltip("Make crouch stabbing always do the same damage as a regular slash");
                }
                UIWidgets::PaddedEnhancementCheckbox("Fix Gerudo Warrior's clothing colors", "gGerudoWarriorClothingFix", true, false);
                UIWidgets::Tooltip("Prevent the Gerudo Warrior's clothes changing color when changing Link's tunic or using bombs in front of her");

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
                UIWidgets::PaddedEnhancementCheckbox("Restore old Gold Skulltula cutscene", "gGsCutscene", true, false);

                ImGui::EndMenu();
            }

            UIWidgets::PaddedEnhancementCheckbox("Autosave", "gAutosave", true, false);
            UIWidgets::Tooltip("Automatically save the game every time a new area is entered or item is obtained\n"
                "To disable saving when obtaining a major item, manually set gAutosaveMajorItems to 0\n"
                "To enable saving when obtaining any item, manually set gAutosaveAllItems to 1\n"
                "gAutosaveAllItems takes priority over gAutosaveMajorItems if both are set to 1\n"
                "gAutosaveMajorItems excludes rupees and health/magic/ammo refills (but includes bombchus)");

            UIWidgets::Spacer(0);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
            if (ImGui::Button(GetWindowButtonText("Cosmetics Editor", CVar_GetS32("gCosmeticsEditorEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVar_GetS32("gCosmeticsEditorEnabled", 0);
                CVar_SetS32("gCosmeticsEditorEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Cosmetics Editor", CVar_GetS32("gCosmeticsEditorEnabled", 0));
            }
            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor(1);

            EXPERIMENTAL();

            const char* fps_cvar = "gInterpolationFPS";
            {
            #if defined(__SWITCH__) || defined(__WIIU__)
                int minFps = 20;
                int maxFps = 60;
            #else
                int minFps = 20;
                int maxFps = 360;
            #endif

                int val = CVar_GetS32(fps_cvar, minFps);
                val = fmax(fmin(val, maxFps), 20);

            #ifdef __WIIU__
                // only support divisors of 60 on the Wii U
                val = 60 / (60 / val);
            #endif

                int fps = val;

                if (fps == 20)
                {
                    ImGui::Text("Frame interpolation: Off");
                }
                else
                {
                    ImGui::Text("Frame interpolation: %d FPS", fps);
                }

                std::string MinusBTNFPSI = " - ##FPSInterpolation";
                std::string PlusBTNFPSI = " + ##FPSInterpolation";
                if (ImGui::Button(MinusBTNFPSI.c_str())) {
                #ifdef __WIIU__
                    if (val >= 60) val = 30;
                    else val = 20;
                #else
                    val--;
                #endif
                    CVar_SetS32(fps_cvar, val);
                    SohImGui::RequestCvarSaveOnNextTick();
                }
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
            #ifdef __SWITCH__
                ImGui::PushItemWidth(ImGui::GetWindowSize().x - 110.0f);
            #elif defined(__WIIU__)
                ImGui::PushItemWidth(ImGui::GetWindowSize().x - 79.0f * 2);
            #else
                ImGui::PushItemWidth(ImGui::GetWindowSize().x - 79.0f);
            #endif
                if (ImGui::SliderInt("##FPSInterpolation", &val, minFps, maxFps, "", ImGuiSliderFlags_AlwaysClamp))
                {
                #ifdef __WIIU__
                    // only support divisors of 60 on the Wii U
                    val = 60 / (60 / val);
                #endif
                    if (val > 360)
                    {
                        val = 360;
                    }
                    else if (val < 20)
                    {
                        val = 20;
                    }

                    CVar_SetS32(fps_cvar, val);
                    SohImGui::RequestCvarSaveOnNextTick();
                }
                ImGui::PopItemWidth();
                UIWidgets::Tooltip("Interpolate extra frames to get smoother graphics\n"
                    "Set to match your monitor's refresh rate, or a divisor of it\n"
                    "A higher target FPS than your monitor's refresh rate will just waste resources, "
                    "and might give a worse result.\n"
                    "For consistent input lag, set this value and your monitor's refresh rate to a multiple of 20\n"
                    "Ctrl+Click for keyboard input");

                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                if (ImGui::Button(PlusBTNFPSI.c_str())) {
                #ifdef __WIIU__
                    if (val <= 20) val = 30;
                    else val = 60;
                #else
                    val++;
                #endif
                    CVar_SetS32(fps_cvar, val);
                    SohImGui::RequestCvarSaveOnNextTick();
                }
            }

            if (SohImGui::WindowBackend() == SohImGui::Backend::DX11)
            {
                UIWidgets::Spacer(0);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 4.0f));
                if (ImGui::Button("Match Refresh Rate"))
                {
                    int hz = roundf(SohImGui::WindowRefreshRate());
                    if (hz >= 20 && hz <= 360)
                    {
                        CVar_SetS32(fps_cvar, hz);
                        SohImGui::RequestCvarSaveOnNextTick();
                    }
                }
                ImGui::PopStyleVar(1);
                UIWidgets::Spacer(0);
            }
            UIWidgets::EnhancementCheckbox("Disable LOD", "gDisableLOD");
            UIWidgets::Tooltip("Turns off the Level of Detail setting, making models use their higher-poly variants at any distance");
            UIWidgets::PaddedEnhancementCheckbox("Disable Draw Distance", "gDisableDrawDistance", true, false);
            UIWidgets::Tooltip("Turns off the objects draw distance, making objects being visible from a longer range");
            if (CVar_GetS32("gDisableDrawDistance", 0) == 0) {
                CVar_SetS32("gDisableKokiriDrawDistance", 0);
            } else if (CVar_GetS32("gDisableDrawDistance", 0) == 1) {
                UIWidgets::PaddedEnhancementCheckbox("Kokiri Draw Distance", "gDisableKokiriDrawDistance", true, false);
                UIWidgets::Tooltip("The Kokiri are mystical beings that fade into view when approached\nEnabling this will remove their draw distance");
            }
            UIWidgets::PaddedEnhancementCheckbox("Skip Text", "gSkipText", true, false);
            UIWidgets::Tooltip("Holding down B skips text");
            UIWidgets::PaddedEnhancementCheckbox("Free Camera", "gFreeCamera", true, false);
            UIWidgets::Tooltip("Enables camera control\nNote: You must remap C buttons off of the right stick in the controller config menu, and map the camera stick to the right stick.");

         #ifdef __SWITCH__
            UIWidgets::Spacer(0);
            int slot = CVar_GetS32("gSwitchPerfMode", (int)Ship::SwitchProfiles::STOCK);
            ImGui::Text("Switch performance mode");
            if (ImGui::BeginCombo("##perf", SWITCH_CPU_PROFILES[slot])) {
                for (int sId = 0; sId <= Ship::SwitchProfiles::POWERSAVINGM3; sId++) {
                    if (ImGui::Selectable(SWITCH_CPU_PROFILES[sId], sId == slot)) {
                        SPDLOG_INFO("Profile:: %s", SWITCH_CPU_PROFILES[sId]);
                        CVar_SetS32("gSwitchPerfMode", sId);
                        Ship::Switch::ApplyOverclock();
                        SohImGui::RequestCvarSaveOnNextTick();
                    }

                }
                ImGui::EndCombo();
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
            UIWidgets::PaddedEnhancementCheckbox("Moon Jump on L", "gMoonJumpOnL", true, false);
            UIWidgets::Tooltip("Holding L makes you float into the air");
            UIWidgets::PaddedEnhancementCheckbox("Super Tunic", "gSuperTunic", true, false);
            UIWidgets::Tooltip("Makes every tunic have the effects of every other tunic");
            UIWidgets::PaddedEnhancementCheckbox("Easy ISG", "gEzISG", true, false);
            UIWidgets::Tooltip("Passive Infinite Sword Glitch\nIt makes your sword's swing effect and hitbox stay active indefinitely");
            UIWidgets::PaddedEnhancementCheckbox("Easy Frame Advancing", "gCheatEasyPauseBufferEnabled", true, false);
            UIWidgets::Tooltip("Continue holding START button when unpausing to only advance a single frame and then re-pause");
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

            {
                static int32_t betaQuestEnabled = CVar_GetS32("gEnableBetaQuest", 0);
                static int32_t lastBetaQuestEnabled = betaQuestEnabled;
                static int32_t betaQuestWorld = CVar_GetS32("gBetaQuestWorld", 0xFFEF);
                static int32_t lastBetaQuestWorld = betaQuestWorld;

                if (!isBetaQuestEnabled) {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                }

                UIWidgets::PaddedEnhancementCheckbox("Enable Beta Quest", "gEnableBetaQuest", true, false);
                UIWidgets::Tooltip("Turns on OoT Beta Quest. *WARNING* This will reset your game.");
                betaQuestEnabled = CVar_GetS32("gEnableBetaQuest", 0);
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
                    CVar_SetS32("gBetaQuestWorld", betaQuestWorld);
                }
                if (betaQuestEnabled != lastBetaQuestEnabled || betaQuestWorld != lastBetaQuestWorld)
                {
                    // Reset the game if the beta quest state or world changed because beta quest happens on redirecting the title screen cutscene.
                    lastBetaQuestEnabled = betaQuestEnabled;
                    lastBetaQuestWorld = betaQuestWorld;
                    CVar_SetS32("gEnableBetaQuest", betaQuestEnabled);
                    CVar_SetS32("gBetaQuestWorld", betaQuestWorld);

                    SohImGui::DispatchConsoleCommand("reset");

                    SohImGui::RequestCvarSaveOnNextTick();
                }

                if (!isBetaQuestEnabled) {
                    ImGui::PopItemFlag();
                    ImGui::PopStyleVar(1);
                }
            }

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
            if (CVar_GetS32("gSkipLogoTitle", 0)) {
                const char* FastFileSelect[5] = {
                    "File N.1",
                    "File N.2",
                    "File N.3",
                    "File select",
                    "Zelda Map Select (require OoT Debug Mode)"
                };
                ImGui::Text("Loading :");
                UIWidgets::EnhancementCombobox("gSaveFileID", FastFileSelect, 5, 0);
                UIWidgets::PaddedEnhancementCheckbox("Create a new save if none", "gCreateNewSave", true, false);
                UIWidgets::Tooltip("Enable the creation of a new save file if none exist in the File number selected\nNo file name will be assigned please do in Save editor once you see the first text else your save file name will be named \"00000000\"\nIf disabled you will fall back in File select menu");
            };
            UIWidgets::PaddedEnhancementCheckbox("Better Debug Warp Screen", "gBetterDebugWarpScreen", true, false);
            UIWidgets::Tooltip("Optimized debug warp screen, with the added ability to chose entrances and time of day");
            UIWidgets::PaddedSeparator();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0,0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
            if (ImGui::Button(GetWindowButtonText("Stats", CVar_GetS32("gStatsEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVar_GetS32("gStatsEnabled", 0);
                CVar_SetS32("gStatsEnabled", !currentValue);
                SohImGui::ToggleStatisticsWindow(true);
                SohImGui::RequestCvarSaveOnNextTick();
            }
            UIWidgets::Tooltip("Shows the stats window, with your FPS and frametimes, and the OS you're playing on");
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Console", CVar_GetS32("gConsoleEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVar_GetS32("gConsoleEnabled", 0);
                CVar_SetS32("gConsoleEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::ToggleConsoleWindow(!currentValue);
            }
            UIWidgets::Tooltip("Enables the console window, allowing you to input commands, type help for some examples");
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Save Editor", CVar_GetS32("gSaveEditorEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVar_GetS32("gSaveEditorEnabled", 0);
                CVar_SetS32("gSaveEditorEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Save Editor", CVar_GetS32("gSaveEditorEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Collision Viewer", CVar_GetS32("gCollisionViewerEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVar_GetS32("gCollisionViewerEnabled", 0);
                CVar_SetS32("gCollisionViewerEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Collision Viewer", CVar_GetS32("gCollisionViewerEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Actor Viewer", CVar_GetS32("gActorViewerEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f)))
            {
                bool currentValue = CVar_GetS32("gActorViewerEnabled", 0);
                CVar_SetS32("gActorViewerEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Actor Viewer", CVar_GetS32("gActorViewerEnabled", 0));
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
            if (ImGui::Button(GetWindowButtonText("Randomizer Settings", CVar_GetS32("gRandomizerSettingsEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gRandomizerSettingsEnabled", 0);
                CVar_SetS32("gRandomizerSettingsEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Randomizer Settings", CVar_GetS32("gRandomizerSettingsEnabled", 0));
            }
            ImGui::Dummy(ImVec2(0.0f, 0.0f));
            if (ImGui::Button(GetWindowButtonText("Item Tracker", CVar_GetS32("gItemTrackerEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gItemTrackerEnabled", 0);
                CVar_SetS32("gItemTrackerEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Item Tracker", CVar_GetS32("gItemTrackerEnabled", 0));
            }
            ImGui::Dummy(ImVec2(0.0f, 0.0f));
            if (ImGui::Button(GetWindowButtonText("Item Tracker Settings", CVar_GetS32("gItemTrackerSettingsEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gItemTrackerSettingsEnabled", 0);
                CVar_SetS32("gItemTrackerSettingsEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Item Tracker Settings", CVar_GetS32("gItemTrackerSettingsEnabled", 0));
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

                if (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_KEYSANITY) > 2 ||
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GERUDO_KEYS) > 0 ||
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOSS_KEYSANITY) > 2 ||
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GANONS_BOSS_KEY) > 2 || 
                    !gSaveContext.n64ddFlag) {
                    disableKeyColors = false;
                }

                const char* disableKeyColorsText = 
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

            UIWidgets::EnhancementCheckbox("Crowd Control", "gCrowdControl");
            UIWidgets::Tooltip("Requires a full SoH restart to take effect!\n\nEnables CrowdControl. Will attempt to connect to the local Crowd Control server.");

            UIWidgets::PaddedEnhancementCheckbox("Enemy Randomizer", "gRandomizedEnemies", true, false);
            UIWidgets::Tooltip(
                "Randomizes regular enemy spawns everytime they're spawned.\n\n"
                "WARNING: Potential to softlock rooms because you don't have the equipment to kill them. You can reload the room to get a new set of enemies to spawn."
            );

            ImGui::EndMenu();
        }
    }
}
