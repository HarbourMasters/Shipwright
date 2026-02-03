/**
 * extended_player.c - Extended player item action system
 *
 * Maps custom ITEM_xxx values to PLAYER_IA_xxx actions.
 * Provides model groups, update functions, and init functions for custom items.
 */

#include "extended_player.h"
#include "z64.h"
#include "mods/items/custom_items.h"

// External reference to vanilla arrays
extern int8_t sItemActions[];
extern uint8_t sActionModelGroups[];
extern s32 (*sItemActionUpdateFuncs[])(Player* this, PlayState* play);
extern void (*sItemActionInitFuncs[])(PlayState* play, Player* this);

// External vanilla functions used by custom items
extern s32 func_8083485C(Player* this, PlayState* play);
extern s32 Player_UpperAction_Sword(Player* this, PlayState* play);
extern void Player_InitDefaultIA(PlayState* play, Player* this);

// External custom item upper action functions
extern s32 Player_UpperAction_Beetle(Player* this, PlayState* play);
extern s32 Player_UpperAction_BombArrows(Player* this, PlayState* play);
extern s32 Player_UpperAction_CaneOfSomaria(Player* this, PlayState* play);
extern s32 Player_UpperAction_DekuLeaf(Player* this, PlayState* play);

// External custom item init functions (not declared in custom_items.h)
extern void Player_InitHyliasGraceIA(PlayState* play, Player* this);
extern void Player_InitZonaiPermafrostIA(PlayState* play, Player* this);
extern void Player_InitSwitchHookIA(PlayState* play, Player* this);
extern void Player_InitMogmaMittsIA(PlayState* play, Player* this);
extern void Player_InitWhipIA(PlayState* play, Player* this);
extern void Player_InitDominionRodIA(PlayState* play, Player* this);
extern void Player_InitTimeGateIA(PlayState* play, Player* this);
extern void Player_InitPending1IA(PlayState* play, Player* this);
extern void Player_InitPending2IA(PlayState* play, Player* this);
extern void Player_InitPending3IA(PlayState* play, Player* this);

/**
 * Get the PLAYER_IA_xxx value for a given ITEM_xxx value.
 * Uses switch statement for custom items (following extended_inventory pattern).
 */
int8_t ExtPlayer_GetItemAction(int32_t item) {
    // Handle special cases first
    if (item >= ITEM_NONE_FE) {
        return PLAYER_IA_NONE;
    }
    if (item == ITEM_LAST_USED) {
        return PLAYER_IA_SWORD_CS;
    }
    if (item == ITEM_FISHING_POLE) {
        return PLAYER_IA_FISHING_POLE;
    }

    // Handle items beyond the vanilla sItemActions array with switch
    switch (item) {
        // Items 56-61 (bow combos and swords - originally in expanded vanilla array)
        case ITEM_BOW_ARROW_FIRE:     return PLAYER_IA_BOW_FIRE;
        case ITEM_BOW_ARROW_ICE:      return PLAYER_IA_BOW_ICE;
        case ITEM_BOW_ARROW_LIGHT:    return PLAYER_IA_BOW_LIGHT;
        case ITEM_SWORD_KOKIRI:       return PLAYER_IA_SWORD_KOKIRI;
        case ITEM_SWORD_MASTER:       return PLAYER_IA_SWORD_MASTER;
        case ITEM_SWORD_BGS:          return PLAYER_IA_SWORD_BIGGORON;

        // Custom items (0x9C - 0xB5)
        case ITEM_ROCS_FEATHER:       return PLAYER_IA_ROCS_FEATHER;
        case ITEM_ROCS_CAPE:          return PLAYER_IA_ROCS_CAPE;
        case ITEM_DESIRE_SENSOR:         return PLAYER_IA_DESIRE_SENSOR;
        case ITEM_HYLIAS_GRACE:       return PLAYER_IA_HYLIAS_GRACE;
        case ITEM_ZONAI_PERMAFROST:   return PLAYER_IA_ZONAI_PERMAFROST;
        case ITEM_DEMISE_DESTRUCTION: return PLAYER_IA_DEMISE_DESTRUCTION;
        case ITEM_DEKU_LEAF:          return PLAYER_IA_DEKU_LEAF;
        case ITEM_SWITCH_HOOK:     return PLAYER_IA_SWITCH_HOOK;
        case ITEM_MOGMA_MITTS:        return PLAYER_IA_MOGMA_MITTS;
        case ITEM_GUST_JAR:           return PLAYER_IA_GUST_JAR;
        case ITEM_BALL_AND_CHAIN:     return PLAYER_IA_BALL_AND_CHAIN;
        case ITEM_WHIP:               return PLAYER_IA_WHIP;
        case ITEM_SPINNER:            return PLAYER_IA_SPINNER;
        case ITEM_CANE_OF_SOMARIA:     return PLAYER_IA_CANE_OF_SOMARIA;
        case ITEM_DOMINION_ROD:       return PLAYER_IA_DOMINION_ROD;
        case ITEM_TIME_GATE:          return PLAYER_IA_TIME_GATE;
        case ITEM_BOMB_ARROWS:        return PLAYER_IA_BOMB_ARROWS;
        case ITEM_ROD_FIRE:           return PLAYER_IA_ROD_FIRE;
        case ITEM_ROD_ICE:            return PLAYER_IA_ROD_ICE;
        case ITEM_ROD_LIGHT:          return PLAYER_IA_ROD_LIGHT;
        case ITEM_BEETLE:             return PLAYER_IA_BEETLE;
        case ITEM_SHOVEL:             return PLAYER_IA_SHOVEL;
        case ITEM_PENDING_1:          return PLAYER_IA_PENDING_1;
        case ITEM_PENDING_2:          return PLAYER_IA_PENDING_2;
        case ITEM_PENDING_3:          return PLAYER_IA_PENDING_3;
        default:
            break;
    }

    // For vanilla items, use the original array if within bounds
    if (item < VANILLA_SITEMACTIONS_SIZE) {
        return sItemActions[item];
    }

    // For items in the gap (equipment, songs, quest items, etc.), return NONE
    return PLAYER_IA_NONE;
}

/**
 * Get the model group for a given PLAYER_IA_xxx value.
 * Uses switch statement for custom item actions.
 */
uint8_t ExtPlayer_GetActionModelGroup(int32_t itemAction) {
    // Handle custom item actions with switch
    switch (itemAction) {
        case PLAYER_IA_ROCS_FEATHER:       return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_ROCS_CAPE:          return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_DESIRE_SENSOR:         return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_HYLIAS_GRACE:       return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_ZONAI_PERMAFROST:   return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_DEMISE_DESTRUCTION: return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_DEKU_LEAF:          return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_SWITCH_HOOK:     return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_MOGMA_MITTS:        return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_GUST_JAR:           return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_BALL_AND_CHAIN:     return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_WHIP:               return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_SPINNER:            return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_CANE_OF_SOMARIA:     return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_DOMINION_ROD:       return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_TIME_GATE:          return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_BOMB_ARROWS:        return PLAYER_MODELGROUP_DEFAULT;
        // Rods use BGS (two-handed) model group for proper sword animations
        case PLAYER_IA_ROD_FIRE:           return PLAYER_MODELGROUP_BGS;
        case PLAYER_IA_ROD_ICE:            return PLAYER_MODELGROUP_BGS;
        case PLAYER_IA_ROD_LIGHT:          return PLAYER_MODELGROUP_BGS;
        case PLAYER_IA_BEETLE:             return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_SHOVEL:             return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_PENDING_1:          return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_PENDING_2:          return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_UNUSED_5B:          return PLAYER_MODELGROUP_DEFAULT;
        case PLAYER_IA_PENDING_3:          return PLAYER_MODELGROUP_DEFAULT;
        default:
            break;
    }

    // For vanilla item actions, use the original array if within bounds
    if (itemAction < VANILLA_PLAYER_IA_COUNT) {
        return sActionModelGroups[itemAction];
    }

    return PLAYER_MODELGROUP_DEFAULT;
}

/**
 * Get the update function for a given PLAYER_IA_xxx value.
 * Uses switch statement for custom item actions.
 */
ItemActionUpdateFunc ExtPlayer_GetItemActionUpdateFunc(int32_t itemAction) {
    // Handle custom item actions with switch
    switch (itemAction) {
        case PLAYER_IA_ROCS_FEATHER:       return func_8083485C;
        case PLAYER_IA_ROCS_CAPE:          return func_8083485C;
        case PLAYER_IA_DESIRE_SENSOR:         return func_8083485C;
        case PLAYER_IA_HYLIAS_GRACE:       return func_8083485C;
        case PLAYER_IA_ZONAI_PERMAFROST:   return func_8083485C;
        case PLAYER_IA_DEMISE_DESTRUCTION: return func_8083485C;
        case PLAYER_IA_DEKU_LEAF:          return Player_UpperAction_DekuLeaf;
        case PLAYER_IA_SWITCH_HOOK:     return func_8083485C;
        case PLAYER_IA_MOGMA_MITTS:        return func_8083485C;
        case PLAYER_IA_GUST_JAR:           return func_8083485C;
        case PLAYER_IA_BALL_AND_CHAIN:     return func_8083485C;
        case PLAYER_IA_WHIP:               return func_8083485C;
        case PLAYER_IA_SPINNER:            return func_8083485C;
        case PLAYER_IA_CANE_OF_SOMARIA:     return Player_UpperAction_CaneOfSomaria;
        case PLAYER_IA_DOMINION_ROD:       return func_8083485C;
        case PLAYER_IA_TIME_GATE:          return func_8083485C;
        case PLAYER_IA_BOMB_ARROWS:        return Player_UpperAction_BombArrows;
        // Rods use sword mechanics for charge attacks
        case PLAYER_IA_ROD_FIRE:           return Player_UpperAction_Sword;
        case PLAYER_IA_ROD_ICE:            return Player_UpperAction_Sword;
        case PLAYER_IA_ROD_LIGHT:          return Player_UpperAction_Sword;
        case PLAYER_IA_BEETLE:             return Player_UpperAction_Beetle;
        case PLAYER_IA_SHOVEL:             return func_8083485C;
        case PLAYER_IA_PENDING_1:          return func_8083485C;
        case PLAYER_IA_PENDING_2:          return func_8083485C;
        case PLAYER_IA_UNUSED_5B:          return func_8083485C;
        case PLAYER_IA_PENDING_3:          return func_8083485C;
        default:
            break;
    }

    // For vanilla item actions, use the original array if within bounds
    if (itemAction < VANILLA_PLAYER_IA_COUNT) {
        return sItemActionUpdateFuncs[itemAction];
    }

    return func_8083485C;
}

/**
 * Get the init function for a given PLAYER_IA_xxx value.
 * Uses switch statement for custom item actions.
 */
ItemActionInitFunc ExtPlayer_GetItemActionInitFunc(int32_t itemAction) {
    // Handle custom item actions with switch
    switch (itemAction) {
        case PLAYER_IA_ROCS_FEATHER:       return Player_InitDefaultIA;
        case PLAYER_IA_ROCS_CAPE:          return Player_InitDefaultIA;
        case PLAYER_IA_DESIRE_SENSOR:         return Player_InitDefaultIA;
        case PLAYER_IA_HYLIAS_GRACE:       return Player_InitHyliasGraceIA;
        case PLAYER_IA_ZONAI_PERMAFROST:   return Player_InitZonaiPermafrostIA;
        case PLAYER_IA_DEMISE_DESTRUCTION: return Player_InitDemiseDestructionIA;
        case PLAYER_IA_DEKU_LEAF:          return Player_InitDefaultIA;
        case PLAYER_IA_SWITCH_HOOK:     return Player_InitSwitchHookIA;
        case PLAYER_IA_MOGMA_MITTS:        return Player_InitMogmaMittsIA;
        case PLAYER_IA_GUST_JAR:           return Player_InitGustJarIA;
        case PLAYER_IA_BALL_AND_CHAIN:     return Player_InitBallAndChainIA;
        case PLAYER_IA_WHIP:               return Player_InitWhipIA;
        case PLAYER_IA_SPINNER:            return Player_InitSpinnerIA;
        case PLAYER_IA_CANE_OF_SOMARIA:     return Player_InitCaneOfSomariaIA;
        case PLAYER_IA_DOMINION_ROD:       return Player_InitDominionRodIA;
        case PLAYER_IA_TIME_GATE:          return Player_InitTimeGateIA;
        case PLAYER_IA_BOMB_ARROWS:        return Player_InitBombArrowsIA;
        case PLAYER_IA_ROD_FIRE:           return Player_InitFireRodIA;
        case PLAYER_IA_ROD_ICE:            return Player_InitIceRodIA;
        case PLAYER_IA_ROD_LIGHT:          return Player_InitLightRodIA;
        case PLAYER_IA_BEETLE:             return Player_InitBeetleIA;
        case PLAYER_IA_SHOVEL:             return Player_InitDefaultIA;
        case PLAYER_IA_PENDING_1:          return Player_InitPending1IA;
        case PLAYER_IA_PENDING_2:          return Player_InitPending2IA;
        case PLAYER_IA_UNUSED_5B:          return Player_InitDefaultIA;
        case PLAYER_IA_PENDING_3:          return Player_InitPending3IA;
        default:
            break;
    }

    // For vanilla item actions, use the original array if within bounds
    if (itemAction < VANILLA_PLAYER_IA_COUNT) {
        return sItemActionInitFuncs[itemAction];
    }

    return Player_InitDefaultIA;
}
