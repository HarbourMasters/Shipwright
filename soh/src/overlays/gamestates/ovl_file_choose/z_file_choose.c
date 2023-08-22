#include "file_choose.h"

#include <string.h>

#include "textures/title_static/title_static.h"
#include "textures/parameter_static/parameter_static.h"
#include <textures/icon_item_static/icon_item_static.h>
#include <textures/icon_item_24_static/icon_item_24_static.h>
#include <textures/icon_item_dungeon_static/icon_item_dungeon_static.h>
#include <textures/parameter_static/parameter_static.h>
#include "soh/frame_interpolation.h"
#include <GameVersions.h>
#include "objects/object_mag/object_mag.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "soh_assets.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/boss-rush/BossRush.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include <assert.h>

typedef struct {
    s16 left;
    s16 top;
} IconPosition;

typedef struct {
    s16 width;
    s16 height;
} IconSize;

typedef struct {
    Sprite sprite;
    Color_RGBA8 color;
    u8 item;
    IconPosition pos;
    IconSize size;
} ItemData;

#define CREATE_SPRITE_24(iconTex, spriteId) { iconTex, 24, 24, G_IM_FMT_RGBA, G_IM_SIZ_32b, spriteId }, {0xFF, 0xFF, 0xFF, 0xFF}
#define CREATE_SPRITE_32(iconTex, spriteId) { iconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b, spriteId }, {0xFF, 0xFF, 0xFF, 0xFF}
#define CREATE_SPRITE_SONG(colorR, colorG, colorB) { dgSongNoteTex, 16, 24, G_IM_FMT_IA, G_IM_SIZ_8b, 100 }, {colorR, colorG, colorB, 0xFF}
#define CREATE_SPRITE_RUPEE(colorR, colorG, colorB) { dgRupeeCounterIconTex, 16, 16, G_IM_FMT_IA, G_IM_SIZ_8b, 102 }, {colorR, colorG, colorB, 0xFF}
#define CREATE_SPRITE_SKULL { dgDungeonMapSkullTex, 16, 16, G_IM_FMT_RGBA, G_IM_SIZ_16b, 104 }, {0xFF, 0xFF, 0xFF, 0xFF}
#define CREATE_SPRITE_COUNTER_DIGIT(i) { dgAmmoDigit##i##Tex, 8, 8, G_IM_FMT_IA, G_IM_SIZ_8b, 105+i }

#define ICON_SIZE 12
#define COUNTER_SIZE 16
#define SONG_WIDTH 8
#define SONG_HEIGHT 12

#define LEFT_OFFSET (int)0x37
#define TOP_OFFSET  (int)0x5C

#define COUNTER_DIGITS_LEFT_OFFSET COUNTER_SIZE / 2 - 3
#define COUNTER_DIGITS_TOP_OFFSET COUNTER_SIZE - 3

#define SIZE_NORMAL {ICON_SIZE, ICON_SIZE}
#define SIZE_COUNTER {COUNTER_SIZE, COUNTER_SIZE}
#define SIZE_SONG {SONG_WIDTH, SONG_HEIGHT}

#define INV_IC_POS(x, y) {0x4E + ICON_SIZE * x, 0x00 + ICON_SIZE * y}
#define EQP_IC_POS(x, y) {0x7E + ICON_SIZE * x, 0x2A + ICON_SIZE * y}
#define SNG_IC_POS(x, y) {0x49 + SONG_WIDTH * x, 0x45 + SONG_HEIGHT * y}
#define UPG_IC_POS(x, y) {0x5A + ICON_SIZE * x, 0x2A + ICON_SIZE * y}
#define STN_IC_POS(i) {0x29 + ICON_SIZE * i, 0x31}

static ItemData itemData[88] = {
    {CREATE_SPRITE_32(dgDekuStickIconTex, 1),            ITEM_STICK,            INV_IC_POS(0, 0), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgDekuNutIconTex, 0),              ITEM_NUT,              INV_IC_POS(1, 0), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgBombIconTex, 2),                 ITEM_BOMB,             INV_IC_POS(2, 0), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgFairyBowIconTex, 3),             ITEM_BOW,              INV_IC_POS(3, 0), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgFireArrowIconTex, 4),            ITEM_ARROW_FIRE,       INV_IC_POS(4, 0), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgDinsFireIconTex, 5),             ITEM_DINS_FIRE,        INV_IC_POS(5, 0), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgEmptyBottleIconTex, 20),         ITEM_BOTTLE,           INV_IC_POS(6, 0), SIZE_NORMAL},

    {CREATE_SPRITE_32(dgFairySlingshotIconTex, 6),       ITEM_SLINGSHOT,        INV_IC_POS(0, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgFairyOcarinaIconTex, 7),         ITEM_OCARINA_FAIRY,    INV_IC_POS(1, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgOcarinaofTimeIconTex, 7),        ITEM_OCARINA_TIME,     INV_IC_POS(1, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgBombchuIconTex, 9),              ITEM_BOMBCHU,          INV_IC_POS(2, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgHookshotIconTex, 10),            ITEM_HOOKSHOT,         INV_IC_POS(3, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgLongshotIconTex, 10),            ITEM_LONGSHOT,         INV_IC_POS(3, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgIceArrowIconTex, 12),            ITEM_ARROW_ICE,        INV_IC_POS(4, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgFaroresWindIconTex, 13),         ITEM_FARORES_WIND,     INV_IC_POS(5, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgWeirdEggIconTex, 37),            ITEM_WEIRD_EGG,        INV_IC_POS(6, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgCuccoIconTex, 37),               ITEM_CHICKEN,          INV_IC_POS(6, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgZeldasLetterIconTex, 37),        ITEM_LETTER_ZELDA,     INV_IC_POS(6, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgKeatonMaskIconTex, 37),          ITEM_MASK_KEATON,      INV_IC_POS(6, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgSkullMaskIconTex, 37),           ITEM_MASK_SKULL,       INV_IC_POS(6, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgSpookyMaskIconTex, 37),          ITEM_MASK_SPOOKY,      INV_IC_POS(6, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgBunnyHoodIconTex, 37),           ITEM_MASK_BUNNY,       INV_IC_POS(6, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgGoronMaskIconTex, 37),           ITEM_MASK_GORON,       INV_IC_POS(6, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgZoraMaskIconTex, 37),            ITEM_MASK_ZORA,        INV_IC_POS(6, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgGerudoMaskIconTex, 37),          ITEM_MASK_GERUDO,      INV_IC_POS(6, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgMaskofTruthIconTex, 37),         ITEM_MASK_TRUTH,       INV_IC_POS(6, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgSoldOutIconTex, 37),             ITEM_SOLD_OUT,         INV_IC_POS(6, 1), SIZE_NORMAL},
    
    {CREATE_SPRITE_32(dgBoomerangIconTex, 14),           ITEM_BOOMERANG,        INV_IC_POS(0, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgLensofTruthIconTex, 15),         ITEM_LENS,             INV_IC_POS(1, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgMagicBeansIconTex, 16),          ITEM_BEAN,             INV_IC_POS(2, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgMegatonHammerIconTex, 17),       ITEM_HAMMER,           INV_IC_POS(3, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgLightArrowIconTex, 18),          ITEM_ARROW_LIGHT,      INV_IC_POS(4, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgNayrusLoveIconTex, 19),          ITEM_NAYRUS_LOVE,      INV_IC_POS(5, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgPocketEggIconTex, 53),           ITEM_POCKET_EGG,       INV_IC_POS(6, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgPocketCuccoIconTex, 53),         ITEM_POCKET_CUCCO,     INV_IC_POS(6, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgCojiroIconTex, 53),              ITEM_COJIRO,           INV_IC_POS(6, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgOddMushroomIconTex, 53),         ITEM_ODD_MUSHROOM,     INV_IC_POS(6, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgOddPotionIconTex, 53),           ITEM_ODD_POTION,       INV_IC_POS(6, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgPoachersSawIconTex, 53),         ITEM_SAW,              INV_IC_POS(6, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgBrokenBiggoronSwordIconTex, 53), ITEM_SWORD_BROKEN,     INV_IC_POS(6, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgPrescriptionIconTex, 53),        ITEM_PRESCRIPTION,     INV_IC_POS(6, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgEyeBallFrogIconTex, 53),         ITEM_FROG,             INV_IC_POS(6, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgEyeDropsIconTex, 53),            ITEM_EYEDROPS,         INV_IC_POS(6, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgClaimCheckIconTex, 53),          ITEM_CLAIM_CHECK,      INV_IC_POS(6, 2), SIZE_NORMAL},
    
    {CREATE_SPRITE_32(dgKokiriSwordIconTex, 54),         ITEM_SWORD_KOKIRI,     EQP_IC_POS(0, 0), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgMasterSwordIconTex, 55),         ITEM_SWORD_MASTER,     EQP_IC_POS(1, 0), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgBiggoronSwordIconTex, 56),       ITEM_SWORD_BGS,        EQP_IC_POS(2, 0), SIZE_NORMAL},
    
    {CREATE_SPRITE_32(dgDekuShieldIconTex, 57),          ITEM_SHIELD_DEKU,      EQP_IC_POS(0, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgHylianShieldIconTex, 58),        ITEM_SHIELD_HYLIAN,    EQP_IC_POS(1, 1), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgMirrorShieldIconTex, 59),        ITEM_SHIELD_MIRROR,    EQP_IC_POS(2, 1), SIZE_NORMAL},
    
    {CREATE_SPRITE_32(dgKokiriTunicIconTex, 60),         ITEM_TUNIC_KOKIRI,     EQP_IC_POS(0, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgGoronTunicIconTex, 61),          ITEM_TUNIC_GORON,      EQP_IC_POS(1, 2), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgZoraTunicIconTex, 62),           ITEM_TUNIC_ZORA,       EQP_IC_POS(2, 2), SIZE_NORMAL},
    
    {CREATE_SPRITE_32(dgKokiriBootsIconTex, 63),         ITEM_BOOTS_KOKIRI,     EQP_IC_POS(0, 3), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgIronBootsIconTex, 64),           ITEM_BOOTS_IRON,       EQP_IC_POS(1, 3), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgHoverBootsIconTex, 65),          ITEM_BOOTS_HOVER,      EQP_IC_POS(2, 3), SIZE_NORMAL},

    {CREATE_SPRITE_24(dgKokiriEmeraldIconTex, 87),       ITEM_KOKIRI_EMERALD,   STN_IC_POS(-1),   SIZE_NORMAL},
    {CREATE_SPRITE_24(dgGoronRubyIconTex, 88),           ITEM_GORON_RUBY,       STN_IC_POS(0),    SIZE_NORMAL},
    {CREATE_SPRITE_24(dgZoraSapphireIconTex, 89),        ITEM_ZORA_SAPPHIRE,    STN_IC_POS(1),    SIZE_NORMAL},
    
    {CREATE_SPRITE_24(dgForestMedallionIconTex, 81),     ITEM_MEDALLION_FOREST, {0x37, 0x0A},     SIZE_NORMAL},
    {CREATE_SPRITE_24(dgFireMedallionIconTex, 82),       ITEM_MEDALLION_FIRE,   {0x37, 0x1A},     SIZE_NORMAL},
    {CREATE_SPRITE_24(dgWaterMedallionIconTex, 83),      ITEM_MEDALLION_WATER,  {0x29, 0x22},     SIZE_NORMAL},
    {CREATE_SPRITE_24(dgSpiritMedallionIconTex, 84),     ITEM_MEDALLION_SPIRIT, {0x1B, 0x1A},     SIZE_NORMAL},
    {CREATE_SPRITE_24(dgShadowMedallionIconTex, 85),     ITEM_MEDALLION_SHADOW, {0x1B, 0x0A},     SIZE_NORMAL},
    {CREATE_SPRITE_24(dgLightMedallionIconTex, 86),      ITEM_MEDALLION_LIGHT,  {0x29, 0x02},     SIZE_NORMAL},

    {CREATE_SPRITE_32(dgGoronsBraceletIconTex, 71),      ITEM_BRACELET,         UPG_IC_POS(0, 0), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgSilverGauntletsIconTex, 71),     ITEM_GAUNTLETS_SILVER, UPG_IC_POS(0, 0), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgGoldenGauntletsIconTex, 71),     ITEM_GAUNTLETS_GOLD,   UPG_IC_POS(0, 0), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgSilverScaleIconTex, 74),         ITEM_SCALE_SILVER,     UPG_IC_POS(1, 0), SIZE_NORMAL},
    {CREATE_SPRITE_32(dgGoldenScaleIconTex, 74),         ITEM_SCALE_GOLDEN,     UPG_IC_POS(1, 0), SIZE_NORMAL},
    {CREATE_SPRITE_24(dgSmallMagicJarIconTex, 97),       ITEM_SINGLE_MAGIC,     UPG_IC_POS(2, 0), SIZE_NORMAL},
    {CREATE_SPRITE_24(dgBigMagicJarIconTex, 97),         ITEM_DOUBLE_MAGIC,     UPG_IC_POS(2, 0), SIZE_NORMAL},
    {CREATE_SPRITE_RUPEE(0xC8, 0xFF, 0x64),              ITEM_RUPEE_GREEN,      UPG_IC_POS(0, 1), SIZE_NORMAL},
    {CREATE_SPRITE_24(dgGerudosCardIconTex, 91),         ITEM_GERUDO_CARD,      UPG_IC_POS(1, 1), SIZE_NORMAL},
    {CREATE_SPRITE_24(dgStoneOfAgonyIconTex, 90),        ITEM_STONE_OF_AGONY,   UPG_IC_POS(2, 1), SIZE_NORMAL},

    {CREATE_SPRITE_SONG(224, 107, 255),                  ITEM_SONG_LULLABY,     SNG_IC_POS(0, 0), SIZE_SONG},
    {CREATE_SPRITE_SONG(255, 195, 60),                   ITEM_SONG_EPONA,       SNG_IC_POS(1, 0), SIZE_SONG},
    {CREATE_SPRITE_SONG(127, 255, 137),                  ITEM_SONG_SARIA,       SNG_IC_POS(2, 0), SIZE_SONG},
    {CREATE_SPRITE_SONG(255, 255, 60),                   ITEM_SONG_SUN,         SNG_IC_POS(3, 0), SIZE_SONG},
    {CREATE_SPRITE_SONG(119, 236, 255),                  ITEM_SONG_TIME,        SNG_IC_POS(4, 0), SIZE_SONG},
    {CREATE_SPRITE_SONG(165, 165, 165),                  ITEM_SONG_STORMS,      SNG_IC_POS(5, 0), SIZE_SONG},
    {CREATE_SPRITE_SONG(150, 255, 100),                  ITEM_SONG_MINUET,      SNG_IC_POS(0, 1), SIZE_SONG},
    {CREATE_SPRITE_SONG(255, 80,  40),                   ITEM_SONG_BOLERO,      SNG_IC_POS(1, 1), SIZE_SONG},
    {CREATE_SPRITE_SONG(100, 150, 255),                  ITEM_SONG_SERENADE,    SNG_IC_POS(2, 1), SIZE_SONG},
    {CREATE_SPRITE_SONG(255, 160, 0),                    ITEM_SONG_REQUIEM,     SNG_IC_POS(3, 1), SIZE_SONG},
    {CREATE_SPRITE_SONG(255, 100, 255),                  ITEM_SONG_NOCTURNE,    SNG_IC_POS(4, 1), SIZE_SONG},
    {CREATE_SPRITE_SONG(255, 240, 100),                  ITEM_SONG_PRELUDE,     SNG_IC_POS(5, 1), SIZE_SONG},

    {CREATE_SPRITE_24(dgHeartContainerIconTex, 101),     ITEM_DOUBLE_DEFENSE,   {0x05, -0x04},    SIZE_COUNTER},
};

static u8 color_product(u8 c1, u8 c2) {
    u16 prod = (u16)c1 * (u16)c2;
    u16 div255 = (prod + 1 + (prod >> 8)) >> 8;
    return (u8)div255;
}

static const Color_RGBA8 DIM = {0x40, 0x40, 0x40, 0x90};

void SpriteLoad(FileChooseContext* this, Sprite* sprite);
void SpriteDraw(FileChooseContext* this, Sprite* sprite, int left, int top, int width, int height);

u8 HasItem(s16 fileIndex, u8 item) {
    for (int i = 0; i < ARRAY_COUNT(Save_GetSaveMetaInfo(fileIndex)->inventoryItems); i += 1) {
        u8 it = Save_GetSaveMetaInfo(fileIndex)->inventoryItems[i];
        if (it == item || (item == ITEM_BOTTLE && it >= ITEM_BOTTLE && it <= ITEM_POE)) {
            return 1;
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
        return ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_STRENGTH]) >> gUpgradeShifts[UPG_STRENGTH]) == 1;
    }

    if (item == ITEM_GAUNTLETS_SILVER) {
        return ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_STRENGTH]) >> gUpgradeShifts[UPG_STRENGTH]) == 2;
    }

    if (item == ITEM_GAUNTLETS_GOLD) {
        return ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_STRENGTH]) >> gUpgradeShifts[UPG_STRENGTH]) == 3;
    }

    if (item == ITEM_SCALE_SILVER) {
        return ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_SCALE]) >> gUpgradeShifts[UPG_SCALE]) == 1;
    }

    if (item == ITEM_SCALE_GOLDEN) {
        return ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_SCALE]) >> gUpgradeShifts[UPG_SCALE]) == 2;
    }

    if (item == ITEM_DOUBLE_DEFENSE) {
        return Save_GetSaveMetaInfo(fileIndex)->isDoubleDefenseAcquired;
    }

    //greg
    if (item == ITEM_RUPEE_GREEN) {
        return Save_GetSaveMetaInfo(fileIndex)->gregFound;
    }

    return 0;
}

u8 ShouldRenderItem(s16 fileIndex, u8 item) {
    //strength
    if (item == ITEM_BRACELET && (HasItem(fileIndex, ITEM_GAUNTLETS_SILVER) || HasItem(fileIndex, ITEM_GAUNTLETS_GOLD))) {
        return 0;
    }

    if (item == ITEM_GAUNTLETS_SILVER && !HasItem(fileIndex, ITEM_GAUNTLETS_SILVER)) {
        return 0;
    }

    if (item == ITEM_GAUNTLETS_GOLD && !HasItem(fileIndex, ITEM_GAUNTLETS_GOLD)) {
        return 0;
    }

    //magic
    if (item == ITEM_SINGLE_MAGIC && HasItem(fileIndex, ITEM_DOUBLE_MAGIC)) {
        return 0;
    }

    if (item == ITEM_DOUBLE_MAGIC && !HasItem(fileIndex, ITEM_DOUBLE_MAGIC)) {
        return 0;
    }

    //scales
    if (item == ITEM_SCALE_SILVER && HasItem(fileIndex, ITEM_SCALE_GOLDEN)) {
        return 0;
    }

    if (item == ITEM_SCALE_GOLDEN && !HasItem(fileIndex, ITEM_SCALE_GOLDEN)) {
        return 0;
    }

    //hookshot/longshot
    if (item == ITEM_HOOKSHOT && HasItem(fileIndex, ITEM_LONGSHOT)) {
        return 0;
    }

    if (item == ITEM_LONGSHOT && !HasItem(fileIndex, ITEM_LONGSHOT)) {
        return 0;
    }

    //ocarinas
    if (item == ITEM_OCARINA_FAIRY && HasItem(fileIndex, ITEM_OCARINA_TIME)) {
        return 0;
    }

    if (item == ITEM_OCARINA_TIME && !HasItem(fileIndex, ITEM_OCARINA_TIME)) {
        return 0;
    }

    //trade child
    if (item == ITEM_WEIRD_EGG && !HasItem(fileIndex, ITEM_WEIRD_EGG)) {
        return 0;
    }

    if (item == ITEM_CHICKEN && !HasItem(fileIndex, ITEM_CHICKEN)) {
        return 0;
    }

    if (item == ITEM_LETTER_ZELDA && !HasItem(fileIndex, ITEM_LETTER_ZELDA)) {
        return 0;
    }

    if (
        item == ITEM_MASK_KEATON &&
        (
            HasItem(fileIndex, ITEM_WEIRD_EGG) ||
            HasItem(fileIndex, ITEM_CHICKEN) ||
            HasItem(fileIndex, ITEM_LETTER_ZELDA) ||
            HasItem(fileIndex, ITEM_MASK_SKULL) ||
            HasItem(fileIndex, ITEM_MASK_SPOOKY) ||
            HasItem(fileIndex, ITEM_MASK_BUNNY) ||
            HasItem(fileIndex, ITEM_MASK_GORON) ||
            HasItem(fileIndex, ITEM_MASK_ZORA) ||
            HasItem(fileIndex, ITEM_MASK_GERUDO) ||
            HasItem(fileIndex, ITEM_MASK_TRUTH) ||
            HasItem(fileIndex, ITEM_SOLD_OUT)
        )
    ) {
        return 0;
    }

    if (item == ITEM_MASK_SKULL && !HasItem(fileIndex, ITEM_MASK_SKULL)) {
        return 0;
    }

    if (item == ITEM_MASK_SPOOKY && !HasItem(fileIndex, ITEM_MASK_SPOOKY)) {
        return 0;
    }

    if (item == ITEM_MASK_BUNNY && !HasItem(fileIndex, ITEM_MASK_BUNNY)) {
        return 0;
    }

    if (item == ITEM_MASK_GORON && !HasItem(fileIndex, ITEM_MASK_GORON)) {
        return 0;
    }

    if (item == ITEM_MASK_ZORA && !HasItem(fileIndex, ITEM_MASK_ZORA)) {
        return 0;
    }

    if (item == ITEM_MASK_GERUDO && !HasItem(fileIndex, ITEM_MASK_GERUDO)) {
        return 0;
    }

    if (item == ITEM_MASK_TRUTH && !HasItem(fileIndex, ITEM_MASK_TRUTH)) {
        return 0;
    }

    if (item == ITEM_SOLD_OUT && !HasItem(fileIndex, ITEM_SOLD_OUT)) {
        return 0;
    }

    //trade adult
    if (
        item == ITEM_POCKET_EGG &&
        (
            HasItem(fileIndex, ITEM_POCKET_CUCCO) ||
            HasItem(fileIndex, ITEM_COJIRO) ||
            HasItem(fileIndex, ITEM_ODD_MUSHROOM) ||
            HasItem(fileIndex, ITEM_ODD_POTION) ||
            HasItem(fileIndex, ITEM_SAW) ||
            HasItem(fileIndex, ITEM_SWORD_BROKEN) ||
            HasItem(fileIndex, ITEM_PRESCRIPTION) ||
            HasItem(fileIndex, ITEM_FROG) ||
            HasItem(fileIndex, ITEM_EYEDROPS) ||
            HasItem(fileIndex, ITEM_CLAIM_CHECK)
        )
    ) {
        return 0;
    }

    if (item == ITEM_POCKET_CUCCO && !HasItem(fileIndex, ITEM_POCKET_CUCCO)) {
        return 0;
    }
    
    if (item == ITEM_COJIRO && !HasItem(fileIndex, ITEM_COJIRO)) {
        return 0;
    }
    
    if (item == ITEM_ODD_MUSHROOM && !HasItem(fileIndex, ITEM_ODD_MUSHROOM)) {
        return 0;
    }
    
    if (item == ITEM_ODD_POTION && !HasItem(fileIndex, ITEM_ODD_POTION)) {
        return 0;
    }
    
    if (item == ITEM_SAW && !HasItem(fileIndex, ITEM_SAW)) {
        return 0;
    }
    
    if (item == ITEM_SWORD_BROKEN && !HasItem(fileIndex, ITEM_SWORD_BROKEN)) {
        return 0;
    }
    
    if (item == ITEM_PRESCRIPTION && !HasItem(fileIndex, ITEM_PRESCRIPTION)) {
        return 0;
    }
    
    if (item == ITEM_FROG && !HasItem(fileIndex, ITEM_FROG)) {
        return 0;
    }
    
    if (item == ITEM_EYEDROPS && !HasItem(fileIndex, ITEM_EYEDROPS)) {
        return 0;
    }
    
    if (item == ITEM_CLAIM_CHECK && !HasItem(fileIndex, ITEM_CLAIM_CHECK)) {
        return 0;
    }

    if (item == ITEM_DOUBLE_DEFENSE && !Save_GetSaveMetaInfo(fileIndex)->isDoubleDefenseAcquired) {
        return 0;
    }

    //greg
    if (item == ITEM_RUPEE_GREEN) {
        return Save_GetSaveMetaInfo(fileIndex)->randoSave;
    }

    return 1;
}

static void DrawItems(FileChooseContext* this, s16 fileIndex, u8 alpha) {
    OPEN_DISPS(this->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    for (int i = 0; i < ARRAY_COUNT(itemData); i += 1) {
        ItemData* data = &itemData[i];

        if (ShouldRenderItem(fileIndex, data->item)) {
            if (HasItem(fileIndex, data->item)) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, data->color.r, data->color.g, data->color.b, color_product(data->color.a, alpha));
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, color_product(data->color.r, DIM.r), color_product(data->color.g, DIM.g), color_product(data->color.b, DIM.b), color_product(color_product(data->color.a, DIM.a), alpha));
            }
        
            SpriteLoad(this, &(data->sprite));
            SpriteDraw(this, &(data->sprite), LEFT_OFFSET + data->pos.left, TOP_OFFSET + data->pos.top, data->size.width, data->size.height);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    CLOSE_DISPS(this->state.gfxCtx);
}

typedef enum {
    /* 0x00 */ COUNTER_HEALTH,
    /* 0x01 */ COUNTER_WALLET_CHILD,
    /* 0x02 */ COUNTER_WALLET_ADULT,
    /* 0x03 */ COUNTER_WALLET_GIANT,
    /* 0x04 */ COUNTER_WALLET_TYCOON,
    /* 0x04 */ COUNTER_SKULLTULLAS,
    /* 0x04 */ COUNTER_DEATHS,
} CounterID;

typedef struct {
    Sprite sprite;
    Color_RGBA8 color;
    u8 id;
    IconPosition pos;
    IconSize size;
} CounterData;

static CounterData counterData[7] = {
    {CREATE_SPRITE_24(dgHeartContainerIconTex, 101), COUNTER_HEALTH,        {0x05, 0x00}, SIZE_COUNTER},
    {CREATE_SPRITE_RUPEE(0xC8, 0xFF, 0x64),          COUNTER_WALLET_CHILD,  {0x05, 0x15}, SIZE_COUNTER},
    {CREATE_SPRITE_RUPEE(0x82, 0x82, 0xFF),          COUNTER_WALLET_ADULT,  {0x05, 0x15}, SIZE_COUNTER},
    {CREATE_SPRITE_RUPEE(0xFF, 0x64, 0x64),          COUNTER_WALLET_GIANT,  {0x05, 0x15}, SIZE_COUNTER},
    {CREATE_SPRITE_RUPEE(0xFF, 0x5A, 0xFF),          COUNTER_WALLET_TYCOON, {0x05, 0x15}, SIZE_COUNTER},
    {CREATE_SPRITE_24(dgGoldSkulltulaIconTex, 103),  COUNTER_SKULLTULLAS,   {0x05, 0x2A}, SIZE_COUNTER},
    {CREATE_SPRITE_SKULL,                            COUNTER_DEATHS,        {0x48, 0x2A}, SIZE_COUNTER},
};

static Sprite counterDigitSprites[10] = {
    CREATE_SPRITE_COUNTER_DIGIT(0),
    CREATE_SPRITE_COUNTER_DIGIT(1),
    CREATE_SPRITE_COUNTER_DIGIT(2),
    CREATE_SPRITE_COUNTER_DIGIT(3),
    CREATE_SPRITE_COUNTER_DIGIT(4),
    CREATE_SPRITE_COUNTER_DIGIT(5),
    CREATE_SPRITE_COUNTER_DIGIT(6),
    CREATE_SPRITE_COUNTER_DIGIT(7),
    CREATE_SPRITE_COUNTER_DIGIT(8),
    CREATE_SPRITE_COUNTER_DIGIT(9),
};

u8 ShouldRenderCounter(s16 fileIndex, u8 counterId) {
    if (counterId == COUNTER_WALLET_CHILD) {
        return ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_WALLET]) >> gUpgradeShifts[UPG_WALLET]) == 0;
    }

    if (counterId == COUNTER_WALLET_ADULT) {
        return ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_WALLET]) >> gUpgradeShifts[UPG_WALLET]) == 1;
    }

    if (counterId == COUNTER_WALLET_GIANT) {
        return ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_WALLET]) >> gUpgradeShifts[UPG_WALLET]) == 2;
    }

    if (counterId == COUNTER_WALLET_TYCOON) {
        return ((Save_GetSaveMetaInfo(fileIndex)->upgrades & gUpgradeMasks[UPG_WALLET]) >> gUpgradeShifts[UPG_WALLET]) == 3;
    }

    return 1;
}

u16 GetCurrentCounterValue(s16 fileIndex, u8 counter) {
    //one heart is 16 healthCapacity
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

    return 0;
}

u16 GetMaxCounterValue(s16 fileIndex, u8 counter) {
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

    return 0;
}

void DrawCounterValue(FileChooseContext* this, s16 fileIndex, u8 alpha, CounterData* data) {
    u16 currentValue;
    u16 maxValue;
    s16 hundreds;
    s16 tens;

    currentValue = GetCurrentCounterValue(fileIndex, data->id);
    maxValue = GetMaxCounterValue(fileIndex, data->id);

    //to prevent crashes if you use the save editor
    if (currentValue > 999) {
        currentValue = 999;
    }

    OPEN_DISPS(this->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    if (currentValue == 0) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 130, 130, 130, alpha);
    } else if (currentValue == maxValue) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 255, 0, alpha);
    } else {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, alpha);
    }

    for (hundreds = 0; currentValue >= 100; hundreds++) {
        currentValue -= 100;
    }

    for (tens = 0; currentValue >= 10; tens++) {
        currentValue -= 10;
    }

    if (hundreds != 0) {
        SpriteLoad(this, &counterDigitSprites[hundreds]);
        SpriteDraw(this, &counterDigitSprites[hundreds], LEFT_OFFSET + COUNTER_DIGITS_LEFT_OFFSET - 6 + data->pos.left, TOP_OFFSET + COUNTER_DIGITS_TOP_OFFSET + data->pos.top, 8, 8);

        SpriteLoad(this, &counterDigitSprites[tens]);
        SpriteDraw(this, &counterDigitSprites[tens], LEFT_OFFSET + COUNTER_DIGITS_LEFT_OFFSET + data->pos.left, TOP_OFFSET + COUNTER_DIGITS_TOP_OFFSET + data->pos.top, 8, 8);

        SpriteLoad(this, &counterDigitSprites[currentValue]);
        SpriteDraw(this, &counterDigitSprites[currentValue], LEFT_OFFSET + COUNTER_DIGITS_LEFT_OFFSET + 6 + data->pos.left, TOP_OFFSET + COUNTER_DIGITS_TOP_OFFSET + data->pos.top, 8, 8);
    } else if (tens != 0) {
        SpriteLoad(this, &counterDigitSprites[tens]);
        SpriteDraw(this, &counterDigitSprites[tens], LEFT_OFFSET + COUNTER_DIGITS_LEFT_OFFSET - 3 + data->pos.left, TOP_OFFSET + COUNTER_DIGITS_TOP_OFFSET + data->pos.top, 8, 8);

        SpriteLoad(this, &counterDigitSprites[currentValue]);
        SpriteDraw(this, &counterDigitSprites[currentValue], LEFT_OFFSET + COUNTER_DIGITS_LEFT_OFFSET + 3 + data->pos.left, TOP_OFFSET + COUNTER_DIGITS_TOP_OFFSET + data->pos.top, 8, 8);
    } else {
        SpriteLoad(this, &counterDigitSprites[currentValue]);
        SpriteDraw(this, &counterDigitSprites[currentValue], LEFT_OFFSET + COUNTER_DIGITS_LEFT_OFFSET + data->pos.left, TOP_OFFSET + COUNTER_DIGITS_TOP_OFFSET + data->pos.top, 8, 8);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    CLOSE_DISPS(this->state.gfxCtx);
}

static void DrawCounters(FileChooseContext* this, s16 fileIndex, u8 alpha) {
    OPEN_DISPS(this->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    for (int i = 0; i < ARRAY_COUNT(counterData); i += 1) {
        CounterData* data = &counterData[i];

        if (ShouldRenderCounter(fileIndex, data->id)) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, data->color.r, data->color.g, data->color.b, color_product(data->color.a, alpha));
        
            SpriteLoad(this, &(data->sprite));
            SpriteDraw(this, &(data->sprite), LEFT_OFFSET + data->pos.left, TOP_OFFSET + data->pos.top, data->size.width, data->size.height);

            DrawCounterValue(this, fileIndex, alpha, data);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    CLOSE_DISPS(this->state.gfxCtx);
}

static void DrawMoreInfo(FileChooseContext* this, s16 fileIndex, u8 alpha) {
    DrawItems(this, fileIndex, alpha);
    DrawCounters(this, fileIndex, alpha);
}

#define MIN_QUEST (ResourceMgr_GameHasOriginal() ? FS_QUEST_NORMAL : FS_QUEST_MASTER)
#define MAX_QUEST FS_QUEST_BOSSRUSH

void Sram_InitDebugSave(void);
void Sram_InitBossRushSave();

u8 hasRandomizerQuest() {
    if (strnlen(CVarGetString("gSpoilerLog", ""), 1) != 0) {
        return 1;
    }
    return 0;
}

void FileChoose_DrawTextureI8(GraphicsContext* gfxCtx, const void* texture, s16 texWidth, s16 texHeight, s16 rectLeft, s16 rectTop,
                         s16 rectWidth, s16 rectHeight, s16 dsdx, s16 dtdy) {
    OPEN_DISPS(gfxCtx);
    gDPLoadTextureBlock(POLY_OPA_DISP++, texture, G_IM_FMT_I, G_IM_SIZ_8b, texWidth, texHeight, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPTextureRectangle(POLY_OPA_DISP++, rectLeft << 2, rectTop << 2, (rectLeft + rectWidth) << 2, (rectTop + rectHeight) << 2,
                        G_TX_RENDERTILE, 0, 0, dsdx, dtdy);

    CLOSE_DISPS(gfxCtx);
}

void FileChoose_DrawImageRGBA32(GraphicsContext* gfxCtx, s16 centerX, s16 centerY, const char* source, u32 width, u32 height) {
    u8* curTexture;
    s32 textureCount;
    u32 rectLeft;
    u32 rectTop;
    u32 textureHeight;
    s32 remainingSize;
    s32 textureSize;
    s32 pad;
    s32 i;

    OPEN_DISPS(gfxCtx);

    rectLeft = centerX - (width / 2);
    rectTop = centerY - (height / 2);
    
    gDPSetTileCustom(POLY_OPA_DISP++, G_IM_FMT_RGBA, G_IM_SIZ_32b, width, height, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                     G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    
    gDPSetTextureImage(POLY_OPA_DISP++, G_IM_FMT_RGBA, G_IM_SIZ_32b, width, source);
    
    gDPLoadSync(POLY_OPA_DISP++);
    gDPLoadTile(POLY_OPA_DISP++, G_TX_LOADTILE, 0, 0, (width - 1) << 2, (height - 1) << 2);
    
    gSPTextureRectangle(POLY_OPA_DISP++, rectLeft << 2, rectTop << 2, (rectLeft + (s32)width) << 2,
                        (rectTop + height) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    CLOSE_DISPS(gfxCtx);
}

void FileChoose_DrawTextRec(GraphicsContext* gfxCtx, s32 r, s32 g, s32 b, s32 a, f32 x, f32 y, f32 z, s32 s, s32 t,
                         f32 dx, f32 dy) {
    f32 unk;
    s32 ulx, uly, lrx, lry;
    f32 w, h;
    s32 dsdx, dtdy;

    OPEN_DISPS(gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, r, g, b, a);

    w = 8.0f * z;
    h = 12.0f * z;
    unk = (1.0f / z) * 1024;
    dsdx = unk * dx;
    dtdy = dy * unk;

    ulx = (x - w) * 4.0f;
    uly = (y - h) * 4.0f;
    lrx = (x + w) * 4.0f;
    lry = (y + h) * 4.0f;
    gSPTextureRectangle(POLY_OPA_DISP++, ulx, uly, lrx, lry, G_TX_RENDERTILE, s, t, dsdx, dtdy);
    CLOSE_DISPS(gfxCtx);
}

static s16 sUnused = 106;

static s16 sScreenFillAlpha = 255;

static u8 isFastFileIdIncompatible = 0;

static Gfx sScreenFillSetupDL[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_1PRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_CLD_SURF2),
    gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
    gsSPEndDisplayList(),
};

static s16 sFileInfoBoxPartWidths[] = { 36, 36, 36, 36, 24 };

s16 sWindowContentColors[2][3] = {
    { 100, 150, 255 }, // blue
    { 100, 100, 100 }, // gray
};

static int FileChoose_IsSaveCompatible(const SaveFileMetaInfo* restrict meta) {
    bool valid = true;
    if (meta->requiresMasterQuest) {
        valid = valid && ResourceMgr_GameHasMasterQuest();
    }
    if (meta->requiresOriginal) {
        valid = valid && ResourceMgr_GameHasOriginal();
    }
    return valid;
}

void FileChoose_SetView(FileChooseContext* this, f32 eyeX, f32 eyeY, f32 eyeZ) {
    Vec3f eye;
    Vec3f lookAt;
    Vec3f up;

    lookAt.x = lookAt.y = lookAt.z = 0.0f;
    up.x = up.z = 0.0f;

    eye.x = eyeX;
    eye.y = eyeY;
    eye.z = eyeZ;

    up.y = 1.0f;

    func_800AA358(&this->view, &eye, &lookAt, &up);
    func_800AAA50(&this->view, 0x7F);
}

Gfx* FileChoose_QuadTextureIA8(Gfx* gfx, void* texture, s16 width, s16 height, s16 point)
{
    gDPLoadTextureBlock(gfx++, texture, G_IM_FMT_IA, G_IM_SIZ_8b, width, height, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSP1Quadrangle(gfx++, point, point + 2, point + 3, point + 1, 0);

    return gfx;
}

void FileChoose_InitModeUpdate(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    if (this->menuMode == FS_MENU_MODE_INIT) {
        this->menuMode = FS_MENU_MODE_CONFIG;
        this->configMode = CM_FADE_IN_START;
        this->nextTitleLabel = FS_TITLE_OPEN_FILE;
    }
}

void FileChoose_InitModeDraw(GameState* thisx) {
}

/**
 * Fade in the menu window and title label.
 * If a file is occupied fade in the name, name box, and connector.
 * Fade in the copy erase and options button according to the window alpha.
 */
void FileChoose_FadeInMenuElements(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 i;

    this->titleAlpha[0] += VREG(1);
    this->windowAlpha += VREG(2);

    for (i = 0; i < 3; i++) {
        this->fileButtonAlpha[i] = this->windowAlpha;

        if (Save_GetSaveMetaInfo(i)->valid) {
            this->nameBoxAlpha[i] = this->nameAlpha[i] = this->windowAlpha;
            this->connectorAlpha[i] += VREG(1);
            if (this->connectorAlpha[i] >= 255) {
                this->connectorAlpha[i] = 255;
            }
        }
    }

    this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
        this->optionButtonAlpha = this->windowAlpha;
}

/**
 * Converts a numerical value to ones-tens-hundreds digits
 */
void FileChoose_SplitNumber(u16 value, s16* hundreds, s16* tens, s16* ones) {
    *hundreds = 0;
    *tens = 0;
    *ones = value;

    while (true) {
        if ((*ones - 100) < 0) {
            break;
        }
        (*hundreds)++;
        *ones -= 100;
    }

    while (true) {
        if ((*ones - 10) < 0) {
            break;
        }
        (*tens)++;
        *ones -= 10;
    }
}

/**
 * Reduce the alpha of the black screen fill to create a fade in effect.
 * Additionally, slide the window from the right to the center of the screen.
 * Update function for `CM_FADE_IN_START`
 */
void FileChoose_StartFadeIn(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    FileChoose_FadeInMenuElements(&this->state);
    sScreenFillAlpha -= 40;
    this->windowPosX -= 20;

    if (this->windowPosX <= -94) {
        this->windowPosX = -94;
        this->configMode = CM_FADE_IN_END;
        sScreenFillAlpha = 0;
    }
}

/**
 * Finish fading in the remaining menu elements.
 * Fade in the controls text at the bottom of the screen.
 * Update function for `CM_FADE_IN_END`
 */
void FileChoose_FinishFadeIn(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->controlsAlpha += VREG(1);
    FileChoose_FadeInMenuElements(&this->state);

    if (this->titleAlpha[0] >= 255) {
        this->titleAlpha[0] = 255;
        this->controlsAlpha = 255;
        this->windowAlpha = 200;
        this->configMode = CM_MAIN_MENU;
        GameInteractor_ExecuteOnPresentFileSelect();
    }
}

void SpriteLoad(FileChooseContext* this, Sprite* sprite) {
    OPEN_DISPS(this->state.gfxCtx);

    /*
     * Due to macro expansion and the token-pasting operator (##), we cannot pass sprite->im_siz in directly.
     * Instead we must call gDPLoadTextureBlock with the raw IM_SIZ define name itself to properly expand the correct
     * defines internally.
     */

    if (sprite->im_siz == G_IM_SIZ_8b) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, sprite->tex, sprite->im_fmt,
                            G_IM_SIZ_8b,
                            sprite->width, sprite->height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    } else if (sprite->im_siz == G_IM_SIZ_16b) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, sprite->tex, sprite->im_fmt,
                            G_IM_SIZ_16b,
                            sprite->width, sprite->height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    } else {
        gDPLoadTextureBlock(POLY_OPA_DISP++, sprite->tex, sprite->im_fmt,
                            G_IM_SIZ_32b,
                            sprite->width, sprite->height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    }

    CLOSE_DISPS(this->state.gfxCtx);
}

void SpriteDraw(FileChooseContext* this, Sprite* sprite, int left, int top, int width, int height) {
    int width_factor = (1 << 10) * sprite->width / width;
    int height_factor = (1 << 10) * sprite->height / height;

    OPEN_DISPS(this->state.gfxCtx);

    gSPWideTextureRectangle(POLY_OPA_DISP++, left << 2, top << 2, (left + width) << 2, (top + height) << 2,
                            G_TX_RENDERTILE, 0, 0, width_factor, height_factor);

    CLOSE_DISPS(this->state.gfxCtx);
}

bool fileSelectSpoilerFileLoaded = false;

void DrawSeedHashSprites(FileChooseContext* this) {
    OPEN_DISPS(this->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    // Draw icons on the main menu, when a rando file is selected, and when quest selection is set to rando
    if ((this->configMode == CM_MAIN_MENU &&
        (this->selectMode != SM_CONFIRM_FILE || Save_GetSaveMetaInfo(this->selectedFileIndex)->randoSave == 1)) ||
        (this->configMode == CM_QUEST_MENU && this->questType[this->buttonIndex] == FS_QUEST_RANDOMIZER)) {

        if (this->fileInfoAlpha[this->selectedFileIndex] > 0) {
            // Use file info alpha to match fading
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0xFF, 0xFF, 0xFF, this->fileInfoAlpha[this->selectedFileIndex]);

            u16 xStart = 64;
            // Draw Seed Icons for specific file
            for (unsigned int i = 0; i < 5; i++) {
                if (Save_GetSaveMetaInfo(this->selectedFileIndex)->randoSave == 1) {
                    SpriteLoad(this, GetSeedTexture(Save_GetSaveMetaInfo(this->selectedFileIndex)->seedHash[i]));
                    SpriteDraw(this, GetSeedTexture(Save_GetSaveMetaInfo(this->selectedFileIndex)->seedHash[i]),
                                xStart + (40 * i), 10, 24, 24);
                }
            }
        }

        // Fade top seed icons based on main menu fade and if save supports rando
        u8 alpha = MAX(this->optionButtonAlpha, Save_GetSaveMetaInfo(this->selectedFileIndex)->randoSave == 1 ? 0xFF : 0);
        if (alpha >= 200) {
            alpha = 0xFF;
        }

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0xFF, 0xFF, 0xFF, alpha);

        // Draw Seed Icons for spoiler log
        if (this->configMode == CM_QUEST_MENU && this->questType[this->buttonIndex] == FS_QUEST_RANDOMIZER && strnlen(CVarGetString("gSpoilerLog", ""), 1) != 0 && fileSelectSpoilerFileLoaded) {
            u16 xStart = 64;
            for (unsigned int i = 0; i < 5; i++) {
                SpriteLoad(this, GetSeedTexture(gSaveContext.seedIcons[i]));
                SpriteDraw(this, GetSeedTexture(gSaveContext.seedIcons[i]), xStart + (40 * i), 10, 24, 24);
            }
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);

    CLOSE_DISPS(this->state.gfxCtx);
}

u8 generating;

void FileChoose_UpdateRandomizer() {
    if (CVarGetInteger("gRandoGenerating", 0) != 0 && generating == 0) {
            generating = 1;
            func_800F5E18(SEQ_PLAYER_BGM_MAIN, NA_BGM_HORSE, 0, 7, 1);
            return;
    } else if (CVarGetInteger("gRandoGenerating", 0) == 0 && generating) {
            if (SpoilerFileExists(CVarGetString("gSpoilerLog", ""))) {
                Audio_PlayFanfare(NA_BGM_HORSE_GOAL);
            } else {
                func_80078884(NA_SE_SY_OCARINA_ERROR);
            }
            func_800F5E18(SEQ_PLAYER_BGM_MAIN, NA_BGM_FILE_SELECT, 0, 7, 1);
            generating = 0;
            return;
    } else if (generating) {
            return;
    }

    if (!SpoilerFileExists(CVarGetString("gSpoilerLog", ""))) {
            CVarSetString("gSpoilerLog", "");
            fileSelectSpoilerFileLoaded = false;
    }

    if ((CVarGetInteger("gNewFileDropped", 0) != 0) || (CVarGetInteger("gNewSeedGenerated", 0) != 0) ||
        (!fileSelectSpoilerFileLoaded && SpoilerFileExists(CVarGetString("gSpoilerLog", "")))) {
            if (CVarGetInteger("gNewFileDropped", 0) != 0) {
            CVarSetString("gSpoilerLog", CVarGetString("gDroppedFile", "None"));
            }
            bool silent = true;
            if ((CVarGetInteger("gNewFileDropped", 0) != 0) || (CVarGetInteger("gNewSeedGenerated", 0) != 0)) {
            silent = false;
            }
            CVarSetInteger("gNewSeedGenerated", 0);
            CVarSetInteger("gNewFileDropped", 0);
            CVarSetString("gDroppedFile", "");
            fileSelectSpoilerFileLoaded = false;
            const char* fileLoc = CVarGetString("gSpoilerLog", "");
            Randomizer_LoadSettings(fileLoc);
            Randomizer_LoadHintLocations(fileLoc);
            Randomizer_LoadRequiredTrials(fileLoc);
            Randomizer_LoadItemLocations(fileLoc, silent);
            Randomizer_LoadMerchantMessages(fileLoc);
            Randomizer_LoadMasterQuestDungeons(fileLoc);
            Randomizer_LoadEntranceOverrides(fileLoc, silent);
            fileSelectSpoilerFileLoaded = true;
    }
}

static s16 sLastFileChooseButtonIndex;

/**
 * Update the cursor and wait for the player to select a button to change menus accordingly.
 * If an empty file is selected, enter the name entry config mode.
 * If an occupied file is selected, enter the `Select` menu mode.
 * If copy, erase, or options is selected, set config mode accordingly.
 * Lastly, set any warning labels if appropriate.
 * Update function for `CM_MAIN_MENU`
 */
void FileChoose_UpdateMainMenu(GameState* thisx) {
    static u8 emptyName[] = { 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E };
    static u8 linkName[] = { 0x15, 0x2C, 0x31, 0x2E, 0x3E, 0x3E, 0x3E, 0x3E };
    FileChooseContext* this = (FileChooseContext*)thisx;
    Input* input = &this->state.input[0];
    bool dpad = CVarGetInteger("gDpadText", 0);

    FileChoose_UpdateRandomizer();

    if (CHECK_BTN_ALL(input->press.button, BTN_START) || CHECK_BTN_ALL(input->press.button, BTN_A)) {
        if (this->buttonIndex <= FS_BTN_MAIN_FILE_3) {
            if (!Save_GetSaveMetaInfo(this->buttonIndex)->valid) {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                this->prevConfigMode = this->configMode;
                this->configMode = CM_ROTATE_TO_QUEST_MENU;
                this->logoAlpha = 0;
            } else if(!FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(this->buttonIndex))) {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }  
            else {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                this->actionTimer = 8;
                this->selectMode = SM_FADE_MAIN_TO_SELECT;
                this->selectedFileIndex = this->buttonIndex;
                this->menuMode = FS_MENU_MODE_SELECT;
                this->nextTitleLabel = FS_TITLE_OPEN_FILE;
            }
        } else {
            if (this->warningLabel == FS_WARNING_NONE) {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                this->prevConfigMode = this->configMode;

                if (this->buttonIndex == FS_BTN_MAIN_COPY) {
                    this->configMode = CM_SETUP_COPY_SOURCE;
                    this->nextTitleLabel = FS_TITLE_COPY_FROM;
                } else if (this->buttonIndex == FS_BTN_MAIN_ERASE) {
                    this->configMode = CM_SETUP_ERASE_SELECT;
                    this->nextTitleLabel = FS_TITLE_ERASE_FILE;
                } else {
                    this->configMode = CM_MAIN_TO_OPTIONS;
                    this->kbdButton = 0;
                    this->kbdX = 0;
                    this->kbdY = 0;
                    this->charBgAlpha = 0;
                    this->newFileNameCharCount = 0;
                    this->nameEntryBoxPosX = 120;
                }

                sLastFileChooseButtonIndex = -1;

                this->actionTimer = 8;
            } else {
                Audio_PlaySoundGeneral(NA_SE_SY_FSEL_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }
        }
    } else {
        if ((ABS(this->stickRelY) > 30) || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DDOWN | BTN_DUP))) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

            if ((this->stickRelY > 30) || (dpad && CHECK_BTN_ALL(input->press.button, BTN_DUP))) {
                this->buttonIndex--;
                if (this->buttonIndex < FS_BTN_MAIN_FILE_1) {
                    this->buttonIndex = FS_BTN_MAIN_OPTIONS;
                }
            } else {
                this->buttonIndex++;
                if (this->buttonIndex > FS_BTN_MAIN_OPTIONS) {
                    this->buttonIndex = FS_BTN_MAIN_FILE_1;
                }
            }
        }

        if (this->buttonIndex == FS_BTN_MAIN_COPY) {
            if (!Save_GetSaveMetaInfo(0)->valid && !Save_GetSaveMetaInfo(1)->valid && !Save_GetSaveMetaInfo(2)->valid) {
                this->warningButtonIndex = this->buttonIndex;
                this->warningLabel = FS_WARNING_NO_FILE_COPY;
                this->emptyFileTextAlpha = 255;
            } else if (Save_GetSaveMetaInfo(0)->valid && Save_GetSaveMetaInfo(1)->valid &&
                       Save_GetSaveMetaInfo(2)->valid) {
                this->warningButtonIndex = this->buttonIndex;
                this->warningLabel = FS_WARNING_NO_EMPTY_FILES;
                this->emptyFileTextAlpha = 255;
            } else {
                this->warningLabel = FS_WARNING_NONE;
            }
        } else if (this->buttonIndex == FS_BTN_MAIN_ERASE) {
            if (!Save_GetSaveMetaInfo(0)->valid && !Save_GetSaveMetaInfo(1)->valid && !Save_GetSaveMetaInfo(2)->valid) {
                this->warningButtonIndex = this->buttonIndex;
                this->warningLabel = FS_WARNING_NO_FILE_ERASE;
                this->emptyFileTextAlpha = 255;
            } else {
                this->warningLabel = FS_WARNING_NONE;
            }
        } else {
            this->warningLabel = FS_WARNING_NONE;
        }

        if (sLastFileChooseButtonIndex != this->buttonIndex) {
            GameInteractor_ExecuteOnUpdateFileSelectSelection(this->buttonIndex);
            sLastFileChooseButtonIndex = this->buttonIndex;
        }
    }
}

void FileChoose_UpdateStickDirectionPromptAnim(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    f32 arrowAnimTween;
    f32 new_var3;       // likely fake temp
    s32 new_var2 = 155; // likely fake temp
    f32 stickAnimTween;

    arrowAnimTween = this->arrowAnimTween;
    stickAnimTween = this->stickAnimTween;
    if (this->arrowAnimState == 0) {
        arrowAnimTween += 0.027f;
        if (arrowAnimTween > 1.0f) {
            arrowAnimTween = 1.0f;
            this->arrowAnimState = 1;
        }

    } else {
        arrowAnimTween -= 0.027f;
        if (arrowAnimTween < 0.0f) {
            arrowAnimTween = 0.0f;
            this->arrowAnimState = 0;
        }
    }

    this->arrowAnimTween = arrowAnimTween;
    if (this->stickAnimState == 0) {
        stickAnimTween += 0.027f;
        if (stickAnimTween > 1.0f) {
            stickAnimTween = 1.0f;
            this->stickAnimState = 1;
        }

    } else {
        stickAnimTween = 0.0f;
        this->stickAnimState = 0;
    }

    this->stickAnimTween = stickAnimTween;
    this->stickLeftPrompt.arrowColorR = (u8)(155 - ((s32)(75.0f * arrowAnimTween)));
    this->stickLeftPrompt.arrowColorG = (u8)(new_var2 - (s32)(55.0f * arrowAnimTween));
    new_var3 = (75.0f * arrowAnimTween);
    this->stickLeftPrompt.arrowColorB = (u8)(255 - ((s32)(75.0f * arrowAnimTween)));
    this->stickLeftPrompt.arrowColorA = (u8)(200 - ((s32)(50.0f * arrowAnimTween)));
    this->stickRightPrompt.arrowColorR = (u8)(new_var2 - (s32)new_var3);
    this->stickRightPrompt.arrowColorG = (u8)(155 - (s32)new_var3);
    this->stickRightPrompt.arrowColorB = (u8)(255 - ((s32)(75.0f * arrowAnimTween)));
    this->stickRightPrompt.arrowColorA = (u8)(200 - ((s32)(50.0f * arrowAnimTween)));
    this->stickRightPrompt.arrowTexX = 260.0f;
    this->stickLeftPrompt.arrowTexX = 63.0f;
    this->stickRightPrompt.stickTexX = 244.0f;
    this->stickLeftPrompt.stickTexX = 79.0f;
    this->stickRightPrompt.stickTexX += (8.0f * stickAnimTween);
    this->stickLeftPrompt.stickTexX -= (8.0f * stickAnimTween);
    this->stickLeftPrompt.arrowTexY = this->stickRightPrompt.arrowTexY = 135.0f;
    this->stickLeftPrompt.stickTexY = this->stickRightPrompt.stickTexY = 139.0f;
}

void FileChoose_StartQuestMenu(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->logoAlpha += 25;

    if (this->logoAlpha >= 255) {
        this->logoAlpha = 255;
        this->configMode = CM_QUEST_MENU;

        GameInteractor_ExecuteOnUpdateFileQuestSelection(this->questType[this->buttonIndex]);
    }
}

void FileChoose_StartBossRushMenu(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->logoAlpha -= 25;
    this->bossRushUIAlpha = 0;
    this->bossRushArrowOffset = 0;

    if (this->logoAlpha >= 0) {
        this->logoAlpha = 0;
        this->configMode = CM_BOSS_RUSH_MENU;
    }
}

void FileChoose_UpdateQuestMenu(GameState* thisx) {
    static u8 emptyName[] = { 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E };
    static u8 linkName[] = { 0x15, 0x2C, 0x31, 0x2E, 0x3E, 0x3E, 0x3E, 0x3E };
    FileChoose_UpdateStickDirectionPromptAnim(thisx);
    FileChooseContext* this = (FileChooseContext*)thisx;
    Input* input = &this->state.input[0];
    s8 i = 0;
    bool dpad = CVarGetInteger("gDpadText", 0);

    FileChoose_UpdateRandomizer();

    if (ABS(this->stickRelX) > 30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DLEFT | BTN_DRIGHT))) {
        if (this->stickRelX > 30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DRIGHT))) {
            this->questType[this->buttonIndex] += 1;
            while ((this->questType[this->buttonIndex] == FS_QUEST_MASTER && !ResourceMgr_GameHasMasterQuest()) || 
                (this->questType[this->buttonIndex] == FS_QUEST_RANDOMIZER && !hasRandomizerQuest())) {
                // If Master Quest is selected without a Master Quest OTR present or when Randomizer Quest is
                // selected without a loaded Randomizer seed, skip past it.
                this->questType[this->buttonIndex] += 1;
            }
        } else if (this->stickRelX < -30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DLEFT))) {
            this->questType[this->buttonIndex] -= 1;
            while ((this->questType[this->buttonIndex] == FS_QUEST_MASTER && !ResourceMgr_GameHasMasterQuest()) ||
                (this->questType[this->buttonIndex] == FS_QUEST_RANDOMIZER && !hasRandomizerQuest())) {
                // If Master Quest is selected without a Master Quest OTR present or when Randomizer Quest is
                // selected without a loaded Randomizer seed, skip past it.
                this->questType[this->buttonIndex] -= 1;
            }
        }

        // If current buttonIndex is higher or lower than the min/max value, wrap around.
        if (this->questType[this->buttonIndex] > MAX_QUEST) {
            this->questType[this->buttonIndex] = MIN_QUEST;
        } else if (this->questType[this->buttonIndex] < MIN_QUEST) {
            this->questType[this->buttonIndex] = MAX_QUEST;
        }

        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

        GameInteractor_ExecuteOnUpdateFileQuestSelection(this->questType[this->buttonIndex]);
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_A)) {

        gSaveContext.isMasterQuest = this->questType[this->buttonIndex] == FS_QUEST_MASTER;
        gSaveContext.n64ddFlag = this->questType[this->buttonIndex] == FS_QUEST_RANDOMIZER;
        gSaveContext.isBossRush = this->questType[this->buttonIndex] == FS_QUEST_BOSSRUSH;
        gSaveContext.isBossRushPaused = false;

        if (this->questType[this->buttonIndex] == FS_QUEST_BOSSRUSH) {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            this->prevConfigMode = this->configMode;
            this->configMode = CM_ROTATE_TO_BOSS_RUSH_MENU;
            return;
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            osSyncPrintf("Selected Dungeon Quest: %d\n", gSaveContext.isMasterQuest);
            this->prevConfigMode = this->configMode;
            this->configMode = CM_ROTATE_TO_NAME_ENTRY;
            this->logoAlpha = 0;
            CVarSetInteger("gOnFileSelectNameEntry", 1);
            this->kbdButton = FS_KBD_BTN_NONE;
            this->charPage = FS_CHAR_PAGE_ENG;
            this->kbdX = 0;
            this->kbdY = 0;
            this->charIndex = 0;
            this->charBgAlpha = 0;
            this->newFileNameCharCount = CVarGetInteger("gLinkDefaultName", 0) ? 4 : 0;
            this->nameEntryBoxPosX = 120;
            this->nameEntryBoxAlpha = 0;
            memcpy(Save_GetSaveMetaInfo(this->buttonIndex)->playerName, CVarGetInteger("gLinkDefaultName", 0) ? &linkName : &emptyName, 8);
            return;
        }
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->configMode = CM_QUEST_TO_MAIN;
        sLastFileChooseButtonIndex = -1;
        return;
    }
}

static s8 sLastBossRushOptionIndex = -1;
static s8 sLastBossRushOptionValue = -1;

void FileChoose_UpdateBossRushMenu(GameState* thisx) {
    FileChoose_UpdateStickDirectionPromptAnim(thisx);
    FileChooseContext* this = (FileChooseContext*)thisx;
    Input* input = &this->state.input[0];
    bool dpad = CVarGetInteger("gDpadText", 0);

    // Fade in elements after opening Boss Rush options menu
    this->bossRushUIAlpha += 25;
    if (this->bossRushUIAlpha > 255) {
        this->bossRushUIAlpha = 255;
    }

    // Animate up/down arrows.
    this->bossRushArrowOffset += 1;
    if (this->bossRushArrowOffset >= 30) {
        this->bossRushArrowOffset = 0;
    }
    
    // Move menu selection up or down.
    if (ABS(this->stickRelY) > 30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DDOWN | BTN_DUP))) {
        // Move down
        if (this->stickRelY < -30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DDOWN))) {
            // When selecting past the last option, cycle back to the first option.
            if ((this->bossRushIndex + 1) > BOSSRUSH_OPTIONS_AMOUNT - 1) {
                this->bossRushIndex = 0;
                this->bossRushOffset = 0;
            } else {
                this->bossRushIndex++;
                // When last visible option is selected when moving down, offset the list down by one.
                if (this->bossRushIndex - this->bossRushOffset > BOSSRUSH_MAX_OPTIONS_ON_SCREEN - 1) {
                    this->bossRushOffset++;
                }
            }
        } else if (this->stickRelY > 30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DUP))) {
            // When selecting past the first option, cycle back to the last option and offset the list to view it properly.
            if ((this->bossRushIndex - 1) < 0) {
                this->bossRushIndex = BOSSRUSH_OPTIONS_AMOUNT - 1;
                this->bossRushOffset = this->bossRushIndex - BOSSRUSH_MAX_OPTIONS_ON_SCREEN + 1;
            } else {
                // When first visible option is selected when moving up, offset the list up by one.
                if (this->bossRushIndex - this->bossRushOffset == 0) {
                    this->bossRushOffset--;
                }
                this->bossRushIndex--;
            }
        }

        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }

    // Cycle through choices for currently selected option.
    if (ABS(this->stickRelX) > 30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DLEFT | BTN_DRIGHT))) {
        if (this->stickRelX > 30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DRIGHT))) {
            // If exceeding the amount of choices for the selected option, cycle back to the first.
            if ((gSaveContext.bossRushOptions[this->bossRushIndex] + 1) == BossRush_GetSettingOptionsAmount(this->bossRushIndex)) {
                gSaveContext.bossRushOptions[this->bossRushIndex] = 0;
            } else {
                gSaveContext.bossRushOptions[this->bossRushIndex]++;
            }
        } else if (this->stickRelX < -30 || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DLEFT))) {
            // If cycling back when already at the first choice for the selected option, cycle back to the last choice.
            if ((gSaveContext.bossRushOptions[this->bossRushIndex] - 1) < 0) {
                gSaveContext.bossRushOptions[this->bossRushIndex] = BossRush_GetSettingOptionsAmount(this->bossRushIndex) - 1;
            } else {
                gSaveContext.bossRushOptions[this->bossRushIndex]--;
            }
        }

        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    }

    if (sLastBossRushOptionIndex != this->bossRushIndex ||
        sLastBossRushOptionValue != gSaveContext.bossRushOptions[this->bossRushIndex]) {
        GameInteractor_ExecuteOnUpdateFileBossRushOptionSelection(this->bossRushIndex, gSaveContext.bossRushOptions[this->bossRushIndex]);
        sLastBossRushOptionIndex = this->bossRushIndex;
        sLastBossRushOptionValue = gSaveContext.bossRushOptions[this->bossRushIndex];
    }

    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->configMode = CM_BOSS_RUSH_TO_QUEST;
        return;
    }

    // Load into the game.
    if (CHECK_BTN_ALL(input->press.button, BTN_START) || CHECK_BTN_ALL(input->press.button, BTN_A)) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        this->buttonIndex = 0xFE;
        this->menuMode = FS_MENU_MODE_SELECT;
        this->selectMode = SM_FADE_OUT;
        this->prevConfigMode = this->configMode;
        return;
    }
}

/**
 * Update function for `CM_UNUSED_31`
 */
void FileChoose_UnusedCM31(GameState* thisx) {
}

/**
 * Delay the next config mode from running until `XREG(73)` reaches 254.
 * Because the timer increments by 2, the delay is 127 frames (assuming the value was not changed by reg editor).
 * Unused in the final game, was possibly used for debugging.
 * Update function for `CM_UNUSED_DELAY`
 */
void FileChoose_UnusedCMDelay(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    XREG(73) += 2;

    if (XREG(73) == 254) {
        this->configMode = this->nextConfigMode;
        XREG(73) = 0;
    }
}

/**
 * Rotate the window from the main menu to the name entry menu.
 * Update function for `CM_ROTATE_TO_NAME_ENTRY`
 */
void FileChoose_RotateToNameEntry(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->windowRot += VREG(16);

    if (this->windowRot >= 628.0f) {
        this->windowRot = 628.0f;
        this->configMode = CM_START_NAME_ENTRY;
    }
}

/**
 * Rotate the window from the main menu to the options menu.
 * Update function for `CM_MAIN_TO_OPTIONS`
 */
void FileChoose_RotateToOptions(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->windowRot += VREG(16);

    if (this->windowRot >= 314.0f) {
        this->windowRot = 314.0f;
        this->configMode = CM_START_OPTIONS;
    }
}

/**
 * Rotate the window from the options menu to the main menu.
 * Update function for `CM_NAME_ENTRY_TO_MAIN` and `CM_OPTIONS_TO_MAIN`
 */
void FileChoose_RotateToMain(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    if (this->configMode == CM_QUEST_TO_MAIN || this->configMode == CM_OPTIONS_TO_MAIN) {
        this->windowRot -= VREG(16);

        if (this->windowRot <= 0.0f) {
            this->windowRot = 0.0f;
            this->configMode = CM_MAIN_MENU;
        }
    }

    if (this->configMode == CM_NAME_ENTRY_TO_MAIN && this->prevConfigMode == CM_MAIN_MENU) {
        this->windowRot += VREG(16);

        if (this->windowRot >= 942.0f) {
            this->windowRot = 0.0f;
            this->configMode = CM_MAIN_MENU;
        }
    }
}

void FileChoose_RotateToQuest(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    if (this->configMode == CM_NAME_ENTRY_TO_QUEST_MENU || this->configMode == CM_BOSS_RUSH_TO_QUEST) {
        this->windowRot -= VREG(16);

        if (this->windowRot <= 314.0f) {
            this->windowRot = 314.0f;
            this->configMode = CM_START_QUEST_MENU;
        }
    } else {
        this->windowRot += VREG(16);

        if (this->windowRot >= 314.0f) {
            this->windowRot = 314.0f;
            this->configMode = CM_START_QUEST_MENU;
        }
    }
}

void FileChoose_RotateToBossRush(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->windowRot += VREG(16);

    if (this->windowRot >= 628.0f) {
        this->windowRot = 628.0f;
        this->configMode = CM_START_BOSS_RUSH_MENU;
    }
}

static void (*gConfigModeUpdateFuncs[])(GameState*) = {
    FileChoose_StartFadeIn,        FileChoose_FinishFadeIn,
    FileChoose_UpdateMainMenu,     FileChoose_SetupCopySource,
    FileChoose_SelectCopySource,   FileChoose_SetupCopyDest1,
    FileChoose_SetupCopyDest2,     FileChoose_SelectCopyDest,
    FileChoose_ExitToCopySource1,  FileChoose_ExitToCopySource2,
    FileChoose_SetupCopyConfirm1,  FileChoose_SetupCopyConfirm2,
    FileChoose_CopyConfirm,        FileChoose_ReturnToCopyDest,
    FileChoose_CopyAnim1,          FileChoose_CopyAnim2,
    FileChoose_CopyAnim3,          FileChoose_CopyAnim4,
    FileChoose_CopyAnim5,          FileChoose_ExitCopyToMain,
    FileChoose_SetupEraseSelect,   FileChoose_EraseSelect,
    FileChoose_SetupEraseConfirm1, FileChoose_SetupEraseConfirm2,
    FileChoose_EraseConfirm,       FileChoose_ExitToEraseSelect1,
    FileChoose_ExitToEraseSelect2, FileChoose_EraseAnim1,
    FileChoose_EraseAnim2,         FileChoose_EraseAnim3,
    FileChoose_ExitEraseToMain,    FileChoose_UnusedCM31,
    FileChoose_RotateToNameEntry,  FileChoose_UpdateKeyboardCursor,
    FileChoose_StartNameEntry,     FileChoose_RotateToMain,
    FileChoose_RotateToOptions,    FileChoose_UpdateOptionsMenu,
    FileChoose_StartOptions,       FileChoose_RotateToMain,
    FileChoose_UnusedCMDelay,      FileChoose_RotateToQuest,
    FileChoose_UpdateQuestMenu,    FileChoose_StartQuestMenu,
    FileChoose_RotateToMain,       FileChoose_RotateToQuest,
    FileChoose_RotateToBossRush,   FileChoose_UpdateBossRushMenu,
    FileChoose_StartBossRushMenu,  FileChoose_RotateToQuest,
};

/**
 * Updates the alpha of the cursor to make it pulsate.
 * On the debug rom, this function also handles switching languages with controller 3.
 */
void FileChoose_PulsateCursor(GameState* thisx) {
    static s16 cursorAlphaTargets[] = { 70, 200 };
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 alphaStep;

    alphaStep = ABS(this->highlightColor[3] - cursorAlphaTargets[this->highlightPulseDir]) / XREG(35);

    if (this->highlightColor[3] >= cursorAlphaTargets[this->highlightPulseDir]) {
        this->highlightColor[3] -= alphaStep;
    } else {
        this->highlightColor[3] += alphaStep;
    }

    XREG(35)--;

    if (XREG(35) == 0) {
        this->highlightColor[3] = cursorAlphaTargets[this->highlightPulseDir];
        XREG(35) = XREG(36 + this->highlightPulseDir);
        this->highlightPulseDir ^= 1;
    }
}

void FileChoose_ConfigModeUpdate(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    gConfigModeUpdateFuncs[this->configMode](&this->state);
}

void FileChoose_SetWindowVtx(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 i;
    s16 j;
    s16 x;
    s16 tmp;
    s16 tmp2;
    s16 tmp3;

    this->windowVtx = Graph_Alloc(this->state.gfxCtx, sizeof(Vtx) * 80);
    tmp = this->windowPosX - 90;

    for (x = 0, i = 0; i < 4; i++) {
        tmp += 0x40;
        tmp2 = (i == 3) ? 0x30 : 0x40;

        for (j = 0, tmp3 = 0x50; j < 5; j++, x += 4, tmp3 -= 0x20) {
            this->windowVtx[x].v.ob[0] = this->windowVtx[x + 2].v.ob[0] = tmp;

            this->windowVtx[x + 1].v.ob[0] = this->windowVtx[x + 3].v.ob[0] = tmp + tmp2;

            this->windowVtx[x].v.ob[1] = this->windowVtx[x + 1].v.ob[1] = tmp3;

            this->windowVtx[x + 2].v.ob[1] = this->windowVtx[x + 3].v.ob[1] = tmp3 - 0x20;

            this->windowVtx[x].v.ob[2] = this->windowVtx[x + 1].v.ob[2] = this->windowVtx[x + 2].v.ob[2] =
                this->windowVtx[x + 3].v.ob[2] = 0;

            this->windowVtx[x].v.flag = this->windowVtx[x + 1].v.flag = this->windowVtx[x + 2].v.flag =
                this->windowVtx[x + 3].v.flag = 0;

            this->windowVtx[x].v.tc[0] = this->windowVtx[x].v.tc[1] = this->windowVtx[x + 1].v.tc[1] =
                this->windowVtx[x + 2].v.tc[0] = 0;

            this->windowVtx[x + 1].v.tc[0] = this->windowVtx[x + 3].v.tc[0] = tmp2 * 0x20;

            this->windowVtx[x + 2].v.tc[1] = this->windowVtx[x + 3].v.tc[1] = 0x400;

            this->windowVtx[x].v.cn[0] = this->windowVtx[x + 2].v.cn[0] = this->windowVtx[x].v.cn[1] =
                this->windowVtx[x + 2].v.cn[1] = this->windowVtx[x].v.cn[2] = this->windowVtx[x + 2].v.cn[2] =
                    this->windowVtx[x + 1].v.cn[0] = this->windowVtx[x + 3].v.cn[0] = this->windowVtx[x + 1].v.cn[1] =
                        this->windowVtx[x + 3].v.cn[1] = this->windowVtx[x + 1].v.cn[2] =
                            this->windowVtx[x + 3].v.cn[2] = this->windowVtx[x].v.cn[3] =
                                this->windowVtx[x + 2].v.cn[3] = this->windowVtx[x + 1].v.cn[3] =
                                    this->windowVtx[x + 3].v.cn[3] = 255;
        }
    }
}

static s16 D_80812818[] = { 0x001A, 0x000A, 0x000A, 0x000A };
static s16 D_80812820[] = { 0x0020, 0x000C, 0x000C, 0x000C };
static s16 D_80812828[] = { 0x0010, 0x000C, 0x000C, 0x000C };
static s16 D_80812830[] = { 0x0040, 0x0054, 0x0068, 0x0274, 0x0278, 0x027C };
static s16 D_8081283C[] = { 0x0040, 0x0054, 0x0068, 0x0278 };
static s16 D_80812844[] = { 0x0274, 0x0278 };
static s16 D_80812848[] = { 0x0274, 0x0278 };

void FileChoose_SetWindowContentVtx(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 phi_t2;
    s16 phi_t0;
    s16 phi_t5;
    s16 phi_a1;
    s16 phi_ra;
    s16 temp_t1;

    this->windowContentVtx = Graph_Alloc(this->state.gfxCtx, 0x288 * sizeof(Vtx));

    for (phi_t2 = 0; phi_t2 < 0x288; phi_t2 += 4) {
        this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = 0x12C;
        this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
            this->windowContentVtx[phi_t2].v.ob[0] + 0x10;

        this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] = 0;
        this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
            this->windowContentVtx[phi_t2].v.ob[1] - 0x10;

        this->windowContentVtx[phi_t2].v.ob[2] = this->windowContentVtx[phi_t2 + 1].v.ob[2] =
            this->windowContentVtx[phi_t2 + 2].v.ob[2] = this->windowContentVtx[phi_t2 + 3].v.ob[2] = 0;

        this->windowContentVtx[phi_t2].v.flag = this->windowContentVtx[phi_t2 + 1].v.flag =
            this->windowContentVtx[phi_t2 + 2].v.flag = this->windowContentVtx[phi_t2 + 3].v.flag = 0;

        this->windowContentVtx[phi_t2].v.tc[0] = this->windowContentVtx[phi_t2].v.tc[1] =
            this->windowContentVtx[phi_t2 + 1].v.tc[1] = this->windowContentVtx[phi_t2 + 2].v.tc[0] = 0;

        this->windowContentVtx[phi_t2 + 1].v.tc[0] = this->windowContentVtx[phi_t2 + 2].v.tc[1] =
            this->windowContentVtx[phi_t2 + 3].v.tc[0] = this->windowContentVtx[phi_t2 + 3].v.tc[1] = 0x200;

        this->windowContentVtx[phi_t2].v.cn[0] = this->windowContentVtx[phi_t2 + 1].v.cn[0] =
            this->windowContentVtx[phi_t2 + 2].v.cn[0] = this->windowContentVtx[phi_t2 + 3].v.cn[0] =
                this->windowContentVtx[phi_t2].v.cn[1] = this->windowContentVtx[phi_t2 + 1].v.cn[1] =
                    this->windowContentVtx[phi_t2 + 2].v.cn[1] = this->windowContentVtx[phi_t2 + 3].v.cn[1] =
                        this->windowContentVtx[phi_t2].v.cn[2] = this->windowContentVtx[phi_t2 + 1].v.cn[2] =
                            this->windowContentVtx[phi_t2 + 2].v.cn[2] = this->windowContentVtx[phi_t2 + 3].v.cn[2] =
                                this->windowContentVtx[phi_t2].v.cn[3] = this->windowContentVtx[phi_t2 + 1].v.cn[3] =
                                    this->windowContentVtx[phi_t2 + 2].v.cn[3] =
                                        this->windowContentVtx[phi_t2 + 3].v.cn[3] = 0xFF;
    }

    this->windowContentVtx[0].v.ob[0] = this->windowContentVtx[2].v.ob[0] = this->windowPosX;
    this->windowContentVtx[1].v.ob[0] = this->windowContentVtx[3].v.ob[0] = this->windowContentVtx[0].v.ob[0] + 0x80;
    this->windowContentVtx[0].v.ob[1] = this->windowContentVtx[1].v.ob[1] = 0x48;
    this->windowContentVtx[2].v.ob[1] = this->windowContentVtx[3].v.ob[1] = this->windowContentVtx[0].v.ob[1] - 0x10;
    this->windowContentVtx[1].v.tc[0] = this->windowContentVtx[3].v.tc[0] = 0x1000;

    for (phi_a1 = 0, phi_t2 = 4; phi_a1 < 3; phi_a1++) {
        phi_t0 = this->windowPosX - 6;

        for (phi_t5 = 0; phi_t5 < 5; phi_t5++, phi_t2 += 4) {
            this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
            this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
                this->windowContentVtx[phi_t2].v.ob[0] + sFileInfoBoxPartWidths[phi_t5];

            this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] =
                this->fileNamesY[phi_a1] + 0x2C;

            this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
                this->windowContentVtx[phi_t2].v.ob[1] - 0x38;

            this->windowContentVtx[phi_t2 + 1].v.tc[0] = this->windowContentVtx[phi_t2 + 3].v.tc[0] =
                sFileInfoBoxPartWidths[phi_t5] << 5;
            this->windowContentVtx[phi_t2 + 2].v.tc[1] = this->windowContentVtx[phi_t2 + 3].v.tc[1] = 0x700;
            phi_t0 += sFileInfoBoxPartWidths[phi_t5];
        }
    }

    phi_t0 = this->windowPosX - 6;
    phi_ra = 0x2C;

    for (phi_t5 = 0; phi_t5 < 3; phi_t5++, phi_t2 += 20, phi_ra -= 0x10) {
        this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;

        this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
            this->windowContentVtx[phi_t2].v.ob[0] + 0x40;

        this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] =
            this->buttonYOffsets[phi_t5] + phi_ra;
        this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
            this->windowContentVtx[phi_t2].v.ob[1] - 0x10;

        this->windowContentVtx[phi_t2 + 1].v.tc[0] = this->windowContentVtx[phi_t2 + 3].v.tc[0] = 0x800;

        this->windowContentVtx[phi_t2 + 4].v.ob[0] = this->windowContentVtx[phi_t2 + 6].v.ob[0] = phi_t0 + 0x40;

        this->windowContentVtx[phi_t2 + 5].v.ob[0] = this->windowContentVtx[phi_t2 + 7].v.ob[0] =
            this->windowContentVtx[phi_t2 + 4].v.ob[0] + 0x6C;

        this->windowContentVtx[phi_t2 + 4].v.ob[1] = this->windowContentVtx[phi_t2 + 5].v.ob[1] =
            this->buttonYOffsets[phi_t5] + phi_ra;

        this->windowContentVtx[phi_t2 + 6].v.ob[1] = this->windowContentVtx[phi_t2 + 7].v.ob[1] =
            this->windowContentVtx[phi_t2 + 4].v.ob[1] - 0x10;

        this->windowContentVtx[phi_t2 + 5].v.tc[0] = this->windowContentVtx[phi_t2 + 7].v.tc[0] = 0xD80;

        if ((this->configMode == CM_COPY_ANIM_2) && (phi_t5 == this->copyDestFileIndex)) {
            temp_t1 = this->fileNamesY[phi_t5] + 0x2C;
        } else if (((this->configMode == CM_COPY_ANIM_3) || (this->configMode == CM_COPY_ANIM_4)) &&
                   (phi_t5 == this->copyDestFileIndex)) {
            temp_t1 = this->buttonYOffsets[phi_t5] + phi_ra;
        } else {
            temp_t1 = phi_ra + this->buttonYOffsets[phi_t5] + this->fileNamesY[phi_t5];
        }

        this->windowContentVtx[phi_t2 + 8].v.ob[0] = this->windowContentVtx[phi_t2 + 10].v.ob[0] = phi_t0 + 0xA8;

        this->windowContentVtx[phi_t2 + 9].v.ob[0] = this->windowContentVtx[phi_t2 + 11].v.ob[0] =
            this->windowContentVtx[phi_t2 + 8].v.ob[0] + 0x2C;

        this->windowContentVtx[phi_t2 + 8].v.ob[1] = this->windowContentVtx[phi_t2 + 9].v.ob[1] = temp_t1;
        this->windowContentVtx[phi_t2 + 10].v.ob[1] = this->windowContentVtx[phi_t2 + 11].v.ob[1] =
            this->windowContentVtx[phi_t2 + 8].v.ob[1] - 0x10;

        this->windowContentVtx[phi_t2 + 9].v.tc[0] = this->windowContentVtx[phi_t2 + 11].v.tc[0] = 0x580;
        this->windowContentVtx[phi_t2 + 12].v.ob[0] = this->windowContentVtx[phi_t2 + 14].v.ob[0] = phi_t0 + 0x34;
        this->windowContentVtx[phi_t2 + 13].v.ob[0] = this->windowContentVtx[phi_t2 + 15].v.ob[0] =
            this->windowContentVtx[phi_t2 + 12].v.ob[0] + 0x18;

        this->windowContentVtx[phi_t2 + 12].v.ob[1] = this->windowContentVtx[phi_t2 + 13].v.ob[1] =
            this->buttonYOffsets[phi_t5] + phi_ra;

        this->windowContentVtx[phi_t2 + 14].v.ob[1] = this->windowContentVtx[phi_t2 + 15].v.ob[1] =
            this->windowContentVtx[phi_t2 + 12].v.ob[1] - 0x10;

        this->windowContentVtx[phi_t2 + 13].v.tc[0] = this->windowContentVtx[phi_t2 + 15].v.tc[0] = 0x300;
        this->windowContentVtx[phi_t2 + 16].v.ob[0] = this->windowContentVtx[phi_t2 + 18].v.ob[0] = phi_t0 + 0x9C;
        this->windowContentVtx[phi_t2 + 17].v.ob[0] = this->windowContentVtx[phi_t2 + 19].v.ob[0] =
            this->windowContentVtx[phi_t2 + 16].v.ob[0] + 0x18;

        this->windowContentVtx[phi_t2 + 16].v.ob[1] = this->windowContentVtx[phi_t2 + 17].v.ob[1] =
            this->buttonYOffsets[phi_t5] + phi_ra;

        this->windowContentVtx[phi_t2 + 18].v.ob[1] = this->windowContentVtx[phi_t2 + 19].v.ob[1] =
            this->windowContentVtx[phi_t2 + 16].v.ob[1] - 0x10;

        this->windowContentVtx[phi_t2 + 17].v.tc[0] = this->windowContentVtx[phi_t2 + 19].v.tc[0] = 0x300;
    }

    phi_ra = 0x2C;

    for (phi_t5 = 0; phi_t5 < 3; phi_t5++, phi_ra -= WREG(38)) {
        if (Save_GetSaveMetaInfo(phi_t5)->valid) {
            phi_t0 = this->windowPosX - WREG(39);

            if ((this->configMode == 0xF) && (phi_t5 == this->copyDestFileIndex)) {
                temp_t1 = this->fileNamesY[phi_t5] + 0x2C;
            } else if (((this->configMode == CM_COPY_ANIM_3) || (this->configMode == CM_COPY_ANIM_4)) &&
                       (phi_t5 == this->copyDestFileIndex)) {
                temp_t1 = this->buttonYOffsets[phi_t5] + phi_ra;
            } else {
                temp_t1 = phi_ra + this->buttonYOffsets[phi_t5] + this->fileNamesY[phi_t5];
            }

            temp_t1 += 2;

            for (phi_a1 = 0; phi_a1 < 8; phi_a1++, phi_t2 += 4, phi_t0 += WREG(40)) {
                this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] =
                    WREG(41) + phi_t0 + 0x40;
                this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
                    this->windowContentVtx[phi_t2].v.ob[0] + WREG(42);
                this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] = temp_t1 - 3;
                this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
                    this->windowContentVtx[phi_t2].v.ob[1] - WREG(43);
            }

            phi_t0 = this->windowPosX - 14;
            temp_t1 -= 0x16;

            for (phi_a1 = 0; phi_a1 < 4; phi_a1++, phi_t2 += 4) {
                this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
                this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
                    this->windowContentVtx[phi_t2].v.ob[0] + D_80812820[phi_a1];
                this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] = temp_t1;
                this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
                    this->windowContentVtx[phi_t2].v.ob[1] - D_80812828[phi_a1];
                phi_t0 += D_80812818[phi_a1];
            }

            this->windowContentVtx[phi_t2 - 15].v.tc[0] = this->windowContentVtx[phi_t2 - 13].v.tc[0] = 0x400;

            phi_t0 = this->windowPosX + 63;
            temp_t1 += 4;

            for (phi_a1 = 0; phi_a1 < 20; phi_a1++, phi_t2 += 4, phi_t0 += 9) {
                this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
                this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
                    this->windowContentVtx[phi_t2].v.ob[0] + 0xA;
                this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] = temp_t1;
                this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
                    this->windowContentVtx[phi_t2].v.ob[1] - 0xA;

                if (phi_a1 == 9) {
                    phi_t0 = this->windowPosX + 54;
                    temp_t1 -= 8;
                }
            }

            phi_t0 = this->windowPosX + 4;
            temp_t1 -= 0xA;

            for (phi_a1 = 0; phi_a1 < 10; phi_a1++, phi_t2 += 4, phi_t0 += 0x10) {
                this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
                this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
                    this->windowContentVtx[phi_t2].v.ob[0] + 0x10;
                this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] = temp_t1;
                this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
                    this->windowContentVtx[phi_t2].v.ob[1] - 0x10;
            }
        } else {
            phi_t2 += 0xA8;
        }
    }

    phi_t0 = this->windowPosX - 6;
    phi_ra = -0xC;

    for (phi_t5 = 0; phi_t5 < 2; phi_t5++, phi_t2 += 4, phi_ra -= 0x10) {
        this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
        this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
            this->windowContentVtx[phi_t2].v.ob[0] + 0x40;
        this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] =
            this->buttonYOffsets[phi_t5 + 3] + phi_ra;
        this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
            this->windowContentVtx[phi_t2].v.ob[1] - 0x10;
        this->windowContentVtx[phi_t2 + 1].v.tc[0] = this->windowContentVtx[phi_t2 + 3].v.tc[0] = 0x800;
    }

    this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = phi_t0;
    this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
        this->windowContentVtx[phi_t2].v.ob[0] + 0x40;
    this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] =
        this->buttonYOffsets[5] - 0x34;
    this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
        this->windowContentVtx[phi_t2].v.ob[1] - 0x10;
    this->windowContentVtx[phi_t2 + 1].v.tc[0] = this->windowContentVtx[phi_t2 + 3].v.tc[0] = 0x800;

    phi_t2 += 4;

    if (((this->menuMode == FS_MENU_MODE_CONFIG) && (this->configMode >= CM_MAIN_MENU)) ||
        ((this->menuMode == FS_MENU_MODE_SELECT) && (this->selectMode == SM_CONFIRM_FILE))) {
        if (this->menuMode == FS_MENU_MODE_CONFIG) {
            if ((this->configMode == CM_SELECT_COPY_SOURCE) || (this->configMode == CM_SELECT_COPY_DEST) ||
                (this->configMode == CM_ERASE_SELECT)) {
                phi_t5 = D_8081283C[this->buttonIndex];
            } else if ((this->configMode == CM_ERASE_CONFIRM) || (this->configMode == CM_COPY_CONFIRM)) {
                phi_t5 = D_80812844[this->buttonIndex];
            } else {
                phi_t5 = D_80812830[this->buttonIndex];
            }
        } else {
            phi_t5 = D_80812848[this->confirmButtonIndex];
        }

        this->windowContentVtx[phi_t2].v.ob[0] = this->windowContentVtx[phi_t2 + 2].v.ob[0] = this->windowPosX - 0xA;
        this->windowContentVtx[phi_t2 + 1].v.ob[0] = this->windowContentVtx[phi_t2 + 3].v.ob[0] =
            this->windowContentVtx[phi_t2].v.ob[0] + 0x48;
        this->windowContentVtx[phi_t2].v.ob[1] = this->windowContentVtx[phi_t2 + 1].v.ob[1] =
            this->windowContentVtx[phi_t5].v.ob[1] + 4;
        this->windowContentVtx[phi_t2 + 2].v.ob[1] = this->windowContentVtx[phi_t2 + 3].v.ob[1] =
            this->windowContentVtx[phi_t2].v.ob[1] - 0x18;
        this->windowContentVtx[phi_t2 + 1].v.tc[0] = this->windowContentVtx[phi_t2 + 3].v.tc[0] = 0x900;
        this->windowContentVtx[phi_t2 + 2].v.tc[1] = this->windowContentVtx[phi_t2 + 3].v.tc[1] = 0x300;
    }

    this->windowContentVtx[phi_t2 + 4].v.ob[0] = this->windowContentVtx[phi_t2 + 6].v.ob[0] = this->windowPosX + 0x3A;
    this->windowContentVtx[phi_t2 + 5].v.ob[0] = this->windowContentVtx[phi_t2 + 7].v.ob[0] =
        this->windowContentVtx[phi_t2 + 4].v.ob[0] + 0x80;
    this->windowContentVtx[phi_t2 + 4].v.ob[1] = this->windowContentVtx[phi_t2 + 5].v.ob[1] =
        this->windowContentVtx[D_80812830[this->warningButtonIndex]].v.ob[1];
    this->windowContentVtx[phi_t2 + 6].v.ob[1] = this->windowContentVtx[phi_t2 + 7].v.ob[1] =
        this->windowContentVtx[phi_t2 + 4].v.ob[1] - 0x10;
    this->windowContentVtx[phi_t2 + 5].v.tc[0] = this->windowContentVtx[phi_t2 + 7].v.tc[0] = 0x1000;
}

static u16 D_8081284C[] = { 0x007C, 0x0124, 0x01CC };

static void* sQuestItemTextures[] = {
    gFileSelKokiriEmeraldTex,   gFileSelGoronRubyTex,       gFileSelZoraSapphireTex,
    gFileSelForestMedallionTex, gFileSelFireMedallionTex,   gFileSelWaterMedallionTex,
    gFileSelSpiritMedallionTex, gFileSelShadowMedallionTex, gFileSelLightMedallionTex,
};

static s16 sQuestItemRed[] = { 255, 255, 255, 0, 255, 0, 255, 200, 200 };
static s16 sQuestItemGreen[] = { 255, 255, 255, 255, 60, 100, 130, 50, 200 };
static s16 sQuestItemBlue[] = { 255, 255, 255, 0, 0, 255, 0, 255, 0 };
static s16 sQuestItemFlags[] = { 0x0012, 0x0013, 0x0014, 0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005 };
static s16 sNamePrimColors[2][3] = { { 255, 255, 255 }, { 100, 100, 100 } };
static void* sHeartTextures[] = { gHeartFullTex, gDefenseHeartFullTex };

void FileChoose_DrawFileInfo(GameState* thisx, s16 fileIndex, s16 isActive) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    Font* sp54 = &this->font;
    s32 heartType;
    s16 i;
    s16 vtxOffset;
    s16 j;
    s16 deathCountSplit[3];

    Color_RGB8 heartColor = {HEARTS_PRIM_R, HEARTS_PRIM_G, HEARTS_PRIM_B};
    if (CVarGetInteger("gCosmetics.Consumable_Hearts.Changed", 0)) {
        heartColor = CVarGetColor24("gCosmetics.Consumable_Hearts.Value", heartColor);
    }
    Color_RGB8 heartBorder = {HEARTS_ENV_R, HEARTS_ENV_G, HEARTS_ENV_B};
    if (CVarGetInteger("gCosmetics.Consumable_HeartBorder.Changed", 0)) {
        heartBorder = CVarGetColor24("gCosmetics.Consumable_HeartBorder.Value", heartBorder);
    }
    Color_RGB8 ddColor = {HEARTS_DD_ENV_R, HEARTS_DD_ENV_G, HEARTS_DD_ENV_B};
    if (CVarGetInteger("gCosmetics.Consumable_DDHearts.Changed", 0)) {
        ddColor = CVarGetColor24("gCosmetics.Consumable_DDHearts.Value", ddColor);
    }
    Color_RGB8 ddBorder = {HEARTS_DD_PRIM_R, HEARTS_DD_PRIM_G, HEARTS_DD_PRIM_B};
    if (CVarGetInteger("gCosmetics.Consumable_DDHeartBorder.Changed", 0)) {
        ddBorder = CVarGetColor24("gCosmetics.Consumable_DDHeartBorder.Value", ddBorder);
    }

    OPEN_DISPS(this->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);

    // draw file name
    if (this->nameAlpha[fileIndex] != 0) {
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_8081284C[fileIndex]], 32, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, sNamePrimColors[isActive][0], sNamePrimColors[isActive][1],
                        sNamePrimColors[isActive][2], this->nameAlpha[fileIndex]);

        for (i = 0, vtxOffset = 0; vtxOffset < 0x20; i++, vtxOffset += 4) {
            FileChoose_DrawCharacter(
                this->state.gfxCtx, sp54->fontBuf + Save_GetSaveMetaInfo(fileIndex)->playerName[i] * FONT_CHAR_TEX_SIZE,
                vtxOffset);
        }
    }

    if ((fileIndex == this->selectedFileIndex) || (fileIndex == this->copyDestFileIndex)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                          PRIMITIVE, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, 255, 255, 255, this->fileInfoAlpha[fileIndex]);
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_8081284C[fileIndex]] + 0x24, 12, 0);

        FileChoose_SplitNumber(Save_GetSaveMetaInfo(fileIndex)->deaths, &deathCountSplit[0], &deathCountSplit[1],
                               &deathCountSplit[2]);

        // draw death count
        if (CVarGetInteger("gFileSelectMoreInfo", 0) == 0 || this->menuMode != FS_MENU_MODE_SELECT) {
            for (i = 0, vtxOffset = 0; i < 3; i++, vtxOffset += 4) {
                FileChoose_DrawCharacter(this->state.gfxCtx, sp54->fontBuf + deathCountSplit[i] * FONT_CHAR_TEX_SIZE,
                                         vtxOffset);
            }
        }

        gDPPipeSync(POLY_OPA_DISP++);

        heartType = (Save_GetSaveMetaInfo(fileIndex)->defense == 0) ? 0 : 1;

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        if (heartType) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, ddColor.r, ddColor.g, ddColor.b, this->fileInfoAlpha[fileIndex]);
            gDPSetEnvColor(POLY_OPA_DISP++, ddBorder.r, ddBorder.g, ddBorder.b, 255);
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, heartColor.r, heartColor.g, heartColor.b, this->fileInfoAlpha[fileIndex]);
            gDPSetEnvColor(POLY_OPA_DISP++, heartBorder.r, heartBorder.g, heartBorder.b, 255);
        }

        i = Save_GetSaveMetaInfo(fileIndex)->healthCapacity / 0x10;

        if (CVarGetInteger("gFileSelectMoreInfo", 0) == 0 || this->menuMode != FS_MENU_MODE_SELECT) {
            // draw hearts
            for (vtxOffset = 0, j = 0; j < i; j++, vtxOffset += 4) {
                gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_8081284C[fileIndex] + vtxOffset] + 0x30, 4, 0);

                POLY_OPA_DISP = FileChoose_QuadTextureIA8(POLY_OPA_DISP, sHeartTextures[heartType], 0x10, 0x10, 0);
            }
        }

        gDPPipeSync(POLY_OPA_DISP++);

        // Use file info alpha to match fading
        u8 textAlpha = this->fileInfoAlpha[fileIndex];
        if (textAlpha >= 200) {
            textAlpha = 255;
        }

        if (CVarGetInteger("gFileSelectMoreInfo", 0) != 0 && this->menuMode == FS_MENU_MODE_SELECT) {
            DrawMoreInfo(this, fileIndex, textAlpha);
        } else {
            // draw quest items
            for (vtxOffset = 0, j = 0; j < 9; j++, vtxOffset += 4) {
                if (Save_GetSaveMetaInfo(fileIndex)->questItems & gBitFlags[sQuestItemFlags[j]]) {
                    gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[D_8081284C[fileIndex] + vtxOffset] + 0x80, 4, 0);
                    gDPPipeSync(POLY_OPA_DISP++);
                    gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x00, sQuestItemRed[j], sQuestItemGreen[j], sQuestItemBlue[j],
                                    this->fileInfoAlpha[fileIndex]);
                    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

                    if (j < 3) {
                        gDPLoadTextureBlock(POLY_OPA_DISP++, sQuestItemTextures[j], G_IM_FMT_RGBA, G_IM_SIZ_32b, 16, 16, 0,
                                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP,
                                            G_TX_NOMASK, G_TX_NOLOD);
                        gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

                    } else {
                        POLY_OPA_DISP = FileChoose_QuadTextureIA8(POLY_OPA_DISP, sQuestItemTextures[j], 0x10, 0x10, 0);
                    }
                }
            }
        }

        // Draw rando seed warning when build version doesn't match for Major or Minor number
        if (Save_GetSaveMetaInfo(fileIndex)->randoSave == 1 &&
            this->menuMode == FS_MENU_MODE_SELECT &&
            (gBuildVersionMajor != Save_GetSaveMetaInfo(fileIndex)->buildVersionMajor ||
            gBuildVersionMinor != Save_GetSaveMetaInfo(fileIndex)->buildVersionMinor)) {

            // Stub out a dummy play state to be able to use the dialog system (MessageCtx)
            PlayState dummyPlay;
            PlayState* dummyPlayPtr = &dummyPlay;

            // Set the MessageCtx and GameState onto the dummy play state
            dummyPlayPtr->msgCtx = this->msgCtx;
            dummyPlayPtr->state = this->state;

            // Load the custom text ID without doing a textbox
            Message_OpenText(dummyPlayPtr, TEXT_RANDO_SAVE_VERSION_WARNING);
            // Force the context into message print mode
            dummyPlayPtr->msgCtx.msgMode = MSGMODE_TEXT_NEXT_MSG;
            Message_Decode(dummyPlayPtr);

            // Set the draw pos to end of text to render it all at once
            dummyPlayPtr->msgCtx.textDrawPos = dummyPlayPtr->msgCtx.decodedTextLen;
            dummyPlayPtr->msgCtx.textColorAlpha = textAlpha;

            // Set position and spacing values
            R_TEXT_LINE_SPACING = 10;
            R_TEXT_INIT_XPOS = 128;
            R_TEXT_INIT_YPOS = 154;

            Gfx* gfx = Graph_GfxPlusOne(POLY_OPA_DISP);
            Message_DrawText(dummyPlayPtr, &gfx);

            POLY_OPA_DISP = gfx;
        }
    }

    CLOSE_DISPS(this->state.gfxCtx);
}

static void* sFileInfoBoxTextures[] = {
    gFileSelFileInfoBox1Tex, gFileSelFileInfoBox2Tex, gFileSelFileInfoBox3Tex,
    gFileSelFileInfoBox4Tex, gFileSelFileInfoBox5Tex,
};

static void* sTitleLabels[3][9] = {
    { gFileSelPleaseSelectAFileENGTex, gFileSelOpenThisFileENGTex, gFileSelCopyWhichFileENGTex,
      gFileSelCopyToWhichFileENGTex, gFileSelAreYouSureENGTex, gFileSelFileCopiedENGTex, gFileSelEraseWhichFileENGTex,
      gFileSelAreYouSure2ENGTex, gFileSelFileErasedENGTex },
    { gFileSelPleaseSelectAFileGERTex, gFileSelOpenThisFileGERTex, gFileSelWhichFile1GERTex,
      gFileSelCopyToWhichFileGERTex, gFileSelAreYouSureGERTex, gFileSelFileCopiedGERTex, gFileSelWhichFile2GERTex,
      gFileSelAreYouSure2GERTex, gFileSelFileErasedGERTex },
    { gFileSelPleaseSelectAFileFRATex, gFileSelOpenThisFileFRATex, gFileSelCopyWhichFileFRATex,
      gFileSelCopyToWhichFileFRATex, gFileSelAreYouSureFRATex, gFileSelFileCopiedFRATex, gFileSelEraseWhichFileFRATex,
      gFileSelAreYouSure2FRATex, gFileSelFileErasedFRATex }
};

static void* sWarningLabels[3][5] = {
    { gFileSelNoFileToCopyENGTex, gFileSelNoFileToEraseENGTex, gFileSelNoEmptyFileENGTex, gFileSelFileEmptyENGTex,
      gFileSelFileInUseENGTex },
    { gFileSelNoFileToCopyGERTex, gFileSelNoFileToEraseGERTex, gFileSelNoEmptyFileGERTex, gFileSelFileEmptyGERTex,
      gFileSelFileInUseGERTex },
    { gFileSelNoFileToCopyFRATex, gFileSelNoFileToEraseFRATex, gFileSelNoEmptyFileFRATex, gFileSelFileEmptyFRATex,
      gFileSelFileInUseFRATex },
};

static void* sFileButtonTextures[3][3] = {
    { gFileSelFile1ButtonENGTex, gFileSelFile2ButtonENGTex, gFileSelFile3ButtonENGTex },
    { gFileSelFile1ButtonGERTex, gFileSelFile2ButtonGERTex, gFileSelFile3ButtonGERTex },
    { gFileSelFile1ButtonFRATex, gFileSelFile2ButtonFRATex, gFileSelFile3ButtonFRATex },
};

static void* sActionButtonTextures[3][4] = {
    { gFileSelCopyButtonENGTex, gFileSelEraseButtonENGTex, gFileSelYesButtonENGTex, gFileSelQuitButtonENGTex },
    { gFileSelCopyButtonGERTex, gFileSelEraseButtonGERTex, gFileSelYesButtonGERTex, gFileSelQuitButtonGERTex },
    { gFileSelCopyButtonFRATex, gFileSelEraseButtonFRATex, gFileSelYesButtonFRATex, gFileSelQuitButtonFRATex },
};

static void* sOptionsButtonTextures[] = {
    gFileSelOptionsButtonENGTex,
    gFileSelOptionsButtonGERTex,
    gFileSelOptionsButtonENGTex,
};

const char* FileChoose_GetQuestChooseTitleTexName(Language lang) {
    switch (lang) {
        case LANGUAGE_ENG:
        default:
            return gFileSelPleaseChooseAQuestENGTex;
        case LANGUAGE_FRA:
            return gFileSelPleaseChooseAQuestFRATex;
        case LANGUAGE_GER:
            return gFileSelPleaseChooseAQuestGERTex;
    }
}

const char* FileChoose_GetBossRushOptionsTitleTexName(Language lang) {
    switch (lang) {
        case LANGUAGE_ENG:
        default:
            return gFileSelBossRushSettingsENGText;
        case LANGUAGE_FRA:
            return gFileSelBossRushSettingsFRAText;
        case LANGUAGE_GER:
            return gFileSelBossRushSettingsGERText;
    }
}

/**
 * Draw most window contents including buttons, labels, and icons.
 * Does not include anything from the keyboard and settings windows.
 */
void FileChoose_DrawWindowContents(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 fileIndex;
    s16 temp;
    s16 i;
    s16 quadVtxIndex;
    s16 isActive;
    s16 pad;
    char* tex;

    switch (this->configMode) { 
        case CM_QUEST_MENU:
        case CM_ROTATE_TO_NAME_ENTRY:
        case CM_START_QUEST_MENU:
        case CM_QUEST_TO_MAIN:
        case CM_NAME_ENTRY_TO_QUEST_MENU:
        case CM_ROTATE_TO_BOSS_RUSH_MENU:
            tex = FileChoose_GetQuestChooseTitleTexName(gSaveContext.language);
            break;
        case CM_BOSS_RUSH_MENU:
        case CM_START_BOSS_RUSH_MENU:
        case CM_BOSS_RUSH_TO_QUEST:
            tex = FileChoose_GetBossRushOptionsTitleTexName(gSaveContext.language);
            break;
        default:
            tex = sTitleLabels[gSaveContext.language][this->titleLabel];
            break;
    }

    OPEN_DISPS(this->state.gfxCtx);

    // draw title label
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    gSPVertex(POLY_OPA_DISP++, this->windowContentVtx, 4, 0);
    gDPLoadTextureBlock(POLY_OPA_DISP++, tex, G_IM_FMT_IA,
                        G_IM_SIZ_8b, 128, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                        G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    // draw next title label
    if ((this->configMode == CM_QUEST_MENU) || (this->configMode == CM_START_QUEST_MENU) || 
        this->configMode == CM_NAME_ENTRY_TO_QUEST_MENU) {
        // draw control stick prompts.
        Gfx_SetupDL_39Opa(this->state.gfxCtx);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPLoadTextureBlock(POLY_OPA_DISP++, gArrowCursorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        FileChoose_DrawTextRec(this->state.gfxCtx, this->stickLeftPrompt.arrowColorR,
                                this->stickLeftPrompt.arrowColorG, this->stickLeftPrompt.arrowColorB,
                                this->stickLeftPrompt.arrowColorA, this->stickLeftPrompt.arrowTexX,
                                this->stickLeftPrompt.arrowTexY, this->stickLeftPrompt.z, 0, 0, -1.0f, 1.0f);
        FileChoose_DrawTextRec(this->state.gfxCtx, this->stickRightPrompt.arrowColorR,
                                this->stickRightPrompt.arrowColorG, this->stickRightPrompt.arrowColorB,
                                this->stickRightPrompt.arrowColorA, this->stickRightPrompt.arrowTexX,
                                this->stickRightPrompt.arrowTexY, this->stickRightPrompt.z, 0, 0, 1.0f, 1.0f);
        gDPLoadTextureBlock(POLY_OPA_DISP++, gControlStickTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        FileChoose_DrawTextRec(this->state.gfxCtx, this->stickLeftPrompt.stickColorR,
                                this->stickLeftPrompt.stickColorG, this->stickLeftPrompt.stickColorB,
                                this->stickLeftPrompt.stickColorA, this->stickLeftPrompt.stickTexX,
                                this->stickLeftPrompt.stickTexY, this->stickLeftPrompt.z, 0, 0, -1.0f, 1.0f);
        FileChoose_DrawTextRec(this->state.gfxCtx, this->stickRightPrompt.stickColorR,
                                this->stickRightPrompt.stickColorG, this->stickRightPrompt.stickColorB,
                                this->stickRightPrompt.stickColorA, this->stickRightPrompt.stickTexX,
                                this->stickRightPrompt.stickTexY, this->stickRightPrompt.z, 0, 0, 1.0f, 1.0f);
        switch (this->questType[this->buttonIndex]) {
            case FS_QUEST_NORMAL:
            default:
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->logoAlpha);
                FileChoose_DrawTextureI8(this->state.gfxCtx, gTitleTheLegendOfTextTex, 72, 8, 156, 108, 72, 8, 1024, 1024);
                FileChoose_DrawTextureI8(this->state.gfxCtx, gTitleOcarinaOfTimeTMTextTex, 96, 8, 154, 163, 96, 8, 1024, 1024);
                FileChoose_DrawImageRGBA32(this->state.gfxCtx, 160, 135, gTitleZeldaShieldLogoTex, 160, 160);
                break;

            case FS_QUEST_MASTER:
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->logoAlpha);
                FileChoose_DrawTextureI8(this->state.gfxCtx, gTitleTheLegendOfTextTex, 72, 8, 156, 108, 72, 8, 1024, 1024);
                FileChoose_DrawTextureI8(this->state.gfxCtx, gTitleOcarinaOfTimeTMTextTex, 96, 8, 154, 163, 96, 8, 1024, 1024);
                FileChoose_DrawImageRGBA32(this->state.gfxCtx, 160, 135, gTitleZeldaShieldLogoMQTex, 160, 160);
                FileChoose_DrawImageRGBA32(this->state.gfxCtx, 182, 180, gTitleMasterQuestSubtitleTex, 128, 32);
                break;
            
            case FS_QUEST_RANDOMIZER:
                DrawSeedHashSprites(this);
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->logoAlpha);
                FileChoose_DrawTextureI8(this->state.gfxCtx, gTitleTheLegendOfTextTex, 72, 8, 156, 108, 72, 8, 1024, 1024);
                FileChoose_DrawTextureI8(this->state.gfxCtx, gTitleOcarinaOfTimeTMTextTex, 96, 8, 154, 163, 96, 8, 1024, 1024);
                FileChoose_DrawImageRGBA32(this->state.gfxCtx, 160, 135, ResourceMgr_GameHasOriginal() ? gTitleZeldaShieldLogoTex : gTitleZeldaShieldLogoMQTex, 160, 160);
                FileChoose_DrawImageRGBA32(this->state.gfxCtx, 182, 180, gTitleRandomizerSubtitleTex, 128, 32);
                break;

            case FS_QUEST_BOSSRUSH:
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->logoAlpha);
                FileChoose_DrawTextureI8(this->state.gfxCtx, gTitleTheLegendOfTextTex, 72, 8, 156, 108, 72, 8, 1024, 1024);
                FileChoose_DrawTextureI8(this->state.gfxCtx, gTitleOcarinaOfTimeTMTextTex, 96, 8, 154, 163, 96, 8, 1024, 1024);
                FileChoose_DrawImageRGBA32(this->state.gfxCtx, 160, 135, ResourceMgr_GameHasOriginal() ? gTitleZeldaShieldLogoTex : gTitleZeldaShieldLogoMQTex, 160, 160);
                FileChoose_DrawImageRGBA32(this->state.gfxCtx, 182, 180, gTitleBossRushSubtitleTex, 128, 32);
                break;
        }
    } else if (this->configMode == CM_BOSS_RUSH_MENU) {

        uint8_t listOffset = this->bossRushOffset;
        uint8_t textAlpha = this->bossRushUIAlpha;

        // Draw arrows to indicate that the list can scroll up or down.
        // Arrow up
        if (listOffset > 0) {
            uint16_t arrowUpX = 140;
            uint16_t arrowUpY = 76 - (this->bossRushArrowOffset / 10);
            gDPLoadTextureBlock(POLY_OPA_DISP++, gArrowUpTex, G_IM_FMT_IA,
                                G_IM_SIZ_16b, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSPWideTextureRectangle(POLY_OPA_DISP++, arrowUpX << 2, arrowUpY << 2, (arrowUpX + 8) << 2,
                                    (arrowUpY + 8) << 2, G_TX_RENDERTILE, 0, 0, (1 << 11), (1 << 11));
        }
        // Arrow down
        if (BOSSRUSH_OPTIONS_AMOUNT - listOffset > BOSSRUSH_MAX_OPTIONS_ON_SCREEN) {
            uint16_t arrowDownX = 140;
            uint16_t arrowDownY = 181 + (this->bossRushArrowOffset / 10);
            gDPLoadTextureBlock(POLY_OPA_DISP++, gArrowDownTex, G_IM_FMT_IA,
                                G_IM_SIZ_16b, 16, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSPWideTextureRectangle(POLY_OPA_DISP++, arrowDownX << 2, arrowDownY << 2, (arrowDownX + 8) << 2,
                                    (arrowDownY + 8) << 2, G_TX_RENDERTILE, 0, 0, (1 << 11), (1 << 11));
        }

        // Draw options. There's more options than what fits on the screen, so the visible options
        // depend on the current offset of the list. Currently selected option pulses in
        // color and has arrows surrounding the option.
        for (uint8_t i = listOffset; i - listOffset < BOSSRUSH_MAX_OPTIONS_ON_SCREEN; i++) {
            uint16_t textYOffset = (i - listOffset) * 16;

            // Option name.
            Interface_DrawTextLine(this->state.gfxCtx, BossRush_GetSettingName(i, gSaveContext.language), 
                65, (87 + textYOffset), 255, 255, 80, textAlpha, 0.8f, true);

            // Selected choice for option.
            uint16_t finalKerning = Interface_DrawTextLine(this->state.gfxCtx, BossRush_GetSettingChoiceName(i, gSaveContext.bossRushOptions[i], gSaveContext.language), 
                165, (87 + textYOffset), 255, 255, 255, textAlpha, 0.8f, true);

            // Draw arrows around selected option.
            if (this->bossRushIndex == i) {
                Gfx_SetupDL_39Opa(this->state.gfxCtx);
                gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                gDPLoadTextureBlock(POLY_OPA_DISP++, gArrowCursorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                                    G_TX_NOLOD);
                FileChoose_DrawTextRec(this->state.gfxCtx, this->stickLeftPrompt.arrowColorR,
                                       this->stickLeftPrompt.arrowColorG, this->stickLeftPrompt.arrowColorB,
                                       textAlpha, 160, (92 + textYOffset), 0.42f, 0, 0, -1.0f,
                                       1.0f);
                FileChoose_DrawTextRec(this->state.gfxCtx, this->stickRightPrompt.arrowColorR,
                                       this->stickRightPrompt.arrowColorG, this->stickRightPrompt.arrowColorB,
                                       textAlpha, (171 + finalKerning),
                                       (92 + textYOffset), 0.42f, 0, 0, 1.0f, 1.0f);
            }
        }

    } else if (this->configMode != CM_ROTATE_TO_NAME_ENTRY && this->configMode != CM_START_BOSS_RUSH_MENU &&
               this->configMode != CM_ROTATE_TO_BOSS_RUSH_MENU && this->configMode != CM_BOSS_RUSH_TO_QUEST) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[1]);
        gDPLoadTextureBlock(POLY_OPA_DISP++, sTitleLabels[gSaveContext.language][this->nextTitleLabel], G_IM_FMT_IA,
                            G_IM_SIZ_8b, 128, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

        temp = 4;

        gDPPipeSync(POLY_OPA_DISP++);

        // draw file info box (large box when a file is selected)
        for (fileIndex = 0; fileIndex < 3; fileIndex++, temp += 20) {
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            this->fileInfoAlpha[fileIndex]);

            // Draw the small file name box instead when more meta info is enabled
            if (CVarGetInteger("gFileSelectMoreInfo", 0) != 0 && this->menuMode == FS_MENU_MODE_SELECT) {
                // Location of file 1 small name box vertices
                gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[68], 4, 0);

                gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelNameBoxTex, G_IM_FMT_IA, G_IM_SIZ_16b, 108, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
                gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
            } else {
                gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[temp], 20, 0);

                for (quadVtxIndex = 0, i = 0; i < 5; i++, quadVtxIndex += 4) {
                    gDPLoadTextureBlock(POLY_OPA_DISP++, sFileInfoBoxTextures[i], G_IM_FMT_IA, G_IM_SIZ_16b,
                                        sFileInfoBoxPartWidths[i], 56, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                    gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1, 0);
                }
            }
        }

        for (i = 0; i < 3; i++, temp += 20) {
            // draw file button
            gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[temp], 20, 0);

            isActive = 0;

            if (!FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(i)) && Save_GetSaveMetaInfo(i)->valid) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[1][0], sWindowContentColors[1][1],
                                sWindowContentColors[1][2], this->fileButtonAlpha[i]);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[isActive][0],
                                sWindowContentColors[isActive][1], sWindowContentColors[isActive][2],
                                this->fileButtonAlpha[i]);
            }

            gDPLoadTextureBlock(POLY_OPA_DISP++, sFileButtonTextures[gSaveContext.language][i], G_IM_FMT_IA,
                                G_IM_SIZ_16b, 64, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

            // draw file name box
            if (!FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(i))) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[1][0], sWindowContentColors[1][1],
                                sWindowContentColors[1][2], this->nameBoxAlpha[i]);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[isActive][0],
                                sWindowContentColors[isActive][1], sWindowContentColors[isActive][2],
                                this->nameBoxAlpha[i]);
            }

            gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelNameBoxTex, G_IM_FMT_IA, G_IM_SIZ_16b, 108, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);

            // draw rando label
            if (Save_GetSaveMetaInfo(i)->randoSave) {
                if (!FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(i))) {
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[1][0], sWindowContentColors[1][1],
                                    sWindowContentColors[1][2], this->nameBoxAlpha[i]);
                } else {
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[isActive][0],
                                    sWindowContentColors[isActive][1], sWindowContentColors[isActive][2],
                                    this->nameAlpha[i]);
                }
                gDPLoadTextureBlock(POLY_OPA_DISP++,
                                    gFileSelRANDButtonTex,
                                    G_IM_FMT_IA, G_IM_SIZ_16b, 44, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                gSP1Quadrangle(POLY_OPA_DISP++, 8, 10, 11, 9, 0);
            }
            // Draw MQ label
            if (Save_GetSaveMetaInfo(i)->requiresMasterQuest && !Save_GetSaveMetaInfo(i)->randoSave &&
                Save_GetSaveMetaInfo(i)->valid) {
                if (!FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(i))) {
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[1][0], sWindowContentColors[1][1],
                                    sWindowContentColors[1][2], this->nameBoxAlpha[i]);
                } else {
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[isActive][0],
                                    sWindowContentColors[isActive][1], sWindowContentColors[isActive][2],
                                    this->nameAlpha[i]);
                }
                gDPLoadTextureBlock(POLY_OPA_DISP++,
                                    gFileSelMQButtonTex,
                                    G_IM_FMT_IA, G_IM_SIZ_16b, 44, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
                gSP1Quadrangle(POLY_OPA_DISP++, 8, 10, 11, 9, 0);
            }

            // draw connectors
            if (!FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(i)) && Save_GetSaveMetaInfo(i)->valid) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[1][0], sWindowContentColors[1][1],
                                sWindowContentColors[1][2], this->fileButtonAlpha[i]);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sWindowContentColors[isActive][0],
                                sWindowContentColors[isActive][1], sWindowContentColors[isActive][2],
                                this->connectorAlpha[i]);
            }
            gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelConnectorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 12, 14, 15, 13, 0);

            if (Save_GetSaveMetaInfo(i)->randoSave || Save_GetSaveMetaInfo(i)->requiresMasterQuest) {
                gSP1Quadrangle(POLY_OPA_DISP++, 16, 18, 19, 17, 0);
            }
        }

        // draw file info
        for (fileIndex = 0; fileIndex < 3; fileIndex++) {
            isActive = 0;
            FileChoose_DrawFileInfo(&this->state, fileIndex, isActive);
        }

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
        gSPVertex(POLY_OPA_DISP++, &this->windowContentVtx[0x274], 20, 0);

        // draw primary action buttons (copy/erase)
        for (quadVtxIndex = 0, i = 0; i < 2; i++, quadVtxIndex += 4) {
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            this->actionButtonAlpha[i]);
            gDPLoadTextureBlock(POLY_OPA_DISP++, sActionButtonTextures[gSaveContext.language][i], G_IM_FMT_IA,
                                G_IM_SIZ_16b, 64, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1, 0);
        }

        gDPPipeSync(POLY_OPA_DISP++);

        // draw confirm buttons (yes/quit)
        for (quadVtxIndex = 0, i = 0; i < 2; i++, quadVtxIndex += 4) {
            temp = this->confirmButtonTexIndices[i];

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            this->confirmButtonAlpha[i]);
            gDPLoadTextureBlock(POLY_OPA_DISP++, sActionButtonTextures[gSaveContext.language][temp], G_IM_FMT_IA,
                                G_IM_SIZ_16b, 64, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, quadVtxIndex, quadVtxIndex + 2, quadVtxIndex + 3, quadVtxIndex + 1, 0);
        }

        // draw options button
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                        this->optionButtonAlpha);
        gDPLoadTextureBlock(POLY_OPA_DISP++, sOptionsButtonTextures[gSaveContext.language], G_IM_FMT_IA, G_IM_SIZ_16b,
                            64, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, 8, 10, 11, 9, 0);

        // draw highlight over currently selected button
        if (((this->menuMode == FS_MENU_MODE_CONFIG) &&
             ((this->configMode == CM_MAIN_MENU) || (this->configMode == CM_SELECT_COPY_SOURCE) ||
              (this->configMode == CM_SELECT_COPY_DEST) || (this->configMode == CM_COPY_CONFIRM) ||
              (this->configMode == CM_ERASE_SELECT) || (this->configMode == CM_ERASE_CONFIRM))) ||
            ((this->menuMode == FS_MENU_MODE_SELECT) && (this->selectMode == SM_CONFIRM_FILE))) {
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0,
                              0, PRIMITIVE, 0);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->highlightColor[0], this->highlightColor[1],
                            this->highlightColor[2], this->highlightColor[3]);
            gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelBigButtonHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 72, 24, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 12, 14, 15, 13, 0);
        }

        // draw warning labels
        if (this->warningLabel > FS_WARNING_NONE) {
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->emptyFileTextAlpha);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
            gDPLoadTextureBlock(POLY_OPA_DISP++, sWarningLabels[gSaveContext.language][this->warningLabel], G_IM_FMT_IA,
                                G_IM_SIZ_8b, 128, 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, 16, 18, 19, 17, 0);
        }

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);

        CLOSE_DISPS(this->state.gfxCtx);
    }
}

void FileChoose_ConfigModeDraw(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    f32 eyeX;
    f32 eyeY;
    f32 eyeZ;

    OPEN_DISPS(this->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);

    eyeX = 1000.0f * Math_CosS(ZREG(11)) - 1000.0f * Math_SinS(ZREG(11));
    eyeY = ZREG(13);
    eyeZ = 1000.0f * Math_SinS(ZREG(11)) + 1000.0f * Math_CosS(ZREG(11));

    FileChoose_SetView(this, eyeX, eyeY, eyeZ);
    SkyboxDraw_Draw(&this->skyboxCtx, this->state.gfxCtx, 1, this->envCtx.skyboxBlend, eyeX, eyeY, eyeZ);
    gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_NONE);
    ZREG(11) += ZREG(10);
    Environment_UpdateSkybox(NULL, SKYBOX_NORMAL_SKY, &this->envCtx, &this->skyboxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    Gfx_SetupDL_42Opa(this->state.gfxCtx);
    FileChoose_SetView(this, 0.0f, 0.0f, 64.0f);
    FileChoose_SetWindowVtx(&this->state);
    FileChoose_SetWindowContentVtx(&this->state);

    FrameInterpolation_RecordOpenChild(this, this->configMode);

    if ((this->configMode != CM_NAME_ENTRY) && (this->configMode != CM_START_NAME_ENTRY) &&
        (this->configMode != CM_QUEST_MENU) && this->configMode != CM_NAME_ENTRY_TO_QUEST_MENU) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                        this->windowAlpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);

        if (this->windowRot != 0) {
            if (this->configMode == CM_ROTATE_TO_QUEST_MENU || 
                (this->configMode >= CM_MAIN_TO_OPTIONS && this->configMode <= CM_OPTIONS_TO_MAIN) ||
                this->configMode == CM_QUEST_TO_MAIN) {
                Matrix_RotateX(this->windowRot / 100.0f, MTXMODE_APPLY);
            } else {
                Matrix_RotateX((this->windowRot - 942.0f) / 100.0f, MTXMODE_APPLY);
            }
        }

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileChoose_DrawWindowContents(&this->state);
    }

    // draw name entry menu
    if ((this->configMode >= CM_ROTATE_TO_NAME_ENTRY) && (this->configMode <= CM_NAME_ENTRY_TO_MAIN)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                        this->windowAlpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
        Matrix_RotateX((this->windowRot - 628.0f) / 100.0f, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileChoose_DrawNameEntry(&this->state);
    }

    // draw options menu
    if ((this->configMode >= CM_MAIN_TO_OPTIONS) && (this->configMode <= CM_OPTIONS_TO_MAIN)) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                        this->windowAlpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
        Matrix_RotateX((this->windowRot - 314.0f) / 100.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileChoose_DrawOptions(&this->state);
    }

    // draw quest menu
    if ((this->configMode == CM_QUEST_MENU) || (this->configMode == CM_ROTATE_TO_QUEST_MENU) || 
        (this->configMode == CM_ROTATE_TO_NAME_ENTRY) || this->configMode == CM_QUEST_TO_MAIN ||
        this->configMode == CM_NAME_ENTRY_TO_QUEST_MENU || this->configMode == CM_ROTATE_TO_BOSS_RUSH_MENU) {
        // window
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                        this->windowAlpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
        Matrix_RotateX((this->windowRot - 314.0f) / 100.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileChoose_DrawWindowContents(&this->state);
    }

    // Draw Boss Rush Options Menu
    if (this->configMode == CM_BOSS_RUSH_MENU || this->configMode == CM_ROTATE_TO_BOSS_RUSH_MENU ||
        this->configMode == CM_START_BOSS_RUSH_MENU || this->configMode == CM_BOSS_RUSH_TO_QUEST) {
        // window
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                        this->windowAlpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
        Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
        Matrix_RotateX((this->windowRot - 628.0f) / 100.0f, MTXMODE_APPLY);

        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

        gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
        gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

        gDPPipeSync(POLY_OPA_DISP++);

        FileChoose_DrawWindowContents(&this->state);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    FileChoose_SetView(this, 0.0f, 0.0f, 64.0f);

    DrawSeedHashSprites(this);

    FrameInterpolation_RecordCloseChild();

    CLOSE_DISPS(this->state.gfxCtx);
}

/**
 * Fade out the main menu elements to transition to select mode.
 * Update function for `SM_FADE_MAIN_TO_SELECT`
 */
void FileChoose_FadeMainToSelect(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 i;

    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] -= 25;
            this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
                this->optionButtonAlpha = this->fileButtonAlpha[i];

            if (Save_GetSaveMetaInfo(i)->valid) {
                this->nameAlpha[i] = this->nameBoxAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] -= 31;
            }
        }
    }

    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->actionTimer = 8;
        this->selectMode++;
        this->confirmButtonIndex = FS_BTN_CONFIRM_YES;
        sLastFileChooseButtonIndex = -1;
    }
}

/**
 * Moves the selected file to the top of the window.
 * Update function for `SM_MOVE_FILE_TO_TOP`
 */
void FileChoose_MoveSelectedFileToTop(GameState* thisx) {
    static s16 fileYOffsets[] = { 0, 16, 32 }; // amount to move by to reach the top of the screen
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 yStep;

    yStep = ABS(this->buttonYOffsets[this->buttonIndex] - fileYOffsets[this->buttonIndex]) / this->actionTimer;
    this->buttonYOffsets[this->buttonIndex] += yStep;
    this->actionTimer--;

    if ((this->actionTimer == 0) || (this->buttonYOffsets[this->buttonIndex] == fileYOffsets[this->buttonIndex])) {
        this->buttonYOffsets[FS_BTN_SELECT_YES] = this->buttonYOffsets[FS_BTN_SELECT_QUIT] = -24;
        this->actionTimer = 8;
        this->selectMode++;
    }
}

/**
 * Fade in the file info for the selected file.
 * Update function for `SM_FADE_IN_FILE_INFO`
 */
void FileChoose_FadeInFileInfo(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->fileInfoAlpha[this->buttonIndex] += 25;
    this->nameBoxAlpha[this->buttonIndex] -= 50;

    if (this->nameBoxAlpha[this->buttonIndex] <= 0) {
        this->nameBoxAlpha[this->buttonIndex] = 0;
    }

    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->fileInfoAlpha[this->buttonIndex] = 200;
        this->actionTimer = 8;
        this->selectMode++;
    }

    this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] = this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] =
        this->fileInfoAlpha[this->buttonIndex];
}

/**
 * Update the cursor and handle the option that the player picks for confirming the selected file.
 * Update function for `SM_CONFIRM_FILE`
 */
void FileChoose_ConfirmFile(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    Input* input = &this->state.input[0];
    bool dpad = CVarGetInteger("gDpadText", 0);

    if (CHECK_BTN_ALL(input->press.button, BTN_START) || (CHECK_BTN_ALL(input->press.button, BTN_A))) {
        if (this->confirmButtonIndex == FS_BTN_CONFIRM_YES) {
            func_800AA000(300.0f, 180, 20, 100);
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            // Reset Boss Rush because it's only ever saved in memory.
            gSaveContext.isBossRush = 0;
            this->selectMode = SM_FADE_OUT;
            func_800F6964(0xF);
        } else {
            Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            this->selectMode++;
        }
    } else if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CLOSE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        this->selectMode++;
    } else if ((ABS(this->stickRelY) >= 30) || (dpad && CHECK_BTN_ANY(input->press.button, BTN_DDOWN | BTN_DUP))) {
        Audio_PlaySoundGeneral(NA_SE_SY_FSEL_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        this->confirmButtonIndex ^= 1;
    }

    if (sLastFileChooseButtonIndex != this->confirmButtonIndex) {
        GameInteractor_ExecuteOnUpdateFileSelectConfirmationSelection(this->confirmButtonIndex);
        sLastFileChooseButtonIndex = this->confirmButtonIndex;
    }
}

/**
 * Fade out the file info for the selected file before returning to the main menu.
 * Update function for `SM_FADE_OUT_FILE_INFO`
 */
void FileChoose_FadeOutFileInfo(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    this->fileInfoAlpha[this->buttonIndex] -= 25;
    this->nameBoxAlpha[this->buttonIndex] += 25;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->buttonYOffsets[FS_BTN_SELECT_YES] = this->buttonYOffsets[FS_BTN_SELECT_QUIT] = 0;
        this->nameBoxAlpha[this->buttonIndex] = 200;
        this->fileInfoAlpha[this->buttonIndex] = 0;
        this->nextTitleLabel = FS_TITLE_SELECT_FILE;
        this->actionTimer = 8;
        this->selectMode++;
        sLastFileChooseButtonIndex = -1;
    }

    this->confirmButtonAlpha[0] = this->confirmButtonAlpha[1] = this->fileInfoAlpha[this->buttonIndex];
}

/**
 * Move the selected file back to the slot position then go to config mode for the main menu.
 * Update function for `SM_MOVE_FILE_TO_SLOT`
 */
void FileChoose_MoveSelectedFileToSlot(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    s16 yStep;
    s16 i;

    yStep = ABS(this->buttonYOffsets[this->buttonIndex]) / this->actionTimer;
    this->buttonYOffsets[this->buttonIndex] -= yStep;

    if (this->buttonYOffsets[this->buttonIndex] <= 0) {
        this->buttonYOffsets[this->buttonIndex] = 0;
    }

    for (i = 0; i < 3; i++) {
        if (i != this->buttonIndex) {
            this->fileButtonAlpha[i] += 25;

            if (this->fileButtonAlpha[i] >= 200) {
                this->fileButtonAlpha[i] = 200;
            }

            this->actionButtonAlpha[FS_BTN_ACTION_COPY] = this->actionButtonAlpha[FS_BTN_ACTION_ERASE] =
                this->optionButtonAlpha = this->fileButtonAlpha[i];

            if (Save_GetSaveMetaInfo(i)->valid) {
                this->nameBoxAlpha[i] = this->nameAlpha[i] = this->fileButtonAlpha[i];
                this->connectorAlpha[i] += 31;
            }
        }
    }

    this->titleAlpha[0] -= 31;
    this->titleAlpha[1] += 31;
    this->actionTimer--;

    if (this->actionTimer == 0) {
        this->titleAlpha[0] = 255;
        this->titleAlpha[1] = 0;
        this->titleLabel = this->nextTitleLabel;
        this->actionTimer = 8;
        this->menuMode = FS_MENU_MODE_CONFIG;
        this->configMode = CM_MAIN_MENU;
        this->nextConfigMode = CM_MAIN_MENU;
        this->selectMode = SM_FADE_MAIN_TO_SELECT;
    }
}

/**
 * Fill the screen with black to fade out.
 * Update function for `SM_FADE_OUT`
 */
void FileChoose_FadeOut(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    sScreenFillAlpha += VREG(10);

    if (sScreenFillAlpha >= 255) {
        sScreenFillAlpha = 255;
        this->selectMode++;
    }
}

/**
 * Load the save for the appropriate file and start the game.
 * Note: On Debug ROM, File 1 will go to Map Select.
 * Update function for `SM_LOAD_GAME`
 */
void FileChoose_LoadGame(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    u16 swordEquipMask;
    s32 pad;

    Audio_PlaySoundGeneral(NA_SE_SY_FSEL_DECIDE_L, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    gSaveContext.fileNum = this->buttonIndex;
    gSaveContext.gameMode = 0;

    if ((this->buttonIndex == FS_BTN_SELECT_FILE_1 && CVarGetInteger("gDebugEnabled", 0)) || this->buttonIndex == 0xFF) {
        if (this->buttonIndex == 0xFF) {
            Sram_InitDebugSave();
        } else {
            Sram_OpenSave();
        }
        SET_NEXT_GAMESTATE(&this->state, Select_Init, SelectContext);
    } else {
        if (this->buttonIndex == 0xFE) {
            Sram_InitBossRushSave();
        } else {
            Sram_OpenSave();
        }
        SET_NEXT_GAMESTATE(&this->state, Play_Init, PlayState);
    }

    this->state.running = false;

    Randomizer_LoadSettings("");
    Randomizer_LoadHintLocations("");
    Randomizer_LoadItemLocations("", true);
    Randomizer_LoadRequiredTrials("");
    Randomizer_LoadMerchantMessages("");
    Randomizer_LoadMasterQuestDungeons("");
    Randomizer_LoadEntranceOverrides("", true);

    gSaveContext.respawn[0].entranceIndex = -1;
    gSaveContext.respawnFlag = 0;
    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
    gSaveContext.natureAmbienceId = 0xFF;
    gSaveContext.showTitleCard = true;
    if (!CVarGetInteger("gDogFollowsEverywhere", 0)) {
        gSaveContext.dogParams = 0;
    }
    gSaveContext.timer1State = 0;
    gSaveContext.timer2State = 0;
    gSaveContext.eventInf[0] = 0;
    gSaveContext.eventInf[1] = 0;
    gSaveContext.eventInf[2] = 0;
    gSaveContext.eventInf[3] = 0;
    gSaveContext.unk_13EE = 0x32;
    gSaveContext.nayrusLoveTimer = 0;
    gSaveContext.healthAccumulator = 0;
    gSaveContext.magicState = 0;
    gSaveContext.prevMagicState = 0;
    gSaveContext.forcedSeqId = NA_BGM_GENERAL_SFX;
    gSaveContext.skyboxTime = 0;
    gSaveContext.nextTransitionType = 0xFF;
    gSaveContext.nextCutsceneIndex = 0xFFEF;
    gSaveContext.cutsceneTrigger = 0;
    gSaveContext.chamberCutsceneNum = 0;
    gSaveContext.nextDayTime = 0xFFFF;
    gSaveContext.retainWeatherMode = 0;

    for (int buttonIndex = 0; buttonIndex < ARRAY_COUNT(gSaveContext.buttonStatus); buttonIndex++) {
        gSaveContext.buttonStatus[buttonIndex] = BTN_ENABLED;
    }

    gSaveContext.unk_13E7 = gSaveContext.unk_13E8 = gSaveContext.unk_13EA = gSaveContext.unk_13EC =
        gSaveContext.magicCapacity = 0;

    gSaveContext.magicFillTarget = gSaveContext.magic;
    gSaveContext.magic = 0;
    gSaveContext.magicLevel = gSaveContext.magic;

    osSyncPrintf(VT_FGCOL(GREEN));
    osSyncPrintf("Z_MAGIC_NOW_NOW=%d  MAGIC_NOW=%d\n", ((void)0, gSaveContext.magicFillTarget), gSaveContext.magic);
    osSyncPrintf(VT_RST);

    gSaveContext.naviTimer = 0;

    // SWORDLESS LINK IS BACK BABY
    if (CVarGetInteger("gSwordlessLink", 0) != 0)
    {
        if ((gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KOKIRI) &&
            (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_MASTER) &&
            (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_BGS) &&
            (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KNIFE)) {

            gSaveContext.equips.buttonItems[0] = ITEM_NONE;
            swordEquipMask = BOMSWAP16(gEquipMasks[EQUIP_SWORD]) & gSaveContext.equips.equipment;
            gSaveContext.equips.equipment &= gEquipNegMasks[EQUIP_SWORD];
            gSaveContext.inventory.equipment ^= (gBitFlags[swordEquipMask - 1] << BOMSWAP16(gEquipShifts[EQUIP_SWORD]));
        }
    }

    if (gSaveContext.n64ddFlag) {
        // Setup the modified entrance table and entrance shuffle table for rando
        Entrance_Init();

        // Handle randomized spawn positions after the save context has been setup from load
        // When remeber save location is on, set save warp if the save was in an a grotto, or
        // the entrance index is -1 from shuffle overwarld spawn
        if (Randomizer_GetSettingValue(RSK_SHUFFLE_ENTRANCES) && ((!CVarGetInteger("gRememberSaveLocation", 0) ||
            gSaveContext.savedSceneNum == SCENE_YOUSEI_IZUMI_TATE || gSaveContext.savedSceneNum == SCENE_KAKUSIANA) ||
            (CVarGetInteger("gRememberSaveLocation", 0) && Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_SPAWNS) && gSaveContext.entranceIndex == -1))) {
            Entrance_SetSavewarpEntrance();
        }
    }

    GameInteractor_ExecuteOnLoadGame(gSaveContext.fileNum);
}

static void (*gSelectModeUpdateFuncs[])(GameState*) = {
    FileChoose_FadeMainToSelect, FileChoose_MoveSelectedFileToTop,  FileChoose_FadeInFileInfo, FileChoose_ConfirmFile,
    FileChoose_FadeOutFileInfo,  FileChoose_MoveSelectedFileToSlot, FileChoose_FadeOut,        FileChoose_LoadGame,
};

void FileChoose_SelectModeUpdate(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;

    gSelectModeUpdateFuncs[this->selectMode](&this->state);
}

void FileChoose_SelectModeDraw(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    f32 eyeX;
    f32 eyeY;
    f32 eyeZ;

    OPEN_DISPS(this->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);

    eyeX = 1000.0f * Math_CosS(ZREG(11)) - 1000.0f * Math_SinS(ZREG(11));
    eyeY = ZREG(13);
    eyeZ = 1000.0f * Math_SinS(ZREG(11)) + 1000.0f * Math_CosS(ZREG(11));

    FileChoose_SetView(this, eyeX, eyeY, eyeZ);
    SkyboxDraw_Draw(&this->skyboxCtx, this->state.gfxCtx, 1, this->envCtx.skyboxBlend, eyeX, eyeY, eyeZ);
    gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_NONE);
    ZREG(11) += ZREG(10);
    Environment_UpdateSkybox(NULL, SKYBOX_NORMAL_SKY, &this->envCtx, &this->skyboxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    Gfx_SetupDL_42Opa(this->state.gfxCtx);
    FileChoose_SetView(this, 0.0f, 0.0f, 64.0f);
    FileChoose_SetWindowVtx(&this->state);
    FileChoose_SetWindowContentVtx(&this->state);

    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                    this->windowAlpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    Matrix_Translate(0.0f, 0.0f, -93.6f, MTXMODE_NEW);
    Matrix_Scale(0.78f, 0.78f, 0.78f, MTXMODE_APPLY);
    Matrix_RotateX(this->windowRot / 100.0f, MTXMODE_APPLY);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(this->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPVertex(POLY_OPA_DISP++, &this->windowVtx[0], 32, 0);
    gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow1DL);

    gSPVertex(POLY_OPA_DISP++, &this->windowVtx[32], 32, 0);
    gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow2DL);

    gSPVertex(POLY_OPA_DISP++, &this->windowVtx[64], 16, 0);
    gSPDisplayList(POLY_OPA_DISP++, gFileSelWindow3DL);

    FileChoose_DrawWindowContents(&this->state);
    gDPPipeSync(POLY_OPA_DISP++);
    FileChoose_SetView(this, 0.0f, 0.0f, 64.0f);

    DrawSeedHashSprites(this);

    CLOSE_DISPS(this->state.gfxCtx);
}

static void (*gFileSelectDrawFuncs[])(GameState*) = {
    FileChoose_InitModeDraw,
    FileChoose_ConfigModeDraw,
    FileChoose_SelectModeDraw,
};

static void (*gFileSelectUpdateFuncs[])(GameState*) = {
    FileChoose_InitModeUpdate,
    FileChoose_ConfigModeUpdate,
    FileChoose_SelectModeUpdate,
};

void FileChoose_Main(GameState* thisx) {
    static void* controlsTextures[] = {
        gFileSelControlsENGTex,
        gFileSelControlsGERTex,
        gFileSelControlsFRATex,
    };
    FileChooseContext* this = (FileChooseContext*)thisx;
    Input* input = &this->state.input[0];

    Color_RGB8 helpTextColor = { 100, 255, 255 };
    if (CVarGetInteger("gCosmetics.Title_FileChoose.Changed", 0)) {
        Color_RGB8 backgroundColor = CVarGetColor24("gCosmetics.Title_FileChoose.Value", (Color_RGB8){ 100, 150, 255 });
        this->windowColor[0] = backgroundColor.r;
        this->windowColor[1] = backgroundColor.g;
        this->windowColor[2] = backgroundColor.b;
        this->highlightColor[0] = MIN(backgroundColor.r + 100, 255);
        this->highlightColor[1] = MIN(backgroundColor.g + 100, 255);
        this->highlightColor[2] = MIN(backgroundColor.b + 100, 255);
        helpTextColor.r = MIN(backgroundColor.r + 100, 255);
        helpTextColor.g = MIN(backgroundColor.g + 100, 255);
        helpTextColor.b = MIN(backgroundColor.b + 100, 255);
        sWindowContentColors[0][0] = backgroundColor.r;
        sWindowContentColors[0][1] = backgroundColor.g;
        sWindowContentColors[0][2] = backgroundColor.b;
    } else {
        this->windowColor[0] = 100;
        this->windowColor[1] = 150;
        this->windowColor[2] = 255;
        this->highlightColor[0] = 155;
        this->highlightColor[1] = 255;
        this->highlightColor[2] = 255;
        sWindowContentColors[0][0] = 100;
        sWindowContentColors[0][1] = 150;
        sWindowContentColors[0][2] = 255;
    }

    if (CVarGetInteger("gTimeFlowFileSelect", 0) != 0) {
        gSaveContext.skyboxTime += 0x10;
    }

    if (CVarGetInteger("gSkipLogoTitle", 0) && CVarGetInteger("gSaveFileID", FASTFILE_1) <= FASTFILE_3 && !isFastFileIdIncompatible) {
        if (Save_Exist(CVarGetInteger("gSaveFileID", FASTFILE_1)) && FileChoose_IsSaveCompatible(Save_GetSaveMetaInfo(CVarGetInteger("gSaveFileID", FASTFILE_1)))) {
            this->buttonIndex = CVarGetInteger("gSaveFileID", FASTFILE_1);
            this->menuMode = FS_MENU_MODE_SELECT;
            this->selectMode = SM_LOAD_GAME;
        } else {
            isFastFileIdIncompatible = 1;
        }
    } else if (CVarGetInteger("gSkipLogoTitle", 0) && CVarGetInteger("gSaveFileID", FASTFILE_1) == FASTFILE_MAP_SELECT) {
        this->buttonIndex = 0xFF;
        this->menuMode = FS_MENU_MODE_SELECT;
        this->selectMode = SM_LOAD_GAME;
    }

    OPEN_DISPS(this->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    gSPSegment(POLY_OPA_DISP++, 0x01, this->staticSegment);
    gSPSegment(POLY_OPA_DISP++, 0x02, this->parameterSegment);

    Gfx_SetupFrame(this->state.gfxCtx, 0, 0, 0);

    this->stickRelX = input->rel.stick_x;
    this->stickRelY = input->rel.stick_y;

    if (CVarGetInteger("gDpadHoldChange", 1) && CVarGetInteger("gDpadText", 0)) {
        if (CHECK_BTN_ALL(input->cur.button, BTN_DLEFT)) {
            if (CHECK_BTN_ALL(input->press.button, BTN_DLEFT)) {
                this->inputTimerX = 10;
                this->stickXDir = -1;
            } else if (--(this->inputTimerX) < 0) {
                this->inputTimerX = XREG(6);
                input->press.button |= BTN_DLEFT;
            }
        } else if (CHECK_BTN_ALL(input->rel.button, BTN_DLEFT)) {
            this->stickXDir = 0;
        } else if (CHECK_BTN_ALL(input->cur.button, BTN_DRIGHT)) {
            if (CHECK_BTN_ALL(input->press.button, BTN_DRIGHT)) {
                this->inputTimerX = 10;
                this->stickXDir = 1;
            } else if (--(this->inputTimerX) < 0) {
                this->inputTimerX = XREG(6);
                input->press.button |= BTN_DRIGHT;
            }
        } else if (CHECK_BTN_ALL(input->rel.button, BTN_DRIGHT)) {
            this->stickXDir = 0;
        }

        if (CHECK_BTN_ALL(input->cur.button, BTN_DDOWN)) {
            if (CHECK_BTN_ALL(input->press.button, BTN_DDOWN)) {
                this->inputTimerY = 10;
                this->stickYDir = -1;
            } else if (--(this->inputTimerY) < 0) {
                this->inputTimerY = XREG(6);
                input->press.button |= BTN_DDOWN;
            }
        } else if (CHECK_BTN_ALL(input->rel.button, BTN_DDOWN)) {
            this->stickYDir = 0;
        } else if (CHECK_BTN_ALL(input->cur.button, BTN_DUP)) {
            if (CHECK_BTN_ALL(input->press.button, BTN_DUP)) {
                this->inputTimerY = 10;
                this->stickYDir = -1;
            } else if (--(this->inputTimerY) < 0) {
                this->inputTimerY = XREG(6);
                input->press.button |= BTN_DUP;
            }
        } else if (CHECK_BTN_ALL(input->rel.button, BTN_DUP)) {
            this->stickYDir = 0;
        }
    }

    if (this->stickRelX < -30) {
        if (this->stickXDir == -1) {
            this->inputTimerX--;
            if (this->inputTimerX < 0) {
                this->inputTimerX = 2;
            } else {
                this->stickRelX = 0;
            }
        } else {
            this->inputTimerX = 10;
            this->stickXDir = -1;
        }
    } else if (this->stickRelX > 30) {
        if (this->stickXDir == 1) {
            this->inputTimerX--;
            if (this->inputTimerX < 0) {
                this->inputTimerX = 2;
            } else {
                this->stickRelX = 0;
            }
        } else {
            this->inputTimerX = 10;
            this->stickXDir = 1;
        }
    } else {
        this->stickXDir = 0;
    }

    if (this->stickRelY < -30) {
        if (this->stickYDir == -1) {
            this->inputTimerY--;
            if (this->inputTimerY < 0) {
                this->inputTimerY = 2;
            } else {
                this->stickRelY = 0;
            }
        } else {
            this->inputTimerY = 10;
            this->stickYDir = -1;
        }
    } else if (this->stickRelY > 30) {
        if (this->stickYDir == 1) {
            this->inputTimerY--;
            if (this->inputTimerY < 0) {
                this->inputTimerY = 2;
            } else {
                this->stickRelY = 0;
            }
        } else {
            this->inputTimerY = 10;
            this->stickYDir = 1;
        }
    } else {
        this->stickYDir = 0;
    }

    this->emptyFileTextAlpha = 0;

    FileChoose_PulsateCursor(&this->state);
    gFileSelectUpdateFuncs[this->menuMode](&this->state);
    FrameInterpolation_StartRecord();
    gFileSelectDrawFuncs[this->menuMode](&this->state);
    FrameInterpolation_StopRecord();

    // do not draw controls text in the options menu
    if ((this->configMode <= CM_NAME_ENTRY_TO_MAIN) || (this->configMode >= CM_UNUSED_DELAY)) {
        Gfx_SetupDL_39Opa(this->state.gfxCtx);

        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, helpTextColor.r, helpTextColor.g, helpTextColor.b, this->controlsAlpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
        gDPLoadTextureBlock(POLY_OPA_DISP++, controlsTextures[gSaveContext.language], G_IM_FMT_IA, G_IM_SIZ_8b, 144, 16,
                            0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                            G_TX_NOLOD, G_TX_NOLOD);
        gSPTextureRectangle(POLY_OPA_DISP++, 0x0168, 0x0330, 0x03A8, 0x0370, G_TX_RENDERTILE, 0, 0, 0x0400, 0x0400);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gSPDisplayList(POLY_OPA_DISP++, sScreenFillSetupDL);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, sScreenFillAlpha);
    gDPFillRectangle(POLY_OPA_DISP++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);

    CLOSE_DISPS(this->state.gfxCtx);
}

void FileChoose_InitContext(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    EnvironmentContext* envCtx = &this->envCtx;

    ZREG(7) = 32;
    ZREG(8) = 22;
    ZREG(9) = 20;
    ZREG(10) = -10;
    ZREG(11) = 0;
    ZREG(12) = 1000;
    ZREG(13) = -700;
    ZREG(14) = 164;
    ZREG(15) = 104;
    ZREG(16) = 160;
    ZREG(17) = 100;
    ZREG(18) = 162;
    ZREG(19) = 152;
    ZREG(20) = 214;

    XREG(13) = 580;
    XREG(14) = 400;
    XREG(35) = 20;
    XREG(36) = 20;
    XREG(37) = 20;
    XREG(43) = 8;
    XREG(44) = -78;
    XREG(45) = 0;
    XREG(46) = 0;
    XREG(47) = 0;
    XREG(48) = 0;
    XREG(49) = 3;
    XREG(50) = 8;
    XREG(51) = 8;
    XREG(52) = 10;
    XREG(73) = 0;

    VREG(0) = 14;
    VREG(1) = 5;
    VREG(2) = 4;
    VREG(4) = 1;
    VREG(5) = 6;
    VREG(6) = 2;
    VREG(7) = 6;
    VREG(8) = 80;

    sScreenFillAlpha = 255;

    VREG(10) = 10;
    VREG(11) = 30;
    VREG(12) = -100;
    VREG(13) = -85;
    VREG(14) = 4;
    VREG(16) = 25;
    VREG(17) = 1;
    VREG(18) = 1;
    VREG(20) = 92;
    VREG(21) = 171;
    VREG(22) = 11;
    VREG(23) = 10;
    VREG(24) = 26;
    VREG(25) = 2;
    VREG(26) = 1;
    VREG(27) = 0;
    VREG(28) = 0;
    VREG(29) = 160;
    VREG(30) = 64;
    VREG(31) = 154;
    VREG(32) = 152;
    VREG(33) = 106;

    WREG(38) = 16;
    WREG(39) = 9;
    WREG(40) = 10;
    WREG(41) = 14;
    WREG(42) = 11;
    WREG(43) = 12;

    this->menuMode = FS_MENU_MODE_INIT;

    this->buttonIndex = this->selectMode = this->selectedFileIndex = this->copyDestFileIndex =
        this->confirmButtonIndex = 0;

    this->confirmButtonTexIndices[0] = 2;
    this->confirmButtonTexIndices[1] = 3;
    this->titleLabel = FS_TITLE_SELECT_FILE;
    this->nextTitleLabel = FS_TITLE_OPEN_FILE;
    this->highlightPulseDir = 1;
    this->unk_1CAAC = 0xC;
    this->highlightColor[0] = 155;
    this->highlightColor[1] = 255;
    this->highlightColor[2] = 255;
    this->highlightColor[3] = 70;
    this->configMode = CM_FADE_IN_START;
    this->windowRot = 0.0f;
    this->stickXDir = this->inputTimerX = 0;
    this->stickYDir = this->inputTimerY = 0;
    this->kbdX = this->kbdY = this->charIndex = 0;
    this->kbdButton = FS_KBD_BTN_NONE;

    this->windowColor[0] = 100;
    this->windowColor[1] = 150;
    this->windowColor[2] = 255;

    this->windowAlpha = this->titleAlpha[0] = this->titleAlpha[1] = this->fileButtonAlpha[0] =
        this->fileButtonAlpha[1] = this->fileButtonAlpha[2] = this->nameBoxAlpha[0] = this->nameBoxAlpha[1] =
            this->nameBoxAlpha[2] = this->nameAlpha[0] = this->nameAlpha[1] = this->nameAlpha[2] =
                this->connectorAlpha[0] = this->connectorAlpha[1] = this->connectorAlpha[2] = this->fileInfoAlpha[0] =
                    this->fileInfoAlpha[1] = this->fileInfoAlpha[2] = this->actionButtonAlpha[FS_BTN_ACTION_COPY] =
                        this->actionButtonAlpha[FS_BTN_ACTION_ERASE] = this->confirmButtonAlpha[FS_BTN_CONFIRM_YES] =
                            this->confirmButtonAlpha[FS_BTN_CONFIRM_QUIT] = this->optionButtonAlpha =
                                this->nameEntryBoxAlpha = this->controlsAlpha = this->emptyFileTextAlpha = 0;

    this->windowPosX = 6;
    this->actionTimer = 8;
    this->warningLabel = FS_WARNING_NONE;

    this->warningButtonIndex = this->buttonYOffsets[0] = this->buttonYOffsets[1] = this->buttonYOffsets[2] =
        this->buttonYOffsets[3] = this->buttonYOffsets[4] = this->buttonYOffsets[5] = this->fileNamesY[0] =
            this->fileNamesY[1] = this->fileNamesY[2] = 0;

    this->unk_1CAD6[0] = 0;
    this->unk_1CAD6[1] = 3;
    this->unk_1CAD6[2] = 6;
    this->unk_1CAD6[3] = 8;
    this->unk_1CAD6[4] = 10;

    this->stickLeftPrompt.stickColorR = 200;
    this->stickLeftPrompt.stickColorG = 200;
    this->stickLeftPrompt.stickColorB = 200;
    this->stickLeftPrompt.stickColorA = 180;
    this->stickLeftPrompt.stickTexX = 79;
    this->stickLeftPrompt.stickTexY = 139;
    this->stickLeftPrompt.arrowColorR = 155;
    this->stickLeftPrompt.arrowColorG = 155;
    this->stickLeftPrompt.arrowColorB = 255;
    this->stickLeftPrompt.arrowColorA = 200;
    this->stickLeftPrompt.arrowTexX = 63;
    this->stickLeftPrompt.arrowTexY = 135;
    this->stickLeftPrompt.z = 1;
    this->stickLeftPrompt.isEnabled = false;

    this->stickRightPrompt.stickColorR = 200;
    this->stickRightPrompt.stickColorG = 200;
    this->stickRightPrompt.stickColorB = 200;
    this->stickRightPrompt.stickColorA = 180;
    this->stickRightPrompt.stickTexX = 244;
    this->stickRightPrompt.stickTexY = 139;
    this->stickRightPrompt.arrowColorR = 155;
    this->stickRightPrompt.arrowColorG = 155;
    this->stickRightPrompt.arrowColorB = 255;
    this->stickRightPrompt.arrowColorA = 200;
    this->stickRightPrompt.arrowTexX = 260;
    this->stickRightPrompt.arrowTexY = 135;
    this->stickRightPrompt.z = 1;
    this->stickRightPrompt.isEnabled = false;

    this->arrowAnimState = 0;
    this->stickAnimState = 0;
    this->arrowAnimTween = 0;
    this->stickAnimTween = 0;

    this->bossRushIndex = 0;
    this->bossRushOffset = 0;

    ShrinkWindow_SetVal(0);

    gSaveContext.skyboxTime = 0;
    gSaveContext.dayTime = 0;

    Skybox_Init(&this->state, &this->skyboxCtx, SKYBOX_NORMAL_SKY);

    gTimeIncrement = 10;

    envCtx->unk_19 = 0;
    envCtx->unk_1A = 0;
    envCtx->unk_21 = 0;
    envCtx->unk_22 = 0;
    envCtx->skyboxDmaState = SKYBOX_DMA_INACTIVE;
    envCtx->skybox1Index = 99;
    envCtx->skybox2Index = 99;
    envCtx->unk_1F = 0;
    envCtx->unk_20 = 0;
    envCtx->unk_BD = 0;
    envCtx->unk_17 = 2;
    envCtx->skyboxDisabled = 0;
    envCtx->skyboxBlend = 0;
    envCtx->unk_84 = 0.0f;
    envCtx->unk_88 = 0.0f;

    Environment_UpdateSkybox(NULL, SKYBOX_NORMAL_SKY, &this->envCtx, &this->skyboxCtx);

    for (int buttonIndex = 0; buttonIndex < ARRAY_COUNT(gSaveContext.buttonStatus); buttonIndex++) {
        gSaveContext.buttonStatus[buttonIndex] = BTN_ENABLED;
    }
}

void FileChoose_Destroy(GameState* thisx) {
}

void FileChoose_Init(GameState* thisx) {
    FileChooseContext* this = (FileChooseContext*)thisx;
    size_t size = (u32)_title_staticSegmentRomEnd - (u32)_title_staticSegmentRomStart;
    s32 pad;
    this->logoAlpha = 0;
    this->questType[0] = MIN_QUEST;
    this->questType[1] = MIN_QUEST;
    this->questType[2] = MIN_QUEST;
    fileSelectSpoilerFileLoaded = false;
    isFastFileIdIncompatible = 0;
    CVarSetInteger("gOnFileSelectNameEntry", 0);

    SREG(30) = 1;
    osSyncPrintf("SIZE=%x\n", size);

    this->staticSegment = GAMESTATE_ALLOC_MC(&this->state, size);
    assert(this->staticSegment != NULL);
    DmaMgr_SendRequest1(this->staticSegment, (u32)_title_staticSegmentRomStart, size, __FILE__, __LINE__);

    size = (u32)_parameter_staticSegmentRomEnd - (u32)_parameter_staticSegmentRomStart;
    this->parameterSegment = GAMESTATE_ALLOC_MC(&this->state, size);
    assert(this->parameterSegment != NULL);
    DmaMgr_SendRequest1(this->parameterSegment, (u32)_parameter_staticSegmentRomStart, size, __FILE__,
                        __LINE__);

    // Load some registers used by the dialog system
    Regs_InitData(NULL); // Passing in NULL as we dont have a playstate, and it isn't used in the func

    Matrix_Init(&this->state);
    View_Init(&this->view, this->state.gfxCtx);
    this->state.main = FileChoose_Main;
    this->state.destroy = FileChoose_Destroy;
    FileChoose_InitContext(&this->state);
    Font_LoadOrderedFont(&this->font);
    Audio_QueueSeqCmd(0xF << 28 | SEQ_PLAYER_BGM_MAIN << 24 | 0xA);
    func_800F5E18(SEQ_PLAYER_BGM_MAIN, NA_BGM_FILE_SELECT, 0, 7, 1);
}
