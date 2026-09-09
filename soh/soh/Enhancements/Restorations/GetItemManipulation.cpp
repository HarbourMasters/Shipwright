#include <libultraship/bridge/consolevariablebridge.h>

#include "GetItemManipulation.h"
#include "soh/Enhancements/item-tables/ItemTableManager.h"

extern "C" {
#include "z64.h"
extern PlayState* gPlayState;
GetItemID RetrieveGetItemIDFromItemID(ItemID itemID);
}

// On N64, Get Item Manipulation (GIM) makes the player receive an item with a negative getItemId,
// so the vanilla lookup `sGetItemTable[getItemId - 1]` reads out of bounds below the table. The
// bytes it lands on are fixed player overlay data, so each negative getItemId deterministically
// resolves to the itemId below, indexed by getItemId + 128. getItemId is s8 on console, so
// [-128, -1] covers every value reachable there.
//
// Tables from:
// ItemID ->
// https://docs.google.com/spreadsheets/d/1SLJzamokLb7wDOaJh5x8DsxmMBy9oIYawyDN3dAWppw/edit?gid=870929937#gid=870929937
// Text   -> https://docs.google.com/spreadsheets/d/13N9lJqF5JSBZkVkVTvkrorJ8PlEBMowUH4nkrG93BuQ/edit?gid=0#gid=0

// Format for matrix below:
// [NTSC 1.0], [NTSC 1.1], [NTSC 1.2], [PAL1.0/1.1], [(GC U/J) / (MQ J)], [(GC E) / (MQ U/E)], [IQUE CHN/TWN], [MQ
// Debug], [MZX NTSC], [MZX PAL]
static const ItemID sGimItemIdsVer[40][10] = {
    /* 80 */ { ITEM_POE, ITEM_MASK_GORON, ITEM_POE, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_BULLET_BAG_30,
               ITEM_POE, ITEM_STICK },
    /* 81 */
    { ITEM_BOMB, ITEM_STICK, ITEM_BOMB, ITEM_ARROW_ICE, ITEM_ARROW_ICE, ITEM_ARROW_ICE, ITEM_ARROW_ICE, ITEM_BEAN,
      ITEM_BOMB, ITEM_ARROW_ICE },
    /* 82 */
    { ITEM_CHICKEN, ITEM_STICK, ITEM_MASK_KEATON, ITEM_ODD_MUSHROOM, ITEM_ODD_MUSHROOM, ITEM_ODD_MUSHROOM,
      ITEM_ODD_MUSHROOM, ITEM_BOMB, ITEM_NONE, ITEM_ODD_MUSHROOM },
    /* 83 */
    { ITEM_BEAN, ITEM_BOMBS_10, ITEM_BEAN, ITEM_BOMBS_5, ITEM_BOMBS_5, ITEM_BOMBS_5, ITEM_BOMBS_5, ITEM_BOMB, ITEM_BEAN,
      ITEM_BOMBS_5 },
    /* 84 */
    { ITEM_NUT, ITEM_MASK_GORON, ITEM_NUT, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_SONG_MINUET, ITEM_NUT,
      ITEM_STICK },
    /* 85 */
    { ITEM_BOMB, ITEM_NONE, ITEM_BOMB, ITEM_ARROW_ICE, ITEM_ARROW_ICE, ITEM_ARROW_ICE, ITEM_ARROW_ICE, ITEM_BOMBS_10,
      ITEM_BOMB, ITEM_ARROW_ICE },
    /* 86 */
    { ITEM_PRESCRIPTION, ITEM_STICK, ITEM_EYEDROPS, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_TUNIC_GORON,
      ITEM_NONE, ITEM_STICK },
    /* 87 */
    { ITEM_BOMBS_10, ITEM_BOOTS_KOKIRI, ITEM_BOMBS_10, ITEM_SWORD_MASTER, ITEM_SWORD_MASTER, ITEM_SWORD_MASTER,
      ITEM_SWORD_MASTER, ITEM_NONE, ITEM_BOMBS_10, ITEM_SWORD_MASTER },
    /* 88 */
    { ITEM_TUNIC_GORON, ITEM_STICK, ITEM_TUNIC_GORON, ITEM_BRACELET, ITEM_BRACELET, ITEM_BRACELET, ITEM_BRACELET,
      ITEM_ARROW_FIRE, ITEM_TUNIC_GORON, ITEM_BRACELET },
    /* 89 */
    { ITEM_NONE, ITEM_MASK_KEATON, ITEM_NONE, ITEM_SWORD_MASTER, ITEM_SWORD_MASTER, ITEM_SWORD_MASTER,
      ITEM_SWORD_MASTER, ITEM_BOOTS_HOVER, ITEM_NONE, ITEM_SWORD_MASTER },
    /* 8A */
    { ITEM_ARROW_FIRE, ITEM_SHIELD_MIRROR, ITEM_ARROW_FIRE, ITEM_ODD_POTION, ITEM_ODD_POTION, ITEM_ODD_POTION,
      ITEM_ODD_POTION, ITEM_STICK, ITEM_ARROW_FIRE, ITEM_ODD_POTION },
    /* 8B */
    { ITEM_BOOTS_HOVER, ITEM_BOOTS_IRON, ITEM_BOOTS_HOVER, ITEM_BOMBS_10, ITEM_BOMBS_10, ITEM_BOMBS_10, ITEM_BOMBS_10,
      ITEM_STICK, ITEM_BOOTS_HOVER, ITEM_BOMBS_10 },
    /* 8C */
    { ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_ARROW_FIRE, ITEM_STICK,
      ITEM_STICK },
    /* 8D */
    { ITEM_STICK, ITEM_BEAN, ITEM_STICK, ITEM_SWORD_MASTER, ITEM_SWORD_MASTER, ITEM_SWORD_MASTER, ITEM_SWORD_MASTER,
      ITEM_MASK_KEATON, ITEM_STICK, ITEM_SWORD_MASTER },
    /* 8E */
    { ITEM_ARROW_FIRE, ITEM_POCKET_EGG, ITEM_ARROW_FIRE, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK,
      ITEM_BULLET_BAG_50, ITEM_ARROW_FIRE, ITEM_STICK },
    /* 8F */
    { ITEM_MASK_KEATON, ITEM_BOMB, ITEM_MASK_KEATON, ITEM_ARROW_ICE, ITEM_ARROW_ICE, ITEM_ARROW_ICE, ITEM_ARROW_ICE,
      ITEM_BOMBS_5, ITEM_MASK_KEATON, ITEM_ARROW_ICE },
    /* 90 */
    { ITEM_LETTER_ZELDA, ITEM_NUT, ITEM_MASK_KEATON, ITEM_MASK_GORON, ITEM_MASK_GORON, ITEM_MASK_GORON, ITEM_MASK_GORON,
      ITEM_ARROW_FIRE, ITEM_NONE, ITEM_MASK_GORON },
    /* 91 */
    { ITEM_BOMBS_5, ITEM_MASK_SKULL, ITEM_BOMBS_5, ITEM_SWORD_MASTER, ITEM_SWORD_MASTER, ITEM_SWORD_MASTER,
      ITEM_SWORD_MASTER, ITEM_POTION_RED, ITEM_BOMBS_5, ITEM_SWORD_MASTER },
    /* 92 */
    { ITEM_ARROW_FIRE, ITEM_STICK, ITEM_ARROW_FIRE, ITEM_POCKET_EGG, ITEM_POCKET_EGG, ITEM_POCKET_EGG, ITEM_POCKET_EGG,
      ITEM_STICK, ITEM_ARROW_FIRE, ITEM_POCKET_EGG },
    /* 93 */
    { ITEM_POTION_RED, ITEM_BOMB, ITEM_POTION_RED, ITEM_BOMBS_10, ITEM_BOMBS_10, ITEM_BOMBS_10, ITEM_BOMBS_10,
      ITEM_ARROW_ICE, ITEM_POTION_RED, ITEM_BOMBS_10 },
    /* 94 */
    { ITEM_STICK, ITEM_MASK_SPOOKY, ITEM_STICK, ITEM_MASK_GORON, ITEM_MASK_GORON, ITEM_MASK_GORON, ITEM_MASK_GORON,
      ITEM_STICK, ITEM_STICK, ITEM_MASK_GORON },
    /* 95 */
    { ITEM_ARROW_ICE, ITEM_ARROW_ICE, ITEM_ARROW_ICE, ITEM_MASK_KEATON, ITEM_MASK_KEATON, ITEM_MASK_KEATON,
      ITEM_MASK_KEATON, ITEM_BOMB, ITEM_ARROW_ICE, ITEM_MASK_KEATON },
    /* 96 */
    { ITEM_STICK, ITEM_POE, ITEM_STICK, ITEM_MILK_BOTTLE, ITEM_MASK_SPOOKY, ITEM_BLUE_FIRE, ITEM_ARROW_LIGHT,
      ITEM_SLINGSHOT, ITEM_STICK, ITEM_NONE },
    /* 97 */
    { ITEM_BOMB, ITEM_SWORD_MASTER, ITEM_BOMB, ITEM_BOMBS_5, ITEM_BOMBS_5, ITEM_BOMBS_5, ITEM_BOMBS_5,
      ITEM_PRESCRIPTION, ITEM_BOMB, ITEM_BOMBS_5 },
    /* 98 */
    { ITEM_SLINGSHOT, ITEM_STICK, ITEM_SLINGSHOT, ITEM_POE, ITEM_POE, ITEM_POE, ITEM_POE, ITEM_NONE, ITEM_SLINGSHOT,
      ITEM_POE },
    /* 99 */
    { ITEM_PRESCRIPTION, ITEM_NONE, ITEM_PRESCRIPTION, ITEM_BOW, ITEM_BOW, ITEM_BOW, ITEM_BOW, ITEM_BOMBS_5,
      ITEM_PRESCRIPTION, ITEM_BOW },
    /* 9A */
    { ITEM_NONE, ITEM_SLINGSHOT, ITEM_NONE, ITEM_SLINGSHOT, ITEM_SLINGSHOT, ITEM_SLINGSHOT, ITEM_SLINGSHOT,
      ITEM_SLINGSHOT, ITEM_NONE, ITEM_SLINGSHOT },
    /* 9B */
    { ITEM_BOMBS_5, ITEM_SCALE_GOLDEN, ITEM_BOMBS_5, ITEM_BOMBS_5, ITEM_BOMBS_5, ITEM_BOMBS_5, ITEM_BOMBS_5,
      ITEM_BOMBS_10, ITEM_BOMBS_5, ITEM_BOMBS_5 },
    /* 9C */
    { ITEM_SLINGSHOT, ITEM_STICK, ITEM_SLINGSHOT, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_NUT,
      ITEM_SLINGSHOT, ITEM_STICK },
    /* 9D */
    { ITEM_BOMBS_10, ITEM_MASK_KEATON, ITEM_BOMBS_10, ITEM_ARROWS_LARGE, ITEM_ARROWS_LARGE, ITEM_ARROWS_LARGE,
      ITEM_ARROWS_LARGE, ITEM_ODD_POTION, ITEM_BOMBS_10, ITEM_ARROWS_LARGE },
    /* 9E */
    { ITEM_NUT, ITEM_NONE, ITEM_NUT, ITEM_BOMB, ITEM_BOMB, ITEM_BOMB, ITEM_BOMB, ITEM_STICK, ITEM_NUT, ITEM_BOMB },
    /* 9F */
    { ITEM_ODD_POTION, ITEM_BOMBS_10, ITEM_ODD_POTION, ITEM_GAUNTLETS_SILVER, ITEM_GAUNTLETS_SILVER,
      ITEM_GAUNTLETS_SILVER, ITEM_GAUNTLETS_SILVER, ITEM_NUT, ITEM_ODD_POTION, ITEM_GAUNTLETS_SILVER },
    /* A0 */
    { ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK,
      ITEM_STICK },
    /* A1 */
    { ITEM_NUTS_5, ITEM_FROG, ITEM_NUTS_5, ITEM_MASK_KEATON, ITEM_MASK_KEATON, ITEM_MASK_KEATON, ITEM_MASK_KEATON,
      ITEM_NONE, ITEM_NUT, ITEM_MASK_KEATON },
    /* A2 */
    { ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_NUT, ITEM_NUT, ITEM_NUT, ITEM_NUT, ITEM_SEEDS, ITEM_STICK, ITEM_NUT },
    /* A3 */
    { ITEM_NONE, ITEM_BOMBS_10, ITEM_NONE, ITEM_ARROW_ICE, ITEM_ARROW_ICE, ITEM_ARROW_ICE, ITEM_ARROW_ICE,
      ITEM_BOMBS_10, ITEM_NONE, ITEM_ARROW_ICE },
    /* A4 */
    { ITEM_SAW, ITEM_STICK, ITEM_PRESCRIPTION, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_NONE,
      ITEM_STICK },
    /* A5 */
    { ITEM_BOMBS_10, ITEM_MASK_BUNNY, ITEM_BOMBS_10, ITEM_BOMBS_10, ITEM_BOMBS_10, ITEM_BOMBS_10, ITEM_BOMBS_10,
      ITEM_BOW, ITEM_BOMBS_10, ITEM_BOMBS_10 },
    /* A6 */
    { ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK,
      ITEM_STICK },
    /* A7 */
    { ITEM_BOW, ITEM_STICK, ITEM_BOW, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_STICK, ITEM_BOW, ITEM_STICK }
};

// These values are consistent reguardless of version.
static const ItemID sGimItemIdsExt[88] = {
    /* A8 */ ITEM_STICK,
    /* A9 */ ITEM_STICK,
    /* AA */ ITEM_NUT,
    /* AB */ ITEM_NUT,
    /* AC */ ITEM_STICK,
    /* AD */ ITEM_TUNIC_GORON,
    /* AE */ ITEM_STICK,
    /* AF */ ITEM_TUNIC_GORON,
    /* B0 */ ITEM_NONE,
    /* B1 */ ITEM_TUNIC_GORON,
    /* B2 */ ITEM_STICK,
    /* B3 */ ITEM_TUNIC_GORON,
    /* B4 */ ITEM_STICK,
    /* B5 */ ITEM_TUNIC_GORON,
    /* B6 */ ITEM_STICK,
    /* B7 */ ITEM_TUNIC_GORON,
    /* B8 */ ITEM_ARROW_LIGHT,
    /* B9 */ ITEM_ARROW_LIGHT,
    /* BA */ ITEM_POTION_BLUE,
    /* BB */ ITEM_ARROW_LIGHT,
    /* BC */ ITEM_POTION_BLUE,
    /* BD */ ITEM_POTION_BLUE,
    /* BE */ ITEM_BIG_POE,
    /* BF */ ITEM_POTION_BLUE,
    /* C0 */ ITEM_BIG_POE,
    /* C1 */ ITEM_ARROW_LIGHT,
    /* C2 */ ITEM_POTION_BLUE,
    /* C3 */ ITEM_ARROW_LIGHT,
    /* C4 */ ITEM_POTION_BLUE,
    /* C5 */ ITEM_STICK,
    /* C6 */ ITEM_MASK_BUNNY,
    /* C7 */ ITEM_ARROW_FIRE,
    /* C8 */ ITEM_POCKET_CUCCO,
    /* C9 */ ITEM_ARROW_FIRE,
    /* CA */ ITEM_POCKET_CUCCO,
    /* CB */ ITEM_ARROW_FIRE,
    /* CC */ ITEM_POCKET_EGG,
    /* CD */ ITEM_ARROW_FIRE,
    /* CE */ ITEM_POCKET_CUCCO,
    /* CF */ ITEM_ARROW_FIRE,
    /* D0 */ ITEM_STICK,
    /* D1 */ ITEM_SHIELD_HYLIAN,
    /* D2 */ ITEM_STICK,
    /* D3 */ ITEM_TUNIC_GORON,
    /* D4 */ ITEM_STICK,
    /* D5 */ ITEM_TUNIC_KOKIRI,
    /* D6 */ ITEM_STICK,
    /* D7 */ ITEM_TUNIC_GORON,
    /* D8 */ ITEM_STICK,
    /* D9 */ ITEM_TUNIC_KOKIRI,
    /* DA */ ITEM_STICK,
    /* DB */ ITEM_NONE,
    /* DC */ ITEM_NONE,
    /* DD */ ITEM_NONE,
    /* DE */ ITEM_STICK,
    /* DF */ ITEM_STICK,
    /* E0 */ ITEM_NONE,
    /* E1 */ ITEM_NONE,
    /* E2 */ ITEM_STICK,
    /* E3 */ ITEM_STICK,
    /* E4 */ ITEM_STICK,
    /* E5 */ ITEM_STICK,
    /* E6 */ ITEM_NONE,
    /* E7 */ ITEM_NONE,
    /* E8 */ ITEM_STICK,
    /* E9 */ ITEM_ARROW_FIRE,
    /* EA */ ITEM_LETTER_ZELDA,
    /* EB */ ITEM_ARROW_FIRE,
    /* EC */ ITEM_LETTER_ZELDA,
    /* ED */ ITEM_ARROW_FIRE,
    /* EE */ ITEM_POCKET_EGG,
    /* EF */ ITEM_ARROW_FIRE,
    /* F0 */ ITEM_POCKET_EGG,
    /* F1 */ ITEM_ARROW_FIRE,
    /* F2 */ ITEM_LETTER_ZELDA,
    /* F3 */ ITEM_STICK,
    /* F4 */ ITEM_STICK,
    /* F5 */ ITEM_NONE /* variable */,
    /* F6 */ ITEM_STICK,
    /* F7 */ ITEM_SHIELD_HYLIAN,
    /* F8 */ ITEM_STICK,
    /* F9 */ ITEM_STICK,
    /* FA */ ITEM_STICK,
    /* FB */ ITEM_NONE /* variable */,
    /* FC */ ITEM_STICK,
    /* FD */ ITEM_STICK,
    /* FE */ ITEM_STICK,
    /* FF */ ITEM_STICK
};

// Text type seems universal?
static const uint16_t sGimTextIds[128] = {
    /* 80 */ 0x40,
    /* 81 */ 0x15,
    /* 82 */ 0xA4,
    /* 83 */ 0x25,
    /* 84 */ 0xA7,
    /* 85 */ 0x34,
    /* 86 */ 0x81,
    /* 87 */ 0x90,
    /* 88 */ 0xC6,
    /* 89 */ 0x3C,
    /* 8A */ 0x00,
    /* 8B */ 0x00,
    /* 8C */ 0x00,
    /* 8D */ 0xF0,
    /* 8E */ 0x00,
    /* 8F */ 0xBC,
    /* 90 */ 0xCE,
    /* 91 */ 0x04,
    /* 92 */ 0x00,
    /* 93 */ 0xD6,
    /* 94 */ 0x20,
    /* 95 */ 0x25,
    /* 96 */ 0x01,
    /* 97 */ 0x40,
    /* 98 */ 0x19,
    /* 99 */ 0x94,
    /* 9A */ 0x62,
    /* 9B */ 0x1C,
    /* 9C */ 0x01,
    /* 9D */ 0x00,
    /* 9E */ 0xBF,
    /* 9F */ 0x04,
    /* A0 */ 0x4B,
    /* A1 */ 0x04,
    /* A2 */ 0xA4,
    /* A3 */ 0x1C,
    /* A4 */ 0xBD,
    /* A5 */ 0x08,
    /* A6 */ 0x00,
    /* A7 */ 0x00,
    /* A8 */ 0x00,
    /* A9 */ 0x00,
    /* AA */ 0x01,
    /* AB */ 0x01,
    /* AC */ 0x60,
    /* AD */ 0x00,
    /* AE */ 0xDE,
    /* AF */ 0x00,
    /* B0 */ 0x6C,
    /* B1 */ 0x00,
    /* B2 */ 0x10,
    /* B3 */ 0x33,
    /* B4 */ 0x88,
    /* B5 */ 0x00,
    /* B6 */ 0x70,
    /* B7 */ 0x00,
    /* B8 */ 0x67,
    /* B9 */ 0x7C,
    /* BA */ 0x67,
    /* BB */ 0x7C,
    /* BC */ 0x67,
    /* BD */ 0x67,
    /* BE */ 0x7C,
    /* BF */ 0x67,
    /* C0 */ 0x7C,
    /* C1 */ 0x7C,
    /* C2 */ 0x67,
    /* C3 */ 0x7C,
    /* C4 */ 0x67,
    /* C5 */ 0x00,
    /* C6 */ 0x00,
    /* C7 */ 0x38,
    /* C8 */ 0x00,
    /* C9 */ 0x80,
    /* CA */ 0x00,
    /* CB */ 0x98,
    /* CC */ 0x00,
    /* CD */ 0x50,
    /* CE */ 0x00,
    /* CF */ 0x60,
    /* D0 */ 0x70,
    /* D1 */ 0xA6,
    /* D2 */ 0x48,
    /* D3 */ 0x00,
    /* D4 */ 0xD8,
    /* D5 */ 0x00,
    /* D6 */ 0xEC,
    /* D7 */ 0x00,
    /* D8 */ 0x35,
    /* D9 */ 0x00,
    /* DA */ 0x5C,
    /* DB */ 0xED,
    /* DC */ 0x92,
    /* DD */ 0x71,
    /* DE */ 0x56,
    /* DF */ 0xEA,
    /* E0 */ 0x71,
    /* E1 */ 0x5F,
    /* E2 */ 0xEA,
    /* E3 */ 0x0D,
    /* E4 */ 0x56,
    /* E5 */ 0xEA,
    /* E6 */ 0x56,
    /* E7 */ 0xEA,
    /* E8 */ 0x00,
    /* E9 */ 0x18,
    /* EA */ 0x00,
    /* EB */ 0xE0,
    /* EC */ 0x00,
    /* ED */ 0xF8,
    /* EE */ 0x00,
    /* EF */ 0x70,
    /* F0 */ 0x00,
    /* F1 */ 0xC8,
    /* F2 */ 0x00,
    /* F3 */ 0x00,
    /* F4 */ 0x00,
    /* F5 */ 0x00,
    /* F6 */ 0x00,
    /* F7 */ 0x00,
    /* F8 */ 0x00,
    /* F9 */ 0x00,
    /* FA */ 0x00,
    /* FB */ 0x00,
    /* FC */ 0x00,
    /* FD */ 0x00,
    /* FE */ 0x00,
    /* FF */ 0x00
};

extern "C" GetItemEntry Gim_RetrieveOobGetItemEntry(int16_t getItemId) {
    int32_t version = CVarGetInteger(CVAR_ENHANCEMENT("GetItemManipulation"), GIM_DISABLED);

    uint8_t tableOffset = getItemId + 128;
    uint8_t mVersionOffset = 0;

    // SoH widened Player.getItemId from s8 to s16, so values below the console range can
    // exist here; treat them (and the restoration being disabled) as an item table miss.
    if (getItemId < -128)
        return GET_ITEM_NONE;

    switch (version) {
        case GIM_DISABLED:
            return GET_ITEM_NONE;

        case GIM_NTSC_1_0:
            mVersionOffset = 0;
            break;

        case GIM_NTSC_1_1:
            mVersionOffset = 1;
            break;

        case GIM_NTSC_1_2:
            mVersionOffset = 2;
            break;

        case GIM_PAL_1_0:
        case GIM_PAL_1_1:
            mVersionOffset = 3;
            break;

        case GIM_GC_U:
        case GIM_GC_J:
        case GIM_MQ_J:
            mVersionOffset = 4;
            break;

        case GIM_GC_E:
        case GIM_MQ_U:
        case GIM_MQ_E:
            mVersionOffset = 5;
            break;

        case GIM_IQUE_CHN:
        case GIM_IQUE_TWN:
            mVersionOffset = 6;
            break;

        case GIM_MQ_DEBUG:
            mVersionOffset = 7;
            break;

        case GIM_MZX_NTSC:
            mVersionOffset = 8;
            break;

        case GIM_MZX_PAL:
            mVersionOffset = 9;
            break;
    }

    // Only the received itemId of the console OOB read is documented.
    GetItemID giId;
    ItemID itemId;

    // Determine if the item id is universal or unique
    if ((getItemId & 0xFF) <= 0xA7) {
        itemId = sGimItemIdsVer[tableOffset][mVersionOffset];
        giId = RetrieveGetItemIDFromItemID(itemId);
    } else {
        itemId = sGimItemIdsExt[tableOffset - 40];
        giId = RetrieveGetItemIDFromItemID(itemId);
    }

    GetItemEntry giEntry;

    // Safety in case we retrieved an incompatible GI item type
    if (giId == GI_MAX) {
        // We give them no item because we don't have a valid 'get item'.
        // Allow the games original text value to pass through from text table though.
        giEntry = GET_ITEM_NONE;
    } else {
        giEntry = ItemTableManager::Instance->RetrieveItemEntry(MOD_NONE, giId);
    }

    // Fetch the correct textbox to use.
    giEntry.textId = sGimTextIds[tableOffset];

    return giEntry;
}
