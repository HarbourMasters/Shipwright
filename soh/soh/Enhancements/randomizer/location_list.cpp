#include "static_data.h"

#define TWO_ACTOR_PARAMS(a, b) (abs(a) << 16) | abs(b)

std::array<Rando::Location, RC_MAX> StaticData::locationTable;

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
}