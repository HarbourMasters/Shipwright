#pragma once

typedef struct {
    char tex[512];
    uint16_t width;
    uint16_t height;
    uint8_t im_fmt;
    uint8_t im_siz;
    uint8_t id;
} Sprite;

typedef enum {
    RC_LINKS_POCKET,
    RC_QUEEN_GOHMA,
    RC_KING_DODONGO,
    RC_BARINADE,
    RC_PHANTOM_GANON,
    RC_VOLVAGIA,
    RC_MORPHA,
    RC_BONGO_BONGO,
    RC_TWINROVA,
    RC_GANON,
    RC_GIFT_FROM_SAGES,
    RC_SONG_FROM_IMPA,
    RC_SONG_FROM_MALON,
    RC_SONG_FROM_SARIA,
    RC_SONG_FROM_ROYAL_FAMILYS_TOMB,
    RC_SONG_FROM_OCARINA_OF_TIME,
    RC_SONG_FROM_WINDMILL,
    RC_SHEIK_IN_FOREST,
    RC_SHEIK_IN_CRATER,
    RC_SHEIK_IN_ICE_CAVERN,
    RC_SHEIK_AT_COLOSSUS,
    RC_SHEIK_IN_KAKARIKO,
    RC_SHEIK_AT_TEMPLE,
    RC_KF_MIDOS_TOP_LEFT_CHEST,
    RC_KF_MIDOS_TOP_RIGHT_CHEST,
    RC_KF_MIDOS_BOTTOM_LEFT_CHEST,
    RC_KF_MIDOS_BOTTOM_RIGHT_CHEST,
    RC_KF_KOKIRI_SWORD_CHEST,
    RC_KF_STORMS_GROTTO_CHEST,
    RC_KF_LINKS_HOUSE_COW,
    RC_KF_GS_KNOW_IT_ALL_HOUSE,
    RC_KF_GS_BEAN_PATCH,
    RC_KF_GS_HOUSE_OF_TWINS,
    RC_KF_SHOP_ITEM_1,
    RC_KF_SHOP_ITEM_2,
    RC_KF_SHOP_ITEM_3,
    RC_KF_SHOP_ITEM_4,
    RC_KF_SHOP_ITEM_5,
    RC_KF_SHOP_ITEM_6,
    RC_KF_SHOP_ITEM_7,
    RC_KF_SHOP_ITEM_8,
    RC_LW_GIFT_FROM_SARIA,
    RC_LW_OCARINA_MEMORY_GAME,
    RC_LW_TARGET_IN_WOODS,
    RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST,
    RC_LW_TRADE_COJIRO,
    RC_DEKU_THEATER_SKULL_MASK,
    RC_DEKU_THEATER_MASK_OF_TRUTH,
    RC_LW_SKULL_KID,
    RC_LW_TRADE_ODD_POTION,
    RC_LW_DEKU_SCRUB_NEAR_BRIDGE,
    RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT,
    RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT,
    RC_LW_DEKU_SCRUB_GROTTO_FRONT,
    RC_LW_DEKU_SCRUB_GROTTO_REAR,
    RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE,
    RC_LW_GS_BEAN_PATCH_NEAR_THEATER,
    RC_LW_GS_ABOVE_THEATER,
    RC_SFM_WOLFOS_GROTTO_CHEST,
    RC_SFM_DEKU_SCRUB_GROTTO_FRONT,
    RC_SFM_DEKU_SCRUB_GROTTO_REAR,
    RC_SFM_GS,
    RC_HF_OCARINA_OF_TIME_ITEM,
    RC_HF_NEAR_MARKET_GROTTO_CHEST,
    RC_HF_TEKTITE_GROTTO_FREESTANDING_POH,
    RC_HF_SOUTHEAST_GROTTO_CHEST,
    RC_HF_OPEN_GROTTO_CHEST,
    RC_HF_DEKU_SCRUB_GROTTO,
    RC_HF_COW_GROTTO_COW,
    RC_HF_GS_COW_GROTTO,
    RC_HF_GS_NEAR_KAK_GROTTO,
    RC_MARKET_SHOOTING_GALLERY_REWARD,
    RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE,
    RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE,
    RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS,
    RC_MARKET_LOST_DOG,
    RC_MARKET_TREASURE_CHEST_GAME_REWARD,
    RC_MARKET_10_BIG_POES,
    RC_MARKET_TREASURE_CHEST_GAME_ITEM_1,
    RC_MARKET_TREASURE_CHEST_GAME_ITEM_2,
    RC_MARKET_TREASURE_CHEST_GAME_ITEM_3,
    RC_MARKET_TREASURE_CHEST_GAME_ITEM_4,
    RC_MARKET_TREASURE_CHEST_GAME_ITEM_5,
    RC_MARKET_GS_GUARD_HOUSE,
    RC_MARKET_BAZAAR_ITEM_1,
    RC_MARKET_BAZAAR_ITEM_2,
    RC_MARKET_BAZAAR_ITEM_3,
    RC_MARKET_BAZAAR_ITEM_4,
    RC_MARKET_BAZAAR_ITEM_5,
    RC_MARKET_BAZAAR_ITEM_6,
    RC_MARKET_BAZAAR_ITEM_7,
    RC_MARKET_BAZAAR_ITEM_8,
    RC_MARKET_POTION_SHOP_ITEM_1,
    RC_MARKET_POTION_SHOP_ITEM_2,
    RC_MARKET_POTION_SHOP_ITEM_3,
    RC_MARKET_POTION_SHOP_ITEM_4,
    RC_MARKET_POTION_SHOP_ITEM_5,
    RC_MARKET_POTION_SHOP_ITEM_6,
    RC_MARKET_POTION_SHOP_ITEM_7,
    RC_MARKET_POTION_SHOP_ITEM_8,
    RC_MARKET_BOMBCHU_SHOP_ITEM_1,
    RC_MARKET_BOMBCHU_SHOP_ITEM_2,
    RC_MARKET_BOMBCHU_SHOP_ITEM_3,
    RC_MARKET_BOMBCHU_SHOP_ITEM_4,
    RC_MARKET_BOMBCHU_SHOP_ITEM_5,
    RC_MARKET_BOMBCHU_SHOP_ITEM_6,
    RC_MARKET_BOMBCHU_SHOP_ITEM_7,
    RC_MARKET_BOMBCHU_SHOP_ITEM_8,
    RC_TOT_LIGHT_ARROWS_CUTSCENE,
    RC_HC_MALON_EGG,
    RC_HC_ZELDAS_LETTER,
    RC_HC_GREAT_FAIRY_REWARD,
    RC_HC_GS_TREE,
    RC_HC_GS_STORMS_GROTTO,
    RC_LLR_TALONS_CHICKENS,
    RC_LLR_FREESTANDING_POH,
    RC_LLR_DEKU_SCRUB_GROTTO_LEFT,
    RC_LLR_DEKU_SCRUB_GROTTO_CENTER,
    RC_LLR_DEKU_SCRUB_GROTTO_RIGHT,
    RC_LLR_STABLES_LEFT_COW,
    RC_LLR_STABLES_RIGHT_COW,
    RC_LLR_TOWER_LEFT_COW,
    RC_LLR_TOWER_RIGHT_COW,
    RC_LLR_GS_HOUSE_WINDOW,
    RC_LLR_GS_TREE,
    RC_LLR_GS_RAIN_SHED,
    RC_LLR_GS_BACK_WALL,
    RC_KAK_ANJU_AS_CHILD,
    RC_KAK_ANJU_AS_ADULT,
    RC_KAK_IMPAS_HOUSE_FREESTANDING_POH,
    RC_KAK_WINDMILL_FREESTANDING_POH,
    RC_KAK_MAN_ON_ROOF,
    RC_KAK_OPEN_GROTTO_CHEST,
    RC_KAK_REDEAD_GROTTO_CHEST,
    RC_KAK_SHOOTING_GALLERY_REWARD,
    RC_KAK_TRADE_ODD_MUSHROOM,
    RC_KAK_TRADE_POCKET_CUCCO,
    RC_KAK_10_GOLD_SKULLTULA_REWARD,
    RC_KAK_20_GOLD_SKULLTULA_REWARD,
    RC_KAK_30_GOLD_SKULLTULA_REWARD,
    RC_KAK_40_GOLD_SKULLTULA_REWARD,
    RC_KAK_50_GOLD_SKULLTULA_REWARD,
    RC_KAK_IMPAS_HOUSE_COW,
    RC_KAK_GS_TREE,
    RC_KAK_GS_GUARDS_HOUSE,
    RC_KAK_GS_WATCHTOWER,
    RC_KAK_GS_SKULLTULA_HOUSE,
    RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION,
    RC_KAK_GS_ABOVE_IMPAS_HOUSE,
    RC_KAK_BAZAAR_ITEM_1,
    RC_KAK_BAZAAR_ITEM_2,
    RC_KAK_BAZAAR_ITEM_3,
    RC_KAK_BAZAAR_ITEM_4,
    RC_KAK_BAZAAR_ITEM_5,
    RC_KAK_BAZAAR_ITEM_6,
    RC_KAK_BAZAAR_ITEM_7,
    RC_KAK_BAZAAR_ITEM_8,
    RC_KAK_POTION_SHOP_ITEM_1,
    RC_KAK_POTION_SHOP_ITEM_2,
    RC_KAK_POTION_SHOP_ITEM_3,
    RC_KAK_POTION_SHOP_ITEM_4,
    RC_KAK_POTION_SHOP_ITEM_5,
    RC_KAK_POTION_SHOP_ITEM_6,
    RC_KAK_POTION_SHOP_ITEM_7,
    RC_KAK_POTION_SHOP_ITEM_8,
    RC_GRAVEYARD_SHIELD_GRAVE_CHEST,
    RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST,
    RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST,
    RC_GRAVEYARD_FREESTANDING_POH,
    RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR,
    RC_GRAVEYARD_HOOKSHOT_CHEST,
    RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH,
    RC_GRAVEYARD_GS_BEAN_PATCH,
    RC_GRAVEYARD_GS_WALL,
    RC_DMT_FREESTANDING_POH,
    RC_DMT_CHEST,
    RC_DMT_STORMS_GROTTO_CHEST,
    RC_DMT_TRADE_BROKEN_SWORD,
    RC_DMT_TRADE_EYEDROPS,
    RC_DMT_TRADE_CLAIM_CHECK,
    RC_DMT_GREAT_FAIRY_REWARD,
    RC_DMT_COW_GROTTO_COW,
    RC_DMT_GS_NEAR_KAK,
    RC_DMT_GS_BEAN_PATCH,
    RC_DMT_GS_ABOVE_DODONGOS_CAVERN,
    RC_DMT_GS_FALLING_ROCKS_PATH,
    RC_GC_DARUNIAS_JOY,
    RC_GC_POT_FREESTANDING_POH,
    RC_GC_ROLLING_GORON_AS_CHILD,
    RC_GC_ROLLING_GORON_AS_ADULT,
    RC_GC_MEDIGORON,
    RC_GC_MAZE_LEFT_CHEST,
    RC_GC_MAZE_RIGHT_CHEST,
    RC_GC_MAZE_CENTER_CHEST,
    RC_GC_DEKU_SCRUB_GROTTO_LEFT,
    RC_GC_DEKU_SCRUB_GROTTO_CENTER,
    RC_GC_DEKU_SCRUB_GROTTO_RIGHT,
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
    RC_DMC_VOLCANO_FREESTANDING_POH,
    RC_DMC_WALL_FREESTANDING_POH,
    RC_DMC_UPPER_GROTTO_CHEST,
    RC_DMC_GREAT_FAIRY_REWARD,
    RC_DMC_DEKU_SCRUB,
    RC_DMC_DEKU_SCRUB_GROTTO_LEFT,
    RC_DMC_DEKU_SCRUB_GROTTO_CENTER,
    RC_DMC_DEKU_SCRUB_GROTTO_RIGHT,
    RC_DMC_GS_CRATE,
    RC_DMC_GS_BEAN_PATCH,
    RC_ZR_MAGIC_BEAN_SALESMAN,
    RC_ZR_OPEN_GROTTO_CHEST,
    RC_ZR_FROGS_ZELDAS_LULLABY,
    RC_ZR_FROGS_EPONAS_SONG,
    RC_ZR_FROGS_SARIAS_SONG,
    RC_ZR_FROGS_SUNS_SONG,
    RC_ZR_FROGS_SONG_OF_TIME,
    RC_ZR_FROGS_IN_THE_RAIN,
    RC_ZR_FROGS_OCARINA_GAME,
    RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH,
    RC_ZR_NEAR_DOMAIN_FREESTANDING_POH,
    RC_ZR_DEKU_SCRUB_GROTTO_FRONT,
    RC_ZR_DEKU_SCRUB_GROTTO_REAR,
    RC_ZR_GS_TREE,
    RC_ZR_GS_LADDER,
    RC_ZR_GS_NEAR_RAISED_GROTTOS,
    RC_ZR_GS_ABOVE_BRIDGE,
    RC_ZD_DIVING_MINIGAME,
    RC_ZD_CHEST,
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
    RC_ZF_GREAT_FAIRY_REWARD,
    RC_ZF_ICEBERC_FREESTANDING_POH,
    RC_ZF_BOTTOM_FREESTANDING_POH,
    RC_ZF_GS_ABOVE_THE_LOG,
    RC_ZF_GS_TREE,
    RC_ZF_GS_HIDDEN_CAVE,
    RC_LH_UNDERWATER_ITEM,
    RC_LH_CHILD_FISHING,
    RC_LH_ADULT_FISHING,
    RC_LH_LAB_DIVE,
    RC_LH_TRADE_FROG,
    RC_LH_FREESTANDING_POH,
    RC_LH_SUN,
    RC_LH_DEKU_SCRUB_GROTTO_LEFT,
    RC_LH_DEKU_SCRUB_GROTTO_CENTER,
    RC_LH_DEKU_SCRUB_GROTTO_RIGHT,
    RC_LH_GS_BEAN_PATCH,
    RC_LH_GS_LAB_WALL,
    RC_LH_GS_SMALL_ISLAND,
    RC_LH_GS_LAB_CRATE,
    RC_LH_GS_TREE,
    RC_GV_CRATE_FREESTANDING_POH,
    RC_GV_WATERFALL_FREESTANDING_POH,
    RC_GV_CHEST,
    RC_GV_TRADE_SAW,
    RC_GV_DEKU_SCRUB_GROTTO_FRONT,
    RC_GV_DEKU_SCRUB_GROTTO_REAR,
    RC_GV_COW,
    RC_GV_GS_SMALL_BRIDGE,
    RC_GV_GS_BEAN_PATCH,
    RC_GV_GS_BEHIND_TENT,
    RC_GV_GS_PILLAR,
    RC_GF_CHEST,
    RC_GF_HBA_1000_POINTS,
    RC_GF_HBA_1500_POINTS,
    RC_GF_GERUDO_MEMBERSHIP_CARD,
    RC_GF_NORTH_F1_CARPENTER,
    RC_GF_NORTH_F2_CARPENTER,
    RC_GF_SOUTH_F1_CARPENTER,
    RC_GF_SOUTH_F2_CARPENTER,
    RC_GF_GS_TOP_FLOOR,
    RC_GF_GS_ARCHERY_RANGE,
    RC_HIDEOUT_JAIL_GUARD_1_TORCH,
    RC_HIDEOUT_JAIL_GUARD_2_TORCHES,
    RC_HIDEOUT_JAIL_GUARD_3_TORCHES,
    RC_HIDEOUT_JAIL_GUARD_4_TORCHES,
    RC_HIDEOUT_GERUDO_MEMBERSHIP_CARD,
    RC_WASTELAND_BOMBCHU_SALESMAN,
    RC_WASTELAND_CHEST,
    RC_WASTELAND_GS,
    RC_COLOSSUS_GREAT_FAIRY_REWARD,
    RC_COLOSSUS_FREESTANDING_POH,
    RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT,
    RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR,
    RC_COLOSSUS_GS_BEAN_PATCH,
    RC_COLOSSUS_GS_TREE,
    RC_COLOSSUS_GS_HILL,
    RC_OGC_GREAT_FAIRY_REWARD,
    RC_OGC_GS,
    RC_DEKU_TREE_MAP_CHEST,
    RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST,
    RC_DEKU_TREE_SLINGSHOT_CHEST,
    RC_DEKU_TREE_COMPASS_CHEST,
    RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST,
    RC_DEKU_TREE_BASEMENT_CHEST,
    RC_DEKU_TREE_GS_COMPASS_ROOM,
    RC_DEKU_TREE_GS_BASEMENT_VINES,
    RC_DEKU_TREE_GS_BASEMENT_GATE,
    RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM,
    RC_DEKU_TREE_MQ_MAP_CHEST,
    RC_DEKU_TREE_MQ_SLINGSHOT_CHEST,
    RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST,
    RC_DEKU_TREE_MQ_COMPASS_CHEST,
    RC_DEKU_TREE_MQ_BASEMENT_CHEST,
    RC_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST,
    RC_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST,
    RC_DEKU_TREE_MQ_DEKU_SCRUB,
    RC_DEKU_TREE_MQ_GS_LOBBY,
    RC_DEKU_TREE_MQ_GS_COMPASS_ROOM,
    RC_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM,
    RC_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM,
    RC_DEKU_TREE_QUEEN_GOHMA_HEART,
    RC_DODONGOS_CAVERN_MAP_CHEST,
    RC_DODONGOS_CAVERN_COMPASS_CHEST,
    RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST,
    RC_DODONGOS_CAVERN_BOMB_BAG_CHEST,
    RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST,
    RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS,
    RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY,
    RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,
    RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT,
    RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS,
    RC_DODONGOS_CAVERN_GS_SCARECROW,
    RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS,
    RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS,
    RC_DODONGOS_CAVERN_GS_BACK_ROOM,
    RC_DODONGOS_CAVERN_MQ_MAP_CHEST,
    RC_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST,
    RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST,
    RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST,
    RC_DODONGOS_CAVERN_MQ_COMPASS_CHEST,
    RC_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST,
    RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT,
    RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR,
    RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS,
    RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE,
    RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM,
    RC_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM,
    RC_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM,
    RC_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM,
    RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA,
    RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST,
    RC_DODONGOS_CAVERN_KING_DODONGO_HEART,
    RC_JABU_JABUS_BELLY_BOOMERANG_CHEST,
    RC_JABU_JABUS_BELLY_MAP_CHEST,
    RC_JABU_JABUS_BELLY_COMPASS_CHEST,
    RC_JABU_JABUS_BELLY_DEKU_SCRUB,
    RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM,
    RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER,
    RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER,
    RC_JABU_JABUS_BELLY_GS_NEAR_BOSS,
    RC_JABU_JABUS_BELLY_MQ_MAP_CHEST,
    RC_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST,
    RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST,
    RC_JABU_JABUS_BELLY_MQ_COMPASS_CHEST,
    RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST,
    RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST,
    RC_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST,
    RC_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST,
    RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST,
    RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST,
    RC_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST,
    RC_JABU_JABUS_BELLY_MQ_COW,
    RC_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM,
    RC_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM,
    RC_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM,
    RC_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS,
    RC_JABU_JABUS_BELLY_BARINADE_HEART,
    RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST,
    RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST,
    RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST,
    RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST,
    RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY,
    RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST,
    RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST,
    RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST,
    RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST,
    RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST,
    RC_BOTTOM_OF_THE_WELL_MAP_CHEST,
    RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST,
    RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST,
    RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST,
    RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM,
    RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM,
    RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE,
    RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST,
    RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY,
    RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST,
    RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY,
    RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST,
    RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM,
    RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM,
    RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT,
    RC_FOREST_TEMPLE_FIRST_ROOM_CHEST,
    RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST,
    RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST,
    RC_FOREST_TEMPLE_MAP_CHEST,
    RC_FOREST_TEMPLE_WELL_CHEST,
    RC_FOREST_TEMPLE_EYE_SWITCH_CHEST,
    RC_FOREST_TEMPLE_BOSS_KEY_CHEST,
    RC_FOREST_TEMPLE_FLOORMASTER_CHEST,
    RC_FOREST_TEMPLE_RED_POE_CHEST,
    RC_FOREST_TEMPLE_BOW_CHEST,
    RC_FOREST_TEMPLE_BLUE_POE_CHEST,
    RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST,
    RC_FOREST_TEMPLE_BASEMENT_CHEST,
    RC_FOREST_TEMPLE_GS_FIRST_ROOM,
    RC_FOREST_TEMPLE_GS_LOBBY,
    RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD,
    RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD,
    RC_FOREST_TEMPLE_GS_BASEMENT,
    RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST,
    RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST,
    RC_FOREST_TEMPLE_MQ_WELL_CHEST,
    RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST,
    RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST,
    RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST,
    RC_FOREST_TEMPLE_MQ_REDEAD_CHEST,
    RC_FOREST_TEMPLE_MQ_MAP_CHEST,
    RC_FOREST_TEMPLE_MQ_BOW_CHEST,
    RC_FOREST_TEMPLE_MQ_COMPASS_CHEST,
    RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST,
    RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST,
    RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY,
    RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD,
    RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD,
    RC_FOREST_TEMPLE_MQ_GS_WELL,
    RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM,
    RC_FOREST_TEMPLE_PHANTOM_GANON_HEART,
    RC_FIRE_TEMPLE_NEAR_BOSS_CHEST,
    RC_FIRE_TEMPLE_FLARE_DANCER_CHEST,
    RC_FIRE_TEMPLE_BOSS_KEY_CHEST,
    RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST,
    RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST,
    RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST,
    RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST,
    RC_FIRE_TEMPLE_MAP_CHEST,
    RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST,
    RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST,
    RC_FIRE_TEMPLE_SCARECROW_CHEST,
    RC_FIRE_TEMPLE_COMPASS_CHEST,
    RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST,
    RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST,
    RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP,
    RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM,
    RC_FIRE_TEMPLE_GS_BOULDER_MAZE,
    RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB,
    RC_FIRE_TEMPLE_GS_SCARECROW_TOP,
    RC_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST,
    RC_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST,
    RC_FIRE_TEMPLE_MQ_MAP_CHEST,
    RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST,
    RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST,
    RC_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST,
    RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST,
    RC_FIRE_TEMPLE_MQ_COMPASS_CHEST,
    RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST,
    RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST,
    RC_FIRE_TEMPLE_MQ_FREESTANDING_KEY,
    RC_FIRE_TEMPLE_MQ_CHEST_ON_FIRE,
    RC_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR,
    RC_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE,
    RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER,
    RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM,
    RC_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE,
    RC_FIRE_TEMPLE_VOLVAGIA_HEART,
    RC_WATER_TEMPLE_COMPASS_CHEST,
    RC_WATER_TEMPLE_MAP_CHEST,
    RC_WATER_TEMPLE_CRACKED_WALL_CHEST,
    RC_WATER_TEMPLE_TORCHES_CHEST,
    RC_WATER_TEMPLE_BOSS_KEY_CHEST,
    RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST,
    RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST,
    RC_WATER_TEMPLE_LONGSHOT_CHEST,
    RC_WATER_TEMPLE_RIVER_CHEST,
    RC_WATER_TEMPLE_DRAGON_CHEST,
    RC_WATER_TEMPLE_GS_BEHIND_GATE,
    RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST,
    RC_WATER_TEMPLE_GS_CENTRAL_PILLAR,
    RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM,
    RC_WATER_TEMPLE_GS_RIVER,
    RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST,
    RC_WATER_TEMPLE_MQ_MAP_CHEST,
    RC_WATER_TEMPLE_MQ_COMPASS_CHEST,
    RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST,
    RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST,
    RC_WATER_TEMPLE_MQ_FREESTANDING_KEY,
    RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY,
    RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH,
    RC_WATER_TEMPLE_MQ_GS_RIVER,
    RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA,
    RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH,
    RC_WATER_TEMPLE_MORPHA_HEART,
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
    RC_SHADOW_TEMPLE_FREESTANDING_KEY,
    RC_SHADOW_TEMPLE_WIND_HINT_CHEST,
    RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST,
    RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST,
    RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST,
    RC_SHADOW_TEMPLE_BOSS_KEY_CHEST,
    RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST,
    RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM,
    RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM,
    RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT,
    RC_SHADOW_TEMPLE_GS_NEAR_SHIP,
    RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT,
    RC_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST,
    RC_SHADOW_TEMPLE_MQ_MAP_CHEST,
    RC_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST,
    RC_SHADOW_TEMPLE_MQ_COMPASS_CHEST,
    RC_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST,
    RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST,
    RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST,
    RC_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST,
    RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST,
    RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST,
    RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST,
    RC_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST,
    RC_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST,
    RC_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST,
    RC_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST,
    RC_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST,
    RC_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST,
    RC_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST,
    RC_SHADOW_TEMPLE_MQ_FREESTANDING_KEY,
    RC_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST,
    RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM,
    RC_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM,
    RC_SHADOW_TEMPLE_MQ_GS_AFTER_WIND,
    RC_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP,
    RC_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS,
    RC_SHADOW_TEMPLE_BONGO_BONGO_HEART,
    RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST,
    RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST,
    RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST,
    RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST,
    RC_SPIRIT_TEMPLE_MAP_CHEST,
    RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST,
    RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST,
    RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST,
    RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST,
    RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST,
    RC_SPIRIT_TEMPLE_MQ_MAP_CHEST,
    RC_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST,
    RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST,
    RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST,
    RC_SPIRIT_TEMPLE_MQ_COMPASS_CHEST,
    RC_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST,
    RC_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST,
    RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST,
    RC_SPIRIT_TEMPLE_COMPASS_CHEST,
    RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST,
    RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST,
    RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST,
    RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST,
    RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST,
    RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST,
    RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST,
    RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST,
    RC_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST,
    RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST,
    RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST,
    RC_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST,
    RC_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST,
    RC_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST,
    RC_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST,
    RC_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST,
    RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST,
    RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST,
    RC_SPIRIT_TEMPLE_TOPMOST_CHEST,
    RC_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST,
    RC_SPIRIT_TEMPLE_GS_METAL_FENCE,
    RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM,
    RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM,
    RC_SPIRIT_TEMPLE_GS_LOBBY,
    RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM,
    RC_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM,
    RC_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM,
    RC_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM,
    RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST,
    RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH,
    RC_SPIRIT_TEMPLE_TWINROVA_HEART,
    RC_ICE_CAVERN_MAP_CHEST,
    RC_ICE_CAVERN_COMPASS_CHEST,
    RC_ICE_CAVERN_FREESTANDING_POH,
    RC_ICE_CAVERN_IRON_BOOTS_CHEST,
    RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM,
    RC_ICE_CAVERN_GS_HEART_PIECE_ROOM,
    RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM,
    RC_ICE_CAVERN_MQ_MAP_CHEST,
    RC_ICE_CAVERN_MQ_COMPASS_CHEST,
    RC_ICE_CAVERN_MQ_FREESTANDING_POH,
    RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST,
    RC_ICE_CAVERN_MQ_GS_RED_ICE,
    RC_ICE_CAVERN_MQ_GS_ICE_BLOCK,
    RC_ICE_CAVERN_MQ_GS_SCARECROW,
    RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST,
    RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST,
    RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST,
    RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST,
    RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST,
    RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST,
    RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST,
    RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST,
    RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST,
    RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST,
    RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST,
    RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST,
    RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY,
    RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST,
    RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST,
    RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST,
    RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST,
    RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST,
    RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST,
    RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST,
    RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST,
    RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST,
    RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST,
    RC_GANONS_CASTLE_FOREST_TRIAL_CHEST,
    RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST,
    RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST,
    RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST,
    RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST,
    RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST,
    RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST,
    RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST,
    RC_GANONS_CASTLE_DEKU_SCRUB_LEFT,
    RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT,
    RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT,
    RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT,
    RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY,
    RC_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST,
    RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST,
    RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST,
    RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST,
    RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST,
    RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST,
    RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST,
    RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST,
    RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST,
    RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST,
    RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST,
    RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST,
    RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT,
    RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT,
    RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER,
    RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT,
    RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT,
    RC_GANONS_TOWER_BOSS_KEY_CHEST,
    RC_PIERRE,
    RC_DELIVER_RUTOS_LETTER,
    RC_MASTER_SWORD_PEDESTAL,
    RC_COLOSSUS_GOSSIP_STONE,
    RC_DMC_GOSSIP_STONE,
    RC_DMC_UPPER_GROTTO_GOSSIP_STONE,
    RC_DMT_GOSSIP_STONE,
    RC_DMT_STORMS_GROTTO_GOSSIP_STONE,
    RC_DODONGOS_CAVERN_GOSSIP_STONE,
    RC_FAIRY_GOSSIP_STONE,
    RC_GC_MAZE_GOSSIP_STONE,
    RC_GC_MEDIGORON_GOSSIP_STONE,
    RC_GV_GOSSIP_STONE,
    RC_GY_GOSSIP_STONE,
    RC_HC_MALON_GOSSIP_STONE,
    RC_HC_ROCK_WALL_GOSSIP_STONE,
    RC_HC_STORMS_GROTTO_GOSSIP_STONE,
    RC_HF_COW_GROTTO_GOSSIP_STONE,
    RC_HF_NEAR_MARKET_GOSSIP_STONE,
    RC_HF_OPEN_GROTTO_GOSSIP_STONE,
    RC_HF_SOUTHEAST_GOSSIP_STONE,
    RC_JABU_GOSSIP_STONE,
    RC_KF_DEKU_TREE_LEFT_GOSSIP_STONE,
    RC_KF_DEKU_TREE_RIGHT_GOSSIP_STONE,
    RC_KF_GOSSIP_STONE,
    RC_KF_STORMS_GOSSIP_STONE,
    RC_KAK_OPEN_GROTTO_GOSSIP_STONE,
    RC_LH_LAB_GOSSIP_STONE,
    RC_LH_SOUTHEAST_GOSSIP_STONE,
    RC_LH_SOUTHWEST_GOSSIP_STONE,
    RC_LW_GOSSIP_STONE,
    RC_LW_NEAR_SHORTCUTS_GOSSIP_STONE,
    RC_SFM_MAZE_LOWER_GOSSIP_STONE,
    RC_SFM_MAZE_UPPER_GOSSIP_STONE,
    RC_SFM_SARIA_GOSSIP_STONE,
    RC_TOT_LEFT_CENTER_GOSSIP_STONE,
    RC_TOT_LEFT_GOSSIP_STONE,
    RC_TOT_RIGHT_CENTER_GOSSIP_STONE,
    RC_TOT_RIGHT_GOSSIP_STONE,
    RC_ZD_GOSSIP_STONE,
    RC_ZR_NEAR_DOMAIN_GOSSIP_STONE,
    RC_ZR_NEAR_GROTTOS_GOSSIP_STONE,
    RC_ZR_OPEN_GROTTO_GOSSIP_STONE,
    RC_UNKNOWN_CHECK
} RandomizerCheck;

// based on https://github.com/TestRunnerSRL/OoT-Randomizer/blob/e337d7f603b91a6bacb618fb32cc7fd70ed9ffca/ItemList.py
typedef enum {
    RG_NONE,
    RG_KOKIRI_SWORD,
    RG_GIANTS_KNIFE,
    RG_BIGGORON_SWORD,
    RG_DEKU_SHIELD,
    RG_HYLIAN_SHIELD,
    RG_MIRROR_SHIELD,
    RG_GORON_TUNIC,
    RG_ZORA_TUNIC,
    RG_IRON_BOOTS,
    RG_HOVER_BOOTS,
    RG_BOOMERANG,
    RG_LENS_OF_TRUTH,
    RG_MEGATON_HAMMER,
    RG_STONE_OF_AGONY,
    RG_DINS_FIRE,
    RG_FARORES_WIND,
    RG_NAYRUS_LOVE,
    RG_FIRE_ARROWS,
    RG_ICE_ARROWS,
    RG_LIGHT_ARROWS,
    RG_GERUDO_MEMBERSHIP_CARD,
    RG_MAGIC_BEAN,
    RG_MAGIC_BEAN_PACK,
    RG_DOUBLE_DEFENSE,
    RG_WEIRD_EGG,
    RG_ZELDAS_LETTER,
    RG_POCKET_EGG,
    RG_COJIRO,
    RG_ODD_MUSHROOM,
    RG_ODD_POTION,
    RG_POACHERS_SAW,
    RG_BROKEN_SWORD,
    RG_PRESCRIPTION,
    RG_EYEBALL_FROG,
    RG_EYEDROPS,
    RG_CLAIM_CHECK,
    RG_GOLD_SKULLTULA_TOKEN,
    RG_PROGRESSIVE_HOOKSHOT,
    RG_PROGRESSIVE_STRENGTH,
    RG_PROGRESSIVE_BOMB_BAG,
    RG_PROGRESSIVE_BOW,
    RG_PROGRESSIVE_SLINGSHOT,
    RG_PROGRESSIVE_WALLET,
    RG_PROGRESSIVE_SCALE,
    RG_PROGRESSIVE_NUT_UPGRADE,
    RG_PROGRESSIVE_STICK_UPGRADE,
    RG_PROGRESSIVE_BOMBCHUS,
    RG_PROGRESSIVE_MAGIC_METER,
    RG_PROGRESSIVE_OCARINA,
    RG_PROGRESSIVE_GORONSWORD,
    RG_EMPTY_BOTTLE,
    RG_BOTTLE_WITH_MILK,
    RG_BOTTLE_WITH_RED_POTION,
    RG_BOTTLE_WITH_GREEN_POTION,
    RG_BOTTLE_WITH_BLUE_POTION,
    RG_BOTTLE_WITH_FAIRY,
    RG_BOTTLE_WITH_FISH,
    RG_BOTTLE_WITH_BLUE_FIRE,
    RG_BOTTLE_WITH_BUGS,
    RG_BOTTLE_WITH_POE,
    RG_RUTOS_LETTER,
    RG_BOTTLE_WITH_BIG_POE,
    RG_ZELDAS_LULLABY,
    RG_EPONAS_SONG,
    RG_SARIAS_SONG,
    RG_SUNS_SONG,
    RG_SONG_OF_TIME,
    RG_SONG_OF_STORMS,
    RG_MINUET_OF_FOREST,
    RG_BOLERO_OF_FIRE,
    RG_SERENADE_OF_WATER,
    RG_REQUIEM_OF_SPIRIT,
    RG_NOCTURNE_OF_SHADOW,
    RG_PRELUDE_OF_LIGHT,
    RG_DEKU_TREE_MAP,
    RG_DODONGOS_CAVERN_MAP,
    RG_JABU_JABUS_BELLY_MAP,
    RG_FOREST_TEMPLE_MAP,
    RG_FIRE_TEMPLE_MAP,
    RG_WATER_TEMPLE_MAP,
    RG_SPIRIT_TEMPLE_MAP,
    RG_SHADOW_TEMPLE_MAP,
    RG_BOTTOM_OF_THE_WELL_MAP,
    RG_ICE_CAVERN_MAP,
    RG_DEKU_TREE_COMPASS,
    RG_DODONGOS_CAVERN_COMPASS,
    RG_JABU_JABUS_BELLY_COMPASS,
    RG_FOREST_TEMPLE_COMPASS,
    RG_FIRE_TEMPLE_COMPASS,
    RG_WATER_TEMPLE_COMPASS,
    RG_SPIRIT_TEMPLE_COMPASS,
    RG_SHADOW_TEMPLE_COMPASS,
    RG_BOTTOM_OF_THE_WELL_COMPASS,
    RG_ICE_CAVERN_COMPASS,
    RG_FOREST_TEMPLE_BOSS_KEY,
    RG_FIRE_TEMPLE_BOSS_KEY,
    RG_WATER_TEMPLE_BOSS_KEY,
    RG_SPIRIT_TEMPLE_BOSS_KEY,
    RG_SHADOW_TEMPLE_BOSS_KEY,
    RG_GANONS_CASTLE_BOSS_KEY,
    RG_FOREST_TEMPLE_SMALL_KEY,
    RG_FIRE_TEMPLE_SMALL_KEY,
    RG_WATER_TEMPLE_SMALL_KEY,
    RG_SPIRIT_TEMPLE_SMALL_KEY,
    RG_SHADOW_TEMPLE_SMALL_KEY,
    RG_BOTTOM_OF_THE_WELL_SMALL_KEY,
    RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY,
    RG_GERUDO_FORTRESS_SMALL_KEY,
    RG_GANONS_CASTLE_SMALL_KEY,
    RG_TREASURE_GAME_SMALL_KEY,
    RG_FOREST_TEMPLE_KEY_RING,
    RG_FIRE_TEMPLE_KEY_RING,
    RG_WATER_TEMPLE_KEY_RING,
    RG_SPIRIT_TEMPLE_KEY_RING,
    RG_SHADOW_TEMPLE_KEY_RING,
    RG_BOTTOM_OF_THE_WELL_KEY_RING,
    RG_GERUDO_TRAINING_GROUNDS_KEY_RING,
    RG_GERUDO_FORTRESS_KEY_RING,
    RG_GANONS_CASTLE_KEY_RING,
    RG_KOKIRI_EMERALD,
    RG_GORON_RUBY,
    RG_ZORA_SAPPHIRE,
    RG_FOREST_MEDALLION,
    RG_FIRE_MEDALLION,
    RG_WATER_MEDALLION,
    RG_SPIRIT_MEDALLION,
    RG_SHADOW_MEDALLION,
    RG_LIGHT_MEDALLION,
    RG_RECOVERY_HEART,
    RG_GREEN_RUPEE,
    RG_BLUE_RUPEE,
    RG_RED_RUPEE,
    RG_PURPLE_RUPEE,
    RG_HUGE_RUPEE,
    RG_PIECE_OF_HEART,
    RG_HEART_CONTAINER,
    RG_ICE_TRAP,
    RG_MILK,
    RG_BOMBS_5,
    RG_BOMBS_10,
    RG_BOMBS_20,
    RG_BOMBCHU_5,
    RG_BOMBCHU_10,
    RG_BOMBCHU_20,
    RG_BOMBCHU_DROP,
    RG_ARROWS_5,
    RG_ARROWS_10,
    RG_ARROWS_30,
    RG_DEKU_NUTS_5,
    RG_DEKU_NUTS_10,
    RG_DEKU_SEEDS_30,
    RG_DEKU_STICK_1,
    RG_RED_POTION_REFILL,
    RG_GREEN_POTION_REFILL,
    RG_BLUE_POTION_REFILL,
    RG_TREASURE_GAME_HEART,
    RG_TREASURE_GAME_GREEN_RUPEE,
    RG_BUY_DEKU_NUT_5,
    RG_BUY_ARROWS_30,
    RG_BUY_ARROWS_50,
    RG_BUY_BOMBS_525,
    RG_BUY_DEKU_NUT_10,
    RG_BUY_DEKU_STICK_1,
    RG_BUY_BOMBS_10,
    RG_BUY_FISH,
    RG_BUY_RED_POTION_30,
    RG_BUY_GREEN_POTION,
    RG_BUY_BLUE_POTION,
    RG_BUY_HYLIAN_SHIELD,
    RG_BUY_DEKU_SHIELD,
    RG_BUY_GORON_TUNIC,
    RG_BUY_ZORA_TUNIC,
    RG_BUY_HEART,
    RG_BUY_BOMBCHU_10,
    RG_BUY_BOMBCHU_20,
    RG_BUY_BOMBCHU_5,
    RG_BUY_DEKU_SEEDS_30,
    RG_SOLD_OUT,
    RG_BUY_BLUE_FIRE,
    RG_BUY_BOTTLE_BUG,
    RG_BUY_POE,
    RG_BUY_FAIRYS_SPIRIT,
    RG_BUY_ARROWS_10,
    RG_BUY_BOMBS_20,
    RG_BUY_BOMBS_30,
    RG_BUY_BOMBS_535,
    RG_BUY_RED_POTION_40,
    RG_BUY_RED_POTION_50,
    RG_TRIFORCE,
    RG_HINT
} RandomizerGet;

typedef enum {
    RSK_NONE,
    RSK_FOREST,
    RSK_KAK_GATE,
    RSK_DOOR_OF_TIME,
    RSK_ZORAS_FOUNTAIN,
    RSK_GERUDO_FORTRESS,
    RSK_RAINBOW_BRIDGE,
    RSK_RAINBOW_BRIDGE_STONE_COUNT,
    RSK_RAINBOW_BRIDGE_MEDALLION_COUNT,
    RSK_RAINBOW_BRIDGE_REWARD_COUNT,
    RSK_RAINBOW_BRIDGE_DUNGEON_COUNT,
    RSK_RAINBOW_BRIDGE_TOKEN_COUNT,
    RSK_RANDOM_TRIALS,
    RSK_TRIAL_COUNT,
    RSK_STARTING_OCARINA,
    RSK_SHUFFLE_OCARINA,
    RSK_STARTING_DEKU_SHIELD,
    RSK_STARTING_KOKIRI_SWORD,
    RSK_SHUFFLE_KOKIRI_SWORD,
    RSK_STARTING_MAPS_COMPASSES, //RANDOTODO more options for this, rn it's just start with or own dungeon
    RSK_SHUFFLE_DUNGEON_REWARDS,
    RSK_SHUFFLE_SONGS,
    RSK_SHUFFLE_WEIRD_EGG,
    RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD,
    RSK_ITEM_POOL,
    RSK_ICE_TRAPS,
    RSK_GOSSIP_STONE_HINTS,
    RSK_HINT_CLARITY,
    RSK_HINT_DISTRIBUTION,
    RSK_GANONS_BOSS_KEY,
    RSK_SKIP_CHILD_STEALTH,
    RSK_SKIP_CHILD_ZELDA,
    RSK_STARTING_CONSUMABLES,
    RSK_FULL_WALLETS,
    RSK_EXCLUDE_DEKU_THEATER_MASK_OF_TRUTH,
    RSK_LANGUAGE,
    RSK_EXCLUDE_KAK_10_GOLD_SKULLTULA_REWARD,
    RSK_EXCLUDE_KAK_20_GOLD_SKULLTULA_REWARD,
    RSK_EXCLUDE_KAK_30_GOLD_SKULLTULA_REWARD,
    RSK_EXCLUDE_KAK_40_GOLD_SKULLTULA_REWARD,
    RSK_EXCLUDE_KAK_50_GOLD_SKULLTULA_REWARD,
    RSK_SHUFFLE_CHEST_MINIGAME,
    RSK_CUCCO_COUNT,
    RSK_BIG_POE_COUNT,
    RSK_SKIP_EPONA_RACE,
    RSK_SKIP_TOWER_ESCAPE,
    RSK_SHUFFLE_ENTRANCES,
    RSK_SHUFFLE_DUNGEONS_ENTRANCES,
    RSK_SHUFFLE_OVERWORLD_ENTRANCES,
    RSK_SHUFFLE_INTERIORS_ENTRANCES,
    RSK_SHUFFLE_GROTTOS_ENTRANCES
} RandomizerSettingKey;
