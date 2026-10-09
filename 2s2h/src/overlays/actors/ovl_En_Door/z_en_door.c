/*
 * File: z_en_door.c
 * Overlay: ovl_En_Door
 * Description: Wooden Door
 */

#include "z_en_door.h"

#include "libc/assert.h"

#include "objects/object_kinsta2_obj/object_kinsta2_obj.h"
#include "objects/object_dor01/object_dor01.h"
#include "objects/object_dor02/object_dor02.h"
#include "objects/object_dor03/object_dor03.h"
#include "objects/object_dor04/object_dor04.h"
#include "objects/object_wdor01/object_wdor01.h"
#include "objects/object_wdor02/object_wdor02.h"
#include "objects/object_wdor03/object_wdor03.h"
#include "objects/object_wdor04/object_wdor04.h"
#include "objects/object_wdor05/object_wdor05.h"
#include "objects/object_numa_obj/object_numa_obj.h"
#include "objects/object_kaizoku_obj/object_kaizoku_obj.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"

#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

#define DOOR_AJAR_SLAM_RANGE 120.0f
#define DOOR_AJAR_OPEN_RANGE (2 * DOOR_AJAR_SLAM_RANGE)

void EnDoor_Init(Actor* thisx, PlayState* play2);
void EnDoor_Destroy(Actor* thisx, PlayState* play);
void EnDoor_Update(Actor* thisx, PlayState* play);
void EnDoor_Draw(Actor* thisx, PlayState* play);
void EnDoor_Reset(void);

void EnDoor_Idle(EnDoor* this, PlayState* play);
void EnDoor_AjarWait(EnDoor* this, PlayState* play);
void EnDoor_OpenScheduleActor(EnDoor* this, PlayState* play);
void EnDoor_AjarOpen(EnDoor* this, PlayState* play);
void EnDoor_Open(EnDoor* this, PlayState* play);
void EnDoor_AjarClose(EnDoor* this, PlayState* play);
void EnDoor_WaitForObject(EnDoor* this, PlayState* play);

u8 sDoorSch_SwordsmansSchool[] = {
    /* 0x00 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(3, 0x12 - 0x04),
    /* 0x04 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(6, 0, 23, 0, 0x12 - 0x0A),
    /* 0x0A */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(0, 0, 6, 0, 0x12 - 0x10),
    /* 0x10 */ SCHEDULE_CMD_RET_VAL_S(7),
    /* 0x12 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_PostOffice[] = {
    /* 0x00 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(9, 0, 0x4B - 0x04),
    /* 0x04 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(12, 0, 0x1D - 0x08),
    /* 0x08 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(1, 0x1F - 0x0C),
    /* 0x0C */ SCHEDULE_CMD_BRANCH_S(0x0),
    /* 0x0E */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(13, 0, 0x1C - 0x12),
    /* 0x12 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(15, 0, 0x1D - 0x16),
    /* 0x16 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(0, 0, 0x1C - 0x1A),
    /* 0x1A */ SCHEDULE_CMD_RET_VAL_S(9),
    /* 0x1C */ SCHEDULE_CMD_RET_NONE(),
    /* 0x1D */ SCHEDULE_CMD_RET_VAL_S(8),
    /* 0x1F */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(2, 0x3C - 0x23),
    /* 0x23 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_28_08, 0x2E - 0x27),
    /* 0x27 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(13, 0, 0x2D - 0x2B),
    /* 0x2B */ SCHEDULE_CMD_RET_VAL_S(9),
    /* 0x2D */ SCHEDULE_CMD_RET_NONE(),
    /* 0x2E */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(13, 0, 0x1C - 0x32),
    /* 0x32 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(17, 0, 0x1D - 0x36),
    /* 0x36 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(0, 0, 0x1C - 0x3A),
    /* 0x3A */ SCHEDULE_CMD_RET_VAL_S(9),
    /* 0x3C */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(13, 0, 0x1C - 0x40),
    /* 0x40 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(13, 0, 0x1D - 0x44),
    /* 0x44 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(18, 0, 0x49 - 0x48),
    /* 0x48 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x49 */ SCHEDULE_CMD_RET_VAL_S(9),
    /* 0x4B */ SCHEDULE_CMD_RET_VAL_S(9),
};

u8 sDoorSch_LotteryShop[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(6, 0, 23, 0, 0x8 - 0x6),
    /* 0x6 */ SCHEDULE_CMD_RET_VAL_S(28),
    /* 0x8 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_TradingPost[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(21, 0, 22, 0, 0x7 - 0x6),
    /* 0x6 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x7 */ SCHEDULE_CMD_RET_VAL_S(11),
};

u8 sDoorSch_CuriosityShop[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(22, 0, 5, 0, 0x8 - 0x6),
    /* 0x6 */ SCHEDULE_CMD_RET_VAL_S(10),
    /* 0x8 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_LaundryPool[] = {
    /* 0x00 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(2, 0x13 - 0x04),
    /* 0x04 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_28_08, 0x0A - 0x08),
    /* 0x08 */ SCHEDULE_CMD_RET_VAL_S(12),
    /* 0x0A */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(15, 10, 22, 0, 0x12 - 0x10),
    /* 0x10 */ SCHEDULE_CMD_RET_VAL_S(12),
    /* 0x12 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x13 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(3, 0x28 - 0x17),
    /* 0x17 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(13, 0, 0x28 - 0x1B),
    /* 0x1B */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_51_08, 0x21 - 0x1F),
    /* 0x1F */ SCHEDULE_CMD_RET_VAL_S(12),
    /* 0x21 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(22, 0, 0x27 - 0x25),
    /* 0x25 */ SCHEDULE_CMD_RET_VAL_S(12),
    /* 0x27 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x28 */ SCHEDULE_CMD_RET_VAL_S(12),
};

u8 sDoorSch_BombShop[] = {
    /* 0x0 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_TownShootingGallery[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(22, 0, 0x6 - 0x4),
    /* 0x4 */ SCHEDULE_CMD_RET_VAL_S(15),
    /* 0x6 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_TreasureChestShop[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(22, 0, 0x6 - 0x4),
    /* 0x4 */ SCHEDULE_CMD_RET_VAL_S(16),
    /* 0x6 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_HoneyDarlingShop[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(22, 0, 0x6 - 0x4),
    /* 0x4 */ SCHEDULE_CMD_RET_VAL_S(17),
    /* 0x6 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_MilkBar[] = {
    /* 0x00 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 0, 21, 0, 0x15 - 0x06),
    /* 0x06 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(22, 0, 5, 0, 0x0E - 0x0C),
    /* 0x0C */ SCHEDULE_CMD_RET_VAL_S(18),
    /* 0x0E */ SCHEDULE_CMD_CHECK_MISC_S(SCHEDULE_CHECK_MISC_MASK_ROMANI, 0x13 - 0x11),
    /* 0x11 */ SCHEDULE_CMD_RET_VAL_S(34),
    /* 0x13 */ SCHEDULE_CMD_RET_VAL_S(33),
    /* 0x15 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_InnMainEntrance[] = {
    /* 0x00 */ SCHEDULE_CMD_CHECK_MISC_S(SCHEDULE_CHECK_MISC_ROOM_KEY, 0x17 - 0x03),
    /* 0x03 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(3, 0x0E - 0x07),
    /* 0x07 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(8, 0, 0x0C - 0x0B),
    /* 0x0B */ SCHEDULE_CMD_RET_NONE(),
    /* 0x0C */ SCHEDULE_CMD_RET_VAL_S(19),
    /* 0x0E */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(8, 0, 20, 30, 0x16 - 0x14),
    /* 0x14 */ SCHEDULE_CMD_RET_VAL_S(19),
    /* 0x16 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x17 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_InnUpperEntrance[] = {
    /* 0x0 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_InnGrannys[] = {
    /* 0x0 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_InnStaffRoom[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(3, 0x9 - 0x4),
    /* 0x4 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(18, 0, 0x9 - 0x8),
    /* 0x8 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x9 */ SCHEDULE_CMD_RET_VAL_S(20),
};

u8 sDoorSch_InnKnifeChamber[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_MISC_S(SCHEDULE_CHECK_MISC_ROOM_KEY, 0x5 - 0x3),
    /* 0x3 */ SCHEDULE_CMD_RET_VAL_S(22),
    /* 0x5 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_InnLargeSuite[] = {
    /* 0x0 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_MayorsResidenceMainEntrance[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(3, 0x9 - 0x4),
    /* 0x4 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(10, 0, 0xF - 0x8),
    /* 0x8 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x9 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 0, 20, 0, 0x8 - 0xF),
    /* 0xF */ SCHEDULE_CMD_RET_VAL_S(21),
};

u8 sDoorSch_MayorsResidenceMayorDotour[] = {
    /* 0x0 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_MayorsResidenceMadameAroma[] = {
    /* 0x0 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_MayorsResidenceBedroom[] = {
    /* 0x0 */ SCHEDULE_CMD_RET_NONE(),
};

u8 D_80867710[] = {
    /* 0x0 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_RomaniRanchMamasHouse[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(20, 0, 0x6 - 0x4),
    /* 0x4 */ SCHEDULE_CMD_RET_VAL_S(23),
    /* 0x6 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_RomaniRanchBarn[] = {
    /* 0x00 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(1, 0x0B - 0x04),
    /* 0x04 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(2, 30, 0x0A - 0x08),
    /* 0x08 */ SCHEDULE_CMD_RET_VAL_S(24),
    /* 0x0A */ SCHEDULE_CMD_RET_NONE(),
    /* 0x0B */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(3, 0x0A - 0x0F),
    /* 0x0F */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(20, 0, 0x0A - 0x13),
    /* 0x13 */ SCHEDULE_CMD_RET_VAL_S(12),
};

u8 sDoorSch_RomaniRanchCuccoShack[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(20, 0, 0x6 - 0x4),
    /* 0x4 */ SCHEDULE_CMD_RET_VAL_S(25),
    /* 0x6 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_RomaniRanchDoggyRacetrack[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(20, 0, 0x6 - 0x4),
    /* 0x4 */ SCHEDULE_CMD_RET_VAL_S(26),
    /* 0x6 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_RomaniRanchBedroom[] = {
    /* 0x00 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(2, 0x08 - 0x04),
    /* 0x04 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_99_80, 0x0E - 0x08),
    /* 0x08 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(20, 0, 0x14 - 0x0C),
    /* 0x0C */ SCHEDULE_CMD_RET_VAL_S(27),
    /* 0x0E */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(18, 0, 0x14 - 0x12),
    /* 0x12 */ SCHEDULE_CMD_RET_VAL_S(27),
    /* 0x14 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_IkanaCanyonMusicBoxHouse[] = {
    /* 0x00 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_CLEARED_STONE_TOWER_TEMPLE, 0x1B - 0x04),
    /* 0x04 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_75_20, 0x1B - 0x08),
    /* 0x08 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_14_04, 0x0E - 0x0C),
    /* 0x0C */ SCHEDULE_CMD_RET_VAL_S(29),
    /* 0x0E */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_59_01, 0x1A - 0x12),
    /* 0x12 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_61_02, 0x18 - 0x16),
    /* 0x16 */ SCHEDULE_CMD_RET_VAL_S(30),
    /* 0x18 */ SCHEDULE_CMD_RET_VAL_S(31),
    /* 0x1A */ SCHEDULE_CMD_RET_NONE(),
    /* 0x1B */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_DampesHouse[] = {
    /* 0x0 */ SCHEDULE_CMD_RET_VAL_S(32),
};

u8 sDoorSch_MagicHagsPotionShop[] = {
    /* 0x0 */ SCHEDULE_CMD_RET_NONE(),
};

u8 D_80867780[] = {
    /* 0x0 */ SCHEDULE_CMD_RET_NONE(),
};

u8 sDoorSch_SwampShootingGallery[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(22, 0, 0x6 - 0x4),
    /* 0x4 */ SCHEDULE_CMD_RET_VAL_S(35),
    /* 0x6 */ SCHEDULE_CMD_RET_NONE(),
};

/**
 * A schedule returning none means the door can be used normally.
 * Otherwise the result will be used as an offset relative to text message 0x1800
 */
static u8* sDoorSchedules[] = {
    sDoorSch_SwordsmansSchool,            // ENDOOR_SCH_TYPE_SWORDSMANS_SCHOOL
    sDoorSch_PostOffice,                  // ENDOOR_SCH_TYPE_POST_OFFICE
    sDoorSch_LotteryShop,                 // ENDOOR_SCH_TYPE_LOTTERY_SHOP
    sDoorSch_TradingPost,                 // ENDOOR_SCH_TYPE_TRADING_POST
    sDoorSch_CuriosityShop,               // ENDOOR_SCH_TYPE_CURIOSITY_SHOP
    sDoorSch_LaundryPool,                 // ENDOOR_SCH_TYPE_LAUNDRY_POOL
    sDoorSch_BombShop,                    // ENDOOR_SCH_TYPE_BOMB_SHOP
    sDoorSch_TownShootingGallery,         // ENDOOR_SCH_TYPE_TOWN_SHOOTING_GALLERY
    sDoorSch_TreasureChestShop,           // ENDOOR_SCH_TYPE_TREASURE_CHEST_SHOP
    sDoorSch_HoneyDarlingShop,            // ENDOOR_SCH_TYPE_HONEY_DARLING_SHOP
    sDoorSch_MilkBar,                     // ENDOOR_SCH_TYPE_MILK_BAR
    sDoorSch_InnMainEntrance,             // ENDOOR_SCH_TYPE_INN_MAIN_ENTRANCE
    sDoorSch_InnUpperEntrance,            // ENDOOR_SCH_TYPE_INN_UPPER_ENTRANCE
    sDoorSch_InnGrannys,                  // ENDOOR_SCH_TYPE_INN_GRANNYS
    sDoorSch_InnStaffRoom,                // ENDOOR_SCH_TYPE_INN_STAFF_ROOM
    sDoorSch_InnKnifeChamber,             // ENDOOR_SCH_TYPE_INN_KNIFE_CHAMBER
    sDoorSch_InnLargeSuite,               // ENDOOR_SCH_TYPE_INN_LARGE_SUITE
    sDoorSch_MayorsResidenceMainEntrance, // ENDOOR_SCH_TYPE_MAYORS_RESIDENCE_MAIN_ENTRANCE
    sDoorSch_MayorsResidenceMayorDotour,  // ENDOOR_SCH_TYPE_MAYORS_RESIDENCE_MAYOR_DOTOUR
    sDoorSch_MayorsResidenceMadameAroma,  // ENDOOR_SCH_TYPE_MAYORS_RESIDENCE_MADAME_AROMA
    sDoorSch_MayorsResidenceBedroom,      // ENDOOR_SCH_TYPE_MAYORS_RESIDENCE_BEDROOM
    D_80867710,                           // ENDOOR_SCH_TYPE_21
    sDoorSch_RomaniRanchMamasHouse,       // ENDOOR_SCH_TYPE_ROMANI_RANCH_MAMAS_HOUSE
    sDoorSch_RomaniRanchBarn,             // ENDOOR_SCH_TYPE_ROMANI_RANCH_BARN
    sDoorSch_RomaniRanchCuccoShack,       // ENDOOR_SCH_TYPE_ROMANI_RANCH_CUCCO_SHACK
    sDoorSch_RomaniRanchDoggyRacetrack,   // ENDOOR_SCH_TYPE_ROMANI_RANCH_DOGGY_RACETRACK
    sDoorSch_RomaniRanchBedroom,          // ENDOOR_SCH_TYPE_ROMANI_RANCH_BEDROOM
    sDoorSch_IkanaCanyonMusicBoxHouse,    // ENDOOR_SCH_TYPE_IKANA_CANYON_MUSIC_BOX_HOUSE
    sDoorSch_DampesHouse,                 // ENDOOR_SCH_TYPE_DAMPES_HOUSE
    sDoorSch_MagicHagsPotionShop,         // ENDOOR_SCH_TYPE_MAGIC_HAGS_POTION_SHOP
    D_80867780,                           // ENDOOR_SCH_TYPE_30
    sDoorSch_SwampShootingGallery,        // ENDOOR_SCH_TYPE_SWAMP_SHOOTING_GALLERY
};
static_assert(ARRAY_COUNT(sDoorSchedules) == ENDOOR_SCH_TYPE_MAX,
              "The entry count of `sDoorSchedules` should match the `EnDoorScheduleType` enum");

ActorProfile En_Door_Profile = {
    /**/ ACTOR_EN_DOOR,
    /**/ ACTORCAT_DOOR,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnDoor),
    /**/ EnDoor_Init,
    /**/ EnDoor_Destroy,
    /**/ EnDoor_Update,
    /**/ EnDoor_Draw,
    /**/ EnDoor_Reset,
};

typedef enum EnDoorDListIndex {
    /*  0 */ DOOR_DL_DEFAULT,
    /*  1 */ DOOR_DL_WOODFALL,
    /*  2 */ DOOR_DL_OBSERVATORY_LAB,
    /*  3 */ DOOR_DL_ZORA_HALL,
    /*  4 */ DOOR_DL_SWAMP,
    /*  5 */ DOOR_DL_MAGIC_HAG_POTION_SHOP,
    /*  6 */ DOOR_DL_LOTTERY_CURIOSITY_SHIP_MAYOR_HOUSE,
    /*  7 */ DOOR_DL_POST_OFFICE_TRAIDING_POST,
    /*  8 */ DOOR_DL_INN_SCHOOL,
    /*  9 */ DOOR_DL_MILK_BAR,
    /* 10 */ DOOR_DL_MUSIC_BOX,
    /* 11 */ DOOR_DL_PIRATES_FORTESS,
    /* 12 */ DOOR_DL_OCEANSIDE_SPIDER_HOUSE,
    /* 13 */ DOOR_DL_DEFAULT_FIELD_KEEP,
    /* 14 */ DOOR_DL_MAX
} EnDoorDListIndex;

typedef struct EnDoorInfo {
    /* 0x0 */ s16 sceneId;
    /* 0x2 */ u8 dListIndex;
    /* 0x4 */ s16 objectId;
} EnDoorInfo; // size = 0x6

// TODO: This enum feels kinda overkill...
typedef enum EnDoorObjectInfoIndex {
    /*  0 */ DOOR_OBJKIND_DEFAULT,
    /*  0 */ DOOR_OBJINFO_0 = DOOR_OBJKIND_DEFAULT,
    /*  1 */ DOOR_OBJINFO_1,
    /*  2 */ DOOR_OBJINFO_2,
    /*  3 */ DOOR_OBJINFO_3,
    /*  4 */ DOOR_OBJINFO_4,
    /*  5 */ DOOR_OBJINFO_5,
    /*  6 */ DOOR_OBJINFO_6,
    /*  7 */ DOOR_OBJINFO_7,
    /*  8 */ DOOR_OBJINFO_8,
    /*  9 */ DOOR_OBJINFO_9,
    /* 10 */ DOOR_OBJINFO_10,
    /* 11 */ DOOR_OBJINFO_11,
    /* 12 */ DOOR_OBJINFO_12,
    /* 13 */ DOOR_OBJINFO_13,
    /* 14 */ DOOR_OBJINFO_14,

    /* 15 */ DOOR_OBJKIND_KEEP,
    /* 15 */ DOOR_OBJINFO_15 = DOOR_OBJKIND_KEEP,
    /* 16 */ DOOR_OBJINFO_16,

    /* 17 */ DOOR_OBJKIND_SCHEDULE,
    /* 17 */ DOOR_OBJINFO_17 = DOOR_OBJKIND_SCHEDULE, // ENDOOR_SCH_TYPE_SWORDSMANS_SCHOOL
    /* 18 */ DOOR_OBJINFO_18,                         // ENDOOR_SCH_TYPE_POST_OFFICE
    /* 19 */ DOOR_OBJINFO_19,                         // ENDOOR_SCH_TYPE_LOTTERY_SHOP
    /* 20 */ DOOR_OBJINFO_20,                         // ENDOOR_SCH_TYPE_TRADING_POST
    /* 21 */ DOOR_OBJINFO_21,                         // ENDOOR_SCH_TYPE_CURIOSITY_SHOP
    /* 22 */ DOOR_OBJINFO_22,                         // ENDOOR_SCH_TYPE_LAUNDRY_POOL
    /* 23 */ DOOR_OBJINFO_23,                         // ENDOOR_SCH_TYPE_BOMB_SHOP
    /* 24 */ DOOR_OBJINFO_24,                         // ENDOOR_SCH_TYPE_TOWN_SHOOTING_GALLERY
    /* 25 */ DOOR_OBJINFO_25,                         // ENDOOR_SCH_TYPE_TREASURE_CHEST_SHOP
    /* 26 */ DOOR_OBJINFO_26,                         // ENDOOR_SCH_TYPE_HONEY_DARLING_SHOP
    /* 27 */ DOOR_OBJINFO_27,                         // ENDOOR_SCH_TYPE_MILK_BAR
    /* 28 */ DOOR_OBJINFO_28,                         // ENDOOR_SCH_TYPE_INN_MAIN_ENTRANCE
    /* 29 */ DOOR_OBJINFO_29,                         // ENDOOR_SCH_TYPE_INN_UPPER_ENTRANCE
    /* 30 */ DOOR_OBJINFO_30,                         // ENDOOR_SCH_TYPE_INN_GRANNYS
    /* 31 */ DOOR_OBJINFO_31,                         // ENDOOR_SCH_TYPE_INN_STAFF_ROOM
    /* 32 */ DOOR_OBJINFO_32,                         // ENDOOR_SCH_TYPE_INN_KNIFE_CHAMBER
    /* 33 */ DOOR_OBJINFO_33,                         // ENDOOR_SCH_TYPE_INN_LARGE_SUITE
    /* 34 */ DOOR_OBJINFO_34,                         // ENDOOR_SCH_TYPE_MAYORS_RESIDENCE_MAIN_ENTRANCE
    /* 35 */ DOOR_OBJINFO_35,                         // ENDOOR_SCH_TYPE_MAYORS_RESIDENCE_MAYOR_DOTOUR
    /* 36 */ DOOR_OBJINFO_36,                         // ENDOOR_SCH_TYPE_MAYORS_RESIDENCE_MADAME_AROMA
    /* 37 */ DOOR_OBJINFO_37,                         // ENDOOR_SCH_TYPE_MAYORS_RESIDENCE_BEDROOM
    /* 38 */ DOOR_OBJINFO_38,                         // ENDOOR_SCH_TYPE_21
    /* 39 */ DOOR_OBJINFO_39,                         // ENDOOR_SCH_TYPE_ROMANI_RANCH_MAMAS_HOUSE
    /* 40 */ DOOR_OBJINFO_40,                         // ENDOOR_SCH_TYPE_ROMANI_RANCH_BARN
    /* 41 */ DOOR_OBJINFO_41,                         // ENDOOR_SCH_TYPE_ROMANI_RANCH_CUCCO_SHACK
    /* 42 */ DOOR_OBJINFO_42,                         // ENDOOR_SCH_TYPE_ROMANI_RANCH_DOGGY_RACETRACK
    /* 43 */ DOOR_OBJINFO_43,                         // ENDOOR_SCH_TYPE_ROMANI_RANCH_BEDROOM
    /* 44 */ DOOR_OBJINFO_44,                         // ENDOOR_SCH_TYPE_IKANA_CANYON_MUSIC_BOX_HOUSE
    /* 45 */ DOOR_OBJINFO_45,                         // ENDOOR_SCH_TYPE_DAMPES_HOUSE
    /* 46 */ DOOR_OBJINFO_46,                         // ENDOOR_SCH_TYPE_MAGIC_HAGS_POTION_SHOP
    /* 47 */ DOOR_OBJINFO_47,                         // ENDOOR_SCH_TYPE_30
    /* 48 */ DOOR_OBJINFO_48,                         // ENDOOR_SCH_TYPE_SWAMP_SHOOTING_GALLERY
    /* 49 */ DOOR_OBJINFO_MAX
} EnDoorObjectInfoIndex;

// These static asserts try to ensure the two enums don't get out of sync
static_assert(ENDOOR_SCH_TYPE_SWORDSMANS_SCHOOL == DOOR_OBJINFO_17 - DOOR_OBJKIND_SCHEDULE,
              "The enums values of `EnDoorScheduleType` and `EnDoorObjectInfoIndex` (from `DOOR_OBJKIND_SCHEDULE` "
              "onwards) must be synced.");
static_assert(ENDOOR_SCH_TYPE_MAX == DOOR_OBJINFO_MAX - DOOR_OBJKIND_SCHEDULE,
              "The enums values of `EnDoorScheduleType` and `EnDoorObjectInfoIndex` (from `DOOR_OBJKIND_SCHEDULE` "
              "onwards) must be synced.");

static EnDoorInfo sObjectInfo[] = {
    // DOOR_OBJKIND_DEFAULT
    { SCENE_MITURIN, DOOR_DL_WOODFALL, OBJECT_NUMA_OBJ },                  // DOOR_OBJINFO_0
    { SCENE_TENMON_DAI, DOOR_DL_OBSERVATORY_LAB, OBJECT_DOR01 },           // DOOR_OBJINFO_1
    { SCENE_00KEIKOKU, DOOR_DL_OBSERVATORY_LAB, OBJECT_DOR01 },            // DOOR_OBJINFO_2
    { SCENE_30GYOSON, DOOR_DL_OBSERVATORY_LAB, OBJECT_DOR01 },             // DOOR_OBJINFO_3
    { SCENE_LABO, DOOR_DL_OBSERVATORY_LAB, OBJECT_DOR01 },                 // DOOR_OBJINFO_4
    { SCENE_33ZORACITY, DOOR_DL_ZORA_HALL, OBJECT_DOR02 },                 // DOOR_OBJINFO_5
    { SCENE_UNSET_31, DOOR_DL_ZORA_HALL, OBJECT_DOR02 },                   // DOOR_OBJINFO_6
    { SCENE_BANDROOM, DOOR_DL_ZORA_HALL, OBJECT_DOR02 },                   // DOOR_OBJINFO_7
    { SCENE_20SICHITAI, DOOR_DL_SWAMP, OBJECT_DOR03 },                     // DOOR_OBJINFO_8
    { SCENE_20SICHITAI2, DOOR_DL_SWAMP, OBJECT_DOR03 },                    // DOOR_OBJINFO_9
    { SCENE_MAP_SHOP, DOOR_DL_SWAMP, OBJECT_DOR03 },                       // DOOR_OBJINFO_10
    { SCENE_KAIZOKU, DOOR_DL_PIRATES_FORTESS, OBJECT_KAIZOKU_OBJ },        // DOOR_OBJINFO_11
    { SCENE_PIRATE, DOOR_DL_PIRATES_FORTESS, OBJECT_KAIZOKU_OBJ },         // DOOR_OBJINFO_12
    { SCENE_TORIDE, DOOR_DL_PIRATES_FORTESS, OBJECT_KAIZOKU_OBJ },         // DOOR_OBJINFO_13
    { SCENE_KINDAN2, DOOR_DL_OCEANSIDE_SPIDER_HOUSE, OBJECT_KINSTA2_OBJ }, // DOOR_OBJINFO_14

    // DOOR_OBJKIND_KEEP
    { -1, DOOR_DL_DEFAULT, GAMEPLAY_KEEP },                  // DOOR_OBJINFO_15
    { -1, DOOR_DL_DEFAULT_FIELD_KEEP, GAMEPLAY_FIELD_KEEP }, // DOOR_OBJINFO_16

    // DOOR_OBJKIND_SCHEDULE
    { -1, DOOR_DL_INN_SCHOOL, OBJECT_WDOR03 },                         // DOOR_OBJINFO_17
    { -1, DOOR_DL_POST_OFFICE_TRAIDING_POST, OBJECT_WDOR02 },          // DOOR_OBJINFO_18
    { -1, DOOR_DL_LOTTERY_CURIOSITY_SHIP_MAYOR_HOUSE, OBJECT_WDOR01 }, // DOOR_OBJINFO_19
    { -1, DOOR_DL_POST_OFFICE_TRAIDING_POST, OBJECT_WDOR02 },          // DOOR_OBJINFO_20
    { -1, DOOR_DL_LOTTERY_CURIOSITY_SHIP_MAYOR_HOUSE, OBJECT_WDOR01 }, // DOOR_OBJINFO_21
    { -1, DOOR_DL_DEFAULT, GAMEPLAY_KEEP },                            // DOOR_OBJINFO_22
    { -1, DOOR_DL_DEFAULT, GAMEPLAY_KEEP },                            // DOOR_OBJINFO_23
    { -1, DOOR_DL_DEFAULT, GAMEPLAY_KEEP },                            // DOOR_OBJINFO_24
    { -1, DOOR_DL_DEFAULT, GAMEPLAY_KEEP },                            // DOOR_OBJINFO_25
    { -1, DOOR_DL_DEFAULT, GAMEPLAY_KEEP },                            // DOOR_OBJINFO_26
    { -1, DOOR_DL_MILK_BAR, OBJECT_WDOR04 },                           // DOOR_OBJINFO_27
    { -1, DOOR_DL_INN_SCHOOL, OBJECT_WDOR03 },                         // DOOR_OBJINFO_28
    { -1, DOOR_DL_INN_SCHOOL, OBJECT_WDOR03 },                         // DOOR_OBJINFO_29
    { -1, DOOR_DL_DEFAULT_FIELD_KEEP, GAMEPLAY_FIELD_KEEP },           // DOOR_OBJINFO_30
    { -1, DOOR_DL_DEFAULT_FIELD_KEEP, GAMEPLAY_FIELD_KEEP },           // DOOR_OBJINFO_31
    { -1, DOOR_DL_DEFAULT_FIELD_KEEP, GAMEPLAY_FIELD_KEEP },           // DOOR_OBJINFO_32
    { -1, DOOR_DL_DEFAULT_FIELD_KEEP, GAMEPLAY_FIELD_KEEP },           // DOOR_OBJINFO_33
    { -1, DOOR_DL_LOTTERY_CURIOSITY_SHIP_MAYOR_HOUSE, OBJECT_WDOR01 }, // DOOR_OBJINFO_34
    { -1, DOOR_DL_DEFAULT, GAMEPLAY_KEEP },                            // DOOR_OBJINFO_35
    { -1, DOOR_DL_DEFAULT, GAMEPLAY_KEEP },                            // DOOR_OBJINFO_36
    { -1, DOOR_DL_DEFAULT, GAMEPLAY_KEEP },                            // DOOR_OBJINFO_37
    { -1, DOOR_DL_DEFAULT, GAMEPLAY_KEEP },                            // DOOR_OBJINFO_38
    { -1, DOOR_DL_DEFAULT_FIELD_KEEP, GAMEPLAY_FIELD_KEEP },           // DOOR_OBJINFO_39
    { -1, DOOR_DL_DEFAULT_FIELD_KEEP, GAMEPLAY_FIELD_KEEP },           // DOOR_OBJINFO_40
    { -1, DOOR_DL_DEFAULT_FIELD_KEEP, GAMEPLAY_FIELD_KEEP },           // DOOR_OBJINFO_41
    { -1, DOOR_DL_DEFAULT_FIELD_KEEP, GAMEPLAY_FIELD_KEEP },           // DOOR_OBJINFO_42
    { -1, DOOR_DL_DEFAULT, GAMEPLAY_KEEP },                            // DOOR_OBJINFO_43
    { -1, DOOR_DL_MUSIC_BOX, OBJECT_WDOR05 },                          // DOOR_OBJINFO_44
    { -1, DOOR_DL_DEFAULT_FIELD_KEEP, GAMEPLAY_FIELD_KEEP },           // DOOR_OBJINFO_45
    { -1, DOOR_DL_MAGIC_HAG_POTION_SHOP, OBJECT_DOR04 },               // DOOR_OBJINFO_46
    { -1, DOOR_DL_MILK_BAR, OBJECT_WDOR04 },                           // DOOR_OBJINFO_47
    { -1, DOOR_DL_SWAMP, OBJECT_DOR03 },                               // DOOR_OBJINFO_48
};
static_assert(ARRAY_COUNT(sObjectInfo) == DOOR_OBJINFO_MAX,
              "The entry count of `sObjectInfo` should match the `EnDoorObjectInfoIndex` enum");

static InitChainEntry sInitChain[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_0, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_U16(shape.rot.x, 0, ICHAIN_CONTINUE),
    ICHAIN_U16(shape.rot.z, 0, ICHAIN_STOP),
};

static AnimationHeader* sAnimations[] = {
    // left
    &gDoorFierceDeityZoraOpenLeftAnim, // PLAYER_FORM_FIERCE_DEITY
    &gDoorGoronOpenLeftAnim,           // PLAYER_FORM_GORON
    &gDoorFierceDeityZoraOpenLeftAnim, // PLAYER_FORM_ZORA
    &gDoorDekuOpenLeftAnim,            // PLAYER_FORM_DEKU
    &gDoorHumanOpenLeftAnim,           // PLAYER_FORM_HUMAN
    // right
    &gDoorFierceDeityZoraOpenRightAnim, // PLAYER_FORM_FIERCE_DEITY
    &gDoorGoronOpenRightAnim,           // PLAYER_FORM_GORON
    &gDoorFierceDeityZoraOpenRightAnim, // PLAYER_FORM_ZORA
    &gDoorDekuOpenRightAnim,            // PLAYER_FORM_DEKU
    &gDoorHumanOpenRightAnim,           // PLAYER_FORM_HUMAN
};
static_assert(ARRAY_COUNT(sAnimations) == 2 * PLAYER_FORM_MAX,
              "The entry count of `sAnimations` should be exactly twice as PLAYER_FORM_MAX");

static u8 sAnimOpenFrames[] = {
    // left
    25, // PLAYER_FORM_FIERCE_DEITY
    25, // PLAYER_FORM_GORON
    25, // PLAYER_FORM_ZORA
    25, // PLAYER_FORM_DEKU
    25, // PLAYER_FORM_HUMAN
    // right
    25, // PLAYER_FORM_FIERCE_DEITY
    25, // PLAYER_FORM_GORON
    25, // PLAYER_FORM_ZORA
    25, // PLAYER_FORM_DEKU
    25, // PLAYER_FORM_HUMAN
};
static_assert(ARRAY_COUNT(sAnimOpenFrames) == 2 * PLAYER_FORM_MAX,
              "The entry count of `sAnimOpenFrames` should be exactly twice as PLAYER_FORM_MAX");

static u8 sAnimCloseFrames[] = {
    // left
    60, // PLAYER_FORM_FIERCE_DEITY
    60, // PLAYER_FORM_GORON
    60, // PLAYER_FORM_ZORA
    70, // PLAYER_FORM_DEKU
    70, // PLAYER_FORM_HUMAN
    // right
    60, // PLAYER_FORM_FIERCE_DEITY
    60, // PLAYER_FORM_GORON
    60, // PLAYER_FORM_ZORA
    60, // PLAYER_FORM_DEKU
    70, // PLAYER_FORM_HUMAN
};
static_assert(ARRAY_COUNT(sAnimCloseFrames) == 2 * PLAYER_FORM_MAX,
              "The entry count of `sAnimCloseFrames` should be exactly twice as PLAYER_FORM_MAX");

static Gfx* sDoorDLists[][2] = {
    { gDoorLeftDL, gDoorRightDL },                            // DOOR_DL_DEFAULT
    { gWoodfallDoorDL, gWoodfallDoorDL },                     // DOOR_DL_WOODFALL
    { gObservatoryLabDoorDL, gObservatoryLabDoorDL },         // DOOR_DL_OBSERVATORY_LAB
    { gZoraHallDoorDL, gZoraHallDoorDL },                     // DOOR_DL_ZORA_HALL
    { gSwampDoorDL, gSwampDoorDL },                           // DOOR_DL_SWAMP
    { gMagicHagPotionShopDoorDL, gMagicHagPotionShopDoorDL }, // DOOR_DL_MAGIC_HAG_POTION_SHOP
    { gLotteryCuriosityShopMayorHouseDoorDL,
      gLotteryCuriosityShopMayorHouseDoorDL },                      // DOOR_DL_LOTTERY_CURIOSITY_SHIP_MAYOR_HOUSE
    { gPostOfficeTradingPostDoorDL, gPostOfficeTradingPostDoorDL }, // DOOR_DL_POST_OFFICE_TRAIDING_POST
    { gInnSchoolDoorDL, gInnSchoolDoorDL },                         // DOOR_DL_INN_SCHOOL
    { gMilkBarDoorDL, gMilkBarDoorDL },                             // DOOR_DL_MILK_BAR
    { gMusicBoxHouseDoorDL, gMusicBoxHouseDoorDL },                 // DOOR_DL_MUSIC_BOX
    { gPiratesFortressDoorDL, gPiratesFortressDoorDL },             // DOOR_DL_PIRATES_FORTESS
    { gOceansideSpiderHouseDoorDL, gOceansideSpiderHouseDoorDL },   // DOOR_DL_OCEANSIDE_SPIDER_HOUSE
    { gFieldWoodDoorLeftDL, gFieldWoodDoorRightDL },                // DOOR_DL_DEFAULT_FIELD_KEEP
};
static_assert(ARRAY_COUNT(sDoorDLists) == DOOR_DL_MAX,
              "The entry count of `sDoorDLists` should match the `EnDoorDListIndex` enum");

void EnDoor_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    s32 objectSlot;
    EnDoorInfo* objectInfo = &sObjectInfo[0];
    EnDoor* this = (EnDoor*)thisx;
    s32 i;

    Actor_ProcessInitChain(&this->knobDoor.dyna.actor, sInitChain);

    this->doorType = ENDOOR_GET_TYPE(thisx);
    this->typeVar.data = ENDOOR_GET_TYPE_VAR(thisx);

    if ((this->doorType == ENDOOR_TYPE_FRAMED) && (this->typeVar.frameType == ENDOOR_FRAMED_FRAME)) {
        DynaPolyActor_Init(&this->knobDoor.dyna, 0);
        DynaPolyActor_LoadMesh(play, &this->knobDoor.dyna, &gFramedDoorCol);
    }

    SkelAnime_Init(play, &this->knobDoor.skelAnime, &gDoorSkel, &gDoorFierceDeityZoraOpenLeftAnim, this->limbTable,
                   this->limbTable, DOOR_LIMB_MAX);

    if (this->doorType == ENDOOR_TYPE_SCHEDULE) {
        objectInfo = &sObjectInfo[DOOR_OBJKIND_SCHEDULE + this->typeVar.schType];
    } else {
        // Look for the EnDoorInfo corresponding to the current scene.
        // If no EnDoorInfo matches the current scene then objectInfo will point to the GAMEPLAY_KEEP one
        for (i = 0; i < DOOR_OBJKIND_KEEP; i++, objectInfo++) {
            if (play->sceneId == objectInfo->sceneId) {
                break;
            }
        }

        // If objectInfo is pointing to the GAMEPLAY_KEEP's EnDoorInfo one and GAMEPLAY_FIELD_KEEP is loaded then use
        // that one instead
        if ((i >= DOOR_OBJKIND_KEEP) && (Object_GetSlot(&play->objectCtx, GAMEPLAY_FIELD_KEEP) > OBJECT_SLOT_NONE)) {
            objectInfo++;
        }
    }

    // This assignment is redundant since it is set later again
    this->knobDoor.dlIndex = objectInfo->dListIndex;

    // Check if the object for the selected EnDoorInfo is loaded.
    // If it isn't, then fallback to GAMEPLAY_KEEP
    objectSlot = Object_GetSlot(&play->objectCtx, objectInfo->objectId);
    if (objectSlot <= OBJECT_SLOT_NONE) {
        objectInfo = &sObjectInfo[DOOR_OBJINFO_15];
        objectSlot = Object_GetSlot(&play->objectCtx, objectInfo->objectId);
        if (objectSlot != 0) {
            Actor_Kill(&this->knobDoor.dyna.actor);
            return;
        }
    }

    this->knobDoor.objectSlot = objectSlot;
    this->knobDoor.dlIndex = objectInfo->dListIndex;

    // If the object that will be used is the one from the InitVars then call EnDoor_WaitForObject directly since we
    // know the object will be loaded before this actor has spawned
    if (this->knobDoor.dyna.actor.objectSlot == this->knobDoor.objectSlot) {
        EnDoor_WaitForObject(this, play);
    } else {
        this->actionFunc = EnDoor_WaitForObject;
    }

    Actor_SetFocus(&this->knobDoor.dyna.actor, 35.0f);
}

void EnDoor_Destroy(Actor* thisx, PlayState* play) {
    EnDoor* this = (EnDoor*)thisx;

    if (this->doorType != ENDOOR_TYPE_FRAMED) {
        TransitionActorEntry* transitionEntry =
            &play->transitionActors.list[DOOR_GET_TRANSITION_ID(&this->knobDoor.dyna.actor)];
        if (transitionEntry->id < 0) {
            transitionEntry->id = -transitionEntry->id;
        }
    } else if (this->typeVar.frameType == ENDOOR_FRAMED_FRAME) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->knobDoor.dyna.bgId);
    }
}

void EnDoor_WaitForObject(EnDoor* this, PlayState* play) {
    if (!Object_IsLoaded(&play->objectCtx, this->knobDoor.objectSlot)) {
        return;
    }

    this->knobDoor.dyna.actor.objectSlot = this->knobDoor.objectSlot;
    this->actionFunc = EnDoor_Idle;
    this->knobDoor.dyna.actor.world.rot.y = 0;

    if (this->doorType == ENDOOR_TYPE_LOCKED) {
        if (!Flags_GetSwitch(play, this->typeVar.switchFlag)) {
            this->lockTimer = 10;
        }
    } else if ((this->doorType == ENDOOR_TYPE_AJAR) &&
               (Actor_WorldDistXZToActor(&this->knobDoor.dyna.actor, &GET_PLAYER(play)->actor) >
                DOOR_AJAR_SLAM_RANGE)) {
        this->actionFunc = EnDoor_AjarWait;
        this->knobDoor.dyna.actor.world.rot.y = -0x1800;
    }
}

// 2S2H [Port] Moved static vars out of function scope so they can be cleared in actor reset
// Set to true when the MilkBar door acknowledges player is a member
s32 sDoorIsMilkBarMember;

/**
 * Closed door waiting for interaction.
 *
 * General flow of this function and what is prioritized:
 * - Handle opening request from player
 * - Handle opening request from schedule actor
 * - If not on cs mode:
 *   - If sDoorIsMilkBarMember or player is near the door and looking at it
 *     - Set this door as the one Player can interact with
 *     - If it is a locked door
 *       - Handle loocked door
 *     - Otherwise if the door is an AJAR one
 *       - Display a message saying it won't open
 *     - Otherwise if ENDOOR_TYPE_WHOLE_DAY? or ENDOOR_TYPE_DAY? or ENDOOR_TYPE_NIGHT?
 *       - Left the door closed depending on the day/night state expected by the door
 *     - Otherwise if ENDOOR_TYPE_SCHEDULE
 *       - Run schedule
 *       - Possibly not allow to open the door and prompt a message depending on the schedule result.
 *   - Otherwise if ENDOOR_TYPE_AJAR
 *     - If player is further than DOOR_AJAR_OPEN_RANGE
 *       - Play Ajar's open/close loop
 */
void EnDoor_Idle(EnDoor* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    // 0x1821: Player is a member of the Milk bar
    if (Actor_TalkOfferAccepted(&this->knobDoor.dyna.actor, &play->state) &&
        (this->knobDoor.dyna.actor.textId == 0x1821)) {
        sDoorIsMilkBarMember = true;
    }

    if (this->knobDoor.requestOpen) {
        // Player or an NPC has requested this door to open

        this->actionFunc = EnDoor_Open;
        Animation_PlayOnceSetSpeed(&this->knobDoor.skelAnime, sAnimations[this->knobDoor.animIndex],
                                   (player->stateFlags1 & PLAYER_STATE1_8000000) ? 0.75f : 1.5f);

        // If this is a locked door then handle small key counts, sfx and switch flag
        if (this->lockTimer != 0) {
            DUNGEON_KEY_COUNT(gSaveContext.mapIndex) = DUNGEON_KEY_COUNT(gSaveContext.mapIndex) - 1;
            Flags_SetSwitch(play, this->typeVar.switchFlag);
            Actor_PlaySfx(&this->knobDoor.dyna.actor, NA_SE_EV_CHAIN_KEY_UNLOCK);
        }
    } else if (this->openTimer != 0) {
        // An schedule actor has requested to open this door

        this->actionFunc = EnDoor_OpenScheduleActor;
        Actor_PlaySfx(&this->knobDoor.dyna.actor, NA_SE_EV_DOOR_OPEN);
    } else if (!Player_InCsMode(play)) {
        Vec3f playerPosRelToDoor;

        // Check if player is near this door and looking at it
        Actor_WorldToActorCoords(&this->knobDoor.dyna.actor, &playerPosRelToDoor, &player->actor.world.pos);
        if (sDoorIsMilkBarMember ||
            ((fabsf(playerPosRelToDoor.y) < 20.0f) && (fabsf(playerPosRelToDoor.x) < 20.0f) &&
             GameInteractor_Should(VB_BE_NEAR_DOOR, fabsf(playerPosRelToDoor.z) < 50.0f, &playerPosRelToDoor.z))) {
            s16 yawDiff = player->actor.shape.rot.y - this->knobDoor.dyna.actor.shape.rot.y;

            if (playerPosRelToDoor.z > 0.0f) {
                yawDiff = 0x8000 - yawDiff;
            }
            if (ABS_ALT(yawDiff) < 0x3000) {
                // Set this door as the one Player can interact with
                player->doorType = PLAYER_DOORTYPE_HANDLE;
                player->doorDirection = playerPosRelToDoor.z >= 0.0f ? 1.0f : -1.0f;
                player->doorActor = &this->knobDoor.dyna.actor;

                if (this->lockTimer != 0) {
                    if (DUNGEON_KEY_COUNT(gSaveContext.mapIndex) <= 0) {
                        player->doorType = PLAYER_DOORTYPE_TALKING;
                        // 0x1802: "Missing small key"
                        this->knobDoor.dyna.actor.textId = 0x1802;
                    } else {
                        player->doorTimer = 10;
                    }
                } else if (this->doorType == ENDOOR_TYPE_AJAR) {
                    player->doorType = PLAYER_DOORTYPE_TALKING;
                    // 0x1800: "It won't open"
                    this->knobDoor.dyna.actor.textId = 0x1800;
                } else if ((this->doorType == ENDOOR_TYPE_WHOLE_DAY) || (this->doorType == ENDOOR_TYPE_DAY) ||
                           (this->doorType == ENDOOR_TYPE_NIGHT)) {
                    s32 halfDaysDayBit = (play->actorCtx.halfDaysBit & HALFDAYBIT_DAWNS) >> 1;
                    s32 halfDaysNightBit = play->actorCtx.halfDaysBit & HALFDAYBIT_NIGHTS;
                    s16 openBit = D_801AED48[ENDOOR_GET_HALFDAYBIT_INDEX_FROM_HALFDAYCHECK(this->typeVar.halfDayCheck)];
                    s32 textIdOffset = ENDOOR_GET_TEXTOFFSET_FROM_HALFDAYCHECK(this->typeVar.halfDayCheck);

                    // Check if the door should be closed, and prompt a message if its the case
                    if (((this->doorType == ENDOOR_TYPE_WHOLE_DAY) &&
                         !((halfDaysDayBit | halfDaysNightBit) & openBit)) ||
                        ((this->doorType == ENDOOR_TYPE_DAY) && !(halfDaysNightBit & openBit)) ||
                        ((this->doorType == ENDOOR_TYPE_NIGHT) && !(halfDaysDayBit & openBit))) {
                        //! @bug 0x182D does not exist, and there's no message for like 86 entries
                        s16 baseTextId = 0x182D;

                        if (this->doorType == ENDOOR_TYPE_NIGHT) {
                            // 0x180D to 0x181C: messages indicating certain building are closed at night
                            baseTextId = 0x180D;
                        } else if (this->doorType == ENDOOR_TYPE_DAY) {
                            // 0x181D to 0x1820: messages for when Pamela is inside the Music Box house with the door
                            // closed
                            baseTextId = 0x181D;
                        }
                        player->doorType = PLAYER_DOORTYPE_TALKING;
                        this->knobDoor.dyna.actor.textId = baseTextId + textIdOffset;
                    }
                } else if ((this->doorType == ENDOOR_TYPE_SCHEDULE) && (playerPosRelToDoor.z > 0.0f)) {
                    ScheduleOutput scheduleOutput;

                    if (Schedule_RunScript(play, sDoorSchedules[this->typeVar.schType], &scheduleOutput)) {
                        this->knobDoor.dyna.actor.textId = scheduleOutput.result + 0x1800;

                        // 0x1821: Player is a member of the Milk bar
                        // When player closes this specific message then the door changes to PLAYER_DOORTYPE_PROXIMITY,
                        // allowing player to open the door without having to press the A button again
                        player->doorType = ((this->knobDoor.dyna.actor.textId == 0x1821) && sDoorIsMilkBarMember)
                                               ? PLAYER_DOORTYPE_PROXIMITY
                                               : PLAYER_DOORTYPE_TALKING;
                    }
                }
                func_80122F28(player);
            }
        } else if ((this->doorType == ENDOOR_TYPE_AJAR) &&
                   (this->knobDoor.dyna.actor.xzDistToPlayer > DOOR_AJAR_OPEN_RANGE)) {
            Actor_PlaySfx(&this->knobDoor.dyna.actor, NA_SE_EV_DOOR_OPEN);
            this->actionFunc = EnDoor_AjarOpen;
        }
    }
}

/**
 * Handle opening and closing request from an schedule actor.
 *
 * To trigger this an schedule actor must set the `openTimer` member, where its magnitude specifies for how many frames
 * the door will be open (after the opening animation) and its sign indicates which direction the door will open. When
 * the specified amount of frames has passed the door will closed automatically, without needing intervention from the
 * schedule actor.
 */
void EnDoor_OpenScheduleActor(EnDoor* this, PlayState* play) {
    s32 direction;

    if (this->openTimer != 0) {
        if (this->openTimer >= 0) {
            direction = 1;
        } else {
            direction = -1;
        }
        if (Math_ScaledStepToS(&this->knobDoor.dyna.actor.world.rot.y, direction * 0x3E80, 0x7D0)) {
            Math_StepToC(&this->openTimer, 0, 1);
        }
    } else {
        if (Math_ScaledStepToS(&this->knobDoor.dyna.actor.world.rot.y, 0, 0x7D0)) {
            this->actionFunc = EnDoor_Idle;
            Actor_PlaySfx(&this->knobDoor.dyna.actor, NA_SE_EV_AUTO_DOOR_CLOSE);
        }
    }
}

void EnDoor_AjarWait(EnDoor* this, PlayState* play) {
    if (this->knobDoor.dyna.actor.xzDistToPlayer < DOOR_AJAR_SLAM_RANGE) {
        this->actionFunc = EnDoor_AjarClose;
    }
}

void EnDoor_AjarOpen(EnDoor* this, PlayState* play) {
    if (this->knobDoor.dyna.actor.xzDistToPlayer < DOOR_AJAR_SLAM_RANGE) {
        this->actionFunc = EnDoor_AjarClose;
    } else if (Math_ScaledStepToS(&this->knobDoor.dyna.actor.world.rot.y, -0x1800, 0x100)) {
        this->actionFunc = EnDoor_AjarWait;
    }
}

void EnDoor_AjarClose(EnDoor* this, PlayState* play) {
    if (Math_ScaledStepToS(&this->knobDoor.dyna.actor.world.rot.y, 0, 0x700)) {
        Actor_PlaySfx(&this->knobDoor.dyna.actor, NA_SE_EV_DOOR_CLOSE);
        this->actionFunc = EnDoor_Idle;
    }
}

void EnDoor_Open(EnDoor* this, PlayState* play) {
    s32 numEffects;
    s32 i;

    if (DECR(this->lockTimer) == 0) {
        if (SkelAnime_Update(&this->knobDoor.skelAnime)) {
            this->actionFunc = EnDoor_Idle;
            this->knobDoor.requestOpen = false;
        } else if (Animation_OnFrame(&this->knobDoor.skelAnime, sAnimOpenFrames[this->knobDoor.animIndex])) {
            Actor_PlaySfx(&this->knobDoor.dyna.actor, NA_SE_OC_DOOR_OPEN);
            if (this->knobDoor.skelAnime.playSpeed < 1.5f) {
                numEffects = (s32)(Rand_ZeroOne() * 30.0f) + 50;
                for (i = 0; i < numEffects; i++) {
                    EffectSsBubble_Spawn(play, &this->knobDoor.dyna.actor.world.pos, 60.0f, 100.0f, 50.0f, 0.15f);
                }
            }
        } else if (Animation_OnFrame(&this->knobDoor.skelAnime, sAnimCloseFrames[this->knobDoor.animIndex])) {
            Actor_PlaySfx(&this->knobDoor.dyna.actor, NA_SE_EV_DOOR_CLOSE);
        }
    }
}

void EnDoor_Update(Actor* thisx, PlayState* play) {
    EnDoor* this = (EnDoor*)thisx;

    this->actionFunc(this, play);
}

s32 EnDoor_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    TransitionActorEntry* transitionEntry;
    EnDoor* this = (EnDoor*)thisx;

    if (limbIndex == DOOR_LIMB_4) {
        Gfx** sideDLists = sDoorDLists[this->knobDoor.dlIndex];
        s16 temp;
        s32 dlIndex;

        transitionEntry = NULL;

        if (this->doorType != ENDOOR_TYPE_FRAMED) {
            transitionEntry = &play->transitionActors.list[DOOR_GET_TRANSITION_ID(&this->knobDoor.dyna.actor)];
        }

        rot->z += this->knobDoor.dyna.actor.world.rot.y;
        if ((this->doorType == ENDOOR_TYPE_FRAMED) || (play->roomCtx.prevRoom.num >= 0) ||
            (transitionEntry->sides[0].room == transitionEntry->sides[1].room)) {
            s32 pad;

            temp =
                (this->knobDoor.dyna.actor.shape.rot.y + this->knobDoor.skelAnime.jointTable[DOOR_LIMB_3].z + rot->z) -
                Math_Vec3f_Yaw(&play->view.eye, &this->knobDoor.dyna.actor.world.pos);
            *dList = (ABS_ALT(temp) < 0x4000) ? sideDLists[0] : sideDLists[1];

        } else {
            dlIndex = 0;
            if (transitionEntry->sides[0].room != this->knobDoor.dyna.actor.room) {
                dlIndex = 1;
            }
            *dList = sideDLists[dlIndex];
        }
    }

    return false;
}

void EnDoor_Draw(Actor* thisx, PlayState* play) {
    EnDoor* this = (EnDoor*)thisx;

    // Ensure the object that will be used is loaded
    if (this->knobDoor.dyna.actor.objectSlot == this->knobDoor.objectSlot) {
        OPEN_DISPS(play->state.gfxCtx);

        if ((this->doorType == ENDOOR_TYPE_FRAMED) && (this->typeVar.frameType == ENDOOR_FRAMED_FRAME)) {
            Gfx_DrawDListOpa(play, gameplay_keep_DL_0221B8);
        } else {
            Gfx_SetupDL25_Opa(play->state.gfxCtx);
        }

        SkelAnime_DrawOpa(play, this->knobDoor.skelAnime.skeleton, this->knobDoor.skelAnime.jointTable,
                          EnDoor_OverrideLimbDraw, NULL, &this->knobDoor.dyna.actor);

        if (this->knobDoor.dyna.actor.world.rot.y != 0) {
            if (this->knobDoor.dyna.actor.world.rot.y > 0) {
                gSPDisplayList(POLY_OPA_DISP++, gDoorRightDL);
            } else {
                gSPDisplayList(POLY_OPA_DISP++, gDoorLeftDL);
            }
        }
        if (this->lockTimer) {
            Actor_DrawDoorLock(play, this->lockTimer, DOORLOCK_NORMAL);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnDoor_Reset(void) {
    sDoorIsMilkBarMember = false;
}
