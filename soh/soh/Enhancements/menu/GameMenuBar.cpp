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

namespace UIWidgets {

    // MARK: - Enums

    enum class CheckboxGraphics {
        Cross,
        Checkmark,
        None
    };

    // MARK: - Layout Helper

    // Automatically adds newlines to break up text longer than a specified number of characters
    // Manually included newlines will still be respected and reset the line length
    // If line is midword when it hits the limit, text should break at the last encountered space
    char* WrappedText(const char* text, uint charactersPerLine = 60) {
        std::string newText(text);
        const size_t tipLength = newText.length();
        int lastSpace = -1;
        size_t currentLineLength = 0;
        for (uint currentCharacter = 0; currentCharacter < tipLength; currentCharacter++) {
            if (newText[currentCharacter] == '\n') {
                currentLineLength = 0;
                lastSpace = -1;
                continue;
            } else if (newText[currentCharacter] == ' ') {
                lastSpace = currentCharacter;
            }

            if ((currentLineLength >= charactersPerLine) && (lastSpace >= 0)) {
                newText[lastSpace] = '\n';
                currentLineLength = currentCharacter - lastSpace - 1;
                lastSpace = -1;
            }
            currentLineLength++;
        }
        
        return strdup(newText.c_str());
    }

    char* WrappedText(const std::string& text, uint charactersPerLine) {
        return WrappedText(text.c_str(), charactersPerLine);
    }


    // MARK: - UI Elements

    void Tooltip(const char* text) {
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", WrappedText(text));
        }
    }

    void Spacer(float height) {
        ImGui::Dummy(ImVec2(0.0f, height));
    }

    void PaddedSeparator(bool padTop = true, bool padBottom = true, float extraVerticalTopPadding = 0.0f, float extraVerticalBottomPadding = 0.0f) {
        if (padTop)
            Spacer(0);

        ImGui::Separator();

        if (padBottom)
            Spacer(0);
    }

    void RenderCross(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz) {
        float thickness = ImMax(sz / 5.0f, 1.0f);
        sz -= thickness * 0.5f;
        pos += ImVec2(thickness * 0.25f, thickness * 0.25f);

        draw_list->PathLineTo(ImVec2(pos.x, pos.y));
        draw_list->PathLineTo(ImVec2(pos.x + sz, pos.y + sz));
        draw_list->PathStroke(col, 0, thickness);

        draw_list->PathLineTo(ImVec2(pos.x + sz, pos.y));
        draw_list->PathLineTo(ImVec2(pos.x, pos.y + sz));
        draw_list->PathStroke(col, 0, thickness);
    }

    bool CustomCheckbox(const char* label, bool* v, bool disabled, CheckboxGraphics disabledGraphic) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

        const float square_sz = ImGui::GetFrameHeight();
        const ImVec2 pos = window->DC.CursorPos;
        const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
        ImGui::ItemSize(total_bb, style.FramePadding.y);
        if (!ImGui::ItemAdd(total_bb, id)) {
            IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
            return false;
        }

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
        if (pressed) {
            *v = !(*v);
            ImGui::MarkItemEdited(id);
        }

        const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
        ImGui::RenderNavHighlight(total_bb, id);
        ImGui::RenderFrame(check_bb.Min, check_bb.Max, ImGui::GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
        ImU32 check_col = ImGui::GetColorU32(ImGuiCol_CheckMark);
        ImU32 cross_col = ImGui::GetColorU32(ImVec4(0.50f, 0.50f, 0.50f, 1.00f));
        bool mixed_value = (g.LastItemData.InFlags & ImGuiItemFlags_MixedValue) != 0;
        if (mixed_value) {
            // Undocumented tristate/mixed/indeterminate checkbox (#2644)
            // This may seem awkwardly designed because the aim is to make ImGuiItemFlags_MixedValue supported by all widgets (not just checkbox)
            ImVec2 pad(ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)), ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)));
            window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col, style.FrameRounding);
        } else if ((!disabled && *v) || (disabled && disabledGraphic == CheckboxGraphics::Checkmark)) {
            const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
            ImGui::RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), check_col, square_sz - pad * 2.0f);
        } else if (disabled && disabledGraphic == CheckboxGraphics::Cross) {
            const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
            RenderCross(window->DrawList, check_bb.Min + ImVec2(pad, pad), cross_col, square_sz - pad * 2.0f);
        }

        ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y);
        if (g.LogEnabled)
            ImGui::LogRenderedText(&label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]");
        if (label_size.x > 0.0f)
            ImGui::RenderText(label_pos, label);

        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
        return pressed;
    }

    void EnhancementCheckbox(const char* text, const char* cvarName, bool disabled = false, const char* disabledTooltipText = "", CheckboxGraphics disabledGraphic = CheckboxGraphics::Cross) {
        if (disabled) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        bool val = (bool)CVar_GetS32(cvarName, 0);
        if (CustomCheckbox(text, &val, disabled, disabledGraphic)) {
            CVar_SetS32(cvarName, val);
            SohImGui::RequestCvarSaveOnNextTick();
        }

        if (disabled) {
            ImGui::PopStyleVar(1);
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && strcmp(disabledTooltipText, "") != 0) {
                ImGui::SetTooltip("%s", disabledTooltipText);
            }
            ImGui::PopItemFlag();
        }
    }

    void PaddedEnhancementCheckbox(const char* text, const char* cvarName, bool padTop = true, bool padBottom = true, bool disabled = false, const char* disabledTooltipText = "", CheckboxGraphics disabledGraphic = CheckboxGraphics::Cross) {
        if (padTop) Spacer(0);

        EnhancementCheckbox(text, cvarName, disabled, disabledTooltipText, disabledGraphic);

        if (padBottom) Spacer(0);
    }

    void EnhancementCombobox(const char* name, const char* ComboArray[], size_t arraySize, uint8_t FirstTimeValue) {
        if (FirstTimeValue <= 0) {
            FirstTimeValue = 0;
        }
        uint8_t selected = CVar_GetS32(name, FirstTimeValue);
        uint8_t DefaultValue = selected;
        std::string comboName = std::string("##") + std::string(name);
        if (ImGui::BeginCombo(comboName.c_str(), ComboArray[DefaultValue])) {
            for (uint8_t i = 0; i < arraySize; i++) {
                if (strlen(ComboArray[i]) > 1) {
                    if (ImGui::Selectable(ComboArray[i], i == selected)) {
                        CVar_SetS32(name, i);
                        selected = i;
                        SohImGui::RequestCvarSaveOnNextTick();
                    }
                }
            }
            ImGui::EndCombo();
        }
    }

    void PaddedText(const char* text, bool padTop = true, bool padBottom = true) {
        if (padTop)
            Spacer(0);

        ImGui::Text("%s", text);

        if (padBottom)
            Spacer(0);
    }

    void EnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue = 0, bool PlusMinusButton = false) {
        int val = CVar_GetS32(cvarName, defaultValue);
        ImGui::Text(text, val);
        if(PlusMinusButton) {
            std::string MinusBTNName = " - ##";
            MinusBTNName += cvarName;
            if (ImGui::Button(MinusBTNName.c_str())) {
                val--;
                CVar_SetS32(cvarName, val);
                SohImGui::RequestCvarSaveOnNextTick();
            }
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
        }

        if (ImGui::SliderInt(id, &val, min, max, format))
        {
            CVar_SetS32(cvarName, val);
            SohImGui::RequestCvarSaveOnNextTick();
        }

        if(PlusMinusButton) {
            std::string PlusBTNName = " + ##";
            PlusBTNName += cvarName;
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
            if (ImGui::Button(PlusBTNName.c_str())) {
                val++;
                CVar_SetS32(cvarName, val);
                SohImGui::RequestCvarSaveOnNextTick();
            }
        }

        if (val < min)
        {
            val = min;
            CVar_SetS32(cvarName, val);
            SohImGui::RequestCvarSaveOnNextTick();
        }

        if (val > max)
        {
            val = max;
            CVar_SetS32(cvarName, val);
            SohImGui::RequestCvarSaveOnNextTick();
        }
    }

    void PaddedEnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue = 0, bool PlusMinusButton = false, bool padTop = true, bool padBottom = true) {
        if (padTop)
            Spacer(0);

        EnhancementSliderInt(text, id, cvarName, min, max, format, defaultValue, PlusMinusButton);

        if (padBottom)
            Spacer(0);
    }

    void EnhancementRadioButton(const char* text, const char* cvarName, int id) {
        /*Usage :
        EnhancementRadioButton("My Visible Name","gMyCVarName", MyID);
        First arg is the visible name of the Radio button
        Second is the cvar name where MyID will be saved.
        Note: the CVar name should be the same to each Buddies.
        Example :
            EnhancementRadioButton("English", "gLanguages", 0);
            EnhancementRadioButton("German", "gLanguages", 1);
            EnhancementRadioButton("French", "gLanguages", 2);
        */
        std::string make_invisible = "##";
        make_invisible += text;
        make_invisible += cvarName;

        int val = CVar_GetS32(cvarName, 0);
        if (ImGui::RadioButton(make_invisible.c_str(), id == val)) {
            CVar_SetS32(cvarName, id);
            SohImGui::RequestCvarSaveOnNextTick();
        }
        ImGui::SameLine();
        ImGui::Text("%s", text);
    }
}

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

    void applyEnhancementPresetDefault(void) {
        // D-pad Support on Pause and File Select
        CVar_SetS32("gDpadPauseName", 0);
        // D-pad Support in Ocarina and Text Choice
        CVar_SetS32("gDpadOcarinaText", 0);
        // D-pad Support for Browsing Shop Items
        CVar_SetS32("gDpadShop", 0);
        // D-pad as Equip Items
        CVar_SetS32("gDpadEquips", 0);
        // Allow the cursor to be on any slot
        CVar_SetS32("gPauseAnyCursor", 0);
        // Prevent Dropped Ocarina Inputs
        CVar_SetS32("gDpadNoDropOcarinaInput", 0);
        // Answer Navi Prompt with L Button
        CVar_SetS32("gNaviOnL", 0);

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

        // Red Ganon blood
        CVar_SetS32("gRedGanonBlood", 0);
        // Fish while hovering
        CVar_SetS32("gHoverFishing", 0);
        // N64 Weird Frames
        CVar_SetS32("gN64WeirdFrames", 0);
        // Bombchus out of bounds
        CVar_SetS32("gBombchusOOB", 0);

        CVar_SetS32("gGsCutscene", 0);
        // Autosave
        CVar_SetS32("gAutosave", 0);
    }

    void applyEnhancementPresetVanillaPlus(void) {
        // D-pad Support in Ocarina and Text Choice
        CVar_SetS32("gDpadOcarinaText", 1);
        // D-pad Support for Browsing Shop Items
        CVar_SetS32("gDpadShop", 1);
        // D-pad as Equip Items
        CVar_SetS32("gDpadEquips", 1);
        // Prevent Dropped Ocarina Inputs
        CVar_SetS32("gDpadNoDropOcarinaInput", 1);

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
        CVar_SetS32("gClimbSpeed", 1);
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

        // Disable Navi Call Audio
        CVar_SetS32("gDisableNaviCallAudio", 1);

        // Equipment Toggle
        CVar_SetS32("gEquipmentCanBeRemoved", 1);
        // Count Golden Skulltulas
        CVar_SetS32("gInjectSkulltulaCount", 1);

        // Enable 3D Dropped items/projectiles
        CVar_SetS32("gNewDrops", 1);

        // Fix Anubis fireballs
        CVar_SetS32("gAnubisFix", 1);
    }

    void applyEnhancementPresetRandomizer(void) {
        // Allow the cursor to be on any slot
        CVar_SetS32("gPauseAnyCursor", 1);

        // Instant Fishing
        CVar_SetS32("gInstantFishing", 1);
        // Guarantee Bite
        CVar_SetS32("gGuaranteeFishingBite", 1);
        // Child Minimum Weight (6 to 10)
        CVar_SetS32("gChildMinimumWeightFish", 6);
        // Adult Minimum Weight (8 to 13)
        CVar_SetS32("gAdultMinimumWeightFish", 8);

        // Visual Stone of Agony
        CVar_SetS32("gVisualAgony", 1);
        // Pull grave during the day
        CVar_SetS32("gDayGravePull", 1);
        // Pull out Ocarina to Summon Scarecrow
        CVar_SetS32("gSkipScarecrow", 0);

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

    void Draw() {
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

                // TODO mutual exclusions -- There should be some system to prevent conclifting enhancements from being selected
                UIWidgets::PaddedEnhancementCheckbox("D-pad Support on Pause and File Select", "gDpadPauseName");
                UIWidgets::Tooltip("Enables Pause and File Select screen navigation with the D-pad\nIf used with D-pad as Equip Items, you must hold C-Up to equip instead of navigate");
                UIWidgets::PaddedEnhancementCheckbox("D-pad Support in Text Choice", "gDpadText", true, false);
                UIWidgets::PaddedEnhancementCheckbox("D-pad Support for Browsing Shop Items", "gDpadShop", true, false);
                UIWidgets::PaddedEnhancementCheckbox("D-pad as Equip Items", "gDpadEquips", true, false);
                UIWidgets::Tooltip("Allows the D-pad to be used as extra C buttons");
                UIWidgets::PaddedEnhancementCheckbox("Allow the cursor to be on any slot", "gPauseAnyCursor", true, false);
                UIWidgets::Tooltip("Allows the cursor on the pause menu to be over any slot\nSimilar to Rando and Spaceworld 97");
                UIWidgets::PaddedEnhancementCheckbox("Prevent Dropped Ocarina Inputs", "gDpadNoDropOcarinaInput", true, false);
                UIWidgets::Tooltip("Prevent dropping inputs when playing the ocarina quickly");
                UIWidgets::PaddedEnhancementCheckbox("Answer Navi Prompt with L Button", "gNaviOnL", true, false);
                UIWidgets::Tooltip("Speak to Navi with L but enter first-person camera with C-Up");
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
                    UIWidgets::PaddedEnhancementCheckbox("MM Bunny Hood", "gMMBunnyHood", true, false);
                    UIWidgets::Tooltip("Wearing the Bunny Hood grants a speed increase like in Majora's Mask");
                    UIWidgets::PaddedEnhancementCheckbox("Fast Chests", "gFastChests", true, false);
                    UIWidgets::Tooltip("Kick open every chest");
                    UIWidgets::PaddedEnhancementCheckbox("Skip Pickup Messages", "gFastDrops", true, false);
                    UIWidgets::Tooltip("Skip pickup messages for new consumable items and bottle swipes");
                    UIWidgets::PaddedEnhancementCheckbox("Better Owl", "gBetterOwl", true, false);
                    UIWidgets::Tooltip("The default response to Kaepora Gaebora is always that you understood what he said");
                    UIWidgets::PaddedEnhancementCheckbox("Fast Ocarina Playback", "gFastOcarinaPlayback", true, false);
                    UIWidgets::Tooltip("Skip the part where the Ocarina playback is called when you play a song");
                    UIWidgets::PaddedEnhancementCheckbox("Skip Scarecrow Song", "gSkipScarecrow", true, false);
                    UIWidgets::Tooltip("Pierre appears when Ocarina is pulled out. Requires learning scarecrow song.");
                    UIWidgets::PaddedEnhancementCheckbox("Instant Putaway", "gInstantPutaway", true, false);
                    UIWidgets::Tooltip("Allow Link to put items away without having to wait around");
                    UIWidgets::PaddedEnhancementCheckbox("Instant Boomerang Recall", "gFastBoomerang", true, false);
                    UIWidgets::Tooltip("Instantly return the boomerang to Link by pressing its item button while it's in the air");
                    UIWidgets::PaddedEnhancementCheckbox("Mask Select in Inventory", "gMaskSelect", true, false);
                    UIWidgets::Tooltip("After completing the mask trading sub-quest, press A and any direction on the mask slot to change masks");
                    UIWidgets::PaddedEnhancementCheckbox("Remember Save Location", "gRememberSaveLocation", true, false);
                    UIWidgets::Tooltip("When loading a save, places Link at the last entrance he went through.\n"
                            "This doesn't work if the save was made in a grotto.");
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
                    UIWidgets::PaddedEnhancementCheckbox("Enable Bombchu Drops", "gBombchuDrops", true, false);
                    UIWidgets::Tooltip("Bombchus will sometimes drop in place of bombs");
                    UIWidgets::PaddedEnhancementCheckbox("No Heart Drops", "gNoHeartDrops", true, false);
                    UIWidgets::Tooltip("Disables heart drops, but not heart placements, like from a Deku Scrub running off\nThis simulates Hero Mode from other games in the series");
                    UIWidgets::PaddedEnhancementCheckbox("Always Win Goron Pot", "gGoronPot", true, false);
                    UIWidgets::Tooltip("Always get the heart piece/purple rupee from the spinning Goron pot");
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
                        UIWidgets::PaddedEnhancementSliderInt("Child Minimum Weight: %d", "##cMinimumWeight", "gChildMinimumWeightFish", 6, 10, "", 10, false, true, false);
                        UIWidgets::Tooltip("The minimum weight for the unique fishing reward as a child");
                        UIWidgets::PaddedEnhancementSliderInt("Adult Minimum Weight: %d", "##aMinimumWeight", "gAdultMinimumWeightFish", 8, 13, "", 13, false, true, false);
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
                "To disable saving when obtaining an item, manually set gAutosaveAllItems and gAutosaveMajorItems to 0\n"
                "gAutosaveAllItems takes priority over gAutosaveMajorItems if both are set to 1\n"
                "gAutosaveMajorItems excludes rupees and health/magic/ammo refills (but includes bombchus)");

            UIWidgets::Spacer(0);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
            static ImVec2 buttonSize(200.0f, 0.0f);
            if (ImGui::Button(GetWindowButtonText("Cosmetics Editor", CVar_GetS32("gCosmeticsEditorEnabled", 0)).c_str(), buttonSize))
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
            #ifdef __WIIU__
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
            UIWidgets::Tooltip("Holding down B skips text\nKnown to cause a cutscene softlock in Water Temple\nSoftlock can be fixed by pressing D-Right in Debug mode");
            UIWidgets::PaddedEnhancementCheckbox("Free Camera", "gFreeCamera", true, false);
            UIWidgets::Tooltip("Enables camera control\nNote: You must remap C buttons off of the right stick in the controller config menu, and map the camera stick to the right stick.");

         #ifdef __SWITCH__
            UIWidgets::Spacer(0);
            int slot = CVar_GetS32("gSwitchPerfMode", (int)SwitchProfiles::STOCK);
            ImGui::Text("Switch performance mode");
            if (ImGui::BeginCombo("##perf", SWITCH_CPU_PROFILES[slot])) {
                for (int sId = 0; sId <= SwitchProfiles::POWERSAVINGM3; sId++) {
                    if (ImGui::Selectable(SWITCH_CPU_PROFILES[sId], sId == slot)) {
                        SPDLOG_INFO("Profile:: %s", SWITCH_CPU_PROFILES[sId]);
                        CVar_SetS32("gSwitchPerfMode", sId);
                        Switch::ApplyOverclock();
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
            UIWidgets::PaddedSeparator();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0,0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
            static ImVec2 buttonSize(160.0f, 0.0f);
            if (ImGui::Button(GetWindowButtonText("Stats", CVar_GetS32("gStatsEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gStatsEnabled", 0);
                CVar_SetS32("gStatsEnabled", !currentValue);
                SohImGui::ToggleStatisticsWindow(true);
                SohImGui::RequestCvarSaveOnNextTick();
            }
            UIWidgets::Tooltip("Shows the stats window, with your FPS and frametimes, and the OS you're playing on");
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Console", CVar_GetS32("gConsoleEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gConsoleEnabled", 0);
                CVar_SetS32("gConsoleEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::ToggleConsoleWindow(!currentValue);
            }
            UIWidgets::Tooltip("Enables the console window, allowing you to input commands, type help for some examples");
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Save Editor", CVar_GetS32("gSaveEditorEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gSaveEditorEnabled", 0);
                CVar_SetS32("gSaveEditorEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Save Editor", CVar_GetS32("gSaveEditorEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Collision Viewer", CVar_GetS32("gCollisionViewerEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gCollisionViewerEnabled", 0);
                CVar_SetS32("gCollisionViewerEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Collision Viewer", CVar_GetS32("gCollisionViewerEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Actor Viewer", CVar_GetS32("gActorViewerEnabled", 0)).c_str(), buttonSize))
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
            static ImVec2 buttonSize(200.0f, 0.0f);
            if (ImGui::Button(GetWindowButtonText("Randomizer Settings", CVar_GetS32("gRandomizerSettingsEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gRandomizerSettingsEnabled", 0);
                CVar_SetS32("gRandomizerSettingsEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Randomizer Settings", CVar_GetS32("gRandomizerSettingsEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Item Tracker", CVar_GetS32("gItemTrackerEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gItemTrackerEnabled", 0);
                CVar_SetS32("gItemTrackerEnabled", !currentValue);
                SohImGui::RequestCvarSaveOnNextTick();
                SohImGui::EnableWindow("Item Tracker", CVar_GetS32("gItemTrackerEnabled", 0));
            }
            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor(1);

            ImGui::EndMenu();
        }
    }
}
