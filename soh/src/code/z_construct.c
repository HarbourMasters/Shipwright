#include "global.h"
#include <textures/do_action_static/do_action_static.h>

void func_80110990(PlayState* play) {
    Map_Destroy(play);
}

void func_801109B0(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    u32 parameterSize;
    u8 temp;

    gSaveContext.sunsSongState = SUNSSONG_INACTIVE;
    gSaveContext.unk_13E8 = gSaveContext.unk_13EA = 0;

    View_Init(&interfaceCtx->view, play->state.gfxCtx);

    interfaceCtx->unk_1FA = interfaceCtx->unk_261 = interfaceCtx->unk_1FC = 0;
    interfaceCtx->unk_1EC = interfaceCtx->unk_1EE = interfaceCtx->unk_1F0 = 0;
    interfaceCtx->unk_22E = 0;
    interfaceCtx->unk_230 = 16;
    interfaceCtx->unk_1F4 = 0.0f;
    interfaceCtx->unk_228 = XREG(95);
    interfaceCtx->minimapAlpha = 0;
    interfaceCtx->unk_260 = 0;
    interfaceCtx->unk_244 = interfaceCtx->aAlpha = interfaceCtx->bAlpha = interfaceCtx->cLeftAlpha =
        interfaceCtx->cDownAlpha = interfaceCtx->cRightAlpha = interfaceCtx->dpadUpAlpha = interfaceCtx->dpadDownAlpha =
            interfaceCtx->dpadLeftAlpha = interfaceCtx->dpadRightAlpha = interfaceCtx->healthAlpha =
                interfaceCtx->startAlpha = interfaceCtx->magicAlpha = 0;

    parameterSize = (uintptr_t)_parameter_staticSegmentRomEnd - (uintptr_t)_parameter_staticSegmentRomStart;

    // "Permanent PARAMETER Segment = %x"
    osSyncPrintf("常駐ＰＡＲＡＭＥＴＥＲセグメント=%x\n", parameterSize);

    interfaceCtx->parameterSegment = GAMESTATE_ALLOC_MC(&play->state, parameterSize);

    osSyncPrintf("parameter->parameterSegment=%x\n", interfaceCtx->parameterSegment);

    ASSERT(interfaceCtx->parameterSegment != NULL);
    DmaMgr_SendRequest1(interfaceCtx->parameterSegment, (uintptr_t)_parameter_staticSegmentRomStart, parameterSize,
                        __FILE__, 162);

    interfaceCtx->doActionSegment = GAMESTATE_ALLOC_MC(&play->state, 0x480);

    osSyncPrintf("ＤＯアクション テクスチャ初期=%x\n", 0x480); // "DO Action Texture Initialization"
    osSyncPrintf("parameter->do_actionSegment=%x\n", interfaceCtx->doActionSegment);

    ASSERT(interfaceCtx->doActionSegment != NULL);

    uint32_t attackDoActionTexSize = ResourceMgr_LoadTexSizeByName(gAttackDoActionENGTex);
    memcpy(interfaceCtx->doActionSegment, ResourceMgr_LoadTexByName(gAttackDoActionENGTex), attackDoActionTexSize);
    memcpy(interfaceCtx->doActionSegment + (attackDoActionTexSize / 2), ResourceMgr_LoadTexByName(gCheckDoActionENGTex), attackDoActionTexSize);

    memcpy(interfaceCtx->doActionSegment + attackDoActionTexSize, ResourceMgr_LoadTexByName(gReturnDoActionENGTex), ResourceMgr_LoadTexSizeByName(gReturnDoActionENGTex));

    interfaceCtx->iconItemSegment = GAMESTATE_ALLOC_MC(
        &play->state, 0x1000 * ARRAY_COUNT(gSaveContext.equips.buttonItems));

    // "Icon Item Texture Initialization = %x"
    osSyncPrintf("アイコンアイテム テクスチャ初期=%x\n", 0x4000);
    osSyncPrintf("parameter->icon_itemSegment=%x\n", interfaceCtx->iconItemSegment);

    ASSERT(interfaceCtx->iconItemSegment != NULL);

    osSyncPrintf("Register_Item[%x, %x, %x, %x]\n", gSaveContext.equips.buttonItems[0],
                 gSaveContext.equips.buttonItems[1], gSaveContext.equips.buttonItems[2],
                 gSaveContext.equips.buttonItems[3]);

    for (int buttonIndex = 0; buttonIndex < ARRAY_COUNT(gSaveContext.equips.buttonItems); buttonIndex++) {
        if (gSaveContext.equips.buttonItems[buttonIndex] < 0xF0) {
            DmaMgr_SendRequest1(interfaceCtx->iconItemSegment + 0x1000 * buttonIndex,
                                _icon_item_staticSegmentRomStart +
                                    gSaveContext.equips.buttonItems[buttonIndex] * 0x1000,
                                0x1000, __FILE__, __LINE__);
        } else if (buttonIndex == 0 && gSaveContext.equips.buttonItems[buttonIndex] != 0xFF) {
            DmaMgr_SendRequest1(interfaceCtx->iconItemSegment + 0x1000 * buttonIndex,
                                _icon_item_staticSegmentRomStart +
                                    gSaveContext.equips.buttonItems[buttonIndex] * 0x1000,
                                0x1000, __FILE__, __LINE__);
        }
    }

    osSyncPrintf("ＥＶＥＮＴ＝%d\n", ((void)0, gSaveContext.timer1State));

    if ((gSaveContext.timer1State == 4) || (gSaveContext.timer1State == 8) || (gSaveContext.timer2State == 4) ||
        (gSaveContext.timer2State == 10)) {
        osSyncPrintf("restart_flag=%d\n", ((void)0, gSaveContext.respawnFlag));

        if ((gSaveContext.respawnFlag == -1) || (gSaveContext.respawnFlag == 1)) {
            if (gSaveContext.timer1State == 4) {
                gSaveContext.timer1State = 1;
                gSaveContext.timerX[0] = 140;
                gSaveContext.timerY[0] = 80;
            }
        }

        if ((gSaveContext.timer1State == 4) || (gSaveContext.timer1State == 8)) {
            temp = 0;
        } else {
            temp = 1;
        }

        gSaveContext.timerX[temp] = 26;

        if (gSaveContext.healthCapacity > 0xA0) {
            gSaveContext.timerY[temp] = 54;
        } else {
            gSaveContext.timerY[temp] = 46;
        }
    }

    if ((gSaveContext.timer1State >= 11) && (gSaveContext.timer1State < 16)) {
        gSaveContext.timer1State = 0;
        // "Timer Stop!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf("タイマー停止！！！！！！！！！！！！！！！！！！！！！  = %d\n", gSaveContext.timer1State);
    }

    osSyncPrintf("ＰＡＲＡＭＥＴＥＲ領域＝%x\n", parameterSize + 0x5300); // "Parameter Area = %x"

    HealthMeter_Init(play);
    Map_Init(play);

    interfaceCtx->unk_23C = interfaceCtx->unk_242 = 0;

    R_ITEM_BTN_X(0) = B_BUTTON_X;
    R_B_BTN_COLOR(0) = 255;
    R_B_BTN_COLOR(1) = 30;
    R_B_BTN_COLOR(2) = 30;
    R_ITEM_ICON_X(0) = B_BUTTON_X;
    R_ITEM_AMMO_X(0) = B_BUTTON_X + 2;
    R_A_BTN_X = A_BUTTON_X;
    R_A_ICON_X = A_BUTTON_X;
    R_A_BTN_COLOR(0) = 0;
    R_A_BTN_COLOR(1) = 200;
    R_A_BTN_COLOR(2) = 50;
}

void Message_Init(PlayState* play) {
    MessageContext* msgCtx = &play->msgCtx;
    s32 pad;

    Message_SetTables();

    play->msgCtx.ocarinaMode = OCARINA_MODE_00;

    msgCtx->msgMode = MSGMODE_NONE;
    msgCtx->msgLength = 0;
    msgCtx->textId = msgCtx->textboxEndType = msgCtx->choiceIndex = msgCtx->ocarinaAction = msgCtx->textUnskippable = 0;
    msgCtx->textColorAlpha = 255;

    View_Init(&msgCtx->view, play->state.gfxCtx);

    msgCtx->textboxSegment = GAMESTATE_ALLOC_MC(&play->state, 0x2200);

    osSyncPrintf("message->fukidashiSegment=%x\n", msgCtx->textboxSegment);

    osSyncPrintf("吹き出しgame_alloc=%x\n", 0x2200); // "Textbox game_alloc=%x"
    ASSERT(msgCtx->textboxSegment != NULL);

    Font_LoadOrderedFont(&play->msgCtx.font);

    YREG(31) = 0;
}

void func_80111070(void) {
    YREG(8) = 10;
    YREG(14) = 0;
    YREG(15) = 0;
    R_TEXTBOX_TEXWIDTH = 0;
    R_TEXTBOX_TEXHEIGHT = 0;
    R_TEXTBOX_WIDTH = 50;
    R_TEXTBOX_HEIGHT = 0;
    YREG(24) = -60;
    YREG(25) = 13;
    YREG(26) = 15;
    YREG(27) = 41;
    YREG(28) = 15;
    YREG(32) = 265;
    YREG(33) = 55;
    YREG(34) = 0;
    YREG(35) = 20;
    YREG(36) = 0;
    YREG(37) = 0;
    YREG(38) = 0;
    YREG(40) = 2;
    YREG(41) = 1;
    YREG(42) = 2;
    YREG(43) = 1;
    YREG(44) = 0;
    YREG(45) = 236;
    YREG(46) = 36;
    YREG(47) = 0;
    YREG(48) = -45;
    YREG(49) = -48;
    YREG(50) = 16;
    YREG(51) = 22;
    YREG(52) = -55;
    YREG(53) = -53;
    YREG(54) = 43;
    YREG(55) = 47;
    YREG(56) = -33;
    YREG(57) = -42;
    YREG(58) = -33;
    YREG(59) = -37;
    YREG(60) = 14;
    YREG(61) = -2;
    YREG(62) = -2;
    YREG(63) = -18;
    YREG(64) = -18;
    YREG(67) = 0;
    YREG(68) = 0;
    YREG(69) = 0;
    YREG(70) = 0;
    R_TEXTBOX_ICON_XPOS = -6;
    R_TEXTBOX_ICON_YPOS = 10;
    YREG(73) = -8;
    YREG(74) = 8;
    R_TEXTBOX_ICON_SIZE = 24;
    YREG(76) = 32;
    YREG(77) = 0;
    R_MESSAGE_DEBUGGER_SELECT = 0;
    R_MESSAGE_DEBUGGER_TEXTID = 48;
    YREG(80) = 450;
    YREG(81) = 0;
    YREG(82) = -15;
    YREG(83) = 500;
    YREG(84) = 600;
    YREG(85) = 0;
    YREG(86) = -21;
    YREG(87) = 510;
    R_C_UP_ICON_X = C_UP_BUTTON_X - 7;
    R_C_UP_ICON_Y = C_UP_BUTTON_Y + 4;
    YREG(92) = 8;
    YREG(93) = 6;
    YREG(94) = 3;
    YREG(95) = 1;
    R_MAGIC_FILL_COLOR(0) = 0;
    R_MAGIC_FILL_COLOR(1) = 200;
    R_MAGIC_FILL_COLOR(2) = 0;
    ZREG(9) = 140;
    ZREG(10) = 200;
    ZREG(11) = 0;
    ZREG(12) = 200;
    ZREG(13) = 0;
    ZREG(14) = 110;
    ZREG(15) = 56;
    ZREG(16) = 1;
    ZREG(17) = -50;
    ZREG(18) = -200;
    ZREG(19) = 0;
    ZREG(20) = 0;
    ZREG(21) = 50;
    ZREG(22) = -50;
    ZREG(23) = 20;
    ZREG(24) = 20;
    ZREG(25) = 4;
    ZREG(26) = 20;
    ZREG(27) = 10;
    ZREG(28) = 20;
    ZREG(29) = 4;
    ZREG(30) = 20;
    ZREG(31) = 10;
    ZREG(32) = 0;
    ZREG(33) = 0;
    ZREG(34) = 0;
    ZREG(36) = 0;
    ZREG(37) = 0;
    ZREG(38) = 0;
    R_C_BTN_COLOR(0) = 255;
    R_C_BTN_COLOR(1) = 160;
    R_C_BTN_COLOR(2) = 0;
    ZREG(46) = 1;
    ZREG(47) = 1;
    R_START_LABEL_DD(0) = 100;
    R_START_LABEL_DD(1) = 89;
    R_START_LABEL_DD(2) = 92;
    R_START_LABEL_Y(0) = 20;
    R_START_LABEL_Y(1) = 20;
    R_START_LABEL_Y(2) = 20;
    R_START_LABEL_X(0) = 120;
    R_START_LABEL_X(1) = 119;
    R_START_LABEL_X(2) = 119;
    ZREG(61) = 1;
    R_C_UP_BTN_X = C_UP_BUTTON_X;
    R_C_UP_BTN_Y = C_UP_BUTTON_Y;
    ZREG(64) = 20;
    ZREG(65) = 21;
    ZREG(66) = 122;
    R_ITEM_BTN_X(1) = C_LEFT_BUTTON_X;
    R_ITEM_BTN_X(2) = C_DOWN_BUTTON_X;
    R_ITEM_BTN_X(3) = C_RIGHT_BUTTON_X;
    R_ITEM_BTN_Y(0) = B_BUTTON_Y;
    R_ITEM_BTN_Y(1) = C_LEFT_BUTTON_Y;
    R_ITEM_BTN_Y(2) = C_DOWN_BUTTON_Y;
    R_ITEM_BTN_Y(3) = C_RIGHT_BUTTON_Y;
    R_ITEM_BTN_DD(0) = 565;
    R_ITEM_BTN_DD(1) = 606;
    R_ITEM_BTN_DD(2) = 606;
    R_ITEM_BTN_DD(3) = 606;
    R_ITEM_ICON_X(1) = C_LEFT_BUTTON_X;
    R_ITEM_ICON_X(2) = C_DOWN_BUTTON_X;
    R_ITEM_ICON_X(3) = C_RIGHT_BUTTON_X;
    R_ITEM_ICON_Y(0) = B_BUTTON_Y;
    R_ITEM_ICON_Y(1) = C_LEFT_BUTTON_Y;
    R_ITEM_ICON_Y(2) = C_DOWN_BUTTON_Y;
    R_ITEM_ICON_Y(3) = C_RIGHT_BUTTON_Y;
    R_ITEM_ICON_DD(0) = 550;
    R_ITEM_ICON_DD(1) = 680;
    R_ITEM_ICON_DD(2) = 680;
    R_ITEM_ICON_DD(3) = 680;
    ZREG(94) = 1;
    ZREG(95) = 0;
    XREG(0) = 26;
    XREG(1) = 22;
    XREG(2) = -11;
    XREG(3) = -4;
    XREG(4) = 3;
    XREG(5) = 0;
    XREG(6) = 2;
    XREG(7) = 30;
    XREG(8) = 10;
    XREG(9) = 0;
    XREG(10) = -9550;
    XREG(11) = 9950;
    XREG(12) = 68;
    XREG(13) = 36;
    XREG(14) = 4;
    XREG(15) = 1;
    R_A_BTN_Y = A_BUTTON_Y;
    XREG(18) = -380;
    R_A_ICON_Y = A_BUTTON_Y;
    XREG(21) = 48;
    XREG(25) = 0;
    XREG(26) = 0;
    XREG(27) = 0;
    XREG(28) = 16;
    XREG(29) = 50;
    XREG(30) = 15;
    XREG(31) = 8;
    XREG(32) = 4;
    XREG(33) = 2;
    XREG(34) = 100;
    XREG(35) = 7;
    XREG(36) = 20;
    XREG(37) = 10;
    XREG(38) = 2;
    XREG(39) = 140;
    XREG(40) = 20;
    XREG(41) = 300;
    XREG(42) = 100;
    XREG(43) = 70;
    XREG(44) = 50;
    XREG(45) = 36;
    XREG(46) = 16;
    XREG(47) = 8;
    R_MAGIC_BAR_SMALL_Y = 34;
    R_MAGIC_BAR_X = 18;
    R_MAGIC_BAR_LARGE_Y = 42;
    R_MAGIC_FILL_X = 26;
    XREG(52) = 0;
    XREG(53) = 1;
    R_TEXT_INIT_XPOS = 65;
    R_TEXT_INIT_YPOS = 60;
    R_TEXT_LINE_SPACING = 16;
    R_TEXT_CHAR_SCALE = 80;
    XREG(58) = 80;
    XREG(59) = 12;
    R_TEXT_DROP_SHADOW_OFFSET = 1;
    R_TEXTBOX_BG_YPOS = 3;
    XREG(62) = 0;
    XREG(63) = 100;
    R_TEXTBOX_END_XPOS = 158;
    R_TEXTBOX_END_YPOS = 102;
    R_TEXT_CHOICE_XPOS = 48;
    R_TEXT_CHOICE_YPOS(0) = 54;
    R_TEXT_CHOICE_YPOS(1) = 70;
    R_TEXT_CHOICE_YPOS(2) = 86;
    XREG(70) = -300;
    XREG(71) = 0;
    R_TEXTBOX_X_TARGET = 54;
    R_TEXTBOX_Y_TARGET = 48;
    R_TEXTBOX_WIDTH_TARGET = 128;
    R_TEXTBOX_HEIGHT_TARGET = 64;
    R_TEXTBOX_TEXWIDTH_TARGET = 2048;
    R_TEXTBOX_TEXHEIGHT_TARGET = 512;
    XREG(78) = 96;
    XREG(79) = 98;
    XREG(80) = 0;
    XREG(81) = 50;
    XREG(82) = 25;
    XREG(83) = 100;
    XREG(84) = 100;
    XREG(85) = 0;
    XREG(86) = 0;
    XREG(87) = 0;
    XREG(88) = -50;
    XREG(89) = -100;
    XREG(90) = -500;
    XREG(91) = 0;
    XREG(92) = 100;
    XREG(93) = 100;
    XREG(94) = 160;
    XREG(95) = 200;
    WREG(2) = -6080;
    WREG(3) = 9355;
    WREG(4) = 8;
    WREG(5) = 3;
    WREG(6) = 8;
    WREG(7) = 0;
    WREG(8) = 100;
    WREG(9) = 109;
    WREG(10) = 151;
    WREG(11) = 148;
    WREG(12) = 23;
    WREG(13) = 22;
    WREG(14) = -380;
    WREG(15) = -350;
    WREG(16) = -175;
    WREG(17) = 155;
    WREG(18) = 10;
    WREG(19) = 10;
    WREG(20) = -50;
    WREG(21) = -54;
    WREG(22) = -32;
    WREG(23) = -38;
    WREG(24) = -36;
    WREG(25) = 40;
    WREG(26) = -40;
    WREG(27) = 0;
    WREG(28) = 0;
    R_OW_MINIMAP_X = 238;
    R_OW_MINIMAP_Y = 164;
    R_MINIMAP_DISABLED = CVar_GetS32("gMinimalUI", 0);
    WREG(32) = 122;
    WREG(33) = 60;
    WREG(35) = 0;
    WREG(36) = 0;
    WREG(37) = 100;
    WREG(38) = 99;
    WREG(39) = 109;
    R_B_LABEL_X(0) = B_BUTTON_X - 9;
    R_B_LABEL_X(1) = B_BUTTON_X - 11;
    R_B_LABEL_X(2) = B_BUTTON_X - 12;
    R_B_LABEL_Y(0) = B_BUTTON_Y + 6;
    R_B_LABEL_Y(1) = B_BUTTON_Y + 5;
    R_B_LABEL_Y(2) = B_BUTTON_Y + 5;
    WREG(46) = -380;
    WREG(47) = -360;
    WREG(48) = -350;
    WREG(49) = -48;
    WREG(50) = 16;
    WREG(51) = -62;
    WREG(52) = 22;
    WREG(53) = -84;
    WREG(54) = 20;
    WREG(55) = -53;
    WREG(56) = 40;
    WREG(57) = -64;
    WREG(58) = 47;
    WREG(59) = -84;
    WREG(60) = 44;
    WREG(61) = -42;
    WREG(62) = 32;
    WREG(63) = -45;
    WREG(64) = -37;
    WREG(65) = 30;
    WREG(66) = -50;
    R_DGN_MINIMAP_X = 204;
    R_DGN_MINIMAP_Y = 140;
    WREG(87) = 80;
    WREG(88) = 70;
    WREG(89) = 40;
    WREG(90) = 320;
    WREG(91) = 40;
    WREG(92) = 3;
    WREG(93) = 6;
    WREG(94) = 3;
    WREG(95) = 6;

    if (gSaveContext.gameMode == 0) {
        R_TEXTBOX_X = 52;
        R_TEXTBOX_Y = 36;
        VREG(2) = 214;
        VREG(3) = 76;
        VREG(4) = 304;
        VREG(5) = 430;
        VREG(6) = 1;
        R_TEXTBOX_CLEF_XPOS = 78;
        R_TEXTBOX_CLEF_YPOS = 166;
        VREG(9) = 40;
        R_COMPASS_SCALE_X = 32;
        R_COMPASS_SCALE_Y = 32;
        R_COMPASS_OFFSET_X = 110;
        R_COMPASS_OFFSET_Y = -740;
        R_MINIMAP_COLOR(0) = 0;
        R_MINIMAP_COLOR(1) = 255;
        R_MINIMAP_COLOR(2) = 255;
    }

    VREG(21) = 0;
    VREG(22) = 0;
    VREG(23) = 0;
    VREG(24) = 0;
    VREG(25) = 0;
    VREG(26) = 0;
    VREG(27) = 0;
    R_OCARINA_NOTES_XPOS = 98;
    R_OCARINA_NOTES_XPOS_OFFSET = 18;
    VREG(30) = 0;
    VREG(31) = 0;
    VREG(32) = 0;

    R_TEXT_ADJUST_COLOR_1_R = 70;
    R_TEXT_ADJUST_COLOR_1_G = 255;
    R_TEXT_ADJUST_COLOR_1_B = 80;

    R_TEXT_ADJUST_COLOR_2_R = 70;
    R_TEXT_ADJUST_COLOR_2_G = 255;
    R_TEXT_ADJUST_COLOR_2_B = 80;

    VREG(40) = 9;
    VREG(42) = 250;
    VREG(43) = 440;
    VREG(44) = 10;
    R_OCARINA_NOTES_YPOS(0) = 190;
    R_OCARINA_NOTES_YPOS(1) = 184;
    R_OCARINA_NOTES_YPOS(2) = 176;
    R_OCARINA_NOTES_YPOS(3) = 172;
    R_OCARINA_NOTES_YPOS(4) = 170;
    VREG(50) = 30;
    R_OCARINA_NOTES_YPOS_OFFSET = 0;
    VREG(52) = -16;
    VREG(53) = 230;
    VREG(54) = 230;
    VREG(55) = 120;
    VREG(56) = -720;
    VREG(57) = 255;
    VREG(58) = 255;
    VREG(59) = 255;
    VREG(60) = 20;
    VREG(61) = 100;
    VREG(62) = 0;
    VREG(63) = 10;
    R_ITEM_AMMO_X(1) = C_LEFT_BUTTON_X + 1;
    R_ITEM_AMMO_X(2) = C_DOWN_BUTTON_X + 1;
    R_ITEM_AMMO_X(3) = C_RIGHT_BUTTON_X + 1;
    R_ITEM_AMMO_Y(0) = B_BUTTON_Y + 18;
    R_ITEM_AMMO_Y(1) = C_LEFT_BUTTON_Y + 17;
    R_ITEM_AMMO_Y(2) = C_DOWN_BUTTON_Y + 17;
    R_ITEM_AMMO_Y(3) = C_RIGHT_BUTTON_Y + 17;
    VREG(72) = 0;
    VREG(73) = 0;
    VREG(74) = 0;
    VREG(75) = 0;
    R_ITEM_ICON_WIDTH(0) = 30;
    R_ITEM_ICON_WIDTH(1) = 24;
    R_ITEM_ICON_WIDTH(2) = 24;
    R_ITEM_ICON_WIDTH(3) = 24;
    R_ITEM_BTN_WIDTH(0) = 29;
    R_ITEM_BTN_WIDTH(1) = 27;
    R_ITEM_BTN_WIDTH(2) = 27;
    R_ITEM_BTN_WIDTH(3) = 27;
    VREG(84) = 0;
    VREG(85) = 50;
    VREG(86) = 0;
    VREG(87) = 64;
    VREG(88) = 66;
    VREG(89) = 0;
    VREG(90) = 126;
    VREG(91) = 124;
    VREG(92) = -63;
}

void func_80112098(PlayState* play) {
    func_80111070();
}
