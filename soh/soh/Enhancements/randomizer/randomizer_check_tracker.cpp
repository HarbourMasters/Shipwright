#include "randomizer_check_tracker.h"
#include <soh/OTRGlobals.h>
#include "GlobalCtx2.h"
#include "../libultraship/ImGuiImpl.h"
#include <soh/Enhancements/debugger/ImGuiHelpers.h>
#include <soh/Enhancements/randomizer/randomizer.h>
#include "Lib/nlohmann/json.hpp"

#include <fstream>
#include <array>
#include <bit>
#include <map>
#include <string>
#include <Cvar.h>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern GlobalContext* gGlobalCtx;
}

typedef enum {
    RR_UNKNOWN,
    RR_KF,
    RR_LW,
    RR_HF,
    RR_LH,
    RR_GV,
    RR_GF,
    RR_WASTELAND,
    RR_COLOSSUS,
    RR_MARKET,
    RR_HC,
    RR_KAK,
    RR_GRAVEYARD,
    RR_DMT,
    RR_GC,
    RR_DMC,
    RR_ZR,
    RR_ZD,
    RR_ZF,
    RR_LLR,
    RR_SFM,
    RR_GTG,
    RR_DEKU,
    RR_DC,
    RR_JABU,
    RR_FOREST,
    RR_FIRE,
    RR_WATER,
    RR_SHADOW,
    RR_SPIRIT,
    RR_ICE,
    RR_BOTW,
    RR_TOT,
    RR_OGC,
    RR_GANONS_CASTLE
} RandomizerRegion;

RandomizerRegion currentRegion = RR_UNKNOWN;

void setRegionByCurrentSceneID() {
    switch (gGlobalCtx->sceneNum) {
        case SCENE_SPOT04:
        case SCENE_KOKIRI_HOME:
        case SCENE_KOKIRI_HOME3:
        case SCENE_KOKIRI_HOME4:
        case SCENE_KOKIRI_HOME5:
        case SCENE_KOKIRI_SHOP:
        case SCENE_LINK_HOME:
            currentRegion = RR_KF; // Kokiri Forest
            break;
        case SCENE_SPOT00:
            currentRegion = RR_HF; // Hyrule Field
            break;
        case SCENE_HYLIA_LABO:
        case SCENE_TURIBORI:
        case SCENE_SPOT06:
            currentRegion = RR_LH; // Lake Hylia
            break;
        case SCENE_HAKAANA:
        case SCENE_HAKAANA2:
        case SCENE_HAKAANA_OUKE:
        case SCENE_HUT:
        case SCENE_SPOT02:
            currentRegion = RR_GRAVEYARD; // Graveyard
            break;
        case SCENE_HAKASITARELAY: // TODO: Dampe / Windmill, now in KAK region
        case SCENE_DRAG:
        case SCENE_KINSUTA:
        case SCENE_MAHOUYA:
        case SCENE_SHOP1: // TODO: Bazaar
        case SCENE_SYATEKIJYOU:
        case SCENE_LABO:
        case SCENE_KAKARIKO:
        case SCENE_SPOT01:
            currentRegion = RR_KAK; // Kakariko Village
            break;
        case SCENE_SPOT03:
            currentRegion = RR_ZR; // Zora's River
            break;
        case SCENE_SPOT05:
            currentRegion = RR_SFM; // Sacred Forest Meadow
            break;
        case SCENE_ZOORA:
        case SCENE_SPOT07:
            currentRegion = RR_ZD; // Zora's Domain
            break;
        case SCENE_SPOT08:
            currentRegion = RR_ZF; // Zora's Fountain
            break;
        case SCENE_TENT:
        case SCENE_SPOT09:
            currentRegion = RR_GV; // Gerudo Valley
            break;
        case SCENE_SPOT10:
            currentRegion = RR_LW; // Lost Woods
            break;
        case SCENE_SPOT11:
            currentRegion = RR_COLOSSUS; // Desert Colossus
            break;
        case SCENE_GERUDOWAY:
        case SCENE_SPOT12:
            currentRegion = RR_GF; // Gerudo Fortress
            break;
        case SCENE_SPOT13:
            currentRegion = RR_WASTELAND; // Wasteland
            break;
        case SCENE_HAIRAL_NIWA:
        case SCENE_HAIRAL_NIWA2:
        case SCENE_HAIRAL_NIWA_N:
        case SCENE_NAKANIWA:
        case SCENE_SPOT15:
            currentRegion = RR_HC; // Hyrule Castle
            break;
        case SCENE_SPOT16:
            currentRegion = RR_DMT; // Death Mountain Trail
            break;
        case SCENE_SPOT17:
            currentRegion = RR_DMC; // Death Mountain Crater
            break;
        case SCENE_GOLON:
        case SCENE_SPOT18:
            currentRegion = RR_GC; // Goron City
            break;
        case SCENE_MALON_STABLE:
        case SCENE_SOUKO:
        case SCENE_SPOT20:
            currentRegion = RR_LLR; // Lon Lon Ranch
            break;
        case SCENE_MORIBOSSROOM:
        case SCENE_BMORI1:
            currentRegion = RR_FOREST; // Forest Temple
            break;
        case SCENE_HIDAN:
        case SCENE_FIRE_BS:
            currentRegion = RR_FIRE; // Fire Temple
            break;
        case SCENE_HAKADANCH:
            currentRegion = RR_BOTW; // Bottom of the Well
            break;
        case SCENE_HAKADAN:
        case SCENE_HAKADAN_BS:
            currentRegion = RR_SHADOW; // Shadow temple
            break;
        case SCENE_JYASINBOSS:
        case SCENE_JYASINZOU:
            currentRegion = RR_SPIRIT; // Spirit Temple
            break;
        case SCENE_MIZUSIN:
        case SCENE_MIZUSIN_BS:
            currentRegion = RR_WATER; // Water Temple
            break;
        case SCENE_MEN:
            currentRegion = RR_GTG; // Gerudo Training Grounds
            break;
        case SCENE_ENRUI:
        case SCENE_ENTRA:
        case SCENE_ENTRA_N:
        case SCENE_FACE_SHOP:
        case SCENE_ALLEY_SHOP:
        case SCENE_BOWLING:
        case SCENE_MIHARIGOYA:
        case SCENE_NIGHT_SHOP:
        case SCENE_MARKET_ALLEY:
        case SCENE_MARKET_ALLEY_N:
        case SCENE_MARKET_DAY:
        case SCENE_MARKET_NIGHT:
        case SCENE_MARKET_RUINS:
        case SCENE_KAKARIKO3:
        case SCENE_IMPA:
        case SCENE_TAKARAYA:
            currentRegion = RR_MARKET; // Market
            break;
        case SCENE_TOKINOMA:
        case SCENE_SHRINE:
        case SCENE_SHRINE_N:
        case SCENE_SHRINE_R:
            currentRegion = RR_TOT; // Temple of Time (Interior + exterior)
            break;
        case SCENE_YDAN:
        case SCENE_YDAN_BOSS:
            currentRegion = RR_DEKU; // Inside the Deku Tree
            break;
        case SCENE_BDAN:
        case SCENE_BDAN_BOSS:
            currentRegion = RR_JABU; // Jabu Jabu's Belly
            break;
        case SCENE_DDAN:
        case SCENE_DDAN_BOSS:
            currentRegion = RR_DC; // Dodongon's Cavern
            break;
        case SCENE_ICE_DOUKUTO:
            currentRegion = RR_ICE; // Ice Cavern
            break;
        case SCENE_GANON_TOU:
            currentRegion = RR_OGC; // Outside Ganon's Castle
            break;
        case SCENE_GANON_BOSS:
        case SCENE_GANONTIKA:
        case SCENE_GANON:
            currentRegion = RR_GANONS_CASTLE; // Ganon's Castle
            break;
        default:
            currentRegion = RR_UNKNOWN;
            break;
    }
}

std::unordered_map<RandomizerRegion, std::string> RegionToString = { { RR_UNKNOWN, "Unknown" },
                                                                     { RR_KF, "Kokiri Forest" },
                                                                     { RR_LW, "Lost Woods" },
                                                                     { RR_HF, "Hyrule Field" },
                                                                     { RR_LH, "Lake Hylia" },
                                                                     { RR_GV, "Gerudo Valley" },
                                                                     { RR_GF, "Gerudo Fortress" },
                                                                     { RR_WASTELAND, "Haunted Wasteland" },
                                                                     { RR_COLOSSUS, "Desert Colossus" },
                                                                     { RR_MARKET, "Hyrule Market" },
                                                                     { RR_HC, "Hyrule Castle" },
                                                                     { RR_KAK, "Kakariko Village" },
                                                                     { RR_GRAVEYARD, "Graveyard" },
                                                                     { RR_DMT, "Death Mountain Trail" },
                                                                     { RR_GC, "Goron City" },
                                                                     { RR_DMC, "Death Mountain Crater" },
                                                                     { RR_ZR, "Zora's River" },
                                                                     { RR_ZD, "Zora's Domain" },
                                                                     { RR_ZF, "Zora's Fountain" },
                                                                     { RR_LLR, "Lon Lon Ranch" },
                                                                     { RR_SFM, "Sacret Forest Meadow" },
                                                                     { RR_GTG, "Gerudo Training Grounds" },
                                                                     { RR_DEKU, "Inside the Deku Tree" },
                                                                     { RR_DC, "Dodongo's Cavern" },
                                                                     { RR_JABU, "Inside Jabu-Jabu's Belly" },
                                                                     { RR_FOREST, "Forest Temple" },
                                                                     { RR_FIRE, "Fire Temple" },
                                                                     { RR_WATER, "Water Temple" },
                                                                     { RR_SHADOW, "Shadow Temple" },
                                                                     { RR_SPIRIT, "Sprit Temple" },
                                                                     { RR_ICE, "Ice Cavern" },
                                                                     { RR_BOTW, "Bottom of the Well" },
                                                                     { RR_TOT, "Temple of Time" },
                                                                     { RR_OGC, "Outside Ganon's Castle" },
                                                                     { RR_GANONS_CASTLE, "Ganon's Castle" } };

std::unordered_map<RandomizerCheck, std::string> CheckEnumToName = {
    { RC_UNKNOWN_CHECK, "Invalid Location" },
    { RC_KF_KOKIRI_SWORD_CHEST, "KF Kokiri Sword Chest" },
    { RC_KF_MIDOS_TOP_LEFT_CHEST, "KF Mido Top Left Chest" },
    { RC_KF_MIDOS_TOP_RIGHT_CHEST, "KF Mido Top Right Chest" },
    { RC_KF_MIDOS_BOTTOM_LEFT_CHEST, "KF Mido Bottom Left Chest" },
    { RC_KF_MIDOS_BOTTOM_RIGHT_CHEST, "KF Mido Bottom Right Chest" },
    { RC_KF_STORMS_GROTTO_CHEST, "KF Storms Grotto Chest" },
    { RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST, "LW Near Shortcuts Grotto Chest" },
    { RC_LW_SKULL_KID, "LW Skull Kid" },
    { RC_LW_TRADE_COJIRO, "LW Trade Cojiro" },
    { RC_LW_TRADE_ODD_POTION, "LW Trade Odd Potion" },
    { RC_LW_OCARINA_MEMORY_GAME, "LW Ocarina Memory Game" },
    { RC_LW_TARGET_IN_WOODS, "LW Target in Woods" },
    { RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT, "LW Deku Scrub Near Deku Theater Right" },
    { RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT, "LW Deku Scrub Near Deku Theater Left" },
    { RC_LW_DEKU_SCRUB_NEAR_BRIDGE, "LW Deku Scrub Near Bridge" },
    { RC_LW_DEKU_SCRUB_GROTTO_REAR, "LW Deku Scrub Grotto Rear" },
    { RC_LW_DEKU_SCRUB_GROTTO_FRONT, "LW Deku Scrub Grotto Front" },
    { RC_DEKU_THEATER_SKULL_MASK, "Deku Theater Skull Mask" },
    { RC_DEKU_THEATER_MASK_OF_TRUTH, "Deku Theater Mask of Truth" },
    { RC_SFM_WOLFOS_GROTTO_CHEST, "SFM Wolfos Grotto Chest" },
    { RC_SFM_DEKU_SCRUB_GROTTO_REAR, "SFM Deku Scrub Grotto Rear" },
    { RC_SFM_DEKU_SCRUB_GROTTO_FRONT, "SFM Deku Scrub Grotto Front" },
    { RC_HF_SOUTHEAST_GROTTO_CHEST, "HF Southeast Grotto Chest" },
    { RC_HF_OPEN_GROTTO_CHEST, "HF Open Grotto Chest" },
    { RC_HF_NEAR_MARKET_GROTTO_CHEST, "HF Near Market Grotto Chest" },
    { RC_HF_OCARINA_OF_TIME_ITEM, "HF Ocarina of Time Item" },
    { RC_HF_TEKTITE_GROTTO_FREESTANDING_POH, "HF Tektite Grotto Freestanding PoH" },
    { RC_HF_DEKU_SCRUB_GROTTO, "HF Deku Scrub Grotto" },
    { RC_LH_CHILD_FISHING, "LH Child Fishing" },
    { RC_LH_ADULT_FISHING, "LH Adult Fishing" },
    { RC_LH_LAB_DIVE, "LH Lab Dive" },
    { RC_LH_TRADE_FROG, "LH Lab Trade Eyeball Frog" },
    { RC_LH_UNDERWATER_ITEM, "LH Underwater Item" },
    { RC_LH_SUN, "LH Sun" },
    { RC_LH_FREESTANDING_POH, "LH Freestanding PoH" },
    { RC_LH_DEKU_SCRUB_GROTTO_LEFT, "LH Deku Scrub Grotto Left" },
    { RC_LH_DEKU_SCRUB_GROTTO_RIGHT, "LH Deku Scrub Grotto Right" },
    { RC_LH_DEKU_SCRUB_GROTTO_CENTER, "LH Deku Scrub Grotto Center" },
    { RC_GV_CHEST, "GV Chest" },
    { RC_GV_TRADE_SAW, "GV Trade Saw" },
    { RC_GV_WATERFALL_FREESTANDING_POH, "GV Waterfall Freestanding PoH" },
    { RC_GV_CRATE_FREESTANDING_POH, "GV Crate Freestanding PoH" },
    { RC_GV_DEKU_SCRUB_GROTTO_REAR, "GV Deku Scrub Grotto Rear" },
    { RC_GV_DEKU_SCRUB_GROTTO_FRONT, "GV Deku Scrub Grotto Front" },
    { RC_GF_CHEST, "GF Chest" },
    { RC_GF_HBA_1000_POINTS, "GF HBA 1000 Points" },
    { RC_GF_HBA_1500_POINTS, "GF HBA 1500 Points" },
    { RC_GF_GERUDO_MEMBERSHIP_CARD, "GF Gerudo Membership Card" },
    { RC_GF_NORTH_F1_CARPENTER, "GF North F1 Carpenter" },
    { RC_GF_NORTH_F2_CARPENTER, "GF North F2 Carpenter" },
    { RC_GF_SOUTH_F1_CARPENTER, "GF South F1 Carpenter" },
    { RC_GF_SOUTH_F2_CARPENTER, "GF South F2 Carpenter" },
    { RC_WASTELAND_CHEST, "Wasteland Chest" },
    { RC_WASTELAND_BOMBCHU_SALESMAN, "Wasteland Carpet Salesman" },
    { RC_COLOSSUS_FREESTANDING_POH, "Colossus Freestanding PoH" },
    { RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR, "Colossus Deku Scrub Grotto Rear" },
    { RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT, "Colossus Deku Scrub Grotto Front" },
    { RC_MARKET_TREASURE_CHEST_GAME_REWARD, "MK Treasure Chest Game Reward" },
    { RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE, "MK Bombchu Bowling First Prize" },
    { RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE, "MK Bombchu Bowling Second Prize" },
    { RC_UNKNOWN_CHECK, "MK Bombchu Bowling Bombchus" },
    { RC_MARKET_LOST_DOG, "MK Lost Dog" },
    { RC_MARKET_SHOOTING_GALLERY_REWARD, "MK Shooting Gallery" },
    { RC_MARKET_10_BIG_POES, "MK 10 Big Poes" },
    { RC_MARKET_TREASURE_CHEST_GAME_ITEM_1, "MK Chest Game First Room Chest" },
    { RC_MARKET_TREASURE_CHEST_GAME_ITEM_2, "MK Chest Game Second Room Chest" },
    { RC_MARKET_TREASURE_CHEST_GAME_ITEM_3, "MK Chest Game Third Room Chest" },
    { RC_MARKET_TREASURE_CHEST_GAME_ITEM_4, "MK Chest Game Fourth Room Chest" },
    { RC_MARKET_TREASURE_CHEST_GAME_ITEM_5, "MK Chest Game Fifth Room Chest" },
    { RC_HC_MALON_EGG, "HC Malon Egg" },
    { RC_HC_ZELDAS_LETTER, "HC Zeldas Letter" },
    { RC_KAK_REDEAD_GROTTO_CHEST, "Kak Redead Grotto Chest" },
    { RC_KAK_OPEN_GROTTO_CHEST, "Kak Open Grotto Chest" },
    { RC_KAK_10_GOLD_SKULLTULA_REWARD, "Kak 10 Gold Skulltula Reward" },
    { RC_KAK_20_GOLD_SKULLTULA_REWARD, "Kak 20 Gold Skulltula Reward" },
    { RC_KAK_30_GOLD_SKULLTULA_REWARD, "Kak 30 Gold Skulltula Reward" },
    { RC_KAK_40_GOLD_SKULLTULA_REWARD, "Kak 40 Gold Skulltula Reward" },
    { RC_KAK_50_GOLD_SKULLTULA_REWARD, "Kak 50 Gold Skulltula Reward" },
    { RC_KAK_MAN_ON_ROOF, "Kak Man on Roof" },
    { RC_KAK_SHOOTING_GALLERY_REWARD, "Kak Shooting Gallery Reward" },
    { RC_KAK_TRADE_ODD_MUSHROOM, "Kak Trade Odd Mushroom" },
    { RC_KAK_ANJU_AS_ADULT, "Kak Anju as Adult" },
    { RC_KAK_ANJU_AS_CHILD, "Kak Anju as Child" },
    { RC_KAK_TRADE_POCKET_CUCCO, "Kak Trade Pocket Cucco" },
    { RC_KAK_IMPAS_HOUSE_FREESTANDING_POH, "Kak Impas House Freestanding PoH" },
    { RC_KAK_WINDMILL_FREESTANDING_POH, "Kak Windmill Freestanding PoH" },
    { RC_GRAVEYARD_SHIELD_GRAVE_CHEST, "GY Shield Grave Chest" },
    { RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST, "GY Heart Piece Grave Chest" },
    { RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST, "GY Composers Grave Chest" },
    { RC_GRAVEYARD_HOOKSHOT_CHEST, "GY Hookshot Chest" },
    { RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH, "GY Dampe Race Freestanding PoH" },
    { RC_GRAVEYARD_FREESTANDING_POH, "GY Freestanding PoH" },
    { RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR, "GY Dampe Gravedigging Tour" },
    { RC_DMT_CHEST, "DMT Chest" },
    { RC_DMT_STORMS_GROTTO_CHEST, "DMT Storms Grotto Chest" },
    { RC_DMT_TRADE_BROKEN_SWORD, "DMT Trade Broken Sword" },
    { RC_DMT_TRADE_EYEDROPS, "DMT Trade Eyedrops" },
    { RC_DMT_TRADE_CLAIM_CHECK, "DMT Trade Claim Check" },
    { RC_DMT_FREESTANDING_POH, "DMT Freestanding PoH" },
    { RC_GC_MAZE_LEFT_CHEST, "GC Maze Left Chest" },
    { RC_GC_MAZE_RIGHT_CHEST, "GC Maze Right Chest" },
    { RC_GC_MAZE_CENTER_CHEST, "GC Maze Center Chest" },
    { RC_GC_ROLLING_GORON_AS_CHILD, "GC Rolling Goron as Child" },
    { RC_GC_ROLLING_GORON_AS_ADULT, "GC Rolling Goron as Adult" },
    { RC_GC_DARUNIAS_JOY, "GC Darunias Joy" },
    { RC_GC_POT_FREESTANDING_POH, "GC Pot Freestanding PoH" },
    { RC_GC_DEKU_SCRUB_GROTTO_LEFT, "GC Deku Scrub Grotto Left" },
    { RC_GC_DEKU_SCRUB_GROTTO_RIGHT, "GC Deku Scrub Grotto Right" },
    { RC_GC_DEKU_SCRUB_GROTTO_CENTER, "GC Deku Scrub Grotto Center" },
    { RC_GC_MEDIGORON, "GC Medigoron" },
    { RC_DMC_UPPER_GROTTO_CHEST, "DMC Upper Grotto Chest" },
    { RC_DMC_WALL_FREESTANDING_POH, "DMC Wall Freestanding PoH" },
    { RC_DMC_VOLCANO_FREESTANDING_POH, "DMC Volcano Freestanding PoH" },
    { RC_DMC_DEKU_SCRUB, "DMC Deku Scrub" },
    { RC_DMC_DEKU_SCRUB_GROTTO_LEFT, "DMC Deku Scrub Grotto Left" },
    { RC_DMC_DEKU_SCRUB_GROTTO_RIGHT, "DMC Deku Scrub Grotto Right" },
    { RC_DMC_DEKU_SCRUB_GROTTO_CENTER, "DMC Deku Scrub Grotto Center" },
    { RC_ZR_OPEN_GROTTO_CHEST, "ZR Open Grotto Chest" },
    { RC_ZR_MAGIC_BEAN_SALESMAN, "ZR Magic Bean Salesman" },
    { RC_ZR_FROGS_IN_THE_RAIN, "ZR Frogs in the Rain" },
    { RC_ZR_FROGS_OCARINA_GAME, "ZR Frogs Ocarina Game" },
    { RC_ZR_FROGS_EPONAS_SONG, "ZR Epona's Song" },
    { RC_ZR_FROGS_SARIAS_SONG, "ZR Saria's Song" },
    { RC_ZR_FROGS_SONG_OF_TIME, "ZR Song of Time" },
    { RC_ZR_FROGS_SUNS_SONG, "ZR Sun's Song" },
    { RC_ZR_FROGS_ZELDAS_LULLABY, "ZR Zelda's Lullaby" },
    { RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH, "ZR Near Open Grotto Freestanding PoH" },
    { RC_ZR_NEAR_DOMAIN_FREESTANDING_POH, "ZR Near Domain Freestanding PoH" },
    { RC_ZR_DEKU_SCRUB_GROTTO_REAR, "ZR Deku Scrub Grotto Rear" },
    { RC_ZR_DEKU_SCRUB_GROTTO_FRONT, "ZR Deku Scrub Grotto Front" },
    { RC_ZD_CHEST, "ZD Chest" },
    { RC_ZD_DIVING_MINIGAME, "ZD Diving Minigame" },
    { RC_ZD_KING_ZORA_THAWED, "ZD King Zora Thawed" },
    { RC_ZD_TRADE_PRESCRIPTION, "ZD Trade Prescription" },
    { RC_ZF_ICEBERC_FREESTANDING_POH, "ZF Iceberg Freestanding PoH" },
    { RC_ZF_BOTTOM_FREESTANDING_POH, "ZF Bottom Freestanding PoH" },
    { RC_LLR_TALONS_CHICKENS, "LLR Talons Chickens" },
    { RC_LLR_FREESTANDING_POH, "LLR Freestanding PoH" },
    { RC_LLR_DEKU_SCRUB_GROTTO_LEFT, "LLR Deku Scrub Grotto Left" },
    { RC_LLR_DEKU_SCRUB_GROTTO_RIGHT, "LLR Deku Scrub Grotto Right" },
    { RC_LLR_DEKU_SCRUB_GROTTO_CENTER, "LLR Deku Scrub Grotto Center" },
    { RC_DEKU_TREE_MAP_CHEST, "Deku Tree Map Chest" },
    { RC_DEKU_TREE_COMPASS_CHEST, "Deku Tree Compass Chest" },
    { RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST, "Deku Tree Compass Room Side Chest" },
    { RC_DEKU_TREE_BASEMENT_CHEST, "Deku Tree Basement Chest" },
    { RC_DEKU_TREE_SLINGSHOT_CHEST, "Deku Tree Slingshot Chest" },
    { RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST, "Deku Tree Slingshot Room Side Chest" },
    { RC_DEKU_TREE_MQ_MAP_CHEST, "Deku Tree MQ Map Chest" },
    { RC_DEKU_TREE_MQ_COMPASS_CHEST, "Deku Tree MQ Compass Chest" },
    { RC_DEKU_TREE_MQ_SLINGSHOT_CHEST, "Deku Tree MQ Slingshot Chest" },
    { RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST, "Deku Tree MQ Slingshot Room Back Chest" },
    { RC_DEKU_TREE_MQ_BASEMENT_CHEST, "Deku Tree MQ Basement Chest" },
    { RC_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST, "Deku Tree MQ Before Spinning Log Chest" },
    { RC_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST, "Deku Tree MQ After Spinning Log Chest" },
    { RC_DEKU_TREE_MQ_DEKU_SCRUB, "Deku Tree MQ Deku Scrub" },
    { RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST, "Dodongos Cavern Boss Room Chest" },
    { RC_DODONGOS_CAVERN_MAP_CHEST, "Dodongos Cavern Map Chest" },
    { RC_DODONGOS_CAVERN_COMPASS_CHEST, "Dodongos Cavern Compass Chest" },
    { RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST, "Dodongos Cavern Bomb Flower Platform Chest" },
    { RC_DODONGOS_CAVERN_BOMB_BAG_CHEST, "Dodongos Cavern Bomb Bag Chest" },
    { RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST, "Dodongos Cavern End Of Bridge Chest" },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT, "Dodongos Cavern Deku Scrub Near Bomb Bag Left" },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS, "Dodongos Cavern Deku Scrub Side Room Near Dodongos" },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT, "Dodongos Cavern Deku Scrub Near Bomb Bag Right" },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY, "Dodongos Cavern Deku Scrub Lobby" },
    { RC_DODONGOS_CAVERN_MQ_MAP_CHEST, "Dodongos Cavern MQ Map Chest" },
    { RC_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST, "Dodongos Cavern MQ Bomb Bag Chest" },
    { RC_DODONGOS_CAVERN_MQ_COMPASS_CHEST, "Dodongos Cavern MQ Compass Chest" },
    { RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST, "Dodongos Cavern MQ Larvae Room Chest" },
    { RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST, "Dodongos Cavern MQ Torch Puzzle Room Chest" },
    { RC_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST, "Dodongos Cavern MQ Under Grave Chest" },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR, "Dodongos Cavern Deku Scrub Lobby Rear" },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT, "Dodongos Cavern Deku Scrub Lobby Front" },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE, "Dodongos Cavern Deku Scrub Staircase" },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS,
      "Dodongos Cavern Deku Scrub Side Room Near Lower Lizalfos" },
    { RC_JABU_JABUS_BELLY_MAP_CHEST, "Jabu Jabus Belly Map Chest" },
    { RC_JABU_JABUS_BELLY_COMPASS_CHEST, "Jabu Jabus Belly Compass Chest" },
    { RC_JABU_JABUS_BELLY_BOOMERANG_CHEST, "Jabu Jabus Belly Boomerang Chest" },
    { RC_JABU_JABUS_BELLY_DEKU_SCRUB, "Jabu Jabus Belly Deku Scrub" },
    { RC_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST, "Jabu Jabus Belly MQ First Room Side Chest" },
    { RC_JABU_JABUS_BELLY_MQ_MAP_CHEST, "Jabu Jabus Belly MQ Map Chest" },
    { RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST, "Jabu Jabus Belly MQ Second Room Lower Chest" },
    { RC_JABU_JABUS_BELLY_MQ_COMPASS_CHEST, "Jabu Jabus Belly MQ Compass Chest" },
    { RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST, "Jabu Jabus Belly MQ Second Room Upper Chest" },
    { RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST, "Jabu Jabus Belly MQ Basement Near Switches Chest" },
    { RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST, "Jabu Jabus Belly MQ Basement Near Vines Chest" },
    { RC_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST, "Jabu Jabus Belly MQ Near Boss Chest" },
    { RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST, "Jabu Jabus Belly MQ Falling Like Like Room Chest" },
    { RC_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST, "Jabu Jabus Belly MQ Boomerang Room Small Chest" },
    { RC_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST, "Jabu Jabus Belly MQ Boomerang Chest" },
    { RC_FOREST_TEMPLE_FIRST_ROOM_CHEST, "Forest Temple First Room Chest" },
    { RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST, "Forest Temple First Stalfos Chest" },
    { RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST, "Forest Temple Raised Island Courtyard Chest" },
    { RC_FOREST_TEMPLE_MAP_CHEST, "Forest Temple Map Chest" },
    { RC_FOREST_TEMPLE_WELL_CHEST, "Forest Temple Well Chest" },
    { RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST, "Forest Temple Falling Ceiling Room Chest" },
    { RC_FOREST_TEMPLE_EYE_SWITCH_CHEST, "Forest Temple Eye Switch Chest" },
    { RC_FOREST_TEMPLE_BOSS_KEY_CHEST, "Forest Temple Boss Key Chest" },
    { RC_FOREST_TEMPLE_FLOORMASTER_CHEST, "Forest Temple Floormaster Chest" },
    { RC_FOREST_TEMPLE_BOW_CHEST, "Forest Temple Bow Chest" },
    { RC_FOREST_TEMPLE_RED_POE_CHEST, "Forest Temple Red Poe Chest" },
    { RC_FOREST_TEMPLE_BLUE_POE_CHEST, "Forest Temple Blue Poe Chest" },
    { RC_FOREST_TEMPLE_BASEMENT_CHEST, "Forest Temple Basement Chest" },
    { RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST, "Forest Temple MQ First Room Chest" },
    { RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST, "Forest Temple MQ Wolfos Chest" },
    { RC_FOREST_TEMPLE_MQ_BOW_CHEST, "Forest Temple MQ Bow Chest" },
    { RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST, "Forest Temple MQ Raised Island Courtyard Lower Chest" },
    { RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST, "Forest Temple MQ Raised Island Courtyard Upper Chest" },
    { RC_FOREST_TEMPLE_MQ_WELL_CHEST, "Forest Temple MQ Well Chest" },
    { RC_FOREST_TEMPLE_MQ_MAP_CHEST, "Forest Temple MQ Map Chest" },
    { RC_FOREST_TEMPLE_MQ_COMPASS_CHEST, "Forest Temple MQ Compass Chest" },
    { RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST, "Forest Temple MQ Falling Ceiling Room Chest" },
    { RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST, "Forest Temple MQ Basement Chest" },
    { RC_FOREST_TEMPLE_MQ_REDEAD_CHEST, "Forest Temple MQ Redead Chest" },
    { RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST, "Forest Temple MQ Boss Key Chest" },
    { RC_FIRE_TEMPLE_NEAR_BOSS_CHEST, "Fire Temple Near Boss Chest" },
    { RC_FIRE_TEMPLE_FLARE_DANCER_CHEST, "Fire Temple Flare Dancer Chest" },
    { RC_FIRE_TEMPLE_BOSS_KEY_CHEST, "Fire Temple Boss Key Chest" },
    { RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, "Fire Temple Big Lava Room Blocked Door Chest" },
    { RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST, "Fire Temple Big Lava Room Lower Open Door Chest" },
    { RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST, "Fire Temple Boulder Maze Lower Chest" },
    { RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST, "Fire Temple Boulder Maze Upper Chest" },
    { RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST, "Fire Temple Boulder Maze Side Room Chest" },
    { RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST, "Fire Temple Boulder Maze Shortcut Chest" },
    { RC_FIRE_TEMPLE_SCARECROW_CHEST, "Fire Temple Scarecrow Chest" },
    { RC_FIRE_TEMPLE_MAP_CHEST, "Fire Temple Map Chest" },
    { RC_FIRE_TEMPLE_COMPASS_CHEST, "Fire Temple Compass Chest" },
    { RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST, "Fire Temple Highest Goron Chest" },
    { RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST, "Fire Temple Megaton Hammer Chest" },
    { RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST, "Fire Temple MQ Near Boss Chest" },
    { RC_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST, "Fire Temple MQ Megaton Hammer Chest" },
    { RC_FIRE_TEMPLE_MQ_COMPASS_CHEST, "Fire Temple MQ Compass Chest" },
    { RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST, "Fire Temple MQ Lizalfos Maze Lower Chest" },
    { RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST, "Fire Temple MQ Lizalfos Maze Upper Chest" },
    { RC_FIRE_TEMPLE_MQ_CHEST_ON_FIRE, "Fire Temple MQ Chest on Fire" },
    { RC_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST, "Fire Temple MQ Map Room Side Chest" },
    { RC_FIRE_TEMPLE_MQ_MAP_CHEST, "Fire Temple MQ Map Chest" },
    { RC_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST, "Fire Temple MQ Boss Key Chest" },
    { RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, "Fire Temple MQ Big Lava Room Blocked Door Chest" },
    { RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST, "Fire Temple MQ Lizalfos Maze Side Room Chest" },
    { RC_FIRE_TEMPLE_MQ_FREESTANDING_KEY, "Fire Temple MQ Freestanding Key" },
    { RC_WATER_TEMPLE_MAP_CHEST, "Water Temple Map Chest" },
    { RC_WATER_TEMPLE_COMPASS_CHEST, "Water Temple Compass Chest" },
    { RC_WATER_TEMPLE_TORCHES_CHEST, "Water Temple Torches Chest" },
    { RC_WATER_TEMPLE_DRAGON_CHEST, "Water Temple Dragon Chest" },
    { RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST, "Water Temple Central Bow Target Chest" },
    { RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST, "Water Temple Central Pillar Chest" },
    { RC_WATER_TEMPLE_CRACKED_WALL_CHEST, "Water Temple Cracked Wall Chest" },
    { RC_WATER_TEMPLE_BOSS_KEY_CHEST, "Water Temple Boss Key Chest" },
    { RC_WATER_TEMPLE_LONGSHOT_CHEST, "Water Temple Longshot Chest" },
    { RC_WATER_TEMPLE_RIVER_CHEST, "Water Temple River Chest" },
    { RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST, "Water Temple MQ Central Pillar Chest" },
    { RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST, "Water Temple MQ Boss Key Chest" },
    { RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST, "Water Temple MQ Longshot Chest" },
    { RC_WATER_TEMPLE_MQ_COMPASS_CHEST, "Water Temple MQ Compass Chest" },
    { RC_WATER_TEMPLE_MQ_MAP_CHEST, "Water Temple MQ Map Chest" },
    { RC_WATER_TEMPLE_MQ_FREESTANDING_KEY, "Water Temple MQ Freestanding Key" },
    { RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, "Spirit Temple Silver Gauntlets Chest" },
    { RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST, "Spirit Temple Mirror Shield Chest" },
    { RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST, "Spirit Temple Child Bridge Chest" },
    { RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST, "Spirit Temple Child Early Torches Chest" },
    { RC_SPIRIT_TEMPLE_COMPASS_CHEST, "Spirit Temple Compass Chest" },
    { RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST, "Spirit Temple Early Adult Right Chest" },
    { RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST, "Spirit Temple First Mirror Left Chest" },
    { RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST, "Spirit Temple First Mirror Right Chest" },
    { RC_SPIRIT_TEMPLE_MAP_CHEST, "Spirit Temple Map Chest" },
    { RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST, "Spirit Temple Child Climb North Chest" },
    { RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST, "Spirit Temple Child Climb East Chest" },
    { RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST, "Spirit Temple Sun Block Room Chest" },
    { RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST, "Spirit Temple Statue Room Hand Chest" },
    { RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST, "Spirit Temple Statue Room Northeast Chest" },
    { RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST, "Spirit Temple Near Four Armos Chest" },
    { RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST, "Spirit Temple Hallway Right Invisible Chest" },
    { RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST, "Spirit Temple Hallway Left Invisible Chest" },
    { RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST, "Spirit Temple Boss Key Chest" },
    { RC_SPIRIT_TEMPLE_TOPMOST_CHEST, "Spirit Temple Topmost Chest" },
    { RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST, "Spirit Temple MQ Entrance Front Left Chest" },
    { RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST, "Spirit Temple MQ Entrance Back Right Chest" },
    { RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST, "Spirit Temple MQ Entrance Front Right Chest" },
    { RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST, "Spirit Temple MQ Entrance Back Left Chest" },
    { RC_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST, "Spirit Temple MQ Child Hammer Switch Chest" },
    { RC_SPIRIT_TEMPLE_MQ_MAP_CHEST, "Spirit Temple MQ Map Chest" },
    { RC_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST, "Spirit Temple MQ Map Room Enemy Chest" },
    { RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST, "Spirit Temple MQ Child Climb North Chest" },
    { RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST, "Spirit Temple MQ Child Climb South Chest" },
    { RC_SPIRIT_TEMPLE_MQ_COMPASS_CHEST, "Spirit Temple MQ Compass Chest" },
    { RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST, "Spirit Temple MQ Statue Room Lullaby Chest" },
    { RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST, "Spirit Temple MQ Statue Room Invisible Chest" },
    { RC_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST, "Spirit Temple MQ Silver Block Hallway Chest" },
    { RC_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST, "Spirit Temple MQ Sun Block Room Chest" },
    { RC_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST, "Spirit Temple MQ Symphony Room Chest" },
    { RC_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST, "Spirit Temple MQ Leever Room Chest" },
    { RC_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST, "Spirit Temple MQ Beamos Room Chest" },
    { RC_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST, "Spirit Temple MQ Chest Switch Chest" },
    { RC_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST, "Spirit Temple MQ Boss Key Chest" },
    { RC_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST, "Spirit Temple MQ Mirror Puzzle Invisible Chest" },
    { RC_SHADOW_TEMPLE_MAP_CHEST, "Shadow Temple Map Chest" },
    { RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST, "Shadow Temple Hover Boots Chest" },
    { RC_SHADOW_TEMPLE_COMPASS_CHEST, "Shadow Temple Compass Chest" },
    { RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST, "Shadow Temple Early Silver Rupee Chest" },
    { RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST, "Shadow Temple Invisible Blades Visible Chest" },
    { RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST, "Shadow Temple Invisible Blades Invisible Chest" },
    { RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST, "Shadow Temple Falling Spikes Lower Chest" },
    { RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST, "Shadow Temple Falling Spikes Upper Chest" },
    { RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST, "Shadow Temple Falling Spikes Switch Chest" },
    { RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST, "Shadow Temple Invisible Spikes Chest" },
    { RC_SHADOW_TEMPLE_WIND_HINT_CHEST, "Shadow Temple Wind Hint Chest" },
    { RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST, "Shadow Temple After Wind Enemy Chest" },
    { RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST, "Shadow Temple After Wind Hidden Chest" },
    { RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST, "Shadow Temple Spike Walls Left Chest" },
    { RC_SHADOW_TEMPLE_BOSS_KEY_CHEST, "Shadow Temple Boss Key Chest" },
    { RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST, "Shadow Temple Invisible Floormaster Chest" },
    { RC_SHADOW_TEMPLE_FREESTANDING_KEY, "Shadow Temple Freestanding Key" },
    { RC_SHADOW_TEMPLE_MQ_COMPASS_CHEST, "Shadow Temple MQ Compass Chest" },
    { RC_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST, "Shadow Temple MQ Hover Boots Chest" },
    { RC_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST, "Shadow Temple MQ Early Gibdos Chest" },
    { RC_SHADOW_TEMPLE_MQ_MAP_CHEST, "Shadow Temple MQ Map Chest" },
    { RC_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST, "Shadow Temple MQ Beamos Silver Rupees Chest" },
    { RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST, "Shadow Temple MQ Falling Spikes Switch Chest" },
    { RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST, "Shadow Temple MQ Falling Spikes Lower Chest" },
    { RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST, "Shadow Temple MQ Falling Spikes Upper Chest" },
    { RC_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST, "Shadow Temple MQ Invisible Spikes Chest" },
    { RC_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST, "Shadow Temple MQ Boss Key Chest" },
    { RC_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST, "Shadow Temple MQ Spike Walls Left Chest" },
    { RC_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST, "Shadow Temple MQ Stalfos Room Chest" },
    { RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST, "Shadow Temple MQ Invisible Blades Invisible Chest" },
    { RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST, "Shadow Temple MQ Invisible Blades Visible Chest" },
    { RC_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST, "Shadow Temple MQ Bomb Flower Chest" },
    { RC_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST, "Shadow Temple MQ Wind Hint Chest" },
    { RC_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST, "Shadow Temple MQ After Wind Hidden Chest" },
    { RC_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST, "Shadow Temple MQ After Wind Enemy Chest" },
    { RC_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST, "Shadow Temple MQ Near Ship Invisible Chest" },
    { RC_SHADOW_TEMPLE_MQ_FREESTANDING_KEY, "Shadow Temple MQ Freestanding Key" },
    { RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST, "Bottom of the Well Front Left Fake Wall Chest" },
    { RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST, "Bottom of the Well Front Center Bombable Chest" },
    { RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST, "Bottom of the Well Right Bottom Fake Wall Chest" },
    { RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST, "Bottom of the Well Compass Chest" },
    { RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST, "Bottom of the Well Center Skulltula Chest" },
    { RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST, "Bottom of the Well Back Left Bombable Chest" },
    { RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST, "Bottom of the Well Lens of Truth Chest" },
    { RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST, "Bottom of the Well Invisible Chest" },
    { RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST, "Bottom of the Well Underwater Front Chest" },
    { RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST, "Bottom of the Well Underwater Left Chest" },
    { RC_BOTTOM_OF_THE_WELL_MAP_CHEST, "Bottom of the Well Map Chest" },
    { RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST, "Bottom of the Well Fire Keese Chest" },
    { RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST, "Bottom of the Well Like Like Chest" },
    { RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY, "Bottom of the Well Freestanding Key" },
    { RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST, "Bottom of the Well MQ Map Chest" },
    { RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST, "Bottom of the Well MQ Lens of Truth Chest" },
    { RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST, "Bottom of the Well MQ Compass Chest" },
    { RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY, "Bottom of the Well MQ Dead Hand Freestanding Key" },
    { RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY,
      "Bottom of the Well MQ East Inner Room Freestanding Key" },
    { RC_ICE_CAVERN_MAP_CHEST, "Ice Cavern Map Chest" },
    { RC_ICE_CAVERN_COMPASS_CHEST, "Ice Cavern Compass Chest" },
    { RC_ICE_CAVERN_IRON_BOOTS_CHEST, "Ice Cavern Iron Boots Chest" },
    { RC_ICE_CAVERN_FREESTANDING_POH, "Ice Cavern Freestanding PoH" },
    { RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST, "Ice Cavern MQ Iron Boots Chest" },
    { RC_ICE_CAVERN_MQ_COMPASS_CHEST, "Ice Cavern MQ Compass Chest" },
    { RC_ICE_CAVERN_MQ_MAP_CHEST, "Ice Cavern MQ Map Chest" },
    { RC_ICE_CAVERN_MQ_FREESTANDING_POH, "Ice Cavern MQ Freestanding PoH" },
    { RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST, "Gerudo Training Grounds Lobby Left Chest" },
    { RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST, "Gerudo Training Grounds Lobby Right Chest" },
    { RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST, "Gerudo Training Grounds Stalfos Chest" },
    { RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST, "Gerudo Training Grounds Beamos Chest" },
    { RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST, "Gerudo Training Grounds Hidden Ceiling Chest" },
    { RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST, "Gerudo Training Grounds Maze Path First Chest" },
    { RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST, "Gerudo Training Grounds Maze Path Second Chest" },
    { RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST, "Gerudo Training Grounds Maze Path Third Chest" },
    { RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST, "Gerudo Training Grounds Maze Path Final Chest" },
    { RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST, "Gerudo Training Grounds Maze Right Central Chest" },
    { RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST, "Gerudo Training Grounds Maze Right Side Chest" },
    { RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST,
      "Gerudo Training Grounds Underwater Silver Rupee Chest" },
    { RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST, "Gerudo Training Grounds Hammer Room Clear Chest" },
    { RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST, "Gerudo Training Grounds Hammer Room Switch Chest" },
    { RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST, "Gerudo Training Grounds Eye Statue Chest" },
    { RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST, "Gerudo Training Grounds Near Scarecrow Chest" },
    { RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST, "Gerudo Training Grounds Before Heavy Block Chest" },
    { RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST, "Gerudo Training Grounds Heavy Block First Chest" },
    { RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST, "Gerudo Training Grounds Heavy Block Second Chest" },
    { RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST, "Gerudo Training Grounds Heavy Block Third Chest" },
    { RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST, "Gerudo Training Grounds Heavy Block Fourth Chest" },
    { RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY, "Gerudo Training Grounds Freestanding Key" },
    { RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST, "Gerudo Training Grounds MQ Lobby Right Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST, "Gerudo Training Grounds MQ Lobby Left Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST, "Gerudo Training Grounds MQ First Iron Knuckle Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST, "Gerudo Training Grounds MQ Before Heavy Block Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST, "Gerudo Training Grounds MQ Eye Statue Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST, "Gerudo Training Grounds MQ Flame Circle Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST, "Gerudo Training Grounds MQ Second Iron Knuckle Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST, "Gerudo Training Grounds MQ Dinolfos Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST, "Gerudo Training Grounds MQ Ice Arrows Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST, "Gerudo Training Grounds MQ Maze Right Central Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST, "Gerudo Training Grounds MQ Maze Path First Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST, "Gerudo Training Grounds MQ Maze Right Side Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST, "Gerudo Training Grounds MQ Maze Path Third Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST, "Gerudo Training Grounds MQ Maze Path Second Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST, "Gerudo Training Grounds MQ Hidden Ceiling Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST,
      "Gerudo Training Grounds MQ Underwater Silver Rupee Chest" },
    { RC_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST, "Gerudo Training Grounds MQ Heavy Block Chest" },
    { RC_GANONS_TOWER_BOSS_KEY_CHEST, "Ganon's Tower Boss Key Chest" },
    { RC_GANONS_CASTLE_FOREST_TRIAL_CHEST, "Ganon's Castle Forest Trial Chest" },
    { RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST, "Ganon's Castle Water Trial Left Chest" },
    { RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST, "Ganon's Castle Water Trial Right Chest" },
    { RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST, "Ganon's Castle Shadow Trial Front Chest" },
    { RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST, "Ganon's Castle Shadow Trial Golden Gauntlets Chest" },
    { RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST, "Ganon's Castle Spirit Trial Crystal Switch Chest" },
    { RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST, "Ganon's Castle Spirit Trial Invisible Chest" },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST, "Ganon's Castle Light Trial First Left Chest" },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST, "Ganon's Castle Light Trial Second Left Chest" },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST, "Ganon's Castle Light Trial Third Left Chest" },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST, "Ganon's Castle Light Trial First Right Chest" },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST, "Ganon's Castle Light Trial Second Right Chest" },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST, "Ganon's Castle Light Trial Third Right Chest" },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST, "Ganon's Castle Light Trial Invisible Enemies Chest" },
    { RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST, "Ganon's Castle Light Trial Lullaby Chest" },
    { RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT, "Ganon's Castle Deku Scrub Center-Left" },
    { RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT, "Ganon's Castle Deku Scrub Center-Right" },
    { RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT, "Ganon's Castle Deku Scrub Right" },
    { RC_GANONS_CASTLE_DEKU_SCRUB_LEFT, "Ganon's Castle Deku Scrub Left" },
    { RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST, "Ganon's Castle MQ Water Trial Chest" },
    { RC_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST, "Ganon's Castle MQ Forest Trial Eye Switch Chest" },
    { RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST,
      "Ganon's Castle MQ Forest Trial Frozen Eye Switch Chest" },
    { RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST, "Ganon's Castle MQ Light Trial Lullaby Chest" },
    { RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST, "Ganon's Castle MQ Shadow Trial Bomb Flower Chest" },
    { RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST, "Ganon's Castle MQ Shadow Trial Eye Switch Chest" },
    { RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST,
      "Ganon's Castle MQ Spirit Trial Golden Gauntlets Chest" },
    { RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST, "Ganon's Castle MQ Spirit Trial Sun Back Right Chest" },
    { RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST, "Ganon's Castle MQ Spirit Trial Sun Back Left Chest" },
    { RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST, "Ganon's Castle MQ Spirit Trial Sun Front Left Chest" },
    { RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST, "Ganon's Castle MQ Spirit Trial First Chest" },
    { RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST, "Ganon's Castle MQ Spirit Trial Invisible Chest" },
    { RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY, "Ganon's Castle MQ Forest Trial Freestanding Key" },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT, "Ganon's Castle MQ Deku Scrub Right" },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT, "Ganon's Castle MQ Deku Scrub Center-Left" },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER, "Ganon's Castle MQ Deku Scrub Center" },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT, "Ganon's Castle MQ Deku Scrub Center-Right" },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT, "Ganon's Castle MQ Deku Scrub Left" },
    { RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM, "Deku Tree GS Basement Back Room" },
    { RC_DEKU_TREE_GS_BASEMENT_GATE, "Deku Tree GS Basement Gate" },
    { RC_DEKU_TREE_GS_BASEMENT_VINES, "Deku Tree GS Basement Vines" },
    { RC_DEKU_TREE_GS_COMPASS_ROOM, "Deku Tree GS Compass Room" },
    { RC_DEKU_TREE_MQ_GS_LOBBY, "Deku Tree MQ GS Lobby" },
    { RC_DEKU_TREE_MQ_GS_COMPASS_ROOM, "Deku Tree MQ GS Compass Room" },
    { RC_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM, "Deku Tree MQ GS Basement Graves Room" },
    { RC_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM, "Deku Tree MQ GS Basement Back Room" },
    { RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS, "Dodongos Cavern GS Vines Above Stairs" },
    { RC_DODONGOS_CAVERN_GS_SCARECROW, "Dodongos Cavern GS Scarecrow" },
    { RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS, "Dodongos Cavern GS Alcove Above Stairs" },
    { RC_DODONGOS_CAVERN_GS_BACK_ROOM, "Dodongos Cavern GS Back Room" },
    { RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS, "Dodongos Cavern GS Side Room Near Lower Lizalfos" },
    { RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM, "Dodongos Cavern MQ GS Scrub Room" },
    { RC_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM, "Dodongos Cavern MQ GS Song of Time Block Room" },
    { RC_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM, "Dodongos Cavern MQ GS Lizalfos Room" },
    { RC_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM, "Dodongos Cavern MQ GS Larvae Room" },
    { RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA, "Dodongos Cavern MQ GS Back Room" },
    { RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER, "Jabu Jabus Belly GS Lobby Basement Lower" },
    { RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER, "Jabu Jabus Belly GS Lobby Basement Upper" },
    { RC_JABU_JABUS_BELLY_GS_NEAR_BOSS, "Jabu Jabus Belly GS Near Boss" },
    { RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM, "Jabu Jabus Belly GS Water Switch Room" },
    { RC_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM, "Jabu Jabus Belly MQ GS Tail Parasan Room" },
    { RC_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM, "Jabu Jabus Belly MQ GS Invisible Enemies Room" },
    { RC_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM, "Jabu Jabus Belly MQ GS Boomerang Chest Room" },
    { RC_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS, "Jabu Jabus Belly MQ GS Near Boss" },
    { RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD, "Forest Temple GS Raised Island Courtyard" },
    { RC_FOREST_TEMPLE_GS_FIRST_ROOM, "Forest Temple GS First Room" },
    { RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD, "Forest Temple GS Level Island Courtyard" },
    { RC_FOREST_TEMPLE_GS_LOBBY, "Forest Temple GS Lobby" },
    { RC_FOREST_TEMPLE_GS_BASEMENT, "Forest Temple GS Basement" },
    { RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY, "Forest Temple MQ GS First Hallway" },
    { RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM, "Forest Temple MQ GS Block Push Room" },
    { RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD, "Forest Temple MQ GS Raised Island Courtyard" },
    { RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD, "Forest Temple MQ GS Level Island Courtyard" },
    { RC_FOREST_TEMPLE_MQ_GS_WELL, "Forest Temple MQ GS Well" },
    { RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM, "Fire Temple GS Song of Time Room" },
    { RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP, "Fire Temple GS Boss Key Loop" },
    { RC_FIRE_TEMPLE_GS_BOULDER_MAZE, "Fire Temple GS Boulder Maze" },
    { RC_FIRE_TEMPLE_GS_SCARECROW_TOP, "Fire Temple GS Scarecrow Top" },
    { RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB, "Fire Temple GS Scarecrow Climb" },
    { RC_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE, "Fire Temple MQ GS Above Fire Wall Maze" },
    { RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER, "Fire Temple MQ GS Fire Wall Maze Center" },
    { RC_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR, "Fire Temple MQ GS Big Lava Room Open Door" },
    { RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM, "Fire Temple MQ GS Fire Wall Maze Side Room" },
    { RC_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE, "Fire Temple MQ GS Skull on Fire" },
    { RC_WATER_TEMPLE_GS_BEHIND_GATE, "Water Temple GS Behind Gate" },
    { RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM, "Water Temple GS Falling Platform Room" },
    { RC_WATER_TEMPLE_GS_CENTRAL_PILLAR, "Water Temple GS Central Pillar" },
    { RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST, "Water Temple GS Near Boss Key Chest" },
    { RC_WATER_TEMPLE_GS_RIVER, "Water Temple GS River" },
    { RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH, "Water Temple MQ GS Before Upper Water Switch" },
    { RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA, "Water Temple MQ GS Freestanding Key Area" },
    { RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY, "Water Temple MQ GS Lizalfos Hallway" },
    { RC_WATER_TEMPLE_MQ_GS_RIVER, "Water Temple MQ GS River" },
    { RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH, "Water Temple MQ GS Triple Wall Torch" },
    { RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM, "Spirit Temple GS Hall After Sun Block Room" },
    { RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM, "Spirit Temple GS Boulder Room" },
    { RC_SPIRIT_TEMPLE_GS_LOBBY, "Spirit Temple GS Lobby" },
    { RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM, "Spirit Temple GS Sun on Floor Room" },
    { RC_SPIRIT_TEMPLE_GS_METAL_FENCE, "Spirit Temple GS Metal Fence" },
    { RC_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM, "Spirit Temple MQ GS Symphony Room" },
    { RC_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM, "Spirit Temple MQ GS Leever Room" },
    { RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST, "Spirit Temple MQ GS Nine Thrones Room West" },
    { RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH, "Spirit Temple MQ GS Nine Thrones Room North" },
    { RC_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM, "Spirit Temple MQ GS Sun Block Room" },
    { RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT, "Shadow Temple GS Single Giant Pot" },
    { RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM, "Shadow Temple GS Falling Spikes Room" },
    { RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT, "Shadow Temple GS Triple Giant Pot" },
    { RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM, "Shadow Temple GS Like Like Room" },
    { RC_SHADOW_TEMPLE_GS_NEAR_SHIP, "Shadow Temple GS Near Ship" },
    { RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM, "Shadow Temple MQ GS Falling Spikes Room" },
    { RC_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM, "Shadow Temple MQ GS Wind Hint Room" },
    { RC_SHADOW_TEMPLE_MQ_GS_AFTER_WIND, "Shadow Temple MQ GS After Wind" },
    { RC_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP, "Shadow Temple MQ GS After Ship" },
    { RC_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS, "Shadow Temple MQ GS Near Boss" },
    { RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE, "Bottom of the Well GS Like Like Cage" },
    { RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM, "Bottom of the Well GS East Inner Room" },
    { RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM, "Bottom of the Well GS West Inner Room" },
    { RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT, "Bottom of the Well MQ GS Basement" },
    { RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM, "Bottom of the Well MQ GS Coffin Room" },
    { RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM, "Bottom of the Well MQ GS West Inner Room" },
    { RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM, "Ice Cavern GS Push Block Room" },
    { RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM, "Ice Cavern GS Spinning Scythe Room" },
    { RC_ICE_CAVERN_GS_HEART_PIECE_ROOM, "Ice Cavern GS Heart Piece Room" },
    { RC_ICE_CAVERN_MQ_GS_SCARECROW, "Ice Cavern MQ GS Scarecrow" },
    { RC_ICE_CAVERN_MQ_GS_ICE_BLOCK, "Ice Cavern MQ GS Ice Block" },
    { RC_ICE_CAVERN_MQ_GS_RED_ICE, "Ice Cavern MQ GS Red Ice" },
    { RC_KF_GS_BEAN_PATCH, "KF GS Bean Patch" },
    { RC_KF_GS_KNOW_IT_ALL_HOUSE, "KF GS Know It All House" },
    { RC_KF_GS_HOUSE_OF_TWINS, "KF GS House of Twins" },
    { RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE, "LW GS Bean Patch Near Bridge" },
    { RC_LW_GS_BEAN_PATCH_NEAR_THEATER, "LW GS Bean Patch Near Theater" },
    { RC_LW_GS_ABOVE_THEATER, "LW GS Above Theater" },
    { RC_SFM_GS, "SFM GS" },
    { RC_HF_GS_COW_GROTTO, "HF GS Cow Grotto" },
    { RC_HF_GS_NEAR_KAK_GROTTO, "HF GS Near Kak Grotto" },
    { RC_LH_GS_BEAN_PATCH, "LH GS Bean Patch" },
    { RC_LH_GS_SMALL_ISLAND, "LH GS Small Island" },
    { RC_LH_GS_LAB_WALL, "LH GS Lab Wall" },
    { RC_LH_GS_LAB_CRATE, "LH GS Lab Crate" },
    { RC_LH_GS_TREE, "LH GS Tree" },
    { RC_GV_GS_BEAN_PATCH, "GV GS Bean Patch" },
    { RC_GV_GS_SMALL_BRIDGE, "GV GS Small Bridge" },
    { RC_GV_GS_PILLAR, "GV GS Pillar" },
    { RC_GV_GS_BEHIND_TENT, "GV GS Behind Tent" },
    { RC_GF_GS_ARCHERY_RANGE, "GF GS Archery Range" },
    { RC_GF_GS_TOP_FLOOR, "GF GS Top Floor" },
    { RC_WASTELAND_GS, "Wasteland GS" },
    { RC_COLOSSUS_GS_BEAN_PATCH, "Colossus GS Bean Patch" },
    { RC_COLOSSUS_GS_HILL, "Colossus GS Hill" },
    { RC_COLOSSUS_GS_TREE, "Colossus GS Tree" },
    { RC_OGC_GS, "OGC GS" },
    { RC_HC_GS_STORMS_GROTTO, "HC GS Storms Grotto" },
    { RC_HC_GS_TREE, "HC GS Tree" },
    { RC_MARKET_GS_GUARD_HOUSE, "Market GS Guard House" },
    { RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION, "Kak GS House Under Construction" },
    { RC_KAK_GS_SKULLTULA_HOUSE, "Kak GS Skulltula House" },
    { RC_KAK_GS_GUARDS_HOUSE, "Kak GS Guards House" },
    { RC_KAK_GS_TREE, "Kak GS Tree" },
    { RC_KAK_GS_WATCHTOWER, "Kak GS Watchtower" },
    { RC_KAK_GS_ABOVE_IMPAS_HOUSE, "Kak GS Above Impas House" },
    { RC_GRAVEYARD_GS_WALL, "Graveyard GS Wall" },
    { RC_GRAVEYARD_GS_BEAN_PATCH, "Graveyard GS Bean Patch" },
    { RC_DMC_GS_BEAN_PATCH, "DMC GS Bean Patch" },
    { RC_DMC_GS_CRATE, "DMC GS Crate" },
    { RC_DMT_GS_BEAN_PATCH, "DMT GS Bean Patch" },
    { RC_DMT_GS_NEAR_KAK, "DMT GS Near Kak" },
    { RC_DMT_GS_ABOVE_DODONGOS_CAVERN, "DMT GS Above Dodongos Cavern" },
    { RC_DMT_GS_FALLING_ROCKS_PATH, "DMT GS Falling Rocks Path" },
    { RC_GC_GS_CENTER_PLATFORM, "GC GS Center Platform" },
    { RC_GC_GS_BOULDER_MAZE, "GC GS Boulder Maze" },
    { RC_ZR_GS_LADDER, "ZR GS Ladder" },
    { RC_ZR_GS_TREE, "ZR GS Tree" },
    { RC_ZR_GS_ABOVE_BRIDGE, "ZR GS Above Bridge" },
    { RC_ZR_GS_NEAR_RAISED_GROTTOS, "ZR GS Near Raised Grottos" },
    { RC_ZD_GS_FROZEN_WATERFALL, "ZD GS Frozen Waterfall" },
    { RC_ZF_GS_ABOVE_THE_LOG, "ZF GS Above The Log" },
    { RC_ZF_GS_HIDDEN_CAVE, "ZF GS Hidden Cave" },
    { RC_ZF_GS_TREE, "ZF GS Tree" },
    { RC_LLR_GS_BACK_WALL, "LLR GS Back Wall" },
    { RC_LLR_GS_RAIN_SHED, "LLR GS Rain Shed" },
    { RC_LLR_GS_HOUSE_WINDOW, "LLR GS House Window" },
    { RC_LLR_GS_TREE, "LLR GS Tree" },
    { RC_LINKS_POCKET, "Link's Pocket" },
    { RC_QUEEN_GOHMA, "Queen Gohma" },
    { RC_KING_DODONGO, "King Dodongo" },
    { RC_BARINADE, "Barinade" },
    { RC_PHANTOM_GANON, "Phantom Ganon" },
    { RC_VOLVAGIA, "Volvagia" },
    { RC_MORPHA, "Morpha" },
    { RC_TWINROVA, "Twinrova" },
    { RC_BONGO_BONGO, "Bongo Bongo" },
    { RC_UNKNOWN_CHECK, "Ganon" },
    { RC_DEKU_TREE_QUEEN_GOHMA_HEART, "Deku Tree Queen Gohma Heart Container" },
    { RC_DODONGOS_CAVERN_KING_DODONGO_HEART, "Dodongos Cavern King Dodongo Heart Container" },
    { RC_JABU_JABUS_BELLY_BARINADE_HEART, "Jabu Jabus Belly Barinade Heart Container" },
    { RC_FOREST_TEMPLE_PHANTOM_GANON_HEART, "Forest Temple Phantom Ganon Heart Container" },
    { RC_FIRE_TEMPLE_VOLVAGIA_HEART, "Fire Temple Volvagia Heart Container" },
    { RC_WATER_TEMPLE_MORPHA_HEART, "Water Temple Morpha Heart Container" },
    { RC_SPIRIT_TEMPLE_TWINROVA_HEART, "Spirit Temple Twinrova Heart Container" },
    { RC_SHADOW_TEMPLE_BONGO_BONGO_HEART, "Shadow Temple Bongo Bongo Heart Container" },
    { RC_TOT_LIGHT_ARROWS_CUTSCENE, "ToT Light Arrow Cutscene" },
    { RC_LW_GIFT_FROM_SARIA, "LW Gift From Saria" },
    { RC_ZF_GREAT_FAIRY_REWARD, "ZF Great Fairy Reward" },
    { RC_HC_GREAT_FAIRY_REWARD, "HC Great Fairy Reward" },
    { RC_COLOSSUS_GREAT_FAIRY_REWARD, "Colossus Great Fairy Reward" },
    { RC_DMT_GREAT_FAIRY_REWARD, "DMT Great Fairy Reward" },
    { RC_DMC_GREAT_FAIRY_REWARD, "DMC Great Fairy Reward" },
    { RC_OGC_GREAT_FAIRY_REWARD, "OGC Great Fairy Reward" },
    { RC_SHEIK_IN_FOREST, "Sheik in Forest" },
    { RC_SHEIK_IN_CRATER, "Sheik in Crater" },
    { RC_SHEIK_IN_ICE_CAVERN, "Sheik in Ice Cavern" },
    { RC_SHEIK_AT_COLOSSUS, "Sheik at Colossus" },
    { RC_SHEIK_IN_KAKARIKO, "Sheik in Kakariko" },
    { RC_SHEIK_AT_TEMPLE, "Sheik at Temple" },
    { RC_SONG_FROM_IMPA, "Song from Impa" },
    { RC_SONG_FROM_MALON, "Song from Malon" },
    { RC_SONG_FROM_SARIA, "Song from Saria" },
    { RC_SONG_FROM_ROYAL_FAMILYS_TOMB, "Song from Composers Grave" },
    { RC_SONG_FROM_OCARINA_OF_TIME, "Song from Ocarina of Time" },
    { RC_SONG_FROM_WINDMILL, "Song from Windmill" },
    { RC_KF_LINKS_HOUSE_COW, "KF Links House Cow" },
    { RC_HF_COW_GROTTO_COW, "HF Cow Grotto Cow" },
    { RC_LLR_STABLES_LEFT_COW, "LLR Stables Left Cow" },
    { RC_LLR_STABLES_RIGHT_COW, "LLR Stables Right Cow" },
    { RC_LLR_TOWER_LEFT_COW, "LLR Tower Left Cow" },
    { RC_LLR_TOWER_RIGHT_COW, "LLR Tower Right Cow" },
    { RC_KAK_IMPAS_HOUSE_COW, "Kak Impas House Cow" },
    { RC_DMT_COW_GROTTO_COW, "DMT Cow Grotto Cow" },
    { RC_GV_COW, "GV Cow" },
    { RC_JABU_JABUS_BELLY_MQ_COW, "Jabu Jabus Belly MQ Cow" },
    { RC_KF_SHOP_ITEM_1, "KF Shop Item 1" },
    { RC_KF_SHOP_ITEM_2, "KF Shop Item 2" },
    { RC_KF_SHOP_ITEM_3, "KF Shop Item 3" },
    { RC_KF_SHOP_ITEM_4, "KF Shop Item 4" },
    { RC_KF_SHOP_ITEM_5, "KF Shop Item 5" },
    { RC_KF_SHOP_ITEM_6, "KF Shop Item 6" },
    { RC_KF_SHOP_ITEM_7, "KF Shop Item 7" },
    { RC_KF_SHOP_ITEM_8, "KF Shop Item 8" },
    { RC_KAK_POTION_SHOP_ITEM_1, "Kak Potion Shop Item 1" },
    { RC_KAK_POTION_SHOP_ITEM_2, "Kak Potion Shop Item 2" },
    { RC_KAK_POTION_SHOP_ITEM_3, "Kak Potion Shop Item 3" },
    { RC_KAK_POTION_SHOP_ITEM_4, "Kak Potion Shop Item 4" },
    { RC_KAK_POTION_SHOP_ITEM_5, "Kak Potion Shop Item 5" },
    { RC_KAK_POTION_SHOP_ITEM_6, "Kak Potion Shop Item 6" },
    { RC_KAK_POTION_SHOP_ITEM_7, "Kak Potion Shop Item 7" },
    { RC_KAK_POTION_SHOP_ITEM_8, "Kak Potion Shop Item 8" },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_1, "MK Bombchu Shop Item 1" },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_2, "MK Bombchu Shop Item 2" },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_3, "MK Bombchu Shop Item 3" },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_4, "MK Bombchu Shop Item 4" },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_5, "MK Bombchu Shop Item 5" },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_6, "MK Bombchu Shop Item 6" },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_7, "MK Bombchu Shop Item 7" },
    { RC_MARKET_BOMBCHU_SHOP_ITEM_8, "MK Bombchu Shop Item 8" },
    { RC_MARKET_POTION_SHOP_ITEM_1, "MK Potion Shop Item 1" },
    { RC_MARKET_POTION_SHOP_ITEM_2, "MK Potion Shop Item 2" },
    { RC_MARKET_POTION_SHOP_ITEM_3, "MK Potion Shop Item 3" },
    { RC_MARKET_POTION_SHOP_ITEM_4, "MK Potion Shop Item 4" },
    { RC_MARKET_POTION_SHOP_ITEM_5, "MK Potion Shop Item 5" },
    { RC_MARKET_POTION_SHOP_ITEM_6, "MK Potion Shop Item 6" },
    { RC_MARKET_POTION_SHOP_ITEM_7, "MK Potion Shop Item 7" },
    { RC_MARKET_POTION_SHOP_ITEM_8, "MK Potion Shop Item 8" },
    { RC_MARKET_BAZAAR_ITEM_1, "MK Bazaar Item 1" },
    { RC_MARKET_BAZAAR_ITEM_2, "MK Bazaar Item 2" },
    { RC_MARKET_BAZAAR_ITEM_3, "MK Bazaar Item 3" },
    { RC_MARKET_BAZAAR_ITEM_4, "MK Bazaar Item 4" },
    { RC_MARKET_BAZAAR_ITEM_5, "MK Bazaar Item 5" },
    { RC_MARKET_BAZAAR_ITEM_6, "MK Bazaar Item 6" },
    { RC_MARKET_BAZAAR_ITEM_7, "MK Bazaar Item 7" },
    { RC_MARKET_BAZAAR_ITEM_8, "MK Bazaar Item 8" },
    { RC_KAK_BAZAAR_ITEM_1, "Kak Bazaar Item 1" },
    { RC_KAK_BAZAAR_ITEM_2, "Kak Bazaar Item 2" },
    { RC_KAK_BAZAAR_ITEM_3, "Kak Bazaar Item 3" },
    { RC_KAK_BAZAAR_ITEM_4, "Kak Bazaar Item 4" },
    { RC_KAK_BAZAAR_ITEM_5, "Kak Bazaar Item 5" },
    { RC_KAK_BAZAAR_ITEM_6, "Kak Bazaar Item 6" },
    { RC_KAK_BAZAAR_ITEM_7, "Kak Bazaar Item 7" },
    { RC_KAK_BAZAAR_ITEM_8, "Kak Bazaar Item 8" },
    { RC_ZD_SHOP_ITEM_1, "ZD Shop Item 1" },
    { RC_ZD_SHOP_ITEM_2, "ZD Shop Item 2" },
    { RC_ZD_SHOP_ITEM_3, "ZD Shop Item 3" },
    { RC_ZD_SHOP_ITEM_4, "ZD Shop Item 4" },
    { RC_ZD_SHOP_ITEM_5, "ZD Shop Item 5" },
    { RC_ZD_SHOP_ITEM_6, "ZD Shop Item 6" },
    { RC_ZD_SHOP_ITEM_7, "ZD Shop Item 7" },
    { RC_ZD_SHOP_ITEM_8, "ZD Shop Item 8" },
    { RC_GC_SHOP_ITEM_1, "GC Shop Item 1" },
    { RC_GC_SHOP_ITEM_2, "GC Shop Item 2" },
    { RC_GC_SHOP_ITEM_3, "GC Shop Item 3" },
    { RC_GC_SHOP_ITEM_4, "GC Shop Item 4" },
    { RC_GC_SHOP_ITEM_5, "GC Shop Item 5" },
    { RC_GC_SHOP_ITEM_6, "GC Shop Item 6" },
    { RC_GC_SHOP_ITEM_7, "GC Shop Item 7" },
    { RC_GC_SHOP_ITEM_8, "GC Shop Item 8" },
    { RC_COLOSSUS_GOSSIP_STONE, "Colossus Gossip Stone" },
    { RC_DMC_GOSSIP_STONE, "DMC Gossip Stone" },
    { RC_DMC_UPPER_GROTTO_GOSSIP_STONE, "DMC Upper Grotto Gossip Stone" },
    { RC_DMT_GOSSIP_STONE, "DMT Gossip Stone" },
    { RC_DMT_STORMS_GROTTO_GOSSIP_STONE, "DMT Storms Grotto Gossip Stone" },
    { RC_DODONGOS_CAVERN_GOSSIP_STONE, "Dodongo's Cavern Gossip Stone" },
    { RC_FAIRY_GOSSIP_STONE, "Fairy Gossip Stone" },
    { RC_GC_MAZE_GOSSIP_STONE, "GC Maze Gossip Stone" },
    { RC_GC_MEDIGORON_GOSSIP_STONE, "GC Medigoron Gossip Stone" },
    { RC_GV_GOSSIP_STONE, "GV Gossip Stone" },
    { RC_GY_GOSSIP_STONE, "GY Gossip Stone" },
    { RC_HC_MALON_GOSSIP_STONE, "HC Malon Gossip Stone" },
    { RC_HC_ROCK_WALL_GOSSIP_STONE, "HC Rock Wall Gossip Stone" },
    { RC_HC_STORMS_GROTTO_GOSSIP_STONE, "HC Storms Grotto Gossip Stone" },
    { RC_HF_COW_GROTTO_GOSSIP_STONE, "HF Cow Grotto Gossip Stone" },
    { RC_HF_NEAR_MARKET_GOSSIP_STONE, "HF Near Market Gossip Stone" },
    { RC_HF_OPEN_GROTTO_GOSSIP_STONE, "HF Open Grotto Gossip Stone" },
    { RC_HF_SOUTHEAST_GOSSIP_STONE, "HF Southeast Gossip Stone" },
    { RC_JABU_GOSSIP_STONE, "Jabu Gossip Stone" },
    { RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE, "KF Deku Tree Left Gossip Stone" },
    { RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE, "KF Deku Tree Right Gossip Stone" },
    { RC_KF_GOSSIP_STONE, "KF Gossip Stone" },
    { RC_KF_STORMS_GOSSIP_STONE, "KF Storms Gossip Stone" },
    { RC_KAK_OPEN_GROTTO_GOSSIP_STONE, "Kak Open Grotto Gossip Stone" },
    { RC_LH_LAB_GOSSIP_STONE, "LH Lab Gossip Stone" },
    { RC_LH_SOUTHEAST_GOSSIP_STONE, "LH Southeast Gossip Stone" },
    { RC_LH_SOUTHWEST_GOSSIP_STONE, "LH Southwest Gossip Stone" },
    { RC_LW_GOSSIP_STONE, "LW Gossip Stone" },
    { RC_LW_NEAR_SHORTCUTS_GOSSIP_STONE, "LW Near Shortcuts Gossip Stone" },
    { RC_SFM_MAZE_LOWER_GOSSIP_STONE, "SFM Maze Lower Gossip Stone" },
    { RC_SFM_MAZE_UPPER_GOSSIP_STONE, "SFM Maze Upper Gossip Stone" },
    { RC_SFM_SARIA_GOSSIP_STONE, "SFM Saria Gossip Stone" },
    { RC_TOT_LEFT_CENTER_GOSSIP_STONE, "ToT Left Center Gossip Stone" },
    { RC_TOT_LEFT_GOSSIP_STONE, "ToT Left Gossip Stone" },
    { RC_TOT_RIGHT_CENTER_GOSSIP_STONE, "ToT Right Center Gossip Stone" },
    { RC_TOT_RIGHT_GOSSIP_STONE, "ToT Right Gossip Stone" },
    { RC_ZD_GOSSIP_STONE, "ZD Gossip Stone" },
    { RC_ZR_NEAR_DOMAIN_GOSSIP_STONE, "ZR Near Domain Gossip Stone" },
    { RC_ZR_NEAR_GROTTOS_GOSSIP_STONE, "ZR Near Grottos Gossip Stone" },
    { RC_ZR_OPEN_GROTTO_GOSSIP_STONE, "ZR Open Grotto Gossip Stone" }
};

std::unordered_map<RandomizerGet, std::string> GetEnumToName = {
    { RG_NONE, "No Item" },
    { RG_NONE, "Rien" },
    { RG_KOKIRI_SWORD, "Kokiri Sword" },
    { RG_KOKIRI_SWORD, "�p�e Kokiri" },
    { RG_GIANTS_KNIFE, "Giant's Knife" },
    { RG_GIANTS_KNIFE, "Lame des G�ants" },
    { RG_BIGGORON_SWORD, "Biggoron's Sword" },
    { RG_BIGGORON_SWORD, "�p�e de Biggoron" },
    { RG_DEKU_SHIELD, "Deku Shield" },
    { RG_DEKU_SHIELD, "Bouclier Mojo" },
    { RG_HYLIAN_SHIELD, "Hylian Shield" },
    { RG_HYLIAN_SHIELD, "Bouclier Hylien" },
    { RG_MIRROR_SHIELD, "Mirror Shield" },
    { RG_MIRROR_SHIELD, "Bouclier Miroir" },
    { RG_GORON_TUNIC, "Goron Tunic" },
    { RG_GORON_TUNIC, "Tunique Goron" },
    { RG_ZORA_TUNIC, "Zora Tunic" },
    { RG_ZORA_TUNIC, "Tunique Zora" },
    { RG_IRON_BOOTS, "Iron Boots" },
    { RG_IRON_BOOTS, "Bottes de plomb" },
    { RG_HOVER_BOOTS, "Hover Boots" },
    { RG_HOVER_BOOTS, "Bottes des airs" },
    { RG_BOOMERANG, "Boomerang" },
    { RG_LENS_OF_TRUTH, "Lens of Truth" },
    { RG_LENS_OF_TRUTH, "Monocle de V�rit�" },
    { RG_MEGATON_HAMMER, "Megaton Hammer" },
    { RG_MEGATON_HAMMER, "Masse des Titans" },
    { RG_STONE_OF_AGONY, "Stone of Agony" },
    { RG_STONE_OF_AGONY, "Pierre de Souffrance" },
    { RG_DINS_FIRE, "Din's Fire" },
    { RG_DINS_FIRE, "Feu de Din" },
    { RG_FARORES_WIND, "Farore's Wind" },
    { RG_FARORES_WIND, "Vent de Farore" },
    { RG_NAYRUS_LOVE, "Nayru's Love" },
    { RG_NAYRUS_LOVE, "Amour de Nayru" },
    { RG_FIRE_ARROWS, "Fire Arrow" },
    { RG_FIRE_ARROWS, "Fl�che de Feu" },
    { RG_ICE_ARROWS, "Ice Arrow" },
    { RG_ICE_ARROWS, "Fl�che de Glace" },
    { RG_LIGHT_ARROWS, "Light Arrow" },
    { RG_LIGHT_ARROWS, "Fl�che de Lumi�re" },
    { RG_GERUDO_MEMBERSHIP_CARD, "Gerudo Membership Card" },
    { RG_GERUDO_MEMBERSHIP_CARD, "Carte Gerudo" },
    { RG_MAGIC_BEAN, "Magic Bean" },
    { RG_MAGIC_BEAN, "Haricots Magiques" },
    { RG_MAGIC_BEAN_PACK, "Magic Bean Pack" },
    { RG_MAGIC_BEAN_PACK, "Paquet de Haricots Magiques" },
    { RG_DOUBLE_DEFENSE, "Double Defense" },
    { RG_DOUBLE_DEFENSE, "Double D�fence" },
    { RG_WEIRD_EGG, "Weird Egg" },
    { RG_WEIRD_EGG, "Oeuf Curieux" },
    { RG_ZELDAS_LETTER, "Zelda's Letter" },
    { RG_ZELDAS_LETTER, "Lettre de Zelda" },
    { RG_POCKET_EGG, "Pocket Egg" },
    { RG_POCKET_EGG, "Oeuf de poche" },
    { RG_COJIRO, "Cojiro" },
    { RG_COJIRO, "P'tit Poulet" },
    { RG_ODD_MUSHROOM, "Odd Mushroom" },
    { RG_ODD_MUSHROOM, "Champignon Suspect" },
    { RG_ODD_POTION, "Odd Potion" },
    { RG_ODD_POTION, "Mixture Suspecte " },
    { RG_POACHERS_SAW, "Poacher's Saw" },
    { RG_POACHERS_SAW, "Scie du Chasseur" },
    { RG_BROKEN_SWORD, "Broken Goron's Sword" },
    { RG_BROKEN_SWORD, "�p�e Bris�e de Goron" },
    { RG_PRESCRIPTION, "Prescription" },
    { RG_PRESCRIPTION, "Ordonnance" },
    { RG_EYEBALL_FROG, "Eyeball Frog" },
    { RG_EYEBALL_FROG, "Crapaud-qui-louche" },
    { RG_EYEDROPS, "World's Finest Eyedrops" },
    { RG_EYEDROPS, "Super Gouttes" },
    { RG_CLAIM_CHECK, "Claim Check" },
    { RG_CLAIM_CHECK, "Certificat" },
    { RG_GOLD_SKULLTULA_TOKEN, "Gold Skulltula Token" },
    { RG_GOLD_SKULLTULA_TOKEN, "Symbole de Skulltula d'Or" },
    { RG_PROGRESSIVE_HOOKSHOT, "Progressive Hookshot" },
    { RG_PROGRESSIVE_HOOKSHOT, "Grappin (prog.)" },
    { RG_PROGRESSIVE_STRENGTH, "Progressive Strength Upgrade" },
    { RG_PROGRESSIVE_STRENGTH, "Am�lioration de Force (prog.)" },
    { RG_PROGRESSIVE_BOMB_BAG, "Progressive Bomb Bag" },
    { RG_PROGRESSIVE_BOMB_BAG, "Sac de Bombes (prog.)" },
    { RG_PROGRESSIVE_BOW, "Progressive Bow" },
    { RG_PROGRESSIVE_BOW, "Arc (prog.)" },
    { RG_PROGRESSIVE_SLINGSHOT, "Progressive Slingshot" },
    { RG_PROGRESSIVE_SLINGSHOT, "Lance-Pierre (prog.)" },
    { RG_PROGRESSIVE_WALLET, "Progressive Wallet" },
    { RG_PROGRESSIVE_WALLET, "Bourse (prog.)" },
    { RG_PROGRESSIVE_SCALE, "Progressive Scale" },
    { RG_PROGRESSIVE_SCALE, "�caille (prog.)" },
    { RG_PROGRESSIVE_NUT_UPGRADE, "Progressive Nut Capacity" },
    { RG_PROGRESSIVE_NUT_UPGRADE, "Capacit� de Noix (prog.)" },
    { RG_PROGRESSIVE_STICK_UPGRADE, "Progressive Stick Capacity" },
    { RG_PROGRESSIVE_STICK_UPGRADE, "Capacit� de B�tons (prog.)" },
    { RG_PROGRESSIVE_BOMBCHUS, "Progressive Bombchu" },
    { RG_PROGRESSIVE_BOMBCHUS, "Missiles (prog.)" },
    { RG_PROGRESSIVE_MAGIC_METER, "Progressive Magic Meter" },
    { RG_PROGRESSIVE_MAGIC_METER, "Jauge de Magie (prog.)" },
    { RG_PROGRESSIVE_OCARINA, "Progressive Ocarina" },
    { RG_PROGRESSIVE_OCARINA, "Ocarina (prog.)" },
    { RG_PROGRESSIVE_GORONSWORD, "Progressive Goron Sword" },
    { RG_PROGRESSIVE_GORONSWORD, "�p�e Goron (prog.)" },
    { RG_EMPTY_BOTTLE, "Empty Bottle" },
    { RG_EMPTY_BOTTLE, "Bouteille Vide" },
    { RG_BOTTLE_WITH_MILK, "Bottle with Milk" },
    { RG_BOTTLE_WITH_MILK, "Bouteille avec du Lait" },
    { RG_BOTTLE_WITH_RED_POTION, "Bottle with Red Potion" },
    { RG_BOTTLE_WITH_RED_POTION, "Bouteille avec une Potion Rouge" },
    { RG_BOTTLE_WITH_GREEN_POTION, "Bottle with Green Potion" },
    { RG_BOTTLE_WITH_GREEN_POTION, "Bouteille avec une Potion Verte" },
    { RG_BOTTLE_WITH_BLUE_POTION, "Bottle with Blue Potion" },
    { RG_BOTTLE_WITH_BLUE_POTION, "Bouteille avec une Potion Bleue" },
    { RG_BOTTLE_WITH_FAIRY, "Bottle with Fairy" },
    { RG_BOTTLE_WITH_FAIRY, "Bouteille avec une F�e" },
    { RG_BOTTLE_WITH_FISH, "Bottle with Fish" },
    { RG_BOTTLE_WITH_FISH, "Bouteille avec un Poisson" },
    { RG_BOTTLE_WITH_BLUE_FIRE, "Bottle with Blue Fire" },
    { RG_BOTTLE_WITH_BLUE_FIRE, "Bouteille avec une Flamme Bleue" },
    { RG_BOTTLE_WITH_BUGS, "Bottle with Bugs" },
    { RG_BOTTLE_WITH_BUGS, "Bouteille avec des Insectes" },
    { RG_BOTTLE_WITH_POE, "Bottle with Poe" },
    { RG_BOTTLE_WITH_POE, "Bouteille avec un Esprit" },
    { RG_RUTOS_LETTER, "Bottle with Ruto's Letter" },
    { RG_RUTOS_LETTER, "Bouteille avec la Lettre de Ruto" },
    { RG_BOTTLE_WITH_BIG_POE, "Bottle with Big Poe" },
    { RG_BOTTLE_WITH_BIG_POE, "Bouteille avec une �me" },
    { RG_ZELDAS_LULLABY, "Zelda's Lullaby" },
    { RG_ZELDAS_LULLABY, "Berceuse de Zelda" },
    { RG_EPONAS_SONG, "Epona's Song" },
    { RG_EPONAS_SONG, "Chant d'�pona" },
    { RG_SARIAS_SONG, "Saria's Song" },
    { RG_SARIAS_SONG, "Chant de Saria" },
    { RG_SUNS_SONG, "Sun's Song" },
    { RG_SUNS_SONG, "Chant du Soleil" },
    { RG_SONG_OF_TIME, "Song of Time" },
    { RG_SONG_OF_TIME, "Chant du Temps" },
    { RG_SONG_OF_STORMS, "Song of Storms" },
    { RG_SONG_OF_STORMS, "Chant des Temp�tes" },
    { RG_MINUET_OF_FOREST, "Minuet of Forest" },
    { RG_MINUET_OF_FOREST, "Menuet des Bois" },
    { RG_BOLERO_OF_FIRE, "Bolero of Fire" },
    { RG_BOLERO_OF_FIRE, "Bol�ro du Feu" },
    { RG_SERENADE_OF_WATER, "Serenade of Water" },
    { RG_SERENADE_OF_WATER, "S�r�nade de l'Eau" },
    { RG_REQUIEM_OF_SPIRIT, "Requiem of Spirit" },
    { RG_REQUIEM_OF_SPIRIT, "Requiem des Esprits" },
    { RG_NOCTURNE_OF_SHADOW, "Nocturne of Shadow" },
    { RG_NOCTURNE_OF_SHADOW, "Nocturne de l'Ombre" },
    { RG_PRELUDE_OF_LIGHT, "Prelude of Light" },
    { RG_PRELUDE_OF_LIGHT, "Pr�lude de la Lumi�re" },
    { RG_DEKU_TREE_MAP, "Great Deku Tree Map" },
    { RG_DEKU_TREE_MAP, "Carte de l'Arbre Mojo" },
    { RG_DODONGOS_CAVERN_MAP, "Dodongo's Cavern Map" },
    { RG_DODONGOS_CAVERN_MAP, "Carte de la Caverne Dodongo" },
    { RG_JABU_JABUS_BELLY_MAP, "Jabu-Jabu's Belly Map" },
    { RG_JABU_JABUS_BELLY_MAP, "Carte du Ventre de Jabu-Jabu" },
    { RG_FOREST_TEMPLE_MAP, "Forest Temple Map" },
    { RG_FOREST_TEMPLE_MAP, "Carte du Temple de la For�t" },
    { RG_FIRE_TEMPLE_MAP, "Fire Temple Map" },
    { RG_FIRE_TEMPLE_MAP, "Carte du Temple du Feu" },
    { RG_WATER_TEMPLE_MAP, "Water Temple Map" },
    { RG_WATER_TEMPLE_MAP, "Carte du Temple de l'Eau" },
    { RG_SPIRIT_TEMPLE_MAP, "Spirit Temple Map" },
    { RG_SPIRIT_TEMPLE_MAP, "Carte du Temple de l'Esprit" },
    { RG_SHADOW_TEMPLE_MAP, "Shadow Temple Map" },
    { RG_SHADOW_TEMPLE_MAP, "Carte du Temple de l'Ombre" },
    { RG_BOTTOM_OF_THE_WELL_MAP, "Bottom of the Well Map" },
    { RG_BOTTOM_OF_THE_WELL_MAP, "Carte du Puits" },
    { RG_ICE_CAVERN_MAP, "Ice Cavern Map" },
    { RG_ICE_CAVERN_MAP, "Carte de la Caverne Polaire" },
    { RG_DEKU_TREE_COMPASS, "Great Deku Tree Compass" },
    { RG_DEKU_TREE_COMPASS, "Boussole de l'Arbre Mojo" },
    { RG_DODONGOS_CAVERN_COMPASS, "Dodongo's Cavern Compass" },
    { RG_DODONGOS_CAVERN_COMPASS, "Boussole de la Caverne Dodongo" },
    { RG_JABU_JABUS_BELLY_COMPASS, "Jabu-Jabu's Belly Compass" },
    { RG_JABU_JABUS_BELLY_COMPASS, "Boussole du Ventre de Jabu-Jabu" },
    { RG_FOREST_TEMPLE_COMPASS, "Forest Temple Compass" },
    { RG_FOREST_TEMPLE_COMPASS, "Boussole du Temple de la For�t" },
    { RG_FIRE_TEMPLE_COMPASS, "Fire Temple Compass" },
    { RG_FIRE_TEMPLE_COMPASS, "Boussole du Temple du Feu" },
    { RG_WATER_TEMPLE_COMPASS, "Water Temple Compass" },
    { RG_WATER_TEMPLE_COMPASS, "Boussole du Temple de l'Eau" },
    { RG_SPIRIT_TEMPLE_COMPASS, "Spirit Temple Compass" },
    { RG_SPIRIT_TEMPLE_COMPASS, "Boussole du Temple des Esprits" },
    { RG_SHADOW_TEMPLE_COMPASS, "Shadow Temple Compass" },
    { RG_SHADOW_TEMPLE_COMPASS, "Boussole du Temple de l'Ombre" },
    { RG_BOTTOM_OF_THE_WELL_COMPASS, "Bottom of the Well Compass" },
    { RG_BOTTOM_OF_THE_WELL_COMPASS, "Boussole du Puits" },
    { RG_ICE_CAVERN_COMPASS, "Ice Cavern Compass" },
    { RG_ICE_CAVERN_COMPASS, "Boussole de la Caverne Polaire" },
    { RG_FOREST_TEMPLE_BOSS_KEY, "Forest Temple Big Key" },
    { RG_FOREST_TEMPLE_BOSS_KEY, "Cl� d'Or du Temple de la For�t" },
    { RG_FIRE_TEMPLE_BOSS_KEY, "Fire Temple Big Key" },
    { RG_FIRE_TEMPLE_BOSS_KEY, "Cl� d'Or du Temple du Feu" },
    { RG_WATER_TEMPLE_BOSS_KEY, "Water Temple Big Key" },
    { RG_WATER_TEMPLE_BOSS_KEY, "Cl� d'Or du Temple de l'Eau" },
    { RG_SPIRIT_TEMPLE_BOSS_KEY, "Spirit Temple Big Key" },
    { RG_SPIRIT_TEMPLE_BOSS_KEY, "Cl� d'Or du Temple de l'Esprit" },
    { RG_SHADOW_TEMPLE_BOSS_KEY, "Shadow Temple Big Key" },
    { RG_SHADOW_TEMPLE_BOSS_KEY, "Cl� d'Or du Temple de l'Ombre" },
    { RG_GANONS_CASTLE_BOSS_KEY, "Ganon's Castle Big Key" },
    { RG_GANONS_CASTLE_BOSS_KEY, "Cl� d'Or du Ch�teau de Ganon" },
    { RG_FOREST_TEMPLE_SMALL_KEY, "Forest Temple Small Key" },
    { RG_FOREST_TEMPLE_SMALL_KEY, "Petite Cl� du Temple de la For�t" },
    { RG_FIRE_TEMPLE_SMALL_KEY, "Fire Temple Small Key" },
    { RG_FIRE_TEMPLE_SMALL_KEY, "Petite Cl� du Temple du Feu" },
    { RG_WATER_TEMPLE_SMALL_KEY, "Water Temple Small Key" },
    { RG_WATER_TEMPLE_SMALL_KEY, "Petite Cl� du Temple de l'Eau" },
    { RG_SPIRIT_TEMPLE_SMALL_KEY, "Spirit Temple Small Key" },
    { RG_SPIRIT_TEMPLE_SMALL_KEY, "Petite Cl� du Temple de l'Esprit" },
    { RG_SHADOW_TEMPLE_SMALL_KEY, "Shadow Temple Small Key" },
    { RG_SHADOW_TEMPLE_SMALL_KEY, "Petite Cl� du Temple de l'Ombre" },
    { RG_BOTTOM_OF_THE_WELL_SMALL_KEY, "Bottom of the Well Small Key" },
    { RG_BOTTOM_OF_THE_WELL_SMALL_KEY, "Petite Cl� du Puits" },
    { RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, "Training Grounds Small Key" },
    { RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, "Petite Cl� du Gymnase Gerudo" },
    { RG_GERUDO_FORTRESS_SMALL_KEY, "Gerudo Fortress Small Key" },
    { RG_GERUDO_FORTRESS_SMALL_KEY, "Petite Cl� du Repaire des Voleurs" },
    { RG_GANONS_CASTLE_SMALL_KEY, "Ganon's Castle Small Key" },
    { RG_GANONS_CASTLE_SMALL_KEY, "Petite Cl� du Ch�teau de Ganon" },
    { RG_TREASURE_GAME_SMALL_KEY, "Chest Game Small Key" },
    { RG_TREASURE_GAME_SMALL_KEY, "Petite Cl� du jeu la Chasse-aux-Tr�sors" },
    { RG_FOREST_TEMPLE_KEY_RING, "Forest Temple Key Ring" },
    { RG_FOREST_TEMPLE_KEY_RING, "Trousseau du Temple de la For�t" },
    { RG_FIRE_TEMPLE_KEY_RING, "Fire Temple Key Ring" },
    { RG_FIRE_TEMPLE_KEY_RING, "Trousseau du Temple du Feu" },
    { RG_WATER_TEMPLE_KEY_RING, "Water Temple Key Ring" },
    { RG_WATER_TEMPLE_KEY_RING, "Trousseau du Temple de l'Eau" },
    { RG_SPIRIT_TEMPLE_KEY_RING, "Spirit Temple Key Ring" },
    { RG_SPIRIT_TEMPLE_KEY_RING, "Trousseau du Temple de l'Esprit" },
    { RG_SHADOW_TEMPLE_KEY_RING, "Shadow Temple Key Ring" },
    { RG_SHADOW_TEMPLE_KEY_RING, "Trousseau du Temple de l'Ombre" },
    { RG_BOTTOM_OF_THE_WELL_KEY_RING, "Bottom of the Well Key Ring" },
    { RG_BOTTOM_OF_THE_WELL_KEY_RING, "Trousseau du Puits" },
    { RG_GERUDO_TRAINING_GROUNDS_KEY_RING, "Training Grounds Key Ring" },
    { RG_GERUDO_TRAINING_GROUNDS_KEY_RING, "Trousseau du Gymnase Gerudo" },
    { RG_GERUDO_FORTRESS_KEY_RING, "Gerudo Fortress Key Ring" },
    { RG_GERUDO_FORTRESS_KEY_RING, "Trousseau du Repaire des Voleurs" },
    { RG_GANONS_CASTLE_KEY_RING, "Ganon's Castle Key Ring" },
    { RG_GANONS_CASTLE_KEY_RING, "Trousseau du Ch�teau de Ganon" },
    { RG_KOKIRI_EMERALD, "Kokiri's Emerald" },
    { RG_KOKIRI_EMERALD, "�meraude Kokiri" },
    { RG_GORON_RUBY, "Goron's Ruby" },
    { RG_GORON_RUBY, "Rubis Goron" },
    { RG_ZORA_SAPPHIRE, "Zora's Sapphire" },
    { RG_ZORA_SAPPHIRE, "Saphir Zora" },
    { RG_FOREST_MEDALLION, "Forest Medallion" },
    { RG_FOREST_MEDALLION, "M�daillon de la For�t" },
    { RG_FIRE_MEDALLION, "Fire Medallion" },
    { RG_FIRE_MEDALLION, "M�daillon du Feu" },
    { RG_WATER_MEDALLION, "Water Medallion" },
    { RG_WATER_MEDALLION, "M�daillon de l'Eau" },
    { RG_SPIRIT_MEDALLION, "Spirit Medallion" },
    { RG_SPIRIT_MEDALLION, "M�daillon de l'Esprit" },
    { RG_SHADOW_MEDALLION, "Shadow Medallion" },
    { RG_SHADOW_MEDALLION, "M�daillon de l'Ombre" },
    { RG_LIGHT_MEDALLION, "Light Medallion" },
    { RG_LIGHT_MEDALLION, "M�daillon de la Lumi�re" },
    { RG_RECOVERY_HEART, "Recovery Heart" },
    { RG_RECOVERY_HEART, "Coeur de Vie" },
    { RG_GREEN_RUPEE, "Green Rupee" },
    { RG_GREEN_RUPEE, "Rubis Vert" },
    { RG_BLUE_RUPEE, "Blue Rupee" },
    { RG_BLUE_RUPEE, "Rubis Bleu" },
    { RG_RED_RUPEE, "Red Rupee" },
    { RG_RED_RUPEE, "Rubis Rouge" },
    { RG_PURPLE_RUPEE, "Purple Rupee" },
    { RG_PURPLE_RUPEE, "Rubis Pourpre" },
    { RG_HUGE_RUPEE, "Huge Rupee" },
    { RG_HUGE_RUPEE, "�norme Rubis" },
    { RG_PIECE_OF_HEART, "Piece of Heart" },
    { RG_PIECE_OF_HEART, "Quart de Coeur" },
    { RG_HEART_CONTAINER, "Heart Container" },
    { RG_HEART_CONTAINER, "R�ceptacle de Coeur" },
    { RG_ICE_TRAP, "Ice Trap" },
    { RG_ICE_TRAP, "Pi�ge de Glace" },
    { RG_MILK, "Milk" },
    { RG_MILK, "Lait" },
    { RG_BOMBS_5, "Bombs (5)" },
    { RG_BOMBS_5, "Bombes (5)" },
    { RG_BOMBS_10, "Bombs (10)" },
    { RG_BOMBS_10, "Bombes (10)" },
    { RG_BOMBS_20, "Bombs (20)" },
    { RG_BOMBS_20, "Bombes (20)" },
    { RG_BOMBCHU_5, "Bombchu (5)" },
    { RG_BOMBCHU_5, "Missiles (5)" },
    { RG_BOMBCHU_10, "Bombchu (10)" },
    { RG_BOMBCHU_10, "Missiles (10)" },
    { RG_BOMBCHU_20, "Bombchu (20)" },
    { RG_BOMBCHU_20, "Missiles (20)" },
    { RG_BOMBCHU_DROP, "Bombchu Drop" },
    { RG_BOMBCHU_DROP, "Drop Missile" },
    { RG_ARROWS_5, "Arrows (5)" },
    { RG_ARROWS_5, "Fl�ches (5)" },
    { RG_ARROWS_10, "Arrows (10)" },
    { RG_ARROWS_10, "Fl�ches (10)" },
    { RG_ARROWS_30, "Arrows (30)" },
    { RG_ARROWS_30, "Fl�ches (30)" },
    { RG_DEKU_NUTS_5, "Deku Nuts (5)" },
    { RG_DEKU_NUTS_5, "Noix Mojo (5)" },
    { RG_DEKU_NUTS_10, "Deku Nuts (10)" },
    { RG_DEKU_NUTS_10, "Noix Mojo (10)" },
    { RG_DEKU_SEEDS_30, "Deku Seeds (30)" },
    { RG_DEKU_SEEDS_30, "Graines Mojo (30)" },
    { RG_DEKU_STICK_1, "Deku Stick (1)" },
    { RG_DEKU_STICK_1, "B�ton Mojo (1)" },
    { RG_RED_POTION_REFILL, "Red Potion Refill" },
    { RG_RED_POTION_REFILL, "Recharge de Potion Rouge" },
    { RG_GREEN_POTION_REFILL, "Green Potion Refill" },
    { RG_GREEN_POTION_REFILL, "Recharge de Potion Verte" },
    { RG_BLUE_POTION_REFILL, "Blue Potion Refill" },
    { RG_BLUE_POTION_REFILL, "Recharge de Potion Bleue" },
    { RG_TREASURE_GAME_HEART, "Piece of Heart (Treasure Chest Minigame)" },
    { RG_TREASURE_GAME_HEART, "Quart de Coeur (Chasse-aux-Tr�sors)" },
    { RG_TREASURE_GAME_GREEN_RUPEE, "Green Rupee (Treasure Chest Minigame)" },
    { RG_TREASURE_GAME_GREEN_RUPEE, "Rubis Vert (Chasse-aux-Tr�sors)" },
    { RG_BUY_DEKU_NUT_5, "Buy Deku Nut (5)" },
    { RG_BUY_DEKU_NUT_5, "Acheter: Noix Mojo (5)" },
    { RG_BUY_ARROWS_30, "Buy Arrows (30)" },
    { RG_BUY_ARROWS_30, "Acheter: Fl�ches (30)" },
    { RG_BUY_ARROWS_50, "Buy Arrows (50)" },
    { RG_BUY_ARROWS_50, "Acheter: Fl�ches (50)" },
    { RG_BUY_BOMBS_525, "Buy Bombs (5) [25]" },
    { RG_BUY_BOMBS_525, "Acheter: Bombes (5) [25]" },
    { RG_BUY_DEKU_NUT_10, "Buy Deku Nut (10)" },
    { RG_BUY_DEKU_NUT_10, "Acheter: Noix Mojo (10)" },
    { RG_BUY_DEKU_STICK_1, "Buy Deku Stick (1)" },
    { RG_BUY_DEKU_STICK_1, "Acheter: B�ton Mojo (1)" },
    { RG_BUY_BOMBS_10, "Buy Bombs (10)" },
    { RG_BUY_BOMBS_10, "Acheter: Bombes " },
    { RG_BUY_FISH, "Buy Fish" },
    { RG_BUY_FISH, "Acheter: Poisson" },
    { RG_BUY_RED_POTION_30, "Buy Red Potion [30]" },
    { RG_BUY_RED_POTION_30, "Acheter: Potion Rouge [30]" },
    { RG_BUY_GREEN_POTION, "Buy Green Potion" },
    { RG_BUY_GREEN_POTION, "Acheter: Potion Verte" },
    { RG_BUY_BLUE_POTION, "Buy Blue Potion" },
    { RG_BUY_BLUE_POTION, "Acheter: Potion Bleue" },
    { RG_BUY_HYLIAN_SHIELD, "Buy Hylian Shield" },
    { RG_BUY_HYLIAN_SHIELD, "Acheter: Bouclier Hylien" },
    { RG_BUY_DEKU_SHIELD, "Buy Deku Shield" },
    { RG_BUY_DEKU_SHIELD, "Acheter: Bouclier Mojo" },
    { RG_BUY_GORON_TUNIC, "Buy Goron Tunic" },
    { RG_BUY_GORON_TUNIC, "Acheter: Tunique Goron" },
    { RG_BUY_ZORA_TUNIC, "Buy Zora Tunic" },
    { RG_BUY_ZORA_TUNIC, "Acheter: Tunique Zora" },
    { RG_BUY_HEART, "Buy Heart" },
    { RG_BUY_HEART, "Acheter: Coeur de Vie" },
    { RG_BUY_BOMBCHU_10, "Buy Bombchu (10)" },
    { RG_BUY_BOMBCHU_10, "Acheter: Missiles (10)" },
    { RG_BUY_BOMBCHU_20, "Buy Bombchu (20)" },
    { RG_BUY_BOMBCHU_20, "Acheter: Missiles (20)" },
    { RG_BUY_BOMBCHU_5, "Buy Bombchu (5)" },
    { RG_BUY_BOMBCHU_5, "Acheter: Missiles (5)" },
    { RG_BUY_DEKU_SEEDS_30, "Buy Deku Seeds (30)" },
    { RG_BUY_DEKU_SEEDS_30, "Acheter: Graines Mojo (30)" },
    { RG_SOLD_OUT, "Sold Out" },
    { RG_SOLD_OUT, "Vendu" },
    { RG_SOLD_OUT, "Rupture de de stock" },
    { RG_BUY_BLUE_FIRE, "Buy Blue Fire" },
    { RG_BUY_BLUE_FIRE, "Acheter: Flamme Bleue" },
    { RG_BUY_BOTTLE_BUG, "Buy Bottle Bug" },
    { RG_BUY_BOTTLE_BUG, "Acheter: Insecte en bouteille" },
    { RG_BUY_POE, "Buy Poe" },
    { RG_BUY_POE, "Acheter: Esprit" },
    { RG_BUY_FAIRYS_SPIRIT, "Buy Fairy's Spirit" },
    { RG_BUY_FAIRYS_SPIRIT, "Acheter: Esprit de F�e" },
    { RG_BUY_ARROWS_10, "Buy Arrows (10)" },
    { RG_BUY_ARROWS_10, "Acheter: Fl�ches (10)" },
    { RG_BUY_BOMBS_20, "Buy Bombs (20)" },
    { RG_BUY_BOMBS_20, "Acheter: Bombes (20)" },
    { RG_BUY_BOMBS_30, "Buy Bombs (30)" },
    { RG_BUY_BOMBS_30, "Acheter: Bombes (30)" },
    { RG_BUY_BOMBS_535, "Buy Bombs (5) [35]" },
    { RG_BUY_BOMBS_535, "Acheter: Bombes (5) [35]" },
    { RG_BUY_RED_POTION_40, "Buy Red Potion [40]" },
    { RG_BUY_RED_POTION_40, "Acheter: Potion Rouge [40]" },
    { RG_BUY_RED_POTION_50, "Buy Red Potion [50]" },
    { RG_BUY_RED_POTION_50, "Acheter: Potion Rouge [50]" },
    { RG_TRIFORCE, "Triforce" },
    { RG_HINT, "Hint" },
    { RG_HINT, "Indice" }
};

const RandomizerCheck checks_RR_UNKNOWN[] = {
    RC_UNKNOWN_CHECK,      RC_UNKNOWN_CHECK,   RC_LINKS_POCKET,      RC_UNKNOWN_CHECK,
    RC_FAIRY_GOSSIP_STONE, RC_GY_GOSSIP_STONE, RC_JABU_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_KF[] = {
    RC_KF_KOKIRI_SWORD_CHEST,
    RC_KF_MIDOS_TOP_LEFT_CHEST,
    RC_KF_MIDOS_TOP_RIGHT_CHEST,
    RC_KF_MIDOS_BOTTOM_LEFT_CHEST,
    RC_KF_MIDOS_BOTTOM_RIGHT_CHEST,
    RC_KF_STORMS_GROTTO_CHEST,
    RC_KF_GS_BEAN_PATCH,
    RC_KF_GS_KNOW_IT_ALL_HOUSE,
    RC_KF_GS_HOUSE_OF_TWINS,
    RC_KF_LINKS_HOUSE_COW,
    RC_KF_SHOP_ITEM_1,
    RC_KF_SHOP_ITEM_2,
    RC_KF_SHOP_ITEM_3,
    RC_KF_SHOP_ITEM_4,
    RC_KF_SHOP_ITEM_5,
    RC_KF_SHOP_ITEM_6,
    RC_KF_SHOP_ITEM_7,
    RC_KF_SHOP_ITEM_8,
    RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE,
    RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE,
    RC_KF_GOSSIP_STONE,
    RC_KF_STORMS_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_LW[] = {
    RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST,
    RC_LW_SKULL_KID,
    RC_LW_TRADE_COJIRO,
    RC_LW_TRADE_ODD_POTION,
    RC_LW_OCARINA_MEMORY_GAME,
    RC_LW_TARGET_IN_WOODS,
    RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT,
    RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT,
    RC_LW_DEKU_SCRUB_NEAR_BRIDGE,
    RC_LW_DEKU_SCRUB_GROTTO_REAR,
    RC_LW_DEKU_SCRUB_GROTTO_FRONT,
    RC_DEKU_THEATER_SKULL_MASK,
    RC_DEKU_THEATER_MASK_OF_TRUTH,
    RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE,
    RC_LW_GS_BEAN_PATCH_NEAR_THEATER,
    RC_LW_GS_ABOVE_THEATER,
    RC_LW_GIFT_FROM_SARIA,
    RC_LW_GOSSIP_STONE,
    RC_LW_NEAR_SHORTCUTS_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_HF[] = {
    RC_HF_SOUTHEAST_GROTTO_CHEST,
    RC_HF_OPEN_GROTTO_CHEST,
    RC_HF_NEAR_MARKET_GROTTO_CHEST,
    RC_HF_OCARINA_OF_TIME_ITEM,
    RC_HF_TEKTITE_GROTTO_FREESTANDING_POH,
    RC_HF_DEKU_SCRUB_GROTTO,
    RC_HF_GS_COW_GROTTO,
    RC_HF_GS_NEAR_KAK_GROTTO,
    RC_SONG_FROM_OCARINA_OF_TIME,
    RC_HF_COW_GROTTO_COW,
    RC_HF_COW_GROTTO_GOSSIP_STONE,
    RC_HF_NEAR_MARKET_GOSSIP_STONE,
    RC_HF_OPEN_GROTTO_GOSSIP_STONE,
    RC_HF_SOUTHEAST_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_LH[] = {
    RC_LH_CHILD_FISHING,
    RC_LH_ADULT_FISHING,
    RC_LH_LAB_DIVE,
    RC_LH_TRADE_FROG,
    RC_LH_UNDERWATER_ITEM,
    RC_LH_SUN,
    RC_LH_FREESTANDING_POH,
    RC_LH_DEKU_SCRUB_GROTTO_LEFT,
    RC_LH_DEKU_SCRUB_GROTTO_RIGHT,
    RC_LH_DEKU_SCRUB_GROTTO_CENTER,
    RC_LH_GS_BEAN_PATCH,
    RC_LH_GS_SMALL_ISLAND,
    RC_LH_GS_LAB_WALL,
    RC_LH_GS_LAB_CRATE,
    RC_LH_GS_TREE,
    RC_LH_LAB_GOSSIP_STONE,
    RC_LH_SOUTHEAST_GOSSIP_STONE,
    RC_LH_SOUTHWEST_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_GV[] = {
    RC_GV_CHEST,
    RC_GV_TRADE_SAW,
    RC_GV_WATERFALL_FREESTANDING_POH,
    RC_GV_CRATE_FREESTANDING_POH,
    RC_GV_DEKU_SCRUB_GROTTO_REAR,
    RC_GV_DEKU_SCRUB_GROTTO_FRONT,
    RC_GV_GS_BEAN_PATCH,
    RC_GV_GS_SMALL_BRIDGE,
    RC_GV_GS_PILLAR,
    RC_GV_GS_BEHIND_TENT,
    RC_GV_COW,
    RC_GV_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_GF[] = {
    RC_GF_CHEST,
    RC_GF_HBA_1000_POINTS,
    RC_GF_HBA_1500_POINTS,
    RC_GF_GERUDO_MEMBERSHIP_CARD,
    RC_GF_NORTH_F1_CARPENTER,
    RC_GF_NORTH_F2_CARPENTER,
    RC_GF_SOUTH_F1_CARPENTER,
    RC_GF_SOUTH_F2_CARPENTER,
    RC_GF_GS_ARCHERY_RANGE,
    RC_GF_GS_TOP_FLOOR,
};

const RandomizerCheck checks_RR_WASTELAND[] = {
    RC_WASTELAND_CHEST,
    RC_WASTELAND_BOMBCHU_SALESMAN,
    RC_WASTELAND_GS,
};

const RandomizerCheck checks_RR_COLOSSUS[] = {
    RC_COLOSSUS_FREESTANDING_POH,
    RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR,
    RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT,
    RC_COLOSSUS_GS_BEAN_PATCH,
    RC_COLOSSUS_GS_HILL,
    RC_COLOSSUS_GS_TREE,
    RC_COLOSSUS_GREAT_FAIRY_REWARD,
    RC_SHEIK_AT_COLOSSUS,
    RC_COLOSSUS_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_MARKET[] = {
    RC_MARKET_TREASURE_CHEST_GAME_REWARD,
    RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE,
    RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE,
    RC_MARKET_LOST_DOG,
    RC_MARKET_SHOOTING_GALLERY_REWARD,
    RC_MARKET_10_BIG_POES,
    RC_MARKET_TREASURE_CHEST_GAME_ITEM_1,
    RC_MARKET_TREASURE_CHEST_GAME_ITEM_2,
    RC_MARKET_TREASURE_CHEST_GAME_ITEM_3,
    RC_MARKET_TREASURE_CHEST_GAME_ITEM_4,
    RC_MARKET_TREASURE_CHEST_GAME_ITEM_5,
    RC_MARKET_GS_GUARD_HOUSE,
    RC_MARKET_BOMBCHU_SHOP_ITEM_1,
    RC_MARKET_BOMBCHU_SHOP_ITEM_2,
    RC_MARKET_BOMBCHU_SHOP_ITEM_3,
    RC_MARKET_BOMBCHU_SHOP_ITEM_4,
    RC_MARKET_BOMBCHU_SHOP_ITEM_5,
    RC_MARKET_BOMBCHU_SHOP_ITEM_6,
    RC_MARKET_BOMBCHU_SHOP_ITEM_7,
    RC_MARKET_BOMBCHU_SHOP_ITEM_8,
    RC_MARKET_POTION_SHOP_ITEM_1,
    RC_MARKET_POTION_SHOP_ITEM_2,
    RC_MARKET_POTION_SHOP_ITEM_3,
    RC_MARKET_POTION_SHOP_ITEM_4,
    RC_MARKET_POTION_SHOP_ITEM_5,
    RC_MARKET_POTION_SHOP_ITEM_6,
    RC_MARKET_POTION_SHOP_ITEM_7,
    RC_MARKET_POTION_SHOP_ITEM_8,
    RC_MARKET_BAZAAR_ITEM_1,
    RC_MARKET_BAZAAR_ITEM_2,
    RC_MARKET_BAZAAR_ITEM_3,
    RC_MARKET_BAZAAR_ITEM_4,
    RC_MARKET_BAZAAR_ITEM_5,
    RC_MARKET_BAZAAR_ITEM_6,
    RC_MARKET_BAZAAR_ITEM_7,
    RC_MARKET_BAZAAR_ITEM_8,
};

const RandomizerCheck checks_RR_HC[] = {
    RC_HC_MALON_EGG,
    RC_HC_ZELDAS_LETTER,
    RC_HC_GS_STORMS_GROTTO,
    RC_HC_GS_TREE,
    RC_HC_GREAT_FAIRY_REWARD,
    RC_SONG_FROM_IMPA,
    RC_HC_MALON_GOSSIP_STONE,
    RC_HC_ROCK_WALL_GOSSIP_STONE,
    RC_HC_STORMS_GROTTO_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_KAK[] = {
    RC_KAK_REDEAD_GROTTO_CHEST,
    RC_KAK_OPEN_GROTTO_CHEST,
    RC_KAK_10_GOLD_SKULLTULA_REWARD,
    RC_KAK_20_GOLD_SKULLTULA_REWARD,
    RC_KAK_30_GOLD_SKULLTULA_REWARD,
    RC_KAK_40_GOLD_SKULLTULA_REWARD,
    RC_KAK_50_GOLD_SKULLTULA_REWARD,
    RC_KAK_MAN_ON_ROOF,
    RC_KAK_SHOOTING_GALLERY_REWARD,
    RC_KAK_TRADE_ODD_MUSHROOM,
    RC_KAK_ANJU_AS_ADULT,
    RC_KAK_ANJU_AS_CHILD,
    RC_KAK_TRADE_POCKET_CUCCO,
    RC_KAK_IMPAS_HOUSE_FREESTANDING_POH,
    RC_KAK_WINDMILL_FREESTANDING_POH,
    RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION,
    RC_KAK_GS_SKULLTULA_HOUSE,
    RC_KAK_GS_GUARDS_HOUSE,
    RC_KAK_GS_TREE,
    RC_KAK_GS_WATCHTOWER,
    RC_KAK_GS_ABOVE_IMPAS_HOUSE,
    RC_SHEIK_IN_KAKARIKO,
    RC_SONG_FROM_WINDMILL,
    RC_KAK_IMPAS_HOUSE_COW,
    RC_KAK_POTION_SHOP_ITEM_1,
    RC_KAK_POTION_SHOP_ITEM_2,
    RC_KAK_POTION_SHOP_ITEM_3,
    RC_KAK_POTION_SHOP_ITEM_4,
    RC_KAK_POTION_SHOP_ITEM_5,
    RC_KAK_POTION_SHOP_ITEM_6,
    RC_KAK_POTION_SHOP_ITEM_7,
    RC_KAK_POTION_SHOP_ITEM_8,
    RC_KAK_BAZAAR_ITEM_1,
    RC_KAK_BAZAAR_ITEM_2,
    RC_KAK_BAZAAR_ITEM_3,
    RC_KAK_BAZAAR_ITEM_4,
    RC_KAK_BAZAAR_ITEM_5,
    RC_KAK_BAZAAR_ITEM_6,
    RC_KAK_BAZAAR_ITEM_7,
    RC_KAK_BAZAAR_ITEM_8,
    RC_KAK_OPEN_GROTTO_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_GRAVEYARD[] = {
    RC_GRAVEYARD_SHIELD_GRAVE_CHEST,
    RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST,
    RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST,
    RC_GRAVEYARD_HOOKSHOT_CHEST,
    RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH,
    RC_GRAVEYARD_FREESTANDING_POH,
    RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR,
    RC_GRAVEYARD_GS_WALL,
    RC_GRAVEYARD_GS_BEAN_PATCH,
    RC_SONG_FROM_ROYAL_FAMILYS_TOMB,
};

const RandomizerCheck checks_RR_DMT[] = {
    RC_DMT_CHEST,
    RC_DMT_STORMS_GROTTO_CHEST,
    RC_DMT_TRADE_BROKEN_SWORD,
    RC_DMT_TRADE_EYEDROPS,
    RC_DMT_TRADE_CLAIM_CHECK,
    RC_DMT_FREESTANDING_POH,
    RC_DMT_GS_BEAN_PATCH,
    RC_DMT_GS_NEAR_KAK,
    RC_DMT_GS_ABOVE_DODONGOS_CAVERN,
    RC_DMT_GS_FALLING_ROCKS_PATH,
    RC_DMT_GREAT_FAIRY_REWARD,
    RC_DMT_COW_GROTTO_COW,
    RC_DMT_GOSSIP_STONE,
    RC_DMT_STORMS_GROTTO_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_GC[] = {
    RC_GC_MAZE_LEFT_CHEST,
    RC_GC_MAZE_RIGHT_CHEST,
    RC_GC_MAZE_CENTER_CHEST,
    RC_GC_ROLLING_GORON_AS_CHILD,
    RC_GC_ROLLING_GORON_AS_ADULT,
    RC_GC_DARUNIAS_JOY,
    RC_GC_POT_FREESTANDING_POH,
    RC_GC_DEKU_SCRUB_GROTTO_LEFT,
    RC_GC_DEKU_SCRUB_GROTTO_RIGHT,
    RC_GC_DEKU_SCRUB_GROTTO_CENTER,
    RC_GC_MEDIGORON,
    RC_GC_GS_CENTER_PLATFORM,
    RC_GC_GS_BOULDER_MAZE,
    RC_GC_SHOP_ITEM_1,
    RC_GC_SHOP_ITEM_2,
    RC_GC_SHOP_ITEM_3,
    RC_GC_SHOP_ITEM_4,
    RC_GC_SHOP_ITEM_5,
    RC_GC_SHOP_ITEM_6,
    RC_GC_SHOP_ITEM_7,
    RC_GC_SHOP_ITEM_8,
    RC_GC_MAZE_GOSSIP_STONE,
    RC_GC_MEDIGORON_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_DMC[] = {
    RC_DMC_UPPER_GROTTO_CHEST,
    RC_DMC_WALL_FREESTANDING_POH,
    RC_DMC_VOLCANO_FREESTANDING_POH,
    RC_DMC_DEKU_SCRUB,
    RC_DMC_DEKU_SCRUB_GROTTO_LEFT,
    RC_DMC_DEKU_SCRUB_GROTTO_RIGHT,
    RC_DMC_DEKU_SCRUB_GROTTO_CENTER,
    RC_DMC_GS_BEAN_PATCH,
    RC_DMC_GS_CRATE,
    RC_DMC_GREAT_FAIRY_REWARD,
    RC_SHEIK_IN_CRATER,
    RC_DMC_GOSSIP_STONE,
    RC_DMC_UPPER_GROTTO_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_ZR[] = {
    RC_ZR_OPEN_GROTTO_CHEST,
    RC_ZR_MAGIC_BEAN_SALESMAN,
    RC_ZR_FROGS_ZELDAS_LULLABY,
    RC_ZR_FROGS_EPONAS_SONG,
    RC_ZR_FROGS_SARIAS_SONG,
    RC_ZR_FROGS_SUNS_SONG,
    RC_ZR_FROGS_SONG_OF_TIME,
    RC_ZR_FROGS_IN_THE_RAIN,
    RC_ZR_FROGS_OCARINA_GAME,
    RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH,
    RC_ZR_NEAR_DOMAIN_FREESTANDING_POH,
    RC_ZR_DEKU_SCRUB_GROTTO_REAR,
    RC_ZR_DEKU_SCRUB_GROTTO_FRONT,
    RC_ZR_GS_LADDER,
    RC_ZR_GS_TREE,
    RC_ZR_GS_ABOVE_BRIDGE,
    RC_ZR_GS_NEAR_RAISED_GROTTOS,
    RC_ZR_NEAR_DOMAIN_GOSSIP_STONE,
    RC_ZR_NEAR_GROTTOS_GOSSIP_STONE,
    RC_ZR_OPEN_GROTTO_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_ZD[] = {
    RC_ZD_CHEST,
    RC_ZD_DIVING_MINIGAME,
    RC_ZD_KING_ZORA_THAWED,
    RC_ZD_TRADE_PRESCRIPTION,
    RC_ZD_GS_FROZEN_WATERFALL,
    RC_ZD_SHOP_ITEM_1,
    RC_ZD_SHOP_ITEM_2,
    RC_ZD_SHOP_ITEM_3,
    RC_ZD_SHOP_ITEM_4,
    RC_ZD_SHOP_ITEM_5,
    RC_ZD_SHOP_ITEM_6,
    RC_ZD_SHOP_ITEM_7,
    RC_ZD_SHOP_ITEM_8,
    RC_ZD_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_ZF[] = {
    RC_ZF_ICEBERC_FREESTANDING_POH,
    RC_ZF_BOTTOM_FREESTANDING_POH,
    RC_ZF_GS_ABOVE_THE_LOG,
    RC_ZF_GS_HIDDEN_CAVE,
    RC_ZF_GS_TREE,
    RC_ZF_GREAT_FAIRY_REWARD,
};

const RandomizerCheck checks_RR_LLR[] = {
    RC_LLR_TALONS_CHICKENS,
    RC_LLR_FREESTANDING_POH,
    RC_LLR_DEKU_SCRUB_GROTTO_LEFT,
    RC_LLR_DEKU_SCRUB_GROTTO_RIGHT,
    RC_LLR_DEKU_SCRUB_GROTTO_CENTER,
    RC_LLR_GS_BACK_WALL,
    RC_LLR_GS_RAIN_SHED,
    RC_LLR_GS_HOUSE_WINDOW,
    RC_LLR_GS_TREE,
    RC_SONG_FROM_MALON,
    RC_LLR_STABLES_LEFT_COW,
    RC_LLR_STABLES_RIGHT_COW,
    RC_LLR_TOWER_LEFT_COW,
    RC_LLR_TOWER_RIGHT_COW,
};

const RandomizerCheck checks_RR_SFM[] = {
    RC_SFM_WOLFOS_GROTTO_CHEST,
    RC_SFM_DEKU_SCRUB_GROTTO_REAR,
    RC_SFM_DEKU_SCRUB_GROTTO_FRONT,
    RC_SFM_GS,
    RC_SHEIK_IN_FOREST,
    RC_SONG_FROM_SARIA,
    RC_SFM_MAZE_LOWER_GOSSIP_STONE,
    RC_SFM_MAZE_UPPER_GOSSIP_STONE,
    RC_SFM_SARIA_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_GTG[] = {
    RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST,
    RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST,
    RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST,
    RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST,
    RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST,
    RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST,
    RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST,
    RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST,
    RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST,
    RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST,
    RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST,
    RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST,
    RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST,
    RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST,
    RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST,
    RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST,
    RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST,
    RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST,
    RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST,
    RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST,
    RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST,
    RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY,
};

const RandomizerCheck checks_RR_DEKU[] = {
    RC_DEKU_TREE_MAP_CHEST,
    RC_DEKU_TREE_COMPASS_CHEST,
    RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST,
    RC_DEKU_TREE_BASEMENT_CHEST,
    RC_DEKU_TREE_SLINGSHOT_CHEST,
    RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST,
    RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM,
    RC_DEKU_TREE_GS_BASEMENT_GATE,
    RC_DEKU_TREE_GS_BASEMENT_VINES,
    RC_DEKU_TREE_GS_COMPASS_ROOM,
    RC_QUEEN_GOHMA,
    RC_DEKU_TREE_QUEEN_GOHMA_HEART,
};

const RandomizerCheck checks_RR_DC[] = {
    RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST,
    RC_DODONGOS_CAVERN_MAP_CHEST,
    RC_DODONGOS_CAVERN_COMPASS_CHEST,
    RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST,
    RC_DODONGOS_CAVERN_BOMB_BAG_CHEST,
    RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST,
    RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,
    RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS,
    RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT,
    RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY,
    RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS,
    RC_DODONGOS_CAVERN_GS_SCARECROW,
    RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS,
    RC_DODONGOS_CAVERN_GS_BACK_ROOM,
    RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS,
    RC_KING_DODONGO,
    RC_DODONGOS_CAVERN_KING_DODONGO_HEART,
    RC_DODONGOS_CAVERN_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_JABU[] = {
    RC_JABU_JABUS_BELLY_MAP_CHEST,
    RC_JABU_JABUS_BELLY_COMPASS_CHEST,
    RC_JABU_JABUS_BELLY_BOOMERANG_CHEST,
    RC_JABU_JABUS_BELLY_DEKU_SCRUB,
    RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER,
    RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER,
    RC_JABU_JABUS_BELLY_GS_NEAR_BOSS,
    RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM,
    RC_BARINADE,
    RC_JABU_JABUS_BELLY_BARINADE_HEART,
};

const RandomizerCheck checks_RR_FOREST[] = {
    RC_FOREST_TEMPLE_FIRST_ROOM_CHEST,
    RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST,
    RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST,
    RC_FOREST_TEMPLE_MAP_CHEST,
    RC_FOREST_TEMPLE_WELL_CHEST,
    RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST,
    RC_FOREST_TEMPLE_EYE_SWITCH_CHEST,
    RC_FOREST_TEMPLE_BOSS_KEY_CHEST,
    RC_FOREST_TEMPLE_FLOORMASTER_CHEST,
    RC_FOREST_TEMPLE_BOW_CHEST,
    RC_FOREST_TEMPLE_RED_POE_CHEST,
    RC_FOREST_TEMPLE_BLUE_POE_CHEST,
    RC_FOREST_TEMPLE_BASEMENT_CHEST,
    RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD,
    RC_FOREST_TEMPLE_GS_FIRST_ROOM,
    RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD,
    RC_FOREST_TEMPLE_GS_LOBBY,
    RC_FOREST_TEMPLE_GS_BASEMENT,
    RC_PHANTOM_GANON,
    RC_FOREST_TEMPLE_PHANTOM_GANON_HEART,
};

const RandomizerCheck checks_RR_FIRE[] = {
    RC_FIRE_TEMPLE_NEAR_BOSS_CHEST,
    RC_FIRE_TEMPLE_FLARE_DANCER_CHEST,
    RC_FIRE_TEMPLE_BOSS_KEY_CHEST,
    RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST,
    RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST,
    RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST,
    RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST,
    RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST,
    RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST,
    RC_FIRE_TEMPLE_SCARECROW_CHEST,
    RC_FIRE_TEMPLE_MAP_CHEST,
    RC_FIRE_TEMPLE_COMPASS_CHEST,
    RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST,
    RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST,
    RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM,
    RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP,
    RC_FIRE_TEMPLE_GS_BOULDER_MAZE,
    RC_FIRE_TEMPLE_GS_SCARECROW_TOP,
    RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB,
    RC_VOLVAGIA,
    RC_FIRE_TEMPLE_VOLVAGIA_HEART,
};

const RandomizerCheck checks_RR_WATER[] = {
    RC_WATER_TEMPLE_MAP_CHEST,
    RC_WATER_TEMPLE_COMPASS_CHEST,
    RC_WATER_TEMPLE_TORCHES_CHEST,
    RC_WATER_TEMPLE_DRAGON_CHEST,
    RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST,
    RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST,
    RC_WATER_TEMPLE_CRACKED_WALL_CHEST,
    RC_WATER_TEMPLE_BOSS_KEY_CHEST,
    RC_WATER_TEMPLE_LONGSHOT_CHEST,
    RC_WATER_TEMPLE_RIVER_CHEST,
    RC_WATER_TEMPLE_GS_BEHIND_GATE,
    RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM,
    RC_WATER_TEMPLE_GS_CENTRAL_PILLAR,
    RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST,
    RC_WATER_TEMPLE_GS_RIVER,
    RC_MORPHA,
    RC_WATER_TEMPLE_MORPHA_HEART,
};

const RandomizerCheck checks_RR_SHADOW[] = {
    RC_SHADOW_TEMPLE_MAP_CHEST,
    RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST,
    RC_SHADOW_TEMPLE_COMPASS_CHEST,
    RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST,
    RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST,
    RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST,
    RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST,
    RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST,
    RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST,
    RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST,
    RC_SHADOW_TEMPLE_WIND_HINT_CHEST,
    RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST,
    RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST,
    RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST,
    RC_SHADOW_TEMPLE_BOSS_KEY_CHEST,
    RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST,
    RC_SHADOW_TEMPLE_FREESTANDING_KEY,
    RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT,
    RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM,
    RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT,
    RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM,
    RC_SHADOW_TEMPLE_GS_NEAR_SHIP,
    RC_BONGO_BONGO,
    RC_SHADOW_TEMPLE_BONGO_BONGO_HEART,
};

const RandomizerCheck checks_RR_SPIRIT[] = {
    RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST,
    RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST,
    RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST,
    RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST,
    RC_SPIRIT_TEMPLE_COMPASS_CHEST,
    RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST,
    RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST,
    RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST,
    RC_SPIRIT_TEMPLE_MAP_CHEST,
    RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST,
    RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST,
    RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST,
    RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST,
    RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST,
    RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST,
    RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST,
    RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST,
    RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST,
    RC_SPIRIT_TEMPLE_TOPMOST_CHEST,
    RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM,
    RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM,
    RC_SPIRIT_TEMPLE_GS_LOBBY,
    RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM,
    RC_SPIRIT_TEMPLE_GS_METAL_FENCE,
    RC_TWINROVA,
    RC_SPIRIT_TEMPLE_TWINROVA_HEART,
};

const RandomizerCheck checks_RR_ICE[] = {
    RC_ICE_CAVERN_MAP_CHEST,           RC_ICE_CAVERN_COMPASS_CHEST,      RC_ICE_CAVERN_IRON_BOOTS_CHEST,
    RC_ICE_CAVERN_FREESTANDING_POH,    RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM, RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM,
    RC_ICE_CAVERN_GS_HEART_PIECE_ROOM, RC_SHEIK_IN_ICE_CAVERN,
};

const RandomizerCheck checks_RR_BOTW[] = {
    RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST,
    RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST,
    RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST,
    RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST,
    RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST,
    RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST,
    RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST,
    RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST,
    RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST,
    RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST,
    RC_BOTTOM_OF_THE_WELL_MAP_CHEST,
    RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST,
    RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST,
    RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY,
    RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE,
    RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM,
    RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM,
};

const RandomizerCheck checks_RR_TOT[] = {
    RC_TOT_LIGHT_ARROWS_CUTSCENE,     RC_SHEIK_AT_TEMPLE,
    RC_TOT_LEFT_CENTER_GOSSIP_STONE,  RC_TOT_LEFT_GOSSIP_STONE,
    RC_TOT_RIGHT_CENTER_GOSSIP_STONE, RC_TOT_RIGHT_GOSSIP_STONE,
};

const RandomizerCheck checks_RR_OGC[] = {
    RC_OGC_GS,
    RC_OGC_GREAT_FAIRY_REWARD,
};

const RandomizerCheck checks_RR_GANONS_CASTLE[] = {
    RC_GANONS_TOWER_BOSS_KEY_CHEST,
    RC_GANONS_CASTLE_FOREST_TRIAL_CHEST,
    RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST,
    RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST,
    RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST,
    RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST,
    RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST,
    RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST,
    RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,
    RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT,
    RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT,
    RC_GANONS_CASTLE_DEKU_SCRUB_LEFT,
};

const std::vector<RandomizerCheck> shopChecks = { RC_KF_SHOP_ITEM_1,
                                                  RC_KF_SHOP_ITEM_2,
                                                  RC_KF_SHOP_ITEM_3,
                                                  RC_KF_SHOP_ITEM_4,
                                                  RC_KF_SHOP_ITEM_5,
                                                  RC_KF_SHOP_ITEM_6,
                                                  RC_KF_SHOP_ITEM_7,
                                                  RC_KF_SHOP_ITEM_8,
                                                  RC_KAK_POTION_SHOP_ITEM_1,
                                                  RC_KAK_POTION_SHOP_ITEM_2,
                                                  RC_KAK_POTION_SHOP_ITEM_3,
                                                  RC_KAK_POTION_SHOP_ITEM_4,
                                                  RC_KAK_POTION_SHOP_ITEM_5,
                                                  RC_KAK_POTION_SHOP_ITEM_6,
                                                  RC_KAK_POTION_SHOP_ITEM_7,
                                                  RC_KAK_POTION_SHOP_ITEM_8,
                                                  RC_MARKET_BOMBCHU_SHOP_ITEM_1,
                                                  RC_MARKET_BOMBCHU_SHOP_ITEM_2,
                                                  RC_MARKET_BOMBCHU_SHOP_ITEM_3,
                                                  RC_MARKET_BOMBCHU_SHOP_ITEM_4,
                                                  RC_MARKET_BOMBCHU_SHOP_ITEM_5,
                                                  RC_MARKET_BOMBCHU_SHOP_ITEM_6,
                                                  RC_MARKET_BOMBCHU_SHOP_ITEM_7,
                                                  RC_MARKET_BOMBCHU_SHOP_ITEM_8,
                                                  RC_MARKET_POTION_SHOP_ITEM_1,
                                                  RC_MARKET_POTION_SHOP_ITEM_2,
                                                  RC_MARKET_POTION_SHOP_ITEM_3,
                                                  RC_MARKET_POTION_SHOP_ITEM_4,
                                                  RC_MARKET_POTION_SHOP_ITEM_5,
                                                  RC_MARKET_POTION_SHOP_ITEM_6,
                                                  RC_MARKET_POTION_SHOP_ITEM_7,
                                                  RC_MARKET_POTION_SHOP_ITEM_8,
                                                  RC_MARKET_BAZAAR_ITEM_1,
                                                  RC_MARKET_BAZAAR_ITEM_2,
                                                  RC_MARKET_BAZAAR_ITEM_3,
                                                  RC_MARKET_BAZAAR_ITEM_4,
                                                  RC_MARKET_BAZAAR_ITEM_5,
                                                  RC_MARKET_BAZAAR_ITEM_6,
                                                  RC_MARKET_BAZAAR_ITEM_7,
                                                  RC_MARKET_BAZAAR_ITEM_8,
                                                  RC_KAK_BAZAAR_ITEM_1,
                                                  RC_KAK_BAZAAR_ITEM_2,
                                                  RC_KAK_BAZAAR_ITEM_3,
                                                  RC_KAK_BAZAAR_ITEM_4,
                                                  RC_KAK_BAZAAR_ITEM_5,
                                                  RC_KAK_BAZAAR_ITEM_6,
                                                  RC_KAK_BAZAAR_ITEM_7,
                                                  RC_KAK_BAZAAR_ITEM_8,
                                                  RC_ZD_SHOP_ITEM_1,
                                                  RC_ZD_SHOP_ITEM_2,
                                                  RC_ZD_SHOP_ITEM_3,
                                                  RC_ZD_SHOP_ITEM_4,
                                                  RC_ZD_SHOP_ITEM_5,
                                                  RC_ZD_SHOP_ITEM_6,
                                                  RC_ZD_SHOP_ITEM_7,
                                                  RC_ZD_SHOP_ITEM_8,
                                                  RC_GC_SHOP_ITEM_1,
                                                  RC_GC_SHOP_ITEM_2,
                                                  RC_GC_SHOP_ITEM_3,
                                                  RC_GC_SHOP_ITEM_4,
                                                  RC_GC_SHOP_ITEM_5,
                                                  RC_GC_SHOP_ITEM_6,
                                                  RC_GC_SHOP_ITEM_7,
                                                  RC_GC_SHOP_ITEM_8 };

const std::vector<RandomizerCheck> ignoredChecks = { RC_MARKET_TREASURE_CHEST_GAME_ITEM_1,
                                                     RC_MARKET_TREASURE_CHEST_GAME_ITEM_2,
                                                     RC_MARKET_TREASURE_CHEST_GAME_ITEM_3,
                                                     RC_MARKET_TREASURE_CHEST_GAME_ITEM_4,
                                                     RC_MARKET_TREASURE_CHEST_GAME_ITEM_5,
                                                     RC_UNKNOWN_CHECK,
                                                     RC_LINKS_POCKET };

const std::vector<RandomizerCheck> gsChecks = { RC_KF_GS_KNOW_IT_ALL_HOUSE,
                                                RC_KF_GS_BEAN_PATCH,
                                                RC_KF_GS_HOUSE_OF_TWINS,
                                                RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE,
                                                RC_LW_GS_BEAN_PATCH_NEAR_THEATER,
                                                RC_LW_GS_ABOVE_THEATER,
                                                RC_SFM_GS,
                                                RC_HF_GS_COW_GROTTO,
                                                RC_HF_GS_NEAR_KAK_GROTTO,
                                                RC_MARKET_GS_GUARD_HOUSE,
                                                RC_HC_GS_TREE,
                                                RC_HC_GS_STORMS_GROTTO,
                                                RC_LLR_GS_HOUSE_WINDOW,
                                                RC_LLR_GS_TREE,
                                                RC_LLR_GS_RAIN_SHED,
                                                RC_LLR_GS_BACK_WALL,
                                                RC_KAK_GS_TREE,
                                                RC_KAK_GS_GUARDS_HOUSE,
                                                RC_KAK_GS_WATCHTOWER,
                                                RC_KAK_GS_SKULLTULA_HOUSE,
                                                RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION,
                                                RC_KAK_GS_ABOVE_IMPAS_HOUSE,
                                                RC_GRAVEYARD_GS_BEAN_PATCH,
                                                RC_GRAVEYARD_GS_WALL,
                                                RC_DMT_GS_NEAR_KAK,
                                                RC_DMT_GS_BEAN_PATCH,
                                                RC_DMT_GS_ABOVE_DODONGOS_CAVERN,
                                                RC_DMT_GS_FALLING_ROCKS_PATH,
                                                RC_GC_GS_CENTER_PLATFORM,
                                                RC_GC_GS_BOULDER_MAZE,
                                                RC_DMC_GS_CRATE,
                                                RC_DMC_GS_BEAN_PATCH,
                                                RC_ZR_GS_TREE,
                                                RC_ZR_GS_LADDER,
                                                RC_ZR_GS_NEAR_RAISED_GROTTOS,
                                                RC_ZR_GS_ABOVE_BRIDGE,
                                                RC_ZD_GS_FROZEN_WATERFALL,
                                                RC_ZF_GS_ABOVE_THE_LOG,
                                                RC_ZF_GS_TREE,
                                                RC_ZF_GS_HIDDEN_CAVE,
                                                RC_LH_GS_BEAN_PATCH,
                                                RC_LH_GS_LAB_WALL,
                                                RC_LH_GS_SMALL_ISLAND,
                                                RC_LH_GS_LAB_CRATE,
                                                RC_LH_GS_TREE,
                                                RC_GV_GS_SMALL_BRIDGE,
                                                RC_GV_GS_BEAN_PATCH,
                                                RC_GV_GS_BEHIND_TENT,
                                                RC_GV_GS_PILLAR,
                                                RC_GF_GS_TOP_FLOOR,
                                                RC_GF_GS_ARCHERY_RANGE,
                                                RC_WASTELAND_GS,
                                                RC_COLOSSUS_GS_BEAN_PATCH,
                                                RC_COLOSSUS_GS_TREE,
                                                RC_COLOSSUS_GS_HILL,
                                                RC_OGC_GS,
                                                RC_DEKU_TREE_GS_COMPASS_ROOM,
                                                RC_DEKU_TREE_GS_BASEMENT_VINES,
                                                RC_DEKU_TREE_GS_BASEMENT_GATE,
                                                RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM,
                                                RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS,
                                                RC_DODONGOS_CAVERN_GS_SCARECROW,
                                                RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS,
                                                RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS,
                                                RC_DODONGOS_CAVERN_GS_BACK_ROOM,
                                                RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM,
                                                RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER,
                                                RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER,
                                                RC_JABU_JABUS_BELLY_GS_NEAR_BOSS,
                                                RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM,
                                                RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM,
                                                RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE,
                                                RC_FOREST_TEMPLE_GS_FIRST_ROOM,
                                                RC_FOREST_TEMPLE_GS_LOBBY,
                                                RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD,
                                                RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD,
                                                RC_FOREST_TEMPLE_GS_BASEMENT,
                                                RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP,
                                                RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM,
                                                RC_FIRE_TEMPLE_GS_BOULDER_MAZE,
                                                RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB,
                                                RC_FIRE_TEMPLE_GS_SCARECROW_TOP,
                                                RC_WATER_TEMPLE_GS_BEHIND_GATE,
                                                RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST,
                                                RC_WATER_TEMPLE_GS_CENTRAL_PILLAR,
                                                RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM,
                                                RC_WATER_TEMPLE_GS_RIVER,
                                                RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM,
                                                RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM,
                                                RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT,
                                                RC_SHADOW_TEMPLE_GS_NEAR_SHIP,
                                                RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT,
                                                RC_SPIRIT_TEMPLE_GS_METAL_FENCE,
                                                RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM,
                                                RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM,
                                                RC_SPIRIT_TEMPLE_GS_LOBBY,
                                                RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM,
                                                RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM,
                                                RC_ICE_CAVERN_GS_HEART_PIECE_ROOM,
                                                RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM };

// business scrubs without a major item
const std::vector<RandomizerCheck> scrubChecks = {
    RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT,
    RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT,
    RC_LW_DEKU_SCRUB_GROTTO_REAR,
    RC_SFM_DEKU_SCRUB_GROTTO_FRONT,
    RC_SFM_DEKU_SCRUB_GROTTO_REAR,
    RC_LLR_DEKU_SCRUB_GROTTO_LEFT,
    RC_LLR_DEKU_SCRUB_GROTTO_CENTER,
    RC_LLR_DEKU_SCRUB_GROTTO_RIGHT,
    RC_GC_DEKU_SCRUB_GROTTO_LEFT,
    RC_GC_DEKU_SCRUB_GROTTO_CENTER,
    RC_GC_DEKU_SCRUB_GROTTO_RIGHT,
    RC_DMC_DEKU_SCRUB,
    RC_DMC_DEKU_SCRUB_GROTTO_LEFT,
    RC_DMC_DEKU_SCRUB_GROTTO_CENTER,
    RC_DMC_DEKU_SCRUB_GROTTO_RIGHT,
    RC_ZR_DEKU_SCRUB_GROTTO_FRONT,
    RC_ZR_DEKU_SCRUB_GROTTO_REAR,
    RC_LH_DEKU_SCRUB_GROTTO_LEFT,
    RC_LH_DEKU_SCRUB_GROTTO_CENTER,
    RC_LH_DEKU_SCRUB_GROTTO_RIGHT,
    RC_GV_DEKU_SCRUB_GROTTO_FRONT,
    RC_GV_DEKU_SCRUB_GROTTO_REAR,
    RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT,
    RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR,
    RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS,
    RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY,
    RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,
    RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT,
    RC_JABU_JABUS_BELLY_DEKU_SCRUB,
    RC_GANONS_CASTLE_DEKU_SCRUB_LEFT,
    RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,
    RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT,
    RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT,
};

const std::vector<RandomizerCheck> cowChecks = { RC_KF_LINKS_HOUSE_COW,    RC_HF_COW_GROTTO_COW,
                                                 RC_HF_GS_COW_GROTTO,      RC_LLR_STABLES_LEFT_COW,
                                                 RC_LLR_STABLES_RIGHT_COW, RC_LLR_TOWER_LEFT_COW,
                                                 RC_LLR_TOWER_RIGHT_COW,   RC_KAK_IMPAS_HOUSE_COW,
                                                 RC_DMT_COW_GROTTO_COW,    RC_GV_COW };

bool isShopCheck(const RandomizerCheck check) {
    if (std::find(shopChecks.begin(), shopChecks.end(), check) != shopChecks.end()) {
        return true;
    }
    return false;
}

bool isIgnoredCheck(const RandomizerCheck check) {
    if (std::find(ignoredChecks.begin(), ignoredChecks.end(), check) != ignoredChecks.end()) {
        return true;
    }
    return false;
}

bool isGsCheck(const RandomizerCheck check) {
    if (std::find(gsChecks.begin(), gsChecks.end(), check) != gsChecks.end()) {
        return true;
    }
    return false;
}

bool isScrubCheck(const RandomizerCheck check) {
    if (std::find(scrubChecks.begin(), scrubChecks.end(), check) != scrubChecks.end()) {
        return true;
    }
    return false;
}

bool isCowCheck(const RandomizerCheck check) {
    if (std::find(cowChecks.begin(), cowChecks.end(), check) != cowChecks.end()) {
        return true;
    }
    return false;
}

bool isCheckInCurrentRegion(const RandomizerCheck check) {
    switch (currentRegion) {
        case RR_UNKNOWN:
            return std::find(std::begin(checks_RR_UNKNOWN), std::end(checks_RR_UNKNOWN), check) !=
                   std::end(checks_RR_UNKNOWN);
        case RR_KF:
            return std::find(std::begin(checks_RR_KF), std::end(checks_RR_KF), check) != std::end(checks_RR_KF);
        case RR_LW:
            return std::find(std::begin(checks_RR_LW), std::end(checks_RR_LW), check) != std::end(checks_RR_LW);
        case RR_HF:
            return std::find(std::begin(checks_RR_HF), std::end(checks_RR_HF), check) != std::end(checks_RR_HF);
        case RR_LH:
            return std::find(std::begin(checks_RR_LH), std::end(checks_RR_LH), check) != std::end(checks_RR_LH);
        case RR_GV:
            return std::find(std::begin(checks_RR_GV), std::end(checks_RR_GV), check) != std::end(checks_RR_GV);
        case RR_GF:
            return std::find(std::begin(checks_RR_GF), std::end(checks_RR_GF), check) != std::end(checks_RR_GF);
        case RR_WASTELAND:
            return std::find(std::begin(checks_RR_WASTELAND), std::end(checks_RR_WASTELAND), check) !=
                   std::end(checks_RR_WASTELAND);
        case RR_COLOSSUS:
            return std::find(std::begin(checks_RR_COLOSSUS), std::end(checks_RR_COLOSSUS), check) !=
                   std::end(checks_RR_COLOSSUS);
        case RR_MARKET:
            return std::find(std::begin(checks_RR_MARKET), std::end(checks_RR_MARKET), check) !=
                   std::end(checks_RR_MARKET);
        case RR_HC:
            return std::find(std::begin(checks_RR_HC), std::end(checks_RR_HC), check) != std::end(checks_RR_HC);
        case RR_KAK:
            return std::find(std::begin(checks_RR_KAK), std::end(checks_RR_KAK), check) != std::end(checks_RR_KAK);
        case RR_GRAVEYARD:
            return std::find(std::begin(checks_RR_GRAVEYARD), std::end(checks_RR_GRAVEYARD), check) !=
                   std::end(checks_RR_GRAVEYARD);
        case RR_DMT:
            return std::find(std::begin(checks_RR_DMT), std::end(checks_RR_DMT), check) != std::end(checks_RR_DMT);
        case RR_GC:
            return std::find(std::begin(checks_RR_GC), std::end(checks_RR_GC), check) != std::end(checks_RR_GC);
        case RR_DMC:
            return std::find(std::begin(checks_RR_DMC), std::end(checks_RR_DMC), check) != std::end(checks_RR_DMC);
        case RR_ZR:
            return std::find(std::begin(checks_RR_ZR), std::end(checks_RR_ZR), check) != std::end(checks_RR_ZR);
        case RR_ZD:
            return std::find(std::begin(checks_RR_ZD), std::end(checks_RR_ZD), check) != std::end(checks_RR_ZD);
        case RR_ZF:
            return std::find(std::begin(checks_RR_ZF), std::end(checks_RR_ZF), check) != std::end(checks_RR_ZF);
        case RR_LLR:
            return std::find(std::begin(checks_RR_LLR), std::end(checks_RR_LLR), check) != std::end(checks_RR_LLR);
        case RR_SFM:
            return std::find(std::begin(checks_RR_SFM), std::end(checks_RR_SFM), check) != std::end(checks_RR_SFM);
        case RR_GTG:
            return std::find(std::begin(checks_RR_GTG), std::end(checks_RR_GTG), check) != std::end(checks_RR_GTG);
        case RR_DEKU:
            return std::find(std::begin(checks_RR_DEKU), std::end(checks_RR_DEKU), check) != std::end(checks_RR_DEKU);
        case RR_DC:
            return std::find(std::begin(checks_RR_DC), std::end(checks_RR_DC), check) != std::end(checks_RR_DC);
        case RR_JABU:
            return std::find(std::begin(checks_RR_JABU), std::end(checks_RR_JABU), check) != std::end(checks_RR_JABU);
        case RR_FOREST:
            return std::find(std::begin(checks_RR_FOREST), std::end(checks_RR_FOREST), check) !=
                   std::end(checks_RR_FOREST);
        case RR_FIRE:
            return std::find(std::begin(checks_RR_FIRE), std::end(checks_RR_FIRE), check) != std::end(checks_RR_FIRE);
        case RR_WATER:
            return std::find(std::begin(checks_RR_WATER), std::end(checks_RR_WATER), check) !=
                   std::end(checks_RR_WATER);
        case RR_SHADOW:
            return std::find(std::begin(checks_RR_SHADOW), std::end(checks_RR_SHADOW), check) !=
                   std::end(checks_RR_SHADOW);
        case RR_SPIRIT:
            return std::find(std::begin(checks_RR_SPIRIT), std::end(checks_RR_SPIRIT), check) !=
                   std::end(checks_RR_SPIRIT);
        case RR_ICE:
            return std::find(std::begin(checks_RR_ICE), std::end(checks_RR_ICE), check) != std::end(checks_RR_ICE);
        case RR_BOTW:
            return std::find(std::begin(checks_RR_BOTW), std::end(checks_RR_BOTW), check) != std::end(checks_RR_BOTW);
        case RR_TOT:
            return std::find(std::begin(checks_RR_TOT), std::end(checks_RR_TOT), check) != std::end(checks_RR_TOT);
        case RR_OGC:
            return std::find(std::begin(checks_RR_OGC), std::end(checks_RR_OGC), check) != std::end(checks_RR_OGC);
        case RR_GANONS_CASTLE:
            return std::find(std::begin(checks_RR_GANONS_CASTLE), std::end(checks_RR_GANONS_CASTLE), check) !=
                   std::end(checks_RR_GANONS_CASTLE);
    };

    return false;
}

bool DoesFileExist(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

const std::string trackerSaveFilePath =
    Ship::GlobalCtx2::GetPathRelativeToAppDirectory("Randomizer") + "/checktracker_save.json";

static bool checks[500];
static bool showSpoilers = false;
static bool showChecked = false;
static bool showGs = false;
static bool showSelectedRegion = false;
static bool showAll = false;
static int selectedRegion = 0;
static int currentRegionCheckCount = 0;
static int currentRegionCheckedCheckCount = 0;

void drawCheck(int i) {
    // Skip checked Check
    if (!showChecked && checks[i]) {
        currentRegionCheckCount++;
        currentRegionCheckedCheckCount++;
        return;
    }

    // Skip shop items, Scrubs, Cows (if set so) and Golden Skulltulas (if set so)
    if (isIgnoredCheck(gSaveContext.itemLocations[i].check) || isShopCheck(gSaveContext.itemLocations[i].check) ||
        (!showGs && isGsCheck(gSaveContext.itemLocations[i].check)) ||
        isScrubCheck(gSaveContext.itemLocations[i].check) ||
        (!OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_COWS) &&
         isCowCheck(gSaveContext.itemLocations[i].check))) {
        return;
    }

    bool checked = checks[i];
    if (checked) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 100));
    }
    ImGui::Checkbox(CheckEnumToName[gSaveContext.itemLocations[i].check].c_str(), &checks[i]);
    if (checked) {
        ImGui::PopStyleColor();
    }

    if (showSpoilers) {
        ImGui::SameLine();
        if (checks[i])
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 150, 0, 100));
        else
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 185, 0, 255));
        ImGui::Text("(%s)", GetEnumToName[gSaveContext.itemLocations[i].get].c_str());
        ImGui::PopStyleColor();
    }

    currentRegionCheckCount++;
    if (checks[i])
        currentRegionCheckedCheckCount++;

    ImGui::Dummy(ImVec2(0.0f, 0.5f));
}

void DrawTracker() {
    if (gGlobalCtx == nullptr)
        return;

    static std::string lastActionString = "";
    const char* regionStrings[36] = {
        "Current",
        "Show All",
        RegionToString[RR_KF].c_str(),
        RegionToString[RR_LW].c_str(),
        RegionToString[RR_HF].c_str(),
        RegionToString[RR_LH].c_str(),
        RegionToString[RR_GV].c_str(),
        RegionToString[RR_GF].c_str(),
        RegionToString[RR_WASTELAND].c_str(),
        RegionToString[RR_COLOSSUS].c_str(),
        RegionToString[RR_MARKET].c_str(),
        RegionToString[RR_HC].c_str(),
        RegionToString[RR_KAK].c_str(),
        RegionToString[RR_GRAVEYARD].c_str(),
        RegionToString[RR_DMT].c_str(),
        RegionToString[RR_GC].c_str(),
        RegionToString[RR_DMC].c_str(),
        RegionToString[RR_ZR].c_str(),
        RegionToString[RR_ZD].c_str(),
        RegionToString[RR_ZF].c_str(),
        RegionToString[RR_LLR].c_str(),
        RegionToString[RR_SFM].c_str(),
        RegionToString[RR_GTG].c_str(),
        RegionToString[RR_DEKU].c_str(),
        RegionToString[RR_DC].c_str(),
        RegionToString[RR_JABU].c_str(),
        RegionToString[RR_FOREST].c_str(),
        RegionToString[RR_FIRE].c_str(),
        RegionToString[RR_WATER].c_str(),
        RegionToString[RR_SHADOW].c_str(),
        RegionToString[RR_SPIRIT].c_str(),
        RegionToString[RR_ICE].c_str(),
        RegionToString[RR_BOTW].c_str(),
        RegionToString[RR_TOT].c_str(),
        RegionToString[RR_OGC].c_str(),
        RegionToString[RR_GANONS_CASTLE].c_str(),
    };

    if (ImGui::BeginTabBar("Check Tracker", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
        if (ImGui::BeginTabItem("Check Tracker")) {
            ImGui::Dummy(ImVec2(0.0f, 4.0f));

            if (ImGui::BeginTable("tableCheckTrackerOptions", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Options", ImGuiTableColumnFlags_WidthStretch, 150.0f);
                ImGui::TableHeadersRow();
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                ImGui::Dummy(ImVec2(0.0f, 1.0f));

                ImGui::Checkbox("Show Checked", &showChecked);
                ImGui::SameLine();
                ImGui::Checkbox("Show Skulltulas", &showGs);
                ImGui::SameLine();
                ImGui::Checkbox("Show Spoilers", &showSpoilers);

                ImGui::Dummy(ImVec2(0.0f, 1.0f));
                ImGui::Text("Select Region:");
                ImGui::SameLine();
                SohImGui::EnhancementCombobox("gCheckTrackerSelectedRegion", regionStrings, 36, 0);
                ImGui::Dummy(ImVec2(0.0f, 1.0f));

                ImGui::EndTable();
            }

            ImGui::Dummy(ImVec2(0.0f, 4.0f));

            // get currentRegion from Combobox or gGlobalCtx->sceneNum, or show all
            selectedRegion = (RandomizerRegion)CVar_GetS32("gCheckTrackerSelectedRegion", 0);
            showAll = false;
            if (selectedRegion == 1) {
                showAll = true;
            } else if (selectedRegion > 1) {
                currentRegion = static_cast<RandomizerRegion>(selectedRegion + -1);
            } else {
                setRegionByCurrentSceneID();
            }

            std::string checksCountString =
                std::to_string(currentRegionCheckedCheckCount) + "/" + std::to_string(currentRegionCheckCount);
            std::string checksTitleString =
                showAll ? "All Checks (" + checksCountString + "):"
                        : "Checks in " + RegionToString[currentRegion] + " (" + checksCountString + "):";

            currentRegionCheckCount = 0;
            currentRegionCheckedCheckCount = 0;

            if (ImGui::BeginTable("tableCheckTrackerChecks", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn(checksTitleString.c_str(), ImGuiTableColumnFlags_WidthStretch, 150.0f);
                ImGui::TableHeadersRow();
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Dummy(ImVec2(0.0f, 1.0f));

                for (int i = 0; i < 500; i++) {
                    if (showAll || isCheckInCurrentRegion(gSaveContext.itemLocations[i].check)) {
                        drawCheck(i);
                    }
                }

                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Save/Load/Reset")) {

            ImGui::Dummy(ImVec2(0.0f, 4.0f));

            if (ImGui::BeginTable("tableCheckTrackerSave", 1, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
                ImGui::TableSetupColumn("Tracker Data", ImGuiTableColumnFlags_WidthStretch, 150.0f);
                ImGui::TableHeadersRow();
                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                ImGui::Dummy(ImVec2(0.0f, 1.0f));

                if (ImGui::Button("Reset")) {
                    for (int i = 0; i < 500; i++) {
                        checks[i] = false;
                        lastActionString = "Reseted Tracker Data.";
                    }
                }
                InsertHelpHoverText("Resets all Checks to unchecked.");
                ImGui::SameLine();
                if (ImGui::Button("Load")) {
                    if (DoesFileExist(trackerSaveFilePath)) {
                        std::ifstream ifs(trackerSaveFilePath);
                        nlohmann::json trackerJsonData = nlohmann::json::parse(ifs);
                        int i = 0;
                        for (auto it = trackerJsonData.begin(); it != trackerJsonData.end(); ++it) {
                            checks[i] = it.value();
                            i++;
                        }
                        lastActionString = "Loaded: " + trackerSaveFilePath;
                    }
                }
                InsertHelpHoverText("Load saved Tracker Data from file.");
                ImGui::SameLine();
                if (ImGui::Button("Save")) {
                    nlohmann::json trackerJsonSave = nlohmann::json::array();
                    for (int i = 0; i < 500; i++) {
                        trackerJsonSave.push_back(checks[i]);
                    }
                    std::ofstream output(trackerSaveFilePath);
                    output << std::setw(4) << trackerJsonSave << std::endl;
                    lastActionString = "Saved: " + trackerSaveFilePath;
                }
                InsertHelpHoverText("Save current Tracker Data to file.");
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 125));
                ImGui::Text(lastActionString.c_str());
                ImGui::PopStyleColor();
                ImGui::Dummy(ImVec2(0.0f, 1.0f));

                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }
    }
    ImGui::EndTabBar();
}

void DrawCheckTracker(bool& open) {
    if (!open) {
        CVar_SetS32("gCheckTrackerEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Check Tracker", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    DrawTracker();

    ImGui::End();
}

void InitCheckTracker() {
    SohImGui::AddWindow("Randomizer", "Check Tracker", DrawCheckTracker);
}