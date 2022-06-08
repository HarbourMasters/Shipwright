#include "global.h"
#include "vt.h"

#include <string.h>

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

void GiveLinksPocketMedallion() {
    GetItemID getItemId = GetRandomizedItemIdFromKnownCheck(RC_LINKS_POCKET, RG_NONE);

    s16 item;

    u8 medallion = 0;

    switch (getItemId) {
        case GI_MEDALLION_FOREST:
            item = ITEM_MEDALLION_FOREST;
            medallion = 1;
            break;
        case GI_MEDALLION_FIRE:
            item = ITEM_MEDALLION_FIRE;
            medallion = 1;
            break;
        case GI_MEDALLION_WATER:
            item = ITEM_MEDALLION_WATER;
            medallion = 1;
            break;
        case GI_MEDALLION_SHADOW:
            item = ITEM_MEDALLION_SHADOW;
            medallion = 1;
            break;
        case GI_MEDALLION_SPIRIT:
            item = ITEM_MEDALLION_SPIRIT;
            medallion = 1;
            break;
        case GI_MEDALLION_LIGHT:
            item = ITEM_MEDALLION_LIGHT;
            medallion = 1;
            break;
        case GI_STONE_KOKIRI:
            item = ITEM_KOKIRI_EMERALD;
            break;
        case GI_STONE_GORON:
            item = ITEM_GORON_RUBY;
            break;
        case GI_STONE_ZORA:
            item = ITEM_ZORA_SAPPHIRE;
            break;
    }

    if (medallion == 1) {
        gSaveContext.inventory.questItems |= gBitFlags[item - ITEM_MEDALLION_FOREST + QUEST_MEDALLION_FOREST];
    } else {
        gSaveContext.inventory.questItems |= gBitFlags[item - ITEM_KOKIRI_EMERALD + QUEST_KOKIRI_EMERALD];
    }
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

    switch (gSaveContext.savedSceneNum) {
        case SCENE_YDAN:
        case SCENE_DDAN:
        case SCENE_BDAN:
        case SCENE_BMORI1:
        case SCENE_HIDAN:
        case SCENE_MIZUSIN:
        case SCENE_JYASINZOU:
        case SCENE_HAKADAN:
        case SCENE_HAKADANCH:
        case SCENE_ICE_DOUKUTO:
        case SCENE_GANON:
        case SCENE_MEN:
        case SCENE_GERUDOWAY:
        case SCENE_GANONTIKA:
            gSaveContext.entranceIndex = dungeonEntrances[gSaveContext.savedSceneNum];
            break;
        case SCENE_YDAN_BOSS:
            gSaveContext.entranceIndex = 0;
            break;
        case SCENE_DDAN_BOSS:
            gSaveContext.entranceIndex = 4;
            break;
        case SCENE_BDAN_BOSS:
            gSaveContext.entranceIndex = 0x28;
            break;
        case SCENE_MORIBOSSROOM:
            gSaveContext.entranceIndex = 0x169;
            break;
        case SCENE_FIRE_BS:
            gSaveContext.entranceIndex = 0x165;
            break;
        case SCENE_MIZUSIN_BS:
            gSaveContext.entranceIndex = 0x10;
            break;
        case SCENE_JYASINBOSS:
            gSaveContext.entranceIndex = 0x82;
            break;
        case SCENE_HAKADAN_BS:
            gSaveContext.entranceIndex = 0x37;
            break;
        case SCENE_GANON_SONOGO:
        case SCENE_GANONTIKA_SONOGO:
        case SCENE_GANON_BOSS:
        case SCENE_GANON_FINAL:
        case SCENE_GANON_DEMO:
            gSaveContext.entranceIndex = 0x41B;
            break;

        default:
            if (gSaveContext.savedSceneNum != SCENE_LINK_HOME) {
                gSaveContext.entranceIndex = (LINK_AGE_IN_YEARS == YEARS_CHILD) ? 0xBB : 0x5F4;
            } else {
                gSaveContext.entranceIndex = 0xBB;
            }
            break;
    }

    osSyncPrintf("scene_no = %d\n", gSaveContext.entranceIndex);
    osSyncPrintf(VT_RST);

    if (gSaveContext.health < 0x30) {
        gSaveContext.health = 0x30;
    }

    if (gSaveContext.scarecrowCustomSongSet) {
        osSyncPrintf(VT_FGCOL(BLUE));
        osSyncPrintf("\n====================================================================\n");

        memcpy(gScarecrowCustomSongPtr, gSaveContext.scarecrowCustomSong, sizeof(gSaveContext.scarecrowCustomSong));

        ptr = (u8*)gScarecrowCustomSongPtr;
        for (i = 0; i < ARRAY_COUNT(gSaveContext.scarecrowCustomSong); i++, ptr++) {
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
    if ((gSaveContext.eventChkInf[4] & 1) && !CHECK_QUEST_ITEM(QUEST_SONG_LULLABY) && !gSaveContext.n64ddFlag) {
        i = gSaveContext.eventChkInf[4] & ~1;
        gSaveContext.eventChkInf[4] = i;

        INV_CONTENT(ITEM_LETTER_ZELDA) = ITEM_CHICKEN;

        for (j = 1; j < 4; j++) {
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

    for (i = 0; i < ARRAY_COUNT(gSpoilingItems); i++) {
        if (INV_CONTENT(ITEM_TRADE_ADULT) == gSpoilingItems[i]) {
            INV_CONTENT(gSpoilingItemReverts[i]) = gSpoilingItemReverts[i];

            for (j = 1; j < 4; j++) {
                if (gSaveContext.equips.buttonItems[j] == gSpoilingItems[i]) {
                    gSaveContext.equips.buttonItems[j] = gSpoilingItemReverts[i];
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

    if (fileChooseCtx->buttonIndex != 0 || !CVar_GetS32("gDebugEnabled", 0)) {
        Sram_InitNewSave();
    } else {
        Sram_InitDebugSave();
    }

    gSaveContext.entranceIndex = 0xBB;
    gSaveContext.linkAge = 1;
    gSaveContext.dayTime = 0x6AAB;
    gSaveContext.cutsceneIndex = 0xFFF1;

    if ((fileChooseCtx->buttonIndex == 0 && CVar_GetS32("gDebugEnabled", 0)) || CVar_GetS32("gNaviSkipCutscene", 0)) {
        gSaveContext.cutsceneIndex = 0;
    }

    for (offset = 0; offset < 8; offset++) {
        gSaveContext.playerName[offset] = Save_GetSaveMetaInfo(fileChooseCtx->buttonIndex)->playerName[offset];
    }

    if (CVar_GetS32("gRandomizer", 0) != 0 && CVar_GetString("gSpoilerLog", "") != "") {
        // Set N64DD Flags for save file
        fileChooseCtx->n64ddFlags[fileChooseCtx->buttonIndex] = 1;
        fileChooseCtx->n64ddFlag = 1;
        gSaveContext.n64ddFlag = 1;

        // Set Cutscene flags to skip them
        gSaveContext.eventChkInf[0xC] |= 0x10;
        gSaveContext.eventChkInf[0xC] |= 0x20;
        gSaveContext.eventChkInf[4] |= 0x20;
        gSaveContext.eventChkInf[4] |= 0x8000;
        gSaveContext.infTable[0] |= 1;
        gSaveContext.cutsceneIndex = 0;
        Flags_SetEventChkInf(5);

        // Give Link's pocket item
        GiveLinksPocketMedallion();

        int openForest = GetRandoSettingValue(RSK_FOREST);
        switch (openForest) {
            case 0: // closed
                break;
            case 1: // open
                Flags_SetEventChkInf(7);
                gSaveContext.eventChkInf[0] |= 0x10;
                break;
            case 2: // closed deku
                Flags_SetEventChkInf(7);
                break;
        }

        int doorOfTime = GetRandoSettingValue(RSK_DOOR_OF_TIME);
        switch (doorOfTime) {
            case 0: // open
                gSaveContext.eventChkInf[4] |= 0x800;
                break;
        }

        int kakGate = GetRandoSettingValue(RSK_KAK_GATE);
        switch (kakGate) {
            case 1: // open
                gSaveContext.infTable[7] |= 0x40;
                break;
        }
      
        if(GetRandoSettingValue(RSK_STARTING_KOKIRI_SWORD)) {
            uint32_t bitMask = 1 << 0;
            gSaveContext.inventory.equipment |= bitMask;
        }

        if(GetRandoSettingValue(RSK_STARTING_DEKU_SHIELD)) {
            uint32_t bitMask = 1 << 4;
            gSaveContext.inventory.equipment |= bitMask;
        }

        if(GetRandoSettingValue(RSK_STARTING_OCARINA)) {
            INV_CONTENT(ITEM_OCARINA_FAIRY) = ITEM_OCARINA_FAIRY;
        }

        if(GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES)) {
            uint32_t mapBitMask = 1 << 1;
            uint32_t compassBitMask = 1 << 2;
            uint32_t startingDungeonItemsBitMask = mapBitMask | compassBitMask;
            for(int scene = 0; scene <= 9; scene++) {
                gSaveContext.inventory.dungeonItems[scene] |= startingDungeonItemsBitMask;
            }
        }
    }

    Save_SaveFile();
}

void Sram_InitSram(GameState* gameState) {
    Save_Init();

    func_800F6700(gSaveContext.audioSetting);
}