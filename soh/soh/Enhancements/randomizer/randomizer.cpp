#include "randomizer.h"
#include "Lib/nlohmann/json.hpp"
#include "3drando/settings.hpp"
#include <fstream>
#include <variables.h>
#include <macros.h>
#include <objects/gameplay_keep/gameplay_keep.h>
#include <functions.h>
#include <Cvar.h>
#include <textures/icon_item_static/icon_item_static.h>
#include <textures/icon_item_24_static/icon_item_24_static.h>
#include "../libultraship/ImGuiImpl.h"
#include <thread>
#include "3drando/rando_main.hpp"
#include <soh/Enhancements/debugger/ImGuiHelpers.h>
#include "Lib/ImGui/imgui_internal.h"
#include <soh/Enhancements/custom-message/CustomMessageManager.h>
#include <soh/Enhancements/custom-message/CustomMessageTypes.h>
#include <soh/Enhancements/item-tables/ItemTableManager.h>
#include <stdexcept>

using json = nlohmann::json;
using namespace std::literals::string_literals;

std::unordered_map<uint8_t, Sprite> gSeedTextures;

u8 generated;

const std::string Randomizer::getItemMessageTableID = "Randomizer";
const std::string Randomizer::hintMessageTableID = "RandomizerHints";
const std::string Randomizer::scrubMessageTableID = "RandomizerScrubs";

Randomizer::Randomizer() {
    Sprite bowSprite = { dgFairyBowIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 0 };
    gSeedTextures[0] = bowSprite;

    Sprite bombchuSprite = { dgBombchuIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 1 };
    gSeedTextures[1] = bombchuSprite;

    Sprite beansSprite = { dgMagicBeansIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 2 };
    gSeedTextures[2] = beansSprite;

    Sprite milkSprite = { dgMilkFullIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 3 };
    gSeedTextures[3] = milkSprite;

    Sprite frogSprite = { dgEyeBallFrogIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 4 };
    gSeedTextures[4] = frogSprite;

    Sprite mirrorShieldSprite = { dgMirrorShieldIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 5 };
    gSeedTextures[5] = mirrorShieldSprite;

    Sprite hoverBootsSprite = { dgHoverBootsIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 6 };
    gSeedTextures[6] = hoverBootsSprite;

    Sprite megatonHammerSprite = { dgMegatonHammerIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 7 };
    gSeedTextures[7] = megatonHammerSprite;

    Sprite silverGauntletsSprite = { dgSilverGauntletsIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 8 };
    gSeedTextures[8] = silverGauntletsSprite;

    Sprite ootOcarinaSprite = { dgOcarinaofTimeIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, 9 };
    gSeedTextures[9] = ootOcarinaSprite;
}

Sprite* Randomizer::GetSeedTexture(uint8_t index) {
    return &gSeedTextures[index];
}

Randomizer::~Randomizer() { 
    this->randoSettings.clear();
    this->itemLocations.clear();
}

std::unordered_map<std::string, RandomizerCheck> SpoilerfileCheckNameToEnum = {
    { "Invalid Location", RC_UNKNOWN_CHECK },
    { "KF Kokiri Sword Chest", RC_KF_KOKIRI_SWORD_CHEST },
    { "KF Mido Top Left Chest", RC_KF_MIDOS_TOP_LEFT_CHEST },
    { "KF Mido Top Right Chest", RC_KF_MIDOS_TOP_RIGHT_CHEST },
    { "KF Mido Bottom Left Chest", RC_KF_MIDOS_BOTTOM_LEFT_CHEST },
    { "KF Mido Bottom Right Chest", RC_KF_MIDOS_BOTTOM_RIGHT_CHEST },
    { "KF Storms Grotto Chest", RC_KF_STORMS_GROTTO_CHEST },
    { "LW Near Shortcuts Grotto Chest", RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST },
    { "LW Skull Kid", RC_LW_SKULL_KID },
    { "LW Trade Cojiro", RC_LW_TRADE_COJIRO },
    { "LW Trade Odd Potion", RC_LW_TRADE_ODD_POTION },
    { "LW Ocarina Memory Game", RC_LW_OCARINA_MEMORY_GAME },
    { "LW Target in Woods", RC_LW_TARGET_IN_WOODS },
    { "LW Deku Scrub Near Deku Theater Right", RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT },
    { "LW Deku Scrub Near Deku Theater Left", RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT },
    { "LW Deku Scrub Near Bridge", RC_LW_DEKU_SCRUB_NEAR_BRIDGE },
    { "LW Deku Scrub Grotto Rear", RC_LW_DEKU_SCRUB_GROTTO_REAR },
    { "LW Deku Scrub Grotto Front", RC_LW_DEKU_SCRUB_GROTTO_FRONT },
    { "Deku Theater Skull Mask", RC_DEKU_THEATER_SKULL_MASK },
    { "Deku Theater Mask of Truth", RC_DEKU_THEATER_MASK_OF_TRUTH },
    { "SFM Wolfos Grotto Chest", RC_SFM_WOLFOS_GROTTO_CHEST },
    { "SFM Deku Scrub Grotto Rear", RC_SFM_DEKU_SCRUB_GROTTO_REAR },
    { "SFM Deku Scrub Grotto Front", RC_SFM_DEKU_SCRUB_GROTTO_FRONT },
    { "HF Southeast Grotto Chest", RC_HF_SOUTHEAST_GROTTO_CHEST },
    { "HF Open Grotto Chest", RC_HF_OPEN_GROTTO_CHEST },
    { "HF Near Market Grotto Chest", RC_HF_NEAR_MARKET_GROTTO_CHEST },
    { "HF Ocarina of Time Item", RC_HF_OCARINA_OF_TIME_ITEM },
    { "HF Tektite Grotto Freestanding PoH", RC_HF_TEKTITE_GROTTO_FREESTANDING_POH },
    { "HF Deku Scrub Grotto", RC_HF_DEKU_SCRUB_GROTTO },
    { "LH Child Fishing", RC_LH_CHILD_FISHING },
    { "LH Adult Fishing", RC_LH_ADULT_FISHING },
    { "LH Lab Dive", RC_LH_LAB_DIVE },
    { "LH Lab Trade Eyeball Frog", RC_LH_TRADE_FROG },
    { "LH Underwater Item", RC_LH_UNDERWATER_ITEM },
    { "LH Sun", RC_LH_SUN },
    { "LH Freestanding PoH", RC_LH_FREESTANDING_POH },
    { "LH Deku Scrub Grotto Left", RC_LH_DEKU_SCRUB_GROTTO_LEFT },
    { "LH Deku Scrub Grotto Right", RC_LH_DEKU_SCRUB_GROTTO_RIGHT },
    { "LH Deku Scrub Grotto Center", RC_LH_DEKU_SCRUB_GROTTO_CENTER },
    { "GV Chest", RC_GV_CHEST },
    { "GV Trade Saw", RC_GV_TRADE_SAW },
    { "GV Waterfall Freestanding PoH", RC_GV_WATERFALL_FREESTANDING_POH },
    { "GV Crate Freestanding PoH", RC_GV_CRATE_FREESTANDING_POH },
    { "GV Deku Scrub Grotto Rear", RC_GV_DEKU_SCRUB_GROTTO_REAR },
    { "GV Deku Scrub Grotto Front", RC_GV_DEKU_SCRUB_GROTTO_FRONT },
    { "GF Chest", RC_GF_CHEST },
    { "GF HBA 1000 Points", RC_GF_HBA_1000_POINTS },
    { "GF HBA 1500 Points", RC_GF_HBA_1500_POINTS },
    { "GF Gerudo Membership Card", RC_GF_GERUDO_MEMBERSHIP_CARD },
    { "GF North F1 Carpenter", RC_GF_NORTH_F1_CARPENTER },
    { "GF North F2 Carpenter", RC_GF_NORTH_F2_CARPENTER },
    { "GF South F1 Carpenter", RC_GF_SOUTH_F1_CARPENTER },
    { "GF South F2 Carpenter", RC_GF_SOUTH_F2_CARPENTER },
    { "Wasteland Chest", RC_WASTELAND_CHEST },
    { "Wasteland Carpet Salesman", RC_WASTELAND_BOMBCHU_SALESMAN },
    { "Colossus Freestanding PoH", RC_COLOSSUS_FREESTANDING_POH },
    { "Colossus Deku Scrub Grotto Rear", RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR },
    { "Colossus Deku Scrub Grotto Front", RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT },
    { "MK Treasure Chest Game Reward", RC_MARKET_TREASURE_CHEST_GAME_REWARD },
    { "MK Bombchu Bowling First Prize", RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE },
    { "MK Bombchu Bowling Second Prize", RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE },
    { "MK Bombchu Bowling Bombchus", RC_UNKNOWN_CHECK },
    { "MK Lost Dog", RC_MARKET_LOST_DOG },
    { "MK Shooting Gallery", RC_MARKET_SHOOTING_GALLERY_REWARD },
    { "MK 10 Big Poes", RC_MARKET_10_BIG_POES },
    { "MK Chest Game First Room Chest", RC_MARKET_TREASURE_CHEST_GAME_ITEM_1 },
    { "MK Chest Game Second Room Chest", RC_MARKET_TREASURE_CHEST_GAME_ITEM_2 },
    { "MK Chest Game Third Room Chest", RC_MARKET_TREASURE_CHEST_GAME_ITEM_3 },
    { "MK Chest Game Fourth Room Chest", RC_MARKET_TREASURE_CHEST_GAME_ITEM_4 },
    { "MK Chest Game Fifth Room Chest", RC_MARKET_TREASURE_CHEST_GAME_ITEM_5 },
    { "HC Malon Egg", RC_HC_MALON_EGG },
    { "HC Zeldas Letter", RC_HC_ZELDAS_LETTER },
    { "Kak Redead Grotto Chest", RC_KAK_REDEAD_GROTTO_CHEST },
    { "Kak Open Grotto Chest", RC_KAK_OPEN_GROTTO_CHEST },
    { "Kak 10 Gold Skulltula Reward", RC_KAK_10_GOLD_SKULLTULA_REWARD },
    { "Kak 20 Gold Skulltula Reward", RC_KAK_20_GOLD_SKULLTULA_REWARD },
    { "Kak 30 Gold Skulltula Reward", RC_KAK_30_GOLD_SKULLTULA_REWARD },
    { "Kak 40 Gold Skulltula Reward", RC_KAK_40_GOLD_SKULLTULA_REWARD },
    { "Kak 50 Gold Skulltula Reward", RC_KAK_50_GOLD_SKULLTULA_REWARD },
    { "Kak Man on Roof", RC_KAK_MAN_ON_ROOF },
    { "Kak Shooting Gallery Reward", RC_KAK_SHOOTING_GALLERY_REWARD },
    { "Kak Trade Odd Mushroom", RC_KAK_TRADE_ODD_MUSHROOM },
    { "Kak Anju as Adult", RC_KAK_ANJU_AS_ADULT },
    { "Kak Anju as Child", RC_KAK_ANJU_AS_CHILD },
    { "Kak Trade Pocket Cucco", RC_KAK_TRADE_POCKET_CUCCO },
    { "Kak Impas House Freestanding PoH", RC_KAK_IMPAS_HOUSE_FREESTANDING_POH },
    { "Kak Windmill Freestanding PoH", RC_KAK_WINDMILL_FREESTANDING_POH },
    { "GY Shield Grave Chest", RC_GRAVEYARD_SHIELD_GRAVE_CHEST },
    { "GY Heart Piece Grave Chest", RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST },
    { "GY Composers Grave Chest", RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST },
    { "GY Hookshot Chest", RC_GRAVEYARD_HOOKSHOT_CHEST },
    { "GY Dampe Race Freestanding PoH", RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH },
    { "GY Freestanding PoH", RC_GRAVEYARD_FREESTANDING_POH },
    { "GY Dampe Gravedigging Tour", RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR },
    { "DMT Chest", RC_DMT_CHEST },
    { "DMT Storms Grotto Chest", RC_DMT_STORMS_GROTTO_CHEST },
    { "DMT Trade Broken Sword", RC_DMT_TRADE_BROKEN_SWORD },
    { "DMT Trade Eyedrops", RC_DMT_TRADE_EYEDROPS },
    { "DMT Trade Claim Check", RC_DMT_TRADE_CLAIM_CHECK },
    { "DMT Freestanding PoH", RC_DMT_FREESTANDING_POH },
    { "GC Maze Left Chest", RC_GC_MAZE_LEFT_CHEST },
    { "GC Maze Right Chest", RC_GC_MAZE_RIGHT_CHEST },
    { "GC Maze Center Chest", RC_GC_MAZE_CENTER_CHEST },
    { "GC Rolling Goron as Child", RC_GC_ROLLING_GORON_AS_CHILD },
    { "GC Rolling Goron as Adult", RC_GC_ROLLING_GORON_AS_ADULT },
    { "GC Darunias Joy", RC_GC_DARUNIAS_JOY },
    { "GC Pot Freestanding PoH", RC_GC_POT_FREESTANDING_POH },
    { "GC Deku Scrub Grotto Left", RC_GC_DEKU_SCRUB_GROTTO_LEFT },
    { "GC Deku Scrub Grotto Right", RC_GC_DEKU_SCRUB_GROTTO_RIGHT },
    { "GC Deku Scrub Grotto Center", RC_GC_DEKU_SCRUB_GROTTO_CENTER },
    { "GC Medigoron", RC_GC_MEDIGORON },
    { "DMC Upper Grotto Chest", RC_DMC_UPPER_GROTTO_CHEST },
    { "DMC Wall Freestanding PoH", RC_DMC_WALL_FREESTANDING_POH },
    { "DMC Volcano Freestanding PoH", RC_DMC_VOLCANO_FREESTANDING_POH },
    { "DMC Deku Scrub", RC_DMC_DEKU_SCRUB },
    { "DMC Deku Scrub Grotto Left", RC_DMC_DEKU_SCRUB_GROTTO_LEFT },
    { "DMC Deku Scrub Grotto Right", RC_DMC_DEKU_SCRUB_GROTTO_RIGHT },
    { "DMC Deku Scrub Grotto Center", RC_DMC_DEKU_SCRUB_GROTTO_CENTER },
    { "ZR Open Grotto Chest", RC_ZR_OPEN_GROTTO_CHEST },
    { "ZR Magic Bean Salesman", RC_ZR_MAGIC_BEAN_SALESMAN },
    { "ZR Frogs Zelda's Lullaby", RC_ZR_FROGS_ZELDAS_LULLABY },
    { "ZR Frogs Epona's Song", RC_ZR_FROGS_EPONAS_SONG },
    { "ZR Frogs Saria's Song", RC_ZR_FROGS_SARIAS_SONG },
    { "ZR Frogs Sun's Song", RC_ZR_FROGS_SUNS_SONG },
    { "ZR Frogs Song of Time", RC_ZR_FROGS_SONG_OF_TIME },
    { "ZR Frogs in the Rain", RC_ZR_FROGS_IN_THE_RAIN },
    { "ZR Frogs Ocarina Game", RC_ZR_FROGS_OCARINA_GAME },
    { "ZR Near Open Grotto Freestanding PoH", RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH },
    { "ZR Near Domain Freestanding PoH", RC_ZR_NEAR_DOMAIN_FREESTANDING_POH },
    { "ZR Deku Scrub Grotto Rear", RC_ZR_DEKU_SCRUB_GROTTO_REAR },
    { "ZR Deku Scrub Grotto Front", RC_ZR_DEKU_SCRUB_GROTTO_FRONT },
    { "ZD Chest", RC_ZD_CHEST },
    { "ZD Diving Minigame", RC_ZD_DIVING_MINIGAME },
    { "ZD King Zora Thawed", RC_ZD_KING_ZORA_THAWED },
    { "ZD Trade Prescription", RC_ZD_TRADE_PRESCRIPTION },
    { "ZF Iceberg Freestanding PoH", RC_ZF_ICEBERC_FREESTANDING_POH },
    { "ZF Bottom Freestanding PoH", RC_ZF_BOTTOM_FREESTANDING_POH },
    { "LLR Talons Chickens", RC_LLR_TALONS_CHICKENS },
    { "LLR Freestanding PoH", RC_LLR_FREESTANDING_POH },
    { "LLR Deku Scrub Grotto Left", RC_LLR_DEKU_SCRUB_GROTTO_LEFT },
    { "LLR Deku Scrub Grotto Right", RC_LLR_DEKU_SCRUB_GROTTO_RIGHT },
    { "LLR Deku Scrub Grotto Center", RC_LLR_DEKU_SCRUB_GROTTO_CENTER },
    { "Deku Tree Map Chest", RC_DEKU_TREE_MAP_CHEST },
    { "Deku Tree Compass Chest", RC_DEKU_TREE_COMPASS_CHEST },
    { "Deku Tree Compass Room Side Chest", RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST },
    { "Deku Tree Basement Chest", RC_DEKU_TREE_BASEMENT_CHEST },
    { "Deku Tree Slingshot Chest", RC_DEKU_TREE_SLINGSHOT_CHEST },
    { "Deku Tree Slingshot Room Side Chest", RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST },
    { "Deku Tree MQ Map Chest", RC_DEKU_TREE_MQ_MAP_CHEST },
    { "Deku Tree MQ Compass Chest", RC_DEKU_TREE_MQ_COMPASS_CHEST },
    { "Deku Tree MQ Slingshot Chest", RC_DEKU_TREE_MQ_SLINGSHOT_CHEST },
    { "Deku Tree MQ Slingshot Room Back Chest", RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST },
    { "Deku Tree MQ Basement Chest", RC_DEKU_TREE_MQ_BASEMENT_CHEST },
    { "Deku Tree MQ Before Spinning Log Chest", RC_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST },
    { "Deku Tree MQ After Spinning Log Chest", RC_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST },
    { "Deku Tree MQ Deku Scrub", RC_DEKU_TREE_MQ_DEKU_SCRUB },
    { "Dodongos Cavern Boss Room Chest", RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST },
    { "Dodongos Cavern Map Chest", RC_DODONGOS_CAVERN_MAP_CHEST },
    { "Dodongos Cavern Compass Chest", RC_DODONGOS_CAVERN_COMPASS_CHEST },
    { "Dodongos Cavern Bomb Flower Platform Chest", RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST },
    { "Dodongos Cavern Bomb Bag Chest", RC_DODONGOS_CAVERN_BOMB_BAG_CHEST },
    { "Dodongos Cavern End Of Bridge Chest", RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST },
    { "Dodongos Cavern Deku Scrub Near Bomb Bag Left", RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT },
    { "Dodongos Cavern Deku Scrub Side Room Near Dodongos", RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS },
    { "Dodongos Cavern Deku Scrub Near Bomb Bag Right", RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT },
    { "Dodongos Cavern Deku Scrub Lobby", RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY },
    { "Dodongos Cavern MQ Map Chest", RC_DODONGOS_CAVERN_MQ_MAP_CHEST },
    { "Dodongos Cavern MQ Bomb Bag Chest", RC_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST },
    { "Dodongos Cavern MQ Compass Chest", RC_DODONGOS_CAVERN_MQ_COMPASS_CHEST },
    { "Dodongos Cavern MQ Larvae Room Chest", RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST },
    { "Dodongos Cavern MQ Torch Puzzle Room Chest", RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST },
    { "Dodongos Cavern MQ Under Grave Chest", RC_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST },
    { "Dodongos Cavern Deku Scrub Lobby Rear", RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR },
    { "Dodongos Cavern Deku Scrub Lobby Front", RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT },
    { "Dodongos Cavern Deku Scrub Staircase", RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE },
    { "Dodongos Cavern Deku Scrub Side Room Near Lower Lizalfos",
      RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS },
    { "Jabu Jabus Belly Map Chest", RC_JABU_JABUS_BELLY_MAP_CHEST },
    { "Jabu Jabus Belly Compass Chest", RC_JABU_JABUS_BELLY_COMPASS_CHEST },
    { "Jabu Jabus Belly Boomerang Chest", RC_JABU_JABUS_BELLY_BOOMERANG_CHEST },
    { "Jabu Jabus Belly Deku Scrub", RC_JABU_JABUS_BELLY_DEKU_SCRUB },
    { "Jabu Jabus Belly MQ First Room Side Chest", RC_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST },
    { "Jabu Jabus Belly MQ Map Chest", RC_JABU_JABUS_BELLY_MQ_MAP_CHEST },
    { "Jabu Jabus Belly MQ Second Room Lower Chest", RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST },
    { "Jabu Jabus Belly MQ Compass Chest", RC_JABU_JABUS_BELLY_MQ_COMPASS_CHEST },
    { "Jabu Jabus Belly MQ Second Room Upper Chest", RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST },
    { "Jabu Jabus Belly MQ Basement Near Switches Chest", RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST },
    { "Jabu Jabus Belly MQ Basement Near Vines Chest", RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST },
    { "Jabu Jabus Belly MQ Near Boss Chest", RC_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST },
    { "Jabu Jabus Belly MQ Falling Like Like Room Chest", RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST },
    { "Jabu Jabus Belly MQ Boomerang Room Small Chest", RC_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST },
    { "Jabu Jabus Belly MQ Boomerang Chest", RC_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST },
    { "Forest Temple First Room Chest", RC_FOREST_TEMPLE_FIRST_ROOM_CHEST },
    { "Forest Temple First Stalfos Chest", RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST },
    { "Forest Temple Raised Island Courtyard Chest", RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST },
    { "Forest Temple Map Chest", RC_FOREST_TEMPLE_MAP_CHEST },
    { "Forest Temple Well Chest", RC_FOREST_TEMPLE_WELL_CHEST },
    { "Forest Temple Falling Ceiling Room Chest", RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST },
    { "Forest Temple Eye Switch Chest", RC_FOREST_TEMPLE_EYE_SWITCH_CHEST },
    { "Forest Temple Boss Key Chest", RC_FOREST_TEMPLE_BOSS_KEY_CHEST },
    { "Forest Temple Floormaster Chest", RC_FOREST_TEMPLE_FLOORMASTER_CHEST },
    { "Forest Temple Bow Chest", RC_FOREST_TEMPLE_BOW_CHEST },
    { "Forest Temple Red Poe Chest", RC_FOREST_TEMPLE_RED_POE_CHEST },
    { "Forest Temple Blue Poe Chest", RC_FOREST_TEMPLE_BLUE_POE_CHEST },
    { "Forest Temple Basement Chest", RC_FOREST_TEMPLE_BASEMENT_CHEST },
    { "Forest Temple MQ First Room Chest", RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST },
    { "Forest Temple MQ Wolfos Chest", RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST },
    { "Forest Temple MQ Bow Chest", RC_FOREST_TEMPLE_MQ_BOW_CHEST },
    { "Forest Temple MQ Raised Island Courtyard Lower Chest", RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST },
    { "Forest Temple MQ Raised Island Courtyard Upper Chest", RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST },
    { "Forest Temple MQ Well Chest", RC_FOREST_TEMPLE_MQ_WELL_CHEST },
    { "Forest Temple MQ Map Chest", RC_FOREST_TEMPLE_MQ_MAP_CHEST },
    { "Forest Temple MQ Compass Chest", RC_FOREST_TEMPLE_MQ_COMPASS_CHEST },
    { "Forest Temple MQ Falling Ceiling Room Chest", RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST },
    { "Forest Temple MQ Basement Chest", RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST },
    { "Forest Temple MQ Redead Chest", RC_FOREST_TEMPLE_MQ_REDEAD_CHEST },
    { "Forest Temple MQ Boss Key Chest", RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST },
    { "Fire Temple Near Boss Chest", RC_FIRE_TEMPLE_NEAR_BOSS_CHEST },
    { "Fire Temple Flare Dancer Chest", RC_FIRE_TEMPLE_FLARE_DANCER_CHEST },
    { "Fire Temple Boss Key Chest", RC_FIRE_TEMPLE_BOSS_KEY_CHEST },
    { "Fire Temple Big Lava Room Blocked Door Chest", RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST },
    { "Fire Temple Big Lava Room Lower Open Door Chest", RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST },
    { "Fire Temple Boulder Maze Lower Chest", RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST },
    { "Fire Temple Boulder Maze Upper Chest", RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST },
    { "Fire Temple Boulder Maze Side Room Chest", RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST },
    { "Fire Temple Boulder Maze Shortcut Chest", RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST },
    { "Fire Temple Scarecrow Chest", RC_FIRE_TEMPLE_SCARECROW_CHEST },
    { "Fire Temple Map Chest", RC_FIRE_TEMPLE_MAP_CHEST },
    { "Fire Temple Compass Chest", RC_FIRE_TEMPLE_COMPASS_CHEST },
    { "Fire Temple Highest Goron Chest", RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST },
    { "Fire Temple Megaton Hammer Chest", RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST },
    { "Fire Temple MQ Near Boss Chest", RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST },
    { "Fire Temple MQ Megaton Hammer Chest", RC_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST },
    { "Fire Temple MQ Compass Chest", RC_FIRE_TEMPLE_MQ_COMPASS_CHEST },
    { "Fire Temple MQ Lizalfos Maze Lower Chest", RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST },
    { "Fire Temple MQ Lizalfos Maze Upper Chest", RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST },
    { "Fire Temple MQ Chest on Fire", RC_FIRE_TEMPLE_MQ_CHEST_ON_FIRE },
    { "Fire Temple MQ Map Room Side Chest", RC_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST },
    { "Fire Temple MQ Map Chest", RC_FIRE_TEMPLE_MQ_MAP_CHEST },
    { "Fire Temple MQ Boss Key Chest", RC_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST },
    { "Fire Temple MQ Big Lava Room Blocked Door Chest", RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST },
    { "Fire Temple MQ Lizalfos Maze Side Room Chest", RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST },
    { "Fire Temple MQ Freestanding Key", RC_FIRE_TEMPLE_MQ_FREESTANDING_KEY },
    { "Water Temple Map Chest", RC_WATER_TEMPLE_MAP_CHEST },
    { "Water Temple Compass Chest", RC_WATER_TEMPLE_COMPASS_CHEST },
    { "Water Temple Torches Chest", RC_WATER_TEMPLE_TORCHES_CHEST },
    { "Water Temple Dragon Chest", RC_WATER_TEMPLE_DRAGON_CHEST },
    { "Water Temple Central Bow Target Chest", RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST },
    { "Water Temple Central Pillar Chest", RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST },
    { "Water Temple Cracked Wall Chest", RC_WATER_TEMPLE_CRACKED_WALL_CHEST },
    { "Water Temple Boss Key Chest", RC_WATER_TEMPLE_BOSS_KEY_CHEST },
    { "Water Temple Longshot Chest", RC_WATER_TEMPLE_LONGSHOT_CHEST },
    { "Water Temple River Chest", RC_WATER_TEMPLE_RIVER_CHEST },
    { "Water Temple MQ Central Pillar Chest", RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST },
    { "Water Temple MQ Boss Key Chest", RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST },
    { "Water Temple MQ Longshot Chest", RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST },
    { "Water Temple MQ Compass Chest", RC_WATER_TEMPLE_MQ_COMPASS_CHEST },
    { "Water Temple MQ Map Chest", RC_WATER_TEMPLE_MQ_MAP_CHEST },
    { "Water Temple MQ Freestanding Key", RC_WATER_TEMPLE_MQ_FREESTANDING_KEY },
    { "Spirit Temple Silver Gauntlets Chest", RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST },
    { "Spirit Temple Mirror Shield Chest", RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST },
    { "Spirit Temple Child Bridge Chest", RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST },
    { "Spirit Temple Child Early Torches Chest", RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST },
    { "Spirit Temple Compass Chest", RC_SPIRIT_TEMPLE_COMPASS_CHEST },
    { "Spirit Temple Early Adult Right Chest", RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST },
    { "Spirit Temple First Mirror Left Chest", RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST },
    { "Spirit Temple First Mirror Right Chest", RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST },
    { "Spirit Temple Map Chest", RC_SPIRIT_TEMPLE_MAP_CHEST },
    { "Spirit Temple Child Climb North Chest", RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST },
    { "Spirit Temple Child Climb East Chest", RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST },
    { "Spirit Temple Sun Block Room Chest", RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST },
    { "Spirit Temple Statue Room Hand Chest", RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST },
    { "Spirit Temple Statue Room Northeast Chest", RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST },
    { "Spirit Temple Near Four Armos Chest", RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST },
    { "Spirit Temple Hallway Right Invisible Chest", RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST },
    { "Spirit Temple Hallway Left Invisible Chest", RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST },
    { "Spirit Temple Boss Key Chest", RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST },
    { "Spirit Temple Topmost Chest", RC_SPIRIT_TEMPLE_TOPMOST_CHEST },
    { "Spirit Temple MQ Entrance Front Left Chest", RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST },
    { "Spirit Temple MQ Entrance Back Right Chest", RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST },
    { "Spirit Temple MQ Entrance Front Right Chest", RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST },
    { "Spirit Temple MQ Entrance Back Left Chest", RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST },
    { "Spirit Temple MQ Child Hammer Switch Chest", RC_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST },
    { "Spirit Temple MQ Map Chest", RC_SPIRIT_TEMPLE_MQ_MAP_CHEST },
    { "Spirit Temple MQ Map Room Enemy Chest", RC_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST },
    { "Spirit Temple MQ Child Climb North Chest", RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST },
    { "Spirit Temple MQ Child Climb South Chest", RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST },
    { "Spirit Temple MQ Compass Chest", RC_SPIRIT_TEMPLE_MQ_COMPASS_CHEST },
    { "Spirit Temple MQ Statue Room Lullaby Chest", RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST },
    { "Spirit Temple MQ Statue Room Invisible Chest", RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST },
    { "Spirit Temple MQ Silver Block Hallway Chest", RC_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST },
    { "Spirit Temple MQ Sun Block Room Chest", RC_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST },
    { "Spirit Temple MQ Symphony Room Chest", RC_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST },
    { "Spirit Temple MQ Leever Room Chest", RC_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST },
    { "Spirit Temple MQ Beamos Room Chest", RC_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST },
    { "Spirit Temple MQ Chest Switch Chest", RC_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST },
    { "Spirit Temple MQ Boss Key Chest", RC_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST },
    { "Spirit Temple MQ Mirror Puzzle Invisible Chest", RC_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST },
    { "Shadow Temple Map Chest", RC_SHADOW_TEMPLE_MAP_CHEST },
    { "Shadow Temple Hover Boots Chest", RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST },
    { "Shadow Temple Compass Chest", RC_SHADOW_TEMPLE_COMPASS_CHEST },
    { "Shadow Temple Early Silver Rupee Chest", RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST },
    { "Shadow Temple Invisible Blades Visible Chest", RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST },
    { "Shadow Temple Invisible Blades Invisible Chest", RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST },
    { "Shadow Temple Falling Spikes Lower Chest", RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST },
    { "Shadow Temple Falling Spikes Upper Chest", RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST },
    { "Shadow Temple Falling Spikes Switch Chest", RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST },
    { "Shadow Temple Invisible Spikes Chest", RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST },
    { "Shadow Temple Wind Hint Chest", RC_SHADOW_TEMPLE_WIND_HINT_CHEST },
    { "Shadow Temple After Wind Enemy Chest", RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST },
    { "Shadow Temple After Wind Hidden Chest", RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST },
    { "Shadow Temple Spike Walls Left Chest", RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST },
    { "Shadow Temple Boss Key Chest", RC_SHADOW_TEMPLE_BOSS_KEY_CHEST },
    { "Shadow Temple Invisible Floormaster Chest", RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST },
    { "Shadow Temple Freestanding Key", RC_SHADOW_TEMPLE_FREESTANDING_KEY },
    { "Shadow Temple MQ Compass Chest", RC_SHADOW_TEMPLE_MQ_COMPASS_CHEST },
    { "Shadow Temple MQ Hover Boots Chest", RC_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST },
    { "Shadow Temple MQ Early Gibdos Chest", RC_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST },
    { "Shadow Temple MQ Map Chest", RC_SHADOW_TEMPLE_MQ_MAP_CHEST },
    { "Shadow Temple MQ Beamos Silver Rupees Chest", RC_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST },
    { "Shadow Temple MQ Falling Spikes Switch Chest", RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST },
    { "Shadow Temple MQ Falling Spikes Lower Chest", RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST },
    { "Shadow Temple MQ Falling Spikes Upper Chest", RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST },
    { "Shadow Temple MQ Invisible Spikes Chest", RC_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST },
    { "Shadow Temple MQ Boss Key Chest", RC_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST },
    { "Shadow Temple MQ Spike Walls Left Chest", RC_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST },
    { "Shadow Temple MQ Stalfos Room Chest", RC_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST },
    { "Shadow Temple MQ Invisible Blades Invisible Chest", RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST },
    { "Shadow Temple MQ Invisible Blades Visible Chest", RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST },
    { "Shadow Temple MQ Bomb Flower Chest", RC_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST },
    { "Shadow Temple MQ Wind Hint Chest", RC_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST },
    { "Shadow Temple MQ After Wind Hidden Chest", RC_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST },
    { "Shadow Temple MQ After Wind Enemy Chest", RC_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST },
    { "Shadow Temple MQ Near Ship Invisible Chest", RC_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST },
    { "Shadow Temple MQ Freestanding Key", RC_SHADOW_TEMPLE_MQ_FREESTANDING_KEY },
    { "Bottom of the Well Front Left Fake Wall Chest", RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST },
    { "Bottom of the Well Front Center Bombable Chest", RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST },
    { "Bottom of the Well Right Bottom Fake Wall Chest", RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST },
    { "Bottom of the Well Compass Chest", RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST },
    { "Bottom of the Well Center Skulltula Chest", RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST },
    { "Bottom of the Well Back Left Bombable Chest", RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST },
    { "Bottom of the Well Lens of Truth Chest", RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST },
    { "Bottom of the Well Invisible Chest", RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST },
    { "Bottom of the Well Underwater Front Chest", RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST },
    { "Bottom of the Well Underwater Left Chest", RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST },
    { "Bottom of the Well Map Chest", RC_BOTTOM_OF_THE_WELL_MAP_CHEST },
    { "Bottom of the Well Fire Keese Chest", RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST },
    { "Bottom of the Well Like Like Chest", RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST },
    { "Bottom of the Well Freestanding Key", RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY },
    { "Bottom of the Well MQ Map Chest", RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST },
    { "Bottom of the Well MQ Lens of Truth Chest", RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST },
    { "Bottom of the Well MQ Compass Chest", RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST },
    { "Bottom of the Well MQ Dead Hand Freestanding Key", RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY },
    { "Bottom of the Well MQ East Inner Room Freestanding Key",
      RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY },
    { "Ice Cavern Map Chest", RC_ICE_CAVERN_MAP_CHEST },
    { "Ice Cavern Compass Chest", RC_ICE_CAVERN_COMPASS_CHEST },
    { "Ice Cavern Iron Boots Chest", RC_ICE_CAVERN_IRON_BOOTS_CHEST },
    { "Ice Cavern Freestanding PoH", RC_ICE_CAVERN_FREESTANDING_POH },
    { "Ice Cavern MQ Iron Boots Chest", RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST },
    { "Ice Cavern MQ Compass Chest", RC_ICE_CAVERN_MQ_COMPASS_CHEST },
    { "Ice Cavern MQ Map Chest", RC_ICE_CAVERN_MQ_MAP_CHEST },
    { "Ice Cavern MQ Freestanding PoH", RC_ICE_CAVERN_MQ_FREESTANDING_POH },
    { "Gerudo Training Grounds Lobby Left Chest", RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST },
    { "Gerudo Training Grounds Lobby Right Chest", RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST },
    { "Gerudo Training Grounds Stalfos Chest", RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST },
    { "Gerudo Training Grounds Beamos Chest", RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST },
    { "Gerudo Training Grounds Hidden Ceiling Chest", RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST },
    { "Gerudo Training Grounds Maze Path First Chest", RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST },
    { "Gerudo Training Grounds Maze Path Second Chest", RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST },
    { "Gerudo Training Grounds Maze Path Third Chest", RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST },
    { "Gerudo Training Grounds Maze Path Final Chest", RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST },
    { "Gerudo Training Grounds Maze Right Central Chest", RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST },
    { "Gerudo Training Grounds Maze Right Side Chest", RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST },
    { "Gerudo Training Grounds Underwater Silver Rupee Chest",
      RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST },
    { "Gerudo Training Grounds Hammer Room Clear Chest", RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST },
    { "Gerudo Training Grounds Hammer Room Switch Chest", RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST },
    { "Gerudo Training Grounds Eye Statue Chest", RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST },
    { "Gerudo Training Grounds Near Scarecrow Chest", RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST },
    { "Gerudo Training Grounds Before Heavy Block Chest", RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST },
    { "Gerudo Training Grounds Heavy Block First Chest", RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST },
    { "Gerudo Training Grounds Heavy Block Second Chest", RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST },
    { "Gerudo Training Grounds Heavy Block Third Chest", RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST },
    { "Gerudo Training Grounds Heavy Block Fourth Chest", RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST },
    { "Gerudo Training Grounds Freestanding Key", RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY },
    { "Gerudo Training Grounds MQ Lobby Right Chest", RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST },
    { "Gerudo Training Grounds MQ Lobby Left Chest", RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST },
    { "Gerudo Training Grounds MQ First Iron Knuckle Chest", RC_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST },
    { "Gerudo Training Grounds MQ Before Heavy Block Chest", RC_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST },
    { "Gerudo Training Grounds MQ Eye Statue Chest", RC_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST },
    { "Gerudo Training Grounds MQ Flame Circle Chest", RC_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST },
    { "Gerudo Training Grounds MQ Second Iron Knuckle Chest", RC_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST },
    { "Gerudo Training Grounds MQ Dinolfos Chest", RC_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST },
    { "Gerudo Training Grounds MQ Ice Arrows Chest", RC_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST },
    { "Gerudo Training Grounds MQ Maze Right Central Chest", RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST },
    { "Gerudo Training Grounds MQ Maze Path First Chest", RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST },
    { "Gerudo Training Grounds MQ Maze Right Side Chest", RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST },
    { "Gerudo Training Grounds MQ Maze Path Third Chest", RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST },
    { "Gerudo Training Grounds MQ Maze Path Second Chest", RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST },
    { "Gerudo Training Grounds MQ Hidden Ceiling Chest", RC_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST },
    { "Gerudo Training Grounds MQ Underwater Silver Rupee Chest",
      RC_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST },
    { "Gerudo Training Grounds MQ Heavy Block Chest", RC_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST },
    { "Ganon's Tower Boss Key Chest", RC_GANONS_TOWER_BOSS_KEY_CHEST },
    { "Ganon's Castle Forest Trial Chest", RC_GANONS_CASTLE_FOREST_TRIAL_CHEST },
    { "Ganon's Castle Water Trial Left Chest", RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST },
    { "Ganon's Castle Water Trial Right Chest", RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST },
    { "Ganon's Castle Shadow Trial Front Chest", RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST },
    { "Ganon's Castle Shadow Trial Golden Gauntlets Chest", RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST },
    { "Ganon's Castle Spirit Trial Crystal Switch Chest", RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST },
    { "Ganon's Castle Spirit Trial Invisible Chest", RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST },
    { "Ganon's Castle Light Trial First Left Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST },
    { "Ganon's Castle Light Trial Second Left Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST },
    { "Ganon's Castle Light Trial Third Left Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST },
    { "Ganon's Castle Light Trial First Right Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST },
    { "Ganon's Castle Light Trial Second Right Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST },
    { "Ganon's Castle Light Trial Third Right Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST },
    { "Ganon's Castle Light Trial Invisible Enemies Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST },
    { "Ganon's Castle Light Trial Lullaby Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST },
    { "Ganon's Castle Deku Scrub Center-Left", RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT },
    { "Ganon's Castle Deku Scrub Center-Right", RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT },
    { "Ganon's Castle Deku Scrub Right", RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT },
    { "Ganon's Castle Deku Scrub Left", RC_GANONS_CASTLE_DEKU_SCRUB_LEFT },
    { "Ganon's Castle MQ Water Trial Chest", RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST },
    { "Ganon's Castle MQ Forest Trial Eye Switch Chest", RC_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST },
    { "Ganon's Castle MQ Forest Trial Frozen Eye Switch Chest",
      RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST },
    { "Ganon's Castle MQ Light Trial Lullaby Chest", RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST },
    { "Ganon's Castle MQ Shadow Trial Bomb Flower Chest", RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST },
    { "Ganon's Castle MQ Shadow Trial Eye Switch Chest", RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST },
    { "Ganon's Castle MQ Spirit Trial Golden Gauntlets Chest",
      RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST },
    { "Ganon's Castle MQ Spirit Trial Sun Back Right Chest", RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST },
    { "Ganon's Castle MQ Spirit Trial Sun Back Left Chest", RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST },
    { "Ganon's Castle MQ Spirit Trial Sun Front Left Chest", RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST },
    { "Ganon's Castle MQ Spirit Trial First Chest", RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST },
    { "Ganon's Castle MQ Spirit Trial Invisible Chest", RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST },
    { "Ganon's Castle MQ Forest Trial Freestanding Key", RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY },
    { "Ganon's Castle MQ Deku Scrub Right", RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT },
    { "Ganon's Castle MQ Deku Scrub Center-Left", RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT },
    { "Ganon's Castle MQ Deku Scrub Center", RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER },
    { "Ganon's Castle MQ Deku Scrub Center-Right", RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT },
    { "Ganon's Castle MQ Deku Scrub Left", RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT },
    { "Deku Tree GS Basement Back Room", RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM },
    { "Deku Tree GS Basement Gate", RC_DEKU_TREE_GS_BASEMENT_GATE },
    { "Deku Tree GS Basement Vines", RC_DEKU_TREE_GS_BASEMENT_VINES },
    { "Deku Tree GS Compass Room", RC_DEKU_TREE_GS_COMPASS_ROOM },
    { "Deku Tree MQ GS Lobby", RC_DEKU_TREE_MQ_GS_LOBBY },
    { "Deku Tree MQ GS Compass Room", RC_DEKU_TREE_MQ_GS_COMPASS_ROOM },
    { "Deku Tree MQ GS Basement Graves Room", RC_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM },
    { "Deku Tree MQ GS Basement Back Room", RC_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM },
    { "Dodongos Cavern GS Vines Above Stairs", RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS },
    { "Dodongos Cavern GS Scarecrow", RC_DODONGOS_CAVERN_GS_SCARECROW },
    { "Dodongos Cavern GS Alcove Above Stairs", RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS },
    { "Dodongos Cavern GS Back Room", RC_DODONGOS_CAVERN_GS_BACK_ROOM },
    { "Dodongos Cavern GS Side Room Near Lower Lizalfos", RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS },
    { "Dodongos Cavern MQ GS Scrub Room", RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM },
    { "Dodongos Cavern MQ GS Song of Time Block Room", RC_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM },
    { "Dodongos Cavern MQ GS Lizalfos Room", RC_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM },
    { "Dodongos Cavern MQ GS Larvae Room", RC_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM },
    { "Dodongos Cavern MQ GS Back Room", RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA },
    { "Jabu Jabus Belly GS Lobby Basement Lower", RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER },
    { "Jabu Jabus Belly GS Lobby Basement Upper", RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER },
    { "Jabu Jabus Belly GS Near Boss", RC_JABU_JABUS_BELLY_GS_NEAR_BOSS },
    { "Jabu Jabus Belly GS Water Switch Room", RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM },
    { "Jabu Jabus Belly MQ GS Tail Parasan Room", RC_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM },
    { "Jabu Jabus Belly MQ GS Invisible Enemies Room", RC_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM },
    { "Jabu Jabus Belly MQ GS Boomerang Chest Room", RC_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM },
    { "Jabu Jabus Belly MQ GS Near Boss", RC_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS },
    { "Forest Temple GS Raised Island Courtyard", RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD },
    { "Forest Temple GS First Room", RC_FOREST_TEMPLE_GS_FIRST_ROOM },
    { "Forest Temple GS Level Island Courtyard", RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD },
    { "Forest Temple GS Lobby", RC_FOREST_TEMPLE_GS_LOBBY },
    { "Forest Temple GS Basement", RC_FOREST_TEMPLE_GS_BASEMENT },
    { "Forest Temple MQ GS First Hallway", RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY },
    { "Forest Temple MQ GS Block Push Room", RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM },
    { "Forest Temple MQ GS Raised Island Courtyard", RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD },
    { "Forest Temple MQ GS Level Island Courtyard", RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD },
    { "Forest Temple MQ GS Well", RC_FOREST_TEMPLE_MQ_GS_WELL },
    { "Fire Temple GS Song of Time Room", RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM },
    { "Fire Temple GS Boss Key Loop", RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP },
    { "Fire Temple GS Boulder Maze", RC_FIRE_TEMPLE_GS_BOULDER_MAZE },
    { "Fire Temple GS Scarecrow Top", RC_FIRE_TEMPLE_GS_SCARECROW_TOP },
    { "Fire Temple GS Scarecrow Climb", RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB },
    { "Fire Temple MQ GS Above Fire Wall Maze", RC_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE },
    { "Fire Temple MQ GS Fire Wall Maze Center", RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER },
    { "Fire Temple MQ GS Big Lava Room Open Door", RC_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR },
    { "Fire Temple MQ GS Fire Wall Maze Side Room", RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM },
    { "Fire Temple MQ GS Skull on Fire", RC_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE },
    { "Water Temple GS Behind Gate", RC_WATER_TEMPLE_GS_BEHIND_GATE },
    { "Water Temple GS Falling Platform Room", RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM },
    { "Water Temple GS Central Pillar", RC_WATER_TEMPLE_GS_CENTRAL_PILLAR },
    { "Water Temple GS Near Boss Key Chest", RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST },
    { "Water Temple GS River", RC_WATER_TEMPLE_GS_RIVER },
    { "Water Temple MQ GS Before Upper Water Switch", RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH },
    { "Water Temple MQ GS Freestanding Key Area", RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA },
    { "Water Temple MQ GS Lizalfos Hallway", RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY },
    { "Water Temple MQ GS River", RC_WATER_TEMPLE_MQ_GS_RIVER },
    { "Water Temple MQ GS Triple Wall Torch", RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH },
    { "Spirit Temple GS Hall After Sun Block Room", RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM },
    { "Spirit Temple GS Boulder Room", RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM },
    { "Spirit Temple GS Lobby", RC_SPIRIT_TEMPLE_GS_LOBBY },
    { "Spirit Temple GS Sun on Floor Room", RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM },
    { "Spirit Temple GS Metal Fence", RC_SPIRIT_TEMPLE_GS_METAL_FENCE },
    { "Spirit Temple MQ GS Symphony Room", RC_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM },
    { "Spirit Temple MQ GS Leever Room", RC_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM },
    { "Spirit Temple MQ GS Nine Thrones Room West", RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST },
    { "Spirit Temple MQ GS Nine Thrones Room North", RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH },
    { "Spirit Temple MQ GS Sun Block Room", RC_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM },
    { "Shadow Temple GS Single Giant Pot", RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT },
    { "Shadow Temple GS Falling Spikes Room", RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM },
    { "Shadow Temple GS Triple Giant Pot", RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT },
    { "Shadow Temple GS Like Like Room", RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM },
    { "Shadow Temple GS Near Ship", RC_SHADOW_TEMPLE_GS_NEAR_SHIP },
    { "Shadow Temple MQ GS Falling Spikes Room", RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM },
    { "Shadow Temple MQ GS Wind Hint Room", RC_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM },
    { "Shadow Temple MQ GS After Wind", RC_SHADOW_TEMPLE_MQ_GS_AFTER_WIND },
    { "Shadow Temple MQ GS After Ship", RC_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP },
    { "Shadow Temple MQ GS Near Boss", RC_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS },
    { "Bottom of the Well GS Like Like Cage", RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE },
    { "Bottom of the Well GS East Inner Room", RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM },
    { "Bottom of the Well GS West Inner Room", RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM },
    { "Bottom of the Well MQ GS Basement", RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT },
    { "Bottom of the Well MQ GS Coffin Room", RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM },
    { "Bottom of the Well MQ GS West Inner Room", RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM },
    { "Ice Cavern GS Push Block Room", RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM },
    { "Ice Cavern GS Spinning Scythe Room", RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM },
    { "Ice Cavern GS Heart Piece Room", RC_ICE_CAVERN_GS_HEART_PIECE_ROOM },
    { "Ice Cavern MQ GS Scarecrow", RC_ICE_CAVERN_MQ_GS_SCARECROW },
    { "Ice Cavern MQ GS Ice Block", RC_ICE_CAVERN_MQ_GS_ICE_BLOCK },
    { "Ice Cavern MQ GS Red Ice", RC_ICE_CAVERN_MQ_GS_RED_ICE },
    { "KF GS Bean Patch", RC_KF_GS_BEAN_PATCH },
    { "KF GS Know It All House", RC_KF_GS_KNOW_IT_ALL_HOUSE },
    { "KF GS House of Twins", RC_KF_GS_HOUSE_OF_TWINS },
    { "LW GS Bean Patch Near Bridge", RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE },
    { "LW GS Bean Patch Near Theater", RC_LW_GS_BEAN_PATCH_NEAR_THEATER },
    { "LW GS Above Theater", RC_LW_GS_ABOVE_THEATER },
    { "SFM GS", RC_SFM_GS },
    { "HF GS Cow Grotto", RC_HF_GS_COW_GROTTO },
    { "HF GS Near Kak Grotto", RC_HF_GS_NEAR_KAK_GROTTO },
    { "LH GS Bean Patch", RC_LH_GS_BEAN_PATCH },
    { "LH GS Small Island", RC_LH_GS_SMALL_ISLAND },
    { "LH GS Lab Wall", RC_LH_GS_LAB_WALL },
    { "LH GS Lab Crate", RC_LH_GS_LAB_CRATE },
    { "LH GS Tree", RC_LH_GS_TREE },
    { "GV GS Bean Patch", RC_GV_GS_BEAN_PATCH },
    { "GV GS Small Bridge", RC_GV_GS_SMALL_BRIDGE },
    { "GV GS Pillar", RC_GV_GS_PILLAR },
    { "GV GS Behind Tent", RC_GV_GS_BEHIND_TENT },
    { "GF GS Archery Range", RC_GF_GS_ARCHERY_RANGE },
    { "GF GS Top Floor", RC_GF_GS_TOP_FLOOR },
    { "Wasteland GS", RC_WASTELAND_GS },
    { "Colossus GS Bean Patch", RC_COLOSSUS_GS_BEAN_PATCH },
    { "Colossus GS Hill", RC_COLOSSUS_GS_HILL },
    { "Colossus GS Tree", RC_COLOSSUS_GS_TREE },
    { "OGC GS", RC_OGC_GS },
    { "HC GS Storms Grotto", RC_HC_GS_STORMS_GROTTO },
    { "HC GS Tree", RC_HC_GS_TREE },
    { "Market GS Guard House", RC_MARKET_GS_GUARD_HOUSE },
    { "Kak GS House Under Construction", RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION },
    { "Kak GS Skulltula House", RC_KAK_GS_SKULLTULA_HOUSE },
    { "Kak GS Guards House", RC_KAK_GS_GUARDS_HOUSE },
    { "Kak GS Tree", RC_KAK_GS_TREE },
    { "Kak GS Watchtower", RC_KAK_GS_WATCHTOWER },
    { "Kak GS Above Impas House", RC_KAK_GS_ABOVE_IMPAS_HOUSE },
    { "Graveyard GS Wall", RC_GRAVEYARD_GS_WALL },
    { "Graveyard GS Bean Patch", RC_GRAVEYARD_GS_BEAN_PATCH },
    { "DMC GS Bean Patch", RC_DMC_GS_BEAN_PATCH },
    { "DMC GS Crate", RC_DMC_GS_CRATE },
    { "DMT GS Bean Patch", RC_DMT_GS_BEAN_PATCH },
    { "DMT GS Near Kak", RC_DMT_GS_NEAR_KAK },
    { "DMT GS Above Dodongos Cavern", RC_DMT_GS_ABOVE_DODONGOS_CAVERN },
    { "DMT GS Falling Rocks Path", RC_DMT_GS_FALLING_ROCKS_PATH },
    { "GC GS Center Platform", RC_GC_GS_CENTER_PLATFORM },
    { "GC GS Boulder Maze", RC_GC_GS_BOULDER_MAZE },
    { "ZR GS Ladder", RC_ZR_GS_LADDER },
    { "ZR GS Tree", RC_ZR_GS_TREE },
    { "ZR GS Above Bridge", RC_ZR_GS_ABOVE_BRIDGE },
    { "ZR GS Near Raised Grottos", RC_ZR_GS_NEAR_RAISED_GROTTOS },
    { "ZD GS Frozen Waterfall", RC_ZD_GS_FROZEN_WATERFALL },
    { "ZF GS Above The Log", RC_ZF_GS_ABOVE_THE_LOG },
    { "ZF GS Hidden Cave", RC_ZF_GS_HIDDEN_CAVE },
    { "ZF GS Tree", RC_ZF_GS_TREE },
    { "LLR GS Back Wall", RC_LLR_GS_BACK_WALL },
    { "LLR GS Rain Shed", RC_LLR_GS_RAIN_SHED },
    { "LLR GS House Window", RC_LLR_GS_HOUSE_WINDOW },
    { "LLR GS Tree", RC_LLR_GS_TREE },
    { "Link's Pocket", RC_LINKS_POCKET },
    { "Queen Gohma", RC_QUEEN_GOHMA },
    { "King Dodongo", RC_KING_DODONGO },
    { "Barinade", RC_BARINADE },
    { "Phantom Ganon", RC_PHANTOM_GANON },
    { "Volvagia", RC_VOLVAGIA },
    { "Morpha", RC_MORPHA },
    { "Twinrova", RC_TWINROVA },
    { "Bongo Bongo", RC_BONGO_BONGO },
    { "Ganon", RC_UNKNOWN_CHECK },
    { "Deku Tree Queen Gohma Heart Container", RC_DEKU_TREE_QUEEN_GOHMA_HEART },
    { "Dodongos Cavern King Dodongo Heart Container", RC_DODONGOS_CAVERN_KING_DODONGO_HEART },
    { "Jabu Jabus Belly Barinade Heart Container", RC_JABU_JABUS_BELLY_BARINADE_HEART },
    { "Forest Temple Phantom Ganon Heart Container", RC_FOREST_TEMPLE_PHANTOM_GANON_HEART },
    { "Fire Temple Volvagia Heart Container", RC_FIRE_TEMPLE_VOLVAGIA_HEART },
    { "Water Temple Morpha Heart Container", RC_WATER_TEMPLE_MORPHA_HEART },
    { "Spirit Temple Twinrova Heart Container", RC_SPIRIT_TEMPLE_TWINROVA_HEART },
    { "Shadow Temple Bongo Bongo Heart Container", RC_SHADOW_TEMPLE_BONGO_BONGO_HEART },
    { "ToT Light Arrow Cutscene", RC_TOT_LIGHT_ARROWS_CUTSCENE },
    { "LW Gift From Saria", RC_LW_GIFT_FROM_SARIA },
    { "ZF Great Fairy Reward", RC_ZF_GREAT_FAIRY_REWARD },
    { "HC Great Fairy Reward", RC_HC_GREAT_FAIRY_REWARD },
    { "Colossus Great Fairy Reward", RC_COLOSSUS_GREAT_FAIRY_REWARD },
    { "DMT Great Fairy Reward", RC_DMT_GREAT_FAIRY_REWARD },
    { "DMC Great Fairy Reward", RC_DMC_GREAT_FAIRY_REWARD },
    { "OGC Great Fairy Reward", RC_OGC_GREAT_FAIRY_REWARD },
    { "Sheik in Forest", RC_SHEIK_IN_FOREST },
    { "Sheik in Crater", RC_SHEIK_IN_CRATER },
    { "Sheik in Ice Cavern", RC_SHEIK_IN_ICE_CAVERN },
    { "Sheik at Colossus", RC_SHEIK_AT_COLOSSUS },
    { "Sheik in Kakariko", RC_SHEIK_IN_KAKARIKO },
    { "Sheik at Temple", RC_SHEIK_AT_TEMPLE },
    { "Song from Impa", RC_SONG_FROM_IMPA },
    { "Song from Malon", RC_SONG_FROM_MALON },
    { "Song from Saria", RC_SONG_FROM_SARIA },
    { "Song from Composers Grave", RC_SONG_FROM_ROYAL_FAMILYS_TOMB },
    { "Song from Ocarina of Time", RC_SONG_FROM_OCARINA_OF_TIME },
    { "Song from Windmill", RC_SONG_FROM_WINDMILL },
    { "KF Links House Cow", RC_KF_LINKS_HOUSE_COW },
    { "HF Cow Grotto Cow", RC_HF_COW_GROTTO_COW },
    { "LLR Stables Left Cow", RC_LLR_STABLES_LEFT_COW },
    { "LLR Stables Right Cow", RC_LLR_STABLES_RIGHT_COW },
    { "LLR Tower Left Cow", RC_LLR_TOWER_LEFT_COW },
    { "LLR Tower Right Cow", RC_LLR_TOWER_RIGHT_COW },
    { "Kak Impas House Cow", RC_KAK_IMPAS_HOUSE_COW },
    { "DMT Cow Grotto Cow", RC_DMT_COW_GROTTO_COW },
    { "GV Cow", RC_GV_COW },
    { "Jabu Jabus Belly MQ Cow", RC_JABU_JABUS_BELLY_MQ_COW },
    { "KF Shop Item 1", RC_KF_SHOP_ITEM_1 },
    { "KF Shop Item 2", RC_KF_SHOP_ITEM_2 },
    { "KF Shop Item 3", RC_KF_SHOP_ITEM_3 },
    { "KF Shop Item 4", RC_KF_SHOP_ITEM_4 },
    { "KF Shop Item 5", RC_KF_SHOP_ITEM_5 },
    { "KF Shop Item 6", RC_KF_SHOP_ITEM_6 },
    { "KF Shop Item 7", RC_KF_SHOP_ITEM_7 },
    { "KF Shop Item 8", RC_KF_SHOP_ITEM_8 },
    { "Kak Potion Shop Item 1", RC_KAK_POTION_SHOP_ITEM_1 },
    { "Kak Potion Shop Item 2", RC_KAK_POTION_SHOP_ITEM_2 },
    { "Kak Potion Shop Item 3", RC_KAK_POTION_SHOP_ITEM_3 },
    { "Kak Potion Shop Item 4", RC_KAK_POTION_SHOP_ITEM_4 },
    { "Kak Potion Shop Item 5", RC_KAK_POTION_SHOP_ITEM_5 },
    { "Kak Potion Shop Item 6", RC_KAK_POTION_SHOP_ITEM_6 },
    { "Kak Potion Shop Item 7", RC_KAK_POTION_SHOP_ITEM_7 },
    { "Kak Potion Shop Item 8", RC_KAK_POTION_SHOP_ITEM_8 },
    { "MK Bombchu Shop Item 1", RC_MARKET_BOMBCHU_SHOP_ITEM_1 },
    { "MK Bombchu Shop Item 2", RC_MARKET_BOMBCHU_SHOP_ITEM_2 },
    { "MK Bombchu Shop Item 3", RC_MARKET_BOMBCHU_SHOP_ITEM_3 },
    { "MK Bombchu Shop Item 4", RC_MARKET_BOMBCHU_SHOP_ITEM_4 },
    { "MK Bombchu Shop Item 5", RC_MARKET_BOMBCHU_SHOP_ITEM_5 },
    { "MK Bombchu Shop Item 6", RC_MARKET_BOMBCHU_SHOP_ITEM_6 },
    { "MK Bombchu Shop Item 7", RC_MARKET_BOMBCHU_SHOP_ITEM_7 },
    { "MK Bombchu Shop Item 8", RC_MARKET_BOMBCHU_SHOP_ITEM_8 },
    { "MK Potion Shop Item 1", RC_MARKET_POTION_SHOP_ITEM_1 },
    { "MK Potion Shop Item 2", RC_MARKET_POTION_SHOP_ITEM_2 },
    { "MK Potion Shop Item 3", RC_MARKET_POTION_SHOP_ITEM_3 },
    { "MK Potion Shop Item 4", RC_MARKET_POTION_SHOP_ITEM_4 },
    { "MK Potion Shop Item 5", RC_MARKET_POTION_SHOP_ITEM_5 },
    { "MK Potion Shop Item 6", RC_MARKET_POTION_SHOP_ITEM_6 },
    { "MK Potion Shop Item 7", RC_MARKET_POTION_SHOP_ITEM_7 },
    { "MK Potion Shop Item 8", RC_MARKET_POTION_SHOP_ITEM_8 },
    { "MK Bazaar Item 1", RC_MARKET_BAZAAR_ITEM_1 },
    { "MK Bazaar Item 2", RC_MARKET_BAZAAR_ITEM_2 },
    { "MK Bazaar Item 3", RC_MARKET_BAZAAR_ITEM_3 },
    { "MK Bazaar Item 4", RC_MARKET_BAZAAR_ITEM_4 },
    { "MK Bazaar Item 5", RC_MARKET_BAZAAR_ITEM_5 },
    { "MK Bazaar Item 6", RC_MARKET_BAZAAR_ITEM_6 },
    { "MK Bazaar Item 7", RC_MARKET_BAZAAR_ITEM_7 },
    { "MK Bazaar Item 8", RC_MARKET_BAZAAR_ITEM_8 },
    { "Kak Bazaar Item 1", RC_KAK_BAZAAR_ITEM_1 },
    { "Kak Bazaar Item 2", RC_KAK_BAZAAR_ITEM_2 },
    { "Kak Bazaar Item 3", RC_KAK_BAZAAR_ITEM_3 },
    { "Kak Bazaar Item 4", RC_KAK_BAZAAR_ITEM_4 },
    { "Kak Bazaar Item 5", RC_KAK_BAZAAR_ITEM_5 },
    { "Kak Bazaar Item 6", RC_KAK_BAZAAR_ITEM_6 },
    { "Kak Bazaar Item 7", RC_KAK_BAZAAR_ITEM_7 },
    { "Kak Bazaar Item 8", RC_KAK_BAZAAR_ITEM_8 },
    { "ZD Shop Item 1", RC_ZD_SHOP_ITEM_1 },
    { "ZD Shop Item 2", RC_ZD_SHOP_ITEM_2 },
    { "ZD Shop Item 3", RC_ZD_SHOP_ITEM_3 },
    { "ZD Shop Item 4", RC_ZD_SHOP_ITEM_4 },
    { "ZD Shop Item 5", RC_ZD_SHOP_ITEM_5 },
    { "ZD Shop Item 6", RC_ZD_SHOP_ITEM_6 },
    { "ZD Shop Item 7", RC_ZD_SHOP_ITEM_7 },
    { "ZD Shop Item 8", RC_ZD_SHOP_ITEM_8 },
    { "GC Shop Item 1", RC_GC_SHOP_ITEM_1 },
    { "GC Shop Item 2", RC_GC_SHOP_ITEM_2 },
    { "GC Shop Item 3", RC_GC_SHOP_ITEM_3 },
    { "GC Shop Item 4", RC_GC_SHOP_ITEM_4 },
    { "GC Shop Item 5", RC_GC_SHOP_ITEM_5 },
    { "GC Shop Item 6", RC_GC_SHOP_ITEM_6 },
    { "GC Shop Item 7", RC_GC_SHOP_ITEM_7 },
    { "GC Shop Item 8", RC_GC_SHOP_ITEM_8 },
    { "Colossus Gossip Stone", RC_COLOSSUS_GOSSIP_STONE },
    { "DMC Gossip Stone", RC_DMC_GOSSIP_STONE },
    { "DMC Upper Grotto Gossip Stone", RC_DMC_UPPER_GROTTO_GOSSIP_STONE },
    { "DMT Gossip Stone", RC_DMT_GOSSIP_STONE },
    { "DMT Storms Grotto Gossip Stone", RC_DMT_STORMS_GROTTO_GOSSIP_STONE },
    { "Dodongo's Cavern Gossip Stone", RC_DODONGOS_CAVERN_GOSSIP_STONE },
    { "Fairy Gossip Stone", RC_FAIRY_GOSSIP_STONE },
    { "GC Maze Gossip Stone", RC_GC_MAZE_GOSSIP_STONE },
    { "GC Medigoron Gossip Stone", RC_GC_MEDIGORON_GOSSIP_STONE },
    { "GV Gossip Stone", RC_GV_GOSSIP_STONE },
    { "GY Gossip Stone", RC_GY_GOSSIP_STONE },
    { "HC Malon Gossip Stone", RC_HC_MALON_GOSSIP_STONE },
    { "HC Rock Wall Gossip Stone", RC_HC_ROCK_WALL_GOSSIP_STONE },
    { "HC Storms Grotto Gossip Stone", RC_HC_STORMS_GROTTO_GOSSIP_STONE },
    { "HF Cow Grotto Gossip Stone", RC_HF_COW_GROTTO_GOSSIP_STONE },
    { "HF Near Market Gossip Stone", RC_HF_NEAR_MARKET_GOSSIP_STONE },
    { "HF Open Grotto Gossip Stone", RC_HF_OPEN_GROTTO_GOSSIP_STONE },
    { "HF Southeast Gossip Stone", RC_HF_SOUTHEAST_GOSSIP_STONE },
    { "Jabu Gossip Stone", RC_JABU_GOSSIP_STONE },
    { "KF Deku Tree Left Gossip Stone", RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE },
    { "KF Deku Tree Right Gossip Stone", RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE },
    { "KF Gossip Stone", RC_KF_GOSSIP_STONE },
    { "KF Storms Gossip Stone", RC_KF_STORMS_GOSSIP_STONE },
    { "Kak Open Grotto Gossip Stone", RC_KAK_OPEN_GROTTO_GOSSIP_STONE },
    { "LH Lab Gossip Stone", RC_LH_LAB_GOSSIP_STONE },
    { "LH Southeast Gossip Stone", RC_LH_SOUTHEAST_GOSSIP_STONE },
    { "LH Southwest Gossip Stone", RC_LH_SOUTHWEST_GOSSIP_STONE },
    { "LW Gossip Stone", RC_LW_GOSSIP_STONE },
    { "LW Near Shortcuts Gossip Stone", RC_LW_NEAR_SHORTCUTS_GOSSIP_STONE },
    { "SFM Maze Lower Gossip Stone", RC_SFM_MAZE_LOWER_GOSSIP_STONE },
    { "SFM Maze Upper Gossip Stone", RC_SFM_MAZE_UPPER_GOSSIP_STONE },
    { "SFM Saria Gossip Stone", RC_SFM_SARIA_GOSSIP_STONE },
    { "ToT Left Center Gossip Stone", RC_TOT_LEFT_CENTER_GOSSIP_STONE },
    { "ToT Left Gossip Stone", RC_TOT_LEFT_GOSSIP_STONE },
    { "ToT Right Center Gossip Stone", RC_TOT_RIGHT_CENTER_GOSSIP_STONE },
    { "ToT Right Gossip Stone", RC_TOT_RIGHT_GOSSIP_STONE },
    { "ZD Gossip Stone", RC_ZD_GOSSIP_STONE },
    { "ZR Near Domain Gossip Stone", RC_ZR_NEAR_DOMAIN_GOSSIP_STONE },
    { "ZR Near Grottos Gossip Stone", RC_ZR_NEAR_GROTTOS_GOSSIP_STONE },
    { "ZR Open Grotto Gossip Stone", RC_ZR_OPEN_GROTTO_GOSSIP_STONE }
};

std::unordered_map<s16, s16> getItemIdToItemId = {
    { GI_BOW, ITEM_BOW },
    { GI_ARROW_FIRE, ITEM_ARROW_FIRE },
    { GI_DINS_FIRE, ITEM_DINS_FIRE },
    { GI_SLINGSHOT, ITEM_SLINGSHOT },
    { GI_OCARINA_FAIRY, ITEM_OCARINA_FAIRY },
    { GI_OCARINA_OOT, ITEM_OCARINA_TIME },
    { GI_HOOKSHOT, ITEM_HOOKSHOT },
    { GI_LONGSHOT, ITEM_LONGSHOT },
    { GI_ARROW_ICE, ITEM_ARROW_ICE },
    { GI_FARORES_WIND, ITEM_FARORES_WIND },
    { GI_BOOMERANG, ITEM_BOOMERANG },
    { GI_LENS, ITEM_LENS },
    { GI_HAMMER, ITEM_HAMMER },
    { GI_ARROW_LIGHT, ITEM_ARROW_LIGHT },
    { GI_NAYRUS_LOVE, ITEM_NAYRUS_LOVE },
    { GI_BOTTLE, ITEM_BOTTLE },
    { GI_POTION_RED, ITEM_POTION_RED },
    { GI_POTION_GREEN, ITEM_POTION_GREEN },
    { GI_POTION_BLUE, ITEM_POTION_BLUE },
    { GI_FAIRY, ITEM_FAIRY },
    { GI_FISH, ITEM_FISH },
    { GI_MILK_BOTTLE, ITEM_MILK_BOTTLE },
    { GI_LETTER_RUTO, ITEM_LETTER_RUTO },
    { GI_BLUE_FIRE, ITEM_BLUE_FIRE },
    { GI_BUGS, ITEM_BUG },
    { GI_BIG_POE, ITEM_BIG_POE },
    { GI_POE, ITEM_POE },
    { GI_WEIRD_EGG, ITEM_WEIRD_EGG },
    { GI_LETTER_ZELDA, ITEM_LETTER_ZELDA },
    { GI_POCKET_EGG, ITEM_POCKET_EGG },
    { GI_COJIRO, ITEM_COJIRO },
    { GI_ODD_MUSHROOM, ITEM_ODD_MUSHROOM },
    { GI_ODD_POTION, ITEM_ODD_POTION },
    { GI_SAW, ITEM_SAW },
    { GI_SWORD_BROKEN, ITEM_SWORD_BROKEN },
    { GI_PRESCRIPTION, ITEM_PRESCRIPTION },
    { GI_FROG, ITEM_FROG },
    { GI_EYEDROPS, ITEM_EYEDROPS },
    { GI_CLAIM_CHECK, ITEM_CLAIM_CHECK }
};

std::unordered_map<std::string, RandomizerGet> SpoilerfileGetNameToEnum = {
    { "No Item", RG_NONE },
    { "Rien", RG_NONE },
    { "Kokiri Sword", RG_KOKIRI_SWORD },
    { "Épée Kokiri", RG_KOKIRI_SWORD },
    { "Giant's Knife", RG_GIANTS_KNIFE },
    { "Lame des Géants", RG_GIANTS_KNIFE },
    { "Biggoron's Sword", RG_BIGGORON_SWORD },
    { "Épée de Biggoron", RG_BIGGORON_SWORD },
    { "Deku Shield", RG_DEKU_SHIELD },
    { "Bouclier Mojo", RG_DEKU_SHIELD },
    { "Hylian Shield", RG_HYLIAN_SHIELD },
    { "Bouclier Hylien", RG_HYLIAN_SHIELD },
    { "Mirror Shield", RG_MIRROR_SHIELD },
    { "Bouclier Miroir", RG_MIRROR_SHIELD },
    { "Goron Tunic", RG_GORON_TUNIC },
    { "Tunique Goron", RG_GORON_TUNIC },
    { "Zora Tunic", RG_ZORA_TUNIC },
    { "Tunique Zora", RG_ZORA_TUNIC },
    { "Iron Boots", RG_IRON_BOOTS },
    { "Bottes de plomb", RG_IRON_BOOTS },
    { "Hover Boots", RG_HOVER_BOOTS },
    { "Bottes des airs", RG_HOVER_BOOTS },
    { "Boomerang", RG_BOOMERANG },
    { "Lens of Truth", RG_LENS_OF_TRUTH },
    { "Monocle de Vérité", RG_LENS_OF_TRUTH },
    { "Megaton Hammer", RG_MEGATON_HAMMER },
    { "Masse des Titans", RG_MEGATON_HAMMER },
    { "Stone of Agony", RG_STONE_OF_AGONY },
    { "Pierre de Souffrance", RG_STONE_OF_AGONY },
    { "Din's Fire", RG_DINS_FIRE },
    { "Feu de Din", RG_DINS_FIRE },
    { "Farore's Wind", RG_FARORES_WIND },
    { "Vent de Farore", RG_FARORES_WIND },
    { "Nayru's Love", RG_NAYRUS_LOVE },
    { "Amour de Nayru", RG_NAYRUS_LOVE },
    { "Fire Arrow", RG_FIRE_ARROWS },
    { "Flèche de Feu", RG_FIRE_ARROWS },
    { "Ice Arrow", RG_ICE_ARROWS },
    { "Flèche de Glace", RG_ICE_ARROWS },
    { "Light Arrow", RG_LIGHT_ARROWS },
    { "Flèche de Lumière", RG_LIGHT_ARROWS },
    { "Gerudo Membership Card", RG_GERUDO_MEMBERSHIP_CARD },
    { "Carte Gerudo", RG_GERUDO_MEMBERSHIP_CARD },
    { "Magic Bean", RG_MAGIC_BEAN },
    { "Haricots Magiques", RG_MAGIC_BEAN },
    { "Magic Bean Pack", RG_MAGIC_BEAN_PACK },
    { "Paquet de Haricots Magiques", RG_MAGIC_BEAN_PACK },
    { "Double Defense", RG_DOUBLE_DEFENSE },
    { "Double Défence", RG_DOUBLE_DEFENSE },
    { "Weird Egg", RG_WEIRD_EGG },
    { "Oeuf Curieux", RG_WEIRD_EGG },
    { "Zelda's Letter", RG_ZELDAS_LETTER },
    { "Lettre de Zelda", RG_ZELDAS_LETTER },
    { "Pocket Egg", RG_POCKET_EGG },
    { "Oeuf de poche", RG_POCKET_EGG },
    { "Cojiro", RG_COJIRO },
    { "P'tit Poulet", RG_COJIRO },
    { "Odd Mushroom", RG_ODD_MUSHROOM },
    { "Champignon Suspect", RG_ODD_MUSHROOM },
    { "Odd Potion", RG_ODD_POTION },
    { "Mixture Suspecte ", RG_ODD_POTION },
    { "Poacher's Saw", RG_POACHERS_SAW },
    { "Scie du Chasseur", RG_POACHERS_SAW },
    { "Broken Goron's Sword", RG_BROKEN_SWORD },
    { "Épée Brisée de Goron", RG_BROKEN_SWORD },
    { "Prescription", RG_PRESCRIPTION },
    { "Ordonnance", RG_PRESCRIPTION },
    { "Eyeball Frog", RG_EYEBALL_FROG },
    { "Crapaud-qui-louche", RG_EYEBALL_FROG },
    { "World's Finest Eyedrops", RG_EYEDROPS },
    { "Super Gouttes", RG_EYEDROPS },
    { "Claim Check", RG_CLAIM_CHECK },
    { "Certificat", RG_CLAIM_CHECK },
    { "Gold Skulltula Token", RG_GOLD_SKULLTULA_TOKEN },
    { "Symbole de Skulltula d'Or", RG_GOLD_SKULLTULA_TOKEN },
    { "Progressive Hookshot", RG_PROGRESSIVE_HOOKSHOT },
    { "Grappin (prog.)", RG_PROGRESSIVE_HOOKSHOT },
    { "Progressive Strength Upgrade", RG_PROGRESSIVE_STRENGTH },
    { "Amélioration de Force (prog.)", RG_PROGRESSIVE_STRENGTH },
    { "Progressive Bomb Bag", RG_PROGRESSIVE_BOMB_BAG },
    { "Sac de Bombes (prog.)", RG_PROGRESSIVE_BOMB_BAG },
    { "Progressive Bow", RG_PROGRESSIVE_BOW },
    { "Arc (prog.)", RG_PROGRESSIVE_BOW },
    { "Progressive Slingshot", RG_PROGRESSIVE_SLINGSHOT },
    { "Lance-Pierre (prog.)", RG_PROGRESSIVE_SLINGSHOT },
    { "Progressive Wallet", RG_PROGRESSIVE_WALLET },
    { "Bourse (prog.)", RG_PROGRESSIVE_WALLET },
    { "Progressive Scale", RG_PROGRESSIVE_SCALE },
    { "Écaille (prog.)", RG_PROGRESSIVE_SCALE },
    { "Progressive Nut Capacity", RG_PROGRESSIVE_NUT_UPGRADE },
    { "Capacité de Noix (prog.)", RG_PROGRESSIVE_NUT_UPGRADE },
    { "Progressive Stick Capacity", RG_PROGRESSIVE_STICK_UPGRADE },
    { "Capacité de Bâtons (prog.)", RG_PROGRESSIVE_STICK_UPGRADE },
    { "Progressive Bombchu", RG_PROGRESSIVE_BOMBCHUS },
    { "Missiles (prog.)", RG_PROGRESSIVE_BOMBCHUS },
    { "Progressive Magic Meter", RG_PROGRESSIVE_MAGIC_METER },
    { "Jauge de Magie (prog.)", RG_PROGRESSIVE_MAGIC_METER },
    { "Progressive Ocarina", RG_PROGRESSIVE_OCARINA },
    { "Ocarina (prog.)", RG_PROGRESSIVE_OCARINA },
    { "Progressive Goron Sword", RG_PROGRESSIVE_GORONSWORD },
    { "Épée Goron (prog.)", RG_PROGRESSIVE_GORONSWORD },
    { "Empty Bottle", RG_EMPTY_BOTTLE },
    { "Bouteille Vide", RG_EMPTY_BOTTLE },
    { "Bottle with Milk", RG_BOTTLE_WITH_MILK },
    { "Bouteille avec du Lait", RG_BOTTLE_WITH_MILK },
    { "Bottle with Red Potion", RG_BOTTLE_WITH_RED_POTION },
    { "Bouteille avec une Potion Rouge", RG_BOTTLE_WITH_RED_POTION },
    { "Bottle with Green Potion", RG_BOTTLE_WITH_GREEN_POTION },
    { "Bouteille avec une Potion Verte", RG_BOTTLE_WITH_GREEN_POTION },
    { "Bottle with Blue Potion", RG_BOTTLE_WITH_BLUE_POTION },
    { "Bouteille avec une Potion Bleue", RG_BOTTLE_WITH_BLUE_POTION },
    { "Bottle with Fairy", RG_BOTTLE_WITH_FAIRY },
    { "Bouteille avec une Fée", RG_BOTTLE_WITH_FAIRY },
    { "Bottle with Fish", RG_BOTTLE_WITH_FISH },
    { "Bouteille avec un Poisson", RG_BOTTLE_WITH_FISH },
    { "Bottle with Blue Fire", RG_BOTTLE_WITH_BLUE_FIRE },
    { "Bouteille avec une Flamme Bleue", RG_BOTTLE_WITH_BLUE_FIRE },
    { "Bottle with Bugs", RG_BOTTLE_WITH_BUGS },
    { "Bouteille avec des Insectes", RG_BOTTLE_WITH_BUGS },
    { "Bottle with Poe", RG_BOTTLE_WITH_POE },
    { "Bouteille avec un Esprit", RG_BOTTLE_WITH_POE },
    { "Bottle with Ruto's Letter", RG_RUTOS_LETTER },
    { "Bouteille avec la Lettre de Ruto", RG_RUTOS_LETTER },
    { "Bottle with Big Poe", RG_BOTTLE_WITH_BIG_POE },
    { "Bouteille avec une Âme", RG_BOTTLE_WITH_BIG_POE },
    { "Zelda's Lullaby", RG_ZELDAS_LULLABY },
    { "Berceuse de Zelda", RG_ZELDAS_LULLABY },
    { "Epona's Song", RG_EPONAS_SONG },
    { "Chant d'Épona", RG_EPONAS_SONG },
    { "Saria's Song", RG_SARIAS_SONG },
    { "Chant de Saria", RG_SARIAS_SONG },
    { "Sun's Song", RG_SUNS_SONG },
    { "Chant du Soleil", RG_SUNS_SONG },
    { "Song of Time", RG_SONG_OF_TIME },
    { "Chant du Temps", RG_SONG_OF_TIME },
    { "Song of Storms", RG_SONG_OF_STORMS },
    { "Chant des Tempêtes", RG_SONG_OF_STORMS },
    { "Minuet of Forest", RG_MINUET_OF_FOREST },
    { "Menuet des Bois", RG_MINUET_OF_FOREST },
    { "Bolero of Fire", RG_BOLERO_OF_FIRE },
    { "Boléro du Feu", RG_BOLERO_OF_FIRE },
    { "Serenade of Water", RG_SERENADE_OF_WATER },
    { "Sérénade de l'Eau", RG_SERENADE_OF_WATER },
    { "Requiem of Spirit", RG_REQUIEM_OF_SPIRIT },
    { "Requiem des Esprits", RG_REQUIEM_OF_SPIRIT },
    { "Nocturne of Shadow", RG_NOCTURNE_OF_SHADOW },
    { "Nocturne de l'Ombre", RG_NOCTURNE_OF_SHADOW },
    { "Prelude of Light", RG_PRELUDE_OF_LIGHT },
    { "Prélude de la Lumière", RG_PRELUDE_OF_LIGHT },
    { "Great Deku Tree Map", RG_DEKU_TREE_MAP },
    { "Carte de l'Arbre Mojo", RG_DEKU_TREE_MAP },
    { "Dodongo's Cavern Map", RG_DODONGOS_CAVERN_MAP },
    { "Carte de la Caverne Dodongo", RG_DODONGOS_CAVERN_MAP },
    { "Jabu-Jabu's Belly Map", RG_JABU_JABUS_BELLY_MAP },
    { "Carte du Ventre de Jabu-Jabu", RG_JABU_JABUS_BELLY_MAP },
    { "Forest Temple Map", RG_FOREST_TEMPLE_MAP },
    { "Carte du Temple de la Forêt", RG_FOREST_TEMPLE_MAP },
    { "Fire Temple Map", RG_FIRE_TEMPLE_MAP },
    { "Carte du Temple du Feu", RG_FIRE_TEMPLE_MAP },
    { "Water Temple Map", RG_WATER_TEMPLE_MAP },
    { "Carte du Temple de l'Eau", RG_WATER_TEMPLE_MAP },
    { "Spirit Temple Map", RG_SPIRIT_TEMPLE_MAP },
    { "Carte du Temple de l'Esprit", RG_SPIRIT_TEMPLE_MAP },
    { "Shadow Temple Map", RG_SHADOW_TEMPLE_MAP },
    { "Carte du Temple de l'Ombre", RG_SHADOW_TEMPLE_MAP },
    { "Bottom of the Well Map", RG_BOTTOM_OF_THE_WELL_MAP },
    { "Carte du Puits", RG_BOTTOM_OF_THE_WELL_MAP },
    { "Ice Cavern Map", RG_ICE_CAVERN_MAP },
    { "Carte de la Caverne Polaire", RG_ICE_CAVERN_MAP },
    { "Great Deku Tree Compass", RG_DEKU_TREE_COMPASS },
    { "Boussole de l'Arbre Mojo", RG_DEKU_TREE_COMPASS },
    { "Dodongo's Cavern Compass", RG_DODONGOS_CAVERN_COMPASS },
    { "Boussole de la Caverne Dodongo", RG_DODONGOS_CAVERN_COMPASS },
    { "Jabu-Jabu's Belly Compass", RG_JABU_JABUS_BELLY_COMPASS },
    { "Boussole du Ventre de Jabu-Jabu", RG_JABU_JABUS_BELLY_COMPASS },
    { "Forest Temple Compass", RG_FOREST_TEMPLE_COMPASS },
    { "Boussole du Temple de la Forêt", RG_FOREST_TEMPLE_COMPASS },
    { "Fire Temple Compass", RG_FIRE_TEMPLE_COMPASS },
    { "Boussole du Temple du Feu", RG_FIRE_TEMPLE_COMPASS },
    { "Water Temple Compass", RG_WATER_TEMPLE_COMPASS },
    { "Boussole du Temple de l'Eau", RG_WATER_TEMPLE_COMPASS },
    { "Spirit Temple Compass", RG_SPIRIT_TEMPLE_COMPASS },
    { "Boussole du Temple des Esprits", RG_SPIRIT_TEMPLE_COMPASS },
    { "Shadow Temple Compass", RG_SHADOW_TEMPLE_COMPASS },
    { "Boussole du Temple de l'Ombre", RG_SHADOW_TEMPLE_COMPASS },
    { "Bottom of the Well Compass", RG_BOTTOM_OF_THE_WELL_COMPASS },
    { "Boussole du Puits", RG_BOTTOM_OF_THE_WELL_COMPASS },
    { "Ice Cavern Compass", RG_ICE_CAVERN_COMPASS },
    { "Boussole de la Caverne Polaire", RG_ICE_CAVERN_COMPASS },
    { "Forest Temple Big Key", RG_FOREST_TEMPLE_BOSS_KEY },
    { "Clé d'Or du Temple de la Forêt", RG_FOREST_TEMPLE_BOSS_KEY },
    { "Fire Temple Big Key", RG_FIRE_TEMPLE_BOSS_KEY },
    { "Clé d'Or du Temple du Feu", RG_FIRE_TEMPLE_BOSS_KEY },
    { "Water Temple Big Key", RG_WATER_TEMPLE_BOSS_KEY },
    { "Clé d'Or du Temple de l'Eau", RG_WATER_TEMPLE_BOSS_KEY },
    { "Spirit Temple Big Key", RG_SPIRIT_TEMPLE_BOSS_KEY },
    { "Clé d'Or du Temple de l'Esprit", RG_SPIRIT_TEMPLE_BOSS_KEY },
    { "Shadow Temple Big Key", RG_SHADOW_TEMPLE_BOSS_KEY },
    { "Clé d'Or du Temple de l'Ombre", RG_SHADOW_TEMPLE_BOSS_KEY },
    { "Ganon's Castle Big Key", RG_GANONS_CASTLE_BOSS_KEY },
    { "Clé d'Or du Château de Ganon", RG_GANONS_CASTLE_BOSS_KEY },
    { "Forest Temple Small Key", RG_FOREST_TEMPLE_SMALL_KEY },
    { "Petite Clé du Temple de la Forêt", RG_FOREST_TEMPLE_SMALL_KEY },
    { "Fire Temple Small Key", RG_FIRE_TEMPLE_SMALL_KEY },
    { "Petite Clé du Temple du Feu", RG_FIRE_TEMPLE_SMALL_KEY },
    { "Water Temple Small Key", RG_WATER_TEMPLE_SMALL_KEY },
    { "Petite Clé du Temple de l'Eau", RG_WATER_TEMPLE_SMALL_KEY },
    { "Spirit Temple Small Key", RG_SPIRIT_TEMPLE_SMALL_KEY },
    { "Petite Clé du Temple de l'Esprit", RG_SPIRIT_TEMPLE_SMALL_KEY },
    { "Shadow Temple Small Key", RG_SHADOW_TEMPLE_SMALL_KEY },
    { "Petite Clé du Temple de l'Ombre", RG_SHADOW_TEMPLE_SMALL_KEY },
    { "Bottom of the Well Small Key", RG_BOTTOM_OF_THE_WELL_SMALL_KEY },
    { "Petite Clé du Puits", RG_BOTTOM_OF_THE_WELL_SMALL_KEY },
    { "Training Grounds Small Key", RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY },
    { "Petite Clé du Gymnase Gerudo", RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY },
    { "Gerudo Fortress Small Key", RG_GERUDO_FORTRESS_SMALL_KEY },
    { "Petite Clé du Repaire des Voleurs", RG_GERUDO_FORTRESS_SMALL_KEY },
    { "Ganon's Castle Small Key", RG_GANONS_CASTLE_SMALL_KEY },
    { "Petite Clé du Château de Ganon", RG_GANONS_CASTLE_SMALL_KEY },
    { "Chest Game Small Key", RG_TREASURE_GAME_SMALL_KEY },
    { "Petite Clé du jeu la Chasse-aux-Trésors", RG_TREASURE_GAME_SMALL_KEY },
    { "Forest Temple Key Ring", RG_FOREST_TEMPLE_KEY_RING },
    { "Trousseau du Temple de la Forêt", RG_FOREST_TEMPLE_KEY_RING },
    { "Fire Temple Key Ring", RG_FIRE_TEMPLE_KEY_RING },
    { "Trousseau du Temple du Feu", RG_FIRE_TEMPLE_KEY_RING },
    { "Water Temple Key Ring", RG_WATER_TEMPLE_KEY_RING },
    { "Trousseau du Temple de l'Eau", RG_WATER_TEMPLE_KEY_RING },
    { "Spirit Temple Key Ring", RG_SPIRIT_TEMPLE_KEY_RING },
    { "Trousseau du Temple de l'Esprit", RG_SPIRIT_TEMPLE_KEY_RING },
    { "Shadow Temple Key Ring", RG_SHADOW_TEMPLE_KEY_RING },
    { "Trousseau du Temple de l'Ombre", RG_SHADOW_TEMPLE_KEY_RING },
    { "Bottom of the Well Key Ring", RG_BOTTOM_OF_THE_WELL_KEY_RING },
    { "Trousseau du Puits", RG_BOTTOM_OF_THE_WELL_KEY_RING },
    { "Training Grounds Key Ring", RG_GERUDO_TRAINING_GROUNDS_KEY_RING },
    { "Trousseau du Gymnase Gerudo", RG_GERUDO_TRAINING_GROUNDS_KEY_RING },
    { "Gerudo Fortress Key Ring", RG_GERUDO_FORTRESS_KEY_RING },
    { "Trousseau du Repaire des Voleurs", RG_GERUDO_FORTRESS_KEY_RING },
    { "Ganon's Castle Key Ring", RG_GANONS_CASTLE_KEY_RING },
    { "Trousseau du Château de Ganon", RG_GANONS_CASTLE_KEY_RING },
    { "Kokiri's Emerald", RG_KOKIRI_EMERALD },
    { "Émeraude Kokiri", RG_KOKIRI_EMERALD },
    { "Goron's Ruby", RG_GORON_RUBY },
    { "Rubis Goron", RG_GORON_RUBY },
    { "Zora's Sapphire", RG_ZORA_SAPPHIRE },
    { "Saphir Zora", RG_ZORA_SAPPHIRE },
    { "Forest Medallion", RG_FOREST_MEDALLION },
    { "Médaillon de la Forêt", RG_FOREST_MEDALLION },
    { "Fire Medallion", RG_FIRE_MEDALLION },
    { "Médaillon du Feu", RG_FIRE_MEDALLION },
    { "Water Medallion", RG_WATER_MEDALLION },
    { "Médaillon de l'Eau", RG_WATER_MEDALLION },
    { "Spirit Medallion", RG_SPIRIT_MEDALLION },
    { "Médaillon de l'Esprit", RG_SPIRIT_MEDALLION },
    { "Shadow Medallion", RG_SHADOW_MEDALLION },
    { "Médaillon de l'Ombre", RG_SHADOW_MEDALLION },
    { "Light Medallion", RG_LIGHT_MEDALLION },
    { "Médaillon de la Lumière", RG_LIGHT_MEDALLION },
    { "Recovery Heart", RG_RECOVERY_HEART },
    { "Coeur de Vie", RG_RECOVERY_HEART },
    { "Green Rupee", RG_GREEN_RUPEE },
    { "Rubis Vert", RG_GREEN_RUPEE },
    { "Blue Rupee", RG_BLUE_RUPEE },
    { "Rubis Bleu", RG_BLUE_RUPEE },
    { "Red Rupee", RG_RED_RUPEE },
    { "Rubis Rouge", RG_RED_RUPEE },
    { "Purple Rupee", RG_PURPLE_RUPEE },
    { "Rubis Pourpre", RG_PURPLE_RUPEE },
    { "Huge Rupee", RG_HUGE_RUPEE },
    { "Énorme Rubis", RG_HUGE_RUPEE },
    { "Piece of Heart", RG_PIECE_OF_HEART },
    { "Quart de Coeur", RG_PIECE_OF_HEART },
    { "Heart Container", RG_HEART_CONTAINER },
    { "Réceptacle de Coeur", RG_HEART_CONTAINER },
    { "Ice Trap", RG_ICE_TRAP },
    { "Piège de Glace", RG_ICE_TRAP },
    { "Milk", RG_MILK },
    { "Lait", RG_MILK },
    { "Bombs (5)", RG_BOMBS_5 },
    { "Bombes (5)", RG_BOMBS_5 },
    { "Bombs (10)", RG_BOMBS_10 },
    { "Bombes (10)", RG_BOMBS_10 },
    { "Bombs (20)", RG_BOMBS_20 },
    { "Bombes (20)", RG_BOMBS_20 },
    { "Bombchu (5)", RG_BOMBCHU_5 },
    { "Missiles (5)", RG_BOMBCHU_5 },
    { "Bombchu (10)", RG_BOMBCHU_10 },
    { "Missiles (10)", RG_BOMBCHU_10 },
    { "Bombchu (20)", RG_BOMBCHU_20 },
    { "Missiles (20)", RG_BOMBCHU_20 },
    { "Bombchu Drop", RG_BOMBCHU_DROP },
    { "Drop Missile", RG_BOMBCHU_DROP },
    { "Arrows (5)", RG_ARROWS_5 },
    { "Flèches (5)", RG_ARROWS_5 },
    { "Arrows (10)", RG_ARROWS_10 },
    { "Flèches (10)", RG_ARROWS_10 },
    { "Arrows (30)", RG_ARROWS_30 },
    { "Flèches (30)", RG_ARROWS_30 },
    { "Deku Nuts (5)", RG_DEKU_NUTS_5 },
    { "Noix Mojo (5)", RG_DEKU_NUTS_5 },
    { "Deku Nuts (10)", RG_DEKU_NUTS_10 },
    { "Noix Mojo (10)", RG_DEKU_NUTS_10 },
    { "Deku Seeds (30)", RG_DEKU_SEEDS_30 },
    { "Graines Mojo (30)", RG_DEKU_SEEDS_30 },
    { "Deku Stick (1)", RG_DEKU_STICK_1 },
    { "Bâton Mojo (1)", RG_DEKU_STICK_1 },
    { "Red Potion Refill", RG_RED_POTION_REFILL },
    { "Recharge de Potion Rouge", RG_RED_POTION_REFILL },
    { "Green Potion Refill", RG_GREEN_POTION_REFILL },
    { "Recharge de Potion Verte", RG_GREEN_POTION_REFILL },
    { "Blue Potion Refill", RG_BLUE_POTION_REFILL },
    { "Recharge de Potion Bleue", RG_BLUE_POTION_REFILL },
    { "Piece of Heart (Treasure Chest Minigame)", RG_TREASURE_GAME_HEART },
    { "Quart de Coeur (Chasse-aux-Trésors)", RG_TREASURE_GAME_HEART },
    { "Green Rupee (Treasure Chest Minigame)", RG_TREASURE_GAME_GREEN_RUPEE },
    { "Rubis Vert (Chasse-aux-Trésors)", RG_TREASURE_GAME_GREEN_RUPEE },
    { "Buy Deku Nut (5)", RG_BUY_DEKU_NUT_5 },
    { "Acheter: Noix Mojo (5)", RG_BUY_DEKU_NUT_5 },
    { "Buy Arrows (30)", RG_BUY_ARROWS_30 },
    { "Acheter: Flèches (30)", RG_BUY_ARROWS_30 },
    { "Buy Arrows (50)", RG_BUY_ARROWS_50 },
    { "Acheter: Flèches (50)", RG_BUY_ARROWS_50 },
    { "Buy Bombs (5) [25]", RG_BUY_BOMBS_525 },
    { "Acheter: Bombes (5) [25]", RG_BUY_BOMBS_525 },
    { "Buy Deku Nut (10)", RG_BUY_DEKU_NUT_10 },
    { "Acheter: Noix Mojo (10)", RG_BUY_DEKU_NUT_10 },
    { "Buy Deku Stick (1)", RG_BUY_DEKU_STICK_1 },
    { "Acheter: Bâton Mojo (1)", RG_BUY_DEKU_STICK_1 },
    { "Buy Bombs (10)", RG_BUY_BOMBS_10 },
    { "Acheter: Bombes ", RG_BUY_BOMBS_10 },
    { "Buy Fish", RG_BUY_FISH },
    { "Acheter: Poisson", RG_BUY_FISH },
    { "Buy Red Potion [30]", RG_BUY_RED_POTION_30 },
    { "Acheter: Potion Rouge [30]", RG_BUY_RED_POTION_30 },
    { "Buy Green Potion", RG_BUY_GREEN_POTION },
    { "Acheter: Potion Verte", RG_BUY_GREEN_POTION },
    { "Buy Blue Potion", RG_BUY_BLUE_POTION },
    { "Acheter: Potion Bleue", RG_BUY_BLUE_POTION },
    { "Buy Hylian Shield", RG_BUY_HYLIAN_SHIELD },
    { "Acheter: Bouclier Hylien", RG_BUY_HYLIAN_SHIELD },
    { "Buy Deku Shield", RG_BUY_DEKU_SHIELD },
    { "Acheter: Bouclier Mojo", RG_BUY_DEKU_SHIELD },
    { "Buy Goron Tunic", RG_BUY_GORON_TUNIC },
    { "Acheter: Tunique Goron", RG_BUY_GORON_TUNIC },
    { "Buy Zora Tunic", RG_BUY_ZORA_TUNIC },
    { "Acheter: Tunique Zora", RG_BUY_ZORA_TUNIC },
    { "Buy Heart", RG_BUY_HEART },
    { "Acheter: Coeur de Vie", RG_BUY_HEART },
    { "Buy Bombchu (10)", RG_BUY_BOMBCHU_10 },
    { "Acheter: Missiles (10)", RG_BUY_BOMBCHU_10 },
    { "Buy Bombchu (20)", RG_BUY_BOMBCHU_20 },
    { "Acheter: Missiles (20)", RG_BUY_BOMBCHU_20 },
    { "Buy Bombchu (5)", RG_BUY_BOMBCHU_5 },
    { "Acheter: Missiles (5)", RG_BUY_BOMBCHU_5 },
    { "Buy Deku Seeds (30)", RG_BUY_DEKU_SEEDS_30 },
    { "Acheter: Graines Mojo (30)", RG_BUY_DEKU_SEEDS_30 },
    { "Sold Out", RG_SOLD_OUT },
    { "Rupture de stock", RG_SOLD_OUT },
    { "Buy Blue Fire", RG_BUY_BLUE_FIRE },
    { "Acheter: Flamme Bleue", RG_BUY_BLUE_FIRE },
    { "Buy Bottle Bug", RG_BUY_BOTTLE_BUG },
    { "Acheter: Insecte en bouteille", RG_BUY_BOTTLE_BUG },
    { "Buy Poe", RG_BUY_POE },
    { "Acheter: Esprit", RG_BUY_POE },
    { "Buy Fairy's Spirit", RG_BUY_FAIRYS_SPIRIT },
    { "Acheter: Esprit de Fée", RG_BUY_FAIRYS_SPIRIT },
    { "Buy Arrows (10)", RG_BUY_ARROWS_10 },
    { "Acheter: Flèches (10)", RG_BUY_ARROWS_10 },
    { "Buy Bombs (20)", RG_BUY_BOMBS_20 },
    { "Acheter: Bombes (20)", RG_BUY_BOMBS_20 },
    { "Buy Bombs (30)", RG_BUY_BOMBS_30 },
    { "Acheter: Bombes (30)", RG_BUY_BOMBS_30 },
    { "Buy Bombs (5) [35]", RG_BUY_BOMBS_535 },
    { "Acheter: Bombes (5) [35]", RG_BUY_BOMBS_535 },
    { "Buy Red Potion [40]", RG_BUY_RED_POTION_40 },
    { "Acheter: Potion Rouge [40]", RG_BUY_RED_POTION_40 },
    { "Buy Red Potion [50]", RG_BUY_RED_POTION_50 },
    { "Acheter: Potion Rouge [50]", RG_BUY_RED_POTION_50 },
    { "Triforce", RG_TRIFORCE },
    { "Hint", RG_HINT },
    { "Indice", RG_HINT }
};

std::unordered_map<std::string, RandomizerSettingKey> SpoilerfileSettingNameToEnum = {
    { "Open Settings:Forest", RSK_FOREST },
    { "Open Settings:Kakariko Gate", RSK_KAK_GATE },
    { "Open Settings:Door of Time", RSK_DOOR_OF_TIME },
    { "Open Settings:Zora's Fountain", RSK_ZORAS_FOUNTAIN },
    { "Open Settings:Gerudo Fortress", RSK_GERUDO_FORTRESS },
    { "Open Settings:Rainbow Bridge", RSK_RAINBOW_BRIDGE },
    { "Open Settings:Stone Count", RSK_RAINBOW_BRIDGE_STONE_COUNT },
    { "Open Settings:Medallion Count", RSK_RAINBOW_BRIDGE_MEDALLION_COUNT },
    { "Open Settings:Reward Count", RSK_RAINBOW_BRIDGE_REWARD_COUNT },
    { "Open Settings:Dungeon Count", RSK_RAINBOW_BRIDGE_DUNGEON_COUNT },
    { "Open Settings:Token Count", RSK_RAINBOW_BRIDGE_TOKEN_COUNT },
    { "Open Settings:Random Ganon's Trials", RSK_RANDOM_TRIALS },
    { "Open Settings:Trial Count", RSK_TRIAL_COUNT },
    { "Shuffle Settings:Shuffle Gerudo Card", RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD },
    { "Shuffle Settings:Shuffle Cows", RSK_SHUFFLE_COWS },
    { "Shuffle Settings:Tokensanity", RSK_SHUFFLE_TOKENS },
    { "Shuffle Settings:Shuffle Adult Trade", RSK_SHUFFLE_ADULT_TRADE },
    { "Start with Deku Shield", RSK_STARTING_DEKU_SHIELD },
    { "Start with Kokiri Sword", RSK_STARTING_KOKIRI_SWORD },
    { "Start with Fairy Ocarina", RSK_STARTING_OCARINA },
    { "Shuffle Dungeon Items:Maps/Compasses", RSK_STARTING_MAPS_COMPASSES },
    { "Shuffle Dungeon Items:Small Keys", RSK_KEYSANITY },
    { "Shuffle Dungeon Items:Gerudo Fortress Keys", RSK_GERUDO_KEYS },
    { "Shuffle Dungeon Items:Boss Keys", RSK_BOSS_KEYSANITY },
    { "Shuffle Dungeon Items:Ganon's Boss Key", RSK_GANONS_BOSS_KEY },
    { "Misc Settings:Gossip Stone Hints", RSK_GOSSIP_STONE_HINTS },
    { "Misc Settings:Hint Clarity", RSK_HINT_CLARITY },
    { "Misc Settings:Hint Distribution", RSK_HINT_DISTRIBUTION },
    { "Skip Child Zelda", RSK_SKIP_CHILD_ZELDA },
    { "Start with Consumables", RSK_STARTING_CONSUMABLES },
    { "Start with Max Rupees", RSK_FULL_WALLETS },
    { "Timesaver Settings:Cuccos to return", RSK_CUCCO_COUNT },
    { "Timesaver Settings:Big Poe Target Count", RSK_BIG_POE_COUNT },
    { "Timesaver Settings:Skip Child Stealth", RSK_SKIP_CHILD_STEALTH },
    { "Timesaver Settings:Skip Epona Race", RSK_SKIP_EPONA_RACE },
    { "Timesaver Settings:Skip Tower Escape", RSK_SKIP_TOWER_ESCAPE },
    { "Timesaver Settings:Complete Mask Quest", RSK_COMPLETE_MASK_QUEST },
    { "Timesaver Settings:Enable Glitch-Useful Cutscenes", RSK_ENABLE_GLITCH_CUTSCENES },
};

s32 Randomizer::GetItemIDFromGetItemID(s32 getItemId) {
    if (getItemIdToItemId.count(getItemId) == 0) {
        return -1;
    }

    return getItemIdToItemId[getItemId];
}

std::string sanitize(std::string stringValue) {
    // Add backslashes.
    for (auto i = stringValue.begin();;) {
        auto const pos =
            std::find_if(i, stringValue.end(), [](char const c) { return '\\' == c || '\'' == c || '"' == c; });
        if (pos == stringValue.end()) {
            break;
        }
        i = std::next(stringValue.insert(pos, '\\'), 2);
    }

    // Removes others.
    stringValue.erase(std::remove_if(stringValue.begin(), stringValue.end(),
                                     [](char const c) { return '\n' == c || '\r' == c || '\0' == c || '\x1A' == c; }),
                      stringValue.end());

    return stringValue;
}

#pragma optimize("", off)
#pragma GCC push_options
#pragma GCC optimize ("O0")
bool Randomizer::SpoilerFileExists(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        std::ifstream spoilerFileStream(sanitize(spoilerFileName));
        if (!spoilerFileStream) {
            return false;
        } else {
            return true;
        }
    }

    return false;
}
#pragma GCC pop_options
#pragma optimize("", on)

void Randomizer::LoadRandomizerSettings(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseRandomizerSettingsFile(spoilerFileName);
    }

    for(auto randoSetting : gSaveContext.randoSettings) {
        if(randoSetting.key == RSK_NONE) break;
        this->randoSettings[randoSetting.key] = randoSetting.value;
    }
}

void Randomizer::LoadHintLocations(const char* spoilerFileName) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseHintLocationsFile(spoilerFileName);
    }

    CustomMessageManager::Instance->ClearMessageTable(Randomizer::hintMessageTableID);
    CustomMessageManager::Instance->AddCustomMessageTable(Randomizer::hintMessageTableID);

    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_ALTAR_CHILD,
                                           { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, gSaveContext.childAltarText,
                                             gSaveContext.childAltarText, gSaveContext.childAltarText });
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_ALTAR_ADULT,
                                           { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, gSaveContext.adultAltarText,
                                             gSaveContext.adultAltarText, gSaveContext.adultAltarText });
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_GANONDORF,
                                           { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM, gSaveContext.ganonHintText,
                                             gSaveContext.ganonHintText, gSaveContext.ganonHintText });
    CustomMessageManager::Instance->CreateMessage(
        Randomizer::hintMessageTableID, TEXT_GANONDORF_NOHINT,
                                           { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM, gSaveContext.ganonText,
                                             gSaveContext.ganonText, gSaveContext.ganonText });

    this->childAltarText = gSaveContext.childAltarText;
    this->adultAltarText = gSaveContext.adultAltarText;
    this->ganonHintText = gSaveContext.ganonHintText;
    this->ganonText = gSaveContext.ganonText;

    for (auto hintLocation : gSaveContext.hintLocations) {
        if(hintLocation.check == RC_LINKS_POCKET) break;
        this->hintLocations[hintLocation.check] = hintLocation.hintText;
        CustomMessageManager::Instance->CreateMessage(
            Randomizer::hintMessageTableID, hintLocation.check, { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, hintLocation.hintText, hintLocation.hintText, hintLocation.hintText });
    }
}

void Randomizer::LoadItemLocations(const char* spoilerFileName, bool silent) {
    if (strcmp(spoilerFileName, "") != 0) {
        ParseItemLocationsFile(spoilerFileName, silent);
    }

    for (auto itemLocation : gSaveContext.itemLocations) {
        this->itemLocations[itemLocation.check] = itemLocation.get;
    }

    itemLocations[RC_UNKNOWN_CHECK] = RG_NONE;
}

void Randomizer::ParseRandomizerSettingsFile(const char* spoilerFileName) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream)
        return;

    bool success = false;

    try {
        // clear out existing settings
        // RANDOTODO don't use magic number for settings array size
        for(size_t i = 0; i < 300; i++) {
            gSaveContext.randoSettings[i].key = RSK_NONE;
            gSaveContext.randoSettings[i].value = 0;
        }

        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;
        json settingsJson = spoilerFileJson["settings"];

        int index = 0;

        for (auto it = settingsJson.begin(); it != settingsJson.end(); ++it) {
            // todo load into cvars for UI
            
            std::string numericValueString;
            if(SpoilerfileSettingNameToEnum.count(it.key())) {
                gSaveContext.randoSettings[index].key = SpoilerfileSettingNameToEnum[it.key()];
                // this is annoying but the same strings are used in different orders
                // and i don't want the spoilerfile to just have numbers instead of
                // human readable settings values so it'll have to do for now
                switch(gSaveContext.randoSettings[index].key) {
                    case RSK_FOREST:
                        if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Closed Deku") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_KAK_GATE:
                        if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    case RSK_DOOR_OF_TIME:
                        if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Song only") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_ZORAS_FOUNTAIN:
                        if(it.value() == "Closed") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Closed as child") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_GERUDO_FORTRESS:
                        if(it.value() == "Normal") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Fast") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Open") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_RAINBOW_BRIDGE:
                        if(it.value() == "Always open") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Stones") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Medallions") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Dungeon rewards") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Dungeons") {
                            gSaveContext.randoSettings[index].value = 5;
                        } else if(it.value() == "Tokens") {
                            gSaveContext.randoSettings[index].value = 6;
                        }
                        break;
                    case RSK_RAINBOW_BRIDGE_STONE_COUNT:
                    case RSK_RAINBOW_BRIDGE_MEDALLION_COUNT:
                    case RSK_RAINBOW_BRIDGE_REWARD_COUNT:
                    case RSK_RAINBOW_BRIDGE_DUNGEON_COUNT:
                    case RSK_RAINBOW_BRIDGE_TOKEN_COUNT:
                    case RSK_TRIAL_COUNT:
                    case RSK_BIG_POE_COUNT:
                    case RSK_CUCCO_COUNT:
                        numericValueString = it.value();
                        gSaveContext.randoSettings[index].value = std::stoi(numericValueString);
                        break;
                    case RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD:
                    case RSK_SHUFFLE_COWS:
                    case RSK_SHUFFLE_ADULT_TRADE:
                    case RSK_RANDOM_TRIALS:
                    case RSK_STARTING_DEKU_SHIELD:
                    case RSK_STARTING_KOKIRI_SWORD:
                    case RSK_COMPLETE_MASK_QUEST:
                    case RSK_ENABLE_GLITCH_CUTSCENES:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "On") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    case RSK_STARTING_MAPS_COMPASSES:
                        if(it.value() == "Start With") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Own Dungeon") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = 5;
                        }
                        break;
                    case RSK_STARTING_OCARINA:
                        if(it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Fairy Ocarina") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    case RSK_GOSSIP_STONE_HINTS:
                        if(it.value() == "No Hints") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Need Nothing") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Mask of Truth") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Stone of Agony") {
                            gSaveContext.randoSettings[index].value = 3;
                        }
                        break;
                    case RSK_HINT_CLARITY:
                        if(it.value() == "Obscure") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Ambiguous") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Clear") {
                            gSaveContext.randoSettings[index].value = 2;
                        }
                        break;
                    case RSK_HINT_DISTRIBUTION:
                        if(it.value() == "Useless") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Balanced") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Strong") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Very Strong") {
                            gSaveContext.randoSettings[index].value = 3;
                        }
                        break;
                    case RSK_KEYSANITY:
                        if(it.value() == "Start With") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Own Dungeon") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = 5;
                        }
                        break;
                    case RSK_BOSS_KEYSANITY:
                        if(it.value() == "Start With") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Own Dungeon") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = 5;
                        }
                        break;
                    case RSK_GANONS_BOSS_KEY:
                        if(it.value() == "Vanilla") {
                            gSaveContext.randoSettings[index].value = 0;
                        } else if(it.value() == "Own dungeon") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if(it.value() == "Start with") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if(it.value() == "Any Dungeon") {
                            gSaveContext.randoSettings[index].value = 3;
                        } else if(it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 4;
                        } else if(it.value() == "Anywhere") {
                            gSaveContext.randoSettings[index].value = 5;
                        }
                        break;
                    case RSK_SKIP_CHILD_ZELDA:
                        gSaveContext.randoSettings[index].value = it.value();
                        break;
                    case RSK_STARTING_CONSUMABLES:
                    case RSK_FULL_WALLETS:
                        if(it.value() == "No") {
                            gSaveContext.randoSettings[index].value = 0;            
                        } else if(it.value() == "Yes") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    case RSK_SKIP_CHILD_STEALTH:
                    case RSK_SKIP_EPONA_RACE:
                    case RSK_SKIP_TOWER_ESCAPE:
                        if(it.value() == "Don't Skip") {
                            gSaveContext.randoSettings[index].value = 0;
                        } else if (it.value() == "Skip") {
                            gSaveContext.randoSettings[index].value = 1;
                        }
                        break;
                    case RSK_SHUFFLE_TOKENS:
                        if (it.value() == "Off") {
                            gSaveContext.randoSettings[index].value = 0;
                        } else if (it.value() == "Dungeons") {
                            gSaveContext.randoSettings[index].value = 1;
                        } else if (it.value() == "Overworld") {
                            gSaveContext.randoSettings[index].value = 2;
                        } else if (it.value() == "All Tokens") {
                            gSaveContext.randoSettings[index].value = 3;
                        }
                        break;
                }
                index++;
            }
        }

        success = true;
    } catch (const std::exception& e) {
        return;
    }
}

std::string AltarIconString(char iconChar) {
    std::string iconString = "";
    switch (iconChar) {
        case '0':
            // Kokiri Emerald
            iconString += 0x13;
            iconString += 0x6C;
            break;
        case '1':
            // Goron Ruby
            iconString += 0x13;
            iconString += 0x6D;
            break;
        case '2':
            // Zora Sapphire
            iconString += 0x13;
            iconString += 0x6E;
            break;
        case '3':
            // Forest Medallion
            iconString += 0x13;
            iconString += 0x66;
            break;
        case '4':
            // Fire Medallion
            iconString += 0x13;
            iconString += 0x67;
            break;
        case '5':
            // Water Medallion
            iconString += 0x13;
            iconString += 0x68;
            break;
        case '6':
            // Spirit Medallion
            iconString += 0x13;
            iconString += 0x69;
            break;
        case '7':
            // Shadow Medallion
            iconString += 0x13;
            iconString += 0x6A;
            break;
        case '8':
            // Light Medallion
            iconString += 0x13;
            iconString += 0x6B;
            break;
        case 'o':
            // Open DOT (master sword)
            iconString += 0x13;
            iconString += 0x3C;
            break;
        case 'c':
            // Closed DOT (fairy ocarina)
            iconString += 0x13;
            iconString += 0x07;
            break;
        case 'i':
            // Intended DOT (oot)
            iconString += 0x13;
            iconString += 0x08;
            break;
        case 'l':
            // Light Arrow (for bridge reqs)
            iconString += 0x13;
            iconString += 0x12;
            break;
        case 'b':
            // Boss Key (ganon boss key location)
            iconString += 0x13;
            iconString += 0x74;
            break;
        case 'L':
            // Bow with Light Arrow
            iconString += 0x13;
            iconString += 0x3A;
            break;
        case 'k':
            // Kokiri Tunic
            iconString += 0x13;
            iconString += 0x41;
            break;
    }
    return iconString;
}

std::string FormatJsonHintText(std::string jsonHint) {
    std::string formattedHintMessage = jsonHint;

    CustomMessageManager::Instance->FormatCustomMessage(formattedHintMessage);

    // add icons to altar text
    for (char iconChar : {'0', '1', '2', '3', '4', '5', '6', '7', '8', 'o', 'c', 'i', 'l', 'b', 'L', 'k'}) {
        std::string textToReplace = "$";
        textToReplace += iconChar;
        size_t start_pos = formattedHintMessage.find(textToReplace);
        if(!(start_pos == std::string::npos)) {
            std::string iconString = AltarIconString(iconChar);
            formattedHintMessage.replace(start_pos, textToReplace.length(), iconString);
        }
    }

    formattedHintMessage += 0x02;

    return formattedHintMessage;
}

void Randomizer::ParseHintLocationsFile(const char* spoilerFileName) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream)
        return;

    bool success = false;

    try {
        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;

        std::string childAltarJsonText = spoilerFileJson["childAltarText"].get<std::string>();
        std::string formattedChildAltarText = FormatJsonHintText(childAltarJsonText);
        memcpy(gSaveContext.childAltarText, formattedChildAltarText.c_str(), formattedChildAltarText.length());

        std::string adultAltarJsonText = spoilerFileJson["adultAltarText"].get<std::string>();
        std::string formattedAdultAltarText = FormatJsonHintText(adultAltarJsonText);
        memcpy(gSaveContext.adultAltarText, formattedAdultAltarText.c_str(), formattedAdultAltarText.length());

        std::string ganonHintJsonText = spoilerFileJson["ganonHintText"].get<std::string>();
        std::string formattedGanonHintJsonText = FormatJsonHintText(ganonHintJsonText);
        memcpy(gSaveContext.ganonHintText, formattedGanonHintJsonText.c_str(), formattedGanonHintJsonText.length());

        std::string ganonJsonText = spoilerFileJson["ganonText"].get<std::string>();
        std::string formattedGanonJsonText = FormatJsonHintText(ganonJsonText);
        memcpy(gSaveContext.ganonText, formattedGanonJsonText.c_str(), formattedGanonJsonText.length());

        json hintsJson = spoilerFileJson["hints"];
        int index = 0;
        for (auto it = hintsJson.begin(); it != hintsJson.end(); ++it) {
            gSaveContext.hintLocations[index].check = SpoilerfileCheckNameToEnum[it.key()];

            std::string hintMessage = FormatJsonHintText(it.value());
            memcpy(gSaveContext.hintLocations[index].hintText, hintMessage.c_str(), hintMessage.length());

            index++;
        }

        success = true;
    } catch (const std::exception& e) {
        return;
    }
}

void Randomizer::ParseItemLocationsFile(const char* spoilerFileName, bool silent) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream)
        return;

    bool success = false;

    try {
        json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;
        json locationsJson = spoilerFileJson["locations"];
        json hashJson = spoilerFileJson["file_hash"];

        int index = 0;
        for (auto it = hashJson.begin(); it != hashJson.end(); ++it) {
            gSaveContext.seedIcons[index] = gSeedTextures[it.value()].id;
            index++;
        }

        index = 0;
        for (auto it = locationsJson.begin(); it != locationsJson.end(); ++it) {
            if (it->is_structured()) {
                json itemJson = *it;
                for (auto itemit = itemJson.begin(); itemit != itemJson.end(); ++itemit) {
                    // todo handle prices
                    if (itemit.key() == "item") {

                        gSaveContext.itemLocations[index].check = SpoilerfileCheckNameToEnum[it.key()];
                        gSaveContext.itemLocations[index].get = SpoilerfileGetNameToEnum[itemit.value()];
                    }
                }
            } else {
                gSaveContext.itemLocations[index].check = SpoilerfileCheckNameToEnum[it.key()];
                gSaveContext.itemLocations[index].get = SpoilerfileGetNameToEnum[it.value()];
            }

            index++;
        }

        if(!silent) {
            Audio_PlaySoundGeneral(NA_SE_SY_CORRECT_CHIME, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
        success = true;
    } catch (const std::exception& e) {
        Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        return;
    }
}

s16 Randomizer::GetRandomizedItemId(GetItemID ogId, s16 actorId, s16 actorParams, s16 sceneNum) {
    s16 itemId = GetItemFromActor(actorId, actorParams, sceneNum, ogId);
    return itemId;
}

s16 Randomizer::GetItemFromActor(s16 actorId, s16 actorParams, s16 sceneNum, GetItemID ogItemId) {
    return GetItemFromGet(this->itemLocations[GetCheckFromActor(sceneNum, actorId, actorParams)], ogItemId);
}

s16 Randomizer::GetItemFromGet(RandomizerGet randoGet, GetItemID ogItemId) {
    switch (randoGet) {
        case RG_NONE:
            return ogItemId;

        case RG_KOKIRI_SWORD:
            return !CHECK_OWNED_EQUIP(EQUIP_SWORD, 0) ? GI_SWORD_KOKIRI : GI_RUPEE_BLUE;
        case RG_GIANTS_KNIFE:
            return GI_SWORD_KNIFE;
        case RG_BIGGORON_SWORD:
            return !CHECK_OWNED_EQUIP(EQUIP_SWORD, 2) ? GI_SWORD_BGS : GI_RUPEE_BLUE;

        case RG_DEKU_SHIELD:
            return GI_SHIELD_DEKU;
        case RG_HYLIAN_SHIELD:
            return GI_SHIELD_HYLIAN;
        case RG_MIRROR_SHIELD:
            return !CHECK_OWNED_EQUIP(EQUIP_SHIELD, 2) ? GI_SHIELD_MIRROR : GI_RUPEE_BLUE;

        case RG_GORON_TUNIC:
            return GI_TUNIC_GORON;
        case RG_ZORA_TUNIC:
            return GI_TUNIC_ZORA;

        case RG_IRON_BOOTS:
            return !CHECK_OWNED_EQUIP(EQUIP_BOOTS, 1) ? GI_BOOTS_IRON : GI_RUPEE_BLUE;
        case RG_HOVER_BOOTS:
            return !CHECK_OWNED_EQUIP(EQUIP_BOOTS, 2) ? GI_BOOTS_HOVER : GI_RUPEE_BLUE;

        case RG_BOOMERANG:
            return INV_CONTENT(ITEM_BOOMERANG) == ITEM_NONE ? GI_BOOMERANG : GI_RUPEE_BLUE;

        case RG_LENS_OF_TRUTH:
            return INV_CONTENT(ITEM_LENS) == ITEM_NONE ? GI_LENS : GI_RUPEE_BLUE;

        case RG_MEGATON_HAMMER:
            return INV_CONTENT(ITEM_HAMMER) == ITEM_NONE ? GI_HAMMER : GI_RUPEE_BLUE;

        case RG_STONE_OF_AGONY:
            return GI_STONE_OF_AGONY;

        case RG_DINS_FIRE:
            return INV_CONTENT(ITEM_DINS_FIRE) == ITEM_NONE ? GI_DINS_FIRE : GI_RUPEE_BLUE;
        case RG_FARORES_WIND:
            return INV_CONTENT(ITEM_FARORES_WIND) == ITEM_NONE ? GI_FARORES_WIND : GI_RUPEE_BLUE;
        case RG_NAYRUS_LOVE:
            return INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_NONE ? GI_NAYRUS_LOVE : GI_RUPEE_BLUE;

        case RG_FIRE_ARROWS:
            return INV_CONTENT(ITEM_ARROW_FIRE) == ITEM_NONE ? GI_ARROW_FIRE : GI_RUPEE_BLUE;
        case RG_ICE_ARROWS:
            return INV_CONTENT(ITEM_ARROW_ICE) == ITEM_NONE ? GI_ARROW_ICE : GI_RUPEE_BLUE;
        case RG_LIGHT_ARROWS:
            return INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_NONE ? GI_ARROW_LIGHT : GI_RUPEE_BLUE;
        
        case RG_DOUBLE_DEFENSE:
            return !gSaveContext.doubleDefense ? (GetItemID)RG_DOUBLE_DEFENSE : GI_RUPEE_BLUE;

        case RG_GERUDO_MEMBERSHIP_CARD:
            return GI_GERUDO_CARD;

        case RG_MAGIC_BEAN:
            return GI_BEAN;
        case RG_MAGIC_BEAN_PACK:
            return GI_BEAN; //todo make it 10 of them

        case RG_WEIRD_EGG:
            return GI_WEIRD_EGG;

        case RG_ZELDAS_LETTER:
            return GI_LETTER_ZELDA;
        case RG_RUTOS_LETTER:
            return GI_LETTER_RUTO;

        case RG_POCKET_EGG:
            return GI_POCKET_EGG;
        case RG_COJIRO:
            return GI_COJIRO;
        case RG_ODD_MUSHROOM:
            return GI_ODD_MUSHROOM;
        case RG_ODD_POTION:
            return GI_ODD_POTION;
        case RG_POACHERS_SAW:
            return GI_SAW;
        case RG_BROKEN_SWORD:
            return GI_SWORD_BROKEN;
        case RG_PRESCRIPTION:
            return GI_PRESCRIPTION;
        case RG_EYEBALL_FROG:
            return GI_FROG;
        case RG_EYEDROPS:
            return GI_EYEDROPS;
        case RG_CLAIM_CHECK:
            return GI_CLAIM_CHECK;
        
        case RG_GOLD_SKULLTULA_TOKEN:
            return GI_SKULL_TOKEN;

        case RG_PROGRESSIVE_HOOKSHOT:
            switch (gSaveContext.inventory.items[SLOT_HOOKSHOT]) {
                case ITEM_NONE:
                    return GI_HOOKSHOT;
                case ITEM_HOOKSHOT:
                    return GI_LONGSHOT;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_STRENGTH:
            switch (CUR_UPG_VALUE(UPG_STRENGTH)) {
                case 0:
                    return GI_BRACELET;
                case 1:
                    return GI_GAUNTLETS_SILVER;
                case 2:
                    return GI_GAUNTLETS_GOLD;
            }
            return GI_RUPEE_BLUE;
        
        case RG_PROGRESSIVE_BOMB_BAG:
            switch (CUR_UPG_VALUE(UPG_BOMB_BAG)) {
                case 0:
                    return GI_BOMB_BAG_20;
                case 1:
                    return GI_BOMB_BAG_30;
                case 2:
                    return GI_BOMB_BAG_40;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_BOW:
            switch (CUR_UPG_VALUE(UPG_QUIVER)) {
                case 0:
                    return GI_BOW;
                case 1:
                    return GI_QUIVER_40;
                case 2:
                    return GI_QUIVER_50;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_SLINGSHOT:
            switch (CUR_UPG_VALUE(UPG_BULLET_BAG)) {
                case 0:
                    return GI_SLINGSHOT;
                case 1:
                    return GI_BULLET_BAG_40;
                case 2:
                    return GI_BULLET_BAG_50;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_WALLET:
            switch (CUR_UPG_VALUE(UPG_WALLET)) {
                case 0:
                    return GI_WALLET_ADULT;
                case 1:
                    return GI_WALLET_GIANT;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_SCALE:
            switch (CUR_UPG_VALUE(UPG_SCALE)) {
                case 0:
                    return GI_SCALE_SILVER;
                case 1:
                    return GI_SCALE_GOLD;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_NUT_UPGRADE:
            switch (CUR_UPG_VALUE(UPG_NUTS)) {
                case 0:
                case 1:
                    return GI_NUT_UPGRADE_30;
                case 2:
                    return GI_NUT_UPGRADE_40;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_STICK_UPGRADE:
            switch (CUR_UPG_VALUE(UPG_STICKS)) {
                case 0:
                case 1:
                    return GI_STICK_UPGRADE_20;
                case 2:
                    return GI_STICK_UPGRADE_30;
            }
            return GI_RUPEE_BLUE;
        
        case RG_PROGRESSIVE_BOMBCHUS:
            return GI_BOMBCHUS_20; //todo progressive?
        
        case RG_PROGRESSIVE_MAGIC_METER:
            switch (gSaveContext.magicLevel) {
                case 0:
                    return (GetItemID)RG_MAGIC_SINGLE;
                case 1:
                    return (GetItemID)RG_MAGIC_DOUBLE;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_OCARINA:
            switch (INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_NONE) {
                case 1:
                    return GI_OCARINA_FAIRY;
                case 0:
                    return GI_OCARINA_OOT;
            }
            return GI_RUPEE_BLUE;

        case RG_PROGRESSIVE_GORONSWORD:
            return GI_SWORD_BGS; //todo progressive?

        case RG_EMPTY_BOTTLE:
            return GI_BOTTLE;
        case RG_BOTTLE_WITH_MILK:
            return GI_MILK_BOTTLE;
            
        // todo test this with keys in own dungeon
        case RG_TREASURE_GAME_SMALL_KEY:
            return GI_DOOR_KEY;

        // todo keyrings
        case RG_FOREST_TEMPLE_KEY_RING:
        case RG_FIRE_TEMPLE_KEY_RING:
        case RG_WATER_TEMPLE_KEY_RING:
        case RG_SPIRIT_TEMPLE_KEY_RING:
        case RG_SHADOW_TEMPLE_KEY_RING:
        case RG_BOTTOM_OF_THE_WELL_KEY_RING:
        case RG_GERUDO_TRAINING_GROUNDS_KEY_RING:
        case RG_GERUDO_FORTRESS_KEY_RING:
        case RG_GANONS_CASTLE_KEY_RING:
            return GI_RUPEE_BLUE;

        case RG_RECOVERY_HEART:
            return GI_HEART;

        case RG_GREEN_RUPEE:
            return GI_RUPEE_GREEN;
        case RG_BLUE_RUPEE:
            return GI_RUPEE_BLUE;
        case RG_RED_RUPEE:
            return GI_RUPEE_RED;
        case RG_PURPLE_RUPEE:
            return GI_RUPEE_PURPLE;
        case RG_HUGE_RUPEE:
            return GI_RUPEE_GOLD;

        case RG_PIECE_OF_HEART:
            return GI_HEART_PIECE;
        case RG_HEART_CONTAINER:
            // todo figure out what GI_HEART_CONTAINER_2 is
            return GI_HEART_CONTAINER;
        case RG_MILK:
            return GI_MILK; //todo logic around needing a bottle?

        case RG_BOMBS_5:
            return CUR_UPG_VALUE(UPG_BOMB_BAG) ? GI_BOMBS_5 : GI_RUPEE_BLUE;
        case RG_BOMBS_10:
            return CUR_UPG_VALUE(UPG_BOMB_BAG) ? GI_BOMBS_10 : GI_RUPEE_BLUE;
        case RG_BOMBS_20:
            return CUR_UPG_VALUE(UPG_BOMB_BAG) ? GI_BOMBS_20 : GI_RUPEE_BLUE;

        case RG_BOMBCHU_5:
            return GI_BOMBCHUS_5;
        case RG_BOMBCHU_10:
            return GI_BOMBCHUS_10;
        case RG_BOMBCHU_20:
            return GI_BOMBCHUS_20;
        case RG_BOMBCHU_DROP:
            return GI_BOMBCHUS_5; //todo figure out what we want to do for chu drops

        case RG_ARROWS_5:
            return CUR_UPG_VALUE(UPG_QUIVER) ? GI_ARROWS_SMALL : GI_RUPEE_BLUE;
        case RG_ARROWS_10:
            return CUR_UPG_VALUE(UPG_QUIVER) ? GI_ARROWS_MEDIUM : GI_RUPEE_BLUE;
        case RG_ARROWS_30:
            return CUR_UPG_VALUE(UPG_QUIVER) ? GI_ARROWS_LARGE : GI_RUPEE_BLUE;

        case RG_DEKU_NUTS_5:
            return GI_NUTS_5;
        case RG_DEKU_NUTS_10:
            return GI_NUTS_10;

        case RG_DEKU_SEEDS_30:
            return CUR_UPG_VALUE(UPG_BULLET_BAG) ? GI_SEEDS_30 : GI_RUPEE_BLUE;

        case RG_DEKU_STICK_1:
            return GI_STICKS_1;

        // RANDOTODO these won't be used until we implement shopsanity/scrub shuffle
        case RG_RED_POTION_REFILL:
        case RG_GREEN_POTION_REFILL:
        case RG_BLUE_POTION_REFILL:
            return GI_NONE;

        case RG_TREASURE_GAME_HEART:
            return GI_HEART_PIECE_WIN;
        case RG_TREASURE_GAME_GREEN_RUPEE:
            return GI_RUPEE_GREEN_LOSE; //todo figure out how this works outside of the game

        case RG_TRIFORCE:
            return GI_RUPEE_BLUE; //todo figure out what this is/does

        case RG_HINT:
            return GI_RUPEE_BLUE; //todo

        default:
            if (!IsItemVanilla(randoGet)) {
                return randoGet;
            }
            return ogItemId;
    }
}

bool Randomizer::IsItemVanilla(RandomizerGet randoGet) {
    switch (randoGet) {
        case RG_NONE:
        case RG_KOKIRI_SWORD: 
        case RG_GIANTS_KNIFE: 
        case RG_BIGGORON_SWORD:
        case RG_DEKU_SHIELD:
        case RG_HYLIAN_SHIELD:
        case RG_MIRROR_SHIELD:
        case RG_GORON_TUNIC:
        case RG_ZORA_TUNIC:
        case RG_IRON_BOOTS:
        case RG_HOVER_BOOTS:
        case RG_BOOMERANG:
        case RG_LENS_OF_TRUTH:
        case RG_MEGATON_HAMMER:
        case RG_STONE_OF_AGONY:
        case RG_DINS_FIRE:
        case RG_FARORES_WIND:
        case RG_NAYRUS_LOVE:
        case RG_FIRE_ARROWS:
        case RG_ICE_ARROWS:
        case RG_LIGHT_ARROWS:
        case RG_GERUDO_MEMBERSHIP_CARD:
        case RG_MAGIC_BEAN:
        case RG_WEIRD_EGG: 
        case RG_ZELDAS_LETTER:
        case RG_RUTOS_LETTER:
        case RG_POCKET_EGG:
        case RG_COJIRO:
        case RG_ODD_MUSHROOM:
        case RG_ODD_POTION:
        case RG_POACHERS_SAW:
        case RG_BROKEN_SWORD:
        case RG_PRESCRIPTION:
        case RG_EYEBALL_FROG:
        case RG_EYEDROPS:
        case RG_CLAIM_CHECK:
        case RG_GOLD_SKULLTULA_TOKEN:
        case RG_PROGRESSIVE_HOOKSHOT:
        case RG_PROGRESSIVE_STRENGTH:
        case RG_PROGRESSIVE_BOMB_BAG:
        case RG_PROGRESSIVE_BOW:
        case RG_PROGRESSIVE_SLINGSHOT:
        case RG_PROGRESSIVE_WALLET:
        case RG_PROGRESSIVE_SCALE:
        case RG_PROGRESSIVE_NUT_UPGRADE:
        case RG_PROGRESSIVE_STICK_UPGRADE:
        case RG_PROGRESSIVE_BOMBCHUS:
        case RG_PROGRESSIVE_OCARINA:
        case RG_PROGRESSIVE_GORONSWORD:
        case RG_EMPTY_BOTTLE:
        case RG_BOTTLE_WITH_MILK:
        case RG_RECOVERY_HEART:
        case RG_GREEN_RUPEE:
        case RG_BLUE_RUPEE:
        case RG_RED_RUPEE:
        case RG_PURPLE_RUPEE:
        case RG_HUGE_RUPEE:
        case RG_PIECE_OF_HEART:
        case RG_HEART_CONTAINER:
        case RG_MILK:
        case RG_BOMBS_5:
        case RG_BOMBS_10:
        case RG_BOMBS_20:
        case RG_BOMBCHU_5:
        case RG_BOMBCHU_10:
        case RG_BOMBCHU_20:
        case RG_BOMBCHU_DROP:
        case RG_ARROWS_5:
        case RG_ARROWS_10:
        case RG_ARROWS_30:
        case RG_DEKU_NUTS_5:
        case RG_DEKU_NUTS_10:
        case RG_DEKU_SEEDS_30:
        case RG_DEKU_STICK_1:
        case RG_RED_POTION_REFILL:
        case RG_GREEN_POTION_REFILL:
        case RG_BLUE_POTION_REFILL:
        case RG_TREASURE_GAME_HEART:
        case RG_TREASURE_GAME_GREEN_RUPEE:
        case RG_BUY_DEKU_NUT_5:
        case RG_BUY_ARROWS_30:
        case RG_BUY_ARROWS_50:
        case RG_BUY_BOMBS_525:
        case RG_BUY_DEKU_NUT_10:
        case RG_BUY_DEKU_STICK_1:
        case RG_BUY_BOMBS_10:
        case RG_BUY_FISH:
        case RG_BUY_RED_POTION_30:
        case RG_BUY_GREEN_POTION:
        case RG_BUY_BLUE_POTION:
        case RG_BUY_HYLIAN_SHIELD:
        case RG_BUY_DEKU_SHIELD:
        case RG_BUY_GORON_TUNIC:
        case RG_BUY_ZORA_TUNIC:
        case RG_BUY_HEART:
        case RG_BUY_BOMBCHU_10:
        case RG_BUY_BOMBCHU_20:
        case RG_BUY_BOMBCHU_5:
        case RG_BUY_DEKU_SEEDS_30:
        case RG_SOLD_OUT:
        case RG_BUY_BLUE_FIRE:
        case RG_BUY_BOTTLE_BUG:
        case RG_BUY_POE:
        case RG_BUY_FAIRYS_SPIRIT:
        case RG_BUY_ARROWS_10:
        case RG_BUY_BOMBS_20:
        case RG_BUY_BOMBS_30:
        case RG_BUY_BOMBS_535:
        case RG_BUY_RED_POTION_40:
        case RG_BUY_RED_POTION_50:
            return true;
        default:
            return false;
    }
}

bool Randomizer::CheckContainsVanillaItem(RandomizerCheck randoCheck) {
    RandomizerGet randoGet = this->itemLocations[randoCheck];
    return IsItemVanilla(randoGet);
}

std::string Randomizer::GetAdultAltarText() const {
    return this->adultAltarText;
}

std::string Randomizer::GetChildAltarText() const {
    return this->childAltarText;
}

std::string Randomizer::GetGanonText() const {
    return ganonText;
}

std::string Randomizer::GetGanonHintText() const {
    return ganonHintText;
}

u8 Randomizer::GetRandoSettingValue(RandomizerSettingKey randoSettingKey) {
    return this->randoSettings[randoSettingKey];
}

s16 Randomizer::GetRandomizedItemIdFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogId) {
    return GetItemFromGet(this->itemLocations[randomizerCheck], ogId);
}

RandomizerCheck Randomizer::GetCheckFromActor(s16 sceneNum, s16 actorId, s16 actorParams) {
    if (!gSaveContext.n64ddFlag) {
        return RC_UNKNOWN_CHECK;
    }

    switch(sceneNum) {
        case 0:
            switch (actorParams) {
                case 2083:
                    return RC_DEKU_TREE_MAP_CHEST;
                case 22789:
                    return RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST;
                case 161:
                    return RC_DEKU_TREE_SLINGSHOT_CHEST;
                case 2050:
                    return RC_DEKU_TREE_COMPASS_CHEST;
                case 22790:
                    return RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST;
                case 22788:
                    return RC_DEKU_TREE_BASEMENT_CHEST;
                case 8200:
                    return RC_DEKU_TREE_GS_COMPASS_ROOM;
                case 8196:
                    return RC_DEKU_TREE_GS_BASEMENT_VINES;
                case 8194:
                    return RC_DEKU_TREE_GS_BASEMENT_GATE;
                case 8193:
                    return RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM;
            }
            break;
        case 1:
            switch (actorParams) {
                case 2088:
                    return RC_DODONGOS_CAVERN_MAP_CHEST;
                case 2053:
                    return RC_DODONGOS_CAVERN_COMPASS_CHEST;
                case 22982:
                    return RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST;
                case 1604:
                    return RC_DODONGOS_CAVERN_BOMB_BAG_CHEST;
                case 21802:
                    return RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST;
                case 4372:
                    return RC_DODONGOS_CAVERN_GOSSIP_STONE;
                case 8464:
                    return RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS;
                case 8450:
                    return RC_DODONGOS_CAVERN_GS_SCARECROW;
                case 8452:
                    return RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS;
                case 8449:
                    return RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS;
                case 8456:
                    return RC_DODONGOS_CAVERN_GS_BACK_ROOM;
            }
            break;
        case 2:
            switch (actorParams) {
                case 4289:
                    return RC_JABU_JABUS_BELLY_BOOMERANG_CHEST;
                case 6178:
                    return RC_JABU_JABUS_BELLY_MAP_CHEST;
                case -18428:
                    return RC_JABU_JABUS_BELLY_COMPASS_CHEST;
                case 8712:
                    return RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM;
                case 8705:
                    return RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER;
                case 8706:
                    return RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER;
                case 8708:
                    return RC_JABU_JABUS_BELLY_GS_NEAR_BOSS;
            }
            break;
        case 3:
            switch (actorParams) {
                case 22595:
                    return RC_FOREST_TEMPLE_FIRST_ROOM_CHEST;
                case 30784:
                    return RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST;
                case 22789:
                    return RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST;
                case 6177:
                    return RC_FOREST_TEMPLE_MAP_CHEST;
                case 22601:
                    return RC_FOREST_TEMPLE_WELL_CHEST;
                case -30364:
                    return RC_FOREST_TEMPLE_EYE_SWITCH_CHEST;
                case 10222:
                    return RC_FOREST_TEMPLE_BOSS_KEY_CHEST;
                case 30786:
                    return RC_FOREST_TEMPLE_FLOORMASTER_CHEST;
                case 30797:
                    return RC_FOREST_TEMPLE_RED_POE_CHEST;
                case -20340:
                    return RC_FOREST_TEMPLE_BOW_CHEST;
                case 6159:
                    return RC_FOREST_TEMPLE_BLUE_POE_CHEST;
                case 22855:
                    return RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST;
                case 22827:
                    return RC_FOREST_TEMPLE_BASEMENT_CHEST;
                case 8962:
                    return RC_FOREST_TEMPLE_GS_FIRST_ROOM;
                case 8968:
                    return RC_FOREST_TEMPLE_GS_LOBBY;
                case 8961:
                    return RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD;
                case 8964:
                    return RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD;
                case 8976:
                    return RC_FOREST_TEMPLE_GS_BASEMENT;
            }
            break;
        case 4:
            switch (actorParams) {
                case 22593:
                    return RC_FIRE_TEMPLE_NEAR_BOSS_CHEST;
                case 31936:
                    return RC_FIRE_TEMPLE_FLARE_DANCER_CHEST;
                case 10220:
                    return RC_FIRE_TEMPLE_BOSS_KEY_CHEST;
                case 22596:
                    return RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST;
                case 22594:
                    return RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST;
                case 22595:
                    return RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST;
                case 22600:
                    return RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST;
                case 2090:
                    return RC_FIRE_TEMPLE_MAP_CHEST;
                case 22603:
                    return RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST;
                case 22598:
                    return RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST;
                case 23245:
                    return RC_FIRE_TEMPLE_SCARECROW_CHEST;
                case 2055:
                    return RC_FIRE_TEMPLE_COMPASS_CHEST;
                case 421:
                    return RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST;
                case 22601:
                    return RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST;
                case 9218:
                    return RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP;
                case 9217:
                    return RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM;
                case 9220:
                    return RC_FIRE_TEMPLE_GS_BOULDER_MAZE;
                case 9232:
                    return RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB;
                case 9224:
                    return RC_FIRE_TEMPLE_GS_SCARECROW_TOP;
            }
            break;
        case 5:
            switch (actorParams) {
                case 2057:
                    return RC_WATER_TEMPLE_COMPASS_CHEST;
                case 6178:
                    return RC_WATER_TEMPLE_MAP_CHEST;
                case 22592:
                    return RC_WATER_TEMPLE_CRACKED_WALL_CHEST;
                case 30785:
                    return RC_WATER_TEMPLE_TORCHES_CHEST;
                case 10213:
                    return RC_WATER_TEMPLE_BOSS_KEY_CHEST;
                case 22598:
                    return RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST;
                case 22600:
                    return RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST;
                case 295:
                    return RC_WATER_TEMPLE_LONGSHOT_CHEST;
                case 22595:
                    return RC_WATER_TEMPLE_RIVER_CHEST;
                case 22602:
                    return RC_WATER_TEMPLE_DRAGON_CHEST;
                case 9473:
                    return RC_WATER_TEMPLE_GS_BEHIND_GATE;
                case 9480:
                    return RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST;
                case 9476:
                    return RC_WATER_TEMPLE_GS_CENTRAL_PILLAR;
                case 9474:
                    return RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM;
                case 9488:
                    return RC_WATER_TEMPLE_GS_RIVER;
            }
            break;
        case 6:
            switch (actorParams) {
                case 21800:
                    return RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST;
                case -30656:
                    return RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST;
                case -32666:
                    return RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST;
                case -31444:
                    return RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST;
                case -18397:
                    return RC_SPIRIT_TEMPLE_MAP_CHEST;
                case -30655:
                    return RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST;
                case 14340:
                    return RC_SPIRIT_TEMPLE_COMPASS_CHEST;
                case 22599:
                    return RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST;
                case -30451:
                    return RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST;
                case -28786:
                    return RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST;
                case -30577:
                    return RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST;
                case -30654:
                    return RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST;
                case 22597:
                    return RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST;
                case 26900:
                    return RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST;
                case 26901:
                    return RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST;
                case 10218:
                    return RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST;
                case -29454:
                    return RC_SPIRIT_TEMPLE_TOPMOST_CHEST;
                case 9744:
                    return RC_SPIRIT_TEMPLE_GS_METAL_FENCE;
                case 9736:
                    return RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM;
                case 9729:
                    return RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM;
                case 9732:
                    return RC_SPIRIT_TEMPLE_GS_LOBBY;
                case 9730:
                    return RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM;
            }
            break;
        case 7:
            switch (actorParams) {
                case 273:
                    return RC_SHADOW_TEMPLE_FREESTANDING_KEY;
                case 6177:
                    return RC_SHADOW_TEMPLE_MAP_CHEST;
                case 5607:
                    return RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST;
                case 6147:
                    return RC_SHADOW_TEMPLE_COMPASS_CHEST;
                case 22594:
                    return RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST;
                case 22668:
                    return RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST;
                case 26998:
                    return RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST;
                case 22853:
                    return RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST;
                case 22662:
                    return RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST;
                case -30652:
                    return RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST;
                case 30857:
                    return RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST;
                case 26965:
                    return RC_SHADOW_TEMPLE_WIND_HINT_CHEST;
                case 30856:
                    return RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST;
                case 26708:
                    return RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST;
                case 22666:
                    return RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST;
                case 10219:
                    return RC_SHADOW_TEMPLE_BOSS_KEY_CHEST;
                case 30797:
                    return RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST;
                case 9992:
                    return RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM;
                case 9986:
                    return RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM;
                case 9985:
                    return RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT;
                case 10000:
                    return RC_SHADOW_TEMPLE_GS_NEAR_SHIP;
                case 9988:
                    return RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT;
            }
            break;
        case 8:
            switch (actorParams) {
                case 273:
                    return RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY;
                case 22600:
                    return RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST;
                case 20578:
                    return RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST;
                case 23684:
                    return RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST;
                case 22793:
                    return RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST;
                case 2049:
                    return RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST;
                case 20558:
                    return RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST;
                case 22597:
                    return RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST;
                case 21802:
                    return RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST;
                case 21836:
                    return RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST;
                case 2087:
                    return RC_BOTTOM_OF_THE_WELL_MAP_CHEST;
                case 23760:
                    return RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST;
                case 4419:
                    return RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST;
                case 27348:
                    return RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST;
                case 10244:
                    return RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM;
                case 10242:
                    return RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM;
                case 10241:
                    return RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE;
            }
            break;
        case 9:
            switch (actorParams) {
                case 2080:
                    return RC_ICE_CAVERN_MAP_CHEST;
                case 2049:
                    return RC_ICE_CAVERN_COMPASS_CHEST;
                case 5570:
                    return RC_ICE_CAVERN_IRON_BOOTS_CHEST;
                case 262:
                    return RC_ICE_CAVERN_FREESTANDING_POH;
                case 10498:
                    return RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM;
                case 10500:
                    return RC_ICE_CAVERN_GS_HEART_PIECE_ROOM;
                case 10497:
                    return RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM;
            }
            break;
        case 10:
            switch (actorParams) {
                case 10219:
                    return RC_GANONS_TOWER_BOSS_KEY_CHEST;
            }
            break;
        case 11:
            switch (actorParams) {
                case 273:
                    return RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY;
                case -30573:
                    return RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST;
                case -30393:
                    return RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST;
                case -30656:
                    return RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST;
                case 31089:
                    return RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST;
                case 31439:
                    return RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST;
                case 30862:
                    return RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST;
                case 26708:
                    return RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST;
                case 24450:
                    return RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST;
                case -30653:
                    return RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST;
                case 22596:
                    return RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST;
                case 31058:
                    return RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST;
                case 22608:
                    return RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST;
                case 23877:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST;
                case 22888:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST;
                case -30643:
                    return RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST;
                case -30655:
                    return RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST;
                case 22603:
                    return RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST;
                case 23206:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST;
                case 22986:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST;
                case 22889:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST;
                case 2860:
                    return RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST;
            }
            break;
        case 12:
            switch(actorParams) {
                case 3089:
                    return RC_GF_NORTH_F1_CARPENTER;
                case 3857:
                    return RC_GF_SOUTH_F2_CARPENTER;
                case 3601:
                    return RC_GF_SOUTH_F1_CARPENTER;
                case 2577:
                    return RC_GF_NORTH_F2_CARPENTER;
            }
            break;
        case 13:
            switch (actorParams) {
                case 30857:
                    return RC_GANONS_CASTLE_FOREST_TRIAL_CHEST;
                case 24455:
                    return RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST;
                case 22790:
                    return RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST;
                case 22664:
                    return RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST;
                case 14021:
                    return RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST;
                case 22668:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST;
                case 24459:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST;
                case 22797:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST;
                case 24462:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST;
                case 22890:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST;
                case 24463:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST;
                case 30800:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST;
                case -30639:
                    return RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST;
                case -29326:
                    return RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST;
                case 26964:
                    return RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST;
            }
            break;
        case 16:
            if(actorParams == 20170) return RC_MARKET_TREASURE_CHEST_GAME_REWARD;

            // RANDOTODO update logic to match 3ds rando when we implement keysanity
            // keep keys og
            if ((actorParams & 0x60) == 0x20) break;

            if (GetRandoSettingValue(RSK_SHUFFLE_CHEST_MINIGAME)) {
                if((actorParams & 0xF) < 2) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_1;
                if((actorParams & 0xF) < 4) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_2;
                if((actorParams & 0xF) < 6) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_3;
                if((actorParams & 0xF) < 8) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_4;
                if((actorParams & 0xF) < 10) return RC_MARKET_TREASURE_CHEST_GAME_ITEM_5;
            }
            break;
        case 17:
            switch (actorId) {
                case 95:
                    return RC_DEKU_TREE_QUEEN_GOHMA_HEART;
                case 93:
                    return RC_QUEEN_GOHMA;
            }
            break;
        case 18:
            switch (actorId) {
                case 10:
                    return RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST;
                case 95:
                    return RC_DODONGOS_CAVERN_KING_DODONGO_HEART;
                case 93:
                    return RC_KING_DODONGO;
            }
            break;
        case 19:
            switch (actorId) {
                case 95:
                    return RC_JABU_JABUS_BELLY_BARINADE_HEART;
                case 93:
                    return RC_BARINADE;
            }
            break;
        case 20:
            switch (actorId) {
                case 95:
                    return RC_FOREST_TEMPLE_PHANTOM_GANON_HEART;
                case 93:
                    return RC_PHANTOM_GANON;
            }
            break;
        case 21:
            switch (actorId) {
                case 95:
                    return RC_FIRE_TEMPLE_VOLVAGIA_HEART;
                case 93:
                    return RC_VOLVAGIA;
            }
            break;
        case 22:
            switch (actorId) {
                case 95:
                    return RC_WATER_TEMPLE_MORPHA_HEART;
                case 93:
                    return RC_MORPHA;
            }
            break;
        case 23:
            switch (actorId) {
                case 95:
                    return RC_SPIRIT_TEMPLE_TWINROVA_HEART;
                case 93:
                    return RC_TWINROVA;
            }
            break;
        case 24:
            switch (actorId) {
                case 95:
                    return RC_SHADOW_TEMPLE_BONGO_BONGO_HEART;
                case 93:
                    return RC_BONGO_BONGO;
            }
            break;
        case 35:
        case 36:
        case 37:
            switch (actorParams) {
                case 14342:
                    return RC_TOT_LEFT_GOSSIP_STONE;
                case 14599:
                    return RC_TOT_LEFT_CENTER_GOSSIP_STONE;
                case 14862:
                    return RC_TOT_RIGHT_CENTER_GOSSIP_STONE;
                case 15120:
                    return RC_TOT_RIGHT_GOSSIP_STONE;
            }
            break;
        case 40:
            switch (actorParams) {
                case 22944:
                    return RC_KF_MIDOS_TOP_LEFT_CHEST;
                case 22945:
                    return RC_KF_MIDOS_TOP_RIGHT_CHEST;
                case 22914:
                    return RC_KF_MIDOS_BOTTOM_LEFT_CHEST;
                case 22787:
                    return RC_KF_MIDOS_BOTTOM_RIGHT_CHEST;
            }
            break;
        case 55:
            switch (actorParams) {
                case 262:
                    return RC_KAK_IMPAS_HOUSE_FREESTANDING_POH;
            }
            break;
        case 56:
            switch (actorParams) {
                case -28152:
                    return RC_LH_GS_LAB_CRATE;
            }
            break;
        case 59:
            // using fountainType as actorParams
            switch (actorParams) {
                case 1:
                    return RC_DMT_GREAT_FAIRY_REWARD;
                case 2:
                    return RC_DMC_GREAT_FAIRY_REWARD;
                case 3:
                    return RC_OGC_GREAT_FAIRY_REWARD;
            }
            break;
        case 61:
            // using fountainType as actorParams
            switch (actorParams) {
                case 1:
                    return RC_ZF_GREAT_FAIRY_REWARD;
                case 2:
                    return RC_HC_GREAT_FAIRY_REWARD;
                case 3:
                    return RC_COLOSSUS_GREAT_FAIRY_REWARD;
            }
            break;
        case 62:
            switch (actorParams) {
                case 2:
                    return RC_HF_DEKU_SCRUB_GROTTO;
                case 10:
                    return RC_LW_DEKU_SCRUB_GROTTO_FRONT;
                case 22988:
                    return RC_KF_STORMS_GROTTO_CHEST;
                case -22988:
                    return RC_KF_STORMS_GOSSIP_STONE;
                case 22964:
                    return RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST;
                case -22964:
                    return RC_LW_NEAR_SHORTCUTS_GOSSIP_STONE;
                case 31409:
                    return RC_SFM_WOLFOS_GROTTO_CHEST;
                case 22944:
                    return RC_HF_NEAR_MARKET_GROTTO_CHEST;
                case -22944:
                    return RC_HF_NEAR_MARKET_GOSSIP_STONE;
                case 22978:
                    return RC_HF_SOUTHEAST_GROTTO_CHEST;
                case -22978:
                    return RC_HF_SOUTHEAST_GOSSIP_STONE;
                case 22947:
                    return RC_HF_OPEN_GROTTO_CHEST;
                case -22947:
                    return RC_HF_OPEN_GROTTO_GOSSIP_STONE;
                case 22984:
                    return RC_KAK_OPEN_GROTTO_CHEST;
                case -22984:
                    return RC_KAK_OPEN_GROTTO_GOSSIP_STONE;
                case 31434:
                    return RC_KAK_REDEAD_GROTTO_CHEST;
                case 23255:
                    return RC_DMT_STORMS_GROTTO_CHEST;
                case -23255:
                    return RC_DMT_STORMS_GROTTO_GOSSIP_STONE;
                case 23802:
                    return RC_DMC_UPPER_GROTTO_CHEST;
                case -23802:
                    return RC_DMC_UPPER_GROTTO_GOSSIP_STONE;
                case 22985:
                    return RC_ZR_OPEN_GROTTO_CHEST;
                case -22985:
                    return RC_ZR_OPEN_GROTTO_GOSSIP_STONE;
                case 262:
                    return RC_HF_TEKTITE_GROTTO_FREESTANDING_POH;
                case 14363:
                    return RC_HF_COW_GROTTO_GOSSIP_STONE;
                case 14355:
                    return RC_HC_STORMS_GROTTO_GOSSIP_STONE;
                case 10753:
                    return RC_HF_GS_COW_GROTTO;
                case 10754:
                    return RC_HF_GS_NEAR_KAK_GROTTO;
                case 11778:
                    return RC_HC_GS_STORMS_GROTTO;
                default:
                    return RC_UNKNOWN_CHECK;
            }
            break;
        case 63:
            switch (actorParams) {
                case -22592:
                    return RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST;
            }
            break;
        case 64:
            switch (actorParams) {
                case 21824:
                    return RC_GRAVEYARD_SHIELD_GRAVE_CHEST;
            }
            break;
        case 65:
            switch (actorParams) {
                case -32736:
                    return RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST;
            }
            break;
        case 72:
            switch (actorParams) {
                case 4352:
                    return RC_GRAVEYARD_HOOKSHOT_CHEST;
                case 262:
                    return RC_KAK_WINDMILL_FREESTANDING_POH;
                case 1798:
                    return RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH;
            }
            break;
        case 76:
            switch (actorParams) {
                case 262:
                    return RC_LLR_FREESTANDING_POH;
            }
            break;
        case 77:
            switch (actorParams) {
                case -29176:
                    return RC_MARKET_GS_GUARD_HOUSE;
            }
            break;
        case 82:
            switch (actorId) {
                case 316:
                    if (LINK_IS_ADULT) {
                        return RC_KAK_ANJU_AS_ADULT;
                    } else {
                        return RC_KAK_ANJU_AS_CHILD;
                    }
            }
            switch (actorParams) {
                case -28640:
                    return RC_KAK_GS_TREE;
                case 20482:
                    return RC_KAK_GS_GUARDS_HOUSE;
                case 20484:
                    return RC_KAK_GS_WATCHTOWER;
                case 20496:
                    return RC_KAK_GS_SKULLTULA_HOUSE;
                case 20488:
                    return RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION;
                case 20544:
                    return RC_KAK_GS_ABOVE_IMPAS_HOUSE;
            }
            break;
        case 83:
            switch (actorParams) {
                case 1030:
                    return RC_GRAVEYARD_FREESTANDING_POH;
                case 7942:
                    return RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR;
                case 14346:
                    return RC_GY_GOSSIP_STONE;
                case 28673:
                    return RC_GRAVEYARD_GS_BEAN_PATCH;
                case 20608:
                    return RC_GRAVEYARD_GS_WALL;
            }
            break;
        case 84:
            switch (actorParams) {
                case 1030:
                    return RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH;
                case 2822:
                    return RC_ZR_NEAR_DOMAIN_FREESTANDING_POH;
                case 14605:
                    return RC_ZR_NEAR_GROTTOS_GOSSIP_STONE;
                case 14860:
                    return RC_ZR_NEAR_DOMAIN_GOSSIP_STONE;
                case -28414:
                    return RC_ZR_GS_TREE;
                case 20737:
                    return RC_ZR_GS_LADDER;
                case 20752:
                    return RC_ZR_GS_NEAR_RAISED_GROTTOS;
                case 20744:
                    return RC_ZR_GS_ABOVE_BRIDGE;
            }
            break;
        case 85:
            switch (actorParams) {
                case 1248:
                    return RC_KF_KOKIRI_SWORD_CHEST;
                case 14366:
                    return RC_KF_GOSSIP_STONE;
                case 14880:
                    return RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE;
                case 14623:
                    return RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE;
                case 19458:
                    return RC_KF_GS_KNOW_IT_ALL_HOUSE;
                case 27649:
                    return RC_KF_GS_BEAN_PATCH;
                case 19460:
                    return RC_KF_GS_HOUSE_OF_TWINS;
            }
            break;
        case 86:
            switch (actorParams) {
                case 14358:
                    return RC_SFM_MAZE_LOWER_GOSSIP_STONE;
                case 14615:
                    return RC_SFM_MAZE_UPPER_GOSSIP_STONE;
                case 14876:
                    return RC_SFM_SARIA_GOSSIP_STONE;
                case 19720:
                    return RC_SFM_GS;
            }
            break;
        case 87:
            switch (actorId) {
                case 271:
                    if (LINK_IS_ADULT) {
                        return RC_LH_SUN;
                    } else {
                        return RC_LH_UNDERWATER_ITEM;
                    }
            }
            switch(actorParams) {
                case 7686:
                    return RC_LH_FREESTANDING_POH;
                case 14339:
                    return RC_LH_LAB_GOSSIP_STONE;
                case 14600:
                    return RC_LH_SOUTHWEST_GOSSIP_STONE;
                case 14863:
                    return RC_LH_SOUTHEAST_GOSSIP_STONE;
                case 29185:
                    return RC_LH_GS_BEAN_PATCH;
                case 20996:
                    return RC_LH_GS_LAB_WALL;
                case 20994:
                    return RC_LH_GS_SMALL_ISLAND;
                case 21008:
                    return RC_LH_GS_TREE;
            }
            break;
        case 88:
            switch (actorParams) {
                case -18496:
                    return RC_ZD_CHEST;
                case 14345:
                    return RC_ZD_GOSSIP_STONE;
                case 20800:
                    return RC_ZD_GS_FROZEN_WATERFALL;
            }
            break;
        case 89:
            switch (actorParams) {
                case 5126:
                    return RC_ZF_BOTTOM_FREESTANDING_POH;
                case 262:
                    return RC_ZF_ICEBERC_FREESTANDING_POH;
                case 15362:
                case 14594:
                    return RC_JABU_GOSSIP_STONE;
                case 14849:
                case 14337:
                    return RC_FAIRY_GOSSIP_STONE;
                case 20740:
                    return RC_ZF_GS_ABOVE_THE_LOG;
                case -28288:
                    return RC_ZF_GS_TREE;
                case 20768:
                    return RC_ZF_GS_HIDDEN_CAVE;
            }
            break;
        case 90:
            switch (actorParams) {
                case 23200:
                    return RC_GV_CHEST;
                case 262:
                    return RC_GV_WATERFALL_FREESTANDING_POH;
                case 518:
                    return RC_GV_CRATE_FREESTANDING_POH;
                case 14353:
                    return RC_GV_GOSSIP_STONE;
                case 21250:
                    return RC_GV_GS_SMALL_BRIDGE;
                case 29441:
                    return RC_GV_GS_BEAN_PATCH;
                case 21256:
                    return RC_GV_GS_BEHIND_TENT;
                case 21252:
                    return RC_GV_GS_PILLAR;
            }
            break;
        case 91:
            switch (actorParams) {
                case 9:
                    return RC_LW_DEKU_SCRUB_NEAR_BRIDGE;
                case 14365:
                    return RC_LW_GOSSIP_STONE;
                case 27905:
                    return RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE;
                case 27906:
                    return RC_LW_GS_BEAN_PATCH_NEAR_THEATER;
                case 19716:
                    return RC_LW_GS_ABOVE_THEATER;
            }
            break;
        case 92:
            switch (actorParams) {
                case 1707:
                    return RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST;
                case 13673:
                    return RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST;
                case 3334:
                    return RC_COLOSSUS_FREESTANDING_POH;
                case 14362:
                    return RC_COLOSSUS_GOSSIP_STONE;
                case 29953:
                    return RC_COLOSSUS_GS_BEAN_PATCH;
                case 21768:
                    return RC_COLOSSUS_GS_TREE;
                case 21764:
                    return RC_COLOSSUS_GS_HILL;
            }
            break;
        case 93:
            switch (actorParams) {
                case 262:
                case 1984:
                    return RC_GF_CHEST;
                case 21506:
                    return RC_GF_GS_TOP_FLOOR;
                case 21505:
                    return RC_GF_GS_ARCHERY_RANGE;
            }
            break;
        case 94:
            switch (actorParams) {
                case -30048:
                    return RC_WASTELAND_CHEST;
                case 13570:
                    return RC_WASTELAND_GS;
            }
            break;
        case 95:
            switch (actorParams) {
                case 14610:
                    return RC_HC_MALON_GOSSIP_STONE;
                case 14347:
                    return RC_HC_ROCK_WALL_GOSSIP_STONE;
                case -29180:
                    return RC_HC_GS_TREE;
            }
            break;
        case 96:
            switch (actorParams) {
                case 7686:
                    return RC_DMT_FREESTANDING_POH;
                case 23201:
                    return RC_DMT_CHEST;
                case 14340:
                    return RC_DMT_GOSSIP_STONE;
                case 12036:
                    return RC_DMT_GS_NEAR_KAK;
                case 28418:
                    return RC_DMT_GS_BEAN_PATCH;
                case 20232:
                    return RC_DMT_GS_ABOVE_DODONGOS_CAVERN;
                case 20240:
                    return RC_DMT_GS_FALLING_ROCKS_PATH;
            }
            break;
        case 97:
            switch (actorParams) {
                case 2054:
                    return RC_DMC_VOLCANO_FREESTANDING_POH;
                case 518:
                    return RC_DMC_WALL_FREESTANDING_POH;
                case 14341:
                    return RC_DMC_GOSSIP_STONE;
                case -28800:
                    return RC_DMC_GS_CRATE;
                case 28417:
                    return RC_DMC_GS_BEAN_PATCH;
            }
            break;
        case 98:
            switch (actorParams) {
                case 23232:
                    return RC_GC_MAZE_LEFT_CHEST;
                case 23201:
                    return RC_GC_MAZE_RIGHT_CHEST;
                case 23202:
                    return RC_GC_MAZE_CENTER_CHEST;
                case 7942:
                    return RC_GC_POT_FREESTANDING_POH;
                case 14357:
                    return RC_GC_MAZE_GOSSIP_STONE;
                case 14873:
                    return RC_GC_MEDIGORON_GOSSIP_STONE;
                case 12064:
                    return RC_GC_GS_CENTER_PLATFORM;
                case -28864:
                    return RC_GC_GS_BOULDER_MAZE;
            }
            break;
        case 99:
            switch (actorParams) {
                case 11012:
                    return RC_LLR_GS_HOUSE_WINDOW;
                case -29944:
                    return RC_LLR_GS_TREE;
                case 11010:
                    return RC_LLR_GS_RAIN_SHED;
                case 11009:
                    return RC_LLR_GS_BACK_WALL;
            }
            break;
        case 100:
            switch (actorParams) {
                case 11777:
                    return RC_OGC_GS;
            }
            break;
    }

    return RC_UNKNOWN_CHECK;
}

std::thread randoThread;

void GenerateRandomizerImgui() {
    CVar_SetS32("gRandoGenerating", 1);
    CVar_Save();

    std::unordered_map<RandomizerSettingKey, u8> cvarSettings;
    cvarSettings[RSK_LOGIC_RULES] = CVar_GetS32("gRandomizeLogicRules", 0);
    cvarSettings[RSK_FOREST] = CVar_GetS32("gRandomizeForest", 0);
    cvarSettings[RSK_KAK_GATE] = CVar_GetS32("gRandomizeKakarikoGate", 0);
    cvarSettings[RSK_DOOR_OF_TIME] = CVar_GetS32("gRandomizeDoorOfTime", 0);
    cvarSettings[RSK_ZORAS_FOUNTAIN] = CVar_GetS32("gRandomizeZorasFountain", 0);
    cvarSettings[RSK_GERUDO_FORTRESS] = CVar_GetS32("gRandomizeGerudoFortress", 0);
    cvarSettings[RSK_RAINBOW_BRIDGE] = CVar_GetS32("gRandomizeRainbowBridge", 0);
    cvarSettings[RSK_RAINBOW_BRIDGE_STONE_COUNT] = CVar_GetS32("gRandomizeStoneCount", 3);
    cvarSettings[RSK_RAINBOW_BRIDGE_MEDALLION_COUNT] = CVar_GetS32("gRandomizeMedallionCount", 6);
    cvarSettings[RSK_RAINBOW_BRIDGE_REWARD_COUNT] = CVar_GetS32("gRandomizeRewardCount", 9);
    cvarSettings[RSK_RAINBOW_BRIDGE_DUNGEON_COUNT] = CVar_GetS32("gRandomizeDungeonCount", 8);
    cvarSettings[RSK_RAINBOW_BRIDGE_TOKEN_COUNT] = CVar_GetS32("gRandomizeTokenCount", 100);
    cvarSettings[RSK_RANDOM_TRIALS] = CVar_GetS32("gRandomizeGanonTrial", 0);
    cvarSettings[RSK_TRIAL_COUNT] = CVar_GetS32("gRandomizeGanonTrialCount", 0);
    cvarSettings[RSK_STARTING_OCARINA] = CVar_GetS32("gRandomizeStartingOcarina", 0);
    cvarSettings[RSK_SHUFFLE_OCARINA] = CVar_GetS32("gRandomizeShuffleOcarinas", 0) ||
                                        CVar_GetS32("gRandomizeStartingOcarina", 0);
    cvarSettings[RSK_STARTING_KOKIRI_SWORD] = CVar_GetS32("gRandomizeStartingKokiriSword", 0);
    cvarSettings[RSK_SHUFFLE_KOKIRI_SWORD] = CVar_GetS32("gRandomizeShuffleKokiriSword", 0) ||
                                             CVar_GetS32("gRandomizeStartingKokiriSword", 0);
    cvarSettings[RSK_STARTING_DEKU_SHIELD] = CVar_GetS32("gRandomizeStartingDekuShield", 0);
    cvarSettings[RSK_STARTING_MAPS_COMPASSES] = CVar_GetS32("gRandomizeStartingMapsCompasses", 2);
    cvarSettings[RSK_SHUFFLE_DUNGEON_REWARDS] = CVar_GetS32("gRandomizeShuffleDungeonReward", 0);
    cvarSettings[RSK_SHUFFLE_SONGS] = CVar_GetS32("gRandomizeShuffleSongs", 0);
    cvarSettings[RSK_SHUFFLE_TOKENS] = CVar_GetS32("gRandomizeShuffleTokens", 0);
    cvarSettings[RSK_SHUFFLE_COWS] = CVar_GetS32("gRandomizeShuffleCows", 0);
    cvarSettings[RSK_SHUFFLE_ADULT_TRADE] = CVar_GetS32("gRandomizeShuffleAdultTrade", 0);
    cvarSettings[RSK_SKIP_CHILD_ZELDA] = CVar_GetS32("gRandomizeSkipChildZelda", 0);

    // if we skip child zelda, we start with zelda's letter, and malon starts
    // at the ranch, so we should *not* shuffle the weird egg
    cvarSettings[RSK_SHUFFLE_WEIRD_EGG] = ((CVar_GetS32("gRandomizeSkipChildZelda", 0) == 0) &&
                                            CVar_GetS32("gRandomizeShuffleWeirdEgg", 0));
    
    cvarSettings[RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD] = CVar_GetS32("gRandomizeShuffleGerudoToken", 0);
    cvarSettings[RSK_SHUFFLE_FROG_SONG_RUPEES] = CVar_GetS32("gRandomizeShuffleFrogSongRupees", 0);
    cvarSettings[RSK_ITEM_POOL] = CVar_GetS32("gRandomizeItemPool", 1);
    cvarSettings[RSK_ICE_TRAPS] = CVar_GetS32("gRandomizeIceTraps", 1);
    cvarSettings[RSK_GOSSIP_STONE_HINTS] = CVar_GetS32("gRandomizeGossipStoneHints", 1);
    cvarSettings[RSK_HINT_CLARITY] = CVar_GetS32("gRandomizeHintClarity", 2);
    cvarSettings[RSK_HINT_DISTRIBUTION] = CVar_GetS32("gRandomizeHintDistribution", 1);
    cvarSettings[RSK_KEYSANITY] = CVar_GetS32("gRandomizeKeysanity", 2);
    cvarSettings[RSK_GERUDO_KEYS] = CVar_GetS32("gRandomizeGerudoKeys", 0);
    cvarSettings[RSK_BOSS_KEYSANITY] = CVar_GetS32("gRandomizeBossKeysanity", 2);
    cvarSettings[RSK_GANONS_BOSS_KEY] = CVar_GetS32("gRandomizeShuffleGanonBossKey", 1);
    cvarSettings[RSK_STARTING_CONSUMABLES] = CVar_GetS32("gRandomizeStartingConsumables", 0);
    cvarSettings[RSK_FULL_WALLETS] = CVar_GetS32("gRandomizeFullWallets", 0);
    
    cvarSettings[RSK_EXCLUDE_DEKU_THEATER_MASK_OF_TRUTH] = CVar_GetS32("gRandomizeExcludeDekuTheaterMaskOfTruth", 0);
    cvarSettings[RSK_EXCLUDE_KAK_10_GOLD_SKULLTULA_REWARD] = CVar_GetS32("gRandomizeExcludeKak10SkullReward", 0);
    cvarSettings[RSK_EXCLUDE_KAK_20_GOLD_SKULLTULA_REWARD] = CVar_GetS32("gRandomizeExcludeKak20SkullReward", 0);
    cvarSettings[RSK_EXCLUDE_KAK_30_GOLD_SKULLTULA_REWARD] = CVar_GetS32("gRandomizeExcludeKak30SkullReward", 0);
    cvarSettings[RSK_EXCLUDE_KAK_40_GOLD_SKULLTULA_REWARD] = CVar_GetS32("gRandomizeExcludeKak40SkullReward", 0);
    cvarSettings[RSK_EXCLUDE_KAK_50_GOLD_SKULLTULA_REWARD] = CVar_GetS32("gRandomizeExcludeKak50SkullReward", 0);

    // RANDOTODO implement chest minigame shuffle with keysanity
    cvarSettings[RSK_SHUFFLE_CHEST_MINIGAME] = false;

    cvarSettings[RSK_LANGUAGE] = CVar_GetS32("gLanguages", 0);

    cvarSettings[RSK_CUCCO_COUNT] = CVar_GetS32("gRandomizeCuccosToReturn", 7);
    cvarSettings[RSK_BIG_POE_COUNT] = CVar_GetS32("gRandomizeBigPoeTargetCount", 10);

    // If we skip child zelda, skip child stealth is pointless, so this needs to be reflected in the spoiler log
    cvarSettings[RSK_SKIP_CHILD_STEALTH] =
        !CVar_GetS32("gRandomizeSkipChildZelda", 0) && CVar_GetS32("gRandomizeSkipChildStealth", 0);

    cvarSettings[RSK_SKIP_EPONA_RACE] = CVar_GetS32("gRandomizeSkipEponaRace", 0);
    cvarSettings[RSK_SKIP_TOWER_ESCAPE] = CVar_GetS32("gRandomizeSkipTowerEscape", 0);
    cvarSettings[RSK_COMPLETE_MASK_QUEST] = CVar_GetS32("gRandomizeCompleteMaskQuest", 0);
    cvarSettings[RSK_ENABLE_GLITCH_CUTSCENES] = CVar_GetS32("gRandomizeEnableGlitchCutscenes", 0);

    cvarSettings[RSK_SKULLS_SUNS_SONG] = CVar_GetS32("gRandomizeGsExpectSunsSong", 0);

    RandoMain::GenerateRando(cvarSettings);

    CVar_SetS32("gRandoGenerating", 0);
    CVar_Save();
    CVar_Load();

    generated = 1;
}

void DrawRandoEditor(bool& open) {
    if (generated) {
        generated = 0;
        randoThread.join();
    }

    if (!open) {
        CVar_SetS32("gRandomizerSettingsEnabled", 0);
        return;
    }

// Randomizer settings
    // Logic Settings
    const char* randoLogicRules[2] = { "Glitchless", "No logic"};

    // Open Settings
    const char* randoForest[3] = { "Closed", "Closed Deku", "Open" };
    const char* randoKakarikoGate[2] = { "Closed", "Open" };
    const char* randoDoorOfTime[3] = { "Closed", "Song only", "Open" };
    const char* randoZorasFountain[3] = { "Closed", "Closed as child", "Open" };
    const char* randoGerudoFortress[3] = { "Normal", "Fast", "Open" };
    const char* randoRainbowBridge[7] = { "Vanilla", "Always open", "Stones", "Medallions", "Dungeon rewards", "Dungeons", "Tokens" };
    const char* randoGanonsTrial[2] = { "Off", "On" };

    // World Settings
    const char* randoStartingAge[3] = { "Child", "Adult", "Random" };
    const char* randoShuffleEntrances[2] = { "Off", "On" };
    const char* randoShuffleDungeonsEntrances[2] = { "Off", "On" };
    const char* randoShuffleOverworldEntrances[2] = { "Off", "On" };
    const char* randoShuffleInteriorsEntrances[2] = { "Off", "On" };
    const char* randoShuffleGrottosEntrances[2] = { "Off", "On" };
    const char* randoBombchusInLogic[2] = { "Off", "On" };
    const char* randoAmmoDrops[3] = { "On + Bombchu", "Off", "On" };
    const char* randoHeartDropsAndRefills[4] = { "On", "No Drop", "No Refill", "Off" };

    // Shuffle Settings
    const char* randoShuffleDungeonRewards[4] = { "End of dungeons", "Any dungeon", "Overworld", "Anywhere" };
    const char* randoLinksPocket[4] = { "Dungeon Reward", "Advancement", "Anything", "Nothing" };
    const char* randoShuffleSongs[3] = { "Song Locations", "Dungeon Rewards", "Anywhere" };
    const char* randoShuffleTokens[4] = { "Off", "Dungeons", "Overworld", "All Tokens" };
    const char* randoShopsanity[7] = { "Off", "0", "1", "2", "3", "4", "Random" };
    const char* randoTokensanity[4] = { "Off", "Dungeons", "Overworld", "All Tokens" };
    const char* randoShuffleScrubs[4] = { "Off", "Affordable", "Expensive", "Random Prices" };
    const char* randoShuffleCows[2] = { "Off", "On" };
    const char* randoShuffleKokiriSword[2] = { "Off", "On" };
    const char* randoShuffleOcarinas[2] = { "Off", "On" };
    const char* randoShuffleWeirdEgg[2] = { "Off", "On" };
    const char* randoShuffleGerudoToken[2] = { "Off", "On" };
    const char* randoShuffleMagicBeans[2] = { "Off", "On" };
    const char* randoShuffleMerchants[3] = { "Off", "On (no hints)", "On (with hints)" };
    const char* randoShuffleFrogSongRupees[2] = { "Off", "On" };
    const char* randoShuffleAdultTrade[2] = { "Off", "On" };

    // Shuffle Dungeon Items Settings
    const char* randoShuffleMapsAndCompasses[6] = { "Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere" };
    const char* randoShuffleSmallKeys[6] = { "Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere" };
    const char* randoShuffleGerudoFortressKeys[4] = { "Vanilla", "Any Dungeon", "Overworld", "Anywhere" };
    const char* randoShuffleBossKeys[6] = { "Start With", "Vanilla", "Own Dungeon", "Any Dungeon", "Overworld", "Anywhere" };
    const char* randoShuffleGanonsBossKey[6] = { "Vanilla", "Own dungeon", "Start with", "Any Dungeon", "Overworld", "Anywhere" };

    // Timesaver Settings
    const char* randoSkipSongReplays[3] = { "Don't skip", "Skip (no SFX)", "Skip (Keep SFX)" };

    // Misc Settings
    const char* randoGossipStoneHints[4] = {"No Hints", "Need Nothing", "Mask of Truth", "Stone of Agony"};
    const char* randoHintClarity[3] = { "Obscure", "Ambiguous", "Clear" };
    const char* randoHintDistribution[4] = {"Useless", "Balanced", "Strong", "Very Strong"};
    const char* randoDamageMultiplier[7] = { "x1", "x2", "x4", "x8", "x16", "OHKO", "x1/2" };
    const char* randoStartingTime[2] = { "Day", "Night" };
    const char* randoChestAnimations[2] = { "Always Fast", "Match contents" };
    const char* randoChestSizeAndColor[2] = { "Vanilla", "Match contents" };
    const char* randoOpenInfoMenuWith[6] = { "Select", "Start", "D-Pad Up", "D-Pad Down", "D-Pad Right", "D-Pad Left" };
    const char* randoRandomTrapDamage[3] = { "Basic", "Advanced", "Off" };

    // Item Pool Settings
    const char* randoItemPool[4] = { "Plentiful", "Balanced", "Scarce", "Minimal" };
    const char* randoIceTraps[5] =  {"Off", "Normal", "Extra", "Mayhem", "Onslaught"};

    // SFX Settings
    const char* randoSFXBackgroundMusic[3] = { "Normal", "No Music", "Random" };
    const char* randoSFXFanfares[3] = { "Normal", "No Fanfares", "Random" };
    const char* randoSFXLowHP[29] = { "Default",
                                      "Completely Random",
                                      "Random Ear-Safe",
                                      "None",
                                      "Bark",
                                      "Bomb Bounce",
                                      "Bongo Bongo Low",
                                      "Business Scrub",
                                      "Carrot Refill",
                                      "Cluck",
                                      "Drawbridge Set",
                                      "Guay",
                                      "HP Recover",
                                      "Horse Trot",
                                      "Iront Boots",
                                      "Moo",
                                      "Mweep!",
                                      "Navi \"Hey!\"",
                                      "Navi Random",
                                      "Notification",
                                      "Pot Shattering",
                                      "Ribbit",
                                      "Rupee (Silver)",
                                      "Soft Beep",
                                      "Switch",
                                      "Sword Bonk",
                                      "Tambourine",
                                      "Timer",
                                      "Zelda Gasp (Adult)" };
    const char* randoSFXHorse[13] = {
        "Default",       "Completely Random", "Random Ear-Safe", "Random Choice", "None",
        "Armos",         "Child Scream",      "Great Fairy",     "Moo",           "Mweep!",
        "Redead Scream", "Ruto Wiggle",       "Stalchild Attack"
    };
    const char* randoSFXNightfall[13] = {
        "Default",        "Completely Random",    "Random Ear-Safe", "Random Choice", "None",
        "Cockadoodiedoo", "Gold Skulltula Token", "Great Fairy",     "Moo",           "Mweep!",
        "Redead Moan",    "Talon Snore",          "Thunder"
    };
    const char* randoSFXHoverBoots[11] = {
        "Default",      "Completely Random",  "Random Ear-Safe", "Random Choice", "None",      "Bark",
        "Cartoon Fall", "Flare Dancer Laugh", "Mweep!",          "Shabom Pop",    "Tambourine"
    };
    const char* randoSFXOcarina[7] = { "Dafault", "Random Choice", "Flute", "Harp", "Whistle", "Malon", "Grind Organ" };
    const char* randoSFXMenu[65] = { "Default",
                                     "Completely Random",
                                     "Random Ear-Safe",
                                     "Random Choise",
                                     "None",
                                     "Bark",
                                     "Bomb Bounce",
                                     "Bongo Bongo High",
                                     "Bongo Bongo Low",
                                     "Bottle Cork",
                                     "Bow Twang",
                                     "Bubble Laugh",
                                     "Carrot Refill",
                                     "Change Item",
                                     "Child Pant",
                                     "Cluck",
                                     "Deku Baba",
                                     "Drawbridge Set",
                                     "Dusk Howl",
                                     "Fanfare (Light)",
                                     "Fanfare (Mediaum)",
                                     "Field Shrub",
                                     "Flare Dancer Startled",
                                     "Ganondorf \"Teh!\"",
                                     "Gohma Larva Croak",
                                     "Gold Skulltula Token",
                                     "Goron Wake",
                                     "Guay",
                                     "Gunshot",
                                     "HP Low",
                                     "HP Recover",
                                     "Hammer Bonk",
                                     "Horse Trot",
                                     "Iron Boots",
                                     "Iron Knuckle",
                                     "Moo",
                                     "Mweep!",
                                     "Notification",
                                     "Phantom Ganon Laugh",
                                     "Plant Explode",
                                     "Pot Shattering",
                                     "Redead Moan",
                                     "Ribbit",
                                     "Rupee",
                                     "Rupee (Silver)",
                                     "Ruto Crash",
                                     "Ruto Lift",
                                     "Ruto Thrown",
                                     "Scrub Emerge",
                                     "Shabom Bounce",
                                     "Shabom Pop",
                                     "Shellblade",
                                     "Skulltula",
                                     "Soft Beep",
                                     "Spit Nut",
                                     "Switch",
                                     "Sword Bonk",
                                     "Talon \"Hmmm\"",
                                     "Talon Snore",
                                     "Talon Surprised",
                                     "Tambourine",
                                     "Target Ennemy",
                                     "Target Neutral",
                                     "Timer",
                                     "Zelda Gasp (Adult)" };
    const char* randoSFXNavi[32] = { "Default",
                                     "Completely Random",
                                     "Random Ear-Safe",
                                     "Random  Choice",
                                     "None",
                                     "Bark",
                                     "Business Scrub",
                                     "Carrot Refill",
                                     "Click",
                                     "Dusk Howl",
                                     "Exploding Crate",
                                     "Explosion",
                                     "Great Fairy",
                                     "Guay",
                                     "HP Low",
                                     "HP Recover",
                                     "Horse Neigh",
                                     "Ice Shattering",
                                     "Moo",
                                     "Mweep!",
                                     "Navi \"Hello!\"",
                                     "Notification",
                                     "Poe",
                                     "Pot Shattering",
                                     "Redead Scream",
                                     "Ribit",
                                     "Ruto Giggle",
                                     "Skulltula",
                                     "Soft Beep",
                                     "Tambourine",
                                     "Timer",
                                     "Zelda Gasp (Adult)" };

        ImGui::SetNextWindowSize(ImVec2(830, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Randomizer Editor", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
            ImGui::End();
            return;
        }

        bool disableEditingRandoSettings = CVar_GetS32("gRandoGenerating", 0) || CVar_GetS32("gOnFileSelectNameEntry", 0);
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, disableEditingRandoSettings);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha,
                            ImGui::GetStyle().Alpha * (disableEditingRandoSettings ? 0.5f : 1.0f));
        SohImGui::EnhancementCheckbox("Enable Randomizer", "gRandomizer");

        if (CVar_GetS32("gRandomizer", 0) == 1) {
            ImGui::Dummy(ImVec2(0.0f, 0.0f));
            if (ImGui::Button("Generate Seed")) {
                if (CVar_GetS32("gRandoGenerating", 0) == 0) {
                    randoThread = std::thread(&GenerateRandomizerImgui);
                }
            }
            ImGui::Dummy(ImVec2(0.0f, 0.0f));
            std::string spoilerfilepath = CVar_GetString("gSpoilerLog", "");
            ImGui::Text("Spoiler File: %s", spoilerfilepath.c_str());

            // RANDOTODO settings presets
            // std::string presetfilepath = CVar_GetString("gLoadedPreset", "");
            // ImGui::Text("Settings File: %s", presetfilepath.c_str());
        }
        PaddedSeparator();

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        static ImVec2 cellPadding(8.0f, 8.0f);

        if (CVar_GetS32("gRandomizer", 0) == 1 && ImGui::BeginTabBar("Randomizer Settings", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
            if (ImGui::BeginTabItem("Main Rules")) {
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
                if (ImGui::BeginTable("tableRandoMainRules", 3, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                    ImGui::TableSetupColumn("Open Settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                    ImGui::TableSetupColumn("Shuffle Settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                    ImGui::TableSetupColumn("Shuffle Dungeon Items", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::TableHeadersRow();
                    ImGui::PopItemFlag();
                    ImGui::TableNextRow();

                    // COLUMN 1 - OPEN SETTINGS
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;
                    ImGui::PushItemWidth(-FLT_MIN);

                    if (CVar_GetS32("gRandomizeAllOpenSettings", 0) != 1) {
                        // Forest
                        ImGui::Text(Settings::OpenForest.GetName().c_str());
                        InsertHelpHoverText(
                            "Closed - Kokiri sword & shield are required to access "
                            "the Deku Tree, and completing the Deku Tree is required to "
                            "access the Hyrule Field exit.\n"
                            "\n"
                            "Closed Deku - Kokiri boy no longer blocks the path to Hyrule "
                            "Field but Mido still requires the Kokiri sword and Deku shield "
                            "to access the tree.\n"
                            "\n"
                            "Open - Mido no longer blocks the path to the Deku Tree. Kokiri "
                            "boy no longer blocks the path out of the forest.");
                        SohImGui::EnhancementCombobox("gRandomizeForest", randoForest, 3, 0);
                        PaddedSeparator();
                        // Kakariko Gate
                        ImGui::Text(Settings::OpenKakariko.GetName().c_str());
                        InsertHelpHoverText(
                            "Closed - The gate will remain closed until Zelda's letter "
                            "is shown to the guard.\n"
                            "\n"
                            "Open - The gate is always open. The happy mask shop "
                            "will open immediately after obtaining Zelda's letter."
                        );
                        SohImGui::EnhancementCombobox("gRandomizeKakarikoGate", randoKakarikoGate, 2, 0);
                        PaddedSeparator();

                        // Door of Time
                        ImGui::Text(Settings::OpenDoorOfTime.GetName().c_str());
                        InsertHelpHoverText(
                            "Closed - The Ocarina of Time, the Song of Time and all "
                            "three spiritual stones are required to open the Door of Time.\n"
                            "\n"
                            "Song only - Play the Song of Time in front of the Door of "
                            "Time to open it.\n"
                            "\n"
                            "Open - The Door of Time is permanently open with no requirements."
                        );
                        SohImGui::EnhancementCombobox("gRandomizeDoorOfTime", randoDoorOfTime, 3, 0);
                        PaddedSeparator();

                        // Zora's Fountain
                        ImGui::Text(Settings::ZorasFountain.GetName().c_str());
                        InsertHelpHoverText(
                            "Closed - King Zora obstructs the way to Zora's Fountain. "
                            "Ruto's letter must be shown as child Link in order to move "
                            "him in both time periods.\n"
                            "\n"
                            "Closed as child - Ruto's Letter is only required to move King Zora "
                            "as child Link. Zora's Fountain starts open as adult.\n"
                            "\n" 
                            "Open - King Zora has already mweeped out of the way in both "
                            "time periods. Ruto's Letter is removed from the item pool."
                        );
                        SohImGui::EnhancementCombobox("gRandomizeZorasFountain", randoZorasFountain, 3, 0);
                        PaddedSeparator();

                        // Gerudo Fortress
                        ImGui::Text(Settings::GerudoFortress.GetName().c_str());
                        InsertHelpHoverText(
                            "Sets the amount of carpenters required to repair the bridge "
                            "in Gerudo Valley.\n"
                            "\n"
                            "Normal - All 4 carpenters are required to be saved.\n"
                            "\n"
                            "Fast - Only the bottom left carpenter requires rescuing.\n"
                            "\n"
                            "Open - The bridge is repaired from the start."
                        );
                        SohImGui::EnhancementCombobox("gRandomizeGerudoFortress", randoGerudoFortress, 3, 0);
                        PaddedSeparator();

                        // Rainbow Bridge
                        ImGui::Text(Settings::Bridge.GetName().c_str());
                        InsertHelpHoverText(
                            "Alters the requirements to open the bridge to Ganon's Castle.\n"
                            "\n"
                            "Vanilla - Obtain the Shadow Medallion, Spirit Medallion and Light Arrows.\n"
                            "\n"
                            "Always open - No requirements.\n"
                            "\n"
                            "Stones - Obtain the specified amount of spiritual stones.\n"
                            "\n"
                            "Medallions - Obtain the specified amount of medallions.\n"
                            "\n"
                            "Dungeon rewards - Obtain the specified total sum of spiritual "
                            "stones or medallions.\n"
                            "\n"
                            "Dungeons - Complete the specified amount of dungeons. Dungeons "
                            "are considered complete after stepping in to the blue warp after "
                            "the boss.\n"
                            "\n"
                            "Tokens - Obtain the specified amount of Skulltula tokens."
                        );
                        SohImGui::EnhancementCombobox("gRandomizeRainbowBridge", randoRainbowBridge, 7, 0);
                        ImGui::PopItemWidth();
                        switch (CVar_GetS32("gRandomizeRainbowBridge", 0)) {
                            case 0:
                                break;
                            case 1:
                                break;
                            case 2:
                                ImGui::Dummy(ImVec2(0.0f, 0.0f));
                                SohImGui::EnhancementSliderInt("Stone Count: %d", "##RandoStoneCount",
                                                               "gRandomizeStoneCount", 1, 3, "", 3, true);
                                break;
                            case 3:
                                ImGui::Dummy(ImVec2(0.0f, 0.0f));
                                SohImGui::EnhancementSliderInt("Medallion Count: %d", "##RandoMedallionCount",
                                                               "gRandomizeMedallionCount", 1, 6, "", 6, true);
                                break;
                            case 4:
                                ImGui::Dummy(ImVec2(0.0f, 0.0f));
                                SohImGui::EnhancementSliderInt("Reward Count: %d", "##RandoRewardCount",
                                                               "gRandomizeRewardCount", 1, 9, "", 9, true);
                                break;
                            case 5:
                                ImGui::Dummy(ImVec2(0.0f, 0.0f));
                                SohImGui::EnhancementSliderInt("Dungeon Count: %d", "##RandoDungeonCount",
                                                               "gRandomizeDungeonCount", 1, 8, "", 8, true);
                                break;
                            case 6:
                                ImGui::Dummy(ImVec2(0.0f, 0.0f));
                                SohImGui::EnhancementSliderInt("Token Count: %d", "##RandoTokenCount",
                                                               "gRandomizeTokenCount", 1, 100, "", 100, true);
                                break;
                        }
                        PaddedSeparator();

                        // Random Ganon's Trials
                        /*
                        ImGui::Text("Random Ganon's Trials");
                        InsertHelpHoverText("Sets a random number or required trials to enter\nGanon's Tower.");
                        SohImGui::EnhancementCombobox("gRandomizeGanonTrial", randoGanonsTrial, 2, 0);
                        if (CVar_GetS32("gRandomizeGanonTrial", 0) == 0) {
                            ImGui::PopItemWidth();
                            SohImGui::EnhancementSliderInt("Ganon's Trial Count: %d", "##RandoTrialCount",
                                                           "gRandomizeGanonTrialCount", 0, 6, "", 6);
                            InsertHelpHoverText("Set the number of trials required to enter Ganon's Tower.");
                        RANDTODO: Switch back to slider when pre-completing some of Ganon's Trials is properly implemnted.
                        }
                        */
                        SohImGui::EnhancementCheckbox("Skip Ganon's Trials", "gRandomizeGanonTrialCount");
                        InsertHelpHoverText(
                            "Sets whether or not Ganon's Castle Trials are required to enter Ganon's Tower.");
                    }
                    
                    // COLUMN 2 - Shuffle Settings
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;
                    ImGui::PushItemWidth(-FLT_MIN);
                    if (CVar_GetS32("gRandomizeAllShuffleSettings", 0) != 1) {

                        // Shuffle Songs
                        ImGui::Text(Settings::ShuffleSongs.GetName().c_str());
                        InsertHelpHoverText(
                            "Song locations - Songs will only appear at locations that normally teach songs.\n"
                            "\n"
                            "Dungeon rewards - Songs appear after beating a major dungeon boss.\n"
                            "The 4 remaining songs are located at:\n"
                            "  - Zelda's lullaby location\n"
                            "  - Ice Cavern's Serenade of Water location\n"
                            "  - Bottom of the Well Lens of Truth location\n"
                            "  - Gerudo Training Ground's Ice Arrows location\n"
                            "\n"
                            "Anywhere - Songs can appear at any location."
                        );

                        SohImGui::EnhancementCombobox("gRandomizeShuffleSongs", randoShuffleSongs, 3, 0);
                        PaddedSeparator();

                        // Shuffle Tokens
                        ImGui::Text(Settings::Tokensanity.GetName().c_str());
                        InsertHelpHoverText(
                            "Shuffles Golden Skulltula Tokens into the item pool. This means "
                            "Golden Skulltulas can contain other items as well.\n"
                            "\n"
                            "Off - GS tokens will not be shuffled.\n"
                            "\n"
                            "Dungeons - Only shuffle GS tokens that are within dungeons.\n"
                            "\n"
                            "Overworld - Only shuffle GS tokens that are outside of dungeons.\n"
                            "\n"
                            "All Tokens - Shuffle all 100 GS tokens."
                        );
                        SohImGui::EnhancementCombobox("gRandomizeShuffleTokens", randoShuffleTokens, 4, 0);
                        PaddedSeparator();

                        SohImGui::EnhancementCheckbox("Nighttime GS expect Sun's Song", "gRandomizeGsExpectSunsSong");
                        InsertHelpHoverText("All Golden Skulltulas that require nighttime to appear will only be "
                                            "expected to be collected after getting Sun's Song.");
                        PaddedSeparator();

                        // Shuffle Cows
                        SohImGui::EnhancementCheckbox(Settings::ShuffleCows.GetName().c_str(), "gRandomizeShuffleCows");
                        InsertHelpHoverText("Cows give a randomized item from the pool upon performing Epona's Song in front of them.");
                        PaddedSeparator();

                        // Shuffle Adult Trade Quest
                        SohImGui::EnhancementCheckbox(Settings::ShuffleAdultTradeQuest.GetName().c_str(), "gRandomizeShuffleAdultTrade");
                        InsertHelpHoverText(
                            "Adds all of the adult trade quest items into the pool, each of which "
                            "can be traded for a unique reward.\n"
                            "\n"
                            "You will be able to choose which of your owned adult trade items is visible "
                            "in the inventory by selecting the item with A and using the control stick or "
                            "D-pad.\n"
                            "\n"
                            "If disabled, only the Claim Check will be found in the pool."
                        );
                        PaddedSeparator();

                        if(CVar_GetS32("gRandomizeStartingKokiriSword", 0) == 0) {
                            // Shuffle Kokiri Sword
                            SohImGui::EnhancementCheckbox(Settings::ShuffleKokiriSword.GetName().c_str(), "gRandomizeShuffleKokiriSword");
                            InsertHelpHoverText(
                                "Shuffles the Kokiri Sword into the item pool.\n"
                                "\n"
                                "This will require the use of sticks until the Kokiri Sword is found."
                            );
                            PaddedSeparator();
                        }

                        if(CVar_GetS32("gRandomizeStartingOcarina", 0) == 0) {
                            // Shuffle Ocarinas
                            SohImGui::EnhancementCheckbox(Settings::ShuffleOcarinas.GetName().c_str(), "gRandomizeShuffleOcarinas");
                            InsertHelpHoverText(
                                "Enabling this shuffles the Fairy Ocarina and the Ocarina of Time into the item pool.\n"
                                "\n"
                                "This will require finding an Ocarina before being able to play songs."
                            );
                            PaddedSeparator();
                        }

                        // Shuffle Weird Egg
                        // Disabled when Skip Child Zelda is active
                        if (!disableEditingRandoSettings) {
                            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, CVar_GetS32("gRandomizeSkipChildZelda", 0));
                            ImGui::PushStyleVar(ImGuiStyleVar_Alpha,
                                                ImGui::GetStyle().Alpha *
                                                    (CVar_GetS32("gRandomizeSkipChildZelda", 0) ? 0.5f : 1.0f));
                        }
                        SohImGui::EnhancementCheckbox(Settings::ShuffleWeirdEgg.GetName().c_str(), "gRandomizeShuffleWeirdEgg");
                        if (!disableEditingRandoSettings) {
                            ImGui::PopStyleVar();
                            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) &&
                                CVar_GetS32("gRandomizeSkipChildZelda", 0)) {
                                ImGui::SetTooltip("%s",
                                                  "This option is disabled because \"Skip Child Zelda\" is enabled");
                            }
                            ImGui::PopItemFlag();
                        }
                        InsertHelpHoverText(
                            "Shuffles the Weird Egg from Malon in to the item pool. Enabling "
                            "\"Skip Child Zelda\" disables this feature.\n"
                            "\n"
                            "The Weird Egg is required to unlock several events:\n"
                            "  - Zelda's Lullaby from Impa\n"
                            "  - Saria's song in Sacred Forest Meadow\n"
                            "  - Epona's song and chicken minigame at Lon Lon Ranch\n"
                            "  - Zelda's letter for Kakariko gate (if set to closed)\n"
                            "  - Happy Mask Shop sidequest\n"
                        );
                        PaddedSeparator();

                        // Shuffle Gerudo Membership Card
                        SohImGui::EnhancementCheckbox(Settings::ShuffleGerudoToken.GetName().c_str(), "gRandomizeShuffleGerudoToken");
                        InsertHelpHoverText(
                            "Shuffles the Gerudo Membership Card into the item pool.\n"
                            "\n"
                            "The Gerudo Card is required to enter the Gerudo Training Grounds, opening "
                            "the gate to Haunted Wasteland and the Horseback Archery minigame."
                        );
                        PaddedSeparator();

                        // Shuffle Frog Song Rupees
                        SohImGui::EnhancementCheckbox(Settings::ShuffleFrogSongRupees.GetName().c_str(), "gRandomizeShuffleFrogSongRupees");
                        InsertHelpHoverText(
                            "Shuffles 5 Purple Rupees into to the item pool, and allows\n"
                            "you to earn items by playing songs at the Frog Choir.\n"
                            "\n"
                            "This setting does not effect the item earned from playing\n"
                            "the Song of Storms and the frog song minigame."
                        );
                    }
                    ImGui::PopItemWidth();

                    // COLUMN 3 - Shuffle Dungeon Items
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;
                    ImGui::PushItemWidth(-FLT_MIN);

                    // Shuffle Dungeon Rewards
                    ImGui::Text(Settings::ShuffleRewards.GetName().c_str());
                    InsertHelpHoverText(
                        "Shuffles the location of spiritual stones and medallions.\n"
                        "\n"
                        "End of dungeons - Spiritual stones and medallions will be given as rewards "
                        "for beating major dungeons. Link will always start with one stone or medallion.\n"
                        "\n"
                        "Any dungeon - Spiritual stones and medallions can be found inside any dungeon.\n"
                        "\n"
                        "Overworld - Spiritual stones and medallions can only be found outside of dungeons.\n"
                        "\n"
                        "Anywhere - Spiritual stones and medallions can appear anywhere."
                    );
                    SohImGui::EnhancementCombobox("gRandomizeShuffleDungeonReward", randoShuffleDungeonRewards, 4, 0);
                    PaddedSeparator();

                    // Maps & Compasses
                    ImGui::Text(Settings::MapsAndCompasses.GetName().c_str());
                    InsertHelpHoverText(
                        "Start with - You will start with Maps & Compasses from all dungeons.\n"
                            "\n"
                            "Vanilla - Maps & Compasses will appear in their vanilla locations.\n"
                            "\n"
                            "Own dungeon - Maps & Compasses can only appear in their respective dungeon.\n"
                            "\n"
                            "Any dungeon - Maps & Compasses can only appear inside of any dungon.\n"
                            "\n"
                            "Overworld - Maps & Compasses can only appear outside of dungeons.\n"
                            "\n"
                            "Anywhere - Maps & Compasses can appear anywhere in the world."
                    );
                    SohImGui::EnhancementCombobox("gRandomizeStartingMapsCompasses", randoShuffleMapsAndCompasses, 6, 2);
                    PaddedSeparator();

                    // Keysanity
                    ImGui::Text(Settings::Keysanity.GetName().c_str());
                    InsertHelpHoverText(
                        "Start with - You will start with all Small Keys from all dungeons.\n"
                        "\n"
                        "Vanilla - Small Keys will appear in their vanilla locations.\n"
                        "\n"
                        "Own dungeon - Small Keys can only appear in their respective dungeon.\n"
                        "\n"
                        "Any dungeon - Small Keys can only appear inside of any dungon.\n"
                        "\n"
                        "Overworld - Small Keys can only appear outside of dungeons.\n"
                        "\n"
                        "Anywhere - Small Keys can appear anywhere in the world."
                    );
                    SohImGui::EnhancementCombobox("gRandomizeKeysanity", randoShuffleSmallKeys, 6, 2);
                    PaddedSeparator();

                    // Gerudo Keys
                    ImGui::Text(Settings::GerudoKeys.GetName().c_str());
                    InsertHelpHoverText(
                        "Vanilla - Thieve's Hideout Keys will appear in their vanilla locations.\n"
                        "\n"
                        "Any dungeon - Thieve's Hideout Keys can only appear inside of any dungon.\n"
                        "\n"
                        "Overworld - Thieve's Hideout Keys can only appear outside of dungeons.\n"
                        "\n"
                        "Anywhere - Thieve's Hideout Keys can appear anywhere in the world."
                    );
                    SohImGui::EnhancementCombobox("gRandomizeGerudoKeys", randoShuffleGerudoFortressKeys, 4, 0);
                    PaddedSeparator();

                    // Boss Keysanity
                    ImGui::Text(Settings::BossKeysanity.GetName().c_str());
                    InsertHelpHoverText(
                        "Start with - You will start with Boss keys from all dungeons.\n"
                        "\n"
                        "Vanilla - Boss Keys will appear in their vanilla locations.\n"
                        "\n"
                        "Own dungeon - Boss Keys can only appear in their respective dungeon.\n"
                        "\n"
                        "Any dungeon - Boss Keys can only appear inside of any dungon.\n"
                        "\n"
                        "Overworld - Boss Keys can only appear outside of dungeons.\n"
                        "\n"
                        "Anywhere - Boss Keys can appear anywhere in the world."
                    );  
                    SohImGui::EnhancementCombobox("gRandomizeBossKeysanity", randoShuffleBossKeys, 6, 2);
                    PaddedSeparator();

                    // Ganon's Boss Key
                    ImGui::Text(Settings::GanonsBossKey.GetName().c_str());
                    InsertHelpHoverText(
                        "Vanilla - Ganon's Boss Key will appear in the vanilla location.\n"
                        "\n"
                        "Own dungeon - Ganon's Boss Key can appear anywhere inside Ganon's Castle.\n"
                        "\n"
                        "Start with - Places Ganon's Boss Key in your starting inventory."
                        "\n"
                        "Any dungeon - Ganon's Boss Key Key can only appear inside of any dungon.\n"
                        "\n"
                        "Overworld - Ganon's Boss Key Key can only appear outside of dungeons.\n"
                        "\n"
                        "Anywhere - Ganon's Boss Key Key can appear anywhere in the world."
                    );
                    SohImGui::EnhancementCombobox("gRandomizeShuffleGanonBossKey", randoShuffleGanonsBossKey, 6, 1);

                    ImGui::PopItemWidth();
                    ImGui::EndTable();
                }
                ImGui::PopStyleVar(1);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Other")) {
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
                if (ImGui::BeginTable("tableRandoOther", 3, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                    ImGui::TableSetupColumn("Timesavers", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                    ImGui::TableSetupColumn("Hint Settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                    ImGui::TableSetupColumn("Item Pool Settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::TableHeadersRow();
                    ImGui::PopItemFlag();
                    ImGui::TableNextRow();
                    
                    // COLUMN 1 - TIME SAVERS
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;

                    // Cuccos to return
                    SohImGui::EnhancementSliderInt("Cuccos to return: %d", "##RandoCuccosToReturn",
                                                    "gRandomizeCuccosToReturn", 0, 7, "", 7, true);
                    InsertHelpHoverText(
                        "The amount of cuccos needed to claim the reward from Anju the cucco lady"
                    );
                    PaddedSeparator();

                    // Big Poe Target Count
                    SohImGui::EnhancementSliderInt("Big Poe Target Count: %d", "##RandoBigPoeTargetCount",
                                                    "gRandomizeBigPoeTargetCount", 1, 10, "", 10, true);
                    InsertHelpHoverText(
                        "The Poe collector will give a reward for turning in this many Big Poes."
                    );
                    PaddedSeparator();

                    // Skip child stealth
                    // Disabled when Skip Child Zelda is active
                    if (!disableEditingRandoSettings) {
                        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, CVar_GetS32("gRandomizeSkipChildZelda", 0));
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha,
                                            ImGui::GetStyle().Alpha *
                                                (CVar_GetS32("gRandomizeSkipChildZelda", 0) ? 0.5f : 1.0f));
                    }
                    SohImGui::EnhancementCheckbox(Settings::SkipChildStealth.GetName().c_str(),
                                                  "gRandomizeSkipChildStealth");
                    if (!disableEditingRandoSettings) {
                        ImGui::PopStyleVar();
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) &&
                            CVar_GetS32("gRandomizeSkipChildZelda", 0)) {
                            ImGui::SetTooltip("%s", "This option is disabled because \"Skip Child Zelda\" is enabled");
                        }
                        ImGui::PopItemFlag();
                    }
                    InsertHelpHoverText("The crawlspace into Hyrule Castle goes straight to Zelda, skipping the guards.");
                    PaddedSeparator();

                    // Skip child zelda
                    SohImGui::EnhancementCheckbox("Skip Child Zelda", "gRandomizeSkipChildZelda");
                    InsertHelpHoverText(
                        "Start with Zelda's Letter in your inventory and skip the sequence up "
                        "until after meeting Zelda. Disables the ability to shuffle Weird Egg."
                    );
                    PaddedSeparator();

                    // Skip Epona race
                    SohImGui::EnhancementCheckbox(Settings::SkipEponaRace.GetName().c_str(), "gRandomizeSkipEponaRace");
                    InsertHelpHoverText(
                        "Epona can be summoned with Epona's Song without needing to race Ingo."
                    );
                    PaddedSeparator();

                    // Skip tower escape
                    SohImGui::EnhancementCheckbox(Settings::SkipTowerEscape.GetName().c_str(), "gRandomizeSkipTowerEscape");
                    InsertHelpHoverText(
                        "The tower escape sequence between Ganondorf and Ganon will be skipped."
                    );
                    PaddedSeparator();

                    // Complete mask quest
                    SohImGui::EnhancementCheckbox(Settings::CompleteMaskQuest.GetName().c_str(), "gRandomizeCompleteMaskQuest");
                    InsertHelpHoverText(
                        "Once the happy mask shop is opened, all masks will be available to be borrowed."
                    );
                    PaddedSeparator();

                    // Enable Glitch-Useful Cutscenes
                    SohImGui::EnhancementCheckbox(Settings::EnableGlitchCutscenes.GetName().c_str(), "gRandomizeEnableGlitchCutscenes");
                    InsertHelpHoverText(
                        "The cutscenes of the Poes in Forest Temple and Darunia in Fire Temple will not be skipped. "
                        "These cutscenes are only useful for glitched gameplay and can be safely skipped otherwise."
                    );

                    // COLUMN 2 - HINT SETTINGS
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;
                    ImGui::PushItemWidth(-FLT_MIN);

                    // Gossip Stone Hints
                    ImGui::Text(Settings::GossipStoneHints.GetName().c_str());
                    InsertHelpHoverText(
                        "Allows Gossip Stones to provide hints on item locations. Hints mentioning "
                        "\"Way of the Hero\" indicate a location that holds an item required to beat "
                        "the seed.\n"
                        "\n"
                        "No hints - No hints will be given at all.\n"
                        "\n"
                        "Need Nothing - Hints are always available from Gossip Stones.\n"
                        "\n"
                        "Need Stone of Agony - Hints are only available after obtaining the Stone of Agony.\n"
                        "\n"
                        "Need Mask of Truth - Hints are only available whilst wearing the Mask of Truth.\n"
                    );

                    SohImGui::EnhancementCombobox("gRandomizeGossipStoneHints", randoGossipStoneHints, 4, 1);
                    if (CVar_GetS32("gRandomizeGossipStoneHints", 1) != 0) {
                        // Hint Clarity
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        ImGui::Indent();
                        ImGui::Text(Settings::ClearerHints.GetName().c_str());
                        InsertHelpHoverText(
                            "Sets the difficulty of hints.\n"
                            "\n"
                            "Obscure - Hints are unique for each item, but the writing may be cryptic.\n"
                            "Ex: Kokiri Sword > a butter knife\n"
                            "\n"
                            "Ambiguous - Hints are clearly written, but may refer to more than one item.\n"
                            "Ex: Kokiri Sword > a sword\n"
                            "\n"
                            "Clear - Hints are clearly written and are unique for each item.\n"
                            "Ex: Kokiri Sword > the Kokiri Sword"
                        );
                        SohImGui::EnhancementCombobox("gRandomizeHintClarity", randoHintClarity, 3, 2);

                        // Hint Distribution
                        ImGui::Dummy(ImVec2(0.0f, 0.0f));
                        ImGui::Text(Settings::HintDistribution.GetName().c_str());
                        InsertHelpHoverText(
                            "Sets how many hints will be useful.\n"
                            "\n"
                            "Useless - Only junk hints.\n"
                            "\n"
                            "Balanced - Recommended hint spread.\n"
                            "\n"
                            "Strong - More useful hints.\n"
                            "\n"
                            "Very Strong - Many powerful hints."
                        );
                        SohImGui::EnhancementCombobox("gRandomizeHintDistribution", randoHintDistribution, 4, 1);
                        ImGui::Unindent();
                    }
                    ImGui::PopItemWidth();

                    // COLUMN 3 - ITEM POOL SETTINGS
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;
                    ImGui::PushItemWidth(-FLT_MIN);
                    ImGui::Text(Settings::ItemPoolValue.GetName().c_str());
                    InsertHelpHoverText(
                        "Sets how many major items appear in the item pool.\n"
                        "\n"
                        "Plentiful - Extra major items are added to the pool.\n"
                        "\n"
                        "Balanced - Original item pool.\n"
                        "\n"
                        "Scarce - Some excess items are removed, including health upgrades.\n"
                        "\n"
                        "Minimal - Most excess items are removed."
                    );
                    SohImGui::EnhancementCombobox("gRandomizeItemPool", randoItemPool, 4, 1);
                    PaddedSeparator();

                    // Ice Traps
                    ImGui::Text(Settings::IceTrapValue.GetName().c_str());
                    InsertHelpHoverText(
                        "Sets how many items are replaced by ice traps.\n"
                        "\n"
                        "Off - No ice traps.\n"
                        "\n"
                        "Normal - Only Ice Traps from the base item pool are shuffled in.\n"
                        "\n"
                        "Extra - Chance to replace added junk items with additional ice traps.\n"
                        "\n"
                        "Mayhem - All added junk items will be Ice Traps.\n"
                        "\n"
                        "Onslaught - All junk items will be replaced by Ice Traps, even those "
                        "in the base pool."
                    );
                    SohImGui::EnhancementCombobox("gRandomizeIceTraps", randoIceTraps, 5, 1);
                    ImGui::PopItemWidth();
                    ImGui::EndTable();
                }
                ImGui::PopStyleVar(1);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Locations")) {
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
                if (ImGui::BeginTable("tableRandoLocations", 2,
                                      ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                    ImGui::TableSetupColumn("Exclude Locations", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                    ImGui::TableSetupColumn(" ", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::TableHeadersRow();
                    ImGui::PopItemFlag();
                    ImGui::TableNextRow();

                    // COLUMN 1 - EXCLUDE LOCATIONS
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;
                    SohImGui::EnhancementCheckbox("Deku Theater Mask of Truth", "gRandomizeExcludeDekuTheaterMaskOfTruth");
                    PaddedSeparator();
                    SohImGui::EnhancementCheckbox("10 Skulltula Reward", "gRandomizeExcludeKak10SkullReward");
                    PaddedSeparator();
                    SohImGui::EnhancementCheckbox("20 Skulltula Reward", "gRandomizeExcludeKak20SkullReward");
                    PaddedSeparator();
                    SohImGui::EnhancementCheckbox("30 Skulltula Reward", "gRandomizeExcludeKak30SkullReward");
                    PaddedSeparator();
                    SohImGui::EnhancementCheckbox("40 Skulltula Reward", "gRandomizeExcludeKak40SkullReward");
                    PaddedSeparator();
                    SohImGui::EnhancementCheckbox("50 Skulltula Reward", "gRandomizeExcludeKak50SkullReward");

                    // COLUMN 2 - EXCLUDE LOCATIONS
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;
                    

                    ImGui::EndTable();
                }
                ImGui::PopStyleVar(1);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Tricks/Glitches")) {
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
                if (ImGui::BeginTable("tableRandoLogic", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::PushItemWidth(170.0);
                    ImGui::Text("Logic Rules");
                    InsertHelpHoverText("Glitchless - No glitches are required, but may require some minor tricks.\n"
                                        "\n"
                                        "No logic - Item placement is completely random. MAY BE IMPOSSIBLE TO BEAT.");
                    SohImGui::EnhancementCombobox("gRandomizeLogicRules", randoLogicRules, 2, 0);
                    ImGui::PopItemWidth();
                    ImGui::EndTable();
                }
                if (ImGui::BeginTable("tableRandoTricksGlitches", 2,
                                      ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                    ImGui::TableSetupColumn("Enable Tricks", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                    ImGui::TableSetupColumn("Enable Glitches", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::TableHeadersRow();
                    ImGui::PopItemFlag();
                    ImGui::TableNextRow();

                    // COLUMN 1 - ENABLE TRICKS
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;
                    ImGui::Text("Coming soon");

                    // COLUMN 2 - ENABLE GLITCHES
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;
                    ImGui::Text("Coming soon");

                    ImGui::EndTable();
                }
                ImGui::PopStyleVar(1);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Starting Inventory")) {
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
                if (ImGui::BeginTable("tableRandoStartingInventory", 3,
                                      ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                    ImGui::TableSetupColumn("Starting Equipment", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                    ImGui::TableSetupColumn("Starting Items", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                    ImGui::TableSetupColumn("Starting Songs", ImGuiTableColumnFlags_WidthStretch, 200.0f);
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::TableHeadersRow();
                    ImGui::PopItemFlag();
                    ImGui::TableNextRow();

                    // COLUMN 1 - STARTING EQUIPMENT
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;
                    SohImGui::EnhancementCheckbox(Settings::StartingKokiriSword.GetName().c_str(), "gRandomizeStartingKokiriSword");
                    PaddedSeparator();
                    SohImGui::EnhancementCheckbox(Settings::StartingDekuShield.GetName().c_str(), "gRandomizeStartingDekuShield");

                    // COLUMN 2 - STARTING ITEMS
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;
                    SohImGui::EnhancementCheckbox(Settings::StartingOcarina.GetName().c_str(), "gRandomizeStartingOcarina");
                    PaddedSeparator();
                    SohImGui::EnhancementCheckbox(Settings::StartingConsumables.GetName().c_str(), "gRandomizeStartingConsumables");
                    PaddedSeparator();
                    SohImGui::EnhancementCheckbox("Full Wallets", "gRandomizeFullWallets");
                    InsertHelpHoverText("Start with a full wallet. All wallet upgrades come filled with rupees.");

                    // COLUMN 3 - STARTING SONGS
                    ImGui::TableNextColumn();
                    window->DC.CurrLineTextBaseOffset = 0.0f;
                    ImGui::Text("Coming soon");

                    ImGui::EndTable();
                }
                ImGui::PopStyleVar(1);
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
        ImGui::End();
    }

void CreateGetItemMessages(std::vector<GetItemMessage> messageEntries) {
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::getItemMessageTableID);
    for (GetItemMessage messageEntry : messageEntries) {
        if (messageEntry.giid == RG_ICE_TRAP) {
            customMessageManager->CreateMessage(Randomizer::getItemMessageTableID, messageEntry.giid,
                                                { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, messageEntry.english,
                                                  messageEntry.german, messageEntry.french });
        } else {
            customMessageManager->CreateGetItemMessage(Randomizer::getItemMessageTableID, messageEntry.giid,
                                                       messageEntry.iid,
                                                       { TEXTBOX_TYPE_BLUE, TEXTBOX_POS_BOTTOM, messageEntry.english,
                                                         messageEntry.german, messageEntry.french });
        }
    }
}

void CreateScrubMessages() {
    CustomMessageManager* customMessageManager = CustomMessageManager::Instance;
    customMessageManager->AddCustomMessageTable(Randomizer::scrubMessageTableID);
    const std::vector<u8> prices = { 10, 40 };
    for (u8 price : prices) {
        customMessageManager->CreateMessage(Randomizer::scrubMessageTableID, price,
            { TEXTBOX_TYPE_BLACK, TEXTBOX_POS_BOTTOM,
              "\x12\x38\x82\All right! You win! In return for&sparing me, I will sell you a&%gmysterious item%w!&%r" +
                  std::to_string(price) + " Rupees%w it is!\x07\x10\xA3",
            // RANDTODO: Translate the below string to German.
              "\x12\x38\x82\All right! You win! In return for&sparing me, I will sell you a&%gmysterious item%w!&%r" +
                  std::to_string(price) + " Rupees%w it is!\x07\x10\xA3",
              "\x12\x38\x82J'abandonne! Tu veux bien m'acheter&un %gobjet mystérieux%w?&Ça fera %r" +
                  std::to_string(price) + " Rubis%w!\x07\x10\xA3"
            });
    }
}

void Randomizer::CreateCustomMessages() {
    // RANDTODO: Translate into french and german and replace GIMESSAGE_UNTRANSLATED
    // with GIMESSAGE(getItemID, itemID, english, german, french).
    const std::vector<GetItemMessage> getItemMessages = {
        GIMESSAGE(RG_ICE_TRAP, ITEM_NONE, "\x08\x06\x30You are a %bFOWL%w!\x0E\x20",
                  "\x08\x06\x15 Du bist ein %bDUMMKOPF%w!\x0E\x20", "\x08\x06\x50%bIDIOT%w\x0E\x20"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_BLUE_FIRE, ITEM_BLUE_FIRE,
                               "You got a %rBottle with Blue &Fire%w! Use it to melt Red Ice!",
                               "Vous obtenez une %rBouteille avec&une Flamme Bleue%w! Utilisez-la&pour faire fondre la %rGlace&Rouge%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_BIG_POE, ITEM_BIG_POE,
                               "You got a %rBig Poe in a Bottle%w!&Sell it to the Ghost Shop!", 
                               "Vous obtenez une %rBouteille avec&une Âme%w! Vendez-la au Marchand&d'Âme"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_BLUE_POTION, ITEM_POTION_BLUE,
                                "You got a %rBottle of Blue Potion%w!&Drink it to replenish your&%ghealth%w and %bmagic%w!",
                                "Vous obtenez une %rBouteille avec&une Potion Bleue%w! Buvez-la pour&restaurer votre %rénergie vitale%w&ainsi que votre %gmagie%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_FISH, ITEM_FISH,
                                "You got a %rFish in a Bottle%w!&It looks fresh and delicious!&They say Jabu-Jabu loves them!",
                                "Vous obtenez une %rBouteille avec&un Poisson%w! Il a l'air délicieux!&Il paraîtrait que %bJabu-Jabu %wen&serait friand!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_BUGS, ITEM_BUG,
                               "You got a %rBug in a Bottle%w!&They love to burrow in&dirt holes!",
                               "Vous obtenez une %rBouteille avec&des Insectes%w! Ils adorent creuser&dans la terre meuble!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_FAIRY, ITEM_FAIRY, 
                                "You got a %rFairy in a Bottle%w!&Use it wisely!",
                                "Vous obtenez une %rBouteille avec&une Fée%w! Faites-en bon usage!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_RED_POTION, ITEM_POTION_RED,
                               "You got a %rBottle of Red Potion%w!&Drink it to replenish your&%ghealth%w!",
                               "Vous obtenez une %rBouteille avec&une Potion Rouge%w! Buvez-la pour&restaurer votre %rénergie vitale%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_GREEN_POTION, ITEM_POTION_GREEN,
                               "You got a %rBottle of Green Potion%w!&Drink it to replenish your&%bmagic%w!",
                               "Vous obtenez une %rBouteille avec&une Potion Verte%w! Buvez-la pour&restaurer votre %gmagie%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTLE_WITH_POE, ITEM_POE,
                               "You got a %rPoe in a Bottle%w!&That creepy Ghost Shop might&be interested in this...",
                               "Vous obtenez une %rBouteille avec&un Esprit%w! Ça intéresserait&peut-être le vendeur d'Âme "),

        GIMESSAGE_NO_GERMAN(RG_GERUDO_FORTRESS_SMALL_KEY, ITEM_KEY_SMALL, "You found a %yThieves Hideout &%wSmall Key!", "Vous obtenez une %rPetite Clé %w&du %yRepaire des Voleurs%w!"),
        GIMESSAGE_NO_GERMAN(RG_FOREST_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %gForest Temple &%wSmall Key!", "Vous obtenez une %rPetite Clé %w&du %gTemple de la Forêt%w!"),
        GIMESSAGE_NO_GERMAN(RG_FIRE_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %rFire Temple &%wSmall Key!", "Vous obtenez une %rPetite Clé %w&du %rTemple du Feu%w!"),
        GIMESSAGE_NO_GERMAN(RG_WATER_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %bWater Temple &%wSmall Key!", "Vous obtenez une %rPetite Clé %w&du %bTemple de l'Eau%w!"),
        GIMESSAGE_NO_GERMAN(RG_SPIRIT_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %ySpirit Temple &%wSmall Key!", "Vous obtenez une %rPetite Clé %w&du %yTemple de l'Esprit%w!"),
        GIMESSAGE_NO_GERMAN(RG_SHADOW_TEMPLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %pShadow Temple &%wSmall Key!", "Vous obtenez une %rPetite Clé %w&du %pTemple de l'Ombre%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTOM_OF_THE_WELL_SMALL_KEY, ITEM_KEY_SMALL, "You found a %pBottom of the &Well %wSmall Key!", "Vous obtenez une %rPetite Clé %w&du %Puits%w!"),
        GIMESSAGE_NO_GERMAN(RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, ITEM_KEY_SMALL, "You found a %yGerudo Training &Grounds %wSmall Key!", "Vous obtenez une %rPetite Clé %w&du %yGymnase Gerudo%w!"),
        GIMESSAGE_NO_GERMAN(RG_GANONS_CASTLE_SMALL_KEY, ITEM_KEY_SMALL, "You found a %rGanon's Castle &%wSmall Key!", "Vous obtenez une %rPetite Clé %w&du %Château de Ganon%w!"),

        GIMESSAGE_NO_GERMAN(RG_FOREST_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %gForest Temple &%wBoss Key!", "Vous obtenez la %rClé d'or %wdu&%gTemple de la Forêt%w!"),
        GIMESSAGE_NO_GERMAN(RG_FIRE_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %rFire Temple &%wBoss Key!", "Vous obtenez la %rClé d'or %wdu&%rTemple du Feu%w!"),
        GIMESSAGE_NO_GERMAN(RG_WATER_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %bWater Temple &%wBoss Key!", "Vous obtenez la %rClé d'or %wdu&%bTemple de l'Eau%w!"),
        GIMESSAGE_NO_GERMAN(RG_SPIRIT_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %ySpirit Temple &%wBoss Key!", "Vous obtenez la %rClé d'or %wdu&%yTemple de l'Esprit%w!"),
        GIMESSAGE_NO_GERMAN(RG_SHADOW_TEMPLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %pShadow Temple &%wBoss Key!", "Vous obtenez la %rClé d'or %wdu&%pTemple de l'Ombre%w!"),
        GIMESSAGE_NO_GERMAN(RG_GANONS_CASTLE_BOSS_KEY, ITEM_KEY_BOSS, "You found the %rGanon's Castle &%wBoss Key!", "Vous obtenez la %rClé d'or %wdu&%rChâteau de Ganon%w!"),
        
        GIMESSAGE_NO_GERMAN(RG_DEKU_TREE_MAP, ITEM_DUNGEON_MAP, "You found the %gDeku Tree &%wMap!", "Vous obtenez la %rCarte %wde&l'%gArbre Mojo%w!"),
        GIMESSAGE_NO_GERMAN(RG_DODONGOS_CAVERN_MAP, ITEM_DUNGEON_MAP, "You found the %rDodongo's Cavern &%wMap!", "Vous obtenez la %rCarte %wde la&%rCaverne Dodongo%w!"),
        GIMESSAGE_NO_GERMAN(RG_JABU_JABUS_BELLY_MAP, ITEM_DUNGEON_MAP, "You found the %bJabu Jabu's Belly &%wMap!", "Vous obtenez la %rCarte %wdu &%bVentre de Jabu-Jabu%w!"),
        GIMESSAGE_NO_GERMAN(RG_FOREST_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %gForest Temple &%wMap!", "Vous obtenez la %rCarte %wdu &%gTemple de la Forêt%w!"),
        GIMESSAGE_NO_GERMAN(RG_FIRE_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %rFire Temple &%wMap!", "Vous obtenez la %rCarte %wdu &%rTemple du Feu%w!"),
        GIMESSAGE_NO_GERMAN(RG_WATER_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %bWater Temple &%wMap!", "Vous obtenez la %rCarte %wdu &%bTemple de l'Eau%w!"),
        GIMESSAGE_NO_GERMAN(RG_SPIRIT_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %ySpirit Temple &%wMap!", "Vous obtenez la %rCarte %wdu &%yTemple de l'Esprit%w!"),
        GIMESSAGE_NO_GERMAN(RG_SHADOW_TEMPLE_MAP, ITEM_DUNGEON_MAP, "You found the %pShadow Temple &%wMap!", "Vous obtenez la %rCarte %wdu &%pTemple de l'Ombre%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTOM_OF_THE_WELL_MAP, ITEM_DUNGEON_MAP, "You found the %pBottom of the &Well %wMap!", "Vous obtenez la %rCarte %wdu &%pPuits%w!"),
        GIMESSAGE_NO_GERMAN(RG_ICE_CAVERN_MAP, ITEM_DUNGEON_MAP, "You found the %cIce Cavern &%wMap!", "Vous obtenez la %rCarte %wde &la %cCaverne Polaire%w!"),

        GIMESSAGE_NO_GERMAN(RG_DEKU_TREE_COMPASS, ITEM_COMPASS, "You found the %gDeku Tree &%wCompass!", "Vous obtenez la %rBoussole %wde&l'%gArbre Mojo%w!"),
        GIMESSAGE_NO_GERMAN(RG_DODONGOS_CAVERN_COMPASS, ITEM_COMPASS, "You found the %rDodongo's Cavern &%wCompass!", "Vous obtenez la %rBoussole %wde la&%rCaverne Dodongo%w!"),
        GIMESSAGE_NO_GERMAN(RG_JABU_JABUS_BELLY_COMPASS, ITEM_COMPASS, "You found the %bJabu Jabu's Belly &%wCompass!", "Vous obtenez la %rBoussole %wdu &%bVentre de Jabu-Jabu%w!"),
        GIMESSAGE_NO_GERMAN(RG_FOREST_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %gForest Temple &%wCompass!", "Vous obtenez la %rBoussole %wdu &%gTemple de la Forêt%w!"),
        GIMESSAGE_NO_GERMAN(RG_FIRE_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %rFire Temple &%wCompass!", "Vous obtenez la %rBoussole %wdu &%rTemple du Feu%w!"),
        GIMESSAGE_NO_GERMAN(RG_WATER_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %bWater Temple &%wCompass!", "Vous obtenez la %rBoussole %wdu &%bTemple de l'Eau%w!"),
        GIMESSAGE_NO_GERMAN(RG_SPIRIT_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %ySpirit Temple &%wCompass!", "Vous obtenez la %rBoussole %wdu &%yTemple de l'Esprit%w!"),
        GIMESSAGE_NO_GERMAN(RG_SHADOW_TEMPLE_COMPASS, ITEM_COMPASS, "You found the %pShadow Temple &%wCompass!", "Vous obtenez la %rBoussole %wdu &%pTemple de l'Ombre%w!"),
        GIMESSAGE_NO_GERMAN(RG_BOTTOM_OF_THE_WELL_COMPASS, ITEM_COMPASS, "You found the %pBottom of the &Well %wCompass!", "Vous obtenez la %rBoussole %wdu &%pPuits%w!"),
        GIMESSAGE_NO_GERMAN(RG_ICE_CAVERN_COMPASS, ITEM_COMPASS, "You found the %cIce Cavern &%wCompass!", "Vous obtenez la %rBoussole %wde &la %cCaverne Polaire%w!"),
    };
    CreateGetItemMessages(getItemMessages);
    CreateScrubMessages();
}

class ExtendedVanillaTableInvalidItemIdException: public std::exception {
    private:
    s16 itemID;

    public:
      ExtendedVanillaTableInvalidItemIdException(s16 itemID): itemID(itemID) {}
      std::string what() {
        return itemID + " is not a valid ItemID for the extendedVanillaGetItemTable. If you are adding a new"
        "item, try adding it to randoGetItemTable instead.";
      }
};

void InitRandoItemTable() {
    // These entries have ItemIDs from vanilla, but not GetItemIDs or entries in the old sGetItemTable
    GetItemEntry extendedVanillaGetItemTable[] = {
        GET_ITEM(ITEM_MEDALLION_LIGHT, OBJECT_GI_MEDAL, GID_MEDALLION_LIGHT, 0x40, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_LIGHT_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_FOREST, OBJECT_GI_MEDAL, GID_MEDALLION_FOREST, 0x3E, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_FOREST_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_FIRE, OBJECT_GI_MEDAL, GID_MEDALLION_FIRE, 0x3C, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_FIRE_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_WATER, OBJECT_GI_MEDAL, GID_MEDALLION_WATER, 0x3D, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_WATER_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_SHADOW, OBJECT_GI_MEDAL, GID_MEDALLION_SHADOW, 0x41, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SHADOW_MEDALLION),
        GET_ITEM(ITEM_MEDALLION_SPIRIT, OBJECT_GI_MEDAL, GID_MEDALLION_SPIRIT, 0x3F, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SPIRIT_MEDALLION),

        GET_ITEM(ITEM_KOKIRI_EMERALD, OBJECT_GI_JEWEL, GID_KOKIRI_EMERALD, 0x80, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_KOKIRI_EMERALD),
        GET_ITEM(ITEM_GORON_RUBY, OBJECT_GI_JEWEL, GID_GORON_RUBY, 0x81, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_GORON_RUBY),
        GET_ITEM(ITEM_ZORA_SAPPHIRE, OBJECT_GI_JEWEL, GID_ZORA_SAPPHIRE, 0x82, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_ZORA_SAPPHIRE),

        GET_ITEM(ITEM_SONG_LULLABY, OBJECT_GI_MELODY, GID_SONG_ZELDA, 0xD4, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_ZELDAS_LULLABY),
        GET_ITEM(ITEM_SONG_SUN, OBJECT_GI_MELODY, GID_SONG_SUN, 0xD3, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SUNS_SONG),
        GET_ITEM(ITEM_SONG_EPONA, OBJECT_GI_MELODY, GID_SONG_EPONA, 0xD2, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_EPONAS_SONG),
        GET_ITEM(ITEM_SONG_STORMS, OBJECT_GI_MELODY, GID_SONG_STORM, 0xD6, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SONG_OF_STORMS),
        GET_ITEM(ITEM_SONG_TIME, OBJECT_GI_MELODY, GID_SONG_TIME, 0xD5, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SONG_OF_TIME),
        GET_ITEM(ITEM_SONG_SARIA, OBJECT_GI_MELODY, GID_SONG_SARIA, 0xD1, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SARIAS_SONG),

        GET_ITEM(ITEM_SONG_MINUET, OBJECT_GI_MELODY, GID_SONG_MINUET, 0x73, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_MINUET_OF_FOREST),
        GET_ITEM(ITEM_SONG_BOLERO, OBJECT_GI_MELODY, GID_SONG_BOLERO, 0x74, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_BOLERO_OF_FIRE),
        GET_ITEM(ITEM_SONG_SERENADE, OBJECT_GI_MELODY, GID_SONG_SERENADE, 0x75, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_SERENADE_OF_WATER),
        GET_ITEM(ITEM_SONG_NOCTURNE, OBJECT_GI_MELODY, GID_SONG_NOCTURNE, 0x77, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_NOCTURNE_OF_SHADOW),
        GET_ITEM(ITEM_SONG_REQUIEM, OBJECT_GI_MELODY, GID_SONG_REQUIEM, 0x76, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_REQUIEM_OF_SPIRIT),
        GET_ITEM(ITEM_SONG_PRELUDE, OBJECT_GI_MELODY, GID_SONG_PRELUDE, 0x78, 0x80, CHEST_ANIM_LONG, MOD_NONE, RG_PRELUDE_OF_LIGHT),
    };

    // These do not have ItemIDs or GetItemIDs from vanilla, so I'm using their
    // RandomizerGet enum values for both.
    GetItemEntry randoGetItemTable[] = {
        GET_ITEM(RG_ICE_TRAP, OBJECT_GI_RUPY, GID_RUPEE_GOLD, 0, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_ICE_TRAP),
        GET_ITEM(RG_MAGIC_SINGLE, OBJECT_GI_MAGICPOT, GID_MAGIC_SMALL, 0xE4, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_MAGIC_SINGLE),
        GET_ITEM(RG_MAGIC_DOUBLE, OBJECT_GI_MAGICPOT, GID_MAGIC_LARGE, 0xE8, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_MAGIC_DOUBLE),
        GET_ITEM(RG_DOUBLE_DEFENSE, OBJECT_GI_HEARTS, GID_HEART_CONTAINER, 0xE9, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DOUBLE_DEFENSE),
        GET_ITEM(RG_BOTTLE_WITH_RED_POTION, OBJECT_GI_LIQUID, GID_POTION_RED, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_RED_POTION),
        GET_ITEM(RG_BOTTLE_WITH_GREEN_POTION, OBJECT_GI_LIQUID, GID_POTION_GREEN, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_GREEN_POTION),
        GET_ITEM(RG_BOTTLE_WITH_BLUE_POTION, OBJECT_GI_LIQUID, GID_POTION_BLUE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_BLUE_POTION),
        GET_ITEM(RG_BOTTLE_WITH_FAIRY, OBJECT_GI_BOTTLE, GID_BOTTLE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_FAIRY),
        GET_ITEM(RG_BOTTLE_WITH_FISH, OBJECT_GI_FISH, GID_FISH, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_FISH),
        GET_ITEM(RG_BOTTLE_WITH_BLUE_FIRE, OBJECT_GI_FIRE, GID_BLUE_FIRE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_BLUE_FIRE),
        GET_ITEM(RG_BOTTLE_WITH_BUGS, OBJECT_GI_INSECT, GID_BUG, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_BUGS),
        GET_ITEM(RG_BOTTLE_WITH_POE, OBJECT_GI_GHOST, GID_POE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_POE),
        GET_ITEM(RG_BOTTLE_WITH_BIG_POE, OBJECT_GI_GHOST, GID_BIG_POE, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTLE_WITH_BIG_POE),
        GET_ITEM(RG_GERUDO_FORTRESS_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_GERUDO_FORTRESS_SMALL_KEY),
        GET_ITEM(RG_FOREST_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_FOREST_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_FIRE_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_FIRE_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_WATER_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_WATER_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_SPIRIT_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_SHADOW_TEMPLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_SHADOW_TEMPLE_SMALL_KEY),
        GET_ITEM(RG_BOTTOM_OF_THE_WELL_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_BOTTOM_OF_THE_WELL_SMALL_KEY),
        GET_ITEM(RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY),
        GET_ITEM(RG_GANONS_CASTLE_SMALL_KEY, OBJECT_GI_KEY, GID_KEY_SMALL, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_SHORT, MOD_RANDOMIZER, RG_GANONS_CASTLE_SMALL_KEY),
        GET_ITEM(RG_FOREST_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FOREST_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_FIRE_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FIRE_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_WATER_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_WATER_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_SPIRIT_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_SHADOW_TEMPLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SHADOW_TEMPLE_BOSS_KEY),
        GET_ITEM(RG_GANONS_CASTLE_BOSS_KEY, OBJECT_GI_BOSSKEY, GID_KEY_BOSS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_GANONS_CASTLE_BOSS_KEY),
        GET_ITEM(RG_DEKU_TREE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DEKU_TREE_MAP),
        GET_ITEM(RG_DODONGOS_CAVERN_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DODONGOS_CAVERN_MAP),
        GET_ITEM(RG_JABU_JABUS_BELLY_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_JABU_JABUS_BELLY_MAP),
        GET_ITEM(RG_FOREST_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FOREST_TEMPLE_MAP),
        GET_ITEM(RG_FIRE_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FIRE_TEMPLE_MAP),
        GET_ITEM(RG_WATER_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_WATER_TEMPLE_MAP),
        GET_ITEM(RG_SPIRIT_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_MAP),
        GET_ITEM(RG_SHADOW_TEMPLE_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SHADOW_TEMPLE_MAP),
        GET_ITEM(RG_BOTTOM_OF_THE_WELL_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTOM_OF_THE_WELL_MAP),
        GET_ITEM(RG_ICE_CAVERN_MAP, OBJECT_GI_MAP, GID_DUNGEON_MAP, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_ICE_CAVERN_MAP),
        GET_ITEM(RG_DEKU_TREE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DEKU_TREE_COMPASS),
        GET_ITEM(RG_DODONGOS_CAVERN_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_DODONGOS_CAVERN_COMPASS),
        GET_ITEM(RG_JABU_JABUS_BELLY_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_JABU_JABUS_BELLY_COMPASS),
        GET_ITEM(RG_FOREST_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FOREST_TEMPLE_COMPASS),
        GET_ITEM(RG_FIRE_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_FIRE_TEMPLE_COMPASS),
        GET_ITEM(RG_WATER_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_WATER_TEMPLE_COMPASS),
        GET_ITEM(RG_SPIRIT_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SPIRIT_TEMPLE_COMPASS),
        GET_ITEM(RG_SHADOW_TEMPLE_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_SHADOW_TEMPLE_COMPASS),
        GET_ITEM(RG_BOTTOM_OF_THE_WELL_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_BOTTOM_OF_THE_WELL_COMPASS),
        GET_ITEM(RG_ICE_CAVERN_COMPASS, OBJECT_GI_COMPASS, GID_COMPASS, TEXT_RANDOMIZER_CUSTOM_ITEM, 0x80, CHEST_ANIM_LONG, MOD_RANDOMIZER, RG_ICE_CAVERN_COMPASS),
    };
    ItemTableManager::Instance->AddItemTable(MOD_RANDOMIZER);
    for (int i = 0; i < ARRAY_COUNT(extendedVanillaGetItemTable); i++) {
        ItemTableManager::Instance->AddItemEntry(MOD_RANDOMIZER, extendedVanillaGetItemTable[i].getItemId, extendedVanillaGetItemTable[i]);
    }
    for (int i = 0; i < ARRAY_COUNT(randoGetItemTable); i++) {
        ItemTableManager::Instance->AddItemEntry(MOD_RANDOMIZER, randoGetItemTable[i].itemId, randoGetItemTable[i]);
    }
}


void InitRando() {
    SohImGui::AddWindow("Randomizer", "Randomizer Settings", DrawRandoEditor);
    Randomizer::CreateCustomMessages();
    InitRandoItemTable();
}

extern "C" {

void Rando_Init(void) {
    InitRando();
}

}
