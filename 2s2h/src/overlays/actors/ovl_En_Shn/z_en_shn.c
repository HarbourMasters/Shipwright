/*
 * File: z_en_shn.c
 * Overlay: ovl_En_Shn
 * Description: Swamp Tourist Center Guide
 * Shashin(Shn) ~ Photo
 */

#include "z_en_shn.h"
#include "z64snap.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnShn_Init(Actor* thisx, PlayState* play);
void EnShn_Destroy(Actor* thisx, PlayState* play);
void EnShn_Update(Actor* thisx, PlayState* play);
void EnShn_Draw(Actor* thisx, PlayState* play);

void func_80AE6A64(EnShn* this, PlayState* play);

static MsgScript D_80AE6F00[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_23_08, 0x0010 - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09C4),
    /* 0x0008 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0009 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09C5),
    /* 0x000C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000D 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_23_08),
    /* 0x0010 0x03 */ MSCRIPT_CMD_CHECK_DEKU(0x019F - 0x0013),
    /* 0x0013 0x03 */ MSCRIPT_CMD_CHECK_HUMAN(0x01A4 - 0x0016),
    /* 0x0016 0x03 */ MSCRIPT_CMD_JUMP(0x019F - 0x0019),
    /* 0x0019 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x00FF),
    /* 0x001C 0x09 */ MSCRIPT_CMD_CHECK_ITEM_ACTION(PLAYER_IA_PICTOGRAPH_BOX, 0x0, 0x013A - 0x0025, 0x016C - 0x0025),
    /* 0x0025 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x003A - 0x0028),
    /* 0x0028 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x0044 - 0x002B),
    /* 0x002B 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x005B - 0x002E),
    /* 0x002E 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x008E - 0x0031),
    /* 0x0031 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x00C7 - 0x0034),
    /* 0x0034 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x006A - 0x0037),
    /* 0x0037 0x03 */ MSCRIPT_CMD_JUMP(0x0081 - 0x003A),
    /* 0x003A 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09E7),
    /* 0x003D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003E 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0041 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0042 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0043 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0044 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09CB),
    /* 0x0047 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0048 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x004B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004C 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09CC),
    /* 0x004F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0050 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09CD),
    /* 0x0053 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0054 0x07 */ MSCRIPT_CMD_CHECK_TEXT_CHOICE(0x0100 - 0x005B, 0x0126 - 0x005B, 0x0126 - 0x005B),
    /* 0x005B 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09D1),
    /* 0x005E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x005F 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0062 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0063 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09D2),
    /* 0x0066 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0067 0x03 */ MSCRIPT_CMD_JUMP(0x0050 - 0x006A),

    /* 0x006A 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09D3),
    /* 0x006D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x006E 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0071 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0072 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09D4),
    /* 0x0075 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0076 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09D5),
    /* 0x0079 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x007A 0x07 */ MSCRIPT_CMD_CHECK_TEXT_CHOICE(0x0113 - 0x0081, 0x0133 - 0x0081, 0x0133 - 0x0081),
    /* 0x0081 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09D9),
    /* 0x0084 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0085 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0088 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0089 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09DA),
    /* 0x008C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x008D 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x008E 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09E8),
    /* 0x0091 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0092 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0095 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0096 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09E9),
    /* 0x0099 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x009A 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09EA),
    /* 0x009D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x009E 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_87_04, 0x00BA - 0x00A3),
    /* 0x00A3 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00A4 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_HEART_PIECE, 0x0),
    /* 0x00A9 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x000C),
    /* 0x00AC 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00AD 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_87_04),
    /* 0x00B0 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x00B3 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09EB),
    /* 0x00B6 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00B7 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00B8 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x00B9 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x00BA 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00BB 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_RUPEE_SILVER, 0x0),
    /* 0x00C0 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0006),
    /* 0x00C3 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00C4 0x03 */ MSCRIPT_CMD_AUTOTALK(0x00B3 - 0x00C7),

    /* 0x00C7 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09EC),
    /* 0x00CA 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00CB 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x00CE 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00CF 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09ED),
    /* 0x00D2 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00D3 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09EE),
    /* 0x00D6 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00D7 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_87_04, 0x00F3 - 0x00DC),
    /* 0x00DC 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00DD 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_HEART_PIECE, 0x0),
    /* 0x00E2 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x000C),
    /* 0x00E5 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00E6 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_87_04),
    /* 0x00E9 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x00EC 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09EF),
    /* 0x00EF 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00F0 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00F1 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x00F2 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x00F3 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00F4 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_RUPEE_SILVER, 0x0),
    /* 0x00F9 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0006),
    /* 0x00FC 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00FD 0x03 */ MSCRIPT_CMD_AUTOTALK(0x00EC - 0x0100),

    /* 0x0100 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0101 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_RUPEE_RED, 0x0),
    /* 0x0106 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0004),
    /* 0x0109 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x010A 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x010D 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09CE),
    /* 0x0110 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0111 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x0112 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0113 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0114 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_RUPEE_BLUE, 0x0),
    /* 0x0119 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0002),
    /* 0x011C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x011D 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x0120 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09D8),
    /* 0x0123 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0124 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x0125 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0126 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0127 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09CF),
    /* 0x012A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x012B 0x03 */ MSCRIPT_CMD_SET_EVENT_INF(EVENTINF_50),
    /* 0x012E 0x01 */ MSCRIPT_CMD_PAUSE(),
    /* 0x012F 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0132 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0133 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09D6),
    /* 0x0136 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0137 0x03 */ MSCRIPT_CMD_JUMP(0x012B - 0x013A),

    /* 0x013A 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_55_40, 0x015B - 0x013F),
    /* 0x013F 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09DB),
    /* 0x0142 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0143 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0146 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0147 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09DC),
    /* 0x014A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x014B 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09DD),
    /* 0x014E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x014F 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09DE),
    /* 0x0152 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0153 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09DF),
    /* 0x0156 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0157 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_55_40),
    /* 0x015A 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x015B 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09E0),
    /* 0x015E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x015F 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0162 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0163 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09E1),
    /* 0x0166 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0167 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09E2),
    /* 0x016A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x016B 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x016C 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_55_40, 0x0189 - 0x0171),
    /* 0x0171 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09DB),
    /* 0x0174 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0175 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09DC),
    /* 0x0178 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0179 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09DD),
    /* 0x017C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x017D 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09DE),
    /* 0x0180 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0181 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09DF),
    /* 0x0184 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0185 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_55_40),
    /* 0x0188 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0189 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09E0),
    /* 0x018C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x018D 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09E1),
    /* 0x0190 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0191 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09E2),
    /* 0x0194 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0195 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0196 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09C9),
    /* 0x0199 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x019A 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09CA),
    /* 0x019D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x019E 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x019F 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09C6),
    /* 0x01A2 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x01A3 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x01A4 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE, 0x0196 - 0x01A9),
    /* 0x01A9 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09C8),
    /* 0x01AC 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x01AD 0x03 */ MSCRIPT_CMD_JUMP(0x0019 - 0x01B0),
};

static MsgScript D_80AE70B0[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09D0),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x0005 0x01 */ MSCRIPT_CMD_DONE(),
};

static MsgScript D_80AE70B8[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x0015 - 0x0003),
    /* 0x0003 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x001B - 0x0006),
    /* 0x0006 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x0031 - 0x0009),
    /* 0x0009 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x0061 - 0x000C),
    /* 0x000C 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x0099 - 0x000F),
    /* 0x000F 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK_CONTINUE(0x003F - 0x0012),
    /* 0x0012 0x03 */ MSCRIPT_CMD_JUMP(0x0055 - 0x0015),
    /* 0x0015 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09E7),
    /* 0x0018 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0019 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x001A 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x001B 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09CB),
    /* 0x001E 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0021 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0022 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09CC),
    /* 0x0025 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0026 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09CD),
    /* 0x0029 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002A 0x07 */ MSCRIPT_CMD_CHECK_TEXT_CHOICE(0x00D1 - 0x0031, 0x00F7 - 0x0031, 0x00F7 - 0x0031),
    /* 0x0031 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09D1),
    /* 0x0034 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0037 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0038 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09D2),
    /* 0x003B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x003C 0x03 */ MSCRIPT_CMD_JUMP(0x0026 - 0x003F),

    /* 0x003F 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09D3),
    /* 0x0042 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0045 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0046 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09D4),
    /* 0x0049 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004A 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09D5),
    /* 0x004D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x004E 0x07 */ MSCRIPT_CMD_CHECK_TEXT_CHOICE(0x00E4 - 0x0055, 0x0104 - 0x0055, 0x0104 - 0x0055),
    /* 0x0055 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09D9),
    /* 0x0058 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x005B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x005C 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09DA),
    /* 0x005F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0060 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0061 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09E8),
    /* 0x0064 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0067 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0068 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09E9),
    /* 0x006B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x006C 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09EA),
    /* 0x006F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0070 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_87_04, 0x008C - 0x0075),
    /* 0x0075 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0076 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_HEART_PIECE, 0x0),
    /* 0x007B 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x000C),
    /* 0x007E 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x007F 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_87_04),
    /* 0x0082 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x0085 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09EB),
    /* 0x0088 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0089 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x008A 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x008B 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x008C 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x008D 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_RUPEE_SILVER, 0x0),
    /* 0x0092 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0006),
    /* 0x0095 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0096 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0085 - 0x0099),

    /* 0x0099 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09EC),
    /* 0x009C 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x009F 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00A0 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09ED),
    /* 0x00A3 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00A4 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09EE),
    /* 0x00A7 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00A8 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_87_08, 0x00C4 - 0x00AD),
    /* 0x00AD 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00AE 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_HEART_PIECE, 0x0),
    /* 0x00B3 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x000C),
    /* 0x00B6 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00B7 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_87_08),
    /* 0x00BA 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x00BD 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09EF),
    /* 0x00C0 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00C1 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00C2 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x00C3 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x00C4 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00C5 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_RUPEE_SILVER, 0x0),
    /* 0x00CA 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0006),
    /* 0x00CD 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00CE 0x03 */ MSCRIPT_CMD_AUTOTALK(0x00BD - 0x00D1),

    /* 0x00D1 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00D2 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_RUPEE_RED, 0x0),
    /* 0x00D7 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0004),
    /* 0x00DA 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00DB 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x00DE 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09CE),
    /* 0x00E1 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00E2 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x00E3 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x00E4 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x00E5 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_RUPEE_BLUE, 0x0),
    /* 0x00EA 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0002),
    /* 0x00ED 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00EE 0x03 */ MSCRIPT_CMD_AUTOTALK(0x0),
    /* 0x00F1 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09D8),
    /* 0x00F4 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00F5 0x01 */ MSCRIPT_CMD_UNSET_AUTOTALK(),
    /* 0x00F6 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x00F7 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00F8 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09CF),
    /* 0x00FB 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x00FC 0x03 */ MSCRIPT_CMD_SET_EVENT_INF(EVENTINF_50),
    /* 0x00FF 0x01 */ MSCRIPT_CMD_PAUSE(),
    /* 0x0100 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0103 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0104 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x09D6),
    /* 0x0107 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0108 0x03 */ MSCRIPT_CMD_JUMP(0x00FC - 0x010B),
};

static MsgScript D_80AE71C4[] = {
    /* 0x0000 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_55_40, 0x0021 - 0x0005),
    /* 0x0005 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09DB),
    /* 0x0008 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0009 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x000C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x000D 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09DC),
    /* 0x0010 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0011 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09DD),
    /* 0x0014 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0015 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09DE),
    /* 0x0018 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0019 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09DF),
    /* 0x001C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001D 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_55_40),
    /* 0x0020 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0021 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x09E0),
    /* 0x0024 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0025 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0028 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0029 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09E1),
    /* 0x002C 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x002D 0x03 */ MSCRIPT_CMD_CONTINUE_TEXT(0x09E2),
    /* 0x0030 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0031 0x01 */ MSCRIPT_CMD_DONE(),
};

ActorProfile En_Shn_Profile = {
    /**/ ACTOR_EN_SHN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SHN,
    /**/ sizeof(EnShn),
    /**/ EnShn_Init,
    /**/ EnShn_Destroy,
    /**/ EnShn_Update,
    /**/ EnShn_Draw,
};

void EnShn_UpdateSkelAnime(EnShn* this) {
    this->skelAnime.playSpeed = this->animPlaySpeed;
    SkelAnime_Update(&this->skelAnime);
}

typedef enum EnShnAnimation {
    /* -1 */ ENSHN_ANIM_NONE = -1,
    /*  0 */ ENSHN_ANIM_0,
    /*  1 */ ENSHN_ANIM_1,
    /*  2 */ ENSHN_ANIM_2,
    /*  3 */ ENSHN_ANIM_3,
    /*  4 */ ENSHN_ANIM_MAX
} EnShnAnimation;

static AnimationInfoS sAnimationInfo[ENSHN_ANIM_MAX] = {
    { &gBurlyGuyHandsOnTableAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENSHN_ANIM_0
    { &gBurlyGuyHandsOnTableAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENSHN_ANIM_1
    { &gBurlyGuyChinScratchAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },   // ENSHN_ANIM_2
    { &gBurlyGuyChinScratchAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },  // ENSHN_ANIM_3
};

s32 EnShn_ChangeAnim(EnShn* this, s32 animIndex) {
    s32 changeAnim = false;
    s32 didAnimChange = false;

    if (this->animIndex != animIndex) {
        changeAnim = true;
    }

    if (changeAnim) {
        this->animIndex = animIndex;
        didAnimChange = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
        this->animPlaySpeed = this->skelAnime.playSpeed;
    }
    return didAnimChange;
}

s32 EnShn_IsFacingPlayer(EnShn* this) {
    s16 phi_v1;
    f32 range;

    if (this->unk_2C8 == 0) {
        phi_v1 = 0;
    } else {
        this->unk_2C8--;
        phi_v1 = this->unk_2C8;
    }
    if (phi_v1 == 0) {
        this->unk_2EC ^= 1;
        this->unk_2C8 = Rand_S16Offset(30, 30);
    }
    if (this->unk_2EC != 0) {
        range = 120.0f;
    } else {
        range = 80.0f;
    }
    return Actor_IsFacingAndNearPlayer(&this->actor, range, 0x238C);
}

Player* EnShn_GetPlayer(EnShn* this, PlayState* play) {
    return GET_PLAYER(play);
}

void func_80AE626C(EnShn* this) {
    s32 topPad;
    Vec3f playerPos;
    Vec3f shnPos;
    s32 bottomPad;

    Math_Vec3f_Copy(&playerPos, &this->shnPlayerRef->actor.world.pos);
    Math_Vec3f_Copy(&shnPos, &this->actor.world.pos);
    Math_ApproachS(&this->unk_2BC, (Math_Vec3f_Yaw(&shnPos, &playerPos) - this->actor.shape.rot.y), 4, 0x2AA8);
    this->unk_2BC = CLAMP(this->unk_2BC, -0x1FFE, 0x1FFE);
    Math_Vec3f_Copy(&shnPos, &this->actor.focus.pos);
    if (this->shnPlayerRef->actor.id == ACTOR_PLAYER) {
        playerPos.y = this->shnPlayerRef->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    } else {
        Math_Vec3f_Copy(&playerPos, &this->shnPlayerRef->actor.focus.pos);
    }
    Math_ApproachS(&this->unk_2BA, Math_Vec3f_Pitch(&shnPos, &playerPos), 4, 0x2AA8);
    this->unk_2BA = CLAMP(this->unk_2BA, -0x1554, 0x1554);
}

void func_80AE63A8(EnShn* this, PlayState* play) {
    s16 phi_v1;
    s16 phi_v1_2;

    this->shnPlayerRef = EnShn_GetPlayer(this, play);
    if ((this->unk_1D8 & 8) && (this->shnPlayerRef != NULL)) {
        if (this->unk_2CA == 0) {
            phi_v1 = 0;
        } else {
            this->unk_2CA--;
            phi_v1 = this->unk_2CA;
        }
        if (phi_v1 == 0) {
            func_80AE626C(this);
            this->unk_1D8 &= ~0x20;
            this->unk_1D8 |= 0x10;
            return;
        }
    }
    if (this->unk_1D8 & 0x10) {
        this->unk_1D8 &= ~0x10;
        this->unk_2BA = 0;
        this->unk_2BC = 0;
        this->unk_2CA = 20;
        return;
    }
    if (this->unk_2CA == 0) {
        phi_v1_2 = 0;
    } else {
        this->unk_2CA--;
        phi_v1_2 = this->unk_2CA;
    }
    if (phi_v1_2 == 0) {
        this->unk_1D8 |= 0x20;
    }
}

void func_80AE6488(EnShn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 talkState = Message_GetState(&play->msgCtx);
    f32 phi_f0_2;
    f32 phi_f0;

    this->unk_2D4 += (this->unk_2D0 != 0.0f) ? 40.0f : -40.0f;
    this->unk_2D4 = CLAMP(this->unk_2D4, 0.0f, 80.0f);
    Matrix_Translate(this->unk_2D4, 0.0f, 0.0f, MTXMODE_APPLY);
    if ((&this->actor == player->talkActor) &&
        ((play->msgCtx.currentTextId < 0xFF) || (play->msgCtx.currentTextId >= 0x201)) &&
        (talkState == TEXT_STATE_FADING) && (this->prevTalkState == TEXT_STATE_FADING)) {
        if (play->state.frames % 2 == 0) {
            if (this->unk_2D0 != 0.0f) {
                this->unk_2D0 = 0.0f;
            } else {
                this->unk_2D0 = 1.0f;
            }
        }
    } else {
        this->unk_2D0 = 0.0f;
    }
    this->prevTalkState = talkState;
}

s32 func_80AE65F4(EnShn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u16 temp = play->msgCtx.currentTextId;

    if (player->stateFlags1 & PLAYER_STATE1_TALKING) {
        if (this->unk_1DA != temp) {
            if ((this->unk_1D8 & 0x80) || (this->unk_1D8 & 0x100)) {
                this->unk_1D8 |= 8;
                EnShn_ChangeAnim(this, ENSHN_ANIM_1);
            }
            if (temp == 0x9C5) {
                //! FAKE:
                if (1) {}

                this->unk_1D8 |= 8;
                EnShn_ChangeAnim(this, ENSHN_ANIM_1);
            }
        }
        this->unk_1DA = temp;
        this->unk_1D8 |= 0x40;
    } else if (this->unk_1D8 & 0x40) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_23_08)) {
            EnShn_ChangeAnim(this, ENSHN_ANIM_3);
        }
        this->unk_1DA = 0;
        this->unk_1D8 &= ~0x40;
    }
    return false;
}

s32 func_80AE6704(Actor* thisx, PlayState* play) {
    static s32 sPictographFlags[] = {
        PICTO_VALID_0,      PICTO_VALID_MONKEY,    PICTO_VALID_BIG_OCTO,
        PICTO_VALID_TINGLE, PICTO_VALID_DEKU_KING, PICTO_VALID_IN_SWAMP,
    };
    EnShn* this = (EnShn*)thisx;
    s32 ret = 0;

    switch (this->unk_2C6) {
        case 0:
            if (!CHECK_QUEST_ITEM(QUEST_PICTOGRAPH)) {
                this->unk_2C6 = 6;
                ret = 1;
            } else {
                this->unk_2C6++;
            }
            break;

        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            if (Snap_CheckFlag(sPictographFlags[this->unk_2C6])) {
                this->unk_2C6 = 6;
                ret = 1;
                REMOVE_QUEST_ITEM(QUEST_PICTOGRAPH);
            } else {
                this->unk_2C6++;
            }
            break;

        case 6:
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_90_40);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
            play->nextEntrance = ENTRANCE(SOUTHERN_SWAMP_POISONED, 6);
            gSaveContext.nextCutsceneIndex = 0;
            play->transitionTrigger = TRANS_TRIGGER_START;
            play->transitionType = TRANS_TYPE_FADE_WHITE;
            gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE_SLOW;
            this->unk_2C6++;
            break;

        default:
            break;
    }
    return ret;
}

MsgScript* EnShn_GetMsgScript(EnShn* this, PlayState* play) {
    if (this->unk_2BE != 0) {
        return D_80AE70B0;
    }
    if (this->unk_1D8 & 0x80) {
        this->msgScriptCallback = func_80AE6704;
        return D_80AE70B8;
    }
    if (this->unk_1D8 & 0x100) {
        return D_80AE71C4;
    }
    this->msgScriptCallback = func_80AE6704;
    return D_80AE6F00;
}

s32 func_80AE68F0(EnShn* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 ret = false;

    if (((this->unk_1D8 & SUBS_OFFER_MODE_MASK) != SUBS_OFFER_MODE_NONE) &&
        Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_1D8 &= ~0x180;
        if (player->exchangeItemAction == PLAYER_IA_PICTOGRAPH_BOX) {
            this->unk_1D8 |= 0x80;
            this->unk_2E4 = player->exchangeItemAction;
        } else if (player->exchangeItemAction != PLAYER_IA_NONE) {
            this->unk_1D8 |= 0x100;
            this->unk_2E4 = player->exchangeItemAction;
        }
        SubS_SetOfferMode(&this->unk_1D8, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
        this->msgScript = EnShn_GetMsgScript(this, play);
        this->unk_2C6 = 0;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_23_08)) {
            this->unk_1D8 |= 8;
        }
        this->actionFunc = func_80AE6A64;
        ret = true;
    }
    return ret;
}

void func_80AE69E8(EnShn* this, PlayState* play) {
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 3, 0x2AA8);
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_23_08) && EnShn_IsFacingPlayer(this)) {
        this->unk_1D8 |= 8;
    } else {
        this->unk_1D8 &= ~0x8;
    }
}

void func_80AE6A64(EnShn* this, PlayState* play) {
    s16 yawBetweenActors;
    Vec3f playerPos;
    Vec3f shnPos;

    if (MsgEvent_RunScript(&this->actor, play, this->msgScript, this->msgScriptCallback, &this->msgScriptPos)) {
        SubS_SetOfferMode(&this->unk_1D8, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk_1D8 &= ~8;
        this->unk_1D8 |= 0x20;
        this->unk_2CA = 20;
        this->msgScriptPos = 0;
        this->unk_2BE = 0;
        this->actionFunc = func_80AE69E8;
    } else if (this->shnPlayerRef != 0) {
        Math_Vec3f_Copy(&playerPos, &this->shnPlayerRef->actor.world.pos);
        Math_Vec3f_Copy(&shnPos, &this->actor.world.pos);
        yawBetweenActors = Math_Vec3f_Yaw(&shnPos, &playerPos);
        Math_ApproachS(&this->actor.shape.rot.y, yawBetweenActors, 4, 0x2AA8);
    }
}

void EnShn_Init(Actor* thisx, PlayState* play) {
    EnShn* this = (EnShn*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gBurlyGuySkel, NULL, this->jointTable, this->morphTable,
                       BURLY_GUY_LIMB_MAX);
    this->animIndex = ENSHN_ANIM_NONE;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_23_08)) {
        EnShn_ChangeAnim(this, ENSHN_ANIM_0);
    } else {
        EnShn_ChangeAnim(this, ENSHN_ANIM_2);
    }

    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    Actor_SetScale(&this->actor, 0.01f);
    this->unk_2E0 = 0;
    this->msgScriptCallback = NULL;
    this->unk_1D8 = 0;
    if (gSaveContext.save.entrance != ENTRANCE(TOURIST_INFORMATION, 2)) {
        SubS_SetOfferMode(&this->unk_1D8, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->unk_2BE = 0;
    } else {
        SubS_SetOfferMode(&this->unk_1D8, SUBS_OFFER_MODE_AUTO, SUBS_OFFER_MODE_MASK);
        this->unk_2BE = 1;
    }
    this->actionFunc = func_80AE69E8;
}

void EnShn_Destroy(Actor* thisx, PlayState* play) {
}

void EnShn_Update(Actor* thisx, PlayState* play) {
    EnShn* this = (EnShn*)thisx;

    func_80AE68F0(this, play);
    this->actionFunc(this, play);
    func_80AE65F4(this, play);
    EnShn_UpdateSkelAnime(this);
    func_80AE63A8(this, play);
    this->unk_2E0 = 0;
    SubS_Offer(&this->actor, play, 120.0f, 40.0f, PLAYER_IA_NONE, this->unk_1D8 & SUBS_OFFER_MODE_MASK);
}

s32 EnShn_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnShn* this = (EnShn*)thisx;

    if (limbIndex == BURLY_GUY_LIMB_HEAD) {
        func_80AE6488(this, play);
        *dList = gSwampGuideHeadDL;
    }
    return false;
}

Vec3f D_80AE7270 = { 1200.0f, 0.0f, 0.0f };
void EnShn_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    if (limbIndex == BURLY_GUY_LIMB_HEAD) {
        Matrix_MultVec3f(&D_80AE7270, &thisx->focus.pos);
        Math_Vec3s_Copy(&thisx->focus.rot, &thisx->world.rot);
    }
}

void EnShn_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
    EnShn* this = (EnShn*)thisx;
    s32 stepRot;
    s32 overrideRot;

    if (!(this->unk_1D8 & 0x20)) {
        if (this->unk_1D8 & 0x10) {
            overrideRot = true;
        } else {
            overrideRot = false;
        }
        stepRot = true;
    } else {
        overrideRot = false;
        stepRot = false;
    }

    if (limbIndex == BURLY_GUY_LIMB_HEAD) {
        SubS_UpdateLimb(this->unk_2BA + 0x4000, this->unk_2BC + this->actor.shape.rot.y + 0x4000, &this->unk_1E8,
                        &this->unk_1F4, stepRot, overrideRot);
        Matrix_Pop();
        Matrix_Translate(this->unk_1E8.x, this->unk_1E8.y, this->unk_1E8.z, MTXMODE_NEW);
        Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_RotateYS(this->unk_1F4.y, MTXMODE_APPLY);
        Matrix_RotateXS(this->unk_1F4.x, MTXMODE_APPLY);
        Matrix_RotateZS(this->unk_1F4.z, MTXMODE_APPLY);
        Matrix_Push();
    }
}

void EnShn_Draw(Actor* thisx, PlayState* play) {
    EnShn* this = (EnShn*)thisx;

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, EnShn_OverrideLimbDraw, EnShn_PostLimbDraw,
                                   EnShn_TransformLimbDraw, &this->actor);
}
