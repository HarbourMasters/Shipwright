#pragma once

#include <array>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include "category.hpp"
#include "cosmetics.hpp"
#include "debug.hpp"
#include "menu.hpp"
#include "pool_functions.hpp"

typedef enum {
    OFF,
    ON,
} BooleanSetting;

typedef enum {
    DONT_SKIP,
    SKIP,
} SkipSetting;

typedef enum {
    LOGIC_GLITCHLESS,
    LOGIC_GLITCHED,
    LOGIC_NONE,
    LOGIC_VANILLA,
} LogicSetting;

typedef enum {
    OPENFOREST_CLOSED,
    OPENFOREST_CLOSED_DEKU,
    OPENFOREST_OPEN,
} OpenForestSetting;

typedef enum {
    OPENKAKARIKO_CLOSED,
    OPENKAKARIKO_OPEN,
} OpenKakarikoSetting;

typedef enum {
    OPENDOOROFTIME_INTENDED,
    OPENDOOROFTIME_CLOSED,
    OPENDOOROFTIME_OPEN,
} OpenDoorOfTimeSetting;

typedef enum {
    ZORASFOUNTAIN_NORMAL,
    ZORASFOUNTAIN_ADULT,
    ZORASFOUNTAIN_OPEN,
} ZorasFountainSetting;

typedef enum {
    GERUDOFORTRESS_NORMAL,
    GERUDOFORTRESS_FAST,
    GERUDOFORTRESS_OPEN,
} GerudoFortressSetting;

typedef enum {
    RAINBOWBRIDGE_VANILLA,
    RAINBOWBRIDGE_OPEN,
    RAINBOWBRIDGE_STONES,
    RAINBOWBRIDGE_MEDALLIONS,
    RAINBOWBRIDGE_REWARDS,
    RAINBOWBRIDGE_DUNGEONS,
    RAINBOWBRIDGE_TOKENS,
} RainbowBridgeSetting;

typedef enum {
    LACSCONDITION_VANILLA,
    LACSCONDITION_STONES,
    LACSCONDITION_MEDALLIONS,
    LACSCONDITION_REWARDS,
    LACSCONDITION_DUNGEONS,
    LACSCONDITION_TOKENS,
} LACSConditionSetting;

typedef enum {
    AGE_CHILD,
    AGE_ADULT,
    AGE_RANDOM,
} AgeSetting;

typedef enum {
    SHUFFLEDUNGEONS_OFF,
    SHUFFLEDUNGEONS_ON,
    SHUFFLEDUNGEONS_GANON,
} ShuffleDungeonEntrancesSetting;

typedef enum {
    SHUFFLEINTERIORS_OFF,
    SHUFFLEINTERIORS_SIMPLE,
    SHUFFLEINTERIORS_ALL,
} ShuffleInteriorEntrancesSetting;

typedef enum {
    AMMODROPS_VANILLA,
    AMMODROPS_BOMBCHU,
    AMMODROPS_NONE,
} AmmoDropsSetting;

typedef enum {
    HEARTDROPREFILL_VANILLA,
    HEARTDROPREFILL_NODROP,
    HEARTDROPREFILL_NOREFILL,
    HEARTDROPREFILL_NODROPREFILL,
} HeartDropRefillSetting;

typedef enum {
    DUNGEONMODE_VANILLA,
    DUNGEONMODE_MQ,
} DungeonMode;

typedef enum {
    REWARDSHUFFLE_END_OF_DUNGEON,
    REWARDSHUFFLE_ANY_DUNGEON,
    REWARDSHUFFLE_OVERWORLD,
    REWARDSHUFFLE_ANYWHERE,
} RewardShuffleSetting;

typedef enum {
    LINKSPOCKETITEM_DUNGEON_REWARD,
    LINKSPOCKETITEM_ADVANCEMENT,
    LINKSPOCKETITEM_ANYTHING,
    LINKSPOCKETITEM_NOTHING,
} LinksPocketSetting;

typedef enum {
    SONGSHUFFLE_SONG_LOCATIONS,
    SONGSHUFFLE_DUNGEON_REWARDS,
    SONGSHUFFLE_ANYWHERE,
} SongShuffleSetting;

typedef enum {
    SHOPSANITY_OFF,
    SHOPSANITY_ZERO,
    SHOPSANITY_ONE,
    SHOPSANITY_TWO,
    SHOPSANITY_THREE,
    SHOPSANITY_FOUR,
    SHOPSANITY_RANDOM,
} ShopsanitySetting;

typedef enum {
    TOKENSANITY_OFF,
    TOKENSANITY_DUNGEONS,
    TOKENSANITY_OVERWORLD,
    TOKENSANITY_ALL_TOKENS,
} TokensanitySetting;

typedef enum {
    SCRUBSANITY_OFF,
    SCRUBSANITY_AFFORDABLE,
    SCRUBSANITY_EXPENSIVE,
    SCRUBSANITY_RANDOM_PRICES,
} ScrubsanitySetting;

typedef enum {
    SHUFFLEMERCHANTS_OFF,
    SHUFFLEMERCHANTS_NO_HINTS,
    SHUFFLEMERCHANTS_HINTS,
} ShuffleMerchantsSetting;

typedef enum {
    SHUFFLEADULTTRADEQUEST_OFF,
    SHUFFLEADULTTRADEQUEST_ON,
} ShuffleAdultTradeQuestSetting;

typedef enum {
    SHUFFLECHESTMINIGAME_OFF,
    SHUFFLECHESTMINIGAME_SINGLE_KEYS,
    SHUFFLECHESTMINIGAME_PACK,
} ShuffleChestMinigameSetting;

typedef enum {
    MAPSANDCOMPASSES_START_WITH,
    MAPSANDCOMPASSES_VANILLA,
    MAPSANDCOMPASSES_OWN_DUNGEON,
    MAPSANDCOMPASSES_ANY_DUNGEON,
    MAPSANDCOMPASSES_OVERWORLD,
    MAPSANDCOMPASSES_ANYWHERE,
} MapsAndCompassesSetting;

typedef enum {
    KEYSANITY_START_WITH,
    KEYSANITY_VANILLA,
    KEYSANITY_OWN_DUNGEON,
    KEYSANITY_ANY_DUNGEON,
    KEYSANITY_OVERWORLD,
    KEYSANITY_ANYWHERE,
} KeysanitySetting;

typedef enum {
    GERUDOKEYS_VANILLA,
    GERUDOKEYS_ANY_DUNGEON,
    GERUDOKEYS_OVERWORLD,
    GERUDOKEYS_ANYWHERE,
} GerudoKeysSetting;

typedef enum {
    KEYRINGS_OFF,
    KEYRINGS_RANDOM,
    KEYRINGS_RANDOM_COUNT,
    KEYRINGS_SELECTION,
} KeyRingsSetting;

typedef enum {
    BOSSKEYSANITY_START_WITH,
    BOSSKEYSANITY_VANILLA,
    BOSSKEYSANITY_OWN_DUNGEON,
    BOSSKEYSANITY_ANY_DUNGEON,
    BOSSKEYSANITY_OVERWORLD,
    BOSSKEYSANITY_ANYWHERE,
} BossKeysanitySetting;

typedef enum {
    GANONSBOSSKEY_VANILLA,
    GANONSBOSSKEY_OWN_DUNGEON,
    GANONSBOSSKEY_START_WITH,
    GANONSBOSSKEY_ANY_DUNGEON,
    GANONSBOSSKEY_OVERWORLD,
    GANONSBOSSKEY_ANYWHERE,
    GANONSBOSSKEY_LACS_VANILLA,
    GANONSBOSSKEY_LACS_MEDALLIONS,
    GANONSBOSSKEY_LACS_STONES,
    GANONSBOSSKEY_LACS_REWARDS,
    GANONSBOSSKEY_LACS_DUNGEONS,
    GANONSBOSSKEY_LACS_TOKENS,
} GanonsBossKeySetting;

typedef enum {
    QUICKTEXT_VANILLA,
    QUICKTEXT_SKIPPABLE,
    QUICKTEXT_INSTANT,
    QUICKTEXT_TURBO,
} QuickTextSetting;

typedef enum {
    SONGREPLAYS_DONT_SKIP,
    SONGREPLAYS_SKIP_NO_SFX,
    SONGREPLAYS_SKIP_KEEP_SFX,
} SkipSongReplaysSetting;

typedef enum {
    INCLUDE,
    EXCLUDE,
} ExcludeLocationSetting;

typedef enum {
    STARTINGINVENTORY_NONE,
} StartingInventorySetting;

typedef enum {
    TRICK_DISABLED,
    TRICK_ENABLED,
} LogicTrickSetting;

typedef enum {
    HINTMODE_OBSCURE,
    HINTMODE_AMBIGUOUS,
    HINTMODE_CLEAR,
} HintModeSetting;

typedef enum {
    DAMAGEMULTIPLIER_HALF,
    DAMAGEMULTIPLIER_DEFAULT,
    DAMAGEMULTIPLIER_DOUBLE,
    DAMAGEMULTIPLIER_QUADRUPLE,
    DAMAGEMULTIPLIER_OCTUPLE,
    DAMAGEMULTIPLIER_SEXDECUPLE,
    DAMAGEMULTIPLIER_OHKO,
} DamageMultiplierSetting;

typedef enum {
    STARTINGTIME_DAY,
    STARTINGTIME_NIGHT,
} StartingTimeSetting;

typedef enum {
    HINTS_NO_HINTS,
    HINTS_NEED_NOTHING,
    HINTS_MASK_OF_TRUTH,
    HINTS_STONE_OF_AGONY,
} GossipStoneHintsSetting;

typedef enum {
    ITEMPOOL_PLENTIFUL,
    ITEMPOOL_BALANCED,
    ITEMPOOL_SCARCE,
    ITEMPOOL_MINIMAL,
} ItemPoolSetting;

typedef enum {
    ICETRAPS_OFF,
    ICETRAPS_NORMAL,
    ICETRAPS_EXTRA,
    ICETRAPS_MAYHEM,
    ICETRAPS_ONSLAUGHT,
} IceTrapSetting;

typedef enum {
    GKDURABILITY_VANILLA,
    GKDURABILITY_RANDOMRISK,
    GKDURABILITY_RANDOMSAFE,
} GkDurabilitySetting;

typedef enum {
    STARTINGBOTTLE_NONE,
    STARTINGBOTTLE_EMPTY_BOTTLE,
    STARTINGBOTTLE_RED_POTION,
    STARTINGBOTTLE_GREEN_POTION,
    STARTINGBOTTLE_BLUE_POTION,
    STARTINGBOTTLE_FAIRY,
    STARTINGBOTTLE_FISH,
    STARTINGBOTTLE_MILK,
    STARTINGBOTTLE_BLUE_FIRE,
    STARTINGBOTTLE_BUGS,
    STARTINGBOTTLE_BIG_POE,
    STARTINGBOTTLE_HALF_MILK,
    STARTINGBOTTLE_POE,
} StartingBottleSetting;

typedef enum {
    STARTINGBGS_NONE,
    STARTINGBGS_GIANTS_KNIFE,
    STARTINGBGS_BIGGORON_SWORD,
} StartingBiggoronSwordSetting;

typedef enum {
    SHUFFLESFX_OFF,
    SHUFFLESFX_ALL,
    SHUFFLESFX_SCENESPECIFIC,
    SHUFFLESFX_CHAOS,
} ShuffleSFXSetting;

typedef enum {
    DUNGEON_NEITHER,
    DUNGEON_BARREN,
    DUNGEON_WOTH,
} DungeonInfo;

typedef enum {
    TRAILCOLOR_VANILLAMODE,
    TRAILCOLOR_FORCEDSIMPLEMODE,
    TRAILCOLOR_RAINBOW,
    TRAILCOLOR_RAINBOW_SIMPLEMODE,
} TrailColorMode;

typedef enum {
    TRAILDURATION_DISABLED,
    TRAILDURATION_VERYSHORT,
    TRAILDURATION_VANILLA,
    TRAILDURATION_LONG,
    TRAILDURATION_VERYLONG,
    TRAILDURATION_LIGHTSABER,
} TrailDuration;

typedef enum {
    PLAY_ON_CONSOLE,
    PLAY_ON_CITRA,
} PlayOption;

typedef struct {
    uint8_t hashIndexes[5];
    uint8_t playOption;

    uint8_t logic;
    uint8_t openForest;
    uint8_t openKakariko;
    uint8_t openDoorOfTime;
    uint8_t zorasFountain;
    uint8_t gerudoFortress;
    uint8_t rainbowBridge;
    uint8_t bridgeStoneCount;
    uint8_t bridgeMedallionCount;
    uint8_t bridgeRewardCount;
    uint8_t bridgeDungeonCount;
    uint8_t bridgeTokenCount;
    uint8_t randomGanonsTrials;
    uint8_t ganonsTrialsCount;

    uint8_t startingAge;
    uint8_t resolvedStartingAge;
    uint8_t shuffleDungeonEntrances;
    uint8_t shuffleOverworldEntrances;
    uint8_t shuffleInteriorEntrances;
    uint8_t shuffleGrottoEntrances;
    uint8_t bombchusInLogic;
    uint8_t ammoDrops;
    uint8_t heartDropRefill;
    uint8_t randomMQDungeons;
    uint8_t mqDungeonCount;
    uint8_t dungeonModesKnown[12]; // 12 dungeons which can be set Vanilla or MQ

    uint8_t shuffleRewards;
    uint8_t linksPocketItem;
    uint8_t shuffleSongs;
    uint8_t tokensanity;
    uint8_t scrubsanity;
    uint8_t shopsanity;
    uint8_t shuffleCows;
    uint8_t shuffleKokiriSword;
    uint8_t shuffleOcarinas;
    uint8_t shuffleWeirdEgg;
    uint8_t shuffleGerudoToken;
    uint8_t shuffleMagicBeans;
    uint8_t shuffleMerchants;
    uint8_t shuffleFrogSongRupees;
    uint8_t shuffleAdultTradeQuest;
    uint8_t shuffleChestMinigame;

    uint8_t mapsAndCompasses;
    uint8_t keysanity;
    uint8_t gerudoKeys;
    uint8_t bossKeysanity;
    uint8_t ganonsBossKey;
    uint8_t lacsCondition;
    uint8_t lacsMedallionCount;
    uint8_t lacsStoneCount;
    uint8_t lacsRewardCount;
    uint8_t lacsDungeonCount;
    uint8_t lacsTokenCount;

    uint8_t ringFortress;
    uint8_t ringForest;
    uint8_t ringFire;
    uint8_t ringWater;
    uint8_t ringSpirit;
    uint8_t ringShadow;
    uint8_t ringWell;
    uint8_t ringGtg;
    uint8_t ringCastle;

    uint8_t skipChildStealth;
    uint8_t skipTowerEscape;
    uint8_t skipEponaRace;
    uint8_t skipMinigamePhases;
    uint8_t freeScarecrow;
    uint8_t fourPoesCutscene;
    uint8_t lakeHyliaOwl;
    uint8_t bigPoeTargetCount;
    uint8_t numRequiredCuccos;
    uint8_t kingZoraSpeed;
    uint8_t completeMaskQuest;
    uint8_t enableGlitchCutscenes;
    uint8_t quickText;
    uint8_t skipSongReplays;
    uint8_t keepFWWarpPoint;
    uint8_t fastBunnyHood;

    uint8_t damageMultiplier;
    uint8_t startingTime;
    uint8_t gossipStoneHints;
    uint8_t compassesShowReward;
    uint8_t compassesShowWotH;
    uint8_t mapsShowDungeonMode;
    uint8_t chestAnimations;
    uint8_t chestSize;
    uint8_t generateSpoilerLog;
    uint8_t ingameSpoilers;
    uint8_t menuOpeningButton;
    uint8_t randomTrapDmg;
    uint8_t blueFireArrows;
    uint8_t sunLightArrows;

    uint8_t faroresWindAnywhere;
    uint8_t stickAsAdult;
    uint8_t boomerangAsAdult;
    uint8_t hammerAsChild;
    uint8_t slingshotAsAdult;
    uint8_t bowAsChild;
    uint8_t hookshotAsChild;
    uint8_t ironbootsAsChild;
    uint8_t hoverbootsAsChild;
    uint8_t masksAsAdult;
    uint8_t kokiriSwordAsAdult;
    uint8_t masterSwordAsChild;
    uint8_t biggoronSwordAsChild;
    uint8_t dekuShieldAsAdult;
    uint8_t mirrorShieldAsChild;
    uint8_t goronTunicAsChild;
    uint8_t zoraTunicAsChild;
    uint8_t gkDurability;

    uint8_t itemPoolValue;
    uint8_t iceTrapValue;
    uint8_t progressiveGoronSword;

    uint8_t mp_Enabled;
    uint8_t mp_SharedProgress;
    uint8_t mp_SyncId;
    uint8_t mp_SharedHealth;
    uint8_t mp_SharedRupees;
    uint8_t mp_SharedAmmo;

    uint8_t zTargeting;
    uint8_t cameraControl;
    uint8_t motionControl;
    uint8_t playMusic;
    uint8_t playSFX;
    uint8_t silenceNavi;
    uint8_t ignoreMaskReaction;

    uint8_t customTunicColors;
    uint8_t customNaviColors;
    uint8_t rainbowIdleNaviInnerColor;
    uint8_t rainbowNPCNaviInnerColor;
    uint8_t rainbowEnemyNaviInnerColor;
    uint8_t rainbowPropNaviInnerColor;
    uint8_t rainbowIdleNaviOuterColor;
    uint8_t rainbowNPCNaviOuterColor;
    uint8_t rainbowEnemyNaviOuterColor;
    uint8_t rainbowPropNaviOuterColor;
    uint8_t customTrailEffects;
    uint8_t rainbowSwordTrailInnerColor;
    uint8_t rainbowSwordTrailOuterColor;
    uint8_t boomerangTrailColorMode;
    uint8_t boomerangTrailDuration;
    uint8_t rainbowChuTrailInnerColor;
    uint8_t rainbowChuTrailOuterColor;
    uint8_t bombchuTrailDuration;

    uint8_t coloredKeys;
    uint8_t coloredBossKeys;
    uint8_t mirrorWorld;

    uint8_t shuffleSFX;
    uint8_t shuffleSFXCategorically;

    union {
        uint8_t dungeonModes[12];
        struct {
            uint8_t dekuTreeDungeonMode;
            uint8_t dodongosCavernDungeonMode;
            uint8_t jabuJabusBellyDungeonMode;
            uint8_t forestTempleDungeonMode;
            uint8_t fireTempleDungeonMode;
            uint8_t waterTempleDungeonMode;
            uint8_t spiritTempleDungeonMode;
            uint8_t shadowTempleDungeonMode;
            uint8_t bottomOfTheWellDungeonMode;
            uint8_t iceCavernDungeonMode;
            uint8_t ganonsCastleDungeonMode;
            uint8_t gerudoTrainingGroundsDungeonMode;
        };
    };

    uint8_t forestTrialSkip;
    uint8_t fireTrialSkip;
    uint8_t waterTrialSkip;
    uint8_t spiritTrialSkip;
    uint8_t shadowTrialSkip;
    uint8_t lightTrialSkip;

    uint32_t linksPocketRewardBitMask;

    uint8_t startingConsumables;
    uint8_t startingMaxRupees;
    uint8_t startingStickCapacity;
    uint8_t startingNutCapacity;
    uint8_t startingBombBag;
    uint8_t startingBombchus;
    uint8_t startingBow;
    uint8_t startingFireArrows;
    uint8_t startingIceArrows;
    uint8_t startingLightArrows;
    uint8_t startingDinsFire;
    uint8_t startingFaroresWind;
    uint8_t startingNayrusLove;
    uint8_t startingSlingshot;
    uint8_t startingBoomerang;
    uint8_t startingLensOfTruth;
    uint8_t startingMagicBean;
    uint8_t startingMegatonHammer;
    uint8_t startingHookshot;
    uint8_t startingIronBoots;
    uint8_t startingHoverBoots;
    uint8_t startingBottle1;
    uint8_t startingBottle2;
    uint8_t startingBottle3;
    uint8_t startingBottle4;
    uint8_t startingRutoBottle;
    uint8_t startingOcarina;
    uint8_t startingKokiriSword;
    uint8_t startingBiggoronSword;
    uint8_t startingMagicMeter;
    uint8_t startingDoubleDefense;
    uint8_t startingHearts;

    uint32_t startingQuestItems;
    uint32_t startingDungeonReward;
    uint32_t startingEquipment;
    uint32_t startingUpgrades;

    uint8_t startingTokens;

} SettingsContext;

class Option {
public:
    static Option Bool(std::string name_, std::vector<std::string> options_, std::vector<std::string_view> optionDescriptions_, OptionCategory category_ = OptionCategory::Setting, uint8_t defaultOption_ = 0, bool defaultHidden_ = false) {
        return Option{false, std::move(name_), std::move(options_), std::move(optionDescriptions_), category_, defaultOption_, defaultHidden_};
    }

    static Option U8(std::string name_, std::vector<std::string> options_, std::vector<std::string_view> optionDescriptions_, OptionCategory category_  = OptionCategory::Setting, uint8_t defaultOption_ = 0, bool defaultHidden_ = false) {
        return Option{uint8_t{0}, std::move(name_), std::move(options_), std::move(optionDescriptions_), category_, defaultOption_, defaultHidden_};
    }

    template <typename T>
    T Value() const {
        return std::get<T>(var);
    }

    template <typename T>
    bool Is(T other) const {
        static_assert(std::is_integral_v<T> || std::is_enum_v<T>,
                      "T must be an integral type or an enum.");

        if constexpr ((std::is_integral_v<T> && !std::is_same_v<bool, T>) || std::is_enum_v<T>) {
            return Value<uint8_t>() == static_cast<uint8_t>(other);
        } else {
            return Value<bool>() == static_cast<bool>(other);
        }
    }

    template <typename T>
    bool IsNot(T other) const {
        return !Is(other);
    }

    explicit operator bool() const {
        if (std::holds_alternative<bool>(var)) {
            return Value<bool>();
        } else {
            return Value<uint8_t>() != 0;
        }
    }

    void SetOptions(std::vector<std::string> o) {
        options = std::move(o);
        SetToDefault();
    }

    size_t GetOptionCount() const {
        return options.size();
    }

    const std::string& GetName() const {
        return name;
    }

    const std::string& GetSelectedOptionText() const {
        return options[selectedOption];
    }

    void SetSelectedOptionText(std::string newText) {
        options[selectedOption] = std::move(newText);
    }

    bool IsDefaultSelected() {
      return selectedOption == defaultOption;
    }

    void SetToDefault() {
      SetSelectedIndex(defaultOption);
      hidden = defaultHidden;
    }

    std::string_view GetSelectedOptionDescription() const {
      //bounds checking
      if (selectedOption >= optionDescriptions.size()) {
        return optionDescriptions[optionDescriptions.size()-1];
      }
      return optionDescriptions[selectedOption];
    }

    uint8_t GetSelectedOptionIndex() const {
      return selectedOption;
    }

    void NextOptionIndex() {
        ++selectedOption;
    }

    void PrevOptionIndex() {
        --selectedOption;
    }

    void SanitizeSelectedOptionIndex() {
        if (selectedOption == options.size()) {
            selectedOption = 0;
        } else if (selectedOption == 0xFF) {
            selectedOption = static_cast<uint8_t>(options.size() - 1);
        }
    }

    void SetVariable() {
      if (std::holds_alternative<bool>(var)) {
        var.emplace<bool>(selectedOption != 0);
      } else {
        var.emplace<uint8_t>(selectedOption);
      }
    }

    void SetDelayedOption() {
      delayedOption = selectedOption;
    }

    void RestoreDelayedOption() {
      selectedOption = delayedOption;
      SetVariable();
    }

    void SetSelectedIndex(size_t idx) {
      selectedOption = idx;
      if (selectedOption >= options.size()) {
        printf("\x1b[30;0HERROR: Incompatible selection for %s\n", name.c_str());
        selectedOption = 0;
      }

      SetVariable();
    }

    void SetSelectedIndexByString(std::string newSetting) {
      using namespace Cosmetics;

      //Special case for custom cosmetic settings
      if (options.size() > CUSTOM_COLOR) {
        if (newSetting.compare(0, 8, CUSTOM_COLOR_PREFIX) == 0 && options[CUSTOM_COLOR].compare(0, 8, CUSTOM_COLOR_PREFIX) == 0) {
          SetSelectedIndex(CUSTOM_COLOR);
          SetSelectedOptionText(newSetting);
          return;
        }
      }

      for (size_t i = 0; i < options.size(); i++) {
        std::string settingName = options[i];
        if (settingName == newSetting) {
          SetSelectedIndex(i);
          return;
        }
      }
    }

    void Lock() {
      locked = true;
    }

    void Unlock() {
      locked = false;
    }

    bool IsLocked() const {
      return locked;
    }

    void Hide() {
      hidden = true;
    }

    void Unhide() {
      hidden = false;
    }

    bool IsHidden() const {
      return hidden;
    }

    bool IsCategory(OptionCategory category) const {
      return category == this->category;
    }

private:
    Option(uint8_t var_, std::string name_, std::vector<std::string> options_, std::vector<std::string_view> optionDescriptions_, OptionCategory category_, uint8_t defaultOption_, bool defaultHidden_)
          : var(var_), name(std::move(name_)), options(std::move(options_)), optionDescriptions(std::move(optionDescriptions_)), category(category_), defaultOption(defaultOption_), defaultHidden(defaultHidden_) {
        selectedOption = defaultOption;
        hidden = defaultHidden;
        SetVariable();
    }

    Option(bool var_, std::string name_, std::vector<std::string> options_, std::vector<std::string_view> optionDescriptions_, OptionCategory category_, uint8_t defaultOption_, bool defaultHidden_)
          : var(var_), name(std::move(name_)),  options(std::move(options_)), optionDescriptions(std::move(optionDescriptions_)), category(category_), defaultOption(defaultOption_), defaultHidden(defaultHidden_) {
        selectedOption = defaultOption;
        hidden = defaultHidden;
        SetVariable();
    }

  std::variant<bool, uint8_t> var;
  std::string name;
  std::vector<std::string> options;
  std::vector<std::string_view> optionDescriptions;
  uint8_t selectedOption = 0;
  uint8_t delayedOption = 0;
  bool locked = false;
  bool hidden = false;
  OptionCategory category;
  uint8_t defaultOption = 0;
  bool defaultHidden = false;
};

enum class MenuType {
  MainMenu,
  SubMenu,
  Action,
};

class Menu {
  public:

    static Menu SubMenu(std::string name_, std::vector<Option *>* settingsList_, bool printInSpoiler_ = true) {
      return Menu{std::move(name_), MenuType::SubMenu, std::move(settingsList_), OPTION_SUB_MENU, printInSpoiler_};
    }

    static Menu SubMenu(std::string name_, std::vector<Menu *>* itemsList_, bool printInSpoiler_ = true) {
      return Menu{std::move(name_), MenuType::SubMenu, std::move(itemsList_), SUB_MENU, printInSpoiler_};
    }

    static Menu Action(std::string name_, uint8_t mode_) {
      return Menu{std::move(name_), MenuType::Action, std::move(mode_)};
    }

    Menu(std::string name_, MenuType type_, std::vector<Option *>* settingsList_, uint8_t mode_, bool printInSpoiler_ = true)
        : name(std::move(name_)), type(type_), settingsList(std::move(settingsList_)), mode(mode_), printInSpoiler(printInSpoiler_) {}

    Menu(std::string name_, MenuType type_, std::vector<Menu *>* itemsList_, uint8_t mode_, bool printInSpoiler_ = true)
        : name(std::move(name_)), type(type_), itemsList(std::move(itemsList_)), mode(mode_), printInSpoiler(printInSpoiler_) {}

    Menu(std::string name_, MenuType type_, uint8_t mode_)
        : name(std::move(name_)), type(type_), mode(mode_) {}

    void ResetMenuIndex() {
      if (mode == OPTION_SUB_MENU) {
        for (size_t i = 0; i < settingsList->size(); i++) {
          if (!settingsList->at(i)->IsLocked() && !settingsList->at(i)->IsHidden()) {
            menuIdx = i;
            settingBound = i;
            return;
          }
        }
      }
      menuIdx = 0;
      settingBound = 0;
    }

    std::string name;
    MenuType type;
    std::vector<Option *>* settingsList;
    std::vector<Menu *>* itemsList;
    uint8_t mode;
    uint16_t menuIdx = 0;
    uint16_t settingBound = 0;
    int selectedSetting = 0;
    bool printInSpoiler = true;
};

namespace Settings {
void UpdateSettings(std::unordered_map<RandomizerSettingKey, uint8_t> cvarSettings, std::set<RandomizerCheck> excludedLocations);
  SettingsContext FillContext();
  void InitSettings();
  void SetDefaultSettings();
  void ResolveExcludedLocationConflicts();
  void RandomizeAllSettings(const bool selectOptions = false);
  void ForceChange(uint32_t kDown, Option* currentSetting);
  const std::vector<Menu*> GetAllOptionMenus();


  extern std::string seed;
  extern std::string version;
  extern std::array<uint8_t, 5> hashIconIndexes;
  extern std::string hash;

  extern bool skipChildZelda;

  extern Option OpenForest;
  extern Option OpenKakariko;
  extern Option OpenDoorOfTime;
  extern Option ZorasFountain;
  extern Option GerudoFortress;
  extern Option Bridge;
  extern Option BridgeStoneCount;
  extern Option BridgeMedallionCount;
  extern Option BridgeRewardCount;
  extern Option BridgeDungeonCount;
  extern Option BridgeTokenCount;
  extern Option RandomGanonsTrials;
  extern Option GanonsTrialsCount;

  extern Option StartingAge;
  extern uint8_t ResolvedStartingAge;
  extern Option ShuffleEntrances;
  extern Option ShuffleDungeonEntrances;
  extern Option ShuffleOverworldEntrances;
  extern Option ShuffleInteriorEntrances;
  extern Option ShuffleGrottoEntrances;
  extern Option BombchusInLogic;
  extern Option AmmoDrops;
  extern Option HeartDropRefill;
  extern Option MQDungeonCount;
  extern Option SetDungeonTypes;

  extern Option ShuffleRewards;
  extern Option LinksPocketItem;
  extern Option ShuffleSongs;
  extern Option Shopsanity;
  extern Option Tokensanity;
  extern Option Scrubsanity;
  extern Option ShuffleCows;
  extern Option ShuffleKokiriSword;
  extern Option ShuffleOcarinas;
  extern Option ShuffleWeirdEgg;
  extern Option ShuffleGerudoToken;
  extern Option ShuffleMagicBeans;
  extern Option ShuffleMerchants;
  extern Option ShuffleFrogSongRupees;
  extern Option ShuffleAdultTradeQuest;
  extern Option ShuffleChestMinigame;

  extern Option MapsAndCompasses;
  extern Option Keysanity;
  extern Option GerudoKeys;
  extern Option BossKeysanity;
  extern Option GanonsBossKey;
  extern uint8_t LACSCondition;
  extern Option LACSMedallionCount;
  extern Option LACSStoneCount;
  extern Option LACSRewardCount;
  extern Option LACSDungeonCount;
  extern Option LACSTokenCount;
  extern Option KeyRings;
  extern Option KeyRingsRandomCount;
  extern Option RingFortress;
  extern Option RingForest;
  extern Option RingFire;
  extern Option RingWater;
  extern Option RingSpirit;
  extern Option RingShadow;
  extern Option RingWell;
  extern Option RingGtg;
  extern Option RingCastle;

  extern Option SkipChildStealth;
  extern Option SkipTowerEscape;
  extern Option SkipEponaRace;
  extern Option SkipMinigamePhases;
  extern Option FreeScarecrow;
  extern Option FourPoesCutscene;
  extern Option LakeHyliaOwl;
  extern Option BigPoeTargetCount;
  extern Option NumRequiredCuccos;
  extern Option KingZoraSpeed;
  extern Option CompleteMaskQuest;
  extern Option EnableGlitchCutscenes;
  extern Option QuickText;
  extern Option SkipSongReplays;
  extern Option KeepFWWarpPoint;
  extern Option FastBunnyHood;

  extern Option GossipStoneHints;
  extern Option ClearerHints;
  extern Option HintDistribution;
  extern Option DamageMultiplier;
  extern Option StartingTime;
  extern Option ChestAnimations;
  extern Option ChestSize;
  extern Option GenerateSpoilerLog;
  extern Option IngameSpoilers;
  extern Option MenuOpeningButton;
  extern Option RandomTrapDmg;
  extern Option BlueFireArrows;
  extern Option SunlightArrows;
  extern bool HasNightStart;

  extern Option FaroresWindAnywhere;
  extern Option StickAsAdult;
  extern Option BoomerangAsAdult;
  extern Option HammerAsChild;
  extern Option SlingshotAsAdult;
  extern Option BowAsChild;
  extern Option HookshotAsChild;
  extern Option IronBootsAsChild;
  extern Option HoverBootsAsChild;
  extern Option MasksAsAdult;
  extern Option KokiriSwordAsAdult;
  extern Option MasterSwordAsChild;
  extern Option BiggoronSwordAsChild;
  extern Option DekuShieldAsAdult;
  extern Option MirrorShieldAsChild;
  extern Option GoronTunicAsChild;
  extern Option ZoraTunicAsChild;
  extern Option GkDurability;

  extern Option ItemPoolValue;
  extern Option IceTrapValue;
  extern Option RemoveDoubleDefense;
  extern Option ProgressiveGoronSword;

  //Starting Inventory
  extern Option StartingConsumables;
  extern Option StartingMaxRupees;
  extern Option StartingStickCapacity;
  extern Option StartingNutCapacity;
  extern Option StartingBombBag;
  extern Option StartingBombchus;
  extern Option StartingBow;
  extern Option StartingFireArrows;
  extern Option StartingIceArrows;
  extern Option StartingLightArrows;
  extern Option StartingDinsFire;
  extern Option StartingFaroresWind;
  extern Option StartingNayrusLove;
  extern Option StartingSlingshot;
  extern Option StartingBoomerang;
  extern Option StartingLensOfTruth;
  extern Option StartingMagicBean;
  extern Option StartingMegatonHammer;
  extern Option StartingHookshot;
  extern Option StartingIronBoots;
  extern Option StartingHoverBoots;
  extern Option StartingBottle1;
  extern Option StartingBottle2;
  extern Option StartingBottle3;
  extern Option StartingBottle4;
  extern Option StartingRutoBottle;
  extern Option StartingOcarina;
  extern Option StartingZeldasLullaby;
  extern Option StartingEponasSong;
  extern Option StartingSariasSong;
  extern Option StartingSunsSong;
  extern Option StartingSongOfTime;
  extern Option StartingSongOfStorms;
  extern Option StartingMinuetOfForest;
  extern Option StartingBoleroOfFire;
  extern Option StartingSerenadeOfWater;
  extern Option StartingRequiemOfSpirit;
  extern Option StartingNocturneOfShadow;
  extern Option StartingPreludeOfLight;
  extern Option StartingKokiriSword;
  extern Option StartingBiggoronSword;
  extern Option StartingDekuShield;
  extern Option StartingHylianShield;
  extern Option StartingMirrorShield;
  extern Option StartingGoronTunic;
  extern Option StartingZoraTunic;
  extern Option StartingMagicMeter;
  extern Option StartingStrength;
  extern Option StartingScale;
  extern Option StartingWallet;
  extern Option StartingShardOfAgony;
  extern Option StartingDoubleDefense;
  extern Option StartingHearts;
  extern Option StartingKokiriEmerald;
  extern Option StartingGoronRuby;
  extern Option StartingZoraSapphire;
  extern Option StartingForestMedallion;
  extern Option StartingFireMedallion;
  extern Option StartingWaterMedallion;
  extern Option StartingSpiritMedallion;
  extern Option StartingShadowMedallion;
  extern Option StartingLightMedallion;
  extern Option StartingSkulltulaToken;

  //Logic Settings
  extern Option Logic;
  extern Option LocationsReachable;
  extern Option NightGSExpectSuns;

  //Trick Settings
  extern Option ToggleAllTricks;
  extern Option LogicGrottosWithoutAgony;
  extern Option LogicVisibleCollision;
  extern Option LogicFewerTunicRequirements;
  extern Option LogicLostWoodsGSBean;
  extern Option LogicLabDiving;
  extern Option LogicLabWallGS;
  extern Option LogicGraveyardPoH;
  extern Option LogicChildDampeRacePoH;
  extern Option LogicGVHammerChest;
  extern Option LogicGerudoKitchen;
  extern Option LogicLensWasteland;
  extern Option LogicReverseWasteland;
  extern Option LogicColossusGS;
  extern Option LogicOutsideGanonsGS;
  extern Option LogicManOnRoof;
  extern Option LogicWindmillPoHHookshot;
  extern Option LogicDMTBombable;
  extern Option LogicDMTSoilGS;
  extern Option LogicDMTSummitHover;
  extern Option LogicLinkGoronDins;
  extern Option LogicGoronCityLeftMost;
  extern Option LogicGoronCityPot;
  extern Option LogicGoronCityPotWithStrength;
  extern Option LogicChildRollingWithStrength;
  extern Option LogicCraterUpperToLower;
  extern Option LogicCraterBeanPoHWithHovers;
  extern Option LogicBiggoronBolero;
  extern Option LogicZoraRiverLower;
  extern Option LogicZoraRiverUpper;
  extern Option LogicZFGreatFairy;
  extern Option LogicDekuB1WebsWithBow;
  extern Option LogicDekuB1Skip;
  extern Option LogicDekuBasementGS;
  extern Option LogicDCStaircase;
  extern Option LogicDCJump;
  extern Option LogicDCSlingshotSkip;
  extern Option LogicDCScarecrowGS;
  extern Option LogicJabuBossGSAdult;
  extern Option LogicJabuScrubJumpDive;
  extern Option LogicForestOutsideBackdoor;
  extern Option LogicForestDoorFrame;
  extern Option LogicForestOutdoorEastGS;
  extern Option LogicFireBossDoorJump;
  extern Option LogicFireStrength;
  extern Option LogicFireScarecrow;
  extern Option LogicFireFlameMaze;
  extern Option LogicFireSongOfTime;
  extern Option LogicWaterTempleTorchLongshot;
  extern Option LogicWaterTempleUpperBoost;
  extern Option LogicWaterCentralBow;
  extern Option LogicWaterCentralGSFW;
  extern Option LogicWaterCrackedWallNothing;
  extern Option LogicWaterCrackedWallHovers;
  extern Option LogicWaterBossKeyRegion;
  extern Option LogicWaterBKJumpDive;
  extern Option LogicWaterNorthBasementLedgeJump;
  extern Option LogicWaterDragonJumpDive;
  extern Option LogicWaterDragonAdult;
  extern Option LogicWaterRiverGS;
  extern Option LogicWaterFallingPlatformGS;
  extern Option LogicSpiritLowerAdultSwitch;
  extern Option LogicSpiritChildBombchu;
  extern Option LogicSpiritWall;
  extern Option LogicSpiritLobbyGS;
  extern Option LogicSpiritMapChest;
  extern Option LogicSpiritSunChest;
  extern Option LogicShadowFireArrowEntry;
  extern Option LogicShadowUmbrella;
  extern Option LogicShadowFreestandingKey;
  extern Option LogicShadowStatue;
  extern Option LogicChildDeadhand;
  extern Option LogicGtgWithoutHookshot;
  extern Option LogicGtgFakeWall;
  extern Option LogicLensSpirit;
  extern Option LogicLensShadow;
  extern Option LogicLensShadowBack;
  extern Option LogicLensBotw;
  extern Option LogicLensGtg;
  extern Option LogicLensCastle;
  extern Option LogicLensJabuMQ;
  extern Option LogicLensSpiritMQ;
  extern Option LogicLensShadowMQ;
  extern Option LogicLensShadowMQBack;
  extern Option LogicLensBotwMQ;
  extern Option LogicLensGtgMQ;
  extern Option LogicLensCastleMQ;
  extern Option LogicSpiritTrialHookshot;
  extern Option LogicFlamingChests;

  //Glitch Settings
  extern Option GlitchRestrictedItems;
  extern Option GlitchSuperStab;
  extern Option GlitchISG;
  extern Option GlitchHover;
  extern Option GlitchBombOI;
  extern Option GlitchHoverBoost;
  extern Option GlitchSuperSlide;
  extern Option GlitchMegaflip;
  extern Option GlitchASlide;
  extern Option GlitchHammerSlide;
  extern Option GlitchLedgeCancel;
  extern Option GlitchActionSwap;
  extern Option GlitchQPA;
  extern Option GlitchHookshotClip;
  extern Option GlitchHookshotJump_Bonk;
  extern Option GlitchHookshotJump_Boots;
  extern Option GlitchCutsceneDive;
  extern Option GlitchNaviDive_Stick;
  extern Option GlitchTripleSlashClip;
  extern Option GlitchLedgeClip;
  extern Option GlitchSeamWalk;
  //Misc Glitch Settings
  extern Option GlitchWWTEscape;
  extern Option GlitchGVTentAsChild;
  extern Option GlitchGFGuardSneak;
  extern Option GlitchItemlessWasteland;
  extern Option GlitchOccamsStatue;
  extern Option GlitchZDOoBJumpSlash;
  extern Option GlitchJabuStickRecoil;
  extern Option GlitchJabuAdult;
  extern Option GlitchBlueFireWall;
  extern Option GlitchClassicHalfie;
  extern Option GlitchModernHalfie;
  extern Option GlitchJabuSwitch;
  extern Option GlitchForestBKSkip;
  extern Option GlitchFireGrunzClip;

  //Multiplayer Settings
  extern Option MP_Enabled;
  extern Option MP_SharedProgress;
  extern Option MP_SyncId;
  extern Option MP_SharedHealth;
  extern Option MP_SharedRupees;
  extern Option MP_SharedAmmo;

  //Ingame Default Settings
  extern Option ZTargeting;
  extern Option CameraControl;
  extern Option MotionControl;
  extern Option TogglePlayMusic;
  extern Option TogglePlaySFX;
  extern Option SilenceNavi;
  extern Option IgnoreMaskReaction;

  //Cosmetic Settings
  extern Option CustomTunicColors;
  extern Option ChildTunicColor;
  extern Option KokiriTunicColor;
  extern Option GoronTunicColor;
  extern Option ZoraTunicColor;
  extern Option SilverGauntletsColor;
  extern Option GoldGauntletsColor;
  extern Option CustomNaviColors;
  extern Option IdleNaviInnerColor;
  extern Option NPCNaviInnerColor;
  extern Option EnemyNaviInnerColor;
  extern Option PropNaviInnerColor;
  extern Option IdleNaviOuterColor;
  extern Option NPCNaviOuterColor;
  extern Option EnemyNaviOuterColor;
  extern Option PropNaviOuterColor;
  extern Option CustomTrailEffects;
  extern Option SwordTrailInnerColor;
  extern Option SwordTrailOuterColor;
  extern Option SwordTrailDuration;
  extern Option BoomerangTrailColor;
  extern Option BoomerangTrailDuration;
  extern Option ChosenSimpleMode;
  extern Option BombchuTrailInnerColor;
  extern Option BombchuTrailOuterColor;
  extern Option BombchuTrailDuration;
  extern std::string finalChildTunicColor;
  extern std::string finalKokiriTunicColor;
  extern std::string finalGoronTunicColor;
  extern std::string finalZoraTunicColor;
  extern std::string finalSilverGauntletsColor;
  extern std::string finalGoldGauntletsColor;
  extern std::string finalIdleNaviInnerColor;
  extern std::string finalNPCNaviInnerColor;
  extern std::string finalEnemyNaviInnerColor;
  extern std::string finalPropNaviInnerColor;
  extern std::string finalIdleNaviOuterColor;
  extern std::string finalNPCNaviOuterColor;
  extern std::string finalEnemyNaviOuterColor;
  extern std::string finalPropNaviOuterColor;
  extern std::string finalSwordTrailInnerColor;
  extern std::string finalSwordTrailOuterColor;
  extern Cosmetics::Color_RGBA8 finalBoomerangColor;
  extern uint8_t boomerangTrailColorMode;
  extern std::string finalChuTrailInnerColor;
  extern std::string finalChuTrailOuterColor;

  extern Option ColoredKeys;
  extern Option ColoredBossKeys;
  extern Option MirrorWorld;

  extern Option ShuffleMusic;
  extern Option ShuffleBGM;
  extern Option ShuffleFanfares;
  extern Option ShuffleOcaMusic;
  extern Option ShuffleSFX;
  extern Option ShuffleSFXCategorically;

  extern uint32_t LinksPocketRewardBitMask;
  extern std::array<uint32_t, 9> rDungeonRewardOverrides;

  extern uint8_t PlayOption;

  extern std::vector<std::vector<Option *>> excludeLocationsOptionsVector;
  extern std::vector<Menu *> excludeLocationsMenus;
  extern std::vector<Option *> startingItemsOptions;
  extern std::vector<Option *> startingSongsOptions;
  extern std::vector<Option *> startingEquipmentOptions;
  extern std::vector<Option *> startingStonesMedallionsOptions;
  extern std::vector<Option *> startingOthersOptions;
  extern std::vector<Option *> trickOptions;
  extern std::vector<Option *> glitchCategories;
  extern std::vector<Option *> miscGlitches;

  extern std::vector<Menu *> startingInventoryOptions;
  extern std::vector<Menu *> detailedLogicOptions;

  extern std::vector<Menu *> mainMenu;

  extern std::vector<Option *> vanillaLogicDefaults;
}
