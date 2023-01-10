#include "global.h"
#include "vt.h"

#include <string.h>
#include <soh/Enhancements/randomizer/randomizerTypes.h>
#include <soh/Enhancements/randomizer/randomizer_inf.h>
#include "soh/Enhancements/randomizer/randomizer_entrance.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"

#define NUM_DUNGEONS 8
#define NUM_COWS 10

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

// RANDOTODO replace most of these GiveLink functions with calls to
// Item_Give in z_parameter, we'll need to update Item_Give to ensure
// nothing breaks when calling it without a valid play first
void GiveLinkRupees(int numOfRupees) {
    int maxRupeeCount;
    if (CUR_UPG_VALUE(UPG_WALLET) == 0) {
        maxRupeeCount = 99;
    } else if (CUR_UPG_VALUE(UPG_WALLET) == 1) {
        maxRupeeCount = 200;
    } else if (CUR_UPG_VALUE(UPG_WALLET) == 2) {
        maxRupeeCount = 500;
    }

    int newRupeeCount = gSaveContext.rupees;
    newRupeeCount += numOfRupees;

    if (newRupeeCount > maxRupeeCount) {
        gSaveContext.rupees = maxRupeeCount;
    } else {
        gSaveContext.rupees = newRupeeCount;
    }
}

void GiveLinkDekuSticks(int howManySticks) {
    int maxStickCount;
    if (CUR_UPG_VALUE(UPG_STICKS) == 0) {
        INV_CONTENT(ITEM_STICK) = ITEM_STICK;
        Inventory_ChangeUpgrade(UPG_STICKS, 1);
        maxStickCount = 10;
    } else if (CUR_UPG_VALUE(UPG_STICKS) == 1) {
        maxStickCount = 10;
    } else if (CUR_UPG_VALUE(UPG_STICKS) == 2) {
        maxStickCount = 20;
    } else if (CUR_UPG_VALUE(UPG_STICKS) == 3) {
        maxStickCount = 30;
    }

    if ((AMMO(ITEM_STICK) + howManySticks) > maxStickCount) {
        AMMO(ITEM_STICK) = maxStickCount;
    } else {
        AMMO(ITEM_STICK) += howManySticks;
    }
}

void GiveLinkDekuNuts(int howManyNuts) {
    int maxNutCount;
    if (CUR_UPG_VALUE(UPG_NUTS) == 0) {
        INV_CONTENT(ITEM_NUT) = ITEM_NUT;
        Inventory_ChangeUpgrade(UPG_NUTS, 1);
        maxNutCount = 20;
    } else if (CUR_UPG_VALUE(UPG_NUTS) == 1) {
        maxNutCount = 20;
    } else if (CUR_UPG_VALUE(UPG_NUTS) == 2) {
        maxNutCount = 30;
    } else if (CUR_UPG_VALUE(UPG_NUTS) == 3) {
        maxNutCount = 40;
    }

    if ((AMMO(ITEM_NUT) + howManyNuts) > maxNutCount) {
        AMMO(ITEM_NUT) = maxNutCount;
    } else {
        AMMO(ITEM_NUT) += howManyNuts;
    }
}

void GiveLinksPocketItem() {
    if (Randomizer_GetSettingValue(RSK_LINKS_POCKET) != RO_LINKS_POCKET_NOTHING)  {
        GetItemEntry getItemEntry = Randomizer_GetItemFromKnownCheck(RC_LINKS_POCKET, RG_NONE);

        if (getItemEntry.modIndex == MOD_NONE) {
            if (getItemEntry.getItemId == GI_SWORD_BGS) {
                gSaveContext.bgsFlag = true;
            }
            Item_Give(NULL, getItemEntry.itemId);
        } else if (getItemEntry.modIndex == MOD_RANDOMIZER) {
            if (getItemEntry.getItemId == RG_ICE_TRAP) {
                gSaveContext.pendingIceTrapCount++;
            } else {
                Randomizer_Item_Give(NULL, getItemEntry);
            }
        }
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

    if (!CVar_GetS32("gRememberSaveLocation", 0) || gSaveContext.savedSceneNum == SCENE_YOUSEI_IZUMI_TATE ||
        gSaveContext.savedSceneNum == SCENE_KAKUSIANA) {
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
    }

    // Setup the modified entrance table and entrance shuffle table for rando
    if (gSaveContext.n64ddFlag) {
        Entrance_Init();
        Entrance_InitEntranceTrackingData();
    }

    osSyncPrintf("scene_no = %d\n", gSaveContext.entranceIndex);
    osSyncPrintf(VT_RST);

    if (gSaveContext.health < 0x30) {
        gSaveContext.health = 0x30;
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
    if ((gSaveContext.eventChkInf[4] & 1) && !CHECK_QUEST_ITEM(QUEST_SONG_LULLABY) && !gSaveContext.n64ddFlag) {
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

    if (!(gSaveContext.n64ddFlag && Randomizer_GetSettingValue(RSK_SHUFFLE_ADULT_TRADE))) {
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

    if (fileChooseCtx->questType[fileChooseCtx->buttonIndex] == 2 && strnlen(CVar_GetString("gSpoilerLog", ""), 1) != 0 &&
        !((Save_GetSaveMetaInfo(fileChooseCtx->buttonIndex)->requiresMasterQuest && !ResourceMgr_GameHasMasterQuest()) ||
          (Save_GetSaveMetaInfo(fileChooseCtx->buttonIndex)->requiresMasterQuest && !ResourceMgr_GameHasOriginal()))) {
        // Set N64DD Flags for save file
        fileChooseCtx->n64ddFlags[fileChooseCtx->buttonIndex] = 1;
        fileChooseCtx->n64ddFlag = 1;
        gSaveContext.n64ddFlag = 1;

        // Sets all rando flags to false
        for (s32 i = 0; i < ARRAY_COUNT(gSaveContext.randomizerInf); i++) {
            gSaveContext.randomizerInf[i] = 0;
        }

        // If any trials aren't required, set them as completed
        for (u16 i = RAND_INF_TRIALS_DONE_LIGHT_TRIAL; i <= RAND_INF_TRIALS_DONE_SHADOW_TRIAL; i++) {
            if (!Randomizer_IsTrialRequired(i)) {
                Flags_SetRandomizerInf(i);
            }
        }

        // Set Cutscene flags to skip them
        gSaveContext.eventChkInf[0xC] |= 0x10; // returned to tot with medallions
        gSaveContext.eventChkInf[0xC] |= 0x20; //sheik at tot pedestal
        gSaveContext.eventChkInf[4] |= 0x20; // master sword pulled
        gSaveContext.eventChkInf[4] |= 0x8000; // entered master sword chamber
        gSaveContext.infTable[0] |= 1;
        if (!Randomizer_GetSettingValue(RSK_ENABLE_GLITCH_CUTSCENES)) {
            gSaveContext.infTable[17] |= 0x400; // Darunia in Fire Temple
        }
        gSaveContext.cutsceneIndex = 0;
        Flags_SetEventChkInf(5);

        // Skip boss cutscenes
        gSaveContext.eventChkInf[7] |= 1; // gohma
        gSaveContext.eventChkInf[7] |= 2; // dodongo
        gSaveContext.eventChkInf[7] |= 4; // phantom ganon
        gSaveContext.eventChkInf[7] |= 8; // volvagia
        gSaveContext.eventChkInf[7] |= 0x10; // morpha
        gSaveContext.eventChkInf[7] |= 0x20; // twinrova
        gSaveContext.eventChkInf[7] |= 0x40; // barinade
        gSaveContext.eventChkInf[7] |= 0x80; // bongo bongo

        // Skip cutscene before Nabooru fight
        gSaveContext.eventChkInf[3] |= 0x800;
        gSaveContext.eventChkInf[12] |= 1;

        // shuffle adult trade quest
        if (Randomizer_GetSettingValue(RSK_SHUFFLE_ADULT_TRADE)) {
            gSaveContext.adultTradeItems = 0;
        }

        // Starts pending ice traps out at 0 before potentially incrementing them down the line.
        gSaveContext.pendingIceTrapCount = 0;

        // Give Link's pocket item
        GiveLinksPocketItem();

        int openForest = Randomizer_GetSettingValue(RSK_FOREST);
        switch (openForest) {
            case RO_FOREST_CLOSED:
                break;
            case RO_FOREST_CLOSED_DEKU:
                Flags_SetEventChkInf(7);
                break;
            case RO_FOREST_OPEN:
                Flags_SetEventChkInf(7);
                gSaveContext.eventChkInf[0] |= 0x10;
                break;
        }

        int startingAge = Randomizer_GetSettingValue(RSK_STARTING_AGE);
        switch (startingAge) {
            case RO_AGE_ADULT: //Adult
                gSaveContext.linkAge = 0;
                gSaveContext.entranceIndex = 0x5F4;
                gSaveContext.savedSceneNum = SCENE_SPOT20; //Set scene num manually to ToT
                break;
            case RO_AGE_CHILD: //Child
                gSaveContext.linkAge = 1;
                gSaveContext.savedSceneNum = -1;
                break;
            default:
                break;
        }

        if (Randomizer_GetSettingValue(RSK_SHUFFLE_OVERWORLD_SPAWNS)) {
            // Override the spawn entrance so entrance rando can take control,
            // and to prevent remember save location from breaking inital spawn
            gSaveContext.entranceIndex = -1;
        }

        int doorOfTime = Randomizer_GetSettingValue(RSK_DOOR_OF_TIME);
        switch (doorOfTime) {
            case RO_DOOROFTIME_OPEN:
                gSaveContext.eventChkInf[4] |= 0x800;
                break;
        }

        if (Randomizer_GetSettingValue(RSK_KAK_GATE) == RO_KAK_GATE_OPEN) {
            gSaveContext.infTable[7] |= 0x40;
        }
      
        if(Randomizer_GetSettingValue(RSK_STARTING_KOKIRI_SWORD)) Item_Give(NULL, ITEM_SWORD_KOKIRI);
        if(Randomizer_GetSettingValue(RSK_STARTING_DEKU_SHIELD)) Item_Give(NULL, ITEM_SHIELD_DEKU);

        if(Randomizer_GetSettingValue(RSK_STARTING_SKULLTULA_TOKEN)) {
            gSaveContext.inventory.questItems |= gBitFlags[QUEST_SKULL_TOKEN];
            gSaveContext.inventory.gsTokens = Randomizer_GetSettingValue(RSK_STARTING_SKULLTULA_TOKEN);
        }

        if(Randomizer_GetSettingValue(RSK_STARTING_OCARINA)) {
            INV_CONTENT(ITEM_OCARINA_FAIRY) = ITEM_OCARINA_FAIRY;
        }

        if(Randomizer_GetSettingValue(RSK_STARTING_MAPS_COMPASSES) == RO_DUNGEON_ITEM_LOC_STARTWITH) {
            uint32_t mapBitMask = 1 << 1;
            uint32_t compassBitMask = 1 << 2;
            uint32_t startingDungeonItemsBitMask = mapBitMask | compassBitMask;
            for(int scene = SCENE_YDAN; scene <= SCENE_ICE_DOUKUTO; scene++) {
                gSaveContext.inventory.dungeonItems[scene] |= startingDungeonItemsBitMask;
            }
        }

        if (Randomizer_GetSettingValue(RSK_STARTING_CONSUMABLES)) {
            GiveLinkDekuSticks(10);
            GiveLinkDekuNuts(20);
        }

        if(Randomizer_GetSettingValue(RSK_SKIP_CHILD_ZELDA)) {
            GetItemEntry getItem = Randomizer_GetItemFromKnownCheck(RC_SONG_FROM_IMPA, RG_ZELDAS_LULLABY);
            s32 giid = getItem.getItemId;

            if (getItem.modIndex == MOD_NONE) {
                if (getItem.getItemId == GI_SWORD_BGS) {
                    gSaveContext.bgsFlag = true;
                }
                Item_Give(NULL, getItem.itemId);
            } else if (getItem.modIndex == MOD_RANDOMIZER) {
                if (getItem.getItemId == RG_ICE_TRAP) {
                    gSaveContext.pendingIceTrapCount++;
                } else {
                    Randomizer_Item_Give(NULL, getItem);
                }
            }

            // malon/talon back at ranch
            gSaveContext.eventChkInf[1] |= (1 << 0);
            gSaveContext.eventChkInf[1] |= (1 << 2);
            gSaveContext.eventChkInf[1] |= (1 << 3);
            gSaveContext.eventChkInf[1] |= (1 << 4);

            // Set "Got Zelda's Letter" flag. Also ensures Saria is back at SFM. TODO: Is this flag used for anything else?
            gSaveContext.eventChkInf[4] |= 1;

            // Got item from impa
            gSaveContext.eventChkInf[5] |= 0x200;

            // set this at the end to ensure we always start with the letter
            // this is for the off chance we got the weird egg from impa (which should never happen)
            INV_CONTENT(ITEM_LETTER_ZELDA) = ITEM_LETTER_ZELDA;
        }

        if (Randomizer_GetSettingValue(RSK_FULL_WALLETS)) {
            GiveLinkRupees(9001);
        }

        if (Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_STARTWITH) {
            gSaveContext.inventory.dungeonKeys[SCENE_BMORI1] = FOREST_TEMPLE_SMALL_KEY_MAX; // Forest
            gSaveContext.sohStats.dungeonKeys[SCENE_BMORI1]     = FOREST_TEMPLE_SMALL_KEY_MAX; // Forest
            gSaveContext.inventory.dungeonKeys[SCENE_HIDAN] = FIRE_TEMPLE_SMALL_KEY_MAX; // Fire
            gSaveContext.sohStats.dungeonKeys[SCENE_HIDAN]     = FIRE_TEMPLE_SMALL_KEY_MAX; // Fire
            gSaveContext.inventory.dungeonKeys[SCENE_MIZUSIN] = WATER_TEMPLE_SMALL_KEY_MAX; // Water
            gSaveContext.sohStats.dungeonKeys[SCENE_MIZUSIN]     = WATER_TEMPLE_SMALL_KEY_MAX; // Water
            gSaveContext.inventory.dungeonKeys[SCENE_JYASINZOU] = SPIRIT_TEMPLE_SMALL_KEY_MAX; // Spirit
            gSaveContext.sohStats.dungeonKeys[SCENE_JYASINZOU]     = SPIRIT_TEMPLE_SMALL_KEY_MAX; // Spirit
            gSaveContext.inventory.dungeonKeys[SCENE_HAKADAN] = SHADOW_TEMPLE_SMALL_KEY_MAX; // Shadow
            gSaveContext.sohStats.dungeonKeys[SCENE_HAKADAN]     = SHADOW_TEMPLE_SMALL_KEY_MAX; // Shadow
            gSaveContext.inventory.dungeonKeys[SCENE_HAKADANCH] = BOTTOM_OF_THE_WELL_SMALL_KEY_MAX; // BotW
            gSaveContext.sohStats.dungeonKeys[SCENE_HAKADANCH]     = BOTTOM_OF_THE_WELL_SMALL_KEY_MAX; // BotW
            gSaveContext.inventory.dungeonKeys[SCENE_MEN] = GERUDO_TRAINING_GROUNDS_SMALL_KEY_MAX; // GTG
            gSaveContext.sohStats.dungeonKeys[SCENE_MEN]     = GERUDO_TRAINING_GROUNDS_SMALL_KEY_MAX; // GTG
            gSaveContext.inventory.dungeonKeys[SCENE_GANONTIKA] = GANONS_CASTLE_SMALL_KEY_MAX; // Ganon
            gSaveContext.sohStats.dungeonKeys[SCENE_GANONTIKA]     = GANONS_CASTLE_SMALL_KEY_MAX; // Ganon
        } else if (Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_VANILLA) {
            // Logic cannot handle vanilla key layout in some dungeons
            // this is because vanilla expects the dungeon major item to be
            // locked behind the keys, which is not always true in rando.
            // We can resolve this by starting with some extra keys
            if (ResourceMgr_IsSceneMasterQuest(SCENE_JYASINZOU)) {
                // MQ Spirit needs 3 keys
                gSaveContext.inventory.dungeonKeys[SCENE_JYASINZOU] = 3;
                gSaveContext.sohStats.dungeonKeys[SCENE_JYASINZOU] = 3;
            }
        }

        if(Randomizer_GetSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_STARTWITH) {
            gSaveContext.inventory.dungeonItems[SCENE_BMORI1] |= 1; // Forest
            gSaveContext.inventory.dungeonItems[SCENE_HIDAN] |= 1; // Fire
            gSaveContext.inventory.dungeonItems[SCENE_MIZUSIN] |= 1; // Water
            gSaveContext.inventory.dungeonItems[SCENE_JYASINZOU] |= 1; // Spirit
            gSaveContext.inventory.dungeonItems[SCENE_HAKADAN] |= 1; // Shadow
        }

        if(Randomizer_GetSettingValue(RSK_GANONS_BOSS_KEY) == RO_GANON_BOSS_KEY_STARTWITH) {
            gSaveContext.inventory.dungeonItems[SCENE_GANON] |= 1;
        }

        HIGH_SCORE(HS_POE_POINTS) = 1000 - (100 * Randomizer_GetSettingValue(RSK_BIG_POE_COUNT));

        if(Randomizer_GetSettingValue(RSK_SKIP_EPONA_RACE)) {
            gSaveContext.eventChkInf[1] |= (1 << 8);
        }

        // skip the z target talk instructions by the kokiri shop
        gSaveContext.sceneFlags[85].swch |= (1 << 0x1F);

        //Ruto already met in jabu and spawns down the hole immediately
        gSaveContext.infTable[20] |= 2;
        gSaveContext.infTable[20] |= 4;

        // Go away ruto (water temple first cutscene)
        gSaveContext.sceneFlags[SCENE_MIZUSIN].swch |= (1 << 0x10);

        // Open lowest Vanilla Fire Temple locked door (to prevent key logic lockouts)
        // Not done on keysanity since this lockout is a non issue when Fire keys can be found outside the temple
        u8 keysanity = Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANYWHERE ||
                       Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_OVERWORLD ||
                       Randomizer_GetSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANY_DUNGEON;
        if (!ResourceMgr_IsSceneMasterQuest(SCENE_HIDAN) && !keysanity) {
            gSaveContext.sceneFlags[SCENE_HIDAN].swch |= (1 << 0x17);
        }

        // Opens locked Water Temple door in vanilla to prevent softlocks
        // West door on the middle level that leads to the water raising thing
        // Happens in 3DS rando and N64 rando as well
        if (!ResourceMgr_IsSceneMasterQuest(SCENE_MIZUSIN)) {
            gSaveContext.sceneFlags[SCENE_MIZUSIN].swch |= (1 << 0x15);
        }

        // Skip intro cutscene when bombing mud wall in Dodongo's cavern
        // this also makes the lower jaw render, and the eyes react to explosives
        Flags_SetEventChkInf(0xB0);

        // skip verbose lake owl, skip to "i'm on my way back to the castle"
        gSaveContext.infTable[25] |= 0x20;

        if (Randomizer_GetSettingValue(RSK_GERUDO_FORTRESS) == RO_GF_FAST ||
            Randomizer_GetSettingValue(RSK_GERUDO_FORTRESS) == RO_GF_OPEN) {
            gSaveContext.eventChkInf[9] |= 2;
            gSaveContext.eventChkInf[9] |= 4;
            gSaveContext.eventChkInf[9] |= 8;
            gSaveContext.sceneFlags[12].swch |= (1 << 0x02);
            gSaveContext.sceneFlags[12].swch |= (1 << 0x03);
            gSaveContext.sceneFlags[12].swch |= (1 << 0x04);
            gSaveContext.sceneFlags[12].swch |= (1 << 0x06);
            gSaveContext.sceneFlags[12].swch |= (1 << 0x07);
            gSaveContext.sceneFlags[12].swch |= (1 << 0x08);
            gSaveContext.sceneFlags[12].swch |= (1 << 0x10);
            gSaveContext.sceneFlags[12].swch |= (1 << 0x12);
            gSaveContext.sceneFlags[12].swch |= (1 << 0x13);
            gSaveContext.sceneFlags[12].collect |= (1 << 0x0A);
            gSaveContext.sceneFlags[12].collect |= (1 << 0x0E);
            gSaveContext.sceneFlags[12].collect |= (1 << 0x0F);
        }

        if (Randomizer_GetSettingValue(RSK_GERUDO_FORTRESS) == RO_GF_OPEN) {
            gSaveContext.eventChkInf[9] |= 1;
            gSaveContext.sceneFlags[12].swch |= (1 << 0x01);
            gSaveContext.sceneFlags[12].swch |= (1 << 0x05);
            gSaveContext.sceneFlags[12].swch |= (1 << 0x11);
            gSaveContext.sceneFlags[12].collect |= (1 << 0x0C);

            if (!Randomizer_GetSettingValue(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD)) {
                Item_Give(NULL, ITEM_GERUDO_CARD);
            }
        }

        // complete mask quest
        if (Randomizer_GetSettingValue(RSK_COMPLETE_MASK_QUEST)) {
            gSaveContext.infTable[7] |= 0x80;      // Soldier Wears Keaton Mask
            gSaveContext.itemGetInf[3] |= 0x100;   // Sold Keaton Mask
            gSaveContext.itemGetInf[3] |= 0x200;   // Sold Skull Mask
            gSaveContext.itemGetInf[3] |= 0x400;   // Sold Spooky Mask
            gSaveContext.itemGetInf[3] |= 0x800;   // bunny hood related
            gSaveContext.itemGetInf[3] |= 0x8000;  // Obtained Mask of Truth
            gSaveContext.eventChkInf[8] |= 0x8000; // sold all masks
        }
    }

    Save_SaveFile();
}

void Sram_InitSram(GameState* gameState) {
    Save_Init();

    func_800F6700(gSaveContext.audioSetting);

    // When going from a rando save to a vanilla save within the same game instance
    // we need to reset the entrance table back to its vanilla state
    Entrance_ResetEntranceTable();
    // Clear out the entrance tracker
    Entrance_ClearEntranceTrackingData();
}
