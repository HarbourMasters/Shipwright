#ifndef SOHMENU_H
#define SOHMENU_H

#include <libultraship/libultraship.h>
#include "UIWidgets.hpp"
#include "Menu.h"
#include "graphic/Fast3D/gfx_rendering_api.h"
#include "soh/cvar_prefixes.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/Presets/Presets.h"

extern "C" {
#include "z64.h"
#include "functions.h"
#include "variables.h"
#include "macros.h"
extern PlayState* gPlayState;
}

#ifdef __cplusplus
extern "C" {
#endif
void enableBetaQuest();
void disableBetaQuest();
#ifdef __cplusplus
}
#endif

namespace SohGui {

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

static const std::unordered_map<int32_t, const char*> textureFilteringMap = {
    { FILTER_THREE_POINT, "Three-Point" },
    { FILTER_LINEAR, "Linear" },
    { FILTER_NONE, "None" },
};

static const std::unordered_map<int32_t, const char*> logLevels = {
    { DEBUG_LOG_TRACE, "Trace" }, { DEBUG_LOG_DEBUG, "Debug" }, { DEBUG_LOG_INFO, "Info" },
    { DEBUG_LOG_WARN, "Warn" },   { DEBUG_LOG_ERROR, "Error" }, { DEBUG_LOG_CRITICAL, "Critical" },
    { DEBUG_LOG_OFF, "Off" },
};

static const std::unordered_map<int32_t, const char*> notificationPosition = {
    { 0, "Top Left" }, { 1, "Top Right" }, { 2, "Bottom Left" }, { 3, "Bottom Right" }, { 4, "Hidden" },
};

static const std::unordered_map<int32_t, const char*> dekuStickCheat = {
    { DEKU_STICK_NORMAL, "Normal" },
    { DEKU_STICK_UNBREAKABLE, "Unbreakable" },
    { DEKU_STICK_UNBREAKABLE_AND_ALWAYS_ON_FIRE, "Unbreakable + Always on Fire" },
};

static const std::unordered_map<int32_t, const char*> skipForcedDialogOptions = {
    { FORCED_DIALOG_SKIP_NONE, "None" },
    { FORCED_DIALOG_SKIP_NAVI, "Navi" },
    { FORCED_DIALOG_SKIP_NPC, "NPCs" },
    { FORCED_DIALOG_SKIP_ALL, "All" },
};

static const std::unordered_map<int32_t, const char*> skipGetItemAnimationOptions = {
    { SGIA_DISABLED, "Disabled" },
    { SGIA_JUNK, "Junk Items" },
    { SGIA_ALL, "All Items" },
};

static const std::unordered_map<int32_t, const char*> chestStyleMatchesContentsOptions = {
    { CSMC_DISABLED, "Disabled" },
    { CSMC_BOTH, "Both" },
    { CSMC_TEXTURE, "Texture Only" },
    { CSMC_SIZE, "Size Only" },
};

static const std::unordered_map<int32_t, const char*> timeTravelOptions = {
    { TIME_TRAVEL_DISABLED, "Disabled" },
    { TIME_TRAVEL_OOT, "Ocarina of Time" },
    { TIME_TRAVEL_ANY, "Any Ocarina" },
};

static const std::unordered_map<int32_t, const char*> sleepingWaterfallOptions = {
    { WATERFALL_ALWAYS, "Always" },
    { WATERFALL_ONCE, "Once" },
    { WATERFALL_NEVER, "Never" },
};

static const std::unordered_map<int32_t, const char*> allPowers = {
    { DAMAGE_VANILLA, "Vanilla (1x)" },      { DAMAGE_DOUBLE, "Double (2x)" },
    { DAMAGE_QUADRUPLE, "Quadruple (4x)" },  { DAMAGE_OCTUPLE, "Octuple (8x)" },
    { DAMAGE_FOOLISH, "Foolish (16x)" },     { DAMAGE_RIDICULOUS, "Ridiculous (32x)" },
    { DAMAGE_MERCILESS, "Merciless (64x)" }, { DAMAGE_TORTURE, "Pure Torture (128x)" },
    { DAMAGE_OHKO, "OHKO (256x)" },
};

static const std::unordered_map<int32_t, const char*> subPowers = {
    { DAMAGE_VANILLA, "Vanilla (1x)" },      { DAMAGE_DOUBLE, "Double (2x)" },
    { DAMAGE_QUADRUPLE, "Quadruple (4x)" },  { DAMAGE_OCTUPLE, "Octuple (8x)" },
    { DAMAGE_FOOLISH, "Foolish (16x)" },     { DAMAGE_RIDICULOUS, "Ridiculous (32x)" },
    { DAMAGE_MERCILESS, "Merciless (64x)" }, { DAMAGE_TORTURE, "Pure Torture (128x)" },
};

static const std::unordered_map<int32_t, const char*> subSubPowers = {
    { DAMAGE_VANILLA, "Vanilla (1x)" },      { DAMAGE_DOUBLE, "Double (2x)" },
    { DAMAGE_QUADRUPLE, "Quadruple (4x)" },  { DAMAGE_OCTUPLE, "Octuple (8x)" },
    { DAMAGE_FOOLISH, "Foolish (16x)" },     { DAMAGE_RIDICULOUS, "Ridiculous (32x)" },
    { DAMAGE_MERCILESS, "Merciless (64x)" },
};

static const std::unordered_map<int32_t, const char*> bonkDamageValues = {
    { BONK_DAMAGE_NONE, "No Damage" },        { BONK_DAMAGE_QUARTER_HEART, "0.25 Hearts" },
    { BONK_DAMAGE_HALF_HEART, "0.5 Hearts" }, { BONK_DAMAGE_1_HEART, "1 Heart" },
    { BONK_DAMAGE_2_HEARTS, "2 Hearts" },     { BONK_DAMAGE_4_HEARTS, "4 Hearts" },
    { BONK_DAMAGE_8_HEARTS, "8 Hearts" },     { BONK_DAMAGE_OHKO, "OHKO" },
};

static const std::unordered_map<int32_t, const char*> cursorAnywhereValues = {
    { PAUSE_ANY_CURSOR_RANDO_ONLY, "Only in Rando" },
    { PAUSE_ANY_CURSOR_ALWAYS_ON, "Always" },
    { PAUSE_ANY_CURSOR_ALWAYS_OFF, "Never" },
};

static const std::unordered_map<int32_t, const char*> swordToggleModes = {
    { SWORD_TOGGLE_NONE, "None" },
    { SWORD_TOGGLE_CHILD, "Child Toggle" },
    { SWORD_TOGGLE_BOTH_AGES, "Both Ages" },
};

static const std::unordered_map<int32_t, const char*> zFightingOptions = {
    { ZFIGHT_FIX_DISABLED, "Disabled" },
    { ZFIGHT_FIX_CONSISTENT_VANISH, "Consistent Vanish" },
    { ZFIGHT_FIX_NO_VANISH, "No Vanish" },
};

static const std::unordered_map<int32_t, const char*> mirroredWorldModes = {
    { MIRRORED_WORLD_OFF, "Disabled" },
    { MIRRORED_WORLD_ALWAYS, "Always" },
    { MIRRORED_WORLD_RANDOM, "Random" },
    { MIRRORED_WORLD_RANDOM_SEEDED, "Random (Seeded)" },
    { MIRRORED_WORLD_DUNGEONS_ALL, "Dungeons" },
    { MIRRORED_WORLD_DUNGEONS_VANILLA, "Dungeons (Vanilla)" },
    { MIRRORED_WORLD_DUNGEONS_MQ, "Dungeons (MQ)" },
    { MIRRORED_WORLD_DUNGEONS_RANDOM, "Dungeons Random" },
    { MIRRORED_WORLD_DUNGEONS_RANDOM_SEEDED, "Dungeons Random (Seeded)" },
};

static const std::unordered_map<int32_t, const char*> enemyRandomizerModes = {
    { ENEMY_RANDOMIZER_OFF, "Disabled" },
    { ENEMY_RANDOMIZER_RANDOM, "Random" },
    { ENEMY_RANDOMIZER_RANDOM_SEEDED, "Random (Seeded)" },
};

static const std::unordered_map<int32_t, const char*> debugSaveFileModes = {
    { 0, "Off" },
    { 1, "Vanilla" },
    { 2, "Maxed" },
};

static const std::unordered_map<int32_t, const char*> bootSequenceLabels = {
    { BOOTSEQUENCE_DEFAULT, "Default" },
    { BOOTSEQUENCE_AUTHENTIC, "Authentic" },
    { BOOTSEQUENCE_FILESELECT, "File Select" },
};

static const std::unordered_map<int32_t, const char*> enhancementPresetList = {
    { ENHANCEMENT_PRESET_DEFAULT, "Default" },
    { ENHANCEMENT_PRESET_VANILLA_PLUS, "Vanilla Plus" },
    { ENHANCEMENT_PRESET_ENHANCED, "Enhanced" },
    { ENHANCEMENT_PRESET_RANDOMIZER, "Randomizer" },
};

class SohMenu : public Ship::Menu {
  public:
    SohMenu(const std::string& consoleVariable, const std::string& name);

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
    void Draw() override;

    void AddSidebarEntry(std::string sectionName, std::string sidbarName, uint32_t columnCount);
    WidgetInfo& AddWidget(WidgetPath& pathInfo, std::string widgetName, WidgetType widgetType);
    void AddMenuSettings();
    void AddMenuEnhancements();
    void AddMenuDevTools();
    void AddMenuRandomizer();
    void AddMenuNetwork();
    static void UpdateLanguageMap(std::unordered_map<int32_t, const char*>& languageMap);

  private:
    char mGitCommitHashTruncated[8];
    bool mIsTaggedVersion;
};
} // namespace SohGui

#endif // SOHMENU_H
