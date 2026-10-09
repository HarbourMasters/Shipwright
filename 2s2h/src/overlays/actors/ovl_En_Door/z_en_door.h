#ifndef Z_EN_DOOR_H
#define Z_EN_DOOR_H

#include "global.h"
#include "z64door.h"
#include "objects/gameplay_keep/gameplay_keep.h"

/**
 * Actor Parameters
 *
 * |                  |       |
 * | Transition Index | Type  | Type var
 * |------------------|-------|---------------
 * | 0 0 0 0 0 0      | 0 0 0 | 0 0 0 0 0 0 0
 * | 6                | 3     | 7
 * |
 *
 * Transition Index     1111110000000000    Set by the actor engine when the door is spawned (See Actor_SpawnTransitionActors and DOOR_GET_TRANSITION_ID)
 * Type                 0000001110000000    Values of the EnDoorType enum
 * Type var             0000000001111111    Generic extra parameter. It is used differently by each door Type (or not used at all)
 *
 *
 * If type is either ENDOOR_TYPE_WHOLE_DAY, ENDOOR_TYPE_DAY or ENDOOR_TYPE_NIGHT then the Type var is bitpacked like this:
 *
 * |             |
 * | Text offset | Half day bit index
 * |-------------|--------------------
 * | 0 0 0 0     | 0 0 0
 * | 4           | 3
 * |
 *
 * Text offset          0000000001111000    Used as an offset relative to a base text id. The base text id is different depending on the door type.
 * Half day bit index   0000000000000111    Index into the D_801AED48 array
 *
 *
 * The rest of the door types either don't use the actionv var or don't bitpack extra parameters on it (using the full 7 bits)
 */

struct EnDoor;

typedef void (*EnDoorActionFunc)(struct EnDoor*, PlayState*);

typedef enum EnDoorType {
    /* 0 */ ENDOOR_TYPE_WHOLE_DAY, // Allows to specify the door may be closed for full specific days. Which days the door is closed is controlled by the "half day bit index" part of the parameter
    /* 1 */ ENDOOR_TYPE_LOCKED,
    /* 2 */ ENDOOR_TYPE_DAY, // Allows to specify the door may be closed for specific dawns. Which dawns the door is closed is controlled by the "half day bit index" part of the parameter
    /* 3 */ ENDOOR_TYPE_NIGHT, // Allows to specify the door may be closed for specific nights. Which nights the door is closed is controlled by the "half day bit index" part of the parameter
    /* 4 */ ENDOOR_TYPE_AJAR, // unused
    /* 5 */ ENDOOR_TYPE_SCHEDULE,
    /* 6 */ ENDOOR_TYPE_6, // unreferenced
    /* 7 */ ENDOOR_TYPE_FRAMED // unused
} EnDoorType;

typedef enum EnDoorScheduleType {
    /*  0 */ ENDOOR_SCH_TYPE_SWORDSMANS_SCHOOL,
    /*  1 */ ENDOOR_SCH_TYPE_POST_OFFICE,
    /*  2 */ ENDOOR_SCH_TYPE_LOTTERY_SHOP,
    /*  3 */ ENDOOR_SCH_TYPE_TRADING_POST,
    /*  4 */ ENDOOR_SCH_TYPE_CURIOSITY_SHOP,
    /*  5 */ ENDOOR_SCH_TYPE_LAUNDRY_POOL,
    /*  6 */ ENDOOR_SCH_TYPE_BOMB_SHOP,
    /*  7 */ ENDOOR_SCH_TYPE_TOWN_SHOOTING_GALLERY,
    /*  8 */ ENDOOR_SCH_TYPE_TREASURE_CHEST_SHOP,
    /*  9 */ ENDOOR_SCH_TYPE_HONEY_DARLING_SHOP,
    /* 10 */ ENDOOR_SCH_TYPE_MILK_BAR,
    /* 11 */ ENDOOR_SCH_TYPE_INN_MAIN_ENTRANCE,
    /* 12 */ ENDOOR_SCH_TYPE_INN_UPPER_ENTRANCE,
    /* 13 */ ENDOOR_SCH_TYPE_INN_GRANNYS,
    /* 14 */ ENDOOR_SCH_TYPE_INN_STAFF_ROOM,
    /* 15 */ ENDOOR_SCH_TYPE_INN_KNIFE_CHAMBER,
    /* 16 */ ENDOOR_SCH_TYPE_INN_LARGE_SUITE,
    /* 17 */ ENDOOR_SCH_TYPE_MAYORS_RESIDENCE_MAIN_ENTRANCE,
    /* 18 */ ENDOOR_SCH_TYPE_MAYORS_RESIDENCE_MAYOR_DOTOUR,
    /* 19 */ ENDOOR_SCH_TYPE_MAYORS_RESIDENCE_MADAME_AROMA,
    /* 20 */ ENDOOR_SCH_TYPE_MAYORS_RESIDENCE_BEDROOM,
    /* 21 */ ENDOOR_SCH_TYPE_21, // unused
    /* 22 */ ENDOOR_SCH_TYPE_ROMANI_RANCH_MAMAS_HOUSE,
    /* 23 */ ENDOOR_SCH_TYPE_ROMANI_RANCH_BARN,
    /* 24 */ ENDOOR_SCH_TYPE_ROMANI_RANCH_CUCCO_SHACK,
    /* 25 */ ENDOOR_SCH_TYPE_ROMANI_RANCH_DOGGY_RACETRACK,
    /* 26 */ ENDOOR_SCH_TYPE_ROMANI_RANCH_BEDROOM,
    /* 27 */ ENDOOR_SCH_TYPE_IKANA_CANYON_MUSIC_BOX_HOUSE,
    /* 28 */ ENDOOR_SCH_TYPE_DAMPES_HOUSE,
    /* 29 */ ENDOOR_SCH_TYPE_MAGIC_HAGS_POTION_SHOP,
    /* 30 */ ENDOOR_SCH_TYPE_30, // unused
    /* 31 */ ENDOOR_SCH_TYPE_SWAMP_SHOOTING_GALLERY,
    /* 32 */ ENDOOR_SCH_TYPE_MAX
} EnDoorScheduleType;

typedef enum EnDoorFramedType {
    /* 0 */ ENDOOR_FRAMED_FRAME,
    /* 1 */ ENDOOR_FRAMED_NOFRAME
} EnDoorFramedType;

#define ENDOOR_GET_TYPE(thisx) (((thisx)->params >> 7) & 7)
#define ENDOOR_GET_TYPE_VAR(thisx) (((thisx)->params) & 0x7F)

// TODO: rename after D_801AED48
#define ENDOOR_GET_HALFDAYBIT_INDEX_FROM_HALFDAYCHECK(typeVar) ((typeVar) & 0x7)
#define ENDOOR_GET_TEXTOFFSET_FROM_HALFDAYCHECK(typeVar) (((typeVar) >> 3) & 0xF)

#define ENDOOR_PACK_HALFDAYCHECK(halfDayBitIndex, textOffset) ((((textOffset) & 0xF) << 3) | ((halfDayBitIndex) & 0x7))

#define ENDOOR_PARAMS(type, typeVar) ((((type) & 7) << 7) | ((typeVar) & 0x7F))

/**
 * `scheduleType` must be a value of the `EnDoorScheduleType` enum
 */
#define ENDOOR_PARAMS_SCHEDULE(scheduleType) ENDOOR_PARAMS(ENDOOR_TYPE_SCHEDULE, scheduleType)

/**
 * `scheduleType` must be a value of the `EnDoorFramedType` enum
 */
#define ENDOOR_PARAMS_FRAMED(frameType) ENDOOR_PARAMS(ENDOOR_TYPE_FRAMED, frameType)

typedef struct EnDoor {
    /* 0x000 */ KnobDoorActor knobDoor;
    /* 0x1A4 */ u8 doorType; // EnDoorType enum
    /* 0x1A5 */ union {
                    u8 data; // generic
                    u8 halfDayCheck; // ENDOOR_TYPE_WHOLE_DAY, ENDOOR_TYPE_DAY and ENDOOR_TYPE_NIGHT
                    u8 switchFlag; // ENDOOR_TYPE_LOCKED
                    u8 schType; // ENDOOR_TYPE_SCHEDULE
                    u8 frameType; // ENDOOR_TYPE_FRAMED
                } typeVar;
    /* 0x1A6 */ u8 lockTimer; // Used by ENDOOR_TYPE_LOCKED. Also controls drawing the lock on the door and side effects of opening a locked door, like decreasing the key count and updating the switch flag
    /* 0x1A7 */ s8 openTimer; // For how long the door will be open. positive/negative means the opening direction. It is meant to be used only by ENDOOR_TYPE_SCHEDULE, and set by schedule actors. See EnDoor_OpenScheduleActor.
    /* 0x1A8 */ Vec3s limbTable[DOOR_LIMB_MAX];
    /* 0x1C8 */ EnDoorActionFunc actionFunc;
} EnDoor;

#endif // Z_EN_DOOR_H
