#include "global.h"
#include "vt.h"

#include <string.h>

#define NUM_DUNGEONS 8
#define NUM_TRIALS 6

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
// nothing breaks when calling it without a valid globalCtx first
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

void GiveLinkRupeesByGetItemId(GetItemID giid) {
    if (giid == GI_RUPEE_GREEN) {
        GiveLinkRupees(1);
    } else if (giid == GI_RUPEE_BLUE) {
        GiveLinkRupees(5);
    } else if (giid == GI_RUPEE_RED) {
        GiveLinkRupees(20);
    } else if (giid == GI_RUPEE_PURPLE) {
        GiveLinkRupees(50);
    } else if (giid == GI_RUPEE_GOLD) {
        GiveLinkRupees(100);
    }
}

void GiveLinkBombchus(GetItemID giid) {
    INV_CONTENT(ITEM_BOMBCHU) = ITEM_BOMBCHU;
    if (giid == GI_BOMBCHUS_5) {
        AMMO(ITEM_BOMBCHU) += 5; 
    } else if (giid == GI_BOMBCHUS_10) {
        AMMO(ITEM_BOMBCHU) += 10; 
    } else if (giid == GI_BOMBCHUS_20) {
        AMMO(ITEM_BOMBCHU) += 20; 
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

void GiveLinkDekuSticksByGetItemId(GetItemID giid) {
    if (giid == GI_STICKS_1) {
        GiveLinkDekuSticks(1); 
    } else if (giid == GI_STICKS_5) {
        GiveLinkDekuSticks(5); 
    } else if (giid == GI_STICKS_10) {
        GiveLinkDekuSticks(10); 
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

void GiveLinkDekuNutsByGetItemId(GetItemID giid) {
    if (giid == GI_NUTS_5) {
        GiveLinkDekuNuts(5); 
    } else if (giid == GI_NUTS_10) {
        GiveLinkDekuNuts(10); 
    }
}

void GiveLinkBeans() {
    INV_CONTENT(ITEM_BEAN) = ITEM_BEAN;
    AMMO(ITEM_BEAN)++;
}

void GiveLinkKokiriSword() {
    uint32_t bitMask = 1 << 0;
    gSaveContext.inventory.equipment |= bitMask;
}

void GiveLinkGiantsKnife() {
    gSaveContext.bgsFlag = 0;
    gSaveContext.swordHealth = 8;
    uint32_t bitMask = 1 << 2;
    gSaveContext.inventory.equipment |= bitMask;
}

void GiveLinkBiggoronSword() {
    gSaveContext.bgsFlag = 1;
    gSaveContext.swordHealth = 8;
    uint32_t bitMask = 1 << 2;
    gSaveContext.inventory.equipment |= bitMask;
}

void GiveLinkDekuShield() {
    uint32_t bitMask = 1 << 4;
    gSaveContext.inventory.equipment |= bitMask;
}

void GiveLinkHylianShield() {
    uint32_t bitMask = 1 << 5;
    gSaveContext.inventory.equipment |= bitMask;
}

void GiveLinkMirrorShield() {
    uint32_t bitMask = 1 << 6;
    gSaveContext.inventory.equipment |= bitMask;
}

void GiveLinkGoronTunic() {
    uint32_t bitMask = 1 << 9;
    gSaveContext.inventory.equipment |= bitMask;
}

void GiveLinkZoraTunic() {
    uint32_t bitMask = 1 << 10;
    gSaveContext.inventory.equipment |= bitMask;
}

void GiveLinkIronBoots() {
    uint32_t bitMask = 1 << 13;
    gSaveContext.inventory.equipment |= bitMask;
}

void GiveLinkHoverBoots() {
    uint32_t bitMask = 1 << 14;
    gSaveContext.inventory.equipment |= bitMask;
}

void GiveLinkStoneOfAgony() {
    uint32_t bitMask = 1 << QUEST_STONE_OF_AGONY;
    gSaveContext.inventory.questItems |= bitMask;    
}

void GiveLinkGerudoCard() {
    uint32_t bitMask = 1 << QUEST_GERUDO_CARD;
    gSaveContext.inventory.questItems |= bitMask;    
}

void GiveLinkPieceOfHeart() {
    int32_t pohCount = (gSaveContext.inventory.questItems & 0xF0000000) >> 28;
    pohCount++;
    gSaveContext.inventory.questItems |= (pohCount << 28);
}

void GiveLinkHeartContainer() {
    gSaveContext.healthCapacity += 16;
    gSaveContext.health += 16;
}

void GiveLinkBulletBagUpgrade(GetItemID giid) {
    if (giid == GI_SLINGSHOT) {
        INV_CONTENT(ITEM_SLINGSHOT) = ITEM_SLINGSHOT;
        AMMO(ITEM_SLINGSHOT) = 30;
        Inventory_ChangeUpgrade(UPG_BULLET_BAG, 1);
    } else if (giid == GI_BULLET_BAG_40) {
        Inventory_ChangeUpgrade(UPG_BULLET_BAG, 2);
        AMMO(ITEM_SLINGSHOT) = 40;
    } else if (giid == GI_BULLET_BAG_50) {
        Inventory_ChangeUpgrade(UPG_BULLET_BAG, 3);
        AMMO(ITEM_SLINGSHOT) = 50;
    }
}

void GiveLinkQuiverUpgrade(GetItemID giid) {
    if (giid == GI_BOW) {
        INV_CONTENT(ITEM_BOW) = ITEM_BOW;
        Inventory_ChangeUpgrade(UPG_QUIVER, 1);
        AMMO(ITEM_BOW) = 30;
    } else if (giid == GI_QUIVER_40) {
        Inventory_ChangeUpgrade(UPG_QUIVER, 2);
        AMMO(ITEM_BOW) = 40;
    } else if (giid == GI_QUIVER_50) {
        Inventory_ChangeUpgrade(UPG_QUIVER, 3);
        AMMO(ITEM_BOW) = 50;
    }
}

void GiveLinkBombBagUpgrade(GetItemID giid) {
    if (giid == GI_BOMB_BAG_20) {
        INV_CONTENT(ITEM_BOMB) = ITEM_BOMB;
        Inventory_ChangeUpgrade(UPG_BOMB_BAG, 1);
        AMMO(ITEM_BOMB) = 20;
    } else if (giid == GI_BOMB_BAG_30) {
        Inventory_ChangeUpgrade(UPG_BOMB_BAG, 2);
        AMMO(ITEM_BOMB) = 30;
    } else if (giid == GI_BOMB_BAG_40) {
        Inventory_ChangeUpgrade(UPG_BOMB_BAG, 3);
        AMMO(ITEM_BOMB) = 40;
    }
}

void GiveLinkStrengthUpgrade(GetItemID giid) {
    if (giid == GI_BRACELET) {
        Inventory_ChangeUpgrade(UPG_STRENGTH, 1);
    } else if (giid == GI_GAUNTLETS_SILVER) {
        Inventory_ChangeUpgrade(UPG_STRENGTH, 2);
    } else if (giid == GI_GAUNTLETS_GOLD) {
        Inventory_ChangeUpgrade(UPG_STRENGTH, 3);
    }
}

void GiveLinkScaleUpgrade(GetItemID giid) {
    if (giid == GI_SCALE_SILVER) {
        Inventory_ChangeUpgrade(UPG_SCALE, 1);
    } else if (giid == GI_SCALE_GOLD) {
        Inventory_ChangeUpgrade(UPG_SCALE, 2);
    }
}

void GiveLinkWalletUpgrade(GetItemID giid) {
    if (giid == GI_WALLET_ADULT) {
        Inventory_ChangeUpgrade(UPG_WALLET, 1);
    } else if (giid == GI_WALLET_GIANT) {
        Inventory_ChangeUpgrade(UPG_WALLET, 2);
    }
}

void GiveLinkDekuStickUpgrade(GetItemID giid) {
    if (giid == GI_STICK_UPGRADE_20) {
        INV_CONTENT(ITEM_STICK) = ITEM_STICK;
        Inventory_ChangeUpgrade(UPG_STICKS, 2);
        AMMO(ITEM_STICK) = 20;
    } else if (giid == GI_STICK_UPGRADE_30) {
        Inventory_ChangeUpgrade(UPG_STICKS, 3);
        AMMO(ITEM_STICK) = 30;
    }
}

void GiveLinkDekuNutUpgrade(GetItemID giid) {
    if (giid == GI_NUT_UPGRADE_30) {
        INV_CONTENT(ITEM_NUT) = ITEM_NUT;
        Inventory_ChangeUpgrade(UPG_NUTS, 2);
        AMMO(ITEM_NUT) = 30;
    } else if (giid == GI_NUT_UPGRADE_40) {
        Inventory_ChangeUpgrade(UPG_NUTS, 3);
        AMMO(ITEM_NUT) = 40;
    }
}

void GiveLinkMagic(GetItemID giid) {
    if (giid == GI_SINGLE_MAGIC) {
        gSaveContext.magicLevel = 1;
        gSaveContext.magicAcquired = true;
        gSaveContext.doubleMagic = false;
    } else if (giid == GI_DOUBLE_MAGIC) {
        gSaveContext.magicLevel = 2;
        gSaveContext.magicAcquired = true;
        gSaveContext.doubleMagic = true;
    }
}

void GiveLinkDoubleDefense() {
    gSaveContext.doubleDefense = 1;
    gSaveContext.inventory.defenseHearts = 20;
}

void GiveLinkSong(GetItemID getItemId) {
    uint32_t bitMask;

    switch (getItemId) {
        case GI_ZELDAS_LULLABY:
            bitMask = 1 << QUEST_SONG_LULLABY;
            break;
        case GI_SUNS_SONG:
            bitMask = 1 << QUEST_SONG_SUN;
            break;
        case GI_EPONAS_SONG:
            bitMask = 1 << QUEST_SONG_EPONA;
            break;
        case GI_SONG_OF_STORMS:
            bitMask = 1 << QUEST_SONG_STORMS;
            break;
        case GI_SONG_OF_TIME:
            bitMask = 1 << QUEST_SONG_TIME;
            break;
        case GI_SARIAS_SONG:
            bitMask = 1 << QUEST_SONG_SARIA;
            break;
        case GI_MINUET_OF_FOREST:
            bitMask = 1 << QUEST_SONG_MINUET;
            break;
        case GI_BOLERO_OF_FIRE:
            bitMask = 1 << QUEST_SONG_BOLERO;
            break;
        case GI_SERENADE_OF_WATER:
            bitMask = 1 << QUEST_SONG_SERENADE;
            break;
        case GI_NOCTURNE_OF_SHADOW:
            bitMask = 1 << QUEST_SONG_NOCTURNE;
            break;
        case GI_REQUIEM_OF_SPIRIT:
            bitMask = 1 << QUEST_SONG_REQUIEM;
            break;
        case GI_PRELUDE_OF_LIGHT:
            bitMask = 1 << QUEST_SONG_PRELUDE;
            break;
    }

    gSaveContext.inventory.questItems |= bitMask;
}

void GiveLinkDungeonReward(GetItemID getItemId) {
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

void GiveLinksPocketMedallion() {
    GetItemID getItemId = Randomizer_GetItemIdFromKnownCheck(RC_LINKS_POCKET, RG_NONE);

    GiveLinkDungeonReward(getItemId);
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

    if (CVar_GetS32("gRandomizer", 0) != 0 &&
        strcmp(CVar_GetString("gSpoilerLog", ""), "") != 0) {
        // Set N64DD Flags for save file
        fileChooseCtx->n64ddFlags[fileChooseCtx->buttonIndex] = 1;
        fileChooseCtx->n64ddFlag = 1;
        gSaveContext.n64ddFlag = 1;

        // Sets all the dungeons to incomplete when generating a rando save. Fixes https://github.com/briaguya-ai/rando-issue-tracker/issues/82
        for (u8 i = 0; i < NUM_DUNGEONS; i++) {
            gSaveContext.dungeonsDone[i] = 0;
        }

        // Sets all Ganon's Trials to incomplete when generating a rando save. Fixes https://github.com/briaguya-ai/rando-issue-tracker/issues/131
        for (u8 i = 0; i < NUM_TRIALS; i++) {
            gSaveContext.trialsDone[i] = 0;
        }

        // Set Cutscene flags to skip them
        gSaveContext.eventChkInf[0xC] |= 0x10; // returned to tot with medallions
        gSaveContext.eventChkInf[0xC] |= 0x20; //sheik at tot pedestal
        gSaveContext.eventChkInf[4] |= 0x20; // master sword pulled
        gSaveContext.eventChkInf[4] |= 0x8000; // entered master sword chamber
        gSaveContext.infTable[0] |= 1;
        // RANDTODO: Don't skip this scene if Don't Skip Glitch Useful Cutscenes is enabled.
        gSaveContext.infTable[17] |= 0x400; // Darunia in Fire Temple
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

        // Give Link's pocket item
        GiveLinksPocketMedallion();

        int openForest = Randomizer_GetSettingValue(RSK_FOREST);
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

        int doorOfTime = Randomizer_GetSettingValue(RSK_DOOR_OF_TIME);
        switch (doorOfTime) {
            case 0: // open
                gSaveContext.eventChkInf[4] |= 0x800;
                break;
        }

        int kakGate = Randomizer_GetSettingValue(RSK_KAK_GATE);
        switch (kakGate) {
            case 1: // open
                gSaveContext.infTable[7] |= 0x40;
                break;
        }
      
        if(Randomizer_GetSettingValue(RSK_STARTING_KOKIRI_SWORD)) GiveLinkKokiriSword();
        if(Randomizer_GetSettingValue(RSK_STARTING_DEKU_SHIELD)) GiveLinkDekuShield();

        if(Randomizer_GetSettingValue(RSK_STARTING_OCARINA)) {
            INV_CONTENT(ITEM_OCARINA_FAIRY) = ITEM_OCARINA_FAIRY;
        }

        if(Randomizer_GetSettingValue(RSK_STARTING_MAPS_COMPASSES)) {
            uint32_t mapBitMask = 1 << 1;
            uint32_t compassBitMask = 1 << 2;
            uint32_t startingDungeonItemsBitMask = mapBitMask | compassBitMask;
            for(int scene = 0; scene <= 9; scene++) {
                gSaveContext.inventory.dungeonItems[scene] |= startingDungeonItemsBitMask;
            }
        }

        if (Randomizer_GetSettingValue(RSK_STARTING_CONSUMABLES)) {
            GiveLinkDekuSticks(10);
            GiveLinkDekuNuts(20);
        }

        if(Randomizer_GetSettingValue(RSK_SKIP_CHILD_ZELDA)) {
            s32 giid = Randomizer_GetItemIdFromKnownCheck(RC_SONG_FROM_IMPA, GI_ZELDAS_LULLABY);
            
            if(giid >= GI_ZELDAS_LULLABY && giid <= GI_PRELUDE_OF_LIGHT) {
                GiveLinkSong(giid);
            } else if (giid == GI_RUPEE_GREEN ||
                       giid == GI_RUPEE_BLUE ||
                       giid == GI_RUPEE_RED ||
                       giid == GI_RUPEE_PURPLE ||
                       giid == GI_RUPEE_GOLD) {
                GiveLinkRupeesByGetItemId(giid);
            } else if (giid == GI_BOMBCHUS_10 ||
                       giid == GI_BOMBCHUS_5 ||
                       giid == GI_BOMBCHUS_20) {
                GiveLinkBombchus(giid);
            } else if (giid == GI_STICKS_1 ||
                       giid == GI_STICKS_5 ||
                       giid == GI_STICKS_10) {
                GiveLinkDekuSticksByGetItemId(giid);
            } else if (giid == GI_NUTS_5 ||
                       giid == GI_NUTS_10) {
                GiveLinkDekuNutsByGetItemId(giid);
            } else if (giid == GI_BEAN) {
                GiveLinkBeans();
            } else if (giid >= GI_MEDALLION_LIGHT && giid <= GI_STONE_ZORA) {
                GiveLinkDungeonReward(giid);
            } else if (giid == GI_SWORD_KOKIRI) {
                GiveLinkKokiriSword();
            } else if (giid == GI_SWORD_BGS) {
                GiveLinkBiggoronSword();
            } else if (giid == GI_SWORD_KNIFE) {
                GiveLinkGiantsKnife();
            } else if (giid == GI_SHIELD_DEKU) {
                GiveLinkDekuShield();
            } else if (giid == GI_SHIELD_HYLIAN) {
                GiveLinkHylianShield();
            } else if (giid == GI_SHIELD_MIRROR) {
                GiveLinkMirrorShield();
            } else if (giid == GI_TUNIC_GORON) {
                GiveLinkGoronTunic();
            } else if (giid == GI_TUNIC_ZORA) {
                GiveLinkZoraTunic();
            } else if (giid == GI_BOOTS_IRON) {
                GiveLinkIronBoots();
            } else if (giid == GI_BOOTS_HOVER) {
                GiveLinkHoverBoots();
            } else if (giid == GI_SLINGSHOT ||
                       giid == GI_BULLET_BAG_40 ||
                       giid == GI_BULLET_BAG_50) {
                GiveLinkBulletBagUpgrade(giid);
            } else if (giid == GI_BOW ||
                       giid == GI_QUIVER_40 ||
                       giid == GI_QUIVER_50) {
                GiveLinkQuiverUpgrade(giid);
            } else if (giid == GI_BOMB_BAG_20 ||
                       giid == GI_BOMB_BAG_30 ||
                       giid == GI_BOMB_BAG_40) {
                GiveLinkBombBagUpgrade(giid);
            } else if (giid == GI_BRACELET ||
                       giid == GI_GAUNTLETS_SILVER ||
                       giid == GI_GAUNTLETS_GOLD) {
                GiveLinkStrengthUpgrade(giid);
            } else if (giid == GI_SCALE_SILVER ||
                       giid == GI_SCALE_GOLD) {
                GiveLinkScaleUpgrade(giid);
            } else if (giid == GI_WALLET_ADULT ||
                       giid == GI_WALLET_GIANT) {
                GiveLinkWalletUpgrade(giid);
            } else if (giid == GI_STONE_OF_AGONY) {
                GiveLinkStoneOfAgony();
            } else if (giid == GI_GERUDO_CARD) {
                GiveLinkGerudoCard();
            } else if (giid == GI_HEART_PIECE) {
                GiveLinkPieceOfHeart();
            } else if (giid == GI_HEART_CONTAINER) {
                GiveLinkHeartContainer();
            } else if (giid == GI_STICK_UPGRADE_20 ||
                       giid == GI_STICK_UPGRADE_30) {
                GiveLinkDekuStickUpgrade(giid);
            } else if (giid == GI_NUT_UPGRADE_30 ||
                       giid == GI_NUT_UPGRADE_40) {
                GiveLinkDekuNutUpgrade(giid);
            } else if (giid == GI_SINGLE_MAGIC ||
                       giid == GI_DOUBLE_MAGIC) {
                GiveLinkMagic(giid);
            } else if (giid == GI_DOUBLE_DEFENSE) {
                GiveLinkDoubleDefense();
            } else {
                s32 iid = Randomizer_GetItemIDFromGetItemID(giid);
                if (iid != -1) INV_CONTENT(iid) = iid;
            }

            // malon/talon back at ranch
            gSaveContext.eventChkInf[1] |= (1 << 0);
            gSaveContext.eventChkInf[1] |= (1 << 2);
            gSaveContext.eventChkInf[1] |= (1 << 3);
            gSaveContext.eventChkInf[1] |= (1 << 4);

            // Got item from impa
            gSaveContext.eventChkInf[5] |= 0x200;

            // make sure saria is at SFM
            gSaveContext.eventChkInf[4] |= (1 << 0);

            // set this at the end to ensure we always start with the letter
            // this is for the off chance we got the weird egg from impa (which should never happen)
            INV_CONTENT(ITEM_LETTER_ZELDA) = ITEM_LETTER_ZELDA;
        }

        if (Randomizer_GetSettingValue(RSK_FULL_WALLETS)) {
            GiveLinkRupees(9001);
        }

        // For Ganon's boss key "Start With" is 0
        if(Randomizer_GetSettingValue(RSK_GANONS_BOSS_KEY) == 0) {
            gSaveContext.inventory.dungeonItems[10] |= 1;
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
        gSaveContext.sceneFlags[05].swch |= (1 << 0x10);

        // Opens locked Water Temple door to prevent softlocks
        // West door on the middle level that leads to the water raising thing
        // Happens in 3DS rando and N64 rando as well
        gSaveContext.sceneFlags[05].swch |= (1 << 0x15);

        // Skip intro cutscene when bombing mud wall in Dodongo's cavern
        // this also makes the lower jaw render, and the eyes react to explosives
        Flags_SetEventChkInf(0xB0);

        // skip verbose lake owl, skip to "i'm on my way back to the castle"
        gSaveContext.infTable[25] |= 0x20;

        // fast gerudo fortress
        if (Randomizer_GetSettingValue(RSK_GERUDO_FORTRESS) == 1 || Randomizer_GetSettingValue(RSK_GERUDO_FORTRESS) == 2) {
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

        // open gerudo fortress
        if (Randomizer_GetSettingValue(RSK_GERUDO_FORTRESS) == 2) {
            gSaveContext.eventChkInf[9] |= 1;
            gSaveContext.sceneFlags[12].swch |= (1 << 0x01);
            gSaveContext.sceneFlags[12].swch |= (1 << 0x05);
            gSaveContext.sceneFlags[12].swch |= (1 << 0x11);
            gSaveContext.sceneFlags[12].collect |= (1 << 0x0C);

            if (!Randomizer_GetSettingValue(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD)) {
                GiveLinkGerudoCard();
            }
        }
    }

    Save_SaveFile();
}

void Sram_InitSram(GameState* gameState) {
    Save_Init();

    func_800F6700(gSaveContext.audioSetting);
}
