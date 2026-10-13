/*
 * File: z_en_pst.c
 * Overlay: ovl_En_Pst
 * Description: Postbox
 */

#include "z_en_pst.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED)

void EnPst_Init(Actor* thisx, PlayState* play);
void EnPst_Destroy(Actor* thisx, PlayState* play);
void EnPst_Update(Actor* thisx, PlayState* play);
void EnPst_Draw(Actor* thisx, PlayState* play);

void EnPst_Talk(EnPst* this, PlayState* play);

typedef enum {
    /* 0 */ POSTBOX_SCH_NONE,
    /* 1 */ POSTBOX_SCH_AVAILABLE,
    /* 2 */ POSTBOX_SCH_CHECKED_BY_POSTMAN
} PostboxScheduleResult;

typedef enum {
    /* 0 */ POSTBOX_BEHAVIOUR_WAIT_FOR_ITEM,
    /* 1 */ POSTBOX_BEHAVIOUR_TAKE_ITEM
} PostboxBehaviour;

static u8 D_80B2C200[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(9, 31, 9, 35, 0x9 - 0x6),
    /* 0x6 */ SCHEDULE_CMD_RET_VAL_L(POSTBOX_SCH_AVAILABLE),
    /* 0x9 */ SCHEDULE_CMD_RET_VAL_L(POSTBOX_SCH_CHECKED_BY_POSTMAN),
};

static u8 D_80B2C20C[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 3, 10, 7, 0x9 - 0x6),
    /* 0x6 */ SCHEDULE_CMD_RET_VAL_L(POSTBOX_SCH_AVAILABLE),
    /* 0x9 */ SCHEDULE_CMD_RET_VAL_L(POSTBOX_SCH_CHECKED_BY_POSTMAN),
};

static u8 D_80B2C218[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 35, 10, 39, 0x9 - 0x6),
    /* 0x6 */ SCHEDULE_CMD_RET_VAL_L(POSTBOX_SCH_AVAILABLE),
    /* 0x9 */ SCHEDULE_CMD_RET_VAL_L(POSTBOX_SCH_CHECKED_BY_POSTMAN),
};

static u8 D_80B2C224[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 53, 10, 57, 0x9 - 0x6),
    /* 0x6 */ SCHEDULE_CMD_RET_VAL_L(POSTBOX_SCH_AVAILABLE),
    /* 0x9 */ SCHEDULE_CMD_RET_VAL_L(POSTBOX_SCH_CHECKED_BY_POSTMAN),
};

static u8 D_80B2C230[] = {
    /* 0x0 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(11, 25, 11, 29, 0x9 - 0x6),
    /* 0x6 */ SCHEDULE_CMD_RET_VAL_L(POSTBOX_SCH_AVAILABLE),
    /* 0x9 */ SCHEDULE_CMD_RET_VAL_L(POSTBOX_SCH_CHECKED_BY_POSTMAN),
};

MsgScript D_80B2C23C[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2784),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x00FF),
    /* 0x0007 0x07 */ MSCRIPT_CMD_CHECK_CALLBACK_MULTI(0x0, 0x003F - 0x000E, 0x0047 - 0x000E),
    /* 0x000E 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x001B - 0x0011),
    /* 0x0011 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0012 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x27A4),
    /* 0x0015 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0016 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0019 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001A 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x001B 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x001C 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2787),
    /* 0x001F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0020 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0023 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0024 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_SOUTH_UPPER_CLOCKTOWN),
    /* 0x0027 0x03 */ MSCRIPT_CMD_DELETE_ITEM(ITEM_LETTER_TO_KAFEI),
    /* 0x002A 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_27_40, 0x0037 - 0x002F),
    /* 0x002F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2788),
    /* 0x0032 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x0035 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0036 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0037 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2789),
    /* 0x003A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x003D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003E 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x003F 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0040 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2785),
    /* 0x0043 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0044 0x03 */ MSCRIPT_CMD_JUMP_3(0x0019 - 0x0047),

    /* 0x0047 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2786),
    /* 0x004A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004B 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80B2C288[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2784),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x00FF),
    /* 0x0007 0x07 */ MSCRIPT_CMD_CHECK_CALLBACK_MULTI(0x0, 0x003F - 0x000E, 0x0047 - 0x000E),
    /* 0x000E 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x001B - 0x0011),
    /* 0x0011 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0012 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x27A4),
    /* 0x0015 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0016 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0019 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001A 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x001B 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x001C 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2787),
    /* 0x001F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0020 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0023 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0024 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_NORTH_CLOCKTOWN),
    /* 0x0027 0x03 */ MSCRIPT_CMD_DELETE_ITEM(ITEM_LETTER_TO_KAFEI),
    /* 0x002A 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_27_80, 0x0037 - 0x002F),
    /* 0x002F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2788),
    /* 0x0032 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x0035 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0036 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0037 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2789),
    /* 0x003A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x003D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003E 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x003F 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0040 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2785),
    /* 0x0043 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0044 0x03 */ MSCRIPT_CMD_JUMP_3(0x0019 - 0x0047),

    /* 0x0047 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2786),
    /* 0x004A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004B 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80B2C2D4[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2784),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x00FF),
    /* 0x0007 0x07 */ MSCRIPT_CMD_CHECK_CALLBACK_MULTI(0x0, 0x003F - 0x000E, 0x0047 - 0x000E),
    /* 0x000E 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x001B - 0x0011),
    /* 0x0011 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0012 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x27A4),
    /* 0x0015 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0016 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0019 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001A 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x001B 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x001C 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2787),
    /* 0x001F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0020 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0023 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0024 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_EAST_UPPER_CLOCKTOWN),
    /* 0x0027 0x03 */ MSCRIPT_CMD_DELETE_ITEM(ITEM_LETTER_TO_KAFEI),
    /* 0x002A 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_28_01, 0x0037 - 0x002F),
    /* 0x002F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2788),
    /* 0x0032 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x0035 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0036 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0037 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2789),
    /* 0x003A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x003D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003E 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x003F 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0040 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2785),
    /* 0x0043 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0044 0x03 */ MSCRIPT_CMD_JUMP_3(0x0019 - 0x0047),

    /* 0x0047 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2786),
    /* 0x004A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004B 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80B2C320[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2784),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x00FF),
    /* 0x0007 0x07 */ MSCRIPT_CMD_CHECK_CALLBACK_MULTI(0x0, 0x003F - 0x000E, 0x0047 - 0x000E),
    /* 0x000E 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x001B - 0x0011),
    /* 0x0011 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0012 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x27A4),
    /* 0x0015 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0016 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0019 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001A 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x001B 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x001C 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2787),
    /* 0x001F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0020 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0023 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0024 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_EAST_LOWER_CLOCKTOWN),
    /* 0x0027 0x03 */ MSCRIPT_CMD_DELETE_ITEM(ITEM_LETTER_TO_KAFEI),
    /* 0x002A 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_28_02, 0x0037 - 0x002F),
    /* 0x002F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2788),
    /* 0x0032 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x0035 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0036 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0037 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2789),
    /* 0x003A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x003D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003E 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x003F 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0040 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2785),
    /* 0x0043 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0044 0x03 */ MSCRIPT_CMD_JUMP_3(0x0019 - 0x0047),

    /* 0x0047 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2786),
    /* 0x004A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004B 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80B2C36C[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2784),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x00FF),
    /* 0x0007 0x07 */ MSCRIPT_CMD_CHECK_CALLBACK_MULTI(0x0, 0x003F - 0x000E, 0x0047 - 0x000E),
    /* 0x000E 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x001B - 0x0011),
    /* 0x0011 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0012 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x27A4),
    /* 0x0015 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0016 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0019 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001A 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x001B 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x001C 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2787),
    /* 0x001F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0020 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0023 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0024 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_SOUTH_LOWER_CLOCKTOWN),
    /* 0x0027 0x03 */ MSCRIPT_CMD_DELETE_ITEM(ITEM_LETTER_TO_KAFEI),
    /* 0x002A 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_28_04, 0x0037 - 0x002F),
    /* 0x002F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2788),
    /* 0x0032 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x0035 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0036 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0037 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2789),
    /* 0x003A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x003D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003E 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x003F 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0040 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2785),
    /* 0x0043 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0044 0x03 */ MSCRIPT_CMD_JUMP_3(0x0019 - 0x0047),

    /* 0x0047 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2786),
    /* 0x004A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004B 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80B2C3B8[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x27A1),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x27A2),
    /* 0x0007 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0008 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0009 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_81_08, 0x001D - 0x000E),
    /* 0x000E 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_HEART_PIECE, 0x0),
    /* 0x0013 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x000C),
    /* 0x0016 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0017 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_81_08),
    /* 0x001A 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0029 - 0x001D),
    /* 0x001D 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_RUPEE_GREEN, 0x0),
    /* 0x0022 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0001),
    /* 0x0025 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0026 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x0029 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x27A3),
    /* 0x002C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002D 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x002E 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80B2C3E8[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2787),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_SOUTH_UPPER_CLOCKTOWN),
    /* 0x0007 0x03 */ MSCRIPT_CMD_DELETE_ITEM(ITEM_LETTER_TO_KAFEI),
    /* 0x000A 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_27_40, 0x0017 - 0x000F),
    /* 0x000F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2788),
    /* 0x0012 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x0015 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0016 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0017 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2789),
    /* 0x001A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x001D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001E 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80B2C408[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2787),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_NORTH_CLOCKTOWN),
    /* 0x0007 0x03 */ MSCRIPT_CMD_DELETE_ITEM(ITEM_LETTER_TO_KAFEI),
    /* 0x000A 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_27_80, 0x0017 - 0x000F),
    /* 0x000F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2788),
    /* 0x0012 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x0015 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0016 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0017 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2789),
    /* 0x001A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x001D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001E 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80B2C428[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2787),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_EAST_UPPER_CLOCKTOWN),
    /* 0x0007 0x03 */ MSCRIPT_CMD_DELETE_ITEM(ITEM_LETTER_TO_KAFEI),
    /* 0x000A 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_28_01, 0x0017 - 0x000F),
    /* 0x000F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2788),
    /* 0x0012 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x0015 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0016 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0017 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2789),
    /* 0x001A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x001D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001E 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80B2C448[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2787),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_EAST_LOWER_CLOCKTOWN),
    /* 0x0007 0x03 */ MSCRIPT_CMD_DELETE_ITEM(ITEM_LETTER_TO_KAFEI),
    /* 0x000A 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_28_02, 0x0017 - 0x000F),
    /* 0x000F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2788),
    /* 0x0012 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x0015 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0016 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0017 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2789),
    /* 0x001A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x001D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001E 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80B2C468[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2787),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_SOUTH_LOWER_CLOCKTOWN),
    /* 0x0007 0x03 */ MSCRIPT_CMD_DELETE_ITEM(ITEM_LETTER_TO_KAFEI),
    /* 0x000A 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_28_04, 0x0017 - 0x000F),
    /* 0x000F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2788),
    /* 0x0012 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x0015 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0016 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0017 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2789),
    /* 0x001A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DEPOSITED_LETTER_TO_KAFEI),
    /* 0x001D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001E 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80B2C488[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x27A4),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80B2C490[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2785),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_DONE(),
};

ActorProfile En_Pst_Profile = {
    /**/ ACTOR_EN_PST,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_PST,
    /**/ sizeof(EnPst),
    /**/ EnPst_Init,
    /**/ EnPst_Destroy,
    /**/ EnPst_Update,
    /**/ EnPst_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT1,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 28, 72, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 1, 0, 0, 0, MASS_IMMOVABLE };

typedef enum PostboxAnimation {
    /* 0 */ POSTBOX_ANIM_IDLE,
    /* 1 */ POSTBOX_ANIM_MAX
} PostboxAnimation;

static AnimationInfoS sAnimationInfo[POSTBOX_ANIM_MAX] = {
    { &gPostboxIdleAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 }, // POSTBOX_ANIM_IDLE
};

void EnPst_UpdateCollision(EnPst* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnPst_HandleLetterDay1(EnPst* this) {
    switch (this->actor.params) {
        case POSTBOX_SOUTH_UPPER_CLOCKTOWN:
            return CHECK_WEEKEVENTREG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_SOUTH_UPPER_CLOCKTOWN);

        case POSTBOX_NORTH_CLOCKTOWN:
            return CHECK_WEEKEVENTREG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_NORTH_CLOCKTOWN);

        case POSTBOX_EAST_UPPER_CLOCKTOWN:
            return CHECK_WEEKEVENTREG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_EAST_UPPER_CLOCKTOWN);

        case POSTBOX_EAST_LOWER_CLOCKTOWN:
            return CHECK_WEEKEVENTREG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_EAST_LOWER_CLOCKTOWN);

        case POSTBOX_SOUTH_LOWER_CLOCKTOWN:
            return CHECK_WEEKEVENTREG(WEEKEVENTREG_DEPOSITED_LETTER_TO_KAFEI_SOUTH_LOWER_CLOCKTOWN);

        default:
            return false;
    }
}

s32 EnPst_HandleLetterDay2(EnPst* this) {
    switch (this->actor.params) {
        case POSTBOX_SOUTH_UPPER_CLOCKTOWN:
            return CHECK_WEEKEVENTREG(WEEKEVENTREG_27_40);

        case POSTBOX_NORTH_CLOCKTOWN:
            return CHECK_WEEKEVENTREG(WEEKEVENTREG_27_80);

        case POSTBOX_EAST_UPPER_CLOCKTOWN:
            return CHECK_WEEKEVENTREG(WEEKEVENTREG_28_01);

        case POSTBOX_EAST_LOWER_CLOCKTOWN:
            return CHECK_WEEKEVENTREG(WEEKEVENTREG_28_02);

        case POSTBOX_SOUTH_LOWER_CLOCKTOWN:
            return CHECK_WEEKEVENTREG(WEEKEVENTREG_28_04);

        default:
            return false;
    }
}

s32 EnPst_ChooseBehaviour(Actor* thisx, PlayState* play) {
    PlayerItemAction itemAction = PLAYER_IA_NONE;
    s32 scriptBranch = 0;
    EnPst* this = (EnPst*)thisx;

    switch (this->behaviour) {
        case POSTBOX_BEHAVIOUR_WAIT_FOR_ITEM:
            switch (Message_GetState(&play->msgCtx)) {
                case TEXT_STATE_CHOICE:
                case TEXT_STATE_EVENT:
                    if (Message_ShouldAdvance(play)) {
                        case TEXT_STATE_PAUSE_MENU:
                            itemAction = func_80123810(play);
                            scriptBranch = 0;
                            if ((itemAction == PLAYER_IA_LETTER_TO_KAFEI) || (itemAction == PLAYER_IA_LETTER_MAMA)) {
                                this->exchangeItemAction = itemAction;
                                this->behaviour++;
                                scriptBranch = 1;
                            } else if (itemAction <= PLAYER_IA_MINUS1) {
                                this->behaviour++;
                                scriptBranch = 3;
                            } else if (itemAction != PLAYER_IA_NONE) {
                                this->behaviour++;
                                scriptBranch = 2;
                            }
                    }
                    break;

                default:
                    break;
            }
            break;

        case POSTBOX_BEHAVIOUR_TAKE_ITEM:
            if (this->exchangeItemAction == PLAYER_IA_LETTER_TO_KAFEI) {
                scriptBranch = 1;
            }
            break;

        default:
            break;
    }
    return scriptBranch;
}

MsgScript* EnPst_GetMsgScript(EnPst* this, PlayState* play) {
    if (Player_GetMask(play) == PLAYER_MASK_POSTMAN) {
        return D_80B2C3B8;
    }

    if (this->stateFlags & 0x10) {
        switch (this->actor.params) {
            case POSTBOX_SOUTH_UPPER_CLOCKTOWN:
                return D_80B2C3E8;

            case POSTBOX_NORTH_CLOCKTOWN:
                return D_80B2C408;

            case POSTBOX_EAST_UPPER_CLOCKTOWN:
                return D_80B2C428;

            case POSTBOX_EAST_LOWER_CLOCKTOWN:
                return D_80B2C448;

            case POSTBOX_SOUTH_LOWER_CLOCKTOWN:
                return D_80B2C468;

            default:
                return NULL;
        }
    }

    if (this->stateFlags & 0x20) {
        if (this->exchangeItemAction == PLAYER_IA_LETTER_MAMA) {
            return D_80B2C488;
        } else {
            return D_80B2C490;
        }
    }

    this->msgScriptCallback = EnPst_ChooseBehaviour;

    switch (this->actor.params) {
        case POSTBOX_SOUTH_UPPER_CLOCKTOWN:
            return D_80B2C23C;

        case POSTBOX_NORTH_CLOCKTOWN:
            return D_80B2C288;

        case POSTBOX_EAST_UPPER_CLOCKTOWN:
            return D_80B2C2D4;

        case POSTBOX_EAST_LOWER_CLOCKTOWN:
            return D_80B2C320;

        case POSTBOX_SOUTH_LOWER_CLOCKTOWN:
            return D_80B2C36C;

        default:
            return NULL;
    }
}

s32 EnPst_CheckTalk(EnPst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 ret = false;

    if (((this->stateFlags & SUBS_OFFER_MODE_MASK) != SUBS_OFFER_MODE_NONE) &&
        Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->stateFlags &= ~0x30;
        if (player->exchangeItemAction == PLAYER_IA_LETTER_TO_KAFEI) {
            this->stateFlags |= 0x10;
            this->exchangeItemAction = player->exchangeItemAction;
        } else if (player->exchangeItemAction != PLAYER_IA_NONE) {
            this->stateFlags |= 0x20;
            this->exchangeItemAction = player->exchangeItemAction;
        }
        // If Letter to Kafei is deposited, value is set to 2
        this->isLetterToKafeiDeposited = EnPst_HandleLetterDay1(this);
        SubS_SetOfferMode(&this->stateFlags, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
        this->behaviour = 0;
        this->msgScriptCallback = NULL;
        this->stateFlags |= 0x40;
        this->msgScript = EnPst_GetMsgScript(this, play);
        this->actionFunc = EnPst_Talk;
        ret = true;
    }
    return ret;
}

s32 EnPst_SetOfferItemModeOnScreen(EnPst* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    SubS_SetOfferMode(&this->stateFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    return true;
}

s32 EnPst_ProcessScheduleOutput(EnPst* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    s32 ret = false;

    this->stateFlags = 0;

    switch (scheduleOutput->result) {
        case POSTBOX_SCH_AVAILABLE:
            ret = EnPst_SetOfferItemModeOnScreen(this, play, scheduleOutput);
            break;

        case POSTBOX_SCH_CHECKED_BY_POSTMAN:
            ret = true;
            break;

        default:
            break;
    }
    return ret;
}

/* The postbox does not have any necessary custom logic when following their schedule,
 * instead it is all done in the process step. This is why this function is empty
 */
void EnPst_HandleSchedule(EnPst* this, PlayState* play) {
}

void EnPst_FollowSchedule(EnPst* this, PlayState* play) {
    static u8* sScheduleScripts[] = {
        D_80B2C200, D_80B2C20C, D_80B2C218, D_80B2C224, D_80B2C230,
    };
    s16 params = this->actor.params;
    ScheduleOutput scheduleOutput;

    if (!Schedule_RunScript(play, sScheduleScripts[params], &scheduleOutput) ||
        ((this->scheduleResult != scheduleOutput.result) &&
         !EnPst_ProcessScheduleOutput(this, play, &scheduleOutput))) {
        this->actor.shape.shadowDraw = NULL;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        scheduleOutput.result = POSTBOX_SCH_NONE;
    } else {
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
    this->scheduleResult = scheduleOutput.result;
    EnPst_HandleSchedule(this, play);
}

void EnPst_Talk(EnPst* this, PlayState* play) {
    if (MsgEvent_RunScript(&this->actor, play, this->msgScript, this->msgScriptCallback, &this->msgScriptPos)) {
        if (EnPst_HandleLetterDay1(this) != this->isLetterToKafeiDeposited) {
            switch (gSaveContext.save.day) {
                case 1:
                    SET_WEEKEVENTREG(WEEKEVENTREG_91_04);
                    break;

                case 2:
                    if (EnPst_HandleLetterDay2(this)) {
                        SET_WEEKEVENTREG(WEEKEVENTREG_91_08);
                    } else {
                        SET_WEEKEVENTREG(WEEKEVENTREG_91_04);
                    }
                    break;

                default:
                    SET_WEEKEVENTREG(WEEKEVENTREG_91_08);
                    break;
            }
        }
        SubS_SetOfferMode(&this->stateFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->msgScriptPos = 0;
        this->actionFunc = EnPst_FollowSchedule;
    }
}

void EnPst_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPst* this = (EnPst*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 18.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gPostboxSkel, NULL, this->jointTable, this->morphTable,
                       POSTBOX_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);
    SubS_SetOfferMode(&this->stateFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, POSTBOX_ANIM_IDLE);
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    Actor_SetScale(&this->actor, 0.02f);
    this->actionFunc = EnPst_FollowSchedule;
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
}

void EnPst_Destroy(Actor* thisx, PlayState* play) {
    EnPst* this = (EnPst*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnPst_Update(Actor* thisx, PlayState* play) {
    EnPst* this = (EnPst*)thisx;

    EnPst_CheckTalk(this, play);
    this->actionFunc(this, play);
    if (this->scheduleResult != POSTBOX_SCH_NONE) {
        if (Actor_IsFacingPlayer(&this->actor, 0x1FFE)) {
            this->unk214 = 0;
            SubS_Offer(&this->actor, play, 60.0f, 20.0f, PLAYER_IA_NONE, this->stateFlags & SUBS_OFFER_MODE_MASK);
        }
        Actor_SetFocus(&this->actor, 20.0f);
        EnPst_UpdateCollision(this, play);
    }
}

s32 EnPst_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnPst* this = (EnPst*)thisx;
    f32 yTranslation;

    if (limbIndex == POSTBOX_LIMB_MAIL_SLOT) {
        if (this->stateFlags & 0x40) {
            yTranslation = -100.0f;
        } else {
            yTranslation = 0.0f;
        }
        Matrix_Translate(0.0f, yTranslation, 0.0f, MTXMODE_APPLY);
    }
    return false;
}

void EnPst_Draw(Actor* thisx, PlayState* play) {
    EnPst* this = (EnPst*)thisx;

    if (this->scheduleResult != POSTBOX_SCH_NONE) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              EnPst_OverrideLimbDraw, NULL, &this->actor);
    }
}
