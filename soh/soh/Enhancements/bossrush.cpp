#include "bossrush.h"
#include "functions.h"
#include "macros.h"

#include <array>
#include <string>
#include <vector>

typedef struct BossRushSetting {
    std::vector<std::string> name;
    std::vector<std::vector<std::string>> choices;
} BossRushSetting;

BossRushSetting BossRushOptions[BOSSRUSH_OPTIONS_AMOUNT] = { 
    {
        { "BOSSES:", "BOSSES DE:", "BOSS:" },
        {
            { "All", "All DE", "Tous" },
            { "Child", "Child DE", "Enfant" },
            { "Adult", "Adult DE", "Adulte" }
        }
    },
    {
        { "HEARTS:", "HEARTS DE:", "COEURS:" },
        {
            { "7", "7", "7" },
            { "10", "10", "10" },
            { "15", "15", "15" },
            { "20", "20", "20" },
            { "3", "3", "3" },
            { "5", "5", "5" }
        }
    },
    {
        { "AMMO:", "AMMO DE:", "MUNITIONS" },
        {
            { "Limited", "Limited DE", "Limit�es" },
            { "Full", "Full DE", "Full FR" },
            { "Maxed", "Maxed DE", "Maxed FR" }
        }
    },
    {
        { "HEAL:", "HEAL DE:", "SOIN:" },
        {
            { "Ganondorf", "Ganondorf DE", "Ganondorf" },
            { "Every Boss", "Every Boss DE", "Tous les Boss" },
            { "Never", "Never DE", "Jamais" }
        }
    },
    {
        { "BOTTLE:", "BOTTLE DE:", "BOUTEILLE:" },
        {
            { "No", "Nein", "Non" },
            { "Yes", "Ja", "Oui" }
        }
    },
    {
        { "LONGSHOT:", "LONGSHOT DE:", "SUPER GRAPPIN:" },
        {
            { "No", "Nein", "Non" },
            { "Yes", "Ja", "Oui" }
        }
    },
    {
        { "HOVER BOOTS:", "HOVER BOOTS DE:", "BOTTES DES AIRS:" },
        {
            { "No", "Nein", "Non" },
            { "Yes", "Ja", "Oui" }
        }
    },
    {
        { "TIMER:", "TIMER DE:", "TIMER:" },
        {
            { "Yes", "Ja", "Oui" },
            { "No", "Nein", "Non" }
        }
    }
};

extern "C" const char* BossRush_GetSettingName(uint8_t optionIndex, uint8_t language) {
    return BossRushOptions[optionIndex].name[language].c_str();
}

extern "C" const char* BossRush_GetSettingChoiceName(uint8_t optionIndex, uint8_t choiceIndex, uint8_t language) {
    return BossRushOptions[optionIndex].choices[choiceIndex][language].c_str();
}

extern "C" uint8_t BossRush_GetSettingOptionsAmount(uint8_t optionIndex) {
    return BossRushOptions[optionIndex].choices.size();
}

extern "C" void BossRush_SpawnBlueWarps(PlayState* play) {

    // Spawn blue warps in Chamber of Sages based on what bosses have been defeated.
    if (gSaveContext.linkAge == LINK_AGE_CHILD) {
        // Forest Medallion (Gohma)
        if (!Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_DEKU_TREE)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, -100, 6, -170, 0, 0, 0, -1, false);
        }
        // Fire Medallion (King Dodongo)
        if (!Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_DODONGOS_CAVERN)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, 100, 6, -170, 0, 0, 0, -1, false);
        }
        // Water Medallion (Barinade)
        if (!Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_JABU_JABUS_BELLY)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, 199, 6, 0, 0, 0, 0, -1, false);
        }
    } else {
        // Light Medallion (Ganondorf)
        if (CheckDungeonCount() == 8) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, -199, 6, 0, 0, 0, 0, -1, false);
        }
        // Forest Medallion (Phantom Ganondorf)
        if (!Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_FOREST_TEMPLE)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, -100, 6, -170, 0, 0, 0, -1, false);
        }
        // Fire Medallion (Volvagia)
        if (!Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_FIRE_TEMPLE)) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_DOOR_WARP1, 100, 6, -170, 0, 0, 0, -1, false);
        }
        // Water Medallion (Morpha)
        if (!Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_WATER_TEMPLE)) {
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

extern "C" void BossRush_HandleBlueWarp(PlayState* play, f32 warpPosX, f32 warpPosZ) {

    // If warping from Chamber of Sages, choose the correct boss room to teleport to.
    if (play->sceneNum == SCENE_KENJYANOMA) {
        // Gohma & Phantom Ganon
        if (warpPosX == -100 && warpPosZ == -170) {
            if (gSaveContext.linkAge == LINK_AGE_CHILD) {
                play->nextEntranceIndex = 0x040F;
            } else {
                play->nextEntranceIndex = 0x000C;
            }
        // King Dodongo & Volvagia
        } else if (warpPosX == 100 && warpPosZ == -170) {
            if (gSaveContext.linkAge == LINK_AGE_CHILD) {
                play->nextEntranceIndex = 0x040B;
            } else {
                play->nextEntranceIndex = 0x0305;
            }
        // Barinade & Morb
        } else if (warpPosX == 199 && warpPosZ == 0) {
            if (gSaveContext.linkAge == LINK_AGE_CHILD) {
                play->nextEntranceIndex = 0x0301;
            } else {
                play->nextEntranceIndex = 0x0417;
            }
        // Twinrova
        } else if (warpPosX == 100 && warpPosZ == 170) {
            play->nextEntranceIndex = 0x05EC;
        // Bongo Bongo
        } else if (warpPosX == -100 && warpPosZ == 170) {
            play->nextEntranceIndex = 0x0413;
        // Ganondork
        } else if (warpPosX == -199 && warpPosZ == 0) {
            play->nextEntranceIndex = 0x041F;
        }
    // If coming from a boss room, teleport back to Chamber of Sages and set flag.
    } else {
        play->nextEntranceIndex = SCENE_HAIRAL_NIWA2;
        switch (play->sceneNum) {
            case SCENE_YDAN_BOSS:
                Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_DEKU_TREE);
                break;
            case SCENE_DDAN_BOSS:
                Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_DODONGOS_CAVERN);
                break;
            case SCENE_BDAN_BOSS:
                Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_JABU_JABUS_BELLY);
                break;
            case SCENE_MORIBOSSROOM:
                Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_FOREST_TEMPLE);
                break;
            case SCENE_FIRE_BS:
                Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_FIRE_TEMPLE);
                break;
            case SCENE_MIZUSIN_BS:
                Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_WATER_TEMPLE);
                break;
            case SCENE_JYASINBOSS:
                Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_SPIRIT_TEMPLE);
                break;
            case SCENE_HAKADAN_BS:
                Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_SHADOW_TEMPLE);
                break;
            default:
                break;
        }

        // When 3 bosses (all child bosses) are defeated, change to Adult Link.
        if (CheckDungeonCount() == 3) {
            play->linkAgeOnLoad = LINK_AGE_ADULT;
            gSaveContext.linkAge = LINK_AGE_ADULT;
            BossRush_SetEquipment(LINK_AGE_ADULT);
        }
    }
}

extern "C" void BossRush_HandleCompleteBoss(PlayState* play) {
    gSaveContext.isBossRushPaused = 1;
    switch (play->sceneNum) {
        case SCENE_YDAN_BOSS:
            gSaveContext.sohStats.itemTimestamp[TIMESTAMP_BOSSRUSH_DEFEAT_GOHMA] = GAMEPLAYSTAT_TOTAL_TIME;
            break;
        case SCENE_DDAN_BOSS:
            gSaveContext.sohStats.itemTimestamp[TIMESTAMP_BOSSRUSH_DEFEAT_KING_DODONGO] = GAMEPLAYSTAT_TOTAL_TIME;
            break;
        case SCENE_BDAN_BOSS:
            gSaveContext.sohStats.itemTimestamp[TIMESTAMP_BOSSRUSH_DEFEAT_BARINADE] = GAMEPLAYSTAT_TOTAL_TIME;
            break;
        case SCENE_MORIBOSSROOM:
            gSaveContext.sohStats.itemTimestamp[TIMESTAMP_BOSSRUSH_DEFEAT_PHANTOM_GANON] = GAMEPLAYSTAT_TOTAL_TIME;
            break;
        case SCENE_FIRE_BS:
            gSaveContext.sohStats.itemTimestamp[TIMESTAMP_BOSSRUSH_DEFEAT_VOLVAGIA] = GAMEPLAYSTAT_TOTAL_TIME;
            break;
        case SCENE_MIZUSIN_BS:
            gSaveContext.sohStats.itemTimestamp[TIMESTAMP_BOSSRUSH_DEFEAT_MORPHA] = GAMEPLAYSTAT_TOTAL_TIME;
            break;
        case SCENE_JYASINBOSS:
            gSaveContext.sohStats.itemTimestamp[TIMESTAMP_BOSSRUSH_DEFEAT_TWINROVA] = GAMEPLAYSTAT_TOTAL_TIME;
            break;
        case SCENE_HAKADAN_BS:
            gSaveContext.sohStats.itemTimestamp[TIMESTAMP_BOSSRUSH_DEFEAT_BONGO_BONGO] = GAMEPLAYSTAT_TOTAL_TIME;
            break;
        default:
            break;
    }
}

extern "C" void BossRush_InitSave() {

    // Set player name to Link just in case it'll be used in textboxes. Player can't input their own name.
    static std::array<char, 8> brPlayerName = { 0x15, 0x12, 0x17, 0x14, 0x3E, 0x3E, 0x3E, 0x3E };
    for (int i = 0; i < ARRAY_COUNT(gSaveContext.playerName); i++) {
        gSaveContext.playerName[i] = brPlayerName[i];
    }

    gSaveContext.isBossRushPaused = 1;
    gSaveContext.entranceIndex = 107;
    gSaveContext.cutsceneIndex = 0x8000;
    gSaveContext.magicLevel = 0;
    gSaveContext.magic = 0x30;
    gSaveContext.isMagicAcquired = 1;

    uint16_t health = 16;
    switch (gSaveContext.bossRushSelectedOptions[BR_OPTIONS_HEARTS]) { 
        case BR_OPTION_HEARTS_CHOICE_7:
            health *= 7;
            break;
        case BR_OPTION_HEARTS_CHOICE_10:
            health *= 10;
            break;
        case BR_OPTION_HEARTS_CHOICE_15:
            health *= 15;
            break;
        case BR_OPTION_HEARTS_CHOICE_20:
            health *= 20;
            break;
        case BR_OPTION_HEARTS_CHOICE_3:
            health *= 3;
            break;
        case BR_OPTION_HEARTS_CHOICE_5:
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

    static std::array<u8, 24> brItems = {
        ITEM_STICK,     ITEM_NUT,  ITEM_BOMB, ITEM_BOW,      ITEM_NONE,        ITEM_NONE,
        ITEM_SLINGSHOT, ITEM_NONE, ITEM_NONE, ITEM_HOOKSHOT, ITEM_NONE,        ITEM_NONE,
        ITEM_BOOMERANG, ITEM_LENS, ITEM_NONE, ITEM_HAMMER,   ITEM_ARROW_LIGHT, ITEM_NONE,
        ITEM_NONE,      ITEM_NONE, ITEM_NONE, ITEM_NONE,     ITEM_NONE,        ITEM_NONE,
    };

    if (gSaveContext.bossRushSelectedOptions[BR_OPTIONS_LONGSHOT] == BR_OPTION_LONGSHOT_CHOICE_YES) {
        brItems[9] = ITEM_LONGSHOT;
    }

    if (gSaveContext.bossRushSelectedOptions[BR_OPTIONS_BOTTLE] == BR_OPTION_BOTTLE_CHOICE_YES) {
        brItems[18] = ITEM_BOTTLE;
    }

    static std::array<s8, 16> brAmmo = { 5, 5, 10, 10, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    for (int item = 0; item < ARRAY_COUNT(gSaveContext.inventory.items); item++) {
        gSaveContext.inventory.items[item] = brItems[item];
    }

    for (int ammo = 0; ammo < ARRAY_COUNT(gSaveContext.inventory.ammo); ammo++) {
        gSaveContext.inventory.ammo[ammo] = brAmmo[ammo];
    }

    gSaveContext.inventory.equipment = 4947;
    gSaveContext.inventory.upgrades = 1196105;

    if (gSaveContext.bossRushSelectedOptions[BR_OPTIONS_BOSSES] == BR_OPTION_BOSSES_CHOICE_ADULT) {
        Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_DEKU_TREE);
        Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_DODONGOS_CAVERN);
        Flags_SetRandomizerInf(RAND_INF_DUNGEONS_DONE_JABU_JABUS_BELLY);
        gSaveContext.linkAge = LINK_AGE_ADULT;
        BossRush_SetEquipment(LINK_AGE_ADULT);
    } else {
        gSaveContext.linkAge = LINK_AGE_CHILD;
        BossRush_SetEquipment(LINK_AGE_CHILD);
    }
}

void BossRush_SetEquipment(uint8_t linkAge) {

    static std::array<u8, 8> brButtonItems;
    static std::array<u8, 7> brCButtonSlots;

    // Set Child Equipment. Used on save init.
    if (linkAge == LINK_AGE_CHILD) {
        brButtonItems = {
            ITEM_SWORD_KOKIRI, ITEM_STICK, ITEM_NUT, ITEM_BOMB, ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE
        };

        brCButtonSlots = { SLOT_STICK, SLOT_NUT, SLOT_BOMB, SLOT_NONE, SLOT_NONE, SLOT_NONE, SLOT_NONE };

        gSaveContext.equips.equipment = 4369;
    // Set Adult equipment. Used when first 3 bosses are defeated and the player is set to adult.
    } else {
        brButtonItems = {
            ITEM_SWORD_MASTER, ITEM_BOW, ITEM_HAMMER, ITEM_BOMB, ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE
        };

        brCButtonSlots = { SLOT_BOW, SLOT_HAMMER, SLOT_BOMB, SLOT_NONE, SLOT_NONE, SLOT_NONE, SLOT_NONE };

        gSaveContext.equips.equipment = 4658;
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
