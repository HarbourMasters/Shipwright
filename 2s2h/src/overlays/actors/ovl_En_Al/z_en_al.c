/*
 * File: z_en_al.c
 * Overlay: ovl_En_Al
 * Description: Madame Aroma
 */

#include "z_en_al.h"

#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnAl_Init(Actor* thisx, PlayState* play);
void EnAl_Destroy(Actor* thisx, PlayState* play);
void EnAl_Update(Actor* thisx, PlayState* play);
void EnAl_Draw(Actor* thisx, PlayState* play);

void func_80BDF6C4(EnAl* this, PlayState* play);

static u8 D_80BDFC70[] = {
    /* 0x00 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(3, 0x11 - 0x04),
    /* 0x04 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(18, 0, 6, 0, 0x0B - 0x0A),
    /* 0x0A */ SCHEDULE_CMD_RET_NONE(),
    /* 0x0B */ SCHEDULE_CMD_RET_TIME(18, 0, 6, 0, 2),
    /* 0x11 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(8, 0, 10, 0, 0x46 - 0x17),
    /* 0x17 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 0, 10, 55, 0x40 - 0x1D),
    /* 0x1D */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 55, 11, 45, 0x30 - 0x23),
    /* 0x23 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(11, 45, 20, 0, 0x2A - 0x29),
    /* 0x29 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x2A */ SCHEDULE_CMD_RET_TIME(11, 45, 20, 0, 1),
    /* 0x30 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(2, 0x3A - 0x34),
    /* 0x34 */ SCHEDULE_CMD_RET_TIME(10, 55, 11, 45, 1),
    /* 0x3A */ SCHEDULE_CMD_RET_TIME(10, 55, 11, 45, 3),
    /* 0x40 */ SCHEDULE_CMD_RET_TIME(10, 0, 10, 55, 1),
    /* 0x46 */ SCHEDULE_CMD_RET_VAL_L(1),
};

MsgScript D_80BDFCBC[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0003 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x27A5),
    /* 0x0007 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0008 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x000B 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x000C 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x27A6),
    /* 0x000F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0010 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0013 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0014 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x27A7),
    /* 0x0017 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0018 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x001B 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x001C 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x27A8),
    /* 0x001F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0020 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0023 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0024 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x27A9),
    /* 0x0027 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0028 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x002B 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x002C 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x27AA),
    /* 0x002F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0030 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0033 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0034 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x27AB),
    /* 0x0037 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0038 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x003B 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x003C 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x27AC),
    /* 0x003F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0040 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0043 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0044 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x27AD),
    /* 0x0047 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_POSTMAN),
    /* 0x004A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x004D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004E 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_89_08),
    /* 0x0051 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0054 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80BDFD14[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_86_20, 0x00A7 - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0008 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0009 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AA6),
    /* 0x000C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000D 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0010 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0011 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AA7),
    /* 0x0014 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0015 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0018 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0019 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AA8),
    /* 0x001C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001D 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0020 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0021 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AA9),
    /* 0x0024 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0025 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0028 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0029 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AAA),
    /* 0x002C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002D 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0030 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0031 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AAB),
    /* 0x0034 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0035 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0038 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0039 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AAC),
    /* 0x003C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003D 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0040 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0041 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AAD),
    /* 0x0044 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0045 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2AAE),
    /* 0x0048 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0049 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x004C 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x004D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AAF),
    /* 0x0050 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0051 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0054 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0055 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB0),
    /* 0x0058 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0059 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x005C 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x005D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB1),
    /* 0x0060 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0061 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0064 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0065 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB2),
    /* 0x0068 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0069 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x006C 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x006D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB3),
    /* 0x0070 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0071 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0074 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0075 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB4),
    /* 0x0078 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0079 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x007C 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x007D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB5),
    /* 0x0080 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0081 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0084 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0085 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB6),
    /* 0x0088 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0089 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x008C 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x008D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB7),
    /* 0x0090 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0091 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0094 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0095 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB8),
    /* 0x0098 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x009B 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_TOTO),
    /* 0x009E 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_GORMAN),
    /* 0x00A1 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00A2 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_86_20),
    /* 0x00A5 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00A6 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x00A7 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x00AA 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x00AB 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AEA),
    /* 0x00AE 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00AF 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x00B2 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x00B3 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AEB),
    /* 0x00B6 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00B7 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x00BA 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00BB 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80BDFDD0[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_92_08, 0x0010 - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AE5),
    /* 0x0008 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x000B 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_92_08),
    /* 0x000E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000F 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0010 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AE6),
    /* 0x0013 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x0016 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0017 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80BDFDE8[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0003 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B19),
    /* 0x0006 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x0009 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000A 0x01 */ MSCRIPT_CMD_PAUSE(),
    /* 0x000B 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x000E 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x000F 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80BDFDF8[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0003 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_57_04, 0x0079 - 0x0008),
    /* 0x0008 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B1A),
    /* 0x000B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000C 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x00FF),
    /* 0x000F 0x09 */ MSCRIPT_CMD_CHECK_ITEM_ACTION(PLAYER_IA_LETTER_MAMA, 0x0030 - 0x0018, 0x0025 - 0x0018, 0x0),
    /* 0x0018 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B1B),
    /* 0x001B 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x001E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001F 0x01 */ MSCRIPT_CMD_PAUSE(),
    /* 0x0020 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0023 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0024 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0025 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2B1C),
    /* 0x0028 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0029 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x002C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002D 0x03 */ MSCRIPT_CMD_JUMP(0x000C - 0x0030),

    /* 0x0030 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2B1D),
    /* 0x0033 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0034 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0037 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0038 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2B1E),
    /* 0x003B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003C 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2B1F),
    /* 0x003F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0040 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0041 0x03 */ MSCRIPT_CMD_DELETE_ITEM(ITEM_LETTER_MAMA),
    /* 0x0044 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_57_08, 0x005C - 0x0049),
    /* 0x0049 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_CHATEAU_BOTTLE, 0x0),
    /* 0x004E 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x006F),
    /* 0x0051 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0054 0x01 */ MSCRIPT_CMD_AWAIT_TEXT_DONE(),
    /* 0x0055 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DELIVERED_PRIORITY_MAIL),
    /* 0x0058 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0059 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0068 - 0x005C),
    /* 0x005C 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_RUPEE_PURPLE, 0x0),
    /* 0x0061 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0005),
    /* 0x0064 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0065 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x0068 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B20),
    /* 0x006B 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x006E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x006F 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x0070 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_57_04),
    /* 0x0073 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_57_08),
    /* 0x0076 0x03 */ MSCRIPT_CMD_JUMP(0x001F - 0x0079),

    /* 0x0079 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B3C),
    /* 0x007C 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x007F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0080 0x03 */ MSCRIPT_CMD_JUMP(0x001F - 0x0083),
};

MsgScript D_80BDFE7C[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2A9C),
    /* 0x0003 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x0006 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0007 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80BDFE84[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2A9D),
    /* 0x0003 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x0006 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0007 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80BDFE8C[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_57_02, 0x0014 - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2A9E),
    /* 0x0008 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0009 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2A9F),
    /* 0x000C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000D 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2AA0),
    /* 0x0010 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0011 0x03 */ MSCRIPT_CMD_JUMP(0x0018 - 0x0014),
    /* 0x0014 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AA0),
    /* 0x0017 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0018 0x07 */ MSCRIPT_CMD_CHECK_TEXT_CHOICE(0x0, 0x0039 - 0x001F, 0x0039 - 0x001F),
    /* 0x001F 0x01 */ MSCRIPT_CMD_PLAY_DECIDE(),
    /* 0x0020 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AA2),
    /* 0x0023 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0024 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0025 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_MASK_KAFEIS_MASK, 0x0),
    /* 0x002A 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x008F),
    /* 0x002D 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0030 0x01 */ MSCRIPT_CMD_AWAIT_TEXT_DONE(),
    /* 0x0031 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_RECEIVED_KAFEIS_MASK),
    /* 0x0034 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x0037 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0038 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0039 0x01 */ MSCRIPT_CMD_PLAY_CANCEL(),
    /* 0x003A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AA1),
    /* 0x003D 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x0040 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0041 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_57_02),
    /* 0x0044 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80BDFED4[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0003 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2B1D),
    /* 0x0006 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0007 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x000A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000B 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2B1E),
    /* 0x000E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2B1F),
    /* 0x0012 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0013 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0014 0x03 */ MSCRIPT_CMD_DELETE_ITEM(ITEM_LETTER_MAMA),
    /* 0x0017 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_57_08, 0x002F - 0x001C),
    /* 0x001C 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_CHATEAU_BOTTLE, 0x0),
    /* 0x0021 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x006F),
    /* 0x0024 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0027 0x01 */ MSCRIPT_CMD_AWAIT_TEXT_DONE(),
    /* 0x0028 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DELIVERED_PRIORITY_MAIL),
    /* 0x002B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002C 0x03 */ MSCRIPT_CMD_AUTOTALK(0x003B - 0x002F),
    /* 0x002F 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_RUPEE_PURPLE, 0x0),
    /* 0x0034 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0005),
    /* 0x0037 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0038 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x003B 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B20),
    /* 0x003E 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x0041 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0042 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x0043 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_57_04),
    /* 0x0046 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_57_08),
    /* 0x0049 0x01 */ MSCRIPT_CMD_PAUSE(),
    /* 0x004A 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x004D 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x004E 0x01 */ MSCRIPT_CMD_DONE(),
};

MsgScript D_80BDFF24[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0003 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2B1C),
    /* 0x0006 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0007 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x00FF),
    /* 0x000A 0x09 */ MSCRIPT_CMD_CHECK_ITEM_ACTION(PLAYER_IA_LETTER_MAMA, 0x0020 - 0x0013, 0x0003 - 0x0013, 0x0),
    /* 0x0013 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B1B),
    /* 0x0016 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x0019 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001A 0x01 */ MSCRIPT_CMD_PAUSE(),
    /* 0x001B 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x001E 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x001F 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0020 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2B1D),
    /* 0x0023 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0024 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0027 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0028 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2B1E),
    /* 0x002B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002C 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2B1F),
    /* 0x002F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0030 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0031 0x03 */ MSCRIPT_CMD_DELETE_ITEM(ITEM_LETTER_MAMA),
    /* 0x0034 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_57_08, 0x004C - 0x0039),
    /* 0x0039 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_CHATEAU_BOTTLE, 0x0),
    /* 0x003E 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x006F),
    /* 0x0041 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0044 0x01 */ MSCRIPT_CMD_AWAIT_TEXT_DONE(),
    /* 0x0045 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_DELIVERED_PRIORITY_MAIL),
    /* 0x0048 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0049 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0058 - 0x004C),
    /* 0x004C 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_RUPEE_PURPLE, 0x0),
    /* 0x0051 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0005),
    /* 0x0054 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0055 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x0058 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B20),
    /* 0x005B 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x005E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x005F 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x0060 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_57_04),
    /* 0x0063 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_57_08),
    /* 0x0066 0x03 */ MSCRIPT_CMD_JUMP(0x001A - 0x0069),

    // Unreachable
    /* 0x0069 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B3C),
    /* 0x006C 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x006F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0070 0x03 */ MSCRIPT_CMD_JUMP(0x001A - 0x0073),
};

ActorProfile En_Al_Profile = {
    /**/ ACTOR_EN_AL,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_AL,
    /**/ sizeof(EnAl),
    /**/ EnAl_Init,
    /**/ EnAl_Destroy,
    /**/ EnAl_Update,
    /**/ EnAl_Draw,
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
    { 14, 62, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /* -1 */ ENAL_ANIM_NONE = -1,
    /*  0 */ ENAL_ANIM_0,
    /*  1 */ ENAL_ANIM_1,
    /*  2 */ ENAL_ANIM_2,
    /*  3 */ ENAL_ANIM_3,
    /*  4 */ ENAL_ANIM_4,
    /*  5 */ ENAL_ANIM_5,
    /*  6 */ ENAL_ANIM_6,
    /*  7 */ ENAL_ANIM_7,
    /*  8 */ ENAL_ANIM_MAX
} EnAlAnimation;

static AnimationInfoS sAnimationInfo[ENAL_ANIM_MAX] = {
    { &object_al_Anim_000C54, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENAL_ANIM_0
    { &object_al_Anim_00DBE0, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENAL_ANIM_1
    { &object_al_Anim_00DBE0, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENAL_ANIM_2
    { &object_al_Anim_00ACA0, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },  // ENAL_ANIM_3
    { &object_al_Anim_00ACA0, -1.0f, 0, -1, ANIMMODE_ONCE, 0 }, // ENAL_ANIM_4
    { &object_al_Anim_00CA28, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENAL_ANIM_5
    { &object_al_Anim_00BCA4, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },  // ENAL_ANIM_6
    { &object_al_Anim_00A764, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENAL_ANIM_7
};

Vec3f D_80BE0070 = { 1000.0f, 0.0f, 0.0f };

Gfx* D_80BE007C[] = {
    gMadameAromaShawlMiddleDL,           gMadameAromaShawlUpperDL,
    gMadameAromaShawlLeftLowerMiddleDL,  gMadameAromaShawlLeftLowerDL,
    gMadameAromaShawlRightLowerMiddleDL, gMadameAromaShawlRightLowerDL,
};

Actor* EnAl_FindActor(EnAl* this, PlayState* play, u8 actorCategory, s16 actorId) {
    Actor* actorIter = NULL;

    while (true) {
        actorIter = SubS_FindActor(play, actorIter, actorCategory, actorId);

        if (actorIter == NULL) {
            break;
        }

        if ((this != (EnAl*)actorIter) && (actorIter->update != NULL)) {
            break;
        }

        if (actorIter->next == NULL) {
            actorIter = NULL;
            break;
        }
        actorIter = actorIter->next;
    }
    return actorIter;
}

void EnAl_UpdateSkelAnime(EnAl* this) {
    this->skelAnime.playSpeed = this->animPlaySpeed;
    SkelAnime_Update(&this->skelAnime);
}

s32 EnAl_ChangeAnim(EnAl* this, s32 animIndex) {
    s32 changeAnim = false;
    s32 didAnimChange = false;

    if ((animIndex == ENAL_ANIM_1) || (animIndex == ENAL_ANIM_2)) {
        if ((this->animIndex != ENAL_ANIM_1) && (this->animIndex != ENAL_ANIM_2)) {
            changeAnim = true;
        }
    } else if (this->animIndex != animIndex) {
        changeAnim = true;
    }

    if (changeAnim) {
        this->animIndex = animIndex;
        didAnimChange = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
        this->animPlaySpeed = this->skelAnime.playSpeed;
    }

    return didAnimChange;
}

void func_80BDE318(EnAl* this, PlayState* play) {
    s32 pad;
    f32 temp;

    Collider_UpdateCylinder(&this->actor, &this->unk_310);
    temp = this->actor.focus.pos.y - this->actor.world.pos.y;
    this->unk_310.dim.height = temp;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->unk_310.base);
}

Actor* func_80BDE384(EnAl* this, PlayState* play) {
    Actor* actor;

    switch (this->scheduleResult) {
        case 2:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_89_08) && CHECK_WEEKEVENTREG(WEEKEVENTREG_85_80)) {
                actor = EnAl_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_PM);
            } else {
                actor = &GET_PLAYER(play)->actor;
            }
            break;

        case 3:
            actor = this->unk_368;
            break;

        default:
            actor = &GET_PLAYER(play)->actor;
            break;
    }

    return actor;
}

s32 func_80BDE408(EnAl* this, s16 csId) {
    s32 ret = false;

    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        CutsceneManager_Queue(csId);
    } else if (CutsceneManager_IsNext(csId)) {
        CutsceneManager_StartWithPlayerCs(csId, &this->actor);
        ret = true;
    } else {
        CutsceneManager_Queue(csId);
    }
    return ret;
}

s16 func_80BDE484(EnAl* this, s32 numCutscenes) {
    s16 csId = this->actor.csId;
    s32 i;

    for (i = 0; i < numCutscenes; i++) {
        csId = CutsceneManager_GetAdditionalCsId(csId);
    }

    return csId;
}

s32 func_80BDE4E0(EnAl* this, s16* arg1, s16 arg2) {
    s32 ret = false;
    s16 sp42;
    Vec3f sp34;
    Vec3f sp28;
    s32 temp;

    if ((this->unk_368 == NULL) || (this->unk_368->update == NULL)) {
        return true;
    }

    if (arg2 == *arg1) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_CHAIR_ROLL);
        EnAl_ChangeAnim(this, ENAL_ANIM_3);
        this->unk_4E8 = 0;
        (*arg1)++;
    }

    if ((arg2 + 1) == *arg1) {
        Math_Vec3f_Copy(&sp28, &this->unk_368->world.pos);
        Math_Vec3f_Copy(&sp34, &this->actor.world.pos);
        sp42 = Math_Vec3f_Yaw(&sp34, &sp28);
        temp = sp42 / 364;
        this->unk_4E8++;
        if ((temp != (this->actor.world.rot.y / 364)) && (this->unk_4E8 < 20)) {
            Math_ApproachS(&this->actor.world.rot.y, sp42, 3, 0x2AA8);
        } else {
            EnAl_ChangeAnim(this, ENAL_ANIM_5);
            this->actor.world.rot.y = sp42;
            this->unk_4E8 = 0;
            (*arg1)++;
            ret = true;
        }
    }
    return ret;
}

s32 func_80BDE678(EnAl* this, s16* arg1, s16 arg2) {
    s32 ret = false;
    s16 sp22;
    s32 temp;

    if ((this->unk_368 == NULL) || (this->unk_368->update == NULL)) {
        return true;
    }

    if (arg2 == *arg1) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_CHAIR_ROLL);
        EnAl_ChangeAnim(this, ENAL_ANIM_4);
        this->unk_4E8 = 0;
        (*arg1)++;
    }

    if ((arg2 + 1) == *arg1) {
        sp22 = this->actor.home.rot.y;
        temp = sp22 / 364;
        this->unk_4E8++;
        if ((temp != (this->actor.world.rot.y / 364)) && (this->unk_4E8 < 20)) {
            Math_ApproachS(&this->actor.world.rot.y, sp22, 3, 0x2AA8);
        } else {
            EnAl_ChangeAnim(this, ENAL_ANIM_2);
            this->actor.world.rot.y = sp22;
            this->unk_4E8 = 0;
            (*arg1)++;
            ret = true;
        }
    }
    return ret;
}

s32 func_80BDE7FC(Actor* thisx, PlayState* play) {
    EnAl* this = (EnAl*)thisx;
    s16 csId = func_80BDE484(this, 0);
    s32 pad2;
    s32 sp20 = false;

    switch (this->unk_4E6) {
        case 0:
            if (!func_80BDE408(this, csId)) {
                break;
            }

        case 2:
        case 4:
        case 6:
        case 8:
            if ((this->actor.child != NULL) && (this->actor.child->update != NULL)) {
                Camera_SetTargetActor(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(csId)),
                                      this->actor.child);
            }
            this->unk_4E6++;
            sp20 = true;
            break;

        case 1:
        case 3:
        case 5:
        case 7:
            Camera_SetTargetActor(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(csId)), &this->actor);
            this->unk_4E6++;
            sp20 = true;
            break;

        case 9:
            CutsceneManager_Stop(csId);
            this->unk_4E6++;
            sp20 = true;
            break;

        default:
            break;
    }
    return sp20;
}

s32 func_80BDE92C(Actor* thisx, PlayState* play) {
    s32 pad;
    EnAl* this = (EnAl*)thisx;
    Actor* sp1C = EnAl_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_GM);
    Actor* temp_v0 = EnAl_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_TOTO);

    if ((sp1C == NULL) || (sp1C->update == NULL) || (temp_v0 == NULL) || (temp_v0->update == NULL)) {
        this->unk_4E6++;
        return true;
    }

    switch (this->unk_4E6) {
        case 0:
        case 1:
        case 3:
        case 5:
        case 8:
        case 10:
        case 11:
        case 13:
        case 15:
        case 17:
            this->actor.child = sp1C;
            this->unk_4E6++;
            break;

        case 7:
        case 9:
        case 12:
            this->actor.child = temp_v0;
            this->unk_4E6++;
            break;

        case 2:
        case 4:
        case 6:
        case 14:
        case 16:
            this->unk_4E6++;
            break;

        default:
            this->unk_4E6++;
            break;
    }
    return true;
}

s32 func_80BDEA14(Actor* thisx, PlayState* play) {
    EnAl* this = (EnAl*)thisx;
    s32 sp18 = false;

    switch (this->unk_4E6) {
        case 0:
        case 1:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_75_02)) {
                sp18 = true;
            } else if (func_80BDE4E0(this, &this->unk_4E6, 0)) {
                sp18 = true;
            }
            break;

        case 2:
        case 3:
            if (func_80BDE678(this, &this->unk_4E6, 2)) {
                sp18 = true;
            }
            break;

        default:
            break;
    }
    return sp18;
}

MsgScript* EnAl_GetMsgScript(EnAl* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->scheduleResult == 3) {
        this->msgScriptCallback = func_80BDE92C;
        return D_80BDFD14;
    }

    switch (this->scheduleResult) {
        case 1:
            if (player->transformation == PLAYER_FORM_DEKU) {
                return D_80BDFDD0;
            }

            if (GameInteractor_Should(VB_MADAME_AROMA_ASK_FOR_HELP,
                                      INV_CONTENT(ITEM_MASK_KAFEIS_MASK) != ITEM_MASK_KAFEIS_MASK, this)) {
                return D_80BDFE8C;
            }

            if (Player_GetMask(play) == PLAYER_MASK_KAFEIS_MASK) {
                return D_80BDFE7C;
            }
            return D_80BDFE84;

        case 2:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_89_08) && CHECK_WEEKEVENTREG(WEEKEVENTREG_85_80)) {
                this->msgScriptCallback = func_80BDE7FC;
                return D_80BDFCBC;
            }

            this->msgScriptCallback = func_80BDEA14;
            if (Player_GetMask(play) != PLAYER_MASK_KAFEIS_MASK) {
                return D_80BDFDE8;
            }

            if (this->unk_4C2 & 0x800) {
                return D_80BDFED4;
            }

            if (this->unk_4C2 & 0x1000) {
                return D_80BDFF24;
            }
            return D_80BDFDF8;

        default:
            return NULL;
    }
}

s32 func_80BDEC2C(EnAl* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 ret = false;

    if (((this->unk_4C2 & SUBS_OFFER_MODE_MASK) != SUBS_OFFER_MODE_NONE) &&
        Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_4C2 &= ~0x1800;
        if (player->exchangeItemAction == PLAYER_IA_LETTER_MAMA) {
            this->unk_4C2 |= 0x800;
            this->unk_4F4 = player->exchangeItemAction;
        } else if (player->exchangeItemAction != PLAYER_IA_NONE) {
            this->unk_4C2 |= 0x1000;
            this->unk_4F4 = player->exchangeItemAction;
        }
        SubS_SetOfferMode(&this->unk_4C2, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
        this->unk_4E6 = 0;
        this->msgScriptCallback = NULL;
        this->actor.child = this->unk_368;
        this->msgScript = EnAl_GetMsgScript(this, play);
        this->unk_4C2 |= 0x20;
        this->actionFunc = func_80BDF6C4;
        ret = true;
    }

    return ret;
}

void func_80BDED20(EnAl* this) {
    s32 pad;
    Vec3f sp40;
    Vec3f sp34;
    s32 pad2;

    Math_Vec3f_Copy(&sp40, &this->unk_368->world.pos);
    Math_Vec3f_Copy(&sp34, &this->actor.world.pos);

    Math_ApproachS(&this->unk_4DE, Math_Vec3f_Yaw(&sp34, &sp40) - this->actor.shape.rot.y, 4, 0x2AA8);
    this->unk_4DE = CLAMP(this->unk_4DE, -0x1FFE, 0x1FFE);

    Math_Vec3f_Copy(&sp34, &this->actor.focus.pos);

    if (this->unk_368->id == ACTOR_PLAYER) {
        sp40.y = ((Player*)this->unk_368)->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    } else {
        Math_Vec3f_Copy(&sp40, &this->unk_368->focus.pos);
    }

    Math_ApproachS(&this->unk_4DC, Math_Vec3f_Pitch(&sp34, &sp40), 4, 0x2AA8);
    this->unk_4DC = CLAMP(this->unk_4DC, -0x1554, 0x1554);
}

void func_80BDEE5C(EnAl* this) {
    if (this->unk_4C2 & 0x20) {
        if ((this->unk_368 != NULL) && (this->unk_368->update != NULL)) {
            if (DECR(this->unk_4E2) == 0) {
                func_80BDED20(this);
                this->unk_4C2 &= ~0x200;
                this->unk_4C2 |= 0x80;
                return;
            }
        }
    }

    if (this->unk_4C2 & 0x80) {
        this->unk_4C2 &= ~0x80;
        this->unk_4DC = 0;
        this->unk_4DE = 0;
        this->unk_4E2 = 0x14;
    } else if (DECR(this->unk_4E2) == 0) {
        this->unk_4C2 |= 0x200;
    }
}

void func_80BDEF3C(EnAl* this, PlayState* play) {
    if (this->unk_4E4 == 0) {
        EnAl_ChangeAnim(this, ENAL_ANIM_7);
        this->unk_4C2 &= ~0x20;
        this->unk_4C2 |= 0x200;
        this->unk_4E4++;
    } else if ((this->unk_4E4 == 1) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        EnAl_ChangeAnim(this, ENAL_ANIM_0);
        this->unk_4C2 &= ~0x200;
        this->unk_4C2 |= 0x20;
        this->unk_4E4++;
    }
}

void func_80BDEFE4(EnAl* this, PlayState* play) {
    if (this->unk_4E4 == 0) {
        EnAl_ChangeAnim(this, ENAL_ANIM_6);
        this->unk_4E4 += 1;
    } else if ((this->unk_4E4 == 1) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        EnAl_ChangeAnim(this, ENAL_ANIM_5);
        this->unk_4E4 += 1;
    }
}

s32 func_80BDF064(EnAl* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u16 sp22 = play->msgCtx.currentTextId;
    Actor* sp1C = EnAl_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_GM);
    Actor* temp_v0 = EnAl_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_TOTO);

    if (player->stateFlags1 & PLAYER_STATE1_TALKING) {
        this->unk_4C2 |= 0x400;
        if (this->unk_4C4 != sp22) {
            switch (sp22) {
                case 0x2AA6:
                case 0x2AAF:
                case 0x2AB4:
                    if ((sp1C != NULL) && (sp1C->update != NULL)) {
                        this->unk_368 = sp1C;
                    }
                    break;

                case 0x2AAD:
                case 0x2AB0:
                    if ((temp_v0 != NULL) && (temp_v0->update != NULL)) {
                        this->unk_368 = temp_v0;
                    }
                    break;
            }

            switch (sp22) {
                case 0x2AA1:
                case 0x2AA2:
                case 0x2AA7:
                case 0x2AE6:
                case 0x2AE8:
                case 0x2AE9:
                case 0x2AB4:
                    this->unk_18C = func_80BDEF3C;
                    this->unk_4E4 = 0;
                    break;

                case 0x27A6:
                case 0x27A8:
                case 0x27AA:
                case 0x2B1E:
                    this->unk_18C = func_80BDEFE4;
                    this->unk_4E4 = 0;
                    break;

                case 0x2B19:
                case 0x2B20:
                case 0x2B3C:
                    EnAl_ChangeAnim(this, ENAL_ANIM_5);
                    break;

                default:
                    break;
            }
        }
        this->unk_4C4 = sp22;
    } else if (this->unk_4C2 & 0x400) {
        this->unk_4C4 = 0;
        this->unk_4C2 &= ~0x400;
    }

    if (this->unk_18C != NULL) {
        this->unk_18C(this, play);
    }

    return false;
}

s32 func_80BDF244(EnAl* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    s32 ret = false;
    Actor* sp20 = EnAl_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_GM);
    Actor* temp_v0 = EnAl_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_TOTO);

    if ((sp20 != NULL) && (sp20->update != NULL) && (temp_v0 != NULL) && (temp_v0->update != NULL)) {
        EnAl_ChangeAnim(this, ENAL_ANIM_0);
        SubS_SetOfferMode(&this->unk_4C2, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk_368 = sp20;
        this->unk_4C2 |= 0x20;
        ret = true;
    }
    return ret;
}

s32 func_80BDF308(EnAl* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    SubS_SetOfferMode(&this->unk_4C2, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);

    switch (scheduleOutput->result) {
        case 1:
            EnAl_ChangeAnim(this, ENAL_ANIM_0);
            break;

        case 2:
            this->unk_4F0 = PLAYER_IA_NONE;
            this->unk_4EA = 0;
            EnAl_ChangeAnim(this, ENAL_ANIM_2);
            break;

        default:
            break;
    }
    return true;
}

s32 func_80BDF390(EnAl* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    s32 ret;

    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->unk_4F0 = PLAYER_IA_NONE;
    this->unk_4C2 = 0;
    this->unk_4D4 = 40.0f;

    switch (scheduleOutput->result) {
        case 3:
            ret = func_80BDF244(this, play, scheduleOutput);
            break;

        case 1:
        case 2:
            ret = func_80BDF308(this, play, scheduleOutput);
            break;

        default:
            ret = false;
            break;
    }
    return ret;
}

void func_80BDF414(EnAl* this, PlayState* play) {
    s16 temp_v0;

    switch (this->unk_4EA) {
        case 0:
        case 1:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_89_08)) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_85_80)) {
                    func_80BDE4E0(this, &this->unk_4EA, 0);
                }
            }
            break;

        case 2:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_89_08)) {
                this->unk_4EA++;
            }
            break;

        case 3:
        case 4:
            func_80BDE678(this, &this->unk_4EA, 3);
            break;
    }

    temp_v0 = this->actor.world.rot.y - this->actor.yawTowardsPlayer;
    if (((this->unk_4EA == 0) && (ABS_ALT(temp_v0) >= 0x5800)) ||
        ((this->unk_4EA == 5) && (ABS_ALT(temp_v0) >= 0x5800)) || (this->unk_4EA == 2)) {
        SubS_SetOfferMode(&this->unk_4C2, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    } else {
        SubS_SetOfferMode(&this->unk_4C2, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
    }
}

void func_80BDF568(EnAl* this, PlayState* play) {
}

void func_80BDF578(EnAl* this, PlayState* play) {
    switch (this->scheduleResult) {
        case 1:
        case 3:
            func_80BDF568(this, play);
            break;

        case 2:
            func_80BDF414(this, play);
            break;

        default:
            break;
    }

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 3, 0x2AA8);
}

void func_80BDF5E8(EnAl* this, PlayState* play) {
    ScheduleOutput scheduleOutput;

    this->unk_4E0 = R_TIME_SPEED + ((void)0, gSaveContext.save.timeSpeedOffset);
    if (!Schedule_RunScript(play, D_80BDFC70, &scheduleOutput) ||
        ((this->scheduleResult != scheduleOutput.result) && !func_80BDF390(this, play, &scheduleOutput))) {
        this->actor.shape.shadowDraw = NULL;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        scheduleOutput.result = 0;
    } else {
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
    this->scheduleResult = scheduleOutput.result;
    this->unk_368 = func_80BDE384(this, play);
    func_80BDF578(this, play);
}

void func_80BDF6C4(EnAl* this, PlayState* play) {
    if (MsgEvent_RunScript(&this->actor, play, this->msgScript, this->msgScriptCallback, &this->msgScriptPos)) {
        SubS_SetOfferMode(&this->unk_4C2, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk_4C2 &= ~0x20;
        this->unk_4C2 |= 0x200;
        this->actor.child = NULL;
        this->unk_4E2 = 20;
        this->msgScriptPos = 0;
        this->actionFunc = func_80BDF5E8;
    } else {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 3, 0x2AA8);
    }
}

void EnAl_Init(Actor* thisx, PlayState* play) {
    EnAl* this = (EnAl*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gMadameAromaSkel, NULL, this->jointTable, this->morphTable,
                       MADAME_AROMA_LIMB_MAX);
    this->animIndex = ENAL_ANIM_NONE;
    EnAl_ChangeAnim(this, ENAL_ANIM_1);
    Collider_InitAndSetCylinder(play, &this->unk_310, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);
    Actor_SetScale(&this->actor, 0.01f);
    this->scheduleResult = 0;
    this->actionFunc = func_80BDF5E8;

    this->actionFunc(this, play);
}

void EnAl_Destroy(Actor* thisx, PlayState* play) {
    EnAl* this = (EnAl*)thisx;

    Collider_DestroyCylinder(play, &this->unk_310);
}

void EnAl_Update(Actor* thisx, PlayState* play) {
    EnAl* this = (EnAl*)thisx;

    func_80BDEC2C(this, play);

    this->actionFunc(this, play);

    func_80BDF064(this, play);

    if (this->scheduleResult != 0) {
        EnAl_UpdateSkelAnime(this);
        func_80BDEE5C(this);
        SubS_Offer(&this->actor, play, this->unk_4D4, 30.0f, this->unk_4F0, this->unk_4C2 & SUBS_OFFER_MODE_MASK);
        func_80BDE318(this, play);
    }
}

s32 EnAl_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    switch (limbIndex) {
        case MADAME_AROMA_LIMB_SHAWL_MIDDLE:
        case MADAME_AROMA_LIMB_SHAWL_UPPER:
        case MADAME_AROMA_LIMB_SHAWL_LEFT_LOWER_MIDDLE:
        case MADAME_AROMA_LIMB_SHAWL_LEFT_LOWER:
        case MADAME_AROMA_LIMB_SHAWL_RIGHT_LOWER_MIDDLE:
        case MADAME_AROMA_LIMB_SHAWL_RIGHT_LOWER:
            *dList = NULL;
            break;

        case MADAME_AROMA_LIMB_HEAD:
            break;

        default:
            break;
    }
    return false;
}

void EnAl_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnAl* this = (EnAl*)thisx;

    switch (limbIndex) {
        case MADAME_AROMA_LIMB_SHAWL_MIDDLE:
            Matrix_Get(&this->unk_190[0]);
            break;

        case MADAME_AROMA_LIMB_SHAWL_UPPER:
            Matrix_Get(&this->unk_190[1]);
            break;

        case MADAME_AROMA_LIMB_SHAWL_LEFT_LOWER_MIDDLE:
            Matrix_Get(&this->unk_190[2]);
            break;

        case MADAME_AROMA_LIMB_SHAWL_LEFT_LOWER:
            Matrix_Get(&this->unk_190[3]);
            break;

        case MADAME_AROMA_LIMB_SHAWL_RIGHT_LOWER_MIDDLE:
            Matrix_Get(&this->unk_190[4]);
            break;

        case MADAME_AROMA_LIMB_SHAWL_RIGHT_LOWER:
            Matrix_Get(&this->unk_190[5]);
            break;

        case MADAME_AROMA_LIMB_HEAD:
            Matrix_MultVec3f(&D_80BE0070, &this->actor.focus.pos);
            Math_Vec3s_Copy(&this->actor.focus.rot, &this->actor.world.rot);
            break;

        default:
            break;
    }
}

void EnAl_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnAl* this = (EnAl*)thisx;
    s32 stepRot;
    s32 overrideRot;

    if (!(this->unk_4C2 & 0x200)) {
        if (this->unk_4C2 & 0x80) {
            overrideRot = true;
        } else {
            overrideRot = false;
        }
        stepRot = true;
    } else {
        overrideRot = false;
        stepRot = false;
    }

    if (limbIndex == MADAME_AROMA_LIMB_HEAD) {
        SubS_UpdateLimb(this->unk_4DC + 0x4000, this->unk_4DE + this->actor.shape.rot.y + 0x4000, &this->unk_36C,
                        &this->unk_378, stepRot, overrideRot);
        Matrix_Pop();
        Matrix_Translate(this->unk_36C.x, this->unk_36C.y, this->unk_36C.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_378.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_378.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_378.z, MTXMODE_APPLY);
        Matrix_Push();
    }
}

void EnAl_Draw(Actor* thisx, PlayState* play) {
    EnAl* this = (EnAl*)thisx;
    s32 i;

    if (this->scheduleResult != 0) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        Matrix_Translate(0.0f, 0.0f, 850.0f, MTXMODE_APPLY);
        SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       this->skelAnime.dListCount, EnAl_OverrideLimbDraw, EnAl_PostLimbDraw,
                                       EnAl_TransformLimbDraw, &this->actor);

        for (i = 0; i < ARRAY_COUNT(this->unk_190); i++) {
            Matrix_Put(&this->unk_190[i]);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, D_80BE007C[i]);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
