#include "ActorBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>

#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/ObjectExtension/ActorListIndex.h"
#include "2s2h/ObjectExtension/ObjectExtension.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "assets/2s2h_assets.h"
#include "assets/overlays/ovl_Obj_Grass/ovl_Obj_Grass.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Obj_Grass/z_obj_grass.h"
#include "overlays/actors/ovl_En_Kusa/z_en_kusa.h"
#include "overlays/actors/ovl_En_Kusa2/z_en_kusa2.h"
#include "overlays/actors/ovl_Obj_Grass_Carry/z_obj_grass_carry.h"
#include "overlays/actors/ovl_Obj_Mure2/z_obj_mure2.h"

void ObjGrass_OverrideMatrixCurrent(MtxF* matrix);
void EnKusa_DrawGrass(Actor* thisx, PlayState* play);
extern ObjGrass* sGrassManager;
}

struct KeatonGrassRing {
    RandoCheckId baseCheckId = RC_UNKNOWN;
    RandoCheckId wonderItemCheckId = RC_UNKNOWN;
};
static ObjectExtension::Register<KeatonGrassRing> keatonGrassRingRegister;

// clang-format off
// For large swathes of grass that share a singular actor
std::map<std::tuple<s16, u8>, std::tuple<s16, RandoCheckId>> grassObjMap = {
    { { SCENE_00KEIKOKU, 0 }, { 18, RC_TERMINA_FIELD_GRASS_01 } },
    { { SCENE_F01, 0 }, { 5, RC_ROMANI_RANCH_GRASS_01 } },
    { { SCENE_20SICHITAI2, 0 }, { 1, RC_SOUTHERN_SWAMP_CLEARED_GRASS_01 } },
    { { SCENE_20SICHITAI2, 2 }, { 1, RC_SOUTHERN_SWAMP_CLEARED_GRASS_13 } },
    { { SCENE_20SICHITAI, 0 }, { 1, RC_SOUTHERN_SWAMP_POISON_GRASS_01 } },
    { { SCENE_20SICHITAI, 2 }, { 1, RC_SOUTHERN_SWAMP_POISON_GRASS_13 } },
    { { SCENE_24KEMONOMITI, 0 }, { 2, RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_03 } },
    { { SCENE_17SETUGEN2, 0 }, { 1, RC_TWIN_ISLANDS_SPRING_GRASS_01 } },
    { { SCENE_KOEPONARACE, 0 }, { 2, RC_GORMAN_TRACK_GRASS_01 } },
    { { SCENE_KAKUSIANA, 5 }, { 2, RC_LONE_PEAK_SHRINE_GRASS_01 } },
    { { SCENE_KAKUSIANA, 12 }, { 1, RC_DEKU_PALACE_BEAN_SALESMAN_GROTTO_GRASS_01 } },
    { { SCENE_KAKUSIANA, 13 }, { 1, RC_TERMINA_FIELD_PEAHAT_GROTTO_GRASS_01 } },
};

std::map<s8, std::tuple<s16, RandoCheckId>> cowGrottoMap = {
    { -1, { 6, RC_GREAT_BAY_COAST_COW_GROTTO_GRASS_01 } },
    { 31, { 6, RC_TERMINA_FIELD_COW_GROTTO_GRASS_01 } },
};

// For lone grass actors spawned directly by the scene
std::map<std::tuple<s16, s16, s16>, RandoCheckId> enKusaMap = {
    // Great Bay Coast (pre-temple clear)
    { { SCENE_30GYOSON, 0, 110 }, RC_GREAT_BAY_COAST_GRASS_01 },
    { { SCENE_30GYOSON, 0, 111 }, RC_GREAT_BAY_COAST_GRASS_02 },
    { { SCENE_30GYOSON, 0, 112 }, RC_GREAT_BAY_COAST_GRASS_03 },
    { { SCENE_30GYOSON, 0, 113 }, RC_GREAT_BAY_COAST_GRASS_04 },
    // Great Bay Coast (post-temple clear)
    { { SCENE_30GYOSON, 0, 117 }, RC_GREAT_BAY_COAST_GRASS_01 },
    { { SCENE_30GYOSON, 0, 115 }, RC_GREAT_BAY_COAST_GRASS_02 },
    { { SCENE_30GYOSON, 0, 118 }, RC_GREAT_BAY_COAST_GRASS_03 },
    { { SCENE_30GYOSON, 0, 116 }, RC_GREAT_BAY_COAST_GRASS_04 },
    { { SCENE_30GYOSON, 0, 114 }, RC_GREAT_BAY_COAST_GRASS_05 }, // Exists in both states
    // Laundry Pool
    { { SCENE_ALLEY, 0, 13 }, RC_CLOCK_TOWN_LAUNDRY_POOL_GRASS_01 },
    { { SCENE_ALLEY, 0, 14 }, RC_CLOCK_TOWN_LAUNDRY_POOL_GRASS_02 },
    { { SCENE_ALLEY, 0, 15 }, RC_CLOCK_TOWN_LAUNDRY_POOL_GRASS_03 },
    // Road to Southern Swamp
    { { SCENE_24KEMONOMITI, 0, 47 }, RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_01 },
    { { SCENE_24KEMONOMITI, 0, 48 }, RC_ROAD_TO_SOUTHERN_SWAMP_GRASS_02 },
    // Mountain Village (spring)
    { { SCENE_10YUKIYAMANOMURA2, 0, 26 }, RC_MOUNTAIN_VILLAGE_SPRING_GRASS_01 },
    { { SCENE_10YUKIYAMANOMURA2, 0, 27 }, RC_MOUNTAIN_VILLAGE_SPRING_GRASS_02 },
    { { SCENE_10YUKIYAMANOMURA2, 0, 28 }, RC_MOUNTAIN_VILLAGE_SPRING_GRASS_03 },
    // Woods of Mystery
    { { SCENE_26SARUNOMORI, 1, 3 }, RC_WOODS_OF_MYSTERY_GRASS_01 },
    { { SCENE_26SARUNOMORI, 1, 4 }, RC_WOODS_OF_MYSTERY_GRASS_02 },
    { { SCENE_26SARUNOMORI, 1, 5 }, RC_WOODS_OF_MYSTERY_GRASS_03 },
    { { SCENE_26SARUNOMORI, 0, 0 }, RC_WOODS_OF_MYSTERY_GRASS_04 },
    { { SCENE_26SARUNOMORI, 0, 1 }, RC_WOODS_OF_MYSTERY_GRASS_05 },
    { { SCENE_26SARUNOMORI, 3, 7 }, RC_WOODS_OF_MYSTERY_GRASS_06 },
    { { SCENE_26SARUNOMORI, 3, 8 }, RC_WOODS_OF_MYSTERY_GRASS_07 },
    { { SCENE_26SARUNOMORI, 4, 6 }, RC_WOODS_OF_MYSTERY_GRASS_08 },
    { { SCENE_26SARUNOMORI, 4, 7 }, RC_WOODS_OF_MYSTERY_GRASS_09 },
    { { SCENE_26SARUNOMORI, 4, 8 }, RC_WOODS_OF_MYSTERY_GRASS_10 },
    { { SCENE_26SARUNOMORI, 4, 9 }, RC_WOODS_OF_MYSTERY_GRASS_11 },
    { { SCENE_26SARUNOMORI, 5, 7 }, RC_WOODS_OF_MYSTERY_GRASS_12 },
    { { SCENE_26SARUNOMORI, 5, 8 }, RC_WOODS_OF_MYSTERY_GRASS_13 },
    { { SCENE_26SARUNOMORI, 8, 1 }, RC_WOODS_OF_MYSTERY_GRASS_14 },
    { { SCENE_26SARUNOMORI, 8, 2 }, RC_WOODS_OF_MYSTERY_GRASS_15 },
    { { SCENE_26SARUNOMORI, 8, 3 }, RC_WOODS_OF_MYSTERY_GRASS_16 },
    { { SCENE_26SARUNOMORI, 8, 4 }, RC_WOODS_OF_MYSTERY_GRASS_17 },
    { { SCENE_26SARUNOMORI, 8, 6 }, RC_WOODS_OF_MYSTERY_GRASS_18 },
    { { SCENE_26SARUNOMORI, 7, 7 }, RC_WOODS_OF_MYSTERY_GRASS_19 },
    { { SCENE_26SARUNOMORI, 7, 8 }, RC_WOODS_OF_MYSTERY_GRASS_20 },
    { { SCENE_26SARUNOMORI, 2, 1 }, RC_WOODS_OF_MYSTERY_GRASS_21 },
    { { SCENE_26SARUNOMORI, 6, 1 }, RC_WOODS_OF_MYSTERY_GRASS_22 },
    { { SCENE_26SARUNOMORI, 6, 2 }, RC_WOODS_OF_MYSTERY_GRASS_23 },
    // Woodfall
    { { SCENE_21MITURINMAE, 0, 18 }, RC_WOODFALL_GRASS_01 },
    { { SCENE_21MITURINMAE, 0, 17 }, RC_WOODFALL_GRASS_02 },
    { { SCENE_21MITURINMAE, 0, 19 }, RC_WOODFALL_GRASS_03 },
    { { SCENE_21MITURINMAE, 0, 22 }, RC_WOODFALL_GRASS_04 },
    { { SCENE_21MITURINMAE, 0, 21 }, RC_WOODFALL_GRASS_05 },
    { { SCENE_21MITURINMAE, 0, 20 }, RC_WOODFALL_GRASS_06 },
    // Woodfall (Clear)
    { { SCENE_21MITURINMAE, 0, 10 }, RC_WOODFALL_GRASS_01 },
    { { SCENE_21MITURINMAE, 0, 9 }, RC_WOODFALL_GRASS_02 },
    { { SCENE_21MITURINMAE, 0, 11 }, RC_WOODFALL_GRASS_03 },
    { { SCENE_21MITURINMAE, 0, 14 }, RC_WOODFALL_GRASS_04 },
    { { SCENE_21MITURINMAE, 0, 13 }, RC_WOODFALL_GRASS_05 },
    { { SCENE_21MITURINMAE, 0, 12 }, RC_WOODFALL_GRASS_06 },
    // Grottos (unique)
    { { SCENE_KAKUSIANA, 0, 1 }, RC_TERMINA_FIELD_GOSSIP_STONE_GROTTO_3_GRASS_01 },
    { { SCENE_KAKUSIANA, 0, 4 }, RC_TERMINA_FIELD_GOSSIP_STONE_GROTTO_3_GRASS_02 },
    { { SCENE_KAKUSIANA, 0, 6 }, RC_TERMINA_FIELD_GOSSIP_STONE_GROTTO_3_GRASS_03 },
    { { SCENE_KAKUSIANA, 0, 7 }, RC_TERMINA_FIELD_GOSSIP_STONE_GROTTO_3_GRASS_04 },
    { { SCENE_KAKUSIANA, 0, 8 }, RC_TERMINA_FIELD_GOSSIP_STONE_GROTTO_3_GRASS_05 },
    { { SCENE_KAKUSIANA, 2, 0 }, RC_TERMINA_FIELD_GOSSIP_STONE_GROTTO_4_GRASS_01 },
    { { SCENE_KAKUSIANA, 2, 1 }, RC_TERMINA_FIELD_GOSSIP_STONE_GROTTO_4_GRASS_02 },
    { { SCENE_KAKUSIANA, 2, 2 }, RC_TERMINA_FIELD_GOSSIP_STONE_GROTTO_4_GRASS_03 },
    { { SCENE_KAKUSIANA, 2, 3 }, RC_TERMINA_FIELD_GOSSIP_STONE_GROTTO_4_GRASS_04 },
    { { SCENE_KAKUSIANA, 2, 4 }, RC_TERMINA_FIELD_GOSSIP_STONE_GROTTO_4_GRASS_05 },
    { { SCENE_KAKUSIANA, 11, 0 }, RC_TERMINA_FIELD_BIO_BABA_GROTTO_GRASS_01 },
    { { SCENE_KAKUSIANA, 11, 3 }, RC_TERMINA_FIELD_BIO_BABA_GROTTO_GRASS_02 },
    // Sprout grass
    { { SCENE_20SICHITAI, 0, 45 }, RC_SOUTHERN_SWAMP_POISON_TOURIST_GRASS_01 },
    { { SCENE_20SICHITAI, 0, 46 }, RC_SOUTHERN_SWAMP_POISON_TOURIST_GRASS_02 },
    { { SCENE_20SICHITAI, 2, 17 }, RC_SOUTHERN_SWAMP_POISON_WOODS_GRASS_01 },
    { { SCENE_20SICHITAI, 2, 18 }, RC_SOUTHERN_SWAMP_POISON_WOODS_GRASS_02 },
    { { SCENE_20SICHITAI2, 0, 39 }, RC_SOUTHERN_SWAMP_CLEARED_TOURIST_GRASS_01 },
    { { SCENE_20SICHITAI2, 0, 40 }, RC_SOUTHERN_SWAMP_CLEARED_TOURIST_GRASS_02 },
    { { SCENE_BOTI, 0, 16 }, RC_IKANA_GRAVEYARD_LOWER_GRASS_04 },
    { { SCENE_BOTI, 0, 17 }, RC_IKANA_GRAVEYARD_LOWER_GRASS_03 },
    { { SCENE_BOTI, 0, 18 }, RC_IKANA_GRAVEYARD_LOWER_GRASS_01 },
    { { SCENE_BOTI, 0, 19 }, RC_IKANA_GRAVEYARD_LOWER_GRASS_02 },
    { { SCENE_BOTI, 0, 20 }, RC_IKANA_GRAVEYARD_LOWER_GRASS_05 },
    { { SCENE_BOTI, 0, 31 }, RC_IKANA_GRAVEYARD_LOWER_GRASS_01 },
    { { SCENE_BOTI, 0, 39 }, RC_IKANA_GRAVEYARD_LOWER_GRASS_02 },
    { { SCENE_BOTI, 0, 40 }, RC_IKANA_GRAVEYARD_LOWER_GRASS_03 },
    { { SCENE_BOTI, 0, 41 }, RC_IKANA_GRAVEYARD_LOWER_GRASS_04 },
    { { SCENE_BOTI, 0, 42 }, RC_IKANA_GRAVEYARD_LOWER_GRASS_05 },
    { { SCENE_CASTLE, 0, 32 }, RC_ANCIENT_CASTLE_OF_IKANA_COURTYARD_GRASS_01 },
    { { SCENE_CASTLE, 0, 33 }, RC_ANCIENT_CASTLE_OF_IKANA_COURTYARD_GRASS_02 },
    { { SCENE_CASTLE, 0, 34 }, RC_ANCIENT_CASTLE_OF_IKANA_COURTYARD_GRASS_03 },
    { { SCENE_CASTLE, 0, 35 }, RC_ANCIENT_CASTLE_OF_IKANA_COURTYARD_GRASS_04 },
    { { SCENE_CASTLE, 0, 36 }, RC_ANCIENT_CASTLE_OF_IKANA_COURTYARD_GRASS_05 },
    { { SCENE_CASTLE, 0, 37 }, RC_ANCIENT_CASTLE_OF_IKANA_COURTYARD_GRASS_06 },
    { { SCENE_CASTLE, 0, 38 }, RC_ANCIENT_CASTLE_OF_IKANA_COURTYARD_GRASS_07 },
    { { SCENE_CASTLE, 0, 39 }, RC_ANCIENT_CASTLE_OF_IKANA_COURTYARD_GRASS_08 },
    { { SCENE_CASTLE, 0, 40 }, RC_ANCIENT_CASTLE_OF_IKANA_COURTYARD_GRASS_09 },
    { { SCENE_CASTLE, 0, 41 }, RC_ANCIENT_CASTLE_OF_IKANA_COURTYARD_GRASS_10 },
    { { SCENE_CASTLE, 0, 42 }, RC_ANCIENT_CASTLE_OF_IKANA_COURTYARD_GRASS_11 },
    { { SCENE_CASTLE, 0, 43 }, RC_ANCIENT_CASTLE_OF_IKANA_COURTYARD_GRASS_12 },
    { { SCENE_HAKUGIN, 4, 60 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_GRASS_01 },
    { { SCENE_HAKUGIN, 4, 61 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_GRASS_02 },
    { { SCENE_HAKUGIN, 4, 62 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_GRASS_03 },
    { { SCENE_HAKUGIN, 4, 63 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_GRASS_04 },
    { { SCENE_HAKUGIN, 4, 64 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_GRASS_05 },
    { { SCENE_HAKUGIN, 4, 65 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_GRASS_06 },
    { { SCENE_HAKUGIN, 4, 66 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_GRASS_07 },
    { { SCENE_HAKUGIN, 4, 67 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_GRASS_08 },
    { { SCENE_HAKUGIN, 4, 68 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_GRASS_09 },
    { { SCENE_HAKUGIN, 4, 69 }, RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_GRASS_10 },
    { { SCENE_IKANA, 0, 29 }, RC_IKANA_CANYON_GRASS_01 },
    { { SCENE_IKANA, 0, 30 }, RC_IKANA_CANYON_GRASS_02 },
    { { SCENE_IKANA, 0, 31 }, RC_IKANA_CANYON_GRASS_03 },
    { { SCENE_IKANA, 0, 32 }, RC_IKANA_CANYON_GRASS_04 },
    { { SCENE_IKANA, 0, 59 }, RC_IKANA_CANYON_GRASS_01 },
    { { SCENE_IKANA, 0, 60 }, RC_IKANA_CANYON_GRASS_02 },
    { { SCENE_IKANA, 0, 61 }, RC_IKANA_CANYON_GRASS_03 },
    { { SCENE_IKANA, 0, 62 }, RC_IKANA_CANYON_GRASS_04 },
    { { SCENE_INISIE_N, 0, 10 }, RC_STONE_TOWER_TEMPLE_ENTRANCE_GRASS_01 },
    { { SCENE_INISIE_N, 0, 11 }, RC_STONE_TOWER_TEMPLE_ENTRANCE_GRASS_02 },
    { { SCENE_INISIE_N, 0, 12 }, RC_STONE_TOWER_TEMPLE_ENTRANCE_GRASS_03 },
    { { SCENE_INISIE_N, 2, 18 }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_GRASS_01 },
    { { SCENE_INISIE_N, 2, 19 }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_GRASS_02 },
    { { SCENE_INISIE_N, 2, 20 }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_GRASS_03 },
    { { SCENE_INISIE_N, 2, 21 }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_GRASS_04 },
    { { SCENE_INISIE_N, 2, 22 }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_GRASS_05 },
    { { SCENE_INISIE_N, 2, 23 }, RC_STONE_TOWER_TEMPLE_SWITCH_ROOM_GRASS_06 },
    { { SCENE_MITURIN, 0, 30 }, RC_WOODFALL_TEMPLE_PRE_BOSS_GRASS_01 },
    { { SCENE_MITURIN, 0, 31 }, RC_WOODFALL_TEMPLE_PRE_BOSS_GRASS_02 },
    { { SCENE_MITURIN, 0, 32 }, RC_WOODFALL_TEMPLE_PRE_BOSS_GRASS_03 },
    { { SCENE_MITURIN, 0, 33 }, RC_WOODFALL_TEMPLE_PRE_BOSS_GRASS_04 },
    { { SCENE_MITURIN, 0, 34 }, RC_WOODFALL_TEMPLE_PRE_BOSS_GRASS_05 },
    { { SCENE_MITURIN, 1, 30 }, RC_WOODFALL_TEMPLE_MAIN_ROOM_LOWER_GRASS_01 },
    { { SCENE_MITURIN, 1, 32 }, RC_WOODFALL_TEMPLE_MAIN_ROOM_LOWER_GRASS_02 },
    { { SCENE_MITURIN, 1, 31 }, RC_WOODFALL_TEMPLE_MAIN_ROOM_UPPER_GRASS_01 },
    { { SCENE_MITURIN, 2, 23 }, RC_WOODFALL_TEMPLE_ENTRANCE_GRASS_01 },
    { { SCENE_MITURIN, 2, 24 }, RC_WOODFALL_TEMPLE_ENTRANCE_GRASS_02 },
    { { SCENE_MITURIN, 2, 25 }, RC_WOODFALL_TEMPLE_ENTRANCE_GRASS_04 },
    { { SCENE_MITURIN, 2, 26 }, RC_WOODFALL_TEMPLE_ENTRANCE_GRASS_05 },
    { { SCENE_MITURIN, 2, 27 }, RC_WOODFALL_TEMPLE_ENTRANCE_GRASS_03 },
    { { SCENE_MITURIN, 4, 5 }, RC_WOODFALL_TEMPLE_COMPASS_ROOM_GRASS_01 },
    { { SCENE_MITURIN, 4, 6 }, RC_WOODFALL_TEMPLE_COMPASS_ROOM_GRASS_02 },
    { { SCENE_MITURIN, 4, 7 }, RC_WOODFALL_TEMPLE_COMPASS_ROOM_GRASS_03 },
    { { SCENE_MITURIN, 5, 18 }, RC_WOODFALL_TEMPLE_WATER_ROOM_GRASS_01 },
    { { SCENE_MITURIN, 5, 19 }, RC_WOODFALL_TEMPLE_WATER_ROOM_GRASS_02 },
    { { SCENE_MITURIN, 6, 6 }, RC_WOODFALL_TEMPLE_MAP_ROOM_GRASS_01 },
    { { SCENE_MITURIN, 6, 7 }, RC_WOODFALL_TEMPLE_MAP_ROOM_GRASS_02 },
    { { SCENE_MITURIN, 6, 8 }, RC_WOODFALL_TEMPLE_MAP_ROOM_GRASS_03 },
    { { SCENE_MITURIN, 6, 9 }, RC_WOODFALL_TEMPLE_MAP_ROOM_GRASS_04 },
    { { SCENE_MITURIN, 6, 10 }, RC_WOODFALL_TEMPLE_MAP_ROOM_GRASS_05 },
    { { SCENE_MITURIN, 10, 6 }, RC_WOODFALL_TEMPLE_UPPER_WALKWAY_GRASS_01 },
    { { SCENE_MITURIN, 10, 7 }, RC_WOODFALL_TEMPLE_UPPER_WALKWAY_GRASS_02 },
    { { SCENE_MITURIN, 10, 8 }, RC_WOODFALL_TEMPLE_UPPER_WALKWAY_GRASS_03 },
    { { SCENE_MITURIN, 10, 9 }, RC_WOODFALL_TEMPLE_UPPER_WALKWAY_GRASS_04 },
    { { SCENE_MITURIN, 10, 10 }, RC_WOODFALL_TEMPLE_UPPER_WALKWAY_GRASS_05 },
    { { SCENE_MITURIN, 10, 11 }, RC_WOODFALL_TEMPLE_UPPER_WALKWAY_GRASS_06 },
    { { SCENE_MITURIN, 10, 12 }, RC_WOODFALL_TEMPLE_UPPER_WALKWAY_GRASS_07 },
    { { SCENE_MITURIN, 10, 13 }, RC_WOODFALL_TEMPLE_UPPER_WALKWAY_GRASS_08 },
    { { SCENE_MITURIN, 10, 14 }, RC_WOODFALL_TEMPLE_UPPER_WALKWAY_GRASS_09 },
    { { SCENE_MITURIN, 10, 15 }, RC_WOODFALL_TEMPLE_UPPER_WALKWAY_GRASS_10 },
    { { SCENE_MITURIN, 10, 16 }, RC_WOODFALL_TEMPLE_UPPER_WALKWAY_GRASS_11 },
    { { SCENE_MITURIN_BS, 0, 10 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_01 },
    { { SCENE_MITURIN_BS, 0, 11 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_02 },
    { { SCENE_MITURIN_BS, 0, 12 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_03 },
    { { SCENE_MITURIN_BS, 0, 13 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_04 },
    { { SCENE_MITURIN_BS, 0, 14 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_05 },
    { { SCENE_MITURIN_BS, 0, 15 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_06 },
    { { SCENE_MITURIN_BS, 0, 16 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_07 },
    { { SCENE_MITURIN_BS, 0, 17 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_08 },
    { { SCENE_MITURIN_BS, 0, 18 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_09 },
    { { SCENE_MITURIN_BS, 0, 19 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_10 },
    { { SCENE_MITURIN_BS, 0, 20 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_11 },
    { { SCENE_MITURIN_BS, 0, 21 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_12 },
    { { SCENE_MITURIN_BS, 0, 22 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_13 },
    { { SCENE_MITURIN_BS, 0, 23 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_14 },
    { { SCENE_MITURIN_BS, 0, 24 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_15 },
    { { SCENE_MITURIN_BS, 0, 25 }, RC_WOODFALL_TEMPLE_BOSS_GRASS_16 },
    { { SCENE_RANDOM, 0, 26 }, RC_SECRET_SHRINE_ENTRANCE_GRASS_01 },
    { { SCENE_RANDOM, 0, 27 }, RC_SECRET_SHRINE_ENTRANCE_GRASS_02 },
    { { SCENE_RANDOM, 0, 28 }, RC_SECRET_SHRINE_ENTRANCE_GRASS_03 },
    { { SCENE_RANDOM, 0, 29 }, RC_SECRET_SHRINE_ENTRANCE_GRASS_04 },
    { { SCENE_RANDOM, 0, 30 }, RC_SECRET_SHRINE_ENTRANCE_GRASS_05 },
    { { SCENE_RANDOM, 0, 31 }, RC_SECRET_SHRINE_ENTRANCE_GRASS_06 },
    { { SCENE_RANDOM, 2, 4 }, RC_SECRET_SHRINE_DINOLFOS_GRASS_01 },
    { { SCENE_RANDOM, 2, 5 }, RC_SECRET_SHRINE_DINOLFOS_GRASS_02 },
    { { SCENE_RANDOM, 2, 6 }, RC_SECRET_SHRINE_DINOLFOS_GRASS_03 },
    { { SCENE_RANDOM, 2, 7 }, RC_SECRET_SHRINE_DINOLFOS_GRASS_04 },
    { { SCENE_RANDOM, 3, 6 }, RC_SECRET_SHRINE_WIZZROBE_GRASS_01 },
    { { SCENE_RANDOM, 3, 7 }, RC_SECRET_SHRINE_WIZZROBE_GRASS_02 },
    { { SCENE_RANDOM, 3, 8 }, RC_SECRET_SHRINE_WIZZROBE_GRASS_03 },
    { { SCENE_RANDOM, 3, 9 }, RC_SECRET_SHRINE_WIZZROBE_GRASS_04 },
    { { SCENE_RANDOM, 3, 10 }, RC_SECRET_SHRINE_WIZZROBE_GRASS_05 },
    { { SCENE_RANDOM, 4, 5 }, RC_SECRET_SHRINE_WART_GRASS_01 },
    { { SCENE_RANDOM, 4, 6 }, RC_SECRET_SHRINE_WART_GRASS_02 },
    { { SCENE_RANDOM, 4, 7 }, RC_SECRET_SHRINE_WART_GRASS_03 },
    { { SCENE_RANDOM, 4, 8 }, RC_SECRET_SHRINE_WART_GRASS_04 },
    { { SCENE_RANDOM, 4, 9 }, RC_SECRET_SHRINE_WART_GRASS_05 },
    { { SCENE_RANDOM, 4, 10 }, RC_SECRET_SHRINE_WART_GRASS_06 },
    { { SCENE_RANDOM, 4, 11 }, RC_SECRET_SHRINE_WART_GRASS_07 },
    { { SCENE_RANDOM, 4, 12 }, RC_SECRET_SHRINE_WART_GRASS_08 },
    { { SCENE_RANDOM, 5, 2 }, RC_SECRET_SHRINE_GARO_MASTER_GRASS_01 },
    { { SCENE_RANDOM, 5, 3 }, RC_SECRET_SHRINE_GARO_MASTER_GRASS_02 },
    { { SCENE_RANDOM, 5, 4 }, RC_SECRET_SHRINE_GARO_MASTER_GRASS_03 },
    { { SCENE_RANDOM, 5, 5 }, RC_SECRET_SHRINE_GARO_MASTER_GRASS_04 },
    { { SCENE_RANDOM, 5, 6 }, RC_SECRET_SHRINE_GARO_MASTER_GRASS_05 },
    { { SCENE_RANDOM, 5, 7 }, RC_SECRET_SHRINE_GARO_MASTER_GRASS_06 },
    { { SCENE_ROMANYMAE, 0, 17 }, RC_MILK_ROAD_GRASS_01 },
    { { SCENE_ROMANYMAE, 0, 18 }, RC_MILK_ROAD_GRASS_02 },
    { { SCENE_ROMANYMAE, 0, 19 }, RC_MILK_ROAD_GRASS_03 },
    { { SCENE_20SICHITAI2, 2, 14 }, RC_SOUTHERN_SWAMP_CLEARED_WOODS_GRASS_01 },
    { { SCENE_20SICHITAI2, 2, 15 }, RC_SOUTHERN_SWAMP_CLEARED_WOODS_GRASS_02 },
    { { SCENE_REDEAD, 3, 9 }, RC_BENEATH_THE_WELL_RIGHT_FIRE_KEESE_GRASS_01 },
    { { SCENE_REDEAD, 3, 10 }, RC_BENEATH_THE_WELL_RIGHT_FIRE_KEESE_GRASS_02 },
    { { SCENE_REDEAD, 3, 11 }, RC_BENEATH_THE_WELL_RIGHT_FIRE_KEESE_GRASS_03 },
    { { SCENE_REDEAD, 3, 12 }, RC_BENEATH_THE_WELL_RIGHT_FIRE_KEESE_GRASS_04 },
    { { SCENE_REDEAD, 5, 22 }, RC_BENEATH_THE_WELL_TWO_SPIKED_BARS_GRASS_01 },
    { { SCENE_REDEAD, 5, 23 }, RC_BENEATH_THE_WELL_TWO_SPIKED_BARS_GRASS_02 },
    { { SCENE_REDEAD, 7, 19 }, RC_BENEATH_THE_WELL_FOUR_SPIKED_BARS_GRASS_01 },
    { { SCENE_REDEAD, 7, 20 }, RC_BENEATH_THE_WELL_FOUR_SPIKED_BARS_GRASS_02 },
    { { SCENE_REDEAD, 7, 21 }, RC_BENEATH_THE_WELL_FOUR_SPIKED_BARS_GRASS_03 },
    { { SCENE_REDEAD, 7, 22 }, RC_BENEATH_THE_WELL_FOUR_SPIKED_BARS_GRASS_04 },
    { { SCENE_REDEAD, 7, 23 }, RC_BENEATH_THE_WELL_FOUR_SPIKED_BARS_GRASS_05 },
    { { SCENE_REDEAD, 9, 6 }, RC_BENEATH_THE_WELL_COW_ROOM_GRASS_01 },
    { { SCENE_REDEAD, 9, 7 }, RC_BENEATH_THE_WELL_COW_ROOM_GRASS_02 },
    { { SCENE_REDEAD, 9, 8 }, RC_BENEATH_THE_WELL_COW_ROOM_GRASS_03 },
};

// For lone grass actors spawned in chest grottos
std::map<s8, RandoCheckId> chestGrottoMap = {    
    {   51, RC_PATH_TO_SNOWHEAD_GROTTO_GRASS_01 },
    {   55, RC_GREAT_BAY_COAST_FISHERMAN_GROTTO_GRASS_01 },
    {   59, RC_MOUNTAIN_VILLAGE_TUNNEL_GROTTO_GRASS_01 },
    {   61, RC_SOUTHERN_SWAMP_GROTTO_GRASS_01 },
    {   62, RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO_GRASS_01 },
    {   63, RC_TERMINA_FIELD_TALL_GRASS_GROTTO_GRASS_01 },
    {  -72, RC_IKANA_GRAVEYARD_GROTTO_GRASS_01 },
    {  -76, RC_IKANA_CANYON_GROTTO_GRASS_01 },
    {   92, RC_WOODS_OF_MYSTERY_GROTTO_GRASS_01 },
    { -102, RC_TERMINA_FIELD_PILLAR_GROTTO_GRASS_01 },
    { -103, RC_TWIN_ISLANDS_RAMP_GROTTO_GRASS_01 },
    { -106, RC_ROAD_TO_IKANA_GROTTO_GRASS_01 },
    { -107, RC_ZORA_CAPE_GROTTO_GRASS_01 },
};

constexpr s8 chestGrottoActorIdsToBaseRc[20] = { 0, 1, 2, 3, -1, 4, 5, -1, -1, -1, 6, 7, 8, -1, 9, -1, 10, 11, 12, 13 };

// For batches of grass spawned by Obj_Mure2
std::map<std::tuple<u16, u8, s16>, RandoCheckId> objMure2GrassMap = {
    { { SCENE_10YUKIYAMANOMURA2, 0, 29 }, RC_MOUNTAIN_VILLAGE_SPRING_GRASS_04 },
    { { SCENE_10YUKIYAMANOMURA2, 0, 30 }, RC_MOUNTAIN_VILLAGE_SPRING_GRASS_13 },
    { { SCENE_10YUKIYAMANOMURA2, 0, 31 }, RC_MOUNTAIN_VILLAGE_SPRING_GRASS_22 },
    { { SCENE_BOTI, 1, 47 }, RC_IKANA_GRAVEYARD_GRASS_01 },
    { { SCENE_BOTI, 1, 7 }, RC_IKANA_GRAVEYARD_GRASS_01 }, // Night layer
};

// For rings of Keaton grass, mapped by their manager actor to the first RC of the ring and the RC of its bonus reward
std::map<std::tuple<s16, s16, s16>, KeatonGrassRing> keatonGrassMap = {
    { { SCENE_BACKTOWN, 0, 16 }, { RC_CLOCK_TOWN_NORTH_KEATON_GRASS_01, RC_CLOCK_TOWN_NORTH_KEATON_GRASS_WONDER_ITEM } },
    { { SCENE_BACKTOWN, 0, 28 }, { RC_CLOCK_TOWN_NORTH_KEATON_GRASS_01, RC_CLOCK_TOWN_NORTH_KEATON_GRASS_WONDER_ITEM } },
    { { SCENE_ROMANYMAE, 0, 6 }, { RC_MILK_ROAD_KEATON_GRASS_01, RC_MILK_ROAD_KEATON_GRASS_WONDER_ITEM } },
    { { SCENE_10YUKIYAMANOMURA2, 0, 44 }, { RC_MOUNTAIN_VILLAGE_SPRING_KEATON_GRASS_01, RC_MOUNTAIN_VILLAGE_SPRING_KEATON_GRASS_WONDER_ITEM } },
    { { SCENE_10YUKIYAMANOMURA2, 0, 50 }, { RC_MOUNTAIN_VILLAGE_SPRING_KEATON_GRASS_01, RC_MOUNTAIN_VILLAGE_SPRING_KEATON_GRASS_WONDER_ITEM } },
};
// clang-format on

void SpawnGrassDrop(Vec3f pos, RandoCheckId randoCheckId) {
    CustomItem::Spawn(
        pos.x, pos.y, pos.z, 0, CustomItem::KILL_ON_TOUCH | CustomItem::TOSS_ON_SPAWN | CustomItem::ABLE_TO_ZORA_RANG,
        randoCheckId,
        [](Actor* actor, PlayState* play) {
            RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            randoSaveCheck.eligible = true;
        },
        [](Actor* actor, PlayState* play) {
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
            RandoItemId randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId);
            Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
            Rando::DrawItem(Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM),
                            (RandoCheckId)CUSTOM_ITEM_PARAM, actor);
        });
}

Gfx* GetObjGrassDList(RandoCheckId randoCheckId) {
    if (!CVarGetInteger("gRando.CSMC", 0)) {
        return (Gfx*)gRandoBushDL;
    }

    RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId);
    RandoItemType randoItemType = Rando::StaticData::Items[randoItemId].randoItemType;

    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
            return (Gfx*)gRandoBushBossKeyDL;
            break;
        case RITYPE_HEALTH:
            return (Gfx*)gRandoBushHeartDL;
            break;
        case RITYPE_LESSER:
            return (Gfx*)gRandoBushMinorDL;
            break;
        case RITYPE_MAJOR:
            return (Gfx*)gRandoBushMajorDL;
            break;
        case RITYPE_MASK:
            return (Gfx*)gRandoBushMaskDL;
            break;
        case RITYPE_SKULLTULA_TOKEN:
            return (Gfx*)gRandoBushTokenDL;
            break;
        case RITYPE_SMALL_KEY:
            return (Gfx*)gRandoBushSmallKeyDL;
            break;
        case RITYPE_STRAY_FAIRY:
            return (Gfx*)gRandoBushFairyDL;
            break;
        case RITYPE_JUNK:
            return (Gfx*)gRandoBushJunkDL;
            break;
        default:
            return (Gfx*)gRandoBushDL;
            break;
    }
}

Gfx* GetObjGrassXluDList(RandoCheckId randoCheckId) {
    if (!CVarGetInteger("gRando.CSMC", 0)) {
        return (Gfx*)gRandoBushXluDL;
    }

    RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId);
    RandoItemType randoItemType = Rando::StaticData::Items[randoItemId].randoItemType;

    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
            return (Gfx*)gRandoBushBossKeyXluDL;
            break;
        case RITYPE_HEALTH:
            return (Gfx*)gRandoBushHeartXluDL;
            break;
        case RITYPE_LESSER:
            return (Gfx*)gRandoBushMinorXluDL;
            break;
        case RITYPE_MAJOR:
            return (Gfx*)gRandoBushMajorXluDL;
            break;
        case RITYPE_MASK:
            return (Gfx*)gRandoBushMaskXluDL;
            break;
        case RITYPE_SKULLTULA_TOKEN:
            return (Gfx*)gRandoBushTokenXluDL;
            break;
        case RITYPE_SMALL_KEY:
            return (Gfx*)gRandoBushSmallKeyXluDL;
            break;
        case RITYPE_STRAY_FAIRY:
            return (Gfx*)gRandoBushFairyXluDL;
            break;
        case RITYPE_JUNK:
            return (Gfx*)gRandoBushJunkXluDL;
            break;
        default:
            return (Gfx*)gRandoBushXluDL;
            break;
    }
}

// Cuttable grass has its own set of models, as the bush ones above are far too large to stand in for a sprout.
Gfx* GetCuttableGrassDList(RandoCheckId randoCheckId) {
    if (!CVarGetInteger("gRando.CSMC", 0)) {
        return (Gfx*)gRandoCuttableGrassRandomDL;
    }

    RandoItemId randoItemId = Rando::ConvertItem(RANDO_SAVE_CHECKS[randoCheckId].randoItemId, randoCheckId);
    RandoItemType randoItemType = Rando::StaticData::Items[randoItemId].randoItemType;

    switch (randoItemType) {
        case RITYPE_BOSS_KEY:
            return (Gfx*)gRandoCuttableGrassBossKeyDL;
            break;
        case RITYPE_HEALTH:
            return (Gfx*)gRandoCuttableGrassHeartDL;
            break;
        case RITYPE_LESSER:
            return (Gfx*)gRandoCuttableGrassMinorDL;
            break;
        case RITYPE_MAJOR:
            return (Gfx*)gRandoCuttableGrassMajorDL;
            break;
        case RITYPE_MASK:
            return (Gfx*)gRandoCuttableGrassMaskDL;
            break;
        case RITYPE_SKULLTULA_TOKEN:
            return (Gfx*)gRandoCuttableGrassTokenDL;
            break;
        case RITYPE_SMALL_KEY:
            return (Gfx*)gRandoCuttableGrassSmallKeyDL;
            break;
        case RITYPE_STRAY_FAIRY:
            return (Gfx*)gRandoCuttableGrassFairyDL;
            break;
        case RITYPE_JUNK:
            return (Gfx*)gRandoCuttableGrassJunkDL;
            break;
        default:
            return (Gfx*)gRandoCuttableGrassRandomDL;
            break;
    }
}

void ObjGrass_RandoDrawOpa(ObjGrass* objGrass, ObjGrassElement* grassElem, s32 j, RandoCheckId randoCheckId) {
    Vec3s rot = { 0, 0, 0 };
    OPEN_DISPS(gPlayState->state.gfxCtx);
    rot.y = grassElem->rotY;
    Matrix_SetTranslateRotateYXZ(grassElem->pos.x, grassElem->pos.y, grassElem->pos.z, &rot);
    Matrix_Scale(objGrass->actor.scale.x, objGrass->actor.scale.y, objGrass->actor.scale.z, MTXMODE_APPLY);
    if (grassElem->flags & OBJ_GRASS_ELEM_ANIM) {
        ObjGrass_OverrideMatrixCurrent(&objGrass->distortionMtx[j]);
    }

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gPlayState->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, GetObjGrassDList(randoCheckId));
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gObjGrass_D_809AA9F0);
    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void ObjGrass_RandoDrawXlu(ObjGrass* objGrass, ObjGrassElement* grassElem, RandoCheckId randoCheckId) {
    Vec3s rot = { 0, 0, 0 };
    OPEN_DISPS(gPlayState->state.gfxCtx);
    rot.y = grassElem->rotY;
    Matrix_SetTranslateRotateYXZ(grassElem->pos.x, grassElem->pos.y, grassElem->pos.z, &rot);
    Matrix_Scale(objGrass->actor.scale.x, objGrass->actor.scale.y, objGrass->actor.scale.z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gPlayState->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, grassElem->alpha);
    gSPDisplayList(POLY_XLU_DISP++, GetObjGrassXluDList(randoCheckId));
    gSPDisplayList(POLY_XLU_DISP++, (Gfx*)gObjGrass_D_809AAA68);
    CLOSE_DISPS(gPlayState->state.gfxCtx);
}

void EnKusaBush_RandoDraw(Actor* actor, PlayState* play) {
    Gfx_DrawDListOpa(play, GetObjGrassDList(Rando::ActorBehavior::GetObjectRandoCheckId(actor)));
}

void EnKusaGrass_RandoDraw(Actor* actor, PlayState* play) {
    EnKusa* grass = (EnKusa*)actor;
    RandoCheckId randoCheckId = Rando::ActorBehavior::GetObjectRandoCheckId(actor);

    if (RANDO_SAVE_CHECKS[randoCheckId].obtained) {
        actor->draw = EnKusa_DrawGrass;
        EnKusa_DrawGrass(actor, play);
        return;
    }

    if (grass->isCut) {
        EnKusa_DrawGrass(actor, play);
        return;
    }

    Gfx_DrawDListOpa(play, GetCuttableGrassDList(randoCheckId));
}

void KeatonGrassRing_DrawWonderItemSparkle(EnKusa2* grassRingActor) {
    if (gGameState->frames % 4 != 0) {
        return;
    }

    EnKusa2* grassBush = grassRingActor->unk_194[(s32)Rand_ZeroFloat(ARRAY_COUNT(grassRingActor->unk_194))];
    if (grassBush == nullptr) {
        return;
    }

    Rando::ActorBehavior::SpawnWonderItemSparkle(&grassBush->actor.world.pos);
}

void Rando::ActorBehavior::InitObjGrassBehavior() {
    /*
     * Identify directly spawned actor grass by scene ID, room, and actor list index. If this is a common chest grotto,
     * use respawn data to retrieve the base RC. The grass actors and RCs are both in contiguous order, so the base RC
     * can be incremented to get each grass actor's target RC value.
     */
    COND_ID_HOOK(OnActorInit, ACTOR_EN_KUSA, IS_RANDO, [](Actor* actor) {
        s16 actorListIndex = GetActorListIndex(actor);
        if (actorListIndex < 0) { // This grass was placed by a spawner, not scene data
            return;
        }

        RandoCheckId randoCheckId = RC_UNKNOWN;
        if (gPlayState->sceneId == SCENE_KAKUSIANA && actor->room == 4) { // Common chest grotto
            auto it = chestGrottoMap.find(gSaveContext.respawn[RESPAWN_MODE_UNK_3].data);
            if (it != chestGrottoMap.end()) {
                randoCheckId = static_cast<RandoCheckId>(it->second + chestGrottoActorIdsToBaseRc[actorListIndex]);
            }
        } else {
            auto it = enKusaMap.find({ gPlayState->sceneId, actor->room, actorListIndex });
            if (it != enKusaMap.end()) {
                randoCheckId = it->second;
            }
        }

        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            return;
        }

        SetObjectRandoCheckId(actor, randoCheckId);
    });

    /*
     * Identify the manager actor of a ring of Keaton grass by scene ID, room, and actor list index, and remember which
     * RCs the ring owns. Its bushes are spawned later, and each of them claims one of those RCs as it attaches.
     */
    COND_ID_HOOK(OnActorInit, ACTOR_EN_KUSA2, IS_RANDO, [](Actor* actor) {
        if (ENKUSA2_GET_1(actor)) { // This is one of the ring's bushes, not the manager
            return;
        }

        auto it = keatonGrassMap.find({ gPlayState->sceneId, actor->room, GetActorListIndex(actor) });
        if (it == keatonGrassMap.end()) {
            return;
        }

        ObjectExtension::GetInstance().Set<KeatonGrassRing>(actor, KeatonGrassRing{ it->second });
    });

    // Only a ring's manager actor is given this extension, so this skips the ring's bushes along with all other grass.
    COND_ID_HOOK(OnActorUpdate, ACTOR_EN_KUSA2, IS_RANDO, [](Actor* actor) {
        KeatonGrassRing* grassRing = ObjectExtension::GetInstance().Get<KeatonGrassRing>(actor);
        if (grassRing == nullptr) {
            return;
        }

        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[grassRing->wonderItemCheckId];
        if (randoSaveCheck.shuffled && !randoSaveCheck.obtained) {
            KeatonGrassRing_DrawWonderItemSparkle((EnKusa2*)actor);
        }
    });

    /*
     * Identify actor grass that was spawned by a spawner actor, by scene ID, room, and the spawner's actor list index.
     * The RCs and child grass are contiguous, so they can increment the base value to get their target RC.
     */

    COND_VB_SHOULD(VB_OBJ_MURE2_SET_CHILD_ROOM, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        ObjMure2* objMure2 = (ObjMure2*)actor;
        s32 i = va_arg(args, s32);
        Actor* child = objMure2->actors[i];
        if (child != nullptr && child->id == ACTOR_EN_KUSA) {
            auto it = objMure2GrassMap.find({ gPlayState->sceneId, actor->room, GetActorListIndex(actor) });
            if (it != objMure2GrassMap.end()) {
                RandoCheckId randoCheckId = static_cast<RandoCheckId>(it->second + i);
                if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
                    return;
                }
                SetObjectRandoCheckId(child, randoCheckId);
            }
        }
    });

    /*
     * This handles non-actor grass, which can be complicated to follow. Scenes define Obj_Grass_Unit, which itself has
     * multiple grass children elements. Once a grass unit is done initializing, its child elements get added to a
     * singleton ObjGrass manager to handle all the grass elements in the scene and room. The grass unit itself gets
     * killed.
     *
     * This hook checks that, upon killing a grass object, whether it is the last one for the scene. If it is, it then
     * consults the ObjGrass manager singleton to iterate its children and grandchildren for each ObjGrassElement to
     * assign its respective RC. RCs are derived from the scene ID and room, as there is only one ObjGrass manager per
     * scene. Cow grottos use respawn data instead. The order that grass elements are processed is deterministic each
     * load and contiguous, so we can just increment the base RC like always.
     */
    COND_ID_HOOK(OnActorKill, ACTOR_OBJ_GRASS_UNIT, IS_RANDO, [](Actor* actor) {
        s16 maxActiveGrassGroups = 0;
        RandoCheckId baseCheckId;

        if (gPlayState->sceneId == SCENE_KAKUSIANA && actor->room == 10) { // Cow grottos
            auto it = cowGrottoMap.find(gSaveContext.respawn[RESPAWN_MODE_UNK_3].data);
            if (it == cowGrottoMap.end()) {
                return;
            }
            maxActiveGrassGroups = std::get<s16>(it->second);
            baseCheckId = std::get<RandoCheckId>(it->second);
        } else {
            auto it = grassObjMap.find({ gPlayState->sceneId, actor->room });
            if (it == grassObjMap.end()) {
                return;
            }
            maxActiveGrassGroups = std::get<s16>(it->second);
            baseCheckId = std::get<RandoCheckId>(it->second);
        }

        if (sGrassManager->activeGrassGroups < maxActiveGrassGroups) {
            return;
        }

        ObjGrassGroup* grassGroup;
        int grassIndex = 0;
        for (int i = 0; i < sGrassManager->activeGrassGroups; i++) {
            grassGroup = &sGrassManager->grassGroups[i];
            for (int j = 0; j < grassGroup->count; j++) {
                RandoCheckId randoCheckId = RandoCheckId(baseCheckId + grassIndex++);
                if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
                    continue;
                }
                SetObjectRandoCheckId(&grassGroup->elements[j], randoCheckId);
            }
        }
    });

    // If actor grass was not spawned directly in the scene, we must manually free the extension RC.
    COND_ID_HOOK(OnActorDestroy, ACTOR_EN_KUSA, IS_RANDO, [](Actor* actor) {
        if (GetActorListIndex(actor) < 0) {
            ObjectExtension_Free(actor);
        }
    });

    // There should only be one of this actor active at any time. Iterate its grandchildren and free the extension.
    COND_ID_HOOK(OnActorDestroy, ACTOR_OBJ_GRASS, IS_RANDO, [](Actor* actor) {
        ObjGrassGroup* grassGroup;
        ObjGrass* objGrass = (ObjGrass*)actor;
        for (int i = 0; i < objGrass->activeGrassGroups; i++) {
            grassGroup = &objGrass->grassGroups[i];
            for (int j = 0; j < grassGroup->count; j++) {
                ObjectExtension_Free(&grassGroup->elements[j]);
            }
        }
    });

    COND_VB_SHOULD(VB_KUSA_DRAW_BE_OVERRIDDEN, IS_RANDO, {
        Actor* actor = va_arg(args, Actor*);
        RandoCheckId randoCheckId = GetObjectRandoCheckId(actor);
        if (randoCheckId != RC_UNKNOWN && !RANDO_SAVE_CHECKS[randoCheckId].obtained) {
            *should = false;
            actor->draw = KUSA_GET_TYPE(actor) == ENKUSA_TYPE_BUSH ? EnKusaBush_RandoDraw : EnKusaGrass_RandoDraw;
        }
    });

    /*
     * A ring's bushes always spawn in the same positions, so a bush's index within the ring is enough to tie it to a
     * specific RC. The RCs are contiguous, so the ring's base value can just be incremented.
     */
    COND_VB_SHOULD(VB_KEATON_GRASS_ATTACH_CHILD, IS_RANDO, {
        EnKusa2* grassBush = va_arg(args, EnKusa2*);
        s32 i = va_arg(args, s32);
        KeatonGrassRing* grassRing = ObjectExtension::GetInstance().Get<KeatonGrassRing>(grassBush->unk_1C0);

        if (grassRing == nullptr) {
            return;
        }

        RandoCheckId randoCheckId = static_cast<RandoCheckId>(grassRing->baseCheckId + i);
        if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].cycleObtained) {
            return;
        }

        SetObjectRandoCheckId(grassBush, randoCheckId);
    });

    COND_VB_SHOULD(VB_KEATON_GRASS_DRAW, IS_RANDO, {
        EnKusa2* grassBush = va_arg(args, EnKusa2*);
        RandoCheckId randoCheckId = GetObjectRandoCheckId(grassBush);
        if (randoCheckId != RC_UNKNOWN && !RANDO_SAVE_CHECKS[randoCheckId].obtained) {
            *should = false;
            Gfx_DrawDListOpa(gPlayState, GetObjGrassDList(randoCheckId));
        }
    });

    COND_VB_SHOULD(VB_OBJGRASS_OPA_DRAW_BE_OVERRIDDEN, IS_RANDO, {
        ObjGrass* objGrass = va_arg(args, ObjGrass*);
        ObjGrassElement* grassElem = va_arg(args, ObjGrassElement*);
        s32 j = va_arg(args, s32);
        RandoCheckId randoCheckId = GetObjectRandoCheckId(grassElem);
        if (randoCheckId != RC_UNKNOWN && !RANDO_SAVE_CHECKS[randoCheckId].obtained) {
            *should = false;
            ObjGrass_RandoDrawOpa(objGrass, grassElem, j, randoCheckId);
        }
    });

    COND_VB_SHOULD(VB_OBJGRASS_XLU_DRAW_BE_OVERRIDDEN, IS_RANDO, {
        ObjGrass* objGrass = va_arg(args, ObjGrass*);
        ObjGrassElement* grassElem = va_arg(args, ObjGrassElement*);
        RandoCheckId randoCheckId = GetObjectRandoCheckId(grassElem);
        if (randoCheckId != RC_UNKNOWN && !RANDO_SAVE_CHECKS[randoCheckId].obtained) {
            *should = false;
            ObjGrass_RandoDrawXlu(objGrass, grassElem, randoCheckId);
        }
    });

    COND_VB_SHOULD(VB_CARRY_GRASS_DRAW_BE_OVERRIDDEN, IS_RANDO, {
        ObjGrassCarry* grassCarryActor = va_arg(args, ObjGrassCarry*);
        Actor* actor = &grassCarryActor->actor;
        RandoCheckId randoCheckId = GetObjectRandoCheckId(grassCarryActor->grassElem);
        SetObjectRandoCheckId(actor, randoCheckId);
        if (randoCheckId != RC_UNKNOWN) {
            *should = false;
            grassCarryActor->actor.draw = EnKusaBush_RandoDraw;
        }
    });

    // The grass has been destroyed, so spawn a collectible item based on the grass's RC value.
    COND_VB_SHOULD(VB_GRASS_DROP_COLLECTIBLE, IS_RANDO, {
        auto actorId = static_cast<ActorId>(va_arg(args, int32_t));
        Vec3f collectiblePos = gZeroVec3f;
        RandoCheckId randoCheckId = RC_UNKNOWN;

        if (actorId == ACTOR_OBJ_GRASS) {
            ObjGrassElement* grassElemActor = va_arg(args, ObjGrassElement*);
            collectiblePos = grassElemActor->pos;
            randoCheckId = GetObjectRandoCheckId(grassElemActor);
        } else if (actorId == ACTOR_EN_KUSA) {
            EnKusa* kusaActor = va_arg(args, EnKusa*);
            collectiblePos = kusaActor->actor.world.pos;
            randoCheckId = GetObjectRandoCheckId(kusaActor);
        } else if (actorId == ACTOR_OBJ_GRASS_CARRY) {
            ObjGrassCarry* grassCarryActor = va_arg(args, ObjGrassCarry*);
            collectiblePos = grassCarryActor->actor.world.pos;
            randoCheckId = GetObjectRandoCheckId(grassCarryActor);
        } else if (actorId == ACTOR_EN_KUSA2) {
            EnKusa2* grassBush = va_arg(args, EnKusa2*);
            collectiblePos = grassBush->actor.world.pos;
            randoCheckId = GetObjectRandoCheckId(grassBush);

            // This bush is the one vanilla would have dropped the ring's red rupee from, so it also owes the check
            // that took that reward's place.
            EnKusa2* grassRingActor = grassBush->unk_1C0;
            KeatonGrassRing* grassRing = ObjectExtension::GetInstance().Get<KeatonGrassRing>(grassRingActor);
            if (grassRing != nullptr && grassRingActor->unk_1BC == 8 &&
                RANDO_SAVE_CHECKS[grassRing->wonderItemCheckId].shuffled &&
                !RANDO_SAVE_CHECKS[grassRing->wonderItemCheckId].cycleObtained) {
                SpawnGrassDrop(collectiblePos, grassRing->wonderItemCheckId);
                *should = false;
            }
        }

        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        SpawnGrassDrop(collectiblePos, randoCheckId);
        *should = false;
    });
}
