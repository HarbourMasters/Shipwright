#include "SohMenu.h"
#include <soh/Enhancements/mods.h>
#include <soh/Enhancements/game-interactor/GameInteractor.h>

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
    AddSidebarEntry("Enhancements", path.sidebarName, 3);

    AddWidget(path, "filler", WIDGET_TEXT);

    // Cheats
    path.sidebarName = "Cheats";
    AddSidebarEntry("Enhancements", path.sidebarName, 3);

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
