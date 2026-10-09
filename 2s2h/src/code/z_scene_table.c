#include "global.h"

#if 0
#define DEFINE_SCENE(name, _enumValue, textId, drawConfig, _restrictionFlags, _persistentCycleFlags) \
    DECLARE_ROM_SEGMENT(name)

#define DEFINE_SCENE_UNSET(_enumValue)

#include "tables/scene_table.h"

#undef DEFINE_SCENE
#undef DEFINE_SCENE_UNSET
#endif

// 2S2H Added columns to scene table: entranceSceneId, betterMapSelectIndex, humanName
#define DEFINE_SCENE(name, _enumValue, textId, drawConfig, _restrictionFlags, _persistentCycleFlags, _entranceSceneId, \
                     _betterMapSelectIndex, _humanName)                                                                \
    { { 0, 0, #name }, textId, 0, drawConfig, 0, 0 },

#define DEFINE_SCENE_UNSET(_enumValue) { 0 },

SceneTableEntry gSceneTable[SCENE_MAX] = {
#include "tables/scene_table.h"
};

#undef DEFINE_SCENE
#undef DEFINE_SCENE_UNSET

static EntranceTableEntry sMayorsResidenceEntrance0[] = {
    { SCENE_SONCHONOIE, 0, 0x4102 },
};

static EntranceTableEntry sMayorsResidenceEntrance1[] = {
    { SCENE_SONCHONOIE, 1, 0x4102 },
};

static EntranceTableEntry sMayorsResidenceEntrance2[] = {
    { SCENE_SONCHONOIE, 2, 0x4102 },
};

static EntranceTableEntry* sMayorsResidenceEntranceTable[] = {
    sMayorsResidenceEntrance0,
    sMayorsResidenceEntrance1,
    sMayorsResidenceEntrance2,
};

static EntranceTableEntry sMajorasLairEntrance0[] = {
    { -SCENE_LAST_BS, 0, 0x0387 },
};

static EntranceTableEntry* sMajorasLairEntranceTable[] = {
    sMajorasLairEntrance0,
};

static EntranceTableEntry sMagicHagsPotionShopEntrance0[] = {
    { SCENE_WITCH_SHOP, 0, 0x4102 },
};

static EntranceTableEntry* sMagicHagsPotionShopEntranceTable[] = {
    sMagicHagsPotionShopEntrance0,
};

static EntranceTableEntry sRanchHouseEntrance0[] = {
    { SCENE_OMOYA, 0, 0x0102 },
};

static EntranceTableEntry sRanchHouseEntrance1[] = {
    { SCENE_OMOYA, 1, 0x4102 },
};

static EntranceTableEntry* sRanchHouseEntranceTable[] = {
    sRanchHouseEntrance0,
    sRanchHouseEntrance1,
};

static EntranceTableEntry sHoneyAndDarlingsShopEntrance0[] = {
    { SCENE_BOWLING, 0, 0x4102 },
};

static EntranceTableEntry* sHoneyAndDarlingsShopEntranceTable[] = {
    sHoneyAndDarlingsShopEntrance0,
};

static EntranceTableEntry sBeneathTheGraveryardEntrance0[] = {
    { -SCENE_HAKASHITA, 0, 0x4102 },
};

static EntranceTableEntry sBeneathTheGraveryardEntrance1[] = {
    { -SCENE_HAKASHITA, 1, 0x4102 },
};

static EntranceTableEntry* sBeneathTheGraveryardEntranceTable[] = {
    sBeneathTheGraveryardEntrance0,
    sBeneathTheGraveryardEntrance1,
};

static EntranceTableEntry sSouthernSwampClearedEntrance0[] = {
    { SCENE_20SICHITAI2, 0, 0xCA14 },
};

static EntranceTableEntry sSouthernSwampClearedEntrance1[] = {
    { SCENE_20SICHITAI2, 1, 0x4102 },
};

static EntranceTableEntry sSouthernSwampClearedEntrance2[] = {
    { SCENE_20SICHITAI2, 2, 0xC102 },
};

static EntranceTableEntry sSouthernSwampClearedEntrance3[] = {
    { SCENE_20SICHITAI2, 3, 0x4102 },
};

static EntranceTableEntry sSouthernSwampClearedEntrance4[] = {
    { SCENE_20SICHITAI2, 4, 0x4102 },
};

static EntranceTableEntry sSouthernSwampClearedEntrance5[] = {
    { SCENE_20SICHITAI2, 5, 0x4102 },
};

static EntranceTableEntry sSouthernSwampClearedEntrance6[] = {
    { SCENE_20SICHITAI2, 6, 0x0102 },
};

static EntranceTableEntry sSouthernSwampClearedEntrance7[] = {
    { SCENE_20SICHITAI2, 7, 0x4102 },
};

static EntranceTableEntry sSouthernSwampClearedEntrance8[] = {
    { SCENE_20SICHITAI2, 8, 0x4102 },
};

static EntranceTableEntry sSouthernSwampClearedEntrance9[] = {
    { SCENE_20SICHITAI2, 9, 0x4A14 },
};

static EntranceTableEntry sSouthernSwampClearedEntrance10[] = {
    { SCENE_20SICHITAI2, 10, 0x4A14 },
};

static EntranceTableEntry* sSouthernSwampClearedEntranceTable[] = {
    sSouthernSwampClearedEntrance0, sSouthernSwampClearedEntrance1,  sSouthernSwampClearedEntrance2,
    sSouthernSwampClearedEntrance3, sSouthernSwampClearedEntrance4,  sSouthernSwampClearedEntrance5,
    sSouthernSwampClearedEntrance6, sSouthernSwampClearedEntrance7,  sSouthernSwampClearedEntrance8,
    sSouthernSwampClearedEntrance9, sSouthernSwampClearedEntrance10,
};

static EntranceTableEntry sCuriosityShopEntrance0[] = {
    { SCENE_AYASHIISHOP, 0, 0x4102 },
};

static EntranceTableEntry sCuriosityShopEntrance1[] = {
    { SCENE_AYASHIISHOP, 1, 0x0102 },
};

static EntranceTableEntry sCuriosityShopEntrance2[] = {
    { SCENE_AYASHIISHOP, 2, 0x8A95 },
};

static EntranceTableEntry sCuriosityShopEntrance3[] = {
    { SCENE_AYASHIISHOP, 3, 0x8102 },
};

static EntranceTableEntry* sCuriosityShopEntranceTable[] = {
    sCuriosityShopEntrance0,
    sCuriosityShopEntrance1,
    sCuriosityShopEntrance2,
    sCuriosityShopEntrance3,
};

static EntranceTableEntry sIkanaCanyonEntrance0[] = {
    { SCENE_IKANA, 0, 0xCA14 }, { SCENE_IKANA, 0, 0x8A14 }, { SCENE_IKANA, 0, 0x0A14 },
    { SCENE_IKANA, 0, 0x4A14 }, { SCENE_IKANA, 0, 0x8102 },
};

static EntranceTableEntry sIkanaCanyonEntrance1[] = {
    { SCENE_IKANA, 1, 0x4102 },
    { SCENE_IKANA, 1, 0x4102 },
    { SCENE_IKANA, 1, 0xCA14 },
    { SCENE_IKANA, 1, 0x4102 },
};

static EntranceTableEntry sIkanaCanyonEntrance2[] = {
    { SCENE_IKANA, 2, 0x4102 },
    { SCENE_IKANA, 2, 0x4102 },
    { SCENE_IKANA, 2, 0x4102 },
    { SCENE_IKANA, 2, 0x4102 },
};

static EntranceTableEntry sIkanaCanyonEntrance3[] = {
    { SCENE_IKANA, 3, 0x4102 },
    { SCENE_IKANA, 3, 0x4102 },
    { SCENE_IKANA, 3, 0x4102 },
    { SCENE_IKANA, 3, 0x4102 },
};

static EntranceTableEntry sIkanaCanyonEntrance4[] = {
    { SCENE_IKANA, 4, 0x4A14 },
    { SCENE_IKANA, 4, 0x4A14 },
    { SCENE_IKANA, 4, 0x4A14 },
    { SCENE_IKANA, 4, 0x4A14 },
};

static EntranceTableEntry sIkanaCanyonEntrance5[] = {
    { SCENE_IKANA, 5, 0x4102 },
    { SCENE_IKANA, 5, 0x4102 },
    { SCENE_IKANA, 5, 0x4102 },
    { SCENE_IKANA, 5, 0x4102 },
};

static EntranceTableEntry sIkanaCanyonEntrance6[] = {
    { SCENE_IKANA, 6, 0x4102 },
    { SCENE_IKANA, 6, 0x4102 },
    { SCENE_IKANA, 6, 0x4102 },
    { SCENE_IKANA, 6, 0x4102 },
};

static EntranceTableEntry sIkanaCanyonEntrance7[] = {
    { SCENE_IKANA, 7, 0x4A14 },
    { SCENE_IKANA, 7, 0x4A14 },
    { SCENE_IKANA, 7, 0x4A14 },
    { SCENE_IKANA, 7, 0x4A14 },
};

static EntranceTableEntry sIkanaCanyonEntrance8[] = {
    { SCENE_IKANA, 8, 0x4102 },
    { SCENE_IKANA, 8, 0x4102 },
    { SCENE_IKANA, 8, 0x4102 },
    { SCENE_IKANA, 8, 0x4102 },
};

static EntranceTableEntry sIkanaCanyonEntrance9[] = {
    { SCENE_IKANA, 9, 0x4A14 },
    { SCENE_IKANA, 9, 0x4A14 },
    { SCENE_IKANA, 9, 0x4A14 },
    { SCENE_IKANA, 9, 0x4A14 },
};

static EntranceTableEntry sIkanaCanyonEntrance10[] = {
    { SCENE_IKANA, 10, 0x4A14 },
    { SCENE_IKANA, 10, 0x4A14 },
    { SCENE_IKANA, 10, 0x4A14 },
    { SCENE_IKANA, 10, 0x4A14 },
};

static EntranceTableEntry sIkanaCanyonEntrance11[] = {
    { SCENE_IKANA, 11, 0x4102 },
    { SCENE_IKANA, 11, 0x4102 },
    { SCENE_IKANA, 11, 0x4102 },
    { SCENE_IKANA, 11, 0x4102 },
};

static EntranceTableEntry sIkanaCanyonEntrance12[] = {
    { SCENE_IKANA, 12, 0x4102 },
    { SCENE_IKANA, 12, 0x4102 },
    { SCENE_IKANA, 12, 0x4102 },
    { SCENE_IKANA, 12, 0x4102 },
};

static EntranceTableEntry sIkanaCanyonEntrance13[] = {
    { SCENE_IKANA, 13, 0x8102 },
    { SCENE_IKANA, 13, 0x8102 },
    { SCENE_IKANA, 13, 0x8102 },
    { SCENE_IKANA, 13, 0x8102 },
};

static EntranceTableEntry sIkanaCanyonEntrance14[] = {
    { SCENE_IKANA, 14, 0x8102 },
    { SCENE_IKANA, 14, 0x8102 },
    { SCENE_IKANA, 14, 0x8102 },
    { SCENE_IKANA, 14, 0x8102 },
};

static EntranceTableEntry sIkanaCanyonEntrance15[] = {
    { SCENE_IKANA, 15, 0x8102 },
    { SCENE_IKANA, 15, 0x8102 },
    { SCENE_IKANA, 15, 0x8102 },
    { SCENE_IKANA, 15, 0x0183 },
};

static EntranceTableEntry* sIkanaCanyonEntranceTable[] = {
    sIkanaCanyonEntrance0,  sIkanaCanyonEntrance1,  sIkanaCanyonEntrance2,  sIkanaCanyonEntrance3,
    sIkanaCanyonEntrance4,  sIkanaCanyonEntrance5,  sIkanaCanyonEntrance6,  sIkanaCanyonEntrance7,
    sIkanaCanyonEntrance8,  sIkanaCanyonEntrance9,  sIkanaCanyonEntrance10, sIkanaCanyonEntrance11,
    sIkanaCanyonEntrance12, sIkanaCanyonEntrance13, sIkanaCanyonEntrance14, sIkanaCanyonEntrance15,
};

static EntranceTableEntry sPiratesFortressEntrance0[] = {
    { SCENE_KAIZOKU, 0, 0x8102 },
    { SCENE_KAIZOKU, 0, 0x8102 },
    { SCENE_KAIZOKU, 0, 0x8102 },
};

static EntranceTableEntry sPiratesFortressEntrance1[] = {
    { SCENE_KAIZOKU, 1, 0x8102 },
    { SCENE_KAIZOKU, 1, 0x8102 },
};

static EntranceTableEntry sPiratesFortressEntrance2[] = {
    { SCENE_KAIZOKU, 2, 0x8102 },
};

static EntranceTableEntry sPiratesFortressEntrance3[] = {
    { SCENE_KAIZOKU, 3, 0x8102 },
};

static EntranceTableEntry sPiratesFortressEntrance4[] = {
    { SCENE_KAIZOKU, 4, 0x8102 },
};

static EntranceTableEntry sPiratesFortressEntrance5[] = {
    { SCENE_KAIZOKU, 5, 0x8102 },
};

static EntranceTableEntry sPiratesFortressEntrance6[] = {
    { SCENE_KAIZOKU, 6, 0x8102 },
};

static EntranceTableEntry sPiratesFortressEntrance7[] = {
    { SCENE_KAIZOKU, 7, 0x8102 },
};

static EntranceTableEntry sPiratesFortressEntrance8[] = {
    { SCENE_KAIZOKU, 8, 0x8102 },
};

static EntranceTableEntry sPiratesFortressEntrance9[] = {
    { SCENE_KAIZOKU, 9, 0x8102 },
};

static EntranceTableEntry sPiratesFortressEntrance10[] = {
    { SCENE_KAIZOKU, 10, 0x8A95 },
};

static EntranceTableEntry sPiratesFortressEntrance11[] = {
    { SCENE_KAIZOKU, 11, 0x8102 },
};

static EntranceTableEntry sPiratesFortressEntrance12[] = {
    { SCENE_KAIZOKU, 12, 0x8102 },
};

static EntranceTableEntry sPiratesFortressEntrance13[] = {
    { SCENE_KAIZOKU, 13, 0x8102 },
};

static EntranceTableEntry sPiratesFortressEntrance14[] = {
    { SCENE_KAIZOKU, 14, 0x8102 },
};

static EntranceTableEntry* sPiratesFortressEntranceTable[] = {
    sPiratesFortressEntrance0,  sPiratesFortressEntrance1,  sPiratesFortressEntrance2,  sPiratesFortressEntrance3,
    sPiratesFortressEntrance4,  sPiratesFortressEntrance5,  sPiratesFortressEntrance6,  sPiratesFortressEntrance7,
    sPiratesFortressEntrance8,  sPiratesFortressEntrance9,  sPiratesFortressEntrance10, sPiratesFortressEntrance11,
    sPiratesFortressEntrance12, sPiratesFortressEntrance13, sPiratesFortressEntrance14,
};

static EntranceTableEntry sMilkBarEntrance0[] = {
    { SCENE_MILK_BAR, 0, 0x4102 }, { SCENE_MILK_BAR, 0, 0x8102 }, { SCENE_MILK_BAR, 0, 0x8102 },
    { SCENE_MILK_BAR, 0, 0x8102 }, { SCENE_MILK_BAR, 0, 0x8102 },
};

static EntranceTableEntry* sMilkBarEntranceTable[] = {
    sMilkBarEntrance0,
};

static EntranceTableEntry sStoneTowerTempleEntrance0[] = {
    { -SCENE_INISIE_N, 0, 0xC102 },
};

static EntranceTableEntry sStoneTowerTempleEntrance1[] = {
    { -SCENE_INISIE_N, 1, 0x4102 },
};

static EntranceTableEntry* sStoneTowerTempleEntranceTable[] = {
    sStoneTowerTempleEntrance0,
    sStoneTowerTempleEntrance1,
};

static EntranceTableEntry sTreasureChestShopEntrance0[] = {
    { SCENE_TAKARAYA, 0, 0x4102 },
};

static EntranceTableEntry sTreasureChestShopEntrance1[] = {
    { SCENE_TAKARAYA, 1, 0x0102 },
};

static EntranceTableEntry* sTreasureChestShopEntranceTable[] = {
    sTreasureChestShopEntrance0,
    sTreasureChestShopEntrance1,
};

static EntranceTableEntry sStoneTowerTempleInvertedEntrance0[] = {
    { -SCENE_INISIE_R, 0, 0xC102 },
};

static EntranceTableEntry sStoneTowerTempleInvertedEntrance1[] = {
    { -SCENE_INISIE_R, 1, 0x8102 },
};

static EntranceTableEntry sStoneTowerTempleInvertedEntrance2[] = {
    { -SCENE_INISIE_R, 2, 0x8102 },
};

static EntranceTableEntry* sStoneTowerTempleInvertedEntranceTable[] = {
    sStoneTowerTempleInvertedEntrance0,
    sStoneTowerTempleInvertedEntrance1,
    sStoneTowerTempleInvertedEntrance2,
};

static EntranceTableEntry sClockTowerRooftopEntrance0[] = {
    { -SCENE_OKUJOU, 0, 0x0102 },
    { -SCENE_OKUJOU, 0, 0x8102 },
    { -SCENE_OKUJOU, 0, 0x0102 },
    { -SCENE_OKUJOU, 0, 0x0102 },
};

static EntranceTableEntry sClockTowerRooftopEntrance1[] = {
    { -SCENE_OKUJOU, 1, 0x0102 },
    { -SCENE_OKUJOU, 1, 0x0102 },
    { -SCENE_OKUJOU, 1, 0x0102 },
};

static EntranceTableEntry sClockTowerRooftopEntrance2[] = {
    { -SCENE_OKUJOU, 2, 0x858B },
    { -SCENE_OKUJOU, 2, 0x858B },
    { -SCENE_OKUJOU, 2, 0x8102 },
};

static EntranceTableEntry* sClockTowerRooftopEntranceTable[] = {
    sClockTowerRooftopEntrance0,
    sClockTowerRooftopEntrance1,
    sClockTowerRooftopEntrance2,
};

static EntranceTableEntry sOpeningDungeonEntrance0[] = {
    { -SCENE_OPENINGDAN, 0, 0x0102 },
    { -SCENE_OPENINGDAN, 0, 0x8102 },
};

static EntranceTableEntry sOpeningDungeonEntrance1[] = {
    { -SCENE_OPENINGDAN, 1, 0x0102 },
};

static EntranceTableEntry sOpeningDungeonEntrance2[] = {
    { -SCENE_OPENINGDAN, 2, 0x0102 },
};

static EntranceTableEntry sOpeningDungeonEntrance3[] = {
    { -SCENE_OPENINGDAN, 3, 0x0102 },
};

static EntranceTableEntry sOpeningDungeonEntrance4[] = {
    { -SCENE_OPENINGDAN, 4, 0x058B },
};

static EntranceTableEntry* sOpeningDungeonEntranceTable[] = {
    sOpeningDungeonEntrance0, sOpeningDungeonEntrance1, sOpeningDungeonEntrance2,
    sOpeningDungeonEntrance3, sOpeningDungeonEntrance4,
};

static EntranceTableEntry sWoodfallTempleEntrance0[] = {
    { -SCENE_MITURIN, 0, 0x4102 },
};

static EntranceTableEntry sWoodfallTempleEntrance1[] = {
    { -SCENE_MITURIN, 1, 0x0102 },
};

static EntranceTableEntry sWoodfallTempleEntrance2[] = {
    { -SCENE_MITURIN, 2, 0x0102 },
};

static EntranceTableEntry* sWoodfallTempleEntranceTable[] = {
    sWoodfallTempleEntrance0,
    sWoodfallTempleEntrance1,
    sWoodfallTempleEntrance2,
};

static EntranceTableEntry sPathToMountainVillageEntrance0[] = {
    { SCENE_13HUBUKINOMITI, 0, 0x8A14 },
    { SCENE_13HUBUKINOMITI, 0, 0x8A14 },
};

static EntranceTableEntry sPathToMountainVillageEntrance1[] = {
    { SCENE_13HUBUKINOMITI, 1, 0x8A14 },
    { SCENE_13HUBUKINOMITI, 1, 0x8A14 },
};

static EntranceTableEntry* sPathToMountainVillageEntranceTable[] = {
    sPathToMountainVillageEntrance0,
    sPathToMountainVillageEntrance1,
};

static EntranceTableEntry sIkanaCastleEntrance0[] = {
    { -SCENE_CASTLE, 0, 0x4102 },
};

static EntranceTableEntry sIkanaCastleEntrance1[] = {
    { -SCENE_CASTLE, 1, 0x4102 },
};

static EntranceTableEntry sIkanaCastleEntrance2[] = {
    { -SCENE_CASTLE, 2, 0x8102 },
};

static EntranceTableEntry sIkanaCastleEntrance3[] = {
    { -SCENE_CASTLE, 3, 0x8102 },
};

static EntranceTableEntry sIkanaCastleEntrance4[] = {
    { -SCENE_CASTLE, 4, 0x8102 },
};

static EntranceTableEntry sIkanaCastleEntrance5[] = {
    { -SCENE_CASTLE, 5, 0x8102 },
};

static EntranceTableEntry sIkanaCastleEntrance6[] = {
    { -SCENE_CASTLE, 6, 0x8102 },
};

static EntranceTableEntry* sIkanaCastleEntranceTable[] = {
    sIkanaCastleEntrance0, sIkanaCastleEntrance1, sIkanaCastleEntrance2, sIkanaCastleEntrance3,
    sIkanaCastleEntrance4, sIkanaCastleEntrance5, sIkanaCastleEntrance6,
};

static EntranceTableEntry sDekuScrubPlaygroundEntrance0[] = {
    { SCENE_DEKUTES, 0, 0x0102 },
};

static EntranceTableEntry sDekuScrubPlaygroundEntrance1[] = {
    { SCENE_DEKUTES, 1, 0x0102 },
};

static EntranceTableEntry* sDekuScrubPlaygroundEntranceTable[] = {
    sDekuScrubPlaygroundEntrance0,
    sDekuScrubPlaygroundEntrance1,
};

static EntranceTableEntry sOdolwasLairEntrance0[] = {
    { -SCENE_MITURIN_BS, 0, 0x8102 },
};

static EntranceTableEntry* sOdolwasLairEntranceTable[] = {
    sOdolwasLairEntrance0,
};

static EntranceTableEntry sTownShootingGalleryEntrance0[] = {
    { SCENE_SYATEKI_MIZU, 0, 0x4102 },
};

static EntranceTableEntry sTownShootingGalleryEntrance1[] = {
    { SCENE_SYATEKI_MIZU, 1, 0x0102 },
};

static EntranceTableEntry* sTownShootingGalleryEntranceTable[] = {
    sTownShootingGalleryEntrance0,
    sTownShootingGalleryEntrance1,
};

static EntranceTableEntry sSnowheadTempleEntrance0[] = {
    { -SCENE_HAKUGIN, 0, 0x4102 },
};

static EntranceTableEntry sSnowheadTempleEntrance1[] = {
    { -SCENE_HAKUGIN, 1, 0x4102 },
};

static EntranceTableEntry* sSnowheadTempleEntranceTable[] = {
    sSnowheadTempleEntrance0,
    sSnowheadTempleEntrance1,
};

static EntranceTableEntry sMilkRoadEntrance0[] = {
    { SCENE_ROMANYMAE, 0, 0xCA14 },
};

static EntranceTableEntry sMilkRoadEntrance1[] = {
    { SCENE_ROMANYMAE, 1, 0x4A14 },
};

static EntranceTableEntry sMilkRoadEntrance2[] = {
    { SCENE_ROMANYMAE, 2, 0x4A14 },
};

static EntranceTableEntry sMilkRoadEntrance3[] = {
    { SCENE_ROMANYMAE, 3, 0x4A14 },
};

static EntranceTableEntry sMilkRoadEntrance4[] = {
    { SCENE_ROMANYMAE, 4, 0x4A14 },
};

static EntranceTableEntry sMilkRoadEntrance5[] = {
    { SCENE_ROMANYMAE, 5, 0x0102 },
};

static EntranceTableEntry sMilkRoadEntrance6[] = {
    { SCENE_ROMANYMAE, 6, 0x0102 },
};

static EntranceTableEntry* sMilkRoadEntranceTable[] = {
    sMilkRoadEntrance0, sMilkRoadEntrance1, sMilkRoadEntrance2, sMilkRoadEntrance3,
    sMilkRoadEntrance4, sMilkRoadEntrance5, sMilkRoadEntrance6,
};

static EntranceTableEntry sPiratesFortressInteriorEntrance0[] = {
    { SCENE_PIRATE, 0, 0x8102 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance1[] = {
    { SCENE_PIRATE, 1, 0x8102 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance2[] = {
    { SCENE_PIRATE, 2, 0x8102 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance3[] = {
    { SCENE_PIRATE, 3, 0x8102 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance4[] = {
    { SCENE_PIRATE, 4, 0x8102 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance5[] = {
    { SCENE_PIRATE, 5, 0x8102 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance6[] = {
    { SCENE_PIRATE, 6, 0x8102 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance7[] = {
    { SCENE_PIRATE, 7, 0x8102 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance8[] = {
    { SCENE_PIRATE, 8, 0x8102 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance9[] = {
    { SCENE_PIRATE, 9, 0x8993 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance10[] = {
    { SCENE_PIRATE, 10, 0x8102 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance11[] = {
    { SCENE_PIRATE, 11, 0x8102 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance12[] = {
    { SCENE_PIRATE, 12, 0x8102 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance13[] = {
    { SCENE_PIRATE, 13, 0x8102 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance14[] = {
    { SCENE_PIRATE, 14, 0x8102 },
};

static EntranceTableEntry sPiratesFortressInteriorEntrance15[] = {
    { SCENE_PIRATE, 15, 0x8102 },
};

static EntranceTableEntry* sPiratesFortressInteriorEntranceTable[] = {
    sPiratesFortressInteriorEntrance0,  sPiratesFortressInteriorEntrance1,  sPiratesFortressInteriorEntrance2,
    sPiratesFortressInteriorEntrance3,  sPiratesFortressInteriorEntrance4,  sPiratesFortressInteriorEntrance5,
    sPiratesFortressInteriorEntrance6,  sPiratesFortressInteriorEntrance7,  sPiratesFortressInteriorEntrance8,
    sPiratesFortressInteriorEntrance9,  sPiratesFortressInteriorEntrance10, sPiratesFortressInteriorEntrance11,
    sPiratesFortressInteriorEntrance12, sPiratesFortressInteriorEntrance13, sPiratesFortressInteriorEntrance14,
    sPiratesFortressInteriorEntrance15,
};

static EntranceTableEntry sSwampShootingGalleryEntrance0[] = {
    { SCENE_SYATEKI_MORI, 0, 0x4102 },
};

static EntranceTableEntry* sSwampShootingGalleryEntranceTable[] = {
    sSwampShootingGalleryEntrance0,
};

static EntranceTableEntry sPinnacleRockEntrance0[] = {
    { SCENE_SINKAI, 0, 0x4A14 },
};

static EntranceTableEntry sPinnacleRockEntrance1[] = {
    { SCENE_SINKAI, 1, 0x0A14 },
};

static EntranceTableEntry* sPinnacleRockEntranceTable[] = {
    sPinnacleRockEntrance0,
    sPinnacleRockEntrance1,
};

static EntranceTableEntry sFairyFountainEntrance0[] = {
    { SCENE_YOUSEI_IZUMI, 0, 0x4102 },
    { SCENE_YOUSEI_IZUMI, 0, 0x8102 },
};

static EntranceTableEntry sFairyFountainEntrance1[] = {
    { SCENE_YOUSEI_IZUMI, 1, 0x4102 },
};

static EntranceTableEntry sFairyFountainEntrance2[] = {
    { SCENE_YOUSEI_IZUMI, 2, 0x4102 },
};

static EntranceTableEntry sFairyFountainEntrance3[] = {
    { SCENE_YOUSEI_IZUMI, 3, 0x4102 },
};

static EntranceTableEntry sFairyFountainEntrance4[] = {
    { SCENE_YOUSEI_IZUMI, 4, 0x4102 },
};

static EntranceTableEntry sFairyFountainEntrance5[] = {
    { SCENE_YOUSEI_IZUMI, 5, 0x8183 },
};

static EntranceTableEntry sFairyFountainEntrance6[] = {
    { SCENE_YOUSEI_IZUMI, 6, 0x858B },
};

static EntranceTableEntry sFairyFountainEntrance7[] = {
    { SCENE_YOUSEI_IZUMI, 7, 0x858B },
};

static EntranceTableEntry sFairyFountainEntrance8[] = {
    { SCENE_YOUSEI_IZUMI, 8, 0x858B },
};

static EntranceTableEntry sFairyFountainEntrance9[] = {
    { SCENE_YOUSEI_IZUMI, 9, 0x858B },
};

static EntranceTableEntry* sFairyFountainEntranceTable[] = {
    sFairyFountainEntrance0, sFairyFountainEntrance1, sFairyFountainEntrance2, sFairyFountainEntrance3,
    sFairyFountainEntrance4, sFairyFountainEntrance5, sFairyFountainEntrance6, sFairyFountainEntrance7,
    sFairyFountainEntrance8, sFairyFountainEntrance9,
};

static EntranceTableEntry sSwampSpiderHouseEntrance0[] = {
    { -SCENE_KINSTA1, 0, 0x4102 },
};

static EntranceTableEntry* sSwampSpiderHouseEntranceTable[] = {
    sSwampSpiderHouseEntrance0,
};

static EntranceTableEntry sOceansideSpiderHouseEntrance0[] = {
    { -SCENE_KINDAN2, 0, 0x4102 },
};

static EntranceTableEntry* sOceansideSpiderHouseEntranceTable[] = {
    sOceansideSpiderHouseEntrance0,
};

static EntranceTableEntry sAstralObservatoryEntrance0[] = {
    { SCENE_TENMON_DAI, 0, 0x0102 },
};

static EntranceTableEntry sAstralObservatoryEntrance1[] = {
    { SCENE_TENMON_DAI, 1, 0x4102 },
};

static EntranceTableEntry sAstralObservatoryEntrance2[] = {
    { SCENE_TENMON_DAI, 2, 0x8115 },
};

static EntranceTableEntry* sAstralObservatoryEntranceTable[] = {
    sAstralObservatoryEntrance0,
    sAstralObservatoryEntrance1,
    sAstralObservatoryEntrance2,
};

static EntranceTableEntry sMoonDekuTrialEntrance0[] = {
    { -SCENE_LAST_DEKU, 0, 0x0387 },
};

static EntranceTableEntry* sMoonDekuTrialEntranceTable[] = {
    sMoonDekuTrialEntrance0,
};

static EntranceTableEntry sDekuPalaceEntrance0[] = {
    { SCENE_22DEKUCITY, 0, 0x4102 },
};

static EntranceTableEntry sDekuPalaceEntrance1[] = {
    { SCENE_22DEKUCITY, 1, 0x8A14 },
};

static EntranceTableEntry sDekuPalaceEntrance2[] = {
    { SCENE_22DEKUCITY, 2, 0xC102 },
};

static EntranceTableEntry sDekuPalaceEntrance3[] = {
    { SCENE_22DEKUCITY, 3, 0xC102 },
};

static EntranceTableEntry sDekuPalaceEntrance4[] = {
    { SCENE_22DEKUCITY, 4, 0x4102 },
};

static EntranceTableEntry sDekuPalaceEntrance5[] = {
    { SCENE_22DEKUCITY, 5, 0x4102 },
};

static EntranceTableEntry sDekuPalaceEntrance6[] = {
    { SCENE_22DEKUCITY, 6, 0x4102 },
};

static EntranceTableEntry sDekuPalaceEntrance7[] = {
    { SCENE_22DEKUCITY, 7, 0x4102 },
};

static EntranceTableEntry sDekuPalaceEntrance8[] = {
    { SCENE_22DEKUCITY, 8, 0x4102 },
};

static EntranceTableEntry sDekuPalaceEntrance9[] = {
    { SCENE_22DEKUCITY, 9, 0x4102 },
};

static EntranceTableEntry sDekuPalaceEntrance10[] = {
    { SCENE_22DEKUCITY, 10, 0x4102 },
};

static EntranceTableEntry* sDekuPalaceEntranceTable[] = {
    sDekuPalaceEntrance0, sDekuPalaceEntrance1, sDekuPalaceEntrance2,  sDekuPalaceEntrance3,
    sDekuPalaceEntrance4, sDekuPalaceEntrance5, sDekuPalaceEntrance6,  sDekuPalaceEntrance7,
    sDekuPalaceEntrance8, sDekuPalaceEntrance9, sDekuPalaceEntrance10,
};

static EntranceTableEntry sMountainSmithyEntrance0[] = {
    { SCENE_KAJIYA, 0, 0x4102 },
};

static EntranceTableEntry* sMountainSmithyEntranceTable[] = {
    sMountainSmithyEntrance0,
};

static EntranceTableEntry sTerminaFieldEntrance0[] = {
    { SCENE_00KEIKOKU, 0, 0x4102 },  { -SCENE_00KEIKOKU, 0, 0x0A14 }, { -SCENE_00KEIKOKU, 0, 0x0A14 },
    { -SCENE_00KEIKOKU, 0, 0x058B }, { SCENE_00KEIKOKU, 0, 0x0A14 },  { SCENE_00KEIKOKU, 0, 0x0A14 },
    { SCENE_00KEIKOKU, 0, 0x058B },  { SCENE_00KEIKOKU, 0, 0x8102 },  { SCENE_00KEIKOKU, 0, 0x0102 },
    { SCENE_00KEIKOKU, 0, 0x8102 },
};

static EntranceTableEntry sTerminaFieldEntrance1[] = {
    { SCENE_00KEIKOKU, 1, 0xCA14 },  { SCENE_00KEIKOKU, 1, 0x0A14 }, { SCENE_00KEIKOKU, 1, 0x058B },
    { -SCENE_00KEIKOKU, 1, 0x858B }, { SCENE_00KEIKOKU, 1, 0x0A14 }, { SCENE_00KEIKOKU, 1, 0x0A14 },
    { SCENE_00KEIKOKU, 1, 0x058B },  { SCENE_00KEIKOKU, 1, 0x858B }, { SCENE_00KEIKOKU, 1, 0x0102 },
    { SCENE_00KEIKOKU, 1, 0x8102 },
};

static EntranceTableEntry sTerminaFieldEntrance2[] = {
    { SCENE_00KEIKOKU, 2, 0xCA14 },  { -SCENE_00KEIKOKU, 2, 0x0A14 }, { -SCENE_00KEIKOKU, 2, 0x0A14 },
    { -SCENE_00KEIKOKU, 2, 0x058B }, { SCENE_00KEIKOKU, 2, 0x0A14 },  { SCENE_00KEIKOKU, 2, 0x0A14 },
};

static EntranceTableEntry sTerminaFieldEntrance3[] = {
    { SCENE_00KEIKOKU, 3, 0xCA14 },  { -SCENE_00KEIKOKU, 3, 0x0A14 }, { -SCENE_00KEIKOKU, 3, 0x0A14 },
    { -SCENE_00KEIKOKU, 3, 0x058B }, { SCENE_00KEIKOKU, 3, 0x0A14 },  { SCENE_00KEIKOKU, 3, 0x0A14 },
};

static EntranceTableEntry sTerminaFieldEntrance4[] = {
    { SCENE_00KEIKOKU, 4, 0xCA14 },  { -SCENE_00KEIKOKU, 4, 0x0A14 }, { -SCENE_00KEIKOKU, 4, 0x0A14 },
    { -SCENE_00KEIKOKU, 4, 0x058B }, { SCENE_00KEIKOKU, 4, 0x0A14 },  { SCENE_00KEIKOKU, 4, 0x0A14 },
};

static EntranceTableEntry sTerminaFieldEntrance5[] = {
    { SCENE_00KEIKOKU, 5, 0xCA14 },  { -SCENE_00KEIKOKU, 5, 0x0A14 }, { -SCENE_00KEIKOKU, 5, 0x0A14 },
    { -SCENE_00KEIKOKU, 5, 0x858B }, { SCENE_00KEIKOKU, 5, 0x0A14 },  { SCENE_00KEIKOKU, 5, 0x0A14 },
};

static EntranceTableEntry sTerminaFieldEntrance6[] = {
    { SCENE_00KEIKOKU, 6, 0x4102 },  { -SCENE_00KEIKOKU, 6, 0x0A14 }, { -SCENE_00KEIKOKU, 6, 0x0A14 },
    { -SCENE_00KEIKOKU, 6, 0x858B }, { SCENE_00KEIKOKU, 6, 0x0A14 },  { SCENE_00KEIKOKU, 6, 0x0A14 },
};

static EntranceTableEntry sTerminaFieldEntrance7[] = {
    { SCENE_00KEIKOKU, 7, 0x4102 },  { -SCENE_00KEIKOKU, 7, 0x0A14 }, { -SCENE_00KEIKOKU, 7, 0x0A14 },
    { -SCENE_00KEIKOKU, 7, 0x858B }, { SCENE_00KEIKOKU, 7, 0x0A14 },  { SCENE_00KEIKOKU, 7, 0x0A14 },
};

static EntranceTableEntry sTerminaFieldEntrance8[] = {
    { SCENE_00KEIKOKU, 8, 0x4102 },  { -SCENE_00KEIKOKU, 8, 0x0A14 }, { -SCENE_00KEIKOKU, 8, 0x0A14 },
    { -SCENE_00KEIKOKU, 8, 0x058B }, { SCENE_00KEIKOKU, 8, 0x0A14 },  { SCENE_00KEIKOKU, 8, 0x0A14 },
};

static EntranceTableEntry sTerminaFieldEntrance9[] = {
    { SCENE_00KEIKOKU, 9, 0x4102 },  { -SCENE_00KEIKOKU, 9, 0x0A14 }, { -SCENE_00KEIKOKU, 9, 0x0A14 },
    { -SCENE_00KEIKOKU, 9, 0x058B }, { SCENE_00KEIKOKU, 9, 0x0A14 },  { SCENE_00KEIKOKU, 9, 0x0A14 },
};

static EntranceTableEntry sTerminaFieldEntrance10[] = {
    { SCENE_00KEIKOKU, 10, 0x8A95 },  { -SCENE_00KEIKOKU, 10, 0x0A14 }, { -SCENE_00KEIKOKU, 10, 0x0A14 },
    { -SCENE_00KEIKOKU, 10, 0x058B }, { SCENE_00KEIKOKU, 10, 0x0A14 },  { SCENE_00KEIKOKU, 10, 0x0A95 },
};

static EntranceTableEntry sTerminaFieldEntrance11[] = {
    { SCENE_00KEIKOKU, 11, 0x4A14 },  { -SCENE_00KEIKOKU, 11, 0x0A14 }, { -SCENE_00KEIKOKU, 11, 0x0A14 },
    { -SCENE_00KEIKOKU, 11, 0x058B }, { SCENE_00KEIKOKU, 11, 0x0A14 },  { SCENE_00KEIKOKU, 11, 0x0A14 },
};

static EntranceTableEntry sTerminaFieldEntrance12[] = {
    { SCENE_00KEIKOKU, 12, 0x0A14 },  { -SCENE_00KEIKOKU, 12, 0x0A14 }, { -SCENE_00KEIKOKU, 12, 0x0A14 },
    { -SCENE_00KEIKOKU, 12, 0x058B }, { SCENE_00KEIKOKU, 12, 0x0A14 },  { SCENE_00KEIKOKU, 12, 0x0A14 },
};

static EntranceTableEntry sTerminaFieldEntrance13[] = {
    { SCENE_00KEIKOKU, 13, 0x0A14 },  { -SCENE_00KEIKOKU, 13, 0x0A14 }, { -SCENE_00KEIKOKU, 13, 0x0A14 },
    { -SCENE_00KEIKOKU, 13, 0x058B }, { SCENE_00KEIKOKU, 13, 0x0A14 },  { SCENE_00KEIKOKU, 13, 0x0A14 },
};

static EntranceTableEntry sTerminaFieldEntrance14[] = {
    { SCENE_00KEIKOKU, 14, 0x858B },
};

static EntranceTableEntry* sTerminaFieldEntranceTable[] = {
    sTerminaFieldEntrance0,  sTerminaFieldEntrance1,  sTerminaFieldEntrance2,  sTerminaFieldEntrance3,
    sTerminaFieldEntrance4,  sTerminaFieldEntrance5,  sTerminaFieldEntrance6,  sTerminaFieldEntrance7,
    sTerminaFieldEntrance8,  sTerminaFieldEntrance9,  sTerminaFieldEntrance10, sTerminaFieldEntrance11,
    sTerminaFieldEntrance12, sTerminaFieldEntrance13, sTerminaFieldEntrance14,
};

static EntranceTableEntry sPostOfficeEntrance0[] = {
    { SCENE_POSTHOUSE, 0, 0x4102 },
};

static EntranceTableEntry* sPostOfficeEntranceTable[] = {
    sPostOfficeEntrance0,
};

static EntranceTableEntry sMarineResearchLabEntrance0[] = {
    { SCENE_LABO, 0, 0x4102 },
};

static EntranceTableEntry* sMarineResearchLabEntranceTable[] = {
    sMarineResearchLabEntrance0,
};

static EntranceTableEntry sDampesHouseEntrance0[] = {
    { SCENE_DANPEI2TEST, 0, 0x4102 },
};

static EntranceTableEntry sDampesHouseEntrance1[] = {
    { SCENE_DANPEI2TEST, 1, 0x0102 },
};

static EntranceTableEntry* sDampesHouseEntranceTable[] = {
    sDampesHouseEntrance0,
    sDampesHouseEntrance1,
};

static EntranceTableEntry sGoronShrineEntrance0[] = {
    { SCENE_16GORON_HOUSE, 0, 0x4102 },
    { SCENE_16GORON_HOUSE, 0, 0x4102 },
};

static EntranceTableEntry sGoronShrineEntrance1[] = {
    { SCENE_16GORON_HOUSE, 1, 0x0102 },
    { SCENE_16GORON_HOUSE, 1, 0x0102 },
};

static EntranceTableEntry sGoronShrineEntrance2[] = {
    { SCENE_16GORON_HOUSE, 2, 0x058B },
    { SCENE_16GORON_HOUSE, 2, 0x058B },
};

static EntranceTableEntry sGoronShrineEntrance3[] = {
    { SCENE_16GORON_HOUSE, 3, 0x0102 },
    { SCENE_16GORON_HOUSE, 3, 0x0102 },
};

static EntranceTableEntry* sGoronShrineEntranceTable[] = {
    sGoronShrineEntrance0,
    sGoronShrineEntrance1,
    sGoronShrineEntrance2,
    sGoronShrineEntrance3,
};

static EntranceTableEntry sZoraHallEntrance0[] = {
    { SCENE_33ZORACITY, 0, 0x4993 },
    { SCENE_33ZORACITY, 0, 0x8102 },
};

static EntranceTableEntry sZoraHallEntrance1[] = {
    { SCENE_33ZORACITY, 1, 0x4102 },
};

static EntranceTableEntry sZoraHallEntrance2[] = {
    { SCENE_33ZORACITY, 2, 0x4102 },
};

static EntranceTableEntry sZoraHallEntrance3[] = {
    { SCENE_33ZORACITY, 3, 0x4102 },
};

static EntranceTableEntry sZoraHallEntrance4[] = {
    { SCENE_33ZORACITY, 4, 0x4102 },
};

static EntranceTableEntry sZoraHallEntrance5[] = {
    { SCENE_33ZORACITY, 5, 0x4102 },
};

static EntranceTableEntry sZoraHallEntrance6[] = {
    { SCENE_33ZORACITY, 6, 0x4102 },
};

static EntranceTableEntry sZoraHallEntrance7[] = {
    { SCENE_33ZORACITY, 7, 0x058B },
};

static EntranceTableEntry sZoraHallEntrance8[] = {
    { SCENE_33ZORACITY, 8, 0x8183 },
};

static EntranceTableEntry* sZoraHallEntranceTable[] = {
    sZoraHallEntrance0, sZoraHallEntrance1, sZoraHallEntrance2, sZoraHallEntrance3, sZoraHallEntrance4,
    sZoraHallEntrance5, sZoraHallEntrance6, sZoraHallEntrance7, sZoraHallEntrance8,
};

static EntranceTableEntry sTradingPostEntrance0[] = {
    { SCENE_8ITEMSHOP, 0, 0x4102 },
};

static EntranceTableEntry sTradingPostEntrance1[] = {
    { SCENE_8ITEMSHOP, 1, 0x0102 },
};

static EntranceTableEntry* sTradingPostEntranceTable[] = {
    sTradingPostEntrance0,
    sTradingPostEntrance1,
};

static EntranceTableEntry sRomaniRanchEntrance0[] = {
    { SCENE_F01, 0, 0x4A14 }, { SCENE_F01, 0, 0x0A14 }, { SCENE_F01, 0, 0x8102 }, { SCENE_F01, 0, 0x858B },
    { SCENE_F01, 0, 0x858B }, { SCENE_F01, 0, 0x0102 }, { SCENE_F01, 0, 0x8102 },
};

static EntranceTableEntry sRomaniRanchEntrance1[] = {
    { SCENE_F01, 1, 0x0A14 },
};

static EntranceTableEntry sRomaniRanchEntrance2[] = {
    { SCENE_F01, 2, 0x0102 },
};

static EntranceTableEntry sRomaniRanchEntrance3[] = {
    { SCENE_F01, 3, 0x0102 },
};

static EntranceTableEntry sRomaniRanchEntrance4[] = {
    { SCENE_F01, 4, 0x8102 },
};

static EntranceTableEntry sRomaniRanchEntrance5[] = {
    { SCENE_F01, 5, 0x8102 },
};

static EntranceTableEntry sRomaniRanchEntrance6[] = {
    { SCENE_F01, 6, 0x0A14 },
};

static EntranceTableEntry sRomaniRanchEntrance7[] = {
    { SCENE_F01, 7, 0x0A14 },
};

static EntranceTableEntry sRomaniRanchEntrance8[] = {
    { SCENE_F01, 8, 0x0A14 },
};

static EntranceTableEntry sRomaniRanchEntrance9[] = {
    { SCENE_F01, 9, 0x0102 },
};

static EntranceTableEntry sRomaniRanchEntrance10[] = {
    { SCENE_F01, 10, 0x0102 },
};

static EntranceTableEntry sRomaniRanchEntrance11[] = {
    { SCENE_F01, 11, 0x0102 },
};

static EntranceTableEntry* sRomaniRanchEntranceTable[] = {
    sRomaniRanchEntrance0, sRomaniRanchEntrance1, sRomaniRanchEntrance2,  sRomaniRanchEntrance3,
    sRomaniRanchEntrance4, sRomaniRanchEntrance5, sRomaniRanchEntrance6,  sRomaniRanchEntrance7,
    sRomaniRanchEntrance8, sRomaniRanchEntrance9, sRomaniRanchEntrance10, sRomaniRanchEntrance11,
};

static EntranceTableEntry sTwinmoldsLairEntrance0[] = {
    { -SCENE_INISIE_BS, 0, 0x0102 },
};

static EntranceTableEntry sTwinmoldsLairEntrance1[] = {
    { -SCENE_INISIE_BS, 1, 0x0102 },
};

static EntranceTableEntry sTwinmoldsLairEntrance2[] = {
    { -SCENE_INISIE_BS, 2, 0x0102 },
};

static EntranceTableEntry sTwinmoldsLairEntrance3[] = {
    { -SCENE_INISIE_BS, 3, 0x0102 },
};

static EntranceTableEntry sTwinmoldsLairEntrance4[] = {
    { -SCENE_INISIE_BS, 4, 0x0102 },
};

static EntranceTableEntry* sTwinmoldsLairEntranceTable[] = {
    sTwinmoldsLairEntrance0, sTwinmoldsLairEntrance1, sTwinmoldsLairEntrance2,
    sTwinmoldsLairEntrance3, sTwinmoldsLairEntrance4,
};

static EntranceTableEntry sGreatBayCoastEntrance0[] = {
    { SCENE_30GYOSON, 0, 0xCA14 },
    { SCENE_30GYOSON, 0, 0xCA14 },
    { SCENE_30GYOSON, 0, 0x8102 },
};

static EntranceTableEntry sGreatBayCoastEntrance1[] = {
    { SCENE_30GYOSON, 1, 0xCA14 },
    { SCENE_30GYOSON, 1, 0xCA14 },
};

static EntranceTableEntry sGreatBayCoastEntrance2[] = {
    { SCENE_30GYOSON, 2, 0x4993 },
    { SCENE_30GYOSON, 2, 0x4993 },
};

static EntranceTableEntry sGreatBayCoastEntrance3[] = {
    { SCENE_30GYOSON, 3, 0x4A14 },
    { SCENE_30GYOSON, 3, 0x4A14 },
};

static EntranceTableEntry sGreatBayCoastEntrance4[] = {
    { SCENE_30GYOSON, 4, 0x4102 },
    { SCENE_30GYOSON, 4, 0x4102 },
};

static EntranceTableEntry sGreatBayCoastEntrance5[] = {
    { SCENE_30GYOSON, 5, 0x4993 },
    { SCENE_30GYOSON, 5, 0x4993 },
};

static EntranceTableEntry sGreatBayCoastEntrance6[] = {
    { SCENE_30GYOSON, 6, 0x4993 },
    { SCENE_30GYOSON, 6, 0x4993 },
};

static EntranceTableEntry sGreatBayCoastEntrance7[] = {
    { SCENE_30GYOSON, 7, 0x4102 },
    { SCENE_30GYOSON, 7, 0x4102 },
};

static EntranceTableEntry sGreatBayCoastEntrance8[] = {
    { SCENE_30GYOSON, 8, 0x4102 },
    { SCENE_30GYOSON, 8, 0x4102 },
};

static EntranceTableEntry sGreatBayCoastEntrance9[] = {
    { SCENE_30GYOSON, 9, 0x058B },
    { SCENE_30GYOSON, 9, 0x058B },
};

static EntranceTableEntry sGreatBayCoastEntrance10[] = {
    { SCENE_30GYOSON, 10, 0x858B },
    { SCENE_30GYOSON, 10, 0x858B },
};

static EntranceTableEntry sGreatBayCoastEntrance11[] = {
    { SCENE_30GYOSON, 11, 0x0A14 },
    { SCENE_30GYOSON, 11, 0x0A14 },
};

static EntranceTableEntry sGreatBayCoastEntrance12[] = {
    { SCENE_30GYOSON, 12, 0x0A14 },
    { SCENE_30GYOSON, 12, 0x0A14 },
};

static EntranceTableEntry sGreatBayCoastEntrance13[] = {
    { SCENE_30GYOSON, 13, 0x0A14 },
    { SCENE_30GYOSON, 13, 0x0A14 },
};

static EntranceTableEntry* sGreatBayCoastEntranceTable[] = {
    sGreatBayCoastEntrance0,  sGreatBayCoastEntrance1,  sGreatBayCoastEntrance2,  sGreatBayCoastEntrance3,
    sGreatBayCoastEntrance4,  sGreatBayCoastEntrance5,  sGreatBayCoastEntrance6,  sGreatBayCoastEntrance7,
    sGreatBayCoastEntrance8,  sGreatBayCoastEntrance9,  sGreatBayCoastEntrance10, sGreatBayCoastEntrance11,
    sGreatBayCoastEntrance12, sGreatBayCoastEntrance13,
};

static EntranceTableEntry sZoraCapeEntrance0[] = {
    { SCENE_31MISAKI, 0, 0x8A14 },
    { SCENE_31MISAKI, 0, 0x8A14 },
};

static EntranceTableEntry sZoraCapeEntrance1[] = {
    { SCENE_31MISAKI, 1, 0x4993 },
    { SCENE_31MISAKI, 1, 0x4993 },
};

static EntranceTableEntry sZoraCapeEntrance2[] = {
    { SCENE_31MISAKI, 2, 0x4102 },
    { SCENE_31MISAKI, 2, 0x4102 },
};

static EntranceTableEntry sZoraCapeEntrance3[] = {
    { SCENE_31MISAKI, 3, 0x0993 },
    { SCENE_31MISAKI, 3, 0x0993 },
};

static EntranceTableEntry sZoraCapeEntrance4[] = {
    { SCENE_31MISAKI, 4, 0x4102 },
    { SCENE_31MISAKI, 4, 0x4102 },
};

static EntranceTableEntry sZoraCapeEntrance5[] = {
    { SCENE_31MISAKI, 5, 0x0102 },
    { SCENE_31MISAKI, 5, 0x0102 },
};

static EntranceTableEntry sZoraCapeEntrance6[] = {
    { SCENE_31MISAKI, 6, 0x0A14 },
    { SCENE_31MISAKI, 6, 0x0A14 },
};

static EntranceTableEntry sZoraCapeEntrance7[] = {
    { SCENE_31MISAKI, 7, 0x0A14 },
    { SCENE_31MISAKI, 7, 0x0A14 },
};

static EntranceTableEntry sZoraCapeEntrance8[] = {
    { SCENE_31MISAKI, 8, 0x0A14 },
    { SCENE_31MISAKI, 8, 0x0A14 },
};

static EntranceTableEntry sZoraCapeEntrance9[] = {
    { SCENE_31MISAKI, 9, 0x0A14 },
    { SCENE_31MISAKI, 9, 0x0A14 },
};

static EntranceTableEntry* sZoraCapeEntranceTable[] = {
    sZoraCapeEntrance0, sZoraCapeEntrance1, sZoraCapeEntrance2, sZoraCapeEntrance3, sZoraCapeEntrance4,
    sZoraCapeEntrance5, sZoraCapeEntrance6, sZoraCapeEntrance7, sZoraCapeEntrance8, sZoraCapeEntrance9,
};

static EntranceTableEntry sLotteryShopEntrance0[] = {
    { SCENE_TAKARAKUJI, 0, 0x4102 },
};

static EntranceTableEntry* sLotteryShopEntranceTable[] = {
    sLotteryShopEntrance0,
};

static EntranceTableEntry sPiratesFortressExteriorEntrance0[] = {
    { SCENE_TORIDE, 0, 0x4993 },
};

static EntranceTableEntry sPiratesFortressExteriorEntrance1[] = {
    { SCENE_TORIDE, 1, 0x8102 },
};

static EntranceTableEntry sPiratesFortressExteriorEntrance2[] = {
    { SCENE_TORIDE, 2, 0x8993 },
};

static EntranceTableEntry sPiratesFortressExteriorEntrance3[] = {
    { SCENE_TORIDE, 3, 0x8993 },
};

static EntranceTableEntry sPiratesFortressExteriorEntrance4[] = {
    { SCENE_TORIDE, 4, 0x8102 },
};

static EntranceTableEntry sPiratesFortressExteriorEntrance5[] = {
    { SCENE_TORIDE, 5, 0x8102 },
};

static EntranceTableEntry sPiratesFortressExteriorEntrance6[] = {
    { SCENE_TORIDE, 6, 0x8102 },
};

static EntranceTableEntry* sPiratesFortressExteriorEntranceTable[] = {
    sPiratesFortressExteriorEntrance0, sPiratesFortressExteriorEntrance1, sPiratesFortressExteriorEntrance2,
    sPiratesFortressExteriorEntrance3, sPiratesFortressExteriorEntrance4, sPiratesFortressExteriorEntrance5,
    sPiratesFortressExteriorEntrance6,
};

static EntranceTableEntry sFishermansHutEntrance0[] = {
    { SCENE_FISHERMAN, 0, 0x4102 },
};

static EntranceTableEntry* sFishermansHutEntranceTable[] = {
    sFishermansHutEntrance0,
};

static EntranceTableEntry sGoronShopEntrance0[] = {
    { SCENE_GORONSHOP, 0, 0x4102 },
};

static EntranceTableEntry* sGoronShopEntranceTable[] = {
    sGoronShopEntrance0,
};

static EntranceTableEntry sDekuKingsChamberEntrance0[] = {
    { -SCENE_DEKU_KING, 0, 0xC102 },
    { -SCENE_DEKU_KING, 0, 0x8102 },
};

static EntranceTableEntry sDekuKingsChamberEntrance1[] = {
    { -SCENE_DEKU_KING, 1, 0xC102 },
};

static EntranceTableEntry sDekuKingsChamberEntrance2[] = {
    { -SCENE_DEKU_KING, 2, 0x858B },
};

static EntranceTableEntry sDekuKingsChamberEntrance3[] = {
    { -SCENE_DEKU_KING, 3, 0x0102 },
};

static EntranceTableEntry* sDekuKingsChamberEntranceTable[] = {
    sDekuKingsChamberEntrance0,
    sDekuKingsChamberEntrance1,
    sDekuKingsChamberEntrance2,
    sDekuKingsChamberEntrance3,
};

static EntranceTableEntry sMoonGoronTrialEntrance0[] = {
    { -SCENE_LAST_GORON, 0, 0x0387 },
};

static EntranceTableEntry* sMoonGoronTrialEntranceTable[] = {
    sMoonGoronTrialEntrance0,
};

static EntranceTableEntry sRoadToSouthernSwampEntrance0[] = {
    { SCENE_24KEMONOMITI, 0, 0x8A14 },
};

static EntranceTableEntry sRoadToSouthernSwampEntrance1[] = {
    { SCENE_24KEMONOMITI, 1, 0x8A14 },
};

static EntranceTableEntry sRoadToSouthernSwampEntrance2[] = {
    { SCENE_24KEMONOMITI, 2, 0x0102 },
};

static EntranceTableEntry* sRoadToSouthernSwampEntranceTable[] = {
    sRoadToSouthernSwampEntrance0,
    sRoadToSouthernSwampEntrance1,
    sRoadToSouthernSwampEntrance2,
};

static EntranceTableEntry sDoggyRacetrackEntrance0[] = {
    { SCENE_F01_B, 0, 0xC102 },
};

static EntranceTableEntry sDoggyRacetrackEntrance1[] = {
    { SCENE_F01_B, 1, 0x0A14 },
};

static EntranceTableEntry* sDoggyRacetrackEntranceTable[] = {
    sDoggyRacetrackEntrance0,
    sDoggyRacetrackEntrance1,
};

static EntranceTableEntry sCuccoShackEntrance0[] = {
    { SCENE_F01C, 0, 0xC102 },
    { SCENE_F01C, 0, 0x8102 },
};

static EntranceTableEntry sCuccoShackEntrance1[] = {
    { SCENE_F01C, 1, 0x0A14 },
};

static EntranceTableEntry* sCuccoShackEntranceTable[] = {
    sCuccoShackEntrance0,
    sCuccoShackEntrance1,
};

static EntranceTableEntry sIkanaGraveyardEntrance0[] = {
    { SCENE_BOTI, 0, 0x4A14 },
    { SCENE_BOTI, 0, 0x8102 },
};

static EntranceTableEntry sIkanaGraveyardEntrance1[] = {
    { SCENE_BOTI, 1, 0x4102 },
};

static EntranceTableEntry sIkanaGraveyardEntrance2[] = {
    { SCENE_BOTI, 2, 0x4102 },
};

static EntranceTableEntry sIkanaGraveyardEntrance3[] = {
    { SCENE_BOTI, 3, 0x4102 },
};

static EntranceTableEntry sIkanaGraveyardEntrance4[] = {
    { SCENE_BOTI, 4, 0x4102 },
};

static EntranceTableEntry sIkanaGraveyardEntrance5[] = {
    { SCENE_BOTI, 5, 0x058B },
};

static EntranceTableEntry* sIkanaGraveyardEntranceTable[] = {
    sIkanaGraveyardEntrance0, sIkanaGraveyardEntrance1, sIkanaGraveyardEntrance2,
    sIkanaGraveyardEntrance3, sIkanaGraveyardEntrance4, sIkanaGraveyardEntrance5,
};

static EntranceTableEntry sGohtsLairEntrance0[] = {
    { -SCENE_HAKUGIN_BS, 0, 0x8102 },
};

static EntranceTableEntry* sGohtsLairEntranceTable[] = {
    sGohtsLairEntrance0,
};

static EntranceTableEntry sSouthernSwampPoisonedEntrance0[] = {
    { SCENE_20SICHITAI, 0, 0xCA14 },
};

static EntranceTableEntry sSouthernSwampPoisonedEntrance1[] = {
    { SCENE_20SICHITAI, 1, 0x4102 },
};

static EntranceTableEntry sSouthernSwampPoisonedEntrance2[] = {
    { SCENE_20SICHITAI, 2, 0xC102 },
};

static EntranceTableEntry sSouthernSwampPoisonedEntrance3[] = {
    { SCENE_20SICHITAI, 3, 0x4102 },
};

static EntranceTableEntry sSouthernSwampPoisonedEntrance4[] = {
    { SCENE_20SICHITAI, 4, 0x4102 },
};

static EntranceTableEntry sSouthernSwampPoisonedEntrance5[] = {
    { SCENE_20SICHITAI, 5, 0x4102 },
};

static EntranceTableEntry sSouthernSwampPoisonedEntrance6[] = {
    { SCENE_20SICHITAI, 6, 0x0102 },
};

static EntranceTableEntry sSouthernSwampPoisonedEntrance7[] = {
    { SCENE_20SICHITAI, 7, 0x4102 },
};

static EntranceTableEntry sSouthernSwampPoisonedEntrance8[] = {
    { SCENE_20SICHITAI, 8, 0x4102 },
};

static EntranceTableEntry sSouthernSwampPoisonedEntrance9[] = {
    { SCENE_20SICHITAI, 9, 0x4A14 },
};

static EntranceTableEntry sSouthernSwampPoisonedEntrance10[] = {
    { SCENE_20SICHITAI, 10, 0x4A14 },
};

static EntranceTableEntry* sSouthernSwampPoisonedEntranceTable[] = {
    sSouthernSwampPoisonedEntrance0, sSouthernSwampPoisonedEntrance1,  sSouthernSwampPoisonedEntrance2,
    sSouthernSwampPoisonedEntrance3, sSouthernSwampPoisonedEntrance4,  sSouthernSwampPoisonedEntrance5,
    sSouthernSwampPoisonedEntrance6, sSouthernSwampPoisonedEntrance7,  sSouthernSwampPoisonedEntrance8,
    sSouthernSwampPoisonedEntrance9, sSouthernSwampPoisonedEntrance10,
};

static EntranceTableEntry sWoodfallEntrance0[] = {
    { SCENE_21MITURINMAE, 0, 0xC102 },
    { SCENE_21MITURINMAE, 0, 0x0183 },
    { SCENE_21MITURINMAE, 0, 0xC102 },
};

static EntranceTableEntry sWoodfallEntrance1[] = {
    { SCENE_21MITURINMAE, 1, 0x4102 },
    { SCENE_21MITURINMAE, 1, 0x0183 },
    { SCENE_21MITURINMAE, 1, 0x4102 },
};

static EntranceTableEntry sWoodfallEntrance2[] = {
    { SCENE_21MITURINMAE, 2, 0x4102 },
    { SCENE_21MITURINMAE, 2, 0x0183 },
    { SCENE_21MITURINMAE, 2, 0x4102 },
};

static EntranceTableEntry sWoodfallEntrance3[] = {
    { SCENE_21MITURINMAE, 3, 0x4102 },
    { SCENE_21MITURINMAE, 3, 0x0183 },
    { SCENE_21MITURINMAE, 3, 0x4102 },
};

static EntranceTableEntry sWoodfallEntrance4[] = {
    { SCENE_21MITURINMAE, 4, 0x4102 },
    { SCENE_21MITURINMAE, 4, 0x0183 },
    { SCENE_21MITURINMAE, 4, 0x4102 },
};

static EntranceTableEntry* sWoodfallEntranceTable[] = {
    sWoodfallEntrance0, sWoodfallEntrance1, sWoodfallEntrance2, sWoodfallEntrance3, sWoodfallEntrance4,
};

static EntranceTableEntry sMoonZoraTrialEntrance0[] = {
    { -SCENE_LAST_ZORA, 0, 0x0387 },
};

static EntranceTableEntry sMoonZoraTrialEntrance1[] = {
    { -SCENE_LAST_ZORA, 1, 0x0102 },
};

static EntranceTableEntry* sMoonZoraTrialEntranceTable[] = {
    sMoonZoraTrialEntrance0,
    sMoonZoraTrialEntrance1,
};

static EntranceTableEntry sGoronVillageSpringEntrance0[] = {
    { SCENE_11GORONNOSATO2, 0, 0xC102 },
    { SCENE_11GORONNOSATO2, 0, 0x8102 },
};

static EntranceTableEntry sGoronVillageSpringEntrance1[] = {
    { SCENE_11GORONNOSATO2, 1, 0x4102 },
};

static EntranceTableEntry sGoronVillageSpringEntrance2[] = {
    { SCENE_11GORONNOSATO2, 2, 0x4102 },
};

static EntranceTableEntry sGoronVillageSpringEntrance3[] = {
    { SCENE_11GORONNOSATO2, 3, 0x4102 },
};

static EntranceTableEntry sGoronVillageSpringEntrance4[] = {
    { SCENE_11GORONNOSATO2, 4, 0x0A14 },
};

static EntranceTableEntry* sGoronVillageSpringEntranceTable[] = {
    sGoronVillageSpringEntrance0, sGoronVillageSpringEntrance1, sGoronVillageSpringEntrance2,
    sGoronVillageSpringEntrance3, sGoronVillageSpringEntrance4,
};

static EntranceTableEntry sGreatBayTempleEntrance0[] = {
    { -SCENE_SEA, 0, 0x4102 },
};

static EntranceTableEntry sGreatBayTempleEntrance1[] = {
    { -SCENE_SEA, 1, 0x0102 },
};

static EntranceTableEntry sGreatBayTempleEntrance2[] = {
    { -SCENE_SEA, 2, 0x0102 },
};

static EntranceTableEntry* sGreatBayTempleEntranceTable[] = {
    sGreatBayTempleEntrance0,
    sGreatBayTempleEntrance1,
    sGreatBayTempleEntrance2,
};

static EntranceTableEntry sWaterfallRapidsEntrance0[] = {
    { SCENE_35TAKI, 0, 0x4102 },
};

static EntranceTableEntry sWaterfallRapidsEntrance1[] = {
    { SCENE_35TAKI, 1, 0x0A14 },
};

static EntranceTableEntry sWaterfallRapidsEntrance2[] = {
    { SCENE_35TAKI, 2, 0x0A14 },
};

static EntranceTableEntry sWaterfallRapidsEntrance3[] = {
    { SCENE_35TAKI, 3, 0x0A14 },
};

static EntranceTableEntry* sWaterfallRapidsEntranceTable[] = {
    sWaterfallRapidsEntrance0,
    sWaterfallRapidsEntrance1,
    sWaterfallRapidsEntrance2,
    sWaterfallRapidsEntrance3,
};

static EntranceTableEntry sBeneathTheWellEntrance0[] = {
    { -SCENE_REDEAD, 0, 0x4102 },
};

static EntranceTableEntry sBeneathTheWellEntrance1[] = {
    { -SCENE_REDEAD, 1, 0x4102 },
};

static EntranceTableEntry* sBeneathTheWellEntranceTable[] = {
    sBeneathTheWellEntrance0,
    sBeneathTheWellEntrance1,
};

static EntranceTableEntry sZoraHallRoomsEntrance0[] = {
    { SCENE_BANDROOM, 0, 0x0102 },
};

static EntranceTableEntry sZoraHallRoomsEntrance1[] = {
    { SCENE_BANDROOM, 1, 0x0102 },
};

static EntranceTableEntry sZoraHallRoomsEntrance2[] = {
    { SCENE_BANDROOM, 2, 0x0102 },
};

static EntranceTableEntry sZoraHallRoomsEntrance3[] = {
    { SCENE_BANDROOM, 3, 0x0102 },
};

static EntranceTableEntry sZoraHallRoomsEntrance4[] = {
    { SCENE_BANDROOM, 4, 0x858B },
};

static EntranceTableEntry sZoraHallRoomsEntrance5[] = {
    { SCENE_BANDROOM, 5, 0x0102 },
};

static EntranceTableEntry sZoraHallRoomsEntrance6[] = {
    { SCENE_BANDROOM, 6, 0x0102 },
};

static EntranceTableEntry* sZoraHallRoomsEntranceTable[] = {
    sZoraHallRoomsEntrance0, sZoraHallRoomsEntrance1, sZoraHallRoomsEntrance2, sZoraHallRoomsEntrance3,
    sZoraHallRoomsEntrance4, sZoraHallRoomsEntrance5, sZoraHallRoomsEntrance6,
};

static EntranceTableEntry sGoronVillageWinterEntrance0[] = {
    { SCENE_11GORONNOSATO, 0, 0xC102 },
};

static EntranceTableEntry sGoronVillageWinterEntrance1[] = {
    { SCENE_11GORONNOSATO, 1, 0x4102 },
};

static EntranceTableEntry sGoronVillageWinterEntrance2[] = {
    { SCENE_11GORONNOSATO, 2, 0x4102 },
};

static EntranceTableEntry sGoronVillageWinterEntrance3[] = {
    { SCENE_11GORONNOSATO, 3, 0x4102 },
};

static EntranceTableEntry sGoronVillageWinterEntrance4[] = {
    { SCENE_11GORONNOSATO, 4, 0x0A14 },
};

static EntranceTableEntry* sGoronVillageWinterEntranceTable[] = {
    sGoronVillageWinterEntrance0, sGoronVillageWinterEntrance1, sGoronVillageWinterEntrance2,
    sGoronVillageWinterEntrance3, sGoronVillageWinterEntrance4,
};

static EntranceTableEntry sGoronGraveryardEntrance0[] = {
    { SCENE_GORON_HAKA, 0, 0x4102 },
};

static EntranceTableEntry sGoronGraveryardEntrance1[] = {
    { SCENE_GORON_HAKA, 1, 0x058B },
};

static EntranceTableEntry* sGoronGraveryardEntranceTable[] = {
    sGoronGraveryardEntrance0,
    sGoronGraveryardEntrance1,
};

static EntranceTableEntry sSakonsHideoutEntrance0[] = {
    { SCENE_SECOM, 0, 0x4102 },
};

static EntranceTableEntry* sSakonsHideoutEntranceTable[] = {
    sSakonsHideoutEntrance0,
};

static EntranceTableEntry sMountainVillageWinterEntrance0[] = {
    { SCENE_10YUKIYAMANOMURA, 0, 0x4A14 },
};

static EntranceTableEntry sMountainVillageWinterEntrance1[] = {
    { SCENE_10YUKIYAMANOMURA, 1, 0x4102 },
};

static EntranceTableEntry sMountainVillageWinterEntrance2[] = {
    { SCENE_10YUKIYAMANOMURA, 2, 0xCA14 },
};

static EntranceTableEntry sMountainVillageWinterEntrance3[] = {
    { SCENE_10YUKIYAMANOMURA, 3, 0x4102 },
};

static EntranceTableEntry sMountainVillageWinterEntrance4[] = {
    { SCENE_10YUKIYAMANOMURA, 4, 0xCA14 },
};

static EntranceTableEntry sMountainVillageWinterEntrance5[] = {
    { SCENE_10YUKIYAMANOMURA, 5, 0x4102 },
};

static EntranceTableEntry sMountainVillageWinterEntrance6[] = {
    { SCENE_10YUKIYAMANOMURA, 6, 0x4A14 },
};

static EntranceTableEntry sMountainVillageWinterEntrance7[] = {
    { SCENE_10YUKIYAMANOMURA, 7, 0x4A14 },
};

static EntranceTableEntry sMountainVillageWinterEntrance8[] = {
    { SCENE_10YUKIYAMANOMURA, 8, 0x4A14 },
};

static EntranceTableEntry* sMountainVillageWinterEntranceTable[] = {
    sMountainVillageWinterEntrance0, sMountainVillageWinterEntrance1, sMountainVillageWinterEntrance2,
    sMountainVillageWinterEntrance3, sMountainVillageWinterEntrance4, sMountainVillageWinterEntrance5,
    sMountainVillageWinterEntrance6, sMountainVillageWinterEntrance7, sMountainVillageWinterEntrance8,
};

static EntranceTableEntry sGhostHutEntrance0[] = {
    { SCENE_TOUGITES, 0, 0x4102 },
};

static EntranceTableEntry sGhostHutEntrance1[] = {
    { SCENE_TOUGITES, 1, 0x0102 },
};

static EntranceTableEntry sGhostHutEntrance2[] = {
    { SCENE_TOUGITES, 2, 0x0102 },
};

static EntranceTableEntry* sGhostHutEntranceTable[] = {
    sGhostHutEntrance0,
    sGhostHutEntrance1,
    sGhostHutEntrance2,
};

static EntranceTableEntry sDekuShrineEntrance0[] = {
    { SCENE_DANPEI, 0, 0x4102 },
};

static EntranceTableEntry sDekuShrineEntrance1[] = {
    { SCENE_DANPEI, 1, 0x0102 },
};

static EntranceTableEntry sDekuShrineEntrance2[] = {
    { SCENE_DANPEI, 2, 0x0102 },
};

static EntranceTableEntry* sDekuShrineEntranceTable[] = {
    sDekuShrineEntrance0,
    sDekuShrineEntrance1,
    sDekuShrineEntrance2,
};

static EntranceTableEntry sRoadToIkanaEntrance0[] = {
    { SCENE_IKANAMAE, 0, 0x8A14 },
};

static EntranceTableEntry sRoadToIkanaEntrance1[] = {
    { SCENE_IKANAMAE, 1, 0x8A14 },
};

static EntranceTableEntry sRoadToIkanaEntrance2[] = {
    { SCENE_IKANAMAE, 2, 0x0A14 },
};

static EntranceTableEntry* sRoadToIkanaEntranceTable[] = {
    sRoadToIkanaEntrance0,
    sRoadToIkanaEntrance1,
    sRoadToIkanaEntrance2,
};

static EntranceTableEntry sMusicBoxHouseEntrance0[] = {
    { SCENE_MUSICHOUSE, 0, 0x4102 },
};

static EntranceTableEntry* sMusicBoxHouseEntranceTable[] = {
    sMusicBoxHouseEntrance0,
};

static EntranceTableEntry sIgosDuIkanasLairEntrance0[] = {
    { -SCENE_IKNINSIDE, 0, 0x8102 },
    { -SCENE_IKNINSIDE, 0, 0x8102 },
};

static EntranceTableEntry* sIgosDuIkanasLairEntranceTable[] = {
    sIgosDuIkanasLairEntrance0,
};

static EntranceTableEntry sSwordmansSchoolEntrance0[] = {
    { SCENE_DOUJOU, 0, 0x4102 },
};

static EntranceTableEntry* sSwordmansSchoolEntranceTable[] = {
    sSwordmansSchoolEntrance0,
};

static EntranceTableEntry sTouristInformationEntrance0[] = {
    { SCENE_MAP_SHOP, 0, 0x4102 },
};

static EntranceTableEntry sTouristInformationEntrance1[] = {
    { SCENE_MAP_SHOP, 1, 0x0102 },
};

static EntranceTableEntry sTouristInformationEntrance2[] = {
    { SCENE_MAP_SHOP, 2, 0x0102 },
};

static EntranceTableEntry* sTouristInformationEntranceTable[] = {
    sTouristInformationEntrance0,
    sTouristInformationEntrance1,
    sTouristInformationEntrance2,
};

static EntranceTableEntry sStoneTowerEntrance0[] = {
    { SCENE_F40, 0, 0x4102 },
};

static EntranceTableEntry sStoneTowerEntrance1[] = {
    { SCENE_F40, 1, 0xCA14 },
};

static EntranceTableEntry sStoneTowerEntrance2[] = {
    { SCENE_F40, 2, 0xC102 },
};

static EntranceTableEntry sStoneTowerEntrance3[] = {
    { SCENE_F40, 3, 0x4A14 },
};

static EntranceTableEntry* sStoneTowerEntranceTable[] = {
    sStoneTowerEntrance0,
    sStoneTowerEntrance1,
    sStoneTowerEntrance2,
    sStoneTowerEntrance3,
};

static EntranceTableEntry sStoneTowerInvertedEntrance0[] = {
    { SCENE_F41, 0, 0x4A14 },
};

static EntranceTableEntry sStoneTowerInvertedEntrance1[] = {
    { SCENE_F41, 1, 0xC102 },
};

static EntranceTableEntry* sStoneTowerInvertedEntranceTable[] = {
    sStoneTowerInvertedEntrance0,
    sStoneTowerInvertedEntrance1,
};

static EntranceTableEntry sMountainVillageSpringEntrance0[] = {
    { SCENE_10YUKIYAMANOMURA2, 0, 0x4A14 },
    { SCENE_10YUKIYAMANOMURA2, 0, 0x4A14 },
};

static EntranceTableEntry sMountainVillageSpringEntrance1[] = {
    { SCENE_10YUKIYAMANOMURA2, 1, 0x4102 },
};

static EntranceTableEntry sMountainVillageSpringEntrance2[] = {
    { SCENE_10YUKIYAMANOMURA2, 2, 0xCA14 },
};

static EntranceTableEntry sMountainVillageSpringEntrance3[] = {
    { SCENE_10YUKIYAMANOMURA2, 3, 0x4102 },
};

static EntranceTableEntry sMountainVillageSpringEntrance4[] = {
    { SCENE_10YUKIYAMANOMURA2, 4, 0xCA14 },
};

static EntranceTableEntry sMountainVillageSpringEntrance5[] = {
    { SCENE_10YUKIYAMANOMURA2, 5, 0x4102 },
};

static EntranceTableEntry sMountainVillageSpringEntrance6[] = {
    { SCENE_10YUKIYAMANOMURA2, 6, 0xCA14 },
};

static EntranceTableEntry sMountainVillageSpringEntrance7[] = {
    { SCENE_10YUKIYAMANOMURA2, 7, 0x458B },
};

static EntranceTableEntry sMountainVillageSpringEntrance8[] = {
    { SCENE_10YUKIYAMANOMURA2, 8, 0x4A14 },
};

static EntranceTableEntry* sMountainVillageSpringEntranceTable[] = {
    sMountainVillageSpringEntrance0, sMountainVillageSpringEntrance1, sMountainVillageSpringEntrance2,
    sMountainVillageSpringEntrance3, sMountainVillageSpringEntrance4, sMountainVillageSpringEntrance5,
    sMountainVillageSpringEntrance6, sMountainVillageSpringEntrance7, sMountainVillageSpringEntrance8,
};

static EntranceTableEntry sPathToSnowheadEntrance0[] = {
    { SCENE_14YUKIDAMANOMITI, 0, 0xCA14 },
    { SCENE_14YUKIDAMANOMITI, 0, 0xCA14 },
};

static EntranceTableEntry sPathToSnowheadEntrance1[] = {
    { SCENE_14YUKIDAMANOMITI, 1, 0xCA14 },
    { SCENE_14YUKIDAMANOMITI, 1, 0xCA14 },
};

static EntranceTableEntry* sPathToSnowheadEntranceTable[] = {
    sPathToSnowheadEntrance0,
    sPathToSnowheadEntrance1,
};

static EntranceTableEntry sSnowheadEntrance0[] = {
    { SCENE_12HAKUGINMAE, 0, 0xCA14 },
    { SCENE_12HAKUGINMAE, 0, 0xCA14 },
};

static EntranceTableEntry sSnowheadEntrance1[] = {
    { SCENE_12HAKUGINMAE, 1, 0x4102 },
    { SCENE_12HAKUGINMAE, 1, 0x4102 },
};

static EntranceTableEntry sSnowheadEntrance2[] = {
    { SCENE_12HAKUGINMAE, 2, 0x4102 },
    { SCENE_12HAKUGINMAE, 2, 0x4102 },
};

static EntranceTableEntry sSnowheadEntrance3[] = {
    { SCENE_12HAKUGINMAE, 3, 0x4102 },
    { SCENE_12HAKUGINMAE, 3, 0x4102 },
};

static EntranceTableEntry* sSnowheadEntranceTable[] = {
    sSnowheadEntrance0,
    sSnowheadEntrance1,
    sSnowheadEntrance2,
    sSnowheadEntrance3,
};

static EntranceTableEntry sPathToGoronVillageWinterEntrance0[] = {
    { SCENE_17SETUGEN, 0, 0x8A14 },
};

static EntranceTableEntry sPathToGoronVillageWinterEntrance1[] = {
    { SCENE_17SETUGEN, 1, 0x8102 },
};

static EntranceTableEntry sPathToGoronVillageWinterEntrance2[] = {
    { SCENE_17SETUGEN, 2, 0x0102 },
};

static EntranceTableEntry* sPathToGoronVillageWinterEntranceTable[] = {
    sPathToGoronVillageWinterEntrance0,
    sPathToGoronVillageWinterEntrance1,
    sPathToGoronVillageWinterEntrance2,
};

static EntranceTableEntry sPathToGoronVillageSpringEntrance0[] = {
    { SCENE_17SETUGEN2, 0, 0x8A14 },
};

static EntranceTableEntry sPathToGoronVillageSpringEntrance1[] = {
    { SCENE_17SETUGEN2, 1, 0x8102 },
};

static EntranceTableEntry sPathToGoronVillageSpringEntrance2[] = {
    { SCENE_17SETUGEN2, 2, 0x0102 },
};

static EntranceTableEntry* sPathToGoronVillageSpringEntranceTable[] = {
    sPathToGoronVillageSpringEntrance0,
    sPathToGoronVillageSpringEntrance1,
    sPathToGoronVillageSpringEntrance2,
};

static EntranceTableEntry sGyorgsLairEntrance0[] = {
    { -SCENE_SEA_BS, 0, 0x8102 },
};

static EntranceTableEntry sGyorgsLairEntrance1[] = {
    { -SCENE_SEA_BS, 1, 0x8102 },
};

static EntranceTableEntry* sGyorgsLairEntranceTable[] = {
    sGyorgsLairEntrance0,
    sGyorgsLairEntrance1,
};

static EntranceTableEntry sSecretShrineEntrance0[] = {
    { -SCENE_RANDOM, 0, 0x4102 },
};

static EntranceTableEntry* sSecretShrineEntranceTable[] = {
    sSecretShrineEntrance0,
};

static EntranceTableEntry sStockPotInnEntrance0[] = {
    { SCENE_YADOYA, 0, 0x4102 },
    { SCENE_YADOYA, 0, 0x8102 },
};

static EntranceTableEntry sStockPotInnEntrance1[] = {
    { SCENE_YADOYA, 1, 0x4102 },
};

static EntranceTableEntry sStockPotInnEntrance2[] = {
    { SCENE_YADOYA, 2, 0x0102 },
};

static EntranceTableEntry sStockPotInnEntrance3[] = {
    { SCENE_YADOYA, 3, 0x0102 },
};

static EntranceTableEntry sStockPotInnEntrance4[] = {
    { SCENE_YADOYA, 4, 0x0102 },
};

static EntranceTableEntry sStockPotInnEntrance5[] = {
    { SCENE_YADOYA, 5, 0x0102 },
};

static EntranceTableEntry* sStockPotInnEntranceTable[] = {
    sStockPotInnEntrance0, sStockPotInnEntrance1, sStockPotInnEntrance2,
    sStockPotInnEntrance3, sStockPotInnEntrance4, sStockPotInnEntrance5,
};

static EntranceTableEntry sGreatBayCutsceneEntrance0[] = {
    { SCENE_KONPEKI_ENT, 0, 0x4102 },
};

static EntranceTableEntry* sGreatBayCutsceneEntranceTable[] = {
    sGreatBayCutsceneEntrance0,
};

static EntranceTableEntry sClockTowerInteriorEntrance0[] = {
    { SCENE_INSIDETOWER, 0, 0x0102 },
};

static EntranceTableEntry sClockTowerInteriorEntrance1[] = {
    { SCENE_INSIDETOWER, 1, 0x0102 },
};

static EntranceTableEntry sClockTowerInteriorEntrance2[] = {
    { SCENE_INSIDETOWER, 2, 0x058B },
};

static EntranceTableEntry sClockTowerInteriorEntrance3[] = {
    { SCENE_INSIDETOWER, 3, 0x0102 },
};

static EntranceTableEntry sClockTowerInteriorEntrance4[] = {
    { SCENE_INSIDETOWER, 4, 0x058B },
};

static EntranceTableEntry sClockTowerInteriorEntrance5[] = {
    { SCENE_INSIDETOWER, 5, 0x0102 },
};

static EntranceTableEntry sClockTowerInteriorEntrance6[] = {
    { SCENE_INSIDETOWER, 6, 0x858B },
};

static EntranceTableEntry* sClockTowerInteriorEntranceTable[] = {
    sClockTowerInteriorEntrance0, sClockTowerInteriorEntrance1, sClockTowerInteriorEntrance2,
    sClockTowerInteriorEntrance3, sClockTowerInteriorEntrance4, sClockTowerInteriorEntrance5,
    sClockTowerInteriorEntrance6,
};

static EntranceTableEntry sWoodsOfMysteryEntrance0[] = {
    { SCENE_26SARUNOMORI, 0, 0x4102 },
};

static EntranceTableEntry* sWoodsOfMysteryEntranceTable[] = {
    sWoodsOfMysteryEntrance0,
};

static EntranceTableEntry sLostWoodsEntrance0[] = {
    { -SCENE_LOST_WOODS, 0, 0x858B },
    { -SCENE_LOST_WOODS, 0, 0x8183 },
    { -SCENE_LOST_WOODS, 0, 0x858B },
    { -SCENE_LOST_WOODS, 0, 0x8102 },
};

static EntranceTableEntry sLostWoodsEntrance1[] = {
    { -SCENE_LOST_WOODS, 1, 0x058B },
    { -SCENE_LOST_WOODS, 1, 0x058B },
    { -SCENE_LOST_WOODS, 1, 0x058B },
    { -SCENE_LOST_WOODS, 1, 0x058B },
};

static EntranceTableEntry sLostWoodsEntrance2[] = {
    { -SCENE_LOST_WOODS, 2, 0x058B },
    { -SCENE_LOST_WOODS, 2, 0x058B },
    { -SCENE_LOST_WOODS, 2, 0x058B },
    { -SCENE_LOST_WOODS, 2, 0x058B },
};

static EntranceTableEntry* sLostWoodsEntranceTable[] = {
    sLostWoodsEntrance0,
    sLostWoodsEntrance1,
    sLostWoodsEntrance2,
};

static EntranceTableEntry sMoonLinkTrialEntrance0[] = {
    { SCENE_LAST_LINK, 0, 0x0387 },
};

static EntranceTableEntry* sMoonLinkTrialEntranceTable[] = {
    sMoonLinkTrialEntrance0,
};

static EntranceTableEntry sTheMoonEntrance0[] = {
    { SCENE_SOUGEN, 0, 0x0387 },
};

static EntranceTableEntry* sTheMoonEntranceTable[] = {
    sTheMoonEntrance0,
};

static EntranceTableEntry sBombShopEntrance0[] = {
    { SCENE_BOMYA, 0, 0x4102 },
};

static EntranceTableEntry sBombShopEntrance1[] = {
    { SCENE_BOMYA, 1, 0x0102 },
};

static EntranceTableEntry* sBombShopEntranceTable[] = {
    sBombShopEntrance0,
    sBombShopEntrance1,
};

static EntranceTableEntry sGiantsChamberEntrance0[] = {
    { SCENE_KYOJINNOMA, 0, 0x0102 }, { SCENE_KYOJINNOMA, 0, 0x0102 }, { SCENE_KYOJINNOMA, 0, 0x0102 },
    { SCENE_KYOJINNOMA, 0, 0x0102 }, { SCENE_KYOJINNOMA, 0, 0x0102 }, { SCENE_KYOJINNOMA, 0, 0x0102 },
    { SCENE_KYOJINNOMA, 0, 0x0102 }, { SCENE_KYOJINNOMA, 0, 0x0102 }, { SCENE_KYOJINNOMA, 0, 0x0102 },
    { SCENE_KYOJINNOMA, 0, 0x0102 }, { SCENE_KYOJINNOMA, 0, 0x0102 },
};

static EntranceTableEntry* sGiantsChamberEntranceTable[] = {
    sGiantsChamberEntrance0,
};

static EntranceTableEntry sGormanTrackEntrance0[] = {
    { SCENE_KOEPONARACE, 0, 0x4A14 },
};

static EntranceTableEntry sGormanTrackEntrance1[] = {
    { SCENE_KOEPONARACE, 1, 0x4A14 },
};

static EntranceTableEntry sGormanTrackEntrance2[] = {
    { SCENE_KOEPONARACE, 2, 0x4A14 },
};

static EntranceTableEntry sGormanTrackEntrance3[] = {
    { SCENE_KOEPONARACE, 3, 0x4A14 },
};

static EntranceTableEntry sGormanTrackEntrance4[] = {
    { SCENE_KOEPONARACE, 4, 0x0102 },
};

static EntranceTableEntry sGormanTrackEntrance5[] = {
    { SCENE_KOEPONARACE, 5, 0x0A14 },
};

static EntranceTableEntry* sGormanTrackEntranceTable[] = {
    sGormanTrackEntrance0, sGormanTrackEntrance1, sGormanTrackEntrance2,
    sGormanTrackEntrance3, sGormanTrackEntrance4, sGormanTrackEntrance5,
};

static EntranceTableEntry sGoronRacetrackEntrance0[] = {
    { SCENE_GORONRACE, 0, 0x4102 },
    { SCENE_GORONRACE, 0, 0x4102 },
};

static EntranceTableEntry sGoronRacetrackEntrance1[] = {
    { SCENE_GORONRACE, 1, 0x8A14 },
    { SCENE_GORONRACE, 1, 0x8A14 },
};

static EntranceTableEntry sGoronRacetrackEntrance2[] = {
    { SCENE_GORONRACE, 2, 0x0A14 },
    { SCENE_GORONRACE, 2, 0x0A14 },
};

static EntranceTableEntry* sGoronRacetrackEntranceTable[] = {
    sGoronRacetrackEntrance0,
    sGoronRacetrackEntrance1,
    sGoronRacetrackEntrance2,
};

static EntranceTableEntry sEastClockTownEntrance0[] = {
    { SCENE_TOWN, 0, 0x4102 },
    { SCENE_TOWN, 0, 0x8102 },
    { SCENE_TOWN, 0, 0x058B },
};

static EntranceTableEntry sEastClockTownEntrance1[] = {
    { SCENE_TOWN, 1, 0xCA14 },
};

static EntranceTableEntry sEastClockTownEntrance2[] = {
    { SCENE_TOWN, 2, 0x4102 },
};

static EntranceTableEntry sEastClockTownEntrance3[] = {
    { SCENE_TOWN, 3, 0xC102 },
};

static EntranceTableEntry sEastClockTownEntrance4[] = {
    { SCENE_TOWN, 4, 0x4102 },
};

static EntranceTableEntry sEastClockTownEntrance5[] = {
    { SCENE_TOWN, 5, 0xC102 },
};

static EntranceTableEntry sEastClockTownEntrance6[] = {
    { SCENE_TOWN, 6, 0x4102 },
};

static EntranceTableEntry sEastClockTownEntrance7[] = {
    { SCENE_TOWN, 7, 0x4102 },
};

static EntranceTableEntry sEastClockTownEntrance8[] = {
    { SCENE_TOWN, 8, 0x4102 },
};

static EntranceTableEntry sEastClockTownEntrance9[] = {
    { SCENE_TOWN, 9, 0x4102 },
};

static EntranceTableEntry sEastClockTownEntrance10[] = {
    { SCENE_TOWN, 10, 0x4102 },
};

static EntranceTableEntry sEastClockTownEntrance11[] = {
    { SCENE_TOWN, 11, 0x4102 },
};

static EntranceTableEntry sEastClockTownEntrance12[] = {
    { SCENE_TOWN, 12, 0x4102 },
};

static EntranceTableEntry* sEastClockTownEntranceTable[] = {
    sEastClockTownEntrance0,  sEastClockTownEntrance1, sEastClockTownEntrance2,  sEastClockTownEntrance3,
    sEastClockTownEntrance4,  sEastClockTownEntrance5, sEastClockTownEntrance6,  sEastClockTownEntrance7,
    sEastClockTownEntrance8,  sEastClockTownEntrance9, sEastClockTownEntrance10, sEastClockTownEntrance11,
    sEastClockTownEntrance12,
};

static EntranceTableEntry sWestClockTownEntrance0[] = {
    { SCENE_ICHIBA, 0, 0x4102 },
    { SCENE_ICHIBA, 0, 0x8102 },
};

static EntranceTableEntry sWestClockTownEntrance1[] = {
    { SCENE_ICHIBA, 1, 0xC102 },
};

static EntranceTableEntry sWestClockTownEntrance2[] = {
    { SCENE_ICHIBA, 2, 0xCA14 },
};

static EntranceTableEntry sWestClockTownEntrance3[] = {
    { SCENE_ICHIBA, 3, 0x4102 },
};

static EntranceTableEntry sWestClockTownEntrance4[] = {
    { SCENE_ICHIBA, 4, 0x4102 },
};

static EntranceTableEntry sWestClockTownEntrance5[] = {
    { SCENE_ICHIBA, 5, 0x4102 },
};

static EntranceTableEntry sWestClockTownEntrance6[] = {
    { SCENE_ICHIBA, 6, 0x4102 },
};

static EntranceTableEntry sWestClockTownEntrance7[] = {
    { SCENE_ICHIBA, 7, 0x4102 },
};

static EntranceTableEntry sWestClockTownEntrance8[] = {
    { SCENE_ICHIBA, 8, 0x4102 },
};

static EntranceTableEntry sWestClockTownEntrance9[] = {
    { SCENE_ICHIBA, 9, 0x4A14 },
};

static EntranceTableEntry* sWestClockTownEntranceTable[] = {
    sWestClockTownEntrance0, sWestClockTownEntrance1, sWestClockTownEntrance2, sWestClockTownEntrance3,
    sWestClockTownEntrance4, sWestClockTownEntrance5, sWestClockTownEntrance6, sWestClockTownEntrance7,
    sWestClockTownEntrance8, sWestClockTownEntrance9,
};

static EntranceTableEntry sNorthClockTownEntrance0[] = {
    { SCENE_BACKTOWN, 0, 0x4102 },
    { SCENE_BACKTOWN, 0, 0x458B },
};

static EntranceTableEntry sNorthClockTownEntrance1[] = {
    { SCENE_BACKTOWN, 1, 0xC102 },
};

static EntranceTableEntry sNorthClockTownEntrance2[] = {
    { SCENE_BACKTOWN, 2, 0xC102 },
};

static EntranceTableEntry sNorthClockTownEntrance3[] = {
    { SCENE_BACKTOWN, 3, 0x4102 },
};

static EntranceTableEntry sNorthClockTownEntrance4[] = {
    { SCENE_BACKTOWN, 4, 0x4102 },
};

static EntranceTableEntry sNorthClockTownEntrance5[] = {
    { SCENE_BACKTOWN, 5, 0x8A14 },
};

static EntranceTableEntry sNorthClockTownEntrance6[] = {
    { SCENE_BACKTOWN, 6, 0x8A14 },
};

static EntranceTableEntry sNorthClockTownEntrance7[] = {
    { SCENE_BACKTOWN, 7, 0x8A14 },
};

static EntranceTableEntry* sNorthClockTownEntranceTable[] = {
    sNorthClockTownEntrance0, sNorthClockTownEntrance1, sNorthClockTownEntrance2, sNorthClockTownEntrance3,
    sNorthClockTownEntrance4, sNorthClockTownEntrance5, sNorthClockTownEntrance6, sNorthClockTownEntrance7,
};

static EntranceTableEntry sSouthClockTownEntrance0[] = {
    { SCENE_CLOCKTOWER, 0, 0x4102 },
    { SCENE_CLOCKTOWER, 0, 0x8102 },
    { SCENE_CLOCKTOWER, 0, 0x0A14 },
    { SCENE_CLOCKTOWER, 0, 0x0102 },
};

static EntranceTableEntry sSouthClockTownEntrance1[] = {
    { SCENE_CLOCKTOWER, 1, 0x4102 },
    { SCENE_CLOCKTOWER, 1, 0x8102 },
};

static EntranceTableEntry sSouthClockTownEntrance2[] = {
    { SCENE_CLOCKTOWER, 2, 0xC102 },
};

static EntranceTableEntry sSouthClockTownEntrance3[] = {
    { SCENE_CLOCKTOWER, 3, 0xCA14 },
};

static EntranceTableEntry sSouthClockTownEntrance4[] = {
    { SCENE_CLOCKTOWER, 4, 0xC102 },
};

static EntranceTableEntry sSouthClockTownEntrance5[] = {
    { SCENE_CLOCKTOWER, 5, 0xC102 },
};

static EntranceTableEntry sSouthClockTownEntrance6[] = {
    { SCENE_CLOCKTOWER, 6, 0xCA14 },
};

static EntranceTableEntry sSouthClockTownEntrance7[] = {
    { SCENE_CLOCKTOWER, 7, 0xCA14 },
};

static EntranceTableEntry sSouthClockTownEntrance8[] = {
    { SCENE_CLOCKTOWER, 8, 0x4A14 },
};

static EntranceTableEntry sSouthClockTownEntrance9[] = {
    { SCENE_CLOCKTOWER, 9, 0x4A14 },
};

static EntranceTableEntry sSouthClockTownEntrance10[] = {
    { SCENE_CLOCKTOWER, 10, 0x058B },
};

static EntranceTableEntry* sSouthClockTownEntranceTable[] = {
    sSouthClockTownEntrance0, sSouthClockTownEntrance1, sSouthClockTownEntrance2,  sSouthClockTownEntrance3,
    sSouthClockTownEntrance4, sSouthClockTownEntrance5, sSouthClockTownEntrance6,  sSouthClockTownEntrance7,
    sSouthClockTownEntrance8, sSouthClockTownEntrance9, sSouthClockTownEntrance10,
};

static EntranceTableEntry sLaundryPoolEntrance0[] = {
    { SCENE_ALLEY, 0, 0xCA14 },
};

static EntranceTableEntry sLaundryPoolEntrance1[] = {
    { SCENE_ALLEY, 1, 0x4A14 },
};

static EntranceTableEntry sLaundryPoolEntrance2[] = {
    { SCENE_ALLEY, 2, 0x4102 },
};

static EntranceTableEntry* sLaundryPoolEntranceTable[] = {
    sLaundryPoolEntrance0,
    sLaundryPoolEntrance1,
    sLaundryPoolEntrance2,
};

static EntranceTableEntry sGrottosEntrance0[] = {
    { SCENE_KAKUSIANA, 0, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance1[] = {
    { SCENE_KAKUSIANA, 1, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance2[] = {
    { SCENE_KAKUSIANA, 2, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance3[] = {
    { SCENE_KAKUSIANA, 3, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance4[] = {
    { SCENE_KAKUSIANA, 4, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance5[] = {
    { SCENE_KAKUSIANA, 5, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance6[] = {
    { SCENE_KAKUSIANA, 6, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance7[] = {
    { SCENE_KAKUSIANA, SCENE_KAKUSIANA, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance8[] = {
    { SCENE_KAKUSIANA, 8, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance9[] = {
    { SCENE_KAKUSIANA, 9, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance10[] = {
    { SCENE_KAKUSIANA, 10, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance11[] = {
    { SCENE_KAKUSIANA, 11, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance12[] = {
    { SCENE_KAKUSIANA, 12, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance13[] = {
    { SCENE_KAKUSIANA, 13, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance14[] = {
    { SCENE_KAKUSIANA, 14, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance15[] = {
    { SCENE_KAKUSIANA, 15, 0x0102 },
};

static EntranceTableEntry sGrottosEntrance16[] = {
    { SCENE_KAKUSIANA, 16, 0x0102 },
};

static EntranceTableEntry* sGrottosEntranceTable[] = {
    sGrottosEntrance0,  sGrottosEntrance1,  sGrottosEntrance2,  sGrottosEntrance3,  sGrottosEntrance4,
    sGrottosEntrance5,  sGrottosEntrance6,  sGrottosEntrance7,  sGrottosEntrance8,  sGrottosEntrance9,
    sGrottosEntrance10, sGrottosEntrance11, sGrottosEntrance12, sGrottosEntrance13, sGrottosEntrance14,
    sGrottosEntrance15, sGrottosEntrance16,
};

static EntranceTableEntry sCutsceneEntrance0[] = {
    { -SCENE_SPOT00, 0, 0x0102 }, { -SCENE_SPOT00, 0, 0x0102 }, { -SCENE_SPOT00, 0, 0x8102 },
    { -SCENE_SPOT00, 0, 0x0102 }, { -SCENE_SPOT00, 0, 0x058B }, { -SCENE_SPOT00, 0, 0x058B },
    { -SCENE_SPOT00, 0, 0x058B }, { -SCENE_SPOT00, 0, 0x058B }, { -SCENE_SPOT00, 0, 0x0183 },
    { -SCENE_SPOT00, 0, 0x0183 }, { -SCENE_SPOT00, 0, 0x8102 }, { -SCENE_SPOT00, 0, 0x0102 },
};

static EntranceTableEntry sCutsceneEntrance1[] = {
    { -SCENE_SPOT00, 1, 0x0183 }, { -SCENE_SPOT00, 1, 0x0102 }, { -SCENE_SPOT00, 1, 0x8102 },
    { -SCENE_SPOT00, 1, 0x0102 }, { -SCENE_SPOT00, 1, 0x8102 }, { -SCENE_SPOT00, 1, 0x058B },
    { -SCENE_SPOT00, 1, 0x058B }, { -SCENE_SPOT00, 1, 0x058B }, { -SCENE_SPOT00, 1, 0x058B },
    { -SCENE_SPOT00, 1, 0x8102 }, { -SCENE_SPOT00, 1, 0x8102 }, { -SCENE_SPOT00, 1, 0x0102 },
};

static EntranceTableEntry sCutsceneEntrance2[] = {
    { -SCENE_SPOT00, 2, 0x0183 }, { -SCENE_SPOT00, 2, 0x0102 }, { -SCENE_SPOT00, 2, 0x8102 },
    { -SCENE_SPOT00, 2, 0x0102 }, { -SCENE_SPOT00, 2, 0x8102 }, { -SCENE_SPOT00, 2, 0x058B },
    { -SCENE_SPOT00, 2, 0x058B }, { -SCENE_SPOT00, 2, 0x058B }, { -SCENE_SPOT00, 2, 0x058B },
    { -SCENE_SPOT00, 2, 0x858B }, { -SCENE_SPOT00, 2, 0x8102 },
};

static EntranceTableEntry sCutsceneEntrance3[] = {
    { -SCENE_SPOT00, 3, 0x0183 }, { -SCENE_SPOT00, 3, 0x0102 }, { -SCENE_SPOT00, 3, 0x8102 },
    { -SCENE_SPOT00, 3, 0x0102 }, { -SCENE_SPOT00, 3, 0x058B }, { -SCENE_SPOT00, 3, 0x058B },
    { -SCENE_SPOT00, 3, 0x058B }, { -SCENE_SPOT00, 3, 0x058B }, { -SCENE_SPOT00, 3, 0x058B },
    { -SCENE_SPOT00, 3, 0x058B }, { -SCENE_SPOT00, 3, 0x8102 },
};

static EntranceTableEntry sCutsceneEntrance4[] = {
    { -SCENE_SPOT00, 4, 0x0183 }, { -SCENE_SPOT00, 4, 0x0102 }, { -SCENE_SPOT00, 4, 0x8102 },
    { -SCENE_SPOT00, 4, 0x0102 }, { -SCENE_SPOT00, 4, 0x058B }, { -SCENE_SPOT00, 4, 0x058B },
    { -SCENE_SPOT00, 4, 0x058B }, { -SCENE_SPOT00, 4, 0x058B }, { -SCENE_SPOT00, 4, 0x058B },
    { -SCENE_SPOT00, 4, 0x058B }, { -SCENE_SPOT00, 4, 0x8102 },
};

static EntranceTableEntry sCutsceneEntrance5[] = {
    { -SCENE_SPOT00, 5, 0x0183 }, { -SCENE_SPOT00, 5, 0x0102 }, { -SCENE_SPOT00, 5, 0x8102 },
    { -SCENE_SPOT00, 5, 0x0102 }, { -SCENE_SPOT00, 5, 0x058B }, { -SCENE_SPOT00, 5, 0x058B },
    { -SCENE_SPOT00, 5, 0x058B }, { -SCENE_SPOT00, 5, 0x058B }, { -SCENE_SPOT00, 5, 0x058B },
    { -SCENE_SPOT00, 5, 0x858B }, { -SCENE_SPOT00, 5, 0x8102 },
};

static EntranceTableEntry sCutsceneEntrance6[] = {
    { -SCENE_SPOT00, 6, 0x0183 }, { -SCENE_SPOT00, 6, 0x0102 }, { -SCENE_SPOT00, 6, 0x8102 },
    { -SCENE_SPOT00, 6, 0x0102 }, { -SCENE_SPOT00, 6, 0x058B }, { -SCENE_SPOT00, 6, 0x058B },
    { -SCENE_SPOT00, 6, 0x058B }, { -SCENE_SPOT00, 6, 0x058B }, { -SCENE_SPOT00, 6, 0x058B },
    { -SCENE_SPOT00, 6, 0x858B }, { -SCENE_SPOT00, 6, 0x8102 },
};

static EntranceTableEntry sCutsceneEntrance7[] = {
    { -SCENE_SPOT00, 7, 0x0183 }, { -SCENE_SPOT00, 7, 0x0102 }, { -SCENE_SPOT00, 7, 0x8102 },
    { -SCENE_SPOT00, 7, 0x0102 }, { -SCENE_SPOT00, 7, 0x058B }, { -SCENE_SPOT00, 7, 0x058B },
    { -SCENE_SPOT00, 7, 0x058B }, { -SCENE_SPOT00, 7, 0x058B }, { -SCENE_SPOT00, 7, 0x058B },
    { -SCENE_SPOT00, 7, 0x858B }, { -SCENE_SPOT00, 7, 0x8102 },
};

static EntranceTableEntry sCutsceneEntrance8[] = {
    { -SCENE_SPOT00, 8, 0x0183 }, { -SCENE_SPOT00, 8, 0x0102 }, { -SCENE_SPOT00, 8, 0x8102 },
    { -SCENE_SPOT00, 8, 0x0102 }, { -SCENE_SPOT00, 8, 0x058B }, { -SCENE_SPOT00, 8, 0x058B },
    { -SCENE_SPOT00, 8, 0x058B }, { -SCENE_SPOT00, 8, 0x058B }, { -SCENE_SPOT00, 8, 0x058B },
    { -SCENE_SPOT00, 8, 0x858B }, { -SCENE_SPOT00, 8, 0x8102 },
};

static EntranceTableEntry sCutsceneEntrance9[] = {
    { -SCENE_SPOT00, 9, 0x0183 }, { -SCENE_SPOT00, 9, 0x0102 }, { -SCENE_SPOT00, 9, 0x8102 },
    { -SCENE_SPOT00, 9, 0x0102 }, { -SCENE_SPOT00, 9, 0x058B }, { -SCENE_SPOT00, 9, 0x058B },
    { -SCENE_SPOT00, 9, 0x058B }, { -SCENE_SPOT00, 9, 0x058B }, { -SCENE_SPOT00, 9, 0x058B },
    { -SCENE_SPOT00, 9, 0x858B }, { -SCENE_SPOT00, 9, 0x8102 },
};

static EntranceTableEntry* sCutsceneEntranceTable[] = {
    sCutsceneEntrance0, sCutsceneEntrance1, sCutsceneEntrance2, sCutsceneEntrance3, sCutsceneEntrance4,
    sCutsceneEntrance5, sCutsceneEntrance6, sCutsceneEntrance7, sCutsceneEntrance8, sCutsceneEntrance9,
};

#define SCENE_ENTRANCE(entranceTable, name) \
    { ARRAY_COUNT(entranceTable), entranceTable, name }

#define SCENE_ENTRANCE_NONE() \
    { 0, NULL, NULL }

// 2Ship [Enhancement] removing the static keyword so that `sSceneEntranceTable` can be externed for BetterWarp menu
SceneEntranceTableEntry sSceneEntranceTable[] = {
    /* 0x00 */ SCENE_ENTRANCE(sMayorsResidenceEntranceTable, "Z2_SONCHONOIE"),
    /* 0x01 */ SCENE_ENTRANCE(sMajorasLairEntranceTable, "Z2_LAST_BS"),
    /* 0x02 */ SCENE_ENTRANCE(sMagicHagsPotionShopEntranceTable, "Z2_WITCH_SHOP"),
    /* 0x03 */ SCENE_ENTRANCE(sRanchHouseEntranceTable, "Z2_OMOYA"),
    /* 0x04 */ SCENE_ENTRANCE(sHoneyAndDarlingsShopEntranceTable, "Z2_BOWLING"),
    /* 0x05 */ SCENE_ENTRANCE(sBeneathTheGraveryardEntranceTable, "Z2_HAKASHITA"),
    /* 0x06 */ SCENE_ENTRANCE(sSouthernSwampClearedEntranceTable, "Z2_20SICHITAI2"),
    /* 0x07 */ SCENE_ENTRANCE(sCuriosityShopEntranceTable, "Z2_AYASHIISHOP"),
    /* 0x08 */ SCENE_ENTRANCE_NONE(),
    /* 0x09 */ SCENE_ENTRANCE_NONE(),
    /* 0x0A */ SCENE_ENTRANCE(sGrottosEntranceTable, "KAKUSIANA"),
    /* 0x0B */ SCENE_ENTRANCE_NONE(),
    /* 0x0C */ SCENE_ENTRANCE_NONE(),
    /* 0x0D */ SCENE_ENTRANCE_NONE(),
    /* 0x0E */ SCENE_ENTRANCE(sCutsceneEntranceTable, "SPOT00"),
    /* 0x0F */ SCENE_ENTRANCE_NONE(),
    /* 0x10 */ SCENE_ENTRANCE(sIkanaCanyonEntranceTable, "Z2_IKANA"),
    /* 0x11 */ SCENE_ENTRANCE(sPiratesFortressEntranceTable, "Z2_KAIZOKU"),
    /* 0x12 */ SCENE_ENTRANCE(sMilkBarEntranceTable, "Z2_MILK_BAR"),
    /* 0x13 */ SCENE_ENTRANCE(sStoneTowerTempleEntranceTable, "Z2_INISIE_N"),
    /* 0x14 */ SCENE_ENTRANCE(sTreasureChestShopEntranceTable, "Z2_TAKARAYA"),
    /* 0x15 */ SCENE_ENTRANCE(sStoneTowerTempleInvertedEntranceTable, "Z2_INISIE_R"),
    /* 0x16 */ SCENE_ENTRANCE(sClockTowerRooftopEntranceTable, "Z2_OKUJOU"),
    /* 0x17 */ SCENE_ENTRANCE(sOpeningDungeonEntranceTable, "Z2_OPENINGDAN"),
    /* 0x18 */ SCENE_ENTRANCE(sWoodfallTempleEntranceTable, "Z2_MITURIN"),
    /* 0x19 */ SCENE_ENTRANCE(sPathToMountainVillageEntranceTable, "Z2_13HUBUKINOMITI"),
    /* 0x1A */ SCENE_ENTRANCE(sIkanaCastleEntranceTable, "Z2_CASTLE"),
    /* 0x1B */ SCENE_ENTRANCE(sDekuScrubPlaygroundEntranceTable, "Z2_DEKUTES"),
    /* 0x1C */ SCENE_ENTRANCE(sOdolwasLairEntranceTable, "Z2_MITURIN_BS"),
    /* 0x1D */ SCENE_ENTRANCE(sTownShootingGalleryEntranceTable, "Z2_SYATEKI_MIZU"),
    /* 0x1E */ SCENE_ENTRANCE(sSnowheadTempleEntranceTable, "Z2_HAKUGIN"),
    /* 0x1F */ SCENE_ENTRANCE(sMilkRoadEntranceTable, "Z2_ROMANYMAE"),
    /* 0x20 */ SCENE_ENTRANCE(sPiratesFortressInteriorEntranceTable, "Z2_PIRATE"),
    /* 0x21 */ SCENE_ENTRANCE(sSwampShootingGalleryEntranceTable, "Z2_SYATEKI_MORI"),
    /* 0x22 */ SCENE_ENTRANCE(sPinnacleRockEntranceTable, "Z2_SINKAI"),
    /* 0x23 */ SCENE_ENTRANCE(sFairyFountainEntranceTable, "Z2_YOUSEI_IZUMI"),
    /* 0x24 */ SCENE_ENTRANCE(sSwampSpiderHouseEntranceTable, "Z2_KINSTA1"),
    /* 0x25 */ SCENE_ENTRANCE(sOceansideSpiderHouseEntranceTable, "Z2_KINDAN2"),
    /* 0x26 */ SCENE_ENTRANCE(sAstralObservatoryEntranceTable, "Z2_TENMON_DAI"),
    /* 0x27 */ SCENE_ENTRANCE(sMoonDekuTrialEntranceTable, "Z2_LAST_DEKU"),
    /* 0x28 */ SCENE_ENTRANCE(sDekuPalaceEntranceTable, "Z2_22DEKUCITY"),
    /* 0x29 */ SCENE_ENTRANCE(sMountainSmithyEntranceTable, "Z2_KAJIYA"),
    /* 0x2A */ SCENE_ENTRANCE(sTerminaFieldEntranceTable, "Z2_00KEIKOKU"),
    /* 0x2B */ SCENE_ENTRANCE(sPostOfficeEntranceTable, "Z2_POSTHOUSE"),
    /* 0x2C */ SCENE_ENTRANCE(sMarineResearchLabEntranceTable, "Z2_LABO"),
    /* 0x2D */ SCENE_ENTRANCE(sDampesHouseEntranceTable, "Z2_DANPEI2TEST"),
    /* 0x2E */ SCENE_ENTRANCE_NONE(),
    /* 0x2F */ SCENE_ENTRANCE(sGoronShrineEntranceTable, "Z2_16GORON_HOUSE"),
    /* 0x30 */ SCENE_ENTRANCE(sZoraHallEntranceTable, "Z2_33ZORACITY"),
    /* 0x31 */ SCENE_ENTRANCE(sTradingPostEntranceTable, "Z2_8ITEMSHOP"),
    /* 0x32 */ SCENE_ENTRANCE(sRomaniRanchEntranceTable, "Z2_F01"),
    /* 0x33 */ SCENE_ENTRANCE(sTwinmoldsLairEntranceTable, "Z2_INISIE_BS"),
    /* 0x34 */ SCENE_ENTRANCE(sGreatBayCoastEntranceTable, "Z2_30GYOSON"),
    /* 0x35 */ SCENE_ENTRANCE(sZoraCapeEntranceTable, "Z2_31MISAKI"),
    /* 0x36 */ SCENE_ENTRANCE(sLotteryShopEntranceTable, "Z2_TAKARAKUJI"),
    /* 0x37 */ SCENE_ENTRANCE_NONE(),
    /* 0x38 */ SCENE_ENTRANCE(sPiratesFortressExteriorEntranceTable, "Z2_TORIDE"),
    /* 0x39 */ SCENE_ENTRANCE(sFishermansHutEntranceTable, "Z2_FISHERMAN"),
    /* 0x3A */ SCENE_ENTRANCE(sGoronShopEntranceTable, "Z2_GORONSHOP"),
    /* 0x3B */ SCENE_ENTRANCE(sDekuKingsChamberEntranceTable, "Z2_DEKU_KING"),
    /* 0x3C */ SCENE_ENTRANCE(sMoonGoronTrialEntranceTable, "Z2_LAST_GORON"),
    /* 0x3D */ SCENE_ENTRANCE(sRoadToSouthernSwampEntranceTable, "Z2_24KEMONOMITI"),
    /* 0x3E */ SCENE_ENTRANCE(sDoggyRacetrackEntranceTable, "Z2_F01_B"),
    /* 0x3F */ SCENE_ENTRANCE(sCuccoShackEntranceTable, "Z2_F01C"),
    /* 0x40 */ SCENE_ENTRANCE(sIkanaGraveyardEntranceTable, "Z2_BOTI"),
    /* 0x41 */ SCENE_ENTRANCE(sGohtsLairEntranceTable, "Z2_HAKUGIN_BS"),
    /* 0x42 */ SCENE_ENTRANCE(sSouthernSwampPoisonedEntranceTable, "Z2_20SICHITAI"),
    /* 0x43 */ SCENE_ENTRANCE(sWoodfallEntranceTable, "Z2_21MITURINMAE"),
    /* 0x44 */ SCENE_ENTRANCE(sMoonZoraTrialEntranceTable, "Z2_LAST_ZORA"),
    /* 0x45 */ SCENE_ENTRANCE(sGoronVillageSpringEntranceTable, "Z2_11GORONNOSATO2"),
    /* 0x46 */ SCENE_ENTRANCE(sGreatBayTempleEntranceTable, "Z2_SEA"),
    /* 0x47 */ SCENE_ENTRANCE(sWaterfallRapidsEntranceTable, "Z2_35TAKI"),
    /* 0x48 */ SCENE_ENTRANCE(sBeneathTheWellEntranceTable, "Z2_REDEAD"),
    /* 0x49 */ SCENE_ENTRANCE(sZoraHallRoomsEntranceTable, "Z2_BANDROOM"),
    /* 0x4A */ SCENE_ENTRANCE(sGoronVillageWinterEntranceTable, "Z2_11GORONNOSATO"),
    /* 0x4B */ SCENE_ENTRANCE(sGoronGraveryardEntranceTable, "Z2_GORON_HAKA"),
    /* 0x4C */ SCENE_ENTRANCE(sSakonsHideoutEntranceTable, "Z2_SECOM"),
    /* 0x4D */ SCENE_ENTRANCE(sMountainVillageWinterEntranceTable, "Z2_10YUKIYAMANOMURA"),
    /* 0x4E */ SCENE_ENTRANCE(sGhostHutEntranceTable, "Z2_TOUGITES"),
    /* 0x4F */ SCENE_ENTRANCE(sDekuShrineEntranceTable, "Z2_DANPEI"),
    /* 0x50 */ SCENE_ENTRANCE(sRoadToIkanaEntranceTable, "Z2_IKANAMAE"),
    /* 0x51 */ SCENE_ENTRANCE(sSwordmansSchoolEntranceTable, "Z2_DOUJOU"),
    /* 0x52 */ SCENE_ENTRANCE(sMusicBoxHouseEntranceTable, "Z2_MUSICHOUSE"),
    /* 0x53 */ SCENE_ENTRANCE(sIgosDuIkanasLairEntranceTable, "Z2_IKNINSIDE"),
    /* 0x54 */ SCENE_ENTRANCE(sTouristInformationEntranceTable, "Z2_MAP_SHOP"),
    /* 0x55 */ SCENE_ENTRANCE(sStoneTowerEntranceTable, "Z2_F40"),
    /* 0x56 */ SCENE_ENTRANCE(sStoneTowerInvertedEntranceTable, "Z2_F41"),
    /* 0x57 */ SCENE_ENTRANCE(sMountainVillageSpringEntranceTable, "Z2_10YUKIYAMANOMURA2"),
    /* 0x58 */ SCENE_ENTRANCE(sPathToSnowheadEntranceTable, "Z2_14YUKIDAMANOMITI"),
    /* 0x59 */ SCENE_ENTRANCE(sSnowheadEntranceTable, "Z2_12HAKUGINMAE"),
    /* 0x5A */ SCENE_ENTRANCE(sPathToGoronVillageWinterEntranceTable, "Z2_17SETUGEN"),
    /* 0x5B */ SCENE_ENTRANCE(sPathToGoronVillageSpringEntranceTable, "Z2_17SETUGEN2"),
    /* 0x5C */ SCENE_ENTRANCE(sGyorgsLairEntranceTable, "Z2_SEA_BS"),
    /* 0x5D */ SCENE_ENTRANCE(sSecretShrineEntranceTable, "Z2_RANDOM"),
    /* 0x5E */ SCENE_ENTRANCE(sStockPotInnEntranceTable, "Z2_YADOYA"),
    /* 0x5F */ SCENE_ENTRANCE(sGreatBayCutsceneEntranceTable, "Z2_KONPEKI_ENT"),
    /* 0x60 */ SCENE_ENTRANCE(sClockTowerInteriorEntranceTable, "Z2_INSIDETOWER"),
    /* 0x61 */ SCENE_ENTRANCE(sWoodsOfMysteryEntranceTable, "Z2_26SARUNOMORI"),
    /* 0x62 */ SCENE_ENTRANCE(sLostWoodsEntranceTable, "Z2_LOST_WOODS"),
    /* 0x63 */ SCENE_ENTRANCE(sMoonLinkTrialEntranceTable, "Z2_LAST_LINK"),
    /* 0x64 */ SCENE_ENTRANCE(sTheMoonEntranceTable, "Z2_SOUGEN"),
    /* 0x65 */ SCENE_ENTRANCE(sBombShopEntranceTable, "Z2_BOMYA"),
    /* 0x66 */ SCENE_ENTRANCE(sGiantsChamberEntranceTable, "Z2_KYOJINNOMA"),
    /* 0x67 */ SCENE_ENTRANCE(sGormanTrackEntranceTable, "Z2_KOEPONARACE"),
    /* 0x68 */ SCENE_ENTRANCE(sGoronRacetrackEntranceTable, "Z2_GORONRACE"),
    /* 0x69 */ SCENE_ENTRANCE(sEastClockTownEntranceTable, "Z2_TOWN"),
    /* 0x6A */ SCENE_ENTRANCE(sWestClockTownEntranceTable, "Z2_ICHIBA"),
    /* 0x6B */ SCENE_ENTRANCE(sNorthClockTownEntranceTable, "Z2_BACKTOWN"),
    /* 0x6C */ SCENE_ENTRANCE(sSouthClockTownEntranceTable, "Z2_CLOCKTOWER"),
    /* 0x6D */ SCENE_ENTRANCE(sLaundryPoolEntranceTable, "Z2_ALLEY"),
};

/**
 * Returns a pointer to an entrance table from a given entrance index.
 */
EntranceTableEntry* Entrance_GetTableEntry(u16 entrance) {
    u32 entranceIndex = entrance;
    EntranceTableEntry** tableEntryP = sSceneEntranceTable[entranceIndex >> 9].table;
    EntranceTableEntry* tableEntry = tableEntryP[(entranceIndex >> 4) & 0x1F];

    return &tableEntry[entranceIndex & 0xF];
}

/**
 * Returns the scene index from a given entrance index.
 */
s32 Entrance_GetSceneId(u16 entrance) {
    EntranceTableEntry* tableEntry = Entrance_GetTableEntry(entrance);

    return tableEntry->sceneId;
}

/**
 * Returns the absolute value scene index (since for some reason some of them are negative) from a given entrance index.
 */
s32 Entrance_GetSceneIdAbsolute(u16 entrance) {
    EntranceTableEntry* tableEntry = Entrance_GetTableEntry(entrance);

    return ABS_ALT(tableEntry->sceneId);
}

/**
 * Returns the spawn index from a given entrance index.
 */
s32 Entrance_GetSpawnNum(u16 entrance) {
    EntranceTableEntry* tableEntry = Entrance_GetTableEntry(entrance);

    return tableEntry->spawnNum;
}

/**
 * Returns the transition effect flags from a given entrance index.
 */
s32 Entrance_GetTransitionFlags(u16 entrance) {
    EntranceTableEntry* tableEntry = Entrance_GetTableEntry(entrance);

    return tableEntry->flags;
}
