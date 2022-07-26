#include "enhancements.h"
#include "../libultraship/ImGuiImpl.h"
#include "../libultraship/Lib/Fast3D/gfx_pc.h"
#include "CVar.h"

#ifdef __SWITCH__
#include "../libultraship/SwitchImpl.h"
#endif

using namespace Ship;

extern "C" {

static const char* powers[9] = {
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

void Enhancements_Draw(void) {
    if (ImGui::BeginMenu("Gameplay")) {
        if (ImGui::BeginMenu("Time Savers")) {
            SohImGui::EnhancementSliderInt("Text Speed: %dx", "##TEXTSPEED", "gTextSpeed", 1, 5, "");
            SohImGui::EnhancementSliderInt("King Zora Speed: %dx", "##MWEEPSPEED", "gMweepSpeed", 1, 5, "");
            SohImGui::EnhancementSliderInt("Biggoron Forge Time: %d days", "##FORGETIME", "gForgeTime", 0, 3, "");
            SohImGui::Tooltip("Allows you to change the number of days it takes for\nBiggoron to forge the Biggoron Sword");
            SohImGui::EnhancementSliderInt("Vine/Ladder Climb speed +%d", "##CLIMBSPEED", "gClimbSpeed", 0, 12, "");

            SohImGui::EnhancementCheckbox("Faster Block Push", "gFasterBlockPush");
            SohImGui::EnhancementCheckbox("No Forced Navi", "gNoForcedNavi");
            SohImGui::Tooltip("Prevent forced Navi conversations");
            SohImGui::EnhancementCheckbox("No Skulltula Freeze", "gSkulltulaFreeze");
            SohImGui::Tooltip("Stops the game from freezing the player\nwhen picking up Gold Skulltulas");
            SohImGui::EnhancementCheckbox("MM Bunny Hood", "gMMBunnyHood");
            SohImGui::Tooltip("Wearing the Bunny Hood grants a speed\nincrease like in Majora's Mask");
            SohImGui::EnhancementCheckbox("Fast Chests", "gFastChests");
            SohImGui::Tooltip("Kick open every chest");
            SohImGui::EnhancementCheckbox("Fast Drops", "gFastDrops");
            SohImGui::Tooltip("Skip first-time pickup messages for consumable items");
            SohImGui::EnhancementCheckbox("Better Owl", "gBetterOwl");
            SohImGui::Tooltip("The default response to Kaepora Gaebora is\nalways that you understood what he said");
            SohImGui::EnhancementCheckbox("Fast Ocarina Playback", "gFastOcarinaPlayback");
            SohImGui::Tooltip("Skip the part where the Ocarina playback is called when you play\na song");
            SohImGui::EnhancementCheckbox("Prevent Dropped Ocarina Inputs", "gDpadNoDropOcarinaInput");
            SohImGui::Tooltip("Prevent dropping inputs when playing the ocarina quickly");
            SohImGui::EnhancementCheckbox("Instant Putaway", "gInstantPutaway");
            SohImGui::Tooltip("Allow Link to put items away without having to wait around");
            SohImGui::EnhancementCheckbox("Mask Select in Inventory", "gMaskSelect");
            SohImGui::Tooltip("After completing the mask trading sub-quest,\npress A and any direction on the mask slot to "
                    "change masks");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Difficulty Options")) {
            ImGui::Text("Damage Multiplier");
            SohImGui::EnhancementCombobox("gDamageMul", powers, 9, 0);
            SohImGui::Tooltip("Modifies all sources of damage not affected by other sliders\n\
2x: Can survive all common attacks from the start of the game\n\
4x: Dies in 1 hit to any substantial attack from the start of the game\n\
8x: Can only survive trivial damage from the start of the game\n\
16x: Can survive all common attacks with max health without double defense\n\
32x: Can survive all common attacks with max health and double defense\n\
64x: Can survive trivial damage with max health without double defense\n\
128x: Can survive trivial damage with max health and double defense\n\
256x: Cannot survive damage");
            ImGui::Text("Fall Damage Multiplier");
            SohImGui::EnhancementCombobox("gFallDamageMul", powers, 8, 0);
            SohImGui::Tooltip("Modifies all fall damage\n\
2x: Can survive all fall damage from the start of the game\n\
4x: Can only survive short fall damage from the start of the game\n\
8x: Cannot survive any fall damage from the start of the game\n\
16x: Can survive all fall damage with max health without double defense\n\
32x: Can survive all fall damage with max health and double defense\n\
64x: Can survive short fall damage with double defense\n\
128x: Cannot survive fall damage");
            ImGui::Text("Void Damage Multiplier");
            SohImGui::EnhancementCombobox("gVoidDamageMul", powers, 7, 0);
            SohImGui::Tooltip("Modifies damage taken after falling into a void\n\
2x: Can survive void damage from the start of the game\n\
4x: Cannot survive void damage from the start of the game\n\
8x: Can survive void damage twice with max health without double defense\n\
16x: Can survive void damage with max health without double defense\n\
32x: Can survive void damage with max health and double defense\n\
64x: Cannot survive void damage");

            SohImGui::EnhancementCheckbox("No Random Drops", "gNoRandomDrops");
            SohImGui::Tooltip("Disables random drops, except from the Goron Pot, Dampe, and bosses");
            SohImGui::EnhancementCheckbox("No Heart Drops", "gNoHeartDrops");
            SohImGui::Tooltip("Disables heart drops, but not heart placements, like from a Deku Scrub running off\nThis "
                    "simulates Hero Mode from other games in the series");
            SohImGui::EnhancementCheckbox("Always Win Goron Pot", "gGoronPot");
            SohImGui::Tooltip("Always get the heart piece/purple rupee from the spinning Goron pot");

            if (ImGui::BeginMenu("Potion Values")) {
                SohImGui::EnhancementCheckbox("Change Red Potion Effect", "gRedPotionEffect");
                SohImGui::Tooltip("Enable the following changes to the amount of health restored by Red Potions");
                SohImGui::EnhancementSliderInt("Red Potion Health: %d", "##REDPOTIONHEALTH", "gRedPotionHealth", 1, 100, "", 0,
                                     true);
                SohImGui::Tooltip("Changes the amount of health restored by Red Potions");
                SohImGui::EnhancementCheckbox("Red Potion Percent Restore", "gRedPercentRestore");
                SohImGui::Tooltip("Toggles from Red Potions restoring a fixed amount of health to a percent of the player's "
                        "current max health");

                SohImGui::EnhancementCheckbox("Change Green Potion Effect", "gGreenPotionEffect");
                SohImGui::Tooltip("Enable the following changes to the amount of mana restored by Green Potions");
                SohImGui::EnhancementSliderInt("Green Potion Mana: %d", "##GREENPOTIONMANA", "gGreenPotionMana", 1, 100, "", 0,
                                     true);
                SohImGui::Tooltip("Changes the amount of mana restored by Green Potions, base max mana is 48, max upgraded mana "
                        "is 96");
                SohImGui::EnhancementCheckbox("Green Potion Percent Restore", "gGreenPercentRestore");
                SohImGui::Tooltip("Toggles from Green Potions restoring a fixed amount of mana to a percent of the player's "
                        "current max mana");

                SohImGui::EnhancementCheckbox("Change Blue Potion Effects", "gBluePotionEffects");
                SohImGui::Tooltip("Enable the following changes to the amount of health and mana restored by Blue Potions");
                SohImGui::EnhancementSliderInt("Blue Potion Health: %d", "##BLUEPOTIONHEALTH", "gBluePotionHealth", 1, 100, "", 0,
                                     true);
                SohImGui::Tooltip("Changes the amount of health restored by Blue Potions");
                SohImGui::EnhancementCheckbox("Blue Potion Health Percent Restore", "gBlueHealthPercentRestore");
                SohImGui::Tooltip("Toggles from Blue Potions restoring a fixed amount of health to a percent of the player's "
                        "current max health");

                SohImGui::EnhancementSliderInt("Blue Potion Mana: %d", "##BLUEPOTIONMANA", "gBluePotionMana", 1, 100, "", 0,
                                     true);
                SohImGui::Tooltip("Changes the amount of mana restored by Blue Potions, base max mana is 48, max upgraded mana "
                        "is 96");
                SohImGui::EnhancementCheckbox("Blue Potion Mana Percent Restore", "gBlueManaPercentRestore");
                SohImGui::Tooltip("Toggles from Blue Potions restoring a fixed amount of mana to a percent of the player's "
                        "current max mana");

                SohImGui::EnhancementCheckbox("Change Milk Effect", "gMilkEffect");
                SohImGui::Tooltip("Enable the following changes to the amount of health restored by Milk");
                SohImGui::EnhancementSliderInt("Milk Health: %d", "##MILKHEALTH", "gMilkHealth", 1, 100, "", 0, true);
                SohImGui::Tooltip("Changes the amount of health restored by Milk");
                SohImGui::EnhancementCheckbox("Milk Percent Restore", "gMilkPercentRestore");
                SohImGui::Tooltip("Toggles from Milk restoring a fixed amount of health to a percent of the player's current max "
                        "health");

                SohImGui::EnhancementCheckbox("Separate Half Milk Effect", "gSeparateHalfMilkEffect");
                SohImGui::Tooltip("Enable the following changes to the amount of health restored by Half Milk\nIf this is "
                        "disabled, Half Milk will behave the same as Full Milk.");
                SohImGui::EnhancementSliderInt("Half Milk Health: %d", "##HALFMILKHEALTH", "gHalfMilkHealth", 1, 100, "", 0,
                                     true);
                SohImGui::Tooltip("Changes the amount of health restored by Half Milk");
                SohImGui::EnhancementCheckbox("Half Milk Percent Restore", "gHalfMilkPercentRestore");
                SohImGui::Tooltip("Toggles from Half Milk restoring a fixed amount of health to a percent of the player's "
                        "current max health");

                SohImGui::EnhancementCheckbox("Change Fairy Effect", "gFairyEffect");
                SohImGui::Tooltip("Enable the following changes to the amount of health restored by Fairies");
                SohImGui::EnhancementSliderInt("Fairy: %d", "##FAIRYHEALTH", "gFairyHealth", 1, 100, "", 0, true);
                SohImGui::Tooltip("Changes the amount of health restored by Fairies");
                SohImGui::EnhancementCheckbox("Fairy Percent Restore", "gFairyPercentRestore");
                SohImGui::Tooltip("Toggles from Fairies restoring a fixed amount of health to a percent of the player's current "
                        "max health");

                SohImGui::EnhancementCheckbox("Change Fairy Revive Effect", "gFairyReviveEffect");
                SohImGui::Tooltip("Enable the following changes to the amount of health restored by Fairy Revivals");
                SohImGui::EnhancementSliderInt("Fairy Revival: %d", "##FAIRYREVIVEHEALTH", "gFairyReviveHealth", 1, 100, "", 0,
                                     true);
                SohImGui::Tooltip("Changes the amount of health restored by Fairy Revivals");
                SohImGui::EnhancementCheckbox("Fairy Revive Percent Restore", "gFairyRevivePercentRestore");
                SohImGui::Tooltip("Toggles from Fairy Revivals restoring a fixed amount of health to a percent of the player's "
                        "current max health");

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Fishing")) {
                SohImGui::EnhancementCheckbox("Instant Fishing", "gInstantFishing");
                SohImGui::Tooltip("All fish will be caught instantly");
                SohImGui::EnhancementCheckbox("Guarantee Bite", "gGuaranteeFishingBite");
                SohImGui::Tooltip("When a line is stable, guarantee bite. Otherwise use default logic");
                SohImGui::EnhancementSliderInt("Child Minimum Weight: %d", "##cMinimumWeight", "gChildMinimumWeightFish", 6, 10,
                                     "", 10);
                SohImGui::Tooltip("The minimum weight for the unique fishing reward as a child");
                SohImGui::EnhancementSliderInt("Adult Minimum Weight: %d", "##aMinimumWeight", "gAdultMinimumWeightFish", 8, 13,
                                     "", 13);
                SohImGui::Tooltip("The minimum weight for the unique fishing reward as an adult");
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Reduced Clutter")) {
            SohImGui::EnhancementCheckbox("Mute Low HP Alarm", "gLowHpAlarm");
            SohImGui::Tooltip("Disable the low HP beeping sound");
            SohImGui::EnhancementCheckbox("Minimal UI", "gMinimalUI");
            SohImGui::Tooltip("Hides most of the UI when not needed\nNote: Doesn't activate until after loading a new scene");
            SohImGui::EnhancementCheckbox("Disable Navi Call Audio", "gDisableNaviCallAudio");
            SohImGui::Tooltip("Disables the voice audio when Navi calls you");

            ImGui::EndMenu();
        }

        SohImGui::EnhancementCheckbox("Visual Stone of Agony", "gVisualAgony");
        SohImGui::Tooltip(
            "Displays an icon and plays a sound when Stone of Agony\nshould be activated, for those without rumble");
        SohImGui::EnhancementCheckbox("Assignable Tunics and Boots", "gAssignableTunicsAndBoots");
        SohImGui::Tooltip("Allows equipping the tunic and boots to c-buttons");
        SohImGui::EnhancementCheckbox("Link's Cow in Both Time Periods", "gCowOfTime");
        SohImGui::Tooltip("Allows the Lon Lon Ranch obstacle course reward to be\nshared across time periods");
        SohImGui::EnhancementCheckbox("Enable visible guard vision", "gGuardVision");
        SohImGui::EnhancementCheckbox("Enable passage of time on file select", "gTimeFlowFileSelect");
        SohImGui::EnhancementCheckbox("Allow the cursor to be on any slot", "gPauseAnyCursor");
        SohImGui::Tooltip("Allows the cursor on the pause menu to be over any slot\nSimilar to Rando and Spaceworld 97");
        SohImGui::EnhancementCheckbox("Count Golden Skulltulas", "gInjectSkulltulaCount");
        SohImGui::Tooltip("Injects Golden Skulltula total count in pickup messages");
        SohImGui::EnhancementCheckbox("Pull grave during the day", "gDayGravePull");
        SohImGui::Tooltip("Allows graves to be pulled when child during the day");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Graphics")) {
        if (ImGui::BeginMenu("Animated Link in Pause Menu")) {
            ImGui::Text("Rotation");
            SohImGui::EnhancementRadioButton("Disabled", "gPauseLiveLinkRotation", 0);
            SohImGui::EnhancementRadioButton("Rotate Link with D-pad", "gPauseLiveLinkRotation", 1);
            SohImGui::Tooltip("Allow you to rotate Link on the Equipment menu with the DPAD\nUse DPAD-Up or DPAD-Down to reset "
                    "Link's rotation");
            SohImGui::EnhancementRadioButton("Rotate Link with C-buttons", "gPauseLiveLinkRotation", 2);
            SohImGui::Tooltip("Allow you to rotate Link on the Equipment menu with the C-buttons\nUse C-Up or C-Down to reset "
                    "Link's rotation");
            SohImGui::EnhancementRadioButton("Rotate Link with Right Stick", "gPauseLiveLinkRotation", 3);
            SohImGui::Tooltip("Allow you to rotate Link on the Equipment menu with the Right Stick\nYou can zoom in by pointing "
                    "up and reset Link's rotation by pointing down");

            if (CVar_GetS32("gPauseLiveLinkRotation", 0) != 0) {
                SohImGui::EnhancementSliderInt("Rotation Speed: %d", "##MinRotationSpeed", "gPauseLiveLinkRotationSpeed", 1, 20,
                                     "");
            }
            ImGui::Separator();
            ImGui::Text("Static loop");
            SohImGui::EnhancementRadioButton("Disabled", "gPauseLiveLink", 0);
            SohImGui::EnhancementRadioButton("Idle (standing)", "gPauseLiveLink", 1);
            SohImGui::EnhancementRadioButton("Idle (look around)", "gPauseLiveLink", 2);
            SohImGui::EnhancementRadioButton("Idle (belt)", "gPauseLiveLink", 3);
            SohImGui::EnhancementRadioButton("Idle (shield)", "gPauseLiveLink", 4);
            SohImGui::EnhancementRadioButton("Idle (test sword)", "gPauseLiveLink", 5);
            SohImGui::EnhancementRadioButton("Idle (yawn)", "gPauseLiveLink", 6);
            SohImGui::EnhancementRadioButton("Battle Stance", "gPauseLiveLink", 7);
            SohImGui::EnhancementRadioButton("Walking (no shield)", "gPauseLiveLink", 8);
            SohImGui::EnhancementRadioButton("Walking (holding shield)", "gPauseLiveLink", 9);
            SohImGui::EnhancementRadioButton("Running (no shield)", "gPauseLiveLink", 10);
            SohImGui::EnhancementRadioButton("Running (holding shield)", "gPauseLiveLink", 11);
            SohImGui::EnhancementRadioButton("Hand on hip", "gPauseLiveLink", 12);
            SohImGui::EnhancementRadioButton("Spin attack charge", "gPauseLiveLink", 13);
            SohImGui::EnhancementRadioButton("Look at hand", "gPauseLiveLink", 14);
            ImGui::Separator();
            ImGui::Text("Randomize");
            SohImGui::EnhancementRadioButton("Random", "gPauseLiveLink", 15);
            SohImGui::Tooltip("Randomize the animation played each time you open the menu");
            SohImGui::EnhancementRadioButton("Random cycle", "gPauseLiveLink", 16);
            SohImGui::Tooltip("Randomize the animation played on the menu after a certain time");
            SohImGui::EnhancementRadioButton("Random cycle (Idle)", "gPauseLiveLink", 17);
            SohImGui::Tooltip("Randomize the animation played on the menu after a certain time (Idle animations only)");
            if (CVar_GetS32("gPauseLiveLink", 0) >= 16) {
                SohImGui::EnhancementSliderInt("Frame to wait: %d", "##MinFrameCount", "gMinFrameCount", 1, 1000, "", 0, true);
            }

            ImGui::EndMenu();
        }
        SohImGui::EnhancementCheckbox("N64 Mode", "gN64Mode");
        SohImGui::Tooltip("Sets aspect ratio to 4:3 and lowers resolution to 240p, the N64's native resolution");
        SohImGui::EnhancementCheckbox("Enable 3D Dropped items/projectiles", "gNewDrops");
        SohImGui::Tooltip("Change most 2D items and projectiles on the overworld to their 3D versions");
        SohImGui::EnhancementCheckbox("Disable Black Bar Letterboxes", "gDisableBlackBars");
        SohImGui::Tooltip("Disables Black Bar Letterboxes during cutscenes and Z-targeting\nNote: there may be minor visual "
                "glitches that\nwere covered up by the black bars\nPlease disable this setting before reporting a bug");
        SohImGui::EnhancementCheckbox("Dynamic Wallet Icon", "gDynamicWalletIcon");
        SohImGui::Tooltip("Changes the rupee in the wallet icon to match the wallet size you currently have");
        SohImGui::EnhancementCheckbox("Always show dungeon entrances", "gAlwaysShowDungeonMinimapIcon");
        SohImGui::Tooltip("Always shows dungeon entrance icons on the minimap");

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Fixes")) {
        SohImGui::EnhancementCheckbox("Fix L&R Pause menu", "gUniformLR");
        SohImGui::Tooltip("Makes the L and R buttons in the pause menu the same color");
        SohImGui::EnhancementCheckbox("Fix L&Z Page switch in Pause menu", "gNGCKaleidoSwitcher");
        SohImGui::Tooltip("Makes L and R switch pages like on the GameCube\nZ opens the Debug Menu instead");
        SohImGui::EnhancementCheckbox("Fix Dungeon entrances", "gFixDungeonMinimapIcon");
        SohImGui::Tooltip("Removes the dungeon entrance icon on the top-left corner\nof the screen when no dungeon is present on "
                "the current map");
        SohImGui::EnhancementCheckbox("Fix Two Handed idle animations", "gTwoHandedIdle");
        SohImGui::Tooltip("Re-enables the two-handed idle animation, a seemingly\nfinished animation that was disabled on "
                "accident in the original game");
        SohImGui::EnhancementCheckbox("Fix the Gravedigging Tour Glitch", "gGravediggingTourFix");
        SohImGui::Tooltip("Fixes a bug where the Gravedigging Tour Heart\nPiece disappears if the area reloads");
        SohImGui::EnhancementCheckbox("Fix Deku Nut upgrade", "gDekuNutUpgradeFix");
        SohImGui::Tooltip(
            "Prevents the Forest Stage Deku Nut upgrade from\nbecoming unobtainable after receiving the Poacher's Saw");
        SohImGui::EnhancementCheckbox("Fix Navi text HUD position", "gNaviTextFix");
        SohImGui::Tooltip("Correctly centers the Navi text prompt on the HUD's C-Up button");
        SohImGui::EnhancementCheckbox("Fix Anubis fireballs", "gAnubisFix");
        SohImGui::Tooltip("Make Anubis fireballs do fire damage when reflected\nback at them with the Mirror Shield");
        SohImGui::EnhancementCheckbox("Fix Megaton Hammer crouch stab", "gCrouchStabHammerFix");
        SohImGui::Tooltip("Make the Megaton Hammer's crouch stab able to destroy\nrocks without first swinging it normally");
        if (CVar_GetS32("gCrouchStabHammerFix", 0) == 0) {
            CVar_SetS32("gCrouchStabFix", 0);
        } else {
            SohImGui::EnhancementCheckbox("Remove power crouch stab", "gCrouchStabFix");
            SohImGui::Tooltip("Make crouch stabbing always do the same damage as a regular slash");
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Restoration")) {
        SohImGui::EnhancementCheckbox("Red Ganon blood", "gRedGanonBlood");
        SohImGui::Tooltip("Restore the original red blood from NTSC 1.0/1.1. Disable for green blood");
        SohImGui::EnhancementCheckbox("Fish while hovering", "gHoverFishing");
        SohImGui::Tooltip("Restore a bug from NTSC 1.0 that allows casting\nthe Fishing Rod while using the Hover Boots");
        SohImGui::EnhancementCheckbox("N64 Weird Frames", "gN64WeirdFrames");
        SohImGui::Tooltip("Restores N64 Weird Frames allowing weirdshots to behave the same as N64");
        SohImGui::EnhancementCheckbox("Bombchus out of bounds", "gBombchusOOB");
        SohImGui::Tooltip("Allows bombchus to explode out of bounds\nSimilar to GameCube and Wii VC");

        ImGui::EndMenu();
    }

    LUS_IMGUI_EXPERIMENTAL();

    const char* fps_cvar = "gInterpolationFPS";
    {
#ifdef __SWITCH__
        int minFps = 20;
        int maxFps = 60;
#else
        int minFps = 20;
        int maxFps = 250;
#endif

        int val = CVar_GetS32(fps_cvar, minFps);
        val = std::max(std::min(val, maxFps), 20);
        int fps = val;

        if (fps == 20) {
            ImGui::Text("Frame interpolation: Off");
        } else {
            ImGui::Text("Frame interpolation: %d FPS", fps);
        }

        std::string MinusBTNFPSI = " - ##FPSInterpolation";
        std::string PlusBTNFPSI = " + ##FPSInterpolation";
        if (ImGui::Button(MinusBTNFPSI.c_str())) {
            val--;
            CVar_SetS32(fps_cvar, val);
            SohImGui::MarkCVarsNeedSave();
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);

        if (ImGui::SliderInt("##FPSInterpolation", &val, minFps, maxFps, "", ImGuiSliderFlags_AlwaysClamp)) {
            if (val > 250) {
                val = 250;
            } else if (val < 20) {
                val = 20;
            }

            CVar_SetS32(fps_cvar, val);
            SohImGui::MarkCVarsNeedSave();
        }

        SohImGui::Tooltip("Interpolate extra frames to get smoother graphics\n"
                "Set to match your monitor's refresh rate, or a divisor of it\n"
                "A higher target FPS than your monitor's refresh rate will just waste resources,\n"
                "and might give a worse result.\n"
                "For consistent input lag, set this value and your monitor's refresh rate to a multiple of 20\n"
                "Ctrl+Click for keyboard input");

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
        if (ImGui::Button(PlusBTNFPSI.c_str())) {
            val++;
            CVar_SetS32(fps_cvar, val);
            SohImGui::MarkCVarsNeedSave();
        }
    }
    if (SohImGui::impl.backend == SohImGui::Backend::DX11) {
        if (ImGui::Button("Match Refresh Rate")) {
            int hz = roundf(gfx_get_detected_hz());
            if (hz >= 20 && hz <= 250) {
                CVar_SetS32(fps_cvar, hz);
                SohImGui::MarkCVarsNeedSave();
            }
        }
    }
    SohImGui::EnhancementCheckbox("Disable LOD", "gDisableLOD");
    SohImGui::Tooltip("Turns off the Level of Detail setting, making models use their higher-poly variants at any distance");
    SohImGui::EnhancementCheckbox("Disable Draw Distance", "gDisableDrawDistance");
    SohImGui::Tooltip("Turns off the objects draw distance,\nmaking objects being visible from a longer range");
    if (CVar_GetS32("gDisableDrawDistance", 0) == 0) {
        CVar_SetS32("gDisableKokiriDrawDistance", 0);
    } else if (CVar_GetS32("gDisableDrawDistance", 0) == 1) {
        SohImGui::EnhancementCheckbox("Kokiri Draw Distance", "gDisableKokiriDrawDistance");
        SohImGui::Tooltip("The Kokiri are mystical beings that fade into view when approached\nEnabling this will remove their "
                "draw distance");
    }
    SohImGui::EnhancementCheckbox("Skip Text", "gSkipText");
    SohImGui::Tooltip("Holding down B skips text\nKnown to cause a cutscene softlock in Water Temple\nSoftlock can be fixed by "
            "pressing D-Right in Debug mode");

    SohImGui::EnhancementCheckbox("Free Camera", "gFreeCamera");
    SohImGui::Tooltip("Enables camera control\nNote: You must remap C buttons off of\nthe right stick in the controller\nconfig "
            "menu, and map the camera stick\nto the right stick.");

#ifdef __SWITCH__
    int slot = CVar_GetS32("gSwitchPerfMode", (int)SwitchProfiles::STOCK);
    ImGui::Text("Switch performance mode");
    if (ImGui::BeginCombo("##perf", SWITCH_CPU_PROFILES[slot])) {
        for (int sId = 0; sId <= SwitchProfiles::POWERSAVINGM3; sId++) {
            if (ImGui::Selectable(SWITCH_CPU_PROFILES[sId], sId == slot)) {
                INFO("Profile:: %s", SWITCH_CPU_PROFILES[sId]);
                CVar_SetS32("gSwitchPerfMode", sId);
                Switch::ApplyOverclock();
                SohImGui::MarkCVarsNeedSave();
            }
        }
        ImGui::EndCombo();
    }
#endif
}

void Enhancements_Init(void) {
    SohImGui::AddTab("Enhancements", Enhancements_Draw);
}

}
