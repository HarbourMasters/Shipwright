/*
 * File: z_en_gm.c
 * Overlay: ovl_En_Gm
 * Description: Gorman
 */

#include "z_en_gm.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnGm_Init(Actor* thisx, PlayState* play);
void EnGm_Destroy(Actor* thisx, PlayState* play);
void EnGm_Update(Actor* thisx, PlayState* play);
void EnGm_Draw(Actor* thisx, PlayState* play);

void func_80950CDC(EnGm* this, PlayState* play);
void func_80950DB8(EnGm* this, PlayState* play);

static u8 D_80951820[] = {
    /* 0x000 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_L(1, 0x13B - 0x005),
    /* 0x005 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_YADOYA, 0x02E - 0x009),
    /* 0x009 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(6, 0, 9, 0, 0x028 - 0x00F),
    /* 0x00F */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(9, 0, 9, 10, 0x022 - 0x015),
    /* 0x015 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(9, 10, 9, 15, 0x01C - 0x01B),
    /* 0x01B */ SCHEDULE_CMD_RET_NONE(),
    /* 0x01C */ SCHEDULE_CMD_RET_TIME(9, 10, 9, 15, 15),
    /* 0x022 */ SCHEDULE_CMD_RET_TIME(9, 0, 9, 10, 24),
    /* 0x028 */ SCHEDULE_CMD_RET_TIME(6, 0, 9, 0, 6),
    /* 0x02E */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_TOWN, 0x07B - 0x032),
    /* 0x032 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(9, 10, 9, 15, 0x075 - 0x038),
    /* 0x038 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(9, 15, 10, 0, 0x06F - 0x03E),
    /* 0x03E */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 0, 10, 5, 0x069 - 0x044),
    /* 0x044 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(12, 15, 12, 20, 0x063 - 0x04A),
    /* 0x04A */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(12, 20, 12, 50, 0x05D - 0x050),
    /* 0x050 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(12, 50, 12, 55, 0x057 - 0x056),
    /* 0x056 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x057 */ SCHEDULE_CMD_RET_TIME(12, 50, 12, 55, 12),
    /* 0x05D */ SCHEDULE_CMD_RET_TIME(12, 20, 12, 50, 22),
    /* 0x063 */ SCHEDULE_CMD_RET_TIME(12, 15, 12, 20, 11),
    /* 0x069 */ SCHEDULE_CMD_RET_TIME(10, 0, 10, 5, 10),
    /* 0x06F */ SCHEDULE_CMD_RET_TIME(9, 15, 10, 0, 21),
    /* 0x075 */ SCHEDULE_CMD_RET_TIME(9, 10, 9, 15, 9),
    /* 0x07B */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_L(SCENE_SONCHONOIE, 0x105 - 0x080),
    /* 0x080 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 0, 10, 5, 0x0FF - 0x086),
    /* 0x086 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 5, 10, 10, 0x0F9 - 0x08C),
    /* 0x08C */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 10, 10, 30, 0x0F3 - 0x092),
    /* 0x092 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 30, 10, 40, 0x0ED - 0x098),
    /* 0x098 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 40, 10, 45, 0x0E7 - 0x09E),
    /* 0x09E */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 45, 10, 55, 0x0E1 - 0x0A4),
    /* 0x0A4 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(10, 55, 11, 45, 0x0DB - 0x0AA),
    /* 0x0AA */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(11, 45, 11, 55, 0x0D5 - 0x0B0),
    /* 0x0B0 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(11, 55, 12, 0, 0x0CF - 0x0B6),
    /* 0x0B6 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(12, 0, 12, 15, 0x0C9 - 0x0BC),
    /* 0x0BC */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(12, 15, 12, 20, 0x0C3 - 0x0C2),
    /* 0x0C2 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x0C3 */ SCHEDULE_CMD_RET_TIME(12, 15, 12, 20, 17),
    /* 0x0C9 */ SCHEDULE_CMD_RET_TIME(12, 0, 12, 15, 29),
    /* 0x0CF */ SCHEDULE_CMD_RET_TIME(11, 55, 12, 0, 19),
    /* 0x0D5 */ SCHEDULE_CMD_RET_TIME(11, 45, 11, 55, 28),
    /* 0x0DB */ SCHEDULE_CMD_RET_TIME(10, 55, 11, 45, 2),
    /* 0x0E1 */ SCHEDULE_CMD_RET_TIME(10, 45, 10, 55, 27),
    /* 0x0E7 */ SCHEDULE_CMD_RET_TIME(10, 40, 10, 45, 18),
    /* 0x0ED */ SCHEDULE_CMD_RET_TIME(10, 30, 10, 40, 26),
    /* 0x0F3 */ SCHEDULE_CMD_RET_TIME(10, 10, 10, 30, 1),
    /* 0x0F9 */ SCHEDULE_CMD_RET_TIME(10, 5, 10, 10, 25),
    /* 0x0FF */ SCHEDULE_CMD_RET_TIME(10, 0, 10, 5, 16),
    /* 0x105 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_MILK_BAR, 0x13A - 0x109),
    /* 0x109 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(12, 55, 13, 0, 0x134 - 0x10F),
    /* 0x10F */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(13, 0, 13, 25, 0x12E - 0x115),
    /* 0x115 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(13, 25, 21, 5, 0x128 - 0x11B),
    /* 0x11B */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(21, 55, 5, 5, 0x122 - 0x121),
    /* 0x121 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x122 */ SCHEDULE_CMD_RET_TIME(21, 55, 5, 5, 5),
    /* 0x128 */ SCHEDULE_CMD_RET_TIME(13, 25, 21, 5, 3),
    /* 0x12E */ SCHEDULE_CMD_RET_TIME(13, 0, 13, 25, 30),
    /* 0x134 */ SCHEDULE_CMD_RET_TIME(12, 55, 13, 0, 20),
    /* 0x13A */ SCHEDULE_CMD_RET_NONE(),
    /* 0x13B */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_L(2, 0x1EB - 0x140),
    /* 0x140 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_L(WEEKEVENTREG_50_01, 0x1CD - 0x145),
    /* 0x145 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_YADOYA, 0x17A - 0x149),
    /* 0x149 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(6, 0, 18, 0, 0x174 - 0x14F),
    /* 0x14F */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(18, 0, 21, 0, 0x16E - 0x155),
    /* 0x155 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(21, 0, 21, 10, 0x168 - 0x15B),
    /* 0x15B */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(21, 10, 21, 15, 0x162 - 0x161),
    /* 0x161 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x162 */ SCHEDULE_CMD_RET_TIME(21, 10, 21, 15, 15),
    /* 0x168 */ SCHEDULE_CMD_RET_TIME(21, 0, 21, 10, 24),
    /* 0x16E */ SCHEDULE_CMD_RET_TIME(18, 0, 21, 0, 6),
    /* 0x174 */ SCHEDULE_CMD_RET_TIME(6, 0, 18, 0, 7),
    /* 0x17A */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_TOWN, 0x1A3 - 0x17E),
    /* 0x17E */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(21, 10, 21, 15, 0x19D - 0x184),
    /* 0x184 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(21, 15, 21, 25, 0x197 - 0x18A),
    /* 0x18A */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(21, 25, 21, 30, 0x191 - 0x190),
    /* 0x190 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x191 */ SCHEDULE_CMD_RET_TIME(21, 25, 21, 30, 14),
    /* 0x197 */ SCHEDULE_CMD_RET_TIME(21, 15, 21, 25, 23),
    /* 0x19D */ SCHEDULE_CMD_RET_TIME(21, 10, 21, 15, 13),
    /* 0x1A3 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_MILK_BAR, 0x1CC - 0x1A7),
    /* 0x1A7 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(21, 30, 21, 45, 0x1C6 - 0x1AD),
    /* 0x1AD */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(21, 45, 22, 10, 0x1C0 - 0x1B3),
    /* 0x1B3 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(22, 10, 6, 0, 0x1BA - 0x1B9),
    /* 0x1B9 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x1BA */ SCHEDULE_CMD_RET_TIME(22, 10, 6, 0, 5),
    /* 0x1C0 */ SCHEDULE_CMD_RET_TIME(21, 45, 22, 10, 30),
    /* 0x1C6 */ SCHEDULE_CMD_RET_TIME(21, 30, 21, 45, 20),
    /* 0x1CC */ SCHEDULE_CMD_RET_NONE(),
    /* 0x1CD */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_YADOYA, 0x1EA - 0x1D1),
    /* 0x1D1 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(6, 0, 18, 0, 0x1E4 - 0x1D7),
    /* 0x1D7 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(18, 0, 6, 0, 0x1DE - 0x1DD),
    /* 0x1DD */ SCHEDULE_CMD_RET_NONE(),
    /* 0x1DE */ SCHEDULE_CMD_RET_TIME(0, 0, 0, 0, 8),
    /* 0x1E4 */ SCHEDULE_CMD_RET_TIME(6, 0, 18, 0, 7),
    /* 0x1EA */ SCHEDULE_CMD_RET_NONE(),
    /* 0x1EB */ SCHEDULE_CMD_RET_NONE(),
};

static s32 D_80951A0C[] = {
    -1, 1, 4, 1, -1, 1, -1, -1, -1, 0, 2, 3, 5, 6, 8, 1, 0, 8, 3, 6, 0, 1, 4, 7, 0, 1, 2, 4, 5, 7, 1,
};

static MsgScript D_80951A88[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB9),
    /* 0x0003 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_GORMAN),
    /* 0x0006 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0007 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_80951A90[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2ABA),
    /* 0x0003 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_GORMAN),
    /* 0x0006 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0007 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_80951A98[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_86_40, 0x001F - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0008 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AA4),
    /* 0x000B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000C 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x000F 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0010 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AA5),
    /* 0x0013 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_GORMAN),
    /* 0x0016 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0017 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x001A 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x001B 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_86_40),
    /* 0x001E 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x001F 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0022 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AEC),
    /* 0x0025 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0026 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0029 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x002A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AED),
    /* 0x002D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002E 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0031 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0032 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AEE),
    /* 0x0035 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_GORMAN),
    /* 0x0038 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0039 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x003C 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x003D 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_80951AD8[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_86_20, 0x00A6 - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0008 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AA6),
    /* 0x000B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000C 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x000F 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0010 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AA7),
    /* 0x0013 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0014 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0017 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0018 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AA8),
    /* 0x001B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001C 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x001F 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0020 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AA9),
    /* 0x0023 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0024 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0027 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0028 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AAA),
    /* 0x002B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002C 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x002F 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0030 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AAB),
    /* 0x0033 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0034 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0037 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x0038 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AAC),
    /* 0x003B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003C 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x003F 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0040 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AAD),
    /* 0x0043 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0044 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2AAE),
    /* 0x0047 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0048 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x004B 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x004C 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AAF),
    /* 0x004F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0050 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0053 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0054 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB0),
    /* 0x0057 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0058 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x005B 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x005C 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB1),
    /* 0x005F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0060 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0063 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0064 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB2),
    /* 0x0067 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0068 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x006B 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x006C 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB3),
    /* 0x006F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0070 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0073 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0074 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB4),
    /* 0x0077 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0078 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x007B 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x007C 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB5),
    /* 0x007F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0080 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0083 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0084 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB6),
    /* 0x0087 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0088 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x008B 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x008C 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB7),
    /* 0x008F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0090 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0093 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0094 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AB8),
    /* 0x0097 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x009A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_TOTO),
    /* 0x009D 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_GORMAN),
    /* 0x00A0 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00A1 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_86_20),
    /* 0x00A4 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00A5 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x00A6 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x00A9 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AEA),
    /* 0x00AC 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00AD 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x00B0 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x00B1 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2AEB),
    /* 0x00B4 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_MADAME_AROMA),
    /* 0x00B7 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_GORMAN),
    /* 0x00BA 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00BB 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x00BE 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00BF 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_80951B98[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_75_01, 0x0047 - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0008 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x000B 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B01),
    /* 0x000E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000F 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0012 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0013 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B02),
    /* 0x0016 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0017 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x001A 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x001B 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B03),
    /* 0x001E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001F 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0022 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0023 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B04),
    /* 0x0026 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0027 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2B05),
    /* 0x002A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002B 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x002E 0x01 */ MSCRIPT_CMD_FOCUS_TO_SELF(),
    /* 0x002F 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B06),
    /* 0x0032 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0033 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0036 0x01 */ MSCRIPT_CMD_FOCUS_TO_CHILD(),
    /* 0x0037 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B07),
    /* 0x003A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_GORMAN),
    /* 0x003D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003E 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_75_01),
    /* 0x0041 0x01 */ MSCRIPT_CMD_PAUSE(),
    /* 0x0042 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0045 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0046 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0047 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B08),
    /* 0x004A 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_GORMAN),
    /* 0x004D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004E 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x004F 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_80951BE8[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0003 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_50_01, 0x0039 - 0x0008),
    /* 0x0008 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_51_80, 0x0039 - 0x000D),
    /* 0x000D 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_75_02, 0x001F - 0x0012),
    /* 0x0012 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B13),
    /* 0x0015 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_GORMAN),
    /* 0x0018 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0019 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_75_02),
    /* 0x001C 0x03 */ MSCRIPT_CMD_JUMP(0x0034 - 0x001F),
    /* 0x001F 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B14),
    /* 0x0022 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0023 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2B15),
    /* 0x0026 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0027 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2B16),
    /* 0x002A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002B 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2B17),
    /* 0x002E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x2B18),
    /* 0x0032 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0033 0x01 */ MSCRIPT_CMD_PAUSE(),
    /* 0x0034 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0037 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0038 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0039 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2B12),
    /* 0x003C 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_GORMAN),
    /* 0x003F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0040 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_80951C2C[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x295A),
    /* 0x0003 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_GORMAN),
    /* 0x0006 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0007 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_80951C34[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2962),
    /* 0x0003 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_GORMAN),
    /* 0x0006 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0007 0x01 */ MSCRIPT_CMD_DONE(),
};

ActorProfile En_Gm_Profile = {
    /**/ ACTOR_EN_GM,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_IN2,
    /**/ sizeof(EnGm),
    /**/ EnGm_Init,
    /**/ EnGm_Destroy,
    /**/ EnGm_Update,
    /**/ EnGm_Draw,
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

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 0, { { 0, 0, 0 }, 20 }, 100 },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /*  -1 */ ENGM_ANIM_NONE = -1,
    /* 0x0 */ ENGM_ANIM_0,
    /* 0x1 */ ENGM_ANIM_1,
    /* 0x2 */ ENGM_ANIM_2,
    /* 0x3 */ ENGM_ANIM_3,
    /* 0x4 */ ENGM_ANIM_4,
    /* 0x5 */ ENGM_ANIM_5,
    /* 0x6 */ ENGM_ANIM_6,
    /* 0x7 */ ENGM_ANIM_7,
    /* 0x8 */ ENGM_ANIM_8,
    /* 0x9 */ ENGM_ANIM_9,
    /* 0xA */ ENGM_ANIM_10,
    /* 0xB */ ENGM_ANIM_11,
    /* 0xC */ ENGM_ANIM_12,
    /* 0xD */ ENGM_ANIM_MAX
} EnGmAnimation;

static AnimationInfoS sAnimationInfo[ENGM_ANIM_MAX] = {
    { &object_in2_Anim_009CDC, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENGM_ANIM_0
    { &object_in2_Anim_009CDC, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENGM_ANIM_1
    { &object_in2_Anim_00A5E0, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENGM_ANIM_2
    { &object_in2_Anim_00A70C, 1.0f, 0, 1, ANIMMODE_LOOP, 0 },   // ENGM_ANIM_3
    { &object_in2_Anim_008090, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENGM_ANIM_4
    { &object_in2_Anim_00898C, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENGM_ANIM_5
    { &object_in2_Anim_009450, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENGM_ANIM_6
    { &object_in2_Anim_00AD18, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENGM_ANIM_7
    { &object_in2_Anim_00AD18, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENGM_ANIM_8
    { &object_in2_Anim_00B8B0, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },  // ENGM_ANIM_9
    { &object_in2_Anim_00BA80, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENGM_ANIM_10
    { &object_in2_Anim_00C03C, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENGM_ANIM_11
    { &object_in2_Anim_00B990, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENGM_ANIM_12
};

Actor* EnGm_FindActor(EnGm* this, PlayState* play, u8 actorCategory, s16 actorId) {
    Actor* actorIter = NULL;

    while (true) {
        actorIter = SubS_FindActor(play, actorIter, actorCategory, actorId);

        if (actorIter == NULL) {
            break;
        }

        if ((this != (EnGm*)actorIter) && (actorIter->update != NULL)) {
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

EnDoor* EnGm_FindScheduleDoor(PlayState* play, s32 scheduleOutputResult) {
    EnDoorScheduleType schType;

    switch (scheduleOutputResult) {
        case 9:
        case 13:
        case 15:
            schType = ENDOOR_SCH_TYPE_INN_MAIN_ENTRANCE;
            break;

        case 10:
        case 11:
        case 16:
        case 17:
            schType = ENDOOR_SCH_TYPE_MAYORS_RESIDENCE_MAIN_ENTRANCE;
            break;

        case 12:
        case 14:
        case 20:
            schType = ENDOOR_SCH_TYPE_MILK_BAR;
            break;

        case 18:
        case 19:
            schType = ENDOOR_SCH_TYPE_MAYORS_RESIDENCE_MADAME_AROMA;
            break;

        default:
            schType = -1;
            break;
    }

    return SubS_FindScheduleDoor(play, schType);
}

s32 EnGm_UpdateSkelAnime(EnGm* this, PlayState* play) {
    s32 isAnimFinished = false;

    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        return false;
    }

    if (this->objectSlot > OBJECT_SLOT_NONE) {
        this->skelAnime.playSpeed = this->animPlaySpeed;
        isAnimFinished = SkelAnime_Update(&this->skelAnime);
    }

    return isAnimFinished;
}

s32 EnGm_ChangeAnim(EnGm* this, PlayState* play, s32 animIndex) {
    s8 objectSlot = this->objectSlot;
    s32 changeAnim = false;
    s32 didAnimChange = false;

    if ((animIndex == ENGM_ANIM_0) || (animIndex == ENGM_ANIM_1)) {
        if ((this->animIndex != ENGM_ANIM_0) && (this->animIndex != ENGM_ANIM_1)) {
            changeAnim = true;
        }
    } else if (this->animIndex != animIndex) {
        changeAnim = true;
    }

    if (changeAnim) {
        if (objectSlot > OBJECT_SLOT_NONE) {
            this->animIndex = animIndex;
            didAnimChange = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
            this->animPlaySpeed = this->skelAnime.playSpeed;
        }
    }

    return didAnimChange;
}

s32 func_8094E0F8(EnGm* this, PlayState* play) {
    s32 ret = false;

    if ((this->unk_260 != play->roomCtx.curRoom.num) && (play->roomCtx.status == 0)) {
        this->unk_260 = play->roomCtx.curRoom.num;
        this->objectSlot = SubS_GetObjectSlot(OBJECT_IN2, play);
        this->actor.draw = NULL;
        this->unk_3FC = 1;
    }

    if (this->unk_3FC == 0) {
        return false;
    }

    if ((this->objectSlot <= OBJECT_SLOT_NONE) || !SubS_IsObjectLoaded(this->objectSlot, play)) {
        ret = true;
    } else {
        this->actor.draw = EnGm_Draw;
        if (this->scheduleResult == 2) {
            this->unk_268 = NULL;
            this->unk_3B8 = 0;
        }
        this->unk_3FC = 0;
    }

    return ret;
}

void func_8094E1DC(EnGm* this, PlayState* play) {
    s32 pad;
    f32 temp;

    if (this->scheduleResult == 7) {
        temp = this->colliderSphere.dim.modelSphere.radius * this->colliderSphere.dim.scale;
        this->colliderSphere.dim.worldSphere.radius = temp;
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderSphere.base);
    } else {
        Collider_UpdateCylinder(&this->actor, &this->colliderCylinder);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderCylinder.base);
    }
}

void func_8094E278(PlayState* play) {
    Actor_Spawn(&play->actorCtx, play, ACTOR_DM_CHAR07, 115.0f, 32.0f, -121.0f, 0, 0, 0, 3);
}

void func_8094E2D0(EnGm* this) {
    s32 phi_a1 = false;

    if (this->unk_3A4 & 0x100) {
        if (DECR(this->unk_3CC) == 0) {
            switch (this->unk_3CA) {
                case 1:
                    if ((this->unk_3CE == 5) || (this->unk_3CE == 2)) {
                        phi_a1 = true;
                        this->unk_3CE = 5;
                    }
                    break;

                case 2:
                    if ((this->unk_3CE == 4) || (this->unk_3CE == 2)) {
                        phi_a1 = true;
                        this->unk_3CE = 4;
                    }
                    break;

                case 3:
                    if (this->unk_3CE == 2) {
                        phi_a1 = true;
                        this->unk_3CE = 2;
                    }
                    break;

                case 4:
                    if (this->unk_3CE == 1) {
                        phi_a1 = true;
                        this->unk_3CE = 1;
                    }
                    break;
            }

            if (!phi_a1 && ((this->unk_3CE == 4) || (this->unk_3CE == 5))) {
                this->unk_3CE = 1;
            }

            if (!phi_a1) {
                this->unk_3CE++;
                if (this->unk_3CE >= 4) {
                    if (this->unk_3CA == 0) {
                        this->unk_3CC = Rand_S16Offset(30, 30);
                    } else {
                        this->unk_3CC = 8;
                    }
                    this->unk_3CE = 0;
                }
            }
        }
    }
}

s32 func_8094E454(EnGm* this, s16 csId) {
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

s16 func_8094E4D0(EnGm* this, s32 numCutscenes) {
    s32 i;
    s16 csId = this->actor.csId;

    for (i = 0; i < numCutscenes; i++) {
        csId = CutsceneManager_GetAdditionalCsId(csId);
    }

    return csId;
}

s32 func_8094E52C(Actor* thisx, PlayState* play) {
    EnGm* this = (EnGm*)thisx;
    s16 csId = func_8094E4D0(this, 0);
    s32 ret = false;

    switch (this->unk_3E0) {
        case 0:
            if (!func_8094E454(this, csId)) {
                break;
            }

        case 2:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_86_40) && (this->unk_3E0 == 2)) {
                CutsceneManager_Stop(csId);
            } else {
                Camera_SetTargetActor(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(csId)), &this->actor);
            }
            this->unk_3E0++;
            ret = true;
            break;

        case 1:
            if ((this->actor.child != NULL) && (this->actor.child->update != NULL)) {
                Camera_SetTargetActor(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(csId)),
                                      this->actor.child);
            }
            this->unk_3E0++;
            ret = true;
            break;

        case 3:
            CutsceneManager_Stop(csId);
            this->unk_3E0++;
            ret = true;
            break;

        default:
            break;
    }

    return ret;
}

s32 func_8094E69C(Actor* thisx, PlayState* play) {
    EnGm* this = (EnGm*)thisx;
    s16 csId = func_8094E4D0(this, 0);
    s16 sp48;
    Vec3f sp3C;
    Vec3f sp30;
    s32 ret = false;

    switch (this->unk_3E0) {
        case 0:
            Actor_PlaySfx(&this->actor, NA_SE_EV_CHAIR_ROLL);
            EnGm_ChangeAnim(this, play, ENGM_ANIM_2);
            this->unk_3E2 = 0;
            this->unk_3E0++;
            break;

        case 1:
            if ((this->actor.child != NULL) || (this->actor.child->update != NULL)) {
                Math_Vec3f_Copy(&sp3C, &this->actor.child->world.pos);
                Math_Vec3f_Copy(&sp30, &this->actor.world.pos);
                sp48 = Math_Vec3f_Yaw(&sp30, &sp3C);
                this->unk_3E2++;
                if (((this->actor.shape.rot.y / 364) != (sp48 / 364)) && (this->unk_3E2 < 20)) {
                    Math_ApproachS(&this->actor.shape.rot.y, sp48, 3, 0x2AA8);
                } else {
                    this->actor.shape.rot.y = sp48;
                    this->unk_3A4 |= 0x20;
                    this->unk_3E2 = 0;
                    this->unk_3E0++;
                    ret = true;
                }
            } else {
                this->unk_3A4 |= 0x20;
                this->actor.shape.rot.y = this->actor.world.rot.y;
                this->unk_3E0++;
                ret = true;
            }
            break;

        case 2:
            if (func_8094E454(this, csId)) {
                case 4:
                case 6:
                    sp48 = CutsceneManager_GetCurrentSubCamId(csId);
                    Camera_SetTargetActor(Play_GetCamera(play, sp48), &this->actor);
                    this->unk_3E0++;
                    ret = true;
            }
            break;

        case 3:
        case 5:
        case 7:
            if ((this->actor.child != NULL) && (this->actor.child->update != NULL)) {
                sp48 = CutsceneManager_GetCurrentSubCamId(csId);
                Camera_SetTargetActor(Play_GetCamera(play, sp48), this->actor.child);
            }
            this->unk_3E0++;
            ret = true;
            break;

        case 8:
            Actor_PlaySfx(&this->actor, NA_SE_EV_CHAIR_ROLL);
            CutsceneManager_Stop(csId);
            this->unk_3E2 = 0;
            this->unk_3E0++;
            break;

        case 9:
            sp48 = this->actor.world.rot.y;
            this->unk_3E2++;
            if (((this->actor.shape.rot.y / 364) != (sp48 / 364)) && (this->unk_3E2 < 20)) {
                Math_ApproachS(&this->actor.shape.rot.y, sp48, 3, 0x2AA8);
                this->unk_3A4 &= ~0x20;
            } else {
                EnGm_ChangeAnim(this, play, ENGM_ANIM_1);
                this->actor.shape.rot.y = sp48;
                this->unk_3E0++;
                this->unk_3E2 = 0;
                ret = true;
            }
            break;

        default:
            break;
    }
    return ret;
}

s32 func_8094EA34(Actor* thisx, PlayState* play) {
    EnGm* this = (EnGm*)thisx;
    s32 pad;
    Actor* al;
    Actor* toto;

    al = EnGm_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_AL);
    toto = EnGm_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_TOTO);
    if ((al == NULL) || (al->update == NULL) || (toto == NULL) || (toto->update == NULL)) {
        this->unk_3E0++;
        return true;
    }

    switch (this->unk_3E0) {
        case 0:
        case 2:
        case 4:
        case 6:
        case 10:
        case 14:
        case 16:
            this->unk_3E0++;
            break;

        case 7:
        case 9:
        case 12:
            this->actor.child = toto;
            this->unk_3E0++;
            break;

        case 1:
        case 3:
        case 5:
        case 8:
        case 11:
        case 13:
        case 15:
        case 17:
            this->actor.child = al;
            this->unk_3E0++;
            break;

        default:
            this->unk_3E0++;
            break;
    }

    return true;
}

s32 func_8094EB1C(Actor* thisx, PlayState* play) {
    EnGm* this = (EnGm*)thisx;
    s32 pad;
    s32 ret = false;
    s16 oldYaw;

    switch (this->unk_3E0) {
        case 0:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_50_01) || CHECK_WEEKEVENTREG(WEEKEVENTREG_51_80) ||
                CHECK_WEEKEVENTREG(WEEKEVENTREG_75_02)) {
                ret = true;
                break;
            }
            Actor_PlaySfx(&this->actor, NA_SE_EV_CHAIR_ROLL);
            EnGm_ChangeAnim(this, play, ENGM_ANIM_2);
            this->unk_3E2 = 0;
            this->unk_3E0++;

        case 1:
            oldYaw = this->actor.yawTowardsPlayer;
            this->unk_3E2++;
            if (((this->actor.shape.rot.y / 364) != (oldYaw / 364)) && (this->unk_3E2 < 20)) {
                Math_ApproachS(&this->actor.shape.rot.y, oldYaw, 3, 0x2AA8);
            } else {
                this->actor.shape.rot.y = oldYaw;
                this->unk_3A4 |= 0x20;
                this->unk_3E2 = 0;
                this->unk_3E0++;
                ret = true;
            }
            break;

        case 2:
            Actor_PlaySfx(&this->actor, NA_SE_EV_CHAIR_ROLL);
            this->unk_3E2 = 0;
            this->unk_3E0++;

        case 3:
            oldYaw = this->actor.world.rot.y;
            this->unk_3E2++;
            if (((this->actor.shape.rot.y / 364) != (oldYaw / 364)) && (this->unk_3E2 < 20)) {
                Math_ApproachS(&this->actor.shape.rot.y, oldYaw, 3, 0x2AA8);
                this->unk_3A4 &= ~0x20;
            } else {
                EnGm_ChangeAnim(this, play, ENGM_ANIM_1);
                this->actor.shape.rot.y = oldYaw;
                this->unk_3E2 = 0;
                this->unk_3E0++;
                ret = true;
            }
            break;
    }
    return ret;
}

MsgScript* EnGm_GetMsgScript(EnGm* this, PlayState* play) {
    switch (this->scheduleResult) {
        case 1:
            this->msgScriptCallback = func_8094E52C;
            return D_80951A98;

        case 2:
            this->msgScriptCallback = func_8094EA34;
            return D_80951AD8;

        case 3:
            this->msgScriptCallback = func_8094E69C;
            return D_80951B98;

        case 5:
            this->msgScriptCallback = func_8094EB1C;
            return D_80951BE8;

        case 7:
            return D_80951C2C;

        case 8:
            return D_80951C34;

        case 6:
        case 21:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 30:
            return D_80951A88;

        case 12:
        case 17:
        case 19:
        case 20:
        case 22:
        case 28:
        case 29:
            return D_80951A90;

        default:
            break;
    }
    return NULL;
}

s32 func_8094EE84(EnGm* this, PlayState* play) {
    s32 ret = false;

    if (((this->unk_3A4 & SUBS_OFFER_MODE_MASK) != SUBS_OFFER_MODE_NONE) &&
        Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        SubS_SetOfferMode(&this->unk_3A4, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
        this->unk_3E0 = 0;
        this->msgScriptCallback = NULL;
        this->actor.child = this->unk_268;
        this->msgScript = EnGm_GetMsgScript(this, play);

        if ((this->scheduleResult == 5) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_50_01) &&
            !CHECK_WEEKEVENTREG(WEEKEVENTREG_51_80) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_75_02)) {
            this->unk_3A4 |= 0x20;
        } else if ((this->scheduleResult != 1) && (this->scheduleResult != 5) && (this->scheduleResult != 7)) {
            this->unk_3A4 |= 0x20;
        }

        if ((this->scheduleResult == 3) && CHECK_WEEKEVENTREG(WEEKEVENTREG_75_01)) {
            this->unk_3A4 &= ~0x20;
        }

        this->actionFunc = func_80950DB8;
        ret = true;
    }
    return ret;
}

s32 func_8094EFC4(EnGm* this, PlayState* play) {
    s32 ret = false;

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (this->unk_3F8 == 0) {
            if ((play->sceneId == SCENE_MILK_BAR) && (gSaveContext.sceneLayer == 2)) {
                EnGm_ChangeAnim(this, play, ENGM_ANIM_0);
                this->scheduleResult = 255;
            }
            this->cueId = 255;
            this->unk_3F8 = 1;
        }
        ret = true;
    } else if (this->unk_3F8 != 0) {
        if (play->sceneId == SCENE_MILK_BAR) {
            this->unk_400 = 0;
        }
        this->unk_3F8 = 0;
    }
    return ret;
}

Actor* func_8094F074(EnGm* this, PlayState* play) {
    Actor* actor;

    switch (this->scheduleResult) {
        case 1:
            actor = EnGm_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_RECEPGIRL);
            break;

        case 2:
            actor = this->unk_268;
            break;

        case 3:
            actor = EnGm_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_TAB);
            break;

        default:
            actor = &GET_PLAYER(play)->actor;
            break;
    }

    return actor;
}

void func_8094F0E0(EnGm* this) {
    s32 pad;
    Vec3f sp40;
    Vec3f sp34;
    s16 sp32;

    Math_Vec3f_Copy(&sp40, &this->unk_268->world.pos);
    Math_Vec3f_Copy(&sp34, &this->actor.world.pos);
    sp32 = Math_Vec3f_Yaw(&sp34, &sp40);
    Math_ApproachS(&this->unk_3BE, (sp32 - this->unk_3C2) - this->actor.shape.rot.y, 4, 0x2AA8);
    this->unk_3BE = CLAMP(this->unk_3BE, -0x1FFE, 0x1FFE);

    Math_ApproachS(&this->unk_3C2, (sp32 - this->unk_3BE) - this->actor.shape.rot.y, 4, 0x2AA8);
    this->unk_3C2 = CLAMP(this->unk_3C2, -0x1C70, 0x1C70);

    Math_Vec3f_Copy(&sp34, &this->actor.focus.pos);

    if (this->unk_268->id == ACTOR_PLAYER) {
        sp40.y = ((Player*)this->unk_268)->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    } else {
        Math_Vec3f_Copy(&sp40, &this->unk_268->focus.pos);
    }

    Math_ApproachS(&this->unk_3BC, Math_Vec3f_Pitch(&sp34, &sp40) - this->unk_3C0, 4, 0x2AA8);
    this->unk_3BC = CLAMP(this->unk_3BC, -0x1554, 0x1554);

    Math_ApproachS(&this->unk_3C0, Math_Vec3f_Pitch(&sp34, &sp40) - this->unk_3BC, 4, 0x2AA8);
    this->unk_3C0 = CLAMP(this->unk_3C0, -0xE38, 0xE38);
}

void func_8094F2E8(EnGm* this) {
    if ((this->unk_3A4 & 0x20) && (this->unk_268 != NULL) && (this->unk_268->update != NULL)) {
        if (DECR(this->unk_3C6) == 0) {
            func_8094F0E0(this);
            this->unk_3A4 &= ~0x200;
            this->unk_3A4 |= 0x80;
            return;
        }
    }

    if (this->unk_3A4 & 0x80) {
        this->unk_3A4 &= ~0x80;
        this->unk_3BC = 0;
        this->unk_3BE = 0;
        this->unk_3C0 = 0;
        this->unk_3C2 = 0;
        this->unk_3C6 = 20;
    } else if (DECR(this->unk_3C6) == 0) {
        this->unk_3A4 |= 0x200;
    }
}

void func_8094F3D0(EnGm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 talkState = Message_GetState(&play->msgCtx);
    s32 textId = play->msgCtx.currentTextId;

    if ((&this->actor == player->talkActor) && ((textId < 0xFF) || (textId > 0x200)) &&
        (talkState == TEXT_STATE_FADING) && (this->prevTalkState == TEXT_STATE_FADING)) {
        if ((play->state.frames % 3) == 0) {
            if (this->unk_3AC == 120.0f) {
                this->unk_3AC = 0.0f;
            } else {
                this->unk_3AC = 120.0f;
            }
        }
    } else {
        this->unk_3AC = 0.0f;
    }

    Math_SmoothStepToF(&this->unk_3B0, this->unk_3AC, 0.8f, 40.0f, 10.0f);
    Matrix_Translate(this->unk_3B0, 0.0f, 0.0f, MTXMODE_APPLY);
    this->prevTalkState = talkState;
}

s32 func_8094F4EC(EnGm* this, PlayState* play) {
    if ((this->scheduleResult != 6) && (this->animIndex == ENGM_ANIM_12)) {
        this->unk_3A4 &= ~0x2000;
        EnGm_ChangeAnim(this, play, ENGM_ANIM_8);
    }
    return true;
}

s32 func_8094F53C(EnGm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u16 sp32 = play->msgCtx.currentTextId;
    Actor* al = EnGm_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_AL);
    Actor* toto = EnGm_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_TOTO);

    if (player->stateFlags1 & (PLAYER_STATE1_TALKING | PLAYER_STATE1_400)) {
        this->unk_3A4 |= 0x400;
        if (this->unk_3A6 != sp32) {
            switch (sp32) {
                case 0x2B13:
                    EnGm_ChangeAnim(this, play, ENGM_ANIM_2);
                    break;

                case 0x2B14:
                case 0x2B18:
                    EnGm_ChangeAnim(this, play, ENGM_ANIM_0);
                    break;

                case 0x2B16:
                    EnGm_ChangeAnim(this, play, ENGM_ANIM_1);
                    break;

                case 0x2B15:
                    EnGm_ChangeAnim(this, play, ENGM_ANIM_5);
                    break;

                case 0x2B17:
                    EnGm_ChangeAnim(this, play, ENGM_ANIM_6);
                    break;

                default:
                    if ((this->animIndex == ENGM_ANIM_7) || (this->animIndex == ENGM_ANIM_8)) {
                        this->unk_3A4 |= 0x2000;
                        EnGm_ChangeAnim(this, play, ENGM_ANIM_12);
                    }
                    break;
            }

            switch (sp32) {
                case 0x2AA6:
                case 0x2AAF:
                case 0x2AB4:
                    if ((al != NULL) && (al->update != NULL)) {
                        this->unk_268 = al;
                    }
                    break;

                case 0x2AAD:
                case 0x2AB0:
                case 0x2AB2:
                    if ((toto != NULL) && (toto->update != NULL)) {
                        this->unk_268 = toto;
                    }
                    break;

                default:
                    break;
            }

            if (sp32 == 0x2AA8) {
                this->unk_3C8 = 0;
                this->unk_3CA = 0;
                this->unk_3CC = 8;
            }
        }
        this->unk_3A6 = sp32;
    } else if (this->unk_3A4 & 0x400) {
        this->unk_18C = NULL;
        this->unk_3A6 = 0;
        this->unk_3A4 &= ~0x400;
        this->unk_3CA = this->unk_3C8;
        this->unk_3CC = 4;
        func_8094F4EC(this, play);
    }

    if (this->unk_18C != NULL) {
        this->unk_18C(this, play);
    }

    if ((this->animIndex == ENGM_ANIM_6) && !(play->actorCtx.flags & ACTORCTX_FLAG_5) &&
        Animation_OnFrame(&this->skelAnime, 20.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_HANKO);
    }

    return false;
}

s32 func_8094F7D0(EnGm* this, PlayState* play, ScheduleOutput* scheduleOutput, u8 actorCategory, s16 actorId) {
    u8 pathIndex = ENGM_GET_PATH_INDEX(&this->actor);
    Vec3s* sp48;
    Vec3f sp3C;
    Vec3f sp30;
    Actor* actor;
    s32 pad;
    s32 ret = false;

    this->timePath = NULL;
    actor = EnGm_FindActor(this, play, actorCategory, actorId);

    if (D_80951A0C[scheduleOutput->result] >= 0) {
        this->timePath = SubS_GetAdditionalPath(play, pathIndex, D_80951A0C[scheduleOutput->result]);
    }

    if ((actor != NULL) && (actor->update != NULL)) {
        if (this->timePath != NULL) {
            sp48 = Lib_SegmentedToVirtual(this->timePath->points);
            Math_Vec3s_ToVec3f(&sp3C, &sp48[this->timePath->count - 2]);
            Math_Vec3s_ToVec3f(&sp30, &sp48[this->timePath->count - 1]);
            this->actor.shape.shadowDraw = NULL;
            this->actor.world.rot.y = Math_Vec3f_Yaw(&sp3C, &sp30);
            Math_Vec3f_Copy(&this->actor.world.pos, &sp30);
            ret = true;
        }
    }
    return ret;
}

s32 func_8094F904(EnGm* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    u16 sp56 = SCRIPT_TIME_NOW;
    u8 pathIndex = ENGM_GET_PATH_INDEX(&this->actor);
    EnDoor* door;
    Vec3s* sp4C;
    Vec3f sp40;
    Vec3f sp34;
    s32 pad;
    s32 ret = false;

    this->timePath = NULL;
    door = EnGm_FindScheduleDoor(play, scheduleOutput->result);

    if (D_80951A0C[scheduleOutput->result] >= 0) {
        this->timePath = SubS_GetAdditionalPath(play, pathIndex, D_80951A0C[scheduleOutput->result]);
    }

    if ((door != NULL) && (door->knobDoor.dyna.actor.update != NULL)) {
        if (this->timePath != NULL) {
            sp4C = Lib_SegmentedToVirtual(this->timePath->points);
            Math_Vec3s_ToVec3f(&sp40, &sp4C[0]);
            Math_Vec3s_ToVec3f(&sp34, &sp4C[1]);
            Math_Vec3f_Copy(&this->unk_278, &sp40);
            Math_Vec3f_Copy(&this->unk_284, &sp34);
            this->actor.world.rot.y = Math_Vec3f_Yaw(&sp40, &sp34);
            Math_Vec3f_Copy(&this->actor.world.pos, &sp40);

            if (ABS_ALT(BINANG_SUB(this->actor.world.rot.y, door->knobDoor.dyna.actor.shape.rot.y)) <= 0x4000) {
                this->unk_261 = -75;
            } else {
                this->unk_261 = 75;
            }

            this->unk_3B8 = scheduleOutput->time1 - scheduleOutput->time0;
            this->unk_3BA = sp56 - scheduleOutput->time0;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->unk_3A4 |= 0x100;
            this->unk_3A4 |= 0x200;
            EnGm_ChangeAnim(this, play, ENGM_ANIM_7);
            this->actor.gravity = 0.0f;
            ret = true;
        }
    }
    return ret;
}

s32 func_8094FAC4(EnGm* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    u16 sp2E = SCRIPT_TIME_NOW;
    u16 phi_v1;
    u8 pathIndex = ENGM_GET_PATH_INDEX(&this->actor);
    u16 tmp;
    s16 pad;
    s32 ret = false;

    this->timePath = NULL;
    if (D_80951A0C[scheduleOutput->result] >= 0) {
        this->timePath = SubS_GetAdditionalPath(play, pathIndex, D_80951A0C[scheduleOutput->result]);
    }

    if ((this->timePath != NULL) && (this->timePath->count < 3)) {
        this->timePath = NULL;
    }

    if (this->timePath != NULL) {
        if ((this->scheduleResult < 9) && (this->scheduleResult != 0) && (this->timePathTimeSpeed >= 0)) {
            phi_v1 = sp2E;
        } else {
            phi_v1 = scheduleOutput->time0;
        }

        if (scheduleOutput->time1 < phi_v1) {
            this->timePathTotalTime = (phi_v1 - scheduleOutput->time1) + 0xFFFF;
        } else {
            this->timePathTotalTime = scheduleOutput->time1 - phi_v1;
        }

        this->timePathElapsedTime = sp2E - phi_v1;
        tmp = phi_v1 = this->timePath->count - (SUBS_TIME_PATHING_ORDER - 1);
        this->timePathWaypointTime = this->timePathTotalTime / tmp;
        this->timePathWaypoint =
            (this->timePathElapsedTime / this->timePathWaypointTime) + (SUBS_TIME_PATHING_ORDER - 1);
        this->unk_3A4 &= ~0x8;
        this->unk_3A4 &= ~0x10;
        SubS_SetOfferMode(&this->unk_3A4, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk_3A4 |= 0x100;
        this->unk_3A4 |= 0x200;
        EnGm_ChangeAnim(this, play, ENGM_ANIM_7);
        this->actor.gravity = -1.0f;
        ret = true;
    }
    return ret;
}

s32 func_8094FCC4(EnGm* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    static Vec3f D_80951D90 = { 64.0f, 0.0f, -122.0f };
    s32 ret = false;

    if (func_8094F7D0(this, play, scheduleOutput, ACTORCAT_NPC, ACTOR_EN_TAB)) {
        if (this->scheduleResult == 0) {
            Math_Vec3f_Copy(&this->actor.world.pos, &D_80951D90);
            SubS_SetOfferMode(&this->unk_3A4, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
            EnGm_ChangeAnim(this, play, ENGM_ANIM_0);
        } else {
            EnGm_ChangeAnim(this, play, ENGM_ANIM_9);
            this->skelAnime.movementFlags = ANIM_FLAG_NOMOVE;
        }
        this->unk_3A4 |= 0x100;
        this->unk_3A4 |= 0x200;
        ret = true;
    }
    return ret;
}

s32 func_8094FD88(EnGm* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    s32 ret = false;

    if (func_8094F7D0(this, play, scheduleOutput, ACTORCAT_NPC, ACTOR_EN_RECEPGIRL)) {
        EnGm_ChangeAnim(this, play, ENGM_ANIM_11);
        SubS_SetOfferMode(&this->unk_3A4, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk_3A4 |= 0x100;
        this->unk_3A4 |= 0x200;
        ret = true;
    }
    return ret;
}

s32 func_8094FE10(EnGm* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    s32 ret = false;
    Actor* al;

    al = EnGm_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_AL);
    if (func_8094F7D0(this, play, scheduleOutput, ACTORCAT_NPC, ACTOR_EN_TOTO) && (al != NULL) &&
        (al->update != NULL)) {
        EnGm_ChangeAnim(this, play, ENGM_ANIM_11);
        SubS_SetOfferMode(&this->unk_3A4, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk_268 = al;
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_86_20)) {
            this->unk_3C8 = 2;
            this->unk_3CA = 2;
            this->unk_3CC = 8;
        }
        this->unk_3A4 |= (0x100 | 0x20);
        this->unk_3A4 |= 0x200;
        ret = true;
    }
    return ret;
}

s32 func_8094FF04(EnGm* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    static Vec3f D_80951D9C = { 64.0f, 0.0f, -122.0f };
    u8 pathIndex = ENGM_GET_PATH_INDEX(&this->actor);
    Vec3s* sp48;
    Vec3f sp3C;
    Vec3f sp30;
    s32 pad;
    s32 ret = false;

    this->timePath = NULL;

    if (D_80951A0C[scheduleOutput->result] >= 0) {
        this->timePath = SubS_GetAdditionalPath(play, pathIndex, D_80951A0C[scheduleOutput->result]);
    }

    if (this->timePath != NULL) {
        sp48 = Lib_SegmentedToVirtual(this->timePath->points);
        Math_Vec3s_ToVec3f(&sp3C, &sp48[this->timePath->count - 2]);
        Math_Vec3s_ToVec3f(&sp30, &sp48[this->timePath->count - 1]);
        this->actor.shape.shadowDraw = NULL;
        this->actor.world.rot.y = Math_Vec3f_Yaw(&sp3C, &sp30);
        if (this->scheduleResult == 0) {
            Math_Vec3f_Copy(&this->actor.world.pos, &D_80951D9C);
            SubS_SetOfferMode(&this->unk_3A4, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
            this->unk_3C8 = 4;
            this->unk_3CA = 4;
            this->unk_3CC = 8;
            EnGm_ChangeAnim(this, play, ENGM_ANIM_0);
            func_8094E278(play);
        } else {
            Math_Vec3f_Copy(&this->actor.world.pos, &sp30);
            EnGm_ChangeAnim(this, play, ENGM_ANIM_9);
            this->skelAnime.movementFlags = ANIM_FLAG_NOMOVE;
        }
        this->unk_400 = 0;
        this->unk_3A4 |= 0x100;
        this->unk_3A4 |= 0x200;
        ret = true;
    }
    return ret;
}

s32 func_80950088(EnGm* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    static Vec3f D_80951DA8 = { 278.0f, 0.0f, 223.0f };
    static Vec3s D_80951DB4 = { 0x0000, 0xC000, 0x0000 };
    s32 pad;

    Math_Vec3f_Copy(&this->actor.world.pos, &D_80951DA8);
    Math_Vec3s_Copy(&this->actor.world.rot, &D_80951DB4);
    Math_Vec3s_Copy(&this->actor.shape.rot, &this->actor.world.rot);
    SubS_SetOfferMode(&this->unk_3A4, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    this->unk_3A4 |= (0x2000 | 0x100);
    this->unk_3A4 |= 0x200;
    EnGm_ChangeAnim(this, play, ENGM_ANIM_12);
    return true;
}

s32 func_80950120(EnGm* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    static Vec3f D_80951DBC = { -525.0f, 214.0f, 515.0f };
    static Vec3s D_80951DC8 = { 0x0000, 0x38E0, 0x0000 };
    s32 pad;

    Math_Vec3f_Copy(&this->actor.world.pos, &D_80951DBC);
    Math_Vec3s_Copy(&this->actor.world.rot, &D_80951DC8);
    Math_Vec3s_Copy(&this->actor.shape.rot, &this->actor.world.rot);
    SubS_SetOfferMode(&this->unk_3A4, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    this->unk_3A4 |= (0x800 | 0x100);
    this->unk_3A4 |= 0x200;
    EnGm_ChangeAnim(this, play, ENGM_ANIM_4);
    return true;
}

s32 func_809501B8(EnGm* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    static Vec3f D_80951DD0 = { -334.0f, 225.0f, 903.0f };
    static Vec3s D_80951DDC = { 0x0000, 0x7FFF, 0x0000 };
    s32 pad;

    Math_Vec3f_Copy(&this->actor.world.pos, &D_80951DD0);
    Math_Vec3s_Copy(&this->actor.world.rot, &D_80951DDC);
    Math_Vec3s_Copy(&this->actor.shape.rot, &this->actor.world.rot);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    SubS_SetOfferMode(&this->unk_3A4, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
    this->unk_3A4 |= (0x1000 | 0x100);
    this->unk_3A4 |= 0x200;
    this->unk_3C8 = 3;
    this->unk_3CA = 3;
    this->unk_3CC = 8;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->unk_3B4 = 60.0f;
    EnGm_ChangeAnim(this, play, ENGM_ANIM_10);
    return true;
}

s32 func_80950280(EnGm* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    s32 phi_v1;

    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->unk_3A4 = 0;
    this->unk_3C8 = 0;
    this->unk_3CA = 0;
    this->unk_3CC = 8;
    this->unk_3B4 = 40.0f;

    switch (scheduleOutput->result) {
        case 1:
            phi_v1 = func_8094FD88(this, play, scheduleOutput);
            break;

        case 2:
            phi_v1 = func_8094FE10(this, play, scheduleOutput);
            break;

        case 3:
            phi_v1 = func_8094FCC4(this, play, scheduleOutput);
            break;

        case 5:
            phi_v1 = func_8094FF04(this, play, scheduleOutput);
            break;

        case 6:
            phi_v1 = func_80950088(this, play, scheduleOutput);
            break;

        case 7:
            phi_v1 = func_809501B8(this, play, scheduleOutput);
            break;

        case 8:
            phi_v1 = func_80950120(this, play, scheduleOutput);
            break;

        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
            phi_v1 = func_8094F904(this, play, scheduleOutput);
            break;

        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
            phi_v1 = func_8094FAC4(this, play, scheduleOutput);
            break;

        default:
            phi_v1 = false;
            break;
    }

    return phi_v1;
}

s32 func_80950388(EnGm* this, PlayState* play) {
    Vec3f sp2C;
    Vec3f sp20;

    if ((this->unk_268 != NULL) && (this->unk_268->update != NULL)) {
        Math_Vec3f_Copy(&sp2C, &this->unk_268->world.pos);
        Math_Vec3f_Copy(&sp20, &this->actor.world.pos);
        this->actor.world.rot.y = Math_Vec3f_Yaw(&sp20, &sp2C);
    }

    return false;
}

s32 func_809503F8(EnGm* this, PlayState* play) {
    s32 pad;

    if (this->animIndex == ENGM_ANIM_9) {
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
            SubS_SetOfferMode(&this->unk_3A4, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
            EnGm_ChangeAnim(this, play, ENGM_ANIM_0);
        } else {
            AnimTaskQueue_AddActorMovement(play, &this->actor, &this->skelAnime, 1.0f);
        }
    }
    return false;
}

s32 func_80950490(EnGm* this, PlayState* play) {
    static s32 D_80951DE4[] = {
        ENGM_ANIM_1, ENGM_ANIM_5, ENGM_ANIM_5, ENGM_ANIM_1, ENGM_ANIM_6, ENGM_ANIM_5,
        ENGM_ANIM_1, ENGM_ANIM_5, ENGM_ANIM_6, ENGM_ANIM_1, ENGM_ANIM_5, ENGM_ANIM_6,
    };
    s32 pad;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_50_01) || CHECK_WEEKEVENTREG(WEEKEVENTREG_51_80)) {
        if (this->unk_400 == 0) {
            this->unk_3C8 = 1;
            this->unk_3CA = 1;
            this->unk_3CC = 8;
            this->unk_400 = 1;
            EnGm_ChangeAnim(this, play, ENGM_ANIM_3);
        }
        return false;
    }

    this->unk_400 = 0;

    switch (this->animIndex) {
        case ENGM_ANIM_9:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
                SubS_SetOfferMode(&this->unk_3A4, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
                this->unk_3C8 = 4;
                this->unk_3CA = 4;
                this->unk_3CC = 8;
                EnGm_ChangeAnim(this, play, ENGM_ANIM_0);
                func_8094E278(play);
            } else {
                AnimTaskQueue_AddActorMovement(play, &this->actor, &this->skelAnime, 1.0f);
            }
            break;

        case ENGM_ANIM_5:
        case ENGM_ANIM_6:
            if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
                EnGm_ChangeAnim(this, play, D_80951DE4[this->unk_3F4]);
                this->unk_3F4++;
                this->unk_3F4 %= 12;
                this->unk_3DE = Rand_S16Offset(30, 30);
            }
            break;

        default:
            if (DECR(this->unk_3DE) == 0) {
                EnGm_ChangeAnim(this, play, D_80951DE4[this->unk_3F4]);
                this->unk_3F4++;
                this->unk_3F4 %= 12;
                this->unk_3DE = Rand_S16Offset(30, 30);
            }
            break;
    }
    return false;
}

s32 func_80950690(EnGm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* al;
    Actor* toto;

    switch (this->scheduleResult) {
        case 2:
            al = EnGm_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_AL);
            toto = EnGm_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_TOTO);
            if ((al != NULL) && (al->update != NULL) && (toto != NULL) && (toto->update != NULL) &&
                !(player->stateFlags1 & PLAYER_STATE1_TALKING)) {
                if (DECR(this->unk_3B8) == 0) {
                    if (al == this->unk_268) {
                        this->unk_268 = toto;
                    } else {
                        this->unk_268 = al;
                    }
                    this->unk_3B8 = Rand_S16Offset(60, 60);
                }
            }
            break;

        case 7:
            this->unk_3D0 += 992;
            if (DECR(this->unk_3B8) == 0) {
                Actor_PlaySfx(&this->actor, NA_SE_VO_GO_SLEEP);
                this->unk_3B8 = 30;
            }
            break;

        case 8:
        default:
            break;
    }

    SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, ENGM_FIDGET_TABLE_LEN);

    return false;
}

s32 func_80950804(EnGm* this, PlayState* play) {
    EnDoor* door;
    Vec3f sp38;
    s32 pad;
    f32 temp_f0;

    door = EnGm_FindScheduleDoor(play, this->scheduleResult);
    if (!SubS_InCsMode(play) && (this->timePathTimeSpeed != 0)) {
        if ((door != NULL) && (door->knobDoor.dyna.actor.update != NULL)) {
            if ((this->unk_3BA / (f32)this->unk_3B8) <= 0.9f) {
                door->openTimer = this->unk_261;
            } else {
                door->openTimer = 0;
            }
        }

        this->unk_3BA = CLAMP(this->unk_3BA, 0, this->unk_3B8);
        temp_f0 = Math_Vec3f_DistXZ(&this->unk_278, &this->unk_284) / this->unk_3B8;
        sp38.x = 0.0f;
        sp38.y = 0.0f;
        sp38.z = this->unk_3BA * temp_f0;
        Lib_Vec3f_TranslateAndRotateY(&this->unk_278, this->actor.world.rot.y, &sp38, &this->actor.world.pos);
        this->unk_3BA += this->timePathTimeSpeed;
        if (Animation_OnFrame(&this->skelAnime, 3.0f) || Animation_OnFrame(&this->skelAnime, 13.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_PIRATE_WALK);
        }
    }

    return false;
}

s32 func_8095097C(EnGm* this, PlayState* play) {
    f32 knots[265];
    Vec3f sp70;
    Vec3f sp64;
    Vec3f timePathTargetPos;
    s32 sp54 = 0;
    s32 sp50 = 0;
    s32 pad2;

    SubS_TimePathing_FillKnots(knots, SUBS_TIME_PATHING_ORDER, this->timePath->count + SUBS_TIME_PATHING_ORDER);

    if (!(this->unk_3A4 & 8)) {
        timePathTargetPos = gZeroVec3f;
        SubS_TimePathing_Update(this->timePath, &this->timePathProgress, &this->timePathElapsedTime,
                                this->timePathWaypointTime, this->timePathTotalTime, &this->timePathWaypoint, knots,
                                &timePathTargetPos, this->timePathTimeSpeed);
        SubS_TimePathing_ComputeInitialY(play, this->timePath, this->timePathWaypoint, &timePathTargetPos);
        this->actor.world.pos.y = timePathTargetPos.y;
        this->unk_3A4 |= 8;
    } else {
        timePathTargetPos = this->timePathTargetPos;
    }

    this->actor.world.pos.x = timePathTargetPos.x;
    this->actor.world.pos.z = timePathTargetPos.z;

    if (SubS_InCsMode(play)) {
        sp54 = this->timePathElapsedTime;
        sp50 = this->timePathWaypoint;
        timePathTargetPos = this->actor.world.pos;
    }

    this->timePathTargetPos = gZeroVec3f;

    if (SubS_TimePathing_Update(this->timePath, &this->timePathProgress, &this->timePathElapsedTime,
                                this->timePathWaypointTime, this->timePathTotalTime, &this->timePathWaypoint, knots,
                                &this->timePathTargetPos, this->timePathTimeSpeed)) {
        this->unk_3A4 |= 0x10;
    } else {
        sp70 = this->actor.world.pos;
        sp64 = this->timePathTargetPos;
        this->actor.world.rot.y = Math_Vec3f_Yaw(&sp70, &sp64);
    }

    if (SubS_InCsMode(play)) {
        this->timePathElapsedTime = sp54;
        this->timePathWaypoint = sp50;
        this->timePathTargetPos = timePathTargetPos;
    } else if (Animation_OnFrame(&this->skelAnime, 3.0f) || Animation_OnFrame(&this->skelAnime, 13.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_PIRATE_WALK);
    }
    return false;
}

void func_80950C24(EnGm* this, PlayState* play) {
    switch (this->scheduleResult) {
        case 1:
            func_80950388(this, play);
            break;

        case 2:
        case 6:
        case 7:
        case 8:
            func_80950690(this, play);
            break;

        case 3:
            func_809503F8(this, play);
            break;

        case 5:
            func_80950490(this, play);
            break;

        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
            func_80950804(this, play);
            break;

        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
            func_8095097C(this, play);
            break;

        default:
            break;
    }

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 3, 0x2AA8);
}

void func_80950CDC(EnGm* this, PlayState* play) {
    ScheduleOutput scheduleOutput;

    this->timePathTimeSpeed = R_TIME_SPEED + ((void)0, gSaveContext.save.timeSpeedOffset);

    if (!Schedule_RunScript(play, D_80951820, &scheduleOutput) ||
        ((this->scheduleResult != scheduleOutput.result) && !func_80950280(this, play, &scheduleOutput))) {
        this->actor.shape.shadowDraw = NULL;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        scheduleOutput.result = 0;
    } else {
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
    this->scheduleResult = scheduleOutput.result;
    this->unk_268 = func_8094F074(this, play);
    func_80950C24(this, play);
}

void func_80950DB8(EnGm* this, PlayState* play) {
    s32 pad;
    Vec3f sp40;
    Vec3f sp34;
    Actor* al;

    if (MsgEvent_RunScript(&this->actor, play, this->msgScript, this->msgScriptCallback, &this->msgScriptPos)) {
        SubS_SetOfferMode(&this->unk_3A4, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        al = EnGm_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_AL);
        if ((this->scheduleResult == 2) && (al != NULL) && (al->update != NULL)) {
            this->unk_268 = al;
            this->unk_3B8 = Rand_S16Offset(60, 60);
        } else {
            this->unk_3A4 &= ~0x20;
        }
        this->unk_3A4 |= 0x200;
        this->unk_3C6 = 20;
        this->msgScriptPos = 0;
        this->actionFunc = func_80950CDC;
    } else if ((this->scheduleResult != 1) && (this->scheduleResult != 2) && (this->scheduleResult != 3) &&
               (this->scheduleResult != 5) && (this->scheduleResult != 7)) {
        if ((this->unk_268 != NULL) && (this->unk_268->update != NULL)) {
            Math_Vec3f_Copy(&sp40, &this->unk_268->world.pos);
            Math_Vec3f_Copy(&sp34, &this->actor.world.pos);
            Math_ApproachS(&this->actor.shape.rot.y, Math_Vec3f_Yaw(&sp34, &sp40), 4, 0x2AA8);
        }
    }
    SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, ENGM_FIDGET_TABLE_LEN);
}

void func_80950F2C(EnGm* this, PlayState* play) {
    s32 csAnimIndex[] = { ENGM_ANIM_0, ENGM_ANIM_0, ENGM_ANIM_3, ENGM_ANIM_2 };
    Player* player = GET_PLAYER(play);
    s32 pad;
    Vec3f sp3C;
    Vec3f sp30;
    s32 cueId;
    s16 yaw;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_526)) {
        cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_526)]->id;
        if (this->cueId != (u8)cueId) {
            if (cueId == 3) {
                Actor_PlaySfx(&this->actor, NA_SE_EV_CHAIR_ROLL);
            }
            this->cueId = cueId;
            EnGm_ChangeAnim(this, play, csAnimIndex[cueId]);
        }

        if ((this->cueId == 3) && (this->unk_268 != NULL) && (this->unk_268->update != NULL)) {
            Math_Vec3f_Copy(&sp3C, &player->actor.world.pos);
            Math_Vec3f_Copy(&sp30, &this->actor.world.pos);
            yaw = Math_Vec3f_Yaw(&sp30, &sp3C);
            if ((this->actor.shape.rot.y / 0x16C) != (yaw / 0x16C)) {
                Math_ApproachS(&this->actor.shape.rot.y, yaw, 3, 0x2AA8);
                this->unk_3A4 &= ~0x20;
            } else {
                this->actor.shape.rot.y = yaw;
            }
        }
    }
}

void EnGm_Init(Actor* thisx, PlayState* play) {
    EnGm* this = (EnGm*)thisx;

    if (EnGm_FindActor(this, play, ACTORCAT_NPC, ACTOR_EN_GM)) {
        Actor_Kill(&this->actor);
        return;
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 22.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_in2_Skel_0078B0, NULL, this->jointTable, this->morphTable,
                       OBJECT_IN2_LIMB_MAX);
    this->animIndex = ENGM_ANIM_NONE;
    EnGm_ChangeAnim(this, play, ENGM_ANIM_0);
    Collider_InitAndSetCylinder(play, &this->colliderCylinder, &this->actor, &sCylinderInit);
    Collider_InitAndSetSphere(play, &this->colliderSphere, &this->actor, &sSphereInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.room = -1;
    this->unk_260 = -128;
    this->unk_3FC = 0;
    this->scheduleResult = 0;
    this->actionFunc = func_80950CDC;
    this->actionFunc(this, play);
}

void EnGm_Destroy(Actor* thisx, PlayState* play) {
    EnGm* this = (EnGm*)thisx;

    Collider_DestroyCylinder(play, &this->colliderCylinder);
    Collider_DestroySphere(play, &this->colliderSphere);
}

void EnGm_Update(Actor* thisx, PlayState* play) {
    EnGm* this = (EnGm*)thisx;

    if (!func_8094E0F8(this, play)) {
        if (!func_8094EE84(this, play) && func_8094EFC4(this, play)) {
            func_80950F2C(this, play);
            EnGm_UpdateSkelAnime(this, play);
            func_8094E2D0(this);
            return;
        }

        this->actionFunc(this, play);

        func_8094F53C(this, play);

        if (this->scheduleResult != 0) {
            EnGm_UpdateSkelAnime(this, play);
            func_8094E2D0(this);
            func_8094F2E8(this);
            SubS_Offer(&this->actor, play, this->unk_3B4, 30.0f, PLAYER_IA_NONE, this->unk_3A4 & SUBS_OFFER_MODE_MASK);
            if ((this->scheduleResult != 3) && (this->scheduleResult != 5) && (this->scheduleResult != 8)) {
                Actor_MoveWithGravity(&this->actor);
                Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
            }
            func_8094E1DC(this, play);
        }
    }
}

s32 EnGm_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    s32 pad;
    EnGm* this = (EnGm*)thisx;
    s32 fidgetIndex;

    if (limbIndex == OBJECT_IN2_LIMB_10) {
        func_8094F3D0(this, play);
    }

    switch (limbIndex) {
        case OBJECT_IN2_LIMB_09:
            fidgetIndex = 0;
            break;

        case OBJECT_IN2_LIMB_0A:
            fidgetIndex = 1;
            break;

        case OBJECT_IN2_LIMB_0D:
            fidgetIndex = 2;
            break;

        default:
            fidgetIndex = 9;
            break;
    }

    if ((this->unk_3A4 & 0x2000) && (fidgetIndex < 9)) {
        rot->y += TRUNCF_BINANG(Math_SinS(this->fidgetTableY[fidgetIndex]) * 200.0f);
        rot->z += TRUNCF_BINANG(Math_CosS(this->fidgetTableZ[fidgetIndex]) * 200.0f);
    }

    return false;
}

void EnGm_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f D_80951E24 = { 1400.0f, 0.0f, 0.0f };
    EnGm* this = (EnGm*)thisx;
    s32 pad[4];
    Vec3f sp30;
    s32 pad2;

    if ((CutsceneManager_GetCurrentCsId() == CS_ID_NONE) && (limbIndex == OBJECT_IN2_LIMB_10)) {
        Matrix_MultVec3f(&D_80951E24, &this->actor.focus.pos);
        Math_Vec3s_Copy(&this->actor.focus.rot, &this->actor.world.rot);
    }

    OPEN_DISPS(play->state.gfxCtx);

    if ((limbIndex == OBJECT_IN2_LIMB_0F) && (this->unk_3A4 & 0x800)) {
        gSPDisplayList(POLY_OPA_DISP++, object_in2_DL_007528);
    }

    CLOSE_DISPS(play->state.gfxCtx);

    if (limbIndex == OBJECT_IN2_LIMB_09) {
        Matrix_MultVec3f(&gZeroVec3f, &sp30);
        Math_Vec3f_ToVec3s(&this->colliderSphere.dim.worldSphere.center, &sp30);
    }
}

void EnGm_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnGm* this = (EnGm*)thisx;
    s32 overrideRot = true;
    s32 stepRot = false;

    if (!(this->unk_3A4 & 0x200)) {
        if (this->unk_3A4 & 0x80) {
            overrideRot = true;
            stepRot = true;
        } else {
            overrideRot = false;
            stepRot = true;
        }
    } else {
        overrideRot = false;
    }

    if (limbIndex == OBJECT_IN2_LIMB_10) {
        SubS_UpdateLimb(BINANG_ADD(this->unk_3BC + this->unk_3C0, 0x4000),
                        BINANG_ADD(this->unk_3BE + this->unk_3C2 + this->actor.shape.rot.y, 0x4000), &this->unk_290,
                        &this->unk_2A8, stepRot, overrideRot);
        Matrix_Pop();
        Matrix_Translate(this->unk_290.x, this->unk_290.y, this->unk_290.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_2A8.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_2A8.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_2A8.z, MTXMODE_APPLY);
        Matrix_Push();
    } else if (limbIndex == OBJECT_IN2_LIMB_09) {
        SubS_UpdateLimb(BINANG_ADD(this->unk_3C0, 0x4000), BINANG_ADD(this->unk_3C2 + this->actor.shape.rot.y, 0x4000),
                        &this->unk_29C, &this->unk_2AE, stepRot, overrideRot);
        Matrix_Pop();
        Matrix_Translate(this->unk_29C.x, this->unk_29C.y, this->unk_29C.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_2AE.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_2AE.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_2AE.z, MTXMODE_APPLY);
        Matrix_Push();
    }
}

void EnGm_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr D_80951E30[] = {
        object_in2_Tex_0054A8, object_in2_Tex_005028, object_in2_Tex_006828,
        object_in2_Tex_005028, object_in2_Tex_005CE8, object_in2_Tex_006C68,
    };
    EnGm* this = (EnGm*)thisx;

    if ((this->scheduleResult != 0) && (this->objectSlot > OBJECT_SLOT_NONE)) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80951E30[this->unk_3CE]));

        SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       this->skelAnime.dListCount, EnGm_OverrideLimbDraw, EnGm_PostLimbDraw,
                                       EnGm_TransformLimbDraw, &this->actor);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
