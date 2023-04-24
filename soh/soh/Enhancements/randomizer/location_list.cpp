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
}