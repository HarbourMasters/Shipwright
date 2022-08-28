#include "randomizer_identify_actor.h"
#include "randomizer_check_objects.h"
#include "randomizer_inf.h"
#include "randomizerTypes.h"
#include <map>

// There has been some talk about potentially just using the RC identifier to store flags rather than randomizer inf, so
// for now we're not going to store randomzierInf in the randomizer check objects, we're just going to map them 1:1 here
std::map<RandomizerCheck, RandomizerInf> rcToRandomizerInf = {
    { RC_KF_LINKS_HOUSE_COW,                                          RAND_INF_COWS_MILKED_KF_LINKS_HOUSE_COW },
    { RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT,                       RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT },
    { RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT,                        RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT },
    { RC_LW_DEKU_SCRUB_NEAR_BRIDGE,                                   RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_NEAR_BRIDGE },
    { RC_LW_DEKU_SCRUB_GROTTO_REAR,                                   RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_GROTTO_REAR },
    { RC_LW_DEKU_SCRUB_GROTTO_FRONT,                                  RAND_INF_SCRUBS_PURCHASED_LW_DEKU_SCRUB_GROTTO_FRONT },
    { RC_SFM_DEKU_SCRUB_GROTTO_REAR,                                  RAND_INF_SCRUBS_PURCHASED_SFM_DEKU_SCRUB_GROTTO_REAR },
    { RC_SFM_DEKU_SCRUB_GROTTO_FRONT,                                 RAND_INF_SCRUBS_PURCHASED_SFM_DEKU_SCRUB_GROTTO_FRONT },
    { RC_HF_DEKU_SCRUB_GROTTO,                                        RAND_INF_SCRUBS_PURCHASED_HF_DEKU_SCRUB_GROTTO },
    { RC_HF_COW_GROTTO_COW,                                           RAND_INF_COWS_MILKED_HF_COW_GROTTO_COW },
    { RC_LH_DEKU_SCRUB_GROTTO_LEFT,                                   RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_LEFT },
    { RC_LH_DEKU_SCRUB_GROTTO_RIGHT,                                  RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_RIGHT },
    { RC_LH_DEKU_SCRUB_GROTTO_CENTER,                                 RAND_INF_SCRUBS_PURCHASED_LH_DEKU_SCRUB_GROTTO_CENTER },
    { RC_GV_DEKU_SCRUB_GROTTO_REAR,                                   RAND_INF_SCRUBS_PURCHASED_GV_DEKU_SCRUB_GROTTO_REAR },
    { RC_GV_DEKU_SCRUB_GROTTO_FRONT,                                  RAND_INF_SCRUBS_PURCHASED_GV_DEKU_SCRUB_GROTTO_FRONT },
    { RC_GV_COW,                                                      RAND_INF_COWS_MILKED_GV_COW },
    { RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR,                             RAND_INF_SCRUBS_PURCHASED_COLOSSUS_DEKU_SCRUB_GROTTO_REAR },
    { RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT,                            RAND_INF_SCRUBS_PURCHASED_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT },
    { RC_KAK_IMPAS_HOUSE_COW,                                         RAND_INF_COWS_MILKED_KAK_IMPAS_HOUSE_COW },
    { RC_DMT_COW_GROTTO_COW,                                          RAND_INF_COWS_MILKED_DMT_COW_GROTTO_COW },
    { RC_GC_DEKU_SCRUB_GROTTO_LEFT,                                   RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_LEFT },
    { RC_GC_DEKU_SCRUB_GROTTO_RIGHT,                                  RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_RIGHT },
    { RC_GC_DEKU_SCRUB_GROTTO_CENTER,                                 RAND_INF_SCRUBS_PURCHASED_GC_DEKU_SCRUB_GROTTO_CENTER },
    { RC_DMC_DEKU_SCRUB,                                              RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB },
    { RC_DMC_DEKU_SCRUB_GROTTO_LEFT,                                  RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_LEFT },
    { RC_DMC_DEKU_SCRUB_GROTTO_RIGHT,                                 RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_RIGHT },
    { RC_DMC_DEKU_SCRUB_GROTTO_CENTER,                                RAND_INF_SCRUBS_PURCHASED_DMC_DEKU_SCRUB_GROTTO_CENTER },
    { RC_ZR_DEKU_SCRUB_GROTTO_REAR,                                   RAND_INF_SCRUBS_PURCHASED_ZR_DEKU_SCRUB_GROTTO_REAR },
    { RC_ZR_DEKU_SCRUB_GROTTO_FRONT,                                  RAND_INF_SCRUBS_PURCHASED_ZR_DEKU_SCRUB_GROTTO_FRONT },
    { RC_LLR_DEKU_SCRUB_GROTTO_LEFT,                                  RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_LEFT },
    { RC_LLR_DEKU_SCRUB_GROTTO_RIGHT,                                 RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_RIGHT },
    { RC_LLR_DEKU_SCRUB_GROTTO_CENTER,                                RAND_INF_SCRUBS_PURCHASED_LLR_DEKU_SCRUB_GROTTO_CENTER },
    { RC_LLR_STABLES_LEFT_COW,                                        RAND_INF_COWS_MILKED_LLR_STABLES_LEFT_COW },
    { RC_LLR_STABLES_RIGHT_COW,                                       RAND_INF_COWS_MILKED_LLR_STABLES_RIGHT_COW },
    { RC_LLR_TOWER_LEFT_COW,                                          RAND_INF_COWS_MILKED_LLR_TOWER_LEFT_COW },
    { RC_LLR_TOWER_RIGHT_COW,                                         RAND_INF_COWS_MILKED_LLR_TOWER_RIGHT_COW },
    { RC_DEKU_TREE_MQ_DEKU_SCRUB,                                     RAND_INF_SCRUBS_PURCHASED_DEKU_TREE_MQ_DEKU_SCRUB },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,               RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS,          RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT,              RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT },
    { RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY,                            RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR,                    RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT,                   RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE,                     RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE },
    { RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS, RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS },
    { RC_JABU_JABUS_BELLY_DEKU_SCRUB,                                 RAND_INF_SCRUBS_PURCHASED_JABU_JABUS_BELLY_DEKU_SCRUB },
    { RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,                        RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT },
    { RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT,                       RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT },
    { RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT,                              RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_RIGHT },
    { RC_GANONS_CASTLE_DEKU_SCRUB_LEFT,                               RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_DEKU_SCRUB_LEFT },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT,                           RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT,                     RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER,                          RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT,                    RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT },
    { RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT,                            RAND_INF_SCRUBS_PURCHASED_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT },
};

RandomizerCheckObject Randomizer::GetCheckObjectFromActor(s16 actorId, s16 sceneNum, s32 actorParams) {
    for (auto const& [checkId, checkObj] : RandomizerCheckObjects::GetAllRCObjects()) {
        if (
            checkObj.actorId == actorId &&
            checkObj.sceneId == sceneNum &&
            checkObj.actorParams == actorParams
        ) {
            return checkObj;
        }
    }

    return RandomizerCheckObjects::GetAllRCObjects()[RC_UNKNOWN_CHECK];
}

ScrubIdentity Randomizer::IdentifyScrub(s16 sceneNum, s16 actorParams, s16 respawnData) {
    struct ScrubIdentity scrubIdentity;

    scrubIdentity.randomizerInf = RAND_INF_MAX;
    scrubIdentity.randomizerCheck = RC_UNKNOWN_CHECK;
    scrubIdentity.getItemId = GI_NONE;
    scrubIdentity.itemPrice = -1;
    scrubIdentity.isShuffled = GetRandoSettingValue(RSK_SHUFFLE_SCRUBS) > 0;

    RandomizerCheckObject rcObject = Randomizer::GetCheckObjectFromActor(ACTOR_EN_DNS, sceneNum, TWO_ACTOR_PARAMS(actorParams == 0x06 ? 0x03 : actorParams, respawnData));

    if (rcObject.rc != RC_UNKNOWN_CHECK) {
        scrubIdentity.randomizerInf = rcToRandomizerInf[rcObject.rc];
        scrubIdentity.randomizerCheck = rcObject.rc;
        scrubIdentity.getItemId = rcObject.ogItemId;
        scrubIdentity.isShuffled = GetRandoSettingValue(RSK_SHUFFLE_SCRUBS) > 0;

        if (rcObject.rc == RC_HF_DEKU_SCRUB_GROTTO || rcObject.rc == RC_LW_DEKU_SCRUB_GROTTO_FRONT || rcObject.rc == RC_LW_DEKU_SCRUB_NEAR_BRIDGE) {
            scrubIdentity.isShuffled = true;
        }

        if (randomizerMerchantPrices.find(scrubIdentity.randomizerCheck) != randomizerMerchantPrices.end()) {
            scrubIdentity.itemPrice = randomizerMerchantPrices[scrubIdentity.randomizerCheck];
        }
    }

    return scrubIdentity;
}

CowIdentity Randomizer::IdentifyCow(s16 sceneNum, s16 posX, s16 posZ) {
    struct CowIdentity cowIdentity;

    cowIdentity.randomizerInf = RAND_INF_MAX;
    cowIdentity.randomizerCheck = RC_UNKNOWN_CHECK;

    RandomizerCheckObject rcObject = Randomizer::GetCheckObjectFromActor(ACTOR_EN_COW, sceneNum, TWO_ACTOR_PARAMS(posX, posZ));

    if (rcObject.rc != RC_UNKNOWN_CHECK) {
        cowIdentity.randomizerInf = rcToRandomizerInf[rcObject.rc];
        cowIdentity.randomizerCheck = rcObject.rc;
    }

    return cowIdentity;
}

RandomizerCheck Randomizer::GetCheckFromActor(s16 actorId, s16 sceneNum, s32 actorParams) {
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