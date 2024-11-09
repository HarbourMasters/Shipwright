#include "BossRush.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#include <array>
#include <string>
#include <vector>

extern "C" {
    #include "functions.h"
    #include "macros.h"
    #include "variables.h"
    #include "src/overlays/actors/ovl_Boss_Goma/z_boss_goma.h"
    #include "src/overlays/actors/ovl_Boss_Mo/z_boss_mo.h"
    #include "src/overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
    extern PlayState* gPlayState;

    Gfx* KaleidoScope_QuadTextureIA8(Gfx* gfx, void* texture, s16 width, s16 height, u16 point);
    #include "textures/icon_item_nes_static/icon_item_nes_static.h"
    #include "textures/icon_item_ger_static/icon_item_ger_static.h"
    #include "textures/icon_item_fra_static/icon_item_fra_static.h"
}

typedef struct BossRushSetting {
    std::array<std::string, LANGUAGE_MAX> name;
    std::vector<std::array<std::string, LANGUAGE_MAX>> choices;
} BossRushSetting;

BossRushSetting BossRushOptions[BR_OPTIONS_MAX] = { 
    {
        { "BOSSES:", "BOSSE:", "BOSS:" },
        {
            { "All", "Alle", "Tous" },
            { "Child", "Kind", "Enfant" },
            { "Adult", "Erwachsener", "Adulte" },
            { "Ganondorf & Ganon", "Ganondorf & Ganon", "Ganondorf & Ganon" }
        }
    },
    {
        { "HEARTS:", "HERZEN:", "COEURS:" },
        {
            { "10", "10", "10" },
            { "15", "15", "15" },
            { "20", "20", "20" },
            { "3", "3", "3" },
            { "5", "5", "5" },
            { "7", "7", "7" }
        }
    },
    {
        { "AMMO:", "MUNITION:", "MUNITIONS:" },
        {
            { "Limited", "Limitiert", "Limitées" },
            { "Full", "Voll", "Pleines" },
            { "Maxed", "Maximum", "Maximum" }
        }
    },
    {
        { "HEAL:", "REGENERATION:", "SOIN:" },
        {
            { "Before Ganondorf", "Vor Ganondorf", "Avant Ganondorf" },
            { "Every Boss", "Bei jedem Boss", "Tous les Boss" },
            { "Never", "Niemals", "Jamais" }
        }
    },
    {
        { "HYPER BOSSES:", "HYPER-BOSSE:", "HYPER BOSS:" },
        {
            { "No", "Nein", "Non" },
            { "Yes", "Ja", "Oui" }
        }
    },
    {
        { "MAGIC:", "MAGIE:", "MAGIE:" },
        {
            { "Single", "Einzel", "Simple" },
            { "Double", "Doppel", "Double" }
        }
    },
    {
        { "BIG. SWORD:", "BIG.-SCHWERT:", "EPÉE DE BIG.:" },
        {
            { "No", "Nein", "Non" },
            { "Yes", "Ja", "Oui" }
        }
    },
    {
        { "BOTTLE:", "FLASCHEN:", "BOUTEILLE:" },
        {
            { "No", "Nein", "Non" },
            { "Empty", "Leer", "Vide" },
            { "Fairy", "Fee", "Fée" },
            { "Red Potion", "Rotes Elixier", "Potion Rouge" },
            { "Green Potion", "Grünes Elixier", "Potion Verte" },
            { "Blue Potion", "Blaues Elixier", "Potion Bleue" }
        }
    },
    {
        { "LONGSHOT:", "ENTERHAKEN:", "SUPER GRAPPIN:" },
        {
            { "No", "Nein", "Non" },
            { "Yes", "Ja", "Oui" }
        }
    },
    {
        { "HOVER BOOTS:", "GLEITSTIEFEL:", "BOTTES DES AIRS:" },
        {
            { "No", "Nein", "Non" },
            { "Yes", "Ja", "Oui" }
        }
    },
    {
        { "BUNNY HOOD:", "HASENOHREN:", "MASQUE DU LAPIN:" },
        {
            { "No", "Nein", "Non" },
            { "Yes", "Ja", "Oui" }
        }
    },
    {
        { "TIMER:", "TIMER:", "TIMER:" },
        {
            { "Yes", "Ja", "Oui" },
            { "No", "Nein", "Non" }
        }
    }
};

const char* BossRush_GetSettingName(u8 optionIndex, u8 language) {
    return BossRushOptions[optionIndex].name[language].c_str();
}

const char* BossRush_GetSettingChoiceName(u8 optionIndex, u8 choiceIndex, u8 language) {
    return BossRushOptions[optionIndex].choices[choiceIndex][language].c_str();
}

u8 BossRush_GetSettingOptionsAmount(u8 optionIndex) {
    return BossRushOptions[optionIndex].choices.size();
}

void BossRush_SpawnBlueWarps(PlayState* play) {

    // Spawn blue warps in Chamber of Sages based on what bosses have been defeated.
    if (gSaveContext.linkAge == LINK_AGE_CHILD) {
        // Forest Medallion (Gohma)
        if (!Flags_GetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, -100, 6, -170, 0, 0, 0, -1, false);
        }
        // Fire Medallion (King Dodongo)
        if (!Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, 100, 6, -170, 0, 0, 0, -1, false);
        }
        // Water Medallion (Barinade)
        if (!Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, 199, 6, 0, 0, 0, 0, -1, false);
        }
    } else {
        // Light Medallion (Ganondorf)
        if (CheckDungeonCount() == 8) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, -199, 6, 0, 0, 0, 0, -1, false);
        }
        // Forest Medallion (Phantom Ganondorf)
        if (!Flags_GetEventChkInf(EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, -100, 6, -170, 0, 0, 0, -1, false);
        }
        // Fire Medallion (Volvagia)
        if (!Flags_GetEventChkInf(EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, 100, 6, -170, 0, 0, 0, -1, false);
        }
        // Water Medallion (Morpha)
        if (!Flags_GetEventChkInf(EVENTCHKINF_USED_WATER_TEMPLE_BLUE_WARP)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, 199, 6, 0, 0, 0, 0, -1, false);
        }
        // Spirit Medallion (Twinrova)
        if (!Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_SPIRIT_TEMPLE)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, 100, 6, 170, 0, 0, 0, -1, false);
        }
        // Shadow Medallion (Bongo Bongo)
        if (!Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_SHADOW_TEMPLE)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, -100, 6, 170, 0, 0, 0, -1, false);
        }
    }
}

void BossRush_SetEquipment(u8 linkAge) {
    std::array<u8, 8> brButtonItems;
    std::array<u8, 7> brCButtonSlots;

    // Set Child Equipment.
    if (linkAge == LINK_AGE_CHILD) {
        brButtonItems = {
            ITEM_SWORD_KOKIRI, ITEM_STICK, ITEM_NUT, ITEM_BOMB, ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE
        };

        brCButtonSlots = { SLOT_STICK, SLOT_NUT, SLOT_BOMB, SLOT_NONE, SLOT_NONE, SLOT_NONE, SLOT_NONE };

        Inventory_ChangeEquipment(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_KOKIRI);
        Inventory_ChangeEquipment(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_DEKU);
    // Set Adult equipment.
    } else {
        brButtonItems = { ITEM_SWORD_MASTER, ITEM_BOW,  ITEM_HAMMER, ITEM_BOMB,
                          ITEM_NONE,         ITEM_NONE, ITEM_NONE,   ITEM_NONE };

        brCButtonSlots = { SLOT_BOW, SLOT_HAMMER, SLOT_BOMB, SLOT_NONE, SLOT_NONE, SLOT_NONE, SLOT_NONE };

        Inventory_ChangeEquipment(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_MASTER);
        Inventory_ChangeEquipment(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_MIRROR);
        Inventory_ChangeEquipment(EQUIP_TYPE_TUNIC, EQUIP_VALUE_TUNIC_GORON);
    }

    // Button Items
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.equips.buttonItems); button++) {
        gSaveContext.equips.buttonItems[button] = brButtonItems[button];
    }

    // C buttons
    for (int button = 0; button < ARRAY_COUNT(gSaveContext.equips.cButtonSlots); button++) {
        gSaveContext.equips.cButtonSlots[button] = brCButtonSlots[button];
    }
}

void BossRush_HandleBlueWarp(PlayState* play, f32 warpPosX, f32 warpPosZ) {
    // If warping from Chamber of Sages, choose the correct boss room to teleport to.
    if (play->sceneNum == SCENE_CHAMBER_OF_THE_SAGES) {
        // Gohma & Phantom Ganon
        if (warpPosX == -100 && warpPosZ == -170) {
            if (gSaveContext.linkAge == LINK_AGE_CHILD) {
                play->nextEntranceIndex = ENTR_DEKU_TREE_BOSS_ENTRANCE;
            } else {
                play->nextEntranceIndex = ENTR_FOREST_TEMPLE_BOSS_ENTRANCE;
            }
        // King Dodongo & Volvagia
        } else if (warpPosX == 100 && warpPosZ == -170) {
            if (gSaveContext.linkAge == LINK_AGE_CHILD) {
                play->nextEntranceIndex = ENTR_DODONGOS_CAVERN_BOSS_ENTRANCE;
            } else {
                play->nextEntranceIndex = ENTR_FIRE_TEMPLE_BOSS_ENTRANCE;
            }
        // Barinade & Morb
        } else if (warpPosX == 199 && warpPosZ == 0) {
            if (gSaveContext.linkAge == LINK_AGE_CHILD) {
                play->nextEntranceIndex = ENTR_JABU_JABU_BOSS_ENTRANCE;
            } else {
                play->nextEntranceIndex = ENTR_WATER_TEMPLE_BOSS_ENTRANCE;
            }
        // Twinrova
        } else if (warpPosX == 100 && warpPosZ == 170) {
            play->nextEntranceIndex = ENTR_SPIRIT_TEMPLE_BOSS_2;
        // Bongo Bongo
        } else if (warpPosX == -100 && warpPosZ == 170) {
            play->nextEntranceIndex = ENTR_SHADOW_TEMPLE_BOSS_ENTRANCE;
        // Ganondork
        } else if (warpPosX == -199 && warpPosZ == 0) {
            play->nextEntranceIndex = ENTR_GANONDORF_BOSS_0;
        } else {
            SPDLOG_ERROR("[BossRush]: Unknown blue warp in chamber of sages at position ({}, {}). Warping back to chamber of sages.", warpPosX, warpPosZ);
            play->nextEntranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;
        }
    // If coming from a boss room, teleport back to Chamber of Sages and set flag.
    } else {
        play->nextEntranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;

        if (CheckDungeonCount() == 3) {
            play->linkAgeOnLoad = LINK_AGE_ADULT;
            gSaveContext.linkAge = LINK_AGE_ADULT;

            // Change to Adult Link.
            if (gSaveContext.bossRushOptions[BR_OPTIONS_BOSSES] == BR_CHOICE_BOSSES_ALL) {
                BossRush_SetEquipment(LINK_AGE_ADULT);
            // Warp to credits.
            } else if (gSaveContext.bossRushOptions[BR_OPTIONS_BOSSES] == BR_CHOICE_BOSSES_CHILD) {
                play->nextEntranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;
                gSaveContext.nextCutsceneIndex = 0xFFF2;
                play->transitionTrigger = TRANS_TRIGGER_START;
                play->transitionType = TRANS_TYPE_FADE_WHITE;
            }
        }
    }

    play->transitionTrigger = TRANS_TRIGGER_START;
    play->transitionType = TRANS_TYPE_FADE_WHITE;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE_SLOW;
}

void BossRush_HandleBlueWarpHeal(PlayState* play) {

    // This function gets called multiple times per blue warp, so only heal when player isn't at max HP.
    if (gSaveContext.bossRushOptions[BR_OPTIONS_HEAL] == BR_CHOICE_HEAL_EVERYBOSS &&
        gSaveContext.health != gSaveContext.healthCapacity) {
        Health_ChangeBy(play, 320);
    }
}

void BossRush_HandleCompleteBoss(PlayState* play) {
    gSaveContext.isBossRushPaused = 1;
    switch (play->sceneNum) {
        case SCENE_DEKU_TREE_BOSS:
            Flags_SetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP);
            break;
        case SCENE_DODONGOS_CAVERN_BOSS:
            Flags_SetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP);
            break;
        case SCENE_JABU_JABU_BOSS:
            Flags_SetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP);
            break;
        case SCENE_FOREST_TEMPLE_BOSS:
            Flags_SetEventChkInf(EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP);
            break;
        case SCENE_FIRE_TEMPLE_BOSS:
            Flags_SetEventChkInf(EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP);
            break;
        case SCENE_WATER_TEMPLE_BOSS:
            Flags_SetEventChkInf(EVENTCHKINF_USED_WATER_TEMPLE_BLUE_WARP);
            break;
        case SCENE_SPIRIT_TEMPLE_BOSS:
            Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_SPIRIT_TEMPLE);
            break;
        case SCENE_SHADOW_TEMPLE_BOSS:
            Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_SHADOW_TEMPLE);
            break;
        default:
            break;
    }

    // Fully heal the player after Ganondorf
    if (gSaveContext.bossRushOptions[BR_OPTIONS_HEAL] == BR_CHOICE_HEAL_EVERYBOSS &&
        play->sceneNum == SCENE_GANONDORF_BOSS) {
        Health_ChangeBy(play, 320);
    }

    if ((CheckDungeonCount() == 3 && gSaveContext.bossRushOptions[BR_OPTIONS_BOSSES] == BR_CHOICE_BOSSES_CHILD) ||
        play->sceneNum == SCENE_GANON_BOSS) {
        gSaveContext.sohStats.playTimer += 2;
        gSaveContext.sohStats.gameComplete = 1;
        gSaveContext.sohStats.itemTimestamp[TIMESTAMP_BOSSRUSH_FINISH] = GAMEPLAYSTAT_TOTAL_TIME;
    }
}

void BossRush_InitSave() {

    // Set player name to Lonk for the few textboxes that show up during Boss Rush. Player can't input their own name.
    std::array<char, 8> brPlayerName = { 21, 50, 49, 46, 62, 62, 62, 62 };
    for (int i = 0; i < ARRAY_COUNT(gSaveContext.playerName); i++) {
        gSaveContext.playerName[i] = brPlayerName[i];
    }

    gSaveContext.questId = QUEST_BOSSRUSH;
    gSaveContext.isBossRushPaused = 1;
    gSaveContext.entranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;
    gSaveContext.cutsceneIndex = 0x8000;
    gSaveContext.isMagicAcquired = 1;

    // Set magic
    if (gSaveContext.bossRushOptions[BR_OPTIONS_MAGIC] == BR_CHOICE_MAGIC_SINGLE) {
        gSaveContext.magicLevel = 1;
        gSaveContext.magic = 48;
    } else {
        gSaveContext.isDoubleMagicAcquired = 1;
        gSaveContext.magicLevel = 2;
        gSaveContext.magic = 96;
    }

    // Set health
    u16 health = 16;
    switch (gSaveContext.bossRushOptions[BR_OPTIONS_HEARTS]) { 
        case BR_CHOICE_HEARTS_7:
            health *= 7;
            break;
        case BR_CHOICE_HEARTS_10:
            health *= 10;
            break;
        case BR_CHOICE_HEARTS_15:
            health *= 15;
            break;
        case BR_CHOICE_HEARTS_20:
            health *= 20;
            break;
        case BR_CHOICE_HEARTS_3:
            health *= 3;
            break;
        case BR_CHOICE_HEARTS_5:
            health *= 5;
            break;
        default:
            break;
    }

    gSaveContext.healthCapacity = health;
    gSaveContext.health = health;

    // Skip boss cutscenes
    gSaveContext.eventChkInf[7] |= 1;    // gohma
    gSaveContext.eventChkInf[7] |= 2;    // dodongo
    gSaveContext.eventChkInf[7] |= 4;    // phantom ganon
    gSaveContext.eventChkInf[7] |= 8;    // volvagia
    gSaveContext.eventChkInf[7] |= 0x10; // morpha
    gSaveContext.eventChkInf[7] |= 0x20; // twinrova
    gSaveContext.eventChkInf[7] |= 0x40; // barinade
    gSaveContext.eventChkInf[7] |= 0x80; // bongo bongo

    // Sets all rando flags to false
    for (s32 i = 0; i < ARRAY_COUNT(gSaveContext.randomizerInf); i++) {
        gSaveContext.randomizerInf[i] = 0;
    }

    // Set items
    std::array<u8, 24> brItems = {
        ITEM_STICK,     ITEM_NUT,  ITEM_BOMB, ITEM_BOW,      ITEM_NONE,        ITEM_NONE,
        ITEM_SLINGSHOT, ITEM_NONE, ITEM_NONE, ITEM_HOOKSHOT, ITEM_NONE,        ITEM_NONE,
        ITEM_BOOMERANG, ITEM_LENS, ITEM_NONE, ITEM_HAMMER,   ITEM_ARROW_LIGHT, ITEM_NONE,
        ITEM_NONE,      ITEM_NONE, ITEM_NONE, ITEM_NONE,     ITEM_NONE,        ITEM_NONE,
    };

    if (gSaveContext.bossRushOptions[BR_OPTIONS_LONGSHOT] == BR_CHOICE_LONGSHOT_YES) {
        brItems[9] = ITEM_LONGSHOT;
    }

    switch (gSaveContext.bossRushOptions[BR_OPTIONS_BOTTLE]) {
        case BR_CHOICE_BOTTLE_EMPTY:
            brItems[18] = ITEM_BOTTLE;
            break;
        case BR_CHOICE_BOTTLE_FAIRY:
            brItems[18] = ITEM_FAIRY;
            break;
        case BR_CHOICE_BOTTLE_REDPOTION:
            brItems[18] = ITEM_POTION_RED;
            break;
        case BR_CHOICE_BOTTLE_GREENPOTION:
            brItems[18] = ITEM_POTION_GREEN;
            break;
        case BR_CHOICE_BOTTLE_BLUEPOTION:
            brItems[18] = ITEM_POTION_BLUE;
            break;
        default:
            break;
    }

    if (gSaveContext.bossRushOptions[BR_OPTIONS_BUNNYHOOD] == BR_CHOICE_BUNNYHOOD_YES) {
        brItems[23] = ITEM_MASK_BUNNY;
    }

    for (int item = 0; item < ARRAY_COUNT(gSaveContext.inventory.items); item++) {
        gSaveContext.inventory.items[item] = brItems[item];
    }

    // Set consumable counts
    std::array<s8, 16> brAmmo = { 5, 5, 10, 10, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    if (gSaveContext.bossRushOptions[BR_OPTIONS_AMMO] == BR_CHOICE_AMMO_FULL) {
        brAmmo = { 10, 20, 20, 30, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    } else if (gSaveContext.bossRushOptions[BR_OPTIONS_AMMO] == BR_CHOICE_AMMO_MAXED) {
        brAmmo = { 30, 40, 40, 50, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    }

    for (int ammo = 0; ammo < ARRAY_COUNT(gSaveContext.inventory.ammo); ammo++) {
        gSaveContext.inventory.ammo[ammo] = brAmmo[ammo];
    }

    // Equipment
    gSaveContext.inventory.equipment |= 1 << 0; // Kokiri Sword
    gSaveContext.inventory.equipment |= 1 << 1; // Master Sword
    gSaveContext.inventory.equipment |= 1 << 4; // Deku Shield
    gSaveContext.inventory.equipment |= 1 << 6; // Mirror Shield
    gSaveContext.inventory.equipment |= 1 << 9; // Goron Tunic
    if (gSaveContext.bossRushOptions[BR_OPTIONS_BGS] == BR_CHOICE_BGS_YES) {
        gSaveContext.inventory.equipment |= 1 << 2; // Biggoron Sword
        gSaveContext.bgsFlag = 1;
    }
    if (gSaveContext.bossRushOptions[BR_OPTIONS_HOVERBOOTS] == BR_CHOICE_HOVERBOOTS_YES) {
        gSaveContext.inventory.equipment |= 1 << 14; // Hover Boots
    }

    // Upgrades
    u8 upgradeLevel = 1;
    if (gSaveContext.bossRushOptions[BR_OPTIONS_AMMO] == BR_CHOICE_AMMO_MAXED) {
        upgradeLevel = 3;
    }
    Inventory_ChangeUpgrade(UPG_QUIVER, upgradeLevel);
    Inventory_ChangeUpgrade(UPG_BOMB_BAG, upgradeLevel);
    Inventory_ChangeUpgrade(UPG_BULLET_BAG, upgradeLevel);
    Inventory_ChangeUpgrade(UPG_STICKS, upgradeLevel);
    Inventory_ChangeUpgrade(UPG_NUTS, upgradeLevel);
    Inventory_ChangeUpgrade(UPG_STRENGTH, 1);

    // Set flags and Link's age based on chosen settings.
    if (gSaveContext.bossRushOptions[BR_OPTIONS_BOSSES] == BR_CHOICE_BOSSES_ADULT ||
        gSaveContext.bossRushOptions[BR_OPTIONS_BOSSES] == BR_CHOICE_BOSSES_GANONDORF_GANON) {
        Flags_SetEventChkInf(EVENTCHKINF_USED_DEKU_TREE_BLUE_WARP);
        Flags_SetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP);
        Flags_SetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP);
        if (gSaveContext.bossRushOptions[BR_OPTIONS_BOSSES] == BR_CHOICE_BOSSES_GANONDORF_GANON) {
            Flags_SetEventChkInf(EVENTCHKINF_USED_FOREST_TEMPLE_BLUE_WARP);
            Flags_SetEventChkInf(EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP);
            Flags_SetEventChkInf(EVENTCHKINF_USED_WATER_TEMPLE_BLUE_WARP);
            Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_SPIRIT_TEMPLE);
            Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_SHADOW_TEMPLE);
        }
        gSaveContext.linkAge = LINK_AGE_ADULT;
        BossRush_SetEquipment(LINK_AGE_ADULT);
    } else {
        gSaveContext.linkAge = LINK_AGE_CHILD;
        BossRush_SetEquipment(LINK_AGE_CHILD);
    }
}

static void* sSavePromptNoChoiceTexs[] = {
    (void*)gPauseNoENGTex,
    (void*)gPauseNoGERTex,
    (void*)gPauseNoFRATex
};

void BossRush_OnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs) {
    va_list args;
    va_copy(args, originalArgs);

    switch (id) {
        // Allow not healing before ganon
        case VB_GANON_HEAL_BEFORE_FIGHT: {
            if (gSaveContext.bossRushOptions[BR_OPTIONS_HEAL] == BR_CHOICE_HEAL_NEVER) {
                *should = false;
            }
            break;
        }
        // Replace the blue warp transitions with ones that lead back to the chamber of sages
        case VB_BLUE_WARP_APPLY_ENTRANCE_AND_CUTSCENE: {
            DoorWarp1* blueWarp = va_arg(args, DoorWarp1*);
            BossRush_HandleBlueWarp(gPlayState, blueWarp->actor.world.pos.x, blueWarp->actor.world.pos.z);
            *should = false;
            break;
        }
        // Spawn clean blue warps (no ruto, adult animation, etc)
        case VB_SPAWN_BLUE_WARP: {
            switch (gPlayState->sceneNum) {
                case SCENE_DEKU_TREE_BOSS: {
                    BossGoma* bossGoma = va_arg(args, BossGoma*);
                    static Vec3f roomCenter = { -150.0f, 0.0f, -350.0f };
                    Vec3f childPos = roomCenter;

                    for (s32 i = 0; i < 10000; i++) {
                        if ((fabsf(childPos.x - GET_PLAYER(gPlayState)->actor.world.pos.x) < 100.0f &&
                             fabsf(childPos.z - GET_PLAYER(gPlayState)->actor.world.pos.z) < 100.0f) ||
                            (fabsf(childPos.x - bossGoma->actor.world.pos.x) < 150.0f &&
                             fabsf(childPos.z - bossGoma->actor.world.pos.z) < 150.0f)) {
                            childPos.x = Rand_CenteredFloat(400.0f) + -150.0f;
                            childPos.z = Rand_CenteredFloat(400.0f) + -350.0f;
                        } else {
                            break;
                        }
                    }
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, childPos.x, bossGoma->actor.world.pos.y, childPos.z, 0, 0, 0, WARP_DUNGEON_ADULT, false);
                    break;
                }
                case SCENE_DODONGOS_CAVERN_BOSS: {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, -890.0f, -1523.76f, -3304.0f, 0, 0, 0, WARP_DUNGEON_ADULT, false);
                    break;
                }
                case SCENE_JABU_JABU_BOSS: {
                    static Vec3f sWarpPos[] = {
                        { 10.0f, 0.0f, 30.0f },
                        { 260.0f, 0.0f, -470.0f },
                        { -240.0f, 0.0f, -470.0f },
                    };

                    s32 sp7C = 2;
                    for (s32 i = 2; i > 0; i -= 1) {
                        if (Math_Vec3f_DistXYZ(&sWarpPos[i], &GET_PLAYER(gPlayState)->actor.world.pos) <
                            Math_Vec3f_DistXYZ(&sWarpPos[i - 1], &GET_PLAYER(gPlayState)->actor.world.pos)) {
                            sp7C = i - 1;
                        }
                    }

                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, sWarpPos[sp7C].x, sWarpPos[sp7C].y, sWarpPos[sp7C].z, 0, 0, 0, WARP_DUNGEON_ADULT, false);
                    break;
                }
                case SCENE_FOREST_TEMPLE_BOSS: {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, 14.0f, -33.0f, -3315.0f, 0, 0, 0, WARP_DUNGEON_ADULT, true);
                    break;
                }
                case SCENE_FIRE_TEMPLE_BOSS: {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, 0.0f, 100.0f, 0.0f, 0, 0, 0, WARP_DUNGEON_ADULT, true);
                    break;
                }
                case SCENE_WATER_TEMPLE_BOSS: {
                    BossMo* bossMo = va_arg(args, BossMo*);
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, bossMo->actor.world.pos.x, -280.0f, bossMo->actor.world.pos.z, 0, 0, 0, WARP_DUNGEON_ADULT, true);
                    break;
                }
                case SCENE_SPIRIT_TEMPLE_BOSS: {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, 600.0f, 230.0f, 0.0f, 0, 0, 0, WARP_DUNGEON_ADULT, true);
                    break;
                }
                case SCENE_SHADOW_TEMPLE_BOSS: {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_DOOR_WARP1, -50.0f, 0.0f, 400.0f, 0, 0, 0, WARP_DUNGEON_ADULT, true);
                    break;
                }
                default: {
                    SPDLOG_WARN("[BossRush]: Blue warp spawned in unhandled scene, ignoring");
                    return;
                }
            }
            *should = false;
            break;
        }
        // Skip past the "Save?" window when pressing B while paused and instead close the menu.
        case VB_CLOSE_PAUSE_MENU: {
            if (CHECK_BTN_ALL(gPlayState->state.input[0].press.button, BTN_B)) {
                *should = true;
            }
            break;
        }
        // Show "No" twice because the player can't continue.
        case VB_RENDER_YES_ON_CONTINUE_PROMPT: {
            Gfx** disp = va_arg(args, Gfx**);
            *disp = KaleidoScope_QuadTextureIA8(*disp, sSavePromptNoChoiceTexs[gSaveContext.language], 48, 16, 12);
            *should = false;
            break;
        }
        // Break the dodongo breakable floor immediately so the player can jump in the hole immediately.
        case VB_BG_BREAKWALL_BREAK: {
            *should = true;
            break;
        }
        // Skip past the "Save?" window when dying and go to the "Continue?" screen immediately.
        case VB_TRANSITION_TO_SAVE_SCREEN_ON_DEATH: {
            PauseContext* pauseCtx = va_arg(args, PauseContext*);
            pauseCtx->state = 0xF;
            *should = false;
            break;
        }
        // Prevent saving
        case VB_BE_ABLE_TO_SAVE:
        // Disable doors so the player can't leave the boss rooms backwards.
        case VB_BE_ABLE_TO_OPEN_DOORS:
        // There's no heart containers in boss rush
        case VB_SPAWN_HEART_CONTAINER:
        // Rupees are useless in boss rush
        case VB_RENDER_RUPEE_COUNTER: {
            *should = false;
            break;
        }
        // Prevent warning spam
        default: {
            break;
        }
    }

    va_end(args);
}

void BossRush_OnActorInitHandler(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);

    if (actor->id == ACTOR_DEMO_SA && gPlayState->sceneNum == SCENE_CHAMBER_OF_THE_SAGES) {
        BossRush_SpawnBlueWarps(gPlayState);
        Actor_Kill(actor);
        GET_PLAYER(gPlayState)->actor.world.rot.y = GET_PLAYER(gPlayState)->actor.shape.rot.y = 27306;
        return;
    }

    // Remove chests, mainly for the chest in King Dodongo's boss room.
    // Remove bushes, used in Gohma's arena.
    // Remove pots, used in Barinade's and Ganondorf's arenas.
    if (actor->id == ACTOR_EN_KUSA || actor->id == ACTOR_OBJ_TSUBO || actor->id == ACTOR_EN_BOX) {
        Actor_Kill(actor);
        return;
    }
}

void BossRush_OnSceneInitHandler(s16 sceneNum) {
    // Unpause the timer when the scene loaded isn't the Chamber of Sages.
    if (sceneNum != SCENE_CHAMBER_OF_THE_SAGES) {
        gSaveContext.isBossRushPaused = 0;
    }
}

void BossRush_OnBossDefeatHandler(void* refActor) {
    BossRush_HandleCompleteBoss(gPlayState);
}

void BossRush_OnBlueWarpUpdate(void* actor) {
    DoorWarp1* blueWarp = static_cast<DoorWarp1*>(actor);

    if (blueWarp->warpTimer > 160) {
        BossRush_HandleBlueWarp(gPlayState, blueWarp->actor.world.pos.x, blueWarp->actor.world.pos.z);
    }
}

void BossRush_RegisterHooks() {
    static u32 onVanillaBehaviorHook = 0;
    static u32 onSceneInitHook = 0;
    static u32 onActorInitHook = 0;
    static u32 onBossDefeatHook = 0;
    static u32 onActorUpdate = 0;

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnVanillaBehavior>(onVanillaBehaviorHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(onSceneInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorInit>(onActorInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnBossDefeat>(onBossDefeatHook);
        GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(onActorUpdate);

        onVanillaBehaviorHook = 0;
        onSceneInitHook = 0;
        onActorInitHook = 0;
        onBossDefeatHook = 0;
        onActorUpdate = 0;

        if (!IS_BOSS_RUSH) return;

        onVanillaBehaviorHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnVanillaBehavior>(BossRush_OnVanillaBehaviorHandler);
        onSceneInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>(BossRush_OnSceneInitHandler);
        onActorInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>(BossRush_OnActorInitHandler);
        onBossDefeatHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnBossDefeat>(BossRush_OnBossDefeatHandler);
        onActorUpdate = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(ACTOR_DOOR_WARP1, BossRush_OnBlueWarpUpdate);
    });
}
