#include "global.h"
#include "vt.h"

#include <string.h>
#include "soh/Enhancements/randomizer/randomizer_entrance.h"
#include "soh/Enhancements/randomizer/savefile.h"

#define NUM_DUNGEONS 8
#define NUM_COWS 10

void Save_LoadFile(void);

/**
 *  Initialize new save.
 *  This save has an empty inventory with 3 hearts and single magic.
 */
void Sram_InitNewSave(void) {
    Save_InitFile(false);
}

/**
 *  Initialize debug save. This is also used on the Title Screen
 *  This save has a mostly full inventory with 10 hearts and single magic.
 *
 *  Some noteable flags that are set:
 *  Showed Mido sword/shield, met Deku Tree, Deku Tree mouth opened,
 *  used blue warp in Gohmas room, Zelda fled castle, light arrow cutscene watched,
 *  and set water level in Water Temple to lowest level.
 */
void Sram_InitDebugSave(void) {
    Save_InitFile(true);
}

void Sram_InitBossRushSave(void) {
    Save_InitFile(false);
}

/**
 *  Copy save currently on the buffer to Save Context and complete various tasks to open the save.
 *  This includes:
 *  - Set proper entrance depending on where the game was saved
 *  - If health is less than 3 hearts, give 3 hearts
 *  - If either scarecrow song is set, copy them from save context to the proper location
 *  - Handle a case where the player saved and quit after zelda cutscene but didnt get the song
 *  - Give and equip master sword if player is adult and doesnt have kokiri sword (bug?)
 *  - Revert any trade items that spoil
 */
void Sram_OpenSave() {
    static s16 dungeonEntrances[] = {
        0x0000, 0x0004, 0x0028, 0x0169, 0x0165, 0x0010, 0x0082, 0x0037,
        0x0098, 0x0088, 0x041B, 0x0008, 0x0486, 0x0467, 0x0179, 0x056C,
    };
    u16 i;
    u16 j;
    u8* ptr;

    Save_LoadFile();

    if (!CVarGetInteger("gRememberSaveLocation", 0) || gSaveContext.savedSceneNum == SCENE_FAIRYS_FOUNTAIN ||
        gSaveContext.savedSceneNum == SCENE_GROTTOS) {
        switch (gSaveContext.savedSceneNum) {
            case SCENE_DEKU_TREE:
            case SCENE_DODONGOS_CAVERN:
            case SCENE_JABU_JABU:
            case SCENE_FOREST_TEMPLE:
            case SCENE_FIRE_TEMPLE:
            case SCENE_WATER_TEMPLE:
            case SCENE_SPIRIT_TEMPLE:
            case SCENE_SHADOW_TEMPLE:
            case SCENE_BOTTOM_OF_THE_WELL:
            case SCENE_ICE_CAVERN:
            case SCENE_GANONS_TOWER:
            case SCENE_GERUDO_TRAINING_GROUND:
            case SCENE_THIEVES_HIDEOUT:
            case SCENE_INSIDE_GANONS_CASTLE:
                gSaveContext.entranceIndex = dungeonEntrances[gSaveContext.savedSceneNum];
                break;
            case SCENE_DEKU_TREE_BOSS:
                gSaveContext.entranceIndex = 0;
                break;
            case SCENE_DODONGOS_CAVERN_BOSS:
                gSaveContext.entranceIndex = 4;
                break;
            case SCENE_JABU_JABU_BOSS:
                gSaveContext.entranceIndex = 0x28;
                break;
            case SCENE_FOREST_TEMPLE_BOSS:
                gSaveContext.entranceIndex = 0x169;
                break;
            case SCENE_FIRE_TEMPLE_BOSS:
                gSaveContext.entranceIndex = 0x165;
                break;
            case SCENE_WATER_TEMPLE_BOSS:
                gSaveContext.entranceIndex = 0x10;
                break;
            case SCENE_SPIRIT_TEMPLE_BOSS:
                gSaveContext.entranceIndex = 0x82;
                break;
            case SCENE_SHADOW_TEMPLE_BOSS:
                gSaveContext.entranceIndex = 0x37;
                break;
            case SCENE_GANONS_TOWER_COLLAPSE_INTERIOR:
            case SCENE_INSIDE_GANONS_CASTLE_COLLAPSE:
            case SCENE_GANONDORF_BOSS:
            case SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR:
            case SCENE_GANON_BOSS:
                gSaveContext.entranceIndex = 0x41B;
                break;

            default:
                if (gSaveContext.savedSceneNum != SCENE_LINKS_HOUSE) {
                    gSaveContext.entranceIndex = (LINK_AGE_IN_YEARS == YEARS_CHILD) ? 0xBB : 0x5F4;
                } else {
                    gSaveContext.entranceIndex = 0xBB;
                }
                break;
        }
    }

    osSyncPrintf("scene_no = %d\n", gSaveContext.entranceIndex);
    osSyncPrintf(VT_RST);

    if (gSaveContext.health < 0x30) {
        gSaveContext.health = CVarGetInteger("gFullHealthSpawn", 0) ? gSaveContext.healthCapacity : 0x30;
    }

    if (gSaveContext.scarecrowLongSongSet) {
        osSyncPrintf(VT_FGCOL(BLUE));
        osSyncPrintf("\n====================================================================\n");

        memcpy(gScarecrowCustomSongPtr, gSaveContext.scarecrowLongSong, sizeof(gSaveContext.scarecrowLongSong));

        ptr = (u8*)gScarecrowCustomSongPtr;
        for (i = 0; i < ARRAY_COUNT(gSaveContext.scarecrowLongSong); i++, ptr++) {
            osSyncPrintf("%d, ", *ptr);
        }

        osSyncPrintf("\n====================================================================\n");
        osSyncPrintf(VT_RST);
    }

    if (gSaveContext.scarecrowSpawnSongSet) {
        osSyncPrintf(VT_FGCOL(GREEN));
        osSyncPrintf("\n====================================================================\n");

        memcpy(gScarecrowSpawnSongPtr, gSaveContext.scarecrowSpawnSong, sizeof(gSaveContext.scarecrowSpawnSong));

        ptr = gScarecrowSpawnSongPtr;
        for (i = 0; i < ARRAY_COUNT(gSaveContext.scarecrowSpawnSong); i++, ptr++) {
            osSyncPrintf("%d, ", *ptr);
        }

        osSyncPrintf("\n====================================================================\n");
        osSyncPrintf(VT_RST);
    }

    // if zelda cutscene has been watched but lullaby was not obtained, restore cutscene and take away letter
    if ((Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_ZELDAS_LETTER)) && !CHECK_QUEST_ITEM(QUEST_SONG_LULLABY) && !IS_RANDO) {
        i = gSaveContext.eventChkInf[4] & ~1;
        gSaveContext.eventChkInf[4] = i;

        INV_CONTENT(ITEM_LETTER_ZELDA) = ITEM_CHICKEN;

        for (j = 1; j < ARRAY_COUNT(gSaveContext.equips.buttonItems); j++) {
            if (gSaveContext.equips.buttonItems[j] == ITEM_LETTER_ZELDA) {
                gSaveContext.equips.buttonItems[j] = ITEM_CHICKEN;
            }
        }
    }

    // check for owning kokiri sword.. to restore master sword? bug or debug feature?
    if (LINK_AGE_IN_YEARS == YEARS_ADULT && !CHECK_OWNED_EQUIP(EQUIP_SWORD, 1)) {
        gSaveContext.inventory.equipment |= gBitFlags[1] << gEquipShifts[EQUIP_SWORD];
        gSaveContext.equips.buttonItems[0] = ITEM_SWORD_MASTER;
        gSaveContext.equips.equipment &= ~0xF;
        gSaveContext.equips.equipment |= 2;
    }

    if (!(IS_RANDO && Randomizer_GetSettingValue(RSK_SHUFFLE_ADULT_TRADE))) {
        for (i = 0; i < ARRAY_COUNT(gSpoilingItems); i++) {
            if (INV_CONTENT(ITEM_TRADE_ADULT) == gSpoilingItems[i]) {
                INV_CONTENT(gSpoilingItemReverts[i]) = gSpoilingItemReverts[i];

                for (j = 1; j < ARRAY_COUNT(gSaveContext.equips.buttonItems); j++) {
                    if (gSaveContext.equips.buttonItems[j] == gSpoilingItems[i]) {
                        gSaveContext.equips.buttonItems[j] = gSpoilingItemReverts[i];
                    }
                }
            }
        }
    }

    gSaveContext.magicLevel = 0;
}

void Sram_InitSave(FileChooseContext* fileChooseCtx) {
    u16 offset;
    u16 j;
    u16* ptr;
    u16 checksum;

    if (fileChooseCtx->buttonIndex != 0 || !CVarGetInteger("gDebugEnabled", 0)) {
        Sram_InitNewSave();
    } else {
        Sram_InitDebugSave();
    }

    gSaveContext.entranceIndex = 0xBB;
    gSaveContext.linkAge = 1;
    gSaveContext.dayTime = 0x6AAB;
    gSaveContext.cutsceneIndex = 0xFFF1;

    if ((fileChooseCtx->buttonIndex == 0 && CVarGetInteger("gDebugEnabled", 0)) || CVarGetInteger("gNaviSkipCutscene", 0)) {
        gSaveContext.cutsceneIndex = 0;
    }

    for (offset = 0; offset < 8; offset++) {
        gSaveContext.playerName[offset] = Save_GetSaveMetaInfo(fileChooseCtx->buttonIndex)->playerName[offset];
    }

    gSaveContext.n64ddFlag = fileChooseCtx->n64ddFlag;

    if (fileChooseCtx->questType[fileChooseCtx->buttonIndex] == QUEST_RANDOMIZER &&
        strnlen(CVarGetString("gSpoilerLog", ""), 1) != 0) {
        gSaveContext.questId = QUEST_RANDOMIZER;

        Randomizer_InitSaveFile();
    }

    Save_SaveFile();
    SaveManager_ThreadPoolWait();
}

void Sram_InitSram(GameState* gameState) {
    Save_Init();

    func_800F6700(gSaveContext.audioSetting);

    // When going from a rando save to a vanilla save within the same game instance
    // we need to reset the entrance table back to its vanilla state
    Entrance_ResetEntranceTable();
}
