#include "settings.hpp"

#include "dungeon.hpp"
#include "fill.hpp"
#include "item_location.hpp"
#include "random.hpp"
#include "randomizer.hpp"
#include "trial.hpp"
#include "keys.hpp"

using namespace Dungeon;
using namespace Trial;

namespace Settings {
  uint32_t seed;
  std::string hash;
  std::string version = RANDOMIZER_VERSION "-" COMMIT_NUMBER;
  std::array<uint8_t, 5> hashIconIndexes;
  std::string seedString;

  bool skipChildZelda = false;

  std::vector<std::string> NumOpts(int min, int max, int step = 1, std::string textBefore = {}, std::string textAfter = {}) {
    std::vector<std::string> options;
    options.reserve((max - min) / step + 1);
    for (int i = min; i <= max; i += step) {
      options.push_back(textBefore + std::to_string(i) + textAfter);
    }
    return options;
  }

  std::vector<std::string> MultiVecOpts(std::vector<std::vector<std::string>> optionsVector) {
    uint32_t totalSize = 0;
    for (auto vector : optionsVector) {
      totalSize += vector.size();
    }
    std::vector<std::string> options;
    options.reserve(totalSize);
    for (auto vector : optionsVector) {
      for (auto op : vector) {
        options.push_back(op);
      }
    }
    return options;
  }

  //                                        Setting name,            Options,                           Category (default: Setting),Default index (default: 0), Default hidden (default: false)
  //Open Settings
  Option RandomizeOpen       = Option::Bool("Randomize Settings",    {"No","Yes"},                      OptionCategory::Toggle);
  Option OpenForest          = Option::U8  ("Forest",                {"Closed", "Closed Deku", "Open"}, OptionCategory::Setting, OPENFOREST_CLOSED);
  Option OpenKakariko        = Option::U8  ("Kakariko Gate",         {"Closed", "Open"});
  Option OpenDoorOfTime      = Option::U8  ("Door of Time",          {"Closed", "Song only", "Open"});
  Option ZorasFountain       = Option::U8  ("Zora's Fountain",       {"Closed", "Closed as child", "Open"});
  Option GerudoFortress      = Option::U8  ("Gerudo Fortress",       {"Normal", "Fast", "Open"});
  Option Bridge              = Option::U8  ("Rainbow Bridge",        {"Vanilla", "Always open", "Stones", "Medallions", "Dungeon rewards", "Dungeons", "Tokens", "Greg"}, OptionCategory::Setting, RAINBOWBRIDGE_VANILLA);
  Option BridgeStoneCount    = Option::U8  ("Stone Count",           {NumOpts(0, 4)},   OptionCategory::Setting, 1, true);
  Option BridgeMedallionCount= Option::U8  ("Medallion Count",       {NumOpts(0, 7)},   OptionCategory::Setting, 1, true);
  Option BridgeRewardCount   = Option::U8  ("Reward Count",          {NumOpts(0, 10)},  OptionCategory::Setting, 1, true);
  Option BridgeDungeonCount  = Option::U8  ("Dungeon Count",         {NumOpts(0, 9)},   OptionCategory::Setting, 1, true);
  Option BridgeTokenCount    = Option::U8  ("Token Count",           {NumOpts(0, 100)}, OptionCategory::Setting, 1, true);
  Option BridgeRewardOptions = Option::U8  ("Bridge Reward Options", {"Standard Rewards", "Greg as Reward", "Greg as Wildcard"});
  Option RandomGanonsTrials  = Option::Bool("Random Ganon's Trials", {"Off", "On"},     OptionCategory::Setting, ON);
  Option GanonsTrialsCount   = Option::U8  ("Trial Count",           {NumOpts(0, 6)},   OptionCategory::Setting, 1, true);
  std::vector<Option *> openOptions = {
    &RandomizeOpen,
    &OpenForest,
    &OpenKakariko,
    &OpenDoorOfTime,
    &ZorasFountain,
    &GerudoFortress,
    &Bridge,
    &BridgeStoneCount,
    &BridgeMedallionCount,
    &BridgeRewardCount,
    &BridgeDungeonCount,
    &BridgeTokenCount,
    &BridgeRewardOptions,
    &RandomGanonsTrials,
    &GanonsTrialsCount,
  };

  //World Settings
  Option RandomizeWorld            = Option::Bool("Randomize Settings",     {"No","Yes"},                 OptionCategory::Toggle);
  Option StartingAge               = Option::U8  ("Starting Age",           {"Child", "Adult", "Random"}, OptionCategory::Setting, AGE_CHILD);
  uint8_t ResolvedStartingAge;
  Option ShuffleEntrances          = Option::Bool("Shuffle Entrances",      {"Off", "On"});
  Option ShuffleDungeonEntrances   = Option::U8  ("Dungeon Entrances",      {"Off", "On", "On + Ganon"});
  Option ShuffleBossEntrances      = Option::U8  ("Boss Entrances",         {"Off", "Age Restricted", "Full"});
  Option ShuffleOverworldEntrances = Option::Bool("Overworld Entrances",    {"Off", "On"});
  Option ShuffleInteriorEntrances  = Option::U8  ("Interior Entrances",     {"Off", "Simple", "All"});
  Option ShuffleGrottoEntrances    = Option::Bool("Grottos Entrances",      {"Off", "On"});
  Option ShuffleOwlDrops           = Option::Bool("Owl Drops",              {"Off", "On"});
  Option ShuffleWarpSongs          = Option::Bool("Warp Songs",             {"Off", "On"});
  Option ShuffleOverworldSpawns    = Option::Bool("Overworld Spawns",       {"Off", "On"});
  Option MixedEntrancePools        = Option::Bool("Mixed Entrance Pools",   {"Off", "On"});
  Option MixDungeons               = Option::Bool("Mix Dungeons",           {"Off", "On"});
  Option MixOverworld              = Option::Bool("Mix Overworld",          {"Off", "On"});
  Option MixInteriors              = Option::Bool("Mix Interiors",          {"Off", "On"});
  Option MixGrottos                = Option::Bool("Mix Grottos",            {"Off", "On"});
  Option DecoupleEntrances         = Option::Bool("Decouple Entrances",     {"Off", "On"});
  Option BombchusInLogic           = Option::Bool("Bombchus in Logic",      {"Off", "On"});
  Option AmmoDrops                 = Option::U8  ("Ammo Drops",             {"On", "On + Bombchu", "Off"},         OptionCategory::Setting, AMMODROPS_BOMBCHU);
  Option HeartDropRefill           = Option::U8  ("Heart Drops and Refills",{"On", "No Drop", "No Refill", "Off"}, OptionCategory::Setting, HEARTDROPREFILL_VANILLA);
  Option TriforceHunt              = Option::U8  ("Triforce Hunt",          {"Off", "On"});
  Option TriforceHuntTotal         = Option::U8  ("Triforce Hunt Total Pieces", {NumOpts(0, 100)});
  Option TriforceHuntRequired      = Option::U8  ("Triforce Hunt Required Pieces", {NumOpts(0, 100)});
  Option MQDungeonCount = Option::U8(
      "MQ Dungeon Count", { MultiVecOpts({ NumOpts(0, 12), { "Random" }, { "Selection" } }) });
  uint8_t MQSet;
  bool DungeonModesKnown[12];
  Option SetDungeonTypes           = Option::Bool("Set Dungeon Types",    {"Off", "On"});
  Option MQDeku                    = Option::U8  ("Deku Tree",            {"Vanilla", "Master Quest", "Random"});
  Option MQDodongo                 = Option::U8  ("Dodongo's Cavern",     {"Vanilla", "Master Quest", "Random"});
  Option MQJabu                    = Option::U8  ("Jabu-Jabu's Belly",    {"Vanilla", "Master Quest", "Random"});
  Option MQForest                  = Option::U8  ("Forest Temple",        {"Vanilla", "Master Quest", "Random"});
  Option MQFire                    = Option::U8  ("Fire Temple",          {"Vanilla", "Master Quest", "Random"});
  Option MQWater                   = Option::U8  ("Water Temple",         {"Vanilla", "Master Quest", "Random"});
  Option MQSpirit                  = Option::U8  ("Spirit Temple",        {"Vanilla", "Master Quest", "Random"});
  Option MQShadow                  = Option::U8  ("Shadow Temple",        {"Vanilla", "Master Quest", "Random"});
  Option MQBotW                    = Option::U8  ("Bottom of the Well",   {"Vanilla", "Master Quest", "Random"});
  Option MQIceCavern               = Option::U8  ("Ice Cavern",           {"Vanilla", "Master Quest", "Random"});
  Option MQGTG                     = Option::U8  ("Training Grounds",     {"Vanilla", "Master Quest", "Random"});
  Option MQCastle                  = Option::U8  ("Ganon's Castle",       {"Vanilla", "Master Quest", "Random"});
  std::vector<Option *> worldOptions = {
    &RandomizeWorld,
    &StartingAge,
    &ShuffleEntrances,
    &ShuffleDungeonEntrances,
    &ShuffleBossEntrances,
    &ShuffleOverworldEntrances,
    &ShuffleInteriorEntrances,
    &ShuffleGrottoEntrances,
    &ShuffleOwlDrops,
    &ShuffleWarpSongs,
    &ShuffleOverworldSpawns,
    &MixedEntrancePools,
    &MixDungeons,
    &MixOverworld,
    &MixInteriors,
    &MixGrottos,
    &DecoupleEntrances,
    &BombchusInLogic,
    &AmmoDrops,
    &HeartDropRefill,
    &TriforceHunt,
    &TriforceHuntTotal,
    &TriforceHuntRequired,
    &MQDungeonCount,
    &SetDungeonTypes,
    &MQDeku,
    &MQDodongo,
    &MQJabu,
    &MQForest,
    &MQFire,
    &MQWater,
    &MQSpirit,
    &MQShadow,
    &MQBotW,
    &MQIceCavern,
    &MQGTG,
    &MQCastle,
  };
  std::vector<Option *> dungeonOptions = {
    &MQDeku,
    &MQDodongo,
    &MQJabu,
    &MQForest,
    &MQFire,
    &MQWater,
    &MQSpirit,
    &MQShadow,
    &MQBotW,
    &MQIceCavern,
    &MQGTG,
    &MQCastle,
  };

  //Shuffle Settings
  Option RandomizeShuffle       = Option::Bool("Randomize Settings",     {"No","Yes"}, OptionCategory::Toggle);
  Option ShuffleRewards         = Option::U8  ("Shuffle Dungeon Rewards",{"End of dungeons", "Any dungeon", "Overworld", "Anywhere"});
  Option LinksPocketItem        = Option::U8  ("Link's Pocket",          {"Dungeon Reward", "Advancement", "Anything", "Nothing"});
  Option ShuffleSongs           = Option::U8  ("Shuffle Songs",          {"Song locations", "Dungeon rewards", "Anywhere"});
  Option Shopsanity             = Option::U8  ("Shopsanity",             {"Off","0 Items","1 Item","2 Items","3 Items","4 Items","Random"});
  Option ShopsanityPrices       = Option::U8  ("Shopsanity Prices",      {"Balanced", "Starting Wallet", "Adult Wallet", "Giant's Wallet", "Tycoon's Wallet" });
  Option ShopsanityPricesAffordable   = Option::Bool("Affordable Prices",                {"Off", "On"});
  Option Tokensanity            = Option::U8  ("Tokensanity",            {"Off", "Dungeons", "Overworld", "All Tokens"});
  Option Scrubsanity            = Option::U8  ("Scrub Shuffle",          {"Off", "Affordable", "Expensive", "Random Prices"});
  Option ShuffleCows            = Option::Bool("Shuffle Cows",           {"Off", "On"});
  Option ShuffleKokiriSword     = Option::Bool("Shuffle Kokiri Sword",   {"Off", "On"});
  Option ShuffleOcarinas        = Option::Bool("Shuffle Ocarinas",       {"Off", "On"});
  Option ShuffleWeirdEgg        = Option::Bool("Shuffle Weird Egg",      {"Off", "On"});
  Option ShuffleGerudoToken     = Option::Bool("Shuffle Gerudo Card",    {"Off", "On"});
  Option ShuffleMagicBeans      = Option::Bool("Shuffle Magic Beans",    {"Off", "On"});
  Option ShuffleMerchants       = Option::U8  ("Shuffle Merchants",      {"Off", "On (No Hints)", "On (With Hints)"});
  Option ShuffleFrogSongRupees  = Option::Bool("Shuffle Frog Song Rupees",{"Off", "On"});
  Option ShuffleAdultTradeQuest = Option::Bool("Shuffle Adult Trade",    {"Off", "On"});
  Option ShuffleChestMinigame   = Option::U8  ("Shuffle Chest Minigame", {"Off", "On (Separate)", "On (Pack)"});
  Option Shuffle100GSReward     = Option::Bool("Shuffle 100 GS Reward",  {"Off", "On"});
  std::vector<Option *> shuffleOptions = {
    &RandomizeShuffle,
    &ShuffleRewards,
    &LinksPocketItem,
    &ShuffleSongs,
    &Shopsanity,
    &ShopsanityPrices,
    &ShopsanityPricesAffordable,
    &Tokensanity,
    &Scrubsanity,
    &ShuffleCows,
    &ShuffleKokiriSword,
    &ShuffleOcarinas,
    &ShuffleWeirdEgg,
    &ShuffleGerudoToken,
    &ShuffleMagicBeans,
    &ShuffleMerchants,
    &ShuffleFrogSongRupees,
    &ShuffleAdultTradeQuest,
    &ShuffleChestMinigame,
    &Shuffle100GSReward,
  };

  //Shuffle Dungeon Items
  Option RandomizeDungeon    = Option::Bool("Randomize Settings",        {"No","Yes"}, OptionCategory::Toggle);
  Option MapsAndCompasses    = Option::U8  ("Maps/Compasses",            {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, MAPSANDCOMPASSES_OWN_DUNGEON);
  Option Keysanity           = Option::U8  ("Small Keys",                {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, KEYSANITY_OWN_DUNGEON);
  Option GerudoKeys          = Option::U8  ("Gerudo Fortress Keys",      {"Vanilla", "Any Dungeon", "Overworld", "Anywhere"});
  Option BossKeysanity       = Option::U8  ("Boss Keys",                 {"Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere"}, OptionCategory::Setting, BOSSKEYSANITY_OWN_DUNGEON);
  Option GanonsBossKey       = Option::U8  ("Ganon's Boss Key",          {"Vanilla", "Own dungeon", "Start with", "Any Dungeon", "Overworld", "Anywhere", "LACS-Vanilla", "LACS-Stones", "LACS-Medallions", "LACS-Rewards", "LACS-Dungeons", "LACS-Tokens", "100 GS Reward", "Triforce Hunt"}, OptionCategory::Setting, GANONSBOSSKEY_VANILLA);
  uint8_t LACSCondition           = 0;
  Option LACSStoneCount      = Option::U8  ("Stone Count",             {NumOpts(0, 4)},   OptionCategory::Setting, 1, true);
  Option LACSMedallionCount  = Option::U8  ("Medallion Count",         {NumOpts(0, 7)},   OptionCategory::Setting, 1, true);
  Option LACSRewardCount     = Option::U8  ("Reward Count",            {NumOpts(0, 10)},  OptionCategory::Setting, 1, true);
  Option LACSDungeonCount    = Option::U8  ("Dungeon Count",           {NumOpts(0, 9)},   OptionCategory::Setting, 1, true);
  Option LACSTokenCount      = Option::U8  ("Token Count",             {NumOpts(0, 100)}, OptionCategory::Setting, 1, true);
  Option LACSRewardOptions   = Option::U8  ("LACS Reward Options",     {"Standard Reward", "Greg as Reward", "Greg as Wildcard"});
  Option KeyRings            = Option::U8  ("Key Rings",               {"Off", "Random", "Count", "Selection"});
  Option KeyRingsRandomCount = Option::U8  ("Keyring Dungeon Count",   {NumOpts(0, 9)}, OptionCategory::Setting, 1);
  Option RingFortress        = Option::Bool("Gerudo Fortress",         {"Off", "On"},   OptionCategory::Setting);
  Option RingForest          = Option::Bool("Forest Temple",           {"Off", "On"},   OptionCategory::Setting);
  Option RingFire            = Option::Bool("Fire Temple",             {"Off", "On"},   OptionCategory::Setting);
  Option RingWater           = Option::Bool("Water Temple",            {"Off", "On"},   OptionCategory::Setting);
  Option RingSpirit          = Option::Bool("Spirit Temple",           {"Off", "On"},   OptionCategory::Setting);
  Option RingShadow          = Option::Bool("Shadow Temple",           {"Off", "On"},   OptionCategory::Setting);
  Option RingWell            = Option::Bool("Bottom of the Well",      {"Off", "On"},   OptionCategory::Setting);
  Option RingGtg             = Option::Bool("GTG",                     {"Off", "On"},   OptionCategory::Setting);
  Option RingCastle          = Option::Bool("Ganon's Castle",          {"Off", "On"},   OptionCategory::Setting);

  std::vector<Option *> shuffleDungeonItemOptions = {
    &RandomizeDungeon,
    &MapsAndCompasses,
    &Keysanity,
    &GerudoKeys,
    &BossKeysanity,
    &GanonsBossKey,
    &LACSStoneCount,
    &LACSMedallionCount,
    &LACSRewardCount,
    &LACSDungeonCount,
    &LACSTokenCount,
    &LACSRewardOptions,
    &KeyRings,
    &KeyRingsRandomCount,
    &RingFortress,
    &RingForest,
    &RingFire,
    &RingWater,
    &RingSpirit,
    &RingShadow,
    &RingWell,
    &RingGtg,
    &RingCastle,
  };
  std::vector<Option *> keyRingOptions = {
    &RingForest,
    &RingFire,
    &RingWater,
    &RingSpirit,
    &RingShadow,
    &RingWell,
    &RingGtg,
    &RingCastle,
  };

  //Timesaver Settings
  Option SkipChildStealth    = Option::Bool("Skip Child Stealth",     {"Don't Skip", "Skip"}, OptionCategory::Setting, SKIP);
  Option SkipTowerEscape     = Option::Bool("Skip Tower Escape",      {"Don't Skip", "Skip"}, OptionCategory::Setting, SKIP);
  Option SkipEponaRace       = Option::Bool("Skip Epona Race",        {"Don't Skip", "Skip"});
  Option SkipMinigamePhases  = Option::Bool("Minigames repetitions",  {"Don't Skip", "Skip"});
  Option FreeScarecrow       = Option::Bool("Skip Scarecrow's Song",  {"Off", "On"});
  Option FourPoesCutscene    = Option::Bool("Four Poes Cutscene",     {"Don't Skip", "Skip"}, OptionCategory::Setting, SKIP);
  Option LakeHyliaOwl        = Option::Bool("Lake Hylia Owl",         {"Don't Skip", "Skip"}, OptionCategory::Setting, SKIP);
  Option BigPoeTargetCount   = Option::U8  ("Big Poe Target Count",   {NumOpts(1, 10)});
  Option NumRequiredCuccos   = Option::U8  ("Cuccos to return",       {NumOpts(0, 7)});
  Option KingZoraSpeed       = Option::U8  ("King Zora Speed",        {"Fast", "Vanilla", "Random"});
  Option CompleteMaskQuest   = Option::Bool("Complete Mask Quest",    {"Off", "On"});
  Option EnableGlitchCutscenes = Option::Bool("Enable Glitch-Useful Cutscenes", {"Off", "On"});
  Option QuickText           = Option::U8  ("Quick Text",             {"0: Vanilla", "1: Skippable", "2: Instant", "3: Turbo"}, OptionCategory::Setting, QUICKTEXT_INSTANT);
  Option SkipSongReplays     = Option::U8  ("Skip Song Replays",    {"Don't Skip", "Skip (No SFX)", "Skip (Keep SFX)"});
  Option KeepFWWarpPoint     = Option::Bool("Keep FW Warp Point",     {"Off", "On"});
  Option FastBunnyHood       = Option::Bool("Fast Bunny Hood",        {"Off", "On"});
  std::vector<Option *> timesaverOptions = {
    &SkipChildStealth,
    &SkipTowerEscape,
    &SkipEponaRace,
    &SkipMinigamePhases,
    &FreeScarecrow,
    &FourPoesCutscene,
    &LakeHyliaOwl,
    &BigPoeTargetCount,
    &NumRequiredCuccos,
    &KingZoraSpeed,
    &CompleteMaskQuest,
    &EnableGlitchCutscenes,
    &QuickText,
    &SkipSongReplays,
    &KeepFWWarpPoint,
    &FastBunnyHood,
  };

  //Misc Settings
  Option GossipStoneHints    = Option::U8  ("Gossip Stone Hints",     {"No Hints", "Need Nothing", "Mask of Truth", "Stone of Agony"}, OptionCategory::Setting, HINTS_NEED_NOTHING);
  Option ClearerHints        = Option::U8  ("Hint Clarity",           {"Obscure", "Ambiguous", "Clear"});
  Option HintDistribution    = Option::U8  ("Hint Distribution",      {"Useless", "Balanced", "Strong", "Very Strong"}, OptionCategory::Setting, 1); // Balanced
  Option AltarHintText       = Option::Bool("ToT Altar Hint",         {"Off", "On"}, OptionCategory::Setting, 1);
  Option LightArrowHintText  = Option::Bool("Light Arrow Hint",       {"Off", "On"}, OptionCategory::Setting, 1);
  Option DampeHintText       = Option::Bool("Dampe's Diary Hint",     {"Off", "On"}, OptionCategory::Setting, 0);
  Option GregHintText        = Option::Bool("Greg the Rupee Hint",    {"Off", "On"}, OptionCategory::Setting, 0);
  Option SariaHintText       = Option::Bool("Saria's Hint",           {"Off", "On"}, OptionCategory::Setting, 0);
  Option FrogsHintText       = Option::Bool("Frog Ocarina Game Hint", {"Off", "On"}, OptionCategory::Setting, 0);
  Option WarpSongHints       = Option::Bool("Warp Song Hints",        {"Off", "On"}, OptionCategory::Setting, 0);
  Option Kak10GSHintText     = Option::Bool("10 GS Hint",             {"Off", "On"}, OptionCategory::Setting, 0);
  Option Kak20GSHintText     = Option::Bool("20 GS Hint",             {"Off", "On"}, OptionCategory::Setting, 0);
  Option Kak30GSHintText     = Option::Bool("30 GS Hint",             {"Off", "On"}, OptionCategory::Setting, 0);
  Option Kak40GSHintText     = Option::Bool("40 GS Hint",             {"Off", "On"}, OptionCategory::Setting, 0);
  Option Kak50GSHintText     = Option::Bool("50 GS Hint",             {"Off", "On"}, OptionCategory::Setting, 0);
  Option ScrubHintText       = Option::Bool("Scrub Hint Text",        {"Off", "On"}, OptionCategory::Setting, 0);
  Option CompassesShowReward = Option::U8  ("Compasses Show Rewards", {"No", "Yes"}, OptionCategory::Setting, 1);
  Option CompassesShowWotH   = Option::U8  ("Compasses Show WotH",    {"No", "Yes"}, OptionCategory::Setting, 1);
  Option MapsShowDungeonMode = Option::U8  ("Maps Show Dungeon Modes",{"No", "Yes"}, OptionCategory::Setting, 1);
  Option DamageMultiplier    = Option::U8  ("Damage Multiplier",      {"x1/2", "x1", "x2", "x4", "x8", "x16", "OHKO"}, OptionCategory::Setting, DAMAGEMULTIPLIER_DEFAULT);
  Option StartingTime        = Option::U8  ("Starting Time",          {"Day", "Night"});
  Option ChestAnimations     = Option::Bool("Chest Animations",       {"Always Fast", "Match Contents"});
  Option ChestSize           = Option::Bool("Chest Size and Color",   {"Vanilla", "Match Contents"});
  Option GenerateSpoilerLog  = Option::Bool("Generate Spoiler Log",   {"No", "Yes"}, OptionCategory::Setting, 1); // On
  Option IngameSpoilers      = Option::Bool("Ingame Spoilers",        {"Hide", "Show"});
  Option RandomTrapDmg       = Option::U8  ("Random Trap Damage",     {"Off", "Basic", "Advanced"}, OptionCategory::Setting, 1); // Basic
  Option BlueFireArrows      = Option::Bool("Blue Fire Arrows",       {"Off", "On"});
  Option SunlightArrows      = Option::Bool("Sunlight Arrows",        {"Off", "On"});

  bool HasNightStart         = false;
  std::vector<Option *> miscOptions = {
    &GossipStoneHints,
    &ClearerHints,
    &HintDistribution,
    &AltarHintText,
    &LightArrowHintText,
    &DampeHintText,
    &GregHintText,
    &SariaHintText,
    &FrogsHintText,
    &WarpSongHints,
    &Kak10GSHintText,
    &Kak20GSHintText,
    &Kak30GSHintText,
    &Kak40GSHintText,
    &Kak50GSHintText,
    &ScrubHintText,
    &CompassesShowReward,
    &CompassesShowWotH,
    &MapsShowDungeonMode,
    &DamageMultiplier,
    &StartingTime,
    &ChestAnimations,
    &ChestSize,
    &GenerateSpoilerLog,
    &IngameSpoilers,
    &RandomTrapDmg,
    &BlueFireArrows,
    &SunlightArrows
  };

  //Item Usability Settings
  Option FaroresWindAnywhere = Option::Bool("Farore's Wind Anywhere", {"Disabled", "Enabled"});
  Option AgeItemsToggle      = Option::U8  ("Lift Age Restrictions",  {"All Disabled",  "All Enabled", "Choose"});
  Option StickAsAdult        = Option::Bool("Adult Deku Stick",     {"Disabled", "Enabled"});
  Option BoomerangAsAdult    = Option::Bool("Adult Boomerang",      {"Disabled", "Enabled"});
  Option HammerAsChild       = Option::Bool("Child Hammer",         {"Disabled", "Enabled"});
  Option SlingshotAsAdult    = Option::Bool("Adult Slingshot",      {"Disabled", "Enabled"});
  Option BowAsChild          = Option::Bool("Child Bow",            {"Disabled", "Enabled"});
  Option HookshotAsChild     = Option::Bool("Child Hookshot",       {"Disabled", "Enabled"});
  Option IronBootsAsChild    = Option::Bool("Child Iron Boots",     {"Disabled", "Enabled"});
  Option HoverBootsAsChild   = Option::Bool("Child Hover Boots",    {"Disabled", "Enabled"});
  Option MasksAsAdult        = Option::Bool("Adult Masks",          {"Disabled", "Enabled"});
  Option KokiriSwordAsAdult  = Option::Bool("Adult Kokiri Sword",   {"Disabled", "Enabled"});
  Option MasterSwordAsChild  = Option::Bool("Child Master Sword",   {"Disabled", "Enabled"});
  Option BiggoronSwordAsChild= Option::Bool("Child Biggoron Sword", {"Disabled", "Enabled"});
  Option DekuShieldAsAdult   = Option::Bool("Adult Deku Shield",    {"Disabled", "Enabled"});
  Option MirrorShieldAsChild = Option::Bool("Child Mirror Shield",  {"Disabled", "Enabled"});
  Option GoronTunicAsChild   = Option::Bool("Child Goron Tunic",    {"Disabled", "Enabled"});
  Option ZoraTunicAsChild    = Option::Bool("Child Zora Tunic",     {"Disabled", "Enabled"});
  Option GkDurability        = Option::U8  ("GK Durability",          {"Vanilla", "Random Risk", "Random Safe"});
  std::vector<Option *> itemUsabilityOptions = {
    &FaroresWindAnywhere,
    &AgeItemsToggle,
    &StickAsAdult,
    &BoomerangAsAdult,
    &HammerAsChild,
    &SlingshotAsAdult,
    &BowAsChild,
    &HookshotAsChild,
    &IronBootsAsChild,
    &HoverBootsAsChild,
    &MasksAsAdult,
    &KokiriSwordAsAdult,
    &MasterSwordAsChild,
    &BiggoronSwordAsChild,
    &DekuShieldAsAdult,
    &MirrorShieldAsChild,
    &GoronTunicAsChild,
    &ZoraTunicAsChild,
    &GkDurability,
  };

  //Item Pool Settings
  Option ItemPoolValue         = Option::U8  ("Item Pool",             {"Plentiful", "Balanced", "Scarce", "Minimal"},    OptionCategory::Setting, ITEMPOOL_BALANCED);
  Option IceTrapValue          = Option::U8  ("Ice Traps",             {"Off", "Normal", "Extra", "Mayhem", "Onslaught"}, OptionCategory::Setting, ICETRAPS_NORMAL);
  Option RemoveDoubleDefense   = Option::Bool("Remove Double Defense", {"No", "Yes"});
  Option ProgressiveGoronSword = Option::Bool("Prog Goron Sword",      {"Disabled", "Enabled"});
  std::vector<Option *> itemPoolOptions = {
    &ItemPoolValue,
    &IceTrapValue,
    &RemoveDoubleDefense,
    &ProgressiveGoronSword,
  };

  //Excluded Locations (Individual definitions made in ItemLocation class)
  std::vector<std::vector<Option *>> excludeLocationsOptionsVector(SPOILER_COLLECTION_GROUP_COUNT);
  Menu excludeKokiriForest          = Menu::SubMenu("Kokiri Forest",           &excludeLocationsOptionsVector[GROUP_KOKIRI_FOREST], false);
  Menu excludeLostWoods             = Menu::SubMenu("Lost Woods",              &excludeLocationsOptionsVector[GROUP_LOST_WOODS], false);
  Menu excludeDekuTree              = Menu::SubMenu("Deku Tree",               &excludeLocationsOptionsVector[GROUP_DUNGEON_DEKU_TREE], false);
  Menu excludeForestTemple          = Menu::SubMenu("Forest Temple",           &excludeLocationsOptionsVector[GROUP_DUNGEON_FOREST_TEMPLE], false);
  Menu excludeKakariko              = Menu::SubMenu("Kakariko Village",        &excludeLocationsOptionsVector[GROUP_KAKARIKO], false);
  Menu excludeBottomWell            = Menu::SubMenu("Bottom of the Well",      &excludeLocationsOptionsVector[GROUP_DUNGEON_BOTTOM_OF_THE_WELL], false);
  Menu excludeShadowTemple          = Menu::SubMenu("Shadow Temple",           &excludeLocationsOptionsVector[GROUP_DUNGEON_SHADOW_TEMPLE], false);
  Menu excludeDeathMountain         = Menu::SubMenu("Death Mountain",          &excludeLocationsOptionsVector[GROUP_DEATH_MOUNTAIN], false);
  Menu excludeGoronCity             = Menu::SubMenu("Goron City",              &excludeLocationsOptionsVector[GROUP_GORON_CITY], false);
  Menu excludeDodongosCavern        = Menu::SubMenu("Dodongo's Cavern",        &excludeLocationsOptionsVector[GROUP_DUNGEON_DODONGOS_CAVERN], false);
  Menu excludeFireTemple            = Menu::SubMenu("Fire Temple",             &excludeLocationsOptionsVector[GROUP_DUNGEON_FIRE_TEMPLE], false);
  Menu excludeZorasRiver            = Menu::SubMenu("Zora's River",            &excludeLocationsOptionsVector[GROUP_ZORAS_RIVER], false);
  Menu excludeZorasDomain           = Menu::SubMenu("Zora's Domain",           &excludeLocationsOptionsVector[GROUP_ZORAS_DOMAIN], false);
  Menu excludeJabuJabu              = Menu::SubMenu("Jabu Jabu's Belly",       &excludeLocationsOptionsVector[GROUP_DUNGEON_JABUJABUS_BELLY], false);
  Menu excludeIceCavern             = Menu::SubMenu("Ice Cavern",              &excludeLocationsOptionsVector[GROUP_DUNGEON_ICE_CAVERN], false);
  Menu excludeHyruleField           = Menu::SubMenu("Hyrule Field",            &excludeLocationsOptionsVector[GROUP_HYRULE_FIELD], false);
  Menu excludeLonLonRanch           = Menu::SubMenu("Lon Lon Ranch",           &excludeLocationsOptionsVector[GROUP_LON_LON_RANCH], false);
  Menu excludeLakeHylia             = Menu::SubMenu("Lake Hylia",              &excludeLocationsOptionsVector[GROUP_LAKE_HYLIA], false);
  Menu excludeWaterTemple           = Menu::SubMenu("Water Temple",            &excludeLocationsOptionsVector[GROUP_DUNGEON_WATER_TEMPLE], false);
  Menu excludeGerudoValley          = Menu::SubMenu("Gerudo Valley",           &excludeLocationsOptionsVector[GROUP_GERUDO_VALLEY], false);
  Menu excludeGerudoTrainingGrounds = Menu::SubMenu("Gerudo Training Grounds", &excludeLocationsOptionsVector[GROUP_GERUDO_TRAINING_GROUND], false);
  Menu excludeSpiritTemple          = Menu::SubMenu("Spirit Temple",           &excludeLocationsOptionsVector[GROUP_DUNGEON_SPIRIT_TEMPLE], false);
  Menu excludeHyruleCastle          = Menu::SubMenu("Hyrule Castle",           &excludeLocationsOptionsVector[GROUP_HYRULE_CASTLE], false);
  Menu excludeGanonsCastle          = Menu::SubMenu("Ganon's Castle",          &excludeLocationsOptionsVector[GROUP_DUNGEON_GANONS_CASTLE], false);
  std::vector<Menu *> excludeLocationsMenus = {
    &excludeKokiriForest,
    &excludeLostWoods,
    &excludeDekuTree,
    &excludeForestTemple,
    &excludeKakariko,
    &excludeBottomWell,
    &excludeShadowTemple,
    &excludeDeathMountain,
    &excludeGoronCity,
    &excludeDodongosCavern,
    &excludeFireTemple,
    &excludeZorasRiver,
    &excludeZorasDomain,
    &excludeJabuJabu,
    &excludeIceCavern,
    &excludeHyruleField,
    &excludeLonLonRanch,
    &excludeLakeHylia,
    &excludeWaterTemple,
    &excludeGerudoValley,
    &excludeGerudoTrainingGrounds,
    &excludeSpiritTemple,
    &excludeHyruleCastle,
    &excludeGanonsCastle,
  };

  //Starting Inventory submenus and menus
  std::vector<std::string> bottleOptions = {"Off", "Empty Bottle", "Red Potion", "Green Potion", "Blue Potion", "Fairy", "Fish", "Milk", "Blue Fire", "Bugs", "Big Poe", "Half Milk", "Poe"};
  Option StartingStickCapacity    = Option::U8  ("Deku Stick Capacity",  {NumOpts(10, 30, 10, {}, " Deku Sticks")});
  Option StartingNutCapacity      = Option::U8  ("Deku Nut Capacity",    {NumOpts(20, 40, 10, {}, " Deku Nuts")});
  Option StartingSlingshot        = Option::U8  ("Slingshot",            {"Off",             "Slingshot (30)",   "Slingshot (40)",    "Slingshot (50)"});
  Option StartingOcarina          = Option::U8  ("Start with Fairy Ocarina",              {"Off",             "Fairy Ocarina",    "Ocarina of Time"});
  Option StartingBombBag          = Option::U8  ("Bombs",                {"Off",             "Bomb Bag (20)",    "Bomb Bag (30)",     "Bomb Bag (40)"});
  Option StartingBombchus         = Option::U8  ("Bombchus",             {"Off",             "20 Bombchus",      "50 Bombchus"});
  Option StartingBoomerang        = Option::U8  ("Boomerang",            {"Off",             "On"});
  Option StartingHookshot         = Option::U8  ("Hookshot",             {"Off",             "Hookshot",         "Longshot"});
  Option StartingBow              = Option::U8  ("Bow",                  {"Off",             "Bow (30)",         "Bow (40)",          "Bow (50)"});
  Option StartingFireArrows       = Option::U8  ("Fire Arrow",           {"Off",             "On"});
  Option StartingIceArrows        = Option::U8  ("Ice Arrow",            {"Off",             "On"});
  Option StartingLightArrows      = Option::U8  ("Light Arrow",          {"Off",             "On"});
  Option StartingMegatonHammer    = Option::U8  ("Megaton Hammer",       {"Off",             "On"});
  Option StartingIronBoots        = Option::U8  ("Iron Boots",           {"Off",             "On"});
  Option StartingHoverBoots       = Option::U8  ("Hover Boots",          {"Off",             "On"});
  Option StartingLensOfTruth      = Option::U8  ("Lens of Truth",        {"Off",             "On"});
  Option StartingDinsFire         = Option::U8  ("Din's Fire",           {"Off",             "On"});
  Option StartingFaroresWind      = Option::U8  ("Farore's Wind",        {"Off",             "On"});
  Option StartingNayrusLove       = Option::U8  ("Nayru's Love",         {"Off",             "On"});
  Option StartingMagicBean        = Option::U8  ("Magic Beans",          {"Off",             "On"});
  Option StartingBottle1          = Option::U8  ("Bottle 1",             bottleOptions);
  Option StartingBottle2          = Option::U8  ("Bottle 2",             bottleOptions);
  Option StartingBottle3          = Option::U8  ("Bottle 3",             bottleOptions);
  Option StartingBottle4          = Option::U8  ("Bottle 4",             bottleOptions);
  Option StartingRutoBottle       = Option::U8  ("Ruto's Letter",        {"Off",             "On"});
  std::vector<Option *> startingItemsOptions = {
    &StartingStickCapacity,
    &StartingNutCapacity,
    &StartingSlingshot,
    &StartingOcarina,
    &StartingBombBag,
    &StartingBombchus,
    &StartingBoomerang,
    &StartingHookshot,
    &StartingBow,
    &StartingFireArrows,
    &StartingIceArrows,
    &StartingLightArrows,
    &StartingMegatonHammer,
    &StartingIronBoots,
    &StartingHoverBoots,
    &StartingLensOfTruth,
    &StartingDinsFire,
    &StartingFaroresWind,
    &StartingNayrusLove,
    &StartingMagicBean,
    &StartingBottle1,
    &StartingBottle2,
    &StartingBottle3,
    &StartingBottle4,
    &StartingRutoBottle,
  };

  Option StartingZeldasLullaby    = Option::U8  ("Start with Zelda's Lullaby",      {"Off", "On"});
  Option StartingEponasSong       = Option::U8  ("Start with Epona's Song",         {"Off", "On"});
  Option StartingSariasSong       = Option::U8  ("Start with Saria's Song",         {"Off", "On"});
  Option StartingSunsSong         = Option::U8  ("Start with Sun's Song",           {"Off", "On"});
  Option StartingSongOfTime       = Option::U8  ("Start with Song of Time",         {"Off", "On"});
  Option StartingSongOfStorms     = Option::U8  ("Start with Song of Storms",       {"Off", "On"});
  Option StartingMinuetOfForest   = Option::U8  ("Start with Minuet of Forest",     {"Off", "On"});
  Option StartingBoleroOfFire     = Option::U8  ("Start with Bolero of Fire",       {"Off", "On"});
  Option StartingSerenadeOfWater  = Option::U8  ("Start with Serenade of Water",    {"Off", "On"});
  Option StartingRequiemOfSpirit  = Option::U8  ("Start with Requiem of Spirit",    {"Off", "On"});
  Option StartingNocturneOfShadow = Option::U8  ("Start with Nocturne of Shadow",   {"Off", "On"});
  Option StartingPreludeOfLight   = Option::U8  ("Start with Prelude of Light",     {"Off", "On"});
  std::vector<Option *> startingSongsOptions = {
    &StartingZeldasLullaby,
    &StartingEponasSong,
    &StartingSariasSong,
    &StartingSunsSong,
    &StartingSongOfTime,
    &StartingSongOfStorms,
    &StartingMinuetOfForest,
    &StartingBoleroOfFire,
    &StartingSerenadeOfWater,
    &StartingRequiemOfSpirit,
    &StartingNocturneOfShadow,
    &StartingPreludeOfLight,
  };

  Option StartingKokiriSword      = Option::U8  ("Start with Kokiri Sword",         {"Off",             "On"});
  Option StartingBiggoronSword    = Option::U8  ("Biggoron Sword",       {"Off",             "Giant's Knife",    "Biggoron Sword"});
  Option StartingDekuShield       = Option::U8  ("Start with Deku Shield",          {"Off",             "On"});
  Option StartingHylianShield     = Option::U8  ("Hylian Shield",        {"Off",             "On"});
  Option StartingMirrorShield     = Option::U8  ("Mirror Shield",        {"Off",             "On"});
  Option StartingGoronTunic       = Option::U8  ("Goron Tunic",          {"Off",             "On"});
  Option StartingZoraTunic        = Option::U8  ("Zora Tunic",           {"Off",             "On"});
  Option StartingStrength         = Option::U8  ("Strength Upgrade",     {"Off",             "Goron Bracelet",   "Silver Gauntlet",  "Golden Gauntlet"});
  Option StartingScale            = Option::U8  ("Scale Upgrade",        {"Off",             "Silver Scale"  ,   "Golden Scale"});
  Option StartingWallet           = Option::U8  ("Wallet Upgrade",       {"Off",             "Adult's Wallet",   "Giant's Wallet" ,  "Tycoon's Wallet"});
  Option StartingShardOfAgony     = Option::U8  ("Stone of Agony",       {"Off",             "On"});
  Option StartingHearts           = Option::U8  ("Hearts",               {NumOpts(1, 20)}, OptionCategory::Setting, 2); // Default 3 hearts
  Option StartingMagicMeter       = Option::U8  ("Magic Meter",          {"Off",             "Single Magic",     "Double Magic"});
  Option StartingDoubleDefense    = Option::U8  ("Double Defense",       {"Off",             "On"});
  std::vector<Option *> startingEquipmentOptions = {
    &StartingKokiriSword,
    &StartingBiggoronSword,
    &StartingDekuShield,
    &StartingHylianShield,
    &StartingMirrorShield,
    &StartingGoronTunic,
    &StartingZoraTunic,
    &StartingStrength,
    &StartingScale,
    &StartingWallet,
    &StartingShardOfAgony,
    &StartingHearts,
    &StartingMagicMeter,
    &StartingDoubleDefense,
  };

  Option StartingKokiriEmerald    = Option::U8  ("Kokiri's Emerald",     {"Off", "On"});
  Option StartingGoronRuby        = Option::U8  ("Goron's Ruby",         {"Off", "On"});
  Option StartingZoraSapphire     = Option::U8  ("Zora's Sapphire",      {"Off", "On"});
  Option StartingLightMedallion   = Option::U8  ("Light Medallion",      {"Off", "On"});
  Option StartingForestMedallion  = Option::U8  ("Forest Medallion",     {"Off", "On"});
  Option StartingFireMedallion    = Option::U8  ("Fire Medallion",       {"Off", "On"});
  Option StartingWaterMedallion   = Option::U8  ("Water Medallion",      {"Off", "On"});
  Option StartingSpiritMedallion  = Option::U8  ("Spirit Medallion",     {"Off", "On"});
  Option StartingShadowMedallion  = Option::U8  ("Shadow Medallion",     {"Off", "On"});
  std::vector<Option *> startingStonesMedallionsOptions = {
    &StartingKokiriEmerald,
    &StartingGoronRuby,
    &StartingZoraSapphire,
    &StartingLightMedallion,
    &StartingForestMedallion,
    &StartingFireMedallion,
    &StartingWaterMedallion,
    &StartingSpiritMedallion,
    &StartingShadowMedallion,
  };

  Option StartingConsumables      = Option::Bool("Start with Consumables", {"No",               "Yes"});
  Option StartingMaxRupees        = Option::Bool("Start with Max Rupees",  {"No",               "Yes"});
  Option StartingSkulltulaToken   = Option::U8  ("Gold Skulltula Tokens",  {NumOpts(0, 100)});
  std::vector<Option *> startingOthersOptions = {
    &StartingConsumables,
    &StartingMaxRupees,
    &StartingSkulltulaToken,
  };

  Menu startingItems            = Menu::SubMenu("Items",                &startingItemsOptions, false);
  Menu startingSongs            = Menu::SubMenu("Ocarina Songs",        &startingSongsOptions, false);
  Menu startingEquipment        = Menu::SubMenu("Equipment & Upgrades", &startingEquipmentOptions, false);
  Menu startingStonesMedallions = Menu::SubMenu("Stones & Medallions",  &startingStonesMedallionsOptions, false);
  Menu startingOthers           = Menu::SubMenu("Other",                &startingOthersOptions, false);
  std::vector<Menu *> startingInventoryOptions = {
    &startingItems,
    &startingSongs,
    &startingEquipment,
    &startingStonesMedallions,
    &startingOthers,
  };
  Option Logic              = Option::U8  ("Logic",                   {"Glitchless", "Glitched", "No Logic", "Vanilla"});
  Option LocationsReachable = Option::Bool("All Locations Reachable", {"Off", "On"}, OptionCategory::Setting, 1); //All Locations Reachable On
  Option NightGSExpectSuns  = Option::Bool("Night GSs Expect Sun's",  {"Off", "On"});
  std::vector<Option *> logicOptions = {
    &Logic,
    &LocationsReachable,
    &NightGSExpectSuns,
  };

  //Function to make defining logic tricks easier to read
  Option LogicTrick(std::string setting) {
    return Option::Bool(setting, {"Disabled", "Enabled"});
  }

  //Detailed Logic Tricks                               ---------------------
  Option LogicVisibleCollision                    = LogicTrick(std::to_string(RT_VISIBLE_COLLISION));
  Option LogicGrottosWithoutAgony                 = LogicTrick(std::to_string(RT_GROTTOS_WITHOUT_AGONY));
  Option LogicFewerTunicRequirements              = LogicTrick(std::to_string(RT_FEWER_TUNIC_REQUIREMENTS));
  Option LogicRustedSwitches                      = LogicTrick(std::to_string(RT_RUSTED_SWITCHES));
  Option LogicFlamingChests                       = LogicTrick(std::to_string(RT_FLAMING_CHESTS));
  Option LogicBunnyHoodJump                       = LogicTrick(std::to_string(RT_BUNNY_HOOD_JUMPS));
  Option LogicDamageBoost                         = LogicTrick(std::to_string(RT_DAMAGE_BOOST_SIMPLE));
  Option LogicHoverBoost                          = LogicTrick(std::to_string(RT_HOVER_BOOST_SIMPLE));
  Option LogicAdultKokiriGS                       = LogicTrick(std::to_string(RT_KF_ADULT_GS));
  Option LogicLostWoodsBridge                     = LogicTrick(std::to_string(RT_LW_BRIDGE));
  Option LogicMidoBackflip                        = LogicTrick(std::to_string(RT_LW_MIDO_BACKFLIP));
  Option LogicLostWoodsGSBean                     = LogicTrick(std::to_string(RT_LW_GS_BEAN));
  Option LogicCastleStormsGS                      = LogicTrick(std::to_string(RT_HC_STORMS_GS));
  Option LogicManOnRoof                           = LogicTrick(std::to_string(RT_KAK_MAN_ON_ROOF));
  Option LogicKakarikoTowerGS                     = LogicTrick(std::to_string(RT_KAK_TOWER_GS));
  Option LogicAdultWindmillPoH                    = LogicTrick(std::to_string(RT_KAK_ADULT_WINDMILL_POH));
  Option LogicChildWindmillPoH                    = LogicTrick(std::to_string(RT_KAK_CHILD_WINDMILL_POH));
  Option LogicKakarikoRooftopGS                   = LogicTrick(std::to_string(RT_KAK_ROOFTOP_GS));
  Option LogicGraveyardPoH                        = LogicTrick(std::to_string(RT_GY_POH));
  Option LogicChildDampeRacePoH                   = LogicTrick(std::to_string(RT_GY_CHILD_DAMPE_RACE_POH));
  Option LogicShadowFireArrowEntry                = LogicTrick(std::to_string(RT_GY_SHADOW_FIRE_ARROWS));
  Option LogicDMTSoilGS                           = LogicTrick(std::to_string(RT_DMT_SOIL_GS));
  Option LogicDMTBombable                         = LogicTrick(std::to_string(RT_DMT_BOMBABLE));
  Option LogicDMTGSLowerHookshot                  = LogicTrick(std::to_string(RT_DMT_HOOKSHOT_LOWER_GS));
  Option LogicDMTGSLowerHovers                    = LogicTrick(std::to_string(RT_DMT_HOVERS_LOWER_GS));
  Option LogicDMTGSLowerBean                      = LogicTrick(std::to_string(RT_DMT_BEAN_LOWER_GS));
  Option LogicDMTGSLowerJS                        = LogicTrick(std::to_string(RT_DMT_JS_LOWER_GS));
  Option LogicDMTClimbHovers                      = LogicTrick(std::to_string(RT_DMT_CLIMB_HOVERS));
  Option LogicDMTGSUpper                          = LogicTrick(std::to_string(RT_DMT_UPPER_GS));
  Option LogicBiggoronBolero                      = LogicTrick(std::to_string(RT_DMT_BOLERO_BIGGORON));
  Option LogicGoronCityPot                        = LogicTrick(std::to_string(RT_GC_POT));
  Option LogicGoronCityPotWithStrength            = LogicTrick(std::to_string(RT_GC_POT_STRENGTH));
  Option LogicChildRollingWithStrength            = LogicTrick(std::to_string(RT_GC_ROLLING_STRENGTH));
  Option LogicGoronCityLeftMost                   = LogicTrick(std::to_string(RT_GC_LEFTMOST));
  Option LogicGoronCityGrotto                     = LogicTrick(std::to_string(RT_GC_GROTTO));
  Option LogicGoronCityLinkGoronDins              = LogicTrick(std::to_string(RT_GC_LINK_GORON_DINS));
  Option LogicCraterBeanPoHWithHovers             = LogicTrick(std::to_string(RT_DMC_HOVER_BEAN_POH));
  Option LogicCraterBoleroJump                    = LogicTrick(std::to_string(RT_DMC_BOLERO_JUMP));
  Option LogicCraterBoulderJS                     = LogicTrick(std::to_string(RT_DMC_BOULDER_JS));
  Option LogicCraterBoulderSkip                   = LogicTrick(std::to_string(RT_DMC_BOULDER_SKIP));
  Option LogicZoraRiverLower                      = LogicTrick(std::to_string(RT_ZR_LOWER));
  Option LogicZoraRiverUpper                      = LogicTrick(std::to_string(RT_ZR_UPPER));
  Option LogicZoraWithHovers                      = LogicTrick(std::to_string(RT_ZR_HOVERS));
  Option LogicZoraWithCucco                       = LogicTrick(std::to_string(RT_ZR_CUCCO));
  Option LogicKingZoraSkip                        = LogicTrick(std::to_string(RT_ZD_KING_ZORA_SKIP));
  Option LogicDomainGS                            = LogicTrick(std::to_string(RT_ZD_GS));
  Option LogicLabWallGS                           = LogicTrick(std::to_string(RT_LH_LAB_WALL_GS));
  Option LogicLabDiving                           = LogicTrick(std::to_string(RT_LH_LAB_DIVING));
  Option LogicWaterHookshotEntry                  = LogicTrick(std::to_string(RT_LH_WATER_HOOKSHOT));
  Option LogicValleyCrateHovers                   = LogicTrick(std::to_string(RT_GV_CRATE_HOVERS));
  Option LogicGerudoKitchen                       = LogicTrick(std::to_string(RT_GF_KITCHEN));
  Option LogicGFJump                              = LogicTrick(std::to_string(RT_GF_JUMP));
  Option LogicWastelandBunnyCrossing              = LogicTrick(std::to_string(RT_HW_BUNNY_CROSSING));
  Option LogicWastelandCrossing                   = LogicTrick(std::to_string(RT_HW_CROSSING));
  Option LogicLensWasteland                       = LogicTrick(std::to_string(RT_LENS_HW));
  Option LogicReverseWasteland                    = LogicTrick(std::to_string(RT_HW_REVERSE));
  Option LogicColossusGS                          = LogicTrick(std::to_string(RT_COLOSSUS_GS));
  Option LogicDekuBasementGS                      = LogicTrick(std::to_string(RT_DEKU_BASEMENT_GS));
  Option LogicDekuB1Skip                          = LogicTrick(std::to_string(RT_DEKU_B1_SKIP));
  Option LogicDekuB1WebsWithBow                   = LogicTrick(std::to_string(RT_DEKU_B1_BOW_WEBS));
  Option LogicDekuMQCompassGS                     = LogicTrick(std::to_string(RT_DEKU_MQ_COMPASS_GS));
  Option LogicDekuMQLog                           = LogicTrick(std::to_string(RT_DEKU_MQ_LOG));
  Option LogicDCScarecrowGS                       = LogicTrick(std::to_string(RT_DC_SCARECROW_GS));
  Option LogicDCVinesGS                           = LogicTrick(std::to_string(RT_DC_VINES_GS));
  Option LogicDCStaircase                         = LogicTrick(std::to_string(RT_DC_STAIRCASE));
  Option LogicDCSlingshotSkip                     = LogicTrick(std::to_string(RT_DC_SLINGSHOT_SKIP));
  Option LogicDCScrubRoom                         = LogicTrick(std::to_string(RT_DC_SCRUB_ROOM));
  Option LogicDCJump                              = LogicTrick(std::to_string(RT_DC_JUMP));
  Option LogicDCHammerFloor                       = LogicTrick(std::to_string(RT_DC_HAMMER_FLOOR));
  Option LogicDCMQChildBombs                      = LogicTrick(std::to_string(RT_DC_MQ_CHILD_BOMBS));
  Option LogicDCMQEyesChild                       = LogicTrick(std::to_string(RT_DC_MQ_CHILD_EYES));
  Option LogicDCMQEyesAdult                       = LogicTrick(std::to_string(RT_DC_MQ_ADULT_EYES));
  Option LogicJabuAlcoveJumpDive                  = LogicTrick(std::to_string(RT_JABU_ALCOVE_JUMP_DIVE));
  Option LogicJabuBossHover                       = LogicTrick(std::to_string(RT_JABU_BOSS_HOVER));
  Option LogicJabuNearBossRanged                  = LogicTrick(std::to_string(RT_JABU_NEAR_BOSS_RANGED));
  Option LogicJabuNearBossExplosives              = LogicTrick(std::to_string(RT_JABU_NEAR_BOSS_EXPLOSIVES));
  Option LogicLensJabuMQ                          = LogicTrick(std::to_string(RT_LENS_JABU_MQ));
  Option LogicJabuMQRangJump                      = LogicTrick(std::to_string(RT_JABU_MQ_RANG_JUMP));
  Option LogicJabuMQSoTGS                         = LogicTrick(std::to_string(RT_JABU_MQ_SOT_GS));
  Option LogicLensBotw                            = LogicTrick(std::to_string(RT_LENS_BOTW));
  Option LogicChildDeadhand                       = LogicTrick(std::to_string(RT_BOTW_CHILD_DEADHAND));
  Option LogicBotwBasement                        = LogicTrick(std::to_string(RT_BOTW_BASEMENT));
  Option LogicBotwMQPits                          = LogicTrick(std::to_string(RT_BOTW_MQ_PITS));
  Option LogicBotwMQDeadHandKey                   = LogicTrick(std::to_string(RT_BOTW_MQ_DEADHAND_KEY));
  Option LogicForestFirstGS                       = LogicTrick(std::to_string(RT_FOREST_FIRST_GS));
  Option LogicForestOutdoorEastGS                 = LogicTrick(std::to_string(RT_FOREST_OUTDOORS_EAST_GS));
  Option LogicForestVines                         = LogicTrick(std::to_string(RT_FOREST_VINES));
  Option LogicForestOutdoorsLedge                 = LogicTrick(std::to_string(RT_FOREST_OUTDOORS_LEDGE));
  Option LogicForestDoorFrame                     = LogicTrick(std::to_string(RT_FOREST_DOORFRAME));
  Option LogicForestOutsideBackdoor               = LogicTrick(std::to_string(RT_FOREST_OUTSIDE_BACKDOOR));
  Option LogicForestMQWellSwim                    = LogicTrick(std::to_string(RT_FOREST_MQ_WELL_SWIM));
  Option LogicForestMQBlockPuzzle                 = LogicTrick(std::to_string(RT_FOREST_MQ_BLOCK_PUZZLE));
  Option LogicForestMQHallwaySwitchJS             = LogicTrick(std::to_string(RT_FOREST_MQ_JS_HALLWAY_SWITCH));
  Option LogicForestMQHallwaySwitchHookshot       = LogicTrick(std::to_string(RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH));
  Option LogicForestMQHallwaySwitchBoomerang      = LogicTrick(std::to_string(RT_FOREST_MQ_RANG_HALLWAY_SWITCH));
  Option LogicFireBossDoorJump                    = LogicTrick(std::to_string(RT_FIRE_BOSS_DOOR_JUMP));
  Option LogicFireSongOfTime                      = LogicTrick(std::to_string(RT_FIRE_SOT));
  Option LogicFireStrength                        = LogicTrick(std::to_string(RT_FIRE_STRENGTH));
  Option LogicFireScarecrow                       = LogicTrick(std::to_string(RT_FIRE_SCARECROW));
  Option LogicFireFlameMaze                       = LogicTrick(std::to_string(RT_FIRE_FLAME_MAZE));
  Option LogicFireMQNearBoss                      = LogicTrick(std::to_string(RT_FIRE_MQ_NEAR_BOSS));
  Option LogicFireMQBlockedChest                  = LogicTrick(std::to_string(RT_FIRE_MQ_BLOCKED_CHEST));
  Option LogicFireMQBKChest                       = LogicTrick(std::to_string(RT_FIRE_MQ_BK_CHEST));
  Option LogicFireMQClimb                         = LogicTrick(std::to_string(RT_FIRE_MQ_CLIMB));
  Option LogicFireMQMazeSideRoom                  = LogicTrick(std::to_string(RT_FIRE_MQ_MAZE_SIDE_ROOM));
  Option LogicFireMQMazeHovers                    = LogicTrick(std::to_string(RT_FIRE_MQ_MAZE_HOVERS));
  Option LogicFireMQMazeJump                      = LogicTrick(std::to_string(RT_FIRE_MQ_MAZE_JUMP));
  Option LogicFireMQAboveMazeGS                   = LogicTrick(std::to_string(RT_FIRE_MQ_ABOVE_MAZE_GS));
  Option LogicFireMQFlameMaze                     = LogicTrick(std::to_string(RT_FIRE_MQ_FLAME_MAZE));
  Option LogicWaterTempleTorchLongshot            = LogicTrick(std::to_string(RT_WATER_LONGSHOT_TORCH));
  Option LogicWaterCrackedWallHovers              = LogicTrick(std::to_string(RT_WATER_CRACKED_WALL_HOVERS));
  Option LogicWaterCrackedWallNothing             = LogicTrick(std::to_string(RT_WATER_CRACKED_WALL));
  Option LogicWaterBossKeyRegion                  = LogicTrick(std::to_string(RT_WATER_BK_REGION));
  Option LogicWaterNorthBasementLedgeJump         = LogicTrick(std::to_string(RT_WATER_NORTH_BASEMENT_LEDGE_JUMP));
  Option LogicWaterBKJumpDive                     = LogicTrick(std::to_string(RT_WATER_BK_JUMP_DIVE));
  Option LogicWaterCentralGSFW                    = LogicTrick(std::to_string(RT_WATER_FW_CENTRAL_GS));
  Option LogicWaterCentralGSIrons                 = LogicTrick(std::to_string(RT_WATER_IRONS_CENTRAL_GS));
  Option LogicWaterCentralBow                     = LogicTrick(std::to_string(RT_WATER_CENTRAL_BOW));
  Option LogicWaterFallingPlatformGSHookshot      = LogicTrick(std::to_string(RT_WATER_HOOKSHOT_FALLING_PLATFORM_GS));
  Option LogicWaterFallingPlatformGSBoomerang     = LogicTrick(std::to_string(RT_WATER_RANG_FALLING_PLATFORM_GS));
  Option LogicWaterRiverGS                        = LogicTrick(std::to_string(RT_WATER_RIVER_GS));
  Option LogicWaterDragonJumpDive                 = LogicTrick(std::to_string(RT_WATER_DRAGON_JUMP_DIVE));
  Option LogicWaterDragonAdult                    = LogicTrick(std::to_string(RT_WATER_ADULT_DRAGON));
  Option LogicWaterDragonChild                    = LogicTrick(std::to_string(RT_WATER_CHILD_DRAGON));
  Option LogicWaterMQCentralPillar                = LogicTrick(std::to_string(RT_WATER_MQ_CENTRAL_PILLAR));
  Option LogicWaterMQLockedGS                     = LogicTrick(std::to_string(RT_WATER_MQ_LOCKED_GS));
  Option LogicLensShadow                          = LogicTrick(std::to_string(RT_LENS_SHADOW));
  Option LogicLensShadowPlatform                  = LogicTrick(std::to_string(RT_LENS_SHADOW_PLATFORM));
  Option LogicLensBongo                           = LogicTrick(std::to_string(RT_LENS_BONGO));
  Option LogicShadowUmbrella                      = LogicTrick(std::to_string(RT_SHADOW_UMBRELLA));
  Option LogicShadowUmbrellaGS                    = LogicTrick(std::to_string(RT_SHADOW_UMBRELLA_GS));
  Option LogicShadowFreestandingKey               = LogicTrick(std::to_string(RT_SHADOW_FREESTANDING_KEY));
  Option LogicShadowStatue                        = LogicTrick(std::to_string(RT_SHADOW_STATUE));
  Option LogicShadowBongo                         = LogicTrick(std::to_string(RT_SHADOW_BONGO));
  Option LogicLensShadowMQ                        = LogicTrick(std::to_string(RT_LENS_SHADOW_MQ));
  Option LogicLensShadowMQInvisibleBlades         = LogicTrick(std::to_string(RT_LENS_SHADOW_MQ_INVISIBLE_BLADES));
  Option LogicLensShadowMQPlatform                = LogicTrick(std::to_string(RT_LENS_SHADOW_MQ_PLATFORM));
  Option LogicLensShadowMQDeadHand                = LogicTrick(std::to_string(RT_LENS_SHADOW_MQ_DEADHAND));
  Option LogicShadowMQGap                         = LogicTrick(std::to_string(RT_SHADOW_MQ_GAP));
  Option LogicShadowMQInvisibleBlades             = LogicTrick(std::to_string(RT_SHADOW_MQ_INVISIBLE_BLADES));
  Option LogicShadowMQHugePit                     = LogicTrick(std::to_string(RT_SHADOW_MQ_HUGE_PIT));
  Option LogicShadowMQWindyWalkway                = LogicTrick(std::to_string(RT_SHADOW_MQ_WINDY_WALKWAY));
  Option LogicLensSpirit                          = LogicTrick(std::to_string(RT_LENS_SPIRIT));
  Option LogicSpiritChildBombchu                  = LogicTrick(std::to_string(RT_SPIRIT_CHILD_CHU));
  Option LogicSpiritLobbyGS                       = LogicTrick(std::to_string(RT_SPIRIT_LOBBY_GS));
  Option LogicSpiritLowerAdultSwitch              = LogicTrick(std::to_string(RT_SPIRIT_LOWER_ADULT_SWITCH));
  Option LogicSpiritLobbyJump                     = LogicTrick(std::to_string(RT_SPIRIT_LOBBY_JUMP));
  Option LogicSpiritPlatformHookshot              = LogicTrick(std::to_string(RT_SPIRIT_PLATFORM_HOOKSHOT));
  Option LogicSpiritMapChest                      = LogicTrick(std::to_string(RT_SPIRIT_MAP_CHEST));
  Option LogicSpiritSunChest                      = LogicTrick(std::to_string(RT_SPIRIT_SUN_CHEST));
  Option LogicSpiritWall                          = LogicTrick(std::to_string(RT_SPIRIT_WALL));
  Option LogicLensSpiritMQ                        = LogicTrick(std::to_string(RT_LENS_SPIRIT_MQ));
  Option LogicSpiritMQSunBlockSoT                 = LogicTrick(std::to_string(RT_SPIRIT_MQ_SUN_BLOCK_SOT));
  Option LogicSpiritMQSunBlockGS                  = LogicTrick(std::to_string(RT_SPIRIT_MQ_SUN_BLOCK_GS));
  Option LogicSpiritMQLowerAdult                  = LogicTrick(std::to_string(RT_SPIRIT_MQ_LOWER_ADULT));
  Option LogicSpiritMQFrozenEye                   = LogicTrick(std::to_string(RT_SPIRIT_MQ_FROZEN_EYE));
  Option LogicIceBlockGS                          = LogicTrick(std::to_string(RT_ICE_BLOCK_GS));
  Option LogicIceMQRedIceGS                       = LogicTrick(std::to_string(RT_ICE_MQ_RED_ICE_GS));
  Option LogicIceMQScarecrow                      = LogicTrick(std::to_string(RT_ICE_MQ_SCARECROW));
  Option LogicLensGtg                             = LogicTrick(std::to_string(RT_LENS_GTG));
  Option LogicGtgWithoutHookshot                  = LogicTrick(std::to_string(RT_GTG_WITHOUT_HOOKSHOT));
  Option LogicGtgFakeWall                         = LogicTrick(std::to_string(RT_GTG_FAKE_WALL));
  Option LogicLensGtgMQ                           = LogicTrick(std::to_string(RT_LENS_GTG_MQ));
  Option LogicGtgMQWithHookshot                   = LogicTrick(std::to_string(RT_GTG_MQ_WITH_HOOKSHOT));
  Option LogicGtgMQWithoutHookshot                = LogicTrick(std::to_string(RT_GTG_MQ_WIHTOUT_HOOKSHOT));
  Option LogicLensCastle                          = LogicTrick(std::to_string(RT_LENS_GANON));
  Option LogicSpiritTrialHookshot                 = LogicTrick(std::to_string(RT_GANON_SPIRIT_TRIAL_HOOKSHOT));
  Option LogicLensCastleMQ                        = LogicTrick(std::to_string(RT_LENS_GANON_MQ));
  Option LogicFireTrialMQ                         = LogicTrick(std::to_string(RT_GANON_MQ_FIRE_TRIAL));
  Option LogicShadowTrialMQ                       = LogicTrick(std::to_string(RT_GANON_MQ_SHADOW_TRIAL));
  Option LogicLightTrialMQ                        = LogicTrick(std::to_string(RT_GANON_MQ_LIGHT_TRIAL));

  std::vector<Option *> trickOptions = {
    &LogicVisibleCollision,
    &LogicGrottosWithoutAgony,
    &LogicFewerTunicRequirements,
    &LogicRustedSwitches,
    &LogicFlamingChests,
    &LogicBunnyHoodJump,
    &LogicDamageBoost,
    &LogicHoverBoost,
    &LogicAdultKokiriGS,
    &LogicLostWoodsBridge,
    &LogicMidoBackflip,
    &LogicLostWoodsGSBean,
    &LogicCastleStormsGS,
    &LogicManOnRoof,
    &LogicKakarikoTowerGS,
    &LogicAdultWindmillPoH,
    &LogicChildWindmillPoH,
    &LogicKakarikoRooftopGS,
    &LogicGraveyardPoH,
    &LogicChildDampeRacePoH,
    &LogicShadowFireArrowEntry,
    &LogicDMTSoilGS,
    &LogicDMTBombable,
    &LogicDMTGSLowerHookshot,
    &LogicDMTGSLowerHovers,
    &LogicDMTGSLowerBean,
    &LogicDMTGSLowerJS,
    &LogicDMTClimbHovers,
    &LogicDMTGSUpper,
    &LogicBiggoronBolero,
    &LogicGoronCityPot,
    &LogicGoronCityPotWithStrength,
    &LogicChildRollingWithStrength,
    &LogicGoronCityLeftMost,
    &LogicGoronCityGrotto,
    &LogicGoronCityLinkGoronDins,
    &LogicCraterBeanPoHWithHovers,
    &LogicCraterBoleroJump,
    &LogicCraterBoulderJS,
    &LogicCraterBoulderSkip,
    &LogicZoraRiverLower,
    &LogicZoraRiverUpper,
    &LogicZoraWithHovers,
    &LogicZoraWithCucco,
    &LogicKingZoraSkip,
    &LogicDomainGS,
    &LogicLabWallGS,
    &LogicLabDiving,
    &LogicWaterHookshotEntry,
    &LogicValleyCrateHovers,
    &LogicGerudoKitchen,
    &LogicGFJump,
    &LogicWastelandBunnyCrossing,
    &LogicWastelandCrossing,
    &LogicLensWasteland,
    &LogicReverseWasteland,
    &LogicColossusGS,
    &LogicDekuBasementGS,
    &LogicDekuB1Skip,
    &LogicDekuB1WebsWithBow,
    &LogicDekuMQCompassGS,
    &LogicDekuMQLog,
    &LogicDCScarecrowGS,
    &LogicDCVinesGS,
    &LogicDCStaircase,
    &LogicDCSlingshotSkip,
    &LogicDCScrubRoom,
    &LogicDCJump,
    &LogicDCHammerFloor,
    &LogicDCMQChildBombs,
    &LogicDCMQEyesChild,
    &LogicDCMQEyesAdult,
    &LogicJabuAlcoveJumpDive,
    &LogicJabuBossHover,
    &LogicJabuNearBossRanged,
    &LogicJabuNearBossExplosives,
    &LogicLensJabuMQ,
    &LogicJabuMQRangJump,
    &LogicJabuMQSoTGS,
    &LogicLensBotw,
    &LogicChildDeadhand,
    &LogicBotwBasement,
    &LogicBotwMQPits,
    &LogicBotwMQDeadHandKey,
    &LogicForestFirstGS,
    &LogicForestOutdoorEastGS,
    &LogicForestVines,
    &LogicForestOutdoorsLedge,
    &LogicForestDoorFrame,
    &LogicForestOutsideBackdoor,
    &LogicForestMQWellSwim,
    &LogicForestMQBlockPuzzle,
    &LogicForestMQHallwaySwitchJS,
    //&LogicForestMQHallwaySwitchHookshot,
    &LogicForestMQHallwaySwitchBoomerang,
    &LogicFireBossDoorJump,
    &LogicFireSongOfTime,
    &LogicFireStrength,
    &LogicFireScarecrow,
    &LogicFireFlameMaze,
    &LogicFireMQNearBoss,
    &LogicFireMQBlockedChest,
    &LogicFireMQBKChest,
    &LogicFireMQClimb,
    &LogicFireMQMazeSideRoom,
    &LogicFireMQMazeHovers,
    &LogicFireMQMazeJump,
    &LogicFireMQAboveMazeGS,
    &LogicFireMQFlameMaze,
    &LogicWaterTempleTorchLongshot,
    &LogicWaterCrackedWallHovers,
    &LogicWaterCrackedWallNothing,
    &LogicWaterBossKeyRegion,
    &LogicWaterNorthBasementLedgeJump,
    &LogicWaterBKJumpDive,
    &LogicWaterCentralGSFW,
    &LogicWaterCentralGSIrons,
    &LogicWaterCentralBow,
    &LogicWaterFallingPlatformGSHookshot,
    &LogicWaterFallingPlatformGSBoomerang,
    &LogicWaterRiverGS,
    &LogicWaterDragonJumpDive,
    &LogicWaterDragonAdult,
    &LogicWaterDragonChild,
    &LogicWaterMQCentralPillar,
    &LogicWaterMQLockedGS,
    &LogicLensShadow,
    &LogicLensShadowPlatform,
    &LogicLensBongo,
    &LogicShadowUmbrella,
    &LogicShadowUmbrellaGS,
    &LogicShadowFreestandingKey,
    &LogicShadowStatue,
    &LogicShadowBongo,
    &LogicLensShadowMQ,
    &LogicLensShadowMQInvisibleBlades,
    &LogicLensShadowMQPlatform,
    &LogicLensShadowMQDeadHand,
    &LogicShadowMQGap,
    &LogicShadowMQInvisibleBlades,
    &LogicShadowMQHugePit,
    &LogicShadowMQWindyWalkway,
    &LogicLensSpirit,
    &LogicSpiritChildBombchu,
    &LogicSpiritLobbyGS,
    &LogicSpiritLowerAdultSwitch,
    &LogicSpiritLobbyJump,
    &LogicSpiritPlatformHookshot,
    &LogicSpiritMapChest,
    &LogicSpiritSunChest,
    &LogicSpiritWall,
    &LogicLensSpiritMQ,
    &LogicSpiritMQSunBlockSoT,
    &LogicSpiritMQSunBlockGS,
    &LogicSpiritMQLowerAdult,
    &LogicSpiritMQFrozenEye,
    &LogicIceBlockGS,
    &LogicIceMQRedIceGS,
    &LogicIceMQScarecrow,
    &LogicLensGtg,
    &LogicGtgWithoutHookshot,
    &LogicGtgFakeWall,
    &LogicLensGtgMQ,
    &LogicGtgMQWithHookshot,
    &LogicGtgMQWithoutHookshot,
    &LogicLensCastle,
    &LogicSpiritTrialHookshot,
    &LogicLensCastleMQ,
    &LogicFireTrialMQ,
    &LogicShadowTrialMQ,
    &LogicLightTrialMQ,
  };
  std::unordered_map<RandomizerTrick,Option *> trickOptionMap = {
    {RT_VISIBLE_COLLISION,&LogicVisibleCollision},
    {RT_GROTTOS_WITHOUT_AGONY,&LogicGrottosWithoutAgony},
    {RT_FEWER_TUNIC_REQUIREMENTS,&LogicFewerTunicRequirements},
    {RT_RUSTED_SWITCHES,&LogicRustedSwitches},
    {RT_FLAMING_CHESTS,&LogicFlamingChests},
    {RT_BUNNY_HOOD_JUMPS,&LogicBunnyHoodJump},
    {RT_DAMAGE_BOOST_SIMPLE,&LogicDamageBoost},
    {RT_HOVER_BOOST_SIMPLE,&LogicHoverBoost},
    {RT_KF_ADULT_GS,&LogicAdultKokiriGS},
    {RT_LW_BRIDGE,&LogicLostWoodsBridge},
    {RT_LW_MIDO_BACKFLIP,&LogicMidoBackflip},
    {RT_LW_GS_BEAN,&LogicLostWoodsGSBean},
    {RT_HC_STORMS_GS,&LogicCastleStormsGS},
    {RT_KAK_MAN_ON_ROOF,&LogicManOnRoof},
    {RT_KAK_TOWER_GS,&LogicKakarikoTowerGS},
    {RT_KAK_ADULT_WINDMILL_POH,&LogicAdultWindmillPoH},
    {RT_KAK_CHILD_WINDMILL_POH,&LogicChildWindmillPoH},
    {RT_KAK_ROOFTOP_GS,&LogicKakarikoRooftopGS},
    {RT_GY_POH,&LogicGraveyardPoH},
    {RT_GY_CHILD_DAMPE_RACE_POH,&LogicChildDampeRacePoH},
    {RT_GY_SHADOW_FIRE_ARROWS,&LogicShadowFireArrowEntry},
    {RT_DMT_SOIL_GS,&LogicDMTSoilGS},
    {RT_DMT_BOMBABLE,&LogicDMTBombable},
    {RT_DMT_HOOKSHOT_LOWER_GS,&LogicDMTGSLowerHookshot},
    {RT_DMT_HOVERS_LOWER_GS,&LogicDMTGSLowerHovers},
    {RT_DMT_BEAN_LOWER_GS,&LogicDMTGSLowerBean},
    {RT_DMT_JS_LOWER_GS,&LogicDMTGSLowerJS},
    {RT_DMT_CLIMB_HOVERS,&LogicDMTClimbHovers},
    {RT_DMT_UPPER_GS,&LogicDMTGSUpper},
    {RT_DMT_BOLERO_BIGGORON,&LogicBiggoronBolero},
    {RT_GC_POT,&LogicGoronCityPot},
    {RT_GC_POT_STRENGTH,&LogicGoronCityPotWithStrength},
    {RT_GC_ROLLING_STRENGTH,&LogicChildRollingWithStrength},
    {RT_GC_LEFTMOST,&LogicGoronCityLeftMost},
    {RT_GC_GROTTO,&LogicGoronCityGrotto},
    {RT_GC_LINK_GORON_DINS,&LogicGoronCityLinkGoronDins},
    {RT_DMC_HOVER_BEAN_POH,&LogicCraterBeanPoHWithHovers},
    {RT_DMC_BOLERO_JUMP,&LogicCraterBoleroJump},
    {RT_DMC_BOULDER_JS,&LogicCraterBoulderJS},
    {RT_DMC_BOULDER_SKIP,&LogicCraterBoulderSkip},
    {RT_ZR_LOWER,&LogicZoraRiverLower},
    {RT_ZR_UPPER,&LogicZoraRiverUpper},
    {RT_ZR_HOVERS,&LogicZoraWithHovers},
    {RT_ZR_CUCCO,&LogicZoraWithCucco},
    {RT_ZD_KING_ZORA_SKIP,&LogicKingZoraSkip},
    {RT_ZD_GS,&LogicDomainGS},
    {RT_LH_LAB_WALL_GS,&LogicLabWallGS},
    {RT_LH_LAB_DIVING,&LogicLabDiving},
    {RT_LH_WATER_HOOKSHOT,&LogicWaterHookshotEntry},
    {RT_GV_CRATE_HOVERS,&LogicValleyCrateHovers},
    {RT_GF_KITCHEN,&LogicGerudoKitchen},
    {RT_GF_JUMP,&LogicGFJump},
    {RT_HW_BUNNY_CROSSING,&LogicWastelandBunnyCrossing},
    {RT_HW_CROSSING,&LogicWastelandCrossing},
    {RT_LENS_HW,&LogicLensWasteland},
    {RT_HW_REVERSE,&LogicReverseWasteland},
    {RT_COLOSSUS_GS,&LogicColossusGS},
    {RT_DEKU_BASEMENT_GS,&LogicDekuBasementGS},
    {RT_DEKU_B1_SKIP,&LogicDekuB1Skip},
    {RT_DEKU_B1_BOW_WEBS,&LogicDekuB1WebsWithBow},
    {RT_DEKU_MQ_COMPASS_GS,&LogicDekuMQCompassGS},
    {RT_DEKU_MQ_LOG,&LogicDekuMQLog},
    {RT_DC_SCARECROW_GS,&LogicDCScarecrowGS},
    {RT_DC_VINES_GS,&LogicDCVinesGS},
    {RT_DC_STAIRCASE,&LogicDCStaircase},
    {RT_DC_SLINGSHOT_SKIP,&LogicDCSlingshotSkip},
    {RT_DC_SCRUB_ROOM,&LogicDCScrubRoom},
    {RT_DC_JUMP,&LogicDCJump},
    {RT_DC_HAMMER_FLOOR,&LogicDCHammerFloor},
    {RT_DC_MQ_CHILD_BOMBS,&LogicDCMQChildBombs},
    {RT_DC_MQ_CHILD_EYES,&LogicDCMQEyesChild},
    {RT_DC_MQ_ADULT_EYES,&LogicDCMQEyesAdult},
    {RT_JABU_ALCOVE_JUMP_DIVE,&LogicJabuAlcoveJumpDive},
    {RT_JABU_BOSS_HOVER,&LogicJabuBossHover},
    {RT_JABU_NEAR_BOSS_RANGED,&LogicJabuNearBossRanged},
    {RT_JABU_NEAR_BOSS_EXPLOSIVES,&LogicJabuNearBossExplosives},
    {RT_LENS_JABU_MQ,&LogicLensJabuMQ},
    {RT_JABU_MQ_RANG_JUMP,&LogicJabuMQRangJump},
    {RT_JABU_MQ_SOT_GS,&LogicJabuMQSoTGS},
    {RT_LENS_BOTW,&LogicLensBotw},
    {RT_BOTW_CHILD_DEADHAND,&LogicChildDeadhand},
    {RT_BOTW_BASEMENT,&LogicBotwBasement},
    {RT_BOTW_MQ_PITS,&LogicBotwMQPits},
    {RT_BOTW_MQ_DEADHAND_KEY,&LogicBotwMQDeadHandKey},
    {RT_FOREST_FIRST_GS,&LogicForestFirstGS},
    {RT_FOREST_OUTDOORS_EAST_GS,&LogicForestOutdoorEastGS},
    {RT_FOREST_VINES,&LogicForestVines},
    {RT_FOREST_OUTDOORS_LEDGE,&LogicForestOutdoorsLedge},
    {RT_FOREST_DOORFRAME,&LogicForestDoorFrame},
    {RT_FOREST_OUTSIDE_BACKDOOR,&LogicForestOutsideBackdoor},
    {RT_FOREST_MQ_WELL_SWIM,&LogicForestMQWellSwim},
    {RT_FOREST_MQ_BLOCK_PUZZLE,&LogicForestMQBlockPuzzle},
    {RT_FOREST_MQ_JS_HALLWAY_SWITCH,&LogicForestMQHallwaySwitchJS},
    {RT_FOREST_MQ_HOOKSHOT_HALLWAY_SWITCH,&LogicForestMQHallwaySwitchHookshot},
    {RT_FOREST_MQ_RANG_HALLWAY_SWITCH,&LogicForestMQHallwaySwitchBoomerang},
    {RT_FIRE_BOSS_DOOR_JUMP,&LogicFireBossDoorJump},
    {RT_FIRE_SOT,&LogicFireSongOfTime},
    {RT_FIRE_STRENGTH,&LogicFireStrength},
    {RT_FIRE_SCARECROW,&LogicFireScarecrow},
    {RT_FIRE_FLAME_MAZE,&LogicFireFlameMaze},
    {RT_FIRE_MQ_NEAR_BOSS,&LogicFireMQNearBoss},
    {RT_FIRE_MQ_BLOCKED_CHEST,&LogicFireMQBlockedChest},
    {RT_FIRE_MQ_BK_CHEST,&LogicFireMQBKChest},
    {RT_FIRE_MQ_CLIMB,&LogicFireMQClimb},
    {RT_FIRE_MQ_MAZE_SIDE_ROOM,&LogicFireMQMazeSideRoom},
    {RT_FIRE_MQ_MAZE_HOVERS,&LogicFireMQMazeHovers},
    {RT_FIRE_MQ_MAZE_JUMP,&LogicFireMQMazeJump},
    {RT_FIRE_MQ_ABOVE_MAZE_GS,&LogicFireMQAboveMazeGS},
    {RT_FIRE_MQ_FLAME_MAZE,&LogicFireMQFlameMaze},
    {RT_WATER_LONGSHOT_TORCH,&LogicWaterTempleTorchLongshot},
    {RT_WATER_CRACKED_WALL_HOVERS,&LogicWaterCrackedWallHovers},
    {RT_WATER_CRACKED_WALL,&LogicWaterCrackedWallNothing},
    {RT_WATER_BK_REGION,&LogicWaterBossKeyRegion},
    {RT_WATER_NORTH_BASEMENT_LEDGE_JUMP,&LogicWaterNorthBasementLedgeJump},
    {RT_WATER_BK_JUMP_DIVE,&LogicWaterBKJumpDive},
    {RT_WATER_FW_CENTRAL_GS,&LogicWaterCentralGSFW},
    {RT_WATER_IRONS_CENTRAL_GS,&LogicWaterCentralGSIrons},
    {RT_WATER_CENTRAL_BOW,&LogicWaterCentralBow},
    {RT_WATER_HOOKSHOT_FALLING_PLATFORM_GS,&LogicWaterFallingPlatformGSHookshot},
    {RT_WATER_RANG_FALLING_PLATFORM_GS,&LogicWaterFallingPlatformGSBoomerang},
    {RT_WATER_RIVER_GS,&LogicWaterRiverGS},
    {RT_WATER_DRAGON_JUMP_DIVE,&LogicWaterDragonJumpDive},
    {RT_WATER_ADULT_DRAGON,&LogicWaterDragonAdult},
    {RT_WATER_CHILD_DRAGON,&LogicWaterDragonChild},
    {RT_WATER_MQ_CENTRAL_PILLAR,&LogicWaterMQCentralPillar},
    {RT_WATER_MQ_LOCKED_GS,&LogicWaterMQLockedGS},
    {RT_LENS_SHADOW,&LogicLensShadow},
    {RT_LENS_SHADOW_PLATFORM,&LogicLensShadowPlatform},
    {RT_LENS_BONGO,&LogicLensBongo},
    {RT_SHADOW_UMBRELLA,&LogicShadowUmbrella},
    {RT_SHADOW_UMBRELLA_GS,&LogicShadowUmbrellaGS},
    {RT_SHADOW_FREESTANDING_KEY,&LogicShadowFreestandingKey},
    {RT_SHADOW_STATUE,&LogicShadowStatue},
    {RT_SHADOW_BONGO,&LogicShadowBongo},
    {RT_LENS_SHADOW_MQ,&LogicLensShadowMQ},
    {RT_LENS_SHADOW_MQ_INVISIBLE_BLADES,&LogicLensShadowMQInvisibleBlades},
    {RT_LENS_SHADOW_MQ_PLATFORM,&LogicLensShadowMQPlatform},
    {RT_LENS_SHADOW_MQ_DEADHAND,&LogicLensShadowMQDeadHand},
    {RT_SHADOW_MQ_GAP,&LogicShadowMQGap},
    {RT_SHADOW_MQ_INVISIBLE_BLADES,&LogicShadowMQInvisibleBlades},
    {RT_SHADOW_MQ_HUGE_PIT,&LogicShadowMQHugePit},
    {RT_SHADOW_MQ_WINDY_WALKWAY,&LogicShadowMQWindyWalkway},
    {RT_LENS_SPIRIT,&LogicLensSpirit},
    {RT_SPIRIT_CHILD_CHU,&LogicSpiritChildBombchu},
    {RT_SPIRIT_LOBBY_GS,&LogicSpiritLobbyGS},
    {RT_SPIRIT_LOWER_ADULT_SWITCH,&LogicSpiritLowerAdultSwitch},
    {RT_SPIRIT_LOBBY_JUMP,&LogicSpiritLobbyJump},
    {RT_SPIRIT_PLATFORM_HOOKSHOT,&LogicSpiritPlatformHookshot},
    {RT_SPIRIT_MAP_CHEST,&LogicSpiritMapChest},
    {RT_SPIRIT_SUN_CHEST,&LogicSpiritSunChest},
    {RT_SPIRIT_WALL,&LogicSpiritWall},
    {RT_LENS_SPIRIT_MQ,&LogicLensSpiritMQ},
    {RT_SPIRIT_MQ_SUN_BLOCK_SOT,&LogicSpiritMQSunBlockSoT},
    {RT_SPIRIT_MQ_SUN_BLOCK_GS,&LogicSpiritMQSunBlockGS},
    {RT_SPIRIT_MQ_LOWER_ADULT,&LogicSpiritMQLowerAdult},
    {RT_SPIRIT_MQ_FROZEN_EYE,&LogicSpiritMQFrozenEye},
    {RT_ICE_BLOCK_GS,&LogicIceBlockGS},
    {RT_ICE_MQ_RED_ICE_GS,&LogicIceMQRedIceGS},
    {RT_ICE_MQ_SCARECROW,&LogicIceMQScarecrow},
    {RT_LENS_GTG,&LogicLensGtg},
    {RT_GTG_WITHOUT_HOOKSHOT,&LogicGtgWithoutHookshot},
    {RT_GTG_FAKE_WALL,&LogicGtgFakeWall},
    {RT_LENS_GTG_MQ,&LogicLensGtgMQ},
    {RT_GTG_MQ_WITH_HOOKSHOT,&LogicGtgMQWithHookshot},
    {RT_GTG_MQ_WIHTOUT_HOOKSHOT,&LogicGtgMQWithoutHookshot},
    {RT_LENS_GANON,&LogicLensCastle},
    {RT_GANON_SPIRIT_TRIAL_HOOKSHOT,&LogicSpiritTrialHookshot},
    {RT_LENS_GANON_MQ,&LogicLensCastleMQ},
    {RT_GANON_MQ_FIRE_TRIAL,&LogicFireTrialMQ},
    {RT_GANON_MQ_SHADOW_TRIAL,&LogicShadowTrialMQ},
    {RT_GANON_MQ_LIGHT_TRIAL,&LogicLightTrialMQ},
  };

  //TODO completely rewrite glitch section since oot3d glitches differ from oot
  
  Option GlitchEquipSwapDins = LogicTrick("");
  Option GlitchEquipSwap     = LogicTrick("");

  std::vector<Option*> glitchCategories = {
    &GlitchEquipSwapDins,
    &GlitchEquipSwap,
  };
  
  std::vector<Option*> miscGlitches = {
    
  };
  

  Menu glitchCategorySettings = Menu::SubMenu("General Categories",     &glitchCategories, false);
  Menu miscGlitchSettings     = Menu::SubMenu("Miscellaneous Glitches", &miscGlitches,     false);
  std::vector<Menu*> glitchOptions = {
    &glitchCategorySettings,
    &miscGlitchSettings,
  };

  Menu loadSettingsPreset       = Menu::Action("Load Settings Preset",       LOAD_PRESET);
  Menu saveSettingsPreset       = Menu::Action("Save Settings Preset",       SAVE_PRESET);
  Menu deleteSettingsPreset     = Menu::Action("Delete Settings Preset",     DELETE_PRESET);
  Menu resetToDefaultSettings   = Menu::Action("Reset to Default Settings",  RESET_TO_DEFAULTS);

  std::vector<Menu *> settingsPresetItems = {
    &loadSettingsPreset,
    &saveSettingsPreset,
    &deleteSettingsPreset,
    &resetToDefaultSettings,
  };

  //Detailed Logic Options Submenu
  Menu logicSettings    = Menu::SubMenu("Logic Options",     &logicOptions);
  Menu excludeLocations = Menu::SubMenu("Exclude Locations", &excludeLocationsMenus, false);
  Menu tricks           = Menu::SubMenu("Logical Tricks",    &trickOptions, false);
  Menu glitchSettings   = Menu::SubMenu("Glitch Options",    &glitchOptions, false);
  std::vector<Menu *> detailedLogicOptions = {
    &logicSettings,
    &excludeLocations,
    &tricks,
    &glitchSettings,
  };

  Menu open                     = Menu::SubMenu("Open Settings",              &openOptions);
  Menu world                    = Menu::SubMenu("World Settings",             &worldOptions);
  Menu shuffle                  = Menu::SubMenu("Shuffle Settings",           &shuffleOptions);
  Menu shuffleDungeonItems      = Menu::SubMenu("Shuffle Dungeon Items",      &shuffleDungeonItemOptions);
  Menu detailedLogic            = Menu::SubMenu("Detailed Logic Settings",    &detailedLogicOptions);
  Menu startingInventory        = Menu::SubMenu("Starting Inventory",         &startingInventoryOptions, false);
  Menu timesaverSettings        = Menu::SubMenu("Timesaver Settings",         &timesaverOptions);
  Menu miscSettings             = Menu::SubMenu("Misc Settings",              &miscOptions);
  Menu itemPoolSettings         = Menu::SubMenu("Item Pool Settings",         &itemPoolOptions);
  Menu itemUsabilitySettings    = Menu::SubMenu("Item Usability Settings",    &itemUsabilityOptions);
  Menu settingsPresets          = Menu::SubMenu("Settings Presets",           &settingsPresetItems);
  Menu generateRandomizer       = Menu::Action ("Generate Randomizer",        GENERATE_MODE);

  //adding a menu with no options crashes, might fix later
  std::vector<Menu *> mainMenu = {
    &open,
    &world,
    &shuffle,
    &shuffleDungeonItems,
    &timesaverSettings,
    &detailedLogic,
    &startingInventory,
    &miscSettings,
    &itemPoolSettings,
    &itemUsabilitySettings,
    &settingsPresets,
    &generateRandomizer,
  };

  //declared here, set in fill.cpp
  uint32_t LinksPocketRewardBitMask = 0;
  std::array<uint32_t, 9> rDungeonRewardOverrides{};

  //Fills and returns a SettingsContext struct.
  //This struct is written to the code.ips patch and allows the game
  //to read what settings the player selected to make in game decisions.
  SettingsContext FillContext() {
    SettingsContext ctx = {};
    ctx.hashIndexes[0] = hashIconIndexes[0];
    ctx.hashIndexes[1] = hashIconIndexes[1];
    ctx.hashIndexes[2] = hashIconIndexes[2];
    ctx.hashIndexes[3] = hashIconIndexes[3];
    ctx.hashIndexes[4] = hashIconIndexes[4];

    ctx.logic                = Logic.Value<uint8_t>();
    ctx.openForest           = OpenForest.Value<uint8_t>();
    ctx.openKakariko         = OpenKakariko.Value<uint8_t>();
    ctx.openDoorOfTime       = OpenDoorOfTime.Value<uint8_t>();
    ctx.zorasFountain        = ZorasFountain.Value<uint8_t>();
    ctx.gerudoFortress       = GerudoFortress.Value<uint8_t>();
    ctx.rainbowBridge        = Bridge.Value<uint8_t>();
    ctx.bridgeStoneCount     = BridgeStoneCount.Value<uint8_t>();
    ctx.bridgeMedallionCount = BridgeMedallionCount.Value<uint8_t>();
    ctx.bridgeRewardCount    = BridgeRewardCount.Value<uint8_t>();
    ctx.bridgeDungeonCount   = BridgeDungeonCount.Value<uint8_t>();
    ctx.bridgeTokenCount     = BridgeTokenCount.Value<uint8_t>();
    ctx.bridgeRewardOptions  = BridgeRewardOptions.Value<uint8_t>();
    ctx.randomGanonsTrials   = (RandomGanonsTrials) ? 1 : 0;
    ctx.ganonsTrialsCount    = GanonsTrialsCount.Value<uint8_t>();

    ctx.startingAge          = StartingAge.Value<uint8_t>();
    ctx.resolvedStartingAge  = ResolvedStartingAge;
    ctx.shuffleDungeonEntrances = ShuffleDungeonEntrances.Value<uint8_t>();
    ctx.shuffleBossEntrances    = ShuffleBossEntrances.Value<uint8_t>();
    ctx.shuffleOverworldEntrances = (ShuffleOverworldEntrances) ? 1 : 0;
    ctx.shuffleInteriorEntrances = ShuffleInteriorEntrances.Value<uint8_t>();
    ctx.shuffleGrottoEntrances  = (ShuffleGrottoEntrances) ? 1 : 0;
    ctx.shuffleOwlDrops         = (ShuffleOwlDrops) ? 1 : 0;
    ctx.shuffleWarpSongs        = (ShuffleWarpSongs) ? 1 : 0;
    ctx.shuffleOverworldSpawns  = (ShuffleOverworldSpawns) ? 1 : 0;
    ctx.mixedEntrancePools      = (MixedEntrancePools) ? 1 : 0;
    ctx.mixDungeons             = (MixDungeons) ? 1 : 0;
    ctx.mixOverworld            = (MixOverworld) ? 1 : 0;
    ctx.mixInteriors            = (MixInteriors) ? 1 : 0;
    ctx.mixGrottos              = (MixGrottos) ? 1 : 0;
    ctx.decoupleEntrances       = (DecoupleEntrances) ? 1 : 0;
    ctx.bombchusInLogic         = (BombchusInLogic) ? 1 : 0;
    ctx.ammoDrops            = AmmoDrops.Value<uint8_t>();
    ctx.heartDropRefill      = HeartDropRefill.Value<uint8_t>();
    ctx.randomMQDungeons        = (MQDungeonCount.Value<uint8_t>() == 13) ? 1 : 0;
    ctx.mqDungeonCount          = MQSet;

    ctx.shuffleRewards       = ShuffleRewards.Value<uint8_t>();
    ctx.linksPocketItem      = LinksPocketItem.Value<uint8_t>();
    ctx.shuffleSongs         = ShuffleSongs.Value<uint8_t>();
    ctx.tokensanity          = Tokensanity.Value<uint8_t>();
    ctx.scrubsanity          = Scrubsanity.Value<uint8_t>();
    ctx.shuffleCows          = (ShuffleCows) ? 1 : 0;
    ctx.shuffleKokiriSword   = (ShuffleKokiriSword) ? 1 : 0;
    ctx.shuffleOcarinas      = (ShuffleOcarinas) ? 1 : 0;
    ctx.shuffleWeirdEgg      = (ShuffleWeirdEgg) ? 1 : 0;
    ctx.shuffleGerudoToken   = (ShuffleGerudoToken) ? 1 : 0;
    ctx.shuffleMagicBeans    = (ShuffleMagicBeans) ? 1 : 0;
    ctx.shuffleMerchants     = ShuffleMerchants.Value<uint8_t>();
    ctx.shuffleFrogSongRupees= (ShuffleFrogSongRupees) ? 1 : 0;
    ctx.shuffleAdultTradeQuest = (ShuffleAdultTradeQuest) ? 1 : 0;
    ctx.shuffleChestMinigame = ShuffleChestMinigame.Value<uint8_t>();
    ctx.shuffle100GsReward   = (Shuffle100GSReward) ? 1 : 0;

    ctx.mapsAndCompasses     = MapsAndCompasses.Value<uint8_t>();
    ctx.keysanity            = Keysanity.Value<uint8_t>();
    ctx.gerudoKeys           = GerudoKeys.Value<uint8_t>();
    ctx.bossKeysanity        = BossKeysanity.Value<uint8_t>();
    ctx.ganonsBossKey        = GanonsBossKey.Value<uint8_t>();
    ctx.lacsCondition        = LACSCondition;
    ctx.lacsStoneCount       = LACSStoneCount.Value<uint8_t>();
    ctx.lacsMedallionCount   = LACSMedallionCount.Value<uint8_t>();
    ctx.lacsRewardCount      = LACSRewardCount.Value<uint8_t>();
    ctx.lacsDungeonCount     = LACSDungeonCount.Value<uint8_t>();
    ctx.lacsTokenCount       = LACSTokenCount.Value<uint8_t>();
    ctx.lacsRewardOptions    = LACSRewardOptions.Value<uint8_t>();

    ctx.ringFortress         = (RingFortress) ? 1 : 0;
    ctx.ringForest           = (RingForest) ? 1 : 0;
    ctx.ringFire             = (RingFire) ? 1 : 0;
    ctx.ringWater            = (RingWater) ? 1 : 0;
    ctx.ringSpirit           = (RingSpirit) ? 1 : 0;
    ctx.ringShadow           = (RingShadow) ? 1 : 0;
    ctx.ringWell             = (RingWell) ? 1 : 0;
    ctx.ringGtg              = (RingGtg) ? 1 : 0;
    ctx.ringCastle           = (RingCastle) ? 1 : 0;

    ctx.skipChildStealth     = (SkipChildStealth) ? 1 : 0;
    ctx.skipTowerEscape      = (SkipTowerEscape) ? 1 : 0;
    ctx.skipEponaRace        = (SkipEponaRace) ? 1 : 0;
    ctx.skipMinigamePhases   = (SkipMinigamePhases) ? 1 : 0;
    ctx.freeScarecrow        = (FreeScarecrow) ? 1 : 0;
    ctx.fourPoesCutscene     = (FourPoesCutscene) ? 1 : 0;
    ctx.lakeHyliaOwl         = (LakeHyliaOwl) ? 1 : 0;
    ctx.bigPoeTargetCount    = BigPoeTargetCount.Value<uint8_t>() + 1;
    ctx.numRequiredCuccos    = NumRequiredCuccos.Value<uint8_t>();
    ctx.kingZoraSpeed        = KingZoraSpeed.Value<uint8_t>();
    ctx.completeMaskQuest    = CompleteMaskQuest ? 1 : 0;
    ctx.enableGlitchCutscenes = EnableGlitchCutscenes ? 1 : 0;
    ctx.quickText            = QuickText.Value<uint8_t>();
    ctx.skipSongReplays      = SkipSongReplays.Value<uint8_t>();
    ctx.keepFWWarpPoint      = KeepFWWarpPoint ? 1 : 0;
    ctx.fastBunnyHood        = FastBunnyHood ? 1 : 0;

    ctx.gossipStoneHints     = GossipStoneHints.Value<uint8_t>();
    ctx.compassesShowReward  = CompassesShowReward.Value<uint8_t>();
    ctx.compassesShowWotH    = CompassesShowWotH.Value<uint8_t>();
    ctx.mapsShowDungeonMode  = MapsShowDungeonMode.Value<uint8_t>();
    ctx.damageMultiplier     = DamageMultiplier.Value<uint8_t>();
    ctx.startingTime         = StartingTime.Value<uint8_t>();
    ctx.chestAnimations      = (ChestAnimations) ? 1 : 0;
    ctx.chestSize            = (ChestSize) ? 1 : 0;
    ctx.generateSpoilerLog   = (GenerateSpoilerLog) ? 1 : 0;
    ctx.ingameSpoilers       = (IngameSpoilers) ? 1 : 0;
    ctx.randomTrapDmg        = RandomTrapDmg.Value<uint8_t>();
    ctx.blueFireArrows       = (BlueFireArrows) ? 1 : 0;
    ctx.sunLightArrows       = (SunlightArrows) ? 1 : 0;

    ctx.faroresWindAnywhere  = (FaroresWindAnywhere) ? 1 : 0;
    ctx.stickAsAdult         = (StickAsAdult) ? 1 : 0;
    ctx.boomerangAsAdult     = (BoomerangAsAdult) ? 1 : 0;
    ctx.hammerAsChild        = (HammerAsChild) ? 1 : 0;
    ctx.slingshotAsAdult     = (SlingshotAsAdult) ? 1 : 0;
    ctx.bowAsChild           = (BowAsChild) ? 1 : 0;
    ctx.hookshotAsChild      = (HookshotAsChild) ? 1 : 0;
    ctx.ironbootsAsChild     = (IronBootsAsChild) ? 1 : 0;
    ctx.hoverbootsAsChild    = (HoverBootsAsChild) ? 1 : 0;
    ctx.masksAsAdult         = (MasksAsAdult) ? 1 : 0;
    ctx.kokiriSwordAsAdult   = (KokiriSwordAsAdult) ? 1 : 0;
    ctx.masterSwordAsChild   = (MasterSwordAsChild) ? 1 : 0;
    ctx.biggoronSwordAsChild = (BiggoronSwordAsChild) ? 1 : 0;
    ctx.dekuShieldAsAdult    = (DekuShieldAsAdult) ? 1 : 0;
    ctx.mirrorShieldAsChild  = (MirrorShieldAsChild) ? 1 : 0;
    ctx.goronTunicAsChild    = (GoronTunicAsChild) ? 1 : 0;
    ctx.zoraTunicAsChild     = (ZoraTunicAsChild) ? 1 : 0;
    ctx.gkDurability         = GkDurability.Value<uint8_t>();

    ctx.itemPoolValue        = ItemPoolValue.Value<uint8_t>();
    ctx.iceTrapValue         = IceTrapValue.Value<uint8_t>();
    ctx.progressiveGoronSword = (ProgressiveGoronSword) ? 1 : 0;

    ctx.linksPocketRewardBitMask = LinksPocketRewardBitMask;

    ctx.dekuTreeDungeonMode              = DekuTree.IsMQ()              ? 1 : 0;
    ctx.dodongosCavernDungeonMode        = DodongosCavern.IsMQ()        ? 1 : 0;
    ctx.jabuJabusBellyDungeonMode        = JabuJabusBelly.IsMQ()        ? 1 : 0;
    ctx.forestTempleDungeonMode          = ForestTemple.IsMQ()          ? 1 : 0;
    ctx.fireTempleDungeonMode            = FireTemple.IsMQ()            ? 1 : 0;
    ctx.waterTempleDungeonMode           = WaterTemple.IsMQ()           ? 1 : 0;
    ctx.spiritTempleDungeonMode          = SpiritTemple.IsMQ()          ? 1 : 0;
    ctx.shadowTempleDungeonMode          = ShadowTemple.IsMQ()          ? 1 : 0;
    ctx.bottomOfTheWellDungeonMode       = BottomOfTheWell.IsMQ()       ? 1 : 0;
    ctx.iceCavernDungeonMode             = IceCavern.IsMQ()             ? 1 : 0;
    ctx.ganonsCastleDungeonMode          = GanonsCastle.IsMQ()          ? 1 : 0;
    ctx.gerudoTrainingGroundsDungeonMode = GerudoTrainingGrounds.IsMQ() ? 1 : 0;

    for (uint8_t i = 0; i < sizeof(ctx.dungeonModesKnown); ++i) {
      ctx.dungeonModesKnown[i] = DungeonModesKnown[i];
    }

    ctx.forestTrialSkip = (ForestTrial.IsSkipped()) ? 1 : 0;
    ctx.fireTrialSkip   = (FireTrial.IsSkipped())   ? 1 : 0;
    ctx.waterTrialSkip  = (WaterTrial.IsSkipped())  ? 1 : 0;
    ctx.spiritTrialSkip = (SpiritTrial.IsSkipped()) ? 1 : 0;
    ctx.shadowTrialSkip = (ShadowTrial.IsSkipped()) ? 1 : 0;
    ctx.lightTrialSkip  = (LightTrial.IsSkipped())  ? 1 : 0;

    //Starting Inventory
    ctx.startingConsumables   = (StartingConsumables) ? 1 : 0;
    ctx.startingMaxRupees     = (StartingMaxRupees) ? 1 : 0;
    ctx.startingStickCapacity = StartingStickCapacity.Value<uint8_t>();
    ctx.startingNutCapacity   = StartingNutCapacity.Value<uint8_t>();
    ctx.startingBombBag       = StartingBombBag.Value<uint8_t>();
    ctx.startingBombchus      = StartingBombchus.Value<uint8_t>();
    ctx.startingBow           = StartingBow.Value<uint8_t>();
    ctx.startingFireArrows    = StartingFireArrows.Value<uint8_t>();
    ctx.startingIceArrows     = StartingIceArrows.Value<uint8_t>();
    ctx.startingLightArrows   = StartingLightArrows.Value<uint8_t>();
    ctx.startingDinsFire      = StartingDinsFire.Value<uint8_t>();
    ctx.startingFaroresWind   = StartingFaroresWind.Value<uint8_t>();
    ctx.startingNayrusLove    = StartingNayrusLove.Value<uint8_t>();
    ctx.startingSlingshot     = StartingSlingshot.Value<uint8_t>();
    ctx.startingBoomerang     = StartingBoomerang.Value<uint8_t>();
    ctx.startingLensOfTruth   = StartingLensOfTruth.Value<uint8_t>();
    ctx.startingMagicBean     = StartingMagicBean.Value<uint8_t>();
    ctx.startingMegatonHammer = StartingMegatonHammer.Value<uint8_t>();
    ctx.startingHookshot      = StartingHookshot.Value<uint8_t>();
    ctx.startingIronBoots     = StartingIronBoots.Value<uint8_t>();
    ctx.startingHoverBoots    = StartingHoverBoots.Value<uint8_t>();
    ctx.startingBottle1       = StartingBottle1.Value<uint8_t>();
    ctx.startingBottle2       = StartingBottle2.Value<uint8_t>();
    ctx.startingBottle3       = StartingBottle3.Value<uint8_t>();
    ctx.startingBottle4       = StartingBottle4.Value<uint8_t>();
    ctx.startingRutoBottle    = StartingRutoBottle.Value<uint8_t>();
    ctx.startingOcarina       = StartingOcarina.Value<uint8_t>();
    ctx.startingKokiriSword   = StartingKokiriSword.Value<uint8_t>();
    ctx.startingBiggoronSword = StartingBiggoronSword.Value<uint8_t>();
    ctx.startingHearts        = StartingHearts.Value<uint8_t>() + 1;
    ctx.startingMagicMeter    = StartingMagicMeter.Value<uint8_t>();
    ctx.startingDoubleDefense = StartingDoubleDefense.Value<uint8_t>();

    //Starting Quest Items
    ctx.startingQuestItems |= StartingMinuetOfForest.Value<uint8_t>()   << 6;
    ctx.startingQuestItems |= StartingBoleroOfFire.Value<uint8_t>()     << 7;
    ctx.startingQuestItems |= StartingSerenadeOfWater.Value<uint8_t>()  << 8;
    ctx.startingQuestItems |= StartingRequiemOfSpirit.Value<uint8_t>()  << 9;
    ctx.startingQuestItems |= StartingNocturneOfShadow.Value<uint8_t>() << 10;
    ctx.startingQuestItems |= StartingPreludeOfLight.Value<uint8_t>()   << 11;
    ctx.startingQuestItems |= StartingZeldasLullaby.Value<uint8_t>()    << 12;
    ctx.startingQuestItems |= StartingEponasSong.Value<uint8_t>()       << 13;
    ctx.startingQuestItems |= StartingSariasSong.Value<uint8_t>()       << 14;
    ctx.startingQuestItems |= StartingSunsSong.Value<uint8_t>()         << 15;
    ctx.startingQuestItems |= StartingSongOfTime.Value<uint8_t>()       << 16;
    ctx.startingQuestItems |= StartingSongOfStorms.Value<uint8_t>()     << 17;
    ctx.startingQuestItems |= StartingShardOfAgony.Value<uint8_t>()     << 21;
    ctx.startingDungeonReward |= StartingKokiriEmerald.Value<uint8_t>()    << 18;
    ctx.startingDungeonReward |= StartingGoronRuby.Value<uint8_t>()        << 19;
    ctx.startingDungeonReward |= StartingZoraSapphire.Value<uint8_t>()     << 20;
    ctx.startingDungeonReward |= StartingLightMedallion.Value<uint8_t>()   << 5;
    ctx.startingDungeonReward |= StartingForestMedallion.Value<uint8_t>()  << 0;
    ctx.startingDungeonReward |= StartingFireMedallion.Value<uint8_t>()    << 1;
    ctx.startingDungeonReward |= StartingWaterMedallion.Value<uint8_t>()   << 2;
    ctx.startingDungeonReward |= StartingSpiritMedallion.Value<uint8_t>()  << 3;
    ctx.startingDungeonReward |= StartingShadowMedallion.Value<uint8_t>()  << 4;

    ctx.startingTokens        = StartingSkulltulaToken.Value<uint8_t>();

    //Give the Gerudo Membership Card if Gerudo Fortress is Open and Shuffle Gerudo Card is off
    if (GerudoFortress.Is(GERUDOFORTRESS_OPEN) && !ShuffleGerudoToken) {
        ctx.startingQuestItems |= 0x00400000;
    }

    //Starting Equipment
    ctx.startingEquipment |= StartingKokiriSword.Value<uint8_t>();
    ctx.startingEquipment |= (StartingBiggoronSword.Value<uint8_t>() ? 1: 0) << 2;
    ctx.startingEquipment |= StartingDekuShield.Value<uint8_t>()    << 4;
    ctx.startingEquipment |= StartingHylianShield.Value<uint8_t>()  << 5;
    ctx.startingEquipment |= StartingMirrorShield.Value<uint8_t>()  << 6;
    ctx.startingEquipment |= StartingGoronTunic.Value<uint8_t>()    << 9;
    ctx.startingEquipment |= StartingZoraTunic.Value<uint8_t>()     << 10;
    ctx.startingEquipment |= StartingIronBoots.Value<uint8_t>()     << 13;
    ctx.startingEquipment |= StartingHoverBoots.Value<uint8_t>()    << 14;

    //Starting Upgrades
    ctx.startingUpgrades |= StartingStrength.Value<uint8_t>() << 6;
    ctx.startingUpgrades |= StartingScale.Value<uint8_t>() << 9;
    ctx.startingUpgrades |= StartingWallet.Value<uint8_t>() << 12;

    return ctx;
  }

  //Include and Lock the desired locations
  static void IncludeAndHide(std::vector<uint32_t> locations) {
    for (uint32_t loc : locations) {
      Location(loc)->GetExcludedOption()->SetSelectedIndex(INCLUDE);
      Location(loc)->GetExcludedOption()->Hide();
    }
  }

  //Unlock the desired locations
  static void Unhide(std::vector<uint32_t> locations) {
    for (uint32_t loc : locations) {
      Location(loc)->GetExcludedOption()->Unhide();
    }
  }

  //Used for toggle options, enables/disables all the options between the 2 given
  void ToggleSet(std::vector<Option *> optionsList, Option* toggleOption, Option* firstOptionInSet, Option* lastOptionInSet) {
    uint32_t i;
    for(i = 0; i < optionsList.size(); i++){
      if(optionsList[i] == firstOptionInSet)
        break;
    }
    for(; i < optionsList.size(); i++){
      switch(toggleOption->Value<uint8_t>()) {
        case 0:
          optionsList[i]->Hide();
          optionsList[i]->SetSelectedIndex(0);
          break;
        case 1:
          optionsList[i]->Hide();
          optionsList[i]->SetSelectedIndex(1);
          break;
        case 2:
          optionsList[i]->Unhide();
          break;
      }
      if(optionsList[i] == lastOptionInSet)
        return;
    }
  }

  //This function will hide certain locations from the Excluded Locations
  //menu if the player's current settings would require non-junk to be placed
  //at those locations. Excluded locations will have junk placed at them.
  void ResolveExcludedLocationConflicts() {

    std::vector<uint32_t> shopLocations = GetLocations(everyPossibleLocation, Category::cShop);
    //For now, just always hide shop locations, as not sure how to handle hiding them-
    //1-4 should always be hidden, while the others should be settings dependent, but random shopsanity makes that more complicated...
    //Excluded shop locations are also wonky
    IncludeAndHide(shopLocations);

    //Force include song locations
    std::vector<uint32_t> songLocations = GetLocations(everyPossibleLocation, Category::cSong);
    std::vector<uint32_t> songDungeonRewards = GetLocations(everyPossibleLocation, Category::cSongDungeonReward);

    //Unhide all song locations, then lock necessary ones
    Unhide(songLocations);
    Unhide(songDungeonRewards);

    if (ShuffleSongs.Is(SONGSHUFFLE_SONG_LOCATIONS)) {
      IncludeAndHide(songLocations);
    } else if (ShuffleSongs.Is(SONGSHUFFLE_DUNGEON_REWARDS)) {
      IncludeAndHide(songDungeonRewards);
    }

    //Force Include Vanilla Skulltula locations
    std::vector<uint32_t> skulltulaLocations = GetLocations(everyPossibleLocation, Category::cSkulltula);
    Unhide(skulltulaLocations);
    if (Tokensanity.IsNot(TOKENSANITY_ALL_TOKENS)) {
      if (Tokensanity.Is(TOKENSANITY_OVERWORLD)) {
        //filter overworld skulls so we're just left with dungeons
        FilterAndEraseFromPool(skulltulaLocations, [](const uint32_t loc){return Location(loc)->GetScene() >= 0x0A;});
      } else if (Tokensanity.Is(TOKENSANITY_DUNGEONS)) {
        //filter dungeon skulls so we're just left with overworld
        FilterAndEraseFromPool(skulltulaLocations, [](const uint32_t loc){return Location(loc)->GetScene() < 0x0A;});
      }
      IncludeAndHide(skulltulaLocations);
    }

    //Force Include scrubs if Scrubsanity is Off
    std::vector<uint32_t> scrubLocations = GetLocations(everyPossibleLocation, Category::cDekuScrub, Category::cDekuScrubUpgrades);
    if (Scrubsanity.Is(OFF)) {
      IncludeAndHide(scrubLocations);
    } else {
      Unhide(scrubLocations);
    }

    //Force include Cows if Shuffle Cows is Off
    std::vector<uint32_t> cowLocations = GetLocations(everyPossibleLocation, Category::cCow);
    if (ShuffleCows) {
      Unhide(cowLocations);
    } else {
      IncludeAndHide(cowLocations);
    }

    //Force include the Kokiri Sword Chest if Shuffle Kokiri Sword is Off
    if (ShuffleKokiriSword) {
      Unhide({KF_KOKIRI_SWORD_CHEST});
    } else {
      IncludeAndHide({KF_KOKIRI_SWORD_CHEST});
    }

    //Force include the ocarina locations if Shuffle Ocarinas is Off
    std::vector<uint32_t> ocarinaLocations = {LW_GIFT_FROM_SARIA, HF_OCARINA_OF_TIME_ITEM};
    if (ShuffleOcarinas) {
      Unhide(ocarinaLocations);
    } else {
      IncludeAndHide(ocarinaLocations);
    }

    //Force include Malon if Shuffle Weird Egg is Off
    if (ShuffleWeirdEgg) {
      Unhide({HC_MALON_EGG});
    } else {
      IncludeAndHide({HC_MALON_EGG});
    }

    //Force include Gerudo Membership Card Location if it's not shuffled
    if (ShuffleGerudoToken) {
      Unhide({GF_GERUDO_MEMBERSHIP_CARD});
    } else {
      IncludeAndHide({GF_GERUDO_MEMBERSHIP_CARD});
    }

    //Force include Magic Bean salesman if Shuffle Magic Beans is off
    if (ShuffleMagicBeans) {
      Unhide({ZR_MAGIC_BEAN_SALESMAN});
    } else {
      IncludeAndHide({ZR_MAGIC_BEAN_SALESMAN});
    }

    //Force include Medigoron, Granny and Carpet salesman if Shuffle Merchants is off
    if (ShuffleMerchants.IsNot(SHUFFLEMERCHANTS_OFF)) {
      Unhide({GC_MEDIGORON});
      Unhide({KAK_GRANNYS_SHOP});
      Unhide({WASTELAND_BOMBCHU_SALESMAN});
    } else {
      IncludeAndHide({GC_MEDIGORON});
      IncludeAndHide({KAK_GRANNYS_SHOP});
      IncludeAndHide({WASTELAND_BOMBCHU_SALESMAN});
    }

    //Force include frog song rupees if they're not shuffled
    if (ShuffleFrogSongRupees) {
      Unhide({ZR_FROGS_ZELDAS_LULLABY});
      Unhide({ZR_FROGS_EPONAS_SONG});
      Unhide({ZR_FROGS_SARIAS_SONG});
      Unhide({ZR_FROGS_SUNS_SONG});
      Unhide({ZR_FROGS_SONG_OF_TIME});
    } else {
      IncludeAndHide({ZR_FROGS_ZELDAS_LULLABY});
      IncludeAndHide({ZR_FROGS_EPONAS_SONG});
      IncludeAndHide({ZR_FROGS_SARIAS_SONG});
      IncludeAndHide({ZR_FROGS_SUNS_SONG});
      IncludeAndHide({ZR_FROGS_SONG_OF_TIME});
    }

    //Force include adult trade quest if Shuffle Adult Trade Quest is off
    std::vector<uint32_t> adultTradeLocations = {KAK_TRADE_POCKET_CUCCO, LW_TRADE_COJIRO, KAK_TRADE_ODD_MUSHROOM, LW_TRADE_ODD_POTION, GV_TRADE_SAW, DMT_TRADE_BROKEN_SWORD, ZD_TRADE_PRESCRIPTION, LH_TRADE_FROG, DMT_TRADE_EYEDROPS};
    if (ShuffleAdultTradeQuest) {
      Unhide(adultTradeLocations);
    } else {
      IncludeAndHide(adultTradeLocations);
    }

    //Force include Chest Game keys if Shuffle Chest Minigame is off
    std::vector<uint32_t> ChestMinigameLocations = {
        MARKET_TREASURE_CHEST_GAME_ITEM_1, MARKET_TREASURE_CHEST_GAME_ITEM_2, MARKET_TREASURE_CHEST_GAME_ITEM_3,
        MARKET_TREASURE_CHEST_GAME_ITEM_4, MARKET_TREASURE_CHEST_GAME_ITEM_5
    };
    if (ShuffleChestMinigame) {
      Unhide(ChestMinigameLocations);
    } else {
      IncludeAndHide(ChestMinigameLocations);
    }

    //Force include 100 GS reward if it isn't shuffled
    if (Shuffle100GSReward) {
      Unhide({KAK_100_GOLD_SKULLTULA_REWARD});
    } else {
      IncludeAndHide({KAK_100_GOLD_SKULLTULA_REWARD});
    }

    //Force include Triforce Hunt if it's off
    if (TriforceHunt) {
      Unhide({ TRIFORCE_COMPLETED });
    } else {
      IncludeAndHide({ TRIFORCE_COMPLETED });
    }

    //Force include Map and Compass Chests when Vanilla
    std::vector<uint32_t> mapChests = GetLocations(everyPossibleLocation, Category::cVanillaMap);
    std::vector<uint32_t> compassChests = GetLocations(everyPossibleLocation, Category::cVanillaCompass);
    if (MapsAndCompasses.Is(MAPSANDCOMPASSES_VANILLA)) {
      IncludeAndHide(mapChests);
      IncludeAndHide(compassChests);
    } else {
      Unhide(mapChests);
      Unhide(compassChests);
    }

    //Force include Vanilla Small Key Locations (except gerudo Fortress) on Vanilla Keys
    std::vector<uint32_t> smallKeyChests = GetLocations(everyPossibleLocation, Category::cVanillaSmallKey);
    if (Keysanity.Is(KEYSANITY_VANILLA)) {
      IncludeAndHide(smallKeyChests);
    } else {
      Unhide(smallKeyChests);
    }

    //Force include Gerudo Fortress carpenter fights if GF Small Keys are Vanilla
    std::vector<uint32_t> vanillaGFKeyLocations = GetLocations(everyPossibleLocation, Category::cVanillaGFSmallKey);
    if (GerudoKeys.Is(GERUDOKEYS_VANILLA)) {
      IncludeAndHide(vanillaGFKeyLocations);
    } else {
      Unhide(vanillaGFKeyLocations);
    }

    //Force include Boss Key Chests if Boss Keys are Vanilla
    std::vector<uint32_t> bossKeyChests = GetLocations(everyPossibleLocation, Category::cVanillaBossKey);
    if (BossKeysanity.Is(BOSSKEYSANITY_VANILLA)) {
      IncludeAndHide(bossKeyChests);
    } else {
      Unhide(bossKeyChests);
    }

    //Force include Ganons Boss Key Chest if ganons boss key has to be there
    if (GanonsBossKey.Is(GANONSBOSSKEY_VANILLA)) {
      IncludeAndHide({GANONS_TOWER_BOSS_KEY_CHEST});
    } else {
      Unhide({GANONS_TOWER_BOSS_KEY_CHEST});
    }

    //Force include Light Arrow item if ganons boss key has to be there
    if (GanonsBossKey.Value<uint8_t>() >= GANONSBOSSKEY_LACS_VANILLA) {
      IncludeAndHide({TOT_LIGHT_ARROWS_CUTSCENE});
    } else {
      Unhide({TOT_LIGHT_ARROWS_CUTSCENE});
    }
  }

  uint8_t DungeonsOfType(uint8_t type) {
    uint8_t count = 0;

    for (Option *option : dungeonOptions) {
      count += (option->Value<uint8_t>() == type) ? 1 : 0;
    }

    return count;
  }

  //Hide certain settings if they aren't relevant or Lock settings if they
  //can't be changed due to another setting that was chosen. (i.e. Closed Forest
  //will force Starting Age to Child).
  void ForceChange(uint32_t kDown, Option* currentSetting) {

    // If Zora's Fountain is open, hide and un-select Ruto's Letter. If not open, hide and un-select bottle 4 instead
    if (ZorasFountain.Is(ZORASFOUNTAIN_OPEN)) {
      startingItemsOptions[23]->Unhide();
      startingItemsOptions[24]->Hide();
      startingItemsOptions[24]->SetSelectedIndex(0);
    } else {
      startingItemsOptions[24]->Unhide();
      startingItemsOptions[23]->Hide();
      startingItemsOptions[23]->SetSelectedIndex(0);
    }

    //Only hide the options for now, select them later in UpdateSettings()
    RandomizeAllSettings();

    //Only go through options if all settings are not randomized
    if (!RandomizeOpen) {
      //Adult is not compatible with Closed Forest
      if (OpenForest.Is(OPENFOREST_CLOSED)) {
        StartingAge.SetSelectedIndex(AGE_CHILD);
      }

      //Adult is also not compatible with the following combination:
      //DoT:Intended, ShuffleOcarinas:false, Logic:Glitchless
      if (OpenDoorOfTime.Is(OPENDOOROFTIME_CLOSED) && !ShuffleOcarinas &&
        Logic.Is(LOGIC_GLITCHLESS)) {
          StartingAge.SetSelectedIndex(AGE_CHILD);
        }

      //Only show stone count option if Stones is selected
      if (Bridge.Is(RAINBOWBRIDGE_STONES)) {
        BridgeStoneCount.Unhide();
      } else {
        BridgeStoneCount.SetSelectedIndex(3);
        BridgeStoneCount.Hide();
      }

      //Only show medallion count option if Medallions is selected
      if (Bridge.Is(RAINBOWBRIDGE_MEDALLIONS)) {
        BridgeMedallionCount.Unhide();
      } else {
        BridgeMedallionCount.Hide();
        BridgeMedallionCount.SetSelectedIndex(6);
      }

      //Only show reward count option if Rewards is selected
      if (Bridge.Is(RAINBOWBRIDGE_REWARDS)) {
        BridgeRewardCount.Unhide();
      } else {
        BridgeRewardCount.SetSelectedIndex(9);
        BridgeRewardCount.Hide();
      }

      //Only show reward count option if Rewards is selected
      if (Bridge.Is(RAINBOWBRIDGE_DUNGEONS)) {
        BridgeDungeonCount.Unhide();
      } else {
        BridgeDungeonCount.SetSelectedIndex(8);
        BridgeDungeonCount.Hide();
      }

      //Only show token count option if Tokens is selected
      if (Bridge.Is(RAINBOWBRIDGE_TOKENS)) {
        BridgeTokenCount.Unhide();
      } else {
        BridgeTokenCount.SetSelectedIndex(1);
        BridgeTokenCount.Hide();
      }

      //Only show Trial Count option if Random Trial Count is off
      if (RandomGanonsTrials) {
        GanonsTrialsCount.SetSelectedIndex(6);
        GanonsTrialsCount.Hide();
      } else {
        GanonsTrialsCount.Unhide();
      }
    }

    //Only go through options if all settings are not randomized
    if (!RandomizeWorld) {
      //Show Shuffle options when Shuffle Entrances is On
      if (ShuffleEntrances) {
        ShuffleDungeonEntrances.Unhide();
        ShuffleBossEntrances.Unhide();
        ShuffleOverworldEntrances.Unhide();
        ShuffleInteriorEntrances.Unhide();
        ShuffleGrottoEntrances.Unhide();
        ShuffleOwlDrops.Unhide();
        ShuffleWarpSongs.Unhide();
        ShuffleOverworldSpawns.Unhide();
        MixedEntrancePools.Unhide();
        DecoupleEntrances.Unhide();
      } else {
        ShuffleDungeonEntrances.SetSelectedIndex(SHUFFLEDUNGEONS_OFF);
        ShuffleDungeonEntrances.Hide();
        ShuffleBossEntrances.SetSelectedIndex(SHUFFLEBOSSES_OFF);
        ShuffleBossEntrances.Hide();
        ShuffleOverworldEntrances.SetSelectedIndex(OFF);
        ShuffleOverworldEntrances.Hide();
        ShuffleInteriorEntrances.SetSelectedIndex(SHUFFLEINTERIORS_OFF);
        ShuffleInteriorEntrances.Hide();
        ShuffleGrottoEntrances.SetSelectedIndex(OFF);
        ShuffleGrottoEntrances.Hide();
        ShuffleOwlDrops.SetSelectedIndex(OFF);
        ShuffleOwlDrops.Hide();
        ShuffleWarpSongs.SetSelectedIndex(OFF);
        ShuffleWarpSongs.Hide();
        ShuffleOverworldSpawns.SetSelectedIndex(OFF);
        ShuffleOverworldSpawns.Hide();
        MixedEntrancePools.SetSelectedIndex(OFF);
        MixedEntrancePools.Hide();
        DecoupleEntrances.SetSelectedIndex(OFF);
        DecoupleEntrances.Hide();
      }

      // Only show the options for mixing each pool if they're already being shuffled
      if (MixedEntrancePools) {
        if (ShuffleDungeonEntrances) {
          MixDungeons.Unhide();
        } else {
          MixDungeons.Hide();
          MixDungeons.SetSelectedIndex(OFF);
        }

        if (ShuffleOverworldEntrances) {
          MixOverworld.Unhide();
        } else {
          MixOverworld.Hide();
          MixOverworld.SetSelectedIndex(OFF);
        }

        if (ShuffleInteriorEntrances.IsNot(OFF)) {
          MixInteriors.Unhide();
        } else {
          MixInteriors.Hide();
          MixInteriors.SetSelectedIndex(OFF);
        }

        if (ShuffleGrottoEntrances) {
          MixGrottos.Unhide();
        } else {
          MixGrottos.Hide();
          MixGrottos.SetSelectedIndex(OFF);
        }
      } else {
        MixDungeons.Hide();
        MixDungeons.SetSelectedIndex(OFF);
        MixOverworld.Hide();
        MixOverworld.SetSelectedIndex(OFF);
        MixInteriors.Hide();
        MixInteriors.SetSelectedIndex(OFF);
        MixGrottos.Hide();
        MixGrottos.SetSelectedIndex(OFF);
      }
    }

    if (SetDungeonTypes) {
      for (Option *option : dungeonOptions) {
        option->Unhide();
      }

      // Restrict MQDungeonCount options so they can't be less than the number of dungeons set to MQ and can't be more
      // than the number of dungeons set to MQ plus the number of dungeons set to random
      uint8_t MQ = DungeonsOfType(1), R = DungeonsOfType(2);
      if (MQDungeonCount.Value<uint8_t>() < MQ) {
        MQDungeonCount.SetSelectedIndex((currentSetting == &MQDungeonCount) ? 13 : MQ);
      } else if (MQDungeonCount.Value<uint8_t>() != 13 && MQDungeonCount.Value<uint8_t>() > MQ + R) {
        MQDungeonCount.SetSelectedIndex((currentSetting == &MQDungeonCount) ? 13 : MQ + R);
      }
    } else {
      for (Option *option : dungeonOptions) {
        option->SetSelectedIndex(2);
        option->Hide();
      }
    }

    //Only go through options if all settings are not randomized
    if (!RandomizeShuffle) {
      // Ganon's Boss Key on 100 GS reward must also have the reward shuffled
      if (GanonsBossKey.Is(GANONSBOSSKEY_FINAL_GS_REWARD)) {
        Shuffle100GSReward.SetSelectedIndex(ON);
      }
    }

    //Force Link's Pocket Item to be a dungeon reward if Shuffle Rewards is end of dungeons
    if (ShuffleRewards.Is(REWARDSHUFFLE_END_OF_DUNGEON)) {
      LinksPocketItem.SetSelectedIndex(LINKSPOCKETITEM_DUNGEON_REWARD);
      if (RandomizeShuffle) {
        //Even if it is supposed to be locked, still hide it to keep the surprise

        LinksPocketItem.Hide();
      }
    }

    if (!RandomizeDungeon) {
      //Only show Stone Count if setting Ganons Boss Key to LACS Stones
      if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_STONES)) {
        LACSStoneCount.Unhide();
      } else {
        LACSStoneCount.SetSelectedIndex(3);
        LACSStoneCount.Hide();
      }
      
      //Only show Medallion Count if setting Ganons Boss Key to LACS Medallions
      if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_MEDALLIONS)) {
        LACSMedallionCount.Unhide();
      } else {
        LACSMedallionCount.SetSelectedIndex(6);
        LACSMedallionCount.Hide();
      }

      //Only show Reward Count if setting Ganons Boss Key to LACS Rewards
      if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_REWARDS)) {
        LACSRewardCount.Unhide();
      } else {
        LACSRewardCount.SetSelectedIndex(9);
        LACSRewardCount.Hide();
      }

      //Only show Dungeon Count if setting Ganons Boss Key to LACS Dungeons
      if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_DUNGEONS)) {
        LACSDungeonCount.Unhide();
      } else {
        LACSDungeonCount.SetSelectedIndex(8);
        LACSDungeonCount.Hide();
      }

      //Only show Token Count if setting Ganons Boss Key to LACS Tokens
      if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_TOKENS)) {
        LACSTokenCount.Unhide();
      } else {
        LACSTokenCount.SetSelectedIndex(100);
        LACSTokenCount.Hide();
      }

      // Only show dungeons when keyring selection is enabled
      if (KeyRings.Is(KEYRINGS_SELECTION)) {
        for (Option *option : keyRingOptions) {
          option->Unhide();
        }
      } else {
        for (Option *option : keyRingOptions) {
          option->Hide();
          option->SetSelectedIndex(0);
        }
      }
    }

    //Only show Skip Song Replays if Quick Text is enabled
    if (QuickText.Is(QUICKTEXT_VANILLA)) {
      SkipSongReplays.SetSelectedIndex(DONT_SKIP);
      SkipSongReplays.Hide();
    } else {
      SkipSongReplays.Unhide();
    }

    //Only show hint options if hints are enabled
    if (GossipStoneHints.Is(HINTS_NO_HINTS)) {
      ClearerHints.Hide();
      HintDistribution.Hide();
    } else {
      ClearerHints.Unhide();
      HintDistribution.Unhide();
    }

    //Manage toggle for item usability options
    ToggleSet(itemUsabilityOptions, &AgeItemsToggle, &StickAsAdult, &ZoraTunicAsChild);

    if (RemoveDoubleDefense) {
      StartingDoubleDefense.SetSelectedIndex(0);
    }

    ResolveExcludedLocationConflicts();
  }

  bool IsMQOption(Option *option) {
    return option == &MQDungeonCount  ||
           option == &SetDungeonTypes ||
           option == &MQDeku          ||
           option == &MQDodongo       ||
           option == &MQJabu          ||
           option == &MQForest        ||
           option == &MQFire          ||
           option == &MQWater         ||
           option == &MQSpirit        ||
           option == &MQShadow        ||
           option == &MQBotW          ||
           option == &MQIceCavern     ||
           option == &MQGTG           ||
           option == &MQCastle;
  }

  //Options that should be saved, set to default, then restored after finishing when vanilla logic is enabled
  std::vector<Option *> vanillaLogicDefaults = {
    &LinksPocketItem,
    &ShuffleRewards,
    &ShuffleSongs,
    &Shopsanity,
    &ShopsanityPrices,
    &ShopsanityPricesAffordable,
    &Scrubsanity,
    &ShuffleCows,
    &ShuffleMagicBeans,
    &ShuffleMerchants,
    &ShuffleFrogSongRupees,
    &ShuffleAdultTradeQuest,
    &Shuffle100GSReward,
    &GossipStoneHints,
  };

  // Randomizes all settings in a category if chosen
  // Hides all relevant options
  void RandomizeAllSettings(const bool selectOptions /*= false*/) {

    // Open Settings
    if (RandomizeOpen) {
      // Skip RandomizeOpen Option
      for (size_t i = 1; i < openOptions.size(); i++) {
        //hide options
        openOptions[i]->Hide();

        //randomize options
        if (selectOptions) {
          openOptions[i]->SetSelectedIndex(Random(0,openOptions[i]->GetOptionCount()));
        }
      }
      // Randomize Ganon Trials
      RandomGanonsTrials.SetSelectedIndex(ON);
    }
    else {
      for (size_t i = 1; i < openOptions.size(); i++) {
        openOptions[i]->Unhide();
      }
    }

    // World Settings
    if (RandomizeWorld) {
      // Skip RandomizeWorld Option
      for (size_t i=1; i < worldOptions.size(); i++) {
        // skip MQ options
        if (IsMQOption(worldOptions[i])) {
          continue;
        }
        worldOptions[i]->Hide();
        //randomize options
        if (selectOptions) {
          worldOptions[i]->SetSelectedIndex(Random(0,worldOptions[i]->GetOptionCount()));
        }
        // Sanity Check Closed Forest
        if (OpenForest.Is(OPENFOREST_CLOSED)) {
          StartingAge.SetSelectedIndex(AGE_CHILD);
        }
      }
    }
    else {
      for (size_t i=1; i < worldOptions.size(); i++) {
        if (IsMQOption(worldOptions[i])) {
          continue;
        }
        worldOptions[i]->Unhide();
      }
    }

    // Sanity Check Entrance Shuffling
    if (!ShuffleEntrances) {
      ShuffleDungeonEntrances.SetSelectedIndex(OFF);
      ShuffleBossEntrances.SetSelectedIndex(OFF);
      ShuffleOverworldEntrances.SetSelectedIndex(OFF);
      ShuffleInteriorEntrances.SetSelectedIndex(OFF);
      ShuffleGrottoEntrances.SetSelectedIndex(OFF);
      ShuffleOwlDrops.SetSelectedIndex(OFF);
      ShuffleWarpSongs.SetSelectedIndex(OFF);
      ShuffleOverworldSpawns.SetSelectedIndex(OFF);
      MixedEntrancePools.SetSelectedIndex(OFF);
      DecoupleEntrances.SetSelectedIndex(OFF);
    }

    if (!MixedEntrancePools) {
      MixDungeons.SetSelectedIndex(OFF);
      MixOverworld.SetSelectedIndex(OFF);
      MixInteriors.SetSelectedIndex(OFF);
      MixGrottos.SetSelectedIndex(OFF);
    }

    // Shuffle Settings
    if (RandomizeShuffle) {
      // Skip RandomizeShuffle Option
      for (size_t i=1; i < shuffleOptions.size(); i++) {
        shuffleOptions[i]->Hide();
        //randomize options
        if (selectOptions) {
          shuffleOptions[i]->SetSelectedIndex(Random(0,shuffleOptions[i]->GetOptionCount()));
        }
      }
      // Double check that this is the case in case of randomization on init
      if (ShuffleRewards.Is(REWARDSHUFFLE_END_OF_DUNGEON)) {
        LinksPocketItem.SetSelectedIndex(LINKSPOCKETITEM_DUNGEON_REWARD);
      }
    }
    else {
      for (size_t i=1; i < shuffleOptions.size(); i++) {
        shuffleOptions[i]->Unhide();
      }
    }

    // Dungeon Shuffle Settings
    if (RandomizeDungeon) {
      // Skip RandomizeDungeon Option
      for (size_t i=1; i < shuffleDungeonItemOptions.size(); i++) {
        shuffleDungeonItemOptions[i]->Hide();
        //randomize options
        if (selectOptions) {
          shuffleDungeonItemOptions[i]->SetSelectedIndex(Random(0,shuffleDungeonItemOptions[i]->GetOptionCount()));
        }
      }
    }
    else {
      for (size_t i=1; i < shuffleDungeonItemOptions.size(); i++) {
        shuffleDungeonItemOptions[i]->Unhide();
      }
    }

    //resolve any settings that need to change
    if (selectOptions) {
      ForceChange(0, nullptr);
    }
  }

  //Function to set flags depending on settings
  void UpdateSettings(std::unordered_map<RandomizerSettingKey, uint8_t> cvarSettings, std::set<RandomizerCheck> excludedLocations, std::set<RandomizerTrick> enabledTricks) {

    //Logic.SetSelectedIndex(cvarSettings[RSK_LOGIC_RULES]);
    //TODO switch back for glitch logic
    switch (cvarSettings[RSK_LOGIC_RULES]) {
      case RO_LOGIC_GLITCHLESS:
        Logic.SetSelectedIndex(0);
        break;
      case RO_LOGIC_NO_LOGIC:
        Logic.SetSelectedIndex(2);
        break;
    }

    LocationsReachable.SetSelectedIndex(cvarSettings[RSK_ALL_LOCATIONS_REACHABLE]);

    AddExcludedOptions();
    for (auto locationKey : everyPossibleLocation) {
      auto location = Location(locationKey);
      if (excludedLocations.count(location->GetRandomizerCheck())) {
        location->GetExcludedOption()->SetSelectedIndex(1);
      } else {
        location->GetExcludedOption()->SetSelectedIndex(0);
      }
    }

    //tricks
    
    for (auto randomizerTrick : enabledTricks) {
      trickOptionMap[randomizerTrick]->SetSelectedIndex(1);
    }
    

    OpenForest.SetSelectedIndex(cvarSettings[RSK_FOREST]);
    OpenKakariko.SetSelectedIndex(cvarSettings[RSK_KAK_GATE]);
    ZorasFountain.SetSelectedIndex(cvarSettings[RSK_ZORAS_FOUNTAIN]);
    OpenDoorOfTime.SetSelectedIndex(cvarSettings[RSK_DOOR_OF_TIME]);
    StartingAge.SetSelectedIndex(cvarSettings[RSK_STARTING_AGE]);
    GerudoFortress.SetSelectedIndex(cvarSettings[RSK_GERUDO_FORTRESS]);
    Bridge.SetSelectedIndex(cvarSettings[RSK_RAINBOW_BRIDGE]);
    BridgeStoneCount.SetSelectedIndex(cvarSettings[RSK_RAINBOW_BRIDGE_STONE_COUNT]);
    BridgeMedallionCount.SetSelectedIndex(cvarSettings[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT]);
    BridgeRewardCount.SetSelectedIndex(cvarSettings[RSK_RAINBOW_BRIDGE_REWARD_COUNT]);
    BridgeDungeonCount.SetSelectedIndex(cvarSettings[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT]);
    BridgeTokenCount.SetSelectedIndex(cvarSettings[RSK_RAINBOW_BRIDGE_TOKEN_COUNT]);
    BridgeRewardOptions.SetSelectedIndex(cvarSettings[RSK_BRIDGE_OPTIONS]);
    if (cvarSettings[RSK_GANONS_TRIALS] == RO_GANONS_TRIALS_RANDOM_NUMBER) {
        RandomGanonsTrials.SetSelectedIndex(1);
    } else {
        RandomGanonsTrials.SetSelectedIndex(0);
    }
    if (cvarSettings[RSK_GANONS_TRIALS] == RO_GANONS_TRIALS_SKIP) {
        GanonsTrialsCount.SetSelectedIndex(0);
    } else {
        GanonsTrialsCount.SetSelectedIndex(cvarSettings[RSK_TRIAL_COUNT]);
    }
    SetDungeonTypes.SetSelectedIndex(false);
    if (cvarSettings[RSK_RANDOM_MQ_DUNGEONS] == RO_MQ_DUNGEONS_RANDOM_NUMBER) {
        MQDungeonCount.SetSelectedIndex(13);
    } else if (cvarSettings[RSK_RANDOM_MQ_DUNGEONS] == RO_MQ_DUNGEONS_NONE) {
        MQDungeonCount.SetSelectedIndex(0);
    } else if (cvarSettings[RSK_RANDOM_MQ_DUNGEONS] == RO_MQ_DUNGEONS_SET_NUMBER) {
        MQDungeonCount.SetSelectedIndex(cvarSettings[RSK_MQ_DUNGEON_COUNT]);
    } else if (cvarSettings[RSK_RANDOM_MQ_DUNGEONS] == RO_MQ_DUNGEONS_SELECTION) {
        SetDungeonTypes.SetSelectedIndex(true);
        MQDeku.SetSelectedIndex(cvarSettings[RSK_MQ_DEKU_TREE]);
        MQDodongo.SetSelectedIndex(cvarSettings[RSK_MQ_DODONGOS_CAVERN]);
        MQJabu.SetSelectedIndex(cvarSettings[RSK_MQ_JABU_JABU]);
        MQForest.SetSelectedIndex(cvarSettings[RSK_MQ_FOREST_TEMPLE]);
        MQFire.SetSelectedIndex(cvarSettings[RSK_MQ_FIRE_TEMPLE]);
        MQWater.SetSelectedIndex(cvarSettings[RSK_MQ_WATER_TEMPLE]);
        MQSpirit.SetSelectedIndex(cvarSettings[RSK_MQ_SPIRIT_TEMPLE]);
        MQShadow.SetSelectedIndex(cvarSettings[RSK_MQ_SHADOW_TEMPLE]);
        MQIceCavern.SetSelectedIndex(cvarSettings[RSK_MQ_ICE_CAVERN]);
        MQBotW.SetSelectedIndex(cvarSettings[RSK_MQ_BOTTOM_OF_THE_WELL]);
        MQGTG.SetSelectedIndex(cvarSettings[RSK_MQ_GTG]);
        MQCastle.SetSelectedIndex(cvarSettings[RSK_MQ_GANONS_CASTLE]);
    }
    ShuffleRewards.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_DUNGEON_REWARDS]);
    ShuffleSongs.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_SONGS]);
    Tokensanity.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_TOKENS]);
    Shopsanity.SetSelectedIndex(cvarSettings[RSK_SHOPSANITY]);
    ShopsanityPrices.SetSelectedIndex(cvarSettings[RSK_SHOPSANITY_PRICES]);
    ShopsanityPricesAffordable.SetSelectedIndex(cvarSettings[RSK_SHOPSANITY_PRICES_AFFORDABLE]);
    Scrubsanity.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_SCRUBS]);
    ShuffleCows.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_COWS]);
    ShuffleKokiriSword.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_KOKIRI_SWORD]);
    ShuffleOcarinas.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_OCARINA]);

    // Shuffle Entrances
    ShuffleEntrances.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_ENTRANCES]);
    ShuffleDungeonEntrances.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_DUNGEON_ENTRANCES]);
    ShuffleBossEntrances.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_BOSS_ENTRANCES]);
    ShuffleOverworldEntrances.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_OVERWORLD_ENTRANCES]);
    ShuffleInteriorEntrances.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_INTERIOR_ENTRANCES]);
    ShuffleGrottoEntrances.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_GROTTO_ENTRANCES]);
    ShuffleOwlDrops.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_OWL_DROPS]);
    ShuffleWarpSongs.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_WARP_SONGS]);
    ShuffleOverworldSpawns.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_OVERWORLD_SPAWNS]);
    MixedEntrancePools.SetSelectedIndex(cvarSettings[RSK_MIXED_ENTRANCE_POOLS]);
    MixDungeons.SetSelectedIndex(cvarSettings[RSK_MIX_DUNGEON_ENTRANCES]);
    MixOverworld.SetSelectedIndex(cvarSettings[RSK_MIX_OVERWORLD_ENTRANCES]);
    MixInteriors.SetSelectedIndex(cvarSettings[RSK_MIX_INTERIOR_ENTRANCES]);
    MixGrottos.SetSelectedIndex(cvarSettings[RSK_MIX_GROTTO_ENTRANCES]);
    DecoupleEntrances.SetSelectedIndex(cvarSettings[RSK_DECOUPLED_ENTRANCES]);

    // if we skip child zelda, we start with zelda's letter, and malon starts
    // at the ranch, so we should *not* shuffle the weird egg
    if(cvarSettings[RSK_SKIP_CHILD_ZELDA]) {
      skipChildZelda = true;
      ShuffleWeirdEgg.SetSelectedIndex(0);
    } else {
      skipChildZelda = false;
      ShuffleWeirdEgg.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_WEIRD_EGG]);
    }
    SkipChildStealth.SetSelectedIndex(cvarSettings[RSK_SKIP_CHILD_STEALTH]);
    
    ShuffleGerudoToken.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD]);
    ShuffleFrogSongRupees.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_FROG_SONG_RUPEES]);

    ShuffleAdultTradeQuest.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_ADULT_TRADE]);
    ShuffleMagicBeans.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_MAGIC_BEANS]);
    ShuffleMerchants.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_MERCHANTS]);

    // Force 100 GS Shuffle if that's where Ganon's Boss Key is
    if (cvarSettings[RSK_GANONS_BOSS_KEY] == RO_GANON_BOSS_KEY_KAK_TOKENS) {
      Shuffle100GSReward.SetSelectedIndex(1);
    } else {
      Shuffle100GSReward.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_100_GS_REWARD]);
    }

    // the  checkbox works because 0 is "Off" and 1 is "Fairy Ocarina"
    StartingOcarina.SetSelectedIndex(cvarSettings[RSK_STARTING_OCARINA]);
    
    // the checkboxes work because 0 is "Off" and 1 is "On"
    StartingDekuShield.SetSelectedIndex(cvarSettings[RSK_STARTING_DEKU_SHIELD]);
    StartingKokiriSword.SetSelectedIndex(cvarSettings[RSK_STARTING_KOKIRI_SWORD]);

    StartingSkulltulaToken.SetSelectedIndex(cvarSettings[RSK_STARTING_SKULLTULA_TOKEN]);

    MapsAndCompasses.SetSelectedIndex(cvarSettings[RSK_STARTING_MAPS_COMPASSES]);

    StartingZeldasLullaby.SetSelectedIndex(cvarSettings[RSK_STARTING_ZELDAS_LULLABY]);
    StartingEponasSong.SetSelectedIndex(cvarSettings[RSK_STARTING_EPONAS_SONG]);
    StartingSariasSong.SetSelectedIndex(cvarSettings[RSK_STARTING_SARIAS_SONG]);
    StartingSunsSong.SetSelectedIndex(cvarSettings[RSK_STARTING_SUNS_SONG]);
    StartingSongOfTime.SetSelectedIndex(cvarSettings[RSK_STARTING_SONG_OF_TIME]);
    StartingSongOfStorms.SetSelectedIndex(cvarSettings[RSK_STARTING_SONG_OF_STORMS]);
    StartingMinuetOfForest.SetSelectedIndex(cvarSettings[RSK_STARTING_MINUET_OF_FOREST]);
    StartingBoleroOfFire.SetSelectedIndex(cvarSettings[RSK_STARTING_BOLERO_OF_FIRE]);
    StartingSerenadeOfWater.SetSelectedIndex(cvarSettings[RSK_STARTING_SERENADE_OF_WATER]);
    StartingRequiemOfSpirit.SetSelectedIndex(cvarSettings[RSK_STARTING_REQUIEM_OF_SPIRIT]);
    StartingNocturneOfShadow.SetSelectedIndex(cvarSettings[RSK_STARTING_NOCTURNE_OF_SHADOW]);
    StartingPreludeOfLight.SetSelectedIndex(cvarSettings[RSK_STARTING_PRELUDE_OF_LIGHT]);

    // RANDOTODO: Implement disabling ammo drops. Currently only "On" (index 0) or "On + Bombchus" (index 1) is implemented.
    AmmoDrops.SetSelectedIndex(cvarSettings[RSK_ENABLE_BOMBCHU_DROPS]);

    BombchusInLogic.SetSelectedIndex(cvarSettings[RSK_BOMBCHUS_IN_LOGIC]);

    StartingConsumables.SetSelectedIndex(cvarSettings[RSK_STARTING_CONSUMABLES]);
    StartingMaxRupees.SetSelectedIndex(cvarSettings[RSK_FULL_WALLETS]);
    
    GossipStoneHints.SetSelectedIndex(cvarSettings[RSK_GOSSIP_STONE_HINTS]);
    ClearerHints.SetSelectedIndex(cvarSettings[RSK_HINT_CLARITY]);
    AltarHintText.SetSelectedIndex(cvarSettings[RSK_TOT_ALTAR_HINT]);
    LightArrowHintText.SetSelectedIndex(cvarSettings[RSK_LIGHT_ARROWS_HINT]);
    DampeHintText.SetSelectedIndex(cvarSettings[RSK_DAMPES_DIARY_HINT]);
    GregHintText.SetSelectedIndex(cvarSettings[RSK_GREG_HINT]);
    SariaHintText.SetSelectedIndex(cvarSettings[RSK_SARIA_HINT]);
    FrogsHintText.SetSelectedIndex(cvarSettings[RSK_FROGS_HINT]);
    WarpSongHints.SetSelectedIndex(cvarSettings[RSK_WARP_SONG_HINTS]);
    Kak10GSHintText.SetSelectedIndex(cvarSettings[RSK_KAK_10_SKULLS_HINT]);
    Kak20GSHintText.SetSelectedIndex(cvarSettings[RSK_KAK_20_SKULLS_HINT]);
    Kak30GSHintText.SetSelectedIndex(cvarSettings[RSK_KAK_30_SKULLS_HINT]);
    Kak40GSHintText.SetSelectedIndex(cvarSettings[RSK_KAK_40_SKULLS_HINT]);
    Kak50GSHintText.SetSelectedIndex(cvarSettings[RSK_KAK_50_SKULLS_HINT]);
    ScrubHintText.SetSelectedIndex(cvarSettings[RSK_SCRUB_TEXT_HINT]);
    HintDistribution.SetSelectedIndex(cvarSettings[RSK_HINT_DISTRIBUTION]);
    BlueFireArrows.SetSelectedIndex(cvarSettings[RSK_BLUE_FIRE_ARROWS]);
    SunlightArrows.SetSelectedIndex(cvarSettings[RSK_SUNLIGHT_ARROWS]);

    ItemPoolValue.SetSelectedIndex(cvarSettings[RSK_ITEM_POOL]);
    IceTrapValue.SetSelectedIndex(cvarSettings[RSK_ICE_TRAPS]);

    Keysanity.SetSelectedIndex(cvarSettings[RSK_KEYSANITY]);
    GerudoKeys.SetSelectedIndex(cvarSettings[RSK_GERUDO_KEYS]);
    BossKeysanity.SetSelectedIndex(cvarSettings[RSK_BOSS_KEYSANITY]);
    if (cvarSettings[RSK_TRIFORCE_HUNT]) {
      GanonsBossKey.SetSelectedIndex(RO_GANON_BOSS_KEY_TRIFORCE_HUNT);
    } else {
      GanonsBossKey.SetSelectedIndex(cvarSettings[RSK_GANONS_BOSS_KEY]);
    }
    LACSStoneCount.SetSelectedIndex(cvarSettings[RSK_LACS_STONE_COUNT]);
    LACSMedallionCount.SetSelectedIndex(cvarSettings[RSK_LACS_MEDALLION_COUNT]);
    LACSRewardCount.SetSelectedIndex(cvarSettings[RSK_LACS_REWARD_COUNT]);
    LACSDungeonCount.SetSelectedIndex(cvarSettings[RSK_LACS_DUNGEON_COUNT]);
    LACSTokenCount.SetSelectedIndex(cvarSettings[RSK_LACS_TOKEN_COUNT]);
    LACSRewardOptions.SetSelectedIndex(cvarSettings[RSK_LACS_OPTIONS]);

    KeyRings.SetSelectedIndex(cvarSettings[RSK_KEYRINGS]);
    KeyRingsRandomCount.SetSelectedIndex(cvarSettings[RSK_KEYRINGS_RANDOM_COUNT]);
    RingFortress.SetSelectedIndex(cvarSettings[RSK_KEYRINGS_GERUDO_FORTRESS]);
    RingForest.SetSelectedIndex(cvarSettings[RSK_KEYRINGS_FOREST_TEMPLE]);
    RingFire.SetSelectedIndex(cvarSettings[RSK_KEYRINGS_FIRE_TEMPLE]);
    RingWater.SetSelectedIndex(cvarSettings[RSK_KEYRINGS_WATER_TEMPLE]);
    RingSpirit.SetSelectedIndex(cvarSettings[RSK_KEYRINGS_SPIRIT_TEMPLE]);
    RingShadow.SetSelectedIndex(cvarSettings[RSK_KEYRINGS_SHADOW_TEMPLE]);
    RingWell.SetSelectedIndex(cvarSettings[RSK_KEYRINGS_BOTTOM_OF_THE_WELL]);
    RingGtg.SetSelectedIndex(cvarSettings[RSK_KEYRINGS_GTG]);
    RingCastle.SetSelectedIndex(cvarSettings[RSK_KEYRINGS_GANONS_CASTLE]);

    NumRequiredCuccos.SetSelectedIndex(cvarSettings[RSK_CUCCO_COUNT]);
    BigPoeTargetCount.SetSelectedIndex(cvarSettings[RSK_BIG_POE_COUNT]-1);

    SkipEponaRace.SetSelectedIndex(cvarSettings[RSK_SKIP_EPONA_RACE]);

    SkipTowerEscape.SetSelectedIndex(cvarSettings[RSK_SKIP_TOWER_ESCAPE]);

    CompleteMaskQuest.SetSelectedIndex(cvarSettings[RSK_COMPLETE_MASK_QUEST]);

    FreeScarecrow.SetSelectedIndex(cvarSettings[RSK_SKIP_SCARECROWS_SONG]);

    EnableGlitchCutscenes.SetSelectedIndex(cvarSettings[RSK_ENABLE_GLITCH_CUTSCENES]);

    NightGSExpectSuns.SetSelectedIndex(cvarSettings[RSK_SKULLS_SUNS_SONG]);

    LinksPocketItem.SetSelectedIndex(cvarSettings[RSK_LINKS_POCKET]);

    // RANDOTODO implement chest shuffle with keysanity
    // ShuffleChestMinigame.SetSelectedIndex(cvarSettings[RSK_SHUFFLE_CHEST_MINIGAME]);

    RandomizeAllSettings(true); //now select any random options instead of just hiding them

    //shuffle the dungeons and then set MQ for as many as necessary
    auto dungeons = dungeonList;
    if (SetDungeonTypes) {
      MQSet = MQDungeonCount.Value<uint8_t>();
      uint8_t dungeonCount = 0;
      std::vector<uint8_t> randMQOption = {};

      for (size_t i = 0; i < dungeons.size(); i++) {
        dungeons[i]->ClearMQ();
        // Dungeon mode assumed known at first
        DungeonModesKnown[i] = true;

        switch (dungeonOptions[i]->Value<uint8_t>()) {
          case 1:
            // Set MQ dungeons and track how many have been set
            dungeons[i]->SetMQ();
            dungeonCount++;
            break;
          case 2:
            // Track indexes of dungeons set to random
            randMQOption.push_back(i);
            // Dungeon mode will be unknown for this dungeon
            DungeonModesKnown[i] = false;
            break;
        }
      }

      // Add more MQ dungeons from the pool set to random until the MQ dungeon count is reached
      Shuffle(randMQOption);

      if (MQSet == 13) {
        MQSet = dungeonCount + Random(0, randMQOption.size() + 1);
      }

      for (uint8_t i = 0; dungeonCount < MQSet; i++) {
        dungeons[randMQOption[i]]->SetMQ();
        dungeonCount++;
      }
    } else {
      Shuffle(dungeons);
      MQSet = MQDungeonCount.Value<uint8_t>();

      //Clear MQ dungeons
      for (size_t i = 0; i < dungeons.size(); i++) {
        dungeons[i]->ClearMQ();
      }

      // If we won't be randomly choosing any MQ dungeons, or if we'll be setting
      // all dungeons to MQ, dungeon modes are assumed known
      bool allDungeonModesKnown = MQSet == 0 || MQSet == dungeons.size();
      for (uint8_t i = 0; i < sizeof(DungeonModesKnown); ++i) {
        DungeonModesKnown[i] = allDungeonModesKnown;
      }

      //Set appropriate amount of MQ dungeons
      if (MQSet == 13) {
        MQSet = Random(0, 13);
      }
      for (uint8_t i = 0; i < MQSet; i++) {
        dungeons[i]->SetMQ();
      }
    }

    TriforceHunt.SetSelectedIndex(cvarSettings[RSK_TRIFORCE_HUNT]);
    TriforceHuntTotal.SetSelectedIndex(cvarSettings[RSK_TRIFORCE_HUNT_PIECES_TOTAL]);
    TriforceHuntRequired.SetSelectedIndex(cvarSettings[RSK_TRIFORCE_HUNT_PIECES_REQUIRED]);

    //Set key ring for each dungeon
    for (size_t i = 0; i < dungeons.size(); i++) {
      dungeons[i]->ClearKeyRing();
    }
    if (KeyRings) {
      // Random Key Rings
      if (KeyRings.Is(KEYRINGS_RANDOM) || KeyRings.Is(KEYRINGS_RANDOM_COUNT)) {
        auto keyRings = keyRingOptions;
        if (GerudoFortress.Is(GERUDOFORTRESS_NORMAL) && GerudoKeys.IsNot(GERUDOKEYS_VANILLA)) {
          keyRings.push_back(&RingFortress);
        }
        int keyRingCount = KeyRings.Is(KEYRINGS_RANDOM_COUNT) ? KeyRingsRandomCount.Value<uint8_t>() : Random(0, keyRings.size());
        Shuffle(keyRings);
        for (size_t i = 0; i < keyRingCount; i++) {
          keyRings[i]->SetSelectedIndex(ON);
        }
      }
      if (RingWell) {
        BottomOfTheWell.SetKeyRing();
      }
      if (RingForest) {
        ForestTemple.SetKeyRing();
      }
      if (RingFire) {
        FireTemple.SetKeyRing();
      }
      if (RingWater) {
        WaterTemple.SetKeyRing();
      }
      if (RingSpirit) {
        SpiritTemple.SetKeyRing();
      }
      if (RingShadow) {
        ShadowTemple.SetKeyRing();
      }
      if (RingGtg) {
        GerudoTrainingGrounds.SetKeyRing();
      }
      if (RingCastle) {
        GanonsCastle.SetKeyRing();
      }
    }

    //shuffle the trials then require the amount set in GanonsTrialsCount
    auto trials = trialList;
    Shuffle(trials);

    //clear required trials
    for (auto& trial : trials) {
      trial->SetAsSkipped();
    }

    //Set appropriate amount of required trials
    if (RandomGanonsTrials) {
      GanonsTrialsCount.SetSelectedIndex(Random(0, GanonsTrialsCount.GetOptionCount()));
    }
    for (uint8_t i = 0; i < GanonsTrialsCount.Value<uint8_t>(); i++) {
      trials[i]->SetAsRequired();
    }

    // If any ER option is on that would allow you to escape forest, then we should set closed forest to closed deku
    if (OpenForest.Is(OPENFOREST_CLOSED) &&
        (ShuffleInteriorEntrances.Is(SHUFFLEINTERIORS_ALL) || ShuffleOverworldEntrances || ShuffleOverworldSpawns ||
         DecoupleEntrances || MixedEntrancePools)) {
        OpenForest.SetSelectedIndex(OPENFOREST_CLOSED_DEKU);
    }

    if (StartingAge.Is(AGE_RANDOM)) {
      int choice = Random(0, 2); //50% chance of each
      if (choice == 0) {
        ResolvedStartingAge = AGE_CHILD;
        StartingAge.SetSelectedIndex(AGE_CHILD);
      }
      else {
        ResolvedStartingAge = AGE_ADULT;
        StartingAge.SetSelectedIndex(AGE_ADULT);
      }
    }
    else {
      ResolvedStartingAge = StartingAge.Value<uint8_t>();
    }

    HasNightStart = StartingTime.Is(STARTINGTIME_NIGHT);

    if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_STONES)) {
      LACSCondition = LACSCONDITION_STONES;
    } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_MEDALLIONS)) {
      LACSCondition = LACSCONDITION_MEDALLIONS;
    } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_REWARDS)) {
      LACSCondition = LACSCONDITION_REWARDS;
    } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_DUNGEONS)) {
      LACSCondition = LACSCONDITION_DUNGEONS;
    } else if (GanonsBossKey.Is(GANONSBOSSKEY_LACS_TOKENS)) {
      LACSCondition = LACSCONDITION_TOKENS;
    } else {
      LACSCondition = LACSCONDITION_VANILLA;
    }

    //If vanilla logic, we want to set all settings which unnecessarily modify vanilla behavior to off
    if (Logic.Is(LOGIC_VANILLA)) {
      for (Option* setting : vanillaLogicDefaults) {
        setting->SetDelayedOption();
        setting->SetSelectedIndex(0);
      }
      Keysanity.SetDelayedOption();
      Keysanity.SetSelectedIndex(3); //Set small keys to any dungeon so FiT basement door will be locked
    }
  }

  //If this is an option menu, return the options
  //Else, recursively call each sub menu of this sub menu
  const std::vector<Menu*> GetMenusRecursive(Menu* menu) {
    std::vector<Menu*> menus;
    if (menu->mode == OPTION_SUB_MENU) {
      menus.push_back(menu);
    } else if (menu->mode == SUB_MENU) {
        for (Menu* subMenu : *menu->itemsList) {
          std::vector<Menu*> foundMenus = GetMenusRecursive(subMenu);
          menus.insert(menus.end(), foundMenus.begin(), foundMenus.end());
        }
    }
    return menus;
  }

  //Recursively look through each menu from the main menu to get all settings
  const std::vector<Menu*> GetAllOptionMenus() {
    std::vector<Menu*> allMenus;
    for (Menu* menu : Settings::mainMenu) {
      std::vector<Menu*> foundMenus = GetMenusRecursive(menu);
      allMenus.insert(allMenus.end(), foundMenus.begin(), foundMenus.end());
    }
    return allMenus;
  }

} // namespace Settings
