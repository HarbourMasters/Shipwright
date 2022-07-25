#include "Cvar.h"
#include "applypresets.h"

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

        // Ironman
        case 4:
            applyEnhancementPresetDefault();
            applyEnhancementPresetIronman();
            break;
    }
}

void applyEnhancementPresetDefault(void) {
    // Text Speed (1 to 5)
    CVar_SetS32("gTextSpeed", 1);
    // King Zora Speed (1 to 5)
    CVar_SetS32("gMweepSpeed", 1);
    // Biggoron Forge Time (0 to 3)
    CVar_SetS32("gForgeTime", 3);
    // Vine/Ladder Climb speed (+0 to +12)
    CVar_SetS32("gClimbSpeed", 0);
    // Faster Block Push
    CVar_SetS32("gFasterBlockPush", 0);
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
    // Prevent Dropped Ocarina Inputs
    CVar_SetS32("gDpadNoDropOcarinaInput", 0);
    // Instant Putaway
    CVar_SetS32("gInstantPutaway", 0);
    // Mask Select in Inventory
    CVar_SetS32("gMaskSelect", 0);

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
    // Link's Cow in Both Time Periods
    CVar_SetS32("gCowOfTime", 0);
    // Enable visible guard vision
    CVar_SetS32("gGuardVision", 0);
    // Enable passage of time on file select
    CVar_SetS32("gTimeFlowFileSelect", 0);
    // Allow the cursor to be on any slot
    CVar_SetS32("gPauseAnyCursor", 0);
    // Count Golden Skulltulas
    CVar_SetS32("gInjectSkulltulaCount", 0);
    // Pull grave during the day
    CVar_SetS32("gDayGravePull", 0);

    // Rotate link (0 to 2)
    CVar_SetS32("gPauseLiveLinkRotation", 0);
    // Pause link animation (0 to 17)
    CVar_SetS32("gPauseLiveLink", 0);

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
}

void applyEnhancementPresetVanillaPlus(void) {
    // Text Speed (1 to 5)
    CVar_SetS32("gTextSpeed", 5);
    // King Zora Speed (1 to 5)
    CVar_SetS32("gMweepSpeed", 2);
    // Faster Block Push
    CVar_SetS32("gFasterBlockPush", 1);
    // Better Owl
    CVar_SetS32("gBetterOwl", 1);
    // Prevent Dropped Ocarina Inputs
    CVar_SetS32("gDpadNoDropOcarinaInput", 1);

    // Assignable Tunics and Boots
    CVar_SetS32("gAssignableTunicsAndBoots", 1);
    // Enable passage of time on file select
    CVar_SetS32("gTimeFlowFileSelect", 1);
    // Count Golden Skulltulas
    CVar_SetS32("gInjectSkulltulaCount", 1);

    // Pause link animation (0 to 17)
    CVar_SetS32("gPauseLiveLink", 17);

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
    // Mask Select in Inventory
    CVar_SetS32("gMaskSelect", 1);

    // Disable Navi Call Audio
    CVar_SetS32("gDisableNaviCallAudio", 1);

    // Count Golden Skulltulas
    CVar_SetS32("gInjectSkulltulaCount", 1);

    // Enable 3D Dropped items/projectiles
    CVar_SetS32("gNewDrops", 1);

    // Fix Anubis fireballs
    CVar_SetS32("gAnubisFix", 1);
}

void applyEnhancementPresetRandomizer(void) {
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
    // Allow the cursor to be on any slot
    CVar_SetS32("gPauseAnyCursor", 1);
    // Pull grave during the day
    CVar_SetS32("gDayGravePull", 1);
}

void applyEnhancementPresetIronman(void) {
    // Text Speed (1 to 5)
    CVar_SetS32("gTextSpeed", 5);
    // No Forced Navi
    CVar_SetS32("gNoForcedNavi", 1);
    // No Skulltula Freeze
    CVar_SetS32("gSkulltulaFreeze", 1);
    // Fast Chests
    CVar_SetS32("gFastChests", 1);
    // Fast Drops
    CVar_SetS32("gFastDrops", 1);
    // Better Owl
    CVar_SetS32("gBetterOwl", 1);
    // Fast Ocarina Playback
    CVar_SetS32("gFastOcarinaPlayback", 1);
    // Prevent Dropped Ocarina Inputs
    CVar_SetS32("gDpadNoDropOcarinaInput", 1);

    // Damage Multiplier (0 to 8)
    CVar_SetS32("gDamageMul", 8);
    // Fall Damage Multiplier (0 to 7)
    CVar_SetS32("gFallDamageMul", 7);
    // Void Damage Multiplier (0 to 6)
    CVar_SetS32("gVoidDamageMul", 6);
    // No Random Drops
    CVar_SetS32("gNoRandomDrops", 1);
    // No Heart Drops
    CVar_SetS32("gNoHeartDrops", 1);

    // Disable Navi Call Audio
    CVar_SetS32("gDisableNaviCallAudio", 1);

    // Assignable Tunics and Boots
    CVar_SetS32("gAssignableTunicsAndBoots", 1);
    // Enable passage of time on file select
    CVar_SetS32("gTimeFlowFileSelect", 1);
    // Count Golden Skulltulas
    CVar_SetS32("gInjectSkulltulaCount", 1);

    // Pause link animation (0 to 17)
    CVar_SetS32("gPauseLiveLink", 17);

    // Enable 3D Dropped items/projectiles
    CVar_SetS32("gNewDrops", 1);
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
    // Fix Megaton Hammer crouch stab
    CVar_SetS32("gCrouchStabHammerFix", 1);
    // Fix all crouch stab
    CVar_SetS32("gCrouchStabFix", 1);

    // Red Ganon blood
    CVar_SetS32("gRedGanonBlood", 1);
    // Fish while hovering
    CVar_SetS32("gHoverFishing", 1);
    // N64 Weird Frames
    CVar_SetS32("gN64WeirdFrames", 1);
    // Bombchus out of bounds
    CVar_SetS32("gBombchusOOB", 1);
}