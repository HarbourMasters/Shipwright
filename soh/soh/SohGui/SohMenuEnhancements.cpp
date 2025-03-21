#include "SohMenu.h"
#include <soh/Enhancements/mods.h>
#include <soh/Enhancements/game-interactor/GameInteractor.h>
#include <soh/OTRGlobals.h>
#include <soh/Enhancements/cosmetics/authenticGfxPatches.h>
#include <soh/Enhancements/enemyrandomizer.h>
#include <soh/Enhancements/presets.h>
#include <soh/Enhancements/TimeDisplay/TimeDisplay.h>

static std::string comboboxTooltip = "";
static int32_t enhancementPresetSelected = ENHANCEMENT_PRESET_DEFAULT;
bool isBetaQuestEnabled = false;

extern "C" {
    void enableBetaQuest() { isBetaQuestEnabled = true; }
    void disableBetaQuest() { isBetaQuestEnabled = false; }
}

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets;

void SohMenu::AddMenuEnhancements() {
    // Add Enhancements Menu
    AddMenuEntry("Enhancements", CVAR_SETTING("Menu.EnhancementsSidebarSection"));

    // Enhancements
    WidgetPath path = { "Enhancements", "Presets", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", path.sidebarName, 3);

    const PresetTypeDefinition presetTypeDef = presetTypes.at(PRESET_TYPE_ENHANCEMENTS);
    for (auto iter = presetTypeDef.presets.begin(); iter != presetTypeDef.presets.end(); ++iter) {
        if (iter->first != 0) comboboxTooltip += "\n\n";
        comboboxTooltip += std::string(iter->second.label) + " - " + std::string(iter->second.description);
    }
    AddWidget(path, "Enhancement Presets", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Select Preset", WIDGET_COMBOBOX)
        .ValuePointer(&enhancementPresetSelected)
        .Callback([](WidgetInfo& info) {
            const std::string presetTypeCvar = CVAR_GENERAL("SelectedPresets.") + std::to_string(PRESET_TYPE_ENHANCEMENTS);
            CVarSetInteger(presetTypeCvar.c_str(), *std::get<int32_t*>(info.valuePointer));
        })
        .Options(ComboboxOptions()
            .ComboMap(enhancementPresetList)
            .DefaultIndex(ENHANCEMENT_PRESET_DEFAULT)
            .Tooltip(comboboxTooltip.c_str())
        );
    AddWidget(path, "Apply Preset##Enhancemnts", WIDGET_BUTTON)
        .Options(ButtonOptions().Size(UIWidgets::Sizes::Inline))
        .Callback([](WidgetInfo& info) {
            const std::string presetTypeCvar = CVAR_GENERAL("SelectedPresets.") + std::to_string(PRESET_TYPE_ENHANCEMENTS);
            const PresetTypeDefinition presetTypeDef = presetTypes.at(PRESET_TYPE_ENHANCEMENTS);
            uint16_t selectedPresetId = CVarGetInteger(presetTypeCvar.c_str(), 0);
            if(selectedPresetId >= presetTypeDef.presets.size()){
                selectedPresetId = 0;
            }
            const PresetDefinition selectedPresetDef = presetTypeDef.presets.at(selectedPresetId);
            for(const char* block : presetTypeDef.blocksToClear) {
                CVarClearBlock(block);
            }
            if (selectedPresetId != 0) {
                applyPreset(selectedPresetDef.entries);
            }
            CVarSetInteger(presetTypeCvar.c_str(), selectedPresetId);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        });

    // Quality of Life
    path.sidebarName = "Quality of Life";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "Saving", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Autosave", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("Autosave"))
        .Options(CheckboxOptions().Tooltip(
            "Save the game automatically on a 3 minute interval and when soft-resetting the game. The interval "
            "autosave will wait if the game is paused in any way (dialogue, pause screen up, cutscenes, "
            "etc.).\n\n"
            "The soft-reset save will *not* trigger in cutscene maps like the Chamber of Sages!"));
    AddWidget(path, "Remember Save Location", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("RememberSaveLocation"))
        .Options(CheckboxOptions().Tooltip(
            "When loading a save, places Link at the last entrance he went through.\n"
            "This doesn't work if the save was made in grottos, fairy fountains, or dungeons."));

    AddWidget(path, "Containers Match Contents", WIDGET_SEPARATOR_TEXT);
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
                     .Tooltip("Chest sizes and textures are changed to help identify the item inside.\n"
                              " - Major items: Large gold chests\n"
                              " - Lesser items: Large brown chests\n"
                              " - Junk items: Small brown chests\n"
                              " - Small keys: Small silver chests\n"
                              " - Boss keys: Vanilla size and texture\n"
                              " - Skulltula Tokens: Small skulltula chest\n"
                              "\n"
                              "NOTE: Textures will not apply if you are using a mod pack with a custom chest model."));
    AddWidget(path, "Chests of Agony", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ChestSizeDependsStoneOfAgony"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchesContents"), CSMC_DISABLED);
        })
        .Options(CheckboxOptions().Tooltip("Only change the size/texture of chests if you have the Stone of Agony."));

    AddWidget(path, "Time of Day", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Nighttime GS Always Spawn", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("NightGSAlwaysSpawn"))
        .Options(CheckboxOptions().Tooltip("Nighttime Skulltulas will spawn during both day and night."));
    AddWidget(path, "Pull Grave During the Day", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DayGravePull"))
        .Options(CheckboxOptions().Tooltip("Allows graves to be pulled when child during the day."));
    AddWidget(path, "Dampe Appears All Night", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DampeAllNight"))
        .Options(CheckboxOptions().Tooltip(
            "Makes Dampe appear anytime during the night, not just his usual working hours."));
    AddWidget(path, "Exit Market at Night", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("MarketSneak"))
        .Options(CheckboxOptions().Tooltip(
            "Allows exiting Hyrule Castle Market Town to Hyrule Field at night by speaking to the guard "
            "next to the gate."));
    AddWidget(path, "Shops and Games Always Open", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("OpenAllHours"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled =
                IS_RANDO && OTRGlobals::Instance->gRandoContext->GetOption(RSK_LOCK_OVERWORLD_DOORS).Is(RO_GENERIC_ON);
        })
        .Options(
            CheckboxOptions()
                .Tooltip("Shops and Minigames are open both day and night. Requires a scene reload to take effect.")
                .DisabledTooltip("This is not compatible with the Locked Overworld Doors Randomizer option."));

    AddWidget(path, "Pause Menu", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Allow the Cursor to be on Any Slot", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("PauseAnyCursor"))
        .Options(
            ComboboxOptions()
                .ComboMap(cursorAnywhereValues)
                .DefaultIndex(PAUSE_ANY_CURSOR_RANDO_ONLY)
                .Tooltip("Allows the cursor on the pause menu to be over any slot. Sometimes required in Randomizer "
                         "to select certain items."));
    AddWidget(path, "Pause Warp", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("PauseWarp"))
        .Options(CheckboxOptions().Tooltip(
            "Selection of warp song in pause menu initiates a warp. Disables song playback."));

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Controls", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Answer Navi Prompt with L Button", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("NaviOnL"))
        .Options(CheckboxOptions().Tooltip("Speak to Navi with L but enter First-Person Camera with C-Up"));
    AddWidget(path, "Don't Require Input for Credits Sequence", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("NoInputForCredits"))
        .Options(CheckboxOptions().Tooltip(
            "Removes the Input Requirement on Text boxes after defeating Ganon, allowing the Credits "
            "Sequence to continue to progress."));
    AddWidget(path, "Include Held Inputs at the Start of Pause Buffer Input Window", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("IncludeHeldInputsBufferWindow"))
        .Options(CheckboxOptions().Tooltip(
            "Typically, inputs that are held prior to the buffer window are not included in the buffer. This "
            "setting changes that behavior to include them. This may cause some inputs to be re-triggered "
            "undesireably, for instance Z-Targetting something you might not want to."));
    AddWidget(path, "Pause Buffer Input Window: %d frames", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("PauseBufferWindow"))
        .Options(IntSliderOptions()
                     .Min(0)
                     .Max(40)
                     .DefaultValue(0)
                     .Format("%d frames")
                     .Tooltip("Adds back in a delay after unpausing before the game resumes playing again, "
                              "where inputs can be held prematurely to be input immediately after the game resumes. "
                              "This essentially brings back behaviour from console releases which are lost on default "
                              "because SoH isn't limited to N64 hardware."));
    AddWidget(path, "Simulated Input Lag: %d frames", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_SIMULATED_INPUT_LAG)
        .Options(IntSliderOptions()
                     .Min(0)
                     .Max(6)
                     .DefaultValue(0)
                     .Format("%d frames")
                     .Tooltip("Buffers your inputs to be executed a specified amount of frames later."));

    AddWidget(path, "Item Count Messages", WIDGET_SEPARATOR_TEXT);
    int numOptions = ARRAY_COUNT(itemCountMessageCVars);
    bool allItemCountsChecked = false;
    AddWidget(path, "All", WIDGET_CHECKBOX)
        .ValuePointer(&allItemCountsChecked)
        .PreFunc([](WidgetInfo& info) {
            int numOptions = ARRAY_COUNT(itemCountMessageCVars);
            *std::get<bool*>(info.valuePointer) = std::all_of(itemCountMessageCVars, itemCountMessageCVars + numOptions,
                                                              [](const char* cvar) { return CVarGetInteger(cvar, 0); });
        })
        .Callback([](WidgetInfo& info) {
            int32_t newValue = *std::get<bool*>(info.valuePointer) ? 1 : 0;
            int numOptions = ARRAY_COUNT(itemCountMessageCVars);
            std::for_each(itemCountMessageCVars, itemCountMessageCVars + numOptions,
                          [newValue](const char* cvar) { CVarSetInteger(cvar, newValue); });

            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        });
    for (int i = 0; i < numOptions; i++) {
        AddWidget(path, itemCountMessageOptions[i], WIDGET_CVAR_CHECKBOX).CVar(itemCountMessageCVars[i]);
    }

    path.column = SECTION_COLUMN_3;
    AddWidget(path, "Misc", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Disable Crit Wiggle", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DisableCritWiggle"))
        .Options(CheckboxOptions().Tooltip("Disable Random Camera Wiggle at Low Health."));
    AddWidget(path, "Better Owl", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("BetterOwl"))
        .Options(CheckboxOptions().Tooltip(
            "The default response to Kaepora Gaebora is always that you understood what he said."));

    AddWidget(path, "Convenience", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Quit Fishing at Door", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("QuitFishingAtDoor"))
        .Options(CheckboxOptions().Tooltip(
            "Fisherman asks if you want to quit at the door if you try to leave the Fishing Pond "
            "while still holding the Fishing Rod."));
    AddWidget(path, "Instant Putaway", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("InstantPutaway"))
        .Options(CheckboxOptions().Tooltip("Allow Link to put items away without having to wait around."));
    AddWidget(path, "Navi Timer Resets on Scene Change", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ResetNaviTimer"))
        .Options(
            CheckboxOptions().Tooltip("Resets the Navi timer on scene change. If you have already talked to her, "
                                      "she will try and talk to you again, instead of needing a save warp or death."));
    AddWidget(path, "Link's Cow in Both Time Periods", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("CowOfTime"))
        .Options(CheckboxOptions().Tooltip(
            "Allows the Lon Lon Ranch Obstacle Course reward to be shared across time periods."));
    AddWidget(path, "Play Zelda's Lullaby to Open Sleeping Waterfall", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SleepingWaterfall"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled =
                IS_RANDO &&
                OTRGlobals::Instance->gRandoContext->GetOption(RSK_SLEEPING_WATERFALL).Is(RO_WATERFALL_OPEN);
            info.options->disabledTooltip = "This setting is forcefully enabled because a randomizer savefile with "
                                            "\"Sleeping Waterfall: Open\" is loaded.";
        })
        .Options(
            ComboboxOptions()
                .ComboMap(sleepingWaterfallOptions)
                .DefaultIndex(WATERFALL_ALWAYS)
                .Tooltip(
                    "Always: Link must always play Zelda's Lullaby to open the waterfall entrance to Zora's Domain.\n"
                    "Once: Link only needs to play Zelda's Lullaby once to open the waterfall; after that, it stays "
                    "open permanently.\n"
                    "Never: Link never needs to play Zelda's Lullaby to open the waterfall. He only needs to have "
                    "learned it and have an Ocarina."));

    // Skips & Speed-ups
    path.sidebarName = "Skips & Speed-ups";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;

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
            "Point in Jabu."));

    AddWidget(path, "Text", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Skip Pickup Messages", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FastDrops"))
        .Options(CheckboxOptions().Tooltip("Skip Pickup Messages for new Consumable Items and Bottle Swipes."));
    AddWidget(path, "Skip Forced Dialog", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipForcedDialog"))
        .Options(ComboboxOptions()
            .ComboMap(skipForcedDialogOptions)
            .DefaultIndex(FORCED_DIALOG_SKIP_NONE)
            .Tooltip("Prevent forced conversations with Navi and/or other NPCs."));
    AddWidget(path, "Skip Text", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SkipText"))
        .Options(CheckboxOptions().Tooltip("Holding down B skips text."));
    AddWidget(path, "Text Speed: %dx", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("TextSpeed"))
        .Options(IntSliderOptions().Min(1).Max(5).DefaultValue(1).Format("%dx"));
    AddWidget(path, "Slow Text Speed: %dx", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("SlowTextSpeed"))
        .Options(IntSliderOptions().Min(1).Max(5).DefaultValue(1).Format("%dx").Tooltip(
            "Changes the speed of sections of text that normally are paced slower than the text surrounding it."));

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Animations", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Faster Heavy Block Lift", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FasterHeavyBlockLift"))
        .Options(CheckboxOptions().Tooltip("Speeds up lifting Silver Rocks and Obelisks."));
    AddWidget(path, "Fast Chests", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FastChests"))
        .Options(CheckboxOptions().Tooltip("Makes Link always kick the chest to open it, instead of doing the longer "
                                           "chest opening animation for major items."));
    AddWidget(path, "Skip Water Take Breath Animation", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SkipSwimDeepEndAnim"))
        .Options(CheckboxOptions().Tooltip("Skips Link's taking breath animation after coming up from water. "
                                           "This setting does not interfere with getting items from underwater."));
    AddWidget(path, "Vine/Ladder Climb Speed +%d", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("ClimbSpeed"))
        .Options(IntSliderOptions().Min(0).Max(12).DefaultValue(0).Format("+%d"));
    AddWidget(path, "Block Pushing Speed +%d", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("FasterBlockPush"))
        .Options(IntSliderOptions().Min(0).Max(5).DefaultValue(0).Format("+%d"));
    AddWidget(path, "Crawl Speed %dx", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("CrawlSpeed"))
        .Options(IntSliderOptions().Min(1).Max(4).DefaultValue(1).Format("%dx"));
    AddWidget(path, "King Zora Speed: %.2fx", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_ENHANCEMENT("MweepSpeed"))
        .Options(FloatSliderOptions().Min(0.1f).Max(5.0f).DefaultValue(1.0f).Format("%.2fx"));

    path.column = SECTION_COLUMN_3;
    AddWidget(path, "Misc", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Skip Child Stealth", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipChildStealth"))
        .Options(CheckboxOptions().Tooltip(
            "The crawlspace into Hyrule Castle goes straight to Zelda, skipping the guards."));
    AddWidget(path, "Skip Tower Escape", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeSavers.SkipTowerEscape"))
        .Options(CheckboxOptions().Tooltip("Skip the tower escape sequence between Ganondorf and Ganon."));
    AddWidget(path, "Skip Scarecrow's Song", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("InstantScarecrow"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled =
                IS_RANDO && OTRGlobals::Instance->gRandoContext->GetOption(RSK_SKIP_SCARECROWS_SONG);
            info.options->disabledTooltip = "This setting is forcefully enabled because a randomized "
                                            "save file with the option \"Skip Scarecrow Song\" is currently loaded.";
        })
        .Options(CheckboxOptions().Tooltip(
            "Pierre appears when an Ocarina is pulled out. Requires learning the Scarecrow's Song first."));
    AddWidget(path, "Faster Rupee Accumulator", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FasterRupeeAccumulator"))
        .Options(CheckboxOptions().Tooltip("Causes your Wallet to fill and empty faster when you gain or lose money."));
    AddWidget(path, "No Skulltula Freeze", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SkulltulaFreeze"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = IS_RANDO && GameInteractor::IsSaveLoaded(true);
            info.options->disabledTooltip =
                "This setting is disabled because a randomizer savefile is loaded. Please use the "
                "\"Skip Get Item Animation\" option within the randomizer enhancements instead.";
        })
        .Options(CheckboxOptions().Tooltip(
            "Stops the game from freezing the player when picking up Gold Skulltula Tokens. Does not"
            "apply in randomizer savefiles."));
    AddWidget(path, "Skip Save Confirmation", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SkipSaveConfirmation"))
        .Options(CheckboxOptions().Tooltip("Skip the \"Game Saved\" confirmation screen."));
    AddWidget(path, "Link as Default File Name", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("LinkDefaultName"))
        .Options(CheckboxOptions().Tooltip("Allows you to have \"Link\" as a premade file name."));
    AddWidget(path, "Biggoron Forge Time: %d days", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("ForgeTime"))
        .Options(IntSliderOptions().Min(0).Max(3).DefaultValue(3).Format("%d days").Tooltip(
            "Allows you to change the number of days it takes for "
            "Biggoron to forge the Biggoron's Sword."));

    // Graphics
    path.sidebarName = "Graphics";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "Mods", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Use Alternate Assets", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("AltAssets"))
        .Options(CheckboxOptions().Tooltip(
            "Toggle between standard assets and alternate assets. Usually mods will indicate if "
            "this setting has to be used or not."
        ));
    AddWidget(path, "Disable Bomb Billboarding", WIDGET_CVAR_CHECKBOX)
        .CVar("DisableBombBillboarding")
        .Options(CheckboxOptions().Tooltip(
            "Disables bombs always rotating to face the camera. To be used in conjunction with mods that want to "
            "replace bombs with 3D objects."
        ));
    AddWidget(path, "Disable Grotto Fixed Rotation", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DisableGrottoRotation"))
        .Options(CheckboxOptions().Tooltip(
            "Disables Grottos rotating with the Camera. To be used in conjuction with mods that want to "
            "replace grottos with 3D objects."
        ));
    AddWidget(path, "Ingame Text Spacing: %d", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("TextSpacing"))
        .Options(IntSliderOptions().Min(4).Max(6).DefaultValue(6).Tooltip(
            "Space between text characters (useful for HD font textures)."));
    
    AddWidget(path, "Models & Textures", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Disable LOD", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DisableLOD"))
        .Options(CheckboxOptions().Tooltip(
            "Turns off the Level of Detail setting, making models use their Higher-Poly variants at any distance."));
    AddWidget(path, "Enemy Health Bars", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("EnemyHealthBar"))
        .Options(CheckboxOptions().Tooltip("Renders a health bar for Enemies when Z-Targeted."));
    AddWidget(path, "Enable 3D Dropped Items/Projectiles", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("NewDrops"))
        .Options(CheckboxOptions().Tooltip(
            "Replaces most 2D items and projectiles on the overworld with their equivalent 3D models."));
    AddWidget(path, "Animated Link in Pause Menu", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("PauseMenuAnimatedLink"))
        .Options(CheckboxOptions().Tooltip(
            "Turns the Static Image of Link in the Pause Menu's Equipment Subsceen "
            "into a model cycling through his idle animations."
        ));
    AddWidget(path, "Show Age-Dependent Equipment", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"))
        .Callback([](WidgetInfo& info) {
            UpdatePatchHand();
        })
        .Options(CheckboxOptions().Tooltip(
            "Makes all equipment visible, regardless of Age."
        ));
    AddWidget(path, "Scale Adult Equipment as Child", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ScaleAdultEquipmentAsChild"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0) == 0;
        })
        .Options(CheckboxOptions().Tooltip(
            "Scales all of the Adult Equipment, as well as moving some a bit, to fit on Child Link better. May "
            "not work properly with some mods."
        ));
    AddWidget(path, "Show Gauntlets in First Person", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FirstPersonGauntlets"))
        .Options(CheckboxOptions().Tooltip(
            "Renders Guantlets when using the Bow and Hookshot like in OoT3D."
        ));
    AddWidget(path, "Show Chains on Both Sides of Locked Doors", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ShowDoorLocksOnBothSides"));
    AddWidget(path, "Color Temple of Time's Medallions", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ToTMedallionsColors"))
        .Callback([](WidgetInfo& info) { PatchToTMedallions(); })
        .Options(CheckboxOptions().Tooltip(
            "When Medallions are collected, the Medallion imprints around the Master Sword Pedestal in the Temple "
            "of Time will become colored-in."));
    
    path.column = SECTION_COLUMN_2;
    AddWidget(path, "UI", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Minimal UI", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("MinimalUI"))
        .Options(CheckboxOptions().Tooltip("Hides most of the UI when not needed.\n"
                                           "NOTE: Doesn't activate until scene transition."));
    AddWidget(path, "Disable Hot/Underwater Warning Text", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DisableTunicWarningText"))
        .Options(CheckboxOptions().Tooltip("Disables warning text when you don't have on the Goron/Zora Tunic "
                                           "in Hot/Underwater conditions."));
    AddWidget(path, "Remember Minimap State Between Areas", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("RememberMapToggleState"))
        .Options(CheckboxOptions().Tooltip(
            "Preverse the minimap visibility state when going between areas rather than default it to \"on\" "
            "when going through loading zones."));
    AddWidget(path, "Visual Stone of Agony", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("VisualAgony"))
        .Options(CheckboxOptions().Tooltip(
            "Displays an icon and plays a sound when Stone of Agony should be activated, for those without rumble."));
    AddWidget(path, "Disable HUD Heart Animations", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("NoHUDHeartAnimation"))
        .Options(CheckboxOptions().Tooltip("Disables the Beating Animation of the Hearts on the HUD."));
    AddWidget(path, "Glitch Line-up Tick", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DrawLineupTick"))
        .Options(CheckboxOptions().Tooltip(
            "Displays a tick in the top center of the screen to help with glitch line-ups in SoH, since traditional "
            "UI based line-ups do not work outside of 4:3"));
    AddWidget(path, "Disable Black Bar Letterboxes", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DisableBlackBars"))
        .Options(CheckboxOptions().Tooltip(
            "Disables Black Bar Letterboxes during cutscenes and Z-Targeting. NOTE: there may be minor visual "
            "glitches that were covered up by the black bars. Please disable this setting before reporting a bug."));
    AddWidget(path, "Dynamic Wallet Icon", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DynamicWalletIcon"))
        .Options(CheckboxOptions().Tooltip(
            "Changes the Rupee in the Wallet icon to match the wallet size you currently have."));
    AddWidget(path, "Always Show Dungeon Entrances", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("AlwaysShowDungeonMinimapIcon"))
        .Options(CheckboxOptions().Tooltip("Always shows dungeon entrance icons on the Minimap."));
    AddWidget(path, "More Info in File Select", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FileSelectMoreInfo"))
        .Options(CheckboxOptions().Tooltip(
            "Shows what items you have collected in the File Select screen, like in N64 Randomizer."));
    AddWidget(path, "Better Ammo Rendering in Pause Menu", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("BetterAmmoRendering"))
        .Options(CheckboxOptions().Tooltip(
            "Ammo counts in the pause menu will work correctly regardless of the position of items in the Inventory."));
    AddWidget(path, "Enable Passage of Time on File Select", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeFlowFileSelect"))
        .Options(CheckboxOptions().Tooltip(
            "The skybox in the background of the File Select screen will go through the day and night cycle over time."));

    path.column = SECTION_COLUMN_3;
    AddWidget(path, "Misc.", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "N64 Mode", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_LOW_RES_MODE)
        .Options(CheckboxOptions().Tooltip(
            "Sets the aspect ratio to 4:3 and lowers resolution to 240p, the N64's native resolution."
        ));
    AddWidget(path, "Remove Spin Attack Darkness", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("RemoveSpinAttackDarkness"))
        .Options(CheckboxOptions().Tooltip(
            "Remove the Darkness that appears when charging a Spin Attack"
        ));
    AddWidget(path, "Draw Distance", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Increase Actor Draw Distance: %dx", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("DisableDrawDistance"))
        .Callback([](WidgetInfo& info) {
            if (CVarGetInteger(CVAR_ENHANCEMENT("DisableDrawDistance"), 1) <= 1) {
                CVarSetInteger(CVAR_ENHANCEMENT("DisableKokiriDrawDistance"), 0);
            }
        })
        .Options(IntSliderOptions().Min(1).Max(5).DefaultValue(1).Format("%dx").Tooltip(
            "Increases the range in which Actors/Objects are drawn."));
    AddWidget(path, "Kokiri Draw Distance", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DisableKokiriDrawDistance"))
        .PreFunc(
            [](WidgetInfo& info) { info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("DisableDrawDistance"), 1) > 1; })
        .Options(CheckboxOptions().Tooltip(
            "The Kokiri are mystical beings that fade into view when approached. Enabling this will remove their "
            "draw distance."));
    AddWidget(path, "Widescreen Actor Culling", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("WidescreenActorCulling"))
        .Options(
            CheckboxOptions().Tooltip("Adjusts the Horizontal Culling Plane to account for Widescreen Resolutions."));
    AddWidget(path, "Cull Glitch Useful Actors", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ExtendedCullingExcludeGlitchActors"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = !CVarGetInteger(CVAR_ENHANCEMENT("WidescreenActorCulling"), 0) &&
                                     CVarGetInteger(CVAR_ENHANCEMENT("DisableDrawDistance"), 1) <= 1;
            info.options->disabledTooltip =
                "Requires Actor Draw Distance to be increased or Widscreen Actor Culling to be enabled.";
        })
        .Options(CheckboxOptions().Tooltip(
            "Exclude Actors that are useful for Glitches from the extended culling ranges. Some actors may still draw "
            "in the extended ranges, but will not \"update\" so that certain glitches that leverage the original "
            "culling requirements will still work.\n\nThe following actors are excluded:\n"
            " - White clothed Gerudos\n"
            " - King Zora\n"
            " - Gossip Stones\n"
            " - Boulders\n"
            " - Blue Warps\n"
            " - Darunia\n"
            " - Gold SKulltulas\n"));

    path.sidebarName = "Items";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "Equipment", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Equip Items on Dpad", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DpadEquips"))
        .Options(CheckboxOptions().Tooltip(
            "Equip items and equipment on the D-Pad. If used with \"D-Pad on Pause Screen\", you must "
            "hold C-Up to equip instead of navgiate."));
    AddWidget(path, "Assignable Tunics and Boots", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("AssignableTunicsAndBoots"))
        .Options(CheckboxOptions().Tooltip("Allows equipping the Tunics and Boots to C-Buttons/D-Pad."));
    // TODO: Revist strength toggle, it's currently separate but should probably be locked behind the
    // Equipment toggle settings or be absorbed by it completely.
    AddWidget(path, "Equipment Toggle", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("EquipmentCanBeRemoved"))
        .Options(CheckboxOptions().Tooltip(
            "Allows equipment to be removed by toggling it off on\n the equipment subscreen."));
    AddWidget(path, "Allow Strength Equipment to be Toggled", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ToggleStrength"))
        .Callback([](WidgetInfo& info) {
            if (!CVarGetInteger(CVAR_ENHANCEMENT("ToggleStrength"), 0)) {
                CVarSetInteger(CVAR_ENHANCEMENT("StrengthDisabled"), 0);
            }
        })
        .Options(CheckboxOptions().Tooltip(
            "Allows Strength to be toggled on and off by pressing A on the Strength Upgrade "
            "in the Equipment Subscreen of the Pause Menu. This allows performing some glitches "
            "that require the player to not have Strength."));
    AddWidget(path, "Sword Toggle Options", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("SwordToggle"))
        .PreFunc(
            [](WidgetInfo& info) { info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("EquipmentCanBeRemoved"), 0) == 0; })
        .Options(ComboboxOptions()
                     .ComboMap(swordToggleModes)
                     .DefaultIndex(SWORD_TOGGLE_NONE)
                     .Tooltip("Introduces Options for unequipping Link's sword\n\n"
                              "None: Only Biggoron's Sword/Giant's Knife can be toggled. Doing so will equip the "
                              "Master Sword.\n\n"
                              "Child Toggle: This will allow for completely unequipping any sword as child link.\n\n"
                              "Both Ages: Any sword can be unequipped as either age. This may lead to swordless "
                              "glitches as Adult."));
    AddWidget(path, "Ask to Equip New Items", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("AskToEquip"))
        .Options(CheckboxOptions().Tooltip("Adds a prompt to equip newly-obtained Swords, Shields, and Tunics."));

    AddWidget(path, "Ocarina", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Prevent Dropped Ocarina Inputs", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DpadNoDropOcarinaInput"))
        .Options(CheckboxOptions().Tooltip("Prevent dropping inputs when playing the Ocarina too quickly."));
    AddWidget(path, "Fast Ocarina Playback", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FastOcarinaPlayback"))
        .Options(CheckboxOptions().Tooltip("Skip the part where the Ocarina Playback is called when you play a song."));
    AddWidget(path, "Time Travel with Song of Time", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("TimeTravel"))
        .Options(ComboboxOptions()
                     .ComboMap(timeTravelOptions)
                     .DefaultIndex(0)
                     .Tooltip("Allows Link to freely change age by playing the Song of Time.\n"
                              "Time Blocks can still be used properly.\n\n"
                              "Requirements:\n"
                              " - Obtained the Ocarina of Time (depends on selection)\n"
                              " - Obtained the Song of Time\n"
                              " - Obtained the Master Sword\n"
                              " - Not within range of a Time Block\n"
                              " - Not within range of Ocarina Playing spots."));

    AddWidget(path, "Masks", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Bunny Hood Effect", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("MMBunnyHood"))
        .Options(CheckboxOptions().Tooltip("Wearing the Bunny Hood grants a speed increase link in Majora's Mask. "
                                           "The longer jump option is not accounted for in Randomizer logic.\n\n"
                                           "Also disables NPC's reactions to wearing the Bunny Hood."));
    AddWidget(path, "Masks Equippable as Adult", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("AdultMasks"))
        .Options(CheckboxOptions().Tooltip("Allows masks to be equipped normally from the pause menu as adult."));
    AddWidget(path, "Persistent Masks", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("PersistentMasks"))
        .Options(
            CheckboxOptions().Tooltip("Stops masks from automatically unequipping on certain situations:\n"
                                      "- When entering a new scene\n"
                                      "- When not in any C button or the D-Pad\n"
                                      "- When saving and quitting\n"
                                      "- When dying\n"
                                      "- When traveling thru time (if \"Masks Equippable as Adult\" is activated)."));
    AddWidget(path, "Invisible Bunny Hood", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("HideBunnyHood"))
        .Options(CheckboxOptions().Tooltip("Turns Bunny Hood Invisible while still maintaining its effects."));
    AddWidget(path, "Mask Select in Inventory", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("MaskSelect"))
        .Options(CheckboxOptions().Tooltip(
            "After completing the mask trading sub-quest, press A and any direction on the mask "
            "slog to change masks"));

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Explosives", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Deku Nuts Explode Bombs", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("NutsExplodeBombs"))
        .Options(CheckboxOptions().Tooltip("Make Deku Nuts explode Bombs, similar to how they interact with Bombchus. "
                                           "This does not affect Bombflowers."));
    AddWidget(path, "Remove Explosive Limit", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("RemoveExplosiveLimit"))
        .Options(CheckboxOptions().Tooltip("Removes the cap of 3 active explosives being deployed at once."));
    AddWidget(path, "Static Explosion Radius", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("StaticExplosionRadius"))
        .Options(CheckboxOptions().Tooltip(
            "Explosions are now a static size, like in Majora's Mask and OoT3D. Makes Bombchu "
            "hovering much easier."));
    AddWidget(path, "Prevent Bombchus Forcing First-Person", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DisableFirstPersonChus"))
        .Options(CheckboxOptions().Tooltip(
            "Prevent Bombchus from forcing the camera into first-person mode when released."));
    AddWidget(path, "Better Bombchu Shopping", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("BetterBombchuShopping"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = IS_RANDO && GameInteractor::IsSaveLoaded(true);
            info.options->disabledTooltip = "This setting is forcefully enabled when you are playing a randomizer.";
        })
        .Options(
            CheckboxOptions().Tooltip("Bombchus do not sell out when bought, and a 10 pack of Bombchus costs 99 rupees "
                                      "instead of 100."));

    AddWidget(path, "Bow / Slingshot", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Equip Multiple Arrows at Once", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SeparateArrows"))
        .Options(CheckboxOptions().Tooltip(
            "Allow the Bow and Magic Arrows to be equipped at the same time on different slots. "
            "NOTE: This will disable the behavior of the 'Equip Dupe' glitch."));
    AddWidget(path, "Skip Magic Arrow Equip Animation", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SkipArrowAnimation"));
    // TODO: See if a Callback could be registered to avoid the need to reload scenes for the next two options.
    AddWidget(path, "Blue Fire Arrows", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("BlueFireArrows"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled =
                OTRGlobals::Instance->gRandoContext->GetOption(RSK_BLUE_FIRE_ARROWS).Is(RO_GENERIC_ON);
            info.options->disabledTooltip = "This setting is forcefully enabled because a randomized savefile with "
                                            "\"Blue Fire Arrows\" is currently loaded.";
        })
        .Options(CheckboxOptions().Tooltip(
            "Allows Ice Arrows to melt Red Ice. May require a room reload if toggled during gameplay."));
    AddWidget(path, "Sunlight Arrows", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SunlightArrows"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled =
                OTRGlobals::Instance->gRandoContext->GetOption(RSK_SUNLIGHT_ARROWS).Is(RO_GENERIC_ON);
            info.options->disabledTooltip = "This setting is forcefully enabled because a randomized savefile with "
                                            "\"Sunlight Arrows\" enabled is currently loaded.";
        })
        .Options(CheckboxOptions().Tooltip(
            "Allows Light Arrows to activate Sun Switches. May require a room reload if toggled during gameplay."));
    AddWidget(path, "Bow and Child/Slingshot as Adult", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("BowSlingshotAmmoFix"))
        .Options(CheckboxOptions().Tooltip("Allows Child to use a Bow with Arrows.\n"
                                           "Allows Adult to use a Slingshot with Seeds.\n\n"
                                           "Requires glitches or the 'Timeless Equipment' cheat to equip."));
    AddWidget(path, "Aiming Reticle for the Bow/Slingshot", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("BowReticle"))
        .Options(CheckboxOptions().Tooltip("Aiming with a Bow or Slingshot will display a reticle as with the Hookshot "
                                           "when the projectile is ready to fire."));

    path.column = SECTION_COLUMN_3;
    AddWidget(path, "Hookshot", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Targetable Hookshot Reticle", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("HookshotableReticle"))
        .Options(CheckboxOptions().Tooltip(
            "Makes the Hookshot Reticle use a different color when aiming at Hookshotable Collision."));

    AddWidget(path, "Boomerang", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Instant Boomerang Recall", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FastBoomerang"))
        .Options(CheckboxOptions().Tooltip("Instantly return the boomerang to Link by pressing its item button while "
                                           "it's in the air."));
    AddWidget(path, "Aim Boomerang in First-Person Mode", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("BoomerangFirstPerson"))
        .Callback([](WidgetInfo& info) {
            if (!CVarGetInteger(CVAR_ENHANCEMENT("BoomerangFirstPerson"), 0)) {
                CVarSetInteger(CVAR_ENHANCEMENT("BoomerangReticle"), 0);
            }
        })
        .Options(CheckboxOptions().Tooltip(
            "Change aiming for the Boomerang from Third-Person to First-Person to see past Link's head."));
    AddWidget(path, "Aiming Reticle for Boomerang", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("BoomerangFirstPerson"))
        .PreFunc(
            [](WidgetInfo& info) { info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("BoomerangFirstPerson"), 0) != 0; })
        .Options(CheckboxOptions().Tooltip("Aiming with the Boomerang will display a reticle as with the Hookshot."));

    AddWidget(path, "Magic Spells", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Better Farore's Wind", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("BetterFarore"))
        .Options(CheckboxOptions().Tooltip(
            "Helps FW persist between ages, gives Child and Adult separate FW points, and can "
            "be used in more places."));
    AddWidget(path, "Faster Farore's Wind", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FastFarores"))
        .Options(CheckboxOptions().Tooltip("Greatly decreases cast time of Farore's Wind magic spell."));

    // Fixes
    path.sidebarName = "Fixes";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;
    AddWidget(path, "Gameplay Fixes", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fix the Gravedigging Tour Glitch", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("GravediggingTourFix"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = IS_RANDO && GameInteractor::IsSaveLoaded(true);
            info.options->disabledTooltip = "This setting is always enabled in randomized save files.";
        })
        .Options(CheckboxOptions().Tooltip(
            "Fixes a bug where the Gravedigging Tour Heart Piece disappears if the area reloads."));
    AddWidget(path, "Fix Raised Floor Switches", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FixFloorSwitches"))
        .Options(CheckboxOptions().Tooltip(
            "Fixes the two raised floor switches, the one in Forest Temple Basement and the one at the top of Fire "
            "Temple. This will lower them, making activating them easier."));
    AddWidget(path, "Fix Zora Hint Dialogue", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FixZoraHintDialogue"))
        .Options(CheckboxOptions().Tooltip(
            "Fixes one Zora's dialogue giving a hint about bringing Ruto's Letter to King Zora to properly occur "
            "before moving King Zora rather than after."));
    AddWidget(path, "Fix Falling from Vine Edges", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FixVineFall"))
        .Options(
            CheckboxOptions().Tooltip("Prevents immediately falling off climbable surfaces if climbing on the edges."));
    AddWidget(path, "Fix Bush Item Drops", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("BushDropFix"))
        .Options(CheckboxOptions().Tooltip(
            "Fixes the bushes to drop items correctly rather than spawning undefined items."));
    AddWidget(path, "Fix Enemies not Spawning Near Water", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("EnemySpawnsOverWaterboxes"))
        .Options(CheckboxOptions().Tooltip(
            "Causes respanwing enemies, like Stalchildren, to appear on land near bodies of water. "
            "Fixes an incorrect calculation that acted like water underneath ground was above it."));
    AddWidget(path, "Fix Poacher's Saw Softlock", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FixSawSoftlock"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = CVarGetInteger(CVAR_ENHANCEMENT("SkipText"), 0) == 1;
            info.options->disabledTooltip = "This option is forced on when Skip Text is enabled.";
        })
        .Options(CheckboxOptions().Tooltip(
            "Prevents the Poacher's Saw softlock from mashing through the text, or with Skip Text enabled."));
    AddWidget(path, "Fix Anubis Fireballs", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("AnubisFix"))
        .Options(CheckboxOptions().Tooltip(
            "Make Anubis Fireballs do Fire damage when reflected back at them with the Mirror Shield."));

    AddWidget(path, "Item-related Fixes", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fix Deku Nut Upgrade", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DekuNutUpgradeFix"))
        .Options(CheckboxOptions().Tooltip("Prevents the Forest Stage Deku Nut upgrade from becoming unobtainable "
                                           "after receiving the Poacher's Saw."));
    AddWidget(path, "Fix Megaton Hammer Crouch Stab", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("CrouchStabHammerFix"))
        .Callback([](WidgetInfo& info) {
            if (!CVarGetInteger(CVAR_ENHANCEMENT("CrouchStabHammerFix"), 0)) {
                CVarClear(CVAR_ENHANCEMENT("CrouchStabFix"));
            }
        })
        .Options(CheckboxOptions().Tooltip(
            "Make the Megaton Hammer's crouch stab able to destroy rocks without first swinging it normally."));
    AddWidget(path, "Remove Power Crouch Stab", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("CrouchStabFix"))
        .PreFunc(
            [](WidgetInfo& info) { info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("CrouchStabHammerFix"), 0) == 0; })
        .Options(CheckboxOptions().Tooltip("Make crouch stabbing always do the same damage as a regular slash."));
    AddWidget(path, "Fix Broken Giant's Knife Bug", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FixGrokenGiantsKnife"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = IS_RANDO && GameInteractor::IsSaveLoaded(true);
            info.options->disabledTooltip = "This setting is forcefully enabled when you are playing a Randomizer.";
        })
        .Callback([](WidgetInfo& info) {
            bool hasGiantsKnife = CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BIGGORON);
            bool hasBrokenKnife = CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE);
            bool knifeIsBroken = gSaveContext.swordHealth == 0.0f;

            if (hasGiantsKnife && (hasBrokenKnife != knifeIsBroken)) {
                func_800849EC(gPlayState);
            }
        })
        .Options(
            CheckboxOptions().Tooltip("Fixes the Broken Giant's Knife flag not being reset when Medigoron fixes it."));

    AddWidget(path, "Camera Fixes", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fix Camera Drift", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FixCameraDrift"))
        .Options(CheckboxOptions().Tooltip(
            "Fixes camera slightly drifting to the left when standing still due to a math error."));
    AddWidget(path, "Fix Camera Swing", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FixCameraSwing"))
        .Options(CheckboxOptions().Tooltip(
            "Fixes camera getting stuck on collision when standing still. Also fixes slight shift "
            "back in camera when Link stops moving."));
    AddWidget(path, "Fix Hanging Ledge Swing Rate", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FixHangingLedgeSwingRate"))
        .Options(CheckboxOptions().Tooltip(
            "Fixes camera swing rate when the player falls off a ledge and the camera swings around."));
    
    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Graphical Fixes", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fix L&R Pause Menu", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FixMenuLR"))
        .Options(CheckboxOptions().Tooltip("Makes the L and R buttons in the pause menu the same color"));
    AddWidget(path, "Fix Dungeon Entrances", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FixDungeonMinimapIcon"))
        .Options(CheckboxOptions().Tooltip(
            "Removes the Dungeon Entrance icon on the top-left corner of the screen when no dungeon is present on the "
            "current map."));
    AddWidget(path, "Fix Two-Handled Idle Animations", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TwoHandedIdle"))
        .Options(CheckboxOptions().Tooltip(
            "Re-Enables the two-handed idle animation, a seemingly finished animation that was disabled on accident "
            "in the original game."));
    AddWidget(path, "Fix Navi Text HUD Position", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("NaviTextFix"))
        .Options(CheckboxOptions().Tooltip("Correctly centers the Navi text prompt on the HUD's C-Up button."));
    AddWidget(path, "Fix Gerudo Warrior's Clothing Colors", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("GerudoWarriorClothingFix"))
        .Options(CheckboxOptions().Tooltip(
            "Prevent the Gerudo Warrior's clothes changing color when changing Link's tunic or "
            "using Bombs in front of her."));
    AddWidget(path, "Fix Out of Bounds Textures", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FixTexturesOOB"))
        .Callback([](WidgetInfo& info) { ApplyAuthenticGfxPatches(); })
        .Options(CheckboxOptions().Tooltip(
            "Fixes authentic out of bounds texture reads, instead loading textures with the correct size."));
    AddWidget(path, "Fix Link's Eyes Open while Sleeping", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FixEyesOpenWhileSleeping"))
        .Options(CheckboxOptions().Tooltip(
            "Fixes Link's eyes being open in the openeing cutscene when he is supposed to be sleeping."));
    AddWidget(path, "Fix Hand Holding Hammer", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FixHammerHand"))
        .Callback([](WidgetInfo& info) { UpdatePatchHand(); })
        .Options(CheckboxOptions().Tooltip(
            "Fixes Adult Link having a backwards Left hand when holding the Megaton Hammer."));
    AddWidget(path, "Fix Vanishing Paths", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("SceneSpecificDirtPathFix"))
        .Callback([](WidgetInfo& info) {
            if (gPlayState != NULL) {
                UpdateDirtPathFixState(gPlayState->sceneNum);
            }
        })
        .Options(
            ComboboxOptions()
                .ComboMap(zFightingOptions)
                .DefaultIndex(ZFIGHT_FIX_DISABLED)
                .Tooltip("Disabled: Paths vanish more the higher the resolution (Z-fighting is based on resolution)\n"
                         "Consistent: Certain paths vanish the same way in all resolutions\n"
                         "No Vanish: Paths do not vanish, Link seems to sink in to some paths\n"
                         "This might affect other decal effects\n"));

    AddWidget(path, "Audio Fixes", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fix Missing Jingle after 5 Silver Rupees", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SilverRupeeJingleExtend"))
        .Options(CheckboxOptions().Tooltip(
            "Adds 5 higher pitches for the Silver Rupee Jingle for the rooms with more than 5 Silver Rupees. "
            "Only relevant for playthroughs involving Master Quest Dungeons."));

    AddWidget(path, "Desync Fixes", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fix Darunia Dancing too Fast", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FixDaruniaDanceSpeed"))
        .Options(CheckboxOptions().Tooltip(
            "Fixes Darunia's dancing speed so he dances to the beat of Saria's Song, like in the Original Game."));
    AddWidget(path, "Fix Credits Timing", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("CreditsFix"))
        .Options(CheckboxOptions().Tooltip(
            "Extend certain credits scenes so the music lines up properly with the visuals."));

    path.column = SECTION_COLUMN_3;
    AddWidget(path, "Graphical Restorations", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Red Ganon Blood", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("RedGanonBlood"))
        .Options(
            CheckboxOptions().Tooltip("Restore the original red blood from NTSC 1.0/1.1. Disable for Green blood."));
    AddWidget(path, "Restore Old Gold Skulltula Cutscene", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("GSCutscene"))
        .Options(CheckboxOptions().Tooltip(
            "Restore pre-release behavior where defeating a Gold Skulltula will play a cutscene showing it die."));
    AddWidget(path, "Pulsate Boss Icon", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("Pulsate Boss Icon"))
        .Options(CheckboxOptions().Tooltip(
            "Restores an unfinished feature to pulsate the boss room icon when you are in the boss room."));

    AddWidget(path, "Glitch Restorations", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fish while Hovering", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("HoverFishing"))
        .Options(CheckboxOptions().Tooltip(
            "Restore a bug from NSTC 1.0 that allows casting the Fishing Rod while using the Hover Boots."));
    AddWidget(path, "N64 Weird Frames", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("N64WeirdFrames"))
        .Options(CheckboxOptions().Tooltip("Restores N64 Weird Frames allwing weirdshots to behave the same as N64."));
    AddWidget(path, "Bombchus Out of Bounds", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("BombchusOOB"))
        .Options(CheckboxOptions().Tooltip("Allows Bombchus to explode out of bounds. Similar to Gamecube and Wii VC"));
    AddWidget(path, "Quick Putaway", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("QuickPutaway"))
        .Options(CheckboxOptions().Tooltip(
            "Restore a bug from NTSC 1.0 that allows putting away an item without an animation and performing "
            "Putaway Ocarina Items."));
    AddWidget(path, "Quick Bongo Kill", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("QuickBongoKill"))
        .Options(CheckboxOptions().Tooltip(
            "Restore a bug from NTSC 1.0 that allows bypassing Bongo Bongo's intro cutscene to quickly kill him."));
    AddWidget(path, "Original RBA Values", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("RestoreRBAValues"))
        .Options(CheckboxOptions().Tooltip("Restores the original outcomes when performing Reverse Bottle Adventure."));
    AddWidget(path, "Early Eyeball Frog", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("EarlyEyeballFrog"))
        .Options(CheckboxOptions().Tooltip(
            "Restores a bug from NTSC 1.0/1.1 that allows you to obtain the eyeball frog from King Zora "
            "instead of the Zora Tunic by Holding Shield."));

    AddWidget(path, "Misc Restorations", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Fix L&Z Page Switch in Pause Menu", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("NGCKaleidoSwitcher"))
        .Options(CheckboxOptions().Tooltip(
            "Makes L and R switch pages like on the Gamecube. Z opens the Debug Menu instead."));

    // Difficulty Options
    path.sidebarName = "Difficulty";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;
    
    AddWidget(path, "Health", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Permanent Heart Loss", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("PermanentHeartLoss"))
        .Callback([](WidgetInfo& info) {
            UpdatePermanentHeartLossState();
        })
        .Options(CheckboxOptions().Tooltip(
            "When you lose 4 quarters of a heart you will permanently lose that Heart Container.\n\n"
            "Disabling this after the fact will restore your Heart Containers."
        ));
    AddWidget(path, "Damage Multiplier", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("DamageMult"))
        .Options(ComboboxOptions()
            .ComboMap(allPowers)
            .DefaultIndex(0)
            .Tooltip(
                "Modifies all sources of damage not affected by other sliders\n"
                    "2x: Can survive all common attacks from the start of the game\n"
                    "4x: Dies in 1 hit to any substantial attack from the start of the game\n"
                    "8x: Can only survive trivial damage from the start of the game\n"
                    "16x: Can survive all common attacks with max health without double defense\n"
                    "32x: Can survive all common attacks with max health and double defense\n"
                    "64x: Can survive trivial damage with max health without double defense\n"
                    "128x: Can survive trivial damage with max health and double defense\n"
                    "256x: Cannot survive damage"
            )
        );
    AddWidget(path, "Fall Damage Multiplier", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("FallDamageMult"))
        .Options(ComboboxOptions()
            .ComboMap(subPowers)
            .Tooltip(
                "Modifies all fall damage\n"
                    "2x: Can survive all fall damage from the start of the game\n"
                    "4x: Can only survive short fall damage from the start of the game\n"
                    "8x: Cannot survive any fall damage from the start of the game\n"
                    "16x: Can survive all fall damage with max health without double defense\n"
                    "32x: Can survive all fall damage with max health and double defense\n"
                    "64x: Can survive short fall damage with double defense\n"
                    "128x: Cannot survive fall damage"
            )
        );
    AddWidget(path, "Void Damage Multiplier", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("VoidDamageMult"))
        .Options(ComboboxOptions()
            .ComboMap(subSubPowers)
            .DefaultIndex(0)
            .Tooltip(
                "Modifies damage taken after falling into a void\n"
                "2x: Can survive void damage from the start of the game\n"
                "4x: Cannot survive void damage from the start of the game\n"
                "8x: Can survive void damage twice with max health without double defense\n"
                "16x: Can survive void damage with max health without double defense\n"
                "32x: Can survive void damage with max health and double defense\n"
                "64x: Cannot survive void damage"
            )
        );
    AddWidget(path, "Bonk Damage Multiplier", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("BonkDamageMult"))
        .Options(ComboboxOptions()
            .ComboMap(bonkDamageValues)
            .DefaultIndex(BONK_DAMAGE_NONE)
            .Tooltip("Modifies Damage taken after Bonking.")
        );
    AddWidget(path, "Spawn with Full Health", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FullHealthSpawn"))
        .Options(CheckboxOptions().Tooltip(
            "Respawn with Full Health instead of 3 hearts."
        ));
    AddWidget(path, "No Heart Drops", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("NoHeartDrops"))
        .Options(CheckboxOptions().Tooltip(
            "Disables Heart Drops, but not Heart Placements, like from a Deku Scrub running off.\n"
            "This simulates Hero Mode from other games in the series."
        ));
    
    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Drops", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "No Random Drops", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("NoRandomDrops"))
        .Options(CheckboxOptions().Tooltip(
            "Disables Random Drops, except from the Goron Pot, Dampe, and Bosses."
        ));
    AddWidget(path, "Enable Bombchu Drops", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("EnableBombchuDrops"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled =
                IS_RANDO && GameInteractor::IsSaveLoaded(true) &&
                OTRGlobals::Instance->gRandoContext->GetOption(RSK_ENABLE_BOMBCHU_DROPS).Is(RO_GENERIC_ON);
            info.options->disabledTooltip = "This setting is forcefully enabled because a randomized savefile with "
            "\"Enable Bombchu Drops\" is loaded.";
        })
        .Options(CheckboxOptions().Tooltip(
            "Bombchus will sometimes drop in place of Bombs."
        ));
    AddWidget(path, "Trees Drop Sticks", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TreesDropSticks"))
        .Options(CheckboxOptions().Tooltip(
            "Bonking into Trees will have a chance to drop up to 3 Sticks. Must have obtained sticks previously."
        ));

    AddWidget(path, "Miscellaneous", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Delete File on Death", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DeleteFileOnDeath"))
        .Options(CheckboxOptions().Tooltip(
            "Dying will delete your file.\n\n"
            ICON_FA_EXCLAMATION_TRIANGLE " WARNING " ICON_FA_EXCLAMATION_TRIANGLE
            "\nTHIS IS NOT REVERSABLE\nUSE AT YOUR OWN RISK!"
        ));
    AddWidget(path, "Always Win Goron Pot", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("GoronPot"))
        .Options(CheckboxOptions().Tooltip(
            "Always get the Heart Piece/Purple Rupee from the Spinning Goron Pot."
        ));
    AddWidget(path, "Always Win Dampe Digging Game", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DampeWin"))
        .Options(CheckboxOptions().Tooltip(
            "Always win the Heart Piece/Purple Rupee on the first dig in Dampe's Grave Digging game. "
            "In a Randomizer file, this is always enabled."
        ));
    AddWidget(path, "All Dogs are Richard", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("AllDogsRichard"))
        .Options(CheckboxOptions().Tooltip(
            "All dogs can be traded in and will count as Richard."
        ));
    AddWidget(path, "Cuccos Stay Put Multiplier: %dx", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("CuccoStayDurationMult"))
        .Options(IntSliderOptions()
            .Min(1)
            .Max(5)
            .DefaultValue(1)
            .Format("%dx")
            .Tooltip(
                "Cuccos will stay in place longer after putting them down, by a multiple of the value of the slider."
            )
        );

    path.column = SECTION_COLUMN_3;
    AddWidget(path, "Enemies", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Hyper Bosses", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("HyperBosses"))
        .Callback([](WidgetInfo& info) { UpdateHyperBossesState(); })
        .Options(CheckboxOptions().Tooltip("All Major Bosses move and act twice as fast."));
    AddWidget(path, "Hyper Enemies", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("HyperEnemies"))
        .Callback([](WidgetInfo& info) { UpdateHyperEnemiesState(); })
        .Options(CheckboxOptions().Tooltip("All Regular Enemies and Mini-Bosses move and act twice as fast."));
    AddWidget(path, "Enable Visual Guard Vision", WIDGET_CVAR_CHECKBOX).CVar(CVAR_ENHANCEMENT("GuardVision"));
    AddWidget(path, "Leever Spawn Rate: %d seconds", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("LeeverSpawnRate"))
        .Options(IntSliderOptions()
                     .Min(0)
                     .Max(10)
                     .DefaultValue(0)
                     .Format("%d seconds")
                     .Tooltip("The time between groups of Leevers spawning."));
    
    // Minigames
    path.sidebarName = "Minigames";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "Shooting Gallery", WIDGET_SEPARATOR_TEXT);
    auto shootingGalleryDisabledFunc = [](WidgetInfo& info) {
        info.options->disabled = !CVarGetInteger(CVAR_ENHANCEMENT("CustomizeShootingGallery"), 0);
        info.options->disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off.";
    };
    AddWidget(path, "Customize Behavior##Shooting", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("CustomizeShootingGallery"))
        .Options(CheckboxOptions().Tooltip("Turn on/off changes to the shooting gallery behavior"));
    AddWidget(path, "Instant Win", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("InstantShootingGalleryWin"))
        .PreFunc(shootingGalleryDisabledFunc)
        .Options(CheckboxOptions().Tooltip("Skips the Shooting Gallery minigame"));
    AddWidget(path, "No Rupee Randomization", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ConstantAdultGallery"))
        .PreFunc(shootingGalleryDisabledFunc)
        .Options(CheckboxOptions().Tooltip(
            "Forces the rupee order to not be randomized as adult, making it the same as child."));
    AddWidget(path, "Child Starting Ammunition: %d seeds", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("ShootingGalleryAmmoChild"))
        .PreFunc(shootingGalleryDisabledFunc)
        .Options(IntSliderOptions()
                     .Min(10)
                     .Max(30)
                     .DefaultValue(15)
                     .Format("%d seeds")
                     .Tooltip("The ammunition at the start of the Shooting Gallery minigame as Child."));
    AddWidget(path, "Adult Starting Ammunition: %d arrows", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("ShootingGalleryAmmoAdult"))
        .PreFunc(shootingGalleryDisabledFunc)
        .Options(IntSliderOptions()
                     .Min(10)
                     .Max(30)
                     .DefaultValue(15)
                     .Format("%d arrows")
                     .Tooltip("The ammunition at the start of the Shooting Gallery minigame as Adult."));

    AddWidget(path, "Bombchu Bowling", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Customize Behavior##Bowling", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("CustomizeBombchuBowling"))
        .Options(CheckboxOptions().Tooltip("Turn on/off changes to the Bombchu Bowling behavior."));
    auto bombchuBowlingDisabledFunc = [](WidgetInfo& info) {
        info.options->disabled = CVarGetInteger(CVAR_ENHANCEMENT("CustomizeBombchuBowling"), 0) == 0;
        info.options->disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off";
    };
    AddWidget(path, "Remove Small Cucco", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("BombchuBowlingNoSmallCucco"))
        .PreFunc(bombchuBowlingDisabledFunc)
        .Options(CheckboxOptions().Tooltip("Prevents the small Cucco from appearing in the Bombchu Bowling minigame."));
    AddWidget(path, "Remove Big Cucco", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("BombchuBowlingNoBigCucco"))
        .PreFunc(bombchuBowlingDisabledFunc)
        .Options(CheckboxOptions().Tooltip("Prevents the big Cucco from appearing in the Bombchu Bowling minigame."));
    AddWidget(path, "Bombchu Count: %d bombchus", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("BombchuBowlingAmmo"))
        .PreFunc(bombchuBowlingDisabledFunc)
        .Options(IntSliderOptions()
                     .Min(3)
                     .Max(20)
                     .DefaultValue(10)
                     .Format("%d bombchus")
                     .Tooltip("The number of Bombchus available at the start of the Bombchu Bowling minigame."));
    AddWidget(path, "Frogs' Ocarina Game", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Customize Behavior##Frogs", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("CustomizeFrogsOcarinaGame"))
        .Options(CheckboxOptions().Tooltip("Turn on/off changes to the Frogs' Ocarina Game behavior."));
    auto frogsOcarinaGameDisabledFunc = [](WidgetInfo& info) {
        info.options->disabled = CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFrogsOcarinaGame"), 0) == 0;
        info.options->disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off.";
    };
    AddWidget(path, "Instant Win##Frogs", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("InstantFrogsGameWin"))
        .PreFunc(frogsOcarinaGameDisabledFunc)
        .Options(CheckboxOptions().Tooltip("Skips the Frogs' Ocarina Game."));
    AddWidget(path, "Unlimited Playback Time##Frogs", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FrogsUnlimitedFailTime"))
        .PreFunc(frogsOcarinaGameDisabledFunc)
        .Options(CheckboxOptions().Tooltip("Removes the timer to play back the song."));
    AddWidget(path, "Modify Note Timer: %dx", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("FrogsModifyFailTime"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = !CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFrogsOcarinaGame"), 0) ||
                                     CVarGetInteger(CVAR_ENHANCEMENT("FrogsUnlimitedFailTime"), 0);
            info.options->disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off or "
                                            "\"Unlimited Playback Time\" is on";
        })
        .Options(IntSliderOptions().Min(1).Max(5).DefaultValue(1).Format("%dx").Tooltip(
            "Adjusts the time allowed for playback before failing."));

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Lost Woods Ocarina Game", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Customize Behavior##LostWoods", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("CustomizeOcarinaGame"))
        .Options(CheckboxOptions().Tooltip("Turn on/off changes to the Lost Woods Ocarina Game behavior."));
    auto ocarinaMemoryGameDisabledFunc = [](WidgetInfo& info) {
        info.options->disabled = CVarGetInteger(CVAR_ENHANCEMENT("CustomizeOcarinaGame"), 0) == 0;
        info.options->disabledTooltip = "This options is disabled because \"Customize Behavior\" is turned off.";
    };
    AddWidget(path, "Instant Win##LostWoods", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("InstantOcarinaGameWin"))
        .PreFunc(ocarinaMemoryGameDisabledFunc)
        .Options(CheckboxOptions().Tooltip("Skips the Lost Woods Ocarina Memory Game."));
    AddWidget(path, "Note Play Speed: %dx", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("OcarinaGame.NoteSpeed"))
        .PreFunc(ocarinaMemoryGameDisabledFunc)
        .Options(IntSliderOptions().Min(1).Max(5).DefaultValue(1).Format("%dx").Tooltip(
            "Adjust the speed that the Skull Kids play the notes."));
    AddWidget(path, "Unlimited Playback Time##LostWoods", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("OcarinaUnlimitedFailTime"))
        .PreFunc(ocarinaMemoryGameDisabledFunc)
        .Options(CheckboxOptions().Tooltip("Removes the timer to play back the song."));
    AddWidget(path, "Number of Starting Notes: %d notes", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("OcarinaGame.StartingNotes"))
        .PreFunc(ocarinaMemoryGameDisabledFunc)
        .Options(IntSliderOptions()
                     .Min(1)
                     .Max(8)
                     .DefaultValue(3)
                     .Format("%d notes")
                     .Tooltip("Adjust the number of notes the Skull Kids play to start the first round."));
    int roundMin = CVarGetInteger(CVAR_ENHANCEMENT("OcarinaGame.StartingNotes"), 3);
    AddWidget(path, "Round One Notes: %d notes", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("OcarinaGame.RoundOneNotes"))
        .PreFunc(ocarinaMemoryGameDisabledFunc)
        .Options(IntSliderOptions()
                     .Min(roundMin)
                     .Max(8)
                     .DefaultValue(5)
                     .Format("%d notes")
                     .Tooltip("Adjust the number of notes you need to play to end the first round."));
    AddWidget(path, "Round Two Notes: %d notes", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("OcarinaGame.RoundTwoNotes"))
        .PreFunc(ocarinaMemoryGameDisabledFunc)
        .Options(IntSliderOptions()
                     .Min(roundMin)
                     .Max(8)
                     .DefaultValue(6)
                     .Format("%d notes")
                     .Tooltip("Adjust the number of notes you need to play to end the second round."));
    AddWidget(path, "Round Three Notes: %d notes", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("OcarinaGame.RoundThreeNotes"))
        .PreFunc(ocarinaMemoryGameDisabledFunc)
        .Options(IntSliderOptions()
                     .Min(roundMin)
                     .Max(8)
                     .DefaultValue(8)
                     .Format("%d notes")
                     .Tooltip("Adjust the number of notes you need to play to end the third round."));

    path.column = SECTION_COLUMN_3;
    AddWidget(path, "Fishing", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Customize Behavior##Fishing", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("CustomizeFishing"))
        .Options(CheckboxOptions().Tooltip("Turn on/off changes to the Fishing behavior"));
    auto fishingDisabledFunc = [](WidgetInfo& info) {
        info.options->disabled = CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFishing"), 0) == 0;
        info.options->disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off.";
    };
    AddWidget(path, "Instant Fishing", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("InstantFishing"))
        .PreFunc(fishingDisabledFunc)
        .Options(CheckboxOptions().Tooltip("All fish will be caught instantly."));
    AddWidget(path, "Guarantee Bite", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("GuaranteeFishingBite"))
        .PreFunc(fishingDisabledFunc)
        .Options(CheckboxOptions().Tooltip("When a line is stable, guarantee bite. Otherwise use Default logic."));
    AddWidget(path, "Fish never Escape", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("FishNeverEscape"))
        .PreFunc(fishingDisabledFunc)
        .Options(CheckboxOptions().Tooltip("Once a hook as been set, Fish will never let go while being reeled in."));
    AddWidget(path, "Loaches always Appear", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("LoachesAlwaysAppear"))
        .PreFunc(fishingDisabledFunc)
        .Options(
            CheckboxOptions().Tooltip("Loaches will always appear in the fishing pond instead of every four visits."));
    AddWidget(path, "Skip Keep Confirmation", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SkipKeepConfirmation"))
        .PreFunc(fishingDisabledFunc)
        .Options(
            CheckboxOptions().Tooltip("The Pond Owner will not ask to confirm if you want to keep a smaller Fish."));
    AddWidget(path, "Child Minimum Weight: %d lbs.", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("MinimumFishWeightChild"))
        .PreFunc(fishingDisabledFunc)
        .Options(IntSliderOptions().Min(3).Max(10).DefaultValue(10).Format("%d lbs.").Tooltip(
            "The minimum weight for the unique Fishing Reward as a Child."));
    AddWidget(path, "Adult Minimum Weight: %d lbs.", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("MinimumFishWeightAdult"))
        .PreFunc(fishingDisabledFunc)
        .Options(IntSliderOptions().Min(6).Max(13).DefaultValue(13).Format("%d lbs.").Tooltip(
            "The minimum weight for the unique fishing reward as an Adult."));
    AddWidget(path, "All Fish are Hyrule Loaches", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("AllHyruleLoaches"))
        .PreFunc(fishingDisabledFunc)
        .Options(IntSliderOptions().Tooltip("Every fish in the Fishing Pond will always be a Hyrule Loach.\n\n"
                                            "NOTE: This requires reloading the area."));

    // Extra Modes
    path.sidebarName = "Extra Modes";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "Mirrored World", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("MirroredWorldMode"))
        .Callback([](WidgetInfo& info) {
            if (gPlayState != NULL) {
                UpdateMirrorModeState(gPlayState->sceneNum);
            }
        })
        .Options(ComboboxOptions()
            .DefaultIndex(MIRRORED_WORLD_OFF)
            .ComboMap(mirroredWorldModes)
            .Tooltip(
                "Mirrors the world horizontally\n\n"
                " - Always: Always mirror the world\n"
                " - Random: Randomly decide to mirror the world on each scene change\n"
                " - Random (Seeded): Scenes are mirrored based on the current randomizer seed/file\n"
                " - Dungeons: Mirror the world in Dungeons\n"
                " - Dungeons (Vanilla): Mirror the world in vanilla Dungeons\n"
                " - Dungeons (MQ): Mirror the world in MQ Dungeons\n"
                " - Dungeons Random: Randomly decide to mirror the world in Dungeons\n"
                " - Dungeons Random (Seeded): Dungeons are mirrored based on the current randomizer seed/file\n"
            )
        );
    AddWidget(path, "Ivan the Fairy (Coop Mode)", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("IvanCoopModeEnabled"))
        .Options(CheckboxOptions().Tooltip(
            "Enables Ivan the Fairy upon the next map change. Player 2 can control Ivan and press the C-Buttons to "
            "use items and mess with Player 1!"
        ));
    AddWidget(path, "Dogs Follow You Everywhere", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("DogFollowsEverywhere"))
        .Options(CheckboxOptions().Tooltip("Allows dogs to follow you anywhere you go, even if you leave the Market."));
    AddWidget(path, "Rupee Dash Mode", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("RupeeDash"))
        .Options(CheckboxOptions().Tooltip(
            "Rupees reduce over time, Link suffers damage when the count hits 0."
        ));
    AddWidget(path, "Rupee Dash Interval %d seconds", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_ENHANCEMENT("RupeeDashInterval"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("RupeeDash"), 0) == 0;
        })
        .Options(IntSliderOptions()
            .Min(1)
            .Max(10)
            .DefaultValue(5)
            .Format("%d seconds")
            .Tooltip(
                "Interval between Rupee reduction in Rupee Dash Mode."
            )
        );
    AddWidget(path, "Shadow Tag Mode", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ShadowTag"))
        .Options(CheckboxOptions().Tooltip(
            "A Wallmaster follows Link everywhere, don't get caught!"
        ));
    AddWidget(path, "Hurt Container Mode", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("HurtContainer"))
        .Callback([](WidgetInfo& info) {
            UpdateHurtContainerModeState(CVarGetInteger(CVAR_ENHANCEMENT("HurtContainer"), 0));
        })
        .Options(CheckboxOptions().Tooltip(
            "Changes Heart Piece and Heart Container functionality.\n\n"
            " - Each Heart Container or full Heart Piece reduces Link's Hearts by 1.\n"
            " - Can be enabled retroactively after a File has already started."
        ));
    AddWidget(path, "Additional Traps", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ExtraTraps.Enabled"))
        .Options(CheckboxOptions().Tooltip(
            "Enables additional Trap variants."
        ));
    AddWidget(path, "Trap Options", WIDGET_SEPARATOR_TEXT)
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) == 0;
        });
    AddWidget(path, "Tier 1 Traps:", WIDGET_TEXT)
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) == 0;
        });
    AddWidget(path, "Freeze Traps", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ExtraTraps.Ice"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) == 0;
        });
    AddWidget(path, "Burn Traps", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ExtraTraps.Burn"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) == 0;
        });
    AddWidget(path, "Shock Traps", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ExtraTraps.Shock"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) == 0;
        });
    AddWidget(path, "Tier 2 Traps:", WIDGET_TEXT)
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) == 0;
        });
    AddWidget(path, "Knockback Traps", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ExtraTraps.Knockback"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) == 0;
        });
    AddWidget(path, "Speed Traps", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ExtraTraps.Speed"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) == 0;
        });
    AddWidget(path, "Bomb Traps", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ExtraTraps.Bomb"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) == 0;
        });
    AddWidget(path, "Tier 3 Traps:", WIDGET_TEXT)
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) == 0;
        });
    AddWidget(path, "Void Traps", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ExtraTraps.Void"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) == 0;
        });
    AddWidget(path, "Ammo Traps", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ExtraTraps.Ammo"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) == 0;
        });
    AddWidget(path, "Death Traps", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ExtraTraps.Kill"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) == 0;
        });
    AddWidget(path, "Teleport Traps", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("ExtraTraps.Teleport"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0) == 0;
        });

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Enemy Randomizer", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("RandomizedEnemies"))
        .Callback([](WidgetInfo& info) {
            GetSelectedEnemies();
        })
        .Options(ComboboxOptions()
            .DefaultIndex(ENEMY_RANDOMIZER_OFF)
            .ComboMap(enemyRandomizerModes)
            .Tooltip(
                "Replaces fixed enemies throughout the game with a random enemy. Bosses, mini-bosses and a few specific regular enemies are excluded.\n"
                "Enemies that need more than Deku Nuts + either Deku Sticks or a sword to kill are excluded from spawning in \"clear enemy\" rooms.\n\n"
                "- Random: Enemies are randomized every time you load a room\n"
                "- Random (Seeded): Enemies are randomized based on the current randomizer seed/file\n"
            )
        );
    AddWidget(path, "Randomized Enemy Sizes", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("RandomizedEnemySizes"))
        .Options(CheckboxOptions().Tooltip("Enemies and Bosses spawn with random sizes."));
    AddWidget(path, "Scale Health with Size", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("EnemySizeScalesHealth"))
        .PreFunc(
            [](WidgetInfo& info) { info.options->disabled = !CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemySizes"), 0); })
        .Options(CheckboxOptions().Tooltip(
            "Scales normal enemies Health with their randomized size. *This will NOT affect bosses*"));
    AddWidget(path, "Enemy List", WIDGET_SEPARATOR_TEXT)
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = !CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemies"), 0);
        });
    AddWidget(path, "Select all Enemies", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("RandomizedEnemyList.All"))
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = !CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemies"), 0);
        });
    AddWidget(path, "Enemy List", WIDGET_SEPARATOR).PreFunc([](WidgetInfo& info) {
        info.isHidden = !CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemies"), 0);
    });
    for (int i = 0; i < RANDOMIZED_ENEMY_SPAWN_TABLE_SIZE; i++) {
        AddWidget(path, enemyNameList[i], WIDGET_CVAR_CHECKBOX)
            .CVar(enemyCVarList[i])
            .Options(CheckboxOptions().DefaultValue(true))
            .PreFunc([](WidgetInfo& info) {
                info.isHidden = !CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemies"), 0);
                info.options->disabled = CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemyList.All"), 0);
                info.options->disabledTooltip = "These options are disabled because \"Select All Enemies\" is enabled.";
            })
            .Callback([](WidgetInfo& info) {
                GetSelectedEnemies();
            });
    }

    // Cheats
    path.sidebarName = "Cheats";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "Infinite...", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Money", WIDGET_CVAR_CHECKBOX).CVar(CVAR_CHEAT("InfiniteMoney"));
    AddWidget(path, "Health", WIDGET_CVAR_CHECKBOX).CVar(CVAR_CHEAT("InfiniteHealth"));
    AddWidget(path, "Ammo", WIDGET_CVAR_CHECKBOX).CVar(CVAR_CHEAT("InfiniteAmmo"));
    AddWidget(path, "Magic", WIDGET_CVAR_CHECKBOX).CVar(CVAR_CHEAT("InfiniteMagic"));
    AddWidget(path, "Nayru's Love", WIDGET_CVAR_CHECKBOX).CVar(CVAR_CHEAT("InfiniteNayru"));
    AddWidget(path, "Epona Boost", WIDGET_CVAR_CHECKBOX).CVar(CVAR_CHEAT("InfiniteEponaBoost"));

    AddWidget(path, "Items", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Timeless Equipment", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("TimelessEquipment"))
        .Options(CheckboxOptions().Tooltip("Allows any item to be equipped, regardless of age.\n"
                                           "Also allows Child to use Adult strength upgrades."));
    AddWidget(path, "Unrestricted Items", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("NoRestrictItems"))
        .Options(CheckboxOptions().Tooltip("Allows you to use any item at any location"));
    AddWidget(path, "Super Tunic", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("SuperTunic"))
        .Options(CheckboxOptions().Tooltip("Makes every tunic have the effects of every other tunic."));
    AddWidget(path, "Fireproof Deku Shield", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("FireproofDekuShield"))
        .Options(CheckboxOptions().Tooltip("Prevents the Deku Shield from burning on contact with fire."));
    AddWidget(path, "Shield with Two-Handed Weapons", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("ShieldTwoHanded"))
        .Options(CheckboxOptions().Tooltip(
            "This allows you to put up for shield with any two-handed weapon in hand except for Deku Sticks."));
    AddWidget(path, "Deku Sticks:", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_CHEAT("DekuStick"))
        .Options(ComboboxOptions().ComboMap(dekuStickCheat).DefaultIndex(DEKU_STICK_NORMAL));
    AddWidget(path, "Bomb Timer Multiplier: %.2fx", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_CHEAT("BombTimerMultiplier"))
        .Options(FloatSliderOptions().Format("%.2f").Min(0.1f).Max(5.0f).DefaultValue(1.0f));
    AddWidget(path, "Hookshot Everything", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("HookshotEverything"))
        .Options(CheckboxOptions().Tooltip("Makes every surface in the game hookshot-able."));
    AddWidget(path, "Hookshot Reach Multiplier: %.2fx", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_CHEAT("HookshotReachMultiplier"))
        .Options(FloatSliderOptions().Format("%.2f").Min(1.0f).Max(5.0f));

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Misc", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "No Clip", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("NoClip"))
        .Options(CheckboxOptions().Tooltip("Allows you to walk through walls."));
    AddWidget(path, "Climb Everything", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("ClimbEverything"))
        .Options(CheckboxOptions().Tooltip("Makes every surface in the game climbable."));
    AddWidget(path, "Moon Jump on L", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("MoonJumpOnL"))
        .Options(CheckboxOptions().Tooltip("Holding L makes you float into the air."));
    AddWidget(path, "No ReDead/Gibdo Freeze", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("NoRedeadFreeze"))
        .Options(
            CheckboxOptions().Tooltip("Prevents ReDeads and Gibdos from being able to freeze you with their scream."));
    AddWidget(path, "Keese/Guay don't Target You", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("NoKeeseGuayTarget"))
        .Options(CheckboxOptions().Tooltip(
            "Keese and Guay no longer target you and simply ignore you as if you were wearing the "
            "Skull Mask."));

    AddWidget(path, "Glitch Aids", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Easy Frame Advancing with Pause", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("EasyFrameAdvance"))
        .Options(CheckboxOptions().Tooltip(
            "Continue holding START button when unpausing to only advance a single frame and then re-pause."));
    AddWidget(path, "Easy ISG", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("EasyISG"))
        .Options(CheckboxOptions().Tooltip("Passive Infinite Sword Glitch\n"
                                           "It makes your sword's swing effect and hitbox stay active indefinitely."));
    AddWidget(path, "Easy QPA", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("EasyQPA"))
        .Options(CheckboxOptions().Tooltip("Gives you the glitched damage value of the quick put away glitch."));
    AddWidget(path, "Clear Cutscene Pointer", WIDGET_BUTTON)
        .Callback([](WidgetInfo& info) { GameInteractor::RawAction::ClearCutscenePointer(); })
        .Options(ButtonOptions()
                     .Tooltip("Clears the cutscene pointer to a value safe for wrong warps.")
                     .Size(UIWidgets::Sizes::Inline));

    AddWidget(path, "Despawn Timers", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Drops Don't Despawn", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("DropsDontDie"))
        .Options(
            CheckboxOptions().Tooltip("Drops from enemies, grass, etc. don't disappear after a set amount of time."));
    AddWidget(path, "Fish don't Despawn", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("NoFishDespawn"))
        .Options(CheckboxOptions().Tooltip("Prevents fish from automatically despawning after a while when dropped."));
    AddWidget(path, "Bugs don't Despawn", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("NoBugsDespawn"))
        .Options(CheckboxOptions().Tooltip("Prevents bugs from automatically despawning after a while when dropped."));

    AddWidget(path, "Time of Day", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Freeze Time", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("FreezeTime"))
        .Options(CheckboxOptions().Tooltip("Freezes the time of day"));
    AddWidget(path, "Time Sync", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("TimeSync"))
        .Options(CheckboxOptions().Tooltip("Syncs the in-game time with the real world time."));

    AddWidget(path, "Instant Age Change", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Change Age", WIDGET_BUTTON)
        .Options(ButtonOptions().Tooltip("Switches Link's age and reloads the area.").Size(UIWidgets::Sizes::Inline))
        .Callback([](WidgetInfo& info) { SwitchAge(); });

    path.column = SECTION_COLUMN_3;
    AddWidget(path, "Save States", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, ICON_FA_EXCLAMATION_TRIANGLE " WARNING!!!! " ICON_FA_EXCLAMATION_TRIANGLE, WIDGET_TEXT)
        .Options(TextOptions().Color(Colors::Orange));
    AddWidget(path,
              "These are NOT like emulator states. They do not save your game progress "
              "and they WILL break across transitions and load zones (like doors). "
              "Support for related issues will not be provided.",
              WIDGET_TEXT);
    AddWidget(path, "I promise I have read the warning", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("SaveStatePromise"))
        .Callback([](WidgetInfo& info) {
            CVarSetInteger(CVAR_CHEAT("SaveStatesEnabled"), 0);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        });
    AddWidget(path, "I understand, enable save states", WIDGET_CVAR_CHECKBOX)
        .PreFunc([](WidgetInfo& info) { info.isHidden = CVarGetInteger(CVAR_CHEAT("SaveStatePromise"), 0) == 0; })
        .CVar(CVAR_CHEAT("SaveStatesEnabled"))
        .Options(CheckboxOptions().Tooltip("F5 to save, F6 to change slots, F7 to load"));

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
    path.column = SECTION_COLUMN_1;
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
    AddSidebarEntry("Enhancements", path.sidebarName, 3);
    AddWidget(path, "Toggle Timers Window", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("TimeDisplayEnabled"))
        .WindowName("Additional Timers")
        .Options(WindowButtonOptions().Tooltip("Enables the separate Additional Timers Window."));
    AddWidget(path, "Font Scale: %.2fx", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_ENHANCEMENT("TimeDisplay.FontScale"))
        .Callback([](WidgetInfo& info) {
            TimeDisplayInitSettings();
        })
        .Options(FloatSliderOptions()
            .Min(1.0f)
            .Max(5.0f)
            .DefaultValue(1.0f)
            .Format("%.2fx")
        );
    AddWidget(path, "Hide Background", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("TimeDisplay.ShowWindowBG"))
        .Callback([](WidgetInfo& info) {
            TimeDisplayInitSettings();
        });
    for (auto& timer : timeDisplayList) {
        AddWidget(path, timer.timeLabel, WIDGET_CVAR_CHECKBOX)
            .CVar(timer.timeEnable)
            .Callback([](WidgetInfo& info) {
                TimeDisplayUpdateDisplayOptions();
            });
    }
}

} // namespace SohGui
