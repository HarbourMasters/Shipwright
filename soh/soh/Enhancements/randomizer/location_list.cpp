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
}