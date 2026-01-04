#include "z64.h"
#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
#include "textures/icon_item_dungeon_static/icon_item_dungeon_static.h"
#include "textures/parameter_static/parameter_static.h"
#include "textures/nes_font_static/nes_font_static.h"
#include "soh_assets.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/SaveManager.h"
#include "soh/frame_interpolation.h"
#include "ship/utils/color.h"
#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "src/overlays/gamestates/ovl_file_choose/file_choose.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "assets/textures/title_static/title_static.h"

/*
 * TODO:
 *  Ocarina Buttons
 *  Swim
 *  Keys
 *  Boss Keys
 *  Boss Souls
 */

#define Save_GetSaveMetaInfo(fileIndex) (&SaveManager::Instance->fileMetaInfo[fileIndex])

typedef struct IconPosition {
    s16 left;
    s16 top;
} IconPosition;

typedef struct IconSize {
    s16 width;
    s16 height;
} IconSize;

typedef struct ItemData {
    Sprite sprite;
    Color_RGBA8 color;
    u8 item;
    IconPosition pos;
    IconSize size;
} ItemData;

#define CREATE_SPRITE_24(iconTex, spriteId)                       \
    { iconTex, 24, 24, G_IM_FMT_RGBA, G_IM_SIZ_32b, spriteId }, { \
        0xFF, 0xFF, 0xFF, 0xFF                                    \
    }
#define CREATE_SPRITE_32(iconTex, spriteId)                       \
    { iconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, spriteId }, { \
        0xFF, 0xFF, 0xFF, 0xFF                                    \
    }
#define CREATE_SPRITE_SONG(colorR, colorG, colorB)              \
    { dgSongNoteTex, 16, 24, G_IM_FMT_IA, G_IM_SIZ_8b, 100 }, { \
        colorR, colorG, colorB, 0xFF                            \
    }
#define CREATE_SPRITE_RUPEE(colorR, colorG, colorB)                     \
    { dgRupeeCounterIconTex, 16, 16, G_IM_FMT_IA, G_IM_SIZ_8b, 102 }, { \
        colorR, colorG, colorB, 0xFF                                    \
    }
#define CREATE_SPRITE_SKULL                                               \
    { dgDungeonMapSkullTex, 16, 16, G_IM_FMT_RGBA, G_IM_SIZ_16b, 104 }, { \
        0xFF, 0xFF, 0xFF, 0xFF                                            \
    }
#define CREATE_SPRITE_COUNTER_DIGIT(i) \
    { dgAmmoDigit##i##Tex, 8, 8, G_IM_FMT_IA, G_IM_SIZ_8b, 105 + i }
#define CREATE_SPRITE_OCARINA_BUTTON(iconTex, spriteId)       \
    { iconTex, 16, 16, G_IM_FMT_I, G_IM_SIZ_4b, spriteId }, { \
        0xFF, 0xFF, 0xFF, 0xFF                                \
    }

#define ICON_SIZE 12
#define COUNTER_SIZE 16
#define SONG_WIDTH 8
#define SONG_HEIGHT 12

#define LEFT_OFFSET (int)0x37
#define TOP_OFFSET (int)0x5C

#define COUNTER_DIGITS_LEFT_OFFSET COUNTER_SIZE / 2 - 3
#define COUNTER_DIGITS_TOP_OFFSET COUNTER_SIZE - 3

#define SIZE_NORMAL \
    { ICON_SIZE, ICON_SIZE }
#define SIZE_COUNTER \
    { COUNTER_SIZE, COUNTER_SIZE }
#define SIZE_SONG \
    { SONG_WIDTH, SONG_HEIGHT }

// the format exclusion is because clang-format thinks that the * signs are pointers
// clang-format off
#define INVENTORY_ICON_POS(x, y) \
    { 0x4E + ICON_SIZE * x, 0x00 + ICON_SIZE * y }
#define EQUIPMENT_ICON_POS(x, y) \
    { 0x7E + ICON_SIZE * x, 0x2A + ICON_SIZE * y }
#define SONG_ICON_POS(x, y) \
    { 0x49 + SONG_WIDTH * x, 0x45 + SONG_HEIGHT * y }
#define UPGRADE_ICON_POS(x, y) \
    { 0x5A + ICON_SIZE * x, 0x2A + ICON_SIZE * y }
#define STONE_ICON_POS(i) \
    { 0x29 + ICON_SIZE * i, 0x31 }
#define OCARINA_BUTTON_ICON_POS(i) \
    { 0xA8 + ICON_SIZE * i, 0x00 }
#define RANDO_ONLY_ITEM_ICON_POS(i) \
    { 0xA8 + ICON_SIZE * i, 0x2A }
// clang-format on

static ItemData itemData[] = {
    { CREATE_SPRITE_32(dgItemIconDekuStickTex, 1), ITEM_STICK, INVENTORY_ICON_POS(0, 0), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconDekuNutTex, 0), ITEM_NUT, INVENTORY_ICON_POS(1, 0), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconBombTex, 2), ITEM_BOMB, INVENTORY_ICON_POS(2, 0), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconBowTex, 3), ITEM_BOW, INVENTORY_ICON_POS(3, 0), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconArrowFireTex, 4), ITEM_ARROW_FIRE, INVENTORY_ICON_POS(4, 0), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconDinsFireTex, 5), ITEM_DINS_FIRE, INVENTORY_ICON_POS(5, 0), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconBottleEmptyTex, 20), ITEM_BOTTLE, INVENTORY_ICON_POS(6, 0), SIZE_NORMAL },

    { CREATE_SPRITE_32(dgItemIconSlingshotTex, 6), ITEM_SLINGSHOT, INVENTORY_ICON_POS(0, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconOcarinaFairyTex, 7), ITEM_OCARINA_FAIRY, INVENTORY_ICON_POS(1, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconOcarinaOfTimeTex, 7), ITEM_OCARINA_TIME, INVENTORY_ICON_POS(1, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconBombchuTex, 9), ITEM_BOMBCHU, INVENTORY_ICON_POS(2, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconHookshotTex, 10), ITEM_HOOKSHOT, INVENTORY_ICON_POS(3, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconLongshotTex, 10), ITEM_LONGSHOT, INVENTORY_ICON_POS(3, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconArrowIceTex, 12), ITEM_ARROW_ICE, INVENTORY_ICON_POS(4, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconFaroresWindTex, 13), ITEM_FARORES_WIND, INVENTORY_ICON_POS(5, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconWeirdEggTex, 37), ITEM_WEIRD_EGG, INVENTORY_ICON_POS(6, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconChickenTex, 37), ITEM_CHICKEN, INVENTORY_ICON_POS(6, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconZeldasLetterTex, 37), ITEM_LETTER_ZELDA, INVENTORY_ICON_POS(6, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconMaskKeatonTex, 37), ITEM_MASK_KEATON, INVENTORY_ICON_POS(6, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconMaskSkullTex, 37), ITEM_MASK_SKULL, INVENTORY_ICON_POS(6, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconMaskSpookyTex, 37), ITEM_MASK_SPOOKY, INVENTORY_ICON_POS(6, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconMaskBunnyHoodTex, 37), ITEM_MASK_BUNNY, INVENTORY_ICON_POS(6, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconMaskGoronTex, 37), ITEM_MASK_GORON, INVENTORY_ICON_POS(6, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconMaskZoraTex, 37), ITEM_MASK_ZORA, INVENTORY_ICON_POS(6, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconMaskGerudoTex, 37), ITEM_MASK_GERUDO, INVENTORY_ICON_POS(6, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconMaskTruthTex, 37), ITEM_MASK_TRUTH, INVENTORY_ICON_POS(6, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconSoldOutTex, 37), ITEM_SOLD_OUT, INVENTORY_ICON_POS(6, 1), SIZE_NORMAL },

    { CREATE_SPRITE_32(dgItemIconBoomerangTex, 14), ITEM_BOOMERANG, INVENTORY_ICON_POS(0, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconLensOfTruthTex, 15), ITEM_LENS, INVENTORY_ICON_POS(1, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconMagicBeanTex, 16), ITEM_BEAN, INVENTORY_ICON_POS(2, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconHammerTex, 17), ITEM_HAMMER, INVENTORY_ICON_POS(3, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconArrowLightTex, 18), ITEM_ARROW_LIGHT, INVENTORY_ICON_POS(4, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconNayrusLoveTex, 19), ITEM_NAYRUS_LOVE, INVENTORY_ICON_POS(5, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconPocketEggTex, 53), ITEM_POCKET_EGG, INVENTORY_ICON_POS(6, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconPocketCuccoTex, 53), ITEM_POCKET_CUCCO, INVENTORY_ICON_POS(6, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconCojiroTex, 53), ITEM_COJIRO, INVENTORY_ICON_POS(6, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconOddMushroomTex, 53), ITEM_ODD_MUSHROOM, INVENTORY_ICON_POS(6, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconOddPotionTex, 53), ITEM_ODD_POTION, INVENTORY_ICON_POS(6, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconPoachersSawTex, 53), ITEM_SAW, INVENTORY_ICON_POS(6, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconBrokenGoronsSwordTex, 53), ITEM_SWORD_BROKEN, INVENTORY_ICON_POS(6, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconPrescriptionTex, 53), ITEM_PRESCRIPTION, INVENTORY_ICON_POS(6, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconEyeballFrogTex, 53), ITEM_FROG, INVENTORY_ICON_POS(6, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconEyeDropsTex, 53), ITEM_EYEDROPS, INVENTORY_ICON_POS(6, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconClaimCheckTex, 53), ITEM_CLAIM_CHECK, INVENTORY_ICON_POS(6, 2), SIZE_NORMAL },

    { CREATE_SPRITE_32(dgItemIconSwordKokiriTex, 54), ITEM_SWORD_KOKIRI, EQUIPMENT_ICON_POS(0, 0), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconSwordMasterTex, 55), ITEM_SWORD_MASTER, EQUIPMENT_ICON_POS(1, 0), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconSwordBiggoronTex, 56), ITEM_SWORD_BGS, EQUIPMENT_ICON_POS(2, 0), SIZE_NORMAL },

    { CREATE_SPRITE_32(dgItemIconShieldDekuTex, 57), ITEM_SHIELD_DEKU, EQUIPMENT_ICON_POS(0, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconShieldHylianTex, 58), ITEM_SHIELD_HYLIAN, EQUIPMENT_ICON_POS(1, 1), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconShieldMirrorTex, 59), ITEM_SHIELD_MIRROR, EQUIPMENT_ICON_POS(2, 1), SIZE_NORMAL },

    { CREATE_SPRITE_32(dgItemIconTunicKokiriTex, 60), ITEM_TUNIC_KOKIRI, EQUIPMENT_ICON_POS(0, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconTunicGoronTex, 61), ITEM_TUNIC_GORON, EQUIPMENT_ICON_POS(1, 2), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconTunicZoraTex, 62), ITEM_TUNIC_ZORA, EQUIPMENT_ICON_POS(2, 2), SIZE_NORMAL },

    { CREATE_SPRITE_32(dgItemIconBootsKokiriTex, 63), ITEM_BOOTS_KOKIRI, EQUIPMENT_ICON_POS(0, 3), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconBootsIronTex, 64), ITEM_BOOTS_IRON, EQUIPMENT_ICON_POS(1, 3), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconBootsHoverTex, 65), ITEM_BOOTS_HOVER, EQUIPMENT_ICON_POS(2, 3), SIZE_NORMAL },

    { CREATE_SPRITE_24(dgQuestIconKokiriEmeraldTex, 87), ITEM_KOKIRI_EMERALD, STONE_ICON_POS(-1), SIZE_NORMAL },
    { CREATE_SPRITE_24(dgQuestIconGoronRubyTex, 88), ITEM_GORON_RUBY, STONE_ICON_POS(0), SIZE_NORMAL },
    { CREATE_SPRITE_24(dgQuestIconZoraSapphireTex, 89), ITEM_ZORA_SAPPHIRE, STONE_ICON_POS(1), SIZE_NORMAL },

    { CREATE_SPRITE_24(dgQuestIconMedallionForestTex, 81), ITEM_MEDALLION_FOREST, { 0x37, 0x0A }, SIZE_NORMAL },
    { CREATE_SPRITE_24(dgQuestIconMedallionFireTex, 82), ITEM_MEDALLION_FIRE, { 0x37, 0x1A }, SIZE_NORMAL },
    { CREATE_SPRITE_24(dgQuestIconMedallionWaterTex, 83), ITEM_MEDALLION_WATER, { 0x29, 0x22 }, SIZE_NORMAL },
    { CREATE_SPRITE_24(dgQuestIconMedallionSpiritTex, 84), ITEM_MEDALLION_SPIRIT, { 0x1B, 0x1A }, SIZE_NORMAL },
    { CREATE_SPRITE_24(dgQuestIconMedallionShadowTex, 85), ITEM_MEDALLION_SHADOW, { 0x1B, 0x0A }, SIZE_NORMAL },
    { CREATE_SPRITE_24(dgQuestIconMedallionLightTex, 86), ITEM_MEDALLION_LIGHT, { 0x29, 0x02 }, SIZE_NORMAL },

    { CREATE_SPRITE_32(dgItemIconGoronsBraceletTex, 71), ITEM_BRACELET, UPGRADE_ICON_POS(0, 0), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconSilverGauntletsTex, 71), ITEM_GAUNTLETS_SILVER, UPGRADE_ICON_POS(0, 0), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconGoldenGauntletsTex, 71), ITEM_GAUNTLETS_GOLD, UPGRADE_ICON_POS(0, 0), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconScaleSilverTex, 74), ITEM_SCALE_SILVER, UPGRADE_ICON_POS(1, 0), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconScaleGoldenTex, 74), ITEM_SCALE_GOLDEN, UPGRADE_ICON_POS(1, 0), SIZE_NORMAL },
    { CREATE_SPRITE_24(dgQuestIconMagicJarSmallTex, 97), ITEM_SINGLE_MAGIC, UPGRADE_ICON_POS(2, 0), SIZE_NORMAL },
    { CREATE_SPRITE_24(dgQuestIconMagicJarBigTex, 97), ITEM_DOUBLE_MAGIC, UPGRADE_ICON_POS(2, 0), SIZE_NORMAL },
    { CREATE_SPRITE_24(dgQuestIconGerudosCardTex, 91), ITEM_GERUDO_CARD, UPGRADE_ICON_POS(1, 1), SIZE_NORMAL },
    { CREATE_SPRITE_24(dgQuestIconStoneOfAgonyTex, 90), ITEM_STONE_OF_AGONY, UPGRADE_ICON_POS(2, 1), SIZE_NORMAL },

    { CREATE_SPRITE_SONG(224, 107, 255), ITEM_SONG_LULLABY, SONG_ICON_POS(0, 0), SIZE_SONG },
    { CREATE_SPRITE_SONG(255, 195, 60), ITEM_SONG_EPONA, SONG_ICON_POS(1, 0), SIZE_SONG },
    { CREATE_SPRITE_SONG(127, 255, 137), ITEM_SONG_SARIA, SONG_ICON_POS(2, 0), SIZE_SONG },
    { CREATE_SPRITE_SONG(255, 255, 60), ITEM_SONG_SUN, SONG_ICON_POS(3, 0), SIZE_SONG },
    { CREATE_SPRITE_SONG(119, 236, 255), ITEM_SONG_TIME, SONG_ICON_POS(4, 0), SIZE_SONG },
    { CREATE_SPRITE_SONG(165, 165, 165), ITEM_SONG_STORMS, SONG_ICON_POS(5, 0), SIZE_SONG },
    { CREATE_SPRITE_SONG(150, 255, 100), ITEM_SONG_MINUET, SONG_ICON_POS(0, 1), SIZE_SONG },
    { CREATE_SPRITE_SONG(255, 80, 40), ITEM_SONG_BOLERO, SONG_ICON_POS(1, 1), SIZE_SONG },
    { CREATE_SPRITE_SONG(100, 150, 255), ITEM_SONG_SERENADE, SONG_ICON_POS(2, 1), SIZE_SONG },
    { CREATE_SPRITE_SONG(255, 160, 0), ITEM_SONG_REQUIEM, SONG_ICON_POS(3, 1), SIZE_SONG },
    { CREATE_SPRITE_SONG(255, 100, 255), ITEM_SONG_NOCTURNE, SONG_ICON_POS(4, 1), SIZE_SONG },
    { CREATE_SPRITE_SONG(255, 240, 100), ITEM_SONG_PRELUDE, SONG_ICON_POS(5, 1), SIZE_SONG },

    { CREATE_SPRITE_24(dgQuestIconHeartContainerTex, 101), ITEM_DOUBLE_DEFENSE, { 0x05, -0x04 }, SIZE_COUNTER },

    // using 0xB0 and later as they're not real item ids
    // TODO: improve this (RandomizerGet doesn't work)

    // disabled due to lus issue
    /*
    { CREATE_SPRITE_OCARINA_BUTTON(dgMsgChar9FButtonATex, 115), 0xB0, OCARINA_BUTTON_ICON_POS(0), SIZE_NORMAL },
    { CREATE_SPRITE_OCARINA_BUTTON(dgMsgCharA5ButtonCUpTex, 116), 0xB1, OCARINA_BUTTON_ICON_POS(1), SIZE_NORMAL },
    { CREATE_SPRITE_OCARINA_BUTTON(dgMsgCharA6ButtonCDownTex, 117), 0xB2, OCARINA_BUTTON_ICON_POS(2), SIZE_NORMAL },
    { CREATE_SPRITE_OCARINA_BUTTON(dgMsgCharA7ButtonCLeftTex, 118), 0xB3, OCARINA_BUTTON_ICON_POS(3), SIZE_NORMAL },
    { CREATE_SPRITE_OCARINA_BUTTON(dgMsgCharA8ButtonCRightTex, 119), 0xB4, OCARINA_BUTTON_ICON_POS(4), SIZE_NORMAL },
    */

    { CREATE_SPRITE_RUPEE(0xC8, 0xFF, 0x64), ITEM_RUPEE_GREEN, RANDO_ONLY_ITEM_ICON_POS(0), SIZE_NORMAL },
    { CREATE_SPRITE_32(dgItemIconFishingPoleTex, 120), ITEM_FISHING_POLE, RANDO_ONLY_ITEM_ICON_POS(1), SIZE_NORMAL },
};

static u8 ColorProduct(u8 c1, u8 c2) {
    u16 prod = (u16)c1 * (u16)c2;
    u16 div255 = (prod + 1 + (prod >> 8)) >> 8;
    return (u8)div255;
}

static const Color_RGBA8 DIM = { 0x40, 0x40, 0x40, 0x90 };

extern "C" void SpriteLoad(FileChooseContext* thisx, Sprite* sprite);
extern "C" void SpriteDraw(FileChooseContext* thisx, Sprite* sprite, int left, int top, int width, int height);

static bool HasItem(s16 fileIndex, u8 item) {
    for (int i = 0; i < ARRAY_COUNT(Save_GetSaveMetaInfo(fileIndex)->inventoryItems); i += 1) {
        u8 it = Save_GetSaveMetaInfo(fileIndex)->inventoryItems[i];
        if (it == item || (item == ITEM_BOTTLE && it >= ITEM_BOTTLE && it <= ITEM_POE)) {
            return true;
        }
    }

    if (item >= ITEM_SONG_MINUET && item <= ITEM_SONG_STORMS) {
        return (Save_GetSaveMetaInfo(fileIndex)->questItems & (1 << (item - 0x54))) != 0;
    }

    if (item >= ITEM_MEDALLION_FOREST && item <= ITEM_MEDALLION_LIGHT) {
        return (Save_GetSaveMetaInfo(fileIndex)->questItems & (1 << (item - 0x66))) != 0;
    }

    if (item >= ITEM_KOKIRI_EMERALD && item <= ITEM_GERUDO_CARD) {
        return (Save_GetSaveMetaInfo(fileIndex)->questItems & (1 << (item - 0x5A))) != 0;
    }

    if (item >= ITEM_SWORD_KOKIRI && item <= ITEM_SWORD_BGS) {
        return (Save_GetSaveMetaInfo(fileIndex)->equipment & (1 << (item - 0x3B))) != 0;
    }

    if (item >= ITEM_SHIELD_DEKU && item <= ITEM_SHIELD_MIRROR) {
        return (Save_GetSaveMetaInfo(fileIndex)->equipment & (1 << (item - 0x3E + 4))) != 0;
    }

    if (item >= ITEM_TUNIC_KOKIRI && item <= ITEM_TUNIC_ZORA) {
        return (Save_GetSaveMetaInfo(fileIndex)->equipment & (1 << (item - 0x41 + 8))) != 0;
    }

    if (item >= ITEM_BOOTS_KOKIRI && item <= ITEM_BOOTS_HOVER) {
        return (Save_GetSaveMetaInfo(fileIndex)->equipment & (1 << (item - 0x44 + 12))) != 0;
    }

    if (item == ITEM_SINGLE_MAGIC) {
        return Save_GetSaveMetaInfo(fileIndex)->isMagicAcquired;
    }

    if (item == ITEM_DOUBLE_MAGIC) {
        return Save_GetSaveMetaInfo(fileIndex)->isDoubleMagicAcquired;
    }

    if (item == ITEM_BRACELET) {
        return ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_STRENGTH]) >>
                gUpgradeShifts[UPG_STRENGTH]) == 1;
    }

    if (item == ITEM_GAUNTLETS_SILVER) {
        return ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_STRENGTH]) >>
                gUpgradeShifts[UPG_STRENGTH]) == 2;
    }

    if (item == ITEM_GAUNTLETS_GOLD) {
        return ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_STRENGTH]) >>
                gUpgradeShifts[UPG_STRENGTH]) == 3;
    }

    if (item == ITEM_SCALE_SILVER) {
        return ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_SCALE]) >> gUpgradeShifts[UPG_SCALE]) ==
               1;
    }

    if (item == ITEM_SCALE_GOLDEN) {
        return ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_SCALE]) >> gUpgradeShifts[UPG_SCALE]) ==
               2;
    }

    if (item == ITEM_DOUBLE_DEFENSE) {
        return Save_GetSaveMetaInfo(fileIndex)->isDoubleDefenseAcquired;
    }

    // greg
    if (item == ITEM_RUPEE_GREEN) {
        return Save_GetSaveMetaInfo(fileIndex)->gregFound;
    }

    if (item == ITEM_FISHING_POLE) {
        return Save_GetSaveMetaInfo(fileIndex)->hasFishingRod;
    }

    return false;
}

static bool ShouldRenderItem(s16 fileIndex, u8 item) {
    // strength
    if (item == ITEM_BRACELET &&
        (HasItem(fileIndex, ITEM_GAUNTLETS_SILVER) || HasItem(fileIndex, ITEM_GAUNTLETS_GOLD))) {
        return false;
    }

    if (item == ITEM_GAUNTLETS_SILVER && !HasItem(fileIndex, ITEM_GAUNTLETS_SILVER)) {
        return false;
    }

    if (item == ITEM_GAUNTLETS_GOLD && !HasItem(fileIndex, ITEM_GAUNTLETS_GOLD)) {
        return false;
    }

    // magic
    if (item == ITEM_SINGLE_MAGIC && HasItem(fileIndex, ITEM_DOUBLE_MAGIC)) {
        return false;
    }

    if (item == ITEM_DOUBLE_MAGIC && !HasItem(fileIndex, ITEM_DOUBLE_MAGIC)) {
        return false;
    }

    // scales
    if (item == ITEM_SCALE_SILVER && HasItem(fileIndex, ITEM_SCALE_GOLDEN)) {
        return false;
    }

    if (item == ITEM_SCALE_GOLDEN && !HasItem(fileIndex, ITEM_SCALE_GOLDEN)) {
        return false;
    }

    // hookshot/longshot
    if (item == ITEM_HOOKSHOT && HasItem(fileIndex, ITEM_LONGSHOT)) {
        return false;
    }

    if (item == ITEM_LONGSHOT && !HasItem(fileIndex, ITEM_LONGSHOT)) {
        return false;
    }

    // ocarinas
    if (item == ITEM_OCARINA_FAIRY && HasItem(fileIndex, ITEM_OCARINA_TIME)) {
        return false;
    }

    if (item == ITEM_OCARINA_TIME && !HasItem(fileIndex, ITEM_OCARINA_TIME)) {
        return false;
    }

    // trade child
    if (item == ITEM_WEIRD_EGG && !HasItem(fileIndex, ITEM_WEIRD_EGG)) {
        return false;
    }

    if (item == ITEM_CHICKEN && !HasItem(fileIndex, ITEM_CHICKEN)) {
        return false;
    }

    if (item == ITEM_LETTER_ZELDA && !HasItem(fileIndex, ITEM_LETTER_ZELDA)) {
        return false;
    }

    if (item == ITEM_MASK_KEATON && (HasItem(fileIndex, ITEM_WEIRD_EGG) || HasItem(fileIndex, ITEM_CHICKEN) ||
                                     HasItem(fileIndex, ITEM_LETTER_ZELDA) || HasItem(fileIndex, ITEM_MASK_SKULL) ||
                                     HasItem(fileIndex, ITEM_MASK_SPOOKY) || HasItem(fileIndex, ITEM_MASK_BUNNY) ||
                                     HasItem(fileIndex, ITEM_MASK_GORON) || HasItem(fileIndex, ITEM_MASK_ZORA) ||
                                     HasItem(fileIndex, ITEM_MASK_GERUDO) || HasItem(fileIndex, ITEM_MASK_TRUTH) ||
                                     HasItem(fileIndex, ITEM_SOLD_OUT))) {
        return false;
    }

    if (item == ITEM_MASK_SKULL && !HasItem(fileIndex, ITEM_MASK_SKULL)) {
        return false;
    }

    if (item == ITEM_MASK_SPOOKY && !HasItem(fileIndex, ITEM_MASK_SPOOKY)) {
        return false;
    }

    if (item == ITEM_MASK_BUNNY && !HasItem(fileIndex, ITEM_MASK_BUNNY)) {
        return false;
    }

    if (item == ITEM_MASK_GORON && !HasItem(fileIndex, ITEM_MASK_GORON)) {
        return false;
    }

    if (item == ITEM_MASK_ZORA && !HasItem(fileIndex, ITEM_MASK_ZORA)) {
        return false;
    }

    if (item == ITEM_MASK_GERUDO && !HasItem(fileIndex, ITEM_MASK_GERUDO)) {
        return false;
    }

    if (item == ITEM_MASK_TRUTH && !HasItem(fileIndex, ITEM_MASK_TRUTH)) {
        return false;
    }

    if (item == ITEM_SOLD_OUT && !HasItem(fileIndex, ITEM_SOLD_OUT)) {
        return false;
    }

    // trade adult
    if (item == ITEM_POCKET_EGG &&
        (HasItem(fileIndex, ITEM_POCKET_CUCCO) || HasItem(fileIndex, ITEM_COJIRO) ||
         HasItem(fileIndex, ITEM_ODD_MUSHROOM) || HasItem(fileIndex, ITEM_ODD_POTION) || HasItem(fileIndex, ITEM_SAW) ||
         HasItem(fileIndex, ITEM_SWORD_BROKEN) || HasItem(fileIndex, ITEM_PRESCRIPTION) ||
         HasItem(fileIndex, ITEM_FROG) || HasItem(fileIndex, ITEM_EYEDROPS) || HasItem(fileIndex, ITEM_CLAIM_CHECK))) {
        return false;
    }

    if (item == ITEM_POCKET_CUCCO && !HasItem(fileIndex, ITEM_POCKET_CUCCO)) {
        return false;
    }

    if (item == ITEM_COJIRO && !HasItem(fileIndex, ITEM_COJIRO)) {
        return false;
    }

    if (item == ITEM_ODD_MUSHROOM && !HasItem(fileIndex, ITEM_ODD_MUSHROOM)) {
        return false;
    }

    if (item == ITEM_ODD_POTION && !HasItem(fileIndex, ITEM_ODD_POTION)) {
        return false;
    }

    if (item == ITEM_SAW && !HasItem(fileIndex, ITEM_SAW)) {
        return false;
    }

    if (item == ITEM_SWORD_BROKEN && !HasItem(fileIndex, ITEM_SWORD_BROKEN)) {
        return false;
    }

    if (item == ITEM_PRESCRIPTION && !HasItem(fileIndex, ITEM_PRESCRIPTION)) {
        return false;
    }

    if (item == ITEM_FROG && !HasItem(fileIndex, ITEM_FROG)) {
        return false;
    }

    if (item == ITEM_EYEDROPS && !HasItem(fileIndex, ITEM_EYEDROPS)) {
        return false;
    }

    if (item == ITEM_CLAIM_CHECK && !HasItem(fileIndex, ITEM_CLAIM_CHECK)) {
        return false;
    }

    if (item == ITEM_DOUBLE_DEFENSE && !Save_GetSaveMetaInfo(fileIndex)->isDoubleDefenseAcquired) {
        return false;
    }

    // greg
    if (item == ITEM_RUPEE_GREEN) {
        return Save_GetSaveMetaInfo(fileIndex)->randoSave;
    }

    if (item == ITEM_FISHING_POLE) {
        return Save_GetSaveMetaInfo(fileIndex)->fishingPoleShuffled;
    }

    return true;
}

static void DrawItems(FileChooseContext* thisx, s16 fileIndex, u8 alpha) {
    OPEN_DISPS(thisx->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    for (int i = 0; i < ARRAY_COUNT(itemData); i += 1) {
        ItemData* data = &itemData[i];

        if (ShouldRenderItem(fileIndex, data->item)) {
            if (HasItem(fileIndex, data->item)) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, data->color.r, data->color.g, data->color.b,
                                ColorProduct(data->color.a, alpha));
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, ColorProduct(data->color.r, DIM.r),
                                ColorProduct(data->color.g, DIM.g), ColorProduct(data->color.b, DIM.b),
                                ColorProduct(ColorProduct(data->color.a, DIM.a), alpha));
            }

            SpriteLoad(thisx, &(data->sprite));
            SpriteDraw(thisx, &(data->sprite), LEFT_OFFSET + data->pos.left, TOP_OFFSET + data->pos.top,
                       data->size.width, data->size.height);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    CLOSE_DISPS(thisx->state.gfxCtx);
}

typedef enum CounterID {
    /* 0x00 */ COUNTER_HEALTH,
    /* 0x01 */ COUNTER_WALLET_NONE,
    /* 0x02 */ COUNTER_WALLET_CHILD,
    /* 0x03 */ COUNTER_WALLET_ADULT,
    /* 0x04 */ COUNTER_WALLET_GIANT,
    /* 0x05 */ COUNTER_WALLET_TYCOON,
    /* 0x06 */ COUNTER_SKULLTULLAS,
    /* 0x07 */ COUNTER_DEATHS,
    /* 0x08 */ COUNTER_TRIFORCE_PIECES,
    /* 0x09 */ COUNTER_MAX,
} CounterID;

typedef struct CounterData {
    Sprite sprite;
    Color_RGBA8 color;
    CounterID id;
    IconPosition pos;
    IconSize size;
} CounterData;

static CounterData counterData[COUNTER_MAX] = {
    { CREATE_SPRITE_24(dgQuestIconHeartContainerTex, 101), COUNTER_HEALTH, { 0x05, 0x00 }, SIZE_COUNTER },
    { CREATE_SPRITE_RUPEE(0x32, 0x40, 0x19), COUNTER_WALLET_NONE, { 0x05, 0x15 }, SIZE_COUNTER },
    { CREATE_SPRITE_RUPEE(0xC8, 0xFF, 0x64), COUNTER_WALLET_CHILD, { 0x05, 0x15 }, SIZE_COUNTER },
    { CREATE_SPRITE_RUPEE(0x82, 0x82, 0xFF), COUNTER_WALLET_ADULT, { 0x05, 0x15 }, SIZE_COUNTER },
    { CREATE_SPRITE_RUPEE(0xFF, 0x64, 0x64), COUNTER_WALLET_GIANT, { 0x05, 0x15 }, SIZE_COUNTER },
    { CREATE_SPRITE_RUPEE(0xFF, 0x5A, 0xFF), COUNTER_WALLET_TYCOON, { 0x05, 0x15 }, SIZE_COUNTER },
    { CREATE_SPRITE_24(dgQuestIconGoldSkulltulaTex, 103), COUNTER_SKULLTULLAS, { 0x05, 0x2A }, SIZE_COUNTER },
    { CREATE_SPRITE_SKULL, COUNTER_DEATHS, { 0x48, 0x2A }, SIZE_COUNTER },
    { CREATE_SPRITE_32(dgTriforcePiece, 121), COUNTER_TRIFORCE_PIECES, { 0x27, 0x10 }, SIZE_COUNTER },
};

static Sprite counterDigitSprites[10] = {
    CREATE_SPRITE_COUNTER_DIGIT(0), CREATE_SPRITE_COUNTER_DIGIT(1), CREATE_SPRITE_COUNTER_DIGIT(2),
    CREATE_SPRITE_COUNTER_DIGIT(3), CREATE_SPRITE_COUNTER_DIGIT(4), CREATE_SPRITE_COUNTER_DIGIT(5),
    CREATE_SPRITE_COUNTER_DIGIT(6), CREATE_SPRITE_COUNTER_DIGIT(7), CREATE_SPRITE_COUNTER_DIGIT(8),
    CREATE_SPRITE_COUNTER_DIGIT(9),
};

static bool ShouldRenderCounter(s16 fileIndex, CounterID counterId) {
    if (counterId == COUNTER_WALLET_NONE) {
        return !Save_GetSaveMetaInfo(fileIndex)->hasWallet;
    }

    if (counterId == COUNTER_WALLET_CHILD) {
        return Save_GetSaveMetaInfo(fileIndex)->hasWallet &&
               ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_WALLET]) >>
                gUpgradeShifts[UPG_WALLET]) == 0;
    }

    if (counterId == COUNTER_WALLET_ADULT) {
        return Save_GetSaveMetaInfo(fileIndex)->hasWallet &&
               ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_WALLET]) >>
                gUpgradeShifts[UPG_WALLET]) == 1;
    }

    if (counterId == COUNTER_WALLET_GIANT) {
        return Save_GetSaveMetaInfo(fileIndex)->hasWallet &&
               ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_WALLET]) >>
                gUpgradeShifts[UPG_WALLET]) == 2;
    }

    if (counterId == COUNTER_WALLET_TYCOON) {
        return Save_GetSaveMetaInfo(fileIndex)->hasWallet &&
               ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_WALLET]) >>
                gUpgradeShifts[UPG_WALLET]) == 3;
    }

    if (counterId == COUNTER_TRIFORCE_PIECES) {
        return Save_GetSaveMetaInfo(fileIndex)->maxTriforcePieces != 0;
    }

    return true;
}

static u16 GetCurrentCounterValue(s16 fileIndex, CounterID counter) {
    // one heart is 16 healthCapacity
    if (counter == COUNTER_HEALTH) {
        return Save_GetSaveMetaInfo(fileIndex)->healthCapacity / 16;
    }

    if (counter >= COUNTER_WALLET_CHILD && counter <= COUNTER_WALLET_TYCOON) {
        return Save_GetSaveMetaInfo(fileIndex)->rupees;
    }

    if (counter == COUNTER_SKULLTULLAS) {
        return Save_GetSaveMetaInfo(fileIndex)->gsTokens;
    }

    if (counter == COUNTER_DEATHS) {
        return Save_GetSaveMetaInfo(fileIndex)->deaths;
    }

    if (counter == COUNTER_TRIFORCE_PIECES) {
        return Save_GetSaveMetaInfo(fileIndex)->triforcePieces;
    }

    return 0;
}

static u16 GetMaxCounterValue(s16 fileIndex, CounterID counter) {
    if (counter == COUNTER_HEALTH) {
        return 20;
    }

    if (counter == COUNTER_WALLET_CHILD) {
        return 99;
    }

    if (counter == COUNTER_WALLET_ADULT) {
        return 200;
    }

    if (counter == COUNTER_WALLET_GIANT) {
        return 500;
    }

    if (counter == COUNTER_WALLET_TYCOON) {
        return 999;
    }

    if (counter == COUNTER_SKULLTULLAS) {
        return 100;
    }

    if (counter == COUNTER_DEATHS) {
        return 999;
    }

    if (counter == COUNTER_TRIFORCE_PIECES) {
        return Save_GetSaveMetaInfo(fileIndex)->maxTriforcePieces;
    }

    return 0;
}

static void DrawCounterValue(FileChooseContext* thisx, s16 fileIndex, u8 alpha, CounterData* data) {
    u16 currentValue;
    u16 maxValue;

    currentValue = GetCurrentCounterValue(fileIndex, data->id);
    maxValue = GetMaxCounterValue(fileIndex, data->id);

    // to prevent crashes if you use the save editor
    if (currentValue > 999) {
        currentValue = 999;
    }

    OPEN_DISPS(thisx->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    if (currentValue == 0) {
        // grey
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 130, 130, 130, alpha);
    } else if (currentValue == maxValue) {
        // green
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 255, 0, alpha);
    } else if (currentValue > maxValue) {
        // cyan
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 255, 255, alpha);
    } else {
        // white
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, alpha);
    }

    s16 hundreds;
    s16 tens;

    for (hundreds = 0; currentValue >= 100; hundreds++) {
        currentValue -= 100;
    }

    for (tens = 0; currentValue >= 10; tens++) {
        currentValue -= 10;
    }

    if (hundreds != 0) {
        SpriteLoad(thisx, &counterDigitSprites[hundreds]);
        SpriteDraw(thisx, &counterDigitSprites[hundreds], LEFT_OFFSET + COUNTER_DIGITS_LEFT_OFFSET - 6 + data->pos.left,
                   TOP_OFFSET + COUNTER_DIGITS_TOP_OFFSET + data->pos.top, 8, 8);

        SpriteLoad(thisx, &counterDigitSprites[tens]);
        SpriteDraw(thisx, &counterDigitSprites[tens], LEFT_OFFSET + COUNTER_DIGITS_LEFT_OFFSET + data->pos.left,
                   TOP_OFFSET + COUNTER_DIGITS_TOP_OFFSET + data->pos.top, 8, 8);

        SpriteLoad(thisx, &counterDigitSprites[currentValue]);
        SpriteDraw(thisx, &counterDigitSprites[currentValue],
                   LEFT_OFFSET + COUNTER_DIGITS_LEFT_OFFSET + 6 + data->pos.left,
                   TOP_OFFSET + COUNTER_DIGITS_TOP_OFFSET + data->pos.top, 8, 8);
    } else if (tens != 0) {
        SpriteLoad(thisx, &counterDigitSprites[tens]);
        SpriteDraw(thisx, &counterDigitSprites[tens], LEFT_OFFSET + COUNTER_DIGITS_LEFT_OFFSET - 3 + data->pos.left,
                   TOP_OFFSET + COUNTER_DIGITS_TOP_OFFSET + data->pos.top, 8, 8);

        SpriteLoad(thisx, &counterDigitSprites[currentValue]);
        SpriteDraw(thisx, &counterDigitSprites[currentValue],
                   LEFT_OFFSET + COUNTER_DIGITS_LEFT_OFFSET + 3 + data->pos.left,
                   TOP_OFFSET + COUNTER_DIGITS_TOP_OFFSET + data->pos.top, 8, 8);
    } else {
        SpriteLoad(thisx, &counterDigitSprites[currentValue]);
        SpriteDraw(thisx, &counterDigitSprites[currentValue], LEFT_OFFSET + COUNTER_DIGITS_LEFT_OFFSET + data->pos.left,
                   TOP_OFFSET + COUNTER_DIGITS_TOP_OFFSET + data->pos.top, 8, 8);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    CLOSE_DISPS(thisx->state.gfxCtx);
}

static void DrawCounters(FileChooseContext* thisx, s16 fileIndex, u8 alpha) {
    OPEN_DISPS(thisx->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    for (int i = 0; i < ARRAY_COUNT(counterData); i += 1) {
        CounterData* data = &counterData[i];

        if (ShouldRenderCounter(fileIndex, data->id)) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, data->color.r, data->color.g, data->color.b,
                            ColorProduct(data->color.a, alpha));

            SpriteLoad(thisx, &(data->sprite));
            SpriteDraw(thisx, &(data->sprite), LEFT_OFFSET + data->pos.left, TOP_OFFSET + data->pos.top,
                       data->size.width, data->size.height);

            DrawCounterValue(thisx, fileIndex, alpha, data);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    CLOSE_DISPS(thisx->state.gfxCtx);
}

static void DrawMoreInfo(FileChooseContext* thisx, s16 fileIndex, u8 alpha) {
    DrawItems(thisx, fileIndex, alpha);
    DrawCounters(thisx, fileIndex, alpha);
}

#define CVAR_FILE_SELECT_MORE_INFO_DEFAULT false
#define CVAR_FILE_SELECT_MORE_INFO_NAME CVAR_ENHANCEMENT("FileSelectMoreInfo")
#define CVAR_FILE_SELECT_MORE_INFO_VALUE \
    CVarGetInteger(CVAR_FILE_SELECT_MORE_INFO_NAME, CVAR_FILE_SELECT_MORE_INFO_DEFAULT)

void RegisterFileSelectMoreInfo() {
    COND_VB_SHOULD(VB_FILE_SELECT_DRAW_DEATHS, CVAR_FILE_SELECT_MORE_INFO_VALUE, {
        FileChooseContext* thisx = va_arg(args, FileChooseContext*);
        *should = thisx->menuMode != FS_MENU_MODE_SELECT;
    });

    COND_VB_SHOULD(VB_FILE_SELECT_DRAW_HEARTS, CVAR_FILE_SELECT_MORE_INFO_VALUE, {
        FileChooseContext* thisx = va_arg(args, FileChooseContext*);
        *should = thisx->menuMode != FS_MENU_MODE_SELECT;
    });

    COND_VB_SHOULD(VB_FILE_SELECT_DRAW_QUEST_ITEMS, CVAR_FILE_SELECT_MORE_INFO_VALUE, {
        FileChooseContext* thisx = va_arg(args, FileChooseContext*);
        s32 fileIndex = va_arg(args, s32);
        u32 textAlpha = va_arg(args, u32);

        if (thisx->menuMode == FS_MENU_MODE_SELECT) {
            DrawMoreInfo(thisx, fileIndex, textAlpha);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_FILE_SELECT_DRAW_FILE_INFO_BOX, CVAR_FILE_SELECT_MORE_INFO_VALUE, {
        FileChooseContext* thisx = va_arg(args, FileChooseContext*);

        // Draw the small file name box instead when more meta info is enabled
        if (thisx->menuMode == FS_MENU_MODE_SELECT) {
            OPEN_DISPS(thisx->state.gfxCtx);

            // Location of file 1 small name box vertices
            gSPVertex(POLY_OPA_DISP++, (uintptr_t)&thisx->windowContentVtx[68], 4, 0);

            gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelNameBoxTex, G_IM_FMT_IA, G_IM_SIZ_16b, 108, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

            CLOSE_DISPS(thisx->state.gfxCtx);
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFileSelectMoreInfo, { CVAR_FILE_SELECT_MORE_INFO_NAME });
