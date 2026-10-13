#include "global.h"
#include "z64horse.h"
#include "overlays/gamestates/ovl_file_choose/z_file_select.h"
#include <string.h>
#include "BenPort.h"
#include "build.h"

#include "2s2h/Enhancements/Saving/SavingEnhancements.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include <libultraship/bridge/consolevariablebridge.h>

void Sram_SyncWriteToFlash(SramContext* sramCtx, s32 curPage, s32 numPages);
void func_80147414(SramContext* sramCtx, s32 fileNum, s32 arg2);

#define CHECK_NEWF(newf)                                                                                 \
    ((newf)[0] != 'Z' || (newf)[1] != 'E' || (newf)[2] != 'L' || (newf)[3] != 'D' || (newf)[4] != 'A' || \
     (newf)[5] != '3')

typedef struct PersistentCycleSceneFlags {
    /* 0x0 */ u32 switch0;
    /* 0x4 */ u32 switch1;
    /* 0x8 */ u32 chest;
    /* 0xC */ u32 collectible;
} PersistentCycleSceneFlags; // size = 0x10

#define PERSISTENT_CYCLE_FLAGS_SET(switch0, switch1, chest, collectible) { switch0, switch1, chest, collectible },
#define PERSISTENT_CYCLE_FLAGS_NONE PERSISTENT_CYCLE_FLAGS_SET(0, 0, 0, 0)

// 2S2H Added columns to scene table: entranceSceneId, betterMapSelectIndex, humanName
#define DEFINE_SCENE(_name, _enumValue, _textId, _drawConfig, _restrictionFlags, persistentCycleFlags, \
                     _entranceSceneId, _betterMapSelectIndex, _humanName)                              \
    persistentCycleFlags
#define DEFINE_SCENE_UNSET(_enumValue) PERSISTENT_CYCLE_FLAGS_NONE

/**
 * Array of bitwise flags which won't be turned off on a cycle reset (will persist between cycles)
 */
PersistentCycleSceneFlags sPersistentCycleSceneFlags[SCENE_MAX] = {
#include "tables/scene_table.h"
};

#undef DEFINE_SCENE
#undef DEFINE_SCENE_UNSET

// Each flag has 2 bits to store persistence over the three-day reset cycle
// Only 1 of these bits need to be set to persist (Values 1, 2, 3).
// Therefore, the final game does not distinguish between these two macros in use
#define PERSISTENT_WEEKEVENTREG(flag) (3 << (2 * BIT_FLAG_TO_SHIFT(flag)))
#define PERSISTENT_WEEKEVENTREG_ALT(flag) (2 << (2 * BIT_FLAG_TO_SHIFT(flag)))

// weekEventReg flags which will be not be cleared on a cycle reset
//! @note The index of the flag in this array must be the same to its index in the WeekeventReg array
//!       Only the mask is read from the `PERSISTENT_` macros.
u16 sPersistentCycleWeekEventRegs[ARRAY_COUNT(gSaveContext.save.saveInfo.weekEventReg)] = {
    /*  0 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_TERMINA_FIELD) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_IKANA_GRAVEYARD) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_ROMANI_RANCH) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_GORMAN_TRACK) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_MOUNTAIN_VILLAGE_WINTER) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_GORON_SHRINE) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_SNOWHEAD),
    /*  1 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_SOUTHERN_SWAMP_POISONED) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WOODFALL) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_DEKU_PALACE) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_GREAT_BAY_COAST) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_PIRATES_FORTRESS) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_ZORA_HALL) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WATERFALL_RAPIDS) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_IKANA_CANYON),
    /*  2 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_IKANA_CASTLE) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_STONE_TOWER) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_STONE_TOWER_INVERTED) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_EAST_CLOCK_TOWN) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WEST_CLOCK_TOWN) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_NORTH_CLOCK_TOWN) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WOODFALL_TEMPLE) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_SNOWHEAD_TEMPLE),
    /*  3 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_PIRATES_FORTRESS_EXTERIOR) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_STONE_TOWER_TEMPLE) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_STONE_TOWER_TEMPLE_INVERTED) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_THE_MOON) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_MOON_DEKU_TRIAL) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_MOON_GORON_TRIAL) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_MOON_ZORA_TRIAL) | PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_03_80),
    /*  4 */ 0,
    /*  5 */ 0,
    /*  6 */ 0,
    /*  7 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_ENTERED_WOODFALL_TEMPLE_PRISON),
    /*  8 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_DOGGY_RACETRACK_HEART_PIECE),
    /*  9 */ 0,
    /* 10 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_WALLET_UPGRADE),
    /* 11 */ 0,
    /* 12 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KOTAKE_BOTTLE),
    /* 13 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_OCEANSIDE_WALLET_UPGRADE),
    /* 14 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_DEKU_PLAYGROUND_HEART_PIECE),
    /* 15 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_15_20),
    /* 16 */ 0,
    /* 17 */ 0,
    /* 18 */ 0,
    /* 19 */ 0,
    /* 20 */ 0,
    /* 21 */ 0,
    /* 22 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_ALIENS_BOTTLE) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_HONEY_AND_DARLING_HEART_PIECE),
    /* 23 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GREAT_SPIN_ATTACK) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BEAVER_RACE_BOTTLE),
    /* 24 */ PERSISTENT_WEEKEVENTREG_ALT(WEEKEVENTREG_24_02) | PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_24_80),
    /* 25 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BEAVER_BROS_HEART_PIECE),
    /* 26 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_26_40),
    /* 27 */ 0,
    /* 28 */ 0,
    /* 29 */ 0,
    /* 30 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_30_10) | PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_30_20) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_30_40) | PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_30_80),
    /* 31 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_31_01) | PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_31_02) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_31_04) | PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_31_20),
    /* 32 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SEAHORSE_HEART_PIECE) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_HEART_PIECE) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_HEART_PIECE),
    /* 33 */ 0,
    /* 34 */ 0,
    /* 35 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_CLOCK_TOWN) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_WOODFALL) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_SNOWHEAD) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_ROMANI_RANCH) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_GREAT_BAY) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_TINGLE_MAP_BOUGHT_STONE_TOWER) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_FROG_CHOIR_HEART_PIECE),
    /* 36 */ 0,
    /* 37 */ 0,
    /* 38 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_38_20),
    /* 39 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_EVAN_HEART_PIECE),
    /* 40 */ 0,
    /* 41 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GORON_RACE_BOTTLE),
    /* 42 */ 0,
    /* 43 */ 0,
    /* 44 */ 0,
    /* 45 */ 0,
    /* 46 */ 0,
    /* 47 */ 0,
    /* 48 */ 0,
    /* 49 */ 0,
    /* 50 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_50_02) | PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_50_04),
    /* 51 */ PERSISTENT_WEEKEVENTREG_ALT(WEEKEVENTREG_51_04),
    /* 52 */ 0,
    /* 53 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BUSINESS_SCRUB_HEART_PIECE) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_53_40),
    /* 54 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SPIRIT_HOUSE_HEART_PIECE),
    /* 55 */ 0,
    /* 56 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_MARINE_RESEARCH_LAB_FISH_HEART_PIECE),
    /* 57 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_57_08),
    /* 58 */ 0,
    /* 59 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_59_04) | PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_BANK_HEART_PIECE) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWAMP_SHOOTING_GALLERY_QUIVER_UPGRADE) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_TOWN_SHOOTING_GALLERY_QUIVER_UPGRADE),
    /* 60 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_MAYOR_HEART_PIECE),
    /* 61 */ 0,
    /* 62 */ 0,
    /* 63 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_SWORDSMANS_SCHOOL_HEART_PIECE),
    /* 64 */ 0,
    /* 65 */ 0,
    /* 66 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_ANJU) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_KAFEI) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_CURIOSITY_SHOP_MAN) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_BOMB_SHOP_LADY) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_ROMANI) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_CREMIA) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_MAYOR_DOTOUR) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 67 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_TOTO) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_GORMAN) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_POSTMAN) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_ROSA_SISTERS) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_TOILET_HAND) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_ANJUS_GRANDMOTHER) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_KAMARO) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_GROG),
    /* 68 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_GORMAN_BROTHERS) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_SHIRO) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_GURU_GURU) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_MET_BOMBERS) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_ROOM_KEY) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_PROMISED_MIDNIGHT_MEETING) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_PROMISED_TO_MEET_KAFEI) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_LETTER_TO_KAFEI),
    /* 69 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_PENDANT_OF_MEMORIES) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_DELIVERED_PENDANT_OF_MEMORIES) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_ESCAPED_SAKONS_HIDEOUT) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_PROMISED_TO_HELP_WITH_ALIENS) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_DEFENDED_AGAINST_ALIENS) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_ALIENS_BOTTLE) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_ESCORTED_CREMIA),
    /* 70 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_ROMANIS_MASK) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_KEATON_MASK) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_PRIORITY_MAIL) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_DELIVERED_PRIORITY_MAIL) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_LEARNED_SECRET_CODE) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_BOMBERS_NOTEBOOK) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_MAYOR_HP) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_ROSA_SISTERS_HP),
    /* 71 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_TOILET_HAND_HP) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_GRANDMA_SHORT_STORY_HP) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_GRANDMA_LONG_STORY_HP) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_POSTMAN_HP) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAFEIS_MASK) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_ALL_NIGHT_MASK) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_BUNNY_HOOD) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_GAROS_MASK),
    /* 72 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_CIRCUS_LEADERS_MASK) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_POSTMANS_HAT) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_COUPLES_MASK) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_BLAST_MASK) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAMAROS_MASK) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_STONE_MASK) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_BOMBERS_NOTEBOOK_EVENT_RECEIVED_BREMEN_MASK) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_72_80),
    /* 73 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_73_08),
    /* 74 */ 0,
    /* 75 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_ROSA_SISTERS_HEART_PIECE),
    /* 76 */ 0,
    /* 77 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_POSTMAN_COUNTING_GAME_HEART_PIECE),
    /* 78 */ 0,
    /* 79 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KEATON_HEART_PIECE),
    /* 80 */ 0,
    /* 81 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_81_08),
    /* 82 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_FISHERMANS_JUMPING_GAME_HEART_PIECE),
    /* 83 */ 0,
    /* 84 */ 0,
    /* 85 */ 0,
    /* 86 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_86_80),
    /* 87 */
    PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_87_04) | PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_87_08) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_87_10) | PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_87_20) |
        PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_87_40) | PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_87_80),
    /* 88 */ 0,
    /* 89 */ 0,
    /* 90 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_GOSSIP_STONE_GROTTO_HEART_PIECE),
    /* 91 */ 0,
    /* 92 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_92_80),
    /* 93 */ PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_93_04) | PERSISTENT_WEEKEVENTREG(WEEKEVENTREG_93_08),
    /* 94 */ 0,
    /* 95 */ 0,
    /* 96 */ 0,
    /* 97 */ 0,
    /* 98 */ 0,
    /* 99 */ 0,
};

// Unused remnant values from OoT. Not the correct sizes in MM.
#define OOT_SAVECONTEXT_SIZE 0x1428
#define OOT_SLOT_SIZE (OOT_SAVECONTEXT_SIZE + 0x28)
#define OOT_SRAM_HEADER_SIZE 0x10
#define OOT_SLOT_OFFSET(index) (OOT_SRAM_HEADER_SIZE + 0x10 + (index * OOT_SLOT_SIZE))

// used in other files
u32 gSramSlotOffsets[] = {
    OOT_SLOT_OFFSET(0),
    OOT_SLOT_OFFSET(1),
    OOT_SLOT_OFFSET(2),
    // the latter three saves are backup saves for the former saves (in OoT)
    OOT_SLOT_OFFSET(3),
    OOT_SLOT_OFFSET(4),
    OOT_SLOT_OFFSET(5),
};

u8 gAmmoItems[ITEM_NUM_SLOTS] = {
    ITEM_NONE,           // SLOT_OCARINA
    ITEM_BOW,            // SLOT_BOW
    ITEM_NONE,           // SLOT_ARROW_FIRE
    ITEM_NONE,           // SLOT_ARROW_ICE
    ITEM_NONE,           // SLOT_ARROW_LIGHT
    ITEM_NONE,           // SLOT_TRADE_DEED
    ITEM_BOMB,           // SLOT_BOMB
    ITEM_BOMBCHU,        // SLOT_BOMBCHU
    ITEM_DEKU_STICK,     // SLOT_DEKU_STICK
    ITEM_DEKU_NUT,       // SLOT_DEKU_NUT
    ITEM_MAGIC_BEANS,    // SLOT_MAGIC_BEANS
    ITEM_NONE,           // SLOT_TRADE_KEY_MAMA
    ITEM_POWDER_KEG,     // SLOT_POWDER_KEG
    ITEM_PICTOGRAPH_BOX, // SLOT_PICTOGRAPH_BOX
    ITEM_NONE,           // SLOT_LENS_OF_TRUTH
    ITEM_NONE,           // SLOT_HOOKSHOT
    ITEM_NONE,           // SLOT_SWORD_GREAT_FAIRY
    ITEM_NONE,           // SLOT_TRADE_COUPLE
    ITEM_NONE,           // SLOT_BOTTLE_1
    ITEM_NONE,           // SLOT_BOTTLE_2
    ITEM_NONE,           // SLOT_BOTTLE_3
    ITEM_NONE,           // SLOT_BOTTLE_4
    ITEM_NONE,           // SLOT_BOTTLE_5
    ITEM_NONE,           // SLOT_BOTTLE_6
};

// Stores flash start page number
// 2S2H [Enhancement] Extended for file 3 support
s32 gFlashSaveStartPages[] = {
    0,     // File 1 New Cycle Save
    0x40,  // File 1 New Cycle Save Backup
    0x80,  // File 2 New Cycle Save
    0xC0,  // File 2 New Cycle Save Backup
    0x400, // File 3 New Cycle Save
    0x440, // File 3 New Cycle Save Backup
    0x100, // File 1 Owl Save
    0x180, // File 1 Owl Save Backup
    0x200, // File 2 Owl Save
    0x280, // File 2 Owl Save Backup
    0x480, // File 3 Owl Save
    0x500, // File 3 Owl Save Backup
    0x300, // Sram Header (SaveOptions)
    0x380, // Sram Header Backup (SaveOptions)
};

// Flash rom number of pages
// 2S2H [Enhancement] Extended for file 3 support
s32 gFlashSaveNumPages[] = {
    0x40, // File 1 New Cycle Save
    0x40, // File 1 New Cycle Save Backup
    0x40, // File 2 New Cycle Save
    0x40, // File 2 New Cycle Save Backup
    0x40, // File 3 New Cycle Save
    0x40, // File 3 New Cycle Save Backup
    0x80, // File 1 Owl Save
    0x80, // File 1 Owl Save Backup
    0x80, // File 2 Owl Save
    0x80, // File 2 Owl Save Backup
    0x80, // File 3 Owl Save
    0x80, // File 3 Owl Save Backup
    1,    // Sram Header (SaveOptions)
    1,    // Sram Header Backup (SaveOptions)
};

// Flash rom number of pages on very first time Player enters South Clock Town from the Clock Tower
// 2S2H [Enhancement] Extended for file 3 support
s32 gFlashSpecialSaveNumPages[] = {
    0x80, // File 1 New Cycle Save
    0x80, // File 1 New Cycle Save Backup
    0x80, // File 2 New Cycle Save
    0x80, // File 2 New Cycle Save Backup
    0x80, // File 3 New Cycle Save
    0x80, // File 3 New Cycle Save Backup
    0x80, // File 1 Owl Save
    0x80, // File 1 Owl Save Backup
    0x80, // File 2 Owl Save
    0x80, // File 2 Owl Save Backup
    0x80, // File 3 Owl Save
    0x80, // File 3 Owl Save Backup
    1,    // Sram Header (SaveOptions)
    1,    // Sram Header Backup (SaveOptions)
};

// Owl Save flash rom start page number
s32 gFlashOwlSaveStartPages[] = {
    0x100, // File 1 Owl Save
    0x180, // File 1 Owl Save Backup
    0x200, // File 2 Owl Save
    0x280, // File 2 Owl Save Backup
    // 2S2H [Enhancement] Extended for file 3 support
    0x480, // File 3 Owl Save
    0x500, // File 3 Owl Save Backup
};

// Owl Save flash rom number of pages
s32 gFlashOwlSaveNumPages[] = {
    0x80, // File 1 Owl Save
    0x80, // File 1 Owl Save Backup
    0x80, // File 2 Owl Save
    0x80, // File 2 Owl Save Backup
    // 2S2H [Enhancement] Extended for file 3 support
    0x80, // File 3 Owl Save
    0x80, // File 3 Owl Save Backup
};

// Save Options Sram Header flash rom start page number
s32 gFlashOptionsSaveStartPages[] = {
    0x300, // Sram Header (SaveOptions)
    0x380, // Sram Header Backup (SaveOptions)
};

// Save Options Sram Header flash rom number of pages
s32 gFlashOptionsSaveNumPages[] = {
    1, // Sram Header (SaveOptions)
    1, // Sram Header Backup (SaveOptions)
};

// Flash rom actual size needed
// 2S2H [Enhancement] Extended for file 3 support
s32 gFlashSaveSizes[] = {
    sizeof(Save),                   // size = 0x100C - File 1 New Cycle Save
    sizeof(Save),                   // size = 0x100C - File 1 New Cycle Save Backup
    sizeof(Save),                   // size = 0x100C - File 2 New Cycle Save
    sizeof(Save),                   // size = 0x100C - File 2 New Cycle Save Backup
    sizeof(Save),                   // size = 0x100C - File 3 New Cycle Save
    sizeof(Save),                   // size = 0x100C - File 3 New Cycle Save Backup
    offsetof(SaveContext, fileNum), // size = 0x3CA0 - File 1 Owl Save
    offsetof(SaveContext, fileNum), // size = 0x3CA0 - File 1 Owl Save Backup
    offsetof(SaveContext, fileNum), // size = 0x3CA0 - File 2 Owl Save
    offsetof(SaveContext, fileNum), // size = 0x3CA0 - File 2 Owl Save Backup
    offsetof(SaveContext, fileNum), // size = 0x3CA0 - File 3 Owl Save
    offsetof(SaveContext, fileNum), // size = 0x3CA0 - File 3 Owl Save Backup
};

// Bit Flag array in which sBitFlags8[n] is (1 << n)
u8 sBitFlags8[] = {
    (1 << 0), (1 << 1), (1 << 2), (1 << 3), (1 << 4), (1 << 5), (1 << 6), (1 << 7),
};

u16 D_801F6AF0;
u8 D_801F6AF2;

void Sram_ActivateOwl(u8 owlWarpId) {
    SET_OWL_STATUE_ACTIVATED(owlWarpId);

    if (gSaveContext.save.saveInfo.playerData.owlWarpId == OWL_WARP_NONE) {
        gSaveContext.save.saveInfo.playerData.owlWarpId = owlWarpId;
    }
}

void Sram_ClearHighscores(void) {
    HS_SET_BOAT_ARCHERY_HIGH_SCORE(19);
    HS_SET_HIGH_SCORE_3_LOWER(10);
    HS_SET_HORSE_BACK_BALLOON_TIME(SECONDS_TO_TIMER(60));
    HS_SET_TOWN_SHOOTING_GALLERY_HIGH_SCORE(39);
    HS_SET_SWAMP_SHOOTING_GALLERY_HIGH_SCORE(10);

    gSaveContext.save.saveInfo.dekuPlaygroundHighScores[0] = SECONDS_TO_TIMER(75);
    gSaveContext.save.saveInfo.dekuPlaygroundHighScores[1] = SECONDS_TO_TIMER(75);
    gSaveContext.save.saveInfo.dekuPlaygroundHighScores[2] = SECONDS_TO_TIMER(76);
}

/**
 * Clears specific weekEventReg flags. Used by the "Dawn of the First Day" message
 */
void Sram_ClearFlagsAtDawnOfTheFirstDay(void) {
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_55_02);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_90_01);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_89_40);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_89_08);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_85_80);
}

/**
 * Used by Song of Time (when clicking "Yes") and (indirectly) by the "Dawn of the New Day" cutscene
 */
void Sram_SaveEndOfCycle(PlayState* play) {
    GameInteractor_ExecuteBeforeEndOfCycleSave();
    s16 sceneId;
    s32 j;
    s32 i;
    u8 slot;
    u8 item;

    gSaveContext.save.timeSpeedOffset = 0;
    gSaveContext.save.eventDayCount = 0;
    gSaveContext.save.day = 0;
    gSaveContext.save.time = CLOCK_TIME(6, 0) - 1;

    gSaveContext.save.saveInfo.playerData.threeDayResetCount++;
    if (gSaveContext.save.saveInfo.playerData.threeDayResetCount > 999) {
        gSaveContext.save.saveInfo.playerData.threeDayResetCount = 999;
    }

    sceneId = Play_GetOriginalSceneId(play->sceneId);
    Play_SaveCycleSceneFlags(play);

    play->actorCtx.sceneFlags.chest &= sPersistentCycleSceneFlags[sceneId].chest;
    play->actorCtx.sceneFlags.switches[0] &= sPersistentCycleSceneFlags[sceneId].switch0;
    play->actorCtx.sceneFlags.switches[1] &= sPersistentCycleSceneFlags[sceneId].switch1;
    play->actorCtx.sceneFlags.collectible[0] &= sPersistentCycleSceneFlags[sceneId].collectible;
    play->actorCtx.sceneFlags.clearedRoom = 0;

    for (i = 0; i < SCENE_MAX; i++) {
        gSaveContext.cycleSceneFlags[i].switch0 =
            ((void)0, gSaveContext.cycleSceneFlags[i].switch0) & sPersistentCycleSceneFlags[i].switch0;
        gSaveContext.cycleSceneFlags[i].switch1 =
            ((void)0, gSaveContext.cycleSceneFlags[i].switch1) & sPersistentCycleSceneFlags[i].switch1;
        gSaveContext.cycleSceneFlags[i].chest =
            ((void)0, gSaveContext.cycleSceneFlags[i].chest) & sPersistentCycleSceneFlags[i].chest;
        gSaveContext.cycleSceneFlags[i].collectible =
            ((void)0, gSaveContext.cycleSceneFlags[i].collectible) & sPersistentCycleSceneFlags[i].collectible;
        gSaveContext.cycleSceneFlags[i].clearedRoom = 0;
        gSaveContext.save.saveInfo.permanentSceneFlags[i].unk_14 = 0;
        gSaveContext.save.saveInfo.permanentSceneFlags[i].rooms = 0;
    }

    for (; i < ARRAY_COUNT(gSaveContext.cycleSceneFlags); i++) {
        gSaveContext.cycleSceneFlags[i].chest = 0;
        gSaveContext.cycleSceneFlags[i].switch0 = 0;
        gSaveContext.cycleSceneFlags[i].switch1 = 0;
        gSaveContext.cycleSceneFlags[i].clearedRoom = 0;
        gSaveContext.cycleSceneFlags[i].collectible = 0;
    }

    for (i = 0; i < ARRAY_COUNT(gSaveContext.masksGivenOnMoon); i++) {
        gSaveContext.masksGivenOnMoon[i] = 0;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_84_20)) {
        Inventory_DeleteItem(ITEM_MASK_FIERCE_DEITY, SLOT(ITEM_MASK_FIERCE_DEITY));
    }

    for (i = 0; i < ARRAY_COUNT(sPersistentCycleWeekEventRegs); i++) {
        u16 isPersistentBits = sPersistentCycleWeekEventRegs[i];

        for (j = 0; j < ARRAY_COUNT(sBitFlags8); j++) {
            if (!(isPersistentBits & 3)) {
                gSaveContext.save.saveInfo.weekEventReg[i] =
                    ((void)0, gSaveContext.save.saveInfo.weekEventReg[i]) & (0xFF ^ sBitFlags8[j]);
            }
            isPersistentBits >>= 2;
        }
    }

    for (i = 0; i < ARRAY_COUNT(gSaveContext.eventInf); i++) {
        gSaveContext.eventInf[i] = 0;
    }

    CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_RUPEES);
    CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_BOMB_AMMO);
    CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_NUT_AMMO);
    CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_STICK_AMMO);
    CLEAR_EVENTINF(EVENTINF_THREEDAYRESET_LOST_ARROW_AMMO);

    if (gSaveContext.save.saveInfo.playerData.rupees != 0) {
        SET_EVENTINF(EVENTINF_THREEDAYRESET_LOST_RUPEES);
    }

    if (INV_CONTENT(ITEM_BOMB) == ITEM_BOMB) {
        item = INV_CONTENT(ITEM_BOMB);
        if (AMMO(item) != 0) {
            SET_EVENTINF(EVENTINF_THREEDAYRESET_LOST_BOMB_AMMO);
        }
    }
    if (INV_CONTENT(ITEM_DEKU_NUT) == ITEM_DEKU_NUT) {
        item = INV_CONTENT(ITEM_DEKU_NUT);
        if (AMMO(item) != 0) {
            SET_EVENTINF(EVENTINF_THREEDAYRESET_LOST_NUT_AMMO);
        }
    }
    if (INV_CONTENT(ITEM_DEKU_STICK) == ITEM_DEKU_STICK) {
        item = INV_CONTENT(ITEM_DEKU_STICK);
        if (AMMO(item) != 0) {
            SET_EVENTINF(EVENTINF_THREEDAYRESET_LOST_STICK_AMMO);
        }
    }
    if (INV_CONTENT(ITEM_BOW) == ITEM_BOW) {
        item = INV_CONTENT(ITEM_BOW);
        if (AMMO(item) != 0) {
            SET_EVENTINF(EVENTINF_THREEDAYRESET_LOST_ARROW_AMMO);
        }
    }

    for (i = 0; i < ITEM_NUM_SLOTS; i++) {
        if (gAmmoItems[i] != ITEM_NONE) {
            if ((gSaveContext.save.saveInfo.inventory.items[i] != ITEM_NONE) && (i != SLOT_PICTOGRAPH_BOX)) {
                item = gSaveContext.save.saveInfo.inventory.items[i];
                AMMO(item) = 0;
            }
        }
    }

    for (i = SLOT_BOTTLE_1; i <= SLOT_BOTTLE_6; i++) {
        // Check for all bottled items
        if (gSaveContext.save.saveInfo.inventory.items[i] >= ITEM_POTION_RED) {
            if (gSaveContext.save.saveInfo.inventory.items[i] <= ITEM_OBABA_DRINK) {
                for (j = EQUIP_SLOT_C_LEFT; j <= EQUIP_SLOT_C_RIGHT; j++) {
                    if (GET_CUR_FORM_BTN_ITEM(j) == gSaveContext.save.saveInfo.inventory.items[i]) {
                        SET_CUR_FORM_BTN_ITEM(j, ITEM_BOTTLE);
                        Interface_LoadItemIconImpl(play, j);
                    }
                }
                // #region 2S2H [Dpad]
                for (s32 k = EQUIP_SLOT_D_RIGHT; k <= EQUIP_SLOT_D_UP; k++) {
                    if (DPAD_GET_CUR_FORM_BTN_ITEM(k) == gSaveContext.save.saveInfo.inventory.items[i]) {
                        DPAD_SET_CUR_FORM_BTN_ITEM(k, ITEM_BOTTLE);
                        Interface_Dpad_LoadItemIconImpl(play, k);
                    }
                }
                // #endregion
                gSaveContext.save.saveInfo.inventory.items[i] = ITEM_BOTTLE;
            }
        }
    }

    REMOVE_QUEST_ITEM(QUEST_PICTOGRAPH);

    if (gSaveContext.save.saveInfo.playerData.health < 0x30) {
        gSaveContext.save.saveInfo.playerData.health = 0x30;
    }

    if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) <= EQUIP_VALUE_SWORD_RAZOR) {
        SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_KOKIRI);

        if (CUR_FORM == 0) {
            if ((STOLEN_ITEM_1 >= ITEM_SWORD_GILDED) || (STOLEN_ITEM_2 >= ITEM_SWORD_GILDED)) {
                CUR_FORM_EQUIP(EQUIP_SLOT_B) = ITEM_SWORD_GILDED;
                SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_GILDED);
            } else {
                CUR_FORM_EQUIP(EQUIP_SLOT_B) = ITEM_SWORD_KOKIRI;
            }
        } else {
            if ((STOLEN_ITEM_1 >= ITEM_SWORD_GILDED) || (STOLEN_ITEM_2 >= ITEM_SWORD_GILDED)) {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_SWORD_GILDED;
                SET_EQUIP_VALUE(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_GILDED);
            } else {
                BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_B) = ITEM_SWORD_KOKIRI;
            }
        }
    }

    if ((STOLEN_ITEM_1 == ITEM_SWORD_GREAT_FAIRY) || (STOLEN_ITEM_2 == ITEM_SWORD_GREAT_FAIRY)) {
        INV_CONTENT(ITEM_SWORD_GREAT_FAIRY) = ITEM_SWORD_GREAT_FAIRY;
    }

    if (STOLEN_ITEM_1 == ITEM_BOTTLE) {
        slot = SLOT(ITEM_BOTTLE);
        for (i = BOTTLE_FIRST; i < BOTTLE_MAX; i++) {
            if (gSaveContext.save.saveInfo.inventory.items[slot + i] == ITEM_NONE) {
                gSaveContext.save.saveInfo.inventory.items[slot + i] = ITEM_BOTTLE;
                break;
            }
        }
    }

    if (STOLEN_ITEM_2 == ITEM_BOTTLE) {
        slot = SLOT(ITEM_BOTTLE);
        for (i = BOTTLE_FIRST; i < BOTTLE_MAX; i++) {
            if (gSaveContext.save.saveInfo.inventory.items[slot + i] == ITEM_NONE) {
                gSaveContext.save.saveInfo.inventory.items[slot + i] = ITEM_BOTTLE;
                break;
            }
        }
    }

    SET_STOLEN_ITEM_1(STOLEN_ITEM_NONE);
    SET_STOLEN_ITEM_2(STOLEN_ITEM_NONE);

    Inventory_DeleteItem(ITEM_OCARINA_FAIRY, SLOT_TRADE_DEED);
    Inventory_DeleteItem(ITEM_SLINGSHOT, SLOT_TRADE_KEY_MAMA);
    Inventory_DeleteItem(ITEM_LONGSHOT, SLOT_TRADE_COUPLE);

    for (j = EQUIP_SLOT_C_LEFT; j <= EQUIP_SLOT_C_RIGHT; j++) {
        if ((GET_CUR_FORM_BTN_ITEM(j) >= ITEM_MOONS_TEAR) && (GET_CUR_FORM_BTN_ITEM(j) <= ITEM_PENDANT_OF_MEMORIES)) {
            SET_CUR_FORM_BTN_ITEM(j, ITEM_NONE);
            Interface_LoadItemIconImpl(play, j);
        }
    }
    // #region 2S2H [Dpad]
    for (s32 k = EQUIP_SLOT_D_RIGHT; k <= EQUIP_SLOT_D_UP; k++) {
        if ((DPAD_GET_CUR_FORM_BTN_ITEM(k) >= ITEM_MOONS_TEAR) &&
            (DPAD_GET_CUR_FORM_BTN_ITEM(k) <= ITEM_PENDANT_OF_MEMORIES)) {
            DPAD_SET_CUR_FORM_BTN_ITEM(k, ITEM_NONE);
            Interface_Dpad_LoadItemIconImpl(play, k);
        }
    }
    // #endregion

    gSaveContext.save.saveInfo.skullTokenCount &= ~0xFFFF0000;
    gSaveContext.save.saveInfo.skullTokenCount &= ~0x0000FFFF;
    gSaveContext.save.saveInfo.unk_EA0 = 0;

    gSaveContext.save.saveInfo.alienInfo[0] = 0;
    gSaveContext.save.saveInfo.alienInfo[1] = 0;
    gSaveContext.save.saveInfo.alienInfo[2] = 0;
    gSaveContext.save.saveInfo.alienInfo[3] = 0;
    gSaveContext.save.saveInfo.alienInfo[4] = 0;
    gSaveContext.save.saveInfo.alienInfo[5] = 0;
    gSaveContext.save.saveInfo.alienInfo[6] = 0;

    Sram_ClearHighscores();

    for (i = 0; i < 8; i++) {
        gSaveContext.save.saveInfo.inventory.dungeonItems[i] &= (u8)~1; // remove boss key
        DUNGEON_KEY_COUNT(i) = 0;
        gSaveContext.save.saveInfo.inventory.strayFairies[i] = 0;
    }

    gSaveContext.save.saveInfo.playerData.rupees = 0;
    gSaveContext.save.saveInfo.scarecrowSpawnSongSet = false;
    gSaveContext.powderKegTimer = 0;
    gSaveContext.unk_1014 = 0;
    gSaveContext.jinxTimer = 0;
    gSaveContext.rupeeAccumulator = 0;

    Horse_ResetHorseData(play);
    GameInteractor_ExecuteAfterEndOfCycleSave();
}

void Sram_IncrementDay(void) {
    if (CURRENT_DAY <= 3) {
        gSaveContext.save.day++;
        gSaveContext.save.eventDayCount++;
    }

    gSaveContext.save.saveInfo.bombersCaughtNum = 0;
    gSaveContext.save.saveInfo.bombersCaughtOrder[0] = 0;
    gSaveContext.save.saveInfo.bombersCaughtOrder[1] = 0;
    gSaveContext.save.saveInfo.bombersCaughtOrder[2] = 0;
    gSaveContext.save.saveInfo.bombersCaughtOrder[3] = 0;
    gSaveContext.save.saveInfo.bombersCaughtOrder[4] = 0;

    CLEAR_WEEKEVENTREG(WEEKEVENTREG_73_10);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_85_02);
}

u16 Sram_CalcChecksum(void* data, size_t count) {
    u8* dataPtr = data;
    u16 chkSum = 0;

    while (count-- > 0) {
        chkSum += *dataPtr;
        dataPtr++;
    }
    return chkSum;
}

// Resets `Save` substruct
void Sram_ResetSave(void) {
    gSaveContext.save.entrance = ENTRANCE(CUTSCENE, 0);
    gSaveContext.save.equippedMask = 0;
    gSaveContext.save.isFirstCycle = false;
    gSaveContext.save.unk_06 = 0;
    gSaveContext.save.linkAge = 0;
    gSaveContext.save.isNight = false;
    gSaveContext.save.timeSpeedOffset = 0;
    gSaveContext.save.snowheadCleared = 0;
    gSaveContext.save.hasTatl = false;
    gSaveContext.save.isOwlSave = false;

    memset(&gSaveContext.save.saveInfo, 0, sizeof(SaveInfo));
    // 2S2H
    memset(&gSaveContext.save.shipSaveInfo, 0, sizeof(ShipSaveInfo));
}

/**
 * Initializes with random values the following fields:
 * - lotteryCodes
 * - spiderHouseMaskOrder
 * - bomberCode
 */
void Sram_GenerateRandomSaveFields(void) {
    s32 randBombers;
    s16 sp2A;
    s16 pad;
    s16 i;
    s16 j;
    s32 k;
    s16 randSpiderHouse;

    Sram_ClearHighscores();

    gSaveContext.save.saveInfo.lotteryCodes[0][0] = Rand_S16Offset(0, 10);
    gSaveContext.save.saveInfo.lotteryCodes[0][1] = Rand_S16Offset(0, 10);
    gSaveContext.save.saveInfo.lotteryCodes[0][2] = Rand_S16Offset(0, 10);
    gSaveContext.save.saveInfo.lotteryCodes[1][0] = Rand_S16Offset(0, 10);
    gSaveContext.save.saveInfo.lotteryCodes[1][1] = Rand_S16Offset(0, 10);
    gSaveContext.save.saveInfo.lotteryCodes[1][2] = Rand_S16Offset(0, 10);
    gSaveContext.save.saveInfo.lotteryCodes[2][0] = Rand_S16Offset(0, 10);
    gSaveContext.save.saveInfo.lotteryCodes[2][1] = Rand_S16Offset(0, 10);
    gSaveContext.save.saveInfo.lotteryCodes[2][2] = Rand_S16Offset(0, 10);

    // Needed to match...
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {}
    }

    i = 0;
    sp2A = Rand_S16Offset(0, 16) & 3;
    k = 6;
    while (i != k) {
        randSpiderHouse = Rand_S16Offset(0, 16) & 3;
        if (sp2A != randSpiderHouse) {
            gSaveContext.save.saveInfo.spiderHouseMaskOrder[i] = randSpiderHouse;
            i++;
            sp2A = randSpiderHouse;
        }
    }

    do {
        randBombers = Rand_S16Offset(0, 6);
    } while ((randBombers <= 0) || (randBombers >= 6));

    gSaveContext.save.saveInfo.bomberCode[0] = randBombers;

    i = 1;
    while (i != 5) {
        k = false;

        do {
            randBombers = Rand_S16Offset(0, 6);
        } while ((randBombers <= 0) || (randBombers >= 6));

        sp2A = 0;
        do {
            if (randBombers == gSaveContext.save.saveInfo.bomberCode[sp2A]) {
                k = true;
            }
            sp2A++;
        } while (sp2A < i);

        if (k == false) {
            gSaveContext.save.saveInfo.bomberCode[i] = randBombers;
            i++;
        }
    }
}

SavePlayerData sSaveDefaultPlayerData = {
    { '\0', '\0', '\0', '\0', '\0', '\0' },             // newf
    0,                                                  // threeDayResetCount
    { 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E }, // playerName "        "
    0x30,                                               // healthCapacity
    0x30,                                               // health
    0,                                                  // magicLevel
    MAGIC_NORMAL_METER,                                 // magic
    0,                                                  // rupees
    0,                                                  // swordHealth
    0,                                                  // tatlTimer
    false,                                              // isMagicAcquired
    false,                                              // isDoubleMagicAcquired
    0,                                                  // doubleDefense
    0,                                                  // unk_1F
    OWL_WARP_NONE,                                      // owlWarpId
    0,                                                  // owlActivationFlags
    0xFF,                                               // unk_24
    SCENE_SPOT00,                                       // savedSceneId
};

ItemEquips sSaveDefaultItemEquips = {
    {
        { ITEM_SWORD_KOKIRI, ITEM_NONE, ITEM_NONE, ITEM_NONE },
        { ITEM_SWORD_KOKIRI, ITEM_NONE, ITEM_NONE, ITEM_NONE },
        { ITEM_SWORD_KOKIRI, ITEM_NONE, ITEM_NONE, ITEM_NONE },
        { ITEM_FD, ITEM_NONE, ITEM_NONE, ITEM_NONE },
    },
    {
        { SLOT_OCARINA, SLOT_NONE, SLOT_NONE, SLOT_NONE },
        { SLOT_NONE, SLOT_NONE, SLOT_NONE, SLOT_NONE },
        { SLOT_NONE, SLOT_NONE, SLOT_NONE, SLOT_NONE },
        { SLOT_NONE, SLOT_NONE, SLOT_NONE, SLOT_NONE },
    },
    0x11,
};

// #region 2S2H
DpadSaveInfo sSaveDefaultDpadItemEquips = {
    {
        { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE },
        { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE },
        { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE },
        { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE },
    },
    {
        { SLOT_NONE, SLOT_NONE, SLOT_NONE, SLOT_NONE },
        { SLOT_NONE, SLOT_NONE, SLOT_NONE, SLOT_NONE },
        { SLOT_NONE, SLOT_NONE, SLOT_NONE, SLOT_NONE },
        { SLOT_NONE, SLOT_NONE, SLOT_NONE, SLOT_NONE },
    },
};
// #endregion

Inventory sSaveDefaultInventory = {
    // items
    {
        ITEM_NONE, // SLOT_OCARINA
        ITEM_NONE, // SLOT_BOW
        ITEM_NONE, // SLOT_ARROW_FIRE
        ITEM_NONE, // SLOT_ARROW_ICE
        ITEM_NONE, // SLOT_ARROW_LIGHT
        ITEM_NONE, // SLOT_TRADE_DEED
        ITEM_NONE, // SLOT_BOMB
        ITEM_NONE, // SLOT_BOMBCHU
        ITEM_NONE, // SLOT_DEKU_STICK
        ITEM_NONE, // SLOT_DEKU_NUT
        ITEM_NONE, // SLOT_MAGIC_BEANS
        ITEM_NONE, // SLOT_TRADE_KEY_MAMA
        ITEM_NONE, // SLOT_POWDER_KEG
        ITEM_NONE, // SLOT_PICTOGRAPH_BOX
        ITEM_NONE, // SLOT_LENS_OF_TRUTH
        ITEM_NONE, // SLOT_HOOKSHOT
        ITEM_NONE, // SLOT_SWORD_GREAT_FAIRY
        ITEM_NONE, // SLOT_TRADE_COUPLE
        ITEM_NONE, // SLOT_BOTTLE_1
        ITEM_NONE, // SLOT_BOTTLE_2
        ITEM_NONE, // SLOT_BOTTLE_3
        ITEM_NONE, // SLOT_BOTTLE_4
        ITEM_NONE, // SLOT_BOTTLE_5
        ITEM_NONE, // SLOT_BOTTLE_6
        ITEM_NONE, // SLOT_MASK_POSTMAN
        ITEM_NONE, // SLOT_MASK_ALL_NIGHT
        ITEM_NONE, // SLOT_MASK_BLAST
        ITEM_NONE, // SLOT_MASK_STONE
        ITEM_NONE, // SLOT_MASK_GREAT_FAIRY
        ITEM_NONE, // SLOT_MASK_DEKU
        ITEM_NONE, // SLOT_MASK_KEATON
        ITEM_NONE, // SLOT_MASK_BREMEN
        ITEM_NONE, // SLOT_MASK_BUNNY
        ITEM_NONE, // SLOT_MASK_DON_GERO
        ITEM_NONE, // SLOT_MASK_SCENTS
        ITEM_NONE, // SLOT_MASK_GORON
        ITEM_NONE, // SLOT_MASK_ROMANI
        ITEM_NONE, // SLOT_MASK_CIRCUS_LEADER
        ITEM_NONE, // SLOT_MASK_KAFEIS_MASK
        ITEM_NONE, // SLOT_MASK_COUPLE
        ITEM_NONE, // SLOT_MASK_TRUTH
        ITEM_NONE, // SLOT_MASK_ZORA
        ITEM_NONE, // SLOT_MASK_KAMARO
        ITEM_NONE, // SLOT_MASK_GIBDO
        ITEM_NONE, // SLOT_MASK_GARO
        ITEM_NONE, // SLOT_MASK_CAPTAIN
        ITEM_NONE, // SLOT_MASK_GIANT
        ITEM_NONE, // SLOT_MASK_FIERCE_DEITY
    },
    // ammo
    {
        0, // SLOT_OCARINA
        0, // SLOT_BOW
        0, // SLOT_ARROW_FIRE
        0, // SLOT_ARROW_ICE
        0, // SLOT_ARROW_LIGHT
        0, // SLOT_TRADE_DEED
        0, // SLOT_BOMB
        0, // SLOT_BOMBCHU
        0, // SLOT_DEKU_STICK
        0, // SLOT_DEKU_NUT
        0, // SLOT_MAGIC_BEANS
        0, // SLOT_TRADE_KEY_MAMA
        0, // SLOT_POWDER_KEG
        0, // SLOT_PICTOGRAPH_BOX
        0, // SLOT_LENS_OF_TRUTH
        0, // SLOT_HOOKSHOT
        0, // SLOT_SWORD_GREAT_FAIRY
        0, // SLOT_TRADE_COUPLE
        0, // SLOT_BOTTLE_1
        0, // SLOT_BOTTLE_2
        0, // SLOT_BOTTLE_3
        0, // SLOT_BOTTLE_4
        0, // SLOT_BOTTLE_5
        0, // SLOT_BOTTLE_6
    },
    // upgrades
    (0 << 0) |      // UPG_QUIVER
        (0 << 3) |  // UPG_BOMB_BAG
        (0 << 6) |  // UPG_STRENGTH
        (0 << 9) |  // UPG_SCALE
        (0 << 12) | // UPG_WALLET
        (0 << 14) | // UPG_BULLET_BAG
        (1 << 17) | // UPG_DEKU_STICKS
        (1 << 20),  // UPG_DEKU_NUTS
    // questItems
    0,
    // dungeonItems
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    // dungeonKeys
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
    // defenseHearts
    0,
    // strayFairies
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    // dekuPlaygroundPlayerName
    {
        // "LINK    "
        { 0x15, 0x12, 0x17, 0x14, 0x3E, 0x3E, 0x3E, 0x3E },
        // "LINK    "
        { 0x15, 0x12, 0x17, 0x14, 0x3E, 0x3E, 0x3E, 0x3E },
        // "LINK    "
        { 0x15, 0x12, 0x17, 0x14, 0x3E, 0x3E, 0x3E, 0x3E },
    },
};

u16 sSaveDefaultChecksum = 0;

/**
 *  Initialize new save.
 *  This save has an empty inventory with 3 hearts, sword and shield.
 */
void Sram_InitNewSave(void) {
    gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
    gSaveContext.save.eventDayCount = 0;
    gSaveContext.save.day = 0;
    gSaveContext.save.time = CLOCK_TIME(6, 0) - 1;
    Sram_ResetSave();

    memcpy(&gSaveContext.save.saveInfo.playerData, &sSaveDefaultPlayerData, sizeof(SavePlayerData));
    memcpy(&gSaveContext.save.saveInfo.equips, &sSaveDefaultItemEquips, sizeof(ItemEquips));
    memcpy(&gSaveContext.save.saveInfo.inventory, &sSaveDefaultInventory, sizeof(Inventory));
    memcpy(&gSaveContext.save.saveInfo.checksum, &sSaveDefaultChecksum, sizeof(gSaveContext.save.saveInfo.checksum));

    gSaveContext.save.saveInfo.horseData.sceneId = SCENE_F01;
    gSaveContext.save.saveInfo.horseData.pos.x = -1420;
    gSaveContext.save.saveInfo.horseData.pos.y = 257;
    gSaveContext.save.saveInfo.horseData.pos.z = -1285;
    gSaveContext.save.saveInfo.horseData.yaw = -0x7554;

    gSaveContext.nextCutsceneIndex = 0;
    gSaveContext.save.saveInfo.playerData.magicLevel = 0;

    // #region 2S2H
    memcpy(&gSaveContext.save.shipSaveInfo.dpadEquips, &sSaveDefaultDpadItemEquips, sizeof(DpadSaveInfo));
    memcpy(&gSaveContext.save.shipSaveInfo.commitHash, &gGitCommitHash,
           sizeof(gSaveContext.save.shipSaveInfo.commitHash));
    gSaveContext.save.shipSaveInfo.pauseSaveEntrance = -1;
    gSaveContext.save.shipSaveInfo.persistentBunnyHood = false;
    gSaveContext.save.shipSaveInfo.saveType = SAVETYPE_VANILLA;
    gSaveContext.save.shipSaveInfo.fileCreatedAt = 0;
    gSaveContext.save.shipSaveInfo.fileCompletedAt = 0;
    gSaveContext.save.shipSaveInfo.filePlaytime = 0;
    gSaveContext.shipSaveContext.lastTimeLog = 0;
    //  #endregion

    Sram_GenerateRandomSaveFields();
}

SavePlayerData sSaveDebugPlayerData = {
    { 'Z', 'E', 'L', 'D', 'A', '3' },                   // newf
    0,                                                  // threeDayResetCount
    { 0x15, 0x12, 0x17, 0x14, 0x3E, 0x3E, 0x3E, 0x3E }, // playerName "LINK    "
    0x80,                                               // healthCapacity
    0x80,                                               // health
    0,                                                  // magicLevel
    MAGIC_NORMAL_METER,                                 // magic
    50,                                                 // rupees
    100,                                                // swordHealth
    0,                                                  // tatlTimer
    true,                                               // isMagicAcquired
    false,                                              // isDoubleMagicAcquired
    0,                                                  // doubleDefense
    0,                                                  // unk_1F
    OWL_WARP_NONE,                                      // owlWarpId
    0,                                                  // owlActivationFlags
    0xFF,                                               // unk_24
    SCENE_SPOT00,                                       // savedSceneId
};

ItemEquips sSaveDebugItemEquips = {
    {
        { ITEM_SWORD_KOKIRI, ITEM_BOW, ITEM_POTION_RED, ITEM_OCARINA_OF_TIME },
        { ITEM_SWORD_KOKIRI, ITEM_BOW, ITEM_MASK_GORON, ITEM_OCARINA_OF_TIME },
        { ITEM_SWORD_KOKIRI, ITEM_BOW, ITEM_MASK_ZORA, ITEM_OCARINA_OF_TIME },
        { ITEM_DEKU_NUT, ITEM_DEKU_NUT, ITEM_MASK_DEKU, ITEM_OCARINA_OF_TIME },
    },
    {
        { SLOT_OCARINA, SLOT_BOW, SLOT_BOTTLE_2, SLOT_OCARINA },
        { SLOT_OCARINA, SLOT_MAGIC_BEANS, SLOT_MASK_GORON, SLOT_BOMBCHU },
        { SLOT_OCARINA, SLOT_POWDER_KEG, SLOT_MASK_ZORA, SLOT_BOMBCHU },
        { SLOT_OCARINA, SLOT_BOW, SLOT_MASK_DEKU, SLOT_BOMBCHU },
    },
    0x11,
};

Inventory sSaveDebugInventory = {
    // items
    {
        ITEM_OCARINA_OF_TIME,    // SLOT_OCARINA
        ITEM_BOW,                // SLOT_BOW
        ITEM_ARROW_FIRE,         // SLOT_ARROW_FIRE
        ITEM_ARROW_ICE,          // SLOT_ARROW_ICE
        ITEM_ARROW_LIGHT,        // SLOT_ARROW_LIGHT
        ITEM_MOONS_TEAR,         // SLOT_TRADE_DEED
        ITEM_BOMB,               // SLOT_BOMB
        ITEM_BOMBCHU,            // SLOT_BOMBCHU
        ITEM_DEKU_STICK,         // SLOT_DEKU_STICK
        ITEM_DEKU_NUT,           // SLOT_DEKU_NUT
        ITEM_MAGIC_BEANS,        // SLOT_MAGIC_BEANS
        ITEM_ROOM_KEY,           // SLOT_TRADE_KEY_MAMA
        ITEM_POWDER_KEG,         // SLOT_POWDER_KEG
        ITEM_PICTOGRAPH_BOX,     // SLOT_PICTOGRAPH_BOX
        ITEM_LENS_OF_TRUTH,      // SLOT_LENS_OF_TRUTH
        ITEM_HOOKSHOT,           // SLOT_HOOKSHOT
        ITEM_SWORD_GREAT_FAIRY,  // SLOT_SWORD_GREAT_FAIRY
        ITEM_LETTER_TO_KAFEI,    // SLOT_TRADE_COUPLE
        ITEM_BOTTLE,             // SLOT_BOTTLE_1
        ITEM_POTION_RED,         // SLOT_BOTTLE_2
        ITEM_POTION_GREEN,       // SLOT_BOTTLE_3
        ITEM_POTION_BLUE,        // SLOT_BOTTLE_4
        ITEM_NONE,               // SLOT_BOTTLE_5
        ITEM_NONE,               // SLOT_BOTTLE_6
        ITEM_MASK_POSTMAN,       // SLOT_MASK_POSTMAN
        ITEM_MASK_ALL_NIGHT,     // SLOT_MASK_ALL_NIGHT
        ITEM_MASK_BLAST,         // SLOT_MASK_BLAST
        ITEM_MASK_STONE,         // SLOT_MASK_STONE
        ITEM_MASK_GREAT_FAIRY,   // SLOT_MASK_GREAT_FAIRY
        ITEM_MASK_DEKU,          // SLOT_MASK_DEKU
        ITEM_MASK_KEATON,        // SLOT_MASK_KEATON
        ITEM_MASK_BREMEN,        // SLOT_MASK_BREMEN
        ITEM_MASK_BUNNY,         // SLOT_MASK_BUNNY
        ITEM_MASK_DON_GERO,      // SLOT_MASK_DON_GERO
        ITEM_MASK_SCENTS,        // SLOT_MASK_SCENTS
        ITEM_MASK_GORON,         // SLOT_MASK_GORON
        ITEM_MASK_ROMANI,        // SLOT_MASK_ROMANI
        ITEM_MASK_CIRCUS_LEADER, // SLOT_MASK_CIRCUS_LEADER
        ITEM_MASK_KAFEIS_MASK,   // SLOT_MASK_KAFEIS_MASK
        ITEM_MASK_COUPLE,        // SLOT_MASK_COUPLE
        ITEM_MASK_TRUTH,         // SLOT_MASK_TRUTH
        ITEM_MASK_ZORA,          // SLOT_MASK_ZORA
        ITEM_MASK_KAMARO,        // SLOT_MASK_KAMARO
        ITEM_MASK_GIBDO,         // SLOT_MASK_GIBDO
        ITEM_MASK_GARO,          // SLOT_MASK_GARO
        ITEM_MASK_CAPTAIN,       // SLOT_MASK_CAPTAIN
        ITEM_MASK_GIANT,         // SLOT_MASK_GIANT
        ITEM_MASK_FIERCE_DEITY,  // SLOT_MASK_FIERCE_DEITY
    },
    // ammo
    {
        1,  // SLOT_OCARINA
        30, // SLOT_BOW
        1,  // SLOT_ARROW_FIRE
        1,  // SLOT_ARROW_ICE
        1,  // SLOT_ARROW_LIGHT
        1,  // SLOT_TRADE_DEED
        30, // SLOT_BOMB
        30, // SLOT_BOMBCHU
        30, // SLOT_DEKU_STICK
        30, // SLOT_DEKU_NUT
        1,  // SLOT_MAGIC_BEANS
        1,  // SLOT_TRADE_KEY_MAMA
        1,  // SLOT_POWDER_KEG
        1,  // SLOT_PICTOGRAPH_BOX
        30, // SLOT_LENS_OF_TRUTH
        1,  // SLOT_HOOKSHOT
        1,  // SLOT_SWORD_GREAT_FAIRY
        1,  // SLOT_TRADE_COUPLE
        1,  // SLOT_BOTTLE_1
        1,  // SLOT_BOTTLE_2
        1,  // SLOT_BOTTLE_3
        1,  // SLOT_BOTTLE_4
        0,  // SLOT_BOTTLE_5
        0,  // SLOT_BOTTLE_6
    },
    // upgrades
    (1 << 0) |      // UPG_QUIVER
        (1 << 3) |  // UPG_BOMB_BAG
        (0 << 6) |  // UPG_STRENGTH
        (0 << 9) |  // UPG_SCALE
        (0 << 12) | // UPG_WALLET
        (0 << 14) | // UPG_BULLET_BAG
        (1 << 17) | // UPG_DEKU_STICKS
        (1 << 20),  // UPG_DEKU_NUTS
    // questItems
    (1 << QUEST_SONG_SONATA) | (1 << QUEST_SONG_LULLABY) | (1 << QUEST_SONG_BOSSA_NOVA) | (1 << QUEST_SONG_ELEGY) |
        (1 << QUEST_SONG_OATH) | (1 << QUEST_SONG_TIME) | (1 << QUEST_SONG_HEALING) | (1 << QUEST_SONG_EPONA) |
        (1 << QUEST_SONG_SOARING) | (1 << QUEST_SONG_STORMS) | (1 << QUEST_BOMBERS_NOTEBOOK) |
        (1 << QUEST_SONG_LULLABY_INTRO),
    // dungeonItems
    {
        (1 << DUNGEON_BOSS_KEY) | (1 << DUNGEON_COMPASS) | (1 << DUNGEON_MAP),
        (1 << DUNGEON_BOSS_KEY) | (1 << DUNGEON_COMPASS) | (1 << DUNGEON_MAP),
        (1 << DUNGEON_BOSS_KEY) | (1 << DUNGEON_COMPASS) | (1 << DUNGEON_MAP),
        (1 << DUNGEON_BOSS_KEY) | (1 << DUNGEON_COMPASS) | (1 << DUNGEON_MAP),
        (1 << DUNGEON_BOSS_KEY) | (1 << DUNGEON_COMPASS) | (1 << DUNGEON_MAP),
        (1 << DUNGEON_BOSS_KEY) | (1 << DUNGEON_COMPASS) | (1 << DUNGEON_MAP),
        (1 << DUNGEON_BOSS_KEY) | (1 << DUNGEON_COMPASS) | (1 << DUNGEON_MAP),
        (1 << DUNGEON_BOSS_KEY) | (1 << DUNGEON_COMPASS) | (1 << DUNGEON_MAP),
        (1 << DUNGEON_BOSS_KEY) | (1 << DUNGEON_COMPASS) | (1 << DUNGEON_MAP),
        (1 << DUNGEON_BOSS_KEY) | (1 << DUNGEON_COMPASS) | (1 << DUNGEON_MAP),
    },
    // dungeonKeys
    { 8, 8, 8, 8, 8, 8, 8, 8, 8 },
    // defenseHearts
    0,
    // strayFairies
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    // dekuPlaygroundPlayerName
    {
        // "LINK    "
        { 0x15, 0x12, 0x17, 0x14, 0x3E, 0x3E, 0x3E, 0x3E },
        // "LINK    "
        { 0x15, 0x12, 0x17, 0x14, 0x3E, 0x3E, 0x3E, 0x3E },
        // "LINK    "
        { 0x15, 0x12, 0x17, 0x14, 0x3E, 0x3E, 0x3E, 0x3E },
    },
};

u16 sSaveDebugChecksum = 0;

u8 D_801C6A48[PLAYER_FORM_MAX] = {
    ITEM_MASK_FIERCE_DEITY, // PLAYER_FORM_FIERCE_DEITY
    ITEM_MASK_GORON,        // PLAYER_FORM_GORON
    ITEM_MASK_ZORA,         // PLAYER_FORM_ZORA
    ITEM_MASK_DEKU,         // PLAYER_FORM_DEKU
    ITEM_MASK_FIERCE_DEITY, // PLAYER_FORM_HUMAN
};

u8 D_801C6A50[PLAYER_FORM_MAX] = {
    SLOT_MASK_FIERCE_DEITY, // PLAYER_FORM_FIERCE_DEITY
    SLOT_MASK_GORON,        // PLAYER_FORM_GORON
    SLOT_MASK_ZORA,         // PLAYER_FORM_ZORA
    SLOT_MASK_DEKU,         // PLAYER_FORM_DEKU
    SLOT_MASK_FIERCE_DEITY, // PLAYER_FORM_HUMAN
};

/**
 *  Initialize debug save. This is also used on the Title Screen
 *  This save has a mostly full inventory, every mask and 10 hearts.
 *
 *  Some noteable flags that are set:
 *  TODO: Investigate the flags
 */
void Sram_InitDebugSave(void) {
    Sram_ResetSave();

    memcpy(&gSaveContext.save.saveInfo.playerData, &sSaveDebugPlayerData, sizeof(SavePlayerData));
    memcpy(&gSaveContext.save.saveInfo.equips, &sSaveDebugItemEquips, sizeof(ItemEquips));
    memcpy(&gSaveContext.save.saveInfo.inventory, &sSaveDebugInventory, sizeof(Inventory));
    memcpy(&gSaveContext.save.saveInfo.checksum, &sSaveDebugChecksum, sizeof(gSaveContext.save.saveInfo.checksum));

    if (GET_PLAYER_FORM != PLAYER_FORM_HUMAN) {
        BUTTON_ITEM_EQUIP(0, EQUIP_SLOT_C_DOWN) = D_801C6A48[GET_PLAYER_FORM];
        C_SLOT_EQUIP(0, EQUIP_SLOT_C_DOWN) = D_801C6A50[GET_PLAYER_FORM];
    }

    gSaveContext.save.hasTatl = true;

    gSaveContext.save.saveInfo.horseData.sceneId = SCENE_F01;
    gSaveContext.save.saveInfo.horseData.pos.x = -1420;
    gSaveContext.save.saveInfo.horseData.pos.y = 257;
    gSaveContext.save.saveInfo.horseData.pos.z = -1285;
    gSaveContext.save.saveInfo.horseData.yaw = -0x7554;

    gSaveContext.save.entrance = ENTRANCE(CUTSCENE, 0);
    gSaveContext.save.isFirstCycle = true;

    SET_WEEKEVENTREG(WEEKEVENTREG_15_20);
    SET_WEEKEVENTREG(WEEKEVENTREG_59_04);
    SET_WEEKEVENTREG(WEEKEVENTREG_31_04);

    gSaveContext.cycleSceneFlags[SCENE_INSIDETOWER].switch0 = 1;
    gSaveContext.save.saveInfo.permanentSceneFlags[SCENE_INSIDETOWER].switch0 = 1;
    gSaveContext.save.saveInfo.playerData.magicLevel = 0;

    // #region 2S2H
    memcpy(&gSaveContext.save.shipSaveInfo.dpadEquips, &sSaveDefaultDpadItemEquips, sizeof(DpadSaveInfo));
    memcpy(&gSaveContext.save.shipSaveInfo.commitHash, &gGitCommitHash,
           sizeof(gSaveContext.save.shipSaveInfo.commitHash));
    gSaveContext.save.shipSaveInfo.pauseSaveEntrance = -1;
    gSaveContext.save.shipSaveInfo.persistentBunnyHood = false;
    gSaveContext.save.shipSaveInfo.saveType = SAVETYPE_VANILLA;
    gSaveContext.save.shipSaveInfo.fileCreatedAt = 0;
    gSaveContext.save.shipSaveInfo.fileCompletedAt = 0;
    gSaveContext.save.shipSaveInfo.filePlaytime = 0;
    gSaveContext.shipSaveContext.lastTimeLog = 0;
    // #endregion

    Sram_GenerateRandomSaveFields();
}

void Sram_ResetSaveFromMoonCrash(SramContext* sramCtx) {
    s32 i;
    s32 cutsceneIndex = gSaveContext.save.cutsceneIndex;

    memset(sramCtx->saveBuf, 0, SAVE_BUFFER_SIZE);

    if (SysFlashrom_ReadData(sramCtx->saveBuf, gFlashSaveStartPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER],
                             gFlashSaveNumPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER]) != 0) {
        SysFlashrom_ReadData(
            sramCtx->saveBuf,
            gFlashSaveStartPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER + FLASH_SAVE_BACKUP_OFFSET],
            gFlashSaveNumPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER + FLASH_SAVE_BACKUP_OFFSET]);
    }
    memcpy(&gSaveContext.save, sramCtx->saveBuf, sizeof(Save));
    if (CHECK_NEWF(gSaveContext.save.saveInfo.playerData.newf)) {
        SysFlashrom_ReadData(
            sramCtx->saveBuf,
            gFlashSaveStartPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER + FLASH_SAVE_BACKUP_OFFSET],
            gFlashSaveNumPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER + FLASH_SAVE_BACKUP_OFFSET]);
        memcpy(&gSaveContext, sramCtx->saveBuf, sizeof(Save));
    }
    gSaveContext.save.cutsceneIndex = cutsceneIndex;

    for (i = 0; i < ARRAY_COUNT(gSaveContext.eventInf); i++) {
        gSaveContext.eventInf[i] = 0;
    }

    for (i = 0; i < ARRAY_COUNT(gSaveContext.cycleSceneFlags); i++) {
        gSaveContext.cycleSceneFlags[i].chest = gSaveContext.save.saveInfo.permanentSceneFlags[i].chest;
        gSaveContext.cycleSceneFlags[i].switch0 = gSaveContext.save.saveInfo.permanentSceneFlags[i].switch0;
        gSaveContext.cycleSceneFlags[i].switch1 = gSaveContext.save.saveInfo.permanentSceneFlags[i].switch1;
        gSaveContext.cycleSceneFlags[i].clearedRoom = gSaveContext.save.saveInfo.permanentSceneFlags[i].clearedRoom;
        gSaveContext.cycleSceneFlags[i].collectible = gSaveContext.save.saveInfo.permanentSceneFlags[i].collectible;
    }

    for (i = 0; i < TIMER_ID_MAX; i++) {
        gSaveContext.timerStates[i] = TIMER_STATE_OFF;
        gSaveContext.timerCurTimes[i] = SECONDS_TO_TIMER(0);
        gSaveContext.timerTimeLimits[i] = SECONDS_TO_TIMER(0);
        gSaveContext.timerStartOsTimes[i] = 0;
        gSaveContext.timerStopTimes[i] = SECONDS_TO_TIMER(0);
        gSaveContext.timerPausedOsTimes[i] = 0;
    }

    D_801BDAA0 = true;
    gHorseIsMounted = false;
    gSaveContext.powderKegTimer = 0;
    gSaveContext.unk_1014 = 0;
    gSaveContext.jinxTimer = 0;
}

static u16 sOwlWarpEntrances[OWL_WARP_MAX - 1] = {
    ENTRANCE(GREAT_BAY_COAST, 11),         // OWL_WARP_GREAT_BAY_COAST
    ENTRANCE(ZORA_CAPE, 6),                // OWL_WARP_ZORA_CAPE
    ENTRANCE(SNOWHEAD, 3),                 // OWL_WARP_SNOWHEAD
    ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 8),  // OWL_WARP_MOUNTAIN_VILLAGE
    ENTRANCE(SOUTH_CLOCK_TOWN, 9),         // OWL_WARP_CLOCK_TOWN
    ENTRANCE(MILK_ROAD, 4),                // OWL_WARP_MILK_ROAD
    ENTRANCE(WOODFALL, 4),                 // OWL_WARP_WOODFALL
    ENTRANCE(SOUTHERN_SWAMP_POISONED, 10), // OWL_WARP_SOUTHERN_SWAMP
    ENTRANCE(IKANA_CANYON, 4),             // OWL_WARP_IKANA_CANYON
    ENTRANCE(STONE_TOWER, 3),              // OWL_WARP_STONE_TOWER
};

void Sram_OpenSave(FileSelectState* fileSelect, SramContext* sramCtx) {
    s32 i;
    s32 pad;
    s32 phi_t1;
    s32 pad1;
    s32 fileNum;

    if (gSaveContext.flashSaveAvailable) {
        memset(sramCtx->saveBuf, 0, SAVE_BUFFER_SIZE);

        if (gSaveContext.fileNum == 0xFF) {
            SysFlashrom_ReadData(sramCtx->saveBuf, gFlashSaveStartPages[FLASH_SAVE_FILE_1_NEW_CYCLE_SAVE],
                                 gFlashSaveNumPages[FLASH_SAVE_FILE_1_NEW_CYCLE_SAVE]);
        } else if (fileSelect->isOwlSave[gSaveContext.fileNum + FILE_NUM_OWL_SAVE_OFFSET]) {
            phi_t1 = gSaveContext.fileNum + FILE_NUM_OWL_SAVE_OFFSET;
            phi_t1 *= FLASH_SAVE_MAIN_MULTIPLIER;

            if (SysFlashrom_ReadData(sramCtx->saveBuf, gFlashSaveStartPages[phi_t1], gFlashSaveNumPages[phi_t1]) != 0) {
                SysFlashrom_ReadData(sramCtx->saveBuf, gFlashSaveStartPages[phi_t1 + FLASH_SAVE_BACKUP_OFFSET],
                                     gFlashSaveNumPages[phi_t1 + FLASH_SAVE_BACKUP_OFFSET]);
            }
        } else {
            phi_t1 = gSaveContext.fileNum;
            phi_t1 *= FLASH_SAVE_MAIN_MULTIPLIER;

            if (SysFlashrom_ReadData(sramCtx->saveBuf, gFlashSaveStartPages[phi_t1], gFlashSaveNumPages[phi_t1]) != 0) {
                SysFlashrom_ReadData(sramCtx->saveBuf, gFlashSaveStartPages[phi_t1 + FLASH_SAVE_BACKUP_OFFSET],
                                     gFlashSaveNumPages[phi_t1 + FLASH_SAVE_BACKUP_OFFSET]);
            }
        }

        memcpy(&gSaveContext, sramCtx->saveBuf, gFlashSaveSizes[phi_t1]);

        if (CHECK_NEWF(gSaveContext.save.saveInfo.playerData.newf)) {
            SysFlashrom_ReadData(sramCtx->saveBuf, gFlashSaveStartPages[phi_t1 + FLASH_SAVE_BACKUP_OFFSET],
                                 gFlashSaveNumPages[phi_t1 + FLASH_SAVE_BACKUP_OFFSET]);
            memcpy(&gSaveContext, sramCtx->saveBuf, gFlashSaveSizes[phi_t1]);
        }
    }

    gSaveContext.save.saveInfo.playerData.magicLevel = 0;

    if (!gSaveContext.save.isOwlSave) {
        for (i = 0; i < ARRAY_COUNT(gSaveContext.eventInf); i++) {
            gSaveContext.eventInf[i] = 0;
        }

        for (i = 0; i < ARRAY_COUNT(gSaveContext.cycleSceneFlags); i++) {
            gSaveContext.cycleSceneFlags[i].chest = gSaveContext.save.saveInfo.permanentSceneFlags[i].chest;
            gSaveContext.cycleSceneFlags[i].switch0 = gSaveContext.save.saveInfo.permanentSceneFlags[i].switch0;
            gSaveContext.cycleSceneFlags[i].switch1 = gSaveContext.save.saveInfo.permanentSceneFlags[i].switch1;
            gSaveContext.cycleSceneFlags[i].clearedRoom = gSaveContext.save.saveInfo.permanentSceneFlags[i].clearedRoom;
            gSaveContext.cycleSceneFlags[i].collectible = gSaveContext.save.saveInfo.permanentSceneFlags[i].collectible;
        }

        for (i = 0; i < TIMER_ID_MAX; i++) {
            gSaveContext.timerStates[i] = TIMER_STATE_OFF;
            gSaveContext.timerCurTimes[i] = SECONDS_TO_TIMER(0);
            gSaveContext.timerTimeLimits[i] = SECONDS_TO_TIMER(0);
            gSaveContext.timerStartOsTimes[i] = 0;
            gSaveContext.timerStopTimes[i] = SECONDS_TO_TIMER(0);
            gSaveContext.timerPausedOsTimes[i] = 0;
        }

        if (gSaveContext.save.isFirstCycle) {
            gSaveContext.save.entrance = ENTRANCE(SOUTH_CLOCK_TOWN, 0);
            gSaveContext.save.day = 0;
            gSaveContext.save.time = CLOCK_TIME(6, 0) - 1;
        } else {
            gSaveContext.save.entrance = ENTRANCE(CUTSCENE, 0);
            gSaveContext.nextCutsceneIndex = 0;
            gSaveContext.save.playerForm = PLAYER_FORM_HUMAN;
        }
    } else {
        // When a pauseSaveEntrance is available, prioritize it over the
        // owlWarpId, this means the players last save was a pause save.
        if (gSaveContext.save.shipSaveInfo.pauseSaveEntrance != -1) {
            gSaveContext.save.entrance = gSaveContext.save.shipSaveInfo.pauseSaveEntrance;
        } else {
            // @bug When the player saves at the extra Owl statue in west clock town
            // their owlWarpId is set to 0xF. On hardware this results in the entrance being set to 0
            // 2S2H [Port] We opt to fix this by hardcoding entrance to 0 when owlWarpId is out of bounds
            if (gSaveContext.save.owlWarpId > OWL_WARP_MAX) {
                gSaveContext.save.entrance = 0;
            } else {
                gSaveContext.save.entrance = sOwlWarpEntrances[(void)0, gSaveContext.save.owlWarpId];
            }
        }
        if ((gSaveContext.save.entrance == ENTRANCE(SOUTHERN_SWAMP_POISONED, 10)) &&
            CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)) {
            gSaveContext.save.entrance = ENTRANCE(SOUTHERN_SWAMP_CLEARED, 10);
        } else if ((gSaveContext.save.entrance == ENTRANCE(MOUNTAIN_VILLAGE_WINTER, 8)) &&
                   CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_SNOWHEAD_TEMPLE)) {
            gSaveContext.save.entrance = ENTRANCE(MOUNTAIN_VILLAGE_SPRING, 8);
        }

        for (i = 0; i < ARRAY_COUNT(gSaveContext.cycleSceneFlags); i++) {
            gSaveContext.cycleSceneFlags[i].chest = gSaveContext.save.saveInfo.permanentSceneFlags[i].chest;
            gSaveContext.cycleSceneFlags[i].switch0 = gSaveContext.save.saveInfo.permanentSceneFlags[i].switch0;
            gSaveContext.cycleSceneFlags[i].switch1 = gSaveContext.save.saveInfo.permanentSceneFlags[i].switch1;
            gSaveContext.cycleSceneFlags[i].clearedRoom = gSaveContext.save.saveInfo.permanentSceneFlags[i].clearedRoom;
            gSaveContext.cycleSceneFlags[i].collectible = gSaveContext.save.saveInfo.permanentSceneFlags[i].collectible;
        }

        if (gSaveContext.save.saveInfo.scarecrowSpawnSongSet) {
            memcpy(gScarecrowSpawnSongPtr, gSaveContext.save.saveInfo.scarecrowSpawnSong,
                   sizeof(gSaveContext.save.saveInfo.scarecrowSpawnSong));

            for (i = 0; i < ARRAY_COUNT(gSaveContext.save.saveInfo.scarecrowSpawnSong); i++) {}
        }

        fileNum = gSaveContext.fileNum;

        // Remove Owl saves on save continue
        if (GameInteractor_Should(VB_DELETE_OWL_SAVE, true)) {
            func_80147314(sramCtx, fileNum);
        }
    }
}

// Similar to func_80145698, but accounts for owl saves?
void func_8014546C(SramContext* sramCtx) {
    s32 i;

    if (gSaveContext.save.isOwlSave) {
        for (i = 0; i < ARRAY_COUNT(gSaveContext.cycleSceneFlags); i++) {
            gSaveContext.save.saveInfo.permanentSceneFlags[i].chest = gSaveContext.cycleSceneFlags[i].chest;
            gSaveContext.save.saveInfo.permanentSceneFlags[i].switch0 = gSaveContext.cycleSceneFlags[i].switch0;
            gSaveContext.save.saveInfo.permanentSceneFlags[i].switch1 = gSaveContext.cycleSceneFlags[i].switch1;
            gSaveContext.save.saveInfo.permanentSceneFlags[i].clearedRoom = gSaveContext.cycleSceneFlags[i].clearedRoom;
            gSaveContext.save.saveInfo.permanentSceneFlags[i].collectible = gSaveContext.cycleSceneFlags[i].collectible;
        }

        gSaveContext.save.saveInfo.checksum = 0;
        gSaveContext.save.saveInfo.checksum = Sram_CalcChecksum(&gSaveContext, offsetof(SaveContext, fileNum));

        memcpy(sramCtx->saveBuf, &gSaveContext, offsetof(SaveContext, fileNum));
    } else {
        for (i = 0; i < ARRAY_COUNT(gSaveContext.cycleSceneFlags); i++) {
            gSaveContext.save.saveInfo.permanentSceneFlags[i].chest = gSaveContext.cycleSceneFlags[i].chest;
            gSaveContext.save.saveInfo.permanentSceneFlags[i].switch0 = gSaveContext.cycleSceneFlags[i].switch0;
            gSaveContext.save.saveInfo.permanentSceneFlags[i].switch1 = gSaveContext.cycleSceneFlags[i].switch1;
            gSaveContext.save.saveInfo.permanentSceneFlags[i].clearedRoom = gSaveContext.cycleSceneFlags[i].clearedRoom;
            gSaveContext.save.saveInfo.permanentSceneFlags[i].collectible = gSaveContext.cycleSceneFlags[i].collectible;
        }

        gSaveContext.save.saveInfo.checksum = 0;
        gSaveContext.save.saveInfo.checksum = Sram_CalcChecksum(&gSaveContext.save, sizeof(Save));

        if (gSaveContext.flashSaveAvailable) {
            memcpy(sramCtx->saveBuf, &gSaveContext, sizeof(Save));
            memcpy(&sramCtx->saveBuf[SAVE_BUFFER_SIZE_HALF], &gSaveContext.save, sizeof(Save));
        }
    }
}

/**
 * Save permanent scene flags, calculate checksum, copy save context to the save buffer
 */
void func_80145698(SramContext* sramCtx) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(gSaveContext.cycleSceneFlags); i++) {
        gSaveContext.save.saveInfo.permanentSceneFlags[i].chest = gSaveContext.cycleSceneFlags[i].chest;
        gSaveContext.save.saveInfo.permanentSceneFlags[i].switch0 = gSaveContext.cycleSceneFlags[i].switch0;
        gSaveContext.save.saveInfo.permanentSceneFlags[i].switch1 = gSaveContext.cycleSceneFlags[i].switch1;
        gSaveContext.save.saveInfo.permanentSceneFlags[i].clearedRoom = gSaveContext.cycleSceneFlags[i].clearedRoom;
        gSaveContext.save.saveInfo.permanentSceneFlags[i].collectible = gSaveContext.cycleSceneFlags[i].collectible;
    }

    gSaveContext.save.saveInfo.checksum = 0;
    gSaveContext.save.saveInfo.checksum = Sram_CalcChecksum(&gSaveContext.save, sizeof(Save));
    if (gSaveContext.flashSaveAvailable) {
        memcpy(sramCtx->saveBuf, &gSaveContext, sizeof(Save));
        memcpy(&sramCtx->saveBuf[SAVE_BUFFER_SIZE_HALF], &gSaveContext.save, sizeof(Save));
    }
}

// Verifies save and use backup if corrupted?
void func_801457CC(GameState* gameState, SramContext* sramCtx) {
    FileSelectState* fileSelect = (FileSelectState*)gameState;
    u16 sp7A;
    u16 oldCheckSum;
    u16 sp76;
    u16 sp64;
    u16 phi_s2;
    u16 pad;
    u16 sp6E;
    u16 newCheckSum;
    u16 maskCount;

    if (gSaveContext.flashSaveAvailable) {
        D_801F6AF0 = CURRENT_TIME;
        D_801F6AF2 = gSaveContext.flashSaveAvailable;
        sp64 = 0;

        for (sp76 = 0; sp76 < FLASH_SAVE_MAX / 2; sp76++, sp64 += FLASH_SAVE_MAIN_MULTIPLIER) {
            memset(sramCtx->saveBuf, 0, SAVE_BUFFER_SIZE);

            phi_s2 = false;
            sp6E = 0;
            // read main save from flash
            if (SysFlashrom_ReadData(sramCtx->saveBuf, gFlashSaveStartPages[sp64], gFlashSaveNumPages[sp64]) != 0) {
                // main save didn't work
                sp6E = 1;
                // read backup save from flash
                if (SysFlashrom_ReadData(sramCtx->saveBuf, gFlashSaveStartPages[sp64 + FLASH_SAVE_BACKUP_OFFSET],
                                         gFlashSaveNumPages[sp64 + FLASH_SAVE_BACKUP_OFFSET]) != 0) {
                    // backup save didn't work
                    phi_s2 = true;
                }
            }

            if (sp76 < FILE_NUM_MAX) {
                // Non-owl save
                // sp76 = 0: file 1
                // sp76 = 1: file 2

                fileSelect->isOwlSave[sp76] = 0;
                if (phi_s2) {
                    // both main save and backup save failed
                    memset(sramCtx->saveBuf, 0, SAVE_BUFFER_SIZE);
                    memcpy(&gSaveContext, sramCtx->saveBuf, gFlashSaveSizes[sp64]);
                } else {
                    memcpy(&gSaveContext, sramCtx->saveBuf, gFlashSaveSizes[sp64]);

                    // test checksum of main save
                    phi_s2 = gSaveContext.save.saveInfo.checksum;
                    gSaveContext.save.saveInfo.checksum = 0;
                    newCheckSum = Sram_CalcChecksum(&gSaveContext, gFlashSaveSizes[sp64]);
                    gSaveContext.save.saveInfo.checksum = phi_s2;

                    if (CHECK_NEWF(gSaveContext.save.saveInfo.playerData.newf) || (newCheckSum != phi_s2)) {
                        // checksum didnt match, try backup save
                        sp6E = 1;

                        if ((gSaveContext.save.saveInfo.playerData.newf[0] == 'Z') &&
                            (gSaveContext.save.saveInfo.playerData.newf[1] == 'E') &&
                            (gSaveContext.save.saveInfo.playerData.newf[2] == 'L') &&
                            (gSaveContext.save.saveInfo.playerData.newf[3] == 'D') &&
                            (gSaveContext.save.saveInfo.playerData.newf[4] == 'A')) {}

                        // read backup save from flash
                        phi_s2 = false;
                        if (SysFlashrom_ReadData(sramCtx->saveBuf,
                                                 gFlashSaveStartPages[sp64 + FLASH_SAVE_BACKUP_OFFSET],
                                                 gFlashSaveNumPages[sp64 + FLASH_SAVE_BACKUP_OFFSET]) != 0) {
                            // backup save didn't work
                            phi_s2 = true;
                        }

                        memcpy(&gSaveContext, sramCtx->saveBuf, gFlashSaveSizes[sp64]);

                        // test checksum of backup save
                        oldCheckSum = gSaveContext.save.saveInfo.checksum;
                        gSaveContext.save.saveInfo.checksum = 0;

                        if (phi_s2 || CHECK_NEWF(gSaveContext.save.saveInfo.playerData.newf) ||
                            (oldCheckSum != Sram_CalcChecksum(&gSaveContext, gFlashSaveSizes[sp64]))) {
                            // backup save didn't work
                            memset(sramCtx->saveBuf, 0, SAVE_BUFFER_SIZE);
                            memcpy(&gSaveContext.save, sramCtx->saveBuf, sizeof(Save));
                            sp6E = 999;
                        }
                    }
                }

                gSaveContext.save.saveInfo.checksum = 0;
                //! FAKE: [sp64 + 0]?
                gSaveContext.save.saveInfo.checksum = Sram_CalcChecksum(&gSaveContext, gFlashSaveSizes[sp64 + 0]);

                for (sp7A = 0; sp7A < ARRAY_COUNT(gSaveContext.save.saveInfo.playerData.newf); sp7A++) {
                    fileSelect->newf[sp76][sp7A] = gSaveContext.save.saveInfo.playerData.newf[sp7A];
                }

                if (!CHECK_NEWF(fileSelect->newf[sp76])) {
                    fileSelect->threeDayResetCount[sp76] = gSaveContext.save.saveInfo.playerData.threeDayResetCount;

                    for (sp7A = 0; sp7A < ARRAY_COUNT(gSaveContext.save.saveInfo.playerData.playerName); sp7A++) {
                        fileSelect->fileNames[sp76][sp7A] = gSaveContext.save.saveInfo.playerData.playerName[sp7A];
                    }

                    fileSelect->healthCapacity[sp76] = gSaveContext.save.saveInfo.playerData.healthCapacity;
                    fileSelect->health[sp76] = gSaveContext.save.saveInfo.playerData.health;
                    fileSelect->defenseHearts[sp76] = gSaveContext.save.saveInfo.inventory.defenseHearts;
                    fileSelect->questItems[sp76] = gSaveContext.save.saveInfo.inventory.questItems;
                    fileSelect->time[sp76] = CURRENT_TIME;
                    fileSelect->day[sp76] = gSaveContext.save.day;
                    fileSelect->isOwlSave[sp76] = gSaveContext.save.isOwlSave;
                    fileSelect->rupees[sp76] = gSaveContext.save.saveInfo.playerData.rupees;
                    fileSelect->walletUpgrades[sp76] = CUR_UPG_VALUE(UPG_WALLET);

                    for (sp7A = 0, maskCount = 0; sp7A < MASK_NUM_SLOTS; sp7A++) {
                        if (gSaveContext.save.saveInfo.inventory.items[sp7A + ITEM_NUM_SLOTS] == ITEM_NONE) {
                            continue;
                        }
                        maskCount++;
                    }
                    fileSelect->maskCount[sp76] = maskCount;
                    fileSelect->heartPieceCount[sp76] = GET_QUEST_HEART_PIECE_COUNT;

                    GameInteractor_ExecuteOnFileSelectSaveLoad(sp76, false, &gSaveContext);
                }

                if (sp6E == 1) {
                    // backup save
                    memcpy(&sramCtx->saveBuf[SAVE_BUFFER_SIZE_HALF], &gSaveContext.save, sizeof(Save));
                    Sram_SyncWriteToFlash(sramCtx, gFlashSaveStartPages[sp64], gFlashSpecialSaveNumPages[sp64]);
                } else if (!sp6E) {
                    // main save
                    oldCheckSum = gSaveContext.save.saveInfo.checksum;

                    if (SysFlashrom_ReadData(sramCtx->saveBuf, gFlashSaveStartPages[sp64 + FLASH_SAVE_BACKUP_OFFSET],
                                             gFlashSaveNumPages[sp64 + FLASH_SAVE_BACKUP_OFFSET]) != 0) {
                        phi_s2 = 1;
                    } else {
                        memcpy(&gSaveContext.save, sramCtx->saveBuf, sizeof(Save));
                        phi_s2 = gSaveContext.save.saveInfo.checksum;
                        gSaveContext.save.saveInfo.checksum = 0;
                        sp7A = Sram_CalcChecksum(&gSaveContext.save, sizeof(Save));
                    }

                    if (CHECK_NEWF(gSaveContext.save.saveInfo.playerData.newf) || (sp7A != phi_s2) ||
                        (oldCheckSum != phi_s2)) {
                        SysFlashrom_ReadData(sramCtx->saveBuf, gFlashSaveStartPages[sp64], gFlashSaveNumPages[sp64]);
                        memcpy(&gSaveContext.save, sramCtx->saveBuf, sizeof(Save));
                        memcpy(&sramCtx->saveBuf[SAVE_BUFFER_SIZE_HALF], &gSaveContext.save, sizeof(Save));
                        Sram_SyncWriteToFlash(sramCtx, gFlashSaveStartPages[sp64], gFlashSpecialSaveNumPages[sp64]);
                    }
                }
            } else if (sp76 < FILE_NUM_MAX_WITH_OWL_SAVE) {
                // Owl Save:
                // sp76 = 2: file 1
                // sp76 = 3: file 2
                fileSelect->isOwlSave[sp76] = 0;

                if (!CHECK_NEWF(fileSelect->newf[sp76 - FILE_NUM_OWL_SAVE_OFFSET])) {
                    if (phi_s2) {
                        // both main save and backup save failed
                        memset(sramCtx->saveBuf, 0, SAVE_BUFFER_SIZE);
                        memcpy(&gSaveContext, sramCtx->saveBuf, gFlashSaveSizes[sp64]);
                    } else {
                        memcpy(&gSaveContext, sramCtx->saveBuf, gFlashSaveSizes[sp64]);
                        phi_s2 = gSaveContext.save.saveInfo.checksum;

                        // test checksum of main save
                        gSaveContext.save.saveInfo.checksum = 0;
                        newCheckSum = Sram_CalcChecksum(&gSaveContext, gFlashSaveSizes[sp64]);
                        gSaveContext.save.saveInfo.checksum = phi_s2;

                        if (CHECK_NEWF(gSaveContext.save.saveInfo.playerData.newf) || (newCheckSum != phi_s2)) {
                            // checksum didnt match, try backup save
                            sp6E = 1;
                            if ((gSaveContext.save.saveInfo.playerData.newf[0] == 'Z') &&
                                (gSaveContext.save.saveInfo.playerData.newf[1] == 'E') &&
                                (gSaveContext.save.saveInfo.playerData.newf[2] == 'L') &&
                                (gSaveContext.save.saveInfo.playerData.newf[3] == 'D') &&
                                (gSaveContext.save.saveInfo.playerData.newf[4] == 'A')) {}

                            phi_s2 = false;
                            // read backup save from flash
                            if (SysFlashrom_ReadData(sramCtx->saveBuf,
                                                     gFlashSaveStartPages[sp64 + FLASH_SAVE_BACKUP_OFFSET],
                                                     gFlashSaveNumPages[sp64 + FLASH_SAVE_BACKUP_OFFSET]) != 0) {
                                // backup save didn't work
                                phi_s2 = true;
                            }

                            memcpy(&gSaveContext, sramCtx->saveBuf, gFlashSaveSizes[sp64]);

                            // test checksum of backup save
                            oldCheckSum = gSaveContext.save.saveInfo.checksum;
                            gSaveContext.save.saveInfo.checksum = 0;

                            if (phi_s2 || CHECK_NEWF(gSaveContext.save.saveInfo.playerData.newf) ||
                                (oldCheckSum != Sram_CalcChecksum(&gSaveContext, gFlashSaveSizes[sp64]))) {
                                memset(sramCtx->saveBuf, 0, SAVE_BUFFER_SIZE);
                                memcpy(&gSaveContext, sramCtx->saveBuf, gFlashSaveSizes[sp64]);
                                sp6E = 999;
                            }
                        }
                    }

                    gSaveContext.save.saveInfo.checksum = 0;
                    //! FAKE: [sp64 + 0]?
                    gSaveContext.save.saveInfo.checksum = Sram_CalcChecksum(&gSaveContext, gFlashSaveSizes[sp64 + 0]);

                    for (sp7A = 0; sp7A < ARRAY_COUNT(gSaveContext.save.saveInfo.playerData.newf); sp7A++) {
                        fileSelect->newf[sp76][sp7A] = gSaveContext.save.saveInfo.playerData.newf[sp7A];
                    }

                    if (!CHECK_NEWF(fileSelect->newf[sp76])) {
                        fileSelect->threeDayResetCount[sp76] = gSaveContext.save.saveInfo.playerData.threeDayResetCount;

                        for (sp7A = 0; sp7A < ARRAY_COUNT(gSaveContext.save.saveInfo.playerData.playerName); sp7A++) {
                            fileSelect->fileNames[sp76][sp7A] =
                                (u32)gSaveContext.save.saveInfo.playerData.playerName[sp7A];
                        }

                        fileSelect->healthCapacity[sp76] = gSaveContext.save.saveInfo.playerData.healthCapacity;
                        fileSelect->health[sp76] = gSaveContext.save.saveInfo.playerData.health;
                        fileSelect->defenseHearts[sp76] = gSaveContext.save.saveInfo.inventory.defenseHearts;
                        fileSelect->questItems[sp76] = gSaveContext.save.saveInfo.inventory.questItems;
                        fileSelect->time[sp76] = CURRENT_TIME;
                        fileSelect->day[sp76] = gSaveContext.save.day;
                        fileSelect->isOwlSave[sp76] = gSaveContext.save.isOwlSave;
                        fileSelect->rupees[sp76] = gSaveContext.save.saveInfo.playerData.rupees;
                        fileSelect->walletUpgrades[sp76] = CUR_UPG_VALUE(UPG_WALLET);

                        for (sp7A = 0, maskCount = 0; sp7A < MASK_NUM_SLOTS; sp7A++) {
                            if (gSaveContext.save.saveInfo.inventory.items[sp7A + ITEM_NUM_SLOTS] == ITEM_NONE) {
                                continue;
                            }
                            maskCount++;
                        }
                        fileSelect->maskCount[sp76] = maskCount;
                        fileSelect->heartPieceCount[sp76] = GET_QUEST_HEART_PIECE_COUNT;

                        GameInteractor_ExecuteOnFileSelectSaveLoad(sp76 - FILE_NUM_OWL_SAVE_OFFSET, true,
                                                                   &gSaveContext);
                    }

                    if (sp6E == 1) {
                        // backup save
                        Sram_SyncWriteToFlash(sramCtx, gFlashSaveStartPages[sp64], gFlashSaveNumPages[sp64]);
                        Sram_SyncWriteToFlash(sramCtx, gFlashSaveStartPages[sp64 + FLASH_SAVE_BACKUP_OFFSET],
                                              gFlashSaveNumPages[sp64 + FLASH_SAVE_BACKUP_OFFSET]);
                    } else if (!sp6E) {
                        // main save
                        oldCheckSum = gSaveContext.save.saveInfo.checksum;

                        if (SysFlashrom_ReadData(sramCtx->saveBuf,
                                                 gFlashSaveStartPages[sp64 + FLASH_SAVE_BACKUP_OFFSET],
                                                 gFlashSaveNumPages[sp64 + FLASH_SAVE_BACKUP_OFFSET]) != 0) {
                            phi_s2 = 1;
                        } else {
                            memcpy(&gSaveContext, sramCtx->saveBuf, gFlashSaveSizes[sp64]);
                            phi_s2 = gSaveContext.save.saveInfo.checksum;
                            gSaveContext.save.saveInfo.checksum = 0;
                            sp7A = Sram_CalcChecksum(&gSaveContext, gFlashSaveSizes[sp64]);
                        }

                        if (CHECK_NEWF(gSaveContext.save.saveInfo.playerData.newf) || (sp7A != phi_s2) ||
                            (oldCheckSum != phi_s2)) {
                            SysFlashrom_ReadData(sramCtx->saveBuf, gFlashSaveStartPages[sp64],
                                                 gFlashSaveNumPages[sp64]);
                            memcpy(&gSaveContext, sramCtx->saveBuf, gFlashSaveSizes[sp64]);
                            Sram_SyncWriteToFlash(sramCtx, gFlashSaveStartPages[sp64], gFlashSaveNumPages[sp64]);
                            Sram_SyncWriteToFlash(sramCtx, gFlashSaveStartPages[sp64 + FLASH_SAVE_BACKUP_OFFSET],
                                                  gFlashSaveNumPages[sp64 + FLASH_SAVE_BACKUP_OFFSET]);
                        }
                    }
                } else {
                    memset(sramCtx->saveBuf, 0, SAVE_BUFFER_SIZE);
                    memcpy(&gSaveContext, sramCtx->saveBuf, gFlashSaveSizes[sp64]);
                    Sram_SyncWriteToFlash(sramCtx, gFlashSaveStartPages[sp64], gFlashSaveNumPages[sp64]);
                    Sram_SyncWriteToFlash(sramCtx, gFlashSaveStartPages[sp64 + FLASH_SAVE_BACKUP_OFFSET],
                                          gFlashSaveNumPages[sp64 + FLASH_SAVE_BACKUP_OFFSET]);
                }
            } else {
                if (phi_s2) {
                    gSaveContext.options.optionId = 0xA51D;
                    gSaveContext.options.language = LANGUAGE_ENG;
                    gSaveContext.options.audioSetting = SAVE_AUDIO_STEREO;
                    gSaveContext.options.languageSetting = 0;
                    gSaveContext.options.zTargetSetting = 0;
                } else {
                    memcpy(&gSaveContext.options, sramCtx->saveBuf, sizeof(SaveOptions));
                    if (gSaveContext.options.optionId != 0xA51D) {
                        gSaveContext.options.optionId = 0xA51D;
                        gSaveContext.options.language = LANGUAGE_ENG;
                        gSaveContext.options.audioSetting = SAVE_AUDIO_STEREO;
                        gSaveContext.options.languageSetting = 0;
                        gSaveContext.options.zTargetSetting = 0;
                    }
                }
                Audio_SetFileSelectSettings(gSaveContext.options.audioSetting);
            }
        }

        gSaveContext.save.time = D_801F6AF0;
        gSaveContext.flashSaveAvailable = D_801F6AF2;
    }

    gSaveContext.options.language = LANGUAGE_ENG;
}

void Sram_EraseSave(FileSelectState* fileSelect2, SramContext* sramCtx, s32 fileNum) {
    FileSelectState* fileSelect = fileSelect2;
    s32 pad;

    if (gSaveContext.flashSaveAvailable) {
        if (fileSelect->isOwlSave[fileNum + FILE_NUM_OWL_SAVE_OFFSET]) {
            func_80147314(sramCtx, fileNum);
            fileSelect->isOwlSave[fileNum + FILE_NUM_OWL_SAVE_OFFSET] = false;
        }
        memset(sramCtx->saveBuf, 0, SAVE_BUFFER_SIZE);
        memcpy(&gSaveContext, sramCtx->saveBuf, sizeof(Save));
    }

    gSaveContext.save.time = D_801F6AF0;
    gSaveContext.flashSaveAvailable = D_801F6AF2;
}

void Sram_CopySave(FileSelectState* fileSelect2, SramContext* sramCtx) {
    FileSelectState* fileSelect = fileSelect2;
    u16 i;
    s16 maskCount;

    if (gSaveContext.flashSaveAvailable) {
        if (fileSelect->isOwlSave[fileSelect->selectedFileIndex + FILE_NUM_OWL_SAVE_OFFSET]) {
            func_80147414(sramCtx, fileSelect->selectedFileIndex, fileSelect->copyDestFileIndex);
            fileSelect->threeDayResetCount[fileSelect->copyDestFileIndex + FILE_NUM_OWL_SAVE_OFFSET] =
                gSaveContext.save.saveInfo.playerData.threeDayResetCount;

            for (i = 0; i < ARRAY_COUNT(gSaveContext.save.saveInfo.playerData.playerName); i++) {
                fileSelect->fileNames[fileSelect->copyDestFileIndex + FILE_NUM_OWL_SAVE_OFFSET][i] =
                    gSaveContext.save.saveInfo.playerData.playerName[i];
            }

            fileSelect->healthCapacity[fileSelect->copyDestFileIndex + FILE_NUM_OWL_SAVE_OFFSET] =
                gSaveContext.save.saveInfo.playerData.healthCapacity;
            fileSelect->health[fileSelect->copyDestFileIndex + FILE_NUM_OWL_SAVE_OFFSET] =
                gSaveContext.save.saveInfo.playerData.health;
            fileSelect->defenseHearts[fileSelect->copyDestFileIndex + FILE_NUM_OWL_SAVE_OFFSET] =
                gSaveContext.save.saveInfo.inventory.defenseHearts;
            fileSelect->questItems[fileSelect->copyDestFileIndex + FILE_NUM_OWL_SAVE_OFFSET] =
                gSaveContext.save.saveInfo.inventory.questItems;
            fileSelect->time[fileSelect->copyDestFileIndex + FILE_NUM_OWL_SAVE_OFFSET] = CURRENT_TIME;
            fileSelect->day[fileSelect->copyDestFileIndex + FILE_NUM_OWL_SAVE_OFFSET] = gSaveContext.save.day;
            fileSelect->isOwlSave[fileSelect->copyDestFileIndex + FILE_NUM_OWL_SAVE_OFFSET] =
                gSaveContext.save.isOwlSave;
            fileSelect->rupees[fileSelect->copyDestFileIndex + FILE_NUM_OWL_SAVE_OFFSET] =
                gSaveContext.save.saveInfo.playerData.rupees;
            fileSelect->walletUpgrades[fileSelect->copyDestFileIndex + FILE_NUM_OWL_SAVE_OFFSET] =
                CUR_UPG_VALUE(UPG_WALLET);

            for (i = 0, maskCount = 0; i < MASK_NUM_SLOTS; i++) {
                if (gSaveContext.save.saveInfo.inventory.items[i + ITEM_NUM_SLOTS] != ITEM_NONE) {
                    maskCount++;
                }
            }

            fileSelect->maskCount[fileSelect->copyDestFileIndex + FILE_NUM_OWL_SAVE_OFFSET] = maskCount;
            fileSelect->heartPieceCount[fileSelect->copyDestFileIndex + FILE_NUM_OWL_SAVE_OFFSET] =
                GET_QUEST_HEART_PIECE_COUNT;

            GameInteractor_ExecuteOnFileSelectSaveLoad(fileSelect->copyDestFileIndex, true, &gSaveContext);
        }

        // clear buffer
        memset(sramCtx->saveBuf, 0, SAVE_BUFFER_SIZE);
        // read to buffer
        if (SysFlashrom_ReadData(&sramCtx->saveBuf[0],
                                 gFlashSaveStartPages[fileSelect->selectedFileIndex * FLASH_SAVE_MAIN_MULTIPLIER],
                                 gFlashSaveNumPages[fileSelect->selectedFileIndex * FLASH_SAVE_MAIN_MULTIPLIER])) {}

        if (SysFlashrom_ReadData(&sramCtx->saveBuf[SAVE_BUFFER_SIZE_HALF],
                                 gFlashSaveStartPages[fileSelect->selectedFileIndex * FLASH_SAVE_MAIN_MULTIPLIER +
                                                      FLASH_SAVE_BACKUP_OFFSET],
                                 gFlashSaveNumPages[fileSelect->selectedFileIndex * FLASH_SAVE_MAIN_MULTIPLIER +
                                                    FLASH_SAVE_BACKUP_OFFSET])) {}

        // copy buffer to save context
        memcpy(&gSaveContext.save, sramCtx->saveBuf, sizeof(Save));

        fileSelect->threeDayResetCount[fileSelect->copyDestFileIndex] =
            gSaveContext.save.saveInfo.playerData.threeDayResetCount;

        for (i = 0; i < ARRAY_COUNT(gSaveContext.save.saveInfo.playerData.playerName); i++) {
            fileSelect->fileNames[fileSelect->copyDestFileIndex][i] =
                gSaveContext.save.saveInfo.playerData.playerName[i];
        }

        fileSelect->healthCapacity[fileSelect->copyDestFileIndex] =
            gSaveContext.save.saveInfo.playerData.healthCapacity;
        fileSelect->health[fileSelect->copyDestFileIndex] = gSaveContext.save.saveInfo.playerData.health;
        fileSelect->defenseHearts[fileSelect->copyDestFileIndex] = gSaveContext.save.saveInfo.inventory.defenseHearts;
        fileSelect->questItems[fileSelect->copyDestFileIndex] = gSaveContext.save.saveInfo.inventory.questItems;
        fileSelect->time[fileSelect->copyDestFileIndex] = CURRENT_TIME;
        fileSelect->day[fileSelect->copyDestFileIndex] = gSaveContext.save.day;
        fileSelect->isOwlSave[fileSelect->copyDestFileIndex] = gSaveContext.save.isOwlSave;
        fileSelect->rupees[fileSelect->copyDestFileIndex] = gSaveContext.save.saveInfo.playerData.rupees;
        fileSelect->walletUpgrades[fileSelect->copyDestFileIndex] = CUR_UPG_VALUE(UPG_WALLET);

        for (i = 0, maskCount = 0; i < MASK_NUM_SLOTS; i++) {
            if (gSaveContext.save.saveInfo.inventory.items[i + ITEM_NUM_SLOTS] != ITEM_NONE) {
                maskCount++;
            }
        }

        fileSelect->maskCount[fileSelect->copyDestFileIndex] = maskCount;
        fileSelect->heartPieceCount[fileSelect->copyDestFileIndex] = GET_QUEST_HEART_PIECE_COUNT;

        GameInteractor_ExecuteOnFileSelectSaveLoad(fileSelect->copyDestFileIndex, false, &gSaveContext);
    }

    gSaveContext.save.time = D_801F6AF0;
    gSaveContext.flashSaveAvailable = D_801F6AF2;
}

void Sram_InitSave(FileSelectState* fileSelect2, SramContext* sramCtx) {
    s32 phi_v0;
    u16 i;
    FileSelectState* fileSelect = fileSelect2;
    s16 maskCount;

    if (gSaveContext.flashSaveAvailable) {
        Sram_InitNewSave();
        if (fileSelect->buttonIndex == 0) {
            gSaveContext.save.cutsceneIndex = 0xFFF0;
        }

        for (phi_v0 = 0; phi_v0 < ARRAY_COUNT(gSaveContext.save.saveInfo.playerData.playerName); phi_v0++) {
            gSaveContext.save.saveInfo.playerData.playerName[phi_v0] =
                fileSelect->fileNames[fileSelect->buttonIndex][phi_v0];
        }

        gSaveContext.save.saveInfo.playerData.newf[0] = 'Z';
        gSaveContext.save.saveInfo.playerData.newf[1] = 'E';
        gSaveContext.save.saveInfo.playerData.newf[2] = 'L';
        gSaveContext.save.saveInfo.playerData.newf[3] = 'D';
        gSaveContext.save.saveInfo.playerData.newf[4] = 'A';
        gSaveContext.save.saveInfo.playerData.newf[5] = '3';

        gSaveContext.save.saveInfo.checksum = Sram_CalcChecksum(&gSaveContext.save, sizeof(Save));

        GameInteractor_ExecuteOnSaveInit(fileSelect->buttonIndex);

        memcpy(sramCtx->saveBuf, &gSaveContext.save, sizeof(Save));
        memcpy(&sramCtx->saveBuf[SAVE_BUFFER_SIZE_HALF], &gSaveContext.save, sizeof(Save));

        for (i = 0; i < ARRAY_COUNT(gSaveContext.save.saveInfo.playerData.newf); i++) {
            fileSelect->newf[fileSelect->buttonIndex][i] = gSaveContext.save.saveInfo.playerData.newf[i];
        }

        fileSelect->threeDayResetCount[fileSelect->buttonIndex] =
            gSaveContext.save.saveInfo.playerData.threeDayResetCount;

        for (i = 0; i < ARRAY_COUNT(gSaveContext.save.saveInfo.playerData.playerName); i++) {
            fileSelect->fileNames[fileSelect->buttonIndex][i] = gSaveContext.save.saveInfo.playerData.playerName[i];
        }

        fileSelect->healthCapacity[fileSelect->buttonIndex] = gSaveContext.save.saveInfo.playerData.healthCapacity;
        fileSelect->health[fileSelect->buttonIndex] = gSaveContext.save.saveInfo.playerData.health;
        fileSelect->defenseHearts[fileSelect->buttonIndex] = gSaveContext.save.saveInfo.inventory.defenseHearts;
        fileSelect->questItems[fileSelect->buttonIndex] = gSaveContext.save.saveInfo.inventory.questItems;
        fileSelect->time[fileSelect->buttonIndex] = CURRENT_TIME;
        fileSelect->day[fileSelect->buttonIndex] = gSaveContext.save.day;
        fileSelect->isOwlSave[fileSelect->buttonIndex] = gSaveContext.save.isOwlSave;
        fileSelect->rupees[fileSelect->buttonIndex] = gSaveContext.save.saveInfo.playerData.rupees;
        fileSelect->walletUpgrades[fileSelect->buttonIndex] = CUR_UPG_VALUE(UPG_WALLET);

        for (i = 0, maskCount = 0; i < MASK_NUM_SLOTS; i++) {
            if (gSaveContext.save.saveInfo.inventory.items[i + ITEM_NUM_SLOTS] != ITEM_NONE) {
                maskCount++;
            }
        }

        fileSelect->maskCount[fileSelect->buttonIndex] = maskCount;
        fileSelect->heartPieceCount[fileSelect->buttonIndex] = GET_QUEST_HEART_PIECE_COUNT;

        GameInteractor_ExecuteOnFileSelectSaveLoad(fileSelect->buttonIndex, false, &gSaveContext);
    }

    gSaveContext.save.time = D_801F6AF0;
    gSaveContext.flashSaveAvailable = D_801F6AF2;
}

/**
 *  Write the SaveOptions of SaveContext to the save buffer
 */
void Sram_WriteSaveOptionsToBuffer(SramContext* sramCtx) {
    if (gSaveContext.flashSaveAvailable) {
        gSaveContext.options.language = LANGUAGE_ENG;
        memcpy(sramCtx->saveBuf, &gSaveContext.options, sizeof(SaveOptions));
    }
}

// #region 2S2H [Port] Surround Sound
/**
 * Load global options (SaveOptions) from flash early during initialization.
 * This ensures audio settings are applied before the intro cutscene plays.
 * Note: Does not check flashSaveAvailable since SysFlashrom_ReadData is redirected
 * to SaveManager which reads from JSON files and works independently.
 */
void Sram_LoadGlobalOptions(void) {
    // Use a local buffer since sramCtx->saveBuf isn't allocated yet
    u8 buffer[sizeof(SaveOptions)];
    s32 readFailed = SysFlashrom_ReadData(buffer, gFlashSaveStartPages[FLASH_SAVE_SRAM_HEADER],
                                          gFlashSaveNumPages[FLASH_SAVE_SRAM_HEADER]);

    if (readFailed) {
        // Try backup
        readFailed = SysFlashrom_ReadData(buffer, gFlashSaveStartPages[FLASH_SAVE_SRAM_HEADER_BACKUP],
                                          gFlashSaveNumPages[FLASH_SAVE_SRAM_HEADER_BACKUP]);
    }

    if (readFailed) {
        // No valid save, use defaults (already set by SaveContext_Init)
        return;
    }

    memcpy(&gSaveContext.options, buffer, sizeof(SaveOptions));
    if (gSaveContext.options.optionId != 0xA51D) {
        // Invalid options, keep defaults (already set by SaveContext_Init)
        return;
    }

    Audio_SetFileSelectSettings(gSaveContext.options.audioSetting);
}
// #endregion

void Sram_InitSram(GameState* gameState, SramContext* sramCtx) {
    if (gSaveContext.save.entrance) {} // Required to match

    Audio_SetFileSelectSettings(gSaveContext.options.audioSetting);
}

void Sram_Alloc(GameState* gameState, SramContext* sramCtx) {
    if (gSaveContext.flashSaveAvailable) {
        sramCtx->saveBuf = THA_AllocTailAlign16(&gameState->tha, SAVE_BUFFER_SIZE);
        sramCtx->status = 0;
    }
}

/**
 * Synchronous flash write
 */
void Sram_SyncWriteToFlash(SramContext* sramCtx, s32 curPage, s32 numPages) {
    sramCtx->curPage = curPage;
    sramCtx->numPages = numPages;
    SysFlashrom_WriteDataSync(sramCtx->saveBuf, curPage, numPages);
}

/**
 * Saves the game on the very first time Player enters South Clock Town from the Clock Tower
 */
void Sram_SaveSpecialEnterClockTown(PlayState* play) {
    s32 pad[2];
    SramContext* sramCtx = &play->sramCtx;

    gSaveContext.save.isFirstCycle = true;
    gSaveContext.save.isOwlSave = false;
    // 2S2H [Enhancement] Store playtime before saving
    SavingEnhancements_AdvancePlaytime();
    func_80145698(sramCtx);
    SysFlashrom_WriteDataSync(sramCtx->saveBuf, gFlashSaveStartPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER],
                              gFlashSpecialSaveNumPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER]);
}

/**
 * Saves when beating the game, after showing the "Dawn of the New Day" message
 */
void Sram_SaveSpecialNewDay(PlayState* play) {
    s32 cutsceneIndex = gSaveContext.save.cutsceneIndex;
    s32 day;
    u16 time = CURRENT_TIME;

    day = gSaveContext.save.day;

    CLEAR_WEEKEVENTREG(WEEKEVENTREG_84_20);

    Sram_SaveEndOfCycle(play);
    func_8014546C(&play->sramCtx);

    gSaveContext.save.day = day;
    gSaveContext.save.time = time;
    gSaveContext.save.cutsceneIndex = cutsceneIndex;
    SysFlashrom_WriteDataSync(play->sramCtx.saveBuf,
                              gFlashSaveStartPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER],
                              gFlashSaveNumPages[gSaveContext.fileNum * FLASH_SAVE_MAIN_MULTIPLIER]);
}

void Sram_SetFlashPagesDefault(SramContext* sramCtx, u32 curPage, u32 numPages) {
    sramCtx->curPage = curPage;
    sramCtx->numPages = numPages;
    sramCtx->status = 1;
}

void Sram_StartWriteToFlashDefault(SramContext* sramCtx) {
    // async flash write
    SysFlashrom_WriteDataAsync(sramCtx->saveBuf, sramCtx->curPage, sramCtx->numPages);

    sramCtx->startWriteOsTime = osGetTime();
    sramCtx->status = 2;
}

void Sram_UpdateWriteToFlashDefault(SramContext* sramCtx) {
    if (sramCtx->status == 2) {
        if (SysFlashrom_IsBusy() != 0) {          // if task running
            if (SysFlashrom_AwaitResult() == 0) { // wait for task done
                // task success
                sramCtx->status = 4;
            } else {
                // task failure
                sramCtx->status = 4;
            }
        }
    } else if (GameInteractor_Should(VB_SAVE_DELAY,
                                     OSTIME_TO_TIMER(osGetTime() - sramCtx->startWriteOsTime) >= SECONDS_TO_TIMER(2))) {
        sramCtx->status = 0;
    }
}

void Sram_SetFlashPagesOwlSave(SramContext* sramCtx, s32 curPage, s32 numPages) {
    sramCtx->curPage = curPage;
    sramCtx->numPages = numPages;
    sramCtx->status = 6;
}

void Sram_StartWriteToFlashOwlSave(SramContext* sramCtx) {
    SysFlashrom_WriteDataAsync(sramCtx->saveBuf, sramCtx->curPage, sramCtx->numPages);

    sramCtx->startWriteOsTime = osGetTime();
    sramCtx->status = 7;
}

void Sram_UpdateWriteToFlashOwlSave(SramContext* sramCtx) {
    if (sramCtx->status == 7) {
        if (SysFlashrom_IsBusy() != 0) {          // Is task running
            if (SysFlashrom_AwaitResult() == 0) { // Wait for task done
                SysFlashrom_WriteDataAsync(sramCtx->saveBuf, sramCtx->curPage + 0x80, sramCtx->numPages);
                sramCtx->status = 8;
            } else {
                SysFlashrom_WriteDataAsync(sramCtx->saveBuf, sramCtx->curPage + 0x80, sramCtx->numPages);
                sramCtx->status = 8;
            }
        }
    } else if (sramCtx->status == 8) {
        if (SysFlashrom_IsBusy() != 0) {          // Is task running
            if (SysFlashrom_AwaitResult() == 0) { // Wait for task done
                sramCtx->status = 4;
            } else {
                sramCtx->status = 4;
            }
        }
    } else if (GameInteractor_Should(VB_SAVE_DELAY,
                                     OSTIME_TO_TIMER(osGetTime() - sramCtx->startWriteOsTime) >= SECONDS_TO_TIMER(2))) {
        sramCtx->status = 0;
        memset(sramCtx->saveBuf, 0, SAVE_BUFFER_SIZE);
        gSaveContext.save.isOwlSave = false;
        gSaveContext.save.saveInfo.checksum = 0;
        // flash read to buffer then copy to save context
        SysFlashrom_ReadData(sramCtx->saveBuf, sramCtx->curPage, sramCtx->numPages);
        memcpy(&gSaveContext, sramCtx->saveBuf, offsetof(SaveContext, fileNum));
    }
}

void func_80147314(SramContext* sramCtx, s32 fileNum) {
    s32 pad;

    gSaveContext.save.isOwlSave = false;

    gSaveContext.save.saveInfo.playerData.newf[0] = '\0';
    gSaveContext.save.saveInfo.playerData.newf[1] = '\0';
    gSaveContext.save.saveInfo.playerData.newf[2] = '\0';
    gSaveContext.save.saveInfo.playerData.newf[3] = '\0';
    gSaveContext.save.saveInfo.playerData.newf[4] = '\0';
    gSaveContext.save.saveInfo.playerData.newf[5] = '\0';

    gSaveContext.save.saveInfo.checksum = 0;
    gSaveContext.save.saveInfo.checksum = Sram_CalcChecksum(&gSaveContext, offsetof(SaveContext, fileNum));

    memcpy(sramCtx->saveBuf, &gSaveContext, offsetof(SaveContext, fileNum));
    Sram_SyncWriteToFlash(sramCtx, gFlashOwlSaveStartPages[fileNum * FLASH_SAVE_MAIN_MULTIPLIER],
                          gFlashOwlSaveNumPages[fileNum * FLASH_SAVE_MAIN_MULTIPLIER]);
    //! Note: should be `gFlashOwlSaveNumPages[fileNum * FLASH_SAVE_MAIN_MULTIPLIER + FLASH_SAVE_BACKUP_OFFSET]`?
    Sram_SyncWriteToFlash(sramCtx,
                          gFlashOwlSaveStartPages[fileNum * FLASH_SAVE_MAIN_MULTIPLIER + FLASH_SAVE_BACKUP_OFFSET],
                          gFlashOwlSaveNumPages[fileNum * FLASH_SAVE_MAIN_MULTIPLIER]);

    gSaveContext.save.isOwlSave = true;

    gSaveContext.save.saveInfo.playerData.newf[0] = 'Z';
    gSaveContext.save.saveInfo.playerData.newf[1] = 'E';
    gSaveContext.save.saveInfo.playerData.newf[2] = 'L';
    gSaveContext.save.saveInfo.playerData.newf[3] = 'D';
    gSaveContext.save.saveInfo.playerData.newf[4] = 'A';
    gSaveContext.save.saveInfo.playerData.newf[5] = '3';
}

// Used by `Sram_CopySave` with `isOwlSave` set
void func_80147414(SramContext* sramCtx, s32 fileNum, s32 arg2) {
    s32 pad;

    // Clear save buffer
    memset(sramCtx->saveBuf, 0, SAVE_BUFFER_SIZE);

    // Read save file
    if (SysFlashrom_ReadData(sramCtx->saveBuf, gFlashOwlSaveStartPages[fileNum * FLASH_SAVE_MAIN_MULTIPLIER],
                             gFlashOwlSaveNumPages[fileNum * FLASH_SAVE_MAIN_MULTIPLIER]) != 0) {
        // If failed, read backup save file
        SysFlashrom_ReadData(sramCtx->saveBuf,
                             gFlashOwlSaveStartPages[fileNum * FLASH_SAVE_MAIN_MULTIPLIER + FLASH_SAVE_BACKUP_OFFSET],
                             gFlashOwlSaveNumPages[fileNum * FLASH_SAVE_MAIN_MULTIPLIER + FLASH_SAVE_BACKUP_OFFSET]);
    }

    // Copy buffer to save context
    memcpy(&gSaveContext, sramCtx->saveBuf, offsetof(SaveContext, fileNum));

    Sram_SyncWriteToFlash(sramCtx, gFlashOwlSaveStartPages[arg2 * FLASH_SAVE_MAIN_MULTIPLIER],
                          gFlashOwlSaveNumPages[arg2 * FLASH_SAVE_MAIN_MULTIPLIER]);
    Sram_SyncWriteToFlash(sramCtx,
                          gFlashOwlSaveStartPages[arg2 * FLASH_SAVE_MAIN_MULTIPLIER + FLASH_SAVE_BACKUP_OFFSET],
                          gFlashOwlSaveNumPages[arg2 * FLASH_SAVE_MAIN_MULTIPLIER]);
}

void Sram_nop8014750C(UNK_TYPE4 arg0) {
}
