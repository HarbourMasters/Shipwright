#include "static_data.h"

#define TWO_ACTOR_PARAMS(a, b) (abs(a) << 16) | abs(b)

std::array<Rando::Location, RC_MAX> StaticData::locationTable;

typedef enum {
    DUNGEON_DEKU_TREE = 0,
    DUNGEON_DODONGOS_CAVERN,
    DUNGEON_JABUJABUS_BELLY,
    DUNGEON_FOREST_TEMPLE,
    DUNGEON_FIRE_TEMPLE,
    DUNGEON_WATER_TEMPLE,
    DUNGEON_SPIRIT_TEMPLE,
    DUNGEON_SHADOW_TEMPLE,
    DUNGEON_BOTTOM_OF_THE_WELL,
    DUNGEON_ICE_CAVERN,
    DUNGEON_GANONS_CASTLE_SECOND_PART,
    DUNGEON_GERUDO_TRAINING_GROUNDS,
    DUNGEON_GERUDO_FORTRESS,
    DUNGEON_GANONS_CASTLE_FIRST_PART,
    DUNGEON_GANONS_CASTLE_FLOOR_BENEATH_BOSS_CHAMBER,
    DUNGEON_GANONS_CASTLE_CRUMBLING,
    DUNGEON_TREASURE_CHEST_SHOP,
    DUNGEON_DEKU_TREE_BOSS_ROOM,
    DUNGEON_DODONGOS_CAVERN_BOSS_ROOM,
    DUNGEON_JABUJABUS_BELLY_BOSS_ROOM,
} DungeonId;

void StaticData::InitLocationTable() {
    using namespace Rando;
    locationTable[RC_UNKNOWN_CHECK] = Location::Base(
        RC_UNKNOWN_CHECK, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_INVALID, ACTOR_ID_MAX, SCENE_ID_MAX, 0x00, 0x00,
        "Invalid Location", "Invalid Location", NONE, RG_NONE, {}, SpoilerCollectionCheck::None());

    // Kokiri Forest
    locationTable[RC_KF_KOKIRI_SWORD_CHEST] = Location::Chest(
        RC_KF_KOKIRI_SWORD_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KOKIRI_FOREST, ACTOR_EN_BOX, SCENE_SPOT04, 1248,
        0x00, "Kokiri Sword Chest", "KF Kokiri Sword Chest", RC_KF_KOKIRI_SWORD_CHEST, RG_KOKIRI_SWORD,
        {
            Category::cKokiriForest,
            Category::cForest,
        },
        SpoilerCollectionCheckGroup::GROUP_KOKIRI_FOREST);
    locationTable[RC_KF_MIDOS_TOP_LEFT_CHEST] =
        Location::Chest(RC_KF_MIDOS_TOP_LEFT_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KOKIRI_FOREST, ACTOR_EN_BOX,
                        SCENE_KOKIRI_HOME4, 22944, 0x00, "Mido Top Left Chest", "KF Mido Top Left Chest",
                        RC_KF_MIDOS_TOP_LEFT_CHEST, RG_BLUE_RUPEE,
                        {
                            Category::cKokiriForest,
                            Category::cForest,
                        },
                        SpoilerCollectionCheckGroup::GROUP_KOKIRI_FOREST);
    locationTable[RC_KF_MIDOS_TOP_RIGHT_CHEST] =
        Location::Chest(RC_KF_MIDOS_TOP_LEFT_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KOKIRI_FOREST, ACTOR_EN_BOX,
                        SCENE_KOKIRI_HOME4, 22944, 0x01, "Mido Top Right Chest", "KF Mido Top Right Chest",
                        RC_KF_MIDOS_TOP_RIGHT_CHEST, RG_BLUE_RUPEE,
                        {
                            Category::cKokiriForest,
                            Category::cForest,
                        },
                        SpoilerCollectionCheckGroup::GROUP_KOKIRI_FOREST);
    locationTable[RC_KF_MIDOS_BOTTOM_LEFT_CHEST] =
        Location::Chest(RC_KF_MIDOS_BOTTOM_LEFT_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KOKIRI_FOREST,
                        ACTOR_EN_BOX, SCENE_KOKIRI_HOME4, 22914, 0x02, "Mido Bottom Left Chest",
                        "KF Mido Bottom Left Chest", RC_KF_MIDOS_BOTTOM_LEFT_CHEST, RG_GREG_RUPEE,
                        {
                            Category::cKokiriForest,
                            Category::cForest,
                        },
                        SpoilerCollectionCheckGroup::GROUP_KOKIRI_FOREST);
    locationTable[RC_KF_MIDOS_BOTTOM_RIGHT_CHEST] =
        Location::Chest(RC_KF_MIDOS_BOTTOM_RIGHT_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KOKIRI_FOREST,
                        ACTOR_EN_BOX, SCENE_KOKIRI_HOME4, 22787, 0x03, "Mido Bottom Right Chest",
                        "KF Mido Bottom Right Chest", RC_KF_MIDOS_BOTTOM_RIGHT_CHEST, RG_RECOVERY_HEART,
                        {
                            Category::cKokiriForest,
                            Category::cForest,
                        },
                        SpoilerCollectionCheckGroup::GROUP_KOKIRI_FOREST);
    locationTable[RC_KF_STORMS_GROTTO_CHEST] = Location::Chest(
        RC_KF_STORMS_GROTTO_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KOKIRI_FOREST, ACTOR_EN_BOX, SCENE_KAKUSIANA,
        22988, 0x0C, "Storms Grotto Chest", "KF Storms Grotto Chest", RC_KF_STORMS_GROTTO_CHEST, RG_RED_RUPEE,
        { Category::cKokiriForest, Category::cForest, Category::cGrotto },
        SpoilerCollectionCheckGroup::GROUP_KOKIRI_FOREST);

    // Lost Woods
    locationTable[RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST] = Location::Chest(
        RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_LOST_WOODS, ACTOR_EN_BOX,
        SCENE_KAKUSIANA, 22964, 0x3E, "Near Shortcuts Grotto Chest", "LW Near Shortcuts Grotto Chest",
        RC_KF_STORMS_GROTTO_CHEST, RG_RED_RUPEE, { Category::cKokiriForest, Category::cForest, Category::cGrotto },
        SpoilerCollectionCheckGroup::GROUP_KOKIRI_FOREST);
    locationTable[RC_LW_SKULL_KID] =
        Location::Base(RC_LW_SKULL_KID, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_LOST_WOODS, ACTOR_ID_MAX, SCENE_SPOT10,
                       0x00, 0x3E, "Skull Kid", "LW Skull Kid", RC_LW_SKULL_KID, RG_PIECE_OF_HEART,
                       {
                           Category::cLostWoods,
                           Category::cForest,
                       },
                       SpoilerCollectionCheck::ItemGetInf(30), SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_LW_TRADE_COJIRO] = Location::Base(
        RC_LW_TRADE_COJIRO, RCQUEST_BOTH, RCTYPE_ADULT_TRADE, RCAREA_LOST_WOODS, ACTOR_ID_MAX, SCENE_SPOT10, 0x00, 0x1F,
        "Trade Cojiro", "LW Trade Cojiro", RC_LW_TRADE_COJIRO, RG_ODD_MUSHROOM,
        { Category::cLostWoods, Category::cForest, Category::cAdultTrade },
        SpoilerCollectionCheck::RandomizerInf(0x5B, 0x1F), SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_LW_TRADE_ODD_POTION] = Location::Base(
        RC_LW_TRADE_ODD_POTION, RCQUEST_BOTH, RCTYPE_ADULT_TRADE, RCAREA_LOST_WOODS, ACTOR_ID_MAX, SCENE_SPOT10, 0x00,
        0x21, "Trade Odd Potion", "LW Trade Odd Potion", RC_LW_TRADE_COJIRO, RG_ODD_MUSHROOM,
        { Category::cLostWoods, Category::cForest, Category::cAdultTrade },
        SpoilerCollectionCheck::RandomizerInf(0x5B, 0x1F), SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_LW_OCARINA_MEMORY_GAME] = Location::Base(
        RC_LW_OCARINA_MEMORY_GAME, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_LOST_WOODS, ACTOR_ID_MAX, SCENE_SPOT10, 0x00,
        0x21, "Ocarina Memory Game", "LW Ocarina Memory Game", RC_LW_OCARINA_MEMORY_GAME, RG_PIECE_OF_HEART,
        { Category::cLostWoods, Category::cForest, Category::cMinigame }, SpoilerCollectionCheck::ItemGetInf(31),
        SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_LW_TARGET_IN_WOODS] = Location::Base(
        RC_LW_TARGET_IN_WOODS, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_LOST_WOODS, ACTOR_ID_MAX, SCENE_SPOT10, 0x00, 0x60,
        "Target in Woods", "LW Target in Woods", RC_LW_TARGET_IN_WOODS, RG_PROGRESSIVE_SLINGSHOT,
        {
            Category::cLostWoods,
            Category::cForest,
        },
        SpoilerCollectionCheck::ItemGetInf(21), SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT] =
        Location::Base(RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_LOST_WOODS,
                       ACTOR_EN_DNS, SCENE_SPOT10, 0x00, 0x30, "Deku Scrub Near Deku Theater Right",
                       "LW Deku Scrub Near Deku Theater Right", RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT,
                       RG_BUY_DEKU_NUT_5, { Category::cLostWoods, Category::cForest, Category::cDekuScrub },
                       SpoilerCollectionCheck::Scrub(0x5B, 0x01), SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT] =
        Location::Base(RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_LOST_WOODS,
                       ACTOR_EN_DNS, SCENE_SPOT10, 0x01, 0x31, "Deku Scrub Near Deku Theater Left",
                       "LW Deku Scrub Near Deku Theater Left", RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT,
                       RG_BUY_DEKU_STICK_1, { Category::cLostWoods, Category::cForest, Category::cDekuScrub },
                       SpoilerCollectionCheck::Scrub(0x5B, 0x02), SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_LW_DEKU_SCRUB_NEAR_BRIDGE] =
        Location::Base(RC_LW_DEKU_SCRUB_NEAR_BRIDGE, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_LOST_WOODS, ACTOR_EN_DNS,
                       SCENE_SPOT10, 0x09, 0x77, "Deku Scrub Near Bridge", "LW Deku Scrub Near Bridge",
                       RC_LW_DEKU_SCRUB_NEAR_BRIDGE, RG_PROGRESSIVE_STICK_UPGRADE,
                       { Category::cLostWoods, Category::cForest, Category::cDekuScrub, Category::cDekuScrubUpgrades },
                       SpoilerCollectionCheck::Scrub(0x5B, 0x0A), SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_LW_DEKU_SCRUB_GROTTO_REAR] =
        Location::GrottoScrub(RC_LW_DEKU_SCRUB_GROTTO_REAR, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_LOST_WOODS, ACTOR_EN_DNS,
                              SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x03, 0xF5), 0x33, "Deku Scrub Grotto Rear",
                              "LW Deku Scrub Grotto Rear", RC_LW_DEKU_SCRUB_GROTTO_REAR, RG_BUY_DEKU_SEEDS_30,
                              { Category::cLostWoods, Category::cForest, Category::cDekuScrub, Category::cGrotto },
                              SpoilerCollectionCheck::Scrub(0x1F, 0x04), SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_LW_DEKU_SCRUB_GROTTO_FRONT] = Location::GrottoScrub(
        RC_LW_DEKU_SCRUB_GROTTO_FRONT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_LOST_WOODS, ACTOR_EN_DNS, SCENE_KAKUSIANA,
        TWO_ACTOR_PARAMS(0x0A, 0xF5), 0x79, "Deku Scrub Grotto Front", "LW Deku Scrub Grotto Front",
        RC_LW_DEKU_SCRUB_GROTTO_FRONT, RG_PROGRESSIVE_NUT_UPGRADE,
        { Category::cLostWoods, Category::cForest, Category::cDekuScrub, Category::cDekuScrubUpgrades,
          Category::cGrotto },
        SpoilerCollectionCheck::Scrub(0x1F, 0x0B), SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_DEKU_THEATER_SKULL_MASK] = Location::Base(
        RC_DEKU_THEATER_SKULL_MASK, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_LOST_WOODS, ACTOR_ID_MAX, SCENE_KAKUSIANA,
        0x00, 0x77, "Deku Theater Skull Mask", "Deku Theater Skull Mask", RC_DEKU_THEATER_SKULL_MASK,
        RG_PROGRESSIVE_STICK_UPGRADE, { Category::cLostWoods, Category::cForest, Category::cGrotto },
        SpoilerCollectionCheck::Chest(0x3E, 0x1F), SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_DEKU_THEATER_MASK_OF_TRUTH] =
        Location::Base(RC_DEKU_THEATER_MASK_OF_TRUTH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_LOST_WOODS, ACTOR_ID_MAX,
                       SCENE_KAKUSIANA, 0x00, 0x7A, "Deku Theater Mask of Truth", "Deku Theater Mask of Truth",
                       RC_DEKU_THEATER_MASK_OF_TRUTH, RG_PROGRESSIVE_NUT_UPGRADE,
                       { Category::cLostWoods, Category::cForest, Category::cNeedSpiritualStones, Category::cGrotto },
                       SpoilerCollectionCheck::Chest(0x3E, 0x1E), SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);

    // Sacred Forest Meadow
    locationTable[RC_SFM_WOLFOS_GROTTO_CHEST] = Location::Chest(
        RC_SFM_WOLFOS_GROTTO_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_SACRED_FOREST_MEADOW, ACTOR_EN_BOX,
        SCENE_KAKUSIANA, 31409, 0x11, "Wolfos Grotto Chest", "SFM Wolfos Grotto Chest", RC_SFM_WOLFOS_GROTTO_CHEST,
        RG_PURPLE_RUPEE, { Category::cSacredForestMeadow, Category::cForest, Category::cGrotto },
        SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_SFM_DEKU_SCRUB_GROTTO_REAR] = Location::GrottoScrub(
        RC_SFM_DEKU_SCRUB_GROTTO_REAR, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_SACRED_FOREST_MEADOW, ACTOR_EN_DNS,
        SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x07, 0xEE), 0x39, "Deku Scrub Grotto Rear", "SFM Deku Scrub Grotto Rear",
        RC_SFM_DEKU_SCRUB_GROTTO_REAR, RG_BUY_RED_POTION_30,
        { Category::cSacredForestMeadow, Category::cForest, Category::cDekuScrub, Category::cGrotto },
        SpoilerCollectionCheck::Scrub(0x18, 0x08), SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_SFM_DEKU_SCRUB_GROTTO_FRONT] = Location::GrottoScrub(
        RC_SFM_DEKU_SCRUB_GROTTO_FRONT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_SACRED_FOREST_MEADOW, ACTOR_EN_DNS,
        SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x08, 0xEE), 0x3A, "Deku Scrub Grotto Front", "SFM Deku Scrub Grotto Front",
        RC_SFM_DEKU_SCRUB_GROTTO_FRONT, RG_BUY_GREEN_POTION,
        { Category::cSacredForestMeadow, Category::cForest, Category::cDekuScrub, Category::cGrotto },
        SpoilerCollectionCheck::Scrub(0x18, 0x09), SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);

    // Hyrule Field
    locationTable[RC_HF_SOUTHEAST_GROTTO_CHEST] = Location::Chest(
        RC_HF_SOUTHEAST_GROTTO_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_HYRULE_FIELD, ACTOR_EN_BOX, SCENE_KAKUSIANA,
        22978, 0x02, "Southeast Grotto Chest", "HF Southeast Grotto Chest", RC_HF_SOUTHEAST_GROTTO_CHEST, RG_RED_RUPEE,
        {
            Category::cHyruleField,
            Category::cGrotto,
        },
        SpoilerCollectionCheckGroup::GROUP_HYRULE_FIELD);
    locationTable[RC_HF_OPEN_GROTTO_CHEST] = Location::Chest(
        RC_HF_OPEN_GROTTO_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_HYRULE_FIELD, ACTOR_EN_BOX, SCENE_KAKUSIANA,
        22947, 0x03, "Open Grotto Chest", "HF Open Grotto Chest", RC_HF_OPEN_GROTTO_CHEST, RG_BLUE_RUPEE,
        {
            Category::cHyruleField,
            Category::cGrotto,
        },
        SpoilerCollectionCheckGroup::GROUP_HYRULE_FIELD);
    locationTable[RC_HF_NEAR_MARKET_GROTTO_CHEST] =
        Location::Chest(RC_HF_NEAR_MARKET_GROTTO_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_HYRULE_FIELD,
                        ACTOR_EN_BOX, SCENE_KAKUSIANA, 22944, 0x00, "Near Market Grotto Chest",
                        "HF Near Market Grotto Chest", RC_HF_NEAR_MARKET_GROTTO_CHEST, RG_BLUE_RUPEE,
                        {
                            Category::cHyruleField,
                            Category::cGrotto,
                        },
                        SpoilerCollectionCheckGroup::GROUP_HYRULE_FIELD);
    locationTable[RC_HF_OCARINA_OF_TIME_ITEM] = Location::Base(
        RC_HF_OCARINA_OF_TIME_ITEM, RCQUEST_BOTH, RCTYPE_OCARINA, RCAREA_HYRULE_FIELD, ACTOR_ID_MAX, SCENE_SPOT00, 0x00,
        0x0C, "Ocarina of Time Item", "HF Ocarina of Time Item", RC_HF_OCARINA_OF_TIME_ITEM, RG_PROGRESSIVE_OCARINA,
        {
            Category::cHyruleField,
            Category::cNeedSpiritualStones,
        },
        SpoilerCollectionCheck::EventChkInf(0x43), SpoilerCollectionCheckGroup::GROUP_HYRULE_FIELD);
    locationTable[RC_HF_TEKTITE_GROTTO_FREESTANDING_POH] = Location::Collectable(
        RC_HF_TEKTITE_GROTTO_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_HYRULE_FIELD, ACTOR_EN_ITEM00,
        SCENE_KAKUSIANA, 262, 0x01, "Tektite Grotto Freestanding PoH", "HF Tektite Grotto Freestanding PoH",
        RC_HF_TEKTITE_GROTTO_FREESTANDING_POH, RG_PIECE_OF_HEART,
        {
            Category::cHyruleField,
            Category::cGrotto,
        },
        SpoilerCollectionCheckGroup::GROUP_HYRULE_FIELD);
    locationTable[RC_HF_DEKU_SCRUB_GROTTO] = Location::GrottoScrub(
        RC_HF_DEKU_SCRUB_GROTTO, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_HYRULE_FIELD, ACTOR_EN_DNS, SCENE_KAKUSIANA,
        TWO_ACTOR_PARAMS(0x02, 0xE6), 0x3E, "Deku Scrub Grotto", "HF Deku Scrub Grotto", RC_HF_DEKU_SCRUB_GROTTO,
        RG_PIECE_OF_HEART,
        { Category::cHyruleField, Category::cDekuScrub, Category::cDekuScrubUpgrades, Category::cGrotto },
        SpoilerCollectionCheck::Scrub(0x10, 0x03), SpoilerCollectionCheckGroup::GROUP_HYRULE_FIELD);
    locationTable[RC_LH_CHILD_FISHING] = Location::Base(
        RC_LH_CHILD_FISHING, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_LAKE_HYLIA, ACTOR_ID_MAX, SCENE_TURIBORI, 0x00, 0x3E,
        "Child Fishing", "LH Child Fishing", RC_LH_CHILD_FISHING, RG_PIECE_OF_HEART,
        {
            Category::cLakeHylia,
            Category::cMinigame,
        },
        SpoilerCollectionCheck::Fishing(0x02), SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);

    // Lake Hylia
    locationTable[RC_LH_ADULT_FISHING] = Location::Base(
        RC_LH_ADULT_FISHING, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_LAKE_HYLIA, ACTOR_ID_MAX, SCENE_TURIBORI, 0x00, 0x38,
        "Adult Fishing", "LH Adult Fishing", RC_LH_ADULT_FISHING, RG_PROGRESSIVE_SCALE,
        {
            Category::cLakeHylia,
            Category::cMinigame,
        },
        SpoilerCollectionCheck::Fishing(0x03), SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);
    locationTable[RC_LH_LAB_DIVE] =
        Location::Base(RC_LH_LAB_DIVE, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_LAKE_HYLIA, ACTOR_ID_MAX, SCENE_HYLIA_LABO,
                       0x00, 0x3E, "Lab Dive", "LH Lab Dive", RC_LH_LAB_DIVE, RG_PIECE_OF_HEART,
                       {
                           Category::cLakeHylia,
                       },
                       SpoilerCollectionCheck::ItemGetInf(24), SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);
    locationTable[RC_LH_TRADE_FROG] = Location::Base(
        RC_LH_TRADE_FROG, RCQUEST_BOTH, RCTYPE_ADULT_TRADE, RCAREA_LAKE_HYLIA, ACTOR_ID_MAX, SCENE_HYLIA_LABO, 0x00,
        0x25, "Lab Trade Eyeball Frog", "LH Lab Trade Eyeball Frog", RC_LH_TRADE_FROG, RG_EYEDROPS,
        { Category::cLakeHylia, Category::cAdultTrade }, SpoilerCollectionCheck::RandomizerInf(0x38, 0x1F),
        SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);
    locationTable[RC_LH_UNDERWATER_ITEM] = Location::Base(
        RC_LH_UNDERWATER_ITEM, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_LAKE_HYLIA, ACTOR_ID_MAX, SCENE_SPOT06, 0x00, 0x15,
        "Underwater Item", "LH Underwater Item", RC_LH_UNDERWATER_ITEM, RG_RUTOS_LETTER,
        {
            Category::cLakeHylia,
        },
        SpoilerCollectionCheck::EventChkInf(0x31), SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);
    locationTable[RC_LH_SUN] =
        Location::Base(RC_LH_SUN, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_LAKE_HYLIA, ACTOR_ID_MAX, SCENE_SPOT06, 0x00,
                       0x58, "Sun", "LH Sun", RC_LH_SUN, RG_FIRE_ARROWS,
                       {
                           Category::cLakeHylia,
                       },
                       SpoilerCollectionCheck::Chest(0x57, 0x1F), SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);
    locationTable[RC_LH_FREESTANDING_POH] = Location::Collectable(
        RC_LH_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_LAKE_HYLIA, ACTOR_EN_ITEM00, SCENE_SPOT06, 7686,
        0x1E, "Freestanding PoH", "LH Freestanding PoH", RC_LH_FREESTANDING_POH, RG_PIECE_OF_HEART,
        {
            Category::cLakeHylia,
        },
        SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);
    locationTable[RC_LH_DEKU_SCRUB_GROTTO_LEFT] =
        Location::GrottoScrub(RC_LH_DEKU_SCRUB_GROTTO_LEFT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_LAKE_HYLIA, ACTOR_EN_DNS,
                              SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x00, 0xEF), 0x30, "Deku Scrub Grotto Left",
                              "LH Deku Scrub Grotto Left", RC_LH_DEKU_SCRUB_GROTTO_LEFT, RG_BUY_DEKU_NUT_5,
                              { Category::cLakeHylia, Category::cDekuScrub, Category::cGrotto },
                              SpoilerCollectionCheck::Scrub(0x19, 0x01), SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);
    locationTable[RC_LH_DEKU_SCRUB_GROTTO_RIGHT] =
        Location::GrottoScrub(RC_LH_DEKU_SCRUB_GROTTO_RIGHT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_LAKE_HYLIA,
                              ACTOR_EN_DNS, SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x05, 0xEF), 0x37,
                              "Deku Scrub Grotto Right", "LH Deku Scrub Grotto Right", RC_LH_DEKU_SCRUB_GROTTO_RIGHT,
                              RG_BUY_BOMBS_535, { Category::cLakeHylia, Category::cDekuScrub, Category::cGrotto },
                              SpoilerCollectionCheck::Scrub(0x19, 0x06), SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);
    locationTable[RC_LH_DEKU_SCRUB_GROTTO_CENTER] =
        Location::GrottoScrub(RC_LH_DEKU_SCRUB_GROTTO_CENTER, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_LAKE_HYLIA,
                              ACTOR_EN_DNS, SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x03, 0xEF), 0x33,
                              "Deku Scrub Grotto Center", "LH Deku Scrub Grotto Center", RC_LH_DEKU_SCRUB_GROTTO_CENTER,
                              RG_BUY_DEKU_SEEDS_30, { Category::cLakeHylia, Category::cDekuScrub, Category::cGrotto },
                              SpoilerCollectionCheck::Scrub(0x19, 0x04), SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);
    // Gerudo Valley
    locationTable[RC_GV_CHEST] =
        Location::Chest(RC_GV_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GERUDO_VALLEY, ACTOR_EN_BOX, SCENE_SPOT09,
                        23200, 0x00, "Chest", "GV Chest", RC_GV_CHEST, RG_PURPLE_RUPEE,
                        {
                            Category::cGerudoValley,
                            Category::cGerudo,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GV_TRADE_SAW] = Location::Base(
        RC_GV_TRADE_SAW, RCQUEST_BOTH, RCTYPE_ADULT_TRADE, RCAREA_GERUDO_VALLEY, ACTOR_ID_MAX, SCENE_SPOT09, 0x00, 0x22,
        "Trade Saw", "GV Trade Saw", RC_GV_TRADE_SAW, RG_BROKEN_SWORD,
        { Category::cGerudoValley, Category::cGerudo, Category::cAdultTrade },
        SpoilerCollectionCheck::RandomizerInf(0x5A, 0x1F), SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GV_WATERFALL_FREESTANDING_POH] =
        Location::Collectable(RC_GV_WATERFALL_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GERUDO_VALLEY,
                              ACTOR_EN_ITEM00, SCENE_SPOT09, 262, 0x01, "Waterfall Freestanding PoH",
                              "GV Waterfall Freestanding PoH", RC_GV_WATERFALL_FREESTANDING_POH, RG_PIECE_OF_HEART,
                              {
                                  Category::cGerudoValley,
                                  Category::cGerudo,
                              },
                              SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GV_CRATE_FREESTANDING_POH] =
        Location::Collectable(RC_GV_CRATE_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GERUDO_VALLEY,
                              ACTOR_EN_ITEM00, SCENE_SPOT09, 518, 0x02, "Crate Freestanding PoH",
                              "GV Crate Freestanding PoH", RC_GV_CRATE_FREESTANDING_POH, RG_PIECE_OF_HEART,
                              {
                                  Category::cGerudoValley,
                                  Category::cGerudo,
                              },
                              SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GV_DEKU_SCRUB_GROTTO_REAR] = Location::GrottoScrub(
        RC_GV_DEKU_SCRUB_GROTTO_REAR, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_GERUDO_VALLEY, ACTOR_EN_DNS, SCENE_KAKUSIANA,
        TWO_ACTOR_PARAMS(0x07, 0xF0), 0x39, "Deku Scrub Grotto Rear", "GV Deku Scrub Grotto Rear",
        RC_GV_DEKU_SCRUB_GROTTO_FRONT, RG_BUY_GREEN_POTION,
        { Category::cGerudoValley, Category::cGerudo, Category::cDekuScrub, Category::cGrotto },
        SpoilerCollectionCheck::Scrub(0x1A, 0x09), SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GV_DEKU_SCRUB_GROTTO_FRONT] = Location::GrottoScrub(
        RC_GV_DEKU_SCRUB_GROTTO_FRONT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_GERUDO_VALLEY, ACTOR_EN_DNS, SCENE_KAKUSIANA,
        TWO_ACTOR_PARAMS(0x08, 0xF0), 0x3A, "Deku Scrub Grotto Front", "GV Deku Scrub Grotto Front",
        RC_GV_DEKU_SCRUB_GROTTO_FRONT, RG_BUY_GREEN_POTION,
        { Category::cGerudoValley, Category::cGerudo, Category::cDekuScrub, Category::cGrotto },
        SpoilerCollectionCheck::Scrub(0x1A, 0x09), SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);

    // Gerudo Fortress
    locationTable[RC_GF_CHEST] =
        Location::Chest(RC_GF_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GERUDO_FORTRESS, ACTOR_EN_BOX, SCENE_SPOT12,
                        1984, 0x00, "Chest", "GF Chest", RC_GF_CHEST, RG_PIECE_OF_HEART,
                        {
                            Category::cGerudoFortress,
                            Category::cGerudo,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GF_HBA_1000_POINTS] =
        Location::Base(RC_GF_HBA_1000_POINTS, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GERUDO_FORTRESS, ACTOR_ID_MAX,
                       SCENE_SPOT12, 0x00, 0x3E, "GF HBA 1000 Points", "GF HBA 1000 Points", RC_GF_HBA_1000_POINTS,
                       RG_PIECE_OF_HEART, { Category::cGerudoFortress, Category::cGerudo, Category::cMinigame },
                       SpoilerCollectionCheck::InfTable(0x19, 0x08), SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GF_HBA_1500_POINTS] =
        Location::Base(RC_GF_HBA_1500_POINTS, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GERUDO_FORTRESS, ACTOR_ID_MAX,
                       SCENE_SPOT12, 0x00, 0x30, "GF HBA 1500 Points", "GF HBA 1500 Points", RC_GF_HBA_1500_POINTS,
                       RG_PROGRESSIVE_BOW, { Category::cGerudoFortress, Category::cGerudo, Category::cMinigame },
                       SpoilerCollectionCheck::ItemGetInf(7), SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GF_GERUDO_MEMBERSHIP_CARD] =
        Location::Base(RC_GF_GERUDO_MEMBERSHIP_CARD, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GERUDO_FORTRESS,
                       ACTOR_ID_MAX, SCENE_GERUDOWAY, 0x00, 0x3A, "GF Gerudo Membership Card",
                       "GF Gerudo Membership Card", RC_GF_GERUDO_MEMBERSHIP_CARD, RG_GERUDO_MEMBERSHIP_CARD,
                       {
                           Category::cGerudoFortress,
                           Category::cGerudo,
                       },
                       SpoilerCollectionCheck::GerudoToken(), SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GF_NORTH_F1_CARPENTER] = Location::Collectable(
        RC_GF_NORTH_F1_CARPENTER, RCQUEST_BOTH, RCTYPE_GF_KEY, RCAREA_GERUDO_FORTRESS, ACTOR_EN_ITEM00, SCENE_GERUDOWAY,
        3089, 0x0C, "GF North F1 Carpenter", "GF North F1 Carpenter", RC_GF_NORTH_F1_CARPENTER,
        RG_GERUDO_FORTRESS_SMALL_KEY, { Category::cGerudoFortress, Category::cGerudo, Category::cVanillaGFSmallKey },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GF_NORTH_F2_CARPENTER] = Location::Collectable(
        RC_GF_NORTH_F2_CARPENTER, RCQUEST_BOTH, RCTYPE_GF_KEY, RCAREA_GERUDO_FORTRESS, ACTOR_EN_ITEM00, SCENE_GERUDOWAY,
        2577, 0x0A, "GF North F2 Carpenter", "GF North F2 Carpenter", RC_GF_NORTH_F2_CARPENTER,
        RG_GERUDO_FORTRESS_SMALL_KEY, { Category::cGerudoFortress, Category::cGerudo, Category::cVanillaGFSmallKey },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GF_SOUTH_F1_CARPENTER] = Location::Collectable(
        RC_GF_SOUTH_F1_CARPENTER, RCQUEST_BOTH, RCTYPE_GF_KEY, RCAREA_GERUDO_FORTRESS, ACTOR_EN_ITEM00, SCENE_GERUDOWAY,
        3601, 0x0E, "GF South F1 Carpenter", "GF South F1 Carpenter", RC_GF_SOUTH_F1_CARPENTER,
        RG_GERUDO_FORTRESS_SMALL_KEY, { Category::cGerudoFortress, Category::cGerudo, Category::cVanillaGFSmallKey },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GF_SOUTH_F2_CARPENTER] = Location::Collectable(
        RC_GF_SOUTH_F2_CARPENTER, RCQUEST_BOTH, RCTYPE_GF_KEY, RCAREA_GERUDO_FORTRESS, ACTOR_EN_ITEM00, SCENE_GERUDOWAY,
        3857, 0x0F, "GF South F2 Carpenter", "GF South F2 Carpenter", RC_GF_SOUTH_F2_CARPENTER,
        RG_GERUDO_FORTRESS_SMALL_KEY, { Category::cGerudoFortress, Category::cGerudo, Category::cVanillaGFSmallKey },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);

    // Haunted Wasteland
    locationTable[RC_WASTELAND_CHEST] =
        Location::Chest(RC_WASTELAND_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_WASTELAND, ACTOR_EN_BOX, SCENE_SPOT13,
                        -30048, 0x00, "Chest", "Wasteland Chest", RC_WASTELAND_CHEST, RG_PURPLE_RUPEE,
                        {
                            Category::cHauntedWasteland,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_WASTELAND_BOMBCHU_SALESMAN] = Location::Base(
        RC_WASTELAND_BOMBCHU_SALESMAN, RCQUEST_BOTH, RCTYPE_MERCHANT, RCAREA_WASTELAND, ACTOR_ID_MAX, SCENE_SPOT13,
        0x00, 0x03, "Carpet Salesman", "Wasteland Carpet Salesman", RC_WASTELAND_BOMBCHU_SALESMAN, RG_BOMBCHU_10,
        {
            Category::cHauntedWasteland,
            Category::cMerchant,
        },
        SpoilerCollectionCheck::Merchant(0x5E, 0x86), SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);

    // Desert Colossus
    locationTable[RC_COLOSSUS_FREESTANDING_POH] =
        Location::Collectable(RC_COLOSSUS_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_DESERT_COLOSSUS,
                              ACTOR_EN_ITEM00, SCENE_SPOT11, 3334, 0x0D, "Freestanding PoH",
                              "Colossus Freestanding PoH", RC_COLOSSUS_FREESTANDING_POH, RG_PIECE_OF_HEART,
                              {
                                  Category::cDesertColossus,
                              },
                              SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR] = Location::GrottoScrub(
        RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_DESERT_COLOSSUS, ACTOR_EN_DNS,
        SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x07, 0xFD), 0x39, "Deku Scrub Grotto Rear",
        "Colossus Deku Scrub Grotto Rear", RC_COLOSSUS_DEKU_SCRUB_GROTTO_REAR, RG_BUY_RED_POTION_30,
        { Category::cDesertColossus, Category::cDekuScrub, Category::cGrotto },
        SpoilerCollectionCheck::Scrub(0x27, 0x08), SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT] = Location::GrottoScrub(
        RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_DESERT_COLOSSUS, ACTOR_EN_DNS,
        SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x08, 0xFD), 0x3A, "Deku Scrub Grotto Front",
        "Colossus Deku Scrub Grotto Front", RC_COLOSSUS_DEKU_SCRUB_GROTTO_FRONT, RG_BUY_GREEN_POTION,
        { Category::cDesertColossus, Category::cDekuScrub, Category::cGrotto },
        SpoilerCollectionCheck::Scrub(0x27, 0x09), SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);

    // Market
    locationTable[RC_MARKET_TREASURE_CHEST_GAME_REWARD] =
        Location::Chest(RC_MARKET_TREASURE_CHEST_GAME_REWARD, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_MARKET,
                        ACTOR_EN_BOX, SCENE_TAKARAYA, 0x00, 0x0A, "Treasure Chest Game Reward",
                        "MK Treasure Chest Game Reward", RC_MARKET_TREASURE_CHEST_GAME_REWARD, RG_TREASURE_GAME_HEART,
                        { Category::cInnerMarket, Category::cMarket, Category::cMinigame },
                        SpoilerCollectionCheck::ItemGetInf(19), SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);
    locationTable[RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE] =
        Location::Base(RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_MARKET,
                       ACTOR_ID_MAX, SCENE_BOWLING, 0x00, 0x33, "Bombchu Bowling First Prize",
                       "MK Bombchu Bowling First Prize", RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE, RG_PROGRESSIVE_BOMB_BAG,
                       { Category::cInnerMarket, Category::cMarket, Category::cMinigame },
                       SpoilerCollectionCheck::ItemGetInf(25), SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);
    locationTable[RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE] =
        Location::Base(RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_MARKET,
                       ACTOR_ID_MAX, SCENE_BOWLING, 0x00, 0x3E, "Bombchu Bowling Second Prize",
                       "MK Bombchu Bowling Second Prize", RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE, RG_PIECE_OF_HEART,
                       { Category::cInnerMarket, Category::cMarket, Category::cMinigame },
                       SpoilerCollectionCheck::ItemGetInf(26), SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);
    locationTable[RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS] = Location::Base(
        RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_MARKET, ACTOR_ID_MAX, SCENE_BOWLING,
        0x00, 0x00, "Bombchu Bowling Bombchus", "MK Bombchu Bowling Bombchus", RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS,
        RG_BOMBCHU_DROP, { Category::cInnerMarket, Category::cMarket, Category::cMinigame },
        SpoilerCollectionCheck::None(), SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);
    locationTable[RC_MARKET_LOST_DOG] =
        Location::Base(RC_MARKET_LOST_DOG, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_MARKET, ACTOR_ID_MAX, SCENE_IMPA, 0x00,
                       0x3E, "Lost Dog", "MK Lost Dog", RC_MARKET_LOST_DOG, RG_PIECE_OF_HEART,
                       {
                           Category::cInnerMarket,
                           Category::cMarket,
                       },
                       SpoilerCollectionCheck::InfTable(0x19, 0x09), SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);
    locationTable[RC_MARKET_SHOOTING_GALLERY_REWARD] = Location::Base(
        RC_MARKET_SHOOTING_GALLERY_REWARD, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_MARKET, ACTOR_ID_MAX,
        SCENE_SYATEKIJYOU, 0x00, 0x60, "Shooting Gallery", "MK Shooting Gallery", RC_MARKET_SHOOTING_GALLERY_REWARD,
        RG_PROGRESSIVE_SLINGSHOT, { Category::cInnerMarket, Category::cMarket, Category::cMinigame },
        SpoilerCollectionCheck::ItemGetInf(5), SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);
    locationTable[RC_MARKET_10_BIG_POES] = Location::Base(
        RC_MARKET_10_BIG_POES, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_MARKET, ACTOR_ID_MAX, SCENE_MIHARIGOYA, 0x00, 0x0F,
        "10 Big Poes", "MK 10 Big Poes", RC_MARKET_10_BIG_POES, RG_EMPTY_BOTTLE,
        {
            Category::cInnerMarket,
            Category::cMarket,
        },
        SpoilerCollectionCheck::BigPoePoints(), SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);
    locationTable[RC_MARKET_TREASURE_CHEST_GAME_ITEM_1] = Location::Chest(
        RC_MARKET_TREASURE_CHEST_GAME_ITEM_1, RCQUEST_BOTH, RCTYPE_CHEST_GAME, RCAREA_MARKET, ACTOR_EN_BOX,
        SCENE_TAKARAYA, 0x00, 0x01, "Chest Game First Room Chest", "MK Chest Game First Room Chest",
        RC_MARKET_TREASURE_CHEST_GAME_ITEM_1, RG_TREASURE_GAME_SMALL_KEY,
        { Category::cInnerMarket, Category::cMarket, Category::cMinigame, Category::cChestMinigame },
        SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);
    locationTable[RC_MARKET_TREASURE_CHEST_GAME_ITEM_2] = Location::Chest(
        RC_MARKET_TREASURE_CHEST_GAME_ITEM_2, RCQUEST_BOTH, RCTYPE_CHEST_GAME, RCAREA_MARKET, ACTOR_EN_BOX,
        SCENE_TAKARAYA, 0x00, 0x03, "Chest Game Second Room Chest", "MK Chest Game Second Room Chest",
        RC_MARKET_TREASURE_CHEST_GAME_ITEM_2, RG_TREASURE_GAME_SMALL_KEY,
        { Category::cInnerMarket, Category::cMarket, Category::cMinigame, Category::cChestMinigame },
        SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);
    locationTable[RC_MARKET_TREASURE_CHEST_GAME_ITEM_3] = Location::Chest(
        RC_MARKET_TREASURE_CHEST_GAME_ITEM_3, RCQUEST_BOTH, RCTYPE_CHEST_GAME, RCAREA_MARKET, ACTOR_EN_BOX,
        SCENE_TAKARAYA, 0x00, 0x05, "Chest Game Third Room Chest", "MK Chest Game Third Room Chest",
        RC_MARKET_TREASURE_CHEST_GAME_ITEM_3, RG_TREASURE_GAME_SMALL_KEY,
        { Category::cInnerMarket, Category::cMarket, Category::cMinigame, Category::cChestMinigame },
        SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);
    locationTable[RC_MARKET_TREASURE_CHEST_GAME_ITEM_4] = Location::Chest(
        RC_MARKET_TREASURE_CHEST_GAME_ITEM_4, RCQUEST_BOTH, RCTYPE_CHEST_GAME, RCAREA_MARKET, ACTOR_EN_BOX,
        SCENE_TAKARAYA, 0x00, 0x07, "Chest Game Fourth Room Chest", "MK Chest Game Fourth Room Chest",
        RC_MARKET_TREASURE_CHEST_GAME_ITEM_4, RG_TREASURE_GAME_SMALL_KEY,
        { Category::cInnerMarket, Category::cMarket, Category::cMinigame, Category::cChestMinigame },
        SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);
    locationTable[RC_MARKET_TREASURE_CHEST_GAME_ITEM_5] = Location::Chest(
        RC_MARKET_TREASURE_CHEST_GAME_ITEM_5, RCQUEST_BOTH, RCTYPE_CHEST_GAME, RCAREA_MARKET, ACTOR_EN_BOX,
        SCENE_TAKARAYA, 0x00, 0x09, "Chest Game Fifth Room Chest", "MK Chest Game Fifth Room Chest",
        RC_MARKET_TREASURE_CHEST_GAME_ITEM_5, RG_TREASURE_GAME_SMALL_KEY,
        { Category::cInnerMarket, Category::cMarket, Category::cMinigame, Category::cChestMinigame },
        SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);

    // Hyrule Castle
    locationTable[RC_HC_MALON_EGG] =
        Location::Base(RC_HC_MALON_EGG, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_HYRULE_CASTLE, ACTOR_ID_MAX, SCENE_SPOT15,
                       0x00, 0x47, "Malon Egg", "HC Malon Egg", RC_HC_MALON_EGG, RG_WEIRD_EGG,
                       {
                           Category::cHyruleCastle,
                           Category::cMarket,
                       },
                       SpoilerCollectionCheck::EventChkInf(0x12), SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);
    locationTable[RC_HC_ZELDAS_LETTER] = Location::Base(
        RC_HC_ZELDAS_LETTER, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_HYRULE_CASTLE, ACTOR_ID_MAX, SCENE_NAKANIWA, 0x00,
        0x0B, "Zeldas Letter", "HC Zeldas Letter", RC_HC_ZELDAS_LETTER, RG_ZELDAS_LETTER,
        {
            Category::cHyruleCastle,
            Category::cMarket,
        },
        SpoilerCollectionCheck::EventChkInf(0x40), SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);

    // Kakariko
    locationTable[RC_KAK_REDEAD_GROTTO_CHEST] = Location::Chest(
        RC_KAK_REDEAD_GROTTO_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KAKARIKO_VILLAGE, ACTOR_EN_BOX,
        SCENE_KAKUSIANA, 31434, 0x0A, "Redead Grotto Chest", "Kak Redead Grotto Chest", RC_KAK_REDEAD_GROTTO_CHEST,
        RG_HUGE_RUPEE, { Category::cKakarikoVillage, Category::cKakariko, Category::cGrotto },
        SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_OPEN_GROTTO_CHEST] = Location::Chest(
        RC_KAK_OPEN_GROTTO_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KAKARIKO_VILLAGE, ACTOR_EN_BOX, SCENE_KAKUSIANA,
        22984, 0x08, "Open Grotto Chest", "Kak Open Grotto Chest", RC_KAK_OPEN_GROTTO_CHEST, RG_RED_RUPEE,
        { Category::cKakarikoVillage, Category::cKakariko, Category::cGrotto },
        SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_10_GOLD_SKULLTULA_REWARD] =
        Location::Base(RC_KAK_10_GOLD_SKULLTULA_REWARD, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KAKARIKO_VILLAGE,
                       ACTOR_ID_MAX, SCENE_KINSUTA, 0x00, 0x45, "10 Gold Skulltula Reward",
                       "Kak 10 Gold Skulltula Reward", RC_KAK_10_GOLD_SKULLTULA_REWARD, RG_PROGRESSIVE_WALLET,
                       { Category::cKakarikoVillage, Category::cKakariko, Category::cSkulltulaHouse },
                       SpoilerCollectionCheck::EventChkInf(0xDA), SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_20_GOLD_SKULLTULA_REWARD] =
        Location::Base(RC_KAK_20_GOLD_SKULLTULA_REWARD, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KAKARIKO_VILLAGE,
                       ACTOR_ID_MAX, SCENE_KINSUTA, 0x00, 0x39, "20 Gold Skulltula Reward",
                       "Kak 20 Gold Skulltula Reward", RC_KAK_20_GOLD_SKULLTULA_REWARD, RG_STONE_OF_AGONY,
                       { Category::cKakarikoVillage, Category::cKakariko, Category::cSkulltulaHouse },
                       SpoilerCollectionCheck::EventChkInf(0xDB), SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_30_GOLD_SKULLTULA_REWARD] =
        Location::Base(RC_KAK_30_GOLD_SKULLTULA_REWARD, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KAKARIKO_VILLAGE,
                       ACTOR_ID_MAX, SCENE_KINSUTA, 0x00, 0x46, "30 Gold Skulltula Reward",
                       "Kak 30 Gold Skulltula Reward", RC_KAK_30_GOLD_SKULLTULA_REWARD, RG_PROGRESSIVE_WALLET,
                       { Category::cKakarikoVillage, Category::cKakariko, Category::cSkulltulaHouse },
                       SpoilerCollectionCheck::EventChkInf(0xDC), SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_40_GOLD_SKULLTULA_REWARD] =
        Location::Base(RC_KAK_40_GOLD_SKULLTULA_REWARD, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KAKARIKO_VILLAGE,
                       ACTOR_ID_MAX, SCENE_KINSUTA, 0x00, 0x03, "40 Gold Skulltula Reward",
                       "Kak 40 Gold Skulltula Reward", RC_KAK_40_GOLD_SKULLTULA_REWARD, RG_BOMBCHU_10,
                       { Category::cKakarikoVillage, Category::cKakariko, Category::cSkulltulaHouse },
                       SpoilerCollectionCheck::EventChkInf(0xDD), SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_50_GOLD_SKULLTULA_REWARD] =
        Location::Base(RC_KAK_50_GOLD_SKULLTULA_REWARD, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KAKARIKO_VILLAGE,
                       ACTOR_ID_MAX, SCENE_KINSUTA, 0x00, 0x3E, "50 Gold Skulltula Reward",
                       "Kak 50 Gold Skulltula Reward", RC_KAK_50_GOLD_SKULLTULA_REWARD, RG_PIECE_OF_HEART,
                       { Category::cKakarikoVillage, Category::cKakariko, Category::cSkulltulaHouse },
                       SpoilerCollectionCheck::EventChkInf(0xDE), SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_100_GOLD_SKULLTULA_REWARD] =
        Location::Base(RC_KAK_100_GOLD_SKULLTULA_REWARD, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KAKARIKO_VILLAGE,
                       ACTOR_ID_MAX, SCENE_KINSUTA, 0x00, 0x3E, "100 Gold Skulltula Reward",
                       "Kak 100 Gold Skulltula Reward", RC_KAK_100_GOLD_SKULLTULA_REWARD, RG_HUGE_RUPEE,
                       { Category::cKakarikoVillage, Category::cKakariko, Category::cSkulltulaHouse },
                       SpoilerCollectionCheck::EventChkInf(0xDF), SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_MAN_ON_ROOF] = Location::Base(
        RC_KAK_MAN_ON_ROOF, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KAKARIKO_VILLAGE, ACTOR_ID_MAX, SCENE_SPOT01, 0x00,
        0x3E, "Man on Roof", "Kak Man on Roof", RC_KAK_MAN_ON_ROOF, RG_PIECE_OF_HEART,
        {
            Category::cKakarikoVillage,
            Category::cKakariko,
        },
        SpoilerCollectionCheck::ItemGetInf(29), SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_SHOOTING_GALLERY_REWARD] =
        Location::Base(RC_KAK_SHOOTING_GALLERY_REWARD, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KAKARIKO_VILLAGE,
                       ACTOR_ID_MAX, SCENE_SYATEKIJYOU, 0x00, 0x30, "Shooting Gallery Reward",
                       "Kak Shooting Gallery Reward", RC_KAK_SHOOTING_GALLERY_REWARD, RG_PROGRESSIVE_BOW,
                       { Category::cKakarikoVillage, Category::cKakariko, Category::cMinigame },
                       SpoilerCollectionCheck::Chest(0x42, 0x1F), SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_TRADE_ODD_MUSHROOM] = Location::Base(
        RC_KAK_TRADE_ODD_MUSHROOM, RCQUEST_BOTH, RCTYPE_ADULT_TRADE, RCAREA_KAKARIKO_VILLAGE, ACTOR_ID_MAX,
        SCENE_MAHOUYA, 0x00, 0x20, "Trade Odd Mushroom", "Kak Trade Odd Mushroom", RC_KAK_TRADE_ODD_MUSHROOM,
        RG_ODD_POTION, { Category::cKakarikoVillage, Category::cKakariko, Category::cAdultTrade },
        SpoilerCollectionCheck::ItemGetInf(56), SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_ANJU_AS_ADULT] = Location::Base(
        RC_KAK_ANJU_AS_ADULT, RCQUEST_BOTH, RCTYPE_ADULT_TRADE, RCAREA_KAKARIKO_VILLAGE, ACTOR_ID_MAX, SCENE_SPOT01,
        0x00, 0x1D, "Anju as Adult", "Kak Anju as Adult", RC_KAK_ANJU_AS_ADULT, RG_CLAIM_CHECK,
        {
            Category::cKakarikoVillage,
            Category::cKakariko,
        },
        SpoilerCollectionCheck::ItemGetInf(36), SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_ANJU_AS_CHILD] =
        Location::Base(RC_KAK_ANJU_AS_CHILD, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KAKARIKO_VILLAGE, ACTOR_ID_MAX,
                       SCENE_SPOT01, 0x00, 0x0F, "Anju as Child", "Kak Anju as Child", RC_KAK_ANJU_AS_CHILD,
                       RG_EMPTY_BOTTLE, { Category::cKakarikoVillage, Category::cKakariko, Category::cMinigame },
                       SpoilerCollectionCheck::ItemGetInf(4), SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_TRADE_POCKET_CUCCO] = Location::Base(
        RC_KAK_TRADE_POCKET_CUCCO, RCQUEST_BOTH, RCTYPE_ADULT_TRADE, RCAREA_KAKARIKO_VILLAGE, ACTOR_ID_MAX,
        SCENE_SPOT01, 0x00, 0x0E, "Trade Pocket Cucco", "Kak Trade Pocket Cucco", RC_KAK_TRADE_POCKET_CUCCO, RG_COJIRO,
        { Category::cKakarikoVillage, Category::cKakariko, Category::cAdultTrade },
        SpoilerCollectionCheck::ItemGetInf(38), SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_IMPAS_HOUSE_FREESTANDING_POH] = Location::Collectable(
        RC_KAK_IMPAS_HOUSE_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KAKARIKO_VILLAGE, ACTOR_EN_ITEM00,
        SCENE_LABO, 262, 0x01, "Impas House Freestanding PoH", "Kak Impas House Freestanding PoH",
        RC_KAK_IMPAS_HOUSE_FREESTANDING_POH, RG_PIECE_OF_HEART,
        {
            Category::cKakarikoVillage,
            Category::cKakariko,
        },
        SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_WINDMILL_FREESTANDING_POH] =
        Location::Collectable(RC_KAK_WINDMILL_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_KAKARIKO_VILLAGE,
                              ACTOR_EN_ITEM00, SCENE_HAKASITARELAY, 262, 0x01, "Windmill Freestanding PoH",
                              "Kak Windmill Freestanding PoH", RC_KAK_WINDMILL_FREESTANDING_POH, RG_PIECE_OF_HEART,
                              {
                                  Category::cKakarikoVillage,
                                  Category::cKakariko,
                              },
                              SpoilerCollectionCheckGroup::GROUP_KAKARIKO);

    // Graveyard
    locationTable[RC_GRAVEYARD_SHIELD_GRAVE_CHEST] = Location::Chest(
        RC_GRAVEYARD_SHIELD_GRAVE_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GRAVEYARD, ACTOR_EN_BOX, SCENE_HAKAANA2,
        21824, 0x00, "Shield Grave Chest", "GY Shield Grave Chest", RC_GRAVEYARD_SHIELD_GRAVE_CHEST, RG_HYLIAN_SHIELD,
        {
            Category::cGraveyard,
            Category::cKakariko,
        },
        SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST] =
        Location::Chest(RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GRAVEYARD,
                        ACTOR_EN_BOX, SCENE_HAKAANA, -22592, 0x00, "Heart Piece Grave Chest",
                        "GY Heart Piece Grave Chest", RC_GRAVEYARD_HEART_PIECE_GRAVE_CHEST, RG_PIECE_OF_HEART,
                        {
                            Category::cGraveyard,
                            Category::cKakariko,
                        },
                        SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST] =
        Location::Chest(RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GRAVEYARD,
                        ACTOR_EN_BOX, SCENE_HAKAANA_OUKE, -32736, 0x00, "Composers Grave Chest",
                        "GY Composers Grave Chest", RC_GRAVEYARD_ROYAL_FAMILYS_TOMB_CHEST, RG_BOMBS_5,
                        {
                            Category::cGraveyard,
                            Category::cKakariko,
                        },
                        SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_GRAVEYARD_HOOKSHOT_CHEST] = Location::Chest(
        RC_GRAVEYARD_HOOKSHOT_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GRAVEYARD, ACTOR_EN_BOX, SCENE_HAKASITARELAY,
        4352, 0x00, "Hookshot Chest", "GY Hookshot Chest", RC_GRAVEYARD_HOOKSHOT_CHEST, RG_PROGRESSIVE_HOOKSHOT,
        {
            Category::cGraveyard,
            Category::cKakariko,
        },
        SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH] =
        Location::Collectable(RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GRAVEYARD,
                              ACTOR_EN_ITEM00, SCENE_HAKASITARELAY, 1798, 0x07, "Dampe Race Freestanding PoH",
                              "GY Dampe Race Freestanding PoH", RC_GRAVEYARD_DAMPE_RACE_FREESTANDING_POH,
                              RG_PIECE_OF_HEART, { Category::cGraveyard, Category::cKakariko, Category::cMinigame },
                              SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR] = Location::Collectable(
        RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GRAVEYARD, ACTOR_EN_ITEM00,
        SCENE_SPOT02, 7942, 0x08, "Dampe Gravedigging Tour", "GY Dampe Gravedigging Tour",
        RC_GRAVEYARD_DAMPE_GRAVEDIGGING_TOUR, RG_PIECE_OF_HEART,
        {
            Category::cGraveyard,
            Category::cKakariko,
        },
        SpoilerCollectionCheck::Gravedigger(0x53, 0x1F), SpoilerCollectionCheckGroup::GROUP_KAKARIKO);

    // Death Mountain
    locationTable[RC_DMT_CHEST] =
        Location::Chest(RC_DMT_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_DEATH_MOUNTAIN_TRAIL, ACTOR_EN_BOX,
                        SCENE_SPOT16, 23201, 0x01, "Chest", "DMT Chest", RC_DMT_CHEST, RG_PURPLE_RUPEE,
                        {
                            Category::cDeathMountainTrail,
                            Category::cDeathMountain,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMT_STORMS_GROTTO_CHEST] = Location::Chest(
        RC_DMT_STORMS_GROTTO_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_DEATH_MOUNTAIN_TRAIL, ACTOR_EN_BOX,
        SCENE_KAKUSIANA, 23255, 0x17, "Storms Grotto Chest", "DMT Storms Grotto Chest", RC_DMT_STORMS_GROTTO_CHEST,
        RG_HUGE_RUPEE, { Category::cDeathMountainTrail, Category::cDeathMountain, Category::cGrotto },
        SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMT_TRADE_BROKEN_SWORD] = Location::Base(
        RC_DMT_TRADE_BROKEN_SWORD, RCQUEST_BOTH, RCTYPE_ADULT_TRADE, RCAREA_DEATH_MOUNTAIN_TRAIL, ACTOR_ID_MAX,
        SCENE_SPOT16, 0x00, 0x23, "Trade Broken Sword", "DMT Trade Broken Sword", RC_DMT_TRADE_BROKEN_SWORD,
        RG_PRESCRIPTION, { Category::cDeathMountainTrail, Category::cDeathMountain, Category::cAdultTrade },
        SpoilerCollectionCheck::RandomizerInf(0x60, 0x1D), SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMT_TRADE_EYEDROPS] = Location::Base(
        RC_DMT_TRADE_EYEDROPS, RCQUEST_BOTH, RCTYPE_ADULT_TRADE, RCAREA_DEATH_MOUNTAIN_TRAIL, ACTOR_ID_MAX,
        SCENE_SPOT16, 0x00, 0x26, "Trade Eyedrops", "DMT Trade Eyedrops", RC_DMT_TRADE_EYEDROPS, RG_CLAIM_CHECK,
        { Category::cDeathMountainTrail, Category::cDeathMountain, Category::cAdultTrade },
        SpoilerCollectionCheck::RandomizerInf(0x60, 0x1E), SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMT_TRADE_CLAIM_CHECK] = Location::Base(
        RC_DMT_TRADE_CLAIM_CHECK, RCQUEST_BOTH, RCTYPE_ADULT_TRADE, RCAREA_DEATH_MOUNTAIN_TRAIL, ACTOR_ID_MAX,
        SCENE_SPOT16, 0x00, 0x57, "Trade Claim Check", "DMT Trade Claim Check", RC_DMT_TRADE_CLAIM_CHECK,
        RG_BIGGORON_SWORD, { Category::cDeathMountainTrail, Category::cDeathMountain },
        SpoilerCollectionCheck::Chest(0x60, 0x1F), SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMT_FREESTANDING_POH] =
        Location::Collectable(RC_DMT_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_DEATH_MOUNTAIN_TRAIL,
                              ACTOR_EN_ITEM00, SCENE_SPOT16, 7686, 0x1E, "Freestanding PoH", "DMT Freestanding PoH",
                              RC_DMT_FREESTANDING_POH, RG_PIECE_OF_HEART,
                              {
                                  Category::cDeathMountainTrail,
                                  Category::cDeathMountain,
                              },
                              SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);

    // Goron City
    locationTable[RC_GC_MAZE_LEFT_CHEST] = Location::Chest(
        RC_GC_MAZE_LEFT_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GORON_CITY, ACTOR_EN_BOX, SCENE_SPOT18, 23232,
        0x00, "Maze Left Chest", "GC Maze Left Chest", RC_GC_MAZE_LEFT_CHEST, RG_HUGE_RUPEE,
        {
            Category::cGoronCity,
        },
        SpoilerCollectionCheckGroup::GROUP_GORON_CITY);
    locationTable[RC_GC_MAZE_RIGHT_CHEST] = Location::Chest(
        RC_GC_MAZE_RIGHT_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GORON_CITY, ACTOR_EN_BOX, SCENE_SPOT18, 23201,
        0x01, "Maze Right Chest", "GC Maze Right Chest", RC_GC_MAZE_RIGHT_CHEST, RG_PURPLE_RUPEE,
        {
            Category::cGoronCity,
        },
        SpoilerCollectionCheckGroup::GROUP_GORON_CITY);
    locationTable[RC_GC_MAZE_CENTER_CHEST] = Location::Chest(
        RC_GC_MAZE_CENTER_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GORON_CITY, ACTOR_EN_BOX, SCENE_SPOT18, 23202,
        0x02, "Maze Center Chest", "GC Maze Center Chest", RC_GC_MAZE_CENTER_CHEST, RG_PURPLE_RUPEE,
        {
            Category::cGoronCity,
        },
        SpoilerCollectionCheckGroup::GROUP_GORON_CITY);
    locationTable[RC_GC_ROLLING_GORON_AS_CHILD] =
        Location::Base(RC_GC_ROLLING_GORON_AS_CHILD, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GORON_CITY, ACTOR_ID_MAX,
                       SCENE_SPOT18, 0x00, 0x34, "Rolling Goron as Child", "GC Rolling Goron as Child",
                       RC_GC_ROLLING_GORON_AS_CHILD, RG_PROGRESSIVE_BOMB_BAG,
                       {
                           Category::cGoronCity,
                       },
                       SpoilerCollectionCheck::InfTable(0x11, 0x06), SpoilerCollectionCheckGroup::GROUP_GORON_CITY);
    locationTable[RC_GC_ROLLING_GORON_AS_ADULT] = Location::Base(
        RC_GC_ROLLING_GORON_AS_ADULT, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GORON_CITY, ACTOR_ID_MAX, SCENE_SPOT18,
        0x00, 0x2C, "Rolling Goron as Adult", "GC Rolling Goron as Adult", RC_GC_ROLLING_GORON_AS_ADULT, RG_GORON_TUNIC,
        {
            Category::cGoronCity,
        },
        SpoilerCollectionCheck::InfTable(0x10, 0x01), SpoilerCollectionCheckGroup::GROUP_GORON_CITY);
    locationTable[RC_GC_DARUNIAS_JOY] =
        Location::Base(RC_GC_DARUNIAS_JOY, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GORON_CITY, ACTOR_ID_MAX, SCENE_SPOT18,
                       0x00, 0x54, "Darunias Joy", "GC Darunias Joy", RC_GC_DARUNIAS_JOY, RG_PROGRESSIVE_STRENGTH,
                       {
                           Category::cGoronCity,
                       },
                       SpoilerCollectionCheck::Chest(0x62, 0x1E), SpoilerCollectionCheckGroup::GROUP_GORON_CITY);
    locationTable[RC_GC_POT_FREESTANDING_POH] = Location::Collectable(
        RC_GC_POT_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_GORON_CITY, ACTOR_EN_ITEM00, SCENE_SPOT18,
        7942, 0x1F, "Pot Freestanding PoH", "GC Pot Freestanding PoH", RC_GC_POT_FREESTANDING_POH, RG_PIECE_OF_HEART,
        {
            Category::cGoronCity,
        },
        SpoilerCollectionCheckGroup::GROUP_GORON_CITY);
    locationTable[RC_GC_DEKU_SCRUB_GROTTO_LEFT] =
        Location::GrottoScrub(RC_GC_DEKU_SCRUB_GROTTO_LEFT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_GORON_CITY, ACTOR_EN_DNS,
                              SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x00, 0xFB), 0x30, "Deku Scrub Grotto Left",
                              "GC Deku Scrub Grotto Left", RC_GC_DEKU_SCRUB_GROTTO_LEFT, RG_BUY_DEKU_NUT_5,
                              { Category::cGoronCity, Category::cDekuScrub, Category::cGrotto },
                              SpoilerCollectionCheck::Scrub(0x25, 0x01), SpoilerCollectionCheckGroup::GROUP_GORON_CITY);
    locationTable[RC_GC_DEKU_SCRUB_GROTTO_RIGHT] =
        Location::GrottoScrub(RC_GC_DEKU_SCRUB_GROTTO_RIGHT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_GORON_CITY,
                              ACTOR_EN_DNS, SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x05, 0xFB), 0x37,
                              "Deku Scrub Grotto Right", "GC Deku Scrub Grotto Right", RC_GC_DEKU_SCRUB_GROTTO_RIGHT,
                              RG_BUY_BOMBS_535, { Category::cGoronCity, Category::cDekuScrub, Category::cGrotto },
                              SpoilerCollectionCheck::Scrub(0x25, 0x06), SpoilerCollectionCheckGroup::GROUP_GORON_CITY);
    locationTable[RC_GC_DEKU_SCRUB_GROTTO_CENTER] =
        Location::GrottoScrub(RC_GC_DEKU_SCRUB_GROTTO_CENTER, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_GORON_CITY,
                              ACTOR_EN_DNS, SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x03, 0xFB), 0x33,
                              "Deku Scrub Grotto Center", "GC Deku Scrub Grotto Center", RC_GC_DEKU_SCRUB_GROTTO_CENTER,
                              RG_BUY_ARROWS_30, { Category::cGoronCity, Category::cDekuScrub, Category::cGrotto },
                              SpoilerCollectionCheck::Scrub(0x25, 0x04), SpoilerCollectionCheckGroup::GROUP_GORON_CITY);
    locationTable[RC_GC_MEDIGORON] =
        Location::Base(RC_GC_MEDIGORON, RCQUEST_BOTH, RCTYPE_MERCHANT, RCAREA_GORON_CITY, ACTOR_ID_MAX, SCENE_SPOT18,
                       0x00, 0x51, "Medigoron", "GC Medigoron", RC_GC_MEDIGORON, RG_GIANTS_KNIFE,
                       {
                           Category::cGoronCity,
                           Category::cMerchant,
                       },
                       SpoilerCollectionCheck::Merchant(0x62, 0x87), SpoilerCollectionCheckGroup::GROUP_GORON_CITY);

    // Death Mountain Crater
    locationTable[RC_DMC_UPPER_GROTTO_CHEST] = Location::Chest(
        RC_DMC_UPPER_GROTTO_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_DEATH_MOUNTAIN_CRATER, ACTOR_EN_BOX,
        SCENE_KAKUSIANA, 23802, 0x1A, "Upper Grotto Chest", "DMC Upper Grotto Chest", RC_DMC_UPPER_GROTTO_CHEST,
        RG_BOMBS_20, { Category::cDeathMountainCrater, Category::cDeathMountain, Category::cGrotto },
        SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMC_WALL_FREESTANDING_POH] =
        Location::Collectable(RC_DMC_WALL_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_DEATH_MOUNTAIN_CRATER,
                              ACTOR_EN_ITEM00, SCENE_SPOT17, 518, GI_HEART_PIECE, "Wall Freestanding PoH",
                              "DMC Wall Freestanding PoH", RC_DMC_WALL_FREESTANDING_POH, RG_PIECE_OF_HEART,
                              {
                                  Category::cDeathMountainCrater,
                                  Category::cDeathMountain,
                              },
                              SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMC_VOLCANO_FREESTANDING_POH] = Location::Collectable(
        RC_DMC_VOLCANO_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_DEATH_MOUNTAIN_CRATER, ACTOR_EN_ITEM00,
        SCENE_SPOT17, 2054, 0x08, "Volcano Freestanding PoH", "DMC Volcano Freestanding PoH",
        RC_DMC_WALL_FREESTANDING_POH, RG_PIECE_OF_HEART,
        {
            Category::cDeathMountainCrater,
            Category::cDeathMountain,
        },
        SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMC_DEKU_SCRUB] =
        Location::Base(RC_DMC_DEKU_SCRUB, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_DEATH_MOUNTAIN_CRATER, ACTOR_EN_DNS,
                       SCENE_SPOT17, 0x05, 0x37, "Deku Scrub", "DMC Deku Scrub", RC_DMC_DEKU_SCRUB, RG_BUY_BOMBS_535,
                       { Category::cDeathMountainCrater, Category::cDeathMountain, Category::cDekuScrub },
                       SpoilerCollectionCheck::Scrub(0x61, 0x06), SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMC_DEKU_SCRUB_GROTTO_LEFT] = Location::GrottoScrub(
        RC_DMC_DEKU_SCRUB_GROTTO_LEFT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_DEATH_MOUNTAIN_CRATER, ACTOR_EN_DNS,
        SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x00, 0xF9), 0x30, "Deku Scrub Grotto Left", "DMC Deku Scrub Grotto Left",
        RC_DMC_DEKU_SCRUB_GROTTO_LEFT, RG_BUY_DEKU_NUT_5,
        { Category::cDeathMountainCrater, Category::cDeathMountain, Category::cDekuScrub, Category::cGrotto },
        SpoilerCollectionCheck::Scrub(0x23, 0x01), SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMC_DEKU_SCRUB_GROTTO_RIGHT] = Location::GrottoScrub(
        RC_DMC_DEKU_SCRUB_GROTTO_RIGHT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_DEATH_MOUNTAIN_CRATER, ACTOR_EN_DNS,
        SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x05, 0xF9), 0x37, "Deku Scrub Grotto Right", "DMC Deku Scrub Grotto Right",
        RC_DMC_DEKU_SCRUB_GROTTO_RIGHT, RG_BUY_BOMBS_535,
        { Category::cDeathMountainCrater, Category::cDeathMountain, Category::cDekuScrub, Category::cGrotto },
        SpoilerCollectionCheck::Scrub(0x23, 0x06), SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMC_DEKU_SCRUB_GROTTO_CENTER] = Location::GrottoScrub(
        RC_DMC_DEKU_SCRUB_GROTTO_CENTER, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_DEATH_MOUNTAIN_CRATER, ACTOR_EN_DNS,
        SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x03, 0xF9), 0x33, "Deku Scrub Grotto Center", "DMC Deku Scrub Grotto Center",
        RC_DMC_DEKU_SCRUB_GROTTO_CENTER, RG_BUY_ARROWS_30,
        { Category::cDeathMountainCrater, Category::cDeathMountain, Category::cDekuScrub, Category::cGrotto },
        SpoilerCollectionCheck::Scrub(0x23, 0x04), SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);

    // Zoras River
    locationTable[RC_ZR_OPEN_GROTTO_CHEST] = Location::Chest(
        RC_ZR_OPEN_GROTTO_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_ZORAS_RIVER, ACTOR_EN_BOX, SCENE_KAKUSIANA,
        22985, 0x09, "Open Grotto Chest", "ZR Open Grotto Chest", RC_ZR_OPEN_GROTTO_CHEST, RG_RED_RUPEE,
        {
            Category::cZorasRiver,
            Category::cGrotto,
        },
        SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_MAGIC_BEAN_SALESMAN] = Location::Base(
        RC_ZR_MAGIC_BEAN_SALESMAN, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_ZORAS_RIVER, ACTOR_ID_MAX, SCENE_SPOT03, 0x00,
        0x16, "Magic Bean Salesman", "ZR Magic Bean Salesman", RC_ZR_MAGIC_BEAN_SALESMAN, RG_MAGIC_BEAN,
        {
            Category::cZorasRiver,
        },
        SpoilerCollectionCheck::MagicBeans(0x54, 0x01), SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_FROGS_ZELDAS_LULLABY] = Location::Base(
        RC_ZR_FROGS_ZELDAS_LULLABY, RCQUEST_BOTH, RCTYPE_FROG_SONG, RCAREA_ZORAS_RIVER, ACTOR_EN_FR, SCENE_SPOT03, 0x00,
        0x3E, "Frogs Zelda's Lullaby", "ZR Frogs Zelda's Lullaby", RC_ZR_FROGS_ZELDAS_LULLABY, RG_PURPLE_RUPEE,
        {
            Category::cZorasRiver,
        },
        SpoilerCollectionCheck::EventChkInf(0xD1), SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_FROGS_EPONAS_SONG] = Location::Base(
        RC_ZR_FROGS_EPONAS_SONG, RCQUEST_BOTH, RCTYPE_FROG_SONG, RCAREA_ZORAS_RIVER, ACTOR_EN_FR, SCENE_SPOT03, 0x00,
        0x3E, "Frogs Epona's Song", "ZR Frogs Epona's Song", RC_ZR_FROGS_EPONAS_SONG, RG_PURPLE_RUPEE,
        {
            Category::cZorasRiver,
        },
        SpoilerCollectionCheck::EventChkInf(0xD2), SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_FROGS_SARIAS_SONG] = Location::Base(
        RC_ZR_FROGS_SARIAS_SONG, RCQUEST_BOTH, RCTYPE_FROG_SONG, RCAREA_ZORAS_RIVER, ACTOR_EN_FR, SCENE_SPOT03, 0x00,
        0x3E, "Frogs Saria's Song", "ZR Frogs Saria's Song", RC_ZR_FROGS_SARIAS_SONG, RG_PURPLE_RUPEE,
        {
            Category::cZorasRiver,
        },
        SpoilerCollectionCheck::EventChkInf(0xD4), SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_FROGS_SUNS_SONG] = Location::Base(
        RC_ZR_FROGS_SUNS_SONG, RCQUEST_BOTH, RCTYPE_FROG_SONG, RCAREA_ZORAS_RIVER, ACTOR_EN_FR, SCENE_SPOT03, 0x00,
        0x3E, "Frogs Sun's Song", "ZR Frogs Sun's Song", RC_ZR_FROGS_SUNS_SONG, RG_PURPLE_RUPEE,
        {
            Category::cZorasRiver,
        },
        SpoilerCollectionCheck::EventChkInf(0xD3), SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_FROGS_SONG_OF_TIME] = Location::Base(
        RC_ZR_FROGS_SONG_OF_TIME, RCQUEST_BOTH, RCTYPE_FROG_SONG, RCAREA_ZORAS_RIVER, ACTOR_EN_FR, SCENE_SPOT03, 0x00,
        0x3E, "Frogs Song of Time", "ZR Frogs Song of Time", RC_ZR_FROGS_SONG_OF_TIME, RG_PURPLE_RUPEE,
        {
            Category::cZorasRiver,
        },
        SpoilerCollectionCheck::EventChkInf(0xD5), SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_FROGS_IN_THE_RAIN] = Location::Base(
        RC_ZR_FROGS_IN_THE_RAIN, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_ZORAS_RIVER, ACTOR_EN_FR, SCENE_SPOT03, 0x00,
        0x3E, "Frogs in the Rain", "ZR Frogs in the Rain", RC_ZR_FROGS_IN_THE_RAIN, RG_PIECE_OF_HEART,
        {
            Category::cZorasRiver,
        },
        SpoilerCollectionCheck::EventChkInf(0xD6), SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_FROGS_OCARINA_GAME] = Location::Base(
        RC_ZR_FROGS_OCARINA_GAME, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_ZORAS_RIVER, ACTOR_EN_FR, SCENE_SPOT03, 0x00,
        0x76, "Frogs Ocarina Game", "ZR Frogs Ocarina Game", RC_ZR_FROGS_OCARINA_GAME, RG_PIECE_OF_HEART,
        {
            Category::cZorasRiver,
            Category::cMinigame,
        },
        SpoilerCollectionCheck::EventChkInf(0xD0), SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH] = Location::Collectable(
        RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_ZORAS_RIVER, ACTOR_EN_ITEM00,
        SCENE_SPOT03, 1030, 0x04, "Near Open Grotto Freestanding PoH", "ZR Near Open Grotto Freestanding PoH",
        RC_ZR_NEAR_OPEN_GROTTO_FREESTANDING_POH, RG_PIECE_OF_HEART,
        {
            Category::cZorasRiver,
        },
        SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_NEAR_DOMAIN_FREESTANDING_POH] =
        Location::Collectable(RC_ZR_NEAR_DOMAIN_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_ZORAS_RIVER,
                              ACTOR_EN_ITEM00, SCENE_SPOT03, 2822, 0x0B, "Near Domain Freestanding PoH",
                              "ZR Near Domain Freestanding PoH", RC_ZR_NEAR_DOMAIN_FREESTANDING_POH, RG_PIECE_OF_HEART,
                              {
                                  Category::cZorasRiver,
                              },
                              SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_DEKU_SCRUB_GROTTO_REAR] = Location::GrottoScrub(
        RC_ZR_DEKU_SCRUB_GROTTO_REAR, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_ZORAS_RIVER, ACTOR_EN_DNS, SCENE_KAKUSIANA,
        TWO_ACTOR_PARAMS(0x07, 0xEB), 0x39, "Deku Scrub Grotto Rear", "ZR Deku Scrub Grotto Rear",
        RC_ZR_DEKU_SCRUB_GROTTO_REAR, RG_BUY_RED_POTION_30,
        { Category::cZorasRiver, Category::cDekuScrub, Category::cGrotto }, SpoilerCollectionCheck::Scrub(0x15, 0x08),
        SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_DEKU_SCRUB_GROTTO_FRONT] = Location::GrottoScrub(
        RC_ZR_DEKU_SCRUB_GROTTO_FRONT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_ZORAS_RIVER, ACTOR_EN_DNS, SCENE_KAKUSIANA,
        TWO_ACTOR_PARAMS(0x08, 0xEB), 0x3A, "Deku Scrub Grotto Front", "ZR Deku Scrub Grotto Front",
        RC_ZR_DEKU_SCRUB_GROTTO_FRONT, RG_BUY_GREEN_POTION,
        { Category::cZorasRiver, Category::cDekuScrub, Category::cGrotto }, SpoilerCollectionCheck::Scrub(0x15, 0x09),
        SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);

    // Zoras Domain
    locationTable[RC_ZD_CHEST] =
        Location::Chest(RC_ZD_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_ZORAS_DOMAIN, ACTOR_EN_BOX, SCENE_SPOT07,
                        -18496, 0x00, "Chest", "ZD Chest", RC_ZD_CHEST, RG_PIECE_OF_HEART,
                        {
                            Category::cZorasDomain,
                        },
                        SpoilerCollectionCheckGroup::GROUP_ZORAS_DOMAIN);
    locationTable[RC_ZD_DIVING_MINIGAME] = Location::Base(
        RC_ZD_DIVING_MINIGAME, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_ZORAS_DOMAIN, ACTOR_ID_MAX, SCENE_SPOT07, 0x00,
        0x37, "Diving Minigame", "ZD Diving Minigame", RC_ZD_DIVING_MINIGAME, RG_PROGRESSIVE_SCALE,
        {
            Category::cZorasDomain,
            Category::cMinigame,
        },
        SpoilerCollectionCheck::EventChkInf(0x38), SpoilerCollectionCheckGroup::GROUP_ZORAS_DOMAIN);
    locationTable[RC_ZD_KING_ZORA_THAWED] = Location::Base(
        RC_ZD_KING_ZORA_THAWED, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_ZORAS_DOMAIN, ACTOR_ID_MAX, SCENE_SPOT07, 0x00,
        0x2D, "King Zora Thawed", "ZD King Zora Thawed", RC_ZD_KING_ZORA_THAWED, RG_ZORA_TUNIC,
        {
            Category::cZorasDomain,
        },
        SpoilerCollectionCheck::InfTable(0x13, 0x01), SpoilerCollectionCheckGroup::GROUP_ZORAS_DOMAIN);
    locationTable[RC_ZD_TRADE_PRESCRIPTION] =
        Location::Base(RC_ZD_TRADE_PRESCRIPTION, RCQUEST_BOTH, RCTYPE_ADULT_TRADE, RCAREA_ZORAS_DOMAIN, ACTOR_ID_MAX,
                       SCENE_SPOT07, 0x00, 0x24, "Trade Prescription", "ZD Trade Prescription",
                       RC_ZD_TRADE_PRESCRIPTION, RG_EYEBALL_FROG, { Category::cZorasDomain, Category::cAdultTrade },
                       SpoilerCollectionCheck::Chest(0x58, 0x1F), SpoilerCollectionCheckGroup::GROUP_ZORAS_DOMAIN);

    // Zora's Fountain
    locationTable[RC_ZF_ICEBERC_FREESTANDING_POH] =
        Location::Collectable(RC_ZF_ICEBERC_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_ZORAS_FOUNTAIN,
                              ACTOR_EN_ITEM00, SCENE_SPOT08, 262, 0x01, "Iceberg Freestanding PoH",
                              "ZF Iceberg Freestanding PoH", RC_ZF_ICEBERC_FREESTANDING_POH, RG_PIECE_OF_HEART,
                              {
                                  Category::cZorasFountain,
                              },
                              SpoilerCollectionCheckGroup::GROUP_ZORAS_DOMAIN);
    locationTable[RC_ZF_BOTTOM_FREESTANDING_POH] =
        Location::Collectable(RC_ZF_BOTTOM_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_ZORAS_FOUNTAIN,
                              ACTOR_EN_ITEM00, SCENE_SPOT08, 5126, 0x14, "Bottom Freestanding PoH",
                              "ZF Bottom Freestanding PoH", RC_ZF_BOTTOM_FREESTANDING_POH, RG_PIECE_OF_HEART,
                              {
                                  Category::cZorasFountain,
                              },
                              SpoilerCollectionCheckGroup::GROUP_ZORAS_DOMAIN);

    // Lon Lon Ranch
    locationTable[RC_LLR_TALONS_CHICKENS] =
        Location::Base(RC_LLR_TALONS_CHICKENS, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_LON_LON_RANCH, ACTOR_ID_MAX,
                       SCENE_SOUKO, 0x00, 0x14, "Talons Chickens", "LLR Talons Chickens", RC_LLR_TALONS_CHICKENS,
                       RG_BOTTLE_WITH_MILK, { Category::cLonLonRanch, Category::cMinigame },
                       SpoilerCollectionCheck::ItemGetInf(10), SpoilerCollectionCheckGroup::GROUP_LON_LON_RANCH);
    locationTable[RC_LLR_FREESTANDING_POH] = Location::Collectable(
        RC_LLR_FREESTANDING_POH, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_LON_LON_RANCH, ACTOR_EN_ITEM00, SCENE_SOUKO, 262,
        0x01, "Freestanding PoH", "LLR Freestanding PoH", RC_LLR_FREESTANDING_POH, RG_PIECE_OF_HEART,
        {
            Category::cLonLonRanch,
        },
        SpoilerCollectionCheckGroup::GROUP_LON_LON_RANCH);
    locationTable[RC_LLR_DEKU_SCRUB_GROTTO_LEFT] = Location::GrottoScrub(
        RC_LLR_DEKU_SCRUB_GROTTO_LEFT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_LON_LON_RANCH, ACTOR_EN_DNS, SCENE_KAKUSIANA,
        TWO_ACTOR_PARAMS(0x00, 0xFC), 0x30, "Deku Scrub Grotto Left", "LLR Deku Scrub Grotto Left",
        RC_LLR_DEKU_SCRUB_GROTTO_LEFT, RG_BUY_DEKU_NUT_5,
        { Category::cLonLonRanch, Category::cDekuScrub, Category::cGrotto }, SpoilerCollectionCheck::Scrub(0x26, 0x01),
        SpoilerCollectionCheckGroup::GROUP_LON_LON_RANCH);
    locationTable[RC_LLR_DEKU_SCRUB_GROTTO_RIGHT] = Location::GrottoScrub(
        RC_LLR_DEKU_SCRUB_GROTTO_RIGHT, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_LON_LON_RANCH, ACTOR_EN_DNS, SCENE_KAKUSIANA,
        TWO_ACTOR_PARAMS(0x05, 0xFC), 0x37, "Deku Scrub Grotto Right", "LLR Deku Scrub Grotto Right",
        RC_LLR_DEKU_SCRUB_GROTTO_RIGHT, RG_BUY_BOMBS_535,
        { Category::cLonLonRanch, Category::cDekuScrub, Category::cGrotto }, SpoilerCollectionCheck::Scrub(0x26, 0x06),
        SpoilerCollectionCheckGroup::GROUP_LON_LON_RANCH);
    locationTable[RC_LLR_DEKU_SCRUB_GROTTO_CENTER] = Location::GrottoScrub(
        RC_LLR_DEKU_SCRUB_GROTTO_CENTER, RCQUEST_BOTH, RCTYPE_SCRUB, RCAREA_LON_LON_RANCH, ACTOR_EN_DNS,
        SCENE_KAKUSIANA, TWO_ACTOR_PARAMS(0x03, 0xFC), 0x33, "Deku Scrub Grotto Center", "LLR Deku Scrub Grotto Center",
        RC_LLR_DEKU_SCRUB_GROTTO_CENTER, RG_BUY_DEKU_SEEDS_30,
        { Category::cLonLonRanch, Category::cDekuScrub, Category::cGrotto }, SpoilerCollectionCheck::Scrub(0x26, 0x04),
        SpoilerCollectionCheckGroup::GROUP_LON_LON_RANCH);

    // Dungeons

    // Deku Tree Vanilla
    locationTable[RC_DEKU_TREE_MAP_CHEST] = Location::Chest(
        RC_DEKU_TREE_MAP_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_DEKU_TREE, ACTOR_EN_BOX, SCENE_YDAN, 2083,
        0x03, "Map Chest", "Deku Tree Map Chest", RC_DEKU_TREE_MAP_CHEST, RG_DEKU_TREE_MAP,
        {
            Category::cDekuTree,
            Category::cVanillaMap,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_COMPASS_CHEST] = Location::Chest(
        RC_DEKU_TREE_COMPASS_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_DEKU_TREE, ACTOR_EN_BOX, SCENE_YDAN,
        2050, 0x02, "Compass Chest", "Deku Tree Compass Chest", RC_DEKU_TREE_COMPASS_CHEST, RG_DEKU_TREE_COMPASS,
        {
            Category::cDekuTree,
            Category::cVanillaCompass,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST] =
        Location::Chest(RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_DEKU_TREE,
                        ACTOR_EN_BOX, SCENE_YDAN, 22790, 0x06, "Compass Room Side Chest",
                        "Deku Tree Compass Room Side Chest", RC_DEKU_TREE_COMPASS_ROOM_SIDE_CHEST, RG_RECOVERY_HEART,
                        {
                            Category::cDekuTree,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_BASEMENT_CHEST] = Location::Chest(
        RC_DEKU_TREE_BASEMENT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_DEKU_TREE, ACTOR_EN_BOX, SCENE_YDAN,
        22788, 0x04, "Basement Chest", "Deku Tree Basement Chest", RC_DEKU_TREE_BASEMENT_CHEST, RG_RECOVERY_HEART,
        {
            Category::cDekuTree,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_SLINGSHOT_CHEST] = Location::Chest(
        RC_DEKU_TREE_SLINGSHOT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_DEKU_TREE, ACTOR_EN_BOX, SCENE_YDAN, 161,
        0x01, "Slingshot Chest", "Deku Tree Slingshot Chest", RC_DEKU_TREE_SLINGSHOT_CHEST, RG_PROGRESSIVE_SLINGSHOT,
        {
            Category::cDekuTree,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST] = Location::Chest(
        RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_DEKU_TREE, ACTOR_EN_BOX,
        SCENE_YDAN, 22789, 0x05, "Slingshot Room Side Chest", "Deku Tree Slingshot Room Side Chest",
        RC_DEKU_TREE_SLINGSHOT_ROOM_SIDE_CHEST, RG_RECOVERY_HEART,
        {
            Category::cDekuTree,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);

    // Deku Tree MQ
    locationTable[RC_DEKU_TREE_MQ_MAP_CHEST] = Location::Chest(
        RC_DEKU_TREE_MQ_MAP_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_DEKU_TREE, ACTOR_EN_BOX, SCENE_YDAN, 2083,
        0x03, "MQ Map Chest", "Deku Tree MQ Map Chest", RC_DEKU_TREE_MQ_MAP_CHEST, RG_DEKU_TREE_MAP,
        {
            Category::cDekuTree,
            Category::cVanillaMap,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_MQ_COMPASS_CHEST] = Location::Chest(
        RC_DEKU_TREE_MQ_COMPASS_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_DEKU_TREE, ACTOR_EN_BOX, SCENE_YDAN, 2049,
        0x01, "MQ Compass Chest", "Deku Tree MQ Compass Chest", RC_DEKU_TREE_MQ_COMPASS_CHEST, RG_DEKU_TREE_COMPASS,
        {
            Category::cDekuTree,
            Category::cVanillaCompass,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_MQ_SLINGSHOT_CHEST] =
        Location::Chest(RC_DEKU_TREE_MQ_SLINGSHOT_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_DEKU_TREE, ACTOR_EN_BOX,
                        SCENE_YDAN, 4262, 0x06, "MQ Slingshot Chest", "Deku Tree MQ Slingshot Chest",
                        RC_DEKU_TREE_MQ_SLINGSHOT_CHEST, RG_PROGRESSIVE_SLINGSHOT,
                        {
                            Category::cDekuTree,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST] = Location::Chest(
        RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_DEKU_TREE, ACTOR_EN_BOX,
        SCENE_YDAN, -31454, 0x02, "MQ Slingshot Room Back Chest", "Deku Tree MQ Slingshot Room Back Chest",
        RC_DEKU_TREE_MQ_SLINGSHOT_ROOM_BACK_CHEST, RG_DEKU_SHIELD,
        {
            Category::cDekuTree,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_MQ_BASEMENT_CHEST] = Location::Chest(
        RC_DEKU_TREE_MQ_BASEMENT_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_DEKU_TREE, ACTOR_EN_BOX, SCENE_YDAN, -31452,
        0x04, "MQ Basement Chest", "Deku Tree MQ Basement Chest", RC_DEKU_TREE_MQ_BASEMENT_CHEST, RG_DEKU_SHIELD,
        {
            Category::cDekuTree,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST] = Location::Chest(
        RC_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_DEKU_TREE, ACTOR_EN_BOX,
        SCENE_YDAN, 22789, 0x05, "MQ Before Spinning Log Chest", "Deku Tree MQ Before Spinning Log Chest",
        RC_DEKU_TREE_MQ_BEFORE_SPINNING_LOG_CHEST, RG_RECOVERY_HEART,
        {
            Category::cDekuTree,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST] = Location::Chest(
        RC_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_DEKU_TREE, ACTOR_EN_BOX,
        SCENE_YDAN, 23200, 0x00, "MQ After Spinning Log Chest", "Deku Tree MQ After Spinning Log Chest",
        RC_DEKU_TREE_MQ_AFTER_SPINNING_LOG_CHEST, RG_PURPLE_RUPEE,
        {
            Category::cDekuTree,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_MQ_DEKU_SCRUB] = Location::Base(
        RC_DEKU_TREE_MQ_DEKU_SCRUB, RCQUEST_MQ, RCTYPE_SCRUB, RCAREA_DEKU_TREE, ACTOR_EN_DNS, SCENE_YDAN, 0x04, 0x34,
        "MQ Deku Scrub", "Deku Tree MQ Deku Scrub", RC_DEKU_TREE_MQ_DEKU_SCRUB, RG_BUY_DEKU_SHIELD,
        {
            Category::cDekuTree,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x00, 0x05), SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);

    // Dodongo's Cavern Shared
    locationTable[RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST] =
        Location::Chest(RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_DODONGOS_CAVERN,
                        ACTOR_EN_BOX, SCENE_DDAN_BOSS, 20512, 0x00, "Boss Room Chest",
                        "Dodongos Cavern Boss Room Chest", RC_DODONGOS_CAVERN_BOSS_ROOM_CHEST, RG_BOMBS_5,
                        {
                            Category::cDodongosCavern,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);

    // Dodongo's Cavern Vanilla
    locationTable[RC_DODONGOS_CAVERN_MAP_CHEST] =
        Location::Chest(RC_DODONGOS_CAVERN_MAP_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_DODONGOS_CAVERN,
                        ACTOR_EN_BOX, SCENE_DDAN, 2088, 0x08, "Map Chest", "Dodongos Cavern Map Chest",
                        RC_DODONGOS_CAVERN_MAP_CHEST, RG_DODONGOS_CAVERN_MAP,
                        {
                            Category::cDodongosCavern,
                            Category::cVanillaMap,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_COMPASS_CHEST] =
        Location::Chest(RC_DODONGOS_CAVERN_COMPASS_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_DODONGOS_CAVERN,
                        ACTOR_EN_BOX, SCENE_DDAN, 2053, 0x05, "Compass Chest", "Dodongos Cavern Compass Chest",
                        RC_DODONGOS_CAVERN_COMPASS_CHEST, RG_DODONGOS_CAVERN_COMPASS,
                        {
                            Category::cDodongosCavern,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST] = Location::Chest(
        RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_DODONGOS_CAVERN,
        ACTOR_EN_BOX, SCENE_DDAN, 22982, 0x06, "Bomb Flower Platform Chest",
        "Dodongos Cavern Bomb Flower Platform Chest", RC_DODONGOS_CAVERN_BOMB_FLOWER_PLATFORM_CHEST, RG_RED_RUPEE,
        {
            Category::cDodongosCavern,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_BOMB_BAG_CHEST] =
        Location::Chest(RC_DODONGOS_CAVERN_BOMB_BAG_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_DODONGOS_CAVERN,
                        ACTOR_EN_BOX, SCENE_DDAN, 1604, 0x04, "Bomb Bag Chest", "Dodongos Cavern Bomb Bag Chest",
                        RC_DODONGOS_CAVERN_BOMB_BAG_CHEST, RG_PROGRESSIVE_BOMB_BAG,
                        {
                            Category::cDodongosCavern,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST] =
        Location::Chest(RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_DODONGOS_CAVERN, ACTOR_EN_BOX, SCENE_DDAN, 21802, 0x0A, "End Of Bridge Chest",
                        "Dodongos Cavern End Of Bridge Chest", RC_DODONGOS_CAVERN_END_OF_BRIDGE_CHEST, RG_DEKU_SHIELD,
                        {
                            Category::cDodongosCavern,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT] = Location::Base(
        RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT, RCQUEST_VANILLA, RCTYPE_SCRUB, RCAREA_DODONGOS_CAVERN,
        ACTOR_EN_DNS, SCENE_DDAN, 0x00, 0x30, "Deku Scrub Near Bomb Bag Left",
        "Dodongos Cavern Deku Scrub Near Bomb Bag Left", RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT,
        RG_BUY_DEKU_NUT_5,
        {
            Category::cDodongosCavern,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x01, 0x01), SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS] = Location::Base(
        RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS, RCQUEST_VANILLA, RCTYPE_SCRUB, RCAREA_DODONGOS_CAVERN,
        ACTOR_EN_DNS, SCENE_DDAN, 0x01, 0x31, "Deku Scrub Side Room Near Dodongos",
        "Dodongos Cavern Deku Scrub Side Room Near Dodongos", RC_DODONGOS_CAVERN_DEKU_SCRUB_SIDE_ROOM_NEAR_DODONGOS,
        RG_BUY_DEKU_STICK_1,
        {
            Category::cDodongosCavern,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x01, 0x02), SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT] = Location::Base(
        RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT, RCQUEST_VANILLA, RCTYPE_SCRUB, RCAREA_DODONGOS_CAVERN,
        ACTOR_EN_DNS, SCENE_DDAN, 0x03, 0x33, "Deku Scrub Near Bomb Bag Right",
        "Dodongos Cavern Deku Scrub Near Bomb Bag Right", RC_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_RIGHT,
        RG_BUY_DEKU_SEEDS_30,
        {
            Category::cDodongosCavern,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x01, 0x04), SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY] = Location::Base(
        RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY, RCQUEST_VANILLA, RCTYPE_SCRUB, RCAREA_DODONGOS_CAVERN, ACTOR_EN_DNS,
        SCENE_DDAN, 0x04, 0x34, "Deku Scrub Lobby", "Dodongos Cavern Deku Scrub Lobby",
        RC_DODONGOS_CAVERN_DEKU_SCRUB_LOBBY, RG_BUY_DEKU_SHIELD,
        {
            Category::cDodongosCavern,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x01, 0x05), SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);

    // Dodongo's Cavern MQ
    locationTable[RC_DODONGOS_CAVERN_MQ_MAP_CHEST] =
        Location::Chest(RC_DODONGOS_CAVERN_MQ_MAP_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_DODONGOS_CAVERN,
                        ACTOR_EN_BOX, SCENE_DDAN, 2080, 0x00, "MQ Map Chest", "Dodongos Cavern MQ Map Chest",
                        RC_DODONGOS_CAVERN_MQ_MAP_CHEST, RG_DODONGOS_CAVERN_MAP,
                        {
                            Category::cDodongosCavern,
                            Category::cVanillaMap,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST] =
        Location::Chest(RC_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_DODONGOS_CAVERN,
                        ACTOR_EN_BOX, SCENE_DDAN, 1604, 0x04, "MQ Bomb Bag Chest", "Dodongos Cavern MQ Bomb Bag Chest",
                        RC_DODONGOS_CAVERN_MQ_BOMB_BAG_CHEST, RG_PROGRESSIVE_BOMB_BAG,
                        {
                            Category::cDodongosCavern,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_MQ_COMPASS_CHEST] =
        Location::Chest(RC_DODONGOS_CAVERN_MQ_COMPASS_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_DODONGOS_CAVERN,
                        ACTOR_EN_BOX, SCENE_DDAN, 6149, 0x05, "MQ Compass Chest", "Dodongos Cavern MQ Compass Chest",
                        RC_DODONGOS_CAVERN_MQ_COMPASS_CHEST, RG_DODONGOS_CAVERN_COMPASS,
                        {
                            Category::cDodongosCavern,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST] =
        Location::Chest(RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_DODONGOS_CAVERN,
                        ACTOR_EN_BOX, SCENE_DDAN, 29986, 0x02, "MQ Larvae Room Chest",
                        "Dodongos Cavern MQ Larvae Room Chest", RC_DODONGOS_CAVERN_MQ_LARVAE_ROOM_CHEST, RG_DEKU_SHIELD,
                        {
                            Category::cDodongosCavern,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST] = Location::Chest(
        RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_DODONGOS_CAVERN,
        ACTOR_EN_BOX, SCENE_DDAN, 22947, 0x03, "MQ Torch Puzzle Room Chest",
        "Dodongos Cavern MQ Torch Puzzle Room Chest", RC_DODONGOS_CAVERN_MQ_TORCH_PUZZLE_ROOM_CHEST, RG_BLUE_RUPEE,
        {
            Category::cDodongosCavern,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST] = Location::Chest(
        RC_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_DODONGOS_CAVERN, ACTOR_EN_BOX,
        SCENE_DDAN, 21825, 0x01, "MQ Under Grave Chest", "Dodongos Cavern MQ Under Grave Chest",
        RC_DODONGOS_CAVERN_MQ_UNDER_GRAVE_CHEST, RG_HYLIAN_SHIELD,
        {
            Category::cDodongosCavern,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR] = Location::Base(
        RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR, RCQUEST_MQ, RCTYPE_SCRUB, RCAREA_DODONGOS_CAVERN, ACTOR_EN_DNS,
        SCENE_DDAN, 0x01, 0x31, "MQ Deku Scrub Lobby Rear", "Dodongos Cavern Deku Scrub Lobby Rear",
        RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_REAR, RG_BUY_DEKU_STICK_1,
        {
            Category::cDodongosCavern,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x01, 0x02), SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT] = Location::Base(
        RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT, RCQUEST_MQ, RCTYPE_SCRUB, RCAREA_DODONGOS_CAVERN, ACTOR_EN_DNS,
        SCENE_DDAN, 0x03, 0x33, "MQ Deku Scrub Lobby Front", "Dodongos Cavern Deku Scrub Lobby Front",
        RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_LOBBY_FRONT, RG_BUY_DEKU_SEEDS_30,
        {
            Category::cDodongosCavern,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x01, 0x04), SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE] = Location::Base(
        RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE, RCQUEST_MQ, RCTYPE_SCRUB, RCAREA_DODONGOS_CAVERN, ACTOR_EN_DNS,
        SCENE_DDAN, 0x04, 0x34, "MQ Deku Scrub Staircase", "Dodongos Cavern Deku Scrub Staircase",
        RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_STAIRCASE, RG_BUY_DEKU_SHIELD,
        {
            Category::cDodongosCavern,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x01, 0x05), SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS] = Location::Base(
        RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS, RCQUEST_MQ, RCTYPE_SCRUB,
        RCAREA_DODONGOS_CAVERN, ACTOR_EN_DNS, SCENE_DDAN, 0x07, 0x39, "MQ Deku Scrub Side Room Near Lower Lizalfos",
        "Dodongos Cavern Deku Scrub Side Room Near Lower Lizalfos",
        RC_DODONGOS_CAVERN_MQ_DEKU_SCRUB_SIDE_ROOM_NEAR_LOWER_LIZALFOS, RG_BUY_RED_POTION_30,
        {
            Category::cDodongosCavern,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x01, 0x08), SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);

    // Jabu-Jabu's Belly Vanilla
    locationTable[RC_JABU_JABUS_BELLY_MAP_CHEST] =
        Location::Chest(RC_JABU_JABUS_BELLY_MAP_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_JABU_JABUS_BELLY,
                        ACTOR_EN_BOX, SCENE_BDAN, 6178, 0x02, "Map Chest", "Jabu Jabus Belly Map Chest",
                        RC_JABU_JABUS_BELLY_MAP_CHEST, RG_JABU_JABUS_BELLY_MAP,
                        {
                            Category::cJabuJabusBelly,
                            Category::cVanillaMap,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_COMPASS_CHEST] =
        Location::Chest(RC_JABU_JABUS_BELLY_COMPASS_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_JABU_JABUS_BELLY,
                        ACTOR_EN_BOX, SCENE_BDAN, -18428, 0x04, "Compass Chest", "Jabu Jabus Belly Compass Chest",
                        RC_JABU_JABUS_BELLY_COMPASS_CHEST, RG_JABU_JABUS_BELLY_COMPASS,
                        {
                            Category::cJabuJabusBelly,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_BOOMERANG_CHEST] =
        Location::Chest(RC_JABU_JABUS_BELLY_BOOMERANG_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_JABU_JABUS_BELLY,
                        ACTOR_EN_BOX, SCENE_BDAN, 4289, 0x01, "Boomerang Chest", "Jabu Jabus Belly Boomerang Chest",
                        RC_JABU_JABUS_BELLY_BOOMERANG_CHEST, RG_BOOMERANG,
                        {
                            Category::cJabuJabusBelly,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_DEKU_SCRUB] = Location::Base(
        RC_JABU_JABUS_BELLY_DEKU_SCRUB, RCQUEST_VANILLA, RCTYPE_SCRUB, RCAREA_JABU_JABUS_BELLY, ACTOR_EN_DNS,
        SCENE_BDAN, 0x00, 0x30, "Deku Scrub", "Jabu Jabus Belly Deku Scrub", RC_JABU_JABUS_BELLY_DEKU_SCRUB,
        RG_BUY_DEKU_NUT_5,
        {
            Category::cJabuJabusBelly,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x02, 0x01), SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);

    // Jabu-Jabu's Belly MQ
    locationTable[RC_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST] = Location::Chest(
        RC_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_JABU_JABUS_BELLY,
        ACTOR_EN_BOX, SCENE_BDAN, -32699, 0x05, "MQ First Room Side Chest", "Jabu Jabus Belly MQ First Room Side Chest",
        RC_JABU_JABUS_BELLY_MQ_FIRST_ROOM_SIDE_CHEST, RG_DEKU_NUTS_5,
        {
            Category::cJabuJabusBelly,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_MQ_MAP_CHEST] =
        Location::Chest(RC_JABU_JABUS_BELLY_MQ_MAP_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_JABU_JABUS_BELLY,
                        ACTOR_EN_BOX, SCENE_BDAN, -18397, 0x03, "MQ Map Chest", "Jabu Jabus Belly MQ Map Chest",
                        RC_JABU_JABUS_BELLY_MQ_MAP_CHEST, RG_JABU_JABUS_BELLY_MAP,
                        {
                            Category::cJabuJabusBelly,
                            Category::cVanillaMap,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST] = Location::Chest(
        RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_JABU_JABUS_BELLY,
        ACTOR_EN_BOX, SCENE_BDAN, 20546, 0x02, "MQ Second Room Lower Chest",
        "Jabu Jabus Belly MQ Second Room Lower Chest", RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_LOWER_CHEST, RG_DEKU_NUTS_5,
        {
            Category::cJabuJabusBelly,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_MQ_COMPASS_CHEST] =
        Location::Chest(RC_JABU_JABUS_BELLY_MQ_COMPASS_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_JABU_JABUS_BELLY,
                        ACTOR_EN_BOX, SCENE_BDAN, -18432, 0x00, "MQ Compass Chest", "Jabu Jabus Belly MQ Compass Chest",
                        RC_JABU_JABUS_BELLY_MQ_COMPASS_CHEST, RG_JABU_JABUS_BELLY_COMPASS,
                        {
                            Category::cJabuJabusBelly,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST] =
        Location::Chest(RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_JABU_JABUS_BELLY, ACTOR_EN_BOX, SCENE_BDAN, -30457, 0x07, "MQ Second Room Upper Chest",
                        "Jabu Jabus Belly MQ Second Room Upper Chest", RC_JABU_JABUS_BELLY_MQ_SECOND_ROOM_UPPER_CHEST,
                        RG_RECOVERY_HEART,
                        {
                            Category::cJabuJabusBelly,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST] =
        Location::Chest(RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_JABU_JABUS_BELLY, ACTOR_EN_BOX, SCENE_BDAN, -32696, 0x08,
                        "MQ Basement Near Switches Chest", "Jabu Jabus Belly MQ Basement Near Switches Chest",
                        RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_SWITCHES_CHEST, RG_DEKU_NUTS_5,
                        {
                            Category::cJabuJabusBelly,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST] =
        Location::Chest(RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_JABU_JABUS_BELLY, ACTOR_EN_BOX, SCENE_BDAN, -32668, 0x04, "MQ Basement Near Vines Chest",
                        "Jabu Jabus Belly MQ Basement Near Vines Chest",
                        RC_JABU_JABUS_BELLY_MQ_BASEMENT_NEAR_VINES_CHEST, RG_BOMBCHU_10,
                        {
                            Category::cJabuJabusBelly,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST] =
        Location::Chest(RC_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_JABU_JABUS_BELLY,
                        ACTOR_EN_BOX, SCENE_BDAN, -31446, 0x0A, "MQ Near Boss Chest",
                        "Jabu Jabus Belly MQ Near Boss Chest", RC_JABU_JABUS_BELLY_MQ_NEAR_BOSS_CHEST, RG_DEKU_SHIELD,
                        {
                            Category::cJabuJabusBelly,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST] =
        Location::Chest(RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_JABU_JABUS_BELLY, ACTOR_EN_BOX, SCENE_BDAN, 28905, 0x09,
                        "MQ Falling Like Like Room Chest", "Jabu Jabus Belly MQ Falling Like Like Room Chest",
                        RC_JABU_JABUS_BELLY_MQ_FALLING_LIKE_LIKE_ROOM_CHEST, RG_DEKU_STICK_1,
                        {
                            Category::cJabuJabusBelly,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST] =
        Location::Chest(RC_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_JABU_JABUS_BELLY, ACTOR_EN_BOX, SCENE_BDAN, 20545, 0x01, "MQ Boomerang Room Small Chest",
                        "Jabu Jabus Belly MQ Boomerang Room Small Chest",
                        RC_JABU_JABUS_BELLY_MQ_BOOMERANG_ROOM_SMALL_CHEST, RG_DEKU_NUTS_5,
                        {
                            Category::cJabuJabusBelly,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST] =
        Location::Chest(RC_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_JABU_JABUS_BELLY,
                        ACTOR_EN_BOX, SCENE_BDAN, 4294, 0x06, "MQ Boomerang Chest",
                        "Jabu Jabus Belly MQ Boomerang Chest", RC_JABU_JABUS_BELLY_MQ_BOOMERANG_CHEST, RG_BOOMERANG,
                        {
                            Category::cJabuJabusBelly,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);

    // Forest Temple Vanilla
    locationTable[RC_FOREST_TEMPLE_FIRST_ROOM_CHEST] =
        Location::Chest(RC_FOREST_TEMPLE_FIRST_ROOM_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_FOREST_TEMPLE,
                        ACTOR_EN_BOX, SCENE_BMORI1, 22595, 0x03, "First Room Chest", "Forest Temple First Room Chest",
                        RC_FOREST_TEMPLE_FIRST_ROOM_CHEST, RG_FOREST_TEMPLE_SMALL_KEY,
                        { Category::cForestTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST] =
        Location::Chest(RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_FOREST_TEMPLE,
                        ACTOR_EN_BOX, SCENE_BMORI1, 30784, 0x00, "First Stalfos Chest",
                        "Forest Temple First Stalfos Chest", RC_FOREST_TEMPLE_FIRST_STALFOS_CHEST,
                        RG_FOREST_TEMPLE_SMALL_KEY, { Category::cForestTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST] =
        Location::Chest(RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_FOREST_TEMPLE, ACTOR_EN_BOX, SCENE_BMORI1, 22789, 0x05, "Raised Island Courtyard Chest",
                        "Forest Temple Raised Island Courtyard Chest", RC_FOREST_TEMPLE_RAISED_ISLAND_COURTYARD_CHEST,
                        RG_RECOVERY_HEART,
                        {
                            Category::cForestTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MAP_CHEST] =
        Location::Chest(RC_FOREST_TEMPLE_MAP_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_FOREST_TEMPLE,
                        ACTOR_EN_BOX, SCENE_BMORI1, 6177, 0x01, "Map Chest", "Forest Temple Map Chest",
                        RC_FOREST_TEMPLE_MAP_CHEST, RG_FOREST_TEMPLE_MAP,
                        {
                            Category::cForestTemple,
                            Category::cVanillaMap,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_WELL_CHEST] = Location::Chest(
        RC_FOREST_TEMPLE_WELL_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_FOREST_TEMPLE, ACTOR_EN_BOX,
        SCENE_BMORI1, 22601, 0x09, "Well Chest", "Forest Temple Well Chest", RC_FOREST_TEMPLE_WELL_CHEST,
        RG_FOREST_TEMPLE_SMALL_KEY, { Category::cForestTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST] = Location::Chest(
        RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_FOREST_TEMPLE,
        ACTOR_EN_BOX, SCENE_BMORI1, 22855, 0x07, "Falling Ceiling Room Chest",
        "Forest Temple Falling Ceiling Room Chest", RC_FOREST_TEMPLE_FALLING_CEILING_ROOM_CHEST, RG_ARROWS_10,
        {
            Category::cForestTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_EYE_SWITCH_CHEST] =
        Location::Chest(RC_FOREST_TEMPLE_EYE_SWITCH_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_FOREST_TEMPLE,
                        ACTOR_EN_BOX, SCENE_BMORI1, -30364, 0x04, "Eye Switch Chest", "Forest Temple Eye Switch Chest",
                        RC_FOREST_TEMPLE_EYE_SWITCH_CHEST, RG_ARROWS_30,
                        {
                            Category::cForestTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_BOSS_KEY_CHEST] = Location::Chest(
        RC_FOREST_TEMPLE_BOSS_KEY_CHEST, RCQUEST_VANILLA, RCTYPE_BOSS_KEY, RCAREA_FOREST_TEMPLE, ACTOR_EN_BOX,
        SCENE_BMORI1, 10222, 0x0E, "Boss Key Chest", "Forest Temple Boss Key Chest", RC_FOREST_TEMPLE_BOSS_KEY_CHEST,
        RG_FOREST_TEMPLE_BOSS_KEY, { Category::cForestTemple, Category::cVanillaBossKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_FLOORMASTER_CHEST] =
        Location::Chest(RC_FOREST_TEMPLE_FLOORMASTER_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_FOREST_TEMPLE,
                        ACTOR_EN_BOX, SCENE_BMORI1, 30786, 0x02, "Floormaster Chest", "Forest Temple Floormaster Chest",
                        RC_FOREST_TEMPLE_FLOORMASTER_CHEST, RG_FOREST_TEMPLE_SMALL_KEY,
                        { Category::cForestTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_BOW_CHEST] = Location::Chest(
        RC_FOREST_TEMPLE_BOW_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_FOREST_TEMPLE, ACTOR_EN_BOX, SCENE_BMORI1,
        -20340, 0x0C, "Bow Chest", "Forest Temple Bow Chest", RC_FOREST_TEMPLE_BOW_CHEST, RG_PROGRESSIVE_BOW,
        {
            Category::cForestTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_RED_POE_CHEST] = Location::Chest(
        RC_FOREST_TEMPLE_RED_POE_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_FOREST_TEMPLE, ACTOR_EN_BOX,
        SCENE_BMORI1, 30797, 0x0D, "Red Poe Chest", "Forest Temple Red Poe Chest", RC_FOREST_TEMPLE_RED_POE_CHEST,
        RG_FOREST_TEMPLE_SMALL_KEY, { Category::cForestTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_BLUE_POE_CHEST] =
        Location::Chest(RC_FOREST_TEMPLE_BLUE_POE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_FOREST_TEMPLE,
                        ACTOR_EN_BOX, SCENE_BMORI1, 6159, 0x0F, "Blue Poe Chest", "Forest Temple Blue Poe Chest",
                        RC_FOREST_TEMPLE_BLUE_POE_CHEST, RG_FOREST_TEMPLE_COMPASS,
                        {
                            Category::cForestTemple,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_BASEMENT_CHEST] =
        Location::Chest(RC_FOREST_TEMPLE_BASEMENT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_FOREST_TEMPLE,
                        ACTOR_EN_BOX, SCENE_BMORI1, 22827, 0x0B, "Basement Chest", "Forest Temple Basement Chest",
                        RC_FOREST_TEMPLE_BASEMENT_CHEST, RG_ARROWS_5,
                        {
                            Category::cForestTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);

    // Forest Temple MQ
    locationTable[RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST] =
        Location::Chest(RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_FOREST_TEMPLE,
                        ACTOR_EN_BOX, SCENE_BMORI1, -30653, 0x03, "MQ First Room Chest",
                        "Forest Temple MQ First Room Chest", RC_FOREST_TEMPLE_MQ_FIRST_ROOM_CHEST,
                        RG_FOREST_TEMPLE_SMALL_KEY, { Category::cForestTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST] = Location::Chest(
        RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_FOREST_TEMPLE, ACTOR_EN_BOX,
        SCENE_BMORI1, 30784, 0x00, "MQ Wolfos Chest", "Forest Temple MQ Wolfos Chest", RC_FOREST_TEMPLE_MQ_WOLFOS_CHEST,
        RG_FOREST_TEMPLE_SMALL_KEY, { Category::cForestTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_BOW_CHEST] = Location::Chest(
        RC_FOREST_TEMPLE_MQ_BOW_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_FOREST_TEMPLE, ACTOR_EN_BOX, SCENE_BMORI1,
        -20340, 0x0C, "MQ Bow Chest", "Forest Temple MQ Bow Chest", RC_FOREST_TEMPLE_MQ_BOW_CHEST, RG_PROGRESSIVE_BOW,
        {
            Category::cForestTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST] = Location::Chest(
        RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_FOREST_TEMPLE,
        ACTOR_EN_BOX, SCENE_BMORI1, 22593, 0x01, "MQ Raised Island Courtyard Lower Chest",
        "Forest Temple MQ Raised Island Courtyard Lower Chest", RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_LOWER_CHEST,
        RG_FOREST_TEMPLE_SMALL_KEY, { Category::cForestTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST] = Location::Chest(
        RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_FOREST_TEMPLE,
        ACTOR_EN_BOX, SCENE_BMORI1, 22597, 0x05, "MQ Raised Island Courtyard Upper Chest",
        "Forest Temple MQ Raised Island Courtyard Upper Chest", RC_FOREST_TEMPLE_MQ_RAISED_ISLAND_COURTYARD_UPPER_CHEST,
        RG_FOREST_TEMPLE_SMALL_KEY, { Category::cForestTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_WELL_CHEST] = Location::Chest(
        RC_FOREST_TEMPLE_MQ_WELL_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_FOREST_TEMPLE, ACTOR_EN_BOX, SCENE_BMORI1,
        22601, 0x09, "MQ Well Chest", "Forest Temple MQ Well Chest", RC_FOREST_TEMPLE_MQ_WELL_CHEST,
        RG_FOREST_TEMPLE_SMALL_KEY, { Category::cForestTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_MAP_CHEST] = Location::Chest(
        RC_FOREST_TEMPLE_MQ_MAP_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_FOREST_TEMPLE, ACTOR_EN_BOX, SCENE_BMORI1,
        6189, 0x0D, "MQ Map Chest", "Forest Temple MQ Map Chest", RC_FOREST_TEMPLE_MQ_MAP_CHEST, RG_FOREST_TEMPLE_MAP,
        {
            Category::cForestTemple,
            Category::cVanillaMap,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_COMPASS_CHEST] =
        Location::Chest(RC_FOREST_TEMPLE_MQ_COMPASS_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_FOREST_TEMPLE,
                        ACTOR_EN_BOX, SCENE_BMORI1, 6159, 0x0F, "MQ Compass Chest", "Forest Temple MQ Compass Chest",
                        RC_FOREST_TEMPLE_MQ_COMPASS_CHEST, RG_FOREST_TEMPLE_COMPASS,
                        {
                            Category::cForestTemple,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST] = Location::Chest(
        RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_FOREST_TEMPLE, ACTOR_EN_BOX,
        SCENE_BMORI1, -30426, 0x06, "MQ Falling Ceiling Room Chest", "Forest Temple MQ Falling Ceiling Room Chest",
        RC_FOREST_TEMPLE_MQ_FALLING_CEILING_ROOM_CHEST, RG_ARROWS_5,
        {
            Category::cForestTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST] =
        Location::Chest(RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_FOREST_TEMPLE,
                        ACTOR_EN_BOX, SCENE_BMORI1, 22827, 0x0B, "MQ Basement Chest", "Forest Temple MQ Basement Chest",
                        RC_FOREST_TEMPLE_MQ_BASEMENT_CHEST, RG_ARROWS_5,
                        {
                            Category::cForestTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_REDEAD_CHEST] = Location::Chest(
        RC_FOREST_TEMPLE_MQ_REDEAD_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_FOREST_TEMPLE, ACTOR_EN_BOX,
        SCENE_BMORI1, 30786, 0x02, "MQ Redead Chest", "Forest Temple MQ Redead Chest", RC_FOREST_TEMPLE_MQ_REDEAD_CHEST,
        RG_FOREST_TEMPLE_SMALL_KEY, { Category::cForestTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_BOSS_KEY_CHEST] =
        Location::Chest(RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST, RCQUEST_MQ, RCTYPE_BOSS_KEY, RCAREA_FOREST_TEMPLE,
                        ACTOR_EN_BOX, SCENE_BMORI1, 10222, 0x0E, "MQ Boss Key Chest", "Forest Temple MQ Boss Key Chest",
                        RC_FOREST_TEMPLE_MQ_BOSS_KEY_CHEST, RG_FOREST_TEMPLE_BOSS_KEY,
                        { Category::cForestTemple, Category::cVanillaBossKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);

    // Fire Temple Vanilla
    locationTable[RC_FIRE_TEMPLE_NEAR_BOSS_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_NEAR_BOSS_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX,
        SCENE_HIDAN, 22593, 0x01, "Near Boss Chest", "Fire Temple Near Boss Chest", RC_FIRE_TEMPLE_NEAR_BOSS_CHEST,
        RG_FIRE_TEMPLE_SMALL_KEY, { Category::cFireTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_FLARE_DANCER_CHEST] =
        Location::Chest(RC_FIRE_TEMPLE_FLARE_DANCER_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_FIRE_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HIDAN, 31936, 0x00, "Flare Dancer Chest", "Fire Temple Flare Dancer Chest",
                        RC_FIRE_TEMPLE_FLARE_DANCER_CHEST, RG_BOMBS_10,
                        {
                            Category::cFireTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_BOSS_KEY_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_BOSS_KEY_CHEST, RCQUEST_VANILLA, RCTYPE_BOSS_KEY, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX, SCENE_HIDAN,
        10220, 0x0C, "Boss Key Chest", "Fire Temple Boss Key Chest", RC_FIRE_TEMPLE_BOSS_KEY_CHEST,
        RG_FIRE_TEMPLE_BOSS_KEY, { Category::cFireTemple, Category::cVanillaBossKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST] =
        Location::Chest(RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
                        RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX, SCENE_HIDAN, 22594, 0x02, "Big Lava Room Blocked Door Chest",
                        "Fire Temple Big Lava Room Blocked Door Chest", RC_FIRE_TEMPLE_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST,
                        RG_FIRE_TEMPLE_SMALL_KEY, { Category::cFireTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_FIRE_TEMPLE,
        ACTOR_EN_BOX, SCENE_HIDAN, 22596, 0x04, "Big Lava Room Lower Open Door Chest",
        "Fire Temple Big Lava Room Lower Open Door Chest", RC_FIRE_TEMPLE_BIG_LAVA_ROOM_LOWER_OPEN_DOOR_CHEST,
        RG_FIRE_TEMPLE_SMALL_KEY, { Category::cFireTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX,
        SCENE_HIDAN, 22595, 0x03, "Boulder Maze Lower Chest", "Fire Temple Boulder Maze Lower Chest",
        RC_FIRE_TEMPLE_BOULDER_MAZE_LOWER_CHEST, RG_FIRE_TEMPLE_SMALL_KEY,
        { Category::cFireTemple, Category::cVanillaSmallKey }, SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX,
        SCENE_HIDAN, 22598, 0x06, "Boulder Maze Upper Chest", "Fire Temple Boulder Maze Upper Chest",
        RC_FIRE_TEMPLE_BOULDER_MAZE_UPPER_CHEST, RG_FIRE_TEMPLE_SMALL_KEY,
        { Category::cFireTemple, Category::cVanillaSmallKey }, SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST] =
        Location::Chest(RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
                        RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX, SCENE_HIDAN, 22600, 0x08, "Boulder Maze Side Room Chest",
                        "Fire Temple Boulder Maze Side Room Chest", RC_FIRE_TEMPLE_BOULDER_MAZE_SIDE_ROOM_CHEST,
                        RG_FIRE_TEMPLE_SMALL_KEY, { Category::cFireTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX,
        SCENE_HIDAN, 22603, 0x0B, "Boulder Maze Shortcut Chest", "Fire Temple Boulder Maze Shortcut Chest",
        RC_FIRE_TEMPLE_BOULDER_MAZE_SHORTCUT_CHEST, RG_FIRE_TEMPLE_SMALL_KEY,
        { Category::cFireTemple, Category::cVanillaSmallKey }, SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_SCARECROW_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_SCARECROW_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX, SCENE_HIDAN,
        23245, 0x0D, "Scarecrow Chest", "Fire Temple Scarecrow Chest", RC_FIRE_TEMPLE_SCARECROW_CHEST, RG_HUGE_RUPEE,
        {
            Category::cFireTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MAP_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_MAP_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX, SCENE_HIDAN,
        2090, 0x0A, "Map Chest", "Fire Temple Map Chest", RC_FIRE_TEMPLE_MAP_CHEST, RG_FIRE_TEMPLE_MAP,
        {
            Category::cFireTemple,
            Category::cVanillaMap,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_COMPASS_CHEST] =
        Location::Chest(RC_FIRE_TEMPLE_COMPASS_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_FIRE_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HIDAN, 2055, 0x07, "Compass Chest", "Fire Temple Compass Chest",
                        RC_FIRE_TEMPLE_COMPASS_CHEST, RG_FIRE_TEMPLE_COMPASS,
                        {
                            Category::cFireTemple,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX,
        SCENE_HIDAN, 22601, 0x09, "Highest Goron Chest", "Fire Temple Highest Goron Chest",
        RC_FIRE_TEMPLE_HIGHEST_GORON_CHEST, RG_FIRE_TEMPLE_SMALL_KEY,
        { Category::cFireTemple, Category::cVanillaSmallKey }, SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST] =
        Location::Chest(RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_FIRE_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HIDAN, 421, 0x05, "Megaton Hammer Chest",
                        "Fire Temple Megaton Hammer Chest", RC_FIRE_TEMPLE_MEGATON_HAMMER_CHEST, RG_MEGATON_HAMMER,
                        {
                            Category::cFireTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);

    // Fire Temple MQ
    locationTable[RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX, SCENE_HIDAN,
        22599, 0x07, "MQ Near Boss Chest", "Fire Temple MQ Near Boss Chest", RC_FIRE_TEMPLE_MQ_NEAR_BOSS_CHEST,
        RG_FIRE_TEMPLE_SMALL_KEY, { Category::cFireTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX,
        SCENE_HIDAN, 4512, 0x00, "MQ Megaton Hammer Chest", "Fire Temple MQ Megaton Hammer Chest",
        RC_FIRE_TEMPLE_MQ_MEGATON_HAMMER_CHEST, RG_MEGATON_HAMMER,
        {
            Category::cFireTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_COMPASS_CHEST] =
        Location::Chest(RC_FIRE_TEMPLE_MQ_COMPASS_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_FIRE_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HIDAN, 2059, 0x0B, "MQ Compass Chest", "Fire Temple MQ Compass Chest",
                        RC_FIRE_TEMPLE_MQ_COMPASS_CHEST, RG_FIRE_TEMPLE_COMPASS,
                        {
                            Category::cFireTemple,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX,
        SCENE_HIDAN, 23747, 0x03, "MQ Lizalfos Maze Lower Chest", "Fire Temple MQ Lizalfos Maze Lower Chest",
        RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_LOWER_CHEST, RG_BOMBS_10,
        {
            Category::cFireTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX,
        SCENE_HIDAN, 23782, 0x06, "MQ Lizalfos Maze Upper Chest", "Fire Temple MQ Lizalfos Maze Upper Chest",
        RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_UPPER_CHEST, RG_BOMBS_10,
        {
            Category::cFireTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_CHEST_ON_FIRE] = Location::Chest(
        RC_FIRE_TEMPLE_MQ_CHEST_ON_FIRE, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX, SCENE_HIDAN,
        22597, 0x05, "MQ Chest on Fire", "Fire Temple MQ Chest on Fire", RC_FIRE_TEMPLE_MQ_CHEST_ON_FIRE,
        RG_FIRE_TEMPLE_SMALL_KEY, { Category::cFireTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST] =
        Location::Chest(RC_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_FIRE_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HIDAN, 30018, 0x02, "MQ Map Room Side Chest",
                        "Fire Temple MQ Map Room Side Chest", RC_FIRE_TEMPLE_MQ_MAP_ROOM_SIDE_CHEST, RG_HYLIAN_SHIELD,
                        {
                            Category::cFireTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_MAP_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_MQ_MAP_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX, SCENE_HIDAN,
        2092, 0x0C, "MQ Map Chest", "Fire Temple MQ Map Chest", RC_FIRE_TEMPLE_MQ_MAP_CHEST, RG_FIRE_TEMPLE_MAP,
        {
            Category::cFireTemple,
            Category::cVanillaMap,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST, RCQUEST_MQ, RCTYPE_BOSS_KEY, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX, SCENE_HIDAN,
        10212, 0x04, "MQ Boss Key Chest", "Fire Temple MQ Boss Key Chest", RC_FIRE_TEMPLE_MQ_BOSS_KEY_CHEST,
        RG_FIRE_TEMPLE_BOSS_KEY, { Category::cFireTemple, Category::cVanillaBossKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_FIRE_TEMPLE,
        ACTOR_EN_BOX, SCENE_HIDAN, 22593, 0x01, "MQ Big Lava Room Blocked Door Chest",
        "Fire Temple MQ Big Lava Room Blocked Door Chest", RC_FIRE_TEMPLE_MQ_BIG_LAVA_ROOM_BLOCKED_DOOR_CHEST,
        RG_FIRE_TEMPLE_SMALL_KEY, { Category::cFireTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST] = Location::Chest(
        RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_FIRE_TEMPLE, ACTOR_EN_BOX,
        SCENE_HIDAN, 22600, 0x08, "MQ Lizalfos Maze Side Room Chest", "Fire Temple MQ Lizalfos Maze Side Room Chest",
        RC_FIRE_TEMPLE_MQ_LIZALFOS_MAZE_SIDE_ROOM_CHEST, RG_FIRE_TEMPLE_SMALL_KEY,
        { Category::cFireTemple, Category::cVanillaSmallKey }, SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_FREESTANDING_KEY] = Location::Collectable(
        RC_FIRE_TEMPLE_MQ_FREESTANDING_KEY, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_FIRE_TEMPLE, ACTOR_EN_ITEM00,
        SCENE_HIDAN, 7185, 0x1C, "MQ Freestanding Key", "Fire Temple MQ Freestanding Key",
        RC_FIRE_TEMPLE_MQ_FREESTANDING_KEY, RG_FIRE_TEMPLE_SMALL_KEY,
        { Category::cFireTemple, Category::cVanillaSmallKey }, SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);

    // Water Temple Vanilla
    locationTable[RC_WATER_TEMPLE_MAP_CHEST] =
        Location::Chest(RC_WATER_TEMPLE_MAP_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_WATER_TEMPLE,
                        ACTOR_EN_BOX, SCENE_MIZUSIN, 6178, 0x02, "Map Chest", "Water Temple Map Chest",
                        RC_WATER_TEMPLE_MAP_CHEST, RG_WATER_TEMPLE_MAP,
                        {
                            Category::cWaterTemple,
                            Category::cVanillaMap,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_COMPASS_CHEST] =
        Location::Chest(RC_WATER_TEMPLE_COMPASS_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_WATER_TEMPLE,
                        ACTOR_EN_BOX, SCENE_MIZUSIN, 2057, 0x09, "Compass Chest", "Water Temple Compass Chest",
                        RC_WATER_TEMPLE_COMPASS_CHEST, RG_WATER_TEMPLE_COMPASS,
                        {
                            Category::cWaterTemple,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_TORCHES_CHEST] = Location::Chest(
        RC_WATER_TEMPLE_TORCHES_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_WATER_TEMPLE, ACTOR_EN_BOX,
        SCENE_MIZUSIN, 30785, 0x01, "Torches Chest", "Water Temple Torches Chest", RC_WATER_TEMPLE_TORCHES_CHEST,
        RG_WATER_TEMPLE_SMALL_KEY, { Category::cWaterTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_DRAGON_CHEST] = Location::Chest(
        RC_WATER_TEMPLE_DRAGON_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_WATER_TEMPLE, ACTOR_EN_BOX,
        SCENE_MIZUSIN, 22602, 0x0A, "Dragon Chest", "Water Temple Dragon Chest", RC_WATER_TEMPLE_DRAGON_CHEST,
        RG_WATER_TEMPLE_SMALL_KEY, { Category::cWaterTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST] =
        Location::Chest(RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
                        RCAREA_WATER_TEMPLE, ACTOR_EN_BOX, SCENE_MIZUSIN, 22600, 0x08, "Central Bow Target Chest",
                        "Water Temple Central Bow Target Chest", RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST,
                        RG_WATER_TEMPLE_SMALL_KEY, { Category::cWaterTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST] =
        Location::Chest(RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_WATER_TEMPLE,
                        ACTOR_EN_BOX, SCENE_MIZUSIN, 22598, 0x06, "Central Pillar Chest",
                        "Water Temple Central Pillar Chest", RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST,
                        RG_WATER_TEMPLE_SMALL_KEY, { Category::cWaterTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_CRACKED_WALL_CHEST] =
        Location::Chest(RC_WATER_TEMPLE_CRACKED_WALL_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_WATER_TEMPLE,
                        ACTOR_EN_BOX, SCENE_MIZUSIN, 22592, 0x00, "Cracked Wall Chest",
                        "Water Temple Cracked Wall Chest", RC_WATER_TEMPLE_CRACKED_WALL_CHEST,
                        RG_WATER_TEMPLE_SMALL_KEY, { Category::cWaterTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_BOSS_KEY_CHEST] = Location::Chest(
        RC_WATER_TEMPLE_BOSS_KEY_CHEST, RCQUEST_VANILLA, RCTYPE_BOSS_KEY, RCAREA_WATER_TEMPLE, ACTOR_EN_BOX,
        SCENE_MIZUSIN, 10213, 0x05, "Boss Key Chest", "Water Temple Boss Key Chest", RC_WATER_TEMPLE_BOSS_KEY_CHEST,
        RG_WATER_TEMPLE_BOSS_KEY, { Category::cWaterTemple, Category::cVanillaBossKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_LONGSHOT_CHEST] =
        Location::Chest(RC_WATER_TEMPLE_LONGSHOT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_WATER_TEMPLE,
                        ACTOR_EN_BOX, SCENE_MIZUSIN, 295, 0x07, "Longshot Chest", "Water Temple Longshot Chest",
                        RC_WATER_TEMPLE_LONGSHOT_CHEST, RG_PROGRESSIVE_HOOKSHOT,
                        {
                            Category::cWaterTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_RIVER_CHEST] = Location::Chest(
        RC_WATER_TEMPLE_RIVER_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_WATER_TEMPLE, ACTOR_EN_BOX,
        SCENE_MIZUSIN, 22595, 0x03, "River Chest", "Water Temple River Chest", RC_WATER_TEMPLE_RIVER_CHEST,
        RG_WATER_TEMPLE_SMALL_KEY, { Category::cWaterTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);

    // Water Temple MQ
    locationTable[RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST] =
        Location::Chest(RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_WATER_TEMPLE,
                        ACTOR_EN_BOX, SCENE_MIZUSIN, -30650, 0x06, "MQ Central Pillar Chest",
                        "Water Temple MQ Central Pillar Chest", RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST,
                        RG_WATER_TEMPLE_SMALL_KEY, { Category::cWaterTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST] = Location::Chest(
        RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST, RCQUEST_MQ, RCTYPE_BOSS_KEY, RCAREA_WATER_TEMPLE, ACTOR_EN_BOX,
        SCENE_MIZUSIN, 10213, 0x05, "MQ Boss Key Chest", "Water Temple MQ Boss Key Chest",
        RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST, RG_WATER_TEMPLE_BOSS_KEY,
        { Category::cWaterTemple, Category::cVanillaBossKey }, SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST] =
        Location::Chest(RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_WATER_TEMPLE,
                        ACTOR_EN_BOX, SCENE_MIZUSIN, -20192, 0x00, "MQ Longshot Chest",
                        "Water Temple MQ Longshot Chest", RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST, RG_PROGRESSIVE_HOOKSHOT,
                        {
                            Category::cWaterTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_MQ_COMPASS_CHEST] =
        Location::Chest(RC_WATER_TEMPLE_MQ_COMPASS_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_WATER_TEMPLE,
                        ACTOR_EN_BOX, SCENE_MIZUSIN, 6145, 0x01, "MQ Compass Chest", "Water Temple MQ Compass Chest",
                        RC_WATER_TEMPLE_MQ_COMPASS_CHEST, RG_WATER_TEMPLE_COMPASS,
                        {
                            Category::cWaterTemple,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_MQ_MAP_CHEST] = Location::Chest(
        RC_WATER_TEMPLE_MQ_MAP_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_WATER_TEMPLE, ACTOR_EN_BOX, SCENE_MIZUSIN,
        -18398, 0x02, "MQ Map Chest", "Water Temple MQ Map Chest", RC_WATER_TEMPLE_MQ_MAP_CHEST, RG_WATER_TEMPLE_MAP,
        {
            Category::cWaterTemple,
            Category::cVanillaMap,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_MQ_FREESTANDING_KEY] =
        Location::Collectable(RC_WATER_TEMPLE_MQ_FREESTANDING_KEY, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_WATER_TEMPLE,
                              ACTOR_EN_ITEM00, SCENE_MIZUSIN, 273, 0x01, "MQ Freestanding Key",
                              "Water Temple MQ Freestanding Key", RC_WATER_TEMPLE_MQ_FREESTANDING_KEY,
                              RG_WATER_TEMPLE_SMALL_KEY, { Category::cWaterTemple, Category::cVanillaSmallKey },
                              SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);

    // Spirit Temple Shared
    locationTable[RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_SPOT11, 1707, 0x0B, "Silver Gauntlets Chest",
                        "Spirit Temple Silver Gauntlets Chest", RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST,
                        RG_PROGRESSIVE_STRENGTH, { Category::cSpiritTemple, Category::cDesertColossus },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST, RCQUEST_BOTH, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX,
        SCENE_SPOT11, 13673, 0x09, "Mirror Shield Chest", "Spirit Temple Mirror Shield Chest",
        SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST, RG_MIRROR_SHIELD, { Category::cSpiritTemple, Category::cDesertColossus },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);

    // Spirit Temple Vanilla
    locationTable[RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, 21800, 0x08, "Child Bridge Chest",
                        "Spirit Temple Child Bridge Chest", RC_SPIRIT_TEMPLE_CHILD_BRIDGE_CHEST, RG_DEKU_SHIELD,
                        {
                            Category::cSpiritTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
                        RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX, SCENE_JYASINZOU, -30656, 0x00, "Child Early Torches Chest",
                        "Spirit Temple Child Early Torches Chest", RC_SPIRIT_TEMPLE_CHILD_EARLY_TORCHES_CHEST,
                        RG_SPIRIT_TEMPLE_SMALL_KEY, { Category::cSpiritTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_COMPASS_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_COMPASS_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, 14340, 0x04, "Compass Chest", "Spirit Temple Compass Chest",
                        RC_SPIRIT_TEMPLE_COMPASS_CHEST, RG_SPIRIT_TEMPLE_COMPASS,
                        {
                            Category::cSpiritTemple,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
                        RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX, SCENE_JYASINZOU, 22599, 0x07, "Early Adult Right Chest",
                        "Spirit Temple Early Adult Right Chest", RC_SPIRIT_TEMPLE_EARLY_ADULT_RIGHT_CHEST,
                        RG_SPIRIT_TEMPLE_SMALL_KEY, { Category::cSpiritTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX, SCENE_JYASINZOU, -30451, 0x0D, "First Mirror Left Chest",
                        "Spirit Temple First Mirror Left Chest", RC_SPIRIT_TEMPLE_FIRST_MIRROR_LEFT_CHEST, RG_ICE_TRAP,
                        {
                            Category::cSpiritTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX,
        SCENE_JYASINZOU, -28786, 0x0E, "First Mirror Right Chest", "Spirit Temple First Mirror Right Chest",
        RC_SPIRIT_TEMPLE_FIRST_MIRROR_RIGHT_CHEST, RG_RECOVERY_HEART,
        {
            Category::cSpiritTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MAP_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_MAP_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, -18397, 0x03, "Map Chest", "Spirit Temple Map Chest",
                        RC_SPIRIT_TEMPLE_MAP_CHEST, RG_SPIRIT_TEMPLE_MAP,
                        {
                            Category::cSpiritTemple,
                            Category::cVanillaMap,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX,
        SCENE_JYASINZOU, -32666, 0x06, "Child Climb North Chest", "Spirit Temple Child Climb North Chest",
        RC_SPIRIT_TEMPLE_CHILD_CLIMB_NORTH_CHEST, RG_BOMBCHU_10,
        {
            Category::cSpiritTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, -31444, 0x0C, "Child Climb East Chest",
                        "Spirit Temple Child Climb East Chest", RC_SPIRIT_TEMPLE_CHILD_CLIMB_EAST_CHEST, RG_DEKU_SHIELD,
                        {
                            Category::cSpiritTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, -30655, 0x01, "Sun Block Room Chest",
                        "Spirit Temple Sun Block Room Chest", RC_SPIRIT_TEMPLE_SUN_BLOCK_ROOM_CHEST,
                        RG_SPIRIT_TEMPLE_SMALL_KEY, { Category::cSpiritTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
                        RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX, SCENE_JYASINZOU, -30654, 0x02, "Statue Room Hand Chest",
                        "Spirit Temple Statue Room Hand Chest", RC_SPIRIT_TEMPLE_STATUE_ROOM_HAND_CHEST,
                        RG_SPIRIT_TEMPLE_SMALL_KEY, { Category::cSpiritTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE,
        ACTOR_EN_BOX, SCENE_JYASINZOU, -30577, 0x0F, "Statue Room Northeast Chest",
        "Spirit Temple Statue Room Northeast Chest", RC_SPIRIT_TEMPLE_STATUE_ROOM_NORTHEAST_CHEST, RG_BLUE_RUPEE,
        {
            Category::cSpiritTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, 22597, 0x05, "Near Four Armos Chest",
                        "Spirit Temple Near Four Armos Chest", RC_SPIRIT_TEMPLE_NEAR_FOUR_ARMOS_CHEST,
                        RG_SPIRIT_TEMPLE_SMALL_KEY, { Category::cSpiritTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX, SCENE_JYASINZOU, 26900, 0x14,
                        "Hallway Right Invisible Chest", "Spirit Temple Hallway Right Invisible Chest",
                        RC_SPIRIT_TEMPLE_HALLWAY_RIGHT_INVISIBLE_CHEST, RG_RECOVERY_HEART,
                        {
                            Category::cSpiritTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE,
        ACTOR_EN_BOX, SCENE_JYASINZOU, 26901, 0x15, "Hallway Left Invisible Chest",
        "Spirit Temple Hallway Left Invisible Chest", RC_SPIRIT_TEMPLE_HALLWAY_LEFT_INVISIBLE_CHEST, RG_RECOVERY_HEART,
        {
            Category::cSpiritTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST, RCQUEST_VANILLA, RCTYPE_BOSS_KEY, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX,
        SCENE_JYASINZOU, 10218, 0x0A, "Boss Key Chest", "Spirit Temple Boss Key Chest", RC_SPIRIT_TEMPLE_BOSS_KEY_CHEST,
        RG_SPIRIT_TEMPLE_BOSS_KEY, { Category::cSpiritTemple, Category::cVanillaBossKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_TOPMOST_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_TOPMOST_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, -29454, 0x12, "Topmost Chest", "Spirit Temple Topmost Chest",
                        RC_SPIRIT_TEMPLE_TOPMOST_CHEST, RG_BOMBS_20,
                        {
                            Category::cSpiritTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);

    // Spirit Temple MQ
    locationTable[RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX,
        SCENE_JYASINZOU, 20602, 0x1A, "MQ Entrance Front Left Chest", "Spirit Temple MQ Entrance Front Left Chest",
        RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_LEFT_CHEST, RG_BOMBCHU_10,
        {
            Category::cSpiritTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX,
        SCENE_JYASINZOU, -32641, 0x1F, "MQ Entrance Back Right Chest", "Spirit Temple MQ Entrance Back Right Chest",
        RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_RIGHT_CHEST, RG_BOMBCHU_10,
        {
            Category::cSpiritTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_SPIRIT_TEMPLE,
        ACTOR_EN_BOX, SCENE_JYASINZOU, -30629, 0x1B, "MQ Entrance Front Right Chest",
        "Spirit Temple MQ Entrance Front Right Chest", RC_SPIRIT_TEMPLE_MQ_ENTRANCE_FRONT_RIGHT_CHEST,
        RG_SPIRIT_TEMPLE_SMALL_KEY, { Category::cSpiritTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX,
        SCENE_JYASINZOU, -30626, 0x1E, "MQ Entrance Back Left Chest", "Spirit Temple MQ Entrance Back Left Chest",
        RC_SPIRIT_TEMPLE_MQ_ENTRANCE_BACK_LEFT_CHEST, RG_SPIRIT_TEMPLE_SMALL_KEY,
        { Category::cSpiritTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX,
        SCENE_JYASINZOU, -30627, 0x1D, "MQ Child Hammer Switch Chest", "Spirit Temple MQ Child Hammer Switch Chest",
        RC_SPIRIT_TEMPLE_MQ_CHILD_HAMMER_SWITCH_CHEST, RG_SPIRIT_TEMPLE_SMALL_KEY,
        { Category::cSpiritTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_MAP_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_MQ_MAP_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, 2080, 0x00, "MQ Map Chest", "Spirit Temple MQ Map Chest",
                        RC_SPIRIT_TEMPLE_MQ_MAP_CHEST, RG_SPIRIT_TEMPLE_MAP,
                        {
                            Category::cSpiritTemple,
                            Category::cVanillaMap,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, 30792, GI_KEY_SMALL, "MQ Map Room Enemy Chest",
                        "Spirit Temple MQ Map Room Enemy Chest", RC_SPIRIT_TEMPLE_MQ_MAP_ROOM_ENEMY_CHEST,
                        RG_SPIRIT_TEMPLE_SMALL_KEY, { Category::cSpiritTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX,
        SCENE_JYASINZOU, 28774, 0x06, "MQ Child Climb North Chest", "Spirit Temple MQ Child Climb North Chest",
        RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_NORTH_CHEST, RG_BOMBCHU_10,
        {
            Category::cSpiritTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, -30644, 0x0C, "MQ Child Climb South Chest",
                        "Spirit Temple MQ Child Climb South Chest", RC_SPIRIT_TEMPLE_MQ_CHILD_CLIMB_SOUTH_CHEST,
                        RG_SPIRIT_TEMPLE_SMALL_KEY, { Category::cSpiritTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_COMPASS_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_MQ_COMPASS_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, -18429, 0x03, "MQ Compass Chest",
                        "Spirit Temple MQ Compass Chest", RC_SPIRIT_TEMPLE_MQ_COMPASS_CHEST, RG_SPIRIT_TEMPLE_COMPASS,
                        {
                            Category::cSpiritTemple,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX,
        SCENE_JYASINZOU, -30577, 0x0F, "MQ Statue Room Lullaby Chest", "Spirit Temple MQ Statue Room Lullaby Chest",
        RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_LULLABY_CHEST, RG_BLUE_RUPEE,
        {
            Category::cSpiritTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX, SCENE_JYASINZOU, 26882, 0x02,
                        "MQ Statue Room Invisible Chest", "Spirit Temple MQ Statue Room Invisible Chest",
                        RC_SPIRIT_TEMPLE_MQ_STATUE_ROOM_INVISIBLE_CHEST, RG_RECOVERY_HEART,
                        {
                            Category::cSpiritTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_SPIRIT_TEMPLE,
        ACTOR_EN_BOX, SCENE_JYASINZOU, -30628, 0x1C, "MQ Silver Block Hallway Chest",
        "Spirit Temple MQ Silver Block Hallway Chest", RC_SPIRIT_TEMPLE_MQ_SILVER_BLOCK_HALLWAY_CHEST,
        RG_SPIRIT_TEMPLE_SMALL_KEY, { Category::cSpiritTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX,
        SCENE_JYASINZOU, -30463, 0x01, "MQ Sun Block Room Chest", "Spirit Temple MQ Sun Block Room Chest",
        RC_SPIRIT_TEMPLE_MQ_SUN_BLOCK_ROOM_CHEST, RG_RECOVERY_HEART,
        {
            Category::cSpiritTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_BOX,
        SCENE_JYASINZOU, 23207, 0x07, "MQ Symphony Room Chest", "Spirit Temple MQ Symphony Room Chest",
        RC_SPIRIT_TEMPLE_MQ_SYMPHONY_ROOM_CHEST, RG_PURPLE_RUPEE,
        {
            Category::cSpiritTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, 31396, 0x04, "MQ Leever Room Chest",
                        "Spirit Temple MQ Leever Room Chest", RC_SPIRIT_TEMPLE_MQ_LEEVER_ROOM_CHEST, RG_PURPLE_RUPEE,
                        {
                            Category::cSpiritTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, 24472, 0x19, "MQ Beamos Room Chest",
                        "Spirit Temple MQ Beamos Room Chest", RC_SPIRIT_TEMPLE_MQ_BEAMOS_ROOM_CHEST, RG_RECOVERY_HEART,
                        {
                            Category::cSpiritTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, 31097, 0x18, "MQ Chest Switch Chest",
                        "Spirit Temple MQ Chest Switch Chest", RC_SPIRIT_TEMPLE_MQ_CHEST_SWITCH_CHEST, RG_ICE_TRAP,
                        {
                            Category::cSpiritTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST] =
        Location::Chest(RC_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST, RCQUEST_MQ, RCTYPE_BOSS_KEY, RCAREA_SPIRIT_TEMPLE,
                        ACTOR_EN_BOX, SCENE_JYASINZOU, 10213, 0x05, "MQ Boss Key Chest",
                        "Spirit Temple MQ Boss Key Chest", RC_SPIRIT_TEMPLE_MQ_BOSS_KEY_CHEST,
                        RG_SPIRIT_TEMPLE_BOSS_KEY, { Category::cSpiritTemple, Category::cVanillaBossKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST] = Location::Chest(
        RC_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_SPIRIT_TEMPLE,
        ACTOR_EN_BOX, SCENE_JYASINZOU, 26706, 0x12, "MQ Mirror Puzzle Invisible Chest",
        "Spirit Temple MQ Mirror Puzzle Invisible Chest", RC_SPIRIT_TEMPLE_MQ_MIRROR_PUZZLE_INVISIBLE_CHEST,
        RG_SPIRIT_TEMPLE_SMALL_KEY, { Category::cSpiritTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);

    // Shadow Temple Vanilla
    locationTable[RC_SHADOW_TEMPLE_MAP_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_MAP_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 6177, 0x01, "Map Chest", "Shadow Temple Map Chest",
                        RC_SHADOW_TEMPLE_MAP_CHEST, RG_SHADOW_TEMPLE_MAP,
                        {
                            Category::cShadowTemple,
                            Category::cVanillaMap,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 5607, 0x07, "Hover Boots Chest", "Shadow Temple Hover Boots Chest",
                        RC_SHADOW_TEMPLE_HOVER_BOOTS_CHEST, RG_HOVER_BOOTS,
                        {
                            Category::cShadowTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_COMPASS_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_COMPASS_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 6147, 0x03, "Compass Chest", "Shadow Temple Compass Chest",
                        RC_SHADOW_TEMPLE_COMPASS_CHEST, RG_SHADOW_TEMPLE_COMPASS,
                        {
                            Category::cShadowTemple,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
                        RCAREA_SHADOW_TEMPLE, ACTOR_EN_BOX, SCENE_HAKADAN, 22594, 0x02, "Early Silver Rupee Chest",
                        "Shadow Temple Early Silver Rupee Chest", RC_SHADOW_TEMPLE_EARLY_SILVER_RUPEE_CHEST,
                        RG_SHADOW_TEMPLE_SMALL_KEY, { Category::cShadowTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST] = Location::Chest(
        RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE,
        ACTOR_EN_BOX, SCENE_HAKADAN, 22668, 0x0C, "Invisible Blades Visible Chest",
        "Shadow Temple Invisible Blades Visible Chest", RC_SHADOW_TEMPLE_INVISIBLE_BLADES_VISIBLE_CHEST, RG_BLUE_RUPEE,
        {
            Category::cShadowTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST] = Location::Chest(
        RC_SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE,
        ACTOR_EN_BOX, SCENE_HAKADAN, 26998, 0x16, "Invisible Blades Invisible Chest",
        "Shadow Temple Invisible Blades Invisible Chest", SHADOW_TEMPLE_INVISIBLE_BLADES_INVISIBLE_CHEST, RG_ARROWS_30,
        {
            Category::cShadowTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST] = Location::Chest(
        RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE,
        ACTOR_EN_BOX, SCENE_HAKADAN, 22853, 0x05, "Falling Spikes Lower Chest",
        "Shadow Temple Falling Spikes Lower Chest", RC_SHADOW_TEMPLE_FALLING_SPIKES_LOWER_CHEST, RG_ARROWS_10,
        {
            Category::cShadowTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST] = Location::Chest(
        RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE,
        ACTOR_EN_BOX, SCENE_HAKADAN, 22662, 0x06, "Falling Spikes Upper Chest",
        "Shadow Temple Falling Spikes Upper Chest", RC_SHADOW_TEMPLE_FALLING_SPIKES_UPPER_CHEST, RG_BLUE_RUPEE,
        {
            Category::cShadowTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
                        RCAREA_SHADOW_TEMPLE, ACTOR_EN_BOX, SCENE_HAKADAN, -30652, 0x04, "Falling Spikes Switch Chest",
                        "Shadow Temple Falling Spikes Switch Chest", RC_SHADOW_TEMPLE_FALLING_SPIKES_SWITCH_CHEST,
                        RG_SHADOW_TEMPLE_SMALL_KEY, { Category::cShadowTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 30857, 0x09, "Invisible Spikes Chest",
                        "Shadow Temple Invisible Spikes Chest", RC_SHADOW_TEMPLE_INVISIBLE_SPIKES_CHEST, RG_BLUE_RUPEE,
                        {
                            Category::cShadowTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_WIND_HINT_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_WIND_HINT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 26965, 0x15, "Wind Hint Chest", "Shadow Temple Wind Hint Chest",
                        RC_SHADOW_TEMPLE_WIND_HINT_CHEST, RG_ARROWS_10,
                        {
                            Category::cShadowTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 30856, 0x08, "After Wind Enemy Chest",
                        "Shadow Temple After Wind Enemy Chest", RC_SHADOW_TEMPLE_AFTER_WIND_ENEMY_CHEST, RG_BLUE_RUPEE,
                        {
                            Category::cShadowTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
                        RCAREA_SHADOW_TEMPLE, ACTOR_EN_BOX, SCENE_HAKADAN, 26708, 0x14, "After Wind Hidden Chest",
                        "Shadow Temple After Wind Hidden Chest", RC_SHADOW_TEMPLE_AFTER_WIND_HIDDEN_CHEST,
                        RG_SHADOW_TEMPLE_SMALL_KEY, { Category::cShadowTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 22666, 0x0A, "Spike Walls Left Chest",
                        "Shadow Temple Spike Walls Left Chest", RC_SHADOW_TEMPLE_SPIKE_WALLS_LEFT_CHEST, RG_BLUE_RUPEE,
                        {
                            Category::cShadowTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_BOSS_KEY_CHEST] = Location::Chest(
        RC_SHADOW_TEMPLE_BOSS_KEY_CHEST, RCQUEST_VANILLA, RCTYPE_BOSS_KEY, RCAREA_SHADOW_TEMPLE, ACTOR_EN_BOX,
        SCENE_HAKADAN, 10219, 0x0B, "Boss Key Chest", "Shadow Temple Boss Key Chest", RC_SHADOW_TEMPLE_BOSS_KEY_CHEST,
        RG_SHADOW_TEMPLE_BOSS_KEY, { Category::cShadowTemple, Category::cVanillaBossKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
                        RCAREA_SHADOW_TEMPLE, ACTOR_EN_BOX, SCENE_HAKADAN, 30797, 0x0D, "Invisible Floormaster Chest",
                        "Shadow Temple Invisible Floormaster Chest", RC_SHADOW_TEMPLE_INVISIBLE_FLOORMASTER_CHEST,
                        RG_SHADOW_TEMPLE_SMALL_KEY, { Category::cShadowTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_FREESTANDING_KEY] =
        Location::Collectable(RC_SHADOW_TEMPLE_FREESTANDING_KEY, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
                              RCAREA_SHADOW_TEMPLE, ACTOR_EN_ITEM00, SCENE_HAKADAN, 273, 0x01, "Freestanding Key",
                              "Shadow Temple Freestanding Key", RC_SHADOW_TEMPLE_FREESTANDING_KEY,
                              RG_SHADOW_TEMPLE_SMALL_KEY, { Category::cShadowTemple, Category::cVanillaSmallKey },
                              SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);

    // Shadow Temple MQ
    locationTable[RC_SHADOW_TEMPLE_MQ_COMPASS_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_MQ_COMPASS_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 6145, 0x01, "MQ Compass Chest", "Shadow Temple MQ Compass Chest",
                        RC_SHADOW_TEMPLE_MQ_COMPASS_CHEST, RG_SHADOW_TEMPLE_COMPASS,
                        {
                            Category::cShadowTemple,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 5607, 0x07, "MQ Hover Boots Chest",
                        "Shadow Temple MQ Hover Boots Chest", RC_SHADOW_TEMPLE_MQ_HOVER_BOOTS_CHEST, RG_HOVER_BOOTS,
                        {
                            Category::cShadowTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 30787, 0x03, "MQ Early Gibdos Chest",
                        "Shadow Temple MQ Early Gibdos Chest", RC_SHADOW_TEMPLE_MQ_EARLY_GIBDOS_CHEST,
                        RG_SHADOW_TEMPLE_SMALL_KEY, { Category::cShadowTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_MAP_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_MQ_MAP_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 2082, 0x02, "MQ Map Chest", "Shadow Temple MQ Map Chest",
                        RC_SHADOW_TEMPLE_MQ_MAP_CHEST, RG_SHADOW_TEMPLE_MAP,
                        {
                            Category::cShadowTemple,
                            Category::cVanillaMap,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST] = Location::Chest(
        RC_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE, ACTOR_EN_BOX,
        SCENE_HAKADAN, -30417, 0x0F, "MQ Beamos Silver Rupees Chest", "Shadow Temple MQ Beamos Silver Rupees Chest",
        RC_SHADOW_TEMPLE_MQ_BEAMOS_SILVER_RUPEES_CHEST, RG_ARROWS_5,
        {
            Category::cShadowTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST] = Location::Chest(
        RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE,
        ACTOR_EN_BOX, SCENE_HAKADAN, 22662, 0x04, "MQ Falling Spikes Switch Chest",
        "Shadow Temple MQ Falling Spikes Switch Chest", RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_SWITCH_CHEST,
        RG_SHADOW_TEMPLE_SMALL_KEY, { Category::cShadowTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST] = Location::Chest(
        RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE, ACTOR_EN_BOX,
        SCENE_HAKADAN, 22853, 0x05, "MQ Falling Spikes Lower Chest", "Shadow Temple MQ Falling Spikes Lower Chest",
        RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_LOWER_CHEST, RG_ARROWS_10,
        {
            Category::cShadowTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST] = Location::Chest(
        RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_SHADOW_TEMPLE,
        ACTOR_EN_BOX, SCENE_HAKADAN, -30652, 0x06, "MQ Falling Spikes Upper Chest",
        "Shadow Temple MQ Falling Spikes Upper Chest", RC_SHADOW_TEMPLE_MQ_FALLING_SPIKES_UPPER_CHEST, RG_ARROWS_5,
        {
            Category::cShadowTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST] = Location::Chest(
        RC_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE, ACTOR_EN_BOX,
        SCENE_HAKADAN, 30857, 0x09, "MQ Invisible Spikes Chest", "Shadow Temple MQ Invisible Spikes Chest",
        RC_SHADOW_TEMPLE_MQ_INVISIBLE_SPIKES_CHEST, RG_BLUE_RUPEE,
        {
            Category::cShadowTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST, RCQUEST_MQ, RCTYPE_BOSS_KEY, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 10219, 0x0B, "MQ Boss Key Chest",
                        "Shadow Temple MQ Boss Key Chest", RC_SHADOW_TEMPLE_MQ_BOSS_KEY_CHEST,
                        RG_SHADOW_TEMPLE_BOSS_KEY, { Category::cShadowTemple, Category::cVanillaBossKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST] = Location::Chest(
        RC_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_SHADOW_TEMPLE, ACTOR_EN_BOX,
        SCENE_HAKADAN, 22666, 0x0A, "MQ Spike Walls Left Chest", "Shadow Temple MQ Spike Walls Left Chest",
        RC_SHADOW_TEMPLE_MQ_SPIKE_WALLS_LEFT_CHEST, RG_BLUE_RUPEE,
        {
            Category::cShadowTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 31184, 0x10, "MQ Stalfos Room Chest",
                        "Shadow Temple MQ Stalfos Room Chest", RC_SHADOW_TEMPLE_MQ_STALFOS_ROOM_CHEST, RG_RED_RUPEE,
                        {
                            Category::cShadowTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST] = Location::Chest(
        RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_SHADOW_TEMPLE,
        ACTOR_EN_BOX, SCENE_HAKADAN, 26710, 0x16, "MQ Invisible Blades Invisible Chest",
        "Shadow Temple MQ Invisible Blades Invisible Chest", RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_INVISIBLE_CHEST,
        RG_SHADOW_TEMPLE_SMALL_KEY, { Category::cShadowTemple, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_SHADOW_TEMPLE, ACTOR_EN_BOX, SCENE_HAKADAN, 22668, 0x0C,
                        "MQ Invisible Blades Visible Chest", "Shadow Temple MQ Invisible Blades Visible Chest",
                        RC_SHADOW_TEMPLE_MQ_INVISIBLE_BLADES_VISIBLE_CHEST, RG_BLUE_RUPEE,
                        {
                            Category::cShadowTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 31053, 0x0D, "MQ Bomb Flower Chest",
                        "Shadow Temple MQ Bomb Flower Chest", RC_SHADOW_TEMPLE_MQ_BOMB_FLOWER_CHEST, RG_ARROWS_10,
                        {
                            Category::cShadowTemple,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_SHADOW_TEMPLE,
                        ACTOR_EN_BOX, SCENE_HAKADAN, 26709, 0x15, "MQ Wind Hint Chest",
                        "Shadow Temple MQ Wind Hint Chest", RC_SHADOW_TEMPLE_MQ_WIND_HINT_CHEST,
                        RG_SHADOW_TEMPLE_SMALL_KEY, { Category::cShadowTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST] = Location::Chest(
        RC_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE, ACTOR_EN_BOX,
        SCENE_HAKADAN, 26932, 0x14, "MQ After Wind Hidden Chest", "Shadow Temple MQ After Wind Hidden Chest",
        RC_SHADOW_TEMPLE_MQ_AFTER_WIND_HIDDEN_CHEST, RG_ARROWS_5,
        {
            Category::cShadowTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST] = Location::Chest(
        RC_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_SHADOW_TEMPLE, ACTOR_EN_BOX,
        SCENE_HAKADAN, 30856, 0x08, "MQ After Wind Enemy Chest", "Shadow Temple MQ After Wind Enemy Chest",
        RC_SHADOW_TEMPLE_MQ_AFTER_WIND_ENEMY_CHEST, RG_BLUE_RUPEE,
        {
            Category::cShadowTemple,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST] =
        Location::Chest(RC_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY,
                        RCAREA_SHADOW_TEMPLE, ACTOR_EN_BOX, SCENE_HAKADAN, 26702, 0x0E, "MQ Near Ship Invisible Chest",
                        "Shadow Temple MQ Near Ship Invisible Chest", RC_SHADOW_TEMPLE_MQ_NEAR_SHIP_INVISIBLE_CHEST,
                        RG_SHADOW_TEMPLE_SMALL_KEY, { Category::cShadowTemple, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_FREESTANDING_KEY] =
        Location::Collectable(RC_SHADOW_TEMPLE_MQ_FREESTANDING_KEY, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_SHADOW_TEMPLE,
                              ACTOR_EN_ITEM00, SCENE_HAKADAN, 1553, 0x06, "MQ Freestanding Key",
                              "Shadow Temple MQ Freestanding Key", RC_SHADOW_TEMPLE_MQ_FREESTANDING_KEY,
                              RG_SHADOW_TEMPLE_SMALL_KEY, { Category::cShadowTemple, Category::cVanillaSmallKey },
                              SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);

    // Bottom of the Well Vanilla
    locationTable[RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST] = Location::Chest(
        RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_BOTTOM_OF_THE_WELL,
        ACTOR_EN_BOX, SCENE_HAKADANCH, 22600, 0x08, "Front Left Fake Wall Chest",
        "Bottom of the Well Front Left Fake Wall Chest", RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST,
        RG_BOTTOM_OF_THE_WELL_SMALL_KEY, { Category::cBottomOfTheWell, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST] =
        Location::Chest(RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_BOX, SCENE_HAKADANCH, 20578, 0x02,
                        "Front Center Bombable Chest", "Bottom of the Well Front Center Bombable Chest",
                        RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST, RG_BOMBCHU_10,
                        {
                            Category::cBottomOfTheWell,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST] = Location::Chest(
        RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
        RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_BOX, SCENE_HAKADANCH, 22597, 0x05, "Right Bottom Fake Wall Chest",
        "Bottom of the Well Right Bottom Fake Wall Chest", RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST,
        RG_BOTTOM_OF_THE_WELL_SMALL_KEY, { Category::cBottomOfTheWell, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST] = Location::Chest(
        RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_BOTTOM_OF_THE_WELL,
        ACTOR_EN_BOX, SCENE_HAKADANCH, 2049, 0x01, "Compass Chest", "Bottom of the Well Compass Chest",
        RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST, RG_BOTTOM_OF_THE_WELL_COMPASS,
        {
            Category::cBottomOfTheWell,
            Category::cVanillaCompass,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST] = Location::Chest(
        RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_BOTTOM_OF_THE_WELL,
        ACTOR_EN_BOX, SCENE_HAKADANCH, 20558, 0x0E, "Center Skulltula Chest",
        "Bottom of the Well Center Skulltula Chest", RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST, RG_DEKU_NUTS_5,
        {
            Category::cBottomOfTheWell,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST] = Location::Chest(
        RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_BOTTOM_OF_THE_WELL,
        ACTOR_EN_BOX, SCENE_HAKADANCH, 23684, 0x04, "Back Left Bombable Chest",
        "Bottom of the Well Back Left Bombable Chest", RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST, RG_DEKU_NUTS_10,
        {
            Category::cBottomOfTheWell,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST] =
        Location::Chest(RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST, RCQUEST_VANILLA, RCTYPE_BOSS_HEART_OR_OTHER_REWARD,
                        RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_BOX, SCENE_HAKADANCH, 4419, 0x03, "Lens of Truth Chest",
                        "Bottom of the Well Lens of Truth Chest", RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST,
                        RG_LENS_OF_TRUTH, { Category::cBottomOfTheWell, Category::cSongDungeonReward },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST] =
        Location::Chest(RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_BOX, SCENE_HAKADANCH, 27348, 0x14, "Invisible Chest",
                        "Bottom of the Well Invisible Chest", RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST, RG_HUGE_RUPEE,
                        {
                            Category::cBottomOfTheWell,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST] = Location::Chest(
        RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_BOTTOM_OF_THE_WELL,
        ACTOR_EN_BOX, SCENE_HAKADANCH, 23760, 0x10, "Underwater Front Chest",
        "Bottom of the Well Underwater Front Chest", RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST, RG_BOMBS_10,
        {
            Category::cBottomOfTheWell,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST] = Location::Chest(
        RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_BOTTOM_OF_THE_WELL,
        ACTOR_EN_BOX, SCENE_HAKADANCH, 22793, 0x09, "Underwater Left Chest", "Bottom of the Well Underwater Left Chest",
        RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST, RG_RECOVERY_HEART,
        {
            Category::cBottomOfTheWell,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_MAP_CHEST] =
        Location::Chest(RC_BOTTOM_OF_THE_WELL_MAP_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_BOTTOM_OF_THE_WELL,
                        ACTOR_EN_BOX, SCENE_HAKADANCH, 2087, 0x07, "Map Chest", "Bottom of the Well Map Chest",
                        RC_BOTTOM_OF_THE_WELL_MAP_CHEST, RG_BOTTOM_OF_THE_WELL_MAP,
                        {
                            Category::cBottomOfTheWell,
                            Category::cVanillaMap,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST] =
        Location::Chest(RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_BOX, SCENE_HAKADANCH, 21802, 0x0A, "Fire Keese Chest",
                        "Bottom of the Well Fire Keese Chest", RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST, RG_DEKU_SHIELD,
                        {
                            Category::cBottomOfTheWell,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST] =
        Location::Chest(RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_BOX, SCENE_HAKADANCH, 21836, 0x0C, "Like Like Chest",
                        "Bottom of the Well Like Like Chest", RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST, RG_HYLIAN_SHIELD,
                        {
                            Category::cBottomOfTheWell,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY] =
        Location::Chest(RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
                        RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_ITEM00, SCENE_HAKADANCH, 273, 0x01, "Freestanding Key",
                        "Bottom of the Well Freestanding Key", RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY,
                        RG_BOTTOM_OF_THE_WELL_SMALL_KEY, { Category::cBottomOfTheWell, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);

    // Bottom of the Well Master Quest
    locationTable[RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST] =
        Location::Chest(RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_BOTTOM_OF_THE_WELL,
                        ACTOR_EN_BOX, SCENE_HAKADANCH, 2083, 0x03, "MQ Map Chest", "Bottom of the Well MQ Map Chest",
                        RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST, RG_BOTTOM_OF_THE_WELL_MAP,
                        {
                            Category::cBottomOfTheWell,
                            Category::cVanillaMap,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST] = Location::Chest(
        RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST, RCQUEST_MQ, RCTYPE_BOSS_HEART_OR_OTHER_REWARD,
        RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_BOX, SCENE_HAKADANCH, -20159, 0x01, "MQ Lens of Truth Chest",
        "Bottom of the Well MQ Lens of Truth Chest", RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST, RG_LENS_OF_TRUTH,
        { Category::cBottomOfTheWell, Category::cSongDungeonReward },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST] = Location::Chest(
        RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_BOX,
        SCENE_HAKADANCH, 6146, 0x02, "MQ Compass Chest", "Bottom of the Well MQ Compass Chest",
        RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST, RG_BOTTOM_OF_THE_WELL_COMPASS,
        {
            Category::cBottomOfTheWell,
            Category::cVanillaCompass,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY] = Location::Collectable(
        RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_BOTTOM_OF_THE_WELL,
        ACTOR_EN_ITEM00, SCENE_HAKADANCH, 529, 0x02, "MQ Dead Hand Freestanding Key",
        "Bottom of the Well MQ Dead Hand Freestanding Key", RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY,
        RG_BOTTOM_OF_THE_WELL_SMALL_KEY, { Category::cBottomOfTheWell, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY] = Location::Collectable(
        RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY, RCQUEST_MQ, RCTYPE_SMALL_KEY,
        RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_ITEM00, SCENE_HAKADANCH, 273, 0x01, "MQ East Inner Room Freestanding Key",
        "Bottom of the Well MQ East Inner Room Freestanding Key",
        RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY, RG_BOTTOM_OF_THE_WELL_SMALL_KEY,
        { Category::cBottomOfTheWell, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);

    // Ice Cavern Vanilla
    locationTable[RC_ICE_CAVERN_MAP_CHEST] = Location::Chest(
        RC_ICE_CAVERN_MAP_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_ICE_CAVERN, ACTOR_EN_BOX,
        SCENE_ICE_DOUKUTO, 2080, 0x00, "Map Chest", "Ice Cavern Map Chest", RC_ICE_CAVERN_MAP_CHEST, RG_ICE_CAVERN_MAP,
        {
            Category::cIceCavern,
            Category::cVanillaMap,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_ICE_CAVERN);
    locationTable[RC_ICE_CAVERN_COMPASS_CHEST] =
        Location::Chest(RC_ICE_CAVERN_COMPASS_CHEST, RCQUEST_VANILLA, RCTYPE_MAP_COMPASS, RCAREA_ICE_CAVERN,
                        ACTOR_EN_BOX, SCENE_ICE_DOUKUTO, 2049, 0x01, "Compass Chest", "Ice Cavern Compass Chest",
                        RC_ICE_CAVERN_COMPASS_CHEST, RG_ICE_CAVERN_COMPASS,
                        {
                            Category::cIceCavern,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_ICE_CAVERN);
    locationTable[RC_ICE_CAVERN_IRON_BOOTS_CHEST] =
        Location::Chest(RC_ICE_CAVERN_IRON_BOOTS_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_ICE_CAVERN,
                        ACTOR_EN_BOX, SCENE_ICE_DOUKUTO, 5570, 0x02, "Iron Boots Chest", "Ice Cavern Iron Boots Chest",
                        RC_ICE_CAVERN_IRON_BOOTS_CHEST, RG_IRON_BOOTS,
                        {
                            Category::cIceCavern,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_ICE_CAVERN);
    locationTable[RC_ICE_CAVERN_FREESTANDING_POH] =
        Location::Collectable(RC_ICE_CAVERN_FREESTANDING_POH, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_ICE_CAVERN,
                              ACTOR_EN_ITEM00, SCENE_ICE_DOUKUTO, 262, 0x01, "Freestanding PoH",
                              "Ice Cavern Freestanding PoH", RC_ICE_CAVERN_FREESTANDING_POH, RG_PIECE_OF_HEART,
                              {
                                  Category::cIceCavern,
                              },
                              SpoilerCollectionCheckGroup::GROUP_DUNGEON_ICE_CAVERN);

    // Ice Cavern Master Quest
    locationTable[RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST] =
        Location::Chest(RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_ICE_CAVERN, ACTOR_EN_BOX,
                        SCENE_ICE_DOUKUTO, 5570, 0x02, "MQ Iron Boots Chest", "Ice Cavern MQ Iron Boots Chest",
                        RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST, RG_IRON_BOOTS,
                        {
                            Category::cIceCavern,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_ICE_CAVERN);
    locationTable[RC_ICE_CAVERN_MQ_COMPASS_CHEST] =
        Location::Chest(RC_ICE_CAVERN_MQ_COMPASS_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_ICE_CAVERN, ACTOR_EN_BOX,
                        SCENE_ICE_DOUKUTO, 2048, 0x00, "MQ Compass Chest", "Ice Cavern MQ Compass Chest",
                        RC_ICE_CAVERN_MQ_COMPASS_CHEST, RG_ICE_CAVERN_COMPASS,
                        {
                            Category::cIceCavern,
                            Category::cVanillaCompass,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_ICE_CAVERN);
    locationTable[RC_ICE_CAVERN_MQ_MAP_CHEST] = Location::Chest(
        RC_ICE_CAVERN_MQ_MAP_CHEST, RCQUEST_MQ, RCTYPE_MAP_COMPASS, RCAREA_ICE_CAVERN, ACTOR_EN_BOX, SCENE_ICE_DOUKUTO,
        -18399, 0x01, "MQ Map Chest", "Ice Cavern MQ Map Chest", RC_ICE_CAVERN_MQ_MAP_CHEST, RG_ICE_CAVERN_MAP,
        {
            Category::cIceCavern,
            Category::cVanillaMap,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_ICE_CAVERN);
    locationTable[RC_ICE_CAVERN_MQ_FREESTANDING_POH] =
        Location::Collectable(RC_ICE_CAVERN_MQ_FREESTANDING_POH, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_ICE_CAVERN,
                              ACTOR_EN_ITEM00, SCENE_ICE_DOUKUTO, 262, 0x01, "MQ Freestanding PoH",
                              "Ice Cavern MQ Freestanding PoH", RC_ICE_CAVERN_MQ_FREESTANDING_POH, RG_PIECE_OF_HEART,
                              {
                                  Category::cIceCavern,
                              },
                              SpoilerCollectionCheckGroup::GROUP_DUNGEON_ICE_CAVERN);

    // Gerudo Training Grounds Vanilla
    locationTable[RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_GERUDO_TRAINING_GROUND,
        ACTOR_EN_BOX, SCENE_MEN, -30573, 0x13, "Lobby Left Chest", "Gerudo Training Grounds Lobby Left Chest",
        RC_GERUDO_TRAINING_GROUND_LOBBY_LEFT_CHEST, RG_BLUE_RUPEE,
        {
            Category::cGerudoTrainingGrounds,
        },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_GERUDO_TRAINING_GROUND,
        ACTOR_EN_BOX, SCENE_MEN, -30393, 0x07, "Lobby Right Chest", "Gerudo Training Grounds Lobby Right Chest",
        RC_GERUDO_TRAINING_GROUND_LOBBY_RIGHT_CHEST, RG_ARROWS_10,
        {
            Category::cGerudoTrainingGrounds,
        },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_GERUDO_TRAINING_GROUND,
        ACTOR_EN_BOX, SCENE_MEN, -30656, 0x00, "Stalfos Chest", "Gerudo Training Grounds Stalfos Chest",
        RC_GERUDO_TRAINING_GROUND_STALFOS_CHEST, RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY,
        { Category::cGerudoTrainingGrounds, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_GERUDO_TRAINING_GROUND,
        ACTOR_EN_BOX, SCENE_MEN, -30655, 0x01, "Beamos Chest", "Gerudo Training Grounds Beamos Chest",
        RC_GERUDO_TRAINING_GROUND_BEAMOS_CHEST, RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY,
        { Category::cGerudoTrainingGrounds, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 22603, 0x0B, "Hidden Ceiling Chest",
        "Gerudo Training Grounds Hidden Ceiling Chest", RC_GERUDO_TRAINING_GROUND_HIDDEN_CEILING_CHEST,
        RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, { Category::cGerudoTrainingGrounds, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 23206, 0x06, "Maze Path First Chest",
                        "Gerudo Training Grounds Maze Path First Chest",
                        RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FIRST_CHEST, RG_PURPLE_RUPEE,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 22986, 0x0A, "Maze Path Second Chest",
                        "Gerudo Training Grounds Maze Path Second Chest",
                        RC_GERUDO_TRAINING_GROUND_MAZE_PATH_SECOND_CHEST, RG_RED_RUPEE,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 22889, 0x09, "Maze Path Third Chest",
        "Gerudo Training Grounds Maze Path Third Chest", RC_GERUDO_TRAINING_GROUND_MAZE_PATH_THIRD_CHEST, RG_ARROWS_30,
        {
            Category::cGerudoTrainingGrounds,
        },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST, RCQUEST_VANILLA, RCTYPE_BOSS_HEART_OR_OTHER_REWARD,
        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 2860, 0x0C, "Maze Path Final Chest",
        "Gerudo Training Grounds Maze Path Final Chest", RC_GERUDO_TRAINING_GROUND_MAZE_PATH_FINAL_CHEST, RG_ICE_ARROWS,
        { Category::cGerudoTrainingGrounds, Category::cSongDungeonReward },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 23877, 0x05, "Maze Right Central Chest",
                        "Gerudo Training Grounds Maze Right Central Chest",
                        RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_CENTRAL_CHEST, RG_BOMBCHU_5,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 22888, 0x08, "Maze Right Side Chest",
        "Gerudo Training Grounds Maze Right Side Chest", RC_GERUDO_TRAINING_GROUND_MAZE_RIGHT_SIDE_CHEST, RG_ARROWS_30,
        {
            Category::cGerudoTrainingGrounds,
        },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, -30643, 0x0D,
                        "Underwater Silver Rupee Chest", "Gerudo Training Grounds Underwater Silver Rupee Chest",
                        RC_GERUDO_TRAINING_GROUND_UNDERWATER_SILVER_RUPEE_CHEST, RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY,
                        { Category::cGerudoTrainingGrounds, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 31058, 0x12, "Hammer Room Clear Chest",
                        "Gerudo Training Grounds Hammer Room Clear Chest",
                        RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_CLEAR_CHEST, RG_ARROWS_10,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 22608, 0x10, "Hammer Room Switch Chest",
        "Gerudo Training Grounds Hammer Room Switch Chest", RC_GERUDO_TRAINING_GROUND_HAMMER_ROOM_SWITCH_CHEST,
        RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, { Category::cGerudoTrainingGrounds, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_GERUDO_TRAINING_GROUND,
        ACTOR_EN_BOX, SCENE_MEN, -30653, 0x03, "Eye Statue Chest", "Gerudo Training Grounds Eye Statue Chest",
        RC_GERUDO_TRAINING_GROUND_EYE_STATUE_CHEST, RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY,
        { Category::cGerudoTrainingGrounds, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 22596, 0x04, "Near Scarecrow Chest",
        "Gerudo Training Grounds Near Scarecrow Chest", RC_GERUDO_TRAINING_GROUND_NEAR_SCARECROW_CHEST,
        RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, { Category::cGerudoTrainingGrounds, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 31089, 0x11, "Before Heavy Block Chest",
                        "Gerudo Training Grounds Before Heavy Block Chest",
                        RC_GERUDO_TRAINING_GROUND_BEFORE_HEAVY_BLOCK_CHEST, RG_ARROWS_30,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 31439, 0x0F, "Heavy Block First Chest",
                        "Gerudo Training Grounds Heavy Block First Chest",
                        RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FIRST_CHEST, RG_HUGE_RUPEE,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 30862, 0x0E, "Heavy Block Second Chest",
                        "Gerudo Training Grounds Heavy Block Second Chest",
                        RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_SECOND_CHEST, RG_BLUE_RUPEE,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 26708, 0x14, "Heavy Block Third Chest",
        "Gerudo Training Grounds Heavy Block Third Chest", RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_THIRD_CHEST,
        RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, { Category::cGerudoTrainingGrounds, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 24450, 0x02, "Heavy Block Fourth Chest",
                        "Gerudo Training Grounds Heavy Block Fourth Chest",
                        RC_GERUDO_TRAINING_GROUND_HEAVY_BLOCK_FOURTH_CHEST, RG_ICE_TRAP,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY] = Location::Collectable(
        RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_GERUDO_TRAINING_GROUND,
        ACTOR_EN_ITEM00, SCENE_MEN, 273, 0x01, "Freestanding Key", "Gerudo Training Grounds Freestanding Key",
        RC_GERUDO_TRAINING_GROUND_FREESTANDING_KEY, RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY,
        { Category::cGerudoTrainingGrounds, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);

    // Gerudo Training Grounds Master Quest
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_GERUDO_TRAINING_GROUND,
        ACTOR_EN_BOX, SCENE_MEN, 23879, 0x07, "MQ Lobby Right Chest", "Gerudo Training Grounds MQ Lobby Right Chest",
        RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_RIGHT_CHEST, RG_BOMBCHU_5,
        {
            Category::cGerudoTrainingGrounds,
        },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_GERUDO_TRAINING_GROUND,
        ACTOR_EN_BOX, SCENE_MEN, 22867, 0x13, "MQ Lobby Left Chest", "Gerudo Training Grounds MQ Lobby Left Chest",
        RC_GERUDO_TRAINING_GROUND_MQ_LOBBY_LEFT_CHEST, RG_ARROWS_10,
        {
            Category::cGerudoTrainingGrounds,
        },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, -30304, 0x00,
                        "MQ First Iron Knuckle Chest", "Gerudo Training Grounds MQ First Iron Knuckle Chest",
                        RC_GERUDO_TRAINING_GROUND_MQ_FIRST_IRON_KNUCKLE_CHEST, RG_BLUE_RUPEE,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 31057, 0x11,
                        "MQ Before Heavy Block Chest", "Gerudo Training Grounds MQ Before Heavy Block Chest",
                        RC_GERUDO_TRAINING_GROUND_MQ_BEFORE_HEAVY_BLOCK_CHEST, RG_ARROWS_10,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_GERUDO_TRAINING_GROUND,
        ACTOR_EN_BOX, SCENE_MEN, -32669, 0x03, "MQ Eye Statue Chest", "Gerudo Training Grounds MQ Eye Statue Chest",
        RC_GERUDO_TRAINING_GROUND_MQ_EYE_STATUE_CHEST, RG_BOMBCHU_10,
        {
            Category::cGerudoTrainingGrounds,
        },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_GERUDO_TRAINING_GROUND,
        ACTOR_EN_BOX, SCENE_MEN, -30642, 0x0E, "MQ Flame Circle Chest", "Gerudo Training Grounds MQ Flame Circle Chest",
        RC_GERUDO_TRAINING_GROUND_MQ_FLAME_CIRCLE_CHEST, RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY,
        { Category::cGerudoTrainingGrounds, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 31058, 0x12,
                        "MQ Second Iron Knuckle Chest", "Gerudo Training Grounds MQ Second Iron Knuckle Chest",
                        RC_GERUDO_TRAINING_GROUND_MQ_SECOND_IRON_KNUCKLE_CHEST, RG_ARROWS_10,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_GERUDO_TRAINING_GROUND,
        ACTOR_EN_BOX, SCENE_MEN, -30655, 0x01, "MQ Dinolfos Chest", "Gerudo Training Grounds MQ Dinolfos Chest",
        RC_GERUDO_TRAINING_GROUND_MQ_DINOLFOS_CHEST, RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY,
        { Category::cGerudoTrainingGrounds, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST, RCQUEST_MQ, RCTYPE_BOSS_HEART_OR_OTHER_REWARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, -17628, 0x04, "MQ Ice Arrows Chest",
                        "Gerudo Training Grounds MQ Ice Arrows Chest", RC_GERUDO_TRAINING_GROUND_MQ_ICE_ARROWS_CHEST,
                        RG_ICE_ARROWS, { Category::cGerudoTrainingGrounds, Category::cSongDungeonReward },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 22661, 0x05,
                        "MQ Maze Right Central Chest", "Gerudo Training Grounds MQ Maze Right Central Chest",
                        RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_CENTRAL_CHEST, RG_BLUE_RUPEE,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 22918, 0x06, "MQ Maze Path First Chest",
                        "Gerudo Training Grounds MQ Maze Path First Chest",
                        RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_FIRST_CHEST, RG_GREEN_RUPEE,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 24136, 0x08, "MQ Maze Right Side Chest",
                        "Gerudo Training Grounds MQ Maze Right Side Chest",
                        RC_GERUDO_TRAINING_GROUND_MQ_MAZE_RIGHT_SIDE_CHEST, RG_TREASURE_GAME_GREEN_RUPEE,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 24137, 0x09, "MQ Maze Path Third Chest",
                        "Gerudo Training Grounds MQ Maze Path Third Chest",
                        RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_THIRD_CHEST, RG_TREASURE_GAME_GREEN_RUPEE,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 22986, 0x0A,
                        "MQ Maze Path Second Chest", "Gerudo Training Grounds MQ Maze Path Second Chest",
                        RC_GERUDO_TRAINING_GROUND_MQ_MAZE_PATH_SECOND_CHEST, RG_RED_RUPEE,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, 23211, 0x0B, "MQ Hidden Ceiling Chest",
                        "Gerudo Training Grounds MQ Hidden Ceiling Chest",
                        RC_GERUDO_TRAINING_GROUND_MQ_HIDDEN_CEILING_CHEST, RG_PURPLE_RUPEE,
                        {
                            Category::cGerudoTrainingGrounds,
                        },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST] =
        Location::Chest(RC_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY,
                        RCAREA_GERUDO_TRAINING_GROUND, ACTOR_EN_BOX, SCENE_MEN, -30643, 0x0D,
                        "MQ Underwater Silver Rupee Chest", "Gerudo Training Grounds MQ Underwater Silver Rupee Chest",
                        RC_GERUDO_TRAINING_GROUND_MQ_UNDERWATER_SILVER_RUPEE_CHEST, RG_TREASURE_GAME_GREEN_RUPEE,
                        { Category::cGerudoTrainingGrounds, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);
    locationTable[RC_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST] = Location::Chest(
        RC_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_GERUDO_TRAINING_GROUND,
        ACTOR_EN_BOX, SCENE_MEN, 31394, 0x02, "MQ Heavy Block Chest", "Gerudo Training Grounds MQ Heavy Block Chest",
        RC_GERUDO_TRAINING_GROUND_MQ_HEAVY_BLOCK_CHEST, RG_PURPLE_RUPEE,
        {
            Category::cGerudoTrainingGrounds,
        },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_TRAINING_GROUND);

    // Ganon's Castle Shared
    locationTable[RC_GANONS_TOWER_BOSS_KEY_CHEST] =
        Location::Chest(RC_GANONS_TOWER_BOSS_KEY_CHEST, RCQUEST_BOTH, RCTYPE_GANON_BOSS_KEY, RCAREA_GANONS_CASTLE,
                        ACTOR_EN_BOX, SCENE_GANON, 10219, 0x0B, "Boss Key Chest", "Ganon's Tower Boss Key Chest",
                        RC_GANONS_TOWER_BOSS_KEY_CHEST, RG_GANONS_CASTLE_BOSS_KEY,
                        {
                            Category::cGanonsCastle,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);

    // Ganon's Castle Vanilla
    locationTable[RC_GANONS_CASTLE_FOREST_TRIAL_CHEST] =
        Location::Chest(RC_GANONS_CASTLE_FOREST_TRIAL_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE,
                        ACTOR_EN_BOX, SCENE_GANONTIKA, 30857, 0x09, "Forest Trial Chest",
                        "Ganon's Castle Forest Trial Chest", RC_GANONS_CASTLE_FOREST_TRIAL_CHEST, RG_BLUE_RUPEE,
                        {
                            Category::cGanonsCastle,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST] =
        Location::Chest(RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE,
                        ACTOR_EN_BOX, SCENE_GANONTIKA, 24455, 0x07, "Water Trial Left Chest",
                        "Ganon's Castle Water Trial Left Chest", RC_GANONS_CASTLE_WATER_TRIAL_LEFT_CHEST, RG_ICE_TRAP,
                        {
                            Category::cGanonsCastle,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE, ACTOR_EN_BOX,
        SCENE_GANONTIKA, 22790, 0x06, "Water Trial Right Chest", "Ganon's Castle Water Trial Right Chest",
        RC_GANONS_CASTLE_WATER_TRIAL_RIGHT_CHEST, RG_RECOVERY_HEART,
        {
            Category::cGanonsCastle,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE, ACTOR_EN_BOX,
        SCENE_GANONTIKA, 22664, 0x08, "Shadow Trial Front Chest", "Ganon's Castle Shadow Trial Front Chest",
        RC_GANONS_CASTLE_SHADOW_TRIAL_FRONT_CHEST, RG_BLUE_RUPEE,
        {
            Category::cGanonsCastle,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST] =
        Location::Chest(RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_GANONS_CASTLE, ACTOR_EN_BOX, SCENE_GANONTIKA, 14021, 0x05,
                        "Shadow Trial Golden Gauntlets Chest", "Ganon's Castle Shadow Trial Golden Gauntlets Chest",
                        RC_GANONS_CASTLE_SHADOW_TRIAL_GOLDEN_GAUNTLETS_CHEST, RG_PROGRESSIVE_STRENGTH,
                        {
                            Category::cGanonsCastle,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST] =
        Location::Chest(RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD,
                        RCAREA_GANONS_CASTLE, ACTOR_EN_BOX, SCENE_GANONTIKA, -29326, GI_BOMBCHUS_20,
                        "Spirit Trial Crystal Switch Chest", "Ganon's Castle Spirit Trial Crystal Switch Chest",
                        RC_GANONS_CASTLE_SPIRIT_TRIAL_CRYSTAL_SWITCH_CHEST, RG_BOMBCHU_20,
                        {
                            Category::cGanonsCastle,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE,
        ACTOR_EN_BOX, SCENE_GANONTIKA, 26964, 0x14, "Spirit Trial Invisible Chest",
        "Ganon's Castle Spirit Trial Invisible Chest", RC_GANONS_CASTLE_SPIRIT_TRIAL_INVISIBLE_CHEST, RG_ARROWS_10,
        {
            Category::cGanonsCastle,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE,
        ACTOR_EN_BOX, SCENE_GANONTIKA, 22668, 0x0C, "Light Trial First Left Chest",
        "Ganon's Castle Light Trial First Left Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_LEFT_CHEST, RG_BLUE_RUPEE,
        {
            Category::cGanonsCastle,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE,
        ACTOR_EN_BOX, SCENE_GANONTIKA, 24459, 0x0B, "Light Trial Second Left Chest",
        "Ganon's Castle Light Trial Second Left Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_LEFT_CHEST, RG_ICE_TRAP,
        {
            Category::cGanonsCastle,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE,
        ACTOR_EN_BOX, SCENE_GANONTIKA, 22797, 0x0D, "Light Trial Third Left Chest",
        "Ganon's Castle Light Trial Third Left Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_LEFT_CHEST, RG_RECOVERY_HEART,
        {
            Category::cGanonsCastle,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE,
        ACTOR_EN_BOX, SCENE_GANONTIKA, 24462, 0x0E, "Light Trial First Right Chest",
        "Ganon's Castle Light Trial First Right Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_FIRST_RIGHT_CHEST, RG_ICE_TRAP,
        {
            Category::cGanonsCastle,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE,
        ACTOR_EN_BOX, SCENE_GANONTIKA, 22890, 0x0A, "Light Trial Second Right Chest",
        "Ganon's Castle Light Trial Second Right Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_SECOND_RIGHT_CHEST, RG_ARROWS_30,
        {
            Category::cGanonsCastle,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST, RCQUEST_VANILLA, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE,
        ACTOR_EN_BOX, SCENE_GANONTIKA, 24463, 0x0F, "Light Trial Third Right Chest",
        "Ganon's Castle Light Trial Third Right Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_THIRD_RIGHT_CHEST, RG_ICE_TRAP,
        {
            Category::cGanonsCastle,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY, RCAREA_GANONS_CASTLE,
        ACTOR_EN_BOX, SCENE_GANONTIKA, 30800, 0x10, "Light Trial Invisible Enemies Chest",
        "Ganon's Castle Light Trial Invisible Enemies Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_INVISIBLE_ENEMIES_CHEST,
        RG_GANONS_CASTLE_SMALL_KEY, { Category::cGanonsCastle, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST] =
        Location::Chest(RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST, RCQUEST_VANILLA, RCTYPE_SMALL_KEY,
                        RCAREA_GANONS_CASTLE, ACTOR_EN_BOX, SCENE_GANONTIKA, -30639, 0x11, "Light Trial Lullaby Chest",
                        "Ganon's Castle Light Trial Lullaby Chest", RC_GANONS_CASTLE_LIGHT_TRIAL_LULLABY_CHEST,
                        RG_GANONS_CASTLE_SMALL_KEY, { Category::cGanonsCastle, Category::cVanillaSmallKey },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT] = Location::Base(
        RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT, RCQUEST_VANILLA, RCTYPE_SCRUB, RCAREA_GANONS_CASTLE, ACTOR_EN_DNS,
        SCENE_GANONTIKA, 0x05, 0x37, "Deku Scrub Center-Left", "Ganon's Castle Deku Scrub Center-Left",
        RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_LEFT, RG_BUY_BOMBS_535,
        {
            Category::cGanonsCastle,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x0D, 0x06), SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT] = Location::Base(
        RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT, RCQUEST_VANILLA, RCTYPE_SCRUB, RCAREA_GANONS_CASTLE, ACTOR_EN_DNS,
        SCENE_GANONTIKA, 0x03, 0x33, "Deku Scrub Center-Right", "Ganon's Castle Deku Scrub Center-Right",
        RC_GANONS_CASTLE_DEKU_SCRUB_CENTER_RIGHT, RG_BUY_ARROWS_30,
        {
            Category::cGanonsCastle,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x0D, 0x04), SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT] = Location::Base(
        RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT, RCQUEST_VANILLA, RCTYPE_SCRUB, RCAREA_GANONS_CASTLE, ACTOR_EN_DNS,
        SCENE_GANONTIKA, 0x07, 0x39, "Deku Scrub Right", "Ganon's Castle Deku Scrub Right",
        RC_GANONS_CASTLE_DEKU_SCRUB_RIGHT, RG_BUY_RED_POTION_30,
        {
            Category::cGanonsCastle,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x0D, 0x08), SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_DEKU_SCRUB_LEFT] = Location::Base(
        RC_GANONS_CASTLE_DEKU_SCRUB_LEFT, RCQUEST_VANILLA, RCTYPE_SCRUB, RCAREA_GANONS_CASTLE, ACTOR_EN_DNS,
        SCENE_GANONTIKA, 0x08, 0x3A, "Deku Scrub Left", "Ganon's Castle Deku Scrub Left",
        RC_GANONS_CASTLE_DEKU_SCRUB_LEFT, RG_BUY_GREEN_POTION,
        {
            Category::cGanonsCastle,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x0D, 0x09), SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);

    // Ganon's Castle MQ
    locationTable[RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST] =
        Location::Chest(RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE,
                        ACTOR_EN_BOX, SCENE_GANONTIKA, 22977, 0x01, "MQ Water Trial Chest",
                        "Ganon's Castle MQ Water Trial Chest", RC_GANONS_CASTLE_MQ_WATER_TRIAL_CHEST, RG_RED_RUPEE,
                        {
                            Category::cGanonsCastle,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST] =
        Location::Chest(RC_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_GANONS_CASTLE, ACTOR_EN_BOX, SCENE_GANONTIKA, -30398, 0x02,
                        "MQ Forest Trial Eye Switch Chest", "Ganon's Castle MQ Forest Trial Eye Switch Chest",
                        RC_GANONS_CASTLE_MQ_FOREST_TRIAL_EYE_SWITCH_CHEST, RG_ARROWS_10,
                        {
                            Category::cGanonsCastle,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE,
        ACTOR_EN_BOX, SCENE_GANONTIKA, -32733, 0x03, "MQ Forest Trial Frozen Eye Switch Chest",
        "Ganon's Castle MQ Forest Trial Frozen Eye Switch Chest",
        RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FROZEN_EYE_SWITCH_CHEST, RG_BOMBS_5,
        {
            Category::cGanonsCastle,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE, ACTOR_EN_BOX,
        SCENE_GANONTIKA, -30460, 0x04, "MQ Light Trial Lullaby Chest", "Ganon's Castle MQ Light Trial Lullaby Chest",
        RC_GANONS_CASTLE_MQ_LIGHT_TRIAL_LULLABY_CHEST, RG_RECOVERY_HEART,
        {
            Category::cGanonsCastle,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST] =
        Location::Chest(RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_GANONS_CASTLE, ACTOR_EN_BOX, SCENE_GANONTIKA, -30400, 0x00,
                        "MQ Shadow Trial Bomb Flower Chest", "Ganon's Castle MQ Shadow Trial Bomb Flower Chest",
                        RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_BOMB_FLOWER_CHEST, RG_ARROWS_10,
                        {
                            Category::cGanonsCastle,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_GANONS_CASTLE,
        ACTOR_EN_BOX, SCENE_GANONTIKA, -30651, 0x05, "MQ Shadow Trial Eye Switch Chest",
        "Ganon's Castle MQ Shadow Trial Eye Switch Chest", RC_GANONS_CASTLE_MQ_SHADOW_TRIAL_EYE_SWITCH_CHEST,
        RG_GANONS_CASTLE_SMALL_KEY, { Category::cGanonsCastle, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE,
        ACTOR_EN_BOX, SCENE_GANONTIKA, -18746, 0x06, "MQ Spirit Trial Golden Gauntlets Chest",
        "Ganon's Castle MQ Spirit Trial Golden Gauntlets Chest",
        RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_GOLDEN_GAUNTLETS_CHEST, RG_PROGRESSIVE_STRENGTH,
        {
            Category::cGanonsCastle,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST] =
        Location::Chest(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_GANONS_CASTLE, ACTOR_EN_BOX, SCENE_GANONTIKA, -30457, 0x07,
                        "MQ Spirit Trial Sun Back Right Chest", "Ganon's Castle MQ Spirit Trial Sun Back Right Chest",
                        RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_RIGHT_CHEST, RG_RECOVERY_HEART,
                        {
                            Category::cGanonsCastle,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_GANONS_CASTLE,
        ACTOR_EN_BOX, SCENE_GANONTIKA, -30648, 0x08, "MQ Spirit Trial Sun Back Left Chest",
        "Ganon's Castle MQ Spirit Trial Sun Back Left Chest", RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_BACK_LEFT_CHEST,
        RG_GANONS_CASTLE_SMALL_KEY, { Category::cGanonsCastle, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST] =
        Location::Chest(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_GANONS_CASTLE, ACTOR_EN_BOX, SCENE_GANONTIKA, -30455, 0x09,
                        "MQ Spirit Trial Sun Front Left Chest", "Ganon's Castle MQ Spirit Trial Sun Front Left Chest",
                        RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_SUN_FRONT_LEFT_CHEST, RG_RECOVERY_HEART,
                        {
                            Category::cGanonsCastle,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST] = Location::Chest(
        RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST, RCQUEST_MQ, RCTYPE_STANDARD, RCAREA_GANONS_CASTLE, ACTOR_EN_BOX,
        SCENE_GANONTIKA, 20586, 0x0A, "MQ Spirit Trial First Chest", "Ganon's Castle MQ Spirit Trial First Chest",
        RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_FIRST_CHEST, RG_BOMBCHU_10,
        {
            Category::cGanonsCastle,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST] =
        Location::Chest(RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST, RCQUEST_MQ, RCTYPE_STANDARD,
                        RCAREA_GANONS_CASTLE, ACTOR_EN_BOX, SCENE_GANONTIKA, 26964, 0x14,
                        "MQ Spirit Trial Invisible Chest", "Ganon's Castle MQ Spirit Trial Invisible Chest",
                        RC_GANONS_CASTLE_MQ_SPIRIT_TRIAL_INVISIBLE_CHEST, RG_ARROWS_10,
                        {
                            Category::cGanonsCastle,
                        },
                        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY] = Location::Collectable(
        RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY, RCQUEST_MQ, RCTYPE_SMALL_KEY, RCAREA_GANONS_CASTLE,
        ACTOR_EN_ITEM00, SCENE_GANONTIKA, 273, 0x01, "MQ Forest Trial Freestanding Key",
        "Ganon's Castle MQ Forest Trial Freestanding Key", RC_GANONS_CASTLE_MQ_FOREST_TRIAL_FREESTANDING_KEY,
        RG_GANONS_CASTLE_SMALL_KEY, { Category::cGanonsCastle, Category::cVanillaSmallKey },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT] = Location::Base(
        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT, RCQUEST_MQ, RCTYPE_SCRUB, RCAREA_GANONS_CASTLE, ACTOR_EN_DNS,
        SCENE_GANONTIKA, 0x00, 0x30, "MQ Deku Scrub Right", "Ganon's Castle MQ Deku Scrub Right",
        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_RIGHT, RG_BUY_DEKU_NUT_5,
        {
            Category::cGanonsCastle,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x0D, 0x01), SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT] = Location::Base(
        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT, RCQUEST_MQ, RCTYPE_SCRUB, RCAREA_GANONS_CASTLE, ACTOR_EN_DNS,
        SCENE_GANONTIKA, 0x05, 0x37, "MQ Deku Scrub Center-Left", "Ganon's Castle MQ Deku Scrub Center-Left",
        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_LEFT, RG_BUY_BOMBS_535,
        {
            Category::cGanonsCastle,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x0D, 0x06), SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER] = Location::Base(
        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER, RCQUEST_MQ, RCTYPE_SCRUB, RCAREA_GANONS_CASTLE, ACTOR_EN_DNS,
        SCENE_GANONTIKA, 0x03, 0x33, "MQ Deku Scrub Center", "Ganon's Castle MQ Deku Scrub Center",
        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER, RG_BUY_ARROWS_30,
        {
            Category::cGanonsCastle,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x0D, 0x04), SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT] = Location::Base(
        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT, RCQUEST_MQ, RCTYPE_SCRUB, RCAREA_GANONS_CASTLE, ACTOR_EN_DNS,
        SCENE_GANONTIKA, 0x07, 0x39, "MQ Deku Scrub Center-Right", "Ganon's Castle MQ Deku Scrub Center-Right",
        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_CENTER_RIGHT, RG_BUY_RED_POTION_30,
        {
            Category::cGanonsCastle,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x0D, 0x08), SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT] = Location::Base(
        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT, RCQUEST_MQ, RCTYPE_SCRUB, RCAREA_GANONS_CASTLE, ACTOR_EN_DNS,
        SCENE_GANONTIKA, 0x08, 0x3A, "MQ Deku Scrub Left", "Ganon's Castle MQ Deku Scrub Left",
        RC_GANONS_CASTLE_MQ_DEKU_SCRUB_LEFT, RG_BUY_GREEN_POTION,
        {
            Category::cGanonsCastle,
            Category::cDekuScrub,
        },
        SpoilerCollectionCheck::Scrub(0x0D, 0x09), SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);

    // Gold Skulltula Tokens

    // Dungeons
    locationTable[RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM] =
        Location::GSToken(RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_DEKU_TREE,
                          ACTOR_EN_SI, SCENE_YDAN, 8193, 0x01, "GS Basement Back Room",
                          "Deku Tree GS Basement Back Room", RC_DEKU_TREE_GS_BASEMENT_BACK_ROOM,
                          {
                              Category::cDekuTree,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_GS_BASEMENT_GATE] = Location::GSToken(
        RC_DEKU_TREE_GS_BASEMENT_GATE, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_DEKU_TREE, ACTOR_EN_SI, SCENE_YDAN,
        8194, 0x02, "GS Basement Gate", "Deku Tree GS Basement Gate", RC_DEKU_TREE_GS_BASEMENT_GATE,
        {
            Category::cDekuTree,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_GS_BASEMENT_VINES] = Location::GSToken(
        RC_DEKU_TREE_GS_BASEMENT_VINES, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_DEKU_TREE, ACTOR_EN_SI, SCENE_YDAN,
        8196, 0x04, "GS Basement Vines", "Deku Tree GS Basement Vines", RC_DEKU_TREE_GS_BASEMENT_VINES,
        {
            Category::cDekuTree,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_GS_COMPASS_ROOM] = Location::GSToken(
        RC_DEKU_TREE_GS_COMPASS_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_DEKU_TREE, ACTOR_EN_SI, SCENE_YDAN,
        8200, 0x08, "GS Compass Room", "Deku Tree GS Compass Room", RC_DEKU_TREE_GS_COMPASS_ROOM,
        {
            Category::cDekuTree,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_MQ_GS_LOBBY] =
        Location::GSToken(RC_DEKU_TREE_MQ_GS_LOBBY, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_DEKU_TREE, ACTOR_EN_SI,
                          SCENE_YDAN, 8194, 0x02, "MQ GS Lobby", "Deku Tree MQ GS Lobby", RC_DEKU_TREE_MQ_GS_LOBBY,
                          {
                              Category::cDekuTree,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_MQ_GS_COMPASS_ROOM] = Location::GSToken(
        RC_DEKU_TREE_MQ_GS_COMPASS_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_DEKU_TREE, ACTOR_EN_SI, SCENE_YDAN,
        8200, 0x08, "MQ GS Compass Room", "Deku Tree MQ GS Compass Room", RC_DEKU_TREE_MQ_GS_COMPASS_ROOM,
        {
            Category::cDekuTree,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM] =
        Location::GSToken(RC_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_DEKU_TREE,
                          ACTOR_EN_SI, SCENE_YDAN, 8196, 0x04, "MQ GS Basement Graves Room",
                          "Deku Tree MQ GS Basement Graves Room", RC_DEKU_TREE_MQ_GS_BASEMENT_GRAVES_ROOM,
                          {
                              Category::cDekuTree,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM] =
        Location::GSToken(RC_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_DEKU_TREE,
                          ACTOR_EN_SI, SCENE_YDAN, 8193, 0x01, "MQ GS Basement Back Room",
                          "Deku Tree MQ GS Basement Back Room", RC_DEKU_TREE_MQ_GS_BASEMENT_BACK_ROOM,
                          {
                              Category::cDekuTree,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);

    // Dodongo's Cavern
    locationTable[RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS] =
        Location::GSToken(RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_DODONGOS_CAVERN, ACTOR_EN_SI, SCENE_DDAN, 8449, 0x01, "GS Vines Above Stairs",
                          "Dodongos Cavern GS Vines Above Stairs", RC_DODONGOS_CAVERN_GS_VINES_ABOVE_STAIRS,
                          {
                              Category::cDodongosCavern,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_GS_SCARECROW] = Location::GSToken(
        RC_DODONGOS_CAVERN_GS_SCARECROW, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_DODONGOS_CAVERN, ACTOR_EN_SI,
        SCENE_DDAN, 8450, 0x02, "GS Scarecrow", "Dodongos Cavern GS Scarecrow", RC_DODONGOS_CAVERN_GS_SCARECROW,
        {
            Category::cDodongosCavern,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS] =
        Location::GSToken(RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_DODONGOS_CAVERN, ACTOR_EN_SI, SCENE_DDAN, 8452, 0x04, "GS Alcove Above Stairs",
                          "Dodongos Cavern GS Alcove Above Stairs", RC_DODONGOS_CAVERN_GS_ALCOVE_ABOVE_STAIRS,
                          {
                              Category::cDodongosCavern,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_GS_BACK_ROOM] = Location::GSToken(
        RC_DODONGOS_CAVERN_GS_BACK_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_DODONGOS_CAVERN, ACTOR_EN_SI,
        SCENE_DDAN, 8456, 0x08, "GS Back Room", "Dodongos Cavern GS Back Room", RC_DODONGOS_CAVERN_GS_BACK_ROOM,
        {
            Category::cDodongosCavern,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS] = Location::GSToken(
        RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
        RCAREA_DODONGOS_CAVERN, ACTOR_EN_SI, SCENE_DDAN, 8464, 0x10, "GS Side Room Near Lower Lizalfos",
        "Dodongos Cavern GS Side Room Near Lower Lizalfos", RC_DODONGOS_CAVERN_GS_SIDE_ROOM_NEAR_LOWER_LIZALFOS,
        {
            Category::cDodongosCavern,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM] =
        Location::GSToken(RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_DODONGOS_CAVERN,
                          ACTOR_EN_SI, SCENE_DDAN, 8450, 0x02, "MQ GS Scrub Room", "Dodongos Cavern MQ GS Scrub Room",
                          RC_DODONGOS_CAVERN_MQ_GS_SCRUB_ROOM,
                          {
                              Category::cDodongosCavern,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM] = Location::GSToken(
        RC_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_DODONGOS_CAVERN,
        ACTOR_EN_SI, SCENE_DDAN, 8456, 0x08, "MQ GS Song of Time Block Room",
        "Dodongos Cavern MQ GS Song of Time Block Room", RC_DODONGOS_CAVERN_MQ_GS_SONG_OF_TIME_BLOCK_ROOM,
        {
            Category::cDodongosCavern,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM] =
        Location::GSToken(RC_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN,
                          RCAREA_DODONGOS_CAVERN, ACTOR_EN_SI, SCENE_DDAN, 8452, 0x04, "MQ GS Lizalfos Room",
                          "Dodongos Cavern MQ GS Lizalfos Room", RC_DODONGOS_CAVERN_MQ_GS_LIZALFOS_ROOM,
                          {
                              Category::cDodongosCavern,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM] =
        Location::GSToken(RC_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_DODONGOS_CAVERN,
                          ACTOR_EN_SI, SCENE_DDAN, 8464, 0x10, "MQ GS Larvae Room", "Dodongos Cavern MQ GS Larvae Room",
                          RC_DODONGOS_CAVERN_MQ_GS_LARVAE_ROOM,
                          {
                              Category::cDodongosCavern,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA] =
        Location::GSToken(RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_DODONGOS_CAVERN,
                          ACTOR_EN_SI, SCENE_DDAN, 8449, 0x01, "MQ GS Back Room", "Dodongos Cavern MQ GS Back Room",
                          RC_DODONGOS_CAVERN_MQ_GS_BACK_AREA,
                          {
                              Category::cDodongosCavern,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER] =
        Location::GSToken(RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_JABU_JABUS_BELLY, ACTOR_EN_SI, SCENE_BDAN, 8705, 0x01, "GS Lobby Basement Lower",
                          "Jabu Jabus Belly GS Lobby Basement Lower", RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_LOWER,
                          {
                              Category::cJabuJabusBelly,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER] =
        Location::GSToken(RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_JABU_JABUS_BELLY, ACTOR_EN_SI, SCENE_BDAN, 8706, 0x02, "GS Lobby Basement Upper",
                          "Jabu Jabus Belly GS Lobby Basement Upper", RC_JABU_JABUS_BELLY_GS_LOBBY_BASEMENT_UPPER,
                          {
                              Category::cJabuJabusBelly,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_GS_NEAR_BOSS] = Location::GSToken(
        RC_JABU_JABUS_BELLY_GS_NEAR_BOSS, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_JABU_JABUS_BELLY, ACTOR_EN_SI,
        SCENE_BDAN, 8708, 0x04, "GS Near Boss", "Jabu Jabus Belly GS Near Boss", RC_JABU_JABUS_BELLY_GS_NEAR_BOSS,
        {
            Category::cJabuJabusBelly,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM] =
        Location::GSToken(RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_JABU_JABUS_BELLY, ACTOR_EN_SI, SCENE_BDAN, 8712, 0x08, "GS Water Switch Room",
                          "Jabu Jabus Belly GS Water Switch Room", RC_JABU_JABUS_BELLY_GS_WATER_SWITCH_ROOM,
                          {
                              Category::cJabuJabusBelly,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM] =
        Location::GSToken(RC_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN,
                          RCAREA_JABU_JABUS_BELLY, ACTOR_EN_SI, SCENE_BDAN, 8708, 0x04, "MQ GS Tail Parasan Room",
                          "Jabu Jabus Belly MQ GS Tail Parasan Room", RC_JABU_JABUS_BELLY_MQ_GS_TAILPASARAN_ROOM,
                          {
                              Category::cJabuJabusBelly,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM] = Location::GSToken(
        RC_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_JABU_JABUS_BELLY,
        ACTOR_EN_SI, SCENE_BDAN, 8712, 0x08, "MQ GS Invisible Enemies Room",
        "Jabu Jabus Belly MQ GS Invisible Enemies Room", RC_JABU_JABUS_BELLY_MQ_GS_INVISIBLE_ENEMIES_ROOM,
        {
            Category::cJabuJabusBelly,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM] =
        Location::GSToken(RC_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN,
                          RCAREA_JABU_JABUS_BELLY, ACTOR_EN_SI, SCENE_BDAN, 8705, 0x01, "MQ GS Boomerang Chest Room",
                          "Jabu Jabus Belly MQ GS Boomerang Chest Room", RC_JABU_JABUS_BELLY_MQ_GS_BOOMERANG_CHEST_ROOM,
                          {
                              Category::cJabuJabusBelly,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS] =
        Location::GSToken(RC_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_JABU_JABUS_BELLY,
                          ACTOR_EN_SI, SCENE_BDAN, 8706, 0x02, "MQ GS Near Boss", "Jabu Jabus Belly MQ GS Near Boss",
                          RC_JABU_JABUS_BELLY_MQ_GS_NEAR_BOSS,
                          {
                              Category::cJabuJabusBelly,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD] =
        Location::GSToken(RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_FOREST_TEMPLE, ACTOR_EN_SI, SCENE_BMORI1, 8961, 0x01, "GS Raised Island Courtyard",
                          "Forest Temple GS Raised Island Courtyard", RC_FOREST_TEMPLE_GS_RAISED_ISLAND_COURTYARD,
                          {
                              Category::cForestTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_GS_FIRST_ROOM] = Location::GSToken(
        RC_FOREST_TEMPLE_GS_FIRST_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_FOREST_TEMPLE, ACTOR_EN_SI,
        SCENE_BMORI1, 8962, 0x02, "GS First Room", "Forest Temple GS First Room", RC_FOREST_TEMPLE_GS_FIRST_ROOM,
        {
            Category::cForestTemple,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD] =
        Location::GSToken(RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_FOREST_TEMPLE, ACTOR_EN_SI, SCENE_BMORI1, 8964, 0x04, "GS Level Island Courtyard",
                          "Forest Temple GS Level Island Courtyard", RC_FOREST_TEMPLE_GS_LEVEL_ISLAND_COURTYARD,
                          {
                              Category::cForestTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_GS_LOBBY] = Location::GSToken(
        RC_FOREST_TEMPLE_GS_LOBBY, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_FOREST_TEMPLE, ACTOR_EN_SI, SCENE_BMORI1,
        8968, 0x08, "GS Lobby", "Forest Temple GS Lobby", RC_FOREST_TEMPLE_GS_LOBBY,
        {
            Category::cForestTemple,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_GS_BASEMENT] = Location::GSToken(
        RC_FOREST_TEMPLE_GS_BASEMENT, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_FOREST_TEMPLE, ACTOR_EN_SI,
        SCENE_BMORI1, 8976, 0x10, "GS Basement", "Forest Temple GS Basement", RC_FOREST_TEMPLE_GS_BASEMENT,
        {
            Category::cForestTemple,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY] =
        Location::GSToken(RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_FOREST_TEMPLE,
                          ACTOR_EN_SI, SCENE_BMORI1, 8962, 0x02, "MQ GS First Hallway",
                          "Forest Temple MQ GS First Hallway", RC_FOREST_TEMPLE_MQ_GS_FIRST_HALLWAY,
                          {
                              Category::cForestTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM] =
        Location::GSToken(RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_FOREST_TEMPLE,
                          ACTOR_EN_SI, SCENE_BMORI1, 8976, 0x10, "MQ GS Block Push Room",
                          "Forest Temple MQ GS Block Push Room", RC_FOREST_TEMPLE_MQ_GS_BLOCK_PUSH_ROOM,
                          {
                              Category::cForestTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD] =
        Location::GSToken(RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD, RCQUEST_MQ, RCTYPE_SKULL_TOKEN,
                          RCAREA_FOREST_TEMPLE, ACTOR_EN_SI, SCENE_BMORI1, 8961, 0x01, "MQ GS Raised Island Courtyard",
                          "Forest Temple MQ GS Raised Island Courtyard", RC_FOREST_TEMPLE_MQ_GS_RAISED_ISLAND_COURTYARD,
                          {
                              Category::cForestTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD] =
        Location::GSToken(RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD, RCQUEST_MQ, RCTYPE_SKULL_TOKEN,
                          RCAREA_FOREST_TEMPLE, ACTOR_EN_SI, SCENE_BMORI1, 8964, 0x04, "MQ GS Level Island Courtyard",
                          "Forest Temple MQ GS Level Island Courtyard", RC_FOREST_TEMPLE_MQ_GS_LEVEL_ISLAND_COURTYARD,
                          {
                              Category::cForestTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_FOREST_TEMPLE_MQ_GS_WELL] = Location::GSToken(
        RC_FOREST_TEMPLE_MQ_GS_WELL, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_FOREST_TEMPLE, ACTOR_EN_SI, SCENE_BMORI1,
        8968, 0x08, "MQ GS Well", "Forest Temple MQ GS Well", RC_FOREST_TEMPLE_MQ_GS_WELL,
        {
            Category::cForestTemple,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);

    // Fire Temple
    locationTable[RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM] =
        Location::GSToken(RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_FIRE_TEMPLE,
                          ACTOR_EN_SI, SCENE_HIDAN, 9217, 0x01, "GS Song of Time Room",
                          "Fire Temple GS Song of Time Room", RC_FIRE_TEMPLE_GS_SONG_OF_TIME_ROOM,
                          {
                              Category::cFireTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP] = Location::GSToken(
        RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_FIRE_TEMPLE, ACTOR_EN_SI,
        SCENE_HIDAN, 9218, 0x02, "GS Boss Key Loop", "Fire Temple GS Boss Key Loop", RC_FIRE_TEMPLE_GS_BOSS_KEY_LOOP,
        {
            Category::cFireTemple,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_GS_BOULDER_MAZE] = Location::GSToken(
        RC_FIRE_TEMPLE_GS_BOULDER_MAZE, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_FIRE_TEMPLE, ACTOR_EN_SI,
        SCENE_HIDAN, 9220, 0x04, "GS Boulder Maze", "Fire Temple GS Boulder Maze", RC_FIRE_TEMPLE_GS_BOULDER_MAZE,
        {
            Category::cFireTemple,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_GS_SCARECROW_TOP] = Location::GSToken(
        RC_FIRE_TEMPLE_GS_SCARECROW_TOP, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_FIRE_TEMPLE, ACTOR_EN_SI,
        SCENE_HIDAN, 9224, 0x08, "GS Scarecrow Top", "Fire Temple GS Scarecrow Top", RC_FIRE_TEMPLE_GS_SCARECROW_TOP,
        {
            Category::cFireTemple,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB] =
        Location::GSToken(RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_FIRE_TEMPLE,
                          ACTOR_EN_SI, SCENE_HIDAN, 9232, 0x10, "GS Scarecrow Climb", "Fire Temple GS Scarecrow Climb",
                          RC_FIRE_TEMPLE_GS_SCARECROW_CLIMB,
                          {
                              Category::cFireTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE] =
        Location::GSToken(RC_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_FIRE_TEMPLE,
                          ACTOR_EN_SI, SCENE_HIDAN, 9218, 0x02, "MQ GS Above Fire Wall Maze",
                          "Fire Temple MQ GS Above Fire Wall Maze", RC_FIRE_TEMPLE_MQ_GS_ABOVE_FIRE_WALL_MAZE,
                          {
                              Category::cFireTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER] =
        Location::GSToken(RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER, RCQUEST_MQ, RCTYPE_SKULL_TOKEN,
                          RCAREA_FIRE_TEMPLE, ACTOR_EN_SI, SCENE_HIDAN, 9224, 0x08, "MQ GS Fire Wall Maze Center",
                          "Fire Temple MQ GS Fire Wall Maze Center", RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_CENTER,
                          {
                              Category::cFireTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR] =
        Location::GSToken(RC_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR, RCQUEST_MQ, RCTYPE_SKULL_TOKEN,
                          RCAREA_FIRE_TEMPLE, ACTOR_EN_SI, SCENE_HIDAN, 9217, 0x01, "MQ GS Big Lava Room Open Door",
                          "Fire Temple MQ GS Big Lava Room Open Door", RC_FIRE_TEMPLE_MQ_GS_BIG_LAVA_ROOM_OPEN_DOOR,
                          {
                              Category::cFireTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM] =
        Location::GSToken(RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN,
                          RCAREA_FIRE_TEMPLE, ACTOR_EN_SI, SCENE_HIDAN, 9232, 0x10, "MQ GS Fire Wall Maze Side Room",
                          "Fire Temple MQ GS Fire Wall Maze Side Room", RC_FIRE_TEMPLE_MQ_GS_FIRE_WALL_MAZE_SIDE_ROOM,
                          {
                              Category::cFireTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE] =
        Location::GSToken(RC_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_FIRE_TEMPLE,
                          ACTOR_EN_SI, SCENE_HIDAN, 9220, 0x04, "MQ GS Skull on Fire",
                          "Fire Temple MQ GS Skull on Fire", RC_FIRE_TEMPLE_MQ_GS_SKULL_ON_FIRE,
                          {
                              Category::cFireTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);

    // Water Temple
    locationTable[RC_WATER_TEMPLE_GS_BEHIND_GATE] = Location::GSToken(
        RC_WATER_TEMPLE_GS_BEHIND_GATE, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_WATER_TEMPLE, ACTOR_EN_SI,
        SCENE_MIZUSIN, 9473, 0x01, "GS Behind Gate", "Water Temple GS Behind Gate", RC_WATER_TEMPLE_GS_BEHIND_GATE,
        {
            Category::cWaterTemple,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM] =
        Location::GSToken(RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_WATER_TEMPLE, ACTOR_EN_SI, SCENE_MIZUSIN, 9474, 0x02, "GS Falling Platform Room",
                          "Water Temple GS Falling Platform Room", RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM,
                          {
                              Category::cWaterTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_GS_CENTRAL_PILLAR] =
        Location::GSToken(RC_WATER_TEMPLE_GS_CENTRAL_PILLAR, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_WATER_TEMPLE,
                          ACTOR_EN_SI, SCENE_MIZUSIN, 9476, 0x04, "GS Central Pillar", "Water Temple GS Central Pillar",
                          RC_WATER_TEMPLE_GS_CENTRAL_PILLAR,
                          {
                              Category::cWaterTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST] =
        Location::GSToken(RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_WATER_TEMPLE, ACTOR_EN_SI, SCENE_MIZUSIN, 9480, 0x08, "GS Near Boss Key Chest",
                          "Water Temple GS Near Boss Key Chest", RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST,
                          {
                              Category::cWaterTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_GS_RIVER] = Location::GSToken(
        RC_WATER_TEMPLE_GS_RIVER, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_WATER_TEMPLE, ACTOR_EN_SI, SCENE_MIZUSIN,
        9488, 0x10, "GS River", "Water Temple GS River", RC_WATER_TEMPLE_GS_RIVER,
        {
            Category::cWaterTemple,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH] = Location::GSToken(
        RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_WATER_TEMPLE,
        ACTOR_EN_SI, SCENE_MIZUSIN, 9476, 0x04, "MQ GS Before Upper Water Switch",
        "Water Temple MQ GS Before Upper Water Switch", RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH,
        {
            Category::cWaterTemple,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA] =
        Location::GSToken(RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA, RCQUEST_MQ, RCTYPE_SKULL_TOKEN,
                          RCAREA_WATER_TEMPLE, ACTOR_EN_SI, SCENE_MIZUSIN, 9480, 0x08, "MQ GS Freestanding Key Area",
                          "Water Temple MQ GS Freestanding Key Area", RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA,
                          {
                              Category::cWaterTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY] =
        Location::GSToken(RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_WATER_TEMPLE,
                          ACTOR_EN_SI, SCENE_MIZUSIN, 9473, 0x01, "MQ GS Lizalfos Hallway",
                          "Water Temple MQ GS Lizalfos Hallway", RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY,
                          {
                              Category::cWaterTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_MQ_GS_RIVER] = Location::GSToken(
        RC_WATER_TEMPLE_MQ_GS_RIVER, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_WATER_TEMPLE, ACTOR_EN_SI, SCENE_MIZUSIN,
        9474, 0x02, "MQ GS River", "Water Temple MQ GS River", RC_WATER_TEMPLE_MQ_GS_RIVER,
        {
            Category::cWaterTemple,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH] =
        Location::GSToken(RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_WATER_TEMPLE,
                          ACTOR_EN_SI, SCENE_MIZUSIN, 9488, 0x10, "MQ GS Triple Wall Torch",
                          "Water Temple MQ GS Triple Wall Torch", RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH,
                          {
                              Category::cWaterTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);

    // Spirit Temple
    locationTable[RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM] = Location::GSToken(
        RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_SPIRIT_TEMPLE,
        ACTOR_EN_SI, SCENE_JYASINZOU, 9729, 0x01, "GS Hall After Sun Block Room",
        "Spirit Temple GS Hall After Sun Block Room", RC_SPIRIT_TEMPLE_GS_HALL_AFTER_SUN_BLOCK_ROOM,
        { Category::cSpiritTemple, Category::cSkulltula }, SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM] =
        Location::GSToken(RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_SPIRIT_TEMPLE,
                          ACTOR_EN_SI, SCENE_JYASINZOU, 9730, 0x02, "GS Boulder Room", "Spirit Temple GS Boulder Room",
                          RC_SPIRIT_TEMPLE_GS_BOULDER_ROOM, { Category::cSpiritTemple, Category::cSkulltula },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_GS_LOBBY] = Location::GSToken(
        RC_SPIRIT_TEMPLE_GS_LOBBY, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_SI,
        SCENE_JYASINZOU, 9732, 0x04, "GS Lobby", "Spirit Temple GS Lobby", RC_SPIRIT_TEMPLE_GS_LOBBY,
        { Category::cSpiritTemple, Category::cSkulltula }, SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM] = Location::GSToken(
        RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_SI,
        SCENE_JYASINZOU, 9736, 0x08, "GS Sun on Floor Room", "Spirit Temple GS Sun on Floor Room",
        RC_SPIRIT_TEMPLE_GS_SUN_ON_FLOOR_ROOM, { Category::cSpiritTemple, Category::cSkulltula },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_GS_METAL_FENCE] = Location::GSToken(
        RC_SPIRIT_TEMPLE_GS_METAL_FENCE, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_SI,
        SCENE_JYASINZOU, 9744, 0x10, "GS Metal Fence", "Spirit Temple GS Metal Fence", RC_SPIRIT_TEMPLE_GS_METAL_FENCE,
        { Category::cSpiritTemple, Category::cSkulltula }, SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM] = Location::GSToken(
        RC_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_SI,
        SCENE_JYASINZOU, 9736, 0x08, "MQ GS Symphony Room", "Spirit Temple MQ GS Symphony Room",
        RC_SPIRIT_TEMPLE_MQ_GS_SYMPHONY_ROOM, { Category::cSpiritTemple, Category::cSkulltula },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM] = Location::GSToken(
        RC_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_SI,
        SCENE_JYASINZOU, 9730, 0x02, "MQ GS Leever Room", "Spirit Temple MQ GS Leever Room",
        RC_SPIRIT_TEMPLE_MQ_GS_LEEVER_ROOM, { Category::cSpiritTemple, Category::cSkulltula },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST] = Location::GSToken(
        RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_SPIRIT_TEMPLE,
        ACTOR_EN_SI, SCENE_JYASINZOU, 9732, 0x04, "MQ GS Nine Thrones Room West",
        "Spirit Temple MQ GS Nine Thrones Room West", RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_WEST,
        { Category::cSpiritTemple, Category::cSkulltula }, SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH] = Location::GSToken(
        RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_SPIRIT_TEMPLE,
        ACTOR_EN_SI, SCENE_JYASINZOU, 9744, 0x10, "MQ GS Nine Thrones Room North",
        "Spirit Temple MQ GS Nine Thrones Room North", RC_SPIRIT_TEMPLE_MQ_GS_NINE_THRONES_ROOM_NORTH,
        { Category::cSpiritTemple, Category::cSkulltula }, SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM] = Location::GSToken(
        RC_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_SPIRIT_TEMPLE, ACTOR_EN_SI,
        SCENE_JYASINZOU, 9729, 0x01, "MQ GS Sun Block Room", "Spirit Temple MQ GS Sun Block Room",
        RC_SPIRIT_TEMPLE_MQ_GS_SUN_BLOCK_ROOM, { Category::cSpiritTemple, Category::cSkulltula },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);

    // Shadow Temple
    locationTable[RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT] =
        Location::GSToken(RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_SHADOW_TEMPLE, ACTOR_EN_SI, SCENE_HAKADAN, 9985, 0x01, "GS Single Giant Pot",
                          "Shadow Temple GS Single Giant Pot", RC_SHADOW_TEMPLE_GS_SINGLE_GIANT_POT,
                          {
                              Category::cShadowTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM] =
        Location::GSToken(RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_SHADOW_TEMPLE, ACTOR_EN_SI, SCENE_HAKADAN, 9986, 0x02, "GS Falling Spikes Room",
                          "Shadow Temple GS Falling Spikes Room", RC_SHADOW_TEMPLE_GS_FALLING_SPIKES_ROOM,
                          {
                              Category::cShadowTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT] =
        Location::GSToken(RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_SHADOW_TEMPLE, ACTOR_EN_SI, SCENE_HAKADAN, 9988, 0x04, "GS Triple Giant Pot",
                          "Shadow Temple GS Triple Giant Pot", RC_SHADOW_TEMPLE_GS_TRIPLE_GIANT_POT,
                          {
                              Category::cShadowTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM] =
        Location::GSToken(RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_SHADOW_TEMPLE,
                          ACTOR_EN_SI, SCENE_HAKADAN, 9992, 0x08, "GS Like Like Room",
                          "Shadow Temple GS Like Like Room", RC_SHADOW_TEMPLE_GS_LIKE_LIKE_ROOM,
                          {
                              Category::cShadowTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_GS_NEAR_SHIP] = Location::GSToken(
        RC_SHADOW_TEMPLE_GS_NEAR_SHIP, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_SHADOW_TEMPLE, ACTOR_EN_SI,
        SCENE_HAKADAN, 10000, 0x10, "GS Near Ship", "Shadow Temple GS Near Ship", RC_SHADOW_TEMPLE_GS_NEAR_SHIP,
        {
            Category::cShadowTemple,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM] =
        Location::GSToken(RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN,
                          RCAREA_SHADOW_TEMPLE, ACTOR_EN_SI, SCENE_HAKADAN, 9986, 0x02, "MQ GS Falling Spikes Room",
                          "Shadow Temple MQ GS Falling Spikes Room", RC_SHADOW_TEMPLE_MQ_GS_FALLING_SPIKES_ROOM,
                          {
                              Category::cShadowTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM] =
        Location::GSToken(RC_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_SHADOW_TEMPLE,
                          ACTOR_EN_SI, SCENE_HAKADAN, 9985, 0x01, "MQ GS Wind Hint Room",
                          "Shadow Temple MQ GS Wind Hint Room", RC_SHADOW_TEMPLE_MQ_GS_WIND_HINT_ROOM,
                          {
                              Category::cShadowTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_GS_AFTER_WIND] =
        Location::GSToken(RC_SHADOW_TEMPLE_MQ_GS_AFTER_WIND, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_SHADOW_TEMPLE,
                          ACTOR_EN_SI, SCENE_HAKADAN, 9992, 0x08, "MQ GS After Wind", "Shadow Temple MQ GS After Wind",
                          RC_SHADOW_TEMPLE_MQ_GS_AFTER_WIND,
                          {
                              Category::cShadowTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP] =
        Location::GSToken(RC_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_SHADOW_TEMPLE,
                          ACTOR_EN_SI, SCENE_HAKADAN, 10000, 0x10, "MQ GS After Ship", "Shadow Temple MQ GS After Ship",
                          RC_SHADOW_TEMPLE_MQ_GS_AFTER_SHIP,
                          {
                              Category::cShadowTemple,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS] = Location::GSToken(
        RC_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_SHADOW_TEMPLE, ACTOR_EN_SI,
        SCENE_HAKADAN, 9988, 0x04, "MQ GS Near Boss", "Shadow Temple MQ GS Near Boss", RC_SHADOW_TEMPLE_MQ_GS_NEAR_BOSS,
        {
            Category::cShadowTemple,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);

    // Bottom of the Well
    locationTable[RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE] =
        Location::GSToken(RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_SI, SCENE_HAKADANCH, 10241, 0x01, "GS Like Like Cage",
                          "Bottom of the Well GS Like Like Cage", RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE,
                          {
                              Category::cBottomOfTheWell,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM] =
        Location::GSToken(RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_SI, SCENE_HAKADANCH, 10242, 0x02, "GS East Inner Room",
                          "Bottom of the Well GS East Inner Room", RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM,
                          {
                              Category::cBottomOfTheWell,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM] =
        Location::GSToken(RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN,
                          RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_SI, SCENE_HAKADANCH, 10244, 0x04, "GS West Inner Room",
                          "Bottom of the Well GS West Inner Room", RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM,
                          {
                              Category::cBottomOfTheWell,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT] =
        Location::GSToken(RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT, RCQUEST_MQ, RCTYPE_SKULL_TOKEN,
                          RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_SI, SCENE_HAKADANCH, 10241, 0x01, "MQ GS Basement",
                          "Bottom of the Well MQ GS Basement", RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT,
                          {
                              Category::cBottomOfTheWell,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM] =
        Location::GSToken(RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN,
                          RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_SI, SCENE_HAKADANCH, 10244, 0x04, "MQ GS Coffin Room",
                          "Bottom of the Well MQ GS Coffin Room", RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM,
                          {
                              Category::cBottomOfTheWell,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);
    locationTable[RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM] =
        Location::GSToken(RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM, RCQUEST_MQ, RCTYPE_SKULL_TOKEN,
                          RCAREA_BOTTOM_OF_THE_WELL, ACTOR_EN_SI, SCENE_HAKADANCH, 10242, 0x02, "MQ GS West Inner Room",
                          "Bottom of the Well MQ GS West Inner Room", RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM,
                          {
                              Category::cBottomOfTheWell,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_BOTTOM_OF_THE_WELL);

    // Ice Cavern
    locationTable[RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM] =
        Location::GSToken(RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_ICE_CAVERN,
                          ACTOR_EN_SI, SCENE_ICE_DOUKUTO, 10497, 0x01, "GS Push Block Room",
                          "Ice Cavern GS Push Block Room", RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM,
                          {
                              Category::cIceCavern,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_ICE_CAVERN);
    locationTable[RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM] =
        Location::GSToken(RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_ICE_CAVERN,
                          ACTOR_EN_SI, SCENE_ICE_DOUKUTO, 10498, 0x02, "GS Spinning Scythe Room",
                          "Ice Cavern GS Spinning Scythe Room", RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM,
                          {
                              Category::cIceCavern,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_ICE_CAVERN);
    locationTable[RC_ICE_CAVERN_GS_HEART_PIECE_ROOM] =
        Location::GSToken(RC_ICE_CAVERN_GS_HEART_PIECE_ROOM, RCQUEST_VANILLA, RCTYPE_SKULL_TOKEN, RCAREA_ICE_CAVERN,
                          ACTOR_EN_SI, SCENE_ICE_DOUKUTO, 10500, 0x04, "GS Heart Piece Room",
                          "Ice Cavern GS Heart Piece Room", RC_ICE_CAVERN_GS_HEART_PIECE_ROOM,
                          {
                              Category::cIceCavern,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_ICE_CAVERN);
    locationTable[RC_ICE_CAVERN_MQ_GS_SCARECROW] = Location::GSToken(
        RC_ICE_CAVERN_MQ_GS_SCARECROW, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_ICE_CAVERN, ACTOR_EN_SI,
        SCENE_ICE_DOUKUTO, 10497, 0x01, "MQ GS Scarecrow", "Ice Cavern MQ GS Scarecrow", RC_ICE_CAVERN_MQ_GS_SCARECROW,
        {
            Category::cIceCavern,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_ICE_CAVERN);
    locationTable[RC_ICE_CAVERN_MQ_GS_ICE_BLOCK] = Location::GSToken(
        RC_ICE_CAVERN_MQ_GS_ICE_BLOCK, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_ICE_CAVERN, ACTOR_EN_SI,
        SCENE_ICE_DOUKUTO, 10500, 0x04, "MQ GS Ice Block", "Ice Cavern MQ GS Ice Block", RC_ICE_CAVERN_MQ_GS_ICE_BLOCK,
        {
            Category::cIceCavern,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_ICE_CAVERN);
    locationTable[RC_ICE_CAVERN_MQ_GS_RED_ICE] = Location::GSToken(
        RC_ICE_CAVERN_MQ_GS_RED_ICE, RCQUEST_MQ, RCTYPE_SKULL_TOKEN, RCAREA_ICE_CAVERN, ACTOR_EN_SI, SCENE_ICE_DOUKUTO,
        10498, 0x02, "MQ GS Red Ice", "Ice Cavern MQ GS Red Ice", RC_ICE_CAVERN_MQ_GS_RED_ICE,
        {
            Category::cIceCavern,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DUNGEON_ICE_CAVERN);

    // Overworld

    // Kokiri Forest
    locationTable[RC_KF_GS_BEAN_PATCH] =
        Location::GSToken(RC_KF_GS_BEAN_PATCH, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_KOKIRI_FOREST, ACTOR_EN_SI,
                          SCENE_SPOT04, 27649, 0x01, "GS Bean Patch", "KF GS Bean Patch", RC_KF_GS_BEAN_PATCH,
                          {
                              Category::cKokiriForest,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_KOKIRI_FOREST);
    locationTable[RC_KF_GS_KNOW_IT_ALL_HOUSE] = Location::GSToken(
        RC_KF_GS_KNOW_IT_ALL_HOUSE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_KOKIRI_FOREST, ACTOR_EN_SI, SCENE_SPOT04,
        19458, 0x02, "GS Know It All House", "KF GS Know It All House", RC_KF_GS_KNOW_IT_ALL_HOUSE,
        {
            Category::cKokiriForest,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_KOKIRI_FOREST);
    locationTable[RC_KF_GS_HOUSE_OF_TWINS] = Location::GSToken(
        RC_KF_GS_HOUSE_OF_TWINS, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_KOKIRI_FOREST, ACTOR_EN_SI, SCENE_SPOT04,
        19460, 0x04, "GS House of Twins", "KF GS House of Twins", RC_KF_GS_HOUSE_OF_TWINS,
        {
            Category::cKokiriForest,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_KOKIRI_FOREST);

    // Lost Woods
    locationTable[RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE] = Location::GSToken(
        RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_LOST_WOODS, ACTOR_EN_SI, SCENE_SPOT10,
        27905, 0x01, "GS Bean Patch Near Bridge", "LW GS Bean Patch Near Bridge", RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE,
        {
            Category::cLostWoods,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_LW_GS_BEAN_PATCH_NEAR_THEATER] =
        Location::GSToken(RC_LW_GS_BEAN_PATCH_NEAR_THEATER, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_LOST_WOODS,
                          ACTOR_EN_SI, SCENE_SPOT10, 27906, 0x02, "GS Bean Patch Near Theater",
                          "LW GS Bean Patch Near Theater", RC_LW_GS_BEAN_PATCH_NEAR_THEATER,
                          {
                              Category::cLostWoods,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_LW_GS_ABOVE_THEATER] =
        Location::GSToken(RC_LW_GS_ABOVE_THEATER, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_LOST_WOODS, ACTOR_EN_SI,
                          SCENE_SPOT10, 19716, 0x04, "GS Above Theater", "LW GS Above Theater", RC_LW_GS_ABOVE_THEATER,
                          {
                              Category::cLostWoods,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);
    locationTable[RC_SFM_GS] =
        Location::GSToken(RC_SFM_GS, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_SACRED_FOREST_MEADOW, ACTOR_EN_SI,
                          SCENE_SPOT05, 19720, 0x08, "GS", "SFM GS", RC_SFM_GS,
                          {
                              Category::cSacredForestMeadow,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_LOST_WOODS);

    // Hyrule Field
    locationTable[RC_HF_GS_COW_GROTTO] =
        Location::GSToken(RC_HF_GS_COW_GROTTO, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_HYRULE_FIELD, ACTOR_EN_SI,
                          SCENE_KAKUSIANA, 10753, 0x01, "GS Cow Grotto", "HF GS Cow Grotto", RC_HF_GS_COW_GROTTO,
                          { Category::cHyruleField, Category::cSkulltula, Category::cGrotto },
                          SpoilerCollectionCheckGroup::GROUP_HYRULE_FIELD);
    locationTable[RC_HF_GS_NEAR_KAK_GROTTO] =
        Location::GSToken(RC_HF_GS_NEAR_KAK_GROTTO, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_HYRULE_FIELD, ACTOR_EN_SI,
                          SCENE_KAKUSIANA, 10754, 0x02, "GS Near Kak Grotto", "HF GS Near Kak Grotto",
                          RC_HF_GS_NEAR_KAK_GROTTO, { Category::cHyruleField, Category::cSkulltula, Category::cGrotto },
                          SpoilerCollectionCheckGroup::GROUP_HYRULE_FIELD);

    // Lake Hylia
    locationTable[RC_LH_GS_BEAN_PATCH] =
        Location::GSToken(RC_LH_GS_BEAN_PATCH, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_LAKE_HYLIA, ACTOR_EN_SI,
                          SCENE_SPOT06, 29185, 0x01, "GS Bean Patch", "LH GS Bean Patch", RC_LH_GS_BEAN_PATCH,
                          {
                              Category::cLakeHylia,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);
    locationTable[RC_LH_GS_SMALL_ISLAND] =
        Location::GSToken(RC_LH_GS_SMALL_ISLAND, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_LAKE_HYLIA, ACTOR_EN_SI,
                          SCENE_SPOT06, 20994, 0x02, "GS Small Island", "LH GS Small Island", RC_LH_GS_SMALL_ISLAND,
                          {
                              Category::cLakeHylia,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);
    locationTable[RC_LH_GS_LAB_WALL] =
        Location::GSToken(RC_LH_GS_LAB_WALL, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_LAKE_HYLIA, ACTOR_EN_SI,
                          SCENE_SPOT06, 20996, 0x04, "GS Lab Wall", "LH GS Lab Wall", RC_LH_GS_LAB_WALL,
                          {
                              Category::cLakeHylia,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);
    locationTable[RC_LH_GS_LAB_CRATE] =
        Location::GSToken(RC_LH_GS_LAB_CRATE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_LAKE_HYLIA, ACTOR_EN_SI,
                          SCENE_HYLIA_LABO, -28152, 0x08, "GS Lab Crate", "LH GS Lab Crate", RC_LH_GS_LAB_CRATE,
                          {
                              Category::cLakeHylia,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);
    locationTable[RC_LH_GS_TREE] =
        Location::GSToken(RC_LH_GS_TREE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_LAKE_HYLIA, ACTOR_EN_SI, SCENE_SPOT06,
                          21008, 0x10, "GS Tree", "LH GS Tree", RC_LH_GS_TREE,
                          {
                              Category::cLakeHylia,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_LAKE_HYLIA);

    // Gerudo Valley
    locationTable[RC_GV_GS_BEAN_PATCH] =
        Location::GSToken(RC_GV_GS_BEAN_PATCH, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_GERUDO_VALLEY, ACTOR_EN_SI,
                          SCENE_SPOT09, 29441, 0x01, "GS Bean Patch", "GV GS Bean Patch", RC_GV_GS_BEAN_PATCH,
                          {
                              Category::cGerudoValley,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GV_GS_SMALL_BRIDGE] =
        Location::GSToken(RC_GV_GS_SMALL_BRIDGE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_GERUDO_VALLEY, ACTOR_EN_SI,
                          SCENE_SPOT09, 21250, 0x02, "GS Small Bridge", "GV GS Small Bridge", RC_GV_GS_SMALL_BRIDGE,
                          {
                              Category::cGerudoValley,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GV_GS_PILLAR] =
        Location::GSToken(RC_GV_GS_PILLAR, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_GERUDO_VALLEY, ACTOR_EN_SI,
                          SCENE_SPOT09, 21252, 0x04, "GS Pillar", "GV GS Pillar", RC_GV_GS_PILLAR,
                          {
                              Category::cGerudoValley,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GV_GS_BEHIND_TENT] =
        Location::GSToken(RC_GV_GS_BEHIND_TENT, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_GERUDO_VALLEY, ACTOR_EN_SI,
                          SCENE_SPOT09, 21256, 0x08, "GS Behind Tent", "GV GS Behind Tent", RC_GV_GS_BEHIND_TENT,
                          {
                              Category::cGerudoValley,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);

    // Gerudo Fortress
    locationTable[RC_GF_GS_ARCHERY_RANGE] =
        Location::GSToken(RC_GF_GS_ARCHERY_RANGE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_GERUDO_FORTRESS, ACTOR_EN_SI,
                          SCENE_SPOT12, 21505, 0x01, "GS Archery Range", "GF GS Archery Range", RC_GF_GS_ARCHERY_RANGE,
                          {
                              Category::cGerudoFortress,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_GF_GS_TOP_FLOOR] =
        Location::GSToken(RC_GF_GS_TOP_FLOOR, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_GERUDO_FORTRESS, ACTOR_EN_SI,
                          SCENE_SPOT12, 21506, 0x02, "GS Top Floor", "GF GS Top Floor", RC_GF_GS_TOP_FLOOR,
                          {
                              Category::cGerudoFortress,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);

    // Wasteland & Desert Colossus
    locationTable[RC_WASTELAND_GS] =
        Location::GSToken(RC_WASTELAND_GS, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_WASTELAND, ACTOR_EN_SI,
                          SCENE_SPOT13, 13570, 0x02, "GS", "Wasteland GS", RC_WASTELAND_GS,
                          {
                              Category::cHauntedWasteland,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_COLOSSUS_GS_BEAN_PATCH] = Location::GSToken(
        RC_COLOSSUS_GS_BEAN_PATCH, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_DESERT_COLOSSUS, ACTOR_EN_SI, SCENE_SPOT11,
        29953, 0x01, "GS Bean Patch", "Colossus GS Bean Patch", RC_COLOSSUS_GS_BEAN_PATCH,
        {
            Category::cDesertColossus,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_COLOSSUS_GS_HILL] =
        Location::GSToken(RC_COLOSSUS_GS_HILL, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_DESERT_COLOSSUS, ACTOR_EN_SI,
                          SCENE_SPOT11, 21764, 0x04, "GS Hill", "Colossus GS Hill", RC_COLOSSUS_GS_HILL,
                          {
                              Category::cDesertColossus,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);
    locationTable[RC_COLOSSUS_GS_TREE] =
        Location::GSToken(RC_COLOSSUS_GS_TREE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_DESERT_COLOSSUS, ACTOR_EN_SI,
                          SCENE_SPOT11, 21768, 0x08, "GS Tree", "Colossus GS Tree", RC_COLOSSUS_GS_TREE,
                          {
                              Category::cDesertColossus,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_GERUDO_VALLEY);

    // Hyrule Castle, Market, and Outside Ganon's Castle
    locationTable[RC_OGC_GS] =
        Location::GSToken(RC_OGC_GS, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_HYRULE_CASTLE, ACTOR_EN_SI,
                          SCENE_GANON_TOU, 11777, 0x01, "OGC GS", "OGC GS", RC_OGC_GS,
                          {
                              Category::cOutsideGanonsCastle,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
    locationTable[RC_HC_GS_STORMS_GROTTO] =
        Location::GSToken(RC_HC_GS_STORMS_GROTTO, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_HYRULE_CASTLE, ACTOR_EN_SI,
                          SCENE_KAKUSIANA, 11778, 0x02, "GS Storms Grotto", "HC GS Storms Grotto",
                          RC_HC_GS_STORMS_GROTTO, { Category::cHyruleCastle, Category::cSkulltula, Category::cGrotto },
                          SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);
    locationTable[RC_HC_GS_TREE] =
        Location::GSToken(RC_HC_GS_TREE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_HYRULE_CASTLE, ACTOR_EN_SI,
                          SCENE_SPOT15, -29180, 0x04, "GS Tree", "HC GS Tree", RC_HC_GS_TREE,
                          {
                              Category::cHyruleCastle,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);
    locationTable[RC_MARKET_GS_GUARD_HOUSE] = Location::GSToken(
        RC_MARKET_GS_GUARD_HOUSE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_MARKET, ACTOR_EN_SI, SCENE_MIHARIGOYA,
        -29176, 0x08, "Market GS Guard House", "Market GS Guard House", RC_MARKET_GS_GUARD_HOUSE,
        {
            Category::cInnerMarket,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_HYRULE_CASTLE);

    // Kakariko
    locationTable[RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION] =
        Location::GSToken(RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_KAKARIKO_VILLAGE,
                          ACTOR_EN_SI, SCENE_SPOT01, 20488, 0x08, "GS House Under Construction",
                          "Kak GS House Under Construction", RC_KAK_GS_HOUSE_UNDER_CONSTRUCTION,
                          {
                              Category::cKakarikoVillage,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_GS_SKULLTULA_HOUSE] = Location::GSToken(
        RC_KAK_GS_SKULLTULA_HOUSE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_KAKARIKO_VILLAGE, ACTOR_EN_SI, SCENE_SPOT01,
        20496, 0x10, "GS Skulltula House", "Kak GS Skulltula House", RC_KAK_GS_SKULLTULA_HOUSE,
        {
            Category::cKakarikoVillage,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_GS_GUARDS_HOUSE] = Location::GSToken(
        RC_KAK_GS_GUARDS_HOUSE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_KAKARIKO_VILLAGE, ACTOR_EN_SI, SCENE_SPOT01,
        20482, 0x02, "GS Guards House", "Kak GS Guards House", RC_KAK_GS_GUARDS_HOUSE,
        {
            Category::cKakarikoVillage,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_GS_TREE] =
        Location::GSToken(RC_KAK_GS_TREE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_KAKARIKO_VILLAGE, ACTOR_EN_SI,
                          SCENE_SPOT01, -28640, 0x20, "GS Tree", "Kak GS Tree", RC_KAK_GS_TREE,
                          {
                              Category::cKakarikoVillage,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_GS_WATCHTOWER] =
        Location::GSToken(RC_KAK_GS_WATCHTOWER, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_KAKARIKO_VILLAGE, ACTOR_EN_SI,
                          SCENE_SPOT01, 20484, 0x04, "GS Watchtower", "Kak GS Watchtower", RC_KAK_GS_WATCHTOWER,
                          {
                              Category::cKakarikoVillage,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_KAK_GS_ABOVE_IMPAS_HOUSE] = Location::GSToken(
        RC_KAK_GS_ABOVE_IMPAS_HOUSE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_KAKARIKO_VILLAGE, ACTOR_EN_SI,
        SCENE_SPOT01, 20544, 0x40, "GS Above Impas House", "Kak GS Above Impas House", RC_KAK_GS_ABOVE_IMPAS_HOUSE,
        {
            Category::cKakarikoVillage,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_KAKARIKO);

    // Graveyard
    locationTable[RC_GRAVEYARD_GS_WALL] =
        Location::GSToken(RC_GRAVEYARD_GS_WALL, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_GRAVEYARD, ACTOR_EN_SI,
                          SCENE_SPOT02, 20608, 0x80, "GS Wall", "Graveyard GS Wall", RC_GRAVEYARD_GS_WALL,
                          {
                              Category::cGraveyard,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_KAKARIKO);
    locationTable[RC_GRAVEYARD_GS_BEAN_PATCH] = Location::GSToken(
        RC_GRAVEYARD_GS_BEAN_PATCH, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_GRAVEYARD, ACTOR_EN_SI, SCENE_SPOT02,
        28673, 0x01, "GS Bean Patch", "Graveyard GS Bean Patch", RC_GRAVEYARD_GS_BEAN_PATCH,
        {
            Category::cGraveyard,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_KAKARIKO);

    // Death Mountain
    locationTable[RC_DMC_GS_BEAN_PATCH] = Location::GSToken(
        RC_DMC_GS_BEAN_PATCH, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_DEATH_MOUNTAIN_CRATER, ACTOR_EN_SI, SCENE_SPOT17,
        28417, 0x01, "GS Bean Patch", "DMC GS Bean Patch", RC_DMC_GS_BEAN_PATCH,
        {
            Category::cDeathMountainCrater,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMC_GS_CRATE] =
        Location::GSToken(RC_DMC_GS_CRATE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_DEATH_MOUNTAIN_CRATER, ACTOR_EN_SI,
                          SCENE_SPOT17, -28800, 0x80, "GS Crate", "DMC GS Crate", RC_DMC_GS_CRATE,
                          {
                              Category::cDeathMountainCrater,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMT_GS_BEAN_PATCH] = Location::GSToken(
        RC_DMT_GS_BEAN_PATCH, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_DEATH_MOUNTAIN_TRAIL, ACTOR_EN_SI, SCENE_SPOT16,
        28418, 0x02, "GS Bean Patch", "DMT GS Bean Patch", RC_DMT_GS_BEAN_PATCH,
        {
            Category::cDeathMountainTrail,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMT_GS_NEAR_KAK] =
        Location::GSToken(RC_DMT_GS_NEAR_KAK, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_DEATH_MOUNTAIN_TRAIL,
                          ACTOR_EN_SI, SCENE_SPOT16, 12036, 0x04, "GS Near Kak", "DMT GS Near Kak", RC_DMT_GS_NEAR_KAK,
                          {
                              Category::cDeathMountainTrail,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMT_GS_ABOVE_DODONGOS_CAVERN] =
        Location::GSToken(RC_DMT_GS_ABOVE_DODONGOS_CAVERN, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN,
                          RCAREA_DEATH_MOUNTAIN_TRAIL, ACTOR_EN_SI, SCENE_SPOT16, 20232, 0x08,
                          "GS Above Dodongos Cavern", "DMT GS Above Dodongos Cavern", RC_DMT_GS_ABOVE_DODONGOS_CAVERN,
                          {
                              Category::cDeathMountainTrail,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_DMT_GS_FALLING_ROCKS_PATH] = Location::GSToken(
        RC_DMT_GS_FALLING_ROCKS_PATH, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_DEATH_MOUNTAIN_TRAIL, ACTOR_EN_SI,
        SCENE_SPOT16, 20240, 0x10, "GS Falling Rocks Path", "DMT GS Falling Rocks Path", RC_DMT_GS_FALLING_ROCKS_PATH,
        {
            Category::cDeathMountainTrail,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_DEATH_MOUNTAIN);
    locationTable[RC_GC_GS_CENTER_PLATFORM] = Location::GSToken(
        RC_GC_GS_CENTER_PLATFORM, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_GORON_CITY, ACTOR_EN_SI, SCENE_SPOT18, 12064,
        0x20, "GS Center Platform", "GC GS Center Platform", RC_GC_GS_CENTER_PLATFORM,
        {
            Category::cGoronCity,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_GORON_CITY);
    locationTable[RC_GC_GS_BOULDER_MAZE] =
        Location::GSToken(RC_GC_GS_BOULDER_MAZE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_GORON_CITY, ACTOR_EN_SI,
                          SCENE_SPOT18, -28864, 0x40, "GS Boulder Maze", "GC GS Boulder Maze", RC_GC_GS_BOULDER_MAZE,
                          {
                              Category::cGoronCity,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_GORON_CITY);

    // Zora's River, Domain, and Fountain
    locationTable[RC_ZR_GS_LADDER] =
        Location::GSToken(RC_ZR_GS_LADDER, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_ZORAS_RIVER, ACTOR_EN_SI,
                          SCENE_SPOT03, 20737, 0x01, "GS Ladder", "ZR GS Ladder", RC_ZR_GS_LADDER,
                          {
                              Category::cZorasRiver,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_GS_TREE] =
        Location::GSToken(RC_ZR_GS_TREE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_ZORAS_RIVER, ACTOR_EN_SI,
                          SCENE_SPOT03, -28414, 0x02, "GS Tree", "ZR GS Tree", RC_ZR_GS_TREE,
                          {
                              Category::cZorasRiver,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_GS_ABOVE_BRIDGE] =
        Location::GSToken(RC_ZR_GS_ABOVE_BRIDGE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_ZORAS_RIVER, ACTOR_EN_SI,
                          SCENE_SPOT03, 20744, 0x10, "GS Above Bridge", "ZR GS Above Bridge", RC_ZR_GS_ABOVE_BRIDGE,
                          {
                              Category::cZorasRiver,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZR_GS_NEAR_RAISED_GROTTOS] = Location::GSToken(
        RC_ZR_GS_NEAR_RAISED_GROTTOS, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_ZORAS_RIVER, ACTOR_EN_SI, SCENE_SPOT03,
        20752, 0x10, "GS Near Raised Grottos", "ZR GS Near Raised Grottos", RC_ZR_GS_NEAR_RAISED_GROTTOS,
        {
            Category::cZorasRiver,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_ZORAS_RIVER);
    locationTable[RC_ZD_GS_FROZEN_WATERFALL] = Location::GSToken(
        RC_ZD_GS_FROZEN_WATERFALL, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_ZORAS_DOMAIN, ACTOR_EN_SI, SCENE_SPOT07,
        20800, 0x40, "GS Frozen Waterfall", "ZD GS Frozen Waterfall", RC_ZD_GS_FROZEN_WATERFALL,
        {
            Category::cZorasDomain,
            Category::cSkulltula,
        },
        SpoilerCollectionCheckGroup::GROUP_ZORAS_DOMAIN);
    locationTable[RC_ZF_GS_ABOVE_THE_LOG] =
        Location::GSToken(RC_ZF_GS_ABOVE_THE_LOG, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_ZORAS_FOUNTAIN, ACTOR_EN_SI,
                          SCENE_SPOT08, 20740, 0x04, "GS Above The Log", "ZF GS Above The Log", RC_ZF_GS_ABOVE_THE_LOG,
                          {
                              Category::cZorasFountain,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_ZORAS_DOMAIN);
    locationTable[RC_ZF_GS_HIDDEN_CAVE] =
        Location::GSToken(RC_ZF_GS_HIDDEN_CAVE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_ZORAS_FOUNTAIN, ACTOR_EN_SI,
                          SCENE_SPOT08, 20768, 0x20, "GS Hidden Cave", "ZF GS Hidden Cave", RC_ZF_GS_HIDDEN_CAVE,
                          {
                              Category::cZorasFountain,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_ZORAS_DOMAIN);

    // Lon Lon Ranch
    locationTable[RC_LLR_GS_BACK_WALL] =
        Location::GSToken(RC_LLR_GS_BACK_WALL, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_LON_LON_RANCH, ACTOR_EN_SI,
                          SCENE_SPOT20, 11009, 0x01, "GS Back Wall", "LLR GS Back Wall", RC_LLR_GS_BACK_WALL,
                          {
                              Category::cLonLonRanch,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_LON_LON_RANCH);
    locationTable[RC_LLR_GS_RAIN_SHED] =
        Location::GSToken(RC_LLR_GS_RAIN_SHED, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_LON_LON_RANCH, ACTOR_EN_SI,
                          SCENE_SPOT20, 11010, 0x02, "GS Rain Shed", "LLR GS Rain Shed", RC_LLR_GS_RAIN_SHED,
                          {
                              Category::cLonLonRanch,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_LON_LON_RANCH);
    locationTable[RC_LLR_GS_HOUSE_WINDOW] =
        Location::GSToken(RC_LLR_GS_HOUSE_WINDOW, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_LON_LON_RANCH, ACTOR_EN_SI,
                          SCENE_SPOT20, 11012, 0x04, "GS House Window", "LLR GS House Window", RC_LLR_GS_HOUSE_WINDOW,
                          {
                              Category::cLonLonRanch,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_LON_LON_RANCH);
    locationTable[RC_LLR_GS_TREE] =
        Location::GSToken(RC_LLR_GS_TREE, RCQUEST_BOTH, RCTYPE_SKULL_TOKEN, RCAREA_LON_LON_RANCH, ACTOR_EN_SI,
                          SCENE_SPOT20, -29944, 0x08, "GS Tree", "LLR GS Tree", RC_LLR_GS_TREE,
                          {
                              Category::cLonLonRanch,
                              Category::cSkulltula,
                          },
                          SpoilerCollectionCheckGroup::GROUP_LON_LON_RANCH);

    // Bosses
    locationTable[RC_LINKS_POCKET] = Location::Reward(
        RC_LINKS_POCKET, RCQUEST_BOTH, RCTYPE_LINKS_POCKET, RCAREA_KOKIRI_FOREST, ACTOR_ID_MAX, SCENE_ID_MAX, 0x00,
        0x00, "Link's Pocket", "Link's Pocket", RC_LINKS_POCKET, RG_LIGHT_MEDALLION, {},
        SpoilerCollectionCheck::AlwaysCollected(), SpoilerCollectionCheckGroup::GROUP_KOKIRI_FOREST);
    locationTable[RC_QUEEN_GOHMA] = Location::Reward(
        RC_QUEEN_GOHMA, RCQUEST_BOTH, RCTYPE_DUNGEON_REWARD, RCAREA_DEKU_TREE, ACTOR_DOOR_WARP1, SCENE_YDAN_BOSS, 0x00,
        DungeonId::DUNGEON_DEKU_TREE, "Queen Gohma", "Queen Gohma", RC_QUEEN_GOHMA, RG_KOKIRI_EMERALD, {},
        SpoilerCollectionCheck::Chest(0x11, 0x1F), SpoilerCollectionCheckGroup::GROUP_DUNGEON_DEKU_TREE);
    locationTable[RC_KING_DODONGO] = Location::Reward(
        RC_KING_DODONGO, RCQUEST_BOTH, RCTYPE_DUNGEON_REWARD, RCAREA_DODONGOS_CAVERN, ACTOR_DOOR_WARP1, SCENE_DDAN_BOSS,
        0x00, DungeonId::DUNGEON_DODONGOS_CAVERN, "King Dodongo", "King Dodongo", RC_KING_DODONGO, RG_GORON_RUBY, {},
        SpoilerCollectionCheck::Chest(0x12, 0x1F), SpoilerCollectionCheckGroup::GROUP_DUNGEON_DODONGOS_CAVERN);
    locationTable[RC_BARINADE] = Location::Reward(
        RC_BARINADE, RCQUEST_BOTH, RCTYPE_DUNGEON_REWARD, RCAREA_JABU_JABUS_BELLY, ACTOR_DOOR_WARP1, SCENE_BDAN_BOSS,
        0x00, DungeonId::DUNGEON_JABUJABUS_BELLY, "Barinade", "Barinade", RC_BARINADE, RG_ZORA_SAPPHIRE, {},
        SpoilerCollectionCheck::Chest(0x13, 0x1F), SpoilerCollectionCheckGroup::GROUP_DUNGEON_JABUJABUS_BELLY);
    locationTable[RC_PHANTOM_GANON] =
        Location::Reward(RC_PHANTOM_GANON, RCQUEST_BOTH, RCTYPE_DUNGEON_REWARD, RCAREA_FOREST_TEMPLE, ACTOR_DOOR_WARP1,
                         SCENE_MORIBOSSROOM, 0x00, DungeonId::DUNGEON_FOREST_TEMPLE, "Phantom Ganon", "Phantom Ganon",
                         RC_PHANTOM_GANON, RG_FOREST_MEDALLION, {}, SpoilerCollectionCheck::Chest(0x14, 0x1F),
                         SpoilerCollectionCheckGroup::GROUP_DUNGEON_FOREST_TEMPLE);
    locationTable[RC_VOLVAGIA] = Location::Reward(
        RC_VOLVAGIA, RCQUEST_BOTH, RCTYPE_DUNGEON_REWARD, RCAREA_FIRE_TEMPLE, ACTOR_DOOR_WARP1, SCENE_FIRE_BS, 0x00,
        DungeonId::DUNGEON_FIRE_TEMPLE, "Volvagia", "Volvagia", RC_VOLVAGIA, RG_FIRE_MEDALLION, {},
        SpoilerCollectionCheck::Chest(0x15, 0x1F), SpoilerCollectionCheckGroup::GROUP_DUNGEON_FIRE_TEMPLE);
    locationTable[RC_MORPHA] = Location::Reward(
        RC_MORPHA, RCQUEST_BOTH, RCTYPE_DUNGEON_REWARD, RCAREA_WATER_TEMPLE, ACTOR_DOOR_WARP1, SCENE_MIZUSIN_BS, 0x00,
        DungeonId::DUNGEON_WATER_TEMPLE, "Morpha", "Morpha", RC_MORPHA, RG_WATER_MEDALLION, {},
        SpoilerCollectionCheck::Chest(0x16, 0x1F), SpoilerCollectionCheckGroup::GROUP_DUNGEON_WATER_TEMPLE);
    locationTable[RC_TWINROVA] = Location::Reward(
        RC_TWINROVA, RCQUEST_BOTH, RCTYPE_DUNGEON_REWARD, RCAREA_SPIRIT_TEMPLE, ACTOR_DOOR_WARP1, SCENE_JYASINBOSS,
        0x00, DungeonId::DUNGEON_SPIRIT_TEMPLE, "Twinrova", "Twinrova", RC_TWINROVA, RG_SPIRIT_MEDALLION, {},
        SpoilerCollectionCheck::Chest(0x17, 0x1F), SpoilerCollectionCheckGroup::GROUP_DUNGEON_SPIRIT_TEMPLE);
    locationTable[RC_BONGO_BONGO] = Location::Reward(
        RC_BONGO_BONGO, RCQUEST_BOTH, RCTYPE_DUNGEON_REWARD, RCAREA_SHADOW_TEMPLE, ACTOR_DOOR_WARP1, SCENE_HAKADAN_BS,
        0x00, DungeonId::DUNGEON_SHADOW_TEMPLE, "Bongo Bongo", "Bongo Bongo", RC_BONGO_BONGO, RG_SHADOW_MEDALLION, {},
        SpoilerCollectionCheck::Chest(0x18, 0x1F), SpoilerCollectionCheckGroup::GROUP_DUNGEON_SHADOW_TEMPLE);
    locationTable[RC_GANON] = Location::Reward(
        RC_GANON, RCQUEST_BOTH, RCTYPE_DUNGEON_REWARD, RCAREA_GANONS_CASTLE, ACTOR_DOOR_WARP1, SCENE_GANON_BOSS, 0x00,
        DungeonId::DUNGEON_GANONS_CASTLE_CRUMBLING, "Ganon", "Ganon", RC_GANON, RG_TRIFORCE, {},
        SpoilerCollectionCheck::None(), SpoilerCollectionCheckGroup::GROUP_DUNGEON_GANONS_CASTLE);
}