#include <unordered_map>
#include "static_data.h"
#include <spdlog/spdlog.h>

namespace Rando {

std::unordered_map<uint32_t, CustomMessage> StaticData::hintTypeNames = {
    {HINT_TYPE_HINT_KEY, CustomMessage("Message")},
    {HINT_TYPE_AREA, CustomMessage("Area")},
    {HINT_TYPE_ITEM, CustomMessage("Item")},
    {HINT_TYPE_TRIAL, CustomMessage("Trial")},
    {HINT_TYPE_ENTRANCE, CustomMessage("Entrance")},
    {HINT_TYPE_ITEM_AREA, CustomMessage("Item Area")},
    {HINT_TYPE_ALTAR_CHILD, CustomMessage("Child Altar")},
    {HINT_TYPE_ALTAR_ADULT, CustomMessage("Adult Altar")},
    {HINT_TYPE_WOTH,  CustomMessage("Way of the Hero")},
    {HINT_TYPE_FOOLISH, CustomMessage("Foolish")},
    {HINT_TYPE_MESSAGE, CustomMessage("Hardcoded Message")}
};

//RANDOTODO When dynamic grotto check names are done, apply it to these hints
//RANDOTODO Translations
std::unordered_map<uint32_t, CustomMessage> StaticData::hintNames = {
    {RH_NONE, CustomMessage("ERROR HINT")},
    {RH_KF_DEKU_TREE_LEFT_GOSSIP_STONE, CustomMessage("KF Left Near Deku Gossip Stone")}, //RANDOTODO find cardinal direction
    {RH_KF_DEKU_TREE_RIGHT_GOSSIP_STONE, CustomMessage("KF Right Near Deku Gossip Stone")},
    {RH_KF_GOSSIP_STONE, CustomMessage("KF Gossip Stone")},
    {RH_KF_STORMS_GROTTO_GOSSIP_STONE, CustomMessage("KF Storms Grotto Gossip Stone")},
    {RH_LW_GOSSIP_STONE, CustomMessage("LW Gossip Stone")},
    {RH_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE, CustomMessage("LW Near Shortcuts Grotto Gossip Stone")},
    {RH_SFM_MAZE_NEAR_LW_GOSSIP_STONE, CustomMessage("SFM Near LW Gossip Stone")},
    {RH_SFM_MAZE_CENTER_GOSSIP_STONE, CustomMessage("SFM Center Gossip Stone")},
    {RH_SFM_SARIA_GOSSIP_STONE, CustomMessage("SFM Near Saria Gossip Stone")},
    {RH_HF_COW_GROTTO_GOSSIP_STONE, CustomMessage("HF Cow Grotto Gossip Stone")},
    {RH_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE, CustomMessage("HF Near Market Grotto Gossip Stone")},
    {RH_HF_OPEN_GROTTO_GOSSIP_STONE, CustomMessage("HF Open Grotto Gossip Stone")},
    {RH_HF_SOUTHEAST_GROTTO_GOSSIP_STONE, CustomMessage("HF Southeast Grotto Gossip Stone")},
    {RH_TOT_LEFT_CENTER_GOSSIP_STONE, CustomMessage("Market Left Center Gossip Stone")},
    {RH_TOT_LEFTMOST_GOSSIP_STONE, CustomMessage("Market Leftmost Center Gossip Stone")},
    {RH_TOT_RIGHT_CENTER_GOSSIP_STONE, CustomMessage("Market Right Center Gossip Stone")},
    {RH_TOT_RIGHTMOST_GOSSIP_STONE, CustomMessage("Market Rightmost Gossip Stone")},
    {RH_HC_MALON_GOSSIP_STONE, CustomMessage("HC Near Malon Gossip Stone")},
    {RH_HC_ROCK_WALL_GOSSIP_STONE, CustomMessage("HC Rock Wall Gossip Stone")},
    {RH_HC_STORMS_GROTTO_GOSSIP_STONE, CustomMessage("HC Storm Grotto Gossip Stone")},
    {RH_KAK_OPEN_GROTTO_GOSSIP_STONE, CustomMessage("Kak Open Grotto Gossip Stone")},
    {RH_GRAVEYARD_GOSSIP_STONE, CustomMessage("Graveyard Gossip Stone")},
    {RH_DMT_GOSSIP_STONE, CustomMessage("DMT Gossip Stone")},
    {RH_DMT_STORMS_GROTTO_GOSSIP_STONE, CustomMessage("DMT Storms Grotto Gossip Stone")},
    {RH_GC_MAZE_GOSSIP_STONE, CustomMessage("GC Maze Gossip Stone")},
    {RH_GC_MEDIGORON_GOSSIP_STONE, CustomMessage("GC Medigoron Gossip Stone")},
    {RH_DMC_GOSSIP_STONE, CustomMessage("DMC Gossip Stone")},
    {RH_DMC_UPPER_GROTTO_GOSSIP_STONE, CustomMessage("DMC Upper Grotto Gossip Stone")},
    {RH_ZR_NEAR_DOMAIN_GOSSIP_STONE, CustomMessage("ZR Near Domain Gossip Stone")},
    {RH_ZR_NEAR_GROTTOS_GOSSIP_STONE, CustomMessage("ZR Near Grottos Gossip Stone")},
    {RH_ZR_OPEN_GROTTO_GOSSIP_STONE, CustomMessage("ZR Open Grotto Gossip Stone")},
    {RH_ZD_GOSSIP_STONE, CustomMessage("ZD Gossip Stone")},
    {RH_ZF_JABU_GOSSIP_STONE, CustomMessage("ZF Near Jabu Gossip Stone")},
    {RH_ZF_FAIRY_GOSSIP_STONE, CustomMessage("ZF Near Fairy Gossip Stone")},
    {RH_LH_LAB_GOSSIP_STONE, CustomMessage("LH Near Lab Gossip Stone")},
    {RH_LH_SOUTHEAST_GOSSIP_STONE, CustomMessage("LH Southeast Gossip Stone")},
    {RH_LH_SOUTHWEST_GOSSIP_STONE, CustomMessage("LH Southwest Gossip Stone")},
    {RH_GV_GOSSIP_STONE, CustomMessage("Gerudo Valley Gossip Stone")},
    {RH_COLOSSUS_GOSSIP_STONE, CustomMessage("Desert Collosus Gossip Stone")},
    {RH_DODONGOS_CAVERN_GOSSIP_STONE, CustomMessage("Dodongo's Cavern Gossip Stone")},
    {RH_GANONDORF_HINT, CustomMessage("Ganondorf Hint")},
    {RH_GANONDORF_JOKE, CustomMessage("Ganondorf Joke")},
    {RH_SHEIK_HINT, CustomMessage("Sheik in Ganons Castle Hint")},
    {RH_DAMPES_DIARY, CustomMessage("Dampe's Diary Hint")},
    {RH_GREG_RUPEE, CustomMessage("Treasure Chest Game Greg Hint")},
    {RH_ALTAR_CHILD, CustomMessage("ToT Altar as Child")},
    {RH_ALTAR_ADULT, CustomMessage("ToT Altar as Adult")},
    {RH_SARIA_HINT, CustomMessage("Saria's Magic Hint")},
    {RH_LOACH_HINT, CustomMessage("Loach Hint")},
    {RH_FISHING_POLE, CustomMessage("Fishing Pole Hint")},
    {RH_MINUET_WARP_LOC, CustomMessage("Minuet of Forest Destination")},
    {RH_BOLERO_WARP_LOC, CustomMessage("Bolero of Fire Destination")},
    {RH_SERENADE_WARP_LOC, CustomMessage("Serenade of Water Destination")},
    {RH_REQUIEM_WARP_LOC, CustomMessage("Requiem of Spirit Destination")},
    {RH_NOCTURNE_WARP_LOC, CustomMessage("Nocturne of Shadow Destination")},
    {RH_PRELUDE_WARP_LOC, CustomMessage("Prelude of Light Destination")},
    {RH_HBA_HINT, CustomMessage("Horseback Archery Hint")},
    {RH_MALON_HINT, CustomMessage("Malon Hint")},
    {RH_CHICKENS_HINT, CustomMessage("Anju's Child Chickens Hint")},
    {RH_BIG_POES_HINT, CustomMessage("Big Poe Reward Hint")},
    {RH_BIGGORON_HINT, CustomMessage("Biggoron Claim Check Hint")},
    {RH_FROGS_HINT, CustomMessage("Final Frogs in River Hint")},
    {RH_OOT_HINT, CustomMessage("Sheik in Temple of Time Hint")},
    {RH_KAK_10_SKULLS_HINT, CustomMessage("10 Skulls Hint")},
    {RH_KAK_20_SKULLS_HINT, CustomMessage("20 Skulls Hint")},
    {RH_KAK_30_SKULLS_HINT, CustomMessage("30 Skulls Hint")},
    {RH_KAK_40_SKULLS_HINT, CustomMessage("40 Skulls Hint")},
    {RH_KAK_50_SKULLS_HINT, CustomMessage("50 Skulls Hint")},
    {RH_KAK_100_SKULLS_HINT, CustomMessage("100 Skulls Hint")},
    {RH_MASK_SHOP_HINT, CustomMessage("Mask Shop Hint")},
};

std::unordered_map<RandomizerCheck, RandomizerHint> StaticData::gossipStoneCheckToHint {
  {RC_COLOSSUS_GOSSIP_STONE,                 RH_COLOSSUS_GOSSIP_STONE},
  {RC_DMC_GOSSIP_STONE,                      RH_DMC_GOSSIP_STONE},
  {RC_DMC_UPPER_GROTTO_GOSSIP_STONE,         RH_DMC_UPPER_GROTTO_GOSSIP_STONE},
  {RC_DMT_GOSSIP_STONE,                      RH_DMT_GOSSIP_STONE},
  {RC_DMT_STORMS_GROTTO_GOSSIP_STONE,        RH_DMT_STORMS_GROTTO_GOSSIP_STONE},
  {RC_DODONGOS_CAVERN_GOSSIP_STONE,          RH_DODONGOS_CAVERN_GOSSIP_STONE},
  {RC_ZF_FAIRY_GOSSIP_STONE,                 RH_ZF_FAIRY_GOSSIP_STONE},
  {RC_GC_MAZE_GOSSIP_STONE,                  RH_GC_MAZE_GOSSIP_STONE},
  {RC_GC_MEDIGORON_GOSSIP_STONE,             RH_GC_MEDIGORON_GOSSIP_STONE},
  {RC_GV_GOSSIP_STONE,                       RH_GV_GOSSIP_STONE},
  {RC_GRAVEYARD_GOSSIP_STONE,                RH_GRAVEYARD_GOSSIP_STONE},
  {RC_HC_MALON_GOSSIP_STONE,                 RH_HC_MALON_GOSSIP_STONE},
  {RC_HC_ROCK_WALL_GOSSIP_STONE,             RH_HC_ROCK_WALL_GOSSIP_STONE},
  {RC_HC_STORMS_GROTTO_GOSSIP_STONE,         RH_HC_STORMS_GROTTO_GOSSIP_STONE},
  {RC_HF_COW_GROTTO_GOSSIP_STONE,            RH_HF_COW_GROTTO_GOSSIP_STONE},
  {RC_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE,    RH_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE},
  {RC_HF_OPEN_GROTTO_GOSSIP_STONE,           RH_HF_OPEN_GROTTO_GOSSIP_STONE},
  {RC_HF_SOUTHEAST_GROTTO_GOSSIP_STONE,      RH_HF_SOUTHEAST_GROTTO_GOSSIP_STONE},
  {RC_ZF_JABU_GOSSIP_STONE,                  RH_ZF_JABU_GOSSIP_STONE},
  {RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE,        RH_KF_DEKU_TREE_LEFT_GOSSIP_STONE},
  {RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE,       RH_KF_DEKU_TREE_RIGHT_GOSSIP_STONE},
  {RC_KF_GOSSIP_STONE,                       RH_KF_GOSSIP_STONE},
  {RC_KF_STORMS_GROTTO_GOSSIP_STONE,         RH_KF_STORMS_GROTTO_GOSSIP_STONE},
  {RC_KAK_OPEN_GROTTO_GOSSIP_STONE,          RH_KAK_OPEN_GROTTO_GOSSIP_STONE},
  {RC_LH_LAB_GOSSIP_STONE,                   RH_LH_LAB_GOSSIP_STONE},
  {RC_LH_SOUTHEAST_GOSSIP_STONE,             RH_LH_SOUTHEAST_GOSSIP_STONE},
  {RC_LH_SOUTHWEST_GOSSIP_STONE,             RH_LH_SOUTHWEST_GOSSIP_STONE},
  {RC_LW_GOSSIP_STONE,                       RH_LW_GOSSIP_STONE},
  {RC_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE, RH_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE},
  {RC_SFM_MAZE_LOWER_GOSSIP_STONE,           RH_SFM_MAZE_NEAR_LW_GOSSIP_STONE},
  {RC_SFM_MAZE_UPPER_GOSSIP_STONE,           RH_SFM_MAZE_CENTER_GOSSIP_STONE},
  {RC_SFM_SARIA_GOSSIP_STONE,                RH_SFM_SARIA_GOSSIP_STONE},
  {RC_TOT_LEFT_CENTER_GOSSIP_STONE,          RH_TOT_LEFT_CENTER_GOSSIP_STONE},
  {RC_TOT_LEFTMOST_GOSSIP_STONE,             RH_TOT_LEFTMOST_GOSSIP_STONE},
  {RC_TOT_RIGHT_CENTER_GOSSIP_STONE,         RH_TOT_RIGHT_CENTER_GOSSIP_STONE},
  {RC_TOT_RIGHTMOST_GOSSIP_STONE,            RH_TOT_RIGHTMOST_GOSSIP_STONE},
  {RC_ZD_GOSSIP_STONE,                       RH_ZD_GOSSIP_STONE},
  {RC_ZR_NEAR_DOMAIN_GOSSIP_STONE,           RH_ZR_NEAR_DOMAIN_GOSSIP_STONE},
  {RC_ZR_NEAR_GROTTOS_GOSSIP_STONE,          RH_ZR_NEAR_GROTTOS_GOSSIP_STONE},
  {RC_ZR_OPEN_GROTTO_GOSSIP_STONE,           RH_ZR_OPEN_GROTTO_GOSSIP_STONE}
};

std::unordered_map<uint32_t, RandomizerHintTextKey> StaticData::areaNames = { //RANDOTODO resolve None in area
    {RA_NONE, RHT_ISOLATED_PLACE}, //explicit none in area hints means we're disconnected
    {RA_LINKS_POCKET, RHT_LINKS_POCKET},
    {RA_KOKIRI_FOREST, RHT_KOKIRI_FOREST},
    {RA_THE_LOST_WOODS, RHT_THE_LOST_WOODS},
    {RA_SACRED_FOREST_MEADOW, RHT_SACRED_FOREST_MEADOW},
    {RA_HYRULE_FIELD, RHT_HYRULE_FIELD},
    {RA_LAKE_HYLIA, RHT_LAKE_HYLIA},
    {RA_GERUDO_VALLEY, RHT_GERUDO_VALLEY},
    {RA_GERUDO_FORTRESS, RHT_GERUDO_FORTRESS},
    {RA_HAUNTED_WASTELAND, RHT_HAUNTED_WASTELAND},
    {RA_DESERT_COLOSSUS, RHT_DESERT_COLOSSUS},
    {RA_THE_MARKET, RHT_THE_MARKET},
    {RA_TEMPLE_OF_TIME, RHT_TEMPLE_OF_TIME},
    {RA_HYRULE_CASTLE, RHT_HYRULE_CASTLE},
    {RA_OUTSIDE_GANONS_CASTLE, RHT_OUTSIDE_GANONS_CASTLE},
    {RA_CASTLE_GROUNDS, RHT_CASTLE_GROUNDS},
    {RA_KAKARIKO_VILLAGE, RHT_KAKARIKO_VILLAGE},
    {RA_THE_GRAVEYARD, RHT_THE_GRAVEYARD},
    {RA_DEATH_MOUNTAIN_TRAIL, RHT_DEATH_MOUNTAIN_TRAIL},
    {RA_GORON_CITY, RHT_GORON_CITY},
    {RA_DEATH_MOUNTAIN_CRATER, RHT_DEATH_MOUNTAIN_CRATER},
    {RA_ZORAS_RIVER, RHT_ZORAS_RIVER},
    {RA_ZORAS_DOMAIN, RHT_ZORAS_DOMAIN},
    {RA_ZORAS_FOUNTAIN, RHT_ZORAS_FOUNTAIN},
    {RA_LON_LON_RANCH, RHT_LON_LON_RANCH},
    {RA_DEKU_TREE, RHT_DEKU_TREE},
    {RA_DODONGOS_CAVERN, RHT_DODONGOS_CAVERN},
    {RA_JABU_JABUS_BELLY, RHT_JABU_JABUS_BELLY},
    {RA_FOREST_TEMPLE, RHT_FOREST_TEMPLE},
    {RA_FIRE_TEMPLE, RHT_FIRE_TEMPLE},
    {RA_WATER_TEMPLE, RHT_WATER_TEMPLE},
    {RA_SPIRIT_TEMPLE, RHT_SPIRIT_TEMPLE},
    {RA_SHADOW_TEMPLE, RHT_SHADOW_TEMPLE},
    {RA_BOTTOM_OF_THE_WELL, RHT_BOTTOM_OF_THE_WELL},
    {RA_ICE_CAVERN, RHT_ICE_CAVERN},
    {RA_GERUDO_TRAINING_GROUND, RHT_GERUDO_TRAINING_GROUND},
    {RA_GANONS_CASTLE, RHT_GANONS_CASTLE}
};

std::unordered_map<uint32_t, RandomizerHintTextKey> StaticData::trialData = {
    {TK_LIGHT_TRIAL, RHT_LIGHT_TRIAL}, 
    {TK_FOREST_TRIAL, RHT_FOREST_TRIAL},
    {TK_FIRE_TRIAL, RHT_FIRE_TRIAL},
    {TK_WATER_TRIAL, RHT_WATER_TRIAL},
    {TK_SHADOW_TRIAL, RHT_SHADOW_TRIAL},
    {TK_SPIRIT_TRIAL, RHT_SPIRIT_TRIAL}
};

std::unordered_map<RandomizerHint, StaticHintInfo> StaticData::staticHintInfoMap = {
  // RH_GANONDORF_HINT is special cased due to being different based on master sword shuffle
  // Altar hints are special cased due to special hint marking rules
  // warp song hints are special cased due to entrences not being done properly yet
  // Ganondorf Joke is special cased as the text is random
  {RH_SHEIK_HINT,          StaticHintInfo(HINT_TYPE_AREA,     {RHT_SHEIK_HINT_LA_ONLY},            RSK_SHEIK_LA_HINT,       true, {}, {RG_LIGHT_ARROWS},            {RC_SHEIK_HINT_GC, RC_SHEIK_HINT_MQ_GC}, true)},
  {RH_DAMPES_DIARY,        StaticHintInfo(HINT_TYPE_AREA,     {RHT_DAMPE_DIARY},                   RSK_DAMPES_DIARY_HINT,   true, {}, {RG_PROGRESSIVE_HOOKSHOT},    {RC_DAMPE_HINT})},
  {RH_GREG_RUPEE,          StaticHintInfo(HINT_TYPE_AREA,     {RHT_GREG_HINT},                     RSK_GREG_HINT,           true, {}, {RG_GREG_RUPEE},              {RC_GREG_HINT})},
  {RH_SARIA_HINT,          StaticHintInfo(HINT_TYPE_AREA,     {RHT_SARIA_TALK_HINT, RHT_SARIA_SONG_HINT}, RSK_SARIA_HINT,   true, {}, {RG_PROGRESSIVE_MAGIC_METER}, {RC_SARIA_SONG_HINT, RC_SONG_FROM_SARIA}, true)},
  {RH_LOACH_HINT,          StaticHintInfo(HINT_TYPE_ITEM,     {RHT_LOACH_HINT},                    RSK_LOACH_HINT,          true, {RC_LH_HYRULE_LOACH})},
  {RH_FISHING_POLE,        StaticHintInfo(HINT_TYPE_AREA,     {RHT_FISHING_POLE_HINT},             RSK_FISHING_POLE_HINT,   true, {}, {RG_FISHING_POLE},            {RC_FISHING_POLE_HINT}, true)},
  {RH_HBA_HINT,            StaticHintInfo(HINT_TYPE_ITEM,     {RHT_HBA_HINT_SIGN, RHT_HBA_HINT_NOT_ON_HORSE, RHT_HBA_HINT_INITIAL, RHT_HBA_HINT_HAVE_1000}, RSK_HBA_HINT, true, {RC_GF_HBA_1000_POINTS, RC_GF_HBA_1500_POINTS})},
  {RH_MALON_HINT,          StaticHintInfo(HINT_TYPE_ITEM,     {RHT_MALON_HINT_TURNING_EVIL, RHT_MALON_HINT_HOW_IS_EPONA, RHT_MALON_HINT_OBSTICLE_COURSE, RHT_MALON_HINT_INGO_TEMPTED}, RSK_MALON_HINT, true, {RC_KF_LINKS_HOUSE_COW})},
  {RH_BIG_POES_HINT,       StaticHintInfo(HINT_TYPE_ITEM,     {RHT_BIG_POES_HINT},                 RSK_BIG_POES_HINT,       true, {RC_MARKET_10_BIG_POES})},
  {RH_CHICKENS_HINT,       StaticHintInfo(HINT_TYPE_ITEM,     {RHT_CHICKENS_HINT},                 RSK_CHICKENS_HINT,       true, {RC_KAK_ANJU_AS_CHILD})},
  {RH_BIGGORON_HINT,       StaticHintInfo(HINT_TYPE_ITEM,     {RHT_BIGGORON_HINT},                 RSK_BIGGORON_HINT,       true, {RC_DMT_TRADE_CLAIM_CHECK})},
  {RH_FROGS_HINT,          StaticHintInfo(HINT_TYPE_ITEM,     {RHT_FROGS_HINT},                    RSK_FROGS_HINT,          true, {RC_ZR_FROGS_OCARINA_GAME})},
  {RH_OOT_HINT,            StaticHintInfo(HINT_TYPE_ITEM,     {RHT_OOT_HINT},                      RSK_OOT_HINT,            true, {RC_HF_OCARINA_OF_TIME_ITEM, RC_SONG_FROM_OCARINA_OF_TIME}, {}, {RC_TOT_SHEIK_HINT})},
  {RH_KAK_10_SKULLS_HINT,  StaticHintInfo(HINT_TYPE_ITEM,     {RHT_SKULLS_HINT},                   RSK_KAK_10_SKULLS_HINT,  true, {RC_KAK_10_GOLD_SKULLTULA_REWARD},  {}, {}, false, 10)},
  {RH_KAK_20_SKULLS_HINT,  StaticHintInfo(HINT_TYPE_ITEM,     {RHT_SKULLS_HINT},                   RSK_KAK_20_SKULLS_HINT,  true, {RC_KAK_20_GOLD_SKULLTULA_REWARD},  {}, {}, false, 20)},
  {RH_KAK_30_SKULLS_HINT,  StaticHintInfo(HINT_TYPE_ITEM,     {RHT_SKULLS_HINT},                   RSK_KAK_30_SKULLS_HINT,  true, {RC_KAK_30_GOLD_SKULLTULA_REWARD},  {}, {}, false, 30)},
  {RH_KAK_40_SKULLS_HINT,  StaticHintInfo(HINT_TYPE_ITEM,     {RHT_SKULLS_HINT},                   RSK_KAK_40_SKULLS_HINT,  true, {RC_KAK_40_GOLD_SKULLTULA_REWARD},  {}, {}, false, 40)},
  {RH_KAK_50_SKULLS_HINT,  StaticHintInfo(HINT_TYPE_ITEM,     {RHT_SKULLS_HINT},                   RSK_KAK_50_SKULLS_HINT,  true, {RC_KAK_50_GOLD_SKULLTULA_REWARD},  {}, {}, false, 50)},
  {RH_KAK_100_SKULLS_HINT, StaticHintInfo(HINT_TYPE_ITEM,     {RHT_SKULLS_HINT},                   RSK_KAK_100_SKULLS_HINT, true, {RC_KAK_100_GOLD_SKULLTULA_REWARD}, {}, {}, false, 100)},
  {RH_MASK_SHOP_HINT,      StaticHintInfo(HINT_TYPE_ITEM,     {RHT_MASK_SHOP_HINT},                RSK_MASK_SHOP_HINT,      true, {RC_DEKU_THEATER_SKULL_MASK, RC_DEKU_THEATER_MASK_OF_TRUTH}, {}, {RC_MASK_SHOP_HINT})}
};

std::unordered_map<std::string, uint32_t> StaticData::PopulateTranslationMap(std::unordered_map<uint32_t, CustomMessage> input){
  std::unordered_map<std::string, uint32_t> output = {};
  for (const auto& [key, message] : input) {
    std::vector<std::string> strings = message.GetAllMessages();
    for (std::string string: strings){
      if (output.contains(string)){
        if (output[string] != key){
          SPDLOG_DEBUG("\tREPEATED STRING IN " + message.GetEnglish(MF_CLEAN) + "\n\n"); //RANDOTODO should this cause an error of some kind?
        }
      } else {
        output[string] = key;
      }
    }
  }
  return output;
}

std::unordered_map<std::string, uint32_t> StaticData::PopulateTranslationMap(std::unordered_map<uint32_t, RandomizerHintTextKey> input){
  std::unordered_map<std::string, uint32_t> output = {};
  for (const auto& [key, text] : input) {
    std::vector<std::string> strings = hintTextTable[text].GetClear().GetAllMessages();
    for (std::string string: strings){
      if (output.contains(string)){
        if (output[string] != key){
          SPDLOG_DEBUG("\tREPEATED STRING WITH " + string + "\n\n"); //RANDOTODO should this cause an error of some kind?
        }
      } else {
        output[string] = key;
      }
    }
  }
  return output;
}

std::unordered_map<std::string, uint32_t> StaticData::hintNameToEnum = {};
std::unordered_map<std::string, uint32_t> StaticData::hintTypeNameToEnum = {};
std::unordered_map<std::string, uint32_t> StaticData::areaNameToEnum = {};
std::unordered_map<std::string, uint32_t> StaticData::trialNameToEnum = {};
std::unordered_map<std::string, RandomizerCheck> StaticData::locationNameToEnum = {}; //is filled in context based on location table, not touching that because of VB 

std::unordered_map<u32, RandomizerHint> StaticData::stoneParamsToHint{
  {0x1,  RH_ZF_FAIRY_GOSSIP_STONE},
  {0x2,  RH_ZF_JABU_GOSSIP_STONE},
  {0x3,  RH_LH_LAB_GOSSIP_STONE},
  {0x4,  RH_DMT_GOSSIP_STONE},
  {0x5,  RH_DMC_GOSSIP_STONE},
  {0x6,  RH_TOT_LEFTMOST_GOSSIP_STONE},
  {0x7,  RH_TOT_LEFT_CENTER_GOSSIP_STONE},
  {0x8,  RH_LH_SOUTHWEST_GOSSIP_STONE},
  {0x9,  RH_ZD_GOSSIP_STONE}, 
  {0xA,  RH_GRAVEYARD_GOSSIP_STONE},
  {0xB,  RH_HC_ROCK_WALL_GOSSIP_STONE},
  {0xC,  RH_ZR_NEAR_DOMAIN_GOSSIP_STONE},
  {0xD,  RH_ZR_NEAR_GROTTOS_GOSSIP_STONE},
  {0xE,  RH_TOT_RIGHT_CENTER_GOSSIP_STONE },
  {0xF,  RH_LH_SOUTHEAST_GOSSIP_STONE},
  {0x10, RH_TOT_RIGHTMOST_GOSSIP_STONE },
  {0x11, RH_GV_GOSSIP_STONE},
  {0x12, RH_HC_MALON_GOSSIP_STONE},
  {0x13, RH_HC_STORMS_GROTTO_GOSSIP_STONE},
  {0x14, RH_DODONGOS_CAVERN_GOSSIP_STONE},
  {0x15, RH_GC_MAZE_GOSSIP_STONE},
  {0x16, RH_SFM_MAZE_NEAR_LW_GOSSIP_STONE},
  {0x17, RH_SFM_MAZE_CENTER_GOSSIP_STONE},
  //generic grottos all use 0x18, grottoChestParamsToHint is used for conversion
  {0x19, RH_GC_MEDIGORON_GOSSIP_STONE},
  {0x1A, RH_COLOSSUS_GOSSIP_STONE},
  {0x1B, RH_HF_COW_GROTTO_GOSSIP_STONE},
  {0x1C, RH_SFM_SARIA_GOSSIP_STONE},
  {0x1D, RH_LW_GOSSIP_STONE},
  {0x1E, RH_KF_GOSSIP_STONE},
  {0x1F, RH_KF_DEKU_TREE_LEFT_GOSSIP_STONE},
  {0x20, RH_KF_DEKU_TREE_RIGHT_GOSSIP_STONE},
};

std::unordered_map<u32, RandomizerHint> StaticData::grottoChestParamsToHint{
  {22944, RH_HF_NEAR_MARKET_GROTTO_GOSSIP_STONE},
  {22978, RH_HF_SOUTHEAST_GROTTO_GOSSIP_STONE},
  {22947, RH_HF_OPEN_GROTTO_GOSSIP_STONE},
  {22964, RH_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE},
  {23255, RH_DMT_STORMS_GROTTO_GOSSIP_STONE},
  {22984, RH_KAK_OPEN_GROTTO_GOSSIP_STONE},
  {22985, RH_ZR_OPEN_GROTTO_GOSSIP_STONE},
  {23802, RH_DMC_UPPER_GROTTO_GOSSIP_STONE},
  {22988, RH_KF_STORMS_GROTTO_GOSSIP_STONE}
};

std::array<HintText, RHT_MAX> StaticData::hintTextTable = {};
}