#include "SohMenu.h"
#include <soh/Enhancements/mods.h>
#include <soh/Enhancements/game-interactor/GameInteractor.h>
#include <soh/OTRGlobals.h>

bool isBetaQuestEnabled = false;

extern "C" {
    void enableBetaQuest() { isBetaQuestEnabled = true; }
    void disableBetaQuest() { isBetaQuestEnabled = false; }
}

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets2;

void SohMenu::AddMenuEnhancements() {
    // Add Enhancements Menu
    AddMenuEntry("Enhancements", CVAR_SETTING("Menu.EnhancementsSidebarSection"));


    // Enhancements
    WidgetPath path = { "Enhancements", "Enhancements", SECTION_COLUMN_1 };
    path.sidebarName = "Time Savers";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);

    AddWidget(path, "Cutscenes", WIDGET_SEPARATOR_TEXT);
    bool allSkipsChecked = false;
    AddWidget(path, "Skip All", WIDGET_CHECKBOX)
        .ValuePointer(&allSkipsChecked)
        .PreFunc([](WidgetInfo& info) {
            *std::get<bool*>(info.valuePointer) =
            CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Intro"), IS_RANDO) &&
            CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Entrances"), IS_RANDO) &&
            CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO) &&
            CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.LearnSong"), IS_RANDO) &&
            CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"), IS_RANDO) &&
            CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.QuickBossDeaths"), IS_RANDO) &&
            CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.OnePoint"), IS_RANDO) &&
            CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipOwlInteractions"), IS_RANDO) &&
            CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO) &&
            CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.DisableTitleCard"), IS_RANDO);
        })
        .Callback([](WidgetInfo& info) {
            int32_t newValue = *std::get<bool*>(info.valuePointer) ? 1 : 0;

            CVarSetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Intro"), newValue);
            CVarSetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Entrances"), newValue);
            CVarSetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), newValue);
            CVarSetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.LearnSong"), newValue);
            CVarSetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"), newValue);
            CVarSetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.QuickBossDeaths"), newValue);
            CVarSetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.OnePoint"), newValue);
            CVarSetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipOwlInteractions"), newValue);
            CVarSetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), newValue);
            CVarSetInteger(CVAR_ENHANCEMENT("TimeSavers.DisableTitleCard"), newValue);

            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        });
    AddWidget(path, "Skip Intro", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Intro"))
        .Options(CheckboxOptions().DefaultValue(IS_RANDO));
    AddWidget(path, "Skip Entrance Cutscenes", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Entrances"))
        .Options(CheckboxOptions().DefaultValue(IS_RANDO));
    AddWidget(path, "Skip Story Cutscenes", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"))
        .Options(CheckboxOptions().DefaultValue(IS_RANDO));
    AddWidget(path, "Skip Song Cutscenes", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.LearnSong"))
        .Options(CheckboxOptions().DefaultValue(IS_RANDO));
    AddWidget(path, "Skip Boss Introductions", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"))
        .Options(CheckboxOptions().DefaultValue(IS_RANDO));
    AddWidget(path, "Quick Boss Deaths", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.QuickBossDeaths"))
        .Options(CheckboxOptions().DefaultValue(IS_RANDO));
    AddWidget(path, "Skip One Point Cutscenes (Chests, Door Unlocks, etc.)", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.OnePoint"))
        .Options(CheckboxOptions().DefaultValue(IS_RANDO));
    AddWidget(path, "Skip Owl Interactions", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipOwlInteractions"))
        .Options(CheckboxOptions().DefaultValue(IS_RANDO));
    AddWidget(path, "Skip Misc Interactions", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"))
        .Options(CheckboxOptions().DefaultValue(IS_RANDO));
    AddWidget(path, "Disable Title Card", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.DisableTitleCard"))
        .Options(CheckboxOptions().DefaultValue(IS_RANDO));
    AddWidget(path, "Exclude Glitch-Aiding Cutscenes", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.GlitchAiding"))
        .Options(CheckboxOptions().Tooltip(
            "Don't skip cutscenes that are associated wiht useful glitches. Currently, it is "
            "only the Fire Temple Darunia CS, Forest Temple Poe Sisters CS, and the Box Skip One "
            "Point in Jabu."
        ));
    AddWidget(path, "Skip Child Stealth", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipChildStealth"))
        .Options(CheckboxOptions().Tooltip(
            "The crawlspace into Hyrule Castle goes straight to Zelda, skipping the guards."
        ));
    AddWidget(path, "Skip Tower Escape", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipTowerEscape"))
        .Options(CheckboxOptions().Tooltip(
            "Skip the tower escape sequence between Ganondorf and Ganon."
        ));
    AddWidget(path, "Skip Get Item Animations", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipGetItemAnimation"))
        .Options(ComboboxOptions().ComboMap(skipGetItemAnimationOptions).DefaultIndex(SGIA_DISABLED));
    AddWidget(path, "Item Scale: %.2f", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipGetItemAnimationScale"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipGetItemAnimation"), SGIA_DISABLED) == SGIA_DISABLED;
        })
        .Options(FloatSliderOptions()
            .Min(5.0f)
            .Max(15.0f)
            .Format("%.2f")
            .DefaultValue(10.0f)
            .Tooltip(
                "The size of the item when it is picked up"
            ));
    
    AddWidget(path, "Text", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Skip Forced Dialog", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipForcedDialog"))
        .Options(CheckboxOptions().Tooltip(
            "Prevent forced conversations with Navi or other NPCs."
        ));
    AddWidget(path, "Text Speed: %dx", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("TextSpeed"))
        .Options(IntSliderOptions()
            .Min(1)
            .Max(5)
            .DefaultValue(1)
            .Format("%dx")
        );
    AddWidget(path, "Skip Text", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SkipText"))
        .Options(CheckboxOptions().Tooltip("Holding down B skips text."));
    AddWidget(path, "Slow Text Speed: %dx", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("SlowTextSpeed"))
        .Options(IntSliderOptions()
            .Min(1)
            .Max(5)
            .DefaultValue(1)
            .Format("%dx")
            .Tooltip(
                "Changes the speed of sections of text that normally are paced slower than the text surrounding it."
            )
        );
    AddWidget(path, "Match Normal Text", WIDGET_BUTTON)
        .Callback([](WidgetInfo& info) {
            CVarSetInteger(CVAR_ENHANCEMENT("SlowTextSpeed"), CVarGetInteger(CVAR_ENHANCEMENT("TextSpeed"), 1));
        })
        .Options(ButtonOptions().Tooltip(
            "Makes the speed of the slow text match the normal text speed above."
        ));
    AddWidget(path, "Skip Pickup Messages", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FastDrops"))
        .Options(CheckboxOptions().Tooltip(
            "Skip Pickup Messages for new Consumable Items and Bottle Swipes."
        ));
    AddWidget(path, "Better Owl", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("BetterOwl"))
        .Options(CheckboxOptions().Tooltip(
            "The default response to Kaepora Gaebora is always that you understood what he said."
        ));

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Gameplay", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Skip Save Confirmation", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SkipSaveConfirmation"))
        .Options(CheckboxOptions().Tooltip("Skip the \"Game Saved\" confirmation screen."));
        AddWidget(path, "Biggoron Forge Time: %d days", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("ForgeTime"))
        .Options(IntSliderOptions()
            .Min(0)
            .Max(3)
            .DefaultValue(3)
            .Format("%d days")
            .Tooltip(
                "Allows you to change the number of days it takes for "
                "Biggoron to forge the Biggoron's Sword."
            )
        );
    AddWidget(path, "Remember Save Location", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("RememberSaveLocation"))
        .Options(CheckboxOptions().Tooltip(
            "When loading a save, places Link at the last entrance he went through.\n"
            "This doesn't work if the save was made in grottos, fairy fountains, or dungeons."
        ));
    AddWidget(path, "Navi Timer Resets", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ResetNaviTimer"))
        .Options(CheckboxOptions().Tooltip(
            "Resets the Navi timer on scene change. If you have already talked to her, "
            "she will try and talk to you again, instead of needing a save warp or death."
        ));
    AddWidget(path, "No Skulltula Freeze", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SkulltulaFreeze"))
        .Options(CheckboxOptions().Tooltip(
            "Stops the game from freezing the player when picking up Gold Skulltula Tokens."
        ));
    AddWidget(path, "Ask to Equip New Items", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("AskToEquip"))
        .Options(CheckboxOptions().Tooltip(
            "Adds a prompt to equip newly-obtained Swords, Shields, and Tunics."
        ));
    AddWidget(path, "Link as Default File Name", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("LinkDefaultName"))
        .Options(CheckboxOptions().Tooltip(
            "Allows you to have \"Link\" as a premade file name."
        ));
    AddWidget(path, "Quit Fishing At Door", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("QuitFishingAtDoor"))
        .Options(CheckboxOptions().Tooltip(
            "Fisherman asks if you want to quit at the door if you try to leave the Fishing Pond "
            "while still holding the Fishing Rod."
        ));
    AddWidget(path, "Time Travel with Song of Time", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("TimeTravel"))
        .Options(ComboboxOptions()
            .ComboMap(timeTravelOptions)
            .DefaultIndex(0)
            .Tooltip(
                "Allows Link to freely change age by playing the Song of Time.\n"
                "Time Blocks can still be used properly.\n\n"
                "Requirements:\n"
                " - Obtained the Ocarina of Time (depends on selection)\n"
                " - Obtained the Song of Time\n"
                " - Obtained the Master Sword\n"
                " - Not within range of a Time Block\n"
                " - Not within range of Ocarina Playing spots."
            )
        );
    AddWidget(path, "Pause Warp", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("PauseWarp"))
        .Options(CheckboxOptions().Tooltip(
            "Selection of warp song in pause menu initiates a warp. Disables song playback."
        ));
    AddWidget(path, "Skip Scarecrow's Song", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("InstantScarecrow"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = IS_RANDO && OTRGlobals::Instance->gRandoContext->GetOption(RSK_SKIP_SCARECROWS_SONG);
            info.options->disabledTooltip = "This setting is forcefully enabled because a randomized "
            "save file with the option \"Skip Scarecrow Song\" is currently loaded.";
        })
        .Options(CheckboxOptions().Tooltip(
            "Pierre appears when an Ocarina is pulled out. Requires learning the Scarecrow's Song first."
        ));
    AddWidget(path, "Time of Day", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Nighttime GS Always Spawn", WIDGET_CVAR_CHECKBOX)
    .CVar(CVAR_ENHANCEMENT("NightGSAlwaysSpawn"))
    .Options(CheckboxOptions().Tooltip(
        "Nighttime Skulltulas will spawn during both day and night."
    ));
    AddWidget(path, "Dampe Appears All Night", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DampeAllNight"))
        .Options(CheckboxOptions().Tooltip(
            "Makes Dampe appear anytime during the night, not just his usual working hours."
        ));
        AddWidget(path, "Exit Market at Night", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("MarketSneak"))
        .Options(CheckboxOptions().Tooltip(
            "Allows exiting Hyrule Castle Market Town to Hyrule Field at night by speaking to the guard "
            "next to the gate."
        ));
    AddWidget(path, "Shops and Games Always Open", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("OpenAllHours"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = IS_RANDO && OTRGlobals::Instance->gRandoContext->GetOption(RSK_LOCK_OVERWORLD_DOORS).Is(RO_GENERIC_ON);
        })
        .Options(CheckboxOptions().Tooltip(
            "Shops and Minigames are open both day and night. Requires a scene reload to take effect."
        ).DisabledTooltip(
            "This is not compatible with the Locked Overworld Doors Randomizer option."
        ));
    path.column = SECTION_COLUMN_3;
    AddWidget(path, "Animations", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "King Zora Speed: %.2fx", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_ENHANCEMENT("MweepSpeed"))
        .Options(FloatSliderOptions()
            .Min(0.1f)
            .Max(5.0f)
            .DefaultValue(1.0f)
            .Format("%.2fx")
        );
    AddWidget(path, "Vine/Ladder Climb Speed +%d", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("Climb Speed"))
        .Options(IntSliderOptions()
            .Min(0)
            .Max(12)
            .DefaultValue(0)
            .Format("+%d")
        );
    AddWidget(path, "Block Pushing Speed +%d", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("FasterBlockPush"))
        .Options(IntSliderOptions()
            .Min(0)
            .Max(5)
            .DefaultValue(0)
            .Format("+%d")
        );
    AddWidget(path, "Crawl Speed %dx", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("CrawlSpeed"))
        .Options(IntSliderOptions()
            .Min(1)
            .Max(4)
            .DefaultValue(1)
            .Format("%dx")
        );
    AddWidget(path, "Faster Heavy Block Lift", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FasterHeavyBlockLift"))
        .Options(CheckboxOptions().Tooltip(
            "Speeds up lifting Silver Rocks and Obelisks."
        ));
    AddWidget(path, "Fast Ocarina Playback", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FastOcarinaPlayback"))
        .Options(CheckboxOptions().Tooltip(
            "Skip the part where the Ocarina Playback is called when you play a song."
        ));
    AddWidget(path, "Skip Magic Arrow Equip Animation", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SkipArrowAnimation"));
    AddWidget(path, "Faster Farore's Wind", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FastFarores"))
        .Options(CheckboxOptions().Tooltip("Greatly decreases cast time of Farore's Wind magic spell."));
    AddWidget(path, "Fast Chests", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FastChests"))
        .Options(CheckboxOptions().Tooltip(
            "Makes Link always kick the chest to open it, instead of doing the longer "
            "chest opening animation for major items."
        ));
    AddWidget(path, "Skip Water Take Breath Animation", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SkipSwimDeepEndAnim"))
        .Options(CheckboxOptions().Tooltip(
            "Skips Link's taking breath animation after coming up from water. "
            "This setting does not interfere with getting items from underwater."
        ));
    AddWidget(path, "Play Zelda's Lullaby to Open Sleeping Waterfall", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SleepingWaterfall"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = IS_RANDO && OTRGlobals::Instance->gRandoContext->GetOption(RSK_SLEEPING_WATERFALL).Is(RO_WATERFALL_OPEN);
            info.options->disabledTooltip = "This setting is forcefully enabled because a randomizer savefile with \"Sleeping Waterfall: Open\" is loaded.";
        })
        .Options(ComboboxOptions()
            .ComboMap(sleepingWaterfallOptions)
            .DefaultIndex(WATERFALL_ALWAYS)
            .Tooltip(
                "Always: Link must always play Zelda's Lullaby to open the waterfall entrance to Zora's Domain.\n"
                "Once: Link only needs to play Zelda's Lullaby once to open the waterfall; after that, it stays "
                "open permanently.\n"
                "Never: Link never needs to play Zelda's Lullaby to open the waterfall. He only needs to have "
                "learned it and have an Ocarina."
            )
        );

    path.sidebarName = "Graphics";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;
    AddWidget(path, "Filler", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Chest Size & Texture Matches Contents", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"))
        .Callback([](WidgetInfo& info) {
            if (CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), CSMC_DISABLED) == CSMC_DISABLED) {
                CVarSetInteger(CVAR_ENHANCEMENT("ChestSizeDependsStoneOfAgony"), 0);
            }
        })
        .Options(ComboboxOptions()
            .ComboMap(chestStyleMatchesContentsOptions)
            .DefaultIndex(CSMC_DISABLED)
            .Tooltip(
                "Chest sizes and textures are changed to help identify the item inside.\n"
                " - Major items: Large gold chests\n"
                " - Lesser items: Large brown chests\n"
                " - Junk items: Small brown chests\n"
                " - Small keys: Small silver chests\n"
                " - Boss keys: Vanilla size and texture\n"
                " - Skulltula Tokens: Small skulltula chest\n"
                "\n"
                "NOTE: Textures will not apply if you are using a mod pack with a custom chest model."
            )
        );
    AddWidget(path, "Chests of Agony", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ChestSizeDependsStoneOfAgony"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchesContents"), CSMC_DISABLED);
        })
        .Options(CheckboxOptions().Tooltip(
            "Only change the size/texture of chests if you have the Stone of Agony."
        ));

    // Cheats
    path.sidebarName = "Cheats";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "Inventory", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Super Tunic", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("SuperTunic"))
        .Options(CheckboxOptions().Tooltip(
            "Makes every tunic have the effects of every other tunic."
        ));
    AddWidget(path, "Easy ISG", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("EasyISG"))
        .Options(CheckboxOptions().Tooltip(
            "Passive Infinite Sword Glitch\n"
            "It makes your sword's swing effect and hitbox stay active indefinitely."
        ));
    AddWidget(path, "Easy QPA", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("EasyQPA"))
        .Options(CheckboxOptions().Tooltip(
            "Gives you the glitched damage value of the quick put away glitch."
        ));
    AddWidget(path, "Timeless Equipment", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("TimelessEquipment"))
        .Options(CheckboxOptions().Tooltip(
            "Allows any item to be equipped, regardless of age.\n"
            "Also allows Child to use Adult strength upgrades."
        ));
    AddWidget(path, "Unrestricted Items", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("Unrestricted Items"))
        .Options(CheckboxOptions().Tooltip(
            "Allows you to use any item at any location"
        ));
    AddWidget(path, "Fireproof Deku Shield", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("FireproofDekuShield"))
        .Options(CheckboxOptions().Tooltip(
            "Prevents the Deku Shield from burning on contact with fire."
        ));
    AddWidget(path, "Shield with Two-Handed Weapons", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("ShieldTwoHanded"))
        .Options(CheckboxOptions().Tooltip(
            "This allows you to put up for shield with any two-handed weapon in hand except for Deku Sticks."
        ));
    AddWidget(path, "Deku Sticks:", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_CHEAT("DekuStick"))
        .Options(ComboboxOptions().ComboMap(
            dekuStickCheat
        ).DefaultIndex(DEKU_STICK_NORMAL));
    AddWidget(path, "Bomb Timer Multiplier: %.2fx", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_CHEAT("BombTimerMultiplier"))
        .Options(FloatSliderOptions()
            .Format("%.2f")
            .Min(0.1f)
            .Max(5.0f)
            .DefaultValue(1.0f));
    AddWidget(path, "Hookshot Everything", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("HookshotEverything"))
        .Options(CheckboxOptions().Tooltip(
            "Makes every surface in the game hookshot-able."
        ));
    AddWidget(path, "Hookshot Reach Multiplier: %.2fx", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_CHEAT("HookshotReachMultiplier"))
        .Options(FloatSliderOptions()
            .Format("%.2f")
            .Min(1.0f)
            .Max(5.0f));
    AddWidget(path, "Change Age", WIDGET_BUTTON)
        .Options(ButtonOptions().Tooltip("Switches Link's age and reloads the area."))
        .Callback([](WidgetInfo& info){
            SwitchAge();
        });
    AddWidget(path, "Clear Cutscene Pointer", WIDGET_BUTTON)
        .Callback([](WidgetInfo& info) {
            GameInteractor::RawAction::ClearCutscenePointer();
        })
        .Options(ButtonOptions().Tooltip(
            "Clears the cutscene pointer to a value safe for wrong warps."
        ));

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Infinite...", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Money", WIDGET_CVAR_CHECKBOX).CVar(CVAR_CHEAT("InfiniteMoney"));
    AddWidget(path, "Health", WIDGET_CVAR_CHECKBOX).CVar(CVAR_CHEAT("InfiniteHealth"));
    AddWidget(path, "Ammo", WIDGET_CVAR_CHECKBOX).CVar(CVAR_CHEAT("InfiniteAmmo"));
    AddWidget(path, "Magic", WIDGET_CVAR_CHECKBOX).CVar(CVAR_CHEAT("InfiniteMagic"));
    AddWidget(path, "Nayru's Love", WIDGET_CVAR_CHECKBOX).CVar(CVAR_CHEAT("InfiniteNayru"));
    AddWidget(path, "Epona Boost", WIDGET_CVAR_CHECKBOX).CVar(CVAR_CHEAT("InfiniteEponaBoost"));

    AddWidget(path, "Save States", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, ICON_FA_EXCLAMATION_TRIANGLE " WARNING!!!! " ICON_FA_EXCLAMATION_TRIANGLE, WIDGET_TEXT)
        .Options(WidgetOptions().Color(Colors::Orange));
    AddWidget(path,
        "These are NOT like emulator states. They do not save your game progress "
        "and they WILL break across transitions and load zones (like doors). "
        "Support for related issues will not be provided.", WIDGET_TEXT
    );
    AddWidget(path, "I promise I have read the warning", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("SaveStatePromise"))
        .Callback([](WidgetInfo& info) {
            CVarSetInteger(CVAR_CHEAT("SaveStatesEnabled"), 0);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        });
    AddWidget(path, "I understand, enable save states", WIDGET_CVAR_CHECKBOX)
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_CHEAT("SaveStatePromise"), 0) == 0;
        })
        .CVar(CVAR_CHEAT("SaveStatesEnabled"))
        .Options(CheckboxOptions().Tooltip(
            "F5 to save, F6 to change slots, F7 to load"
        ));
    
    AddWidget(path, "Behavior", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "No Clip", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("NoClip"))
        .Options(CheckboxOptions().Tooltip("Allows you to walk through walls."));
    AddWidget(path, "Climb Everything", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("ClimbEverything"))
        .Options(CheckboxOptions().Tooltip("Makes every surface in the game climbable."));
    AddWidget(path, "Moon Jump on L", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("MoonJumpOnL"))
        .Options(CheckboxOptions().Tooltip("Holding L makes you float into the air."));
    AddWidget(path, "New Easy Frame Advancing", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("EasyFrameAdvance"))
        .Options(CheckboxOptions().Tooltip(
            "Continue holding START button when unpausing to only advance a single frame and then re-pause."
        ));
    AddWidget(path, "Drops Don't Despawn", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("DropsDontDie"))
        .Options(CheckboxOptions().Tooltip(
            "Drops from enemies, grass, etc. don't disappear after a set amount of time."
        ));
    AddWidget(path, "Fish Don't Despawn", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("NoFishDespawn"))
        .Options(CheckboxOptions().Tooltip(
            "Prevents fish from automatically despawning after a while when dropped."
        ));
    AddWidget(path, "Bugs Don't Despawn", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("NoBugsDespawn"))
        .Options(CheckboxOptions().Tooltip(
            "Prevents bugs from automatically despawning after a while when dropped."
        ));
    AddWidget(path, "Freeze Time", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("FreezeTime"))
        .Options(CheckboxOptions().Tooltip("Freezes the time of day"));
    AddWidget(path, "Time Sync", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("TimeSync"))
        .Options(CheckboxOptions().Tooltip("Syncs the in-game time with the real world time."));
    AddWidget(path, "No ReDead/Gibdo Freeze", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("NoRedeadFreeze"))
        .Options(CheckboxOptions().Tooltip(
            "Prevents ReDeads and Gibdos from being able to freeze you with their scream."
        ));
    AddWidget(path, "Keese/Guay Don't Target You", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("NoKeeseGuayTarget"))
        .Options(CheckboxOptions().Tooltip(
            "Keese and Guay no longer target you and simply ignore you as if you were wearing the "
            "Skull Mask."
        ));
    path.column = SECTION_COLUMN_3;
    AddWidget(path, "Beta Quest", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Enable Beta Quest", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("EnableBetaQuest"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = !isBetaQuestEnabled;
        })
        .Callback([](WidgetInfo& info) {
            if (CVarGetInteger(CVAR_CHEAT("EnableBetaQuest"), 0) == 0) {
                CVarClear(CVAR_CHEAT("BetaQuestWorld"));
            } else {
                CVarSetInteger(CVAR_CHEAT("BetaQuestWorld"), 0);
            }
            std::reinterpret_pointer_cast<Ship::ConsoleWindow>(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        })
        .Options(CheckboxOptions().Tooltip(
            "Turns on OoT Beta Quest. *WARNING* This will reset your game."
        ));
    AddWidget(path, "Beta Quest World: %d", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_CHEAT("BetaQuestWorld"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = info.isHidden = CVarGetInteger(CVAR_CHEAT("EnableBetaQuest"), 0) == 0;
        })
        .Callback([](WidgetInfo& info) {
            std::reinterpret_pointer_cast<Ship::ConsoleWindow>(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        })
        .Options(IntSliderOptions()
            .DefaultValue(0)
            .Min(0)
            .Max(8)
            .Tooltip(
                "Set the Beta Quest world to explore. *WARNING* Changing this will reset your game.\n"
                "Ctrl+Click to type in a value."
            ));

    // Cosmetics Editor
    path.sidebarName = "Cosmetics Editor";
    AddSidebarEntry("Enhancements", path.sidebarName, 1);
    AddWidget(path, "Popout Cosmetics Editor Window", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("CosmeticsEditor"))
        .WindowName("Cosmetics Editor")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Cosmetics Editor Window."));


    // Audio Editor
    path.sidebarName = "Audio Editor";
    AddSidebarEntry("Enhancements", path.sidebarName, 1);
    AddWidget(path, "Popout Audio Editor Window", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("AudioEditor"))
        .WindowName("Audio Editor")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Audio Editor Window."));


    // Gameplay Stats
    path.sidebarName = "Gameplay Stats";
    AddSidebarEntry("Enhancements", path.sidebarName, 2);
    AddWidget(path, "Popout Gameplay Stats Window", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("GameplayStats"))
        .WindowName("Gameplay Stats")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Gameplay Stats Window."));


    // Time Splits
    path.sidebarName = "Time Splits";
    AddSidebarEntry("Enhancements", path.sidebarName, 1);
    AddWidget(path, "Popout Time Splits Window", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("TimeSplits"))
        .WindowName("Time Splits")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Time Splits Window."));


    // Timers
    path.sidebarName = "Timers";
    AddSidebarEntry("Enhancements", path.sidebarName, 1);
    AddWidget(path, "Toggle Timers Window", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("AdditionalTimers"))
        .WindowName("Additional Timers")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Additional Timers Window."));
}

} // namespace SohGui
