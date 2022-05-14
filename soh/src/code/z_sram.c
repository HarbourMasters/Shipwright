#include "global.h"
#include "vt.h"

#include <string.h>

// these are the main substructs of save context.
// we are going to hold off on splitting save context until later on,
// so these temporary structs will live here for now.

typedef struct {
    /* 0x00 */ char newf[6]; // string "ZELDAZ"
    /* 0x06 */ s16 deaths;
    /* 0x08 */ char playerName[8];
    /* 0x10 */ s16 n64ddFlag;
    /* 0x12 */ s16 healthCapacity; // "max_life"
    /* 0x14 */ s16 health;         // "now_life"
    /* 0x16 */ s8 magicLevel;
    /* 0x17 */ s8 magic;
    /* 0x18 */ s16 rupees;
    /* 0x1A */ u16 swordHealth;
    /* 0x1C */ u16 naviTimer;
    /* 0x1E */ u8 magicAcquired;
    /* 0x1F */ u8 unk_1F;
    /* 0x20 */ u8 doubleMagic;
    /* 0x21 */ u8 doubleDefense;
    /* 0x22 */ u8 bgsFlag;
    /* 0x23 */ u8 ocarinaGameRoundNum;
    /* 0x24 */ ItemEquips childEquips;
    /* 0x2E */ ItemEquips adultEquips;
    /* 0x38 */ u32 unk_38; // this may be incorrect, currently used for alignement
    /* 0x3C */ char unk_3C[0x0E];
    /* 0x4A */ s16 savedSceneNum;
} SavePlayerData; // size = 0x4C

typedef struct {
    /* 0x0000 */ SavePlayerData playerData; // "S_Private" substruct name
    /* 0x004C */ ItemEquips equips;
    /* 0x0058 */ Inventory inventory;
    /* 0x00B8 */ SavedSceneFlags sceneFlags[124];
    /* 0x0E48 */ FaroresWindData fw;
    /* 0x0E70 */ char unk_E70[0x10];
    /* 0x0E80 */ s32 gsFlags[6];
    /* 0x0E98 */ char unk_E98[0x10];
    /* 0x0EA8 */ s32 horseRaceRecord;
    /* 0x0EAC */ char unk_EAC[0x0C];
    /* 0x0EB8 */ u16 eventChkInf[14]; // "event_chk_inf"
    /* 0x0ED4 */ u16 itemGetInf[4];   // "item_get_inf"
    /* 0x0EDC */ u16 infTable[30];    // "inf_table"
    /* 0x0F18 */ char unk_F18[0x04];
    /* 0x0F1C */ u32 worldMapAreaData; // "area_arrival"
    /* 0x0F20 */ char unk_F20[0x4];
    /* 0x0F24 */ u8 scarecrowCustomSongSet;
    /* 0x0F25 */ u8 scarecrowCustomSong[0x360];
    /* 0x1285 */ char unk_1285[0x24];
    /* 0x12A9 */ u8 scarecrowSpawnSongSet;
    /* 0x12AA */ u8 scarecrowSpawnSong[0x80];
    /* 0x132A */ char unk_132A[0x02];
    /* 0x132C */ HorseData horseData;
    /* 0x1336 */ u16 checksum; // "check_sum"
} SaveInfo;                    // size = 0x1338

typedef struct {
    /* 0x00 */ s32 entranceIndex;
    /* 0x04 */ s32 linkAge; // 0: Adult; 1: Child
    /* 0x08 */ s32 cutsceneIndex;
    /* 0x0C */ u16 dayTime; // "zelda_time"
    /* 0x10 */ s32 nightFlag;
    /* 0x14 */ s32 totalDays;
    /* 0x18 */ s32 unk_18;    // increments with totalDays, gets reset by goron for bgs and one other use
    /* 0x1C */ SaveInfo info; // "information"
} Save;                       // size = 0x1354

#define SAVE_PLAYER_DATA (*((SavePlayerData*)&gSaveContext.newf))
#define SAVE_INFO (*((SaveInfo*)&gSaveContext.newf))

#define SLOT_SIZE (sizeof(SaveContext) + 0x28)
#define CHECKSUM_SIZE (sizeof(Save) / 2)

#define DEATHS OFFSETOF(SaveContext, deaths)
#define NAME OFFSETOF(SaveContext, playerName)
#define N64DD OFFSETOF(SaveContext, n64ddFlag)
#define HEALTH_CAP OFFSETOF(SaveContext, healthCapacity)
#define QUEST OFFSETOF(SaveContext, inventory.questItems)
#define DEFENSE OFFSETOF(SaveContext, inventory.defenseHearts)
#define HEALTH OFFSETOF(SaveContext, health)

#define SLOT_OFFSET(index) (SRAM_HEADER_SIZE + 0x10 + (index * SLOT_SIZE))

u16 gSramSlotOffsets[] = {
    SLOT_OFFSET(0),
    SLOT_OFFSET(1),
    SLOT_OFFSET(2),
    // the latter three saves are backup saves for the former saves
    SLOT_OFFSET(3),
    SLOT_OFFSET(4),
    SLOT_OFFSET(5),
};

static u8 sZeldaMagic[] = { '\0', '\0', '\0', '\x98', '\x09', '\x10', '\x21', 'Z', 'E', 'L', 'D', 'A' };

static SavePlayerData sNewSavePlayerData = {
    { '\0', '\0', '\0', '\0', '\0', '\0' },             // newf
    0,                                                  // deaths
    { 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E }, // playerName
    0,                                                  // n64ddFlag
    0x30,                                               // healthCapacity
    0x30,                                               // defense
    0,                                                  // magicLevel
    0x30,                                               // magic
    0,                                                  // rupees
    0,                                                  // swordHealth
    0,                                                  // naviTimer
    0,                                                  // magicAcquired
    0,                                                  // unk_1F
    0,                                                  // doubleMagic
    0,                                                  // doubleDefense
    0,                                                  // bgsFlag
    0,                                                  // ocarinaGameRoundNum
    {
        { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE }, // buttonItems
        { SLOT_NONE, SLOT_NONE, SLOT_NONE },            // cButtonSlots
        0,                                              // equipment
    },                                                  // childEquips
    {
        { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE }, // buttonItems
        { SLOT_NONE, SLOT_NONE, SLOT_NONE },            // cButtonSlots
        0,                                              // equipment
    },                                                  // adultEquips
    0,                                                  // unk_38
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },       // unk_3C
    0x34,                                               // savedSceneNum
};

static ItemEquips sNewSaveEquips = {
    { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE }, // buttonItems
    { SLOT_NONE, SLOT_NONE, SLOT_NONE },            // cButtonSlots
    0x1100,                                         // equipment
};

static Inventory sNewSaveInventory = {
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, // items
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },                         // ammo
    0x1100,                                                                     // equipment
    0,                                                                          // upgrades
    0,                                                                          // questItems
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },             // dungeonItems
    {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    }, // dungeonKeys
    0, // defenseHearts
    0, // gsTokens
};

static u16 sNewSaveChecksum = 0;

/**
 *  Initialize new save.
 *  This save has an empty inventory with 3 hearts and single magic.
 */
void Sram_InitNewSave(void) {
    SaveContext* temp = &gSaveContext;

    memset(&SAVE_INFO, 0, sizeof(SaveInfo));
    gSaveContext.totalDays = 0;
    gSaveContext.bgsDayCount = 0;

    SAVE_PLAYER_DATA = sNewSavePlayerData;
    gSaveContext.equips = sNewSaveEquips;
    gSaveContext.inventory = sNewSaveInventory;

    temp->checksum = sNewSaveChecksum;
    gSaveContext.horseData.scene = SCENE_SPOT00;
    gSaveContext.horseData.pos.x = -1840;
    gSaveContext.horseData.pos.y = 72;
    gSaveContext.horseData.pos.z = 5497;
    gSaveContext.horseData.angle = -0x6AD9;
    gSaveContext.magicLevel = 0;
    gSaveContext.infTable[29] = 1;
    gSaveContext.sceneFlags[5].swch = 0x40000000;
}

static SavePlayerData sDebugSavePlayerData = {
    { 'Z', 'E', 'L', 'D', 'A', 'Z' },                   // newf
    0,                                                  // deaths
    { 0x15, 0x12, 0x17, 0x14, 0x3E, 0x3E, 0x3E, 0x3E }, // playerName ( "LINK" )
    0,                                                  // n64ddFlag
    0xE0,                                               // healthCapacity
    0xE0,                                               // health
    0,                                                  // magicLevel
    0x30,                                               // magic
    150,                                                // rupees
    8,                                                  // swordHealth
    0,                                                  // naviTimer
    1,                                                  // magicAcquired
    0,                                                  // unk_1F
    0,                                                  // doubleMagic
    0,                                                  // doubleDefense
    0,                                                  // bgsFlag
    0,                                                  // ocarinaGameRoundNum
    {
        { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE }, // buttonItems
        { SLOT_NONE, SLOT_NONE, SLOT_NONE },            // cButtonSlots
        0,                                              // equipment
    },                                                  // childEquips
    {
        { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE }, // buttonItems
        { SLOT_NONE, SLOT_NONE, SLOT_NONE },            // cButtonSlots
        0,                                              // equipment
    },                                                  // adultEquips
    0,                                                  // unk_38
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },       // unk_3C
    0x51,                                               // savedSceneNum
};

static ItemEquips sDebugSaveEquips = {
    { ITEM_SWORD_MASTER, ITEM_BOW, ITEM_BOMB, ITEM_OCARINA_FAIRY }, // buttonItems
    { SLOT_BOW, SLOT_BOMB, SLOT_OCARINA },                          // cButtonSlots
    0x1122,                                                         // equipment
};

static Inventory sDebugSaveInventory = {
    {
        ITEM_STICK,     ITEM_NUT,           ITEM_BOMB,         ITEM_BOW,         ITEM_ARROW_FIRE,  ITEM_DINS_FIRE,
        ITEM_SLINGSHOT, ITEM_OCARINA_FAIRY, ITEM_BOMBCHU,      ITEM_HOOKSHOT,    ITEM_ARROW_ICE,   ITEM_FARORES_WIND,
        ITEM_BOOMERANG, ITEM_LENS,          ITEM_BEAN,         ITEM_HAMMER,      ITEM_ARROW_LIGHT, ITEM_NAYRUS_LOVE,
        ITEM_BOTTLE,    ITEM_POTION_RED,    ITEM_POTION_GREEN, ITEM_POTION_BLUE, ITEM_POCKET_EGG,  ITEM_WEIRD_EGG,
    },                                                              // items
    { 50, 50, 10, 30, 1, 1, 30, 1, 50, 1, 1, 1, 1, 1, 1, 1 },       // ammo
    0x7777,                                                         // equipment
    0x125249,                                                       // upgrades
    0x1E3FFFF,                                                      // questItems
    { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // dungeonItems
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 },    // dungeonKeys
    0,                                                              // defenseHearts
    0,                                                              // gsTokens
};

static u16 sDebugSaveChecksum = 0;

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
    SaveContext* temp = &gSaveContext;

    memset(&SAVE_INFO, 0, sizeof(SaveInfo));
    gSaveContext.totalDays = 0;
    gSaveContext.bgsDayCount = 0;

    SAVE_PLAYER_DATA = sDebugSavePlayerData;
    gSaveContext.equips = sDebugSaveEquips;
    gSaveContext.inventory = sDebugSaveInventory;

    temp->checksum = sDebugSaveChecksum;
    gSaveContext.horseData.scene = SCENE_SPOT00;
    gSaveContext.horseData.pos.x = -1840;
    gSaveContext.horseData.pos.y = 72;
    gSaveContext.horseData.pos.z = 5497;
    gSaveContext.horseData.angle = -0x6AD9;
    gSaveContext.infTable[0] |= 0x5009;
    gSaveContext.eventChkInf[0] |= 0x123F;
    gSaveContext.eventChkInf[8] |= 1;
    gSaveContext.eventChkInf[12] |= 0x10;

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        gSaveContext.equips.buttonItems[0] = ITEM_SWORD_KOKIRI;
        Inventory_ChangeEquipment(EQUIP_SWORD, 1);
        if (gSaveContext.fileNum == 0xFF) {
            gSaveContext.equips.buttonItems[1] = ITEM_SLINGSHOT;
            gSaveContext.equips.cButtonSlots[0] = SLOT_SLINGSHOT;
            Inventory_ChangeEquipment(EQUIP_SHIELD, 1);
        }
    }

    gSaveContext.entranceIndex = 0xCD;
    gSaveContext.magicLevel = 0;
    gSaveContext.sceneFlags[5].swch = 0x40000000;
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
void Sram_OpenSave(SramContext* sramCtx) {
    static s16 dungeonEntrances[] = {
        0x0000, 0x0004, 0x0028, 0x0169, 0x0165, 0x0010, 0x0082, 0x0037,
        0x0098, 0x0088, 0x041B, 0x0008, 0x0486, 0x0467, 0x0179, 0x056C,
    };
    u16 i;
    u16 j;
    u8* ptr;

    osSyncPrintf("個人Ｆｉｌｅ作成\n"); // "Create personal file"
    i = gSramSlotOffsets[gSaveContext.fileNum];
    osSyncPrintf("ぽいんと＝%x(%d)\n", i, gSaveContext.fileNum); // "Point="

    memcpy(&gSaveContext, sramCtx->readBuff + i, sizeof(Save));

    osSyncPrintf(VT_FGCOL(YELLOW));
    osSyncPrintf("SCENE_DATA_ID = %d   SceneNo = %d\n", gSaveContext.savedSceneNum,
                 ((void)0, gSaveContext.entranceIndex));

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
        case SCENE_KAKUSIANA:
            if (gSaveContext.savedSceneNum != SCENE_LINK_HOME) {
                gSaveContext.entranceIndex = (LINK_AGE_IN_YEARS == YEARS_CHILD) ? 0xBB : 0x5F4;
            } else {
                gSaveContext.entranceIndex = 0xBB;
            }
            break;
        default:
            if (!CVar_GetS32("gSaveEntrance", 0)) {
                if (gSaveContext.savedSceneNum != SCENE_LINK_HOME) {
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
    if ((gSaveContext.eventChkInf[4] & 1) && !CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
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

/**
 *  Write the contents of the Save Context to a main and backup slot in SRAM.
 *  Note: The whole Save Context is written even though only the `save` substruct is read back later
 */
void Sram_WriteSave(SramContext* sramCtx) {
    u16 offset;
    u16 checksum;
    u16 j;
    u16* ptr;

    gSaveContext.checksum = 0;

    ptr = (u16*)&gSaveContext;
    checksum = 0;
    j = 0;

    for (offset = 0; offset < CHECKSUM_SIZE; offset++) {
        if (++j == 0x20) {
            j = 0;
        }
        checksum += *ptr++;
    }

    gSaveContext.checksum = checksum;

    ptr = (u16*)&gSaveContext;
    checksum = 0;

    for (offset = 0; offset < CHECKSUM_SIZE; offset++) {
        if (++j == 0x20) {
            j = 0;
        }
        checksum += *ptr++;
    }

    offset = gSramSlotOffsets[gSaveContext.fileNum];
    SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000) + offset, &gSaveContext, SLOT_SIZE, OS_WRITE);

    ptr = (u16*)&gSaveContext;
    checksum = 0;

    for (offset = 0; offset < CHECKSUM_SIZE; offset++) {
        if (++j == 0x20) {
            j = 0;
        }
        checksum += *ptr++;
    }

    offset = gSramSlotOffsets[gSaveContext.fileNum + 3];
    SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000) + offset, &gSaveContext, SLOT_SIZE, OS_WRITE);
}

/**
 *  For all 3 slots, verify that the checksum is correct. If corrupted, attempt to load a backup save.
 *  If backup is also corrupted, default to a new save (or debug save for slot 0 on debug rom).
 *
 *  After verifying all 3 saves, pass relevant data to File Select to be displayed.
 */
void Sram_VerifyAndLoadAllSaves(FileChooseContext* fileChooseCtx, SramContext* sramCtx) {
    u16 i;
    u16 newChecksum;
    u16 slotNum;
    u16 offset;
    u16 j;
    u16 oldChecksum;
    u16* ptr;
    u16 dayTime;

    osSyncPrintf("ＳＲＡＭ ＳＴＡＲＴ─ＬＯＡＤ\n");
    memset(sramCtx->readBuff,0, SRAM_SIZE);
    SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE, OS_READ);

    dayTime = ((void)0, gSaveContext.dayTime);

    for (slotNum = 0; slotNum < 3; slotNum++) {
        offset = gSramSlotOffsets[slotNum];
        osSyncPrintf("ぽいんと＝%x(%d)    SAVE_MAX=%d\n", offset, gSaveContext.fileNum, sizeof(Save));
        memcpy(&gSaveContext, sramCtx->readBuff + offset, sizeof(Save));

        oldChecksum = gSaveContext.checksum;
        gSaveContext.checksum = 0;
        ptr = (u16*)&gSaveContext;
        osSyncPrintf("\n＝＝＝＝＝＝＝＝＝＝＝＝＝  Ｓ（%d） ＝＝＝＝＝＝＝＝＝＝＝＝＝\n", slotNum);

        for (i = newChecksum = j = 0; i < CHECKSUM_SIZE; i++, offset += 2) {
            newChecksum += *ptr++;
        }

        // "SAVE checksum calculation"
        osSyncPrintf("\nＳＡＶＥチェックサム計算  j=%x  mmm=%x  ", newChecksum, oldChecksum);

        if (newChecksum != oldChecksum) {
            // checksum didnt match, try backup save
            osSyncPrintf("ＥＲＲＯＲ！！！ ＝ %x(%d)\n", gSramSlotOffsets[slotNum], slotNum);
            offset = gSramSlotOffsets[slotNum + 3];
            memcpy(&gSaveContext, sramCtx->readBuff + offset, sizeof(Save));

            oldChecksum = gSaveContext.checksum;
            gSaveContext.checksum = 0;
            ptr = (u16*)&gSaveContext;
            osSyncPrintf("================= ＢＡＣＫ─ＵＰ ========================\n");

            for (i = newChecksum = j = 0; i < CHECKSUM_SIZE; i++, offset += 2) {
                newChecksum += *ptr++;
            }
            // "(B) SAVE checksum calculation"
            osSyncPrintf("\n（Ｂ）ＳＡＶＥチェックサム計算  j=%x  mmm=%x  ", newChecksum, oldChecksum);

            if (newChecksum != oldChecksum) {
                // backup save didnt work, make new save
                osSyncPrintf("ＥＲＲＯＲ！！！ ＝ %x(%d+3)\n", gSramSlotOffsets[slotNum + 3], slotNum);
                memset(&gSaveContext.entranceIndex, 0, sizeof(s32));
                memset(&gSaveContext.linkAge, 0, sizeof(s32));
                memset(&gSaveContext.cutsceneIndex, 0, sizeof(s32));
                // note that gSaveContext.dayTime is not actually the sizeof(s32)
                memset(&gSaveContext.dayTime, 0, sizeof(s32));
                memset(&gSaveContext.nightFlag, 0, sizeof(s32));
                memset(&gSaveContext.totalDays, 0, sizeof(s32));
                memset(&gSaveContext.bgsDayCount, 0, sizeof(s32));

                if (!slotNum && CVar_GetS32("gDebugEnabled", 0)) {
                    Sram_InitDebugSave();
                    gSaveContext.newf[0] = 'Z';
                    gSaveContext.newf[1] = 'E';
                    gSaveContext.newf[2] = 'L';
                    gSaveContext.newf[3] = 'D';
                    gSaveContext.newf[4] = 'A';
                    gSaveContext.newf[5] = 'Z';
                    osSyncPrintf("newf=%x,%x,%x,%x,%x,%x\n", gSaveContext.newf[0], gSaveContext.newf[1],
                                 gSaveContext.newf[2], gSaveContext.newf[3], gSaveContext.newf[4],
                                 gSaveContext.newf[5]);
                } else {
                    Sram_InitNewSave();
                }

                ptr = (u16*)&gSaveContext;
                osSyncPrintf("\n--------------------------------------------------------------\n");

                for (i = newChecksum = j = 0; i < CHECKSUM_SIZE; i++) {
                    osSyncPrintf("%x ", *ptr);
                    if (++j == 0x20) {
                        osSyncPrintf("\n");
                        j = 0;
                    }
                    newChecksum += *ptr++;
                }

                gSaveContext.checksum = newChecksum;
                osSyncPrintf("\nCheck_Sum=%x(%x)\n", gSaveContext.checksum, newChecksum);

                i = gSramSlotOffsets[slotNum + 3];
                SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000) + i, &gSaveContext, SLOT_SIZE, OS_WRITE);

                osSyncPrintf("????#%x,%x,%x,%x,%x,%x\n", gSaveContext.newf[0], gSaveContext.newf[1],
                             gSaveContext.newf[2], gSaveContext.newf[3], gSaveContext.newf[4], gSaveContext.newf[5]);
                osSyncPrintf("\nぽいんと＝%x(%d+3)  check_sum=%x(%x)\n", i, slotNum, gSaveContext.checksum,
                             newChecksum);
            }

            i = gSramSlotOffsets[slotNum];
            SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000) + i, &gSaveContext, SLOT_SIZE, OS_WRITE);

            osSyncPrintf("ぽいんと＝%x(%d)  check_sum=%x(%x)\n", i, slotNum, gSaveContext.checksum, newChecksum);
        } else {
            osSyncPrintf("\nＳＡＶＥデータ ＯＫ！！！！\n"); // "SAVE data OK! ! ! !"
        }
    }

    memset(sramCtx->readBuff,0, SRAM_SIZE);
    SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE, OS_READ);
    gSaveContext.dayTime = dayTime;

    osSyncPrintf("SAVECT=%x, NAME=%x, LIFE=%x, ITEM=%x,  64DD=%x,  HEART=%x\n", DEATHS, NAME, HEALTH_CAP, QUEST, N64DD,
                 DEFENSE);

    memcpy(&fileChooseCtx->deaths[0], sramCtx->readBuff + SLOT_OFFSET(0) + DEATHS, sizeof(fileChooseCtx->deaths[0]));
    memcpy(&fileChooseCtx->deaths[1], sramCtx->readBuff + SLOT_OFFSET(1) + DEATHS, sizeof(fileChooseCtx->deaths[0]));
    memcpy(&fileChooseCtx->deaths[2], sramCtx->readBuff + SLOT_OFFSET(2) + DEATHS, sizeof(fileChooseCtx->deaths[0]));

    memcpy(&fileChooseCtx->fileNames[0], sramCtx->readBuff + SLOT_OFFSET(0) + NAME,
            sizeof(fileChooseCtx->fileNames[0]));
    memcpy(&fileChooseCtx->fileNames[1], sramCtx->readBuff + SLOT_OFFSET(1) + NAME,
            sizeof(fileChooseCtx->fileNames[0]));
    memcpy(&fileChooseCtx->fileNames[2], sramCtx->readBuff + SLOT_OFFSET(2) + NAME,
            sizeof(fileChooseCtx->fileNames[0]));

    memcpy(&fileChooseCtx->healthCapacities[0], sramCtx->readBuff + SLOT_OFFSET(0) + HEALTH_CAP,
            sizeof(fileChooseCtx->healthCapacities[0]));
    memcpy(&fileChooseCtx->healthCapacities[1], sramCtx->readBuff + SLOT_OFFSET(1) + HEALTH_CAP,
            sizeof(fileChooseCtx->healthCapacities[0]));
    memcpy(&fileChooseCtx->healthCapacities[2], sramCtx->readBuff + SLOT_OFFSET(2) + HEALTH_CAP,
            sizeof(fileChooseCtx->healthCapacities[0]));

    memcpy(&fileChooseCtx->questItems[0], sramCtx->readBuff + SLOT_OFFSET(0) + QUEST,
            sizeof(fileChooseCtx->questItems[0]));
    memcpy(&fileChooseCtx->questItems[1], sramCtx->readBuff + SLOT_OFFSET(1) + QUEST,
            sizeof(fileChooseCtx->questItems[0]));
    memcpy(&fileChooseCtx->questItems[2], sramCtx->readBuff + SLOT_OFFSET(2) + QUEST,
            sizeof(fileChooseCtx->questItems[0]));

    memcpy(&fileChooseCtx->n64ddFlags[0], sramCtx->readBuff + SLOT_OFFSET(0) + N64DD,
            sizeof(fileChooseCtx->n64ddFlags[0]));
    memcpy(&fileChooseCtx->n64ddFlags[1], sramCtx->readBuff + SLOT_OFFSET(1) + N64DD,
            sizeof(fileChooseCtx->n64ddFlags[0]));
    memcpy(&fileChooseCtx->n64ddFlags[2], sramCtx->readBuff + SLOT_OFFSET(2) + N64DD,
            sizeof(fileChooseCtx->n64ddFlags[0]));

    memcpy(&fileChooseCtx->defense[0], sramCtx->readBuff + SLOT_OFFSET(0) + DEFENSE,
            sizeof(fileChooseCtx->defense[0]));
    memcpy(&fileChooseCtx->defense[1], sramCtx->readBuff + SLOT_OFFSET(1) + DEFENSE,
            sizeof(fileChooseCtx->defense[0]));
    memcpy(&fileChooseCtx->defense[2], sramCtx->readBuff + SLOT_OFFSET(2) + DEFENSE,
            sizeof(fileChooseCtx->defense[0]));

    memcpy(&fileChooseCtx->health[0], sramCtx->readBuff + SLOT_OFFSET(0) + HEALTH, sizeof(fileChooseCtx->health[0]));
    memcpy(&fileChooseCtx->health[1], sramCtx->readBuff + SLOT_OFFSET(1) + HEALTH, sizeof(fileChooseCtx->health[0]));
    memcpy(&fileChooseCtx->health[2], sramCtx->readBuff + SLOT_OFFSET(2) + HEALTH, sizeof(fileChooseCtx->health[0]));

    osSyncPrintf("f_64dd=%d, %d, %d\n", fileChooseCtx->n64ddFlags[0], fileChooseCtx->n64ddFlags[1],
                 fileChooseCtx->n64ddFlags[2]);
    osSyncPrintf("heart_status=%d, %d, %d\n", fileChooseCtx->defense[0], fileChooseCtx->defense[1],
                 fileChooseCtx->defense[2]);
    osSyncPrintf("now_life=%d, %d, %d\n", fileChooseCtx->health[0], fileChooseCtx->health[1], fileChooseCtx->health[2]);
}

void Sram_InitSave(FileChooseContext* fileChooseCtx, SramContext* sramCtx) {
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
        gSaveContext.playerName[offset] = fileChooseCtx->fileNames[fileChooseCtx->buttonIndex][offset];
    }

    gSaveContext.newf[0] = 'Z';
    gSaveContext.newf[1] = 'E';
    gSaveContext.newf[2] = 'L';
    gSaveContext.newf[3] = 'D';
    gSaveContext.newf[4] = 'A';
    gSaveContext.newf[5] = 'Z';

    gSaveContext.n64ddFlag = fileChooseCtx->n64ddFlag;
    osSyncPrintf("６４ＤＤフラグ=%d\n", fileChooseCtx->n64ddFlag);
    osSyncPrintf("newf=%x,%x,%x,%x,%x,%x\n", gSaveContext.newf[0], gSaveContext.newf[1], gSaveContext.newf[2],
                 gSaveContext.newf[3], gSaveContext.newf[4], gSaveContext.newf[5]);
    osSyncPrintf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    ptr = (u16*)&gSaveContext;
    j = 0;
    checksum = 0;

    for (offset = 0; offset < CHECKSUM_SIZE; offset++) {
        osSyncPrintf("%x ", *ptr);
        checksum += *ptr++;
        if (++j == 0x20) {
            osSyncPrintf("\n");
            j = 0;
        }
    }

    gSaveContext.checksum = checksum;
    osSyncPrintf("\nチェックサム＝%x\n", gSaveContext.checksum); // "Checksum = %x"

    offset = gSramSlotOffsets[gSaveContext.fileNum];
    osSyncPrintf("I=%x no=%d\n", offset, gSaveContext.fileNum);
    memcpy(sramCtx->readBuff + offset, &gSaveContext, sizeof(Save));

    offset = gSramSlotOffsets[gSaveContext.fileNum + 3];
    osSyncPrintf("I=%x no=%d\n", offset, gSaveContext.fileNum + 3);
    memcpy(sramCtx->readBuff + offset, &gSaveContext, sizeof(Save));

    SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE, OS_WRITE);

    osSyncPrintf("ＳＡＶＥ終了\n"); // "SAVE end"
    osSyncPrintf("z_common_data.file_no = %d\n", gSaveContext.fileNum);
    osSyncPrintf("SAVECT=%x, NAME=%x, LIFE=%x, ITEM=%x,  SAVE_64DD=%x\n", DEATHS, NAME, HEALTH_CAP, QUEST, N64DD);

    j = gSramSlotOffsets[gSaveContext.fileNum];

    memcpy(&fileChooseCtx->deaths[gSaveContext.fileNum], sramCtx->readBuff + j + DEATHS,
            sizeof(fileChooseCtx->deaths[0]));
    memcpy(&fileChooseCtx->fileNames[gSaveContext.fileNum], sramCtx->readBuff + j + NAME,
            sizeof(fileChooseCtx->fileNames[0]));
    memcpy(&fileChooseCtx->healthCapacities[gSaveContext.fileNum], sramCtx->readBuff + j + HEALTH_CAP,
            sizeof(fileChooseCtx->healthCapacities[0]));
    memcpy(&fileChooseCtx->questItems[gSaveContext.fileNum], sramCtx->readBuff + j + QUEST,
            sizeof(fileChooseCtx->questItems[0]));
    memcpy(&fileChooseCtx->n64ddFlags[gSaveContext.fileNum], sramCtx->readBuff + j + N64DD,
            sizeof(fileChooseCtx->n64ddFlags[0]));
    memcpy(&fileChooseCtx->defense[gSaveContext.fileNum], sramCtx->readBuff + j + DEFENSE,
            sizeof(fileChooseCtx->defense[0]));
    memcpy(&fileChooseCtx->health[gSaveContext.fileNum], sramCtx->readBuff + j + HEALTH,
            sizeof(fileChooseCtx->health[0]));

    osSyncPrintf("f_64dd[%d]=%d\n", gSaveContext.fileNum, fileChooseCtx->n64ddFlags[gSaveContext.fileNum]);
    osSyncPrintf("heart_status[%d]=%d\n", gSaveContext.fileNum, fileChooseCtx->defense[gSaveContext.fileNum]);
    osSyncPrintf("now_life[%d]=%d\n", gSaveContext.fileNum, fileChooseCtx->health[gSaveContext.fileNum]);
}

void Sram_EraseSave(FileChooseContext* fileChooseCtx, SramContext* sramCtx) {
    s32 offset;

    Sram_InitNewSave();

    offset = gSramSlotOffsets[fileChooseCtx->selectedFileIndex];
    memcpy(sramCtx->readBuff + offset, &gSaveContext, sizeof(Save));
    SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000) + offset, &gSaveContext, SLOT_SIZE, OS_WRITE);

    memcpy(&fileChooseCtx->n64ddFlags[fileChooseCtx->selectedFileIndex], sramCtx->readBuff + offset + N64DD,
            sizeof(fileChooseCtx->n64ddFlags[0]));

    offset = gSramSlotOffsets[fileChooseCtx->selectedFileIndex + 3];
    memcpy(sramCtx->readBuff + offset, &gSaveContext, sizeof(Save));
    SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000) + offset, &gSaveContext, SLOT_SIZE, OS_WRITE);

    osSyncPrintf("ＣＬＥＡＲ終了\n");
}

void Sram_CopySave(FileChooseContext* fileChooseCtx, SramContext* sramCtx) {
    s32 offset;

    osSyncPrintf("ＲＥＡＤ=%d(%x)  ＣＯＰＹ=%d(%x)\n", fileChooseCtx->selectedFileIndex,
                 gSramSlotOffsets[fileChooseCtx->selectedFileIndex], fileChooseCtx->copyDestFileIndex,
                 gSramSlotOffsets[fileChooseCtx->copyDestFileIndex]);

    offset = gSramSlotOffsets[fileChooseCtx->selectedFileIndex];
    memcpy(&gSaveContext, sramCtx->readBuff + offset, sizeof(Save));

    offset = gSramSlotOffsets[fileChooseCtx->copyDestFileIndex];
    memcpy(sramCtx->readBuff + offset, &gSaveContext, sizeof(Save));

    offset = gSramSlotOffsets[fileChooseCtx->copyDestFileIndex + 3];
    memcpy(sramCtx->readBuff + offset, &gSaveContext, sizeof(Save));

    SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE, OS_WRITE);

    offset = gSramSlotOffsets[fileChooseCtx->copyDestFileIndex];

    memcpy(&fileChooseCtx->deaths[fileChooseCtx->copyDestFileIndex], sramCtx->readBuff + offset + DEATHS,
            sizeof(fileChooseCtx->deaths[0]));
    memcpy(&fileChooseCtx->fileNames[fileChooseCtx->copyDestFileIndex], sramCtx->readBuff + offset + NAME,
            sizeof(fileChooseCtx->fileNames[0]));
    memcpy(&fileChooseCtx->healthCapacities[fileChooseCtx->copyDestFileIndex], sramCtx->readBuff + offset + HEALTH_CAP,
            sizeof(fileChooseCtx->healthCapacities[0]));
    memcpy(&fileChooseCtx->questItems[fileChooseCtx->copyDestFileIndex], sramCtx->readBuff + offset + QUEST,
            sizeof(fileChooseCtx->questItems[0]));
    memcpy(&fileChooseCtx->n64ddFlags[fileChooseCtx->copyDestFileIndex], sramCtx->readBuff + offset + N64DD,
            sizeof(fileChooseCtx->n64ddFlags[0]));
    memcpy(&fileChooseCtx->defense[fileChooseCtx->copyDestFileIndex], sramCtx->readBuff + offset + DEFENSE,
            sizeof(fileChooseCtx->defense[0]));
    memcpy(&fileChooseCtx->health[fileChooseCtx->copyDestFileIndex], (sramCtx->readBuff + offset) + HEALTH,
            sizeof(fileChooseCtx->health[0]));

    osSyncPrintf("f_64dd[%d]=%d\n", gSaveContext.fileNum, fileChooseCtx->n64ddFlags[gSaveContext.fileNum]);
    osSyncPrintf("heart_status[%d]=%d\n", gSaveContext.fileNum, fileChooseCtx->defense[gSaveContext.fileNum]);
    osSyncPrintf("ＣＯＰＹ終了\n"); // "Copy end"
}

/**
 *  Write the first 16 bytes of the read buffer to the SRAM header
 */
void Sram_WriteSramHeader(SramContext* sramCtx) {
    SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_HEADER_SIZE, OS_WRITE);
}

void Sram_InitSram(GameState* gameState, SramContext* sramCtx) {
    u16 i;

    osSyncPrintf("sram_initialize( Game *game, Sram *sram )\n");
    SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE, OS_READ);

    for (i = 0; i < ARRAY_COUNTU(sZeldaMagic) - 3; i++) {
        if (sZeldaMagic[i + SRAM_HEADER_MAGIC] != sramCtx->readBuff[i + SRAM_HEADER_MAGIC]) {
            osSyncPrintf("ＳＲＡＭ破壊！！！！！！\n"); // "SRAM destruction! ! ! ! ! !"
            gSaveContext.language = sramCtx->readBuff[SRAM_HEADER_LANGUAGE];
            memcpy(sramCtx->readBuff, sZeldaMagic, sizeof(sZeldaMagic));
            sramCtx->readBuff[SRAM_HEADER_LANGUAGE] = gSaveContext.language;
            Sram_WriteSramHeader(sramCtx);
        }
    }

    gSaveContext.audioSetting = sramCtx->readBuff[SRAM_HEADER_SOUND] & 3;
    gSaveContext.zTargetSetting = sramCtx->readBuff[SRAM_HEADER_ZTARGET] & 1;
    gSaveContext.language = sramCtx->readBuff[SRAM_HEADER_LANGUAGE];

    if (gSaveContext.language >= LANGUAGE_MAX) {
        gSaveContext.language = LANGUAGE_ENG;
        sramCtx->readBuff[SRAM_HEADER_LANGUAGE] = gSaveContext.language;
        Sram_WriteSramHeader(sramCtx);
    }

    if (CHECK_BTN_ANY(gameState->input[2].cur.button, BTN_DRIGHT)) {
        memset(sramCtx->readBuff, 0,SRAM_SIZE);
        for (i = 0; i < CHECKSUM_SIZE; i++) {
            sramCtx->readBuff[i] = i;
        }
        SsSram_ReadWrite(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE, OS_WRITE);
        osSyncPrintf("ＳＲＡＭ破壊！！！！！！\n"); // "SRAM destruction! ! ! ! ! !"
    }

    // "GOOD! GOOD! Size = %d + %d = %d"
    osSyncPrintf("ＧＯＯＤ！ＧＯＯＤ！ サイズ＝%d + %d ＝ %d\n", sizeof(SaveInfo), 4, sizeof(SaveInfo) + 4);
    osSyncPrintf(VT_FGCOL(BLUE));
    osSyncPrintf("Na_SetSoundOutputMode = %d\n", gSaveContext.audioSetting);
    osSyncPrintf("Na_SetSoundOutputMode = %d\n", gSaveContext.audioSetting);
    osSyncPrintf("Na_SetSoundOutputMode = %d\n", gSaveContext.audioSetting);
    osSyncPrintf(VT_RST);
    func_800F6700(gSaveContext.audioSetting);
}

void Sram_Alloc(GameState* gameState, SramContext* sramCtx) {
    sramCtx->readBuff = GameState_Alloc(gameState, SRAM_SIZE, "../z_sram.c", 1294);
    ASSERT(sramCtx->readBuff != NULL, "sram->read_buff != NULL", "../z_sram.c", 1295);
}

void Sram_Init(GlobalContext* globalCtx, SramContext* sramCtx) {
}
